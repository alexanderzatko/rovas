<?php

/**
 * @file
 * Default view: investment
 */

$view = new view();
$view->name = 'investment';
$view->description = '';
$view->tag = 'default';
$view->base_table = 'node';
$view->human_name = 'Investment';
$view->core = 7;
$view->api_version = '3.0';
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

/* Display: Master */
$handler = $view->new_display('default', 'Master', 'default');
$handler->display->display_options['title'] = 'Investing in projects';
$handler->display->display_options['use_more_always'] = FALSE;
$handler->display->display_options['access']['type'] = 'role';
$handler->display->display_options['access']['role'] = array(
  2 => '2',
);
$handler->display->display_options['cache']['type'] = 'none';
$handler->display->display_options['query']['type'] = 'views_query';
$handler->display->display_options['exposed_form']['type'] = 'basic';
$handler->display->display_options['pager']['type'] = 'full';
$handler->display->display_options['pager']['options']['items_per_page'] = '10';
$handler->display->display_options['pager']['options']['offset'] = '0';
$handler->display->display_options['pager']['options']['id'] = '0';
$handler->display->display_options['pager']['options']['quantity'] = '3';
$handler->display->display_options['pager']['options']['tags']['first'] = '«';
$handler->display->display_options['pager']['options']['tags']['previous'] = '‹';
$handler->display->display_options['pager']['options']['tags']['next'] = '›';
$handler->display->display_options['pager']['options']['tags']['last'] = '»';
$handler->display->display_options['style_plugin'] = 'table';
$handler->display->display_options['style_options']['row_class'] = 'table-investing';
$handler->display->display_options['style_options']['columns'] = array(
  'name' => 'name',
  'title' => 'title',
  'field_merit_reward' => 'field_merit_reward',
  'field_project_total_hours' => 'field_project_total_hours',
  'field_rate' => 'field_rate',
  'php' => 'php',
  'php_2' => 'php_2',
);
$handler->display->display_options['style_options']['class'] = '';
$handler->display->display_options['style_options']['default'] = 'php';
$handler->display->display_options['style_options']['info'] = array(
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
  'field_merit_reward' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_project_total_hours' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_rate' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'php' => array(
    'sortable' => 1,
    'default_sort_order' => 'desc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'php_2' => array(
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
);
/* Header: Global: Text area */
$handler->display->display_options['header']['area']['id'] = 'area';
$handler->display->display_options['header']['area']['table'] = 'views';
$handler->display->display_options['header']['area']['field'] = 'area';
$handler->display->display_options['header']['area']['content'] = 'Shown below is a list of projects, set up by their owners to allow buying shares.
Owning one hour share has the same Merit-earning effect as having reported one hour of work in the project.';
$handler->display->display_options['header']['area']['format'] = 'filtered_html';
/* Footer: Global: Text area */
$handler->display->display_options['footer']['area']['id'] = 'area';
$handler->display->display_options['footer']['area']['table'] = 'views';
$handler->display->display_options['footer']['area']['field'] = 'area';
$handler->display->display_options['footer']['area']['content'] = 'If you do not see a project you want to reward in the list, try suggesting the project owner to allow investments.';
$handler->display->display_options['footer']['area']['format'] = 'filtered_html';
/* Relationship: Content: Content author */
$handler->display->display_options['relationships']['uid']['id'] = 'uid';
$handler->display->display_options['relationships']['uid']['table'] = 'node';
$handler->display->display_options['relationships']['uid']['field'] = 'uid';
/* Field: User: Name */
$handler->display->display_options['fields']['name']['id'] = 'name';
$handler->display->display_options['fields']['name']['table'] = 'users';
$handler->display->display_options['fields']['name']['field'] = 'name';
$handler->display->display_options['fields']['name']['relationship'] = 'uid';
$handler->display->display_options['fields']['name']['label'] = 'Owner';
/* Field: Entity translation: Title: translated */
$handler->display->display_options['fields']['title_field_et']['id'] = 'title_field_et';
$handler->display->display_options['fields']['title_field_et']['table'] = 'field_data_title_field';
$handler->display->display_options['fields']['title_field_et']['field'] = 'title_field_et';
$handler->display->display_options['fields']['title_field_et']['label'] = 'Project';
$handler->display->display_options['fields']['title_field_et']['type'] = 'title_linked';
$handler->display->display_options['fields']['title_field_et']['settings'] = array(
  'title_style' => '_none',
  'title_link' => 'content',
  'enable_trim' => 0,
  'trim_length' => '50',
  'title_class' => '',
);
/* Field: Content: Merit reward */
$handler->display->display_options['fields']['field_merit_reward']['id'] = 'field_merit_reward';
$handler->display->display_options['fields']['field_merit_reward']['table'] = 'field_data_field_merit_reward';
$handler->display->display_options['fields']['field_merit_reward']['field'] = 'field_merit_reward';
$handler->display->display_options['fields']['field_merit_reward']['label'] = 'Reward';
$handler->display->display_options['fields']['field_merit_reward']['settings'] = array(
  'thousand_separator' => '',
  'decimal_separator' => '.',
  'scale' => '2',
  'prefix_suffix' => 0,
);
/* Field: Content: Time worked */
$handler->display->display_options['fields']['field_project_total_hours']['id'] = 'field_project_total_hours';
$handler->display->display_options['fields']['field_project_total_hours']['table'] = 'field_data_field_project_total_hours';
$handler->display->display_options['fields']['field_project_total_hours']['field'] = 'field_project_total_hours';
$handler->display->display_options['fields']['field_project_total_hours']['label'] = 'Time';
$handler->display->display_options['fields']['field_project_total_hours']['type'] = 'custom_formatters_labor_time_formatter';
$handler->display->display_options['fields']['field_project_total_hours']['settings'] = array(
  'thousand_separator' => '',
  'decimal_separator' => '.',
  'scale' => '2',
  'prefix_suffix' => 1,
);
/* Field: Content: Time share cost */
$handler->display->display_options['fields']['field_rate']['id'] = 'field_rate';
$handler->display->display_options['fields']['field_rate']['table'] = 'field_data_field_rate';
$handler->display->display_options['fields']['field_rate']['field'] = 'field_rate';
$handler->display->display_options['fields']['field_rate']['label'] = 'Cost';
$handler->display->display_options['fields']['field_rate']['settings'] = array(
  'thousand_separator' => '',
  'decimal_separator' => '.',
  'scale' => '2',
  'prefix_suffix' => 0,
);
/* Field: yield */
$handler->display->display_options['fields']['php']['id'] = 'php';
$handler->display->display_options['fields']['php']['table'] = 'views';
$handler->display->display_options['fields']['php']['field'] = 'php';
$handler->display->display_options['fields']['php']['ui_name'] = 'yield';
$handler->display->display_options['fields']['php']['label'] = 'Yield';
$handler->display->display_options['fields']['php']['use_php_setup'] = 0;
$handler->display->display_options['fields']['php']['php_value'] = '$obj = entity_metadata_wrapper(\'node\',$data->nid);
return rovas_purchase_shares_yield($obj);';
$handler->display->display_options['fields']['php']['php_output'] = '<?php 
echo round($value,2);
?>';
$handler->display->display_options['fields']['php']['use_php_click_sortable'] = '4';
$handler->display->display_options['fields']['php']['php_click_sortable'] = '';
/* Field: Global: PHP */
$handler->display->display_options['fields']['php_2']['id'] = 'php_2';
$handler->display->display_options['fields']['php_2']['table'] = 'views';
$handler->display->display_options['fields']['php_2']['field'] = 'php';
$handler->display->display_options['fields']['php_2']['label'] = 'Action';
$handler->display->display_options['fields']['php_2']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['php_2']['use_php_setup'] = 0;
$handler->display->display_options['fields']['php_2']['php_output'] = '<?php 

$tip = t(\'Show the investment configuration page\');

echo \'<a href="/buyshares/\' . $data->nid . \'" title="\'.$tip.\'">\';
echo \'<i class="glyphicon glyphicon-cog"></i>\';
//echo t(\'detail\');
echo \'</a>\';
?>';
$handler->display->display_options['fields']['php_2']['use_php_click_sortable'] = '0';
$handler->display->display_options['fields']['php_2']['php_click_sortable'] = '';
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
/* Filter criterion: Content: Allow (field_allow_purchasing_shares) */
$handler->display->display_options['filters']['field_allow_purchasing_shares_value']['id'] = 'field_allow_purchasing_shares_value';
$handler->display->display_options['filters']['field_allow_purchasing_shares_value']['table'] = 'field_data_field_allow_purchasing_shares';
$handler->display->display_options['filters']['field_allow_purchasing_shares_value']['field'] = 'field_allow_purchasing_shares_value';
$handler->display->display_options['filters']['field_allow_purchasing_shares_value']['value'] = array(
  1 => '1',
);

/* Display: Page */
$handler = $view->new_display('page', 'Page', 'page');
$handler->display->display_options['path'] = 'invest';
$handler->display->display_options['menu']['type'] = 'normal';
$handler->display->display_options['menu']['title'] = 'Investing';
$handler->display->display_options['menu']['description'] = 'Purchase Merit shares in projects';
$handler->display->display_options['menu']['weight'] = '-3';
$handler->display->display_options['menu']['name'] = 'main-menu';
$handler->display->display_options['menu']['context'] = 0;
$handler->display->display_options['menu']['context_only_inline'] = 0;
$translatables['investment'] = array(
  t('Master'),
  t('Investing in projects'),
  t('more'),
  t('Apply'),
  t('Reset'),
  t('Sort by'),
  t('Asc'),
  t('Desc'),
  t('Items per page'),
  t('- All -'),
  t('Offset'),
  t('«'),
  t('‹'),
  t('›'),
  t('»'),
  t('Shown below is a list of projects, set up by their owners to allow buying shares.
Owning one hour share has the same Merit-earning effect as having reported one hour of work in the project.'),
  t('If you do not see a project you want to reward in the list, try suggesting the project owner to allow investments.'),
  t('author'),
  t('Owner'),
  t('Project'),
  t('Reward'),
  t('Time'),
  t('Cost'),
  t('Yield'),
  t('Action'),
  t('Page'),
);

