<?php

/**
 * @file
 * Default view: users_reports
 */

$view = new view();
$view->name = 'users_reports';
$view->description = '';
$view->tag = 'default';
$view->base_table = 'node';
$view->human_name = 'users reports';
$view->core = 7;
$view->api_version = '3.0';
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

/* Display: Master */
$handler = $view->new_display('default', 'Master', 'default');
$handler->display->display_options['use_ajax'] = TRUE;
$handler->display->display_options['use_more_always'] = FALSE;
$handler->display->display_options['access']['type'] = 'perm';
$handler->display->display_options['cache']['type'] = 'none';
$handler->display->display_options['query']['type'] = 'views_query';
$handler->display->display_options['exposed_form']['type'] = 'basic';
$handler->display->display_options['pager']['type'] = 'some';
$handler->display->display_options['pager']['options']['items_per_page'] = '10';
$handler->display->display_options['pager']['options']['offset'] = '0';
$handler->display->display_options['style_plugin'] = 'table';
$handler->display->display_options['style_options']['columns'] = array(
  'created' => 'created',
  'title_field_et' => 'title_field_et',
  'field_hours_worked' => 'field_hours_worked',
  'field_verification_status' => 'field_verification_status',
);
$handler->display->display_options['style_options']['class'] = '';
$handler->display->display_options['style_options']['default'] = '-1';
$handler->display->display_options['style_options']['info'] = array(
  'created' => array(
    'sortable' => 1,
    'default_sort_order' => 'desc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'title_field_et' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_hours_worked' => array(
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
);
/* Header: Global: Text area */
$handler->display->display_options['header']['area_1']['id'] = 'area_1';
$handler->display->display_options['header']['area_1']['table'] = 'views';
$handler->display->display_options['header']['area_1']['field'] = 'area';
$handler->display->display_options['header']['area_1']['empty'] = TRUE;
$handler->display->display_options['header']['area_1']['content'] = 'Most recent work reports';
$handler->display->display_options['header']['area_1']['format'] = 'plain_text';
/* Field: Content: Post date */
$handler->display->display_options['fields']['created']['id'] = 'created';
$handler->display->display_options['fields']['created']['table'] = 'node';
$handler->display->display_options['fields']['created']['field'] = 'created';
$handler->display->display_options['fields']['created']['date_format'] = 'short_timestamp_with_spelled_month';
$handler->display->display_options['fields']['created']['second_date_format'] = 'privatemsg_current_day';
/* Field: Entity translation: Title: translated */
$handler->display->display_options['fields']['title_field_et']['id'] = 'title_field_et';
$handler->display->display_options['fields']['title_field_et']['table'] = 'field_data_title_field';
$handler->display->display_options['fields']['title_field_et']['field'] = 'title_field_et';
$handler->display->display_options['fields']['title_field_et']['label'] = 'Title';
$handler->display->display_options['fields']['title_field_et']['type'] = 'title_linked';
$handler->display->display_options['fields']['title_field_et']['settings'] = array(
  'title_style' => '_none',
  'title_link' => 'content',
  'enable_trim' => 0,
  'trim_length' => '50',
  'title_class' => '',
);
/* Field: Content: Time worked */
$handler->display->display_options['fields']['field_hours_worked']['id'] = 'field_hours_worked';
$handler->display->display_options['fields']['field_hours_worked']['table'] = 'field_data_field_hours_worked';
$handler->display->display_options['fields']['field_hours_worked']['field'] = 'field_hours_worked';
$handler->display->display_options['fields']['field_hours_worked']['type'] = 'custom_formatters_labor_time_formatter';
$handler->display->display_options['fields']['field_hours_worked']['settings'] = array(
  'thousand_separator' => '',
  'decimal_separator' => '.',
  'scale' => '2',
  'prefix_suffix' => 1,
);
/* Field: Content: Verification status */
$handler->display->display_options['fields']['field_verification_status']['id'] = 'field_verification_status';
$handler->display->display_options['fields']['field_verification_status']['table'] = 'field_data_field_verification_status';
$handler->display->display_options['fields']['field_verification_status']['field'] = 'field_verification_status';
$handler->display->display_options['fields']['field_verification_status']['type'] = 'i18n_list_default';
/* Sort criterion: Content: Post date */
$handler->display->display_options['sorts']['created']['id'] = 'created';
$handler->display->display_options['sorts']['created']['table'] = 'node';
$handler->display->display_options['sorts']['created']['field'] = 'created';
$handler->display->display_options['sorts']['created']['order'] = 'DESC';
/* Contextual filter: Content: Author uid */
$handler->display->display_options['arguments']['uid']['id'] = 'uid';
$handler->display->display_options['arguments']['uid']['table'] = 'node';
$handler->display->display_options['arguments']['uid']['field'] = 'uid';
$handler->display->display_options['arguments']['uid']['default_argument_type'] = 'fixed';
$handler->display->display_options['arguments']['uid']['summary']['number_of_records'] = '0';
$handler->display->display_options['arguments']['uid']['summary']['format'] = 'default_summary';
$handler->display->display_options['arguments']['uid']['summary_options']['items_per_page'] = '25';
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
$translatables['users_reports'] = array(
  t('Master'),
  t('more'),
  t('Apply'),
  t('Reset'),
  t('Sort by'),
  t('Asc'),
  t('Desc'),
  t('Most recent work reports'),
  t('Post date'),
  t('Title'),
  t('Time worked'),
  t('Verification status'),
  t('All'),
);

