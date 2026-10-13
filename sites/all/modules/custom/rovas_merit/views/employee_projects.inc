<?php

/**
 * @file
 * Default view: employee_projects
 */

$view = new view();
$view->name = 'employee_projects';
$view->description = '';
$view->tag = 'default';
$view->base_table = 'node';
$view->human_name = 'Employee Merit allocations';
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
/* Relationship: Entity Reference: Referencing entity */
$handler->display->display_options['relationships']['reverse_field_project_node']['id'] = 'reverse_field_project_node';
$handler->display->display_options['relationships']['reverse_field_project_node']['table'] = 'node';
$handler->display->display_options['relationships']['reverse_field_project_node']['field'] = 'reverse_field_project_node';
/* Field: Content: Nid */
$handler->display->display_options['fields']['nid']['id'] = 'nid';
$handler->display->display_options['fields']['nid']['table'] = 'node';
$handler->display->display_options['fields']['nid']['field'] = 'nid';
$handler->display->display_options['fields']['nid']['label'] = '';
$handler->display->display_options['fields']['nid']['element_label_colon'] = FALSE;
/* Field: Content: Title */
$handler->display->display_options['fields']['title']['id'] = 'title';
$handler->display->display_options['fields']['title']['table'] = 'node';
$handler->display->display_options['fields']['title']['field'] = 'title';
$handler->display->display_options['fields']['title']['label'] = '';
$handler->display->display_options['fields']['title']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['title']['link_to_node'] = FALSE;
/* Sort criterion: Content: Post date */
$handler->display->display_options['sorts']['created']['id'] = 'created';
$handler->display->display_options['sorts']['created']['table'] = 'node';
$handler->display->display_options['sorts']['created']['field'] = 'created';
$handler->display->display_options['sorts']['created']['order'] = 'DESC';
/* Contextual filter: Content: Worker Name (field_coworker) */
$handler->display->display_options['arguments']['field_coworker_target_id']['id'] = 'field_coworker_target_id';
$handler->display->display_options['arguments']['field_coworker_target_id']['table'] = 'field_data_field_coworker';
$handler->display->display_options['arguments']['field_coworker_target_id']['field'] = 'field_coworker_target_id';
$handler->display->display_options['arguments']['field_coworker_target_id']['relationship'] = 'reverse_field_project_node';
$handler->display->display_options['arguments']['field_coworker_target_id']['default_action'] = 'default';
$handler->display->display_options['arguments']['field_coworker_target_id']['default_argument_type'] = 'current_user';
$handler->display->display_options['arguments']['field_coworker_target_id']['summary']['number_of_records'] = '0';
$handler->display->display_options['arguments']['field_coworker_target_id']['summary']['format'] = 'default_summary';
$handler->display->display_options['arguments']['field_coworker_target_id']['summary_options']['items_per_page'] = '25';
/* Contextual filter: Content: Project (field_project) */
$handler->display->display_options['arguments']['field_project_target_id']['id'] = 'field_project_target_id';
$handler->display->display_options['arguments']['field_project_target_id']['table'] = 'field_data_field_project';
$handler->display->display_options['arguments']['field_project_target_id']['field'] = 'field_project_target_id';
$handler->display->display_options['arguments']['field_project_target_id']['default_action'] = 'default';
$handler->display->display_options['arguments']['field_project_target_id']['default_argument_type'] = 'php';
$handler->display->display_options['arguments']['field_project_target_id']['default_argument_options']['code'] = 'if(arg(1) == \'add\' && is_numeric(arg(3)))
return arg(3);
else
return \'all\';';
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
  'project' => 'project',
);

/* Display: Entity Reference */
$handler = $view->new_display('entityreference', 'Entity Reference', 'entityreference_1');
$handler->display->display_options['defaults']['title'] = FALSE;
$handler->display->display_options['pager']['type'] = 'some';
$handler->display->display_options['defaults']['style_plugin'] = FALSE;
$handler->display->display_options['style_plugin'] = 'entityreference_style';
$handler->display->display_options['style_options']['search_fields'] = array(
  'nid' => 'nid',
  'field_project' => 0,
);
$handler->display->display_options['defaults']['style_options'] = FALSE;
$handler->display->display_options['defaults']['row_plugin'] = FALSE;
$handler->display->display_options['row_plugin'] = 'entityreference_fields';
$handler->display->display_options['defaults']['row_options'] = FALSE;
$translatables['employee_projects'] = array(
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
  t('Content referencing Content from field_project'),
  t('All'),
  t('Entity Reference'),
);

