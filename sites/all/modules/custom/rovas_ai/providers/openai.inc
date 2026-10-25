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

  // Model refusal (content empty, refusal set) — do not burn credits retrying.
  if (empty($result['ok']) && !empty($result['error']) && $result['error'] === 'refused') {
    return $result;
  }

  // OpenRouter 402: "can only afford N" when max_tokens exceeds credit headroom.
  if (empty($result['ok'])
    && !empty($result['error'])
    && $result['error'] === 'http_402'
    && !empty($result['affordable_tokens'])
    && empty($options['rovas_ai_no_retry'])) {
    $result = rovas_ai_openai_retry_with_affordable($messages, $options, $result);
  }

  // finish=length: model hit the output cap — retry once with a modest bump,
  // then fall back to affordable if that 402s. Never jump to 3000 (OpenRouter
  // rejects when max_tokens > credit headroom even if usage would be lower).
  if (empty($result['ok'])
    && !empty($result['error'])
    && $result['error'] === 'truncated'
    && empty($options['rovas_ai_no_retry'])) {
    $opts = $options;
    $current = isset($opts['max_tokens']) ? (int) $opts['max_tokens'] : ROVAS_AI_MAX_TOKENS;
    $opts['max_tokens'] = min(max((int) round($current * 1.5), $current + 256), 2000);
    $opts['rovas_ai_no_retry'] = TRUE;
    watchdog('rovas_ai', 'Retrying chat with max_tokens=@n after truncated', array(
      '@n' => $opts['max_tokens'],
    ), WATCHDOG_NOTICE);
    $result = rovas_ai_openai_chat_request($messages, $opts);
    if (empty($result['ok']) && !empty($result['error']) && $result['error'] === 'http_402'
      && !empty($result['affordable_tokens'])) {
      $result = rovas_ai_openai_retry_with_affordable($messages, $options, $result);
    }
  }

  // empty_content with finish=stop is usually transient or a soft refusal —
  // retry once at the SAME budget (bumping tokens caused 402s and dropped tiles).
  if (empty($result['ok'])
    && !empty($result['error'])
    && $result['error'] === 'empty_content'
    && empty($options['rovas_ai_no_retry'])) {
    $opts = $options;
    $opts['rovas_ai_no_retry'] = TRUE;
    watchdog('rovas_ai', 'Retrying chat once after empty_content (same max_tokens)', array(), WATCHDOG_NOTICE);
    $result = rovas_ai_openai_chat_request($messages, $opts);
    if (empty($result['ok']) && !empty($result['error']) && $result['error'] === 'http_402'
      && !empty($result['affordable_tokens'])) {
      $result = rovas_ai_openai_retry_with_affordable($messages, $options, $result);
    }
  }

  return $result;
}

/**
 * Retry a chat request using OpenRouter's reported affordable max_tokens.
 *
 * @param array $messages
 * @param array $options
 * @param array $failed
 *
 * @return array
 */
function rovas_ai_openai_retry_with_affordable(array $messages, array $options, array $failed) {
  $afford = isset($failed['affordable_tokens']) ? (int) $failed['affordable_tokens'] : 0;
  if ($afford < 64) {
    return $failed;
  }
  // Stay under the stated afford ceiling (OpenRouter counts the max_tokens cap).
  $use = max(64, $afford - 16);
  $opts = $options;
  $opts['max_tokens'] = $use;
  $opts['rovas_ai_no_retry'] = TRUE;
  watchdog('rovas_ai', 'Retrying chat with max_tokens=@n after http_402 (affordable=@a)', array(
    '@n' => $use,
    '@a' => $afford,
  ), WATCHDOG_NOTICE);
  return rovas_ai_openai_chat_request($messages, $opts);
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
    $affordable = 0;
    $err_msg = '';
    if (!empty($response->data)) {
      $decoded = json_decode($response->data, TRUE);
      if (is_array($decoded) && !empty($decoded['error']['message'])) {
        $err_msg = (string) $decoded['error']['message'];
        watchdog('rovas_ai', 'OpenAI HTTP @c: @m', array(
          '@c' => $code,
          '@m' => drupal_substr($err_msg, 0, 200),
        ), WATCHDOG_WARNING);
        // OpenRouter: "can only afford 3101"
        if (preg_match('/can only afford\s+(\d+)/i', $err_msg, $am)) {
          $affordable = (int) $am[1];
        }
      }
      else {
        watchdog('rovas_ai', 'OpenAI HTTP @c (no JSON error body)', array('@c' => $code), WATCHDOG_WARNING);
      }
    }
    $out = array(
      'ok' => FALSE,
      'content' => '',
      'usage' => array(),
      'error' => $err,
    );
    if ($affordable > 0) {
      $out['affordable_tokens'] = $affordable;
    }
    if ($err_msg !== '') {
      $out['error_message'] = $err_msg;
    }
    return $out;
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
    $refusal = '';
    if (!empty($message['refusal']) && is_string($message['refusal'])) {
      $refusal = trim($message['refusal']);
    }
    watchdog('rovas_ai', 'OpenAI response missing content (finish=@f keys=@k max_tokens=@m refusal=@r)', array(
      '@f' => $finish,
      '@k' => $msg_keys,
      '@m' => $max_tokens,
      '@r' => $refusal !== '' ? drupal_substr($refusal, 0, 180) : '-',
    ), WATCHDOG_WARNING);
    $error = 'empty_content';
    if ($finish === 'length') {
      $error = 'truncated';
    }
    elseif ($refusal !== '') {
      $error = 'refused';
    }
    return array(
      'ok' => FALSE,
      'content' => '',
      'usage' => $usage,
      'error' => $error,
      'finish_reason' => $finish,
      'max_tokens' => $max_tokens,
      'refusal' => $refusal,
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
