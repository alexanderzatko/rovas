<?php

/**
 * @file
 * Default view: reference_reports
 */

$view = new view();
$view->name = 'reference_reports';
$view->description = 'selects reports filtered by supplied project and coworker';
$view->tag = 'default';
$view->base_table = 'node';
$view->human_name = 'reference reports';
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
/* Relationship: Content: Content author */
$handler->display->display_options['relationships']['uid']['id'] = 'uid';
$handler->display->display_options['relationships']['uid']['table'] = 'node';
$handler->display->display_options['relationships']['uid']['field'] = 'uid';
/* Field: User: Name */
$handler->display->display_options['fields']['name']['id'] = 'name';
$handler->display->display_options['fields']['name']['table'] = 'users';
$handler->display->display_options['fields']['name']['field'] = 'name';
$handler->display->display_options['fields']['name']['relationship'] = 'uid';
$handler->display->display_options['fields']['name']['label'] = '';
$handler->display->display_options['fields']['name']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['name']['link_to_user'] = FALSE;
/* Field: Content: Date */
$handler->display->display_options['fields']['field_date']['id'] = 'field_date';
$handler->display->display_options['fields']['field_date']['table'] = 'field_data_field_date';
$handler->display->display_options['fields']['field_date']['field'] = 'field_date';
$handler->display->display_options['fields']['field_date']['label'] = '';
$handler->display->display_options['fields']['field_date']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_date']['settings'] = array(
  'format_type' => 'rovas_short',
  'custom_date_format' => '',
  'fromto' => 'both',
  'multiple_number' => '',
  'multiple_from' => '',
  'multiple_to' => '',
  'show_remaining_days' => 0,
);
/* Field: Content: Title */
$handler->display->display_options['fields']['title']['id'] = 'title';
$handler->display->display_options['fields']['title']['table'] = 'node';
$handler->display->display_options['fields']['title']['field'] = 'title';
$handler->display->display_options['fields']['title']['label'] = '';
$handler->display->display_options['fields']['title']['alter']['word_boundary'] = FALSE;
$handler->display->display_options['fields']['title']['alter']['ellipsis'] = FALSE;
/* Sort criterion: Content: Date (field_date) */
$handler->display->display_options['sorts']['field_date_value']['id'] = 'field_date_value';
$handler->display->display_options['sorts']['field_date_value']['table'] = 'field_data_field_date';
$handler->display->display_options['sorts']['field_date_value']['field'] = 'field_date_value';
$handler->display->display_options['sorts']['field_date_value']['order'] = 'DESC';
/* Contextual filter: Content: Project (field_project) */
$handler->display->display_options['arguments']['field_project_target_id']['id'] = 'field_project_target_id';
$handler->display->display_options['arguments']['field_project_target_id']['table'] = 'field_data_field_project';
$handler->display->display_options['arguments']['field_project_target_id']['field'] = 'field_project_target_id';
$handler->display->display_options['arguments']['field_project_target_id']['default_action'] = 'default';
$handler->display->display_options['arguments']['field_project_target_id']['default_argument_type'] = 'fixed';
$handler->display->display_options['arguments']['field_project_target_id']['summary']['number_of_records'] = '0';
$handler->display->display_options['arguments']['field_project_target_id']['summary']['format'] = 'default_summary';
$handler->display->display_options['arguments']['field_project_target_id']['summary_options']['items_per_page'] = '25';
/* Contextual filter: Content: Coworkers (field_attendees) */
$handler->display->display_options['arguments']['field_attendees_target_id']['id'] = 'field_attendees_target_id';
$handler->display->display_options['arguments']['field_attendees_target_id']['table'] = 'field_data_field_attendees';
$handler->display->display_options['arguments']['field_attendees_target_id']['field'] = 'field_attendees_target_id';
$handler->display->display_options['arguments']['field_attendees_target_id']['default_argument_type'] = 'fixed';
$handler->display->display_options['arguments']['field_attendees_target_id']['summary']['number_of_records'] = '0';
$handler->display->display_options['arguments']['field_attendees_target_id']['summary']['format'] = 'default_summary';
$handler->display->display_options['arguments']['field_attendees_target_id']['summary_options']['items_per_page'] = '25';
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

/* Display: Entity Reference */
$handler = $view->new_display('entityreference', 'Entity Reference', 'entityreference_1');
$handler->display->display_options['defaults']['title'] = FALSE;
$handler->display->display_options['pager']['type'] = 'some';
$handler->display->display_options['defaults']['style_plugin'] = FALSE;
$handler->display->display_options['style_plugin'] = 'entityreference_style';
$handler->display->display_options['style_options']['search_fields'] = array(
  'title' => 'title',
  'field_date' => 'field_date',
);
$handler->display->display_options['defaults']['style_options'] = FALSE;
$handler->display->display_options['defaults']['row_plugin'] = FALSE;
$handler->display->display_options['row_plugin'] = 'entityreference_fields';
$handler->display->display_options['defaults']['row_options'] = FALSE;
$translatables['reference_reports'] = array(
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
  t('author'),
  t('All'),
  t('Entity Reference'),
);

