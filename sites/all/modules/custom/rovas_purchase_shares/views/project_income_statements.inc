<?php

/**
 * @file
 * Default view: project_income_statements
 */

$view = new view();
$view->name = 'project_income_statements';
$view->description = '';
$view->tag = 'default';
$view->base_table = 'node';
$view->human_name = 'Project Income statements';
$view->core = 7;
$view->api_version = '3.0';
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

/* Display: Master */
$handler = $view->new_display('default', 'Master', 'default');
$handler->display->display_options['title'] = 'Project Income statements';
$handler->display->display_options['use_more_always'] = FALSE;
$handler->display->display_options['access']['type'] = 'php';
$handler->display->display_options['access']['php_access'] = 'global $user;

if ($user->uid == 1) {
  return TRUE;
}

$project_id = arg(1);
if (empty($project_id)) {
  $project_id = 35384;
}

$project_node = node_load($project_id);

// Grant access to the project node author.
if ($project_node->uid == $user->uid) {
  return TRUE;
}

$privileged_users = field_get_items(\'node\', $project_node, \'field_privileged_users\');

if (empty($privileged_users)) {
  return FALSE;
}

foreach ($privileged_users as $item) {
  if ($item[\'target_id\'] == $user->uid) {
    return TRUE;
  }
}

return FALSE;';
$handler->display->display_options['cache']['type'] = 'none';
$handler->display->display_options['query']['type'] = 'views_query';
$handler->display->display_options['exposed_form']['type'] = 'basic';
$handler->display->display_options['pager']['type'] = 'full';
$handler->display->display_options['pager']['options']['items_per_page'] = '10';
$handler->display->display_options['style_plugin'] = 'table';
$handler->display->display_options['style_options']['columns'] = array(
  'title' => 'title',
  'field_amount' => 'field_amount',
  'field_closing_date' => 'field_closing_date',
);
$handler->display->display_options['style_options']['class'] = '';
$handler->display->display_options['style_options']['default'] = 'field_closing_date';
$handler->display->display_options['style_options']['info'] = array(
  'title' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_amount' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_closing_date' => array(
    'sortable' => 1,
    'default_sort_order' => 'desc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
);
/* Header: Global: PHP */
$handler->display->display_options['header']['php']['id'] = 'php';
$handler->display->display_options['header']['php']['table'] = 'views';
$handler->display->display_options['header']['php']['field'] = 'php';
$handler->display->display_options['header']['php']['label'] = 'Accounting movements for project';
$handler->display->display_options['header']['php']['php_output'] = '<?php
$project_id = arg(1);
if (empty($project_id)) {
  $project_id = 35384;
}
$node = node_load($project_id);
$path = drupal_get_path_alias(\'node/\' . $project_id);
print \'<a style="font-size: 16px; font-weight: bold;" href="/\' . $path . \'">\' . check_plain($node->title) . \'</a>\';
?>';
/* Field: Content: Title */
$handler->display->display_options['fields']['title']['id'] = 'title';
$handler->display->display_options['fields']['title']['table'] = 'node';
$handler->display->display_options['fields']['title']['field'] = 'title';
$handler->display->display_options['fields']['title']['alter']['word_boundary'] = FALSE;
$handler->display->display_options['fields']['title']['alter']['ellipsis'] = FALSE;
/* Field: Content: Amount */
$handler->display->display_options['fields']['field_amount']['id'] = 'field_amount';
$handler->display->display_options['fields']['field_amount']['table'] = 'field_data_field_amount';
$handler->display->display_options['fields']['field_amount']['field'] = 'field_amount';
$handler->display->display_options['fields']['field_amount']['settings'] = array(
  'thousand_separator' => '',
  'decimal_separator' => '.',
  'scale' => '2',
  'prefix_suffix' => 1,
);
/* Field: Content: Closing date */
$handler->display->display_options['fields']['field_closing_date']['id'] = 'field_closing_date';
$handler->display->display_options['fields']['field_closing_date']['table'] = 'field_data_field_closing_date';
$handler->display->display_options['fields']['field_closing_date']['field'] = 'field_closing_date';
$handler->display->display_options['fields']['field_closing_date']['label'] = 'Billing period';
$handler->display->display_options['fields']['field_closing_date']['settings'] = array(
  'format_type' => 'long',
  'custom_date_format' => '',
  'fromto' => 'both',
  'multiple_number' => '',
  'multiple_from' => '',
  'multiple_to' => '',
  'show_remaining_days' => 0,
);
/* Sort criterion: Content: Post date */
$handler->display->display_options['sorts']['created']['id'] = 'created';
$handler->display->display_options['sorts']['created']['table'] = 'node';
$handler->display->display_options['sorts']['created']['field'] = 'created';
$handler->display->display_options['sorts']['created']['order'] = 'DESC';
/* Contextual filter: Content: Affected node (field_affected_node) */
$handler->display->display_options['arguments']['field_affected_node_target_id']['id'] = 'field_affected_node_target_id';
$handler->display->display_options['arguments']['field_affected_node_target_id']['table'] = 'field_data_field_affected_node';
$handler->display->display_options['arguments']['field_affected_node_target_id']['field'] = 'field_affected_node_target_id';
$handler->display->display_options['arguments']['field_affected_node_target_id']['default_action'] = 'default';
$handler->display->display_options['arguments']['field_affected_node_target_id']['default_argument_type'] = 'php';
$handler->display->display_options['arguments']['field_affected_node_target_id']['default_argument_options']['code'] = '$project_id = arg(1);
if (empty($project_id)) {
return 35384;
}
else {
return $project_id;
}';
$handler->display->display_options['arguments']['field_affected_node_target_id']['summary']['number_of_records'] = '0';
$handler->display->display_options['arguments']['field_affected_node_target_id']['summary']['format'] = 'default_summary';
$handler->display->display_options['arguments']['field_affected_node_target_id']['summary_options']['items_per_page'] = '25';
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
  'accounting_movements_nodes' => 'accounting_movements_nodes',
);

/* Display: Page */
$handler = $view->new_display('page', 'Page', 'page');
$handler->display->display_options['path'] = 'project-income-statements';
$translatables['project_income_statements'] = array(
  t('Master'),
  t('Project Income statements'),
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
  t('Accounting movements for project'),
  t('Title'),
  t('Amount'),
  t('Billing period'),
  t('All'),
  t('Page'),
);

