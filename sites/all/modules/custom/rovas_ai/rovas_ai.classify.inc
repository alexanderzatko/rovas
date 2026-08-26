<?php

/**
 * @file
 * Two-pass NACE classification + title suggestion; Services callback.
 */

/**
 * Public classify runner (loaded via rovas_ai_classify_work()).
 *
 * @param string $description
 * @param string $lang
 * @param array $options
 *
 * @return array
 */
function rovas_ai_classify_work_run($description, $lang = 'en', array $options = array()) {
  module_load_include('inc', 'rovas_ai', 'rovas_ai.nace');

  $description = trim((string) $description);
  $lang = rovas_ai_normalize_lang($lang);
  $uid = isset($options['uid']) ? (int) $options['uid'] : 0;

  $empty = array(
    'status' => 'error',
    'nace_tid' => '',
    'nace_code' => '',
    'nace_label' => '',
    'title' => '',
    'clarifying_question' => '',
    'confidence' => 0,
    'error' => '',
  );

  if ($description === '') {
    $empty['status'] = 'need_more_info';
    $empty['clarifying_question'] = rovas_ai_clarify_default($lang, 'empty');
    return $empty;
  }

  if (!rovas_ai_is_configured()) {
    $empty['status'] = 'unavailable';
    $empty['error'] = 'not_configured';
    return $empty;
  }

  if (!rovas_ai_budget_allow($uid)) {
    $empty['status'] = 'unavailable';
    $empty['error'] = 'budget_exceeded';
    return $empty;
  }

  // One budget slot per classify attempt (may be 2–3 LLM calls when top-2 tie).
  rovas_ai_budget_consume($uid);

  $threshold = rovas_ai_confidence_threshold();
  $sections = rovas_ai_nace_sections($lang);
  if (empty($sections)) {
    $empty['status'] = 'unavailable';
    $empty['error'] = 'nace_empty';
    return $empty;
  }

  $pass1 = rovas_ai_classify_pass_section($description, $lang, $sections);
  if ($pass1['status'] !== 'ok') {
    return array_merge($empty, $pass1);
  }

  $candidates = !empty($pass1['candidates']) && is_array($pass1['candidates'])
    ? $pass1['candidates']
    : array();
  if (empty($candidates)) {
    return array_merge($empty, array(
      'status' => 'need_more_info',
      'clarifying_question' => rovas_ai_clarify_default($lang, 'section'),
      'confidence' => 0,
      'error' => 'no_section',
    ));
  }

  // Drop candidates below the admin threshold (keep at least the top one for messaging).
  $viable = array();
  foreach ($candidates as $cand) {
    if (!empty($cand['confidence']) && $cand['confidence'] >= $threshold) {
      $viable[] = $cand;
    }
  }
  if (empty($viable)) {
    $top = $candidates[0];
    return array_merge($empty, array(
      'status' => 'need_more_info',
      'clarifying_question' => !empty($pass1['clarifying_question'])
        ? $pass1['clarifying_question']
        : rovas_ai_clarify_default($lang, 'section'),
      'confidence' => isset($top['confidence']) ? $top['confidence'] : 0,
    ));
  }

  // When top-2 section confidences are close, merge their leaves into ONE
  // catalog so pass 2 makes a single, directly comparative choice. Two
  // separate pass-2 calls (one per section) would each confidently pick the
  // "least bad" option from its own list — LLM confidence is not calibrated
  // across unrelated prompts, so comparing two isolated scores is unreliable.
  $sections_to_use = array($viable[0]);
  $tie = rovas_ai_section_tie_margin();
  if ($tie > 0 && count($viable) >= 2) {
    $gap = abs($viable[0]['confidence'] - $viable[1]['confidence']);
    if ($gap <= $tie) {
      $sections_to_use[] = $viable[1];
    }
  }

  $leaf_lists = array();
  $resolved_sections = array();
  foreach ($sections_to_use as $cand) {
    $section = rovas_ai_nace_section_by_code($cand['section'], $lang);
    if (!$section) {
      continue;
    }
    $resolved_sections[] = $section;
    $leaf_lists[] = rovas_ai_nace_leaves_under($section['tid'], $lang);
  }

  if (empty($resolved_sections)) {
    return array_merge($empty, array(
      'status' => 'need_more_info',
      'clarifying_question' => rovas_ai_clarify_default($lang, 'section'),
      'confidence' => $viable[0]['confidence'],
      'error' => 'unknown_section',
    ));
  }

  $leaves = rovas_ai_nace_merge_leaf_lists($leaf_lists);
  if (empty($leaves)) {
    return array_merge($empty, array(
      'status' => 'need_more_info',
      'clarifying_question' => rovas_ai_clarify_default($lang, 'leaf'),
      'confidence' => $viable[0]['confidence'],
      'error' => 'no_leaves',
    ));
  }

  $multi_section = count($resolved_sections) > 1;
  $pass2 = rovas_ai_classify_pass_leaf($description, $lang, $leaves, $multi_section);
  if ($pass2['status'] !== 'ok') {
    return array_merge($empty, $pass2);
  }

  if ($pass2['confidence'] < $threshold) {
    return array_merge($empty, array(
      'status' => 'need_more_info',
      'clarifying_question' => !empty($pass2['clarifying_question'])
        ? $pass2['clarifying_question']
        : rovas_ai_clarify_default($lang, 'leaf'),
      'confidence' => $pass2['confidence'],
    ));
  }

  $leaf = rovas_ai_nace_find_in_list($leaves, $pass2['code']);
  if (!$leaf) {
    return array_merge($empty, array(
      'status' => 'need_more_info',
      'clarifying_question' => rovas_ai_clarify_default($lang, 'leaf'),
      'confidence' => $pass2['confidence'],
      'error' => 'unknown_code',
    ));
  }

  $title = trim((string) $pass2['title']);
  if ($title === '') {
    $title = drupal_substr($description, 0, 80);
  }
  if (drupal_strlen($title) > 120) {
    $title = drupal_substr($title, 0, 117) . '…';
  }

  return array(
    'status' => 'ok',
    'nace_tid' => (string) $leaf['tid'],
    'nace_code' => $leaf['code'],
    'nace_label' => $leaf['label'],
    'title' => $title,
    'clarifying_question' => '',
    'confidence' => $pass2['confidence'],
    'error' => '',
  );
}

/**
 * Pass 1: pick top-level NACE section(s).
 *
 * @param string $description
 * @param string $lang
 * @param array $sections
 *
 * @return array
 *   status ok → candidates[] of {section, confidence}, plus legacy section/confidence.
 */
function rovas_ai_classify_pass_section($description, $lang, array $sections) {
  $catalog = rovas_ai_nace_prompt_lines($sections);
  $system = 'You classify volunteer/civic work into NACE Rev.2 section letter(s). '
    . 'Reply with JSON only. Preferred shape: '
    . '{"sections":[{"section":"R","confidence":0.85},{"section":"P","confidence":0.8}]} '
    . 'or {"need_more_info":true,"question":"...","confidence":0.0}. '
    . 'Return 1 or 2 candidates ranked best-first. Include a second section when another '
    . 'letter is plausible (especially when teaching/coaching/instructing could be Education P '
    . 'while the activity setting looks like recreation R, or similar cross-boundary cases). '
    . 'Each section must be a code from the list. confidence is 0-1. '
    . 'question must be one short clarifying question in language code ' . $lang . '.';

  $user = "Work description:\n" . $description . "\n\nNACE sections (code | label):\n" . $catalog;

  $chat = rovas_ai_chat(
    array(
      array('role' => 'system', 'content' => $system),
      array('role' => 'user', 'content' => $user),
    ),
    array(
      'task_id' => 'nace_classify',
      // Keep headroom: OpenRouter sometimes returns empty content when the
      // completion hits max_tokens exactly (finish_reason=length).
      'max_tokens' => 256,
    )
  );

  if (empty($chat['ok'])) {
    return array(
      'status' => 'unavailable',
      'error' => !empty($chat['error']) ? $chat['error'] : 'llm_error',
      'confidence' => 0,
      'clarifying_question' => '',
      'candidates' => array(),
    );
  }

  $parsed = rovas_ai_parse_json_object($chat['content']);
  if ($parsed === NULL) {
    return array(
      'status' => 'unavailable',
      'error' => 'bad_json',
      'confidence' => 0,
      'clarifying_question' => '',
      'candidates' => array(),
    );
  }

  $confidence = isset($parsed['confidence']) ? (float) $parsed['confidence'] : 0.0;
  if (!empty($parsed['need_more_info'])) {
    $q = isset($parsed['question']) ? trim((string) $parsed['question']) : '';
    return array(
      'status' => 'need_more_info',
      'clarifying_question' => $q !== '' ? $q : rovas_ai_clarify_default($lang, 'section'),
      'confidence' => $confidence,
      'error' => '',
      'candidates' => array(),
    );
  }

  $candidates = rovas_ai_classify_parse_section_candidates($parsed);
  if (empty($candidates)) {
    return array(
      'status' => 'need_more_info',
      'clarifying_question' => rovas_ai_clarify_default($lang, 'section'),
      'confidence' => $confidence,
      'error' => '',
      'candidates' => array(),
    );
  }

  return array(
    'status' => 'ok',
    'section' => $candidates[0]['section'],
    'confidence' => $candidates[0]['confidence'],
    'candidates' => $candidates,
    'clarifying_question' => isset($parsed['question']) ? trim((string) $parsed['question']) : '',
    'error' => '',
  );
}

/**
 * Normalize pass-1 JSON into ranked section candidates.
 *
 * Accepts {"sections":[...]} or legacy {"section":"F","confidence":0.9}.
 *
 * @param array $parsed
 *
 * @return array
 *   List of {section, confidence}, max 2, best first.
 */
function rovas_ai_classify_parse_section_candidates(array $parsed) {
  $out = array();
  if (!empty($parsed['sections']) && is_array($parsed['sections'])) {
    foreach ($parsed['sections'] as $row) {
      if (!is_array($row)) {
        continue;
      }
      $code = isset($row['section']) ? trim((string) $row['section']) : '';
      if ($code === '') {
        continue;
      }
      $out[] = array(
        'section' => $code,
        'confidence' => isset($row['confidence']) ? (float) $row['confidence'] : 0.0,
      );
      if (count($out) >= 2) {
        break;
      }
    }
  }
  if (empty($out) && !empty($parsed['section'])) {
    $out[] = array(
      'section' => trim((string) $parsed['section']),
      'confidence' => isset($parsed['confidence']) ? (float) $parsed['confidence'] : 0.0,
    );
    if (!empty($parsed['section2'])) {
      $out[] = array(
        'section' => trim((string) $parsed['section2']),
        'confidence' => isset($parsed['confidence2']) ? (float) $parsed['confidence2'] : 0.0,
      );
    }
  }

  // Dedupe by section letter (keep highest confidence).
  $by_code = array();
  foreach ($out as $cand) {
    $key = drupal_strtoupper($cand['section']);
    if (!isset($by_code[$key]) || $cand['confidence'] > $by_code[$key]['confidence']) {
      $by_code[$key] = $cand;
    }
  }
  $out = array_values($by_code);
  usort($out, '_rovas_ai_sort_section_candidates');
  return array_slice($out, 0, 2);
}

/**
 * usort callback: higher confidence first.
 */
function _rovas_ai_sort_section_candidates($a, $b) {
  if ($a['confidence'] == $b['confidence']) {
    return 0;
  }
  return ($a['confidence'] > $b['confidence']) ? -1 : 1;
}

/**
 * Pass 2: pick leaf code + title from a leaf catalog.
 *
 * @param string $description
 * @param string $lang
 * @param array $leaves
 *   Candidate leaves, possibly merged from more than one NACE section.
 * @param bool $multi_section
 *   TRUE when $leaves spans two candidate sections (boundary case), so the
 *   prompt asks for a genuine side-by-side comparison rather than assuming
 *   all candidates are already in the "right" industry.
 *
 * @return array
 */
function rovas_ai_classify_pass_leaf($description, $lang, array $leaves, $multi_section = FALSE) {
  $catalog = rovas_ai_nace_prompt_lines($leaves);
  $system = 'You pick the single best-fitting NACE leaf code for the work and suggest a '
    . 'short work-report title. Reply with JSON only. Either '
    . '{"code":"43.21","title":"...","confidence":0.0} '
    . 'or {"need_more_info":true,"question":"...","confidence":0.0}. '
    . 'code must be from the list. confidence is 0-1 and must reflect true fit, not just '
    . 'the best of a narrow list — if none of the codes describe the actual activity well, '
    . 'use need_more_info instead of forcing a poor match. '
    . 'title max 80 chars in language ' . $lang . '. '
    . 'question one short clarifying question in ' . $lang . '.';
  if ($multi_section) {
    $system .= ' These codes come from two different plausible industries for this activity — '
      . 'compare them directly and choose the one that best matches what the person actually did, '
      . 'not just the setting it happened in.';
  }

  $user = "Work description:\n" . $description . "\n\nNACE leaf codes (code | label):\n" . $catalog;

  // Larger catalog may need more completion budget for JSON only.
  $max_tokens = 300;
  if (drupal_strlen($catalog) > 8000) {
    $max_tokens = 400;
  }

  $chat = rovas_ai_chat(
    array(
      array('role' => 'system', 'content' => $system),
      array('role' => 'user', 'content' => $user),
    ),
    array(
      'task_id' => 'nace_classify',
      'max_tokens' => $max_tokens,
    )
  );

  if (empty($chat['ok'])) {
    return array(
      'status' => 'unavailable',
      'error' => !empty($chat['error']) ? $chat['error'] : 'llm_error',
      'confidence' => 0,
      'clarifying_question' => '',
    );
  }

  $parsed = rovas_ai_parse_json_object($chat['content']);
  if ($parsed === NULL) {
    return array(
      'status' => 'unavailable',
      'error' => 'bad_json',
      'confidence' => 0,
      'clarifying_question' => '',
    );
  }

  $confidence = isset($parsed['confidence']) ? (float) $parsed['confidence'] : 0.0;
  if (!empty($parsed['need_more_info'])) {
    $q = isset($parsed['question']) ? trim((string) $parsed['question']) : '';
    return array(
      'status' => 'need_more_info',
      'clarifying_question' => $q !== '' ? $q : rovas_ai_clarify_default($lang, 'leaf'),
      'confidence' => $confidence,
      'error' => '',
    );
  }

  $code = isset($parsed['code']) ? trim((string) $parsed['code']) : '';
  $title = isset($parsed['title']) ? trim((string) $parsed['title']) : '';
  if ($code === '') {
    return array(
      'status' => 'need_more_info',
      'clarifying_question' => rovas_ai_clarify_default($lang, 'leaf'),
      'confidence' => $confidence,
      'error' => '',
    );
  }

  return array(
    'status' => 'ok',
    'code' => $code,
    'title' => $title,
    'confidence' => $confidence,
    'clarifying_question' => '',
    'error' => '',
  );
}

/**
 * Decode a JSON object from model content (strip fences if present).
 *
 * @param string $content
 *
 * @return array|null
 */
function rovas_ai_parse_json_object($content) {
  $content = trim((string) $content);
  if ($content === '') {
    return NULL;
  }
  if (preg_match('/^```(?:json)?\s*(.*?)\s*```$/s', $content, $m)) {
    $content = trim($m[1]);
  }
  $decoded = json_decode($content, TRUE);
  if (is_array($decoded)) {
    return $decoded;
  }
  // Last resort: first {...} blob.
  if (preg_match('/\{.*\}/s', $content, $m)) {
    $decoded = json_decode($m[0], TRUE);
    if (is_array($decoded)) {
      return $decoded;
    }
  }
  return NULL;
}

/**
 * Default clarifying question when the model did not supply one.
 *
 * @param string $lang
 * @param string $kind
 *   empty|section|leaf
 *
 * @return string
 */
function rovas_ai_clarify_default($lang, $kind) {
  $sk = ($lang === 'sk');
  switch ($kind) {
    case 'empty':
      return $sk
        ? 'Popíšte prosím, akú prácu ste vykonali.'
        : 'Please describe the work you performed.';

    case 'section':
      return $sk
        ? 'Môžete upresniť odvetvie alebo typ činnosti (napr. stavebníctvo, IT, vzdelávanie)?'
        : 'Can you clarify the industry or type of activity (e.g. construction, IT, education)?';

    case 'leaf':
    default:
      return $sk
        ? 'Môžete podrobnejšie popísať konkrétnu činnosť, ktorú ste vykonali?'
        : 'Can you describe the specific activity you performed in more detail?';
  }
}

/**
 * Services callback: POST /rovas/civic_ai_nace
 *
 * Always HTTP 200 with a status field so Civic can fall back without treating
 * unavailable as a hard transport failure.
 *
 * @param array $data
 *
 * @return array
 */
function rovas_ai_services_classify_nace($data) {
  $uid = rovas_ai_request_uid();
  if ($uid < 1) {
    return services_error('Authentication required', 401, array(
      'error' => 'unauthorized',
      'message' => 'Authentication required',
    ));
  }

  if (!is_array($data)) {
    $data = array();
  }
  $description = isset($data['description']) ? trim((string) $data['description']) : '';
  $lang = isset($data['lang']) ? $data['lang'] : '';

  $result = rovas_ai_classify_work($description, $lang, array('uid' => $uid));

  // Stable response shape for Civic.
  return array(
    'status' => isset($result['status']) ? $result['status'] : 'error',
    'nace_tid' => isset($result['nace_tid']) ? (string) $result['nace_tid'] : '',
    'nace_code' => isset($result['nace_code']) ? (string) $result['nace_code'] : '',
    'nace_label' => isset($result['nace_label']) ? (string) $result['nace_label'] : '',
    'title' => isset($result['title']) ? (string) $result['title'] : '',
    'clarifying_question' => isset($result['clarifying_question']) ? (string) $result['clarifying_question'] : '',
    'confidence' => isset($result['confidence']) ? round((float) $result['confidence'], 4) : 0,
    'error' => isset($result['error']) ? (string) $result['error'] : '',
  );
}
