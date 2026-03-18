<?php
/**
 * @file
 * One-off migration: delete rovas_site_rules DB entries and let Entity API
 * re-import them from hook_default_rules_configuration() via
 * entity_defaults_rebuild(). Rules will be assigned ENTITY_IN_CODE ("Default")
 * status because they are defined purely in code (JSON files + hook).
 *
 * Uses Batch API to avoid lock timeouts when run from the web. When run via
 * Drush CLI, executes directly (no batch) since there's no lock contention.
 *
 * Both delete and rebuild are one-rule-per-batch-operation. Doing all deletes
 * in a single operation caused MySQL "Lock wait timeout exceeded" on the
 * {rules_trigger} and {semaphore} tables.
 *
 * Note: entity_defaults_rebuild() calls hook_default_rules_configuration(),
 * which calls rules_import() for each JSON file. For complex rules this can
 * produce a deep call stack. If Xdebug is active, raise
 * xdebug.max_nesting_level (e.g. 500) or disable Xdebug for CLI cache clears.
 */

/**
 * Runs the migration. Uses batch when in web context, direct when via Drush.
 */
function rovas_site_rules_migration_run() {
  if (rovas_site_rules_migration_is_cli()) {
    rovas_site_rules_migration_run_direct();
  }
  else {
    rovas_site_rules_migration_run_batch();
  }
}

/**
 * Returns TRUE if running in CLI (e.g. Drush) where batch isn't needed.
 */
function rovas_site_rules_migration_is_cli() {
  return (php_sapi_name() === 'cli');
}

/**
 * Returns the list of rule machine names owned by this module.
 *
 * Reads all JSON filenames from rules/ and excludes rules provided by other
 * modules that must not be deleted or reimported here.
 *
 * @return array
 *   Flat array of rule machine names.
 */
function _rovas_site_rules_migration_names() {
  $path = drupal_get_path('module', 'rovas_site_rules') . '/rules/';

  // Rules provided by other modules — never delete or reimport these.
  $exclude = array(
    'rules_check_or_add_time_shareholder_core',
    'rules_create_time_share',
  );

  $names = array();
  foreach (glob($path . '*.json') as $f) {
    $name = basename($f, '.json');
    if (!in_array($name, $exclude)) {
      $names[] = $name;
    }
  }

  return $names;
}

/**
 * Runs the migration via Batch API (web context).
 */
function rovas_site_rules_migration_run_batch() {
  $names = _rovas_site_rules_migration_names();

  $operations = array();

  // Delete phase: one rule per operation to avoid lock-wait timeouts.
  foreach ($names as $name) {
    $operations[] = array('_rovas_site_rules_migration_delete_one', array($name));
  }

  // Rebuild: entity_defaults_rebuild() re-imports all rules from the hook,
  // saving them with ENTITY_IN_CODE ("Default") status.
  $operations[] = array('_rovas_site_rules_migration_rebuild', array());

  $batch = array(
    'title'      => t('Migrating rovas_site_rules'),
    'operations' => $operations,
    'finished'   => '_rovas_site_rules_migration_finished',
  );

  batch_set($batch);
  batch_process();
}

/**
 * Runs the migration directly (CLI context).
 */
function rovas_site_rules_migration_run_direct() {
  set_time_limit(0);

  $names = _rovas_site_rules_migration_names();
  $ctx = array();

  // Delete all rules via direct DB queries (avoids entity_defaults_rebuild
  // recursion from EntityAPIControllerExportable::delete).
  foreach ($names as $name) {
    _rovas_site_rules_migration_delete_one($name, $ctx);
  }
  $deleted = isset($ctx['results']['deleted']) ? $ctx['results']['deleted'] : 0;

  // Re-import from hook: clears cache, rebuilds defaults, clears cache again.
  _rovas_site_rules_migration_rebuild(array(), $ctx);

  if (function_exists('drush_print')) {
    drush_print(dt('Deleted @count rules; re-imported via entity_defaults_rebuild().', array(
      '@count' => $deleted,
    )));
  }
}

/**
 * Batch op: Delete a single rule from the DB by name.
 *
 * Uses direct DB queries instead of $rule->delete() to avoid the infinite
 * recursion caused by EntityAPIControllerExportable::delete() calling
 * entity_defaults_rebuild(), which invokes other modules'
 * hook_default_rules_configuration() (e.g. commerce_checkout), which loads
 * entities, which triggers another rebuild, and so on until PHP hits its
 * maximum function nesting level.
 */
function _rovas_site_rules_migration_delete_one($name, &$context) {
  $id = db_select('rules_config', 'rc')
    ->fields('rc', array('id'))
    ->condition('name', $name)
    ->execute()
    ->fetchField();

  if ($id) {
    db_delete('rules_config')->condition('id', $id)->execute();
    db_delete('rules_trigger')->condition('id', $id)->execute();
    db_delete('rules_tags')->condition('id', $id)->execute();
    db_delete('rules_scheduler')->condition('config', $name)->execute();

    if (!isset($context['results']['deleted'])) {
      $context['results']['deleted'] = 0;
    }
    $context['results']['deleted']++;
  }
  $context['message'] = t('Deleted: @name', array('@name' => $name));
}

/**
 * Batch op: Clear Rules cache, rebuild entity defaults, clear cache again.
 *
 * entity_defaults_rebuild() calls hook_default_rules_configuration(), which
 * imports each rule from its JSON file and returns it to Entity API. Entity API
 * then saves each rule to DB with ENTITY_IN_CODE status, which the Rules UI
 * displays as "Default".
 */
function _rovas_site_rules_migration_rebuild($args, &$context) {
  rules_clear_cache(TRUE);
  entity_defaults_rebuild();
  rules_clear_cache(TRUE);
  $context['message'] = t('Rules rebuilt from code (Default status).');
}

/**
 * Batch finished callback.
 */
function _rovas_site_rules_migration_finished($success, $results, $operations) {
  $deleted = isset($results['deleted']) ? $results['deleted'] : 0;
  if ($success) {
    drupal_set_message(t('Migration complete. Deleted @deleted rules; re-imported from code.', array(
      '@deleted' => $deleted,
    )), 'status');
  }
  else {
    drupal_set_message(t('Migration finished with errors.'), 'error');
  }
}
