<?php

/**
 * @file
 * Default view: labor_reported_chart
 */

$view = new view();
$view->name = 'labor_reported_chart';
$view->description = '';
$view->tag = 'default';
$view->base_table = 'node';
$view->human_name = 'Labor reported chart';
$view->core = 7;
$view->api_version = '3.0';
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

/* Display: Master */
$handler = $view->new_display('default', 'Master', 'default');
$handler->display->display_options['title'] = 'Labor reported';
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
  'field_hours_worked' => 'field_hours_worked',
  'field_date' => 0,
  'raw_field' => 0,
);
$handler->display->display_options['style_options']['field_colors'] = array(
  'field_hours_worked' => '#3592f4',
  'field_date' => '#8bbc21',
  'raw_field' => '#8bbc21',
);
$handler->display->display_options['style_options']['data_labels'] = '';
$handler->display->display_options['style_options']['tooltips'] = 'TRUE';
$handler->display->display_options['style_options']['width'] = '';
$handler->display->display_options['style_options']['height'] = '';
$handler->display->display_options['style_options']['xaxis_title'] = 'date';
$handler->display->display_options['style_options']['xaxis_labels_rotation'] = '0';
$handler->display->display_options['style_options']['yaxis_title'] = 'hours';
$handler->display->display_options['style_options']['yaxis_labels_rotation'] = '0';
/* Field: SUM(Content: Time worked) */
$handler->display->display_options['fields']['field_hours_worked']['id'] = 'field_hours_worked';
$handler->display->display_options['fields']['field_hours_worked']['table'] = 'field_data_field_hours_worked';
$handler->display->display_options['fields']['field_hours_worked']['field'] = 'field_hours_worked';
$handler->display->display_options['fields']['field_hours_worked']['group_type'] = 'sum';
$handler->display->display_options['fields']['field_hours_worked']['settings'] = array(
  'thousand_separator' => '',
  'decimal_separator' => '.',
  'scale' => '2',
  'prefix_suffix' => 1,
);
/* Field: COUNT(Content: Date) */
$handler->display->display_options['fields']['field_date']['id'] = 'field_date';
$handler->display->display_options['fields']['field_date']['table'] = 'field_data_field_date';
$handler->display->display_options['fields']['field_date']['field'] = 'field_date';
$handler->display->display_options['fields']['field_date']['group_type'] = 'count';
$handler->display->display_options['fields']['field_date']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_date']['separator'] = ', ';
/* Field: Raw SQL: Raw field */
$handler->display->display_options['fields']['raw_field']['id'] = 'raw_field';
$handler->display->display_options['fields']['raw_field']['table'] = 'views_raw_sql';
$handler->display->display_options['fields']['raw_field']['field'] = 'raw_field';
$handler->display->display_options['fields']['raw_field']['raw_sql'] = 'DATE_FORMAT(field_data_field_date.field_date_value, \'%b %Y\')';
/* Sort criterion: Raw SQL: Raw sort */
$handler->display->display_options['sorts']['raw_sort']['id'] = 'raw_sort';
$handler->display->display_options['sorts']['raw_sort']['table'] = 'views_raw_sql';
$handler->display->display_options['sorts']['raw_sort']['field'] = 'raw_sort';
$handler->display->display_options['sorts']['raw_sort']['raw_sql'] = 'DATE_FORMAT(field_data_field_date.field_date_value, \'%y%c\')';
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
/* Filter criterion: Content: Date (field_date) */
$handler->display->display_options['filters']['field_date_value']['id'] = 'field_date_value';
$handler->display->display_options['filters']['field_date_value']['table'] = 'field_data_field_date';
$handler->display->display_options['filters']['field_date_value']['field'] = 'field_date_value';
$handler->display->display_options['filters']['field_date_value']['operator'] = '>';
$handler->display->display_options['filters']['field_date_value']['granularity'] = 'month';
$handler->display->display_options['filters']['field_date_value']['default_date'] = 'now - 1 year';

/* Display: Block */
$handler = $view->new_display('block', 'Block', 'block');
$translatables['labor_reported_chart'] = array(
  t('Master'),
  t('Labor reported'),
  t('more'),
  t('Apply'),
  t('Reset'),
  t('Sort by'),
  t('Asc'),
  t('Desc'),
  t('Time worked'),
  t('Date'),
  t('Raw field'),
  t('Block'),
);

