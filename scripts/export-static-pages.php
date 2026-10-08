<?php

/**
 * @file
 * Export Basic page ET title/body into content/pages/<alias>/<lang>.md.
 *
 * Usage:
 *   drush74 --uri=dev.rovas.app scr scripts/export-static-pages.php [pages-dir] [manifest|discover]
 */

if (php_sapi_name() !== 'cli') {
  fwrite(STDERR, "This script must be run from CLI.\n");
  exit(1);
}

require_once dirname(__FILE__) . '/static-pages.inc.php';

$effective_args = static_pages_effective_args(isset($args) ? $args : array(), 'export-static-pages.php');
$pages_dir = static_pages_resolve_dir(!empty($effective_args[0]) ? $effective_args[0] : 'content/pages');
$mode = !empty($effective_args[1]) ? $effective_args[1] : 'manifest';
if (!in_array($mode, array('manifest', 'discover'), TRUE)) {
  fwrite(STDERR, "Usage: drush scr scripts/export-static-pages.php [pages-dir] [manifest|discover]\n");
  exit(1);
}

if (!is_dir($pages_dir) && !mkdir($pages_dir, 0775, TRUE)) {
  fwrite(STDERR, "Cannot create pages directory: {$pages_dir}\n");
  exit(1);
}

$alias_to_nid = array();
if ($mode === 'discover') {
  $alias_to_nid = static_pages_discover_page_aliases();
  if (empty($alias_to_nid)) {
    fwrite(STDERR, "No published page nodes with path aliases found.\n");
    exit(1);
  }
  $aliases = array_keys($alias_to_nid);
  if (!static_pages_write_manifest($pages_dir, $aliases)) {
    fwrite(STDERR, "Failed writing manifest.yml\n");
    exit(1);
  }
  fwrite(STDOUT, "Wrote manifest.yml with " . count($aliases) . " alias(es).\n");
}
else {
  $aliases = static_pages_load_manifest_aliases($pages_dir);
  if (empty($aliases)) {
    fwrite(STDERR, "manifest.yml has no aliases. Run with 'discover' to populate, e.g.:\n");
    fwrite(STDERR, "  scripts/export-static-pages-all.sh dev.rovas.app content/pages discover\n");
    exit(1);
  }
}

$exported = 0;
$errors = 0;

foreach ($aliases as $alias) {
  if (isset($alias_to_nid[$alias])) {
    $node = node_load($alias_to_nid[$alias]);
  }
  else {
    $node = static_pages_load_page_by_alias($alias);
  }
  if (!$node) {
    fwrite(STDERR, "ERROR: no page node for alias '{$alias}'\n");
    $errors++;
    continue;
  }

  $langs = static_pages_node_content_languages($node);
  if (empty($langs)) {
    fwrite(STDERR, "WARNING: node {$node->nid} ({$alias}) has no language content; skipping.\n");
    continue;
  }

  $alias_dir = $pages_dir . '/' . $alias;
  if (!is_dir($alias_dir) && !mkdir($alias_dir, 0775, TRUE)) {
    fwrite(STDERR, "ERROR: cannot create {$alias_dir}\n");
    $errors++;
    continue;
  }

  foreach ($langs as $lang) {
    $values = static_pages_get_node_lang_values($node, $lang);
    // Skip empty language slots.
    if (trim($values['title']) === '' && trim($values['body']) === '') {
      continue;
    }
    $file_lang = ($lang === LANGUAGE_NONE) ? 'und' : $lang;
    $path = $alias_dir . '/' . $file_lang . '.md';
    $contents = static_pages_build_markdown_file($values['title'], $values['body'], $values['format']);
    if (file_put_contents($path, $contents) === FALSE) {
      fwrite(STDERR, "ERROR: failed writing {$path}\n");
      $errors++;
      continue;
    }
    fwrite(STDOUT, "Exported {$alias}/{$file_lang}.md (nid {$node->nid})\n");
    $exported++;
  }
}

fwrite(STDOUT, "Done. Exported {$exported} file(s)");
if ($errors) {
  fwrite(STDOUT, ", {$errors} error(s)");
}
fwrite(STDOUT, ".\n");
// Do not call exit() — Drush scr treats it as an unrecoverable error.
if ($errors && function_exists('drush_set_error')) {
  return drush_set_error('STATIC_PAGES_EXPORT', "Export finished with {$errors} error(s).");
}
