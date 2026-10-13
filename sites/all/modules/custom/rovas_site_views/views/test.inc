<?php

/**
 * @file
 * Default view: test
 */

$view = new view();
$view->name = 'test';
$view->description = 'find already verified reports with verification slots';
$view->tag = 'default';
$view->base_table = 'node';
$view->human_name = 'test over-verified report';
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
$handler->display->display_options['relationships']['reverse_field_report_to_verify_node']['id'] = 'reverse_field_report_to_verify_node';
$handler->display->display_options['relationships']['reverse_field_report_to_verify_node']['table'] = 'node';
$handler->display->display_options['relationships']['reverse_field_report_to_verify_node']['field'] = 'reverse_field_report_to_verify_node';
$handler->display->display_options['relationships']['reverse_field_report_to_verify_node']['label'] = 'slot';
/* Field: Content: Title */
$handler->display->display_options['fields']['title']['id'] = 'title';
$handler->display->display_options['fields']['title']['table'] = 'node';
$handler->display->display_options['fields']['title']['field'] = 'title';
$handler->display->display_options['fields']['title']['label'] = '';
$handler->display->display_options['fields']['title']['alter']['word_boundary'] = FALSE;
$handler->display->display_options['fields']['title']['alter']['ellipsis'] = FALSE;
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
  'work_report' => 'work_report',
);
$handler->display->display_options['filters']['type']['group'] = 1;
/* Filter criterion: Content: Verification status (field_verification_status) */
$handler->display->display_options['filters']['field_verification_status_value']['id'] = 'field_verification_status_value';
$handler->display->display_options['filters']['field_verification_status_value']['table'] = 'field_data_field_verification_status';
$handler->display->display_options['filters']['field_verification_status_value']['field'] = 'field_verification_status_value';
$handler->display->display_options['filters']['field_verification_status_value']['value'] = array(
  1 => '1',
  2 => '2',
);
$handler->display->display_options['filters']['field_verification_status_value']['group'] = 1;
/* Filter criterion: Content: Nid */
$handler->display->display_options['filters']['nid']['id'] = 'nid';
$handler->display->display_options['filters']['nid']['table'] = 'node';
$handler->display->display_options['filters']['nid']['field'] = 'nid';
$handler->display->display_options['filters']['nid']['relationship'] = 'reverse_field_report_to_verify_node';
$handler->display->display_options['filters']['nid']['operator'] = 'not empty';
$translatables['test'] = array(
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
  t('slot'),
);

