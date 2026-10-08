<?php

/**
 * @file
 * Import content/pages/<alias>/<lang>.md into Basic page ET fields.
 *
 * Usage:
 *   drush74 --uri=dev.rovas.app scr scripts/import-static-pages.php [pages-dir] [replace|keep]
 */

if (php_sapi_name() !== 'cli') {
  fwrite(STDERR, "This script must be run from CLI.\n");
  exit(1);
}

require_once dirname(__FILE__) . '/static-pages.inc.php';

$effective_args = static_pages_effective_args(isset($args) ? $args : array(), 'import-static-pages.php');
$pages_dir = static_pages_resolve_dir(!empty($effective_args[0]) ? $effective_args[0] : 'content/pages');
$mode = !empty($effective_args[1]) ? $effective_args[1] : 'replace';
if (!in_array($mode, array('replace', 'keep'), TRUE)) {
  fwrite(STDERR, "Usage: drush scr scripts/import-static-pages.php [pages-dir] [replace|keep]\n");
  exit(1);
}

if (!is_dir($pages_dir)) {
  fwrite(STDERR, "Pages directory not found: {$pages_dir}\n");
  exit(1);
}

$alias_to_nid = static_pages_load_manifest($pages_dir);
if (empty($alias_to_nid)) {
  fwrite(STDERR, "manifest.yml has no aliases. Nothing to import.\n");
  exit(1);
}

$imported = 0;
$skipped = 0;
$errors = 0;

foreach ($alias_to_nid as $alias => $pinned_nid) {
  $err = NULL;
  $node = static_pages_load_page($alias, $pinned_nid, $err);
  if (!$node) {
    fwrite(STDERR, "ERROR: {$alias}: " . ($err ? $err : 'no page node') . "\n");
    $errors++;
    continue;
  }

  $alias_dir = $pages_dir . '/' . $alias;
  if (!is_dir($alias_dir)) {
    fwrite(STDERR, "WARNING: missing directory {$alias_dir}; skipping alias.\n");
    $skipped++;
    continue;
  }

  $files = glob($alias_dir . '/*.md');
  if (empty($files)) {
    fwrite(STDERR, "WARNING: no .md files in {$alias_dir}; skipping.\n");
    $skipped++;
    continue;
  }

  sort($files);
  foreach ($files as $filepath) {
    $basename = basename($filepath, '.md');
    if (!preg_match('/^[a-zA-Z0-9_-]+$/', $basename)) {
      fwrite(STDERR, "WARNING: skip unexpected file {$filepath}\n");
      continue;
    }
    $lang = ($basename === 'und') ? LANGUAGE_NONE : drupal_strtolower($basename);

    $parsed = static_pages_parse_markdown_file($filepath);
    if ($parsed === FALSE) {
      fwrite(STDERR, "ERROR: cannot read {$filepath}\n");
      $errors++;
      continue;
    }

    try {
      $html = static_pages_markdown_to_html($parsed['body']);
    }
    catch (Exception $e) {
      fwrite(STDERR, "ERROR: Markdown convert failed for {$filepath}: " . $e->getMessage() . "\n");
      $errors++;
      continue;
    }

    // Prefer frontmatter format; else keep existing lang format; else filtered_html.
    $format = $parsed['format'];
    if ($format === '') {
      if (!empty($node->body[$lang][0]['format'])) {
        $format = $node->body[$lang][0]['format'];
      }
      else {
        $format = 'filtered_html';
      }
    }

    // Reload node each write so successive langs see prior saves.
    $node = node_load($node->nid, NULL, TRUE);
    if (!$node) {
      fwrite(STDERR, "ERROR: node vanished for alias '{$alias}'\n");
      $errors++;
      break;
    }

    $wrote = static_pages_write_node_lang($node, $lang, $parsed['title'], $html, $format, $mode);
    if ($wrote) {
      fwrite(STDOUT, "Imported {$alias}/{$basename}.md -> nid {$node->nid} ({$lang})\n");
      $imported++;
    }
    else {
      fwrite(STDOUT, "Kept existing {$alias}/{$basename}.md (nid {$node->nid}, {$lang})\n");
      $skipped++;
    }
  }
}

fwrite(STDOUT, "Done. Imported {$imported}, skipped {$skipped}");
if ($errors) {
  fwrite(STDOUT, ", {$errors} error(s)");
}
fwrite(STDOUT, ".\n");
// Do not call exit() — Drush scr treats it as an unrecoverable error.
if ($errors && function_exists('drush_set_error')) {
  return drush_set_error('STATIC_PAGES_IMPORT', "Import finished with {$errors} error(s).");
}
