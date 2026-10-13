<?php

/**
 * @file
 * Default view: projects_with_allocation_thresholds
 */

$view = new view();
$view->name = 'projects_with_allocation_thresholds';
$view->description = '';
$view->tag = 'default';
$view->base_table = 'node';
$view->human_name = 'Projects with allocation thresholds';
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
/* Sort criterion: Content: Post date */
$handler->display->display_options['sorts']['created']['id'] = 'created';
$handler->display->display_options['sorts']['created']['table'] = 'node';
$handler->display->display_options['sorts']['created']['field'] = 'created';
$handler->display->display_options['sorts']['created']['order'] = 'DESC';
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
/* Filter criterion: Content: Preferred allocation threshold (field_preferred_allocation_thres) */
$handler->display->display_options['filters']['field_preferred_allocation_thres_value']['id'] = 'field_preferred_allocation_thres_value';
$handler->display->display_options['filters']['field_preferred_allocation_thres_value']['table'] = 'field_data_field_preferred_allocation_thres';
$handler->display->display_options['filters']['field_preferred_allocation_thres_value']['field'] = 'field_preferred_allocation_thres_value';
$handler->display->display_options['filters']['field_preferred_allocation_thres_value']['operator'] = 'not empty';
/* Filter criterion: Content: Income and allocations (field_pa_wallet_current_level) */
$handler->display->display_options['filters']['field_pa_wallet_current_level_value']['id'] = 'field_pa_wallet_current_level_value';
$handler->display->display_options['filters']['field_pa_wallet_current_level_value']['table'] = 'field_data_field_pa_wallet_current_level';
$handler->display->display_options['filters']['field_pa_wallet_current_level_value']['field'] = 'field_pa_wallet_current_level_value';
$handler->display->display_options['filters']['field_pa_wallet_current_level_value']['operator'] = '>';
$handler->display->display_options['filters']['field_pa_wallet_current_level_value']['value']['value'] = '0';
$translatables['projects_with_allocation_thresholds'] = array(
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

