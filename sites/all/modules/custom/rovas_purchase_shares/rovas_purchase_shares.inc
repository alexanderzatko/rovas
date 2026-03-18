<?php

/**
 * @file
 * rovas_purchase_shares.features.inc
 */

/**
 * Implements hook_views_api().
 */
function rovas_purchase_shares_views_api($module = NULL, $api = NULL) {
  return array("api" => "3.0");
}

/**
 * Implements hook_node_info().
 */
function rovas_purchase_shares_node_info() {
  $items = array(
    'time_share' => array(
      'name' => t('Time share'),
      'base' => 'node_content',
      'description' => '',
      'has_title' => '1',
      'title_label' => t('Title'),
      'help' => '',
    ),
    'timeshare_merit_allocation' => array(
      'name' => t('Time shareholder Merit Allocation'),
      'base' => 'node_content',
      'description' => '',
      'has_title' => '1',
      'title_label' => t('Title'),
      'help' => '',
    ),
  );
  drupal_alter('node_info', $items);
  return $items;
}
