<?php

/**
 * @file
 * Default view: project_user_shareholder
 */

$view = new view();
$view->name = 'project_user_shareholder';
$view->description = '';
$view->tag = 'default';
$view->base_table = 'node';
$view->human_name = 'project user shareholder';
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
/* Sort criterion: Content: Post date */
$handler->display->display_options['sorts']['created']['id'] = 'created';
$handler->display->display_options['sorts']['created']['table'] = 'node';
$handler->display->display_options['sorts']['created']['field'] = 'created';
$handler->display->display_options['sorts']['created']['order'] = 'DESC';
/* Contextual filter: Content: Project (field_project) */
$handler->display->display_options['arguments']['field_project_target_id']['id'] = 'field_project_target_id';
$handler->display->display_options['arguments']['field_project_target_id']['table'] = 'field_data_field_project';
$handler->display->display_options['arguments']['field_project_target_id']['field'] = 'field_project_target_id';
$handler->display->display_options['arguments']['field_project_target_id']['default_action'] = 'not found';
$handler->display->display_options['arguments']['field_project_target_id']['default_argument_type'] = 'fixed';
$handler->display->display_options['arguments']['field_project_target_id']['summary']['number_of_records'] = '0';
$handler->display->display_options['arguments']['field_project_target_id']['summary']['format'] = 'default_summary';
$handler->display->display_options['arguments']['field_project_target_id']['summary_options']['items_per_page'] = '25';
/* Contextual filter: Content: Worker Name (field_coworker) */
$handler->display->display_options['arguments']['field_coworker_target_id']['id'] = 'field_coworker_target_id';
$handler->display->display_options['arguments']['field_coworker_target_id']['table'] = 'field_data_field_coworker';
$handler->display->display_options['arguments']['field_coworker_target_id']['field'] = 'field_coworker_target_id';
$handler->display->display_options['arguments']['field_coworker_target_id']['default_action'] = 'not found';
$handler->display->display_options['arguments']['field_coworker_target_id']['default_argument_type'] = 'fixed';
$handler->display->display_options['arguments']['field_coworker_target_id']['summary']['number_of_records'] = '0';
$handler->display->display_options['arguments']['field_coworker_target_id']['summary']['format'] = 'default_summary';
$handler->display->display_options['arguments']['field_coworker_target_id']['summary_options']['items_per_page'] = '25';
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
  'co_worker_merit_allocation' => 'co_worker_merit_allocation',
);

/* Display: Block */
$handler = $view->new_display('block', 'Block', 'block_1');
$handler->display->display_options['defaults']['group_by'] = FALSE;
$handler->display->display_options['defaults']['pager'] = FALSE;
$handler->display->display_options['pager']['type'] = 'none';
$handler->display->display_options['pager']['options']['offset'] = '0';
$handler->display->display_options['defaults']['fields'] = FALSE;
/* Field: Content: Merit Allocation */
$handler->display->display_options['fields']['field_merit_allocation']['id'] = 'field_merit_allocation';
$handler->display->display_options['fields']['field_merit_allocation']['table'] = 'field_data_field_merit_allocation';
$handler->display->display_options['fields']['field_merit_allocation']['field'] = 'field_merit_allocation';
$handler->display->display_options['fields']['field_merit_allocation']['group_type'] = 'sum';
$handler->display->display_options['fields']['field_merit_allocation']['label'] = '';
$handler->display->display_options['fields']['field_merit_allocation']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_merit_allocation']['type'] = 'number_unformatted';
$handler->display->display_options['fields']['field_merit_allocation']['settings'] = array(
  'thousand_separator' => '',
  'decimal_separator' => '.',
  'scale' => '2',
  'prefix_suffix' => 1,
);
/* Field: Bulk operations: Content */
$handler->display->display_options['fields']['views_bulk_operations']['id'] = 'views_bulk_operations';
$handler->display->display_options['fields']['views_bulk_operations']['table'] = 'views_entity_node';
$handler->display->display_options['fields']['views_bulk_operations']['field'] = 'views_bulk_operations';
$handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['display_type'] = '0';
$handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['enable_select_all_pages'] = 1;
$handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['row_clickable'] = 1;
$handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['force_single'] = 0;
$handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['entity_load_capacity'] = '10';
$handler->display->display_options['defaults']['arguments'] = FALSE;
/* Contextual filter: Content: Project (field_project) */
$handler->display->display_options['arguments']['field_project_target_id']['id'] = 'field_project_target_id';
$handler->display->display_options['arguments']['field_project_target_id']['table'] = 'field_data_field_project';
$handler->display->display_options['arguments']['field_project_target_id']['field'] = 'field_project_target_id';
$handler->display->display_options['arguments']['field_project_target_id']['default_action'] = 'not found';
$handler->display->display_options['arguments']['field_project_target_id']['default_argument_type'] = 'fixed';
$handler->display->display_options['arguments']['field_project_target_id']['summary']['number_of_records'] = '0';
$handler->display->display_options['arguments']['field_project_target_id']['summary']['format'] = 'default_summary';
$handler->display->display_options['arguments']['field_project_target_id']['summary_options']['items_per_page'] = '25';
$translatables['project_user_shareholder'] = array(
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
  t('Block'),
);

