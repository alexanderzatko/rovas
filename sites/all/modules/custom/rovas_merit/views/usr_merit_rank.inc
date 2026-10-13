<?php

/**
 * @file
 * Default view: usr_merit_rank
 */

$view = new view();
$view->name = 'usr_merit_rank';
$view->description = '';
$view->tag = 'default';
$view->base_table = 'userpoints_total';
$view->human_name = 'Usr Merit rank';
$view->core = 7;
$view->api_version = '3.0';
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

/* Display: Master */
$handler = $view->new_display('default', 'Master', 'default');
$handler->display->display_options['title'] = 'Merit score';
$handler->display->display_options['use_more_always'] = FALSE;
$handler->display->display_options['access']['type'] = 'none';
$handler->display->display_options['cache']['type'] = 'none';
$handler->display->display_options['query']['type'] = 'views_query';
$handler->display->display_options['exposed_form']['type'] = 'basic';
$handler->display->display_options['pager']['type'] = 'some';
$handler->display->display_options['pager']['options']['items_per_page'] = '5';
$handler->display->display_options['style_plugin'] = 'default';
$handler->display->display_options['row_plugin'] = 'fields';
/* Relationship: Userpoints total: User */
$handler->display->display_options['relationships']['uid']['id'] = 'uid';
$handler->display->display_options['relationships']['uid']['table'] = 'userpoints_total';
$handler->display->display_options['relationships']['uid']['field'] = 'uid';
/* Field: User: Name */
$handler->display->display_options['fields']['name']['id'] = 'name';
$handler->display->display_options['fields']['name']['table'] = 'users';
$handler->display->display_options['fields']['name']['field'] = 'name';
$handler->display->display_options['fields']['name']['relationship'] = 'uid';
$handler->display->display_options['fields']['name']['exclude'] = TRUE;
/* Field: User: Full name */
$handler->display->display_options['fields']['field_full_name']['id'] = 'field_full_name';
$handler->display->display_options['fields']['field_full_name']['table'] = 'field_data_field_full_name';
$handler->display->display_options['fields']['field_full_name']['field'] = 'field_full_name';
$handler->display->display_options['fields']['field_full_name']['relationship'] = 'uid';
$handler->display->display_options['fields']['field_full_name']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_full_name']['alter']['alter_text'] = TRUE;
$handler->display->display_options['fields']['field_full_name']['alter']['text'] = '[field_full_name] ([name])';
/* Field: Userpoints total: Current total merits */
$handler->display->display_options['fields']['points_1']['id'] = 'points_1';
$handler->display->display_options['fields']['points_1']['table'] = 'userpoints_total';
$handler->display->display_options['fields']['points_1']['field'] = 'points';
$handler->display->display_options['fields']['points_1']['label'] = '';
$handler->display->display_options['fields']['points_1']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['points_1']['suffix'] = ' ME';
/* Field: Userpoints total: Current total merits */
$handler->display->display_options['fields']['points']['id'] = 'points';
$handler->display->display_options['fields']['points']['table'] = 'userpoints_total';
$handler->display->display_options['fields']['points']['field'] = 'points';
$handler->display->display_options['fields']['points']['exclude'] = TRUE;
$handler->display->display_options['fields']['points']['separator'] = '';
/* Field: Global: PHP */
$handler->display->display_options['fields']['php']['id'] = 'php';
$handler->display->display_options['fields']['php']['table'] = 'views';
$handler->display->display_options['fields']['php']['field'] = 'php';
$handler->display->display_options['fields']['php']['label'] = 'Rank';
$handler->display->display_options['fields']['php']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['php']['use_php_setup'] = 0;
$handler->display->display_options['fields']['php']['php_output'] = '<?php
if($static == \'\') {
$result = db_query("SELECT max( points ) AS maximum FROM {userpoints_total}");
$resarray = $result->fetchAll();
$static = $resarray[0]->maximum;
}
$static;
echo \'<div style="position:relative;">\';
echo \'<div style="width: 100%; height 15px;background-color:#F0C584;position:absolute;">&nbsp;</div>\';
echo \'<div style="width: \' . 100 * $row->points / $static . \'%; height 15px;background-color:#D89C3F;position:absolute;top:0px;">&nbsp;</div>\';
//echo \'<div style="position:absolute;top:0px;left:5px;">\' . round(100 * $row->points / $static) . \'</div>\';
echo \'</div>\';
?>';
$handler->display->display_options['fields']['php']['use_php_click_sortable'] = '0';
$handler->display->display_options['fields']['php']['php_click_sortable'] = '';
/* Sort criterion: Userpoints total: Current total merits */
$handler->display->display_options['sorts']['points']['id'] = 'points';
$handler->display->display_options['sorts']['points']['table'] = 'userpoints_total';
$handler->display->display_options['sorts']['points']['field'] = 'points';
$handler->display->display_options['sorts']['points']['order'] = 'DESC';
/* Contextual filter: User: Uid */
$handler->display->display_options['arguments']['uid']['id'] = 'uid';
$handler->display->display_options['arguments']['uid']['table'] = 'users';
$handler->display->display_options['arguments']['uid']['field'] = 'uid';
$handler->display->display_options['arguments']['uid']['relationship'] = 'uid';
$handler->display->display_options['arguments']['uid']['default_action'] = 'default';
$handler->display->display_options['arguments']['uid']['default_argument_type'] = 'current_user';
$handler->display->display_options['arguments']['uid']['summary']['number_of_records'] = '0';
$handler->display->display_options['arguments']['uid']['summary']['format'] = 'default_summary';
$handler->display->display_options['arguments']['uid']['summary_options']['items_per_page'] = '25';

/* Display: Block */
$handler = $view->new_display('block', 'Block', 'block');
$translatables['usr_merit_rank'] = array(
  t('Master'),
  t('Merit score'),
  t('more'),
  t('Apply'),
  t('Reset'),
  t('Sort by'),
  t('Asc'),
  t('Desc'),
  t('Users'),
  t('Name'),
  t('Full name'),
  t('[field_full_name] ([name])'),
  t('.'),
  t(','),
  t(' ME'),
  t('Current total merits'),
  t('Rank'),
  t('All'),
  t('Block'),
);

