<?php

/**
 * @file
 * Default view: supporters_countries
 */

$view = new view();
$view->name = 'supporters_countries';
$view->description = '';
$view->tag = 'default';
$view->base_table = 'commerce_order';
$view->human_name = 'supporters countries';
$view->core = 7;
$view->api_version = '3.0';
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

/* Display: Master */
$handler = $view->new_display('default', 'Master', 'default');
$handler->display->display_options['title'] = 'supporters countries';
$handler->display->display_options['use_more_always'] = FALSE;
$handler->display->display_options['group_by'] = TRUE;
$handler->display->display_options['access']['type'] = 'role';
$handler->display->display_options['access']['role'] = array(
  2 => '2',
);
$handler->display->display_options['cache']['type'] = 'none';
$handler->display->display_options['query']['type'] = 'views_query';
$handler->display->display_options['exposed_form']['type'] = 'basic';
$handler->display->display_options['pager']['type'] = 'none';
$handler->display->display_options['pager']['options']['offset'] = '0';
$handler->display->display_options['style_plugin'] = 'tablesummarized';
$handler->display->display_options['style_options']['columns'] = array(
  'order_id' => 'order_id',
  'commerce_customer_address_country' => 'commerce_customer_address_country',
);
$handler->display->display_options['style_options']['class'] = '';
$handler->display->display_options['style_options']['default'] = 'order_id';
$handler->display->display_options['style_options']['info'] = array(
  'order_id' => array(
    'sortable' => 1,
    'default_sort_order' => 'desc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
    'summarize' => 'total',
  ),
  'commerce_customer_address_country' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
    'summarize' => 'none',
  ),
);
/* Relationship: Commerce Order: Referenced customer profile */
$handler->display->display_options['relationships']['commerce_customer_billing_profile_id']['id'] = 'commerce_customer_billing_profile_id';
$handler->display->display_options['relationships']['commerce_customer_billing_profile_id']['table'] = 'field_data_commerce_customer_billing';
$handler->display->display_options['relationships']['commerce_customer_billing_profile_id']['field'] = 'commerce_customer_billing_profile_id';
$handler->display->display_options['relationships']['commerce_customer_billing_profile_id']['label'] = 'customer';
/* Field: COUNT(Commerce Order: Order ID) */
$handler->display->display_options['fields']['order_id']['id'] = 'order_id';
$handler->display->display_options['fields']['order_id']['table'] = 'commerce_order';
$handler->display->display_options['fields']['order_id']['field'] = 'order_id';
$handler->display->display_options['fields']['order_id']['group_type'] = 'count';
$handler->display->display_options['fields']['order_id']['label'] = 'Count';
/* Field: Commerce Customer profile: Address - Country */
$handler->display->display_options['fields']['commerce_customer_address_country']['id'] = 'commerce_customer_address_country';
$handler->display->display_options['fields']['commerce_customer_address_country']['table'] = 'field_data_commerce_customer_address';
$handler->display->display_options['fields']['commerce_customer_address_country']['field'] = 'commerce_customer_address_country';
$handler->display->display_options['fields']['commerce_customer_address_country']['relationship'] = 'commerce_customer_billing_profile_id';
$handler->display->display_options['fields']['commerce_customer_address_country']['label'] = 'Country';
$handler->display->display_options['fields']['commerce_customer_address_country']['display_name'] = 1;
/* Sort criterion: COUNT(Commerce Order: Order ID) */
$handler->display->display_options['sorts']['order_id']['id'] = 'order_id';
$handler->display->display_options['sorts']['order_id']['table'] = 'commerce_order';
$handler->display->display_options['sorts']['order_id']['field'] = 'order_id';
$handler->display->display_options['sorts']['order_id']['group_type'] = 'count';
$handler->display->display_options['sorts']['order_id']['order'] = 'DESC';
/* Filter criterion: Commerce Customer profile: Address - Country */
$handler->display->display_options['filters']['commerce_customer_address_country']['id'] = 'commerce_customer_address_country';
$handler->display->display_options['filters']['commerce_customer_address_country']['table'] = 'field_data_commerce_customer_address';
$handler->display->display_options['filters']['commerce_customer_address_country']['field'] = 'commerce_customer_address_country';
$handler->display->display_options['filters']['commerce_customer_address_country']['relationship'] = 'commerce_customer_billing_profile_id';
$handler->display->display_options['filters']['commerce_customer_address_country']['operator'] = 'not empty';

/* Display: Page */
$handler = $view->new_display('page', 'Page', 'page');
$handler->display->display_options['path'] = 'supporters-countries';
$translatables['supporters_countries'] = array(
  t('Master'),
  t('supporters countries'),
  t('more'),
  t('Apply'),
  t('Reset'),
  t('Sort by'),
  t('Asc'),
  t('Desc'),
  t('customer'),
  t('Count'),
  t('Country'),
  t('Page'),
);

