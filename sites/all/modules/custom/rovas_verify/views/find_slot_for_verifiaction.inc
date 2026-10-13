<?php

/**
 * @file
 * Default view: find_slot_for_verifiaction
 */

$view = new view();
$view->name = 'find_slot_for_verifiaction';
$view->description = '';
$view->tag = 'default';
$view->base_table = 'node';
$view->human_name = 'Find slot for verifiaction';
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
$handler->display->display_options['pager']['options']['items_per_page'] = '1';
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
/* Contextual filter: Content: Slot owner (field_slip_owner) */
$handler->display->display_options['arguments']['field_slip_owner_target_id']['id'] = 'field_slip_owner_target_id';
$handler->display->display_options['arguments']['field_slip_owner_target_id']['table'] = 'field_data_field_slip_owner';
$handler->display->display_options['arguments']['field_slip_owner_target_id']['field'] = 'field_slip_owner_target_id';
$handler->display->display_options['arguments']['field_slip_owner_target_id']['default_action'] = 'default';
$handler->display->display_options['arguments']['field_slip_owner_target_id']['default_argument_type'] = 'fixed';
$handler->display->display_options['arguments']['field_slip_owner_target_id']['default_argument_options']['argument'] = 'all';
$handler->display->display_options['arguments']['field_slip_owner_target_id']['summary']['number_of_records'] = '0';
$handler->display->display_options['arguments']['field_slip_owner_target_id']['summary']['format'] = 'default_summary';
$handler->display->display_options['arguments']['field_slip_owner_target_id']['summary_options']['items_per_page'] = '25';
/* Contextual filter: Content: Report to verify (field_report_to_verify) */
$handler->display->display_options['arguments']['field_report_to_verify_target_id']['id'] = 'field_report_to_verify_target_id';
$handler->display->display_options['arguments']['field_report_to_verify_target_id']['table'] = 'field_data_field_report_to_verify';
$handler->display->display_options['arguments']['field_report_to_verify_target_id']['field'] = 'field_report_to_verify_target_id';
$handler->display->display_options['arguments']['field_report_to_verify_target_id']['default_action'] = 'default';
$handler->display->display_options['arguments']['field_report_to_verify_target_id']['default_argument_type'] = 'fixed';
$handler->display->display_options['arguments']['field_report_to_verify_target_id']['default_argument_options']['argument'] = '1';
$handler->display->display_options['arguments']['field_report_to_verify_target_id']['summary']['number_of_records'] = '0';
$handler->display->display_options['arguments']['field_report_to_verify_target_id']['summary']['format'] = 'default_summary';
$handler->display->display_options['arguments']['field_report_to_verify_target_id']['summary_options']['items_per_page'] = '25';
$handler->display->display_options['filter_groups']['groups'] = array(
  1 => 'AND',
  2 => 'OR',
);
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
$handler->display->display_options['filters']['type']['group'] = 1;
/* Filter criterion: Content: Verify date (field_verify_date) */
$handler->display->display_options['filters']['field_verify_date_value_1']['id'] = 'field_verify_date_value_1';
$handler->display->display_options['filters']['field_verify_date_value_1']['table'] = 'field_data_field_verify_date';
$handler->display->display_options['filters']['field_verify_date_value_1']['field'] = 'field_verify_date_value';
$handler->display->display_options['filters']['field_verify_date_value_1']['operator'] = 'empty';
$handler->display->display_options['filters']['field_verify_date_value_1']['group'] = 2;
/* Filter criterion: Content: Verify date (field_verify_date) */
$handler->display->display_options['filters']['field_verify_date_value']['id'] = 'field_verify_date_value';
$handler->display->display_options['filters']['field_verify_date_value']['table'] = 'field_data_field_verify_date';
$handler->display->display_options['filters']['field_verify_date_value']['field'] = 'field_verify_date_value';
$handler->display->display_options['filters']['field_verify_date_value']['operator'] = '>=';
$handler->display->display_options['filters']['field_verify_date_value']['group'] = 2;
$handler->display->display_options['filters']['field_verify_date_value']['granularity'] = 'minute';
$handler->display->display_options['filters']['field_verify_date_value']['default_date'] = 'today';
/* Filter criterion: Content: Declined (field_declined) */
$handler->display->display_options['filters']['field_declined_value']['id'] = 'field_declined_value';
$handler->display->display_options['filters']['field_declined_value']['table'] = 'field_data_field_declined';
$handler->display->display_options['filters']['field_declined_value']['field'] = 'field_declined_value';
$handler->display->display_options['filters']['field_declined_value']['operator'] = 'not';
$handler->display->display_options['filters']['field_declined_value']['value'] = array(
  1 => '1',
);
$translatables['find_slot_for_verifiaction'] = array(
  t('Master'),
  t('more'),
  t('Apply'),
  t('Reset'),
  t('Sort by'),
  t('Asc'),
  t('Desc'),
  t('Content'),
  t('- Choose an operation -'),
  t('All'),
);

