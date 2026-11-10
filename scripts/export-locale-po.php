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

// Drush's `scr` can pass the script filename as the first element of $args.
// When that happens, the language code is the next element.
$effective_args = $args;
if (!empty($effective_args[0]) && is_string($effective_args[0])) {
  if (preg_match('/export-locale-po\.php$/', $effective_args[0])) {
    array_shift($effective_args);
  }
}

if (empty($effective_args[0])) {
  fwrite(STDERR, "Usage: drush scr scripts/export-locale-po.php <langcode> [textgroup] [output-path]\n");
  exit(1);
}

$langcode = $effective_args[0];
$textgroup = !empty($effective_args[1]) ? $effective_args[1] : 'default';
$output_path = !empty($effective_args[2]) ? $effective_args[2] : ('translations/po/' . $langcode . '.' . $textgroup . '.po');

module_load_include('inc', 'locale', 'locale.admin');

$languages = language_list();
if (empty($languages[$langcode])) {
  fwrite(STDERR, "Unknown language code: {$langcode}\n");
  exit(1);
}

$language = $languages[$langcode];
$po_data = _locale_export_po_generate($language, _locale_export_get_strings($language, $textgroup));

// Strip query strings from #: location comments. Potx/locale often records the
// full request path (tokens, emails, signatures) when a string was first seen.
$po_data = preg_replace_callback(
  '/^#: (.+)$/m',
  function ($m) {
    $parts = preg_split('/\s+/', trim($m[1]));
    $clean = array();
    foreach ($parts as $part) {
      $q = strpos($part, '?');
      $clean[] = ($q === FALSE) ? $part : substr($part, 0, $q);
    }
    return '#: ' . implode(' ', $clean);
  },
  $po_data
);

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
