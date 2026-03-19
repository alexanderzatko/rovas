<?php
/**
 * Rebuilds the Drupal class registry by scanning all PHP files.
 *
 * Run this from the Drupal root (or any directory) when the registry is
 * broken or out of date — e.g. after restoring an old database backup while
 * the filesystem already has the new module directory structure.
 *
 * Usage:
 *   php ops/rebuild_registry.php
 */

define('DRUPAL_ROOT', '/var/www/rovas');
chdir(DRUPAL_ROOT);
require_once DRUPAL_ROOT . '/includes/bootstrap.inc';
drupal_bootstrap(DRUPAL_BOOTSTRAP_DATABASE);

/**
 * Extracts all class and interface definitions from a PHP file.
 *
 * Returns an array of [type, short_name, fully_qualified_name] triples.
 * Both the short name and the fully-qualified namespaced name are returned
 * so that Drupal's registry lookup works for both PSR-0/PSR-4 and legacy code.
 */
function extract_classes($file) {
  $contents = file_get_contents($file);
  $tokens = token_get_all($contents);
  $results = [];
  $namespace = '';

  for ($i = 0; $i < count($tokens); $i++) {
    $tok = $tokens[$i];

    // Capture namespace declaration.
    if (is_array($tok) && $tok[0] == T_NAMESPACE) {
      $j = $i + 1;
      $ns = '';
      while ($j < count($tokens) && $tokens[$j] !== ';') {
        if (is_array($tokens[$j])) {
          $ns .= $tokens[$j][1];
        }
        $j++;
      }
      $namespace = trim($ns);
    }

    // Capture class and interface definitions.
    if (is_array($tok) && in_array($tok[0], [T_CLASS, T_INTERFACE])) {
      $type = $tok[0] == T_CLASS ? 'class' : 'interface';
      $j = $i + 1;
      while ($j < count($tokens) && is_array($tokens[$j]) && $tokens[$j][0] == T_WHITESPACE) {
        $j++;
      }
      if ($j < count($tokens) && is_array($tokens[$j]) && $tokens[$j][0] == T_STRING) {
        $short = $tokens[$j][1];
        $fqn = $namespace ? $namespace . '\\' . $short : $short;
        $results[] = [$type, $short, $fqn];
      }
    }
  }

  return $results;
}

$scan_dirs = [
  DRUPAL_ROOT . '/includes',
  DRUPAL_ROOT . '/modules',
  DRUPAL_ROOT . '/sites/all/modules',
];

$files = [];
foreach ($scan_dirs as $dir) {
  $it = new RecursiveIteratorIterator(new RecursiveDirectoryIterator($dir));
  foreach ($it as $f) {
    if (in_array($f->getExtension(), ['php', 'inc', 'module', 'install'])) {
      $files[] = $f->getPathname();
    }
  }
}

echo 'Scanning ' . count($files) . " files...\n";

db_query('TRUNCATE TABLE {registry}');
db_query('TRUNCATE TABLE {registry_file}');

$count = 0;
foreach ($files as $full_path) {
  $relative = str_replace(DRUPAL_ROOT . '/', '', $full_path);
  $classes = extract_classes($full_path);

  db_merge('registry_file')
    ->key(['filename' => $relative])
    ->fields(['hash' => md5_file($full_path)])
    ->execute();

  foreach ($classes as $cl) {
    list($type, $short, $fqn) = $cl;
    // Insert both short name and fully-qualified name so both legacy
    // Drupal autoloading and PSR-4 namespace lookups work.
    foreach (array_unique([$short, $fqn]) as $name) {
      db_merge('registry')
        ->key(['name' => $name, 'type' => $type])
        ->fields(['filename' => $relative, 'module' => '', 'weight' => 0])
        ->execute();
      $count++;
    }
  }
}

echo "Inserted $count class/interface entries.\n";
echo "Done. Now run: drush74 cc all -y\n";
