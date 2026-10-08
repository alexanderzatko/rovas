<?php

/**
 * @file
 * Shared helpers for static page export/import Drush scripts.
 */

/**
 * Normalize a pages directory to an absolute path under DRUPAL_ROOT when relative.
 *
 * @param string $pages_dir
 *   Relative or absolute path.
 *
 * @return string
 *   Absolute path without trailing slash.
 */
function static_pages_resolve_dir($pages_dir) {
  $pages_dir = rtrim((string) $pages_dir, '/');
  if ($pages_dir === '') {
    $pages_dir = 'content/pages';
  }
  if ($pages_dir[0] !== '/') {
    $pages_dir = DRUPAL_ROOT . '/' . $pages_dir;
  }
  return $pages_dir;
}

/**
 * Shift the script filename off Drush scr $args when present.
 *
 * @param array $args
 *   Drush script args.
 * @param string $script_basename
 *   Expected script basename, e.g. export-static-pages.php.
 *
 * @return array
 *   Remaining args.
 */
function static_pages_effective_args(array $args, $script_basename) {
  if (!empty($args[0]) && is_string($args[0]) && preg_match('/' . preg_quote($script_basename, '/') . '$/', $args[0])) {
    array_shift($args);
  }
  return array_values($args);
}

/**
 * Load aliases from manifest.yml (simple "aliases:" list of "- item" lines).
 *
 * @param string $pages_dir
 *   Absolute pages directory.
 *
 * @return string[]
 *   Alias strings (may be empty).
 */
function static_pages_load_manifest_aliases($pages_dir) {
  $path = $pages_dir . '/manifest.yml';
  if (!is_file($path)) {
    return array();
  }
  $lines = file($path, FILE_IGNORE_NEW_LINES);
  if ($lines === FALSE) {
    return array();
  }
  $aliases = array();
  $in_list = FALSE;
  foreach ($lines as $line) {
    $trim = trim($line);
    if ($trim === '' || $trim[0] === '#') {
      continue;
    }
    if (preg_match('/^aliases\s*:\s*(.*)$/', $trim, $m)) {
      $in_list = TRUE;
      $inline = trim($m[1]);
      if ($inline === '[]' || $inline === '') {
        continue;
      }
      // Rare inline form: aliases: [a, b]
      if ($inline[0] === '[' && substr($inline, -1) === ']') {
        $inner = trim($inline, '[]');
        if ($inner !== '') {
          foreach (explode(',', $inner) as $part) {
            $alias = trim($part, " \t\"'");
            if ($alias !== '') {
              $aliases[] = $alias;
            }
          }
        }
      }
      continue;
    }
    if ($in_list && preg_match('/^-\s+(.+)$/', $trim, $m)) {
      $alias = trim($m[1], " \t\"'");
      if ($alias !== '') {
        $aliases[] = $alias;
      }
      continue;
    }
    if ($in_list && !preg_match('/^\s/', $line) && strpos($trim, ':') !== FALSE) {
      $in_list = FALSE;
    }
  }
  return array_values(array_unique($aliases));
}

/**
 * Write manifest.yml with the given aliases.
 *
 * @param string $pages_dir
 *   Absolute pages directory.
 * @param string[] $aliases
 *   Path aliases.
 *
 * @return bool
 *   TRUE on success.
 */
function static_pages_write_manifest($pages_dir, array $aliases) {
  if (!is_dir($pages_dir) && !mkdir($pages_dir, 0775, TRUE)) {
    return FALSE;
  }
  $out = "# Path aliases of Basic page (page) nodes managed in Git.\n";
  $out .= "# Populate via: scripts/export-static-pages-all.sh [uri] content/pages discover\n";
  $out .= "# Import/export only touch aliases listed here.\n";
  if (empty($aliases)) {
    $out .= "aliases: []\n";
  }
  else {
    $out .= "aliases:\n";
    foreach ($aliases as $alias) {
      $out .= '  - ' . static_pages_yaml_scalar($alias) . "\n";
    }
  }
  return file_put_contents($pages_dir . '/manifest.yml', $out) !== FALSE;
}

/**
 * Quote a YAML scalar when needed.
 *
 * @param string $value
 *   Raw value.
 *
 * @return string
 *   YAML scalar.
 */
function static_pages_yaml_scalar($value) {
  $value = (string) $value;
  if ($value === '' || preg_match('/[:#\[\]{}\n\"\'\\\\]|^\s|\s$/u', $value)) {
    return '"' . str_replace(array('\\', '"'), array('\\\\', '\\"'), $value) . '"';
  }
  return $value;
}

/**
 * Parse Markdown file with optional YAML frontmatter.
 *
 * @param string $filepath
 *   Absolute path to .md file.
 *
 * @return array|false
 *   Array with keys title, format, body; FALSE on failure.
 */
function static_pages_parse_markdown_file($filepath) {
  $raw = file_get_contents($filepath);
  if ($raw === FALSE) {
    return FALSE;
  }
  $title = '';
  $format = '';
  $body = $raw;
  if (preg_match('/\A---\r?\n(.*?)\r?\n---\r?\n(.*)\z/s', $raw, $m)) {
    $meta = $m[1];
    $body = $m[2];
    foreach (preg_split('/\r?\n/', $meta) as $line) {
      if (!preg_match('/^([A-Za-z0-9_]+)\s*:\s*(.*)$/', trim($line), $km)) {
        continue;
      }
      $key = strtolower($km[1]);
      $val = trim($km[2]);
      if (strlen($val) >= 2 && (($val[0] === '"' && substr($val, -1) === '"') || ($val[0] === "'" && substr($val, -1) === "'"))) {
        $val = stripcslashes(substr($val, 1, -1));
      }
      if ($key === 'title') {
        $title = $val;
      }
      elseif ($key === 'format') {
        $format = $val;
      }
    }
  }
  return array(
    'title' => $title,
    'format' => $format,
    'body' => $body,
  );
}

/**
 * Build Markdown file contents with frontmatter.
 *
 * @param string $title
 *   Page title.
 * @param string $body
 *   Body (HTML or Markdown).
 * @param string $format
 *   Drupal text format machine name.
 *
 * @return string
 *   File contents.
 */
function static_pages_build_markdown_file($title, $body, $format = 'filtered_html') {
  $body = (string) $body;
  $body = str_replace(array("\r\n", "\r"), "\n", $body);
  $body = trim($body, "\n");
  $out = "---\n";
  $out .= 'title: ' . static_pages_yaml_scalar($title) . "\n";
  if ($format !== '') {
    $out .= 'format: ' . static_pages_yaml_scalar($format) . "\n";
  }
  $out .= "---\n\n";
  if ($body !== '') {
    $out .= $body . "\n";
  }
  return $out;
}

/**
 * Convert Markdown (or HTML-in-MD) to HTML via vendored Parsedown.
 *
 * @param string $markdown
 *   Markdown body.
 *
 * @return string
 *   HTML.
 */
function static_pages_markdown_to_html($markdown) {
  static $parsedown = NULL;
  if ($parsedown === NULL) {
    $lib = dirname(__FILE__) . '/lib/Parsedown.php';
    if (!is_file($lib)) {
      throw new Exception("Parsedown not found at {$lib}");
    }
    require_once $lib;
    $parsedown = new Parsedown();
    $parsedown->setSafeMode(FALSE);
    $parsedown->setMarkupEscaped(FALSE);
  }
  return $parsedown->text($markdown);
}

/**
 * Resolve a path alias to a published Basic page node.
 *
 * @param string $alias
 *   Path alias without leading slash.
 *
 * @return object|null
 *   Node object or NULL.
 */
function static_pages_load_page_by_alias($alias) {
  $alias = trim($alias, '/');
  if ($alias === '') {
    return NULL;
  }
  $source = drupal_lookup_path('source', $alias);
  if (!$source) {
    // Try language-neutral lookup via url_alias table.
    $source = db_query('SELECT source FROM {url_alias} WHERE alias = :alias ORDER BY language ASC', array(
      ':alias' => $alias,
    ))->fetchField();
  }
  if (!$source || !preg_match('~^node/(\d+)$~', $source, $m)) {
    return NULL;
  }
  $node = node_load((int) $m[1]);
  if (!$node || $node->type !== 'page') {
    return NULL;
  }
  return $node;
}

/**
 * Discover published page nodes that have a path alias.
 *
 * Prefers LANGUAGE_NONE alias, else any language; skips nodes without alias.
 *
 * @return array
 *   Map of alias => nid.
 */
function static_pages_discover_page_aliases() {
  $nids = db_query("SELECT nid FROM {node} WHERE type = 'page' AND status = 1 ORDER BY nid")->fetchCol();
  $map = array();
  foreach ($nids as $nid) {
    $nid = (int) $nid;
    $alias = db_query("SELECT alias FROM {url_alias} WHERE source = :src AND language = :und ORDER BY pid DESC", array(
      ':src' => 'node/' . $nid,
      ':und' => LANGUAGE_NONE,
    ))->fetchField();
    if (!$alias) {
      $alias = db_query("SELECT alias FROM {url_alias} WHERE source = :src ORDER BY language ASC, pid DESC", array(
        ':src' => 'node/' . $nid,
      ))->fetchField();
    }
    if (!$alias) {
      continue;
    }
    $alias = trim($alias, '/');
    // Prefer first nid if duplicate aliases (should not happen).
    if (!isset($map[$alias])) {
      $map[$alias] = $nid;
    }
  }
  ksort($map);
  return $map;
}

/**
 * Collect language codes that have title_field and/or body for a node.
 *
 * @param object $node
 *   Node object.
 *
 * @return string[]
 *   Langcodes (excludes LANGUAGE_NONE when real langs exist).
 */
function static_pages_node_content_languages($node) {
  $langs = array();
  foreach (array('title_field', 'body') as $field) {
    if (empty($node->{$field}) || !is_array($node->{$field})) {
      continue;
    }
    foreach (array_keys($node->{$field}) as $lang) {
      if ($lang === LANGUAGE_NONE) {
        continue;
      }
      $langs[$lang] = $lang;
    }
  }
  if (empty($langs) && !empty($node->language) && $node->language !== LANGUAGE_NONE) {
    $langs[$node->language] = $node->language;
  }
  // Fall back to und if that is all we have.
  if (empty($langs)) {
    foreach (array('title_field', 'body') as $field) {
      if (!empty($node->{$field}[LANGUAGE_NONE])) {
        $langs[LANGUAGE_NONE] = LANGUAGE_NONE;
        break;
      }
    }
  }
  // Prefer ET translation list order when available.
  if (module_exists('entity_translation') && function_exists('entity_translation_get_handler')) {
    $handler = entity_translation_get_handler('node', $node);
    if ($handler && method_exists($handler, 'getTranslations')) {
      $translations = $handler->getTranslations();
      if (!empty($translations->data) && is_array($translations->data)) {
        $ordered = array();
        foreach (array_keys($translations->data) as $lang) {
          if (isset($langs[$lang])) {
            $ordered[$lang] = $lang;
          }
        }
        foreach ($langs as $lang) {
          $ordered[$lang] = $lang;
        }
        $langs = $ordered;
      }
    }
  }
  return array_values($langs);
}

/**
 * Read title/body/format for one language from a node.
 *
 * @param object $node
 *   Node.
 * @param string $lang
 *   Langcode.
 *
 * @return array
 *   Keys: title, body, format.
 */
function static_pages_get_node_lang_values($node, $lang) {
  $title = '';
  if (!empty($node->title_field[$lang][0]['value'])) {
    $title = $node->title_field[$lang][0]['value'];
  }
  elseif ($lang === $node->language && !empty($node->title)) {
    $title = $node->title;
  }
  elseif (!empty($node->title_field[LANGUAGE_NONE][0]['value']) && $lang === LANGUAGE_NONE) {
    $title = $node->title_field[LANGUAGE_NONE][0]['value'];
  }

  $body = '';
  $format = 'filtered_html';
  if (!empty($node->body[$lang][0]['value'])) {
    $body = $node->body[$lang][0]['value'];
    if (!empty($node->body[$lang][0]['format'])) {
      $format = $node->body[$lang][0]['format'];
    }
  }
  elseif (!empty($node->body[LANGUAGE_NONE][0]['value']) && $lang === LANGUAGE_NONE) {
    $body = $node->body[LANGUAGE_NONE][0]['value'];
    if (!empty($node->body[LANGUAGE_NONE][0]['format'])) {
      $format = $node->body[LANGUAGE_NONE][0]['format'];
    }
  }

  return array(
    'title' => $title,
    'body' => $body,
    'format' => $format,
  );
}

/**
 * Write title_field + body for one language and ensure ET translation row.
 *
 * @param object $node
 *   Node (modified in place; caller should node_load fresh if needed).
 * @param string $lang
 *   Target langcode.
 * @param string $title
 *   Title text.
 * @param string $html_body
 *   HTML body.
 * @param string $format
 *   Text format.
 * @param string $mode
 *   replace|keep.
 *
 * @return bool
 *   TRUE if something was written.
 */
function static_pages_write_node_lang($node, $lang, $title, $html_body, $format, $mode = 'replace') {
  $lang = drupal_strtolower(trim($lang));
  if ($lang === '') {
    return FALSE;
  }
  $mode = ($mode === 'keep') ? 'keep' : 'replace';
  $format = $format !== '' ? $format : 'filtered_html';

  $existing_title = isset($node->title_field[$lang][0]['value']) ? trim($node->title_field[$lang][0]['value']) : '';
  $existing_body = isset($node->body[$lang][0]['value']) ? trim($node->body[$lang][0]['value']) : '';

  $write_title = ($mode === 'replace') || ($existing_title === '');
  $write_body = ($mode === 'replace') || ($existing_body === '');
  if (!$write_title && !$write_body) {
    return FALSE;
  }

  if ($write_title && $title !== '') {
    $node->title_field[$lang][0] = array(
      'value' => $title,
      'format' => '',
      'safe_value' => check_plain($title),
    );
    if (!empty($node->language) && $lang === $node->language) {
      $node->title = $title;
    }
  }
  if ($write_body) {
    $summary = isset($node->body[$lang][0]['summary']) ? $node->body[$lang][0]['summary'] : '';
    $node->body[$lang][0] = array(
      'value' => $html_body,
      'summary' => $summary,
      'format' => $format,
    );
  }

  $source = !empty($node->language) && $node->language !== LANGUAGE_NONE
    ? $node->language
    : (function_exists('rovas_base_translation_node_source_language')
      ? rovas_base_translation_node_source_language($node)
      : 'en');

  $handler = NULL;
  if (module_exists('entity_translation') && function_exists('entity_translation_get_handler')) {
    $handler = entity_translation_get_handler('node', $node, TRUE);
    if ($handler && $lang !== $source && method_exists($handler, 'setTranslation')) {
      $handler->setTranslation(array(
        'translate' => 0,
        'status' => 1,
        'language' => $lang,
        'source' => $source,
        'uid' => !empty($node->uid) ? (int) $node->uid : 1,
        'created' => !empty($node->created) ? (int) $node->created : REQUEST_TIME,
        'changed' => REQUEST_TIME,
      ));
    }
  }

  $prev_title = NULL;
  if (function_exists('rovas_base_translation_begin_presave_title_context')) {
    $prev_title = rovas_base_translation_begin_presave_title_context($source);
  }
  node_save($node);
  if ($handler && method_exists($handler, 'saveTranslations')) {
    $handler->saveTranslations();
  }
  if (function_exists('rovas_base_translation_end_presave_title_context')) {
    rovas_base_translation_end_presave_title_context($prev_title);
  }
  if (function_exists('rovas_base_translation_record_hashes')) {
    // Reload so hashes see saved field values.
    $fresh = node_load($node->nid, NULL, TRUE);
    if ($fresh) {
      rovas_base_translation_record_hashes($fresh, $lang, array('title_field', 'body'));
    }
  }

  return TRUE;
}
