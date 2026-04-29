<?php

/**
 * @file
 * Imports one .po file into Drupal locale tables.
 *
 * Usage:
 *   drush74 --uri=dev.rovas.app scr scripts/import-locale-po.php sk translations/po/sk.default.po replace default
 */

if (php_sapi_name() !== 'cli') {
  fwrite(STDERR, "This script must be run from CLI.\n");
  exit(1);
}

$effective_args = $args;
if (!empty($effective_args[0]) && is_string($effective_args[0])) {
  if (preg_match('/import-locale-po\.php$/', $effective_args[0])) {
    array_shift($effective_args);
  }
}

if (empty($effective_args[0]) || empty($effective_args[1])) {
  fwrite(STDERR, "Usage: drush scr scripts/import-locale-po.php <langcode> <po-path> [replace|keep] [textgroup]\n");
  exit(1);
}

$langcode = $effective_args[0];
$po_path = $effective_args[1];
$mode_name = !empty($effective_args[2]) ? strtolower($effective_args[2]) : 'replace';
$textgroup = !empty($effective_args[3]) ? $effective_args[3] : 'default';
$mode = ($mode_name === 'replace') ? LOCALE_IMPORT_OVERWRITE : LOCALE_IMPORT_KEEP;

$languages = language_list();
if (empty($languages[$langcode])) {
  fwrite(STDERR, "Language is not enabled: {$langcode}\n");
  exit(1);
}

$uri = $po_path;
if ($uri[0] !== '/') {
  $uri = DRUPAL_ROOT . '/' . $uri;
}

if (!file_exists($uri)) {
  fwrite(STDERR, "PO file not found: {$uri}\n");
  exit(1);
}

$file = (object) array(
  'uri' => $uri,
  'filename' => basename($uri),
);

$ok = _locale_import_po($file, $langcode, $mode, $textgroup);
if (!$ok) {
  fwrite(STDERR, "Import failed for {$uri}\n");
  exit(1);
}

fwrite(STDOUT, "Imported {$uri} into {$langcode} (group={$textgroup}, mode={$mode_name})\n");
