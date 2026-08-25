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
  // See https://openrouter.ai/docs and app-attribution guidance.
  if (stripos($base, 'openrouter.ai') !== FALSE) {
    global $base_url;
    $referer = !empty($base_url) ? $base_url : 'https://rovas.app';
    $headers['HTTP-Referer'] = $referer;
    $headers['X-Title'] = 'Rovas AI';
  }

  $response = drupal_http_request($url, array(
    'method' => 'POST',
    'headers' => $headers,
    'data' => drupal_json_encode($body),
    'timeout' => ROVAS_AI_HTTP_TIMEOUT,
  ));

  $code = isset($response->code) ? (int) $response->code : 0;
  if ($code < 200 || $code >= 300) {
    $err = 'http_' . $code;
    if (!empty($response->data)) {
      $decoded = json_decode($response->data, TRUE);
      if (is_array($decoded) && !empty($decoded['error']['message'])) {
        // Do not log API key; message only.
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

  if ($content === '') {
    $finish = isset($choice['finish_reason']) ? $choice['finish_reason'] : '';
    $msg_keys = $message ? implode(',', array_keys($message)) : '(no message)';
    watchdog('rovas_ai', 'OpenAI response missing content (finish=@f keys=@k)', array(
      '@f' => $finish,
      '@k' => $msg_keys,
    ), WATCHDOG_WARNING);
    return array(
      'ok' => FALSE,
      'content' => '',
      'usage' => $usage,
      'error' => $finish === 'length' ? 'truncated' : 'empty_content',
      'finish_reason' => $finish,
    );
  }

  return array(
    'ok' => TRUE,
    'content' => $content,
    'usage' => $usage,
    'error' => '',
  );
}

/**
 * Normalize assistant message content to a string.
 *
 * OpenAI-compatible APIs may return a string or an array of content parts.
 *
 * @param array $message
 *
 * @return string
 */
function rovas_ai_openai_extract_content(array $message) {
  if (!isset($message['content'])) {
    return '';
  }
  $raw = $message['content'];
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
