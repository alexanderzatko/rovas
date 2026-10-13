<?php

/**
 * @file
 * Revert DB overrides for Views owned by rovas_purchase_shares.
 *
 * After deploy, run:
 *   drush74 php-eval "module_load_include('inc', 'rovas_purchase_shares', 'rovas_purchase_shares.views_migration'); rovas_purchase_shares_views_migration_run();"
 */

/**
 * Delete {views_view} / {views_display} rows for this module's default Views.
 *
 * Code defaults then apply after Views cache clear. Does not touch other modules.
 */
function rovas_purchase_shares_views_migration_run() {
  module_load_include('inc', 'rovas_purchase_shares', 'rovas_purchase_shares.views_default');
  if (!function_exists('_rovas_purchase_shares_views_load_default_views')) {
    drupal_set_message(t('rovas_purchase_shares: views loader missing.'), 'error');
    return;
  }
  // Only views/*.inc — do not force-revert the three inline exports in
  // views_default.inc (project_coworkers / merit chart may be Overridden on
  // prod relative to the older code copy).
  $names = array_keys(_rovas_purchase_shares_views_load_default_views());

  if (empty($names)) {
    drupal_set_message(t('rovas_purchase_shares: no default Views to migrate.'), 'warning');
    return;
  }

  $vids = db_select('views_view', 'v')
    ->fields('v', array('vid'))
    ->condition('name', $names, 'IN')
    ->execute()
    ->fetchCol();

  $removed = 0;
  if (!empty($vids)) {
    $removed = db_delete('views_display')
      ->condition('vid', $vids, 'IN')
      ->execute();
    db_delete('views_view')
      ->condition('vid', $vids, 'IN')
      ->execute();
  }

  if (function_exists('views_invalidate_cache')) {
    views_invalidate_cache();
  }
  cache_clear_all('*', 'cache_views', TRUE);
  cache_clear_all('*', 'cache_views_data', TRUE);

  $msg = t('rovas_purchase_shares: removed overrides for @count view(s) (@names). Cleared Views caches.', array(
    '@count' => count($names),
    '@names' => implode(', ', $names),
  ));
  if (drupal_is_cli()) {
    print $msg . "\n";
    print t('Display rows deleted: @n', array('@n' => (int) $removed)) . "\n";
  }
  else {
    drupal_set_message($msg);
  }
}
