<?php

/**
 * @file
 * Default view: report_exists_check
 */

$view = new view();
$view->name = 'report_exists_check';
$view->description = 'Same-day WR lookup for daily merge; only unpublished-verification (status 0) reports are mergeable.';
$view->tag = 'default';
$view->base_table = 'node';
$view->human_name = 'report exists check';
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
$handler->display->display_options['pager']['type'] = 'full';
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
/* Contextual filter: Content: Created date */
$handler->display->display_options['arguments']['created_fulldate']['id'] = 'created_fulldate';
$handler->display->display_options['arguments']['created_fulldate']['table'] = 'node';
$handler->display->display_options['arguments']['created_fulldate']['field'] = 'created_fulldate';
$handler->display->display_options['arguments']['created_fulldate']['default_action'] = 'default';
$handler->display->display_options['arguments']['created_fulldate']['default_argument_type'] = 'fixed';
$handler->display->display_options['arguments']['created_fulldate']['summary']['number_of_records'] = '0';
$handler->display->display_options['arguments']['created_fulldate']['summary']['format'] = 'default_summary';
$handler->display->display_options['arguments']['created_fulldate']['summary_options']['items_per_page'] = '25';
/* Contextual filter: Content: Author uid */
$handler->display->display_options['arguments']['uid']['id'] = 'uid';
$handler->display->display_options['arguments']['uid']['table'] = 'node';
$handler->display->display_options['arguments']['uid']['field'] = 'uid';
$handler->display->display_options['arguments']['uid']['default_action'] = 'default';
$handler->display->display_options['arguments']['uid']['default_argument_type'] = 'fixed';
$handler->display->display_options['arguments']['uid']['summary']['number_of_records'] = '0';
$handler->display->display_options['arguments']['uid']['summary']['format'] = 'default_summary';
$handler->display->display_options['arguments']['uid']['summary_options']['items_per_page'] = '25';
/* Contextual filter: Field: Classification (field_classification) */
$handler->display->display_options['arguments']['field_classification_tid']['id'] = 'field_classification_tid';
$handler->display->display_options['arguments']['field_classification_tid']['table'] = 'field_data_field_classification';
$handler->display->display_options['arguments']['field_classification_tid']['field'] = 'field_classification_tid';
$handler->display->display_options['arguments']['field_classification_tid']['default_action'] = 'default';
$handler->display->display_options['arguments']['field_classification_tid']['default_argument_type'] = 'fixed';
$handler->display->display_options['arguments']['field_classification_tid']['summary']['number_of_records'] = '0';
$handler->display->display_options['arguments']['field_classification_tid']['summary']['format'] = 'default_summary';
$handler->display->display_options['arguments']['field_classification_tid']['summary_options']['items_per_page'] = '25';
/* Contextual filter: Content: Project (field_project) */
$handler->display->display_options['arguments']['field_project_target_id']['id'] = 'field_project_target_id';
$handler->display->display_options['arguments']['field_project_target_id']['table'] = 'field_data_field_project';
$handler->display->display_options['arguments']['field_project_target_id']['field'] = 'field_project_target_id';
$handler->display->display_options['arguments']['field_project_target_id']['default_action'] = 'default';
$handler->display->display_options['arguments']['field_project_target_id']['default_argument_type'] = 'fixed';
$handler->display->display_options['arguments']['field_project_target_id']['summary']['number_of_records'] = '0';
$handler->display->display_options['arguments']['field_project_target_id']['summary']['format'] = 'default_summary';
$handler->display->display_options['arguments']['field_project_target_id']['summary_options']['items_per_page'] = '25';
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
  'work_report' => 'work_report',
);
$handler->display->display_options['filters']['type']['group'] = 1;
/* Filter criterion: Content: Verification status (field_verification_status)
 * Only "not verified" (0) reports are merge targets. Valid (1) or invalid (2)
 * reports are locked; a later same-day session creates a new WR instead. */
$handler->display->display_options['filters']['field_verification_status_value']['id'] = 'field_verification_status_value';
$handler->display->display_options['filters']['field_verification_status_value']['table'] = 'field_data_field_verification_status';
$handler->display->display_options['filters']['field_verification_status_value']['field'] = 'field_verification_status_value';
$handler->display->display_options['filters']['field_verification_status_value']['value'] = array(
  0 => '0',
);
$handler->display->display_options['filters']['field_verification_status_value']['group'] = 1;
$translatables['report_exists_check'] = array(
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
  t('« first'),
  t('‹ previous'),
  t('next ›'),
  t('last »'),
  t('Content'),
  t('- Choose an operation -'),
  t('Nid'),
  t('All'),
);

