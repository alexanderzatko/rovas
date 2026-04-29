<?php

/**
 * @file
 * Exports Drupal locale translations from DB into a .po file.
 *
 * Usage:
 *   drush74 --uri=dev.rovas.app scr scripts/export-locale-po.php fr default translations/po/fr.default.po
 */

if (php_sapi_name() !== 'cli') {
  fwrite(STDERR, "This script must be run from CLI.\n");
  exit(1);
}

if (empty($args[0])) {
  fwrite(STDERR, "Usage: drush scr scripts/export-locale-po.php <langcode> [textgroup] [output-path]\n");
  exit(1);
}

$langcode = $args[0];
$textgroup = !empty($args[1]) ? $args[1] : 'default';
$output_path = !empty($args[2]) ? $args[2] : ('translations/po/' . $langcode . '.' . $textgroup . '.po');

module_load_include('inc', 'locale', 'locale.admin');

$languages = language_list();
if (empty($languages[$langcode])) {
  fwrite(STDERR, "Unknown language code: {$langcode}\n");
  exit(1);
}

$language = $languages[$langcode];
$po_data = _locale_export_po_generate($language, _locale_export_get_strings($language, $textgroup));

$target = $output_path;
if ($target[0] !== '/') {
  $target = DRUPAL_ROOT . '/' . $target;
}

$target_dir = dirname($target);
if (!is_dir($target_dir) && !mkdir($target_dir, 0775, TRUE)) {
  fwrite(STDERR, "Cannot create output directory: {$target_dir}\n");
  exit(1);
}

if (file_put_contents($target, $po_data) === FALSE) {
  fwrite(STDERR, "Failed writing file: {$target}\n");
  exit(1);
}

fwrite(STDOUT, "Exported {$langcode} ({$textgroup}) to {$target}\n");
