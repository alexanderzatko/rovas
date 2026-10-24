<?php

/**
 * @file
 * Default view: work_reports
 */

$view = new view();
$view->name = 'work_reports';
$view->description = '';
$view->tag = 'default';
$view->base_table = 'node';
$view->human_name = 'Work Reports';
$view->core = 7;
$view->api_version = '3.0';
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

/* Display: Master */
$handler = $view->new_display('default', 'Master', 'default');
$handler->display->display_options['use_ajax'] = TRUE;
$handler->display->display_options['use_more_always'] = FALSE;
$handler->display->display_options['access']['type'] = 'none';
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
$handler->display->display_options['style_options']['row_class'] = 'wr-row-[field_verification_status_1]';
$handler->display->display_options['style_options']['columns'] = array(
  'nid' => 'nid',
  'field_work_report_id' => 'field_work_report_id',
  'field_project' => 'field_project',
  'created' => 'created',
  'field_date' => 'field_date',
  'uid' => 'uid',
  'field_full_name' => 'field_full_name',
  'name' => 'name',
  'title_field_et' => 'title_field_et',
  'field_classification' => 'field_classification',
  'field_hours_worked' => 'field_hours_worked',
  'php_1' => 'php_1',
  'field_verification_status' => 'field_verification_status',
  'field_verification_status_icons' => 'field_verification_status',
  'php' => 'php',
  'field_verification_status_1' => 'field_verification_status_1',
);
$handler->display->display_options['style_options']['class'] = '';
$handler->display->display_options['style_options']['default'] = 'created';
$handler->display->display_options['style_options']['info'] = array(
  'nid' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_work_report_id' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_project' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'created' => array(
    'sortable' => 1,
    'default_sort_order' => 'desc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_date' => array(
    'sortable' => 1,
    'default_sort_order' => 'desc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'uid' => array(
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
  'title_field_et' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_classification' => array(
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
  'php_1' => array(
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_verification_status' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => 'views-align-center',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_verification_status_icons' => array(
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
  'field_verification_status_1' => array(
    'sortable' => 0,
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
if (is_numeric(arg(1))) {
	$prid = arg(1);
	$project = node_load($prid);
	$wrapper = entity_metadata_wrapper(\'node\', $project);
	$stage = $wrapper->field_status->value();

	global $user;
	$obj = entity_metadata_wrapper(\'user\', $user);
	$vrfststus = $obj->field_verified->value();


	$result = db_query("SELECT *
	FROM {field_data_field_coworker} w, {field_data_field_project} p, {node} n
	WHERE w.entity_id = p.entity_id
	AND p.field_project_target_id = n.nid
	AND p.bundle = \'co_worker_merit_allocation\'
	AND n.nid =:node
	AND w.field_coworker_target_id =:usr", array(\':node\' => arg(1), \':usr\' => $user->uid));
	$resarray = $result->fetchAll();

//allow adding reports only for projects in stages 0 and 1
//only verified and not accused users can add work report; banned users can add reports only to not owned, private projects
	if (!user_has_role(6) && $vrfststus == 1) {
		if ($stage < 2) {
			//allow adding report if user is in the list of shareholders
			//banned users can add reports only to private projects they do not own
			if($resarray != NULL && !user_has_role(5)) {
				print \'<a class="add-report btn btn-default btn-xs" style="color: white; background-color: green;" role="button" href="/node/add/work-report/\' . arg(1) . \'">\' . t(\'Add Report\') . \'</a><br/><br/>\';
			}
			else if($resarray != NULL && (user_has_role(5) && $wrapper->author->getIdentifier() != $user->uid && $wrapper->field_participation->value() == 0) ) {
				print \'<a class="add-report btn btn-default btn-xs" style="color: white; background-color: green;" role="button" href="/node/add/work-report/\' . arg(1) . \'">\' . t(\'Add Report\') . \'</a><br/><br/>\';
			}
			else {
				print \'<a class="add-report btn btn-default btn-xs disabled" style="color: white; background-color: gray;" role="button" href="#">\' . t(\'Add Report\') . \'</a> <span style="color: lightgray;">\' . t(\'Only not banned shareholders can add work reports.\') . \'</span><br/><br/>\';
			}
		}
		else {
			print \'<a class="add-report btn btn-default btn-xs disabled" style="color: white; background-color: gray;" role="button" href="#">\' . t(\'Add Report\') . \'</a> <span style="color: lightgray;">\' . t(\'This project is closed because its product was sold or expired.\') . \'</span><br/><br/>\';
		}
	}
}
?>';
$handler->display->display_options['footer']['area']['format'] = 'php_code';
/* No results behavior: Global: Text area */
$handler->display->display_options['empty']['area']['id'] = 'area';
$handler->display->display_options['empty']['area']['table'] = 'views';
$handler->display->display_options['empty']['area']['field'] = 'area';
$handler->display->display_options['empty']['area']['empty'] = TRUE;
$handler->display->display_options['empty']['area']['content'] = 'Nobody submitted a work report yet.';
$handler->display->display_options['empty']['area']['format'] = 'filtered_html';
/* Relationship: Content: Content author */
$handler->display->display_options['relationships']['uid']['id'] = 'uid';
$handler->display->display_options['relationships']['uid']['table'] = 'node';
$handler->display->display_options['relationships']['uid']['field'] = 'uid';
/* Field: Content: Nid */
$handler->display->display_options['fields']['nid']['id'] = 'nid';
$handler->display->display_options['fields']['nid']['table'] = 'node';
$handler->display->display_options['fields']['nid']['field'] = 'nid';
$handler->display->display_options['fields']['nid']['label'] = '';
$handler->display->display_options['fields']['nid']['exclude'] = TRUE;
$handler->display->display_options['fields']['nid']['element_label_colon'] = FALSE;
/* Field: Content: Work report ID */
$handler->display->display_options['fields']['field_work_report_id']['id'] = 'field_work_report_id';
$handler->display->display_options['fields']['field_work_report_id']['table'] = 'field_data_field_work_report_id';
$handler->display->display_options['fields']['field_work_report_id']['field'] = 'field_work_report_id';
$handler->display->display_options['fields']['field_work_report_id']['label'] = 'Work report ID';
$handler->display->display_options['fields']['field_work_report_id']['alter']['make_link'] = TRUE;
$handler->display->display_options['fields']['field_work_report_id']['alter']['path'] = 'node/[nid]';
$handler->display->display_options['fields']['field_work_report_id']['settings'] = array(
  'thousand_separator' => '',
  'prefix_suffix' => 1,
);
/* Field: Content: Project */
$handler->display->display_options['fields']['field_project']['id'] = 'field_project';
$handler->display->display_options['fields']['field_project']['table'] = 'field_data_field_project';
$handler->display->display_options['fields']['field_project']['field'] = 'field_project';
$handler->display->display_options['fields']['field_project']['label'] = '';
$handler->display->display_options['fields']['field_project']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_project']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_project']['type'] = 'entityreference_entity_id';
$handler->display->display_options['fields']['field_project']['settings'] = array(
  'bypass_access' => 0,
  'link' => 0,
);
/* Field: Content: Post date */
$handler->display->display_options['fields']['created']['id'] = 'created';
$handler->display->display_options['fields']['created']['table'] = 'node';
$handler->display->display_options['fields']['created']['field'] = 'created';
$handler->display->display_options['fields']['created']['label'] = 'Added';
$handler->display->display_options['fields']['created']['date_format'] = 'custom';
$handler->display->display_options['fields']['created']['custom_date_format'] = 'd M Y';
$handler->display->display_options['fields']['created']['second_date_format'] = 'privatemsg_current_day';
/* Field: Content: Date */
$handler->display->display_options['fields']['field_date']['id'] = 'field_date';
$handler->display->display_options['fields']['field_date']['table'] = 'field_data_field_date';
$handler->display->display_options['fields']['field_date']['field'] = 'field_date';
$handler->display->display_options['fields']['field_date']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_date']['settings'] = array(
  'format_type' => 'custom',
  'custom_date_format' => 'd m Y',
  'fromto' => 'both',
  'multiple_number' => '',
  'multiple_from' => '',
  'multiple_to' => '',
  'show_remaining_days' => 0,
);
/* Field: Content: Author uid */
$handler->display->display_options['fields']['uid']['id'] = 'uid';
$handler->display->display_options['fields']['uid']['table'] = 'node';
$handler->display->display_options['fields']['uid']['field'] = 'uid';
$handler->display->display_options['fields']['uid']['label'] = '';
$handler->display->display_options['fields']['uid']['exclude'] = TRUE;
$handler->display->display_options['fields']['uid']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['uid']['link_to_user'] = FALSE;
/* Field: User: Full name */
$handler->display->display_options['fields']['field_full_name']['id'] = 'field_full_name';
$handler->display->display_options['fields']['field_full_name']['table'] = 'field_data_field_full_name';
$handler->display->display_options['fields']['field_full_name']['field'] = 'field_full_name';
$handler->display->display_options['fields']['field_full_name']['relationship'] = 'uid';
$handler->display->display_options['fields']['field_full_name']['label'] = '';
$handler->display->display_options['fields']['field_full_name']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_full_name']['element_label_colon'] = FALSE;
/* Field: User: Name */
$handler->display->display_options['fields']['name']['id'] = 'name';
$handler->display->display_options['fields']['name']['table'] = 'users';
$handler->display->display_options['fields']['name']['field'] = 'name';
$handler->display->display_options['fields']['name']['relationship'] = 'uid';
$handler->display->display_options['fields']['name']['label'] = 'Worker';
$handler->display->display_options['fields']['name']['alter']['alter_text'] = TRUE;
$handler->display->display_options['fields']['name']['alter']['text'] = '[name]';
$handler->display->display_options['fields']['name']['overwrite_anonymous'] = TRUE;
$handler->display->display_options['fields']['name']['anonymous_text'] = 'user';
/* Field: Entity translation: Title: translated */
$handler->display->display_options['fields']['title_field_et']['id'] = 'title_field_et';
$handler->display->display_options['fields']['title_field_et']['table'] = 'field_data_title_field';
$handler->display->display_options['fields']['title_field_et']['field'] = 'title_field_et';
$handler->display->display_options['fields']['title_field_et']['label'] = 'Activity';
$handler->display->display_options['fields']['title_field_et']['type'] = 'title_linked';
$handler->display->display_options['fields']['title_field_et']['settings'] = array(
  'title_style' => '_none',
  'title_link' => 'content',
  'enable_trim' => 0,
  'trim_length' => '50',
  'title_class' => '',
);
/* Field: Field: Classification */
$handler->display->display_options['fields']['field_classification']['id'] = 'field_classification';
$handler->display->display_options['fields']['field_classification']['table'] = 'field_data_field_classification';
$handler->display->display_options['fields']['field_classification']['field'] = 'field_classification';
$handler->display->display_options['fields']['field_classification']['settings'] = array(
  'token_display_selected' => '',
  'token_display_unselected' => '',
);
$handler->display->display_options['fields']['field_classification']['delta_limit'] = '1';
$handler->display->display_options['fields']['field_classification']['delta_offset'] = '0';
/* Field: Content: Time worked */
$handler->display->display_options['fields']['field_hours_worked']['id'] = 'field_hours_worked';
$handler->display->display_options['fields']['field_hours_worked']['table'] = 'field_data_field_hours_worked';
$handler->display->display_options['fields']['field_hours_worked']['field'] = 'field_hours_worked';
$handler->display->display_options['fields']['field_hours_worked']['label'] = 'Time';
$handler->display->display_options['fields']['field_hours_worked']['type'] = 'custom_formatters_labor_time_formatter';
$handler->display->display_options['fields']['field_hours_worked']['settings'] = array(
  'thousand_separator' => '',
  'decimal_separator' => '.',
  'scale' => '2',
  'prefix_suffix' => 1,
);
/* Field: default label for missing status */
$handler->display->display_options['fields']['php_1']['id'] = 'php_1';
$handler->display->display_options['fields']['php_1']['table'] = 'views';
$handler->display->display_options['fields']['php_1']['field'] = 'php';
$handler->display->display_options['fields']['php_1']['ui_name'] = 'default label for missing status';
$handler->display->display_options['fields']['php_1']['label'] = '';
$handler->display->display_options['fields']['php_1']['exclude'] = TRUE;
$handler->display->display_options['fields']['php_1']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['php_1']['use_php_setup'] = 0;
$handler->display->display_options['fields']['php_1']['php_output'] = '<?php 
echo t(\'not verified\');
?>';
$handler->display->display_options['fields']['php_1']['use_php_click_sortable'] = '0';
$handler->display->display_options['fields']['php_1']['php_click_sortable'] = '';
/* Field: Content: Verification status */
$handler->display->display_options['fields']['field_verification_status']['id'] = 'field_verification_status';
$handler->display->display_options['fields']['field_verification_status']['table'] = 'field_data_field_verification_status';
$handler->display->display_options['fields']['field_verification_status']['field'] = 'field_verification_status';
$handler->display->display_options['fields']['field_verification_status']['label'] = 'Validity';
$handler->display->display_options['fields']['field_verification_status']['element_type'] = 'p';
$handler->display->display_options['fields']['field_verification_status']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_verification_status']['empty'] = '[php_1]';
$handler->display->display_options['fields']['field_verification_status']['type'] = 'i18n_list_default';
$handler->display->display_options['fields']['field_verification_status']['settings'] = array(
  'thousand_separator' => '',
  'prefix_suffix' => 0,
);
/* Field: Content: Verification status icons */
$handler->display->display_options['fields']['field_verification_status_icons']['id'] = 'field_verification_status_icons';
$handler->display->display_options['fields']['field_verification_status_icons']['table'] = 'field_data_field_verification_status_icons';
$handler->display->display_options['fields']['field_verification_status_icons']['field'] = 'field_verification_status_icons';
$handler->display->display_options['fields']['field_verification_status_icons']['label'] = '';
$handler->display->display_options['fields']['field_verification_status_icons']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_verification_status_icons']['type'] = 'computed_field_unsanitized';
/* Field: actions */
$handler->display->display_options['fields']['php']['id'] = 'php';
$handler->display->display_options['fields']['php']['table'] = 'views';
$handler->display->display_options['fields']['php']['field'] = 'php';
$handler->display->display_options['fields']['php']['ui_name'] = 'actions';
$handler->display->display_options['fields']['php']['label'] = 'Action';
$handler->display->display_options['fields']['php']['use_php_setup'] = 0;
$handler->display->display_options['fields']['php']['php_output'] = '<?php 

global $user;
$prid = $data->field_field_project[0][\'raw\'][\'target_id\'];
$node = node_load($data->nid);

$verifiable = rules_invoke_component(\'rules_verifiability\',$node,$user,0);
$editable = rules_invoke_component(\'rules_work_report_editability\',$node,$user);
$deletable = rules_invoke_component(\'rules_work_report_deletability\',$node,$user);
$transferable = _report_transfer_privileges($data->nid);

//edit button
	if ($editable[0]) {
		print \'<a href="/node/\' . $row->nid . \'/edit?destination=node/\'. $prid . \'"><img src="/sites/default/files/pictures/edit.svg" width="16" title="\' . t(\'edit\') . \'" /></a>&nbsp;\';
	}
	else {
		print \'<img src="/sites/default/files/pictures/edit-off.svg" width="16" title="\' . t(\'edit\') . \'" />&nbsp;\';
	}

//delete button
	if ($deletable[0]) {
		print \'<a href="/node/\' . $row->nid . \'/delete?destination=node/\' . $prid . \'"><img src="/sites/default/files/pictures/delete.svg" width="16" title="\' . t(\'delete\') . \'" /></a>&nbsp;\';
	}
	else {
		print \'<img src="/sites/default/files/pictures/delete-off.svg" width="16" title="\' . t(\'delete\') . \'" />&nbsp;\';
	}
//verify button
	if ($verifiable[0]) {
		print \'<a href="/node/\' . $row->nid . \'"><span style="font-size: 12pt; top: 3px;" class="glyphicon glyphicon-scale" title="\' . t(\'verify\') . \'"></span></a>&nbsp;\';
	}
	else {
		print \'<span class="verf-icon-undecided glyphicon glyphicon-scale" title="\' . t(\'verify\') . \'" /></span>&nbsp;\';
	}
//transfer button
	if ($transferable) {
		print \'<a href="/transfer-report/\' . $row->nid . \'"><span style="font-size: 12pt; top: 3px;" class="glyphicon glyphicon-send" title="\' . t(\'transfer\') . \'"></span></a>&nbsp;\';
	}
	else {
		print \'<span class="verf-icon-undecided glyphicon glyphicon-send" title="\' . t(\'transfer\') . \'" /></span>&nbsp;\';
	}

?>';
$handler->display->display_options['fields']['php']['use_php_click_sortable'] = '0';
$handler->display->display_options['fields']['php']['php_click_sortable'] = '';
/* Field: vsforcss */
$handler->display->display_options['fields']['field_verification_status_1']['id'] = 'field_verification_status_1';
$handler->display->display_options['fields']['field_verification_status_1']['table'] = 'field_data_field_verification_status';
$handler->display->display_options['fields']['field_verification_status_1']['field'] = 'field_verification_status';
$handler->display->display_options['fields']['field_verification_status_1']['ui_name'] = 'vsforcss';
$handler->display->display_options['fields']['field_verification_status_1']['label'] = '';
$handler->display->display_options['fields']['field_verification_status_1']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_verification_status_1']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_verification_status_1']['empty'] = '0';
$handler->display->display_options['fields']['field_verification_status_1']['type'] = 'list_key';
/* Sort criterion: Content: Post date */
$handler->display->display_options['sorts']['created']['id'] = 'created';
$handler->display->display_options['sorts']['created']['table'] = 'node';
$handler->display->display_options['sorts']['created']['field'] = 'created';
$handler->display->display_options['sorts']['created']['order'] = 'DESC';
/* Contextual filter: Content: Project (field_project) */
$handler->display->display_options['arguments']['field_project_target_id']['id'] = 'field_project_target_id';
$handler->display->display_options['arguments']['field_project_target_id']['table'] = 'field_data_field_project';
$handler->display->display_options['arguments']['field_project_target_id']['field'] = 'field_project_target_id';
$handler->display->display_options['arguments']['field_project_target_id']['default_action'] = 'default';
$handler->display->display_options['arguments']['field_project_target_id']['default_argument_type'] = 'node';
$handler->display->display_options['arguments']['field_project_target_id']['summary']['number_of_records'] = '0';
$handler->display->display_options['arguments']['field_project_target_id']['summary']['format'] = 'default_summary';
$handler->display->display_options['arguments']['field_project_target_id']['summary_options']['items_per_page'] = '25';
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
$translatables['work_reports'] = array(
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
  t('«'),
  t('‹'),
  t('›'),
  t('»'),
  t('<?php
if (is_numeric(arg(1))) {
	$prid = arg(1);
	$project = node_load($prid);
	$wrapper = entity_metadata_wrapper(\'node\', $project);
	$stage = $wrapper->field_status->value();

	global $user;
	$obj = entity_metadata_wrapper(\'user\', $user);
	$vrfststus = $obj->field_verified->value();


	$result = db_query("SELECT *
	FROM {field_data_field_coworker} w, {field_data_field_project} p, {node} n
	WHERE w.entity_id = p.entity_id
	AND p.field_project_target_id = n.nid
	AND p.bundle = \'co_worker_merit_allocation\'
	AND n.nid =:node
	AND w.field_coworker_target_id =:usr", array(\':node\' => arg(1), \':usr\' => $user->uid));
	$resarray = $result->fetchAll();

//allow adding reports only for projects in stages 0 and 1
//only verified and not accused users can add work report; banned users can add reports only to not owned, private projects
	if (!user_has_role(6) && $vrfststus == 1) {
		if ($stage < 2) {
			//allow adding report if user is in the list of shareholders
			//banned users can add reports only to private projects they do not own
			if($resarray != NULL && !user_has_role(5)) {
				print \'<a class="add-report btn btn-default btn-xs" style="color: white; background-color: green;" role="button" href="/node/add/work-report/\' . arg(1) . \'">\' . t(\'Add Report\') . \'</a><br/><br/>\';
			}
			else if($resarray != NULL && (user_has_role(5) && $wrapper->author->getIdentifier() != $user->uid && $wrapper->field_participation->value() == 0) ) {
				print \'<a class="add-report btn btn-default btn-xs" style="color: white; background-color: green;" role="button" href="/node/add/work-report/\' . arg(1) . \'">\' . t(\'Add Report\') . \'</a><br/><br/>\';
			}
			else {
				print \'<a class="add-report btn btn-default btn-xs disabled" style="color: white; background-color: gray;" role="button" href="#">\' . t(\'Add Report\') . \'</a> <span style="color: lightgray;">\' . t(\'Only not banned shareholders can add work reports.\') . \'</span><br/><br/>\';
			}
		}
		else {
			print \'<a class="add-report btn btn-default btn-xs disabled" style="color: white; background-color: gray;" role="button" href="#">\' . t(\'Add Report\') . \'</a> <span style="color: lightgray;">\' . t(\'This project is closed because its product was sold or expired.\') . \'</span><br/><br/>\';
		}
	}
}
?>'),
  t('Nobody submitted a work report yet.'),
  t('author'),
  t('Work report ID'),
  t('Added'),
  t('Date'),
  t('Worker'),
  t('[name]'),
  t('user'),
  t('Activity'),
  t('Classification'),
  t('Time'),
  t('Validity'),
  t('[php_1]'),
  t('Action'),
  t('All'),
);

