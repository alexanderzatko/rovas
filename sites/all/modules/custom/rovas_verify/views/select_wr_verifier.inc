<?php

/**
 * @file
 * Default view: select_wr_verifier
 */

$view = new view();
$view->name = 'select_wr_verifier';
$view->description = '';
$view->tag = 'default';
$view->base_table = 'node';
$view->human_name = 'select wr verifier';
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
$handler->display->display_options['pager']['type'] = 'full';
$handler->display->display_options['style_plugin'] = 'table';
$handler->display->display_options['style_options']['columns'] = array(
  'uid' => 'uid',
  'delta' => 'delta',
  'field_hours_worked' => 'field_hours_worked',
);
$handler->display->display_options['style_options']['class'] = '';
$handler->display->display_options['style_options']['default'] = '-1';
$handler->display->display_options['style_options']['info'] = array(
  'uid' => array(
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'delta' => array(
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_hours_worked' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
);
/* Field: Content: Author uid */
$handler->display->display_options['fields']['uid']['id'] = 'uid';
$handler->display->display_options['fields']['uid']['table'] = 'node';
$handler->display->display_options['fields']['uid']['field'] = 'uid';
$handler->display->display_options['fields']['uid']['label'] = 'uid';
$handler->display->display_options['fields']['uid']['link_to_user'] = FALSE;
/* Field: Field: Classification (field_classification:delta) */
$handler->display->display_options['fields']['delta']['id'] = 'delta';
$handler->display->display_options['fields']['delta']['table'] = 'field_data_field_classification';
$handler->display->display_options['fields']['delta']['field'] = 'delta';
$handler->display->display_options['fields']['delta']['label'] = 'Categories of interest (field_classification:delta)';
/* Field: SUM(Content: Time worked) */
$handler->display->display_options['fields']['field_hours_worked']['id'] = 'field_hours_worked';
$handler->display->display_options['fields']['field_hours_worked']['table'] = 'field_data_field_hours_worked';
$handler->display->display_options['fields']['field_hours_worked']['field'] = 'field_hours_worked';
$handler->display->display_options['fields']['field_hours_worked']['group_type'] = 'sum';
$handler->display->display_options['fields']['field_hours_worked']['label'] = 'Worked hours';
$handler->display->display_options['fields']['field_hours_worked']['settings'] = array(
  'thousand_separator' => '',
  'decimal_separator' => '.',
  'scale' => '2',
  'prefix_suffix' => 0,
);
/* Sort criterion: SUM(Content: Time worked (field_hours_worked)) */
$handler->display->display_options['sorts']['field_hours_worked_value']['id'] = 'field_hours_worked_value';
$handler->display->display_options['sorts']['field_hours_worked_value']['table'] = 'field_data_field_hours_worked';
$handler->display->display_options['sorts']['field_hours_worked_value']['field'] = 'field_hours_worked_value';
$handler->display->display_options['sorts']['field_hours_worked_value']['group_type'] = 'sum';
$handler->display->display_options['sorts']['field_hours_worked_value']['order'] = 'DESC';
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
/* Filter criterion: Field: Classification (field_classification) */
$handler->display->display_options['filters']['field_classification_tid']['id'] = 'field_classification_tid';
$handler->display->display_options['filters']['field_classification_tid']['table'] = 'field_data_field_classification';
$handler->display->display_options['filters']['field_classification_tid']['field'] = 'field_classification_tid';
$handler->display->display_options['filters']['field_classification_tid']['value'] = array(
  0 => '1794',
);
$handler->display->display_options['filters']['field_classification_tid']['type'] = 'textfield';
$handler->display->display_options['filters']['field_classification_tid']['vocabulary'] = 'tags';
/* Filter criterion: Field: Classification (field_classification:delta) */
$handler->display->display_options['filters']['delta']['id'] = 'delta';
$handler->display->display_options['filters']['delta']['table'] = 'field_data_field_classification';
$handler->display->display_options['filters']['delta']['field'] = 'delta';
$handler->display->display_options['filters']['delta']['operator'] = '>=';
$handler->display->display_options['filters']['delta']['value']['value'] = '0';
$translatables['select_wr_verifier'] = array(
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
  t('uid'),
  t('Categories of interest (field_classification:delta)'),
  t('.'),
  t(','),
  t('Worked hours'),
);

