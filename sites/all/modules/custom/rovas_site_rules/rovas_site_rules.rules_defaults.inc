<?php
/**
 * @file
 * Default Rules configurations for rovas_site_rules.
 */

/**
 * Implements hook_default_rules_configuration().
 *
 * Imports rules from JSON files in rules/. A static guard prevents re-entrant
 * calls (e.g. when rules_import() triggers a cache rebuild internally during
 * entity_defaults_rebuild()). This lets Entity API correctly assign
 * ENTITY_IN_CODE ("Default") status to rules that have not been modified, and
 * ENTITY_OVERRIDDEN ("Overridden") to rules that have been customised in the UI.
 *
 * Note: rules_import() for complex rules can produce a deep call stack. If
 * Xdebug is active, raise xdebug.max_nesting_level (e.g. 500) or run cache
 * clears with Xdebug disabled:
 *   php -d xdebug.mode=off $(which drush) cc all
 */
function rovas_site_rules_default_rules_configuration() {
  static $running = FALSE;
  if ($running) {
    return array();
  }
  $running = TRUE;

  $items = array();

  $path = drupal_get_path('module', 'rovas_site_rules') . '/rules/';

  // Exclude rules already provided by other modules.
  $exclude = array(
    'rules_check_or_add_time_shareholder_core',  // rovas_purchase_shares
    'rules_create_time_share',             // rovas_purchase_shares
  );

  // Rules that depend on other rules in this module; load these last so their
  // component dependencies exist when entity_import resolves component refs.
  $load_last = array(
    'rules_proxy_create_work_report',  // needs rules_duplicate_test, rules_retrieve_user_id_from_api_key_and_token, rules_create_work_report
  );

  $files = glob($path . '*.json');
  $first_pass = array();
  $last_pass = array();
  foreach ($files as $f) {
    $name = basename($f, '.json');
    if (in_array($name, $exclude)) {
      continue;
    }
    if (in_array($name, $load_last)) {
      $last_pass[$name] = $f;
    }
    else {
      $first_pass[$name] = $f;
    }
  }

  foreach (array_merge($first_pass, $last_pass) as $name => $f) {
    $config = _rovas_site_rules_import_rule($f, $name);
    if ($config) {
      $config->module = 'rovas_site_rules';
      $items[$name] = $config;
    }
  }

  $running = FALSE;
  return $items;
}

/**
 * Import a single rule from a JSON file with error reporting.
 *
 * @param string $filepath
 *   Full path to the .json file.
 * @param string $name
 *   Machine name of the rule (for logging).
 *
 * @return object|false
 *   The rules config object or FALSE on failure.
 */
function _rovas_site_rules_import_rule($filepath, $name) {
  $json = file_get_contents($filepath);
  $error_msg = '';
  $config = rules_import($json, $error_msg);
  if (is_object($config)) {
    return $config;
  }
  $message = $error_msg
    ? 'Skipped invalid rule import: %name. Error: %error'
    : 'Skipped invalid rule import: %name';
  $args = array('%name' => $name);
  if ($error_msg) {
    $args['%error'] = $error_msg;
  }
  watchdog('rovas_site_rules', $message, $args, WATCHDOG_WARNING);
  return FALSE;
}
