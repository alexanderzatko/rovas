<?php

/**
 * @file
 * Default view: freemap_countries
 */

$view = new view();
$view->name = 'freemap_countries';
$view->description = '';
$view->tag = 'default';
$view->base_table = 'commerce_order';
$view->human_name = 'freemap countries';
$view->core = 7;
$view->api_version = '3.0';
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

/* Display: Master */
$handler = $view->new_display('default', 'Master', 'default');
$handler->display->display_options['use_more_always'] = FALSE;
$handler->display->display_options['group_by'] = TRUE;
$handler->display->display_options['access']['type'] = 'none';
$handler->display->display_options['cache']['type'] = 'none';
$handler->display->display_options['query']['type'] = 'views_query';
$handler->display->display_options['exposed_form']['type'] = 'basic';
$handler->display->display_options['pager']['type'] = 'none';
$handler->display->display_options['pager']['options']['offset'] = '0';
$handler->display->display_options['style_plugin'] = 'table';
$handler->display->display_options['style_options']['columns'] = array(
  'commerce_customer_address_country' => 'commerce_customer_address_country',
  'order_id' => 'order_id',
);
$handler->display->display_options['style_options']['class'] = '';
$handler->display->display_options['style_options']['default'] = '-1';
$handler->display->display_options['style_options']['info'] = array(
  'commerce_customer_address_country' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'order_id' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
);
/* Relationship: Commerce Order: Referenced customer profile */
$handler->display->display_options['relationships']['commerce_customer_billing_profile_id']['id'] = 'commerce_customer_billing_profile_id';
$handler->display->display_options['relationships']['commerce_customer_billing_profile_id']['table'] = 'field_data_commerce_customer_billing';
$handler->display->display_options['relationships']['commerce_customer_billing_profile_id']['field'] = 'commerce_customer_billing_profile_id';
$handler->display->display_options['relationships']['commerce_customer_billing_profile_id']['label'] = 'billing';
/* Field: Commerce Customer profile: Address - Country */
$handler->display->display_options['fields']['commerce_customer_address_country']['id'] = 'commerce_customer_address_country';
$handler->display->display_options['fields']['commerce_customer_address_country']['table'] = 'field_data_commerce_customer_address';
$handler->display->display_options['fields']['commerce_customer_address_country']['field'] = 'commerce_customer_address_country';
$handler->display->display_options['fields']['commerce_customer_address_country']['relationship'] = 'commerce_customer_billing_profile_id';
$handler->display->display_options['fields']['commerce_customer_address_country']['label'] = '';
$handler->display->display_options['fields']['commerce_customer_address_country']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['commerce_customer_address_country']['display_name'] = 1;
/* Field: COUNT(Commerce Order: Order ID) */
$handler->display->display_options['fields']['order_id']['id'] = 'order_id';
$handler->display->display_options['fields']['order_id']['table'] = 'commerce_order';
$handler->display->display_options['fields']['order_id']['field'] = 'order_id';
$handler->display->display_options['fields']['order_id']['group_type'] = 'count';
$handler->display->display_options['fields']['order_id']['label'] = '';
$handler->display->display_options['fields']['order_id']['element_label_colon'] = FALSE;
/* Sort criterion: COUNT(Commerce Order: Order ID) */
$handler->display->display_options['sorts']['order_id']['id'] = 'order_id';
$handler->display->display_options['sorts']['order_id']['table'] = 'commerce_order';
$handler->display->display_options['sorts']['order_id']['field'] = 'order_id';
$handler->display->display_options['sorts']['order_id']['group_type'] = 'count';
$handler->display->display_options['sorts']['order_id']['order'] = 'DESC';
/* Filter criterion: Commerce Order: Order type */
$handler->display->display_options['filters']['type']['id'] = 'type';
$handler->display->display_options['filters']['type']['table'] = 'commerce_order';
$handler->display->display_options['filters']['type']['field'] = 'type';
$handler->display->display_options['filters']['type']['value'] = array(
  'commerce_order' => 'commerce_order',
);
/* Filter criterion: Commerce Order: Order state */
$handler->display->display_options['filters']['state']['id'] = 'state';
$handler->display->display_options['filters']['state']['table'] = 'commerce_order';
$handler->display->display_options['filters']['state']['field'] = 'state';
$handler->display->display_options['filters']['state']['value'] = array(
  'completed' => 'completed',
);
/* Filter criterion: Commerce Order: Payment recipient (field_payment_recipient) */
$handler->display->display_options['filters']['field_payment_recipient_value']['id'] = 'field_payment_recipient_value';
$handler->display->display_options['filters']['field_payment_recipient_value']['table'] = 'field_data_field_payment_recipient';
$handler->display->display_options['filters']['field_payment_recipient_value']['field'] = 'field_payment_recipient_value';
$handler->display->display_options['filters']['field_payment_recipient_value']['value']['value'] = '35384';

/* Display: Page */
$handler = $view->new_display('page', 'Page', 'page_1');
$handler->display->display_options['path'] = 'freemap-countries';
$translatables['freemap_countries'] = array(
  t('Master'),
  t('more'),
  t('Apply'),
  t('Reset'),
  t('Sort by'),
  t('Asc'),
  t('Desc'),
  t('billing'),
  t('Page'),
);

