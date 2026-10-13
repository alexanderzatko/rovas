<?php

/**
 * @file
 * Default view: uc_orders_user
 */

$view = new view();
$view->name = 'uc_orders_user';
$view->description = '';
$view->tag = 'Ubercart';
$view->base_table = 'uc_orders';
$view->human_name = 'uc_orders_user';
$view->core = 7;
$view->api_version = '3.0';
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

/* Display: Master */
$handler = $view->new_display('default', 'Master', 'default');
$handler->display->display_options['use_more_always'] = FALSE;
$handler->display->display_options['access']['type'] = 'perm';
$handler->display->display_options['access']['perm'] = 'view own orders';
$handler->display->display_options['cache']['type'] = 'none';
$handler->display->display_options['query']['type'] = 'views_query';
$handler->display->display_options['query']['options']['query_comment'] = FALSE;
$handler->display->display_options['exposed_form']['type'] = 'basic';
$handler->display->display_options['pager']['type'] = 'full';
$handler->display->display_options['pager']['options']['items_per_page'] = '10';
$handler->display->display_options['pager']['options']['offset'] = '0';
$handler->display->display_options['pager']['options']['id'] = '0';
$handler->display->display_options['pager']['options']['quantity'] = '9';
$handler->display->display_options['pager']['options']['tags']['first'] = '«';
$handler->display->display_options['pager']['options']['tags']['previous'] = '‹';
$handler->display->display_options['pager']['options']['tags']['next'] = '›';
$handler->display->display_options['pager']['options']['tags']['last'] = '»';
$handler->display->display_options['style_plugin'] = 'table';
$handler->display->display_options['style_options']['columns'] = array(
  'created' => 'created',
  'order_id' => 'order_id',
  'actions' => 'order_id',
  'order_status' => 'order_status',
  'product_count' => 'product_count',
  'order_total' => 'order_total',
);
$handler->display->display_options['style_options']['default'] = 'created';
$handler->display->display_options['style_options']['info'] = array(
  'created' => array(
    'sortable' => 1,
    'default_sort_order' => 'desc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'order_id' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => ' ',
    'empty_column' => 0,
  ),
  'actions' => array(
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'order_status' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'product_count' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'order_total' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
);
/* Header: Global: PHP */
$handler->display->display_options['header']['php']['id'] = 'php';
$handler->display->display_options['header']['php']['table'] = 'views';
$handler->display->display_options['header']['php']['field'] = 'php';
$handler->display->display_options['header']['php']['empty'] = TRUE;
$handler->display->display_options['header']['php']['php_output'] = '<div class="usr-pane-head">
<?php  echo t(\'Order history\'); ?>
</div>';
/* No results behavior: Global: Text area */
$handler->display->display_options['empty']['area']['id'] = 'area';
$handler->display->display_options['empty']['area']['table'] = 'views';
$handler->display->display_options['empty']['area']['field'] = 'area';
$handler->display->display_options['empty']['area']['empty'] = TRUE;
$handler->display->display_options['empty']['area']['content'] = 'You have submitted no orders to date';
$handler->display->display_options['empty']['area']['format'] = 'filtered_html';
/* Field: Broken/missing handler */
$handler->display->display_options['fields']['created']['id'] = 'created';
$handler->display->display_options['fields']['created']['table'] = 'uc_orders';
$handler->display->display_options['fields']['created']['field'] = 'created';
$handler->display->display_options['fields']['created']['label'] = 'Date';
/* Field: Broken/missing handler */
$handler->display->display_options['fields']['order_id']['id'] = 'order_id';
$handler->display->display_options['fields']['order_id']['table'] = 'uc_orders';
$handler->display->display_options['fields']['order_id']['field'] = 'order_id';
$handler->display->display_options['fields']['order_id']['label'] = 'Order #';
$handler->display->display_options['fields']['order_id']['element_label_colon'] = FALSE;
/* Field: Broken/missing handler */
$handler->display->display_options['fields']['actions']['id'] = 'actions';
$handler->display->display_options['fields']['actions']['table'] = 'uc_orders';
$handler->display->display_options['fields']['actions']['field'] = 'actions';
$handler->display->display_options['fields']['actions']['exclude'] = TRUE;
/* Field: Broken/missing handler */
$handler->display->display_options['fields']['order_status']['id'] = 'order_status';
$handler->display->display_options['fields']['order_status']['table'] = 'uc_orders';
$handler->display->display_options['fields']['order_status']['field'] = 'order_status';
$handler->display->display_options['fields']['order_status']['label'] = 'Status';
/* Field: Broken/missing handler */
$handler->display->display_options['fields']['product_count']['id'] = 'product_count';
$handler->display->display_options['fields']['product_count']['table'] = 'uc_orders';
$handler->display->display_options['fields']['product_count']['field'] = 'product_count';
$handler->display->display_options['fields']['product_count']['label'] = 'Purchased';
/* Field: Broken/missing handler */
$handler->display->display_options['fields']['order_total']['id'] = 'order_total';
$handler->display->display_options['fields']['order_total']['table'] = 'uc_orders';
$handler->display->display_options['fields']['order_total']['field'] = 'order_total';
$handler->display->display_options['fields']['order_total']['label'] = 'Paid';
/* Contextual filter: User: Uid */
$handler->display->display_options['arguments']['uid']['id'] = 'uid';
$handler->display->display_options['arguments']['uid']['table'] = 'users';
$handler->display->display_options['arguments']['uid']['field'] = 'uid';
$handler->display->display_options['arguments']['uid']['default_action'] = 'not found';
$handler->display->display_options['arguments']['uid']['exception']['value'] = '';
$handler->display->display_options['arguments']['uid']['default_argument_type'] = 'fixed';
$handler->display->display_options['arguments']['uid']['summary']['number_of_records'] = '0';
$handler->display->display_options['arguments']['uid']['summary']['format'] = 'default_summary';
$handler->display->display_options['arguments']['uid']['summary_options']['items_per_page'] = '25';
$handler->display->display_options['arguments']['uid']['specify_validation'] = TRUE;
/* Filter criterion: Broken/missing handler */
$handler->display->display_options['filters']['order_status']['id'] = 'order_status';
$handler->display->display_options['filters']['order_status']['table'] = 'uc_orders';
$handler->display->display_options['filters']['order_status']['field'] = 'order_status';
$handler->display->display_options['filters']['order_status']['value'] = array(
  '_active' => '_active',
);

/* Display: Page */
$handler = $view->new_display('page', 'Page', 'page');
$handler->display->display_options['path'] = 'user/%/orders';
$handler->display->display_options['menu']['type'] = 'tab';
$handler->display->display_options['menu']['title'] = 'Orders';
$handler->display->display_options['menu']['description'] = 'View your order history.';
$handler->display->display_options['menu']['weight'] = '0';
$handler->display->display_options['menu']['name'] = 'user-menu';
$translatables['uc_orders_user'] = array(
  t('Master'),
  t('more'),
  t('Apply'),
  t('Reset'),
  t('Sort by'),
  t('Asc'),
  t('Desc'),
  t('Items per page'),
  t('- All -'),
  t('Offset'),
  t('«'),
  t('‹'),
  t('›'),
  t('»'),
  t('You have submitted no orders to date'),
  t('Date'),
  t('Order #'),
  t('Broken handler uc_orders.actions'),
  t('Status'),
  t('Purchased'),
  t('Paid'),
  t('All'),
  t('Page'),
);

