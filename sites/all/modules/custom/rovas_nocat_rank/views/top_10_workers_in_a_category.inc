<?php

/**
 * @file
 * Default view: top_10_workers_in_a_category
 */

$view = new view();
$view->name = 'top_10_workers_in_a_category';
$view->description = '';
$view->tag = 'default';
$view->base_table = 'node';
$view->human_name = 'Top 10 workers in a category';
$view->core = 7;
$view->api_version = '3.0';
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

/* Display: Master */
$handler = $view->new_display('default', 'Master', 'default');
$handler->display->display_options['title'] = 'Top 10 workers in category';
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
$handler->display->display_options['style_options']['row_class'] = '[php]';
$handler->display->display_options['style_options']['columns'] = array(
  'uid' => 'uid',
  'php' => 'php',
  'counter' => 'counter',
  'field_full_name' => 'field_full_name',
  'name' => 'name',
  'field_merit_reward' => 'field_merit_reward',
);
$handler->display->display_options['style_options']['default'] = '-1';
$handler->display->display_options['style_options']['info'] = array(
  'uid' => array(
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'php' => array(
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'counter' => array(
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_full_name' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'name' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_merit_reward' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
);
/* Header: Global: Text area */
$handler->display->display_options['header']['area']['id'] = 'area';
$handler->display->display_options['header']['area']['table'] = 'views';
$handler->display->display_options['header']['area']['field'] = 'area';
$handler->display->display_options['header']['area']['content'] = '<?php 
echo t(\'Category\') . \': \';
echo \'<h4>\';

if (is_numeric(arg(1)))  {
$term_w = entity_metadata_wrapper(\'taxonomy_term\', taxonomy_term_load(arg(1)));
$term_name = $term_w->label();
echo \'<a href="/taxonomy/term/\' . arg(1) . \'">\' . $term_name . \'</a>\';
}
else {
echo t(\'All combined\');
}
echo \'</h4>\';

?>';
$handler->display->display_options['header']['area']['format'] = 'php_code';
/* Relationship: Content: Content author */
$handler->display->display_options['relationships']['uid']['id'] = 'uid';
$handler->display->display_options['relationships']['uid']['table'] = 'node';
$handler->display->display_options['relationships']['uid']['field'] = 'uid';
/* Field: Content: Author uid */
$handler->display->display_options['fields']['uid']['id'] = 'uid';
$handler->display->display_options['fields']['uid']['table'] = 'node';
$handler->display->display_options['fields']['uid']['field'] = 'uid';
$handler->display->display_options['fields']['uid']['label'] = 'uid';
$handler->display->display_options['fields']['uid']['exclude'] = TRUE;
$handler->display->display_options['fields']['uid']['link_to_user'] = FALSE;
/* Field: Global: View result counter */
$handler->display->display_options['fields']['counter']['id'] = 'counter';
$handler->display->display_options['fields']['counter']['table'] = 'views';
$handler->display->display_options['fields']['counter']['field'] = 'counter';
$handler->display->display_options['fields']['counter']['label'] = 'Position';
$handler->display->display_options['fields']['counter']['counter_start'] = '1';
$handler->display->display_options['fields']['counter']['reverse'] = 0;
/* Field: User: Name */
$handler->display->display_options['fields']['name']['id'] = 'name';
$handler->display->display_options['fields']['name']['table'] = 'users';
$handler->display->display_options['fields']['name']['field'] = 'name';
$handler->display->display_options['fields']['name']['relationship'] = 'uid';
$handler->display->display_options['fields']['name']['label'] = 'Worker';
$handler->display->display_options['fields']['name']['link_to_user'] = FALSE;
/* Field: SUM(Content: Merit reward) */
$handler->display->display_options['fields']['field_merit_reward']['id'] = 'field_merit_reward';
$handler->display->display_options['fields']['field_merit_reward']['table'] = 'field_data_field_merit_reward';
$handler->display->display_options['fields']['field_merit_reward']['field'] = 'field_merit_reward';
$handler->display->display_options['fields']['field_merit_reward']['group_type'] = 'sum';
$handler->display->display_options['fields']['field_merit_reward']['label'] = 'Merits';
$handler->display->display_options['fields']['field_merit_reward']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_merit_reward']['settings'] = array(
  'thousand_separator' => '',
  'decimal_separator' => '.',
  'scale' => '2',
  'prefix_suffix' => 1,
);
/* Field: Global: PHP */
$handler->display->display_options['fields']['php_1']['id'] = 'php_1';
$handler->display->display_options['fields']['php_1']['table'] = 'views';
$handler->display->display_options['fields']['php_1']['field'] = 'php';
$handler->display->display_options['fields']['php_1']['label'] = 'Merits';
$handler->display->display_options['fields']['php_1']['use_php_setup'] = 0;
$handler->display->display_options['fields']['php_1']['php_output'] = '<?php 
if (!($row->field_merit_reward))
echo 0;
else
echo $row->field_merit_reward;
?>';
$handler->display->display_options['fields']['php_1']['use_php_click_sortable'] = '0';
$handler->display->display_options['fields']['php_1']['php_click_sortable'] = '';
/* Sort criterion: SUM(Content: Merit reward (field_merit_reward)) */
$handler->display->display_options['sorts']['field_merit_reward_value']['id'] = 'field_merit_reward_value';
$handler->display->display_options['sorts']['field_merit_reward_value']['table'] = 'field_data_field_merit_reward';
$handler->display->display_options['sorts']['field_merit_reward_value']['field'] = 'field_merit_reward_value';
$handler->display->display_options['sorts']['field_merit_reward_value']['group_type'] = 'sum';
$handler->display->display_options['sorts']['field_merit_reward_value']['order'] = 'DESC';
/* Contextual filter: Field: Classification (field_classification) */
$handler->display->display_options['arguments']['field_classification_tid']['id'] = 'field_classification_tid';
$handler->display->display_options['arguments']['field_classification_tid']['table'] = 'field_data_field_classification';
$handler->display->display_options['arguments']['field_classification_tid']['field'] = 'field_classification_tid';
$handler->display->display_options['arguments']['field_classification_tid']['default_action'] = 'default';
$handler->display->display_options['arguments']['field_classification_tid']['default_argument_type'] = 'raw';
$handler->display->display_options['arguments']['field_classification_tid']['default_argument_options']['index'] = '0';
$handler->display->display_options['arguments']['field_classification_tid']['default_argument_options']['use_alias'] = TRUE;
$handler->display->display_options['arguments']['field_classification_tid']['summary']['number_of_records'] = '0';
$handler->display->display_options['arguments']['field_classification_tid']['summary']['format'] = 'default_summary';
$handler->display->display_options['arguments']['field_classification_tid']['summary_options']['items_per_page'] = '25';
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
$handler->display->display_options['enabled'] = FALSE;
$handler->display->display_options['defaults']['fields'] = FALSE;
/* Field: Content: Author uid */
$handler->display->display_options['fields']['uid']['id'] = 'uid';
$handler->display->display_options['fields']['uid']['table'] = 'node';
$handler->display->display_options['fields']['uid']['field'] = 'uid';
$handler->display->display_options['fields']['uid']['label'] = 'uid';
$handler->display->display_options['fields']['uid']['exclude'] = TRUE;
$handler->display->display_options['fields']['uid']['link_to_user'] = FALSE;
/* Field: Global: View result counter */
$handler->display->display_options['fields']['counter']['id'] = 'counter';
$handler->display->display_options['fields']['counter']['table'] = 'views';
$handler->display->display_options['fields']['counter']['field'] = 'counter';
$handler->display->display_options['fields']['counter']['label'] = 'Position';
$handler->display->display_options['fields']['counter']['counter_start'] = '1';
$handler->display->display_options['fields']['counter']['reverse'] = 0;
/* Field: User: Name */
$handler->display->display_options['fields']['name']['id'] = 'name';
$handler->display->display_options['fields']['name']['table'] = 'users';
$handler->display->display_options['fields']['name']['field'] = 'name';
$handler->display->display_options['fields']['name']['relationship'] = 'uid';
$handler->display->display_options['fields']['name']['label'] = 'Worker';
/* Field: SUM(Content: Merit reward) */
$handler->display->display_options['fields']['field_merit_reward']['id'] = 'field_merit_reward';
$handler->display->display_options['fields']['field_merit_reward']['table'] = 'field_data_field_merit_reward';
$handler->display->display_options['fields']['field_merit_reward']['field'] = 'field_merit_reward';
$handler->display->display_options['fields']['field_merit_reward']['group_type'] = 'sum';
$handler->display->display_options['fields']['field_merit_reward']['label'] = 'Merits';
$handler->display->display_options['fields']['field_merit_reward']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_merit_reward']['settings'] = array(
  'thousand_separator' => '',
  'decimal_separator' => '.',
  'scale' => '2',
  'prefix_suffix' => 1,
);
/* Field: Global: PHP */
$handler->display->display_options['fields']['php_1']['id'] = 'php_1';
$handler->display->display_options['fields']['php_1']['table'] = 'views';
$handler->display->display_options['fields']['php_1']['field'] = 'php';
$handler->display->display_options['fields']['php_1']['label'] = 'Merits';
$handler->display->display_options['fields']['php_1']['use_php_setup'] = 0;
$handler->display->display_options['fields']['php_1']['php_output'] = '<?php 
if (!($data->field_data_field_merit_reward_field_merit_reward_value))
echo 0;
else
echo round($data->field_data_field_merit_reward_field_merit_reward_value,2);
?>';
$handler->display->display_options['fields']['php_1']['use_php_click_sortable'] = '0';
$handler->display->display_options['fields']['php_1']['php_click_sortable'] = '';
/* Field: Global: PHP */
$handler->display->display_options['fields']['php']['id'] = 'php';
$handler->display->display_options['fields']['php']['table'] = 'views';
$handler->display->display_options['fields']['php']['field'] = 'php';
$handler->display->display_options['fields']['php']['label'] = '';
$handler->display->display_options['fields']['php']['exclude'] = TRUE;
$handler->display->display_options['fields']['php']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['php']['use_php_setup'] = 0;
$handler->display->display_options['fields']['php']['php_output'] = '<?php 

global $user;
echo $user->uid == $row->uid ? \'current\' : \'\';

?>';
$handler->display->display_options['fields']['php']['use_php_click_sortable'] = '0';
$handler->display->display_options['fields']['php']['php_click_sortable'] = '';
$handler->display->display_options['path'] = 'top-10-workers-in-a-category';

/* Display: Services */
$handler = $view->new_display('services', 'Services', 'services_1');
$handler->display->display_options['defaults']['style_plugin'] = FALSE;
$handler->display->display_options['style_plugin'] = 'default';
$handler->display->display_options['defaults']['style_options'] = FALSE;
$handler->display->display_options['defaults']['row_plugin'] = FALSE;
$handler->display->display_options['row_plugin'] = 'fields';
$handler->display->display_options['row_options']['default_field_elements'] = FALSE;
$handler->display->display_options['defaults']['row_options'] = FALSE;
$handler->display->display_options['defaults']['relationships'] = FALSE;
/* Relationship: Content: Content author */
$handler->display->display_options['relationships']['uid']['id'] = 'uid';
$handler->display->display_options['relationships']['uid']['table'] = 'node';
$handler->display->display_options['relationships']['uid']['field'] = 'uid';
/* Relationship: Field: Classification (field_classification) */
$handler->display->display_options['relationships']['field_classification_tid']['id'] = 'field_classification_tid';
$handler->display->display_options['relationships']['field_classification_tid']['table'] = 'field_data_field_classification';
$handler->display->display_options['relationships']['field_classification_tid']['field'] = 'field_classification_tid';
$handler->display->display_options['relationships']['field_classification_tid']['label'] = 'classification';
$handler->display->display_options['defaults']['fields'] = FALSE;
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
$handler->display->display_options['defaults']['arguments'] = FALSE;
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
  'work_report' => 'work_report',
);
$handler->display->display_options['filters']['type']['group'] = 1;
/* Filter criterion: Content: Author uid */
$handler->display->display_options['filters']['uid']['id'] = 'uid';
$handler->display->display_options['filters']['uid']['table'] = 'node';
$handler->display->display_options['filters']['uid']['field'] = 'uid';
$handler->display->display_options['filters']['uid']['operator'] = 'not in';
$handler->display->display_options['filters']['uid']['value'] = array(
  0 => '1',
);
$handler->display->display_options['filters']['uid']['group'] = 1;
/* Filter criterion: Taxonomy term: Term ID */
$handler->display->display_options['filters']['tid_raw']['id'] = 'tid_raw';
$handler->display->display_options['filters']['tid_raw']['table'] = 'taxonomy_term_data';
$handler->display->display_options['filters']['tid_raw']['field'] = 'tid_raw';
$handler->display->display_options['filters']['tid_raw']['relationship'] = 'field_classification_tid';
$handler->display->display_options['filters']['tid_raw']['exposed'] = TRUE;
$handler->display->display_options['filters']['tid_raw']['expose']['operator_id'] = 'tid_raw_op';
$handler->display->display_options['filters']['tid_raw']['expose']['label'] = 'term_id';
$handler->display->display_options['filters']['tid_raw']['expose']['operator'] = 'tid_raw_op';
$handler->display->display_options['filters']['tid_raw']['expose']['identifier'] = 'tid_raw';
$handler->display->display_options['filters']['tid_raw']['expose']['remember_roles'] = array(
  2 => '2',
);
/* Filter criterion: Content: Web Address (field_web_address) */
$handler->display->display_options['filters']['field_web_address_value']['id'] = 'field_web_address_value';
$handler->display->display_options['filters']['field_web_address_value']['table'] = 'field_data_field_web_address';
$handler->display->display_options['filters']['field_web_address_value']['field'] = 'field_web_address_value';
$handler->display->display_options['filters']['field_web_address_value']['operator'] = 'contains';
$handler->display->display_options['filters']['field_web_address_value']['exposed'] = TRUE;
$handler->display->display_options['filters']['field_web_address_value']['expose']['operator_id'] = 'field_web_address_value_op';
$handler->display->display_options['filters']['field_web_address_value']['expose']['label'] = 'Web Address (field_web_address)';
$handler->display->display_options['filters']['field_web_address_value']['expose']['operator'] = 'field_web_address_value_op';
$handler->display->display_options['filters']['field_web_address_value']['expose']['identifier'] = 'field_web_address_value';
$handler->display->display_options['filters']['field_web_address_value']['expose']['remember_roles'] = array(
  2 => '2',
);
$handler->display->display_options['filters']['field_web_address_value']['expose']['autocomplete_items'] = '10';
$handler->display->display_options['filters']['field_web_address_value']['expose']['autocomplete_min_chars'] = '0';
$handler->display->display_options['filters']['field_web_address_value']['expose']['autocomplete_raw_suggestion'] = 1;
$handler->display->display_options['filters']['field_web_address_value']['expose']['autocomplete_raw_dropdown'] = 1;
$handler->display->display_options['filters']['field_web_address_value']['expose']['autocomplete_dependent'] = 0;
$handler->display->display_options['path'] = 'rest_view_top_10_in_category';
$translatables['top_10_workers_in_a_category'] = array(
  t('Master'),
  t('Top 10 workers in category'),
  t('more'),
  t('Apply'),
  t('Reset'),
  t('Sort by'),
  t('Asc'),
  t('Desc'),
  t('<?php 
echo t(\'Category\') . \': \';
echo \'<h4>\';

if (is_numeric(arg(1)))  {
$term_w = entity_metadata_wrapper(\'taxonomy_term\', taxonomy_term_load(arg(1)));
$term_name = $term_w->label();
echo \'<a href="/taxonomy/term/\' . arg(1) . \'">\' . $term_name . \'</a>\';
}
else {
echo t(\'All combined\');
}
echo \'</h4>\';

?>'),
  t('author'),
  t('uid'),
  t('Position'),
  t('Worker'),
  t('Merits'),
  t('All'),
  t('Page'),
  t('Services'),
  t('classification'),
  t('rank'),
  t('rovas_uid'),
  t('merits'),
  t('term_id'),
  t('Web Address (field_web_address)'),
);

