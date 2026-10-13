<?php

/**
 * @file
 * Default view: pending_verifications
 */

$view = new view();
$view->name = 'pending_verifications';
$view->description = '';
$view->tag = 'default';
$view->base_table = 'node';
$view->human_name = 'pending verifications';
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
$handler->display->display_options['pager']['type'] = 'none';
$handler->display->display_options['pager']['options']['offset'] = '0';
$handler->display->display_options['style_plugin'] = 'default';
$handler->display->display_options['row_plugin'] = 'fields';
/* Field: Bulk operations: Content */
$handler->display->display_options['fields']['views_bulk_operations']['id'] = 'views_bulk_operations';
$handler->display->display_options['fields']['views_bulk_operations']['table'] = 'views_entity_node';
$handler->display->display_options['fields']['views_bulk_operations']['field'] = 'views_bulk_operations';
$handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['display_type'] = '0';
$handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['enable_select_all_pages'] = 1;
$handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['row_clickable'] = 1;
$handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['force_single'] = 0;
$handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['entity_load_capacity'] = '10';
/* Field: Content: Nid */
$handler->display->display_options['fields']['nid']['id'] = 'nid';
$handler->display->display_options['fields']['nid']['table'] = 'node';
$handler->display->display_options['fields']['nid']['field'] = 'nid';
/* Sort criterion: Content: Nid */
$handler->display->display_options['sorts']['nid']['id'] = 'nid';
$handler->display->display_options['sorts']['nid']['table'] = 'node';
$handler->display->display_options['sorts']['nid']['field'] = 'nid';
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
/* Filter criterion: Content: Report to verify (field_report_to_verify) */
$handler->display->display_options['filters']['field_report_to_verify_target_id']['id'] = 'field_report_to_verify_target_id';
$handler->display->display_options['filters']['field_report_to_verify_target_id']['table'] = 'field_data_field_report_to_verify';
$handler->display->display_options['filters']['field_report_to_verify_target_id']['field'] = 'field_report_to_verify_target_id';
$handler->display->display_options['filters']['field_report_to_verify_target_id']['operator'] = 'not empty';
/* Filter criterion: Content: Verify date (field_verify_date) */
$handler->display->display_options['filters']['field_verify_date_value']['id'] = 'field_verify_date_value';
$handler->display->display_options['filters']['field_verify_date_value']['table'] = 'field_data_field_verify_date';
$handler->display->display_options['filters']['field_verify_date_value']['field'] = 'field_verify_date_value';
$handler->display->display_options['filters']['field_verify_date_value']['operator'] = '<';
$handler->display->display_options['filters']['field_verify_date_value']['granularity'] = 'second';
$handler->display->display_options['filters']['field_verify_date_value']['default_date'] = 'now + 4 days';
$translatables['pending_verifications'] = array(
  t('Master'),
  t('more'),
  t('Apply'),
  t('Reset'),
  t('Sort by'),
  t('Asc'),
  t('Desc'),
  t('Content'),
  t('- Choose an operation -'),
  t('Nid'),
);

