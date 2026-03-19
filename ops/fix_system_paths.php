<?php
/**
 * Updates the Drupal system table to reflect the new module directory structure.
 *
 * After the modules were reorganized from the flat sites/all/modules/<name>/
 * layout into sites/all/modules/contrib/ and sites/all/modules/custom/
 * subdirectories, a database backup made before that change will contain
 * old file paths in the system table.
 *
 * This script updates those paths by checking which new location each module
 * actually exists at on disk.
 *
 * Usage:
 *   php ops/fix_system_paths.php
 *
 * Run this BEFORE rebuild_registry.php and drush74 cc all.
 */

define('DRUPAL_ROOT', '/var/www/rovas');
chdir(DRUPAL_ROOT);
require_once DRUPAL_ROOT . '/includes/bootstrap.inc';
drupal_bootstrap(DRUPAL_BOOTSTRAP_DATABASE);

$rows = db_query("SELECT filename FROM system WHERE filename LIKE 'sites/all/modules/%'")->fetchCol();

$updated = 0;
$not_found = [];

foreach ($rows as $old_path) {
  // Skip paths that are already in the correct subdirectory.
  if (strpos($old_path, '/contrib/') !== FALSE || strpos($old_path, '/custom/') !== FALSE) {
    continue;
  }

  // Try contrib/ first, then custom/.
  $new_path = NULL;
  foreach (['contrib', 'custom'] as $subdir) {
    $candidate = preg_replace(
      '#^(sites/all/modules/)([^/]+/)#',
      '$1' . $subdir . '/$2',
      $old_path
    );
    if (file_exists(DRUPAL_ROOT . '/' . $candidate)) {
      $new_path = $candidate;
      break;
    }
  }

  if ($new_path) {
    db_query(
      "UPDATE system SET filename = :new WHERE filename = :old",
      [':new' => $new_path, ':old' => $old_path]
    );
    echo "  $old_path\n    -> $new_path\n";
    $updated++;
  }
  else {
    $not_found[] = $old_path;
  }
}

echo "\nUpdated $updated rows in the system table.\n";

if ($not_found) {
  echo "\nWARNING: Could not find new location for " . count($not_found) . " path(s):\n";
  foreach ($not_found as $p) {
    echo "  $p\n";
  }
}

echo "\nDone. Next steps:\n";
echo "  php ops/rebuild_registry.php\n";
echo "  drush74 cc all -y\n";
echo "  drush74 updb -y\n";
