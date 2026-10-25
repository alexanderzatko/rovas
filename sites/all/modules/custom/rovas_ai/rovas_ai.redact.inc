<?php

/**
 * @file
 * Proof-photo PII / face detection (vision) and GD blackout.
 */

define('ROVAS_AI_PROOF_REDACT_CONFIDENCE', 0.3);
define('ROVAS_AI_PROOF_REDACT_MAX_TOKENS', 4000);
define('ROVAS_AI_PROOF_EDIT_MESSAGE', 'Your image was edited by Rovas');
/** Expand each painted box by this fraction of image width/height (recall pad). */
define('ROVAS_AI_PROOF_BOX_PAD', 0.012);

/**
 * Normalize photo_redact_mode from API / form.
 *
 * @param string $mode
 *
 * @return string
 *   none|text|text_faces
 */
function rovas_ai_normalize_photo_redact_mode($mode) {
  $mode = drupal_strtolower(trim((string) $mode));
  if ($mode === 'text' || $mode === 'text_faces') {
    return $mode;
  }
  return 'none';
}

/**
 * User-facing notice when regions were painted.
 *
 * @return string
 */
function rovas_ai_proof_edit_message() {
  return t('Your image was edited by Rovas');
}

/**
 * Detect + redact proof image bytes. Fail-closed when mode ≠ none.
 *
 * @param string $bytes
 *   Raw image bytes.
 * @param string $mime
 *   image/jpeg|image/png|image/webp (best effort).
 * @param string $mode
 *   none|text|text_faces
 * @param array $options
 *   Optional: uid (budget).
 *
 * @return array
 *   ok, bytes, mime, regions_redacted, edited (bool), error, regions (list).
 */
function rovas_ai_redact_proof_image_run($bytes, $mime, $mode, array $options = array()) {
  $mode = rovas_ai_normalize_photo_redact_mode($mode);
  $mime = rovas_ai_normalize_image_mime($mime, $bytes);
  $out = array(
    'ok' => TRUE,
    'bytes' => $bytes,
    'mime' => $mime,
    'regions_redacted' => 0,
    'edited' => FALSE,
    'error' => '',
    'regions' => array(),
  );

  if ($mode === 'none') {
    return $out;
  }

  if (!is_string($bytes) || strlen($bytes) < 32) {
    return array(
      'ok' => FALSE,
      'bytes' => '',
      'mime' => $mime,
      'regions_redacted' => 0,
      'edited' => FALSE,
      'error' => 'invalid_image',
      'regions' => array(),
    );
  }

  $uid = isset($options['uid']) ? (int) $options['uid'] : 0;
  if (!rovas_ai_is_configured()) {
    return array(
      'ok' => FALSE,
      'bytes' => '',
      'mime' => $mime,
      'regions_redacted' => 0,
      'edited' => FALSE,
      'error' => 'not_configured',
      'regions' => array(),
    );
  }
  if (!rovas_ai_budget_allow($uid)) {
    return array(
      'ok' => FALSE,
      'bytes' => '',
      'mime' => $mime,
      'regions_redacted' => 0,
      'edited' => FALSE,
      'error' => 'budget_exceeded',
      'regions' => array(),
    );
  }

  $detect = rovas_ai_detect_proof_pii($bytes, $mime, $mode, $options);
  rovas_ai_budget_consume($uid);
  if (empty($detect['ok'])) {
    return array(
      'ok' => FALSE,
      'bytes' => '',
      'mime' => $mime,
      'regions_redacted' => 0,
      'edited' => FALSE,
      'error' => !empty($detect['error']) ? $detect['error'] : 'detect_failed',
      'regions' => array(),
    );
  }

  $regions = isset($detect['regions']) && is_array($detect['regions']) ? $detect['regions'] : array();
  if (empty($regions)) {
    $out['regions'] = array();
    return $out;
  }

  $painted = rovas_ai_image_paint_boxes($bytes, $mime, $regions);
  if (empty($painted['ok'])) {
    return array(
      'ok' => FALSE,
      'bytes' => '',
      'mime' => $mime,
      'regions_redacted' => 0,
      'edited' => FALSE,
      'error' => !empty($painted['error']) ? $painted['error'] : 'paint_failed',
      'regions' => array(),
    );
  }

  $count = isset($painted['regions_painted']) ? (int) $painted['regions_painted'] : 0;
  return array(
    'ok' => TRUE,
    'bytes' => $painted['bytes'],
    'mime' => $painted['mime'],
    'regions_redacted' => $count,
    'edited' => $count > 0,
    'error' => '',
    'regions' => $regions,
  );
}

/**
 * Vision detect of PII / persons; returns normalized boxes on the original image.
 *
 * Pre-scales to OpenAI detail:high tile size so model coordinates match pixels
 * (otherwise boxes are systematically misplaced).
 *
 * @param string $bytes
 * @param string $mime
 * @param string $mode
 *   text|text_faces
 * @param array $options
 *
 * @return array
 *   ok, regions, error
 */
function rovas_ai_detect_proof_pii($bytes, $mime, $mode, array $options = array()) {
  $mode = rovas_ai_normalize_photo_redact_mode($mode);
  if ($mode === 'none') {
    return array('ok' => TRUE, 'regions' => array(), 'error' => '');
  }

  $mime = rovas_ai_normalize_image_mime($mime, $bytes);
  $prepared = rovas_ai_proof_vision_prepare($bytes, $mime);
  if (empty($prepared['ok'])) {
    return array(
      'ok' => FALSE,
      'regions' => array(),
      'error' => !empty($prepared['error']) ? $prepared['error'] : 'prepare_failed',
    );
  }

  $vision_bytes = $prepared['bytes'];
  $vision_mime = $prepared['mime'];
  $vw = (int) $prepared['width'];
  $vh = (int) $prepared['height'];
  $b64 = base64_encode($vision_bytes);
  $data_url = 'data:' . $vision_mime . ';base64,' . $b64;

  $include_faces = ($mode === 'text_faces');
  $types = $include_faces
    ? 'email, phone, name, person'
    : 'email, phone, name';

  $system = 'You are a careful PII redaction locator for screenshots (email clients, chat, docs). '
    . 'Return ONLY JSON: {"regions":[{"type":"email|phone|name'
    . ($include_faces ? '|person' : '')
    . '","bbox":[x_min,y_min,x_max,y_max],"confidence":0.0}]}. '
    . 'Coordinate contract (STRICT): the image is exactly '
    . $vw . 'x' . $vh . ' pixels. '
    . 'bbox uses integers 0–999 inclusive in that image’s coordinate space '
    . '(0,0 = top-left; 999,999 = bottom-right). '
    . 'x_min < x_max, y_min < y_max. Map pixel x → round(x*999/(W-1)), same for y. '
    . 'Be EXHAUSTIVE: every email address (headers, To/Cc, body, signatures); '
    . 'every phone number (incl. Slovak 09xx…); every personal given+family name '
    . 'of a person (message headers, greetings, signatures, recipient lists). '
    . 'One region per contiguous span; prefer slightly larger boxes over missing text. '
    . 'Do NOT redact org/project names alone, prices, or generic UI chrome. '
    . ($include_faces
      ? 'Also box human faces / clearly identifiable persons as type person. '
      : 'Do NOT include face/person boxes. ')
    . 'Empty regions only if nothing sensitive is visible.';

  $user_text = 'Image size: ' . $vw . 'x' . $vh . '. Mode: ' . $mode
    . '. List ALL sensitive ' . $types . ' regions with bbox [x_min,y_min,x_max,y_max] in 0–999. JSON only.';

  $messages = array(
    array(
      'role' => 'system',
      'content' => $system,
    ),
    array(
      'role' => 'user',
      'content' => array(
        array(
          'type' => 'text',
          'text' => $user_text,
        ),
        array(
          'type' => 'image_url',
          'image_url' => array(
            'url' => $data_url,
            // Image already matches detail:high geometry — avoid a second resize skew.
            'detail' => 'high',
          ),
        ),
      ),
    ),
  );

  $chat_opts = array(
    'task_id' => 'proof_pii_redact',
    'temperature' => 0,
    'max_tokens' => ROVAS_AI_PROOF_REDACT_MAX_TOKENS,
    'timeout' => 120,
  );
  $chat = rovas_ai_chat($messages, $chat_opts);

  if (empty($chat['ok'])) {
    return array(
      'ok' => FALSE,
      'regions' => array(),
      'error' => !empty($chat['error']) ? $chat['error'] : 'chat_failed',
    );
  }

  module_load_include('inc', 'rovas_ai', 'rovas_ai.classify');
  $content = isset($chat['content']) ? $chat['content'] : '';
  $parsed = rovas_ai_parse_json_object($content);

  // Truncated JSON is common when many regions fill max_tokens — retry once.
  $finish = isset($chat['finish_reason']) ? $chat['finish_reason'] : '';
  if (!is_array($parsed) || ($finish === 'length' && empty($parsed['regions']))) {
    $chat_opts['max_tokens'] = ROVAS_AI_PROOF_REDACT_MAX_TOKENS * 2;
    $chat_opts['rovas_ai_no_retry'] = TRUE;
    watchdog('rovas_ai', 'Proof PII JSON retry (finish=@f parse=@p); content head: @c', array(
      '@f' => $finish,
      '@p' => is_array($parsed) ? 'partial' : 'fail',
      '@c' => drupal_substr(preg_replace('/\s+/', ' ', $content), 0, 400),
    ), WATCHDOG_WARNING);
    $chat2 = rovas_ai_chat($messages, $chat_opts);
    if (!empty($chat2['ok'])) {
      $content = isset($chat2['content']) ? $chat2['content'] : '';
      $parsed2 = rovas_ai_parse_json_object($content);
      if (is_array($parsed2)) {
        $parsed = $parsed2;
        $finish = isset($chat2['finish_reason']) ? $chat2['finish_reason'] : $finish;
      }
    }
  }

  if (!is_array($parsed)) {
    watchdog('rovas_ai', 'Proof PII invalid_json (finish=@f json_err=@e); content head: @c', array(
      '@f' => $finish,
      '@e' => function_exists('json_last_error_msg') ? json_last_error_msg() : (string) json_last_error(),
      '@c' => drupal_substr(preg_replace('/\s+/', ' ', $content), 0, 500),
    ), WATCHDOG_WARNING);
    return array(
      'ok' => FALSE,
      'regions' => array(),
      'error' => 'invalid_json',
    );
  }

  $raw = array();
  if (!empty($parsed['regions']) && is_array($parsed['regions'])) {
    $raw = $parsed['regions'];
  }
  elseif (isset($parsed[0]) && is_array($parsed[0])) {
    // Bare list of regions.
    $raw = $parsed;
  }

  $regions = rovas_ai_normalize_proof_regions($raw, $mode);
  watchdog('rovas_ai', 'Proof PII detect: @n region(s) on @w x @h vision canvas (orig @ow x @oh, finish=@f)', array(
    '@n' => count($regions),
    '@w' => $vw,
    '@h' => $vh,
    '@ow' => (int) $prepared['orig_width'],
    '@oh' => (int) $prepared['orig_height'],
    '@f' => $finish,
  ), WATCHDOG_NOTICE);

  return array(
    'ok' => TRUE,
    'regions' => $regions,
    'error' => '',
  );
}

/**
 * OpenAI detail:high effective size (fit 2048², then shortest side = 768).
 *
 * @param int $width
 * @param int $height
 *
 * @return array
 *   array(width, height)
 */
function rovas_ai_openai_high_detail_size($width, $height) {
  $width = max(1, (int) $width);
  $height = max(1, (int) $height);
  $scale = min(1.0, 2048.0 / max($width, $height));
  $w1 = max(1, (int) round($width * $scale));
  $h1 = max(1, (int) round($height * $scale));
  $short = min($w1, $h1);
  $scale2 = 768.0 / $short;
  $w2 = max(1, (int) round($w1 * $scale2));
  $h2 = max(1, (int) round($h1 * $scale2));
  return array($w2, $h2);
}

/**
 * Resize proof bytes to the vision canvas OpenAI detail:high would use.
 *
 * @param string $bytes
 * @param string $mime
 *
 * @return array
 *   ok, bytes, mime, width, height, orig_width, orig_height, error
 */
function rovas_ai_proof_vision_prepare($bytes, $mime) {
  if (!function_exists('imagecreatefromstring')) {
    return array('ok' => FALSE, 'error' => 'gd_unavailable');
  }
  $im = @imagecreatefromstring($bytes);
  if ($im === FALSE) {
    return array('ok' => FALSE, 'error' => 'decode_failed');
  }
  $ow = imagesx($im);
  $oh = imagesy($im);
  if ($ow < 1 || $oh < 1) {
    imagedestroy($im);
    return array('ok' => FALSE, 'error' => 'invalid_dimensions');
  }

  list($nw, $nh) = rovas_ai_openai_high_detail_size($ow, $oh);
  if ($nw === $ow && $nh === $oh) {
    imagedestroy($im);
    return array(
      'ok' => TRUE,
      'bytes' => $bytes,
      'mime' => $mime,
      'width' => $ow,
      'height' => $oh,
      'orig_width' => $ow,
      'orig_height' => $oh,
      'error' => '',
    );
  }

  $dst = imagecreatetruecolor($nw, $nh);
  if ($dst === FALSE) {
    imagedestroy($im);
    return array('ok' => FALSE, 'error' => 'resize_failed');
  }
  imagecopyresampled($dst, $im, 0, 0, 0, 0, $nw, $nh, $ow, $oh);
  imagedestroy($im);

  // JPEG keeps payload smaller for vision upload.
  $encoded = rovas_ai_gd_encode($dst, 'image/jpeg');
  imagedestroy($dst);
  if (empty($encoded['ok'])) {
    return array('ok' => FALSE, 'error' => 'encode_failed');
  }

  return array(
    'ok' => TRUE,
    'bytes' => $encoded['bytes'],
    'mime' => $encoded['mime'],
    'width' => $nw,
    'height' => $nh,
    'orig_width' => $ow,
    'orig_height' => $oh,
    'error' => '',
  );
}

/**
 * Filter and normalize model region list.
 *
 * @param array $raw
 * @param string $mode
 *
 * @return array
 *   List of {type, BoundingBox{Left,Top,Width,Height}, Confidence}
 */
function rovas_ai_normalize_proof_regions(array $raw, $mode) {
  $mode = rovas_ai_normalize_photo_redact_mode($mode);
  $allowed = array('email' => TRUE, 'phone' => TRUE, 'name' => TRUE);
  if ($mode === 'text_faces') {
    $allowed['person'] = TRUE;
  }
  $threshold = ROVAS_AI_PROOF_REDACT_CONFIDENCE;
  $out = array();

  foreach ($raw as $item) {
    if (!is_array($item)) {
      continue;
    }
    $type = isset($item['type']) ? drupal_strtolower(trim((string) $item['type'])) : '';
    if ($type === 'face') {
      $type = 'person';
    }
    if (!isset($allowed[$type])) {
      continue;
    }
    $conf = 1.0;
    if (isset($item['confidence'])) {
      $conf = (float) $item['confidence'];
    }
    elseif (isset($item['Confidence'])) {
      $conf = (float) $item['Confidence'];
    }
    // Model may use 0–100.
    if ($conf > 1.0) {
      $conf = $conf / 100.0;
    }
    if ($conf < $threshold) {
      continue;
    }

    $box = rovas_ai_normalize_bounding_box($item);
    if ($box === NULL) {
      continue;
    }

    $out[] = array(
      'type' => $type,
      'BoundingBox' => $box,
      'Confidence' => $conf * 100.0,
    );
  }

  return $out;
}

/**
 * Extract normalized Left/Top/Width/Height from a region item.
 *
 * Accepts Rekognition-style maps, x_min/x_max maps, or bbox [x0,y0,x1,y1]
 * in 0–1 or 0–999 space.
 *
 * @param array $item
 *
 * @return array|null
 */
function rovas_ai_normalize_bounding_box(array $item) {
  $x0 = $y0 = $x1 = $y1 = NULL;

  // Preferred: bbox [x_min, y_min, x_max, y_max]
  foreach (array('bbox', 'box', 'BoundingBox') as $key) {
    if (!isset($item[$key]) || !is_array($item[$key])) {
      continue;
    }
    $arr = $item[$key];
    if (isset($arr[0]) && isset($arr[1]) && isset($arr[2]) && isset($arr[3])
      && !isset($arr['Left']) && !isset($arr['left']) && !isset($arr['Width'])) {
      $x0 = (float) $arr[0];
      $y0 = (float) $arr[1];
      $x1 = (float) $arr[2];
      $y1 = (float) $arr[3];
      break;
    }
  }

  if ($x0 === NULL) {
    $src = NULL;
    if (!empty($item['box']) && is_array($item['box'])) {
      $src = $item['box'];
    }
    elseif (!empty($item['BoundingBox']) && is_array($item['BoundingBox'])) {
      $src = $item['BoundingBox'];
    }
    elseif (!empty($item['bbox']) && is_array($item['bbox'])) {
      $src = $item['bbox'];
    }
    elseif (isset($item['Left']) || isset($item['left']) || isset($item['x_min'])) {
      $src = $item;
    }
    if ($src === NULL) {
      return NULL;
    }

    if (isset($src['x_min']) || isset($src['xmin'])) {
      $x0 = isset($src['x_min']) ? (float) $src['x_min'] : (float) $src['xmin'];
      $y0 = isset($src['y_min']) ? (float) $src['y_min'] : (isset($src['ymin']) ? (float) $src['ymin'] : 0);
      $x1 = isset($src['x_max']) ? (float) $src['x_max'] : (isset($src['xmax']) ? (float) $src['xmax'] : $x0);
      $y1 = isset($src['y_max']) ? (float) $src['y_max'] : (isset($src['ymax']) ? (float) $src['ymax'] : $y0);
    }
    else {
      $left = isset($src['Left']) ? (float) $src['Left'] : (isset($src['left']) ? (float) $src['left'] : NULL);
      $top = isset($src['Top']) ? (float) $src['Top'] : (isset($src['top']) ? (float) $src['top'] : NULL);
      $width = isset($src['Width']) ? (float) $src['Width'] : (isset($src['width']) ? (float) $src['width'] : NULL);
      $height = isset($src['Height']) ? (float) $src['Height'] : (isset($src['height']) ? (float) $src['height'] : NULL);
      if ($left === NULL || $top === NULL || $width === NULL || $height === NULL) {
        return NULL;
      }
      $x0 = $left;
      $y0 = $top;
      $x1 = $left + $width;
      $y1 = $top + $height;
    }
  }

  // 0–999 (or 0–1000) grid → 0–1.
  if ($x1 > 1.5 || $y1 > 1.5 || $x0 > 1.5 || $y0 > 1.5) {
    $x0 /= 999.0;
    $y0 /= 999.0;
    $x1 /= 999.0;
    $y1 /= 999.0;
  }

  if ($x1 < $x0) {
    $t = $x0;
    $x0 = $x1;
    $x1 = $t;
  }
  if ($y1 < $y0) {
    $t = $y0;
    $y0 = $y1;
    $y1 = $t;
  }

  $left = max(0.0, min(1.0, $x0));
  $top = max(0.0, min(1.0, $y0));
  $width = max(0.0, min(1.0 - $left, $x1 - $x0));
  $height = max(0.0, min(1.0 - $top, $y1 - $y0));
  if ($width < 0.001 || $height < 0.001) {
    return NULL;
  }

  return array(
    'Left' => $left,
    'Top' => $top,
    'Width' => $width,
    'Height' => $height,
  );
}

/**
 * Paint black rectangles over normalized boxes (nabezky-style GD).
 *
 * @param string $bytes
 * @param string $mime
 * @param array $regions
 *   Each with BoundingBox Left/Top/Width/Height in [0,1], optional Confidence.
 *
 * @return array
 *   ok, bytes, mime, regions_painted, error
 */
function rovas_ai_image_paint_boxes($bytes, $mime, array $regions) {
  if (!function_exists('imagecreatetruecolor')) {
    watchdog('rovas_ai', 'GD library not available for proof redaction', array(), WATCHDOG_ERROR);
    return array(
      'ok' => FALSE,
      'bytes' => '',
      'mime' => $mime,
      'regions_painted' => 0,
      'error' => 'gd_unavailable',
    );
  }

  $mime = rovas_ai_normalize_image_mime($mime, $bytes);
  $im = @imagecreatefromstring($bytes);
  if ($im === FALSE) {
    watchdog('rovas_ai', 'Failed to decode image for proof redaction', array(), WATCHDOG_WARNING);
    return array(
      'ok' => FALSE,
      'bytes' => '',
      'mime' => $mime,
      'regions_painted' => 0,
      'error' => 'decode_failed',
    );
  }

  $imwidth = imagesx($im);
  $imheight = imagesy($im);
  if ($imwidth < 1 || $imheight < 1) {
    imagedestroy($im);
    return array(
      'ok' => FALSE,
      'bytes' => '',
      'mime' => $mime,
      'regions_painted' => 0,
      'error' => 'invalid_dimensions',
    );
  }

  $clr = imagecolorallocate($im, 0, 0, 0);
  $painted = 0;
  $threshold = ROVAS_AI_PROOF_REDACT_CONFIDENCE * 100.0;
  $pad_x = (int) max(4, round($imwidth * ROVAS_AI_PROOF_BOX_PAD));
  $pad_y = (int) max(4, round($imheight * ROVAS_AI_PROOF_BOX_PAD));

  foreach ($regions as $region) {
    if (!is_array($region) || empty($region['BoundingBox']) || !is_array($region['BoundingBox'])) {
      continue;
    }
    if (isset($region['Confidence']) && (float) $region['Confidence'] < $threshold) {
      continue;
    }
    $bbox = $region['BoundingBox'];
    $x1 = (int) floor($bbox['Left'] * $imwidth) - $pad_x;
    $y1 = (int) floor($bbox['Top'] * $imheight) - $pad_y;
    $x2 = (int) ceil(($bbox['Left'] + $bbox['Width']) * $imwidth) + $pad_x;
    $y2 = (int) ceil(($bbox['Top'] + $bbox['Height']) * $imheight) + $pad_y;
    $x1 = max(0, min($imwidth - 1, $x1));
    $y1 = max(0, min($imheight - 1, $y1));
    $x2 = max($x1 + 1, min($imwidth, $x2));
    $y2 = max($y1 + 1, min($imheight, $y2));
    imagefilledrectangle($im, $x1, $y1, $x2, $y2, $clr);
    $painted++;
  }

  $saved = rovas_ai_gd_encode($im, $mime);
  imagedestroy($im);
  if (empty($saved['ok'])) {
    return array(
      'ok' => FALSE,
      'bytes' => '',
      'mime' => $mime,
      'regions_painted' => 0,
      'error' => 'encode_failed',
    );
  }

  return array(
    'ok' => TRUE,
    'bytes' => $saved['bytes'],
    'mime' => $saved['mime'],
    'regions_painted' => $painted,
    'error' => '',
  );
}

/**
 * Encode a GD image resource to bytes.
 *
 * @param resource $im
 * @param string $mime
 *
 * @return array
 *   ok, bytes, mime
 */
function rovas_ai_gd_encode($im, $mime) {
  $out_mime = $mime;
  ob_start();
  $ok = FALSE;
  if ($mime === 'image/png' && function_exists('imagepng')) {
    $ok = imagepng($im);
    $out_mime = 'image/png';
  }
  elseif ($mime === 'image/webp' && function_exists('imagewebp')) {
    $ok = imagewebp($im, NULL, 90);
    $out_mime = 'image/webp';
  }
  else {
    // Default JPEG (and fallback when webp encode missing).
    $ok = imagejpeg($im, NULL, 90);
    $out_mime = 'image/jpeg';
  }
  $data = ob_get_clean();
  if (!$ok || $data === FALSE || $data === '') {
    return array('ok' => FALSE, 'bytes' => '', 'mime' => $out_mime);
  }
  return array('ok' => TRUE, 'bytes' => $data, 'mime' => $out_mime);
}

/**
 * Best-effort mime from claim + magic bytes.
 *
 * @param string $mime
 * @param string $bytes
 *
 * @return string
 */
function rovas_ai_normalize_image_mime($mime, $bytes = '') {
  $mime = drupal_strtolower(trim((string) $mime));
  if ($mime === 'image/jpg') {
    $mime = 'image/jpeg';
  }
  if (in_array($mime, array('image/jpeg', 'image/png', 'image/webp', 'image/gif'), TRUE)) {
    return $mime === 'image/gif' ? 'image/png' : $mime;
  }
  if (is_string($bytes) && strlen($bytes) >= 12) {
    if (substr($bytes, 0, 3) === "\xFF\xD8\xFF") {
      return 'image/jpeg';
    }
    if (substr($bytes, 0, 8) === "\x89PNG\r\n\x1a\n") {
      return 'image/png';
    }
    if (substr($bytes, 0, 4) === 'RIFF' && substr($bytes, 8, 4) === 'WEBP') {
      return 'image/webp';
    }
  }
  return 'image/jpeg';
}

/**
 * Append _rovas_edited before extension when file was painted.
 *
 * @param string $filename
 *
 * @return string
 */
function rovas_ai_proof_edited_filename($filename) {
  $filename = preg_replace('/[^a-zA-Z0-9._-]/', '_', (string) $filename);
  if ($filename === '' || $filename === '_') {
    $filename = 'photo.jpg';
  }
  if (preg_match('/_rovas_edited(\.[^.]+)?$/i', $filename)) {
    return $filename;
  }
  $dot = strrpos($filename, '.');
  if ($dot === FALSE) {
    return $filename . '_rovas_edited';
  }
  return substr($filename, 0, $dot) . '_rovas_edited' . substr($filename, $dot);
}

/**
 * Persist a proof-photo edit row.
 *
 * @param int $fid
 * @param int $nid
 * @param string $mode
 * @param int $regions
 */
function rovas_ai_photo_edit_record($fid, $nid, $mode, $regions) {
  $fid = (int) $fid;
  $nid = (int) $nid;
  if ($fid < 1 || !db_table_exists('rovas_ai_photo_edit')) {
    return;
  }
  $mode = rovas_ai_normalize_photo_redact_mode($mode);
  db_merge('rovas_ai_photo_edit')
    ->key(array('fid' => $fid))
    ->fields(array(
      'nid' => $nid,
      'mode' => $mode,
      'regions' => max(0, (int) $regions),
      'created' => REQUEST_TIME,
    ))
    ->execute();
}

/**
 * Load photo edit metadata for a work report (or by fids).
 *
 * @param int $nid
 * @param array $fids
 *   Optional filter.
 *
 * @return array
 *   List of API-shaped photo_edits (url filled by caller if needed).
 */
function rovas_ai_photo_edits_for_nid($nid, array $fids = array()) {
  $nid = (int) $nid;
  if ($nid < 1 || !db_table_exists('rovas_ai_photo_edit')) {
    return array();
  }
  $q = db_select('rovas_ai_photo_edit', 'e')
    ->fields('e')
    ->condition('e.nid', $nid);
  if (!empty($fids)) {
    $q->condition('e.fid', array_map('intval', $fids), 'IN');
  }
  $q->orderBy('e.created', 'ASC');
  $rows = $q->execute()->fetchAll();
  $out = array();
  $message = rovas_ai_proof_edit_message();
  foreach ($rows as $row) {
    if ((int) $row->regions < 1) {
      continue;
    }
    $file = file_load((int) $row->fid);
    $filename = $file ? $file->filename : '';
    $url = '';
    if ($file && !empty($file->uri)) {
      $scheme = file_uri_scheme($file->uri);
      if (!$scheme || $scheme === 'public') {
        $url = file_create_url($file->uri);
      }
    }
    $out[] = array(
      'fid' => (int) $row->fid,
      'filename' => $filename,
      'url' => $url ? $url : NULL,
      'mode' => (string) $row->mode,
      'regions_redacted' => (int) $row->regions,
      'message' => $message,
    );
  }
  return $out;
}

/**
 * Redact bytes of an existing managed file in place (web upload path).
 *
 * Never leaves unredacted content at the public URI when mode ≠ none and
 * regions were found: writes redacted bytes over the same URI.
 *
 * @param object $file
 *   Drupal file entity.
 * @param string $mode
 * @param array $options
 *   uid, nid (optional, for recording).
 *
 * @return array
 *   Same shape as rovas_ai_redact_proof_image plus file (updated object|null).
 */
function rovas_ai_redact_managed_file($file, $mode, array $options = array()) {
  $mode = rovas_ai_normalize_photo_redact_mode($mode);
  if ($mode === 'none' || empty($file->fid) || empty($file->uri)) {
    return array(
      'ok' => TRUE,
      'bytes' => '',
      'mime' => isset($file->filemime) ? $file->filemime : '',
      'regions_redacted' => 0,
      'edited' => FALSE,
      'error' => '',
      'regions' => array(),
      'file' => $file,
    );
  }

  $path = drupal_realpath($file->uri);
  if ($path === FALSE || !is_readable($path)) {
    return array(
      'ok' => FALSE,
      'bytes' => '',
      'mime' => '',
      'regions_redacted' => 0,
      'edited' => FALSE,
      'error' => 'unreadable',
      'regions' => array(),
      'file' => NULL,
    );
  }

  $bytes = file_get_contents($path);
  $mime = !empty($file->filemime) ? $file->filemime : file_get_mimetype($file->uri);
  $result = rovas_ai_redact_proof_image($bytes, $mime, $mode, $options);
  $result['file'] = $file;
  if (empty($result['ok'])) {
    return $result;
  }

  if (!empty($result['edited']) && $result['regions_redacted'] > 0) {
    $new_name = rovas_ai_proof_edited_filename($file->filename);
    // Overwrite same URI contents first so public path never keeps original.
    if (file_unmanaged_save_data($result['bytes'], $file->uri, FILE_EXISTS_REPLACE) === FALSE) {
      $result['ok'] = FALSE;
      $result['error'] = 'save_failed';
      $result['edited'] = FALSE;
      return $result;
    }
    $file->filemime = $result['mime'];
    $file->filename = $new_name;
    // Optionally rename URI basename for clarity.
    $dir = drupal_dirname($file->uri);
    $dest = $dir . '/' . $new_name;
    if ($dest !== $file->uri) {
      $moved = file_unmanaged_move($file->uri, $dest, FILE_EXISTS_RENAME);
      if ($moved) {
        $file->uri = $moved;
        $file->filename = drupal_basename($moved);
      }
    }
    file_save($file);
    if (!empty($options['nid'])) {
      rovas_ai_photo_edit_record((int) $file->fid, (int) $options['nid'], $mode, (int) $result['regions_redacted']);
    }
    $result['file'] = $file;
  }

  return $result;
}
