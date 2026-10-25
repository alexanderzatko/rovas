<?php

/**
 * @file
 * Proof-photo PII / face detection (vision) and GD blackout.
 */

define('ROVAS_AI_PROOF_REDACT_CONFIDENCE', 0.3);
/** Output budget for region JSON; keep modest — OpenRouter 402s if max_tokens > credits. */
define('ROVAS_AI_PROOF_REDACT_MAX_TOKENS', 2000);
define('ROVAS_AI_PROOF_EDIT_MESSAGE', 'Your image was edited by Rovas');
/** Expand each painted box by this fraction of image width/height (edge pad). */
define('ROVAS_AI_PROOF_BOX_PAD', 0.018);
/** Extra dilation as a fraction of each box’s own width/height (vision undershoot). */
define('ROVAS_AI_PROOF_BOX_DILATE', 0.35);
/** Soft cap: hitting this many regions means the model likely exhausted the left-to-right scan. */
define('ROVAS_AI_PROOF_REGION_SOFT_CAP', 28);
/** Wide canvases (dual-window screenshots) are split into overlapping vertical tiles. */
define('ROVAS_AI_PROOF_WIDE_ASPECT', 1.55);

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
  // Detect may use 1–4 LLM calls (tiled wide screenshots); count once per action.
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

  // Paint on the same canvas the model saw, then scale back to the original
  // pixel size (avoids coordinate skew from OpenAI’s internal resize).
  $paint_bytes = !empty($detect['vision_bytes']) ? $detect['vision_bytes'] : $bytes;
  $paint_mime = !empty($detect['vision_mime']) ? $detect['vision_mime'] : $mime;
  $painted = rovas_ai_image_paint_boxes($paint_bytes, $paint_mime, $regions);
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

  $final_bytes = $painted['bytes'];
  $final_mime = $painted['mime'];
  $ow = isset($detect['orig_width']) ? (int) $detect['orig_width'] : 0;
  $oh = isset($detect['orig_height']) ? (int) $detect['orig_height'] : 0;
  $vw = isset($detect['vision_width']) ? (int) $detect['vision_width'] : 0;
  $vh = isset($detect['vision_height']) ? (int) $detect['vision_height'] : 0;
  if ($ow > 0 && $oh > 0 && ($ow !== $vw || $oh !== $vh)) {
    $scaled = rovas_ai_image_rescale($final_bytes, $final_mime, $ow, $oh, $mime);
    if (!empty($scaled['ok'])) {
      $final_bytes = $scaled['bytes'];
      $final_mime = $scaled['mime'];
    }
  }

  $count = isset($painted['regions_painted']) ? (int) $painted['regions_painted'] : 0;
  watchdog('rovas_ai', 'Proof PII paint: @p of @n region(s) applied', array(
    '@p' => $count,
    '@n' => count($regions),
  ), WATCHDOG_NOTICE);

  return array(
    'ok' => TRUE,
    'bytes' => $final_bytes,
    'mime' => $final_mime,
    'regions_redacted' => $count,
    'edited' => $count > 0,
    'error' => '',
    'regions' => $regions,
  );
}

/**
 * Vision detect of PII / persons; returns normalized boxes on the vision canvas.
 *
 * Pre-scales to OpenAI detail:high size. Wide dual-window screenshots are split
 * into overlapping vertical tiles so one pass cannot exhaust the region budget
 * on a single pane.
 *
 * @param string $bytes
 * @param string $mime
 * @param string $mode
 *   text|text_faces
 * @param array $options
 *
 * @return array
 *   ok, regions, error, vision_* , orig_*
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
  $tiles = rovas_ai_proof_pii_tile_plan($vw, $vh);

  $regions = array();
  $finish_parts = array();
  $any_ok = FALSE;
  $last_error = 'chat_failed';

  foreach ($tiles as $ti => $tile) {
    $crop = rovas_ai_proof_crop_bytes($vision_bytes, $vision_mime, $tile['x'], $tile['y'], $tile['w'], $tile['h']);
    if (empty($crop['ok'])) {
      $last_error = !empty($crop['error']) ? $crop['error'] : 'crop_failed';
      continue;
    }
    $tw = (int) $crop['width'];
    $th = (int) $crop['height'];
    $data_url = 'data:' . $crop['mime'] . ';base64,' . base64_encode($crop['bytes']);

    // Fine-first per tile (tiling keeps density manageable). Compact only if truncated.
    $pass = rovas_ai_proof_pii_vision_pass($data_url, $tw, $th, $mode, FALSE);
    if (empty($pass['ok'])) {
      $last_error = !empty($pass['error']) ? $pass['error'] : 'chat_failed';
      // One failed tile should not discard others on multi-tile runs.
      if (count($tiles) === 1) {
        return array(
          'ok' => FALSE,
          'regions' => array(),
          'error' => $last_error,
        );
      }
      continue;
    }

    $any_ok = TRUE;
    $finish = isset($pass['finish_reason']) ? $pass['finish_reason'] : '';
    $tile_regions = isset($pass['regions']) ? $pass['regions'] : array();

    if ($finish === 'length') {
      watchdog('rovas_ai', 'Proof PII tile @i compact retry (finish=length regions=@n @twx@th)', array(
        '@i' => $ti,
        '@n' => count($tile_regions),
        '@tw' => $tw,
        '@th' => $th,
      ), WATCHDOG_NOTICE);
      $pass2 = rovas_ai_proof_pii_vision_pass($data_url, $tw, $th, $mode, TRUE);
      if (!empty($pass2['ok']) && !empty($pass2['regions'])
        && (empty($pass2['finish_reason']) || $pass2['finish_reason'] !== 'length')) {
        $tile_regions = $pass2['regions'];
        $finish = isset($pass2['finish_reason']) ? $pass2['finish_reason'] : 'stop';
      }
      else {
        $last_error = 'truncated_regions';
        if (count($tiles) === 1) {
          return array(
            'ok' => FALSE,
            'regions' => array(),
            'error' => 'truncated_regions',
          );
        }
        continue;
      }
    }

    $mapped = rovas_ai_proof_map_tile_regions($tile_regions, $tile, $vw, $vh);
    $regions = array_merge($regions, $mapped);
    $finish_parts[] = 't' . $ti . ':' . $finish . ':' . count($mapped);
  }

  if (!$any_ok) {
    return array(
      'ok' => FALSE,
      'regions' => array(),
      'error' => $last_error,
    );
  }

  // Only merge strong overlaps — loose “near” merging produced huge L-shaped blobs.
  $regions = rovas_ai_merge_proof_regions($regions);
  $regions = rovas_ai_dilate_proof_regions($regions);
  $sample = array();
  foreach (array_slice($regions, 0, 8) as $r) {
    if (!empty($r['BoundingBox'])) {
      $b = $r['BoundingBox'];
      $sample[] = (isset($r['type']) ? $r['type'] : '?') . ':' . round($b['Left'], 3) . ',' . round($b['Top'], 3) . ',' . round($b['Width'], 3) . ',' . round($b['Height'], 3);
    }
  }
  $x_span = rovas_ai_proof_regions_x_span($regions);
  watchdog('rovas_ai', 'Proof PII detect: @n region(s) on @w x @h vision canvas (orig @ow x @oh, tiles=@t, finish=@f, x=@x) sample=@s', array(
    '@n' => count($regions),
    '@w' => $vw,
    '@h' => $vh,
    '@ow' => (int) $prepared['orig_width'],
    '@oh' => (int) $prepared['orig_height'],
    '@t' => count($tiles),
    '@f' => implode(',', $finish_parts),
    '@x' => $x_span,
    '@s' => implode('; ', $sample),
  ), WATCHDOG_NOTICE);

  return array(
    'ok' => TRUE,
    'regions' => $regions,
    'error' => '',
    'vision_bytes' => $vision_bytes,
    'vision_mime' => $vision_mime,
    'vision_width' => $vw,
    'vision_height' => $vh,
    'orig_width' => (int) $prepared['orig_width'],
    'orig_height' => (int) $prepared['orig_height'],
  );
}

/**
 * Vertical tile plan for vision detection.
 *
 * @param int $vw
 * @param int $vh
 *
 * @return array
 *   List of {x,y,w,h} in vision-canvas pixels.
 */
function rovas_ai_proof_pii_tile_plan($vw, $vh) {
  $vw = max(1, (int) $vw);
  $vh = max(1, (int) $vh);
  $aspect = $vw / $vh;
  // Single tile for portrait / modest landscape.
  if ($aspect < ROVAS_AI_PROOF_WIDE_ASPECT || $vw < 1100) {
    return array(array('x' => 0, 'y' => 0, 'w' => $vw, 'h' => $vh));
  }
  // Two overlapping halves (covers dual-window desktop screenshots).
  $overlap = (int) max(40, round($vw * 0.08));
  $half = (int) ceil(($vw + $overlap) / 2);
  if ($half >= $vw) {
    return array(array('x' => 0, 'y' => 0, 'w' => $vw, 'h' => $vh));
  }
  $x2 = max(0, $vw - $half);
  return array(
    array('x' => 0, 'y' => 0, 'w' => $half, 'h' => $vh),
    array('x' => $x2, 'y' => 0, 'w' => $vw - $x2, 'h' => $vh),
  );
}

/**
 * Crop image bytes to a pixel rectangle.
 *
 * @param string $bytes
 * @param string $mime
 * @param int $x
 * @param int $y
 * @param int $w
 * @param int $h
 *
 * @return array
 *   ok, bytes, mime, width, height, error
 */
function rovas_ai_proof_crop_bytes($bytes, $mime, $x, $y, $w, $h) {
  if (!function_exists('imagecreatefromstring')) {
    return array('ok' => FALSE, 'error' => 'gd_unavailable');
  }
  $im = @imagecreatefromstring($bytes);
  if ($im === FALSE) {
    return array('ok' => FALSE, 'error' => 'decode_failed');
  }
  $iw = imagesx($im);
  $ih = imagesy($im);
  $x = max(0, (int) $x);
  $y = max(0, (int) $y);
  $w = max(1, min((int) $w, $iw - $x));
  $h = max(1, min((int) $h, $ih - $y));
  if ($x === 0 && $y === 0 && $w === $iw && $h === $ih) {
    imagedestroy($im);
    return array(
      'ok' => TRUE,
      'bytes' => $bytes,
      'mime' => $mime,
      'width' => $iw,
      'height' => $ih,
      'error' => '',
    );
  }
  $dst = imagecreatetruecolor($w, $h);
  if ($dst === FALSE) {
    imagedestroy($im);
    return array('ok' => FALSE, 'error' => 'crop_failed');
  }
  imagecopy($dst, $im, 0, 0, $x, $y, $w, $h);
  imagedestroy($im);
  $encoded = rovas_ai_gd_encode($dst, 'image/jpeg');
  imagedestroy($dst);
  if (empty($encoded['ok'])) {
    return array('ok' => FALSE, 'error' => 'encode_failed');
  }
  return array(
    'ok' => TRUE,
    'bytes' => $encoded['bytes'],
    'mime' => $encoded['mime'],
    'width' => $w,
    'height' => $h,
    'error' => '',
  );
}

/**
 * Map tile-normalized regions onto the full vision canvas.
 *
 * @param array $regions
 * @param array $tile
 *   x,y,w,h in canvas pixels.
 * @param int $vw
 * @param int $vh
 *
 * @return array
 */
function rovas_ai_proof_map_tile_regions(array $regions, array $tile, $vw, $vh) {
  $vw = max(1, (int) $vw);
  $vh = max(1, (int) $vh);
  $tx = (float) $tile['x'];
  $ty = (float) $tile['y'];
  $tw = max(1.0, (float) $tile['w']);
  $th = max(1.0, (float) $tile['h']);
  $out = array();
  foreach ($regions as $r) {
    if (empty($r['BoundingBox']) || !is_array($r['BoundingBox'])) {
      continue;
    }
    $b = $r['BoundingBox'];
    $left = ($tx + $b['Left'] * $tw) / $vw;
    $top = ($ty + $b['Top'] * $th) / $vh;
    $width = ($b['Width'] * $tw) / $vw;
    $height = ($b['Height'] * $th) / $vh;
    $left = max(0.0, min(1.0, $left));
    $top = max(0.0, min(1.0, $top));
    $width = max(0.0, min(1.0 - $left, $width));
    $height = max(0.0, min(1.0 - $top, $height));
    if ($width < 0.002 || $height < 0.002) {
      continue;
    }
    if ($width * $height > 0.35) {
      continue;
    }
    $out[] = array(
      'type' => isset($r['type']) ? $r['type'] : 'name',
      'Confidence' => isset($r['Confidence']) ? $r['Confidence'] : 90,
      'BoundingBox' => array(
        'Left' => $left,
        'Top' => $top,
        'Width' => $width,
        'Height' => $height,
      ),
    );
  }
  return $out;
}

/**
 * Debug helper: min..max Left span of regions.
 *
 * @param array $regions
 *
 * @return string
 */
function rovas_ai_proof_regions_x_span(array $regions) {
  $min = 1.0;
  $max = 0.0;
  $n = 0;
  foreach ($regions as $r) {
    if (empty($r['BoundingBox'])) {
      continue;
    }
    $b = $r['BoundingBox'];
    $l = (float) $b['Left'];
    $rgt = $l + (float) $b['Width'];
    if ($l < $min) {
      $min = $l;
    }
    if ($rgt > $max) {
      $max = $rgt;
    }
    $n++;
  }
  if ($n < 1) {
    return 'none';
  }
  return round($min, 3) . '..' . round($max, 3);
}

/**
 * One vision chat pass for proof PII boxes.
 *
 * @param string $data_url
 * @param int $vw
 * @param int $vh
 * @param string $mode
 * @param bool $compact
 *   When TRUE, ask for ≤25 merged larger boxes (fits token budget).
 *
 * @return array
 *   ok, regions, finish_reason, error
 */
function rovas_ai_proof_pii_vision_pass($data_url, $vw, $vh, $mode, $compact = FALSE) {
  $include_faces = ($mode === 'text_faces');
  $types = $include_faces
    ? 'email, phone, name, person'
    : 'email, phone, name';

  // 0–1000 grid is the localization contract OpenAI documents for vision models.
  $coord = 'bbox MUST be [x_min,y_min,x_max,y_max] on a 0–1000 grid for THIS image '
    . '(' . (int) $vw . '×' . (int) $vh . ' px). '
    . '0,0 = top-left; 1000,1000 = bottom-right; integers; x_max > x_min; y_max > y_min. '
    . 'NOT pixels, NOT 0–1 floats, NOT [x,y,width,height]. '
    . 'Each box must FULLY cover the sensitive glyphs with a small margin '
    . '(especially phone number lines and long To:/recipient rows).';

  $must = 'MUST box: From/To/Cc lines, recipient chip rows, signature names, phone numbers, '
    . 'email addresses, personal names in greetings/closings. '
    . 'Skip prices, dates, and org-only labels. ';

  if ($compact) {
    $system = 'You locate PII in a screenshot for black-box redaction. '
      . 'Return ONLY JSON: {"regions":[{"type":"email|phone|name'
      . ($include_faces ? '|person' : '')
      . '","bbox":[x_min,y_min,x_max,y_max],"confidence":0.9}]}. '
      . $coord
      . ' CRITICAL: at most 18 regions. Prefer FEWER LARGER boxes. '
      . $must
      . 'Complete JSON.';
    $user_text = 'Mode ' . $mode . '. ≤18 merged ' . $types . ' boxes on 0–1000 grid; cover every name/email/phone. JSON only.';
    $max_tokens = min(ROVAS_AI_PROOF_REDACT_MAX_TOKENS, 1600);
  }
  else {
    $system = 'You locate PII in a screenshot for black-box redaction. '
      . 'Return ONLY JSON: {"regions":[{"type":"email|phone|name'
      . ($include_faces ? '|person' : '')
      . '","bbox":[x_min,y_min,x_max,y_max],"confidence":0.9}]}. '
      . $coord
      . ' At most 30 regions; one box per distinct sensitive span (merge only when adjacent). '
      . $must
      . 'Finish the full JSON object.';
    $user_text = 'Mode ' . $mode . '. ≤30 ' . $types . ' regions; bbox=[x_min,y_min,x_max,y_max] on 0–1000 for '
      . (int) $vw . 'x' . (int) $vh . '. Cover To-lines, recipient lists, signatures, phones. JSON only.';
    $max_tokens = ROVAS_AI_PROOF_REDACT_MAX_TOKENS;
  }

  $messages = array(
    array('role' => 'system', 'content' => $system),
    array(
      'role' => 'user',
      'content' => array(
        array('type' => 'text', 'text' => $user_text),
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
    'max_tokens' => $max_tokens,
    'timeout' => 120,
  ));

  if (empty($chat['ok'])) {
    return array(
      'ok' => FALSE,
      'regions' => array(),
      'finish_reason' => '',
      'error' => !empty($chat['error']) ? $chat['error'] : 'chat_failed',
    );
  }

  module_load_include('inc', 'rovas_ai', 'rovas_ai.classify');
  $content = isset($chat['content']) ? $chat['content'] : '';
  $finish = isset($chat['finish_reason']) ? $chat['finish_reason'] : '';
  $parsed = rovas_ai_parse_json_object($content);

  if (!is_array($parsed)) {
    watchdog('rovas_ai', 'Proof PII invalid_json (finish=@f compact=@c); head: @h', array(
      '@f' => $finish,
      '@c' => $compact ? '1' : '0',
      '@h' => drupal_substr(preg_replace('/\s+/', ' ', $content), 0, 400),
    ), WATCHDOG_WARNING);
    return array(
      'ok' => FALSE,
      'regions' => array(),
      'finish_reason' => $finish,
      'error' => 'invalid_json',
    );
  }

  $raw = array();
  if (!empty($parsed['regions']) && is_array($parsed['regions'])) {
    $raw = $parsed['regions'];
  }
  elseif (isset($parsed[0]) && is_array($parsed[0])) {
    $raw = $parsed;
  }

  return array(
    'ok' => TRUE,
    'regions' => rovas_ai_normalize_proof_regions($raw, $mode, $vw, $vh),
    'finish_reason' => $finish,
    'error' => '',
  );
}

/**
 * Merge heavily overlapping normalized regions (reduces paint noise).
 *
 * @param array $regions
 *
 * @return array
 */
function rovas_ai_merge_proof_regions(array $regions) {
  if (count($regions) < 2) {
    return $regions;
  }
  $boxes = array();
  foreach ($regions as $r) {
    if (empty($r['BoundingBox'])) {
      continue;
    }
    $b = $r['BoundingBox'];
    $boxes[] = array(
      'type' => isset($r['type']) ? $r['type'] : 'name',
      'Confidence' => isset($r['Confidence']) ? $r['Confidence'] : 90,
      'L' => $b['Left'],
      'T' => $b['Top'],
      'R' => $b['Left'] + $b['Width'],
      'B' => $b['Top'] + $b['Height'],
    );
  }
  $merged = TRUE;
  while ($merged && count($boxes) > 1) {
    $merged = FALSE;
    $n = count($boxes);
    for ($i = 0; $i < $n; $i++) {
      for ($j = $i + 1; $j < $n; $j++) {
        $a = $boxes[$i];
        $b = $boxes[$j];
        $inter_l = max($a['L'], $b['L']);
        $inter_t = max($a['T'], $b['T']);
        $inter_r = min($a['R'], $b['R']);
        $inter_b = min($a['B'], $b['B']);
        $inter = ($inter_r > $inter_l && $inter_b > $inter_t)
          ? ($inter_r - $inter_l) * ($inter_b - $inter_t) : 0.0;
        $area_a = max(1e-9, ($a['R'] - $a['L']) * ($a['B'] - $a['T']));
        $area_b = max(1e-9, ($b['R'] - $b['L']) * ($b['B'] - $b['T']));
        // Strict IoU-style merge only (loose “near” merge created giant blobs).
        if ($inter / min($area_a, $area_b) >= 0.55) {
          $boxes[$i] = array(
            'type' => $a['type'],
            'Confidence' => max($a['Confidence'], $b['Confidence']),
            'L' => min($a['L'], $b['L']),
            'T' => min($a['T'], $b['T']),
            'R' => max($a['R'], $b['R']),
            'B' => max($a['B'], $b['B']),
          );
          array_splice($boxes, $j, 1);
          $merged = TRUE;
          break 2;
        }
      }
    }
  }

  $out = array();
  foreach ($boxes as $b) {
    $out[] = array(
      'type' => $b['type'],
      'Confidence' => $b['Confidence'],
      'BoundingBox' => array(
        'Left' => $b['L'],
        'Top' => $b['T'],
        'Width' => max(0.001, $b['R'] - $b['L']),
        'Height' => max(0.001, $b['B'] - $b['T']),
      ),
    );
  }
  return $out;
}

/**
 * Rescale image bytes to target pixel size.
 *
 * @param string $bytes
 * @param string $mime
 * @param int $tw
 * @param int $th
 * @param string $out_mime
 *
 * @return array
 *   ok, bytes, mime, error
 */
function rovas_ai_image_rescale($bytes, $mime, $tw, $th, $out_mime = '') {
  if (!function_exists('imagecreatefromstring')) {
    return array('ok' => FALSE, 'error' => 'gd_unavailable');
  }
  $im = @imagecreatefromstring($bytes);
  if ($im === FALSE) {
    return array('ok' => FALSE, 'error' => 'decode_failed');
  }
  $tw = max(1, (int) $tw);
  $th = max(1, (int) $th);
  $dst = imagecreatetruecolor($tw, $th);
  if ($dst === FALSE) {
    imagedestroy($im);
    return array('ok' => FALSE, 'error' => 'resize_failed');
  }
  imagecopyresampled($dst, $im, 0, 0, 0, 0, $tw, $th, imagesx($im), imagesy($im));
  imagedestroy($im);
  $target_mime = $out_mime !== '' ? $out_mime : $mime;
  $encoded = rovas_ai_gd_encode($dst, $target_mime);
  imagedestroy($dst);
  if (empty($encoded['ok'])) {
    return array('ok' => FALSE, 'error' => 'encode_failed');
  }
  return array(
    'ok' => TRUE,
    'bytes' => $encoded['bytes'],
    'mime' => $encoded['mime'],
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
 * @param int $img_w
 *   Vision canvas width (for pixel bbox_px).
 * @param int $img_h
 *
 * @return array
 *   List of {type, BoundingBox{Left,Top,Width,Height}, Confidence}
 */
function rovas_ai_normalize_proof_regions(array $raw, $mode, $img_w = 0, $img_h = 0) {
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

    $box = rovas_ai_normalize_bounding_box($item, $img_w, $img_h);
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
 * Prefers bbox on a 0–1000 grid (vision localization contract). Also accepts
 * bbox_px pixels, 0–1 floats, or Rekognition-style maps. Rejects huge blobs.
 *
 * @param array $item
 * @param int $img_w
 * @param int $img_h
 *
 * @return array|null
 */
function rovas_ai_normalize_bounding_box(array $item, $img_w = 0, $img_h = 0) {
  $x0 = $y0 = $x1 = $y1 = NULL;
  // pixels | grid1000 | unit
  $space = '';

  // Explicit pixel boxes.
  if (!empty($item['bbox_px']) && is_array($item['bbox_px'])
    && isset($item['bbox_px'][0], $item['bbox_px'][1], $item['bbox_px'][2], $item['bbox_px'][3])) {
    $x0 = (float) $item['bbox_px'][0];
    $y0 = (float) $item['bbox_px'][1];
    $x1 = (float) $item['bbox_px'][2];
    $y1 = (float) $item['bbox_px'][3];
    $space = 'pixels';
  }

  // Preferred: bbox / box as 4-tuple on 0–1000 (or legacy pixels).
  if ($x0 === NULL) {
    foreach (array('bbox', 'box') as $key) {
      if (!isset($item[$key]) || !is_array($item[$key])) {
        continue;
      }
      $arr = $item[$key];
      if (isset($arr[0], $arr[1], $arr[2], $arr[3])
        && !isset($arr['Left']) && !isset($arr['left']) && !isset($arr['Width'])) {
        $x0 = (float) $arr[0];
        $y0 = (float) $arr[1];
        $x1 = (float) $arr[2];
        $y1 = (float) $arr[3];
        // Default tuple from current prompts = 0–1000 grid.
        $space = 'grid1000';
        break;
      }
    }
  }

  if ($x0 === NULL) {
    $src = NULL;
    if (!empty($item['BoundingBox']) && is_array($item['BoundingBox'])) {
      $src = $item['BoundingBox'];
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
      $space = (max($x0, $x1, $y0, $y1) > 1.5) ? 'grid1000' : 'unit';
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
      if ($width <= 1.5 && $height <= 1.5 && $left <= 1.5 && $top <= 1.5) {
        $space = 'unit';
      }
      elseif ($img_w > 0 && $width > 1.5) {
        $space = 'pixels';
      }
      else {
        $space = 'grid1000';
      }
    }
  }

  // Pixel xywh salvage (only when explicitly pixel-space).
  if ($space === 'pixels' && $img_w > 0 && $img_h > 0 && $x1 > 0 && $y1 > 0
    && $x1 <= $img_w && $y1 <= $img_h
    && ($x0 + $x1) <= $img_w * 1.02 && ($y0 + $y1) <= $img_h * 1.02
    && $x1 < $x0) {
    $x1 = $x0 + $x1;
    $y1 = $y0 + $y1;
  }

  // Scale → 0–1. Prefer 0–1000 over “looks like pixels” when space is grid1000,
  // otherwise tile crops (~900px) mis-read 0–1000 values as pixels.
  if ($space === 'unit' || ($x1 <= 1.5 && $y1 <= 1.5 && $x0 <= 1.5 && $y0 <= 1.5)) {
    // Already normalized.
  }
  elseif ($space === 'pixels' && $img_w > 1 && $img_h > 1) {
    $x0 /= $img_w;
    $x1 /= $img_w;
    $y0 /= $img_h;
    $y1 /= $img_h;
  }
  elseif ($space === 'grid1000' || max($x0, $x1, $y0, $y1) <= 1000.5) {
    $x0 /= 1000.0;
    $y0 /= 1000.0;
    $x1 /= 1000.0;
    $y1 /= 1000.0;
  }
  elseif ($img_w > 1 && $img_h > 1 && max($x0, $x1) <= $img_w * 1.05 && max($y0, $y1) <= $img_h * 1.05) {
    $x0 /= $img_w;
    $x1 /= $img_w;
    $y0 /= $img_h;
    $y1 /= $img_h;
  }
  else {
    $x0 /= 1000.0;
    $y0 /= 1000.0;
    $x1 /= 1000.0;
    $y1 /= 1000.0;
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
  if ($width < 0.002 || $height < 0.002) {
    return NULL;
  }
  // Drop nonsense full-canvas / huge blobs from bad model output.
  if ($width * $height > 0.35) {
    return NULL;
  }
  if ($width > 0.85 && $height > 0.25) {
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
 * Expand normalized boxes to compensate for vision undershoot.
 *
 * @param array $regions
 *
 * @return array
 */
function rovas_ai_dilate_proof_regions(array $regions) {
  $d = ROVAS_AI_PROOF_BOX_DILATE;
  if ($d <= 0) {
    return $regions;
  }
  $out = array();
  foreach ($regions as $r) {
    if (empty($r['BoundingBox']) || !is_array($r['BoundingBox'])) {
      continue;
    }
    $b = $r['BoundingBox'];
    $w = (float) $b['Width'];
    $h = (float) $b['Height'];
    $dx = max(0.006, $w * $d * 0.5);
    $dy = max(0.008, $h * $d * 0.5);
    // Bias expansion downward/right — models often clip descenders and line ends.
    $left = max(0.0, (float) $b['Left'] - $dx * 0.7);
    $top = max(0.0, (float) $b['Top'] - $dy * 0.5);
    $right = min(1.0, (float) $b['Left'] + $w + $dx * 1.3);
    $bottom = min(1.0, (float) $b['Top'] + $h + $dy * 1.5);
    $r['BoundingBox'] = array(
      'Left' => $left,
      'Top' => $top,
      'Width' => max(0.002, $right - $left),
      'Height' => max(0.002, $bottom - $top),
    );
    $out[] = $r;
  }
  return $out;
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
  $pad_x = (int) max(6, round($imwidth * ROVAS_AI_PROOF_BOX_PAD));
  $pad_y = (int) max(8, round($imheight * ROVAS_AI_PROOF_BOX_PAD));

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
    // Extra right/down pad — models often clip line ends and descenders.
    $x2 = (int) ceil(($bbox['Left'] + $bbox['Width']) * $imwidth) + (int) round($pad_x * 1.2);
    $y2 = (int) ceil(($bbox['Top'] + $bbox['Height']) * $imheight) + (int) round($pad_y * 1.6);
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
