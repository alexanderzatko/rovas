<?php

/**
 * @file
 * Default view: sugar_daddy
 */

$view = new view();
$view->name = 'sugar_daddy';
$view->description = '';
$view->tag = 'default';
$view->base_table = 'node';
$view->human_name = 'sugar daddy';
$view->core = 7;
$view->api_version = '3.0';
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

/* Display: Master */
$handler = $view->new_display('default', 'Master', 'default');
$handler->display->display_options['title'] = 'sugar daddy';
$handler->display->display_options['use_more_always'] = FALSE;
$handler->display->display_options['group_by'] = TRUE;
$handler->display->display_options['access']['type'] = 'perm';
$handler->display->display_options['cache']['type'] = 'none';
$handler->display->display_options['query']['type'] = 'views_query';
$handler->display->display_options['exposed_form']['type'] = 'basic';
$handler->display->display_options['pager']['type'] = 'full';
$handler->display->display_options['pager']['options']['items_per_page'] = '10';
$handler->display->display_options['style_plugin'] = 'tablesummarized';
$handler->display->display_options['style_options']['columns'] = array(
  'raw_field' => 'raw_field',
  'field_amount_paid' => 'field_amount_paid',
  'field_purchase_date' => 'field_purchase_date',
);
$handler->display->display_options['style_options']['class'] = '';
$handler->display->display_options['style_options']['default'] = '-1';
$handler->display->display_options['style_options']['info'] = array(
  'raw_field' => array(
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
    'summarize' => 'none',
  ),
  'field_amount_paid' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
    'summarize' => 'total',
  ),
  'field_purchase_date' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
    'summarize' => 'none',
  ),
);
/* Relationship: Entity Reference: Referenced Entity */
$handler->display->display_options['relationships']['field_buyer_target_id']['id'] = 'field_buyer_target_id';
$handler->display->display_options['relationships']['field_buyer_target_id']['table'] = 'field_data_field_buyer';
$handler->display->display_options['relationships']['field_buyer_target_id']['field'] = 'field_buyer_target_id';
$handler->display->display_options['relationships']['field_buyer_target_id']['label'] = 'buyer';
/* Field: Raw SQL: Raw field */
$handler->display->display_options['fields']['raw_field']['id'] = 'raw_field';
$handler->display->display_options['fields']['raw_field']['table'] = 'views_raw_sql';
$handler->display->display_options['fields']['raw_field']['field'] = 'raw_field';
$handler->display->display_options['fields']['raw_field']['label'] = 'Date';
$handler->display->display_options['fields']['raw_field']['raw_sql'] = 'FROM_UNIXTIME(field_data_field_purchase_date.field_purchase_date_value,\'%d %M %Y\')';
/* Field: SUM(Content: Amount Paid) */
$handler->display->display_options['fields']['field_amount_paid']['id'] = 'field_amount_paid';
$handler->display->display_options['fields']['field_amount_paid']['table'] = 'field_data_field_amount_paid';
$handler->display->display_options['fields']['field_amount_paid']['field'] = 'field_amount_paid';
$handler->display->display_options['fields']['field_amount_paid']['group_type'] = 'sum';
$handler->display->display_options['fields']['field_amount_paid']['settings'] = array(
  'thousand_separator' => '',
  'decimal_separator' => '.',
  'scale' => '2',
  'prefix_suffix' => 1,
);
/* Field: SUM(Content: Purchase Date) */
$handler->display->display_options['fields']['field_purchase_date']['id'] = 'field_purchase_date';
$handler->display->display_options['fields']['field_purchase_date']['table'] = 'field_data_field_purchase_date';
$handler->display->display_options['fields']['field_purchase_date']['field'] = 'field_purchase_date';
$handler->display->display_options['fields']['field_purchase_date']['group_type'] = 'sum';
$handler->display->display_options['fields']['field_purchase_date']['exclude'] = TRUE;
/* Sort criterion: Raw SQL: Raw sort */
$handler->display->display_options['sorts']['raw_sort']['id'] = 'raw_sort';
$handler->display->display_options['sorts']['raw_sort']['table'] = 'views_raw_sql';
$handler->display->display_options['sorts']['raw_sort']['field'] = 'raw_sort';
$handler->display->display_options['sorts']['raw_sort']['order'] = 'DESC';
$handler->display->display_options['sorts']['raw_sort']['raw_sql'] = 'FROM_UNIXTIME(field_data_field_purchase_date.field_purchase_date_value,\'%Y%m%d\')';
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
$handler->display->display_options['filters']['type']['group'] = 1;
/* Filter criterion: User: Name */
$handler->display->display_options['filters']['uid']['id'] = 'uid';
$handler->display->display_options['filters']['uid']['table'] = 'users';
$handler->display->display_options['filters']['uid']['field'] = 'uid';
$handler->display->display_options['filters']['uid']['relationship'] = 'field_buyer_target_id';
$handler->display->display_options['filters']['uid']['value'] = '';
$handler->display->display_options['filters']['uid']['group'] = 1;
$handler->display->display_options['filters']['uid']['exposed'] = TRUE;
$handler->display->display_options['filters']['uid']['expose']['operator_id'] = 'uid_op';
$handler->display->display_options['filters']['uid']['expose']['label'] = 'Name';
$handler->display->display_options['filters']['uid']['expose']['operator'] = 'uid_op';
$handler->display->display_options['filters']['uid']['expose']['identifier'] = 'uid';
$handler->display->display_options['filters']['uid']['expose']['remember_roles'] = array(
  2 => '2',
);
/* Filter criterion: Content: Project (field_receipt_project) */
$handler->display->display_options['filters']['field_receipt_project_target_id']['id'] = 'field_receipt_project_target_id';
$handler->display->display_options['filters']['field_receipt_project_target_id']['table'] = 'field_data_field_receipt_project';
$handler->display->display_options['filters']['field_receipt_project_target_id']['field'] = 'field_receipt_project_target_id';
$handler->display->display_options['filters']['field_receipt_project_target_id']['operator'] = '!=';
$handler->display->display_options['filters']['field_receipt_project_target_id']['value']['value'] = '1992';

/* Display: Page */
$handler = $view->new_display('page', 'Page', 'page');
$handler->display->display_options['path'] = 'sugar-daddy';
$translatables['sugar_daddy'] = array(
  t('Master'),
  t('sugar daddy'),
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
  t('buyer'),
  t('Date'),
  t('Amount Paid'),
  t('Purchase Date'),
  t('Name'),
  t('Page'),
);

