<?php

/**
 * @file
 * Default view: chron_holders_list
 */

$view = new view();
$view->name = 'chron_holders_list';
$view->description = '';
$view->tag = 'default';
$view->base_table = 'users';
$view->human_name = 'Chron holders list';
$view->core = 7;
$view->api_version = '3.0';
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

/* Display: Master */
$handler = $view->new_display('default', 'Master', 'default');
$handler->display->display_options['title'] = 'Chron holders list';
$handler->display->display_options['use_more_always'] = FALSE;
$handler->display->display_options['access']['type'] = 'perm';
$handler->display->display_options['access']['perm'] = 'access user profiles';
$handler->display->display_options['cache']['type'] = 'none';
$handler->display->display_options['query']['type'] = 'views_query';
$handler->display->display_options['exposed_form']['type'] = 'basic';
$handler->display->display_options['pager']['type'] = 'full';
$handler->display->display_options['pager']['options']['items_per_page'] = '10';
$handler->display->display_options['pager']['options']['offset'] = '0';
$handler->display->display_options['pager']['options']['id'] = '0';
$handler->display->display_options['pager']['options']['quantity'] = '3';
$handler->display->display_options['pager']['options']['tags']['first'] = '«';
$handler->display->display_options['pager']['options']['tags']['previous'] = '‹';
$handler->display->display_options['pager']['options']['tags']['next'] = '›';
$handler->display->display_options['pager']['options']['tags']['last'] = '»';
$handler->display->display_options['style_plugin'] = 'table';
$handler->display->display_options['style_options']['columns'] = array(
  'name' => 'name',
  'field_actually_available_chrons' => 'field_actually_available_chrons',
  'field_chrons_balance' => 'field_chrons_balance',
);
$handler->display->display_options['style_options']['class'] = '';
$handler->display->display_options['style_options']['default'] = '-1';
$handler->display->display_options['style_options']['info'] = array(
  'name' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_actually_available_chrons' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_chrons_balance' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
);
/* Field: User: Name */
$handler->display->display_options['fields']['name']['id'] = 'name';
$handler->display->display_options['fields']['name']['table'] = 'users';
$handler->display->display_options['fields']['name']['field'] = 'name';
$handler->display->display_options['fields']['name']['label'] = 'User';
$handler->display->display_options['fields']['name']['alter']['word_boundary'] = FALSE;
$handler->display->display_options['fields']['name']['alter']['ellipsis'] = FALSE;
/* Field: User: Chrons balance */
$handler->display->display_options['fields']['field_chrons_balance']['id'] = 'field_chrons_balance';
$handler->display->display_options['fields']['field_chrons_balance']['table'] = 'field_data_field_chrons_balance';
$handler->display->display_options['fields']['field_chrons_balance']['field'] = 'field_chrons_balance';
$handler->display->display_options['fields']['field_chrons_balance']['settings'] = array(
  'thousand_separator' => '',
  'decimal_separator' => '.',
  'scale' => '2',
  'prefix_suffix' => 1,
);
/* Sort criterion: User: Chrons balance (field_chrons_balance) */
$handler->display->display_options['sorts']['field_chrons_balance_value']['id'] = 'field_chrons_balance_value';
$handler->display->display_options['sorts']['field_chrons_balance_value']['table'] = 'field_data_field_chrons_balance';
$handler->display->display_options['sorts']['field_chrons_balance_value']['field'] = 'field_chrons_balance_value';
$handler->display->display_options['sorts']['field_chrons_balance_value']['order'] = 'DESC';
/* Filter criterion: User: Active status */
$handler->display->display_options['filters']['status']['id'] = 'status';
$handler->display->display_options['filters']['status']['table'] = 'users';
$handler->display->display_options['filters']['status']['field'] = 'status';
$handler->display->display_options['filters']['status']['value'] = '1';
$handler->display->display_options['filters']['status']['group'] = 1;
$handler->display->display_options['filters']['status']['expose']['operator'] = FALSE;

/* Display: Page */
$handler = $view->new_display('page', 'Page', 'page');
$handler->display->display_options['path'] = 'chron-holders-list';
$translatables['chron_holders_list'] = array(
  t('Master'),
  t('Chron holders list'),
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
  t('User'),
  t('Chrons balance'),
  t('Page'),
);

