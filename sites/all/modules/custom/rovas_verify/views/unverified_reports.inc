<?php

/**
 * @file
 * Default view: unverified_reports
 */

$view = new view();
$view->name = 'unverified_reports';
$view->description = '';
$view->tag = 'default';
$view->base_table = 'node';
$view->human_name = 'Unverified reports';
$view->core = 7;
$view->api_version = '3.0';
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

/* Display: Master */
$handler = $view->new_display('default', 'Master', 'default');
$handler->display->display_options['title'] = 'Unverified reports';
$handler->display->display_options['use_more_always'] = FALSE;
$handler->display->display_options['group_by'] = TRUE;
$handler->display->display_options['access']['type'] = 'perm';
$handler->display->display_options['cache']['type'] = 'none';
$handler->display->display_options['query']['type'] = 'views_query';
$handler->display->display_options['exposed_form']['type'] = 'basic';
$handler->display->display_options['pager']['type'] = 'full';
$handler->display->display_options['pager']['options']['items_per_page'] = '10';
$handler->display->display_options['style_plugin'] = 'table';
$handler->display->display_options['style_options']['columns'] = array(
  'created' => 'created',
  'name' => 'name',
  'title' => 'title',
  'nid' => 'nid',
  'field_verification_status' => 'field_verification_status',
  'field_voluntary' => 'field_voluntary',
);
$handler->display->display_options['style_options']['class'] = '';
$handler->display->display_options['style_options']['default'] = 'created';
$handler->display->display_options['style_options']['info'] = array(
  'created' => array(
    'sortable' => 1,
    'default_sort_order' => 'desc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'name' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'title' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'nid' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_verification_status' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_voluntary' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
);
/* Relationship: Entity Reference: Referencing entity */
$handler->display->display_options['relationships']['reverse_field_report_to_verify_node']['id'] = 'reverse_field_report_to_verify_node';
$handler->display->display_options['relationships']['reverse_field_report_to_verify_node']['table'] = 'node';
$handler->display->display_options['relationships']['reverse_field_report_to_verify_node']['field'] = 'reverse_field_report_to_verify_node';
$handler->display->display_options['relationships']['reverse_field_report_to_verify_node']['label'] = 'slots';
/* Relationship: Content: Content author */
$handler->display->display_options['relationships']['uid']['id'] = 'uid';
$handler->display->display_options['relationships']['uid']['table'] = 'node';
$handler->display->display_options['relationships']['uid']['field'] = 'uid';
/* Field: Content: Post date */
$handler->display->display_options['fields']['created']['id'] = 'created';
$handler->display->display_options['fields']['created']['table'] = 'node';
$handler->display->display_options['fields']['created']['field'] = 'created';
$handler->display->display_options['fields']['created']['date_format'] = 'custom';
$handler->display->display_options['fields']['created']['custom_date_format'] = 'd M Y';
$handler->display->display_options['fields']['created']['second_date_format'] = 'privatemsg_current_day';
/* Field: User: Name */
$handler->display->display_options['fields']['name']['id'] = 'name';
$handler->display->display_options['fields']['name']['table'] = 'users';
$handler->display->display_options['fields']['name']['field'] = 'name';
$handler->display->display_options['fields']['name']['relationship'] = 'uid';
$handler->display->display_options['fields']['name']['label'] = 'Author';
/* Field: Content: Title */
$handler->display->display_options['fields']['title']['id'] = 'title';
$handler->display->display_options['fields']['title']['table'] = 'node';
$handler->display->display_options['fields']['title']['field'] = 'title';
$handler->display->display_options['fields']['title']['label'] = '';
$handler->display->display_options['fields']['title']['alter']['word_boundary'] = FALSE;
$handler->display->display_options['fields']['title']['alter']['ellipsis'] = FALSE;
/* Field: COUNT(Content: Nid) */
$handler->display->display_options['fields']['nid']['id'] = 'nid';
$handler->display->display_options['fields']['nid']['table'] = 'node';
$handler->display->display_options['fields']['nid']['field'] = 'nid';
$handler->display->display_options['fields']['nid']['relationship'] = 'reverse_field_report_to_verify_node';
$handler->display->display_options['fields']['nid']['group_type'] = 'count';
$handler->display->display_options['fields']['nid']['label'] = 'slots';
/* Field: Content: Verification status */
$handler->display->display_options['fields']['field_verification_status']['id'] = 'field_verification_status';
$handler->display->display_options['fields']['field_verification_status']['table'] = 'field_data_field_verification_status';
$handler->display->display_options['fields']['field_verification_status']['field'] = 'field_verification_status';
/* Sort criterion: Content: Post date */
$handler->display->display_options['sorts']['created']['id'] = 'created';
$handler->display->display_options['sorts']['created']['table'] = 'node';
$handler->display->display_options['sorts']['created']['field'] = 'created';
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
$handler->display->display_options['filters']['field_verification_status_value']['operator'] = 'not';
$handler->display->display_options['filters']['field_verification_status_value']['value'] = array(
  1 => '1',
  2 => '2',
);
$handler->display->display_options['filters']['field_verification_status_value']['group'] = 1;
$handler->display->display_options['filters']['field_verification_status_value']['expose']['operator_id'] = 'field_verification_status_value_op';
$handler->display->display_options['filters']['field_verification_status_value']['expose']['label'] = 'Verification status';
$handler->display->display_options['filters']['field_verification_status_value']['expose']['operator'] = 'field_verification_status_value_op';
$handler->display->display_options['filters']['field_verification_status_value']['expose']['identifier'] = 'field_verification_status_value';
$handler->display->display_options['filters']['field_verification_status_value']['expose']['remember_roles'] = array(
  2 => '2',
);
/* Filter criterion: Content: Nid */
$handler->display->display_options['filters']['nid']['id'] = 'nid';
$handler->display->display_options['filters']['nid']['table'] = 'node';
$handler->display->display_options['filters']['nid']['field'] = 'nid';
$handler->display->display_options['filters']['nid']['operator'] = '>';
$handler->display->display_options['filters']['nid']['value']['value'] = '37000';
$handler->display->display_options['filters']['nid']['group'] = 1;
/* Filter criterion: Userpoints total: Current total merits */
$handler->display->display_options['filters']['points']['id'] = 'points';
$handler->display->display_options['filters']['points']['table'] = 'userpoints_total';
$handler->display->display_options['filters']['points']['field'] = 'points';
$handler->display->display_options['filters']['points']['operator'] = '>=';
$handler->display->display_options['filters']['points']['value']['value'] = '0';

/* Display: Page */
$handler = $view->new_display('page', 'Page', 'page');
$handler->display->display_options['path'] = 'unverified-reports';
$translatables['unverified_reports'] = array(
  t('Master'),
  t('Unverified reports'),
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
  t('slots'),
  t('author'),
  t('Post date'),
  t('Author'),
  t('Verification status'),
  t('Page'),
);

