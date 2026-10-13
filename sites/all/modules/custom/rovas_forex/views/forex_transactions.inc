<?php

/**
 * @file
 * Default view: forex_transactions
 */

$view = new view();
$view->name = 'forex_transactions';
$view->description = '';
$view->tag = 'default';
$view->base_table = 'node';
$view->human_name = 'Forex transactions';
$view->core = 7;
$view->api_version = '3.0';
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

/* Display: Master */
$handler = $view->new_display('default', 'Master', 'default');
$handler->display->display_options['title'] = 'Forex transactions';
$handler->display->display_options['use_more_always'] = FALSE;
$handler->display->display_options['access']['type'] = 'role';
$handler->display->display_options['access']['role'] = array(
  3 => '3',
);
$handler->display->display_options['cache']['type'] = 'none';
$handler->display->display_options['query']['type'] = 'views_query';
$handler->display->display_options['query']['options']['distinct'] = TRUE;
$handler->display->display_options['query']['options']['pure_distinct'] = TRUE;
$handler->display->display_options['exposed_form']['type'] = 'basic';
$handler->display->display_options['pager']['type'] = 'full';
$handler->display->display_options['pager']['options']['items_per_page'] = '50';
$handler->display->display_options['pager']['options']['offset'] = '0';
$handler->display->display_options['pager']['options']['id'] = '0';
$handler->display->display_options['pager']['options']['quantity'] = '9';
$handler->display->display_options['style_plugin'] = 'table';
$handler->display->display_options['style_options']['columns'] = array(
  'created' => 'created',
  'title' => 'title',
  'field_chron_basket' => 'field_chron_basket',
  'field_conversion_type' => 'field_conversion_type',
  'field_euro_basket' => 'field_euro_basket',
  'field_input_amount' => 'field_input_amount',
  'field_conversion_fee' => 'field_conversion_fee',
  'field_order_record' => 'field_order_record',
);
$handler->display->display_options['style_options']['class'] = '';
$handler->display->display_options['style_options']['default'] = 'created';
$handler->display->display_options['style_options']['info'] = array(
  'created' => array(
    'sortable' => 1,
    'default_sort_order' => 'desc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'title' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_chron_basket' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_conversion_type' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_euro_basket' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_input_amount' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_conversion_fee' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_order_record' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
);
/* Field: Content: Post date */
$handler->display->display_options['fields']['created']['id'] = 'created';
$handler->display->display_options['fields']['created']['table'] = 'node';
$handler->display->display_options['fields']['created']['field'] = 'created';
$handler->display->display_options['fields']['created']['date_format'] = 'uc_store';
$handler->display->display_options['fields']['created']['second_date_format'] = 'privatemsg_current_day';
/* Field: Content: Title */
$handler->display->display_options['fields']['title']['id'] = 'title';
$handler->display->display_options['fields']['title']['table'] = 'node';
$handler->display->display_options['fields']['title']['field'] = 'title';
$handler->display->display_options['fields']['title']['alter']['word_boundary'] = FALSE;
$handler->display->display_options['fields']['title']['alter']['ellipsis'] = FALSE;
/* Field: Content: Chron basket */
$handler->display->display_options['fields']['field_chron_basket']['id'] = 'field_chron_basket';
$handler->display->display_options['fields']['field_chron_basket']['table'] = 'field_data_field_chron_basket';
$handler->display->display_options['fields']['field_chron_basket']['field'] = 'field_chron_basket';
$handler->display->display_options['fields']['field_chron_basket']['settings'] = array(
  'thousand_separator' => '',
  'decimal_separator' => '.',
  'scale' => '2',
  'prefix_suffix' => 1,
);
/* Field: Content: Conversion type */
$handler->display->display_options['fields']['field_conversion_type']['id'] = 'field_conversion_type';
$handler->display->display_options['fields']['field_conversion_type']['table'] = 'field_data_field_conversion_type';
$handler->display->display_options['fields']['field_conversion_type']['field'] = 'field_conversion_type';
/* Field: Content: Euro basket */
$handler->display->display_options['fields']['field_euro_basket']['id'] = 'field_euro_basket';
$handler->display->display_options['fields']['field_euro_basket']['table'] = 'field_data_field_euro_basket';
$handler->display->display_options['fields']['field_euro_basket']['field'] = 'field_euro_basket';
$handler->display->display_options['fields']['field_euro_basket']['settings'] = array(
  'thousand_separator' => '',
  'decimal_separator' => '.',
  'scale' => '2',
  'prefix_suffix' => 1,
);
/* Field: Content: Paid amount */
$handler->display->display_options['fields']['field_input_amount']['id'] = 'field_input_amount';
$handler->display->display_options['fields']['field_input_amount']['table'] = 'field_data_field_input_amount';
$handler->display->display_options['fields']['field_input_amount']['field'] = 'field_input_amount';
$handler->display->display_options['fields']['field_input_amount']['settings'] = array(
  'thousand_separator' => '',
  'decimal_separator' => '.',
  'scale' => '2',
  'prefix_suffix' => 1,
);
/* Field: Field: Conversion fee */
$handler->display->display_options['fields']['field_conversion_fee']['id'] = 'field_conversion_fee';
$handler->display->display_options['fields']['field_conversion_fee']['table'] = 'field_data_field_conversion_fee';
$handler->display->display_options['fields']['field_conversion_fee']['field'] = 'field_conversion_fee';
$handler->display->display_options['fields']['field_conversion_fee']['settings'] = array(
  'thousand_separator' => '',
  'decimal_separator' => '.',
  'scale' => '2',
  'prefix_suffix' => 1,
);
/* Field: Content: Order record */
$handler->display->display_options['fields']['field_order_record']['id'] = 'field_order_record';
$handler->display->display_options['fields']['field_order_record']['table'] = 'field_data_field_order_record';
$handler->display->display_options['fields']['field_order_record']['field'] = 'field_order_record';
$handler->display->display_options['fields']['field_order_record']['settings'] = array(
  'bypass_access' => 0,
  'link' => 0,
);
/* Field: Content: Payment status */
$handler->display->display_options['fields']['field_payment_status']['id'] = 'field_payment_status';
$handler->display->display_options['fields']['field_payment_status']['table'] = 'field_data_field_payment_status';
$handler->display->display_options['fields']['field_payment_status']['field'] = 'field_payment_status';
/* Sort criterion: Content: Post date */
$handler->display->display_options['sorts']['created']['id'] = 'created';
$handler->display->display_options['sorts']['created']['table'] = 'node';
$handler->display->display_options['sorts']['created']['field'] = 'created';
$handler->display->display_options['sorts']['created']['order'] = 'DESC';
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
  'forex_transaction' => 'forex_transaction',
);
$handler->display->display_options['filters']['type']['group'] = 1;
/* Filter criterion: Content: Post date */
$handler->display->display_options['filters']['created']['id'] = 'created';
$handler->display->display_options['filters']['created']['table'] = 'node';
$handler->display->display_options['filters']['created']['field'] = 'created';
$handler->display->display_options['filters']['created']['operator'] = 'between';
$handler->display->display_options['filters']['created']['group'] = 1;
$handler->display->display_options['filters']['created']['exposed'] = TRUE;
$handler->display->display_options['filters']['created']['expose']['operator_id'] = 'created_op';
$handler->display->display_options['filters']['created']['expose']['label'] = 'Post date';
$handler->display->display_options['filters']['created']['expose']['operator'] = 'created_op';
$handler->display->display_options['filters']['created']['expose']['identifier'] = 'created';
$handler->display->display_options['filters']['created']['expose']['remember_roles'] = array(
  2 => '2',
);
/* Filter criterion: Content: Conversion type (field_conversion_type) */
$handler->display->display_options['filters']['field_conversion_type_value']['id'] = 'field_conversion_type_value';
$handler->display->display_options['filters']['field_conversion_type_value']['table'] = 'field_data_field_conversion_type';
$handler->display->display_options['filters']['field_conversion_type_value']['field'] = 'field_conversion_type_value';
$handler->display->display_options['filters']['field_conversion_type_value']['value'] = array(
  'euros_to_chrons' => 'euros_to_chrons',
);
$handler->display->display_options['filters']['field_conversion_type_value']['group'] = 1;
$handler->display->display_options['filters']['field_conversion_type_value']['exposed'] = TRUE;
$handler->display->display_options['filters']['field_conversion_type_value']['expose']['operator_id'] = 'field_conversion_type_value_op';
$handler->display->display_options['filters']['field_conversion_type_value']['expose']['label'] = 'Type';
$handler->display->display_options['filters']['field_conversion_type_value']['expose']['operator'] = 'field_conversion_type_value_op';
$handler->display->display_options['filters']['field_conversion_type_value']['expose']['identifier'] = 'field_conversion_type_value';
$handler->display->display_options['filters']['field_conversion_type_value']['expose']['remember_roles'] = array(
  2 => '2',
);

/* Display: Page */
$handler = $view->new_display('page', 'Page', 'page');
$handler->display->display_options['path'] = 'forex-transactions';
$translatables['forex_transactions'] = array(
  t('Master'),
  t('Forex transactions'),
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
  t('Post date'),
  t('Title'),
  t('Chron basket'),
  t('Conversion type'),
  t('Euro basket'),
  t('Paid amount'),
  t('Conversion fee'),
  t('Order record'),
  t('Payment status'),
  t('Type'),
  t('Page'),
);

