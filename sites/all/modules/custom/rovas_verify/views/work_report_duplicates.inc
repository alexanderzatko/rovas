<?php

/**
 * @file
 * Default view: work_report_duplicates
 */

$view = new view();
$view->name = 'work_report_duplicates';
$view->description = '';
$view->tag = 'default';
$view->base_table = 'node';
$view->human_name = 'work report duplicates';
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
$handler->display->display_options['fields']['nid']['label'] = '';
$handler->display->display_options['fields']['nid']['element_label_colon'] = FALSE;
/* Contextual filter: Content: Project (field_project) */
$handler->display->display_options['arguments']['field_project_target_id']['id'] = 'field_project_target_id';
$handler->display->display_options['arguments']['field_project_target_id']['table'] = 'field_data_field_project';
$handler->display->display_options['arguments']['field_project_target_id']['field'] = 'field_project_target_id';
$handler->display->display_options['arguments']['field_project_target_id']['default_action'] = 'default';
$handler->display->display_options['arguments']['field_project_target_id']['default_argument_type'] = 'fixed';
$handler->display->display_options['arguments']['field_project_target_id']['summary']['number_of_records'] = '0';
$handler->display->display_options['arguments']['field_project_target_id']['summary']['format'] = 'default_summary';
$handler->display->display_options['arguments']['field_project_target_id']['summary_options']['items_per_page'] = '25';
/* Contextual filter: Content: Content access token (field_content_access_token) */
$handler->display->display_options['arguments']['field_content_access_token_value']['id'] = 'field_content_access_token_value';
$handler->display->display_options['arguments']['field_content_access_token_value']['table'] = 'field_data_field_content_access_token';
$handler->display->display_options['arguments']['field_content_access_token_value']['field'] = 'field_content_access_token_value';
$handler->display->display_options['arguments']['field_content_access_token_value']['default_action'] = 'default';
$handler->display->display_options['arguments']['field_content_access_token_value']['default_argument_type'] = 'fixed';
$handler->display->display_options['arguments']['field_content_access_token_value']['summary']['number_of_records'] = '0';
$handler->display->display_options['arguments']['field_content_access_token_value']['summary']['format'] = 'default_summary';
$handler->display->display_options['arguments']['field_content_access_token_value']['summary_options']['items_per_page'] = '25';
$handler->display->display_options['arguments']['field_content_access_token_value']['limit'] = '0';
/* Filter criterion: Content: Type */
$handler->display->display_options['filters']['type']['id'] = 'type';
$handler->display->display_options['filters']['type']['table'] = 'node';
$handler->display->display_options['filters']['type']['field'] = 'type';
$handler->display->display_options['filters']['type']['value'] = array(
  'work_report' => 'work_report',
);
$handler->display->display_options['filters']['type']['group'] = 1;
$translatables['work_report_duplicates'] = array(
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
  t('All'),
);

