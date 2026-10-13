<?php

/**
 * @file
 * Revert DB overrides for Views owned by rovas_forex.
 *
 * After deploy, run:
 *   drush74 php-eval "module_load_include('inc', 'rovas_forex', 'rovas_forex.views_migration'); rovas_forex_views_migration_run();"
 */

/**
 * Delete {views_view} / {views_display} rows for this module's default Views.
 *
 * Code defaults then apply after Views cache clear. Does not touch other modules.
 */
function rovas_forex_views_migration_run() {
  module_load_include('inc', 'rovas_forex', 'rovas_forex.views_default');
  if (!function_exists('_rovas_forex_views_load_default_views')) {
    drupal_set_message(t('rovas_forex: views loader missing.'), 'error');
    return;
  }
  // Only views/*.inc — do not force-revert inline forex_purchases (may be
  // intentionally Overridden vs the older views_default.inc copy).
  $names = array_keys(_rovas_forex_views_load_default_views());

  if (empty($names)) {
    drupal_set_message(t('rovas_forex: no default Views to migrate.'), 'warning');
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

  $msg = t('rovas_forex: removed overrides for @count view(s) (@names). Cleared Views caches.', array(
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
