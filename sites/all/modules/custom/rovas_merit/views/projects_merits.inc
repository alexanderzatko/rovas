<?php

/**
 * @file
 * Default view: projects_merits
 */

$view = new view();
$view->name = 'projects_merits';
$view->description = '';
$view->tag = 'default';
$view->base_table = 'node';
$view->human_name = 'Projects merits';
$view->core = 7;
$view->api_version = '3.0';
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

/* Display: Master */
$handler = $view->new_display('default', 'Master', 'default');
$handler->display->display_options['use_more_always'] = FALSE;
$handler->display->display_options['group_by'] = TRUE;
$handler->display->display_options['access']['type'] = 'perm';
$handler->display->display_options['cache']['type'] = 'none';
$handler->display->display_options['query']['type'] = 'views_query';
$handler->display->display_options['exposed_form']['type'] = 'basic';
$handler->display->display_options['pager']['type'] = 'some';
$handler->display->display_options['pager']['options']['items_per_page'] = '10';
$handler->display->display_options['pager']['options']['offset'] = '0';
$handler->display->display_options['style_plugin'] = 'table';
/* Field: Global: View result counter */
$handler->display->display_options['fields']['counter']['id'] = 'counter';
$handler->display->display_options['fields']['counter']['table'] = 'views';
$handler->display->display_options['fields']['counter']['field'] = 'counter';
$handler->display->display_options['fields']['counter']['label'] = 'rank';
$handler->display->display_options['fields']['counter']['counter_start'] = '1';
$handler->display->display_options['fields']['counter']['reverse'] = 0;
/* Field: Content: Author uid */
$handler->display->display_options['fields']['uid']['id'] = 'uid';
$handler->display->display_options['fields']['uid']['table'] = 'node';
$handler->display->display_options['fields']['uid']['field'] = 'uid';
$handler->display->display_options['fields']['uid']['label'] = 'rovas_uid';
$handler->display->display_options['fields']['uid']['link_to_user'] = FALSE;
/* Field: SUM(Content: Merit reward) */
$handler->display->display_options['fields']['field_merit_reward']['id'] = 'field_merit_reward';
$handler->display->display_options['fields']['field_merit_reward']['table'] = 'field_data_field_merit_reward';
$handler->display->display_options['fields']['field_merit_reward']['field'] = 'field_merit_reward';
$handler->display->display_options['fields']['field_merit_reward']['group_type'] = 'sum';
$handler->display->display_options['fields']['field_merit_reward']['label'] = 'merits';
$handler->display->display_options['fields']['field_merit_reward']['settings'] = array(
  'thousand_separator' => '',
  'decimal_separator' => '.',
  'scale' => '2',
  'prefix_suffix' => 0,
);
/* Field: COUNT(Content: Nid) */
$handler->display->display_options['fields']['nid']['id'] = 'nid';
$handler->display->display_options['fields']['nid']['table'] = 'node';
$handler->display->display_options['fields']['nid']['field'] = 'nid';
$handler->display->display_options['fields']['nid']['group_type'] = 'count';
$handler->display->display_options['fields']['nid']['label'] = 'count';
$handler->display->display_options['fields']['nid']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['nid']['separator'] = '';
/* Sort criterion: SUM(Content: Merit reward (field_merit_reward)) */
$handler->display->display_options['sorts']['field_merit_reward_value']['id'] = 'field_merit_reward_value';
$handler->display->display_options['sorts']['field_merit_reward_value']['table'] = 'field_data_field_merit_reward';
$handler->display->display_options['sorts']['field_merit_reward_value']['field'] = 'field_merit_reward_value';
$handler->display->display_options['sorts']['field_merit_reward_value']['group_type'] = 'sum';
$handler->display->display_options['sorts']['field_merit_reward_value']['order'] = 'DESC';
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
/* Filter criterion: Content: Author uid */
$handler->display->display_options['filters']['uid']['id'] = 'uid';
$handler->display->display_options['filters']['uid']['table'] = 'node';
$handler->display->display_options['filters']['uid']['field'] = 'uid';
$handler->display->display_options['filters']['uid']['operator'] = 'not in';
$handler->display->display_options['filters']['uid']['value'] = array(
  0 => '1',
);
/* Filter criterion: Content: Project site (field_asset_web_address) */
$handler->display->display_options['filters']['field_asset_web_address_value']['id'] = 'field_asset_web_address_value';
$handler->display->display_options['filters']['field_asset_web_address_value']['table'] = 'field_data_field_asset_web_address';
$handler->display->display_options['filters']['field_asset_web_address_value']['field'] = 'field_asset_web_address_value';
$handler->display->display_options['filters']['field_asset_web_address_value']['operator'] = 'contains';
$handler->display->display_options['filters']['field_asset_web_address_value']['value'] = 'test-_-';
$handler->display->display_options['filters']['field_asset_web_address_value']['exposed'] = TRUE;
$handler->display->display_options['filters']['field_asset_web_address_value']['expose']['operator_id'] = 'field_asset_web_address_value_op';
$handler->display->display_options['filters']['field_asset_web_address_value']['expose']['label'] = 'Web Address (field_asset_web_address)';
$handler->display->display_options['filters']['field_asset_web_address_value']['expose']['operator'] = 'field_asset_web_address_value_op';
$handler->display->display_options['filters']['field_asset_web_address_value']['expose']['identifier'] = 'field_asset_web_address_value';
$handler->display->display_options['filters']['field_asset_web_address_value']['expose']['remember_roles'] = array(
  2 => '2',
);
$handler->display->display_options['filters']['field_asset_web_address_value']['expose']['autocomplete_items'] = '10';
$handler->display->display_options['filters']['field_asset_web_address_value']['expose']['autocomplete_min_chars'] = '0';
$handler->display->display_options['filters']['field_asset_web_address_value']['expose']['autocomplete_raw_suggestion'] = 1;
$handler->display->display_options['filters']['field_asset_web_address_value']['expose']['autocomplete_raw_dropdown'] = 1;
$handler->display->display_options['filters']['field_asset_web_address_value']['expose']['autocomplete_dependent'] = 0;

/* Display: Services */
$handler = $view->new_display('services', 'Services', 'services');
$handler->display->display_options['path'] = 'rest_view_project_merits';
$translatables['projects_merits'] = array(
  t('Master'),
  t('more'),
  t('Apply'),
  t('Reset'),
  t('Sort by'),
  t('Asc'),
  t('Desc'),
  t('rank'),
  t('rovas_uid'),
  t('merits'),
  t('count'),
  t('Web Address (field_asset_web_address)'),
  t('Services'),
);

