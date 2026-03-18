<?php
/**
 * @file
 * Diagnostic: test importing rules_proxy_create_work_report.
 * Run via: drush php-script test_import.php (from Drupal root)
 * Or: Devel > Execute PHP, paste contents.
 *
 * Prerequisites: rules_duplicate_test, rules_retrieve_user_id_from_api_key_and_token,
 * and rules_create_work_report must exist in the DB (the proxy rule depends on them).
 */
define('DRUPAL_ROOT', getcwd());
require_once DRUPAL_ROOT . '/includes/bootstrap.inc';
drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);

$path = drupal_get_path('module', 'rovas_site_rules') . '/rules/';
$f = $path . 'rules_proxy_create_work_report.json';

if (!file_exists($f)) {
  echo "File not found: $f\n";
  exit(1);
}

$json = file_get_contents($f);
$error_msg = '';
$config = rules_import($json, $error_msg);

echo "Result type: " . gettype($config) . "\n";

if (is_object($config)) {
  echo "SUCCESS: got object\n";
  echo "  name: " . (isset($config->name) ? $config->name : 'N/A') . "\n";
  echo "  plugin: " . (isset($config->plugin) ? $config->plugin : 'N/A') . "\n";
} else {
  echo "FAIL: rules_import did not return an object\n";
  if ($error_msg) {
    echo "Error: $error_msg\n";
  }
  $messages = drupal_get_messages();
  if (!empty($messages)) {
    print_r($messages);
  }
}
