<?php

/**
 * @file
 * Supporting hooks for rovas_forex.
 */

/**
 * Implements hook_views_api().
 */
function rovas_forex_views_api($module = NULL, $api = NULL) {
  return array('api' => '3.0');
}
