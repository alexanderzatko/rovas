<?php

/**
 * @file
 * Default view: total_chrons
 */

$view = new view();
$view->name = 'total_chrons';
$view->description = '';
$view->tag = 'default';
$view->base_table = 'node';
$view->human_name = 'total chrons';
$view->core = 7;
$view->api_version = '3.0';
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

/* Display: Master */
$handler = $view->new_display('default', 'Master', 'default');
$handler->display->display_options['title'] = 'total chrons';
$handler->display->display_options['use_more_always'] = FALSE;
$handler->display->display_options['group_by'] = TRUE;
$handler->display->display_options['access']['type'] = 'perm';
$handler->display->display_options['cache']['type'] = 'none';
$handler->display->display_options['query']['type'] = 'views_query';
$handler->display->display_options['exposed_form']['type'] = 'basic';
$handler->display->display_options['pager']['type'] = 'none';
$handler->display->display_options['pager']['options']['offset'] = '0';
$handler->display->display_options['style_plugin'] = 'chart';
$handler->display->display_options['style_options']['type'] = 'column';
$handler->display->display_options['style_options']['library'] = '';
$handler->display->display_options['style_options']['label_field'] = 'raw_field';
$handler->display->display_options['style_options']['data_fields'] = array(
  'field_chrons_to_grant' => 'field_chrons_to_grant',
  'raw_field' => 'raw_field',
  'nid' => 0,
);
$handler->display->display_options['style_options']['field_colors'] = array(
  'field_chrons_to_grant' => '#2f7ed8',
  'raw_field' => '#0d233a',
  'nid' => '#8bbc21',
);
$handler->display->display_options['style_options']['data_labels'] = 'TRUE';
$handler->display->display_options['style_options']['tooltips'] = 'TRUE';
$handler->display->display_options['style_options']['width'] = '';
$handler->display->display_options['style_options']['height'] = '';
$handler->display->display_options['style_options']['xaxis_title'] = 'date';
$handler->display->display_options['style_options']['xaxis_labels_rotation'] = '0';
$handler->display->display_options['style_options']['yaxis_title'] = 'chrons';
$handler->display->display_options['style_options']['yaxis_labels_rotation'] = '0';
/* Field: SUM(Content: Reward) */
$handler->display->display_options['fields']['field_chrons_to_grant']['id'] = 'field_chrons_to_grant';
$handler->display->display_options['fields']['field_chrons_to_grant']['table'] = 'field_data_field_chrons_to_grant';
$handler->display->display_options['fields']['field_chrons_to_grant']['field'] = 'field_chrons_to_grant';
$handler->display->display_options['fields']['field_chrons_to_grant']['group_type'] = 'sum';
$handler->display->display_options['fields']['field_chrons_to_grant']['settings'] = array(
  'thousand_separator' => '',
  'decimal_separator' => '.',
  'scale' => '2',
  'prefix_suffix' => 0,
);
/* Field: Raw SQL: Raw field */
$handler->display->display_options['fields']['raw_field']['id'] = 'raw_field';
$handler->display->display_options['fields']['raw_field']['table'] = 'views_raw_sql';
$handler->display->display_options['fields']['raw_field']['field'] = 'raw_field';
$handler->display->display_options['fields']['raw_field']['raw_sql'] = 'DATE_FORMAT(FROM_UNIXTIME(node.created), \'%b %Y\')';
/* Field: COUNT(Content: Nid) */
$handler->display->display_options['fields']['nid']['id'] = 'nid';
$handler->display->display_options['fields']['nid']['table'] = 'node';
$handler->display->display_options['fields']['nid']['field'] = 'nid';
$handler->display->display_options['fields']['nid']['group_type'] = 'count';
$handler->display->display_options['fields']['nid']['label'] = '';
$handler->display->display_options['fields']['nid']['exclude'] = TRUE;
$handler->display->display_options['fields']['nid']['element_label_colon'] = FALSE;
/* Sort criterion: Raw SQL: Raw sort */
$handler->display->display_options['sorts']['raw_sort']['id'] = 'raw_sort';
$handler->display->display_options['sorts']['raw_sort']['table'] = 'views_raw_sql';
$handler->display->display_options['sorts']['raw_sort']['field'] = 'raw_sort';
$handler->display->display_options['sorts']['raw_sort']['raw_sql'] = 'DATE_FORMAT(FROM_UNIXTIME(node.created), \'%Y%m\')';
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
/* Filter criterion: Content: Verification status (field_verification_status) */
$handler->display->display_options['filters']['field_verification_status_value']['id'] = 'field_verification_status_value';
$handler->display->display_options['filters']['field_verification_status_value']['table'] = 'field_data_field_verification_status';
$handler->display->display_options['filters']['field_verification_status_value']['field'] = 'field_verification_status_value';
$handler->display->display_options['filters']['field_verification_status_value']['value'] = array(
  1 => '1',
);
$handler->display->display_options['filters']['field_verification_status_value']['group'] = 1;
/* Filter criterion: Content: Author uid */
$handler->display->display_options['filters']['uid']['id'] = 'uid';
$handler->display->display_options['filters']['uid']['table'] = 'node';
$handler->display->display_options['filters']['uid']['field'] = 'uid';
$handler->display->display_options['filters']['uid']['operator'] = 'not in';
$handler->display->display_options['filters']['uid']['value'] = array(
  0 => '1',
);

/* Display: Page */
$handler = $view->new_display('page', 'Page', 'page');
$handler->display->display_options['path'] = 'total-chrons';
$translatables['total_chrons'] = array(
  t('Master'),
  t('total chrons'),
  t('more'),
  t('Apply'),
  t('Reset'),
  t('Sort by'),
  t('Asc'),
  t('Desc'),
  t('Reward'),
  t('Raw field'),
  t('Page'),
);

