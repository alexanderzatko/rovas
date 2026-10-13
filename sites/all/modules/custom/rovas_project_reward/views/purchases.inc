<?php

/**
 * @file
 * Default view: purchases
 */

$view = new view();
$view->name = 'purchases';
$view->description = '';
$view->tag = 'default';
$view->base_table = 'node';
$view->human_name = 'Purchases';
$view->core = 7;
$view->api_version = '3.0';
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

/* Display: Master */
$handler = $view->new_display('default', 'Master', 'default');
$handler->display->display_options['title'] = 'Purchases';
$handler->display->display_options['use_more_always'] = FALSE;
$handler->display->display_options['access']['type'] = 'php';
$handler->display->display_options['access']['php_access'] = 'global $user;
return arg(1) == $user->uid;';
$handler->display->display_options['cache']['type'] = 'none';
$handler->display->display_options['query']['type'] = 'views_query';
$handler->display->display_options['exposed_form']['type'] = 'basic';
$handler->display->display_options['pager']['type'] = 'full';
$handler->display->display_options['pager']['options']['items_per_page'] = '10';
$handler->display->display_options['style_plugin'] = 'table';
/* Field: Content: Post date */
$handler->display->display_options['fields']['created']['id'] = 'created';
$handler->display->display_options['fields']['created']['table'] = 'node';
$handler->display->display_options['fields']['created']['field'] = 'created';
$handler->display->display_options['fields']['created']['label'] = 'Date Purchased';
$handler->display->display_options['fields']['created']['date_format'] = 'custom';
$handler->display->display_options['fields']['created']['custom_date_format'] = 'd M Y - H:i:s';
$handler->display->display_options['fields']['created']['second_date_format'] = 'privatemsg_current_day';
/* Field: Content: Nid */
$handler->display->display_options['fields']['nid']['id'] = 'nid';
$handler->display->display_options['fields']['nid']['table'] = 'node';
$handler->display->display_options['fields']['nid']['field'] = 'nid';
$handler->display->display_options['fields']['nid']['label'] = 'Purchase receipt ID';
$handler->display->display_options['fields']['nid']['link_to_node'] = TRUE;
/* Field: Content: Project */
$handler->display->display_options['fields']['field_receipt_project_1']['id'] = 'field_receipt_project_1';
$handler->display->display_options['fields']['field_receipt_project_1']['table'] = 'field_data_field_receipt_project';
$handler->display->display_options['fields']['field_receipt_project_1']['field'] = 'field_receipt_project';
$handler->display->display_options['fields']['field_receipt_project_1']['label'] = 'Project ID';
$handler->display->display_options['fields']['field_receipt_project_1']['type'] = 'entityreference_entity_id';
$handler->display->display_options['fields']['field_receipt_project_1']['settings'] = array(
  'bypass_access' => 0,
  'link' => 0,
);
/* Field: Content: Project */
$handler->display->display_options['fields']['field_receipt_project']['id'] = 'field_receipt_project';
$handler->display->display_options['fields']['field_receipt_project']['table'] = 'field_data_field_receipt_project';
$handler->display->display_options['fields']['field_receipt_project']['field'] = 'field_receipt_project';
$handler->display->display_options['fields']['field_receipt_project']['settings'] = array(
  'bypass_access' => 0,
  'link' => 1,
);
/* Field: Content: Amount Paid */
$handler->display->display_options['fields']['field_amount_paid']['id'] = 'field_amount_paid';
$handler->display->display_options['fields']['field_amount_paid']['table'] = 'field_data_field_amount_paid';
$handler->display->display_options['fields']['field_amount_paid']['field'] = 'field_amount_paid';
$handler->display->display_options['fields']['field_amount_paid']['settings'] = array(
  'thousand_separator' => '',
  'decimal_separator' => '.',
  'scale' => '2',
  'prefix_suffix' => 1,
);
/* Sort criterion: Content: Post date */
$handler->display->display_options['sorts']['created']['id'] = 'created';
$handler->display->display_options['sorts']['created']['table'] = 'node';
$handler->display->display_options['sorts']['created']['field'] = 'created';
$handler->display->display_options['sorts']['created']['order'] = 'DESC';
/* Contextual filter: Content: Buyer (field_buyer) */
$handler->display->display_options['arguments']['field_buyer_target_id']['id'] = 'field_buyer_target_id';
$handler->display->display_options['arguments']['field_buyer_target_id']['table'] = 'field_data_field_buyer';
$handler->display->display_options['arguments']['field_buyer_target_id']['field'] = 'field_buyer_target_id';
$handler->display->display_options['arguments']['field_buyer_target_id']['default_action'] = 'default';
$handler->display->display_options['arguments']['field_buyer_target_id']['default_argument_type'] = 'user';
$handler->display->display_options['arguments']['field_buyer_target_id']['default_argument_options']['user'] = FALSE;
$handler->display->display_options['arguments']['field_buyer_target_id']['summary']['number_of_records'] = '0';
$handler->display->display_options['arguments']['field_buyer_target_id']['summary']['format'] = 'default_summary';
$handler->display->display_options['arguments']['field_buyer_target_id']['summary_options']['items_per_page'] = '25';
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
  'purchase_receipt' => 'purchase_receipt',
);

/* Display: Block */
$handler = $view->new_display('block', 'Block', 'block_1');
$handler->display->display_options['block_description'] = 'Purchases';
$translatables['purchases'] = array(
  t('Master'),
  t('Purchases'),
  t('more'),
  t('Apply'),
  t('Reset'),
  t('Sort by'),
  t('Asc'),
  t('Desc'),
  t('Items per page'),
  t('- All -'),
  t('Offset'),
  t('« first'),
  t('‹ previous'),
  t('next ›'),
  t('last »'),
  t('Date Purchased'),
  t('Purchase receipt ID'),
  t('Project ID'),
  t('Project'),
  t('Amount Paid'),
  t('All'),
  t('Block'),
);

