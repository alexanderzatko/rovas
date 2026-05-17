<?php

/**
 * @file
 * Default Views for rovas_forex.
 */

/**
 * Implements hook_views_default_views().
 */
function rovas_forex_views_default_views() {
  $export = array();

  $view = new view();
  $view->name = 'forex_purchases';
  $view->description = '';
  $view->tag = 'default';
  $view->base_table = 'commerce_order';
  $view->human_name = 'forex purchases';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE;

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Project purchases';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'role';
  $handler->display->display_options['access']['role'] = array(
    11 => '11',
  );
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '20';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['pager']['options']['id'] = '0';
  $handler->display->display_options['pager']['options']['quantity'] = '9';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'order_id' => 'order_id',
    'created' => 'created',
    'field_buyer_email' => 'field_buyer_email',
    'php' => 'php',
    'commerce_order_total' => 'commerce_order_total',
    'php_1' => 'php_1',
    'commerce_customer_billing' => 'commerce_customer_billing',
  );
  $handler->display->display_options['style_options']['class'] = '';
  $handler->display->display_options['style_options']['default'] = 'created';
  $handler->display->display_options['style_options']['info'] = array(
    'order_id' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'created' => array(
      'sortable' => 1,
      'default_sort_order' => 'desc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'field_buyer_email' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'php' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'commerce_order_total' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'php_1' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'commerce_customer_billing' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
  /* Field: Commerce Order: Order ID */
  $handler->display->display_options['fields']['order_id']['id'] = 'order_id';
  $handler->display->display_options['fields']['order_id']['table'] = 'commerce_order';
  $handler->display->display_options['fields']['order_id']['field'] = 'order_id';
  $handler->display->display_options['fields']['order_id']['alter']['alter_text'] = TRUE;
  $handler->display->display_options['fields']['order_id']['alter']['text'] = '<a href="/admin/commerce/orders/[order_id]">[order_id]</a>';
  $handler->display->display_options['fields']['order_id']['link_to_order'] = 'customer';
  /* Field: Commerce Order: Created date */
  $handler->display->display_options['fields']['created']['id'] = 'created';
  $handler->display->display_options['fields']['created']['table'] = 'commerce_order';
  $handler->display->display_options['fields']['created']['field'] = 'created';
  $handler->display->display_options['fields']['created']['label'] = 'Created';
  $handler->display->display_options['fields']['created']['date_format'] = 'rovas_short';
  $handler->display->display_options['fields']['created']['custom_date_format'] = 'dm';
  $handler->display->display_options['fields']['created']['second_date_format'] = 'privatemsg_current_day';
  /* Field: Commerce Order: Buyer email */
  $handler->display->display_options['fields']['field_buyer_email']['id'] = 'field_buyer_email';
  $handler->display->display_options['fields']['field_buyer_email']['table'] = 'field_data_field_buyer_email';
  $handler->display->display_options['fields']['field_buyer_email']['field'] = 'field_buyer_email';
  $handler->display->display_options['fields']['field_buyer_email']['label'] = 'Email';
  /* Field: Commerce Order: Billing information */
  $handler->display->display_options['fields']['commerce_customer_billing']['id'] = 'commerce_customer_billing';
  $handler->display->display_options['fields']['commerce_customer_billing']['table'] = 'field_data_commerce_customer_billing';
  $handler->display->display_options['fields']['commerce_customer_billing']['field'] = 'commerce_customer_billing';
  /* Field: Global: PHP */
  $handler->display->display_options['fields']['php']['id'] = 'php';
  $handler->display->display_options['fields']['php']['table'] = 'views';
  $handler->display->display_options['fields']['php']['field'] = 'php';
  $handler->display->display_options['fields']['php']['label'] = 'Product';
  $handler->display->display_options['fields']['php']['use_php_setup'] = 0;
  $handler->display->display_options['fields']['php']['php_output'] = '<?php
$order = commerce_order_load($data->order_id);

$product_name = \'\';
if (!empty($order->data[\'stripe_pi\'][\'payment_intent\'][\'metadata\'][\'product_name\'])) {
  $product_name = $order->data[\'stripe_pi\'][\'payment_intent\'][\'metadata\'][\'product_name\'];
}
elseif (!empty($order->data[\'bank_pi\'][\'payment_intent\'][\'metadata\'][\'product_name\'])) {
  $product_name = $order->data[\'bank_pi\'][\'payment_intent\'][\'metadata\'][\'product_name\'];
}

echo $product_name;
?>';
  $handler->display->display_options['fields']['php']['use_php_click_sortable'] = '0';
  $handler->display->display_options['fields']['php']['php_click_sortable'] = '';
  /* Field: Commerce Order: Order total */
  $handler->display->display_options['fields']['commerce_order_total']['id'] = 'commerce_order_total';
  $handler->display->display_options['fields']['commerce_order_total']['table'] = 'field_data_commerce_order_total';
  $handler->display->display_options['fields']['commerce_order_total']['field'] = 'commerce_order_total';
  $handler->display->display_options['fields']['commerce_order_total']['group_type'] = 'sum';
  $handler->display->display_options['fields']['commerce_order_total']['label'] = 'total';
  $handler->display->display_options['fields']['commerce_order_total']['click_sort_column'] = 'amount';
  $handler->display->display_options['fields']['commerce_order_total']['settings'] = array(
    'calculation' => FALSE,
  );
  $handler->display->display_options['fields']['commerce_order_total']['group_column'] = 'entity_id';
  /* Field: Global: PHP */
  $handler->display->display_options['fields']['php_1']['id'] = 'php_1';
  $handler->display->display_options['fields']['php_1']['table'] = 'views';
  $handler->display->display_options['fields']['php_1']['field'] = 'php';
  $handler->display->display_options['fields']['php_1']['label'] = 'Processor fees';
  $handler->display->display_options['fields']['php_1']['use_php_setup'] = 0;
  $handler->display->display_options['fields']['php_1']['php_output'] = '<?php
echo isset($data->_field_data[\'order_id\'][\'entity\']->data[\'p_processor_balance_txn\'][\'fee\']) ? $data->_field_data[\'order_id\'][\'entity\']->data[\'p_processor_balance_txn\'][\'fee\']: 0;
?>';
  $handler->display->display_options['fields']['php_1']['use_php_click_sortable'] = '0';
  $handler->display->display_options['fields']['php_1']['php_click_sortable'] = '';
  /* Sort criterion: Commerce Order: Created date */
  $handler->display->display_options['sorts']['created']['id'] = 'created';
  $handler->display->display_options['sorts']['created']['table'] = 'commerce_order';
  $handler->display->display_options['sorts']['created']['field'] = 'created';
  $handler->display->display_options['sorts']['created']['order'] = 'DESC';
  /* Contextual filter: Commerce Order: Payment recipient (field_payment_recipient) */
  $handler->display->display_options['arguments']['field_payment_recipient_value']['id'] = 'field_payment_recipient_value';
  $handler->display->display_options['arguments']['field_payment_recipient_value']['table'] = 'field_data_field_payment_recipient';
  $handler->display->display_options['arguments']['field_payment_recipient_value']['field'] = 'field_payment_recipient_value';
  $handler->display->display_options['arguments']['field_payment_recipient_value']['default_action'] = 'default';
  $handler->display->display_options['arguments']['field_payment_recipient_value']['default_argument_type'] = 'fixed';
  $handler->display->display_options['arguments']['field_payment_recipient_value']['default_argument_options']['argument'] = '35384';
  $handler->display->display_options['arguments']['field_payment_recipient_value']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['field_payment_recipient_value']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['field_payment_recipient_value']['summary_options']['items_per_page'] = '25';
  /* Filter criterion: Commerce Order: Order type */
  $handler->display->display_options['filters']['type']['id'] = 'type';
  $handler->display->display_options['filters']['type']['table'] = 'commerce_order';
  $handler->display->display_options['filters']['type']['field'] = 'type';
  $handler->display->display_options['filters']['type']['value'] = array(
    'commerce_order' => 'commerce_order',
  );
  $handler->display->display_options['filters']['type']['group'] = 1;
  /* Filter criterion: Commerce Order: Created date */
  $handler->display->display_options['filters']['created']['id'] = 'created';
  $handler->display->display_options['filters']['created']['table'] = 'commerce_order';
  $handler->display->display_options['filters']['created']['field'] = 'created';
  $handler->display->display_options['filters']['created']['operator'] = 'between';
  $handler->display->display_options['filters']['created']['value']['min'] = '';
  $handler->display->display_options['filters']['created']['value']['max'] = '';
  $handler->display->display_options['filters']['created']['value']['type'] = 'date';
  $handler->display->display_options['filters']['created']['group'] = 1;
  $handler->display->display_options['filters']['created']['exposed'] = TRUE;
  $handler->display->display_options['filters']['created']['expose']['operator_id'] = 'created_op';
  $handler->display->display_options['filters']['created']['expose']['label'] = 'Date range';
  $handler->display->display_options['filters']['created']['expose']['description'] = 'Select start and end dates, then apply or export.';
  $handler->display->display_options['filters']['created']['expose']['use_operator'] = FALSE;
  $handler->display->display_options['filters']['created']['expose']['operator'] = 'created_op';
  $handler->display->display_options['filters']['created']['expose']['identifier'] = 'created';
  $handler->display->display_options['filters']['created']['expose']['remember_roles'] = array(
    2 => '2',
  );
  /* Filter criterion: Commerce Order: E-mail */
  $handler->display->display_options['filters']['mail']['id'] = 'mail';
  $handler->display->display_options['filters']['mail']['table'] = 'commerce_order';
  $handler->display->display_options['filters']['mail']['field'] = 'mail';
  $handler->display->display_options['filters']['mail']['operator'] = 'longerthan';
  $handler->display->display_options['filters']['mail']['value'] = '0';
  $handler->display->display_options['filters']['mail']['group'] = 1;
  /* Filter criterion: Commerce Order: Order status */
  $handler->display->display_options['filters']['status']['id'] = 'status';
  $handler->display->display_options['filters']['status']['table'] = 'commerce_order';
  $handler->display->display_options['filters']['status']['field'] = 'status';
  $handler->display->display_options['filters']['status']['value'] = array(
    'completed' => 'completed',
  );

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page');
  $handler->display->display_options['path'] = 'forex-purchases';

  /* Display: Data export */
  $handler = $view->new_display('views_data_export', 'Data export', 'data_export');
  $handler->display->display_options['pager']['type'] = 'none';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['style_plugin'] = 'views_data_export_csv';
  $handler->display->display_options['style_options']['separator'] = ',';
  $handler->display->display_options['style_options']['quote'] = 1;
  $handler->display->display_options['style_options']['trim'] = 1;
  $handler->display->display_options['style_options']['header'] = 1;
  $handler->display->display_options['style_options']['provide_file'] = 1;
  $handler->display->display_options['style_options']['filename'] = 'forex-purchases.csv';
  $handler->display->display_options['style_options']['parent_sort'] = 1;
  $handler->display->display_options['style_options']['attach_text'] = 'Export CSV';
  $handler->display->display_options['path'] = 'forex-purchases/export';
  $handler->display->display_options['displays'] = array(
    'page' => 'page',
  );
  $handler->display->display_options['use_batch'] = 'batch';
  $handler->display->display_options['fields']['commerce_customer_billing']['exclude'] = TRUE;
  $handler->display->display_options['fields']['order_id']['alter']['alter_text'] = FALSE;
  $handler->display->display_options['fields']['order_id']['link_to_order'] = FALSE;

  $translatables['forex_purchases'] = array(
    t('Master'),
    t('Project purchases'),
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
    t('Order ID'),
    t('<a href="/admin/commerce/orders/[order_id]">[order_id]</a>'),
    t('Created'),
    t('Email'),
    t('Billing information'),
    t('Product'),
    t('total'),
    t('Processor fees'),
    t('All'),
    t('Date range'),
    t('Select start and end dates, then apply or export.'),
    t('Page'),
    t('Data export'),
    t('Export CSV'),
  );

  $export['forex_purchases'] = $view;

  return $export;
}
