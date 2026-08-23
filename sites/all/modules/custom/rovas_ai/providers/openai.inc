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

  $response = drupal_http_request($url, array(
    'method' => 'POST',
    'headers' => array(
      'Content-Type' => 'application/json',
      'Authorization' => 'Bearer ' . $api_key,
    ),
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
  if (!is_array($data) || empty($data['choices'][0]['message']['content'])) {
    watchdog('rovas_ai', 'OpenAI response missing content', array(), WATCHDOG_WARNING);
    return array(
      'ok' => FALSE,
      'content' => '',
      'usage' => isset($data['usage']) && is_array($data['usage']) ? $data['usage'] : array(),
      'error' => 'empty_content',
    );
  }

  $content = trim((string) $data['choices'][0]['message']['content']);
  $usage = isset($data['usage']) && is_array($data['usage']) ? $data['usage'] : array();

  return array(
    'ok' => TRUE,
    'content' => $content,
    'usage' => $usage,
    'error' => '',
  );
}
