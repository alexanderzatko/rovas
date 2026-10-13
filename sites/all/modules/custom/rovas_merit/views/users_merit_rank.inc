<?php

/**
 * @file
 * Default view: users_merit_rank
 */

$view = new view();
$view->name = 'users_merit_rank';
$view->description = 'Table showing users ranked by their merit scores';
$view->tag = 'default';
$view->base_table = 'users';
$view->human_name = 'Users merit rank';
$view->core = 7;
$view->api_version = '3.0';
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

/* Display: Master */
$handler = $view->new_display('default', 'Master', 'default');
$handler->display->display_options['title'] = 'Merit Index';
$handler->display->display_options['use_more_always'] = FALSE;
$handler->display->display_options['access']['type'] = 'role';
$handler->display->display_options['access']['role'] = array(
  3 => '3',
  1 => '1',
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
$handler->display->display_options['style_options']['row_class'] = 'rovas-user';
$handler->display->display_options['style_options']['default_row_class'] = FALSE;
$handler->display->display_options['style_options']['row_class_special'] = FALSE;
$handler->display->display_options['style_options']['columns'] = array(
  'counter' => 'counter',
  'uid' => 'uid',
  'created' => 'created',
  'php_4' => 'php_4',
  'field_full_name' => 'field_full_name',
  'name' => 'name',
  'points' => 'points',
  'php_3' => 'php_3',
  'php_2' => 'php_2',
  'php' => 'php',
  'php_1' => 'php_1',
  'field_country' => 'field_country',
);
$handler->display->display_options['style_options']['class'] = '';
$handler->display->display_options['style_options']['default'] = 'points';
$handler->display->display_options['style_options']['info'] = array(
  'counter' => array(
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'uid' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'created' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'php_4' => array(
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
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'points' => array(
    'sortable' => 1,
    'default_sort_order' => 'desc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'php_3' => array(
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'php_2' => array(
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'php' => array(
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'php_1' => array(
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_country' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => 'views-align-center',
    'separator' => '',
    'empty_column' => 0,
  ),
);
/* Header: Global: Text area */
$handler->display->display_options['header']['area']['id'] = 'area';
$handler->display->display_options['header']['area']['table'] = 'views';
$handler->display->display_options['header']['area']['field'] = 'area';
$handler->display->display_options['header']['area']['label'] = 'Merit list intro';
$handler->display->display_options['header']['area']['content'] = '<img class="meritlogo" src="/sites/default/files/pictures/meritlogo.svg"/>
<span>The <strong>economic value donated to society</strong> by individuals and the domain in which the value was created.</span>';
$handler->display->display_options['header']['area']['format'] = 'full_html';
/* Footer: Global: Text area */
$handler->display->display_options['footer']['area']['id'] = 'area';
$handler->display->display_options['footer']['area']['table'] = 'views';
$handler->display->display_options['footer']['area']['field'] = 'area';
$handler->display->display_options['footer']['area']['content'] = '<span style="font-size: smaller; margin-bottom: 25px; display: block; margin-top: 4px;">*The estimated value is calculated from the average hourly wage in the particular domains in which the donated value was generated. If the average wage in a particular domain is unknown, the rate of 10€/hour is used.</span>';
$handler->display->display_options['footer']['area']['format'] = 'full_html';
/* Relationship: Field: Country (field_country) */
$handler->display->display_options['relationships']['field_country_iso2']['id'] = 'field_country_iso2';
$handler->display->display_options['relationships']['field_country_iso2']['table'] = 'field_data_field_country';
$handler->display->display_options['relationships']['field_country_iso2']['field'] = 'field_country_iso2';
$handler->display->display_options['relationships']['field_country_iso2']['label'] = 'country';
/* Field: Global: View result counter */
$handler->display->display_options['fields']['counter']['id'] = 'counter';
$handler->display->display_options['fields']['counter']['table'] = 'views';
$handler->display->display_options['fields']['counter']['field'] = 'counter';
$handler->display->display_options['fields']['counter']['label'] = '#';
$handler->display->display_options['fields']['counter']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['counter']['counter_start'] = '1';
$handler->display->display_options['fields']['counter']['reverse'] = 0;
/* Field: User: Uid */
$handler->display->display_options['fields']['uid']['id'] = 'uid';
$handler->display->display_options['fields']['uid']['table'] = 'users';
$handler->display->display_options['fields']['uid']['field'] = 'uid';
$handler->display->display_options['fields']['uid']['label'] = '';
$handler->display->display_options['fields']['uid']['exclude'] = TRUE;
$handler->display->display_options['fields']['uid']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['uid']['link_to_user'] = FALSE;
/* Field: User: Created date */
$handler->display->display_options['fields']['created']['id'] = 'created';
$handler->display->display_options['fields']['created']['table'] = 'users';
$handler->display->display_options['fields']['created']['field'] = 'created';
$handler->display->display_options['fields']['created']['label'] = '';
$handler->display->display_options['fields']['created']['exclude'] = TRUE;
$handler->display->display_options['fields']['created']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['created']['date_format'] = 'raw time span';
$handler->display->display_options['fields']['created']['second_date_format'] = 'privatemsg_current_day';
/* Field: User: Full name */
$handler->display->display_options['fields']['field_full_name']['id'] = 'field_full_name';
$handler->display->display_options['fields']['field_full_name']['table'] = 'field_data_field_full_name';
$handler->display->display_options['fields']['field_full_name']['field'] = 'field_full_name';
$handler->display->display_options['fields']['field_full_name']['label'] = '';
$handler->display->display_options['fields']['field_full_name']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_full_name']['element_label_colon'] = FALSE;
/* Field: User: Name */
$handler->display->display_options['fields']['name']['id'] = 'name';
$handler->display->display_options['fields']['name']['table'] = 'users';
$handler->display->display_options['fields']['name']['field'] = 'name';
$handler->display->display_options['fields']['name']['alter']['alter_text'] = TRUE;
$handler->display->display_options['fields']['name']['alter']['text'] = '[name]<br>
<span style="font-size: 11px; font-weight: 400;color: gray;">[created]</span>';
$handler->display->display_options['fields']['name']['alter']['word_boundary'] = FALSE;
$handler->display->display_options['fields']['name']['alter']['ellipsis'] = FALSE;
$handler->display->display_options['fields']['name']['element_label_colon'] = FALSE;
/* Field: Userpoints total: Current total merits */
$handler->display->display_options['fields']['points']['id'] = 'points';
$handler->display->display_options['fields']['points']['table'] = 'userpoints_total';
$handler->display->display_options['fields']['points']['field'] = 'points';
$handler->display->display_options['fields']['points']['label'] = '';
$handler->display->display_options['fields']['points']['exclude'] = TRUE;
$handler->display->display_options['fields']['points']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['points']['separator'] = '';
/* Field: merits */
$handler->display->display_options['fields']['php_3']['id'] = 'php_3';
$handler->display->display_options['fields']['php_3']['table'] = 'views';
$handler->display->display_options['fields']['php_3']['field'] = 'php';
$handler->display->display_options['fields']['php_3']['ui_name'] = 'merits';
$handler->display->display_options['fields']['php_3']['label'] = 'Merits';
$handler->display->display_options['fields']['php_3']['element_type'] = 'span';
$handler->display->display_options['fields']['php_3']['element_class'] = 'merit-score';
$handler->display->display_options['fields']['php_3']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['php_3']['use_php_setup'] = 0;
$handler->display->display_options['fields']['php_3']['php_output'] = '<?php
echo number_format($data->userpoints_total_points,0,\'.\',\' \');
?>';
$handler->display->display_options['fields']['php_3']['use_php_click_sortable'] = '0';
$handler->display->display_options['fields']['php_3']['php_click_sortable'] = '';
/* Field: euros */
$handler->display->display_options['fields']['php_2']['id'] = 'php_2';
$handler->display->display_options['fields']['php_2']['table'] = 'views';
$handler->display->display_options['fields']['php_2']['field'] = 'php';
$handler->display->display_options['fields']['php_2']['ui_name'] = 'euros';
$handler->display->display_options['fields']['php_2']['label'] = 'Value* (€)';
$handler->display->display_options['fields']['php_2']['element_type'] = 'span';
$handler->display->display_options['fields']['php_2']['element_class'] = 'euro-value';
$handler->display->display_options['fields']['php_2']['use_php_setup'] = 0;
$handler->display->display_options['fields']['php_2']['php_output'] = '<?php 
echo number_format(rovas_meritscope_get_user_merit_euro_value($row->uid),0,\',\',\' \');
?>';
$handler->display->display_options['fields']['php_2']['use_php_click_sortable'] = '0';
$handler->display->display_options['fields']['php_2']['php_click_sortable'] = '';
/* Field: row style current user */
$handler->display->display_options['fields']['php']['id'] = 'php';
$handler->display->display_options['fields']['php']['table'] = 'views';
$handler->display->display_options['fields']['php']['field'] = 'php';
$handler->display->display_options['fields']['php']['ui_name'] = 'row style current user';
$handler->display->display_options['fields']['php']['label'] = '';
$handler->display->display_options['fields']['php']['exclude'] = TRUE;
$handler->display->display_options['fields']['php']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['php']['use_php_setup'] = 0;
$handler->display->display_options['fields']['php']['php_output'] = '<?php 
global $user;
if ($user->uid == $row->uid)
echo \'current-user\';
?>';
$handler->display->display_options['fields']['php']['use_php_click_sortable'] = '0';
$handler->display->display_options['fields']['php']['php_click_sortable'] = '';
/* Field: Global: PHP */
$handler->display->display_options['fields']['php_1']['id'] = 'php_1';
$handler->display->display_options['fields']['php_1']['table'] = 'views';
$handler->display->display_options['fields']['php_1']['field'] = 'php';
$handler->display->display_options['fields']['php_1']['label'] = 'Domain';
$handler->display->display_options['fields']['php_1']['use_php_setup'] = 0;
$handler->display->display_options['fields']['php_1']['php_output'] = '<?php
echo rovas_meritscope_display_user_expertise($row->uid,3);
?>';
$handler->display->display_options['fields']['php_1']['use_php_click_sortable'] = '0';
$handler->display->display_options['fields']['php_1']['php_click_sortable'] = '';
/* Field: Field: Country */
$handler->display->display_options['fields']['field_country']['id'] = 'field_country';
$handler->display->display_options['fields']['field_country']['table'] = 'field_data_field_country';
$handler->display->display_options['fields']['field_country']['field'] = 'field_country';
$handler->display->display_options['fields']['field_country']['alter']['alter_text'] = TRUE;
$handler->display->display_options['fields']['field_country']['alter']['text'] = '<span class="country">[field_country]</span>';
$handler->display->display_options['fields']['field_country']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_country']['type'] = 'country_icon';
$handler->display->display_options['fields']['field_country']['settings'] = array(
  'countryiconset' => 'icon_shiny',
  'property' => '',
);
/* Sort criterion: Userpoints total: Current total merits */
$handler->display->display_options['sorts']['points']['id'] = 'points';
$handler->display->display_options['sorts']['points']['table'] = 'userpoints_total';
$handler->display->display_options['sorts']['points']['field'] = 'points';
$handler->display->display_options['sorts']['points']['order'] = 'DESC';
/* Filter criterion: User: Active status */
$handler->display->display_options['filters']['status']['id'] = 'status';
$handler->display->display_options['filters']['status']['table'] = 'users';
$handler->display->display_options['filters']['status']['field'] = 'status';
$handler->display->display_options['filters']['status']['value'] = '1';
$handler->display->display_options['filters']['status']['group'] = 1;
$handler->display->display_options['filters']['status']['expose']['operator'] = FALSE;
/* Filter criterion: User: Name */
$handler->display->display_options['filters']['uid']['id'] = 'uid';
$handler->display->display_options['filters']['uid']['table'] = 'users';
$handler->display->display_options['filters']['uid']['field'] = 'uid';
$handler->display->display_options['filters']['uid']['operator'] = 'not in';
$handler->display->display_options['filters']['uid']['value'] = array(
  0 => '1',
);
$handler->display->display_options['filters']['uid']['group'] = 1;
/* Filter criterion: User: Roles */
$handler->display->display_options['filters']['rid']['id'] = 'rid';
$handler->display->display_options['filters']['rid']['table'] = 'users_roles';
$handler->display->display_options['filters']['rid']['field'] = 'rid';
$handler->display->display_options['filters']['rid']['operator'] = 'not';
$handler->display->display_options['filters']['rid']['value'] = array(
  3 => '3',
  8 => '8',
);
$handler->display->display_options['filters']['rid']['group'] = 1;
/* Filter criterion: Countries: Name - list */
$handler->display->display_options['filters']['name_list']['id'] = 'name_list';
$handler->display->display_options['filters']['name_list']['table'] = 'countries_country';
$handler->display->display_options['filters']['name_list']['field'] = 'name_list';
$handler->display->display_options['filters']['name_list']['relationship'] = 'field_country_iso2';
$handler->display->display_options['filters']['name_list']['group'] = 1;
$handler->display->display_options['filters']['name_list']['exposed'] = TRUE;
$handler->display->display_options['filters']['name_list']['expose']['operator_id'] = 'name_list_op';
$handler->display->display_options['filters']['name_list']['expose']['operator'] = 'name_list_op';
$handler->display->display_options['filters']['name_list']['expose']['identifier'] = 'name_list';
$handler->display->display_options['filters']['name_list']['expose']['remember_roles'] = array(
  2 => '2',
);
/* Filter criterion: Userpoints total: Current total merits */
$handler->display->display_options['filters']['points']['id'] = 'points';
$handler->display->display_options['filters']['points']['table'] = 'userpoints_total';
$handler->display->display_options['filters']['points']['field'] = 'points';
$handler->display->display_options['filters']['points']['operator'] = '>=';
$handler->display->display_options['filters']['points']['value']['value'] = '0';

/* Display: Page */
$handler = $view->new_display('page', 'Page', 'page');
$handler->display->display_options['path'] = 'merit_index';
$translatables['users_merit_rank'] = array(
  t('Master'),
  t('Merit Index'),
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
  t('Merit list intro'),
  t('<img class="meritlogo" src="/sites/default/files/pictures/meritlogo.svg"/>
<span>The <strong>economic value donated to society</strong> by individuals and the domain in which the value was created.</span>'),
  t('<span style="font-size: smaller; margin-bottom: 25px; display: block; margin-top: 4px;">*The estimated value is calculated from the average hourly wage in the particular domains in which the donated value was generated. If the average wage in a particular domain is unknown, the rate of 10€/hour is used.</span>'),
  t('country'),
  t('#'),
  t('Name'),
  t('[name]<br>
<span style="font-size: 11px; font-weight: 400;color: gray;">[created]</span>'),
  t('.'),
  t('Merits'),
  t('Value* (€)'),
  t('Domain'),
  t('Country'),
  t('<span class="country">[field_country]</span>'),
  t('Page'),
);

