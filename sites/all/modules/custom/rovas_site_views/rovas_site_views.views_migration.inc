<?php

/**
 * @file
 * Revert DB overrides for Views owned by rovas_site_views.
 *
 * After deploy, run:
 *   drush74 php-eval "module_load_include('inc', 'rovas_site_views', 'rovas_site_views.views_migration'); rovas_site_views_views_migration_run();"
 */

/**
 * Delete {views_view} / {views_display} rows for this module's default Views.
 *
 * Code defaults then apply after Views cache clear. Does not touch other modules.
 */
function rovas_site_views_views_migration_run() {
  module_load_include('inc', 'rovas_site_views', 'rovas_site_views.views_default');
  if (!function_exists('_rovas_site_views_views_load_default_views')) {
    drupal_set_message(t('rovas_site_views: views loader missing.'), 'error');
    return;
  }
  $names = array_keys(_rovas_site_views_views_load_default_views());

  if (empty($names)) {
    drupal_set_message(t('rovas_site_views: no default Views to migrate.'), 'warning');
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

  $msg = t('rovas_site_views: removed overrides for @count view(s) (@names). Cleared Views caches.', array(
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
