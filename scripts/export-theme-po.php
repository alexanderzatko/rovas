<?php

/**
 * @file
 * Export translations for strings originating in theme files.
 *
 * Usage:
 *   drush74 --uri=dev.rovas.app scr scripts/export-theme-po.php sk translations/po/sk.themes.po
 *   drush74 --uri=dev.rovas.app scr scripts/export-theme-po.php sk translations/po/sk.themes.po sites/all/themes/
 */

if (php_sapi_name() !== 'cli') {
  fwrite(STDERR, "This script must be run from CLI.\n");
  exit(1);
}

function _rovas_theme_po_escape($value) {
  $value = str_replace("\\", "\\\\", $value);
  $value = str_replace('"', '\\"', $value);
  $value = str_replace("\r\n", "\n", $value);
  $value = str_replace("\r", "\n", $value);
  $parts = explode("\n", $value);
  if (count($parts) === 1) {
    return '"' . $parts[0] . '"';
  }
  $out = "\"\"\n";
  foreach ($parts as $part) {
    $out .= '"' . $part . "\\n\"\n";
  }
  return rtrim($out, "\n");
}

$effective_args = $args;
if (!empty($effective_args[0]) && is_string($effective_args[0])) {
  if (preg_match('/export-theme-po\.php$/', $effective_args[0])) {
    array_shift($effective_args);
  }
}

if (empty($effective_args[0])) {
  fwrite(STDERR, "Usage: drush scr scripts/export-theme-po.php <langcode> [output-path] [location-prefix]\n");
  exit(1);
}

$langcode = $effective_args[0];
$output_path = !empty($effective_args[1]) ? $effective_args[1] : ('translations/po/' . $langcode . '.themes.po');
$location_prefix = !empty($effective_args[2]) ? $effective_args[2] : 'sites/all/themes/';
$location_like = $location_prefix . '%';

$languages = language_list();
if (empty($languages[$langcode])) {
  fwrite(STDERR, "Unknown language code: {$langcode}\n");
  exit(1);
}

$rows = db_query(
  "SELECT s.lid, s.source, s.context, s.location, t.translation, t.plural
   FROM {locales_source} s
   LEFT JOIN {locales_target} t ON t.lid = s.lid AND t.language = :language
   WHERE s.location LIKE :location_like
   ORDER BY s.lid ASC, t.plural ASC",
  array(
    ':language' => $langcode,
    ':location_like' => $location_like,
  )
)->fetchAll();

$entries = array();
foreach ($rows as $row) {
  if (!isset($entries[$row->lid])) {
    $entries[$row->lid] = array(
      'source' => (string) $row->source,
      'context' => (string) $row->context,
      'location' => (string) $row->location,
      'translations' => array(),
    );
  }
  if ($row->translation !== NULL) {
    $plural_index = is_numeric($row->plural) ? (int) $row->plural : 0;
    $entries[$row->lid]['translations'][$plural_index] = (string) $row->translation;
  }
}

$language_name = isset($languages[$langcode]->name) ? $languages[$langcode]->name : $langcode;
$po = '';
$po .= "msgid \"\"\n";
$po .= "msgstr \"\"\n";
$po .= "\"Project-Id-Version: rovas themes\\n\"\n";
$po .= "\"POT-Creation-Date: " . gmdate('Y-m-d H:iO') . "\\n\"\n";
$po .= "\"PO-Revision-Date: " . gmdate('Y-m-d H:iO') . "\\n\"\n";
$po .= "\"Language: " . $langcode . "\\n\"\n";
$po .= "\"X-Generator: export-theme-po.php\\n\"\n";
$po .= "\"X-Drupal-Language: " . str_replace('"', '', $language_name) . "\\n\"\n";
$po .= "\n";

foreach ($entries as $entry) {
  // Keep #: locations query-free (avoid leaking tokens/emails from request paths).
  $location = preg_replace('/\?.*$/', '', str_replace("\n", " ", $entry['location']));
  $po .= "#: " . $location . "\n";
  if ($entry['context'] !== '') {
    $po .= "msgctxt " . _rovas_theme_po_escape($entry['context']) . "\n";
  }
  $po .= "msgid " . _rovas_theme_po_escape($entry['source']) . "\n";
  if (count($entry['translations']) <= 1) {
    $value = isset($entry['translations'][0]) ? $entry['translations'][0] : '';
    $po .= "msgstr " . _rovas_theme_po_escape($value) . "\n";
  }
  else {
    ksort($entry['translations']);
    foreach ($entry['translations'] as $index => $value) {
      $po .= "msgstr[" . $index . "] " . _rovas_theme_po_escape($value) . "\n";
    }
  }
  $po .= "\n";
}

$target = $output_path;
if ($target[0] !== '/') {
  $target = DRUPAL_ROOT . '/' . $target;
}

$target_dir = dirname($target);
if (!is_dir($target_dir) && !mkdir($target_dir, 0775, TRUE)) {
  fwrite(STDERR, "Cannot create output directory: {$target_dir}\n");
  exit(1);
}

if (file_put_contents($target, $po) === FALSE) {
  fwrite(STDERR, "Failed writing file: {$target}\n");
  exit(1);
}

fwrite(STDOUT, "Exported {$langcode} theme strings to {$target}\n");
