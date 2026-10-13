<?php

/**
 * @file
 * Default view: projects
 */

$view = new view();
$view->name = 'projects';
$view->description = '';
$view->tag = 'default';
$view->base_table = 'node';
$view->human_name = 'Projects';
$view->core = 7;
$view->api_version = '3.0';
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

/* Display: Master */
$handler = $view->new_display('default', 'Master', 'default');
$handler->display->display_options['title'] = 'Public Projects';
$handler->display->display_options['use_ajax'] = TRUE;
$handler->display->display_options['use_more_always'] = FALSE;
$handler->display->display_options['access']['type'] = 'perm';
$handler->display->display_options['cache']['type'] = 'none';
$handler->display->display_options['query']['type'] = 'views_query';
$handler->display->display_options['exposed_form']['type'] = 'basic';
$handler->display->display_options['pager']['type'] = 'mini';
$handler->display->display_options['pager']['options']['items_per_page'] = '5';
$handler->display->display_options['pager']['options']['offset'] = '0';
$handler->display->display_options['pager']['options']['id'] = '0';
$handler->display->display_options['pager']['options']['tags']['previous'] = '‹';
$handler->display->display_options['pager']['options']['tags']['next'] = '›';
$handler->display->display_options['style_plugin'] = 'table';
$handler->display->display_options['style_options']['columns'] = array(
  'nid' => 'nid',
  'field_project_id' => 'field_project_id',
  'name' => 'name',
  'php' => 'php',
  'title' => 'title',
  'field_full_name' => 'field_full_name',
  'field_system_project' => 'field_system_project',
  'field_merit_reward' => 'field_merit_reward',
  'field_merge_daily_reports' => 'field_merge_daily_reports',
);
$handler->display->display_options['style_options']['class'] = '';
$handler->display->display_options['style_options']['default'] = '-1';
$handler->display->display_options['style_options']['info'] = array(
  'nid' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_project_id' => array(
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
  'php' => array(
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
  'field_full_name' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_system_project' => array(
    'sortable' => 0,
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
  'field_merge_daily_reports' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
);
/* Footer: Global: Text area */
$handler->display->display_options['footer']['area']['id'] = 'area';
$handler->display->display_options['footer']['area']['table'] = 'views';
$handler->display->display_options['footer']['area']['field'] = 'area';
$handler->display->display_options['footer']['area']['empty'] = TRUE;
$handler->display->display_options['footer']['area']['content'] = '<?php
global $user;
$obj = entity_metadata_wrapper(\'user\', $user);
$vrfststus = $obj->field_verified->value();
if ($vrfststus == 1) {
	print \'[<a class="btn btn-default btn-xs" style="color: white; background-color: green;" role="button" href="/node/add/project/">\' . t(\'Add Project\') . \'</a><br/><br/>\';
}
?>';
$handler->display->display_options['footer']['area']['format'] = 'php_code';
/* No results behavior: Global: Text area */
$handler->display->display_options['empty']['area']['id'] = 'area';
$handler->display->display_options['empty']['area']['table'] = 'views';
$handler->display->display_options['empty']['area']['field'] = 'area';
$handler->display->display_options['empty']['area']['label'] = 'projects view message';
$handler->display->display_options['empty']['area']['empty'] = TRUE;
$handler->display->display_options['empty']['area']['content'] = '<?php 
print t(\'You have created no projects yet.\');
?>';
$handler->display->display_options['empty']['area']['format'] = 'php_code';
/* Relationship: Entity Reference: Referencing entity */
$handler->display->display_options['relationships']['reverse_field_project_node']['id'] = 'reverse_field_project_node';
$handler->display->display_options['relationships']['reverse_field_project_node']['table'] = 'node';
$handler->display->display_options['relationships']['reverse_field_project_node']['field'] = 'reverse_field_project_node';
/* Field: Content: Nid */
$handler->display->display_options['fields']['nid']['id'] = 'nid';
$handler->display->display_options['fields']['nid']['table'] = 'node';
$handler->display->display_options['fields']['nid']['field'] = 'nid';
$handler->display->display_options['fields']['nid']['label'] = 'Number';
$handler->display->display_options['fields']['nid']['exclude'] = TRUE;
$handler->display->display_options['fields']['nid']['element_label_colon'] = FALSE;
/* Field: Content: Updated date */
$handler->display->display_options['fields']['changed']['id'] = 'changed';
$handler->display->display_options['fields']['changed']['table'] = 'node';
$handler->display->display_options['fields']['changed']['field'] = 'changed';
$handler->display->display_options['fields']['changed']['label'] = 'Date';
$handler->display->display_options['fields']['changed']['date_format'] = 'custom';
$handler->display->display_options['fields']['changed']['custom_date_format'] = 'd M Y';
$handler->display->display_options['fields']['changed']['second_date_format'] = 'privatemsg_current_day';
/* Field: Content: Title */
$handler->display->display_options['fields']['title']['id'] = 'title';
$handler->display->display_options['fields']['title']['table'] = 'node';
$handler->display->display_options['fields']['title']['field'] = 'title';
$handler->display->display_options['fields']['title']['label'] = 'Name';
$handler->display->display_options['fields']['title']['alter']['word_boundary'] = FALSE;
$handler->display->display_options['fields']['title']['alter']['ellipsis'] = FALSE;
/* Field: Global: PHP */
$handler->display->display_options['fields']['php']['id'] = 'php';
$handler->display->display_options['fields']['php']['table'] = 'views';
$handler->display->display_options['fields']['php']['field'] = 'php';
$handler->display->display_options['fields']['php']['label'] = 'Chrons';
$handler->display->display_options['fields']['php']['use_php_setup'] = 0;
$handler->display->display_options['fields']['php']['php_value'] = '//get Chron amount from work reports
$result = db_query("SELECT sum( ch.field_chrons_to_grant_value ) AS sum
FROM {field_data_field_chrons_to_grant} ch, {field_data_field_project} fp
WHERE ch.entity_id = fp.entity_id
AND fp.field_project_target_id =:node
AND fp.bundle = \'work_report\'", array(\':node\' => $row->nid));

foreach ($result as $row) {
if (is_numeric($row->sum))
return $row->sum;
else
return 0;
}';
$handler->display->display_options['fields']['php']['use_php_click_sortable'] = '0';
$handler->display->display_options['fields']['php']['php_click_sortable'] = '';
/* Field: Global: Custom text */
$handler->display->display_options['fields']['nothing']['id'] = 'nothing';
$handler->display->display_options['fields']['nothing']['table'] = 'views';
$handler->display->display_options['fields']['nothing']['field'] = 'nothing';
$handler->display->display_options['fields']['nothing']['label'] = 'Action';
$handler->display->display_options['fields']['nothing']['alter']['text'] = '<a href="/node/[nid]/edit"><img src="/sites/default/files/pictures/edit.svg" width="16" /></a>&nbsp;&nbsp;<a href="/node/[nid]/delete"><img src="/sites/default/files/pictures/delete.svg" width="16" /></a>';
/* Field: Content: Project owner */
$handler->display->display_options['fields']['field_project_owner']['id'] = 'field_project_owner';
$handler->display->display_options['fields']['field_project_owner']['table'] = 'field_data_field_project_owner';
$handler->display->display_options['fields']['field_project_owner']['field'] = 'field_project_owner';
/* Sort criterion: Content: Post date */
$handler->display->display_options['sorts']['created']['id'] = 'created';
$handler->display->display_options['sorts']['created']['table'] = 'node';
$handler->display->display_options['sorts']['created']['field'] = 'created';
$handler->display->display_options['sorts']['created']['order'] = 'DESC';
/* Contextual filter: Content: Worker Name (field_coworker) */
$handler->display->display_options['arguments']['field_coworker_target_id']['id'] = 'field_coworker_target_id';
$handler->display->display_options['arguments']['field_coworker_target_id']['table'] = 'field_data_field_coworker';
$handler->display->display_options['arguments']['field_coworker_target_id']['field'] = 'field_coworker_target_id';
$handler->display->display_options['arguments']['field_coworker_target_id']['default_argument_type'] = 'fixed';
$handler->display->display_options['arguments']['field_coworker_target_id']['summary']['format'] = 'default_summary';
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
/* Filter criterion: Content: Project Participation (field_participation) */
$handler->display->display_options['filters']['field_participation_value']['id'] = 'field_participation_value';
$handler->display->display_options['filters']['field_participation_value']['table'] = 'field_data_field_participation';
$handler->display->display_options['filters']['field_participation_value']['field'] = 'field_participation_value';
$handler->display->display_options['filters']['field_participation_value']['value'] = array(
  0 => '0',
  1 => '1',
);
$handler->display->display_options['filters']['field_participation_value']['exposed'] = TRUE;
$handler->display->display_options['filters']['field_participation_value']['expose']['operator_id'] = 'field_participation_value_op';
$handler->display->display_options['filters']['field_participation_value']['expose']['label'] = 'Project Participation (field_participation)';
$handler->display->display_options['filters']['field_participation_value']['expose']['operator'] = 'field_participation_value_op';
$handler->display->display_options['filters']['field_participation_value']['expose']['identifier'] = 'field_participation_value';

/* Display: Block */
$handler = $view->new_display('block', 'Block', 'block');
$handler->display->display_options['defaults']['title'] = FALSE;
$handler->display->display_options['title'] = 'Projects I work on';
$handler->display->display_options['defaults']['use_ajax'] = FALSE;
$handler->display->display_options['use_ajax'] = TRUE;
$handler->display->display_options['defaults']['access'] = FALSE;
$handler->display->display_options['access']['type'] = 'role';
$handler->display->display_options['access']['role'] = array(
  6 => '6',
  5 => '5',
  4 => '4',
);
$handler->display->display_options['defaults']['query'] = FALSE;
$handler->display->display_options['query']['type'] = 'views_query';
$handler->display->display_options['query']['options']['distinct'] = TRUE;
$handler->display->display_options['defaults']['pager'] = FALSE;
$handler->display->display_options['pager']['type'] = 'full';
$handler->display->display_options['pager']['options']['items_per_page'] = '5';
$handler->display->display_options['pager']['options']['offset'] = '0';
$handler->display->display_options['pager']['options']['id'] = '1';
$handler->display->display_options['pager']['options']['quantity'] = '3';
$handler->display->display_options['pager']['options']['tags']['first'] = '«';
$handler->display->display_options['pager']['options']['tags']['previous'] = '‹';
$handler->display->display_options['pager']['options']['tags']['next'] = '›';
$handler->display->display_options['pager']['options']['tags']['last'] = '»';
$handler->display->display_options['defaults']['style_plugin'] = FALSE;
$handler->display->display_options['style_plugin'] = 'table';
$handler->display->display_options['style_options']['columns'] = array(
  'nid' => 'nid',
  'changed' => 'changed',
  'title' => 'title',
  'name' => 'name',
  'field_full_name' => 'field_full_name',
  'field_participation' => 'field_participation',
  'field_status' => 'field_status',
  'uid' => 'uid',
  'field_project_total_hours' => 'field_project_total_hours',
  'php' => 'php',
  'field_merit_reward' => 'field_merit_reward',
  'php_1' => 'php_1',
);
$handler->display->display_options['style_options']['class'] = '';
$handler->display->display_options['style_options']['default'] = '-1';
$handler->display->display_options['style_options']['info'] = array(
  'nid' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'changed' => array(
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
  'name' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_full_name' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_participation' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_status' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'uid' => array(
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_project_total_hours' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'php' => array(
    'sortable' => 0,
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
  'php_1' => array(
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
);
$handler->display->display_options['defaults']['style_options'] = FALSE;
$handler->display->display_options['defaults']['row_plugin'] = FALSE;
$handler->display->display_options['defaults']['row_options'] = FALSE;
$handler->display->display_options['defaults']['footer'] = FALSE;
/* Footer: Global: Text area */
$handler->display->display_options['footer']['area']['id'] = 'area';
$handler->display->display_options['footer']['area']['table'] = 'views';
$handler->display->display_options['footer']['area']['field'] = 'area';
$handler->display->display_options['footer']['area']['empty'] = TRUE;
$handler->display->display_options['footer']['area']['content'] = '<?php
global $user;
$obj = entity_metadata_wrapper(\'user\', $user);
$vrfststus = $obj->field_verified->value();
if ($vrfststus == 1) {
	print \'<br/><a class="btn btn-default" style="color: white; background-color: green;" role="button" href="/node/add/project/">\' . t(\'Add Project\') . \'</a><br/><br/>\';
}
?>';
$handler->display->display_options['footer']['area']['format'] = 'php_code';
$handler->display->display_options['defaults']['relationships'] = FALSE;
/* Relationship: Entity Reference: Referencing entity */
$handler->display->display_options['relationships']['reverse_field_project_node']['id'] = 'reverse_field_project_node';
$handler->display->display_options['relationships']['reverse_field_project_node']['table'] = 'node';
$handler->display->display_options['relationships']['reverse_field_project_node']['field'] = 'reverse_field_project_node';
/* Relationship: Content: Content author */
$handler->display->display_options['relationships']['uid']['id'] = 'uid';
$handler->display->display_options['relationships']['uid']['table'] = 'node';
$handler->display->display_options['relationships']['uid']['field'] = 'uid';
$handler->display->display_options['defaults']['fields'] = FALSE;
/* Field: Content: Nid */
$handler->display->display_options['fields']['nid']['id'] = 'nid';
$handler->display->display_options['fields']['nid']['table'] = 'node';
$handler->display->display_options['fields']['nid']['field'] = 'nid';
$handler->display->display_options['fields']['nid']['label'] = 'Number';
$handler->display->display_options['fields']['nid']['exclude'] = TRUE;
$handler->display->display_options['fields']['nid']['element_label_colon'] = FALSE;
/* Field: Content: Updated date */
$handler->display->display_options['fields']['changed']['id'] = 'changed';
$handler->display->display_options['fields']['changed']['table'] = 'node';
$handler->display->display_options['fields']['changed']['field'] = 'changed';
$handler->display->display_options['fields']['changed']['label'] = 'Date';
$handler->display->display_options['fields']['changed']['exclude'] = TRUE;
$handler->display->display_options['fields']['changed']['date_format'] = 'custom';
$handler->display->display_options['fields']['changed']['custom_date_format'] = 'd M Y';
$handler->display->display_options['fields']['changed']['second_date_format'] = 'privatemsg_current_day';
/* Field: Content: Title */
$handler->display->display_options['fields']['title']['id'] = 'title';
$handler->display->display_options['fields']['title']['table'] = 'node';
$handler->display->display_options['fields']['title']['field'] = 'title';
$handler->display->display_options['fields']['title']['label'] = 'Name';
$handler->display->display_options['fields']['title']['alter']['word_boundary'] = FALSE;
$handler->display->display_options['fields']['title']['alter']['ellipsis'] = FALSE;
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
$handler->display->display_options['fields']['field_full_name']['label'] = 'Project owner';
$handler->display->display_options['fields']['field_full_name']['alter']['alter_text'] = TRUE;
$handler->display->display_options['fields']['field_full_name']['alter']['text'] = '[name]';
/* Field: Content: Project Participation */
$handler->display->display_options['fields']['field_participation']['id'] = 'field_participation';
$handler->display->display_options['fields']['field_participation']['table'] = 'field_data_field_participation';
$handler->display->display_options['fields']['field_participation']['field'] = 'field_participation';
$handler->display->display_options['fields']['field_participation']['label'] = 'Participation';
$handler->display->display_options['fields']['field_participation']['type'] = 'i18n_list_default';
/* Field: Content: Status */
$handler->display->display_options['fields']['field_status']['id'] = 'field_status';
$handler->display->display_options['fields']['field_status']['table'] = 'field_data_field_status';
$handler->display->display_options['fields']['field_status']['field'] = 'field_status';
$handler->display->display_options['fields']['field_status']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_status']['type'] = 'i18n_list_default';
/* Field: Content: Author uid */
$handler->display->display_options['fields']['uid']['id'] = 'uid';
$handler->display->display_options['fields']['uid']['table'] = 'node';
$handler->display->display_options['fields']['uid']['field'] = 'uid';
$handler->display->display_options['fields']['uid']['label'] = '';
$handler->display->display_options['fields']['uid']['exclude'] = TRUE;
$handler->display->display_options['fields']['uid']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['uid']['link_to_user'] = FALSE;
/* Field: Content: Time worked */
$handler->display->display_options['fields']['field_project_total_hours']['id'] = 'field_project_total_hours';
$handler->display->display_options['fields']['field_project_total_hours']['table'] = 'field_data_field_project_total_hours';
$handler->display->display_options['fields']['field_project_total_hours']['field'] = 'field_project_total_hours';
$handler->display->display_options['fields']['field_project_total_hours']['label'] = '';
$handler->display->display_options['fields']['field_project_total_hours']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_project_total_hours']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_project_total_hours']['type'] = 'computed_field_computed_value';
/* Field: Global: PHP */
$handler->display->display_options['fields']['php']['id'] = 'php';
$handler->display->display_options['fields']['php']['table'] = 'views';
$handler->display->display_options['fields']['php']['field'] = 'php';
$handler->display->display_options['fields']['php']['label'] = 'Chrons';
$handler->display->display_options['fields']['php']['use_php_setup'] = 0;
$handler->display->display_options['fields']['php']['php_output'] = '<?php 
$outpt = isset($data->field_field_project_total_hours[0][\'raw\'][\'value\']) ? round($data->field_field_project_total_hours[0][\'raw\'][\'value\'] * 10,2) : 0;
echo $outpt;
?>';
$handler->display->display_options['fields']['php']['use_php_click_sortable'] = '1';
$handler->display->display_options['fields']['php']['php_click_sortable'] = '';
/* Field: Content: Merit reward */
$handler->display->display_options['fields']['field_merit_reward']['id'] = 'field_merit_reward';
$handler->display->display_options['fields']['field_merit_reward']['table'] = 'field_data_field_merit_reward';
$handler->display->display_options['fields']['field_merit_reward']['field'] = 'field_merit_reward';
$handler->display->display_options['fields']['field_merit_reward']['label'] = 'Merits';
$handler->display->display_options['fields']['field_merit_reward']['settings'] = array(
  'thousand_separator' => ' ',
  'decimal_separator' => '.',
  'scale' => '2',
  'prefix_suffix' => 1,
);
/* Field: Global: PHP */
$handler->display->display_options['fields']['php_1']['id'] = 'php_1';
$handler->display->display_options['fields']['php_1']['table'] = 'views';
$handler->display->display_options['fields']['php_1']['field'] = 'php';
$handler->display->display_options['fields']['php_1']['label'] = 'Action';
$handler->display->display_options['fields']['php_1']['use_php_setup'] = 0;
$handler->display->display_options['fields']['php_1']['php_output'] = '<?php 
global $user;
$obj = entity_metadata_wrapper(\'user\', $user);
$vrfststus = $obj->field_verified->value();

//only project a verified owner can edit
if ($row->uid == $user->uid && $vrfststus == 1) {
	print \'<a href="/node/\'.$row->nid.\'/edit"><img src="/sites/default/files/pictures/edit.svg" width="16" /></a>&nbsp;&nbsp;\';

	if (rules_invoke_component(\'rules_delete_project_permissive\', $user, node_load($row->nid))) {
		print \'<a href="/node/\'.$row->nid.\'/delete"><img src="/sites/default/files/pictures/delete.svg" width="16" /></a>\';
		}
	else {
		print \'<img src="/sites/default/files/pictures/delete-off.svg" width="16" />\';
	}

}
else {
	print \'<img src="/sites/default/files/pictures/edit-off.svg" width="16" />&nbsp;&nbsp;\';
	print \'<img src="/sites/default/files/pictures/delete-off.svg" width="16" />\';
}
?>';
$handler->display->display_options['fields']['php_1']['use_php_click_sortable'] = '0';
$handler->display->display_options['fields']['php_1']['php_click_sortable'] = '';
$handler->display->display_options['defaults']['arguments'] = FALSE;
/* Contextual filter: Content: Worker Name (field_coworker) */
$handler->display->display_options['arguments']['field_coworker_target_id']['id'] = 'field_coworker_target_id';
$handler->display->display_options['arguments']['field_coworker_target_id']['table'] = 'field_data_field_coworker';
$handler->display->display_options['arguments']['field_coworker_target_id']['field'] = 'field_coworker_target_id';
$handler->display->display_options['arguments']['field_coworker_target_id']['relationship'] = 'reverse_field_project_node';
$handler->display->display_options['arguments']['field_coworker_target_id']['default_action'] = 'default';
$handler->display->display_options['arguments']['field_coworker_target_id']['default_argument_type'] = 'current_user';
$handler->display->display_options['arguments']['field_coworker_target_id']['summary']['number_of_records'] = '0';
$handler->display->display_options['arguments']['field_coworker_target_id']['summary']['format'] = 'default_summary';
$handler->display->display_options['arguments']['field_coworker_target_id']['summary_options']['items_per_page'] = '25';
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
  'project' => 'project',
);
$handler->display->display_options['filters']['type']['group'] = 1;
/* Filter criterion: Content: Status (field_status) */
$handler->display->display_options['filters']['field_status_value']['id'] = 'field_status_value';
$handler->display->display_options['filters']['field_status_value']['table'] = 'field_data_field_status';
$handler->display->display_options['filters']['field_status_value']['field'] = 'field_status_value';
$handler->display->display_options['filters']['field_status_value']['value'] = array(
  0 => '0',
  1 => '1',
);
$handler->display->display_options['filters']['field_status_value']['group'] = 1;

/* Display: Projects ER */
$handler = $view->new_display('entityreference', 'Projects ER', 'entityreference_1');
$handler->display->display_options['defaults']['title'] = FALSE;
$handler->display->display_options['display_description'] = 'Entity relation view to show candidate projects for various fields';
$handler->display->display_options['pager']['type'] = 'some';
$handler->display->display_options['defaults']['style_plugin'] = FALSE;
$handler->display->display_options['style_plugin'] = 'entityreference_style';
$handler->display->display_options['style_options']['search_fields'] = array(
  'nid' => 'nid',
  'title' => 'title',
);
$handler->display->display_options['defaults']['style_options'] = FALSE;
$handler->display->display_options['defaults']['row_plugin'] = FALSE;
$handler->display->display_options['row_plugin'] = 'entityreference_fields';
$handler->display->display_options['defaults']['row_options'] = FALSE;
$handler->display->display_options['defaults']['footer'] = FALSE;
$handler->display->display_options['defaults']['empty'] = FALSE;
$handler->display->display_options['defaults']['relationships'] = FALSE;
$handler->display->display_options['defaults']['fields'] = FALSE;
/* Field: Content: Nid */
$handler->display->display_options['fields']['nid']['id'] = 'nid';
$handler->display->display_options['fields']['nid']['table'] = 'node';
$handler->display->display_options['fields']['nid']['field'] = 'nid';
$handler->display->display_options['fields']['nid']['label'] = '';
$handler->display->display_options['fields']['nid']['element_label_colon'] = FALSE;
/* Field: Content: Title */
$handler->display->display_options['fields']['title']['id'] = 'title';
$handler->display->display_options['fields']['title']['table'] = 'node';
$handler->display->display_options['fields']['title']['field'] = 'title';
$handler->display->display_options['fields']['title']['label'] = '';
$handler->display->display_options['fields']['title']['alter']['word_boundary'] = FALSE;
$handler->display->display_options['fields']['title']['alter']['ellipsis'] = FALSE;
$handler->display->display_options['fields']['title']['element_label_colon'] = FALSE;
$handler->display->display_options['defaults']['arguments'] = FALSE;
/* Contextual filter: Content: Nid */
$handler->display->display_options['arguments']['nid']['id'] = 'nid';
$handler->display->display_options['arguments']['nid']['table'] = 'node';
$handler->display->display_options['arguments']['nid']['field'] = 'nid';
$handler->display->display_options['arguments']['nid']['default_argument_type'] = 'variable';
$handler->display->display_options['arguments']['nid']['default_argument_options']['variable'] = 'variable_realm_list_global';
$handler->display->display_options['arguments']['nid']['summary']['number_of_records'] = '0';
$handler->display->display_options['arguments']['nid']['summary']['format'] = 'default_summary';
$handler->display->display_options['arguments']['nid']['summary_options']['items_per_page'] = '25';
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
  'project' => 'project',
);
/* Filter criterion: Content: Author uid */
$handler->display->display_options['filters']['uid']['id'] = 'uid';
$handler->display->display_options['filters']['uid']['table'] = 'node';
$handler->display->display_options['filters']['uid']['field'] = 'uid';
$handler->display->display_options['filters']['uid']['operator'] = 'not in';
$handler->display->display_options['filters']['uid']['value'] = array(
  0 => '1',
);

/* Display: Merit allocation Shareholder */
$handler = $view->new_display('entityreference', 'Merit allocation Shareholder', 'entityreference_3');
$handler->display->display_options['defaults']['title'] = FALSE;
$handler->display->display_options['display_description'] = 'Entity relation view to show candidate projects for the Shareholder field';
$handler->display->display_options['pager']['type'] = 'none';
$handler->display->display_options['pager']['options']['offset'] = '0';
$handler->display->display_options['defaults']['style_plugin'] = FALSE;
$handler->display->display_options['style_plugin'] = 'entityreference_style';
$handler->display->display_options['style_options']['search_fields'] = array(
  'nid' => 'nid',
  'title' => 'title',
);
$handler->display->display_options['defaults']['style_options'] = FALSE;
$handler->display->display_options['defaults']['row_plugin'] = FALSE;
$handler->display->display_options['row_plugin'] = 'entityreference_fields';
$handler->display->display_options['defaults']['row_options'] = FALSE;
$handler->display->display_options['defaults']['footer'] = FALSE;
$handler->display->display_options['defaults']['empty'] = FALSE;
$handler->display->display_options['defaults']['relationships'] = FALSE;
$handler->display->display_options['defaults']['fields'] = FALSE;
/* Field: Content: Nid */
$handler->display->display_options['fields']['nid']['id'] = 'nid';
$handler->display->display_options['fields']['nid']['table'] = 'node';
$handler->display->display_options['fields']['nid']['field'] = 'nid';
$handler->display->display_options['fields']['nid']['label'] = '';
$handler->display->display_options['fields']['nid']['element_label_colon'] = FALSE;
/* Field: Content: Title */
$handler->display->display_options['fields']['title']['id'] = 'title';
$handler->display->display_options['fields']['title']['table'] = 'node';
$handler->display->display_options['fields']['title']['field'] = 'title';
$handler->display->display_options['fields']['title']['label'] = '';
$handler->display->display_options['fields']['title']['alter']['word_boundary'] = FALSE;
$handler->display->display_options['fields']['title']['alter']['ellipsis'] = FALSE;
$handler->display->display_options['fields']['title']['element_label_colon'] = FALSE;
$handler->display->display_options['defaults']['arguments'] = FALSE;
/* Contextual filter: Content: Nid */
$handler->display->display_options['arguments']['nid']['id'] = 'nid';
$handler->display->display_options['arguments']['nid']['table'] = 'node';
$handler->display->display_options['arguments']['nid']['field'] = 'nid';
$handler->display->display_options['arguments']['nid']['default_action'] = 'default';
$handler->display->display_options['arguments']['nid']['default_argument_type'] = 'php';
$handler->display->display_options['arguments']['nid']['default_argument_options']['code'] = '$ary = explode(\'/\',$_SERVER[\'HTTP_REFERER\']);
$ppid = end($ary);
$buid = $ppid;

$result = db_query("SELECT shproj.field_shareholder_project_target_id AS nid
FROM {node} node
LEFT JOIN {field_data_field_project} proj ON node.nid = proj.entity_id AND (proj.entity_type = \'node\' AND proj.deleted = \'0\')
LEFT JOIN {field_data_field_shareholder_project} shproj ON node.nid = shproj.entity_id AND (shproj.entity_type = \'node\' AND shproj.deleted = \'0\')
WHERE node.status = \'1\'
AND node.type = \'project_merit_allocation\'
AND proj.field_project_target_id =:node", array(\':node\' => $ppid));


foreach ($result as $row) {
	$buid = $buid . \',\' . $row->nid;
}
//watchdog(\'view\',\'found: \' . $buid);
return $buid;';
$handler->display->display_options['arguments']['nid']['summary']['number_of_records'] = '0';
$handler->display->display_options['arguments']['nid']['summary']['format'] = 'default_summary';
$handler->display->display_options['arguments']['nid']['summary_options']['items_per_page'] = '25';
$handler->display->display_options['arguments']['nid']['break_phrase'] = TRUE;
$handler->display->display_options['arguments']['nid']['not'] = TRUE;
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
  'project' => 'project',
);
$handler->display->display_options['filters']['type']['group'] = 1;
/* Filter criterion: Content: Status (field_status) */
$handler->display->display_options['filters']['field_status_value']['id'] = 'field_status_value';
$handler->display->display_options['filters']['field_status_value']['table'] = 'field_data_field_status';
$handler->display->display_options['filters']['field_status_value']['field'] = 'field_status_value';
$handler->display->display_options['filters']['field_status_value']['value'] = array(
  1 => '1',
);
$handler->display->display_options['filters']['field_status_value']['group'] = 1;
/* Filter criterion: Content: Author uid */
$handler->display->display_options['filters']['uid']['id'] = 'uid';
$handler->display->display_options['filters']['uid']['table'] = 'node';
$handler->display->display_options['filters']['uid']['field'] = 'uid';
$handler->display->display_options['filters']['uid']['operator'] = 'not in';
$handler->display->display_options['filters']['uid']['value'] = array(
  0 => '1',
);
$handler->display->display_options['filters']['uid']['group'] = 1;

/* Display: Projects page */
$handler = $view->new_display('page', 'Projects page', 'projectsxf');
$handler->display->display_options['defaults']['title'] = FALSE;
$handler->display->display_options['title'] = 'Find Project';
$handler->display->display_options['defaults']['use_ajax'] = FALSE;
$handler->display->display_options['defaults']['exposed_form'] = FALSE;
$handler->display->display_options['exposed_form']['type'] = 'better_exposed_filters';
$handler->display->display_options['exposed_form']['options']['reset_button'] = TRUE;
$handler->display->display_options['exposed_form']['options']['expose_sort_order'] = FALSE;
$handler->display->display_options['exposed_form']['options']['bef'] = array(
  'general' => array(
    'input_required' => 0,
    'text_input_required' => array(
      'text_input_required' => array(
        'value' => 'Select any filter and click on Apply to see results',
        'format' => 'filtered_html',
      ),
    ),
    'allow_secondary' => 0,
    'secondary_label' => 'Advanced options',
    'secondary_collapse_override' => '0',
  ),
  'field_participation_value_i18n' => array(
    'bef_format' => 'default',
    'more_options' => array(
      'bef_select_all_none' => 0,
      'bef_collapsible' => 0,
      'autosubmit' => 0,
      'is_secondary' => 0,
      'any_label' => '',
      'bef_filter_description' => '',
      'tokens' => array(
        'available' => array(
          0 => 'global_types',
        ),
      ),
      'rewrite' => array(
        'filter_rewrite_values' => '',
      ),
    ),
  ),
  'field_status_value_i18n' => array(
    'bef_format' => 'default',
    'more_options' => array(
      'bef_select_all_none' => 0,
      'bef_collapsible' => 0,
      'autosubmit' => 0,
      'is_secondary' => 0,
      'any_label' => '',
      'bef_filter_description' => '',
      'tokens' => array(
        'available' => array(
          0 => 'global_types',
        ),
      ),
      'rewrite' => array(
        'filter_rewrite_values' => '',
      ),
    ),
  ),
  'field_project_id_value' => array(
    'bef_format' => 'default',
    'slider_options' => array(
      'bef_slider_min' => '0',
      'bef_slider_max' => '99999',
      'bef_slider_step' => '1',
      'bef_slider_animate' => '',
      'bef_slider_orientation' => 'horizontal',
    ),
    'more_options' => array(
      'autosubmit' => 0,
      'is_secondary' => 0,
      'any_label' => '',
      'bef_filter_description' => '',
      'tokens' => array(
        'available' => array(
          0 => 'global_types',
        ),
      ),
      'rewrite' => array(
        'filter_rewrite_values' => '',
      ),
    ),
  ),
  'uid' => array(
    'bef_format' => 'default',
    'more_options' => array(
      'bef_select_all_none' => FALSE,
      'bef_collapsible' => 0,
      'autosubmit' => 0,
      'is_secondary' => 0,
      'any_label' => '',
      'bef_filter_description' => '',
      'tokens' => array(
        'available' => array(
          0 => 'global_types',
        ),
      ),
      'rewrite' => array(
        'filter_rewrite_values' => '',
      ),
    ),
  ),
  'field_system_project_value_i18n' => array(
    'bef_format' => 'bef',
    'more_options' => array(
      'bef_select_all_none' => 0,
      'bef_collapsible' => 0,
      'autosubmit' => 0,
      'is_secondary' => 0,
      'any_label' => '',
      'bef_filter_description' => '',
      'tokens' => array(
        'available' => array(
          0 => 'global_types',
        ),
      ),
      'rewrite' => array(
        'filter_rewrite_values' => 'no|hide',
      ),
    ),
  ),
);
$handler->display->display_options['defaults']['pager'] = FALSE;
$handler->display->display_options['pager']['type'] = 'full';
$handler->display->display_options['pager']['options']['items_per_page'] = '10';
$handler->display->display_options['pager']['options']['offset'] = '0';
$handler->display->display_options['pager']['options']['id'] = '0';
$handler->display->display_options['pager']['options']['quantity'] = '3';
$handler->display->display_options['pager']['options']['tags']['first'] = '«';
$handler->display->display_options['pager']['options']['tags']['previous'] = '‹';
$handler->display->display_options['pager']['options']['tags']['next'] = '›';
$handler->display->display_options['pager']['options']['tags']['last'] = '»';
$handler->display->display_options['defaults']['style_plugin'] = FALSE;
$handler->display->display_options['style_plugin'] = 'table';
$handler->display->display_options['style_options']['row_class'] = 'table-projects';
$handler->display->display_options['style_options']['columns'] = array(
  'nid' => 'nid',
  'field_project_id' => 'field_project_id',
  'name' => 'name',
  'php' => 'php',
  'title' => 'title',
  'title_field_et' => 'title_field_et',
  'field_full_name' => 'field_full_name',
  'field_system_project' => 'field_system_project',
  'field_merit_reward' => 'field_merit_reward',
  'field_merge_daily_reports' => 'field_merge_daily_reports',
);
$handler->display->display_options['style_options']['class'] = '';
$handler->display->display_options['style_options']['default'] = 'field_project_id';
$handler->display->display_options['style_options']['info'] = array(
  'nid' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_project_id' => array(
    'sortable' => 1,
    'default_sort_order' => 'desc',
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
  'php' => array(
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
  'title_field_et' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_full_name' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_system_project' => array(
    'sortable' => 0,
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
  'field_merge_daily_reports' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
);
$handler->display->display_options['defaults']['style_options'] = FALSE;
$handler->display->display_options['defaults']['row_plugin'] = FALSE;
$handler->display->display_options['defaults']['row_options'] = FALSE;
$handler->display->display_options['defaults']['footer'] = FALSE;
/* Footer: Global: Text area */
$handler->display->display_options['footer']['area']['id'] = 'area';
$handler->display->display_options['footer']['area']['table'] = 'views';
$handler->display->display_options['footer']['area']['field'] = 'area';
$handler->display->display_options['footer']['area']['empty'] = TRUE;
$handler->display->display_options['footer']['area']['content'] = '<?php

global $user;
$obj = entity_metadata_wrapper(\'user\', $user);
$vrfststus = $obj->field_verified->value();
if ($vrfststus == 1) {
	print \'<a class="btn btn-default" style="color: white; background-color: green;" role="button" href="/node/add/project/">\' . t(\'Add Project\') . \'</a><br/><br/>\';
}
?>';
$handler->display->display_options['footer']['area']['format'] = 'php_code';
$handler->display->display_options['defaults']['empty'] = FALSE;
/* No results behavior: Global: Text area */
$handler->display->display_options['empty']['area']['id'] = 'area';
$handler->display->display_options['empty']['area']['table'] = 'views';
$handler->display->display_options['empty']['area']['field'] = 'area';
$handler->display->display_options['empty']['area']['label'] = 'projects view message';
$handler->display->display_options['empty']['area']['empty'] = TRUE;
$handler->display->display_options['empty']['area']['content'] = 'No projects found using the selected criteria.';
$handler->display->display_options['empty']['area']['format'] = 'plain_text';
$handler->display->display_options['defaults']['relationships'] = FALSE;
/* Relationship: Content: Content author */
$handler->display->display_options['relationships']['uid']['id'] = 'uid';
$handler->display->display_options['relationships']['uid']['table'] = 'node';
$handler->display->display_options['relationships']['uid']['field'] = 'uid';
$handler->display->display_options['defaults']['fields'] = FALSE;
/* Field: Content: Nid */
$handler->display->display_options['fields']['nid']['id'] = 'nid';
$handler->display->display_options['fields']['nid']['table'] = 'node';
$handler->display->display_options['fields']['nid']['field'] = 'nid';
$handler->display->display_options['fields']['nid']['label'] = 'Number';
$handler->display->display_options['fields']['nid']['exclude'] = TRUE;
$handler->display->display_options['fields']['nid']['element_label_colon'] = FALSE;
/* Field: Content: Project ID */
$handler->display->display_options['fields']['field_project_id']['id'] = 'field_project_id';
$handler->display->display_options['fields']['field_project_id']['table'] = 'field_data_field_project_id';
$handler->display->display_options['fields']['field_project_id']['field'] = 'field_project_id';
$handler->display->display_options['fields']['field_project_id']['alter']['make_link'] = TRUE;
$handler->display->display_options['fields']['field_project_id']['alter']['path'] = '/node/[nid]';
/* Field: User: Name */
$handler->display->display_options['fields']['name']['id'] = 'name';
$handler->display->display_options['fields']['name']['table'] = 'users';
$handler->display->display_options['fields']['name']['field'] = 'name';
$handler->display->display_options['fields']['name']['relationship'] = 'uid';
$handler->display->display_options['fields']['name']['label'] = '';
$handler->display->display_options['fields']['name']['exclude'] = TRUE;
$handler->display->display_options['fields']['name']['element_label_colon'] = FALSE;
/* Field: user php */
$handler->display->display_options['fields']['php']['id'] = 'php';
$handler->display->display_options['fields']['php']['table'] = 'views';
$handler->display->display_options['fields']['php']['field'] = 'php';
$handler->display->display_options['fields']['php']['ui_name'] = 'user php';
$handler->display->display_options['fields']['php']['label'] = 'Owner';
$handler->display->display_options['fields']['php']['use_php_setup'] = 0;
$handler->display->display_options['fields']['php']['php_output'] = '<?php
$uid = isset($data->users_node_uid) ? $data->users_node_uid : 0;
echo \'<a href="/user/\'.$uid.\'">\' . $data->users_node_name . \'</a>\';
$user = user_load($uid);
echo render_merit_badge($user);
?>';
$handler->display->display_options['fields']['php']['use_php_click_sortable'] = '0';
$handler->display->display_options['fields']['php']['php_click_sortable'] = '';
/* Field: Entity translation: Title: translated */
$handler->display->display_options['fields']['title_field_et']['id'] = 'title_field_et';
$handler->display->display_options['fields']['title_field_et']['table'] = 'field_data_title_field';
$handler->display->display_options['fields']['title_field_et']['field'] = 'title_field_et';
$handler->display->display_options['fields']['title_field_et']['label'] = 'Name';
$handler->display->display_options['fields']['title_field_et']['type'] = 'title_linked';
$handler->display->display_options['fields']['title_field_et']['settings'] = array(
  'title_style' => '_none',
  'title_link' => 'content',
  'enable_trim' => 0,
  'trim_length' => '50',
  'title_class' => '',
);
/* Field: User: Full name */
$handler->display->display_options['fields']['field_full_name']['id'] = 'field_full_name';
$handler->display->display_options['fields']['field_full_name']['table'] = 'field_data_field_full_name';
$handler->display->display_options['fields']['field_full_name']['field'] = 'field_full_name';
$handler->display->display_options['fields']['field_full_name']['relationship'] = 'uid';
$handler->display->display_options['fields']['field_full_name']['label'] = 'Owner';
$handler->display->display_options['fields']['field_full_name']['exclude'] = TRUE;
/* Field: Content: System project */
$handler->display->display_options['fields']['field_system_project']['id'] = 'field_system_project';
$handler->display->display_options['fields']['field_system_project']['table'] = 'field_data_field_system_project';
$handler->display->display_options['fields']['field_system_project']['field'] = 'field_system_project';
/* Field: Content: Merit reward */
$handler->display->display_options['fields']['field_merit_reward']['id'] = 'field_merit_reward';
$handler->display->display_options['fields']['field_merit_reward']['table'] = 'field_data_field_merit_reward';
$handler->display->display_options['fields']['field_merit_reward']['field'] = 'field_merit_reward';
$handler->display->display_options['fields']['field_merit_reward']['type'] = 'custom_formatters_merits_formatter';
$handler->display->display_options['fields']['field_merit_reward']['settings'] = array(
  'thousand_separator' => '',
  'decimal_separator' => '.',
  'scale' => '2',
  'prefix_suffix' => 1,
);
/* Field: Content: Merge daily reports */
$handler->display->display_options['fields']['field_merge_daily_reports']['id'] = 'field_merge_daily_reports';
$handler->display->display_options['fields']['field_merge_daily_reports']['table'] = 'field_data_field_merge_daily_reports';
$handler->display->display_options['fields']['field_merge_daily_reports']['field'] = 'field_merge_daily_reports';
$handler->display->display_options['fields']['field_merge_daily_reports']['label'] = 'Merging';
$handler->display->display_options['fields']['field_merge_daily_reports']['type'] = 'i18n_list_default';
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
  'project' => 'project',
);
$handler->display->display_options['filters']['type']['group'] = 1;
/* Filter criterion: Content: Project Participation (field_participation) (translated) */
$handler->display->display_options['filters']['field_participation_value_i18n']['id'] = 'field_participation_value_i18n';
$handler->display->display_options['filters']['field_participation_value_i18n']['table'] = 'field_data_field_participation';
$handler->display->display_options['filters']['field_participation_value_i18n']['field'] = 'field_participation_value_i18n';
$handler->display->display_options['filters']['field_participation_value_i18n']['group'] = 1;
$handler->display->display_options['filters']['field_participation_value_i18n']['exposed'] = TRUE;
$handler->display->display_options['filters']['field_participation_value_i18n']['expose']['operator_id'] = 'field_participation_value_i18n_op';
$handler->display->display_options['filters']['field_participation_value_i18n']['expose']['label'] = 'Project Participation';
$handler->display->display_options['filters']['field_participation_value_i18n']['expose']['operator'] = 'field_participation_value_i18n_op';
$handler->display->display_options['filters']['field_participation_value_i18n']['expose']['identifier'] = 'field_participation_value_i18n';
$handler->display->display_options['filters']['field_participation_value_i18n']['expose']['remember'] = TRUE;
$handler->display->display_options['filters']['field_participation_value_i18n']['expose']['multiple'] = TRUE;
$handler->display->display_options['filters']['field_participation_value_i18n']['expose']['remember_roles'] = array(
  2 => '2',
  3 => '3',
  4 => '4',
  6 => '6',
  5 => '5',
  1 => 0,
);
/* Filter criterion: Content: Status (field_status) (translated) */
$handler->display->display_options['filters']['field_status_value_i18n']['id'] = 'field_status_value_i18n';
$handler->display->display_options['filters']['field_status_value_i18n']['table'] = 'field_data_field_status';
$handler->display->display_options['filters']['field_status_value_i18n']['field'] = 'field_status_value_i18n';
$handler->display->display_options['filters']['field_status_value_i18n']['group'] = 1;
$handler->display->display_options['filters']['field_status_value_i18n']['exposed'] = TRUE;
$handler->display->display_options['filters']['field_status_value_i18n']['expose']['operator_id'] = 'field_status_value_i18n_op';
$handler->display->display_options['filters']['field_status_value_i18n']['expose']['label'] = 'Status';
$handler->display->display_options['filters']['field_status_value_i18n']['expose']['operator'] = 'field_status_value_i18n_op';
$handler->display->display_options['filters']['field_status_value_i18n']['expose']['identifier'] = 'field_status_value_i18n';
$handler->display->display_options['filters']['field_status_value_i18n']['expose']['remember'] = TRUE;
$handler->display->display_options['filters']['field_status_value_i18n']['expose']['multiple'] = TRUE;
$handler->display->display_options['filters']['field_status_value_i18n']['expose']['remember_roles'] = array(
  2 => '2',
  3 => '3',
  4 => '4',
  6 => '6',
  5 => '5',
  1 => 0,
);
/* Filter criterion: Content: Title */
$handler->display->display_options['filters']['title']['id'] = 'title';
$handler->display->display_options['filters']['title']['table'] = 'node';
$handler->display->display_options['filters']['title']['field'] = 'title';
$handler->display->display_options['filters']['title']['operator'] = 'contains';
$handler->display->display_options['filters']['title']['group'] = 1;
$handler->display->display_options['filters']['title']['exposed'] = TRUE;
$handler->display->display_options['filters']['title']['expose']['operator_id'] = 'title_op';
$handler->display->display_options['filters']['title']['expose']['label'] = 'Title';
$handler->display->display_options['filters']['title']['expose']['operator'] = 'title_op';
$handler->display->display_options['filters']['title']['expose']['identifier'] = 'title';
$handler->display->display_options['filters']['title']['expose']['remember_roles'] = array(
  2 => '2',
);
$handler->display->display_options['filters']['title']['expose']['autocomplete_items'] = '10';
$handler->display->display_options['filters']['title']['expose']['autocomplete_min_chars'] = '0';
$handler->display->display_options['filters']['title']['expose']['autocomplete_field'] = 'title';
$handler->display->display_options['filters']['title']['expose']['autocomplete_raw_suggestion'] = 1;
$handler->display->display_options['filters']['title']['expose']['autocomplete_raw_dropdown'] = 1;
$handler->display->display_options['filters']['title']['expose']['autocomplete_dependent'] = 0;
/* Filter criterion: User: Name */
$handler->display->display_options['filters']['uid']['id'] = 'uid';
$handler->display->display_options['filters']['uid']['table'] = 'users';
$handler->display->display_options['filters']['uid']['field'] = 'uid';
$handler->display->display_options['filters']['uid']['relationship'] = 'uid';
$handler->display->display_options['filters']['uid']['value'] = '';
$handler->display->display_options['filters']['uid']['group'] = 1;
$handler->display->display_options['filters']['uid']['exposed'] = TRUE;
$handler->display->display_options['filters']['uid']['expose']['operator_id'] = 'uid_op';
$handler->display->display_options['filters']['uid']['expose']['label'] = 'Worker';
$handler->display->display_options['filters']['uid']['expose']['operator'] = 'uid_op';
$handler->display->display_options['filters']['uid']['expose']['identifier'] = 'uid';
$handler->display->display_options['filters']['uid']['expose']['remember'] = TRUE;
$handler->display->display_options['filters']['uid']['expose']['remember_roles'] = array(
  2 => '2',
  3 => '3',
  4 => '4',
  6 => '6',
  5 => '5',
  1 => 0,
);
/* Filter criterion: Content: Author uid */
$handler->display->display_options['filters']['uid_1']['id'] = 'uid_1';
$handler->display->display_options['filters']['uid_1']['table'] = 'node';
$handler->display->display_options['filters']['uid_1']['field'] = 'uid';
$handler->display->display_options['filters']['uid_1']['operator'] = 'not in';
$handler->display->display_options['filters']['uid_1']['value'] = array(
  0 => '1',
);
$handler->display->display_options['filters']['uid_1']['group'] = 1;
/* Filter criterion: Content: System project (field_system_project) (translated) */
$handler->display->display_options['filters']['field_system_project_value_i18n']['id'] = 'field_system_project_value_i18n';
$handler->display->display_options['filters']['field_system_project_value_i18n']['table'] = 'field_data_field_system_project';
$handler->display->display_options['filters']['field_system_project_value_i18n']['field'] = 'field_system_project_value_i18n';
$handler->display->display_options['filters']['field_system_project_value_i18n']['value'] = array(
  0 => '0',
);
$handler->display->display_options['filters']['field_system_project_value_i18n']['group'] = 1;
$handler->display->display_options['filters']['field_system_project_value_i18n']['expose']['operator_id'] = 'field_system_project_value_i18n_op';
$handler->display->display_options['filters']['field_system_project_value_i18n']['expose']['label'] = 'System project';
$handler->display->display_options['filters']['field_system_project_value_i18n']['expose']['operator'] = 'field_system_project_value_i18n_op';
$handler->display->display_options['filters']['field_system_project_value_i18n']['expose']['identifier'] = 'field_system_project_value_i18n';
$handler->display->display_options['filters']['field_system_project_value_i18n']['expose']['multiple'] = TRUE;
$handler->display->display_options['filters']['field_system_project_value_i18n']['expose']['remember_roles'] = array(
  2 => '2',
  1 => 0,
  3 => 0,
  4 => 0,
  6 => 0,
  5 => 0,
);
$handler->display->display_options['filters']['field_system_project_value_i18n']['expose']['reduce'] = TRUE;
$handler->display->display_options['filters']['field_system_project_value_i18n']['is_grouped'] = TRUE;
$handler->display->display_options['filters']['field_system_project_value_i18n']['group_info']['label'] = 'System projects';
$handler->display->display_options['filters']['field_system_project_value_i18n']['group_info']['identifier'] = 'field_system_project_value_i18n';
$handler->display->display_options['filters']['field_system_project_value_i18n']['group_info']['widget'] = 'radios';
$handler->display->display_options['filters']['field_system_project_value_i18n']['group_info']['default_group'] = '1';
$handler->display->display_options['filters']['field_system_project_value_i18n']['group_info']['group_items'] = array(
  1 => array(
    'title' => 'hide',
    'operator' => 'or',
    'value' => array(
      0 => '0',
    ),
  ),
  2 => array(
    'title' => 'show',
    'operator' => 'not',
    'value' => array(
      0 => '0',
    ),
  ),
);
$handler->display->display_options['views_ef_fieldset']['enabled'] = TRUE;
$handler->display->display_options['views_ef_fieldset']['items'] = array(
  'fieldset:5af537d7df218' => array(
    'id' => 'fieldset:5af537d7df218',
    'pid' => '',
    'weight' => '-2',
    'options' => array(
      'label' => 'Show/hide the filters section',
      'collapse' => 'collapsed',
    ),
  ),
  'filter:field_participation_value_i18n' => array(
    'id' => 'filter:field_participation_value_i18n',
    'pid' => 'fieldset:5af537d7df218',
    'weight' => '-7',
  ),
  'filter:field_status_value_i18n' => array(
    'id' => 'filter:field_status_value_i18n',
    'pid' => 'fieldset:5af537d7df218',
    'weight' => '-6',
  ),
  'filter:uid' => array(
    'id' => 'filter:uid',
    'pid' => 'fieldset:5af537d7df218',
    'weight' => '-5',
  ),
  'button:submit' => array(
    'id' => 'button:submit',
    'pid' => 'fieldset:5af537d7df218',
    'weight' => '-3',
  ),
  'button:reset' => array(
    'id' => 'button:reset',
    'pid' => 'fieldset:5af537d7df218',
    'weight' => '-2',
  ),
  'filter:title' => array(
    'id' => 'filter:title',
    'pid' => 'fieldset:5af537d7df218',
    'weight' => '-4',
  ),
);
$handler->display->display_options['path'] = 'find_project';
$handler->display->display_options['menu']['type'] = 'normal';
$handler->display->display_options['menu']['title'] = 'Projects';
$handler->display->display_options['menu']['weight'] = '0';
$handler->display->display_options['menu']['name'] = 'main-menu';
$handler->display->display_options['menu']['context'] = 0;
$handler->display->display_options['menu']['context_only_inline'] = 0;

/* Display: Parent project ER */
$handler = $view->new_display('entityreference', 'Parent project ER', 'entityreference_2');
$handler->display->display_options['defaults']['title'] = FALSE;
$handler->display->display_options['pager']['type'] = 'some';
$handler->display->display_options['defaults']['style_plugin'] = FALSE;
$handler->display->display_options['style_plugin'] = 'entityreference_style';
$handler->display->display_options['style_options']['search_fields'] = array(
  'nid' => 'nid',
  'title' => 'title',
);
$handler->display->display_options['defaults']['style_options'] = FALSE;
$handler->display->display_options['defaults']['row_plugin'] = FALSE;
$handler->display->display_options['row_plugin'] = 'entityreference_fields';
$handler->display->display_options['row_options']['default_field_elements'] = FALSE;
$handler->display->display_options['defaults']['row_options'] = FALSE;
$handler->display->display_options['defaults']['footer'] = FALSE;
$handler->display->display_options['defaults']['empty'] = FALSE;
$handler->display->display_options['defaults']['relationships'] = FALSE;
/* Relationship: Entity Reference: Referencing entity */
$handler->display->display_options['relationships']['reverse_field_project_node']['id'] = 'reverse_field_project_node';
$handler->display->display_options['relationships']['reverse_field_project_node']['table'] = 'node';
$handler->display->display_options['relationships']['reverse_field_project_node']['field'] = 'reverse_field_project_node';
$handler->display->display_options['relationships']['reverse_field_project_node']['label'] = 'R field_project';
/* Relationship: Entity Reference: Referenced Entity */
$handler->display->display_options['relationships']['field_shareholder_project_target_id']['id'] = 'field_shareholder_project_target_id';
$handler->display->display_options['relationships']['field_shareholder_project_target_id']['table'] = 'field_data_field_shareholder_project';
$handler->display->display_options['relationships']['field_shareholder_project_target_id']['field'] = 'field_shareholder_project_target_id';
$handler->display->display_options['relationships']['field_shareholder_project_target_id']['relationship'] = 'reverse_field_project_node';
$handler->display->display_options['relationships']['field_shareholder_project_target_id']['label'] = 'R field_shareholder_project';
$handler->display->display_options['defaults']['fields'] = FALSE;
/* Field: Content: Nid */
$handler->display->display_options['fields']['nid']['id'] = 'nid';
$handler->display->display_options['fields']['nid']['table'] = 'node';
$handler->display->display_options['fields']['nid']['field'] = 'nid';
$handler->display->display_options['fields']['nid']['label'] = '';
$handler->display->display_options['fields']['nid']['exclude'] = TRUE;
$handler->display->display_options['fields']['nid']['element_label_colon'] = FALSE;
/* Field: Content: Title */
$handler->display->display_options['fields']['title']['id'] = 'title';
$handler->display->display_options['fields']['title']['table'] = 'node';
$handler->display->display_options['fields']['title']['field'] = 'title';
$handler->display->display_options['fields']['title']['label'] = '';
$handler->display->display_options['fields']['title']['alter']['alter_text'] = TRUE;
$handler->display->display_options['fields']['title']['alter']['text'] = '[nid] - [title]';
$handler->display->display_options['fields']['title']['element_label_colon'] = FALSE;
$handler->display->display_options['defaults']['arguments'] = FALSE;
/* Contextual filter: Content: Nid */
$handler->display->display_options['arguments']['nid']['id'] = 'nid';
$handler->display->display_options['arguments']['nid']['table'] = 'node';
$handler->display->display_options['arguments']['nid']['field'] = 'nid';
$handler->display->display_options['arguments']['nid']['relationship'] = 'field_shareholder_project_target_id';
$handler->display->display_options['arguments']['nid']['default_action'] = 'default';
$handler->display->display_options['arguments']['nid']['default_argument_type'] = 'node';
$handler->display->display_options['arguments']['nid']['summary']['number_of_records'] = '0';
$handler->display->display_options['arguments']['nid']['summary']['format'] = 'default_summary';
$handler->display->display_options['arguments']['nid']['summary_options']['items_per_page'] = '25';
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
  'project' => 'project',
  'project_merit_allocation' => 'project_merit_allocation',
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
$translatables['projects'] = array(
  t('Master'),
  t('Public Projects'),
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
  t('‹'),
  t('›'),
  t('last »'),
  t('<?php
global $user;
$obj = entity_metadata_wrapper(\'user\', $user);
$vrfststus = $obj->field_verified->value();
if ($vrfststus == 1) {
	print \'[<a class="btn btn-default btn-xs" style="color: white; background-color: green;" role="button" href="/node/add/project/">\' . t(\'Add Project\') . \'</a><br/><br/>\';
}
?>'),
  t('projects view message'),
  t('<?php 
print t(\'You have created no projects yet.\');
?>'),
  t('Content referencing Content from field_project'),
  t('Number'),
  t('Date'),
  t('Name'),
  t('Chrons'),
  t('Action'),
  t('<a href="/node/[nid]/edit"><img src="/sites/default/files/pictures/edit.svg" width="16" /></a>&nbsp;&nbsp;<a href="/node/[nid]/delete"><img src="/sites/default/files/pictures/delete.svg" width="16" /></a>'),
  t('Project owner'),
  t('All'),
  t('Project Participation (field_participation)'),
  t('Block'),
  t('Projects I work on'),
  t('«'),
  t('»'),
  t('<?php
global $user;
$obj = entity_metadata_wrapper(\'user\', $user);
$vrfststus = $obj->field_verified->value();
if ($vrfststus == 1) {
	print \'<br/><a class="btn btn-default" style="color: white; background-color: green;" role="button" href="/node/add/project/">\' . t(\'Add Project\') . \'</a><br/><br/>\';
}
?>'),
  t('author'),
  t('[name]'),
  t('Participation'),
  t('Status'),
  t('Merits'),
  t('Projects ER'),
  t('Entity relation view to show candidate projects for various fields'),
  t('Merit allocation Shareholder'),
  t('Entity relation view to show candidate projects for the Shareholder field'),
  t('Projects page'),
  t('Find Project'),
  t('Select any filter and click on Apply to see results'),
  t('Advanced options'),
  t('<?php

global $user;
$obj = entity_metadata_wrapper(\'user\', $user);
$vrfststus = $obj->field_verified->value();
if ($vrfststus == 1) {
	print \'<a class="btn btn-default" style="color: white; background-color: green;" role="button" href="/node/add/project/">\' . t(\'Add Project\') . \'</a><br/><br/>\';
}
?>'),
  t('No projects found using the selected criteria.'),
  t('Project ID'),
  t('Owner'),
  t('System project'),
  t('Merit reward'),
  t('Merging'),
  t('Project Participation'),
  t('Title'),
  t('Worker'),
  t('System projects'),
  t('Parent project ER'),
  t('R field_project'),
  t('R field_shareholder_project'),
  t('[nid] - [title]'),
);

