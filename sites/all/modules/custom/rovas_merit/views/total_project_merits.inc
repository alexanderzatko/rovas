<?php

/**
 * @file
 * Default view: total_project_merits
 */

$view = new view();
$view->name = 'total_project_merits';
$view->description = '';
$view->tag = 'default';
$view->base_table = 'node';
$view->human_name = 'total project merits';
$view->core = 7;
$view->api_version = '3.0';
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

/* Display: Master */
$handler = $view->new_display('default', 'Master', 'default');
$handler->display->display_options['use_more_always'] = FALSE;
$handler->display->display_options['group_by'] = TRUE;
$handler->display->display_options['access']['type'] = 'perm';
$handler->display->display_options['cache']['type'] = 'none';
$handler->display->display_options['query']['type'] = 'views_query';
$handler->display->display_options['exposed_form']['type'] = 'basic';
$handler->display->display_options['pager']['type'] = 'none';
$handler->display->display_options['pager']['options']['offset'] = '0';
$handler->display->display_options['style_plugin'] = 'table';
$handler->display->display_options['style_options']['columns'] = array(
  'field_reward_amount' => 'field_reward_amount',
  'nid' => 'nid',
  'nid_1' => 'nid_1',
);
$handler->display->display_options['style_options']['class'] = '';
$handler->display->display_options['style_options']['default'] = '-1';
$handler->display->display_options['style_options']['info'] = array(
  'field_reward_amount' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'nid' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'nid_1' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
);
/* Relationship: Entity Reference: Referenced Entity */
$handler->display->display_options['relationships']['field_parent_purchase_receipt_target_id']['id'] = 'field_parent_purchase_receipt_target_id';
$handler->display->display_options['relationships']['field_parent_purchase_receipt_target_id']['table'] = 'field_data_field_parent_purchase_receipt';
$handler->display->display_options['relationships']['field_parent_purchase_receipt_target_id']['field'] = 'field_parent_purchase_receipt_target_id';
$handler->display->display_options['relationships']['field_parent_purchase_receipt_target_id']['label'] = 'purchase_receipt';
/* Relationship: Entity Reference: Referenced Entity */
$handler->display->display_options['relationships']['field_receipt_project_target_id']['id'] = 'field_receipt_project_target_id';
$handler->display->display_options['relationships']['field_receipt_project_target_id']['table'] = 'field_data_field_receipt_project';
$handler->display->display_options['relationships']['field_receipt_project_target_id']['field'] = 'field_receipt_project_target_id';
$handler->display->display_options['relationships']['field_receipt_project_target_id']['relationship'] = 'field_parent_purchase_receipt_target_id';
$handler->display->display_options['relationships']['field_receipt_project_target_id']['label'] = 'project';
/* Field: SUM(Content: Reward amount) */
$handler->display->display_options['fields']['field_reward_amount']['id'] = 'field_reward_amount';
$handler->display->display_options['fields']['field_reward_amount']['table'] = 'field_data_field_reward_amount';
$handler->display->display_options['fields']['field_reward_amount']['field'] = 'field_reward_amount';
$handler->display->display_options['fields']['field_reward_amount']['group_type'] = 'sum';
$handler->display->display_options['fields']['field_reward_amount']['settings'] = array(
  'thousand_separator' => '',
  'decimal_separator' => '.',
  'scale' => '2',
  'prefix_suffix' => 1,
);
/* Contextual filter: Content: Nid */
$handler->display->display_options['arguments']['nid']['id'] = 'nid';
$handler->display->display_options['arguments']['nid']['table'] = 'node';
$handler->display->display_options['arguments']['nid']['field'] = 'nid';
$handler->display->display_options['arguments']['nid']['relationship'] = 'field_receipt_project_target_id';
$handler->display->display_options['arguments']['nid']['default_argument_type'] = 'fixed';
$handler->display->display_options['arguments']['nid']['summary']['number_of_records'] = '0';
$handler->display->display_options['arguments']['nid']['summary']['format'] = 'default_summary';
$handler->display->display_options['arguments']['nid']['summary_options']['items_per_page'] = '25';
/* Filter criterion: Content: Published status */
$handler->display->display_options['filters']['status']['id'] = 'status';
$handler->display->display_options['filters']['status']['table'] = 'node';
$handler->display->display_options['filters']['status']['field'] = 'status';
$handler->display->display_options['filters']['status']['value'] = 1;
$handler->display->display_options['filters']['status']['group'] = 1;
$handler->display->display_options['filters']['status']['expose']['operator'] = FALSE;
/* Filter criterion: Content: Type */
$handler->display->display_options['filters']['type']['id'] = 'type';
$handler->display->display_options['filters']['type']['table'] = 'node';
$handler->display->display_options['filters']['type']['field'] = 'type';
$handler->display->display_options['filters']['type']['value'] = array(
  'merits_disbursement' => 'merits_disbursement',
);
$translatables['total_project_merits'] = array(
  t('Master'),
  t('more'),
  t('Apply'),
  t('Reset'),
  t('Sort by'),
  t('Asc'),
  t('Desc'),
  t('purchase_receipt'),
  t('project'),
  t('Reward amount'),
  t('All'),
);

