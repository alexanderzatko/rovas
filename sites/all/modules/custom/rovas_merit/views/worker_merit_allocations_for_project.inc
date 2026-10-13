<?php

/**
 * @file
 * Default view: worker_merit_allocations_for_project
 */

$view = new view();
$view->name = 'worker_merit_allocations_for_project';
$view->description = '';
$view->tag = 'default';
$view->base_table = 'node';
$view->human_name = 'Worker merit allocations for project';
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
/* Field: Content: Nid */
$handler->display->display_options['fields']['nid']['id'] = 'nid';
$handler->display->display_options['fields']['nid']['table'] = 'node';
$handler->display->display_options['fields']['nid']['field'] = 'nid';
$handler->display->display_options['fields']['nid']['label'] = '';
$handler->display->display_options['fields']['nid']['element_label_colon'] = FALSE;
/* Field: Bulk operations: Content */
$handler->display->display_options['fields']['views_bulk_operations']['id'] = 'views_bulk_operations';
$handler->display->display_options['fields']['views_bulk_operations']['table'] = 'views_entity_node';
$handler->display->display_options['fields']['views_bulk_operations']['field'] = 'views_bulk_operations';
$handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['display_type'] = '0';
$handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['enable_select_all_pages'] = 1;
$handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['row_clickable'] = 1;
$handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['force_single'] = 0;
$handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['entity_load_capacity'] = '10';
/* Contextual filter: Content: Project (field_project) */
$handler->display->display_options['arguments']['field_project_target_id']['id'] = 'field_project_target_id';
$handler->display->display_options['arguments']['field_project_target_id']['table'] = 'field_data_field_project';
$handler->display->display_options['arguments']['field_project_target_id']['field'] = 'field_project_target_id';
$handler->display->display_options['arguments']['field_project_target_id']['default_argument_type'] = 'fixed';
$handler->display->display_options['arguments']['field_project_target_id']['summary']['number_of_records'] = '0';
$handler->display->display_options['arguments']['field_project_target_id']['summary']['format'] = 'default_summary';
$handler->display->display_options['arguments']['field_project_target_id']['summary_options']['items_per_page'] = '25';
/* Contextual filter: Content: Worker Name (field_coworker) */
$handler->display->display_options['arguments']['field_coworker_target_id']['id'] = 'field_coworker_target_id';
$handler->display->display_options['arguments']['field_coworker_target_id']['table'] = 'field_data_field_coworker';
$handler->display->display_options['arguments']['field_coworker_target_id']['field'] = 'field_coworker_target_id';
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
$handler->display->display_options['defaults']['relationships'] = FALSE;
/* Relationship: Entity Reference: Referenced Entity */
$handler->display->display_options['relationships']['field_project_target_id']['id'] = 'field_project_target_id';
$handler->display->display_options['relationships']['field_project_target_id']['table'] = 'field_data_field_project';
$handler->display->display_options['relationships']['field_project_target_id']['field'] = 'field_project_target_id';
$handler->display->display_options['relationships']['field_project_target_id']['label'] = 'project';
$handler->display->display_options['defaults']['sorts'] = FALSE;
/* Sort criterion: Content: Project Chrons (field_project_chrons) */
$handler->display->display_options['sorts']['field_project_chrons_value']['id'] = 'field_project_chrons_value';
$handler->display->display_options['sorts']['field_project_chrons_value']['table'] = 'field_data_field_project_chrons';
$handler->display->display_options['sorts']['field_project_chrons_value']['field'] = 'field_project_chrons_value';
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
  'co_worker_merit_allocation' => 'co_worker_merit_allocation',
);
/* Filter criterion: Content: Reward recipient (field_reward_target) */
$handler->display->display_options['filters']['field_reward_target_value']['id'] = 'field_reward_target_value';
$handler->display->display_options['filters']['field_reward_target_value']['table'] = 'field_data_field_reward_target';
$handler->display->display_options['filters']['field_reward_target_value']['field'] = 'field_reward_target_value';
$handler->display->display_options['filters']['field_reward_target_value']['relationship'] = 'field_project_target_id';
$handler->display->display_options['filters']['field_reward_target_value']['value'] = array(
  2 => '2',
);
/* Filter criterion: Content: Project Chrons (field_project_chrons) */
$handler->display->display_options['filters']['field_project_chrons_value']['id'] = 'field_project_chrons_value';
$handler->display->display_options['filters']['field_project_chrons_value']['table'] = 'field_data_field_project_chrons';
$handler->display->display_options['filters']['field_project_chrons_value']['field'] = 'field_project_chrons_value';
$handler->display->display_options['filters']['field_project_chrons_value']['operator'] = '>';
$handler->display->display_options['filters']['field_project_chrons_value']['value']['value'] = '0';
$translatables['worker_merit_allocations_for_project'] = array(
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
  t('Block'),
  t('project'),
);

