<?php

/**
 * @file
 * Proof-photo PII / face detection (vision) and GD blackout.
 */

define('ROVAS_AI_PROOF_REDACT_CONFIDENCE', 0.5);
define('ROVAS_AI_PROOF_REDACT_MAX_TOKENS', 1200);
define('ROVAS_AI_PROOF_EDIT_MESSAGE', 'Your image was edited by Rovas');

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
 * Vision detect of PII / persons; returns normalized boxes.
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
  $b64 = base64_encode($bytes);
  $data_url = 'data:' . $mime . ';base64,' . $b64;

  $include_faces = ($mode === 'text_faces');
  $types = $include_faces
    ? 'email, phone, name, person'
    : 'email, phone, name';

  $system = 'You locate sensitive information visible in a screenshot or photo of work proof. '
    . 'Return ONLY a JSON object: {"regions":[{"type":"<one of: ' . $types . '>","box":{"Left":0,"Top":0,"Width":0,"Height":0},"confidence":0.0}]}. '
    . 'box values are normalized 0–1 relative to image width/height (Rekognition-style: Left/Top origin top-left, Width/Height size). '
    . 'type "email" = email addresses; "phone" = phone numbers; "name" = personal full names of people (not project/org brand names); '
    . ($include_faces
      ? '"person" = human faces or clearly identifiable persons. '
      : 'Do NOT include faces or person boxes. ')
    . 'Omit regions you are unsure about. Use empty regions array if nothing sensitive is visible. '
    . 'Tight boxes around the sensitive text or face only.';

  $user_text = 'Find regions to redact. Mode: ' . $mode . '. Respond with JSON only.';

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
            'detail' => 'high',
          ),
        ),
      ),
    ),
  );

  $chat = rovas_ai_chat($messages, array(
    'task_id' => 'proof_pii_redact',
    'temperature' => 0,
    'max_tokens' => ROVAS_AI_PROOF_REDACT_MAX_TOKENS,
    'timeout' => 90,
  ));

  if (empty($chat['ok'])) {
    return array(
      'ok' => FALSE,
      'regions' => array(),
      'error' => !empty($chat['error']) ? $chat['error'] : 'chat_failed',
    );
  }

  module_load_include('inc', 'rovas_ai', 'rovas_ai.classify');
  $parsed = rovas_ai_parse_json_object(isset($chat['content']) ? $chat['content'] : '');
  if (!is_array($parsed)) {
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

  $regions = rovas_ai_normalize_proof_regions($raw, $mode);
  return array(
    'ok' => TRUE,
    'regions' => $regions,
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
 * @param array $item
 *
 * @return array|null
 */
function rovas_ai_normalize_bounding_box(array $item) {
  $src = NULL;
  if (!empty($item['box']) && is_array($item['box'])) {
    $src = $item['box'];
  }
  elseif (!empty($item['BoundingBox']) && is_array($item['BoundingBox'])) {
    $src = $item['BoundingBox'];
  }
  elseif (isset($item['Left']) || isset($item['left'])) {
    $src = $item;
  }
  if ($src === NULL) {
    return NULL;
  }

  // Support x_min/y_min/x_max/y_max in 0–1 or 0–1000.
  if (isset($src['x_min']) || isset($src['xmin'])) {
    $x0 = isset($src['x_min']) ? (float) $src['x_min'] : (float) $src['xmin'];
    $y0 = isset($src['y_min']) ? (float) $src['y_min'] : (isset($src['ymin']) ? (float) $src['ymin'] : 0);
    $x1 = isset($src['x_max']) ? (float) $src['x_max'] : (isset($src['xmax']) ? (float) $src['xmax'] : $x0);
    $y1 = isset($src['y_max']) ? (float) $src['y_max'] : (isset($src['ymax']) ? (float) $src['ymax'] : $y0);
    if ($x1 > 1.5 || $y1 > 1.5) {
      $x0 /= 1000.0;
      $y0 /= 1000.0;
      $x1 /= 1000.0;
      $y1 /= 1000.0;
    }
    $left = $x0;
    $top = $y0;
    $width = max(0, $x1 - $x0);
    $height = max(0, $y1 - $y0);
  }
  else {
    $left = isset($src['Left']) ? (float) $src['Left'] : (isset($src['left']) ? (float) $src['left'] : NULL);
    $top = isset($src['Top']) ? (float) $src['Top'] : (isset($src['top']) ? (float) $src['top'] : NULL);
    $width = isset($src['Width']) ? (float) $src['Width'] : (isset($src['width']) ? (float) $src['width'] : NULL);
    $height = isset($src['Height']) ? (float) $src['Height'] : (isset($src['height']) ? (float) $src['height'] : NULL);
    if ($left === NULL || $top === NULL || $width === NULL || $height === NULL) {
      return NULL;
    }
    // 0–1000 space → 0–1.
    if ($left > 1.5 || $top > 1.5 || $width > 1.5 || $height > 1.5) {
      $left /= 1000.0;
      $top /= 1000.0;
      $width /= 1000.0;
      $height /= 1000.0;
    }
  }

  $left = max(0.0, min(1.0, $left));
  $top = max(0.0, min(1.0, $top));
  $width = max(0.0, min(1.0 - $left, $width));
  $height = max(0.0, min(1.0 - $top, $height));
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

  foreach ($regions as $region) {
    if (!is_array($region) || empty($region['BoundingBox']) || !is_array($region['BoundingBox'])) {
      continue;
    }
    if (isset($region['Confidence']) && (float) $region['Confidence'] < $threshold) {
      continue;
    }
    $bbox = $region['BoundingBox'];
    $x1 = (int) floor($bbox['Left'] * $imwidth);
    $y1 = (int) floor($bbox['Top'] * $imheight);
    $x2 = (int) ceil(($bbox['Left'] + $bbox['Width']) * $imwidth);
    $y2 = (int) ceil(($bbox['Top'] + $bbox['Height']) * $imheight);
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
