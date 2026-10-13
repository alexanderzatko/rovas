<?php

/**
 * @file
 * Default Views for rovas_money_transfer (one file per view under views/).
 */

/**
 * Implements hook_views_default_views().
 */
function rovas_money_transfer_views_default_views() {
  return _rovas_money_transfer_views_load_default_views();
}

/**
 * Load exported Views from views/*.inc.
 *
 * Each file defines $view (Views UI export). Returns keyed by machine name.
 */
function _rovas_money_transfer_views_load_default_views() {
  $export = array();
  $path = drupal_get_path('module', 'rovas_money_transfer') . '/views';
  if (!is_dir($path)) {
    return $export;
  }
  foreach (glob($path . '/*.inc') as $file) {
    $view = NULL;
    $handler = NULL;
    include $file;
    if (isset($view) && is_object($view) && !empty($view->name)) {
      $export[$view->name] = $view;
    }
    unset($view, $handler, $translatables);
  }
  return $export;
}
