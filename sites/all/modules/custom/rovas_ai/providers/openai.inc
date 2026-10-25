<?php

/**
 * @file
 * OpenAI-compatible chat completions provider.
 */

/**
 * Chat via OpenAI-compatible /chat/completions.
 *
 * @param array $messages
 * @param array $options
 *
 * @return array
 *   ok, content, usage, error.
 */
function rovas_ai_openai_chat(array $messages, array $options = array()) {
  $result = rovas_ai_openai_chat_request($messages, $options);

  // Empty content + finish=length often means a reasoning model spent the whole
  // budget on thinking. Retry once with more tokens (reasoning already disabled
  // on OpenRouter in the request body).
  if (empty($result['ok'])
    && !empty($result['error'])
    && ($result['error'] === 'truncated' || $result['error'] === 'empty_content')
    && empty($options['rovas_ai_no_retry'])) {
    $opts = $options;
    $current = isset($opts['max_tokens']) ? (int) $opts['max_tokens'] : ROVAS_AI_MAX_TOKENS;
    $opts['max_tokens'] = max($current * 2, 512);
    $opts['rovas_ai_no_retry'] = TRUE;
    watchdog('rovas_ai', 'Retrying chat with max_tokens=@n after @e', array(
      '@n' => $opts['max_tokens'],
      '@e' => $result['error'],
    ), WATCHDOG_NOTICE);
    $result = rovas_ai_openai_chat_request($messages, $opts);
  }

  return $result;
}

/**
 * Single HTTP chat completion request (no retry).
 *
 * @param array $messages
 * @param array $options
 *
 * @return array
 */
function rovas_ai_openai_chat_request(array $messages, array $options = array()) {
  $api_key = trim((string) variable_get('rovas_ai_api_key', ''));
  if ($api_key === '') {
    return array(
      'ok' => FALSE,
      'content' => '',
      'usage' => array(),
      'error' => 'not_configured',
    );
  }

  $base = rovas_ai_normalize_base_url(variable_get('rovas_ai_base_url', ROVAS_AI_DEFAULT_BASE_URL));
  $url = $base . '/chat/completions';
  $model = !empty($options['model']) ? $options['model'] : ROVAS_AI_DEFAULT_MODEL;
  $max_tokens = isset($options['max_tokens']) ? (int) $options['max_tokens'] : ROVAS_AI_MAX_TOKENS;
  $temperature = isset($options['temperature']) ? (float) $options['temperature'] : 0.1;

  $body = array(
    'model' => $model,
    'messages' => array_values($messages),
    'max_tokens' => max(16, $max_tokens),
    'temperature' => $temperature,
    'response_format' => array('type' => 'json_object'),
  );

  $headers = array(
    'Content-Type' => 'application/json',
    'Authorization' => 'Bearer ' . $api_key,
  );
  // OpenRouter may return usage with empty message.content when these are missing.
  if (stripos($base, 'openrouter.ai') !== FALSE) {
    global $base_url;
    $referer = !empty($base_url) ? $base_url : 'https://rovas.app';
    $headers['HTTP-Referer'] = $referer;
    $headers['X-Title'] = 'Rovas AI';
    // NACE classify needs JSON in content, not thinking tokens. Reasoning counts
    // against max_tokens and often finishes with empty content + finish=length.
    // https://openrouter.ai/docs/guides/best-practices/reasoning-tokens
    $body['reasoning'] = array(
      'effort' => 'none',
      'exclude' => TRUE,
    );
  }

  $timeout = isset($options['timeout']) ? (int) $options['timeout'] : ROVAS_AI_HTTP_TIMEOUT;
  if ($timeout < 10) {
    $timeout = 10;
  }
  $response = drupal_http_request($url, array(
    'method' => 'POST',
    'headers' => $headers,
    'data' => drupal_json_encode($body),
    'timeout' => $timeout,
  ));

  $code = isset($response->code) ? (int) $response->code : 0;
  if ($code < 200 || $code >= 300) {
    $err = 'http_' . $code;
    if (!empty($response->data)) {
      $decoded = json_decode($response->data, TRUE);
      if (is_array($decoded) && !empty($decoded['error']['message'])) {
        watchdog('rovas_ai', 'OpenAI HTTP @c: @m', array(
          '@c' => $code,
          '@m' => drupal_substr($decoded['error']['message'], 0, 200),
        ), WATCHDOG_WARNING);
      }
      else {
        watchdog('rovas_ai', 'OpenAI HTTP @c (no JSON error body)', array('@c' => $code), WATCHDOG_WARNING);
      }
    }
    return array(
      'ok' => FALSE,
      'content' => '',
      'usage' => array(),
      'error' => $err,
    );
  }

  $data = json_decode($response->data, TRUE);
  $usage = (is_array($data) && isset($data['usage']) && is_array($data['usage']))
    ? $data['usage']
    : array();
  $choice = (is_array($data) && !empty($data['choices'][0]) && is_array($data['choices'][0]))
    ? $data['choices'][0]
    : array();
  $message = (!empty($choice['message']) && is_array($choice['message']))
    ? $choice['message']
    : array();
  $content = rovas_ai_openai_extract_content($message);
  $finish = isset($choice['finish_reason']) ? $choice['finish_reason'] : '';

  if ($content === '') {
    $msg_keys = $message ? implode(',', array_keys($message)) : '(no message)';
    watchdog('rovas_ai', 'OpenAI response missing content (finish=@f keys=@k max_tokens=@m)', array(
      '@f' => $finish,
      '@k' => $msg_keys,
      '@m' => $max_tokens,
    ), WATCHDOG_WARNING);
    return array(
      'ok' => FALSE,
      'content' => '',
      'usage' => $usage,
      'error' => $finish === 'length' ? 'truncated' : 'empty_content',
      'finish_reason' => $finish,
      'max_tokens' => $max_tokens,
    );
  }

  return array(
    'ok' => TRUE,
    'content' => $content,
    'usage' => $usage,
    'error' => '',
    'finish_reason' => $finish,
  );
}

/**
 * Normalize assistant message content to a string.
 *
 * Prefer message.content; if empty (reasoning models), try to recover a JSON
 * object from the reasoning text.
 *
 * @param array $message
 *
 * @return string
 */
function rovas_ai_openai_extract_content(array $message) {
  $content = '';
  if (isset($message['content'])) {
    $content = rovas_ai_openai_stringify_content($message['content']);
  }
  if ($content !== '') {
    return $content;
  }

  // Fallback: some reasoning models put the final answer only in reasoning.
  if (!empty($message['reasoning']) && is_string($message['reasoning'])) {
    $from_reasoning = rovas_ai_openai_json_blob_from_text($message['reasoning']);
    if ($from_reasoning !== '') {
      return $from_reasoning;
    }
  }

  return '';
}

/**
 * @param mixed $raw
 *   String or content-parts array.
 *
 * @return string
 */
function rovas_ai_openai_stringify_content($raw) {
  if (is_string($raw)) {
    return trim($raw);
  }
  if (!is_array($raw)) {
    return '';
  }
  $parts = array();
  foreach ($raw as $part) {
    if (is_string($part)) {
      $parts[] = $part;
      continue;
    }
    if (!is_array($part)) {
      continue;
    }
    if (isset($part['text']) && is_string($part['text'])) {
      $parts[] = $part['text'];
    }
    elseif (isset($part['content']) && is_string($part['content'])) {
      $parts[] = $part['content'];
    }
  }
  return trim(implode('', $parts));
}

/**
 * Pull the last JSON object substring from free text, if any.
 *
 * @param string $text
 *
 * @return string
 */
function rovas_ai_openai_json_blob_from_text($text) {
  $text = trim((string) $text);
  if ($text === '') {
    return '';
  }
  if ($text[0] === '{') {
    $decoded = json_decode($text, TRUE);
    if (is_array($decoded)) {
      return $text;
    }
  }
  if (preg_match('/\{[^{}]*(?:\{[^{}]*\}[^{}]*)*\}/s', $text, $m)) {
    $candidate = $m[0];
    $decoded = json_decode($candidate, TRUE);
    if (is_array($decoded)) {
      return $candidate;
    }
  }
  return '';
}
