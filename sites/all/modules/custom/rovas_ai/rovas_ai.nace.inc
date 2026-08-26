<?php

/**
 * @file
 * NACE taxonomy helpers for AI classification (vocabulary tags).
 */

/**
 * Split a NACE term name into code + label.
 *
 * Handles both dash-separated names ("F – Construction", "41.10 – …") and
 * space-separated names used on this site ("41.10 Development of …").
 *
 * @param string $name
 *
 * @return array
 *   array(code, label).
 */
function rovas_ai_nace_code_label($name) {
  $name = trim((string) $name);
  // "F – Construction" / "41.10 – Development..." (en-dash, hyphen, or colon).
  if (preg_match('/^([A-Z0-9][A-Z0-9.]{0,7})\s*[–\-:]+\s*(.+)$/u', $name, $m)) {
    return array($m[1], $m[2]);
  }
  // "F Construction" / "41 Construction…" / "41.10 Development…" / "43.21 …"
  if (preg_match('/^([A-Z]|[0-9]{1,2}(?:\.[0-9]{1,2}){0,2})\s+(.+)$/u', $name, $m)) {
    return array($m[1], $m[2]);
  }
  return array('', $name);
}

/**
 * Localized term display name.
 *
 * @param object $term
 * @param string $lang
 *
 * @return string
 */
function rovas_ai_nace_term_name($term, $lang) {
  $name = $term->name;
  if (function_exists('_rb_translate_term')) {
    $translated = _rb_translate_term($term->tid, $lang);
    if ($translated) {
      $name = $translated;
    }
  }
  return $name;
}

/**
 * Flat map of all NACE terms: tid => {tid, code, label, parent, depth}.
 *
 * @param string $lang
 *
 * @return array
 */
function rovas_ai_nace_flat($lang = 'en') {
  // v2: space-separated codes ("41.10 Label") parse correctly.
  $cid = 'rovas_ai_nace_flat:v2:' . $lang;
  $cache = cache_get($cid);
  if ($cache && is_array($cache->data)) {
    return $cache->data;
  }

  $vocab = taxonomy_vocabulary_machine_name_load('tags');
  if (!$vocab) {
    return array();
  }
  $terms = taxonomy_get_tree($vocab->vid);
  if (empty($terms)) {
    return array();
  }

  $items = array();
  foreach ($terms as $term) {
    $name = rovas_ai_nace_term_name($term, $lang);
    $parsed = rovas_ai_nace_code_label($name);
    $parent = 0;
    if (!empty($term->parents[0])) {
      $parent = (int) $term->parents[0];
    }
    $items[(int) $term->tid] = array(
      'tid' => (int) $term->tid,
      'code' => $parsed[0],
      'label' => $parsed[1],
      'parent' => $parent,
      'depth' => isset($term->depth) ? (int) $term->depth : 0,
    );
  }

  cache_set($cid, $items, 'cache', REQUEST_TIME + 3600);
  return $items;
}

/**
 * Top-level NACE sections with a non-empty code.
 *
 * Prefers depth 0 (taxonomy roots). If those have no usable codes (single
 * unlabeled container), falls back to depth 1 — matching how Rovas often
 * stores letter sections under one root.
 *
 * @param string $lang
 *
 * @return array
 *   List of {tid, code, label}.
 */
function rovas_ai_nace_sections($lang = 'en') {
  $flat = rovas_ai_nace_flat($lang);
  $by_depth = array(0 => array(), 1 => array());
  foreach ($flat as $item) {
    if ($item['code'] === '') {
      continue;
    }
    $d = $item['depth'];
    if ($d === 0 || $d === 1) {
      $by_depth[$d][] = array(
        'tid' => $item['tid'],
        'code' => $item['code'],
        'label' => $item['label'],
      );
    }
  }
  if (!empty($by_depth[0])) {
    return $by_depth[0];
  }
  return $by_depth[1];
}

/**
 * Leaf NACE terms under a section tid (descendants with no children).
 *
 * @param int $section_tid
 * @param string $lang
 *
 * @return array
 *   List of {tid, code, label}.
 */
function rovas_ai_nace_leaves_under($section_tid, $lang = 'en') {
  $section_tid = (int) $section_tid;
  if ($section_tid < 1) {
    return array();
  }

  $vocab = taxonomy_vocabulary_machine_name_load('tags');
  if (!$vocab) {
    return array();
  }

  // Descendants of the section (exclude the section itself).
  $tree = taxonomy_get_tree($vocab->vid, $section_tid);
  if (empty($tree)) {
    // Section itself may be a leaf.
    $term = taxonomy_term_load($section_tid);
    if (!$term) {
      return array();
    }
    $name = rovas_ai_nace_term_name($term, $lang);
    $parsed = rovas_ai_nace_code_label($name);
    if ($parsed[0] === '') {
      return array();
    }
    return array(
      array(
        'tid' => (int) $term->tid,
        'code' => $parsed[0],
        'label' => $parsed[1],
      ),
    );
  }

  $tids = array();
  foreach ($tree as $term) {
    $tids[] = (int) $term->tid;
  }

  // Terms that appear as a parent of another in this subtree are non-leaves.
  $parents = array();
  foreach ($tree as $term) {
    if (!empty($term->parents[0])) {
      $parents[(int) $term->parents[0]] = TRUE;
    }
  }

  $leaves = array();
  foreach ($tree as $term) {
    $tid = (int) $term->tid;
    if (isset($parents[$tid])) {
      continue;
    }
    $name = rovas_ai_nace_term_name($term, $lang);
    $parsed = rovas_ai_nace_code_label($name);
    if ($parsed[0] === '') {
      continue;
    }
    $leaves[] = array(
      'tid' => $tid,
      'code' => $parsed[0],
      'label' => $parsed[1],
    );
  }

  return $leaves;
}

/**
 * Find section item by section code (case-insensitive).
 *
 * @param string $code
 * @param string $lang
 *
 * @return array|null
 */
function rovas_ai_nace_section_by_code($code, $lang = 'en') {
  $code = drupal_strtoupper(trim((string) $code));
  if ($code === '') {
    return NULL;
  }
  foreach (rovas_ai_nace_sections($lang) as $section) {
    if (drupal_strtoupper($section['code']) === $code) {
      return $section;
    }
  }
  return NULL;
}

/**
 * Resolve a leaf by NACE code under an optional section.
 *
 * @param string $code
 * @param string $lang
 * @param int|null $section_tid
 *   If set, only search leaves under this section.
 *
 * @return array|null
 *   {tid, code, label} or NULL.
 */
function rovas_ai_nace_leaf_by_code($code, $lang = 'en', $section_tid = NULL) {
  $needle = rovas_ai_nace_normalize_code($code);
  if ($needle === '') {
    return NULL;
  }

  if ($section_tid) {
    $candidates = rovas_ai_nace_leaves_under($section_tid, $lang);
  }
  else {
    $candidates = array();
    $flat = rovas_ai_nace_flat($lang);
    $parent_of = array();
    foreach ($flat as $item) {
      if ($item['parent']) {
        $parent_of[$item['parent']] = TRUE;
      }
    }
    foreach ($flat as $item) {
      if (isset($parent_of[$item['tid']])) {
        continue;
      }
      if ($item['code'] === '') {
        continue;
      }
      $candidates[] = $item;
    }
  }

  foreach ($candidates as $item) {
    if (rovas_ai_nace_normalize_code($item['code']) === $needle) {
      return array(
        'tid' => (int) $item['tid'],
        'code' => $item['code'],
        'label' => $item['label'],
      );
    }
  }
  return NULL;
}

/**
 * Normalize a model-supplied NACE code for matching.
 *
 * Accepts variants like "42,11", "NACE 42.11", "42.11 – Foo".
 *
 * @param string $code
 *
 * @return string
 *   Lowercase dotted code, or '' if none found.
 */
function rovas_ai_nace_normalize_code($code) {
  $code = trim((string) $code);
  if ($code === '') {
    return '';
  }
  // Prefer an explicit NACE-like token (letter section or digit[.digit]…).
  if (preg_match('/\b([A-Z])\b/u', $code, $m) && !preg_match('/\d/', $code)) {
    return drupal_strtolower($m[1]);
  }
  if (preg_match('/\b(\d{1,2}(?:[.,]\d{1,2}){0,2})\b/u', $code, $m)) {
    return drupal_strtolower(str_replace(',', '.', $m[1]));
  }
  $code = str_replace(',', '.', $code);
  $code = preg_replace('/\s+/u', '', $code);
  return drupal_strtolower($code);
}

/**
 * Find a leaf by code within an already-fetched list (no taxonomy lookup).
 *
 * Used when leaves from multiple sections have been merged in memory, so
 * resolution must not be scoped to a single section tid.
 *
 * @param array $leaves
 *   List of {tid, code, label}.
 * @param string $code
 *
 * @return array|null
 */
function rovas_ai_nace_find_in_list(array $leaves, $code) {
  $needle = rovas_ai_nace_normalize_code($code);
  if ($needle === '') {
    return NULL;
  }
  foreach ($leaves as $item) {
    if (!isset($item['code'])) {
      continue;
    }
    if (rovas_ai_nace_normalize_code($item['code']) === $needle) {
      return $item;
    }
  }
  return NULL;
}

/**
 * Resolve a model code against the pass-2 leaf list, then the full taxonomy.
 *
 * Pass-1 section filtering is a prompt-size optimization; if the model returns
 * a real leaf outside that catalog, accept it rather than failing as unknown_code.
 *
 * @param array $leaves
 * @param string $code
 * @param string $lang
 *
 * @return array|null
 *   {tid, code, label} or NULL.
 */
function rovas_ai_nace_resolve_model_code(array $leaves, $code, $lang = 'en') {
  $normalized = rovas_ai_nace_normalize_code($code);
  if ($normalized === '') {
    return NULL;
  }

  $leaf = rovas_ai_nace_find_in_list($leaves, $normalized);
  if ($leaf) {
    return $leaf;
  }

  // Valid leaf elsewhere in the tree (wrong section in pass 1, or model memory).
  $leaf = rovas_ai_nace_leaf_by_code($normalized, $lang);
  if ($leaf) {
    watchdog('rovas_ai', 'NACE code @c accepted from full taxonomy (not in pass-2 catalog).', array(
      '@c' => $leaf['code'],
    ), WATCHDOG_NOTICE);
    return $leaf;
  }

  return NULL;
}

/**
 * Merge leaf lists from multiple sections, deduplicated by code.
 *
 * @param array $lists
 *   Array of leaf-lists (each a list of {tid, code, label}).
 *
 * @return array
 */
function rovas_ai_nace_merge_leaf_lists(array $lists) {
  $by_code = array();
  foreach ($lists as $leaves) {
    foreach ($leaves as $item) {
      if (empty($item['code'])) {
        continue;
      }
      $key = drupal_strtolower($item['code']);
      if (!isset($by_code[$key])) {
        $by_code[$key] = $item;
      }
    }
  }
  return array_values($by_code);
}

/**
 * Compact "code | label" lines for prompts.
 *
 * @param array $items
 *   List of {code, label}.
 * @param bool $with_hints
 *   TRUE to append short disambiguation notes for known-confusable leaf
 *   codes (see rovas_ai_nace_disambiguation_hints()). Section-level prompts
 *   (pass 1) should pass FALSE; leaf prompts (pass 2) should pass TRUE.
 *
 * @return string
 */
function rovas_ai_nace_prompt_lines(array $items, $with_hints = FALSE) {
  $hints = $with_hints ? rovas_ai_nace_disambiguation_hints() : array();
  $lines = array();
  foreach ($items as $item) {
    if (empty($item['code'])) {
      continue;
    }
    $label = isset($item['label']) ? $item['label'] : '';
    $line = $item['code'] . ' | ' . $label;
    $key = rovas_ai_nace_normalize_code($item['code']);
    if ($key !== '' && isset($hints[$key])) {
      $line .= ' (' . $hints[$key] . ')';
    }
    $lines[] = $line;
  }
  return implode("\n", $lines);
}

/**
 * Short disambiguation notes for NACE leaf codes the model tends to confuse.
 *
 * Keyed by normalized code (see rovas_ai_nace_normalize_code()). NACE labels
 * alone are often too terse for an LLM to tell adjacent/generic-sounding
 * codes apart (e.g. routine snow/street cleaning vs hazardous-site
 * remediation, or volunteer labor on public property vs public-administration
 * regulatory work). Extend this list as new confusions are observed.
 *
 * @return array
 *   normalized code => short English hint (appended after the label).
 */
function rovas_ai_nace_disambiguation_hints() {
  return array(
    // Sections (pass 1) — steer the section choice itself before leaves
    // under the "wrong" section are filtered out and never reach pass 2.
    'e' => 'water supply, sewerage, waste collection/treatment, and remediation of contaminated sites — NOT routine snow/street/outdoor cleaning (see N)',
    'n' => 'business support services incl. cleaning (buildings, streets, snow/ice removal), landscaping, security, rental, travel agencies',
    'f' => 'construction of buildings, roads, and other civil engineering — building/paving/structural work, not routine cleaning or clearing',
    'o' => 'public administration and government regulation/policy-making — NOT hands-on volunteer labor, even on public/municipal property',
    // Leaves (pass 2).
    '81.29' => 'routine outdoor/street cleaning, snow and ice removal, litter clearing — use this for everyday snow shoveling or debris clearing',
    '39.00' => 'remediation of contaminated soil/water and hazardous waste treatment ONLY — NOT routine snow or street cleaning',
    '84.11' => 'general public administration (government offices) — NOT volunteer labor on public property',
    '84.12' => 'government regulation/oversight of health, education, culture services — NOT hands-on volunteer work, even on municipal property',
    '84.13' => 'government regulation of business and economic policy — NOT hands-on labor',
    '42.11' => 'construction, resurfacing, or major repair of roads — use for building/paving work, not routine snow clearing or pothole patching',
    '43.99' => 'other specialised construction not covered elsewhere (e.g. small structural repairs) — check 81.29/42.11 first for cleaning or road work',
    '81.30' => 'landscape care and maintenance of green spaces (mowing, planting, tree care)',
    '81.22' => 'other building and industrial cleaning (interiors, windows, industrial equipment) — not outdoor snow/street work (see 81.29)',
  );
}
