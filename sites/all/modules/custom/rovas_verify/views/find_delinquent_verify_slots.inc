<?php

/**
 * @file
 * Default view: find_delinquent_verify_slots
 */

$view = new view();
$view->name = 'find_delinquent_verify_slots';
$view->description = '';
$view->tag = 'default';
$view->base_table = 'node';
$view->human_name = 'Find delinquent verify slots';
$view->core = 7;
$view->api_version = '3.0';
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

/* Display: Master */
$handler = $view->new_display('default', 'Master', 'default');
$handler->display->display_options['use_more_always'] = FALSE;
$handler->display->display_options['access']['type'] = 'perm';
$handler->display->display_options['cache']['type'] = 'none';
$handler->display->display_options['query']['type'] = 'views_query';
$handler->display->display_options['exposed_form']['type'] = 'basic';
$handler->display->display_options['pager']['type'] = 'some';
$handler->display->display_options['pager']['options']['items_per_page'] = '50';
$handler->display->display_options['pager']['options']['offset'] = '0';
$handler->display->display_options['style_plugin'] = 'default';
$handler->display->display_options['row_plugin'] = 'fields';
/* Field: Content: Title */
$handler->display->display_options['fields']['title']['id'] = 'title';
$handler->display->display_options['fields']['title']['table'] = 'node';
$handler->display->display_options['fields']['title']['field'] = 'title';
$handler->display->display_options['fields']['title']['label'] = '';
$handler->display->display_options['fields']['title']['alter']['word_boundary'] = FALSE;
$handler->display->display_options['fields']['title']['alter']['ellipsis'] = FALSE;
/* Field: Bulk operations: Content */
$handler->display->display_options['fields']['views_bulk_operations']['id'] = 'views_bulk_operations';
$handler->display->display_options['fields']['views_bulk_operations']['table'] = 'views_entity_node';
$handler->display->display_options['fields']['views_bulk_operations']['field'] = 'views_bulk_operations';
$handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['display_type'] = '0';
$handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['enable_select_all_pages'] = 1;
$handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['row_clickable'] = 1;
$handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['force_single'] = 0;
$handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['entity_load_capacity'] = '10';
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
  'verification_slot' => 'verification_slot',
);
/* Filter criterion: Content: Verify date (field_verify_date) */
$handler->display->display_options['filters']['field_verify_date_value']['id'] = 'field_verify_date_value';
$handler->display->display_options['filters']['field_verify_date_value']['table'] = 'field_data_field_verify_date';
$handler->display->display_options['filters']['field_verify_date_value']['field'] = 'field_verify_date_value';
$handler->display->display_options['filters']['field_verify_date_value']['operator'] = '<';
$handler->display->display_options['filters']['field_verify_date_value']['expose']['operator_id'] = 'field_verify_date_value_op';
$handler->display->display_options['filters']['field_verify_date_value']['expose']['label'] = 'Verify date (field_verify_date)';
$handler->display->display_options['filters']['field_verify_date_value']['expose']['operator'] = 'field_verify_date_value_op';
$handler->display->display_options['filters']['field_verify_date_value']['expose']['identifier'] = 'field_verify_date_value';
$handler->display->display_options['filters']['field_verify_date_value']['granularity'] = 'hour';
$handler->display->display_options['filters']['field_verify_date_value']['default_date'] = 'now';
/* Filter criterion: Content: Report to verify (field_report_to_verify) */
$handler->display->display_options['filters']['field_report_to_verify_target_id']['id'] = 'field_report_to_verify_target_id';
$handler->display->display_options['filters']['field_report_to_verify_target_id']['table'] = 'field_data_field_report_to_verify';
$handler->display->display_options['filters']['field_report_to_verify_target_id']['field'] = 'field_report_to_verify_target_id';
$handler->display->display_options['filters']['field_report_to_verify_target_id']['operator'] = 'not empty';
/* Filter criterion: Content: Declined (field_declined) */
$handler->display->display_options['filters']['field_declined_value']['id'] = 'field_declined_value';
$handler->display->display_options['filters']['field_declined_value']['table'] = 'field_data_field_declined';
$handler->display->display_options['filters']['field_declined_value']['field'] = 'field_declined_value';
$handler->display->display_options['filters']['field_declined_value']['operator'] = 'not';
$handler->display->display_options['filters']['field_declined_value']['value'] = array(
  1 => '1',
);

/* Display: Page */
$handler = $view->new_display('page', 'Page', 'page_1');
$handler->display->display_options['defaults']['access'] = FALSE;
$handler->display->display_options['access']['type'] = 'role';
$handler->display->display_options['access']['role'] = array(
  3 => '3',
);
$handler->display->display_options['defaults']['style_plugin'] = FALSE;
$handler->display->display_options['style_plugin'] = 'table';
$handler->display->display_options['style_options']['columns'] = array(
  'nid' => 'nid',
  'views_bulk_operations' => 'views_bulk_operations',
  'field_slip_owner' => 'field_slip_owner',
  'field_report_to_verify' => 'field_report_to_verify',
  'field_verify_date' => 'field_verify_date',
);
$handler->display->display_options['style_options']['class'] = '';
$handler->display->display_options['style_options']['default'] = 'nid';
$handler->display->display_options['style_options']['info'] = array(
  'nid' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'views_bulk_operations' => array(
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_slip_owner' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_report_to_verify' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_verify_date' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
);
$handler->display->display_options['defaults']['style_options'] = FALSE;
$handler->display->display_options['defaults']['row_plugin'] = FALSE;
$handler->display->display_options['defaults']['row_options'] = FALSE;
$handler->display->display_options['defaults']['fields'] = FALSE;
/* Field: Content: Nid */
$handler->display->display_options['fields']['nid']['id'] = 'nid';
$handler->display->display_options['fields']['nid']['table'] = 'node';
$handler->display->display_options['fields']['nid']['field'] = 'nid';
$handler->display->display_options['fields']['nid']['link_to_node'] = TRUE;
/* Field: Bulk operations: Content */
$handler->display->display_options['fields']['views_bulk_operations']['id'] = 'views_bulk_operations';
$handler->display->display_options['fields']['views_bulk_operations']['table'] = 'views_entity_node';
$handler->display->display_options['fields']['views_bulk_operations']['field'] = 'views_bulk_operations';
$handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['display_type'] = '0';
$handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['enable_select_all_pages'] = 1;
$handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['row_clickable'] = 1;
$handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['force_single'] = 0;
$handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['entity_load_capacity'] = '10';
/* Field: Content: Slot owner */
$handler->display->display_options['fields']['field_slip_owner']['id'] = 'field_slip_owner';
$handler->display->display_options['fields']['field_slip_owner']['table'] = 'field_data_field_slip_owner';
$handler->display->display_options['fields']['field_slip_owner']['field'] = 'field_slip_owner';
$handler->display->display_options['fields']['field_slip_owner']['settings'] = array(
  'bypass_access' => 0,
  'link' => 1,
);
/* Field: Content: Report to verify */
$handler->display->display_options['fields']['field_report_to_verify']['id'] = 'field_report_to_verify';
$handler->display->display_options['fields']['field_report_to_verify']['table'] = 'field_data_field_report_to_verify';
$handler->display->display_options['fields']['field_report_to_verify']['field'] = 'field_report_to_verify';
$handler->display->display_options['fields']['field_report_to_verify']['settings'] = array(
  'bypass_access' => 0,
  'link' => 1,
);
/* Field: Content: Verify date */
$handler->display->display_options['fields']['field_verify_date']['id'] = 'field_verify_date';
$handler->display->display_options['fields']['field_verify_date']['table'] = 'field_data_field_verify_date';
$handler->display->display_options['fields']['field_verify_date']['field'] = 'field_verify_date';
$handler->display->display_options['fields']['field_verify_date']['settings'] = array(
  'format_type' => 'long',
  'custom_date_format' => '',
  'fromto' => 'both',
  'multiple_number' => '',
  'multiple_from' => '',
  'multiple_to' => '',
  'show_remaining_days' => 0,
);
$handler->display->display_options['defaults']['filter_groups'] = FALSE;
$handler->display->display_options['defaults']['filters'] = FALSE;
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
  'verification_slot' => 'verification_slot',
);
/* Filter criterion: Content: Verify date (field_verify_date) */
$handler->display->display_options['filters']['field_verify_date_value']['id'] = 'field_verify_date_value';
$handler->display->display_options['filters']['field_verify_date_value']['table'] = 'field_data_field_verify_date';
$handler->display->display_options['filters']['field_verify_date_value']['field'] = 'field_verify_date_value';
$handler->display->display_options['filters']['field_verify_date_value']['operator'] = '<';
$handler->display->display_options['filters']['field_verify_date_value']['exposed'] = TRUE;
$handler->display->display_options['filters']['field_verify_date_value']['expose']['operator_id'] = 'field_verify_date_value_op';
$handler->display->display_options['filters']['field_verify_date_value']['expose']['label'] = 'Verify date';
$handler->display->display_options['filters']['field_verify_date_value']['expose']['operator'] = 'field_verify_date_value_op';
$handler->display->display_options['filters']['field_verify_date_value']['expose']['identifier'] = 'field_verify_date_value';
$handler->display->display_options['filters']['field_verify_date_value']['expose']['remember_roles'] = array(
  2 => '2',
);
$handler->display->display_options['filters']['field_verify_date_value']['granularity'] = 'hour';
$handler->display->display_options['filters']['field_verify_date_value']['default_date'] = 'now + 1 day';
/* Filter criterion: Content: Report to verify (field_report_to_verify) */
$handler->display->display_options['filters']['field_report_to_verify_target_id']['id'] = 'field_report_to_verify_target_id';
$handler->display->display_options['filters']['field_report_to_verify_target_id']['table'] = 'field_data_field_report_to_verify';
$handler->display->display_options['filters']['field_report_to_verify_target_id']['field'] = 'field_report_to_verify_target_id';
$handler->display->display_options['filters']['field_report_to_verify_target_id']['operator'] = 'not empty';
/* Filter criterion: Content: Declined (field_declined) */
$handler->display->display_options['filters']['field_declined_value']['id'] = 'field_declined_value';
$handler->display->display_options['filters']['field_declined_value']['table'] = 'field_data_field_declined';
$handler->display->display_options['filters']['field_declined_value']['field'] = 'field_declined_value';
$handler->display->display_options['filters']['field_declined_value']['operator'] = 'not';
$handler->display->display_options['filters']['field_declined_value']['value'] = array(
  1 => '1',
);
$handler->display->display_options['path'] = 'tobe-delinquent';
$translatables['find_delinquent_verify_slots'] = array(
  t('Master'),
  t('more'),
  t('Apply'),
  t('Reset'),
  t('Sort by'),
  t('Asc'),
  t('Desc'),
  t('Content'),
  t('- Choose an operation -'),
  t('Verify date (field_verify_date)'),
  t('Page'),
  t('Nid'),
  t('Slot owner'),
  t('Report to verify'),
  t('Verify date'),
);

