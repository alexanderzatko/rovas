<?php

/**
 * @file
 * rovas_purchase_shares.views_default.inc
 */

/**
 * Implements hook_views_default_views().
 */
function rovas_purchase_shares_views_default_views() {
  $export = array();

	$view = new view();
	$view->name = 'project_coworkers';
	$view->description = '';
	$view->tag = 'default';
	$view->base_table = 'node';
	$view->human_name = 'Product Coworkers';
	$view->core = 7;
	$view->api_version = '3.0';
	$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

	/* Display: Master */
	$handler = $view->new_display('default', 'Master', 'default');
	$handler->display->display_options['title'] = 'Product Makers';
	$handler->display->display_options['use_ajax'] = TRUE;
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
	$handler->display->display_options['style_plugin'] = 'table';
	$handler->display->display_options['style_options']['columns'] = array(
		'nid_1' => 'nid_1',
		'nid' => 'nid',
		'uid' => 'uid',
		'field_project_id' => 'field_project_id',
		'title' => 'title',
		'name' => 'name',
		'field_full_name' => 'field_full_name',
		'php_1' => 'php_1',
		'field_merit_allocation' => 'field_merit_allocation',
		'php_4' => 'php_4',
		'field_total_hours_worked' => 'field_total_hours_worked',
		'uid_1' => 'uid_1',
		'field_participation' => 'field_participation',
		'field_merits_earned' => 'field_merits_earned',
		'php' => 'php',
	);
	$handler->display->display_options['style_options']['class'] = '';
	$handler->display->display_options['style_options']['default'] = 'field_total_hours_worked';
	$handler->display->display_options['style_options']['info'] = array(
		'nid_1' => array(
			'sortable' => 0,
			'default_sort_order' => 'asc',
			'align' => '',
			'separator' => '',
			'empty_column' => 0,
		),
		'nid' => array(
			'sortable' => 0,
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
		'field_project_id' => array(
			'sortable' => 0,
			'default_sort_order' => 'asc',
			'align' => '',
			'separator' => '',
			'empty_column' => 0,
		),
		'title' => array(
			'sortable' => 0,
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
		'field_full_name' => array(
			'sortable' => 0,
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
		'field_merit_allocation' => array(
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
		'field_total_hours_worked' => array(
			'sortable' => 1,
			'default_sort_order' => 'desc',
			'align' => '',
			'separator' => '',
			'empty_column' => 0,
		),
		'uid_1' => array(
			'sortable' => 0,
			'default_sort_order' => 'asc',
			'align' => '',
			'separator' => '',
			'empty_column' => 0,
		),
		'field_participation' => array(
			'sortable' => 0,
			'default_sort_order' => 'asc',
			'align' => '',
			'separator' => '',
			'empty_column' => 0,
		),
		'field_merits_earned' => array(
			'sortable' => 1,
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
	);
	/* Header: Global: PHP */
	$handler->display->display_options['header']['php']['id'] = 'php';
	$handler->display->display_options['header']['php']['table'] = 'views';
	$handler->display->display_options['header']['php']['field'] = 'php';
	$handler->display->display_options['header']['php']['php_output'] = '<?php 

	if (is_numeric(arg(1))) {
		$prid = arg(1);
		$obj = entity_metadata_wrapper(\'node\', $prid);
		$field_total_shares = $obj->field_total_shares->value();

			if($field_total_shares > 100) {
					print \'<span style="color: red;"> \'. t(\'Warning: The sum of Merit shares is more than 100%! The product can not be offered for sale until this problem is fixed.\') . \'</span>\';
			}
			else if ($field_total_shares == "" || $field_total_shares == 0) {
					print \'<span style="color: gray;"> \'. t(\'Any Merit reward will be distributed among shareholders in proportion to the number of hours reported\') . \'</span>\';
			}
			else if ($field_total_shares == 100) {
					print \'<span style="color: gray;"> \'. t(\'Any Merit reward will be distributed among shareholders according to shares shown.\') . \'</span>\';
			}
			else {
					print \'<span style="color: gray;"> \'. t(\'Any Merit reward will be distributed among shareholders according to shown shares.\') . \' \' . t(\'The remaining\') . \' \' . round(100 - $field_total_shares,2) . t(\'% of Merits will be distributed among listed shareholders according to the number of hours reported.\') . \'</span>\';
			}

			if($obj->field_allow_purchasing_shares->value() == 1) {
				$tip = t(\'Buy a share of Merits in this project\');
				$nid = $entity->nid;
	
				print \'<a class="btn btn-default" href="/rewpro?paytype=buyshares&recipient=\'.$prid.\'" role="button" style="color:white; background-color:#695a8c;" title="\'.$tip.\'">\'. t(\'Buy shares in this project\') .\'</a>&nbsp;&nbsp;\';
			}

		}
	?>';
	/* Footer: Global: PHP */
	$handler->display->display_options['footer']['php']['id'] = 'php';
	$handler->display->display_options['footer']['php']['table'] = 'views';
	$handler->display->display_options['footer']['php']['field'] = 'php';
	$handler->display->display_options['footer']['php']['label'] = 'add worker button';
	$handler->display->display_options['footer']['php']['empty'] = TRUE;
	$handler->display->display_options['footer']['php']['php_output'] = '<?php
	if (is_numeric($view->args[0]))
	$prid = $view->args[0];
	else if (is_numeric($view->args[1]))
	$prid = $view->args[1];
	else
	$prid = 17; //made-up number

	$project = node_load($prid);
	$wrapper = entity_metadata_wrapper(\'node\', $project);
	$ownership = $wrapper->field_participation->value();
	$stage = $wrapper->field_status->value();
	$prowner = $wrapper->author->uid->value();
	global $user;
	$obj = entity_metadata_wrapper(\'user\', $user);
	$vrfststus = $obj->field_verified->value();


	$uid_found = \'no\';

	//determine if the active user already has a record in the shareholder list
	if(isset($results)) {
		foreach ($results as $rec) {
			if ($rec->users_field_data_field_coworker_uid == $user->uid) {
				$uid_found = \'yes\';
				break;
				}
		}
	}

	//only verified and not in-dispute-accused users can see the buttons
	if ($vrfststus == 1 && !isset($user->roles[5]) && !isset($user->roles[6])) {
		//do not allow adding workers for sold or expired products
		if ($stage < 2) {
			//if private, only owner and admin can add workers.
			if($ownership == 0) {
				if($user->uid == 1|| $user->uid == $prowner) {
					echo \'<a class="btn btn-default btn-xs" style="color: white; background-color: green;" role="button" href="/node/add/co-worker-merit-allocation/\' . arg(1) . \'">\' . t(\'Add Worker Shareholder\') . \'</a>&nbsp;&nbsp;\';
					echo \'<a class="btn btn-default btn-xs" style="color: white; background-color: green;" role="button" href="/node/add/project-merit-allocation/\' . arg(1) . \'">\' . t(\'Add Project Shareholder\') . \'</a><br/><br/>\';
				}
				else {
					echo \'<a class="btn btn-default btn-xs disabled" style="color: white; background-color: gray;" role="button" href="#">\' . t(\'Add Worker Shareholder\') . \'</a> <span style="color: lightgray;"></span>&nbsp;&nbsp;\';
					echo \'<a class="btn btn-default btn-xs disabled" style="color: white; background-color: gray;" role="button" href="#">\' . t(\'Add Project Shareholder\') . \'</a> <span style="color: lightgray;">\' . t(\'Only the owner can add shareholders when Participation is set to "private".\') . \'</span><br/><br/>\';
				}
			}

			//if public and 
			else {
			//if user does not have a record yet, app should allow him/her to add him/herself.
				if($uid_found == \'no\') {
					print \'<a class="btn btn-default btn-xs" style="color: white; background-color: green;" role="button" href="/node/\'.$prid.\'/addmyself">\' . t(\'Add Yourself\') . \'</a><br/><br/>\';
				}
				else {
					print \'<a class="btn btn-default btn-xs disabled" style="color: white; background-color: gray;" role="button" href="#">\' . t(\'Add Yourself\') . \'</a> <span style="color: lightgray;">\' . t(\'Your are already a shareholder.\') . \'</span><br/><br/>\';
				}
			}
		}
		else {
			if($ownership == 0) {
				print \'<a class="btn btn-default btn-xs disabled" style="color: white; background-color: gray;" role="button" href="#">\' . t(\'Add Worker\') . \'</a> <span style="color: lightgray;">\' . t(\'This project is closed because its product was sold or expired.\') . \'</span><br/><br/>\';
				}
			else {
				print \'<a class="btn btn-default btn-xs disabled" style="color: white; background-color: gray;" role="button" href="#">\' . t(\'Add Yourself\') . \'</a> <span style="color: lightgray;">\' . t(\'This project is closed because its product was sold or expired.\') . \'</span><br/><br/>\';
				}
		}
	}
	?>';
	/* Relationship: Entity Reference: Referenced Entity */
	$handler->display->display_options['relationships']['field_project_target_id']['id'] = 'field_project_target_id';
	$handler->display->display_options['relationships']['field_project_target_id']['table'] = 'field_data_field_project';
	$handler->display->display_options['relationships']['field_project_target_id']['field'] = 'field_project_target_id';
	/* Relationship: Entity Reference: Referenced Entity */
	$handler->display->display_options['relationships']['field_coworker_target_id']['id'] = 'field_coworker_target_id';
	$handler->display->display_options['relationships']['field_coworker_target_id']['table'] = 'field_data_field_coworker';
	$handler->display->display_options['relationships']['field_coworker_target_id']['field'] = 'field_coworker_target_id';
	/* Relationship: Entity Reference: Referenced Entity */
	$handler->display->display_options['relationships']['field_shareholder_project_target_id']['id'] = 'field_shareholder_project_target_id';
	$handler->display->display_options['relationships']['field_shareholder_project_target_id']['table'] = 'field_data_field_shareholder_project';
	$handler->display->display_options['relationships']['field_shareholder_project_target_id']['field'] = 'field_shareholder_project_target_id';
	/* Field: Content: Nid */
	$handler->display->display_options['fields']['nid_1']['id'] = 'nid_1';
	$handler->display->display_options['fields']['nid_1']['table'] = 'node';
	$handler->display->display_options['fields']['nid_1']['field'] = 'nid';
	$handler->display->display_options['fields']['nid_1']['label'] = '';
	$handler->display->display_options['fields']['nid_1']['exclude'] = TRUE;
	$handler->display->display_options['fields']['nid_1']['element_label_colon'] = FALSE;
	/* Field: Content: Nid */
	$handler->display->display_options['fields']['nid']['id'] = 'nid';
	$handler->display->display_options['fields']['nid']['table'] = 'node';
	$handler->display->display_options['fields']['nid']['field'] = 'nid';
	$handler->display->display_options['fields']['nid']['relationship'] = 'field_project_target_id';
	$handler->display->display_options['fields']['nid']['label'] = 'Project Nid';
	$handler->display->display_options['fields']['nid']['exclude'] = TRUE;
	/* Field: Content: Author uid */
	$handler->display->display_options['fields']['uid']['id'] = 'uid';
	$handler->display->display_options['fields']['uid']['table'] = 'node';
	$handler->display->display_options['fields']['uid']['field'] = 'uid';
	$handler->display->display_options['fields']['uid']['relationship'] = 'field_project_target_id';
	$handler->display->display_options['fields']['uid']['label'] = '';
	$handler->display->display_options['fields']['uid']['exclude'] = TRUE;
	$handler->display->display_options['fields']['uid']['element_label_colon'] = FALSE;
	$handler->display->display_options['fields']['uid']['link_to_user'] = FALSE;
	/* Field: Content: Project ID */
	$handler->display->display_options['fields']['field_project_id']['id'] = 'field_project_id';
	$handler->display->display_options['fields']['field_project_id']['table'] = 'field_data_field_project_id';
	$handler->display->display_options['fields']['field_project_id']['field'] = 'field_project_id';
	$handler->display->display_options['fields']['field_project_id']['relationship'] = 'field_shareholder_project_target_id';
	$handler->display->display_options['fields']['field_project_id']['label'] = '';
	$handler->display->display_options['fields']['field_project_id']['exclude'] = TRUE;
	$handler->display->display_options['fields']['field_project_id']['element_label_colon'] = FALSE;
	/* Field: Content: Title */
	$handler->display->display_options['fields']['title']['id'] = 'title';
	$handler->display->display_options['fields']['title']['table'] = 'node';
	$handler->display->display_options['fields']['title']['field'] = 'title';
	$handler->display->display_options['fields']['title']['relationship'] = 'field_shareholder_project_target_id';
	$handler->display->display_options['fields']['title']['exclude'] = TRUE;
	/* Field: User: Name */
	$handler->display->display_options['fields']['name']['id'] = 'name';
	$handler->display->display_options['fields']['name']['table'] = 'users';
	$handler->display->display_options['fields']['name']['field'] = 'name';
	$handler->display->display_options['fields']['name']['relationship'] = 'field_coworker_target_id';
	$handler->display->display_options['fields']['name']['exclude'] = TRUE;
	/* Field: User: Full name */
	$handler->display->display_options['fields']['field_full_name']['id'] = 'field_full_name';
	$handler->display->display_options['fields']['field_full_name']['table'] = 'field_data_field_full_name';
	$handler->display->display_options['fields']['field_full_name']['field'] = 'field_full_name';
	$handler->display->display_options['fields']['field_full_name']['relationship'] = 'field_coworker_target_id';
	$handler->display->display_options['fields']['field_full_name']['label'] = 'Worker';
	$handler->display->display_options['fields']['field_full_name']['exclude'] = TRUE;
	$handler->display->display_options['fields']['field_full_name']['alter']['text'] = '[field_full_name] ([name])';
	/* Field: Global: PHP */
	$handler->display->display_options['fields']['php_1']['id'] = 'php_1';
	$handler->display->display_options['fields']['php_1']['table'] = 'views';
	$handler->display->display_options['fields']['php_1']['field'] = 'php';
	$handler->display->display_options['fields']['php_1']['label'] = 'Shareholder';
	$handler->display->display_options['fields']['php_1']['use_php_setup'] = 0;
	$handler->display->display_options['fields']['php_1']['php_output'] = '<?php 
	if(is_numeric($data->node_field_data_field_shareholder_project_nid)) {
		echo \'<a href="/node/\' . $data->node_field_data_field_shareholder_project_nid . \'">\';
		echo t(\'Project\') . \' \' . $data->node_field_data_field_shareholder_project_nid . \' (\' . $data->node_field_data_field_shareholder_project_title . \')\';
		echo \'</a>\';
	}
	else {
		echo \'<a href="/user/\' . $data->users_field_data_field_coworker_uid . \'">\';
		echo $data->users_field_data_field_coworker_name;
		echo \'</a>\';
		}
	?>';
	$handler->display->display_options['fields']['php_1']['use_php_click_sortable'] = '0';
	$handler->display->display_options['fields']['php_1']['php_click_sortable'] = '';
	/* Field: Content: Merit Allocation */
	$handler->display->display_options['fields']['field_merit_allocation']['id'] = 'field_merit_allocation';
	$handler->display->display_options['fields']['field_merit_allocation']['table'] = 'field_data_field_merit_allocation';
	$handler->display->display_options['fields']['field_merit_allocation']['field'] = 'field_merit_allocation';
	$handler->display->display_options['fields']['field_merit_allocation']['label'] = 'Share';
	$handler->display->display_options['fields']['field_merit_allocation']['exclude'] = TRUE;
	$handler->display->display_options['fields']['field_merit_allocation']['settings'] = array(
		'thousand_separator' => '',
		'decimal_separator' => '.',
		'scale' => '2',
		'prefix_suffix' => 1,
	);
	/* Field: Global: PHP */
	$handler->display->display_options['fields']['php_4']['id'] = 'php_4';
	$handler->display->display_options['fields']['php_4']['table'] = 'views';
	$handler->display->display_options['fields']['php_4']['field'] = 'php';
	$handler->display->display_options['fields']['php_4']['label'] = 'Share';
	$handler->display->display_options['fields']['php_4']['use_php_setup'] = 0;
	$handler->display->display_options['fields']['php_4']['php_output'] = '<?php
	$alloc = isset($data->field_field_merit_allocation[0][\'raw\'][\'value\']) ? $data->field_field_merit_allocation[0][\'raw\'][\'value\'] : 0;
	if($alloc > 0)
	print round($alloc,1) . \' %\';
	else
	print t(\'work time share\');
	?>';
	$handler->display->display_options['fields']['php_4']['use_php_click_sortable'] = '0';
	$handler->display->display_options['fields']['php_4']['php_click_sortable'] = '';
	/* Field: Content: Total Hours Worked */
	$handler->display->display_options['fields']['field_total_hours_worked']['id'] = 'field_total_hours_worked';
	$handler->display->display_options['fields']['field_total_hours_worked']['table'] = 'field_data_field_total_hours_worked';
	$handler->display->display_options['fields']['field_total_hours_worked']['field'] = 'field_total_hours_worked';
	$handler->display->display_options['fields']['field_total_hours_worked']['label'] = 'Time';
	$handler->display->display_options['fields']['field_total_hours_worked']['element_label_colon'] = FALSE;
	$handler->display->display_options['fields']['field_total_hours_worked']['type'] = 'custom_formatters_labor_time_formatter';
	$handler->display->display_options['fields']['field_total_hours_worked']['settings'] = array(
		'thousand_separator' => '',
		'decimal_separator' => '.',
		'scale' => '2',
		'prefix_suffix' => 0,
	);
	/* Field: User: Uid */
	$handler->display->display_options['fields']['uid_1']['id'] = 'uid_1';
	$handler->display->display_options['fields']['uid_1']['table'] = 'users';
	$handler->display->display_options['fields']['uid_1']['field'] = 'uid';
	$handler->display->display_options['fields']['uid_1']['relationship'] = 'field_coworker_target_id';
	$handler->display->display_options['fields']['uid_1']['label'] = '';
	$handler->display->display_options['fields']['uid_1']['exclude'] = TRUE;
	$handler->display->display_options['fields']['uid_1']['element_label_colon'] = FALSE;
	$handler->display->display_options['fields']['uid_1']['link_to_user'] = FALSE;
	/* Field: Content: Project Participation */
	$handler->display->display_options['fields']['field_participation']['id'] = 'field_participation';
	$handler->display->display_options['fields']['field_participation']['table'] = 'field_data_field_participation';
	$handler->display->display_options['fields']['field_participation']['field'] = 'field_participation';
	$handler->display->display_options['fields']['field_participation']['relationship'] = 'field_project_target_id';
	$handler->display->display_options['fields']['field_participation']['label'] = '';
	$handler->display->display_options['fields']['field_participation']['exclude'] = TRUE;
	$handler->display->display_options['fields']['field_participation']['element_label_colon'] = FALSE;
	$handler->display->display_options['fields']['field_participation']['type'] = 'list_key';
	/* Field: Content: Merits Earned */
	$handler->display->display_options['fields']['field_merits_earned']['id'] = 'field_merits_earned';
	$handler->display->display_options['fields']['field_merits_earned']['table'] = 'field_data_field_merits_earned';
	$handler->display->display_options['fields']['field_merits_earned']['field'] = 'field_merits_earned';
	$handler->display->display_options['fields']['field_merits_earned']['label'] = 'Merits';
	$handler->display->display_options['fields']['field_merits_earned']['alter']['text'] = '[field_merits_earned][field_merit_reward]';
	$handler->display->display_options['fields']['field_merits_earned']['settings'] = array(
		'thousand_separator' => ' ',
		'decimal_separator' => '.',
		'scale' => '2',
		'prefix_suffix' => 0,
	);
	/* Field: Global: PHP */
	$handler->display->display_options['fields']['php']['id'] = 'php';
	$handler->display->display_options['fields']['php']['table'] = 'views';
	$handler->display->display_options['fields']['php']['field'] = 'php';
	$handler->display->display_options['fields']['php']['label'] = 'Action';
	$handler->display->display_options['fields']['php']['use_php_setup'] = 0;
	$handler->display->display_options['fields']['php']['php_output'] = '<?php 
	//if not set yet, set values for product variables
	$prid = $data->node_field_data_field_project_nid;
	$uid = $data->users_field_data_field_coworker_uid;
	$pr_uid = $data->node_field_data_field_project_uid;

	if ($static == \'\') {
		$wrapper = entity_metadata_wrapper(\'node\', $prid);
		$static = $wrapper->field_status->value();
	}

	global $user;

	//open projects
	if ($static < 2) {

		//private product
		if ($data->field_field_participation[0][\'raw\'][\'value\'] == 0) {
			//current user is the project owner
			if($user->uid == $pr_uid) {
				print \'<a href="/node/\' . $data->nid . \'/edit?destination=\'. $prid . \'"><img src="/sites/default/files/pictures/edit.svg" width="16" title="\' . t(\'edit\') . \'" /></a>&nbsp;&nbsp;\';
			}
			else {
				print \'<img src="/sites/default/files/pictures/edit-off.svg" width="16" title="\' . t(\'edit\') . \'" />&nbsp;&nbsp;\';
			}
			//only the project owner can delete allocation, if it is not the one for her(him)self
			if($user->uid == $pr_uid && $user->uid != $uid) {
				print \'<a href="/node/\' . $data->nid . \'/delete/\' . $prid . \'"><img src="/sites/default/files/pictures/delete.svg" width="16" title="\' . t(\'delete\') . \'" /></a>\';
			}
			else {
				print \'<img src="/sites/default/files/pictures/delete-off.svg" width="16" title="\' . t(\'delete\') . \'" />\';
			}
		}
		// open project
		else {
				print \'<img src="/sites/default/files/pictures/edit-off.svg" width="16" title="\' . t(\'edit\') . \'" />&nbsp;&nbsp;\';

			if(($user->uid == $pr_uid && $user->uid != $uid) || ($user->uid == $uid && $user->uid != $pr_uid)) {
				print \'<a href="/node/\' . $data->nid . \'/delete/\' . $prid . \'"><img src="/sites/default/files/pictures/delete.svg" width="16" title="\' . t(\'delete\') . \'" /></a>\';
				}
			else {
				print \'<img src="/sites/default/files/pictures/delete-off.svg" width="16" title="\' . t(\'delete\') . \'" />\';
				}
		}
	}
	//disable everything for closed projects
	else {
		print \'<img src="/sites/default/files/pictures/edit-off.svg" width="16" title="\' . t(\'edit\') . \'" />&nbsp;&nbsp;\';
		print \'<img src="/sites/default/files/pictures/delete-off.svg" width="16" title="\' . t(\'delete\') . \'" />\';
	}
	print \'<span style="display:none;">\' . $data->nid . \'</span>\';
	?>';
	$handler->display->display_options['fields']['php']['use_php_click_sortable'] = '0';
	$handler->display->display_options['fields']['php']['php_click_sortable'] = '';
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
		'project_merit_allocation' => 'project_merit_allocation',
		'co_worker_merit_allocation' => 'co_worker_merit_allocation',
	);
	$translatables['project_coworkers'] = array(
		t('Master'),
		t('Product Makers'),
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
		t('add worker button'),
		t('Content entity referenced from field_project'),
		t('User entity referenced from field_coworker'),
		t('Content entity referenced from field_shareholder_project'),
		t('Project Nid'),
		t('Title'),
		t('Name'),
		t('Worker'),
		t('[field_full_name] ([name])'),
		t('Shareholder'),
		t('Share'),
		t('Time'),
		t('Merits'),
		t('[field_merits_earned][field_merit_reward]'),
		t('Action'),
		t('All'),
	);
  $export['project_coworkers'] = $view;

  $view = new view();
  $view->name = 'project_user_time_shareholder';
  $view->description = '';
  $view->tag = 'default';
  $view->base_table = 'node';
  $view->human_name = 'project user time shareholder';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'some';
  $handler->display->display_options['pager']['options']['items_per_page'] = '1';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['style_plugin'] = 'default';
  $handler->display->display_options['row_plugin'] = 'fields';
  /* Field: Content: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'node';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  $handler->display->display_options['fields']['title']['label'] = '';
  $handler->display->display_options['fields']['title']['alter']['word_boundary'] = FALSE;
  $handler->display->display_options['fields']['title']['alter']['ellipsis'] = FALSE;
  /* Field: Bulk operations: Content */
  $handler->display->display_options['fields']['views_bulk_operations']['id'] = 'views_bulk_operations';
  $handler->display->display_options['fields']['views_bulk_operations']['table'] = 'views_entity_node';
  $handler->display->display_options['fields']['views_bulk_operations']['field'] = 'views_bulk_operations';
  $handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['display_type'] = '0';
  $handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['enable_select_all_pages'] = 1;
  $handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['row_clickable'] = 1;
  $handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['force_single'] = 0;
  $handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['entity_load_capacity'] = '10';
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
  $handler->display->display_options['arguments']['field_project_target_id']['default_argument_type'] = 'fixed';
  $handler->display->display_options['arguments']['field_project_target_id']['default_argument_options']['argument'] = '0';
  $handler->display->display_options['arguments']['field_project_target_id']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['field_project_target_id']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['field_project_target_id']['summary_options']['items_per_page'] = '25';
  /* Contextual filter: Content: Worker Name (field_coworker) */
  $handler->display->display_options['arguments']['field_coworker_target_id']['id'] = 'field_coworker_target_id';
  $handler->display->display_options['arguments']['field_coworker_target_id']['table'] = 'field_data_field_coworker';
  $handler->display->display_options['arguments']['field_coworker_target_id']['field'] = 'field_coworker_target_id';
  $handler->display->display_options['arguments']['field_coworker_target_id']['default_action'] = 'default';
  $handler->display->display_options['arguments']['field_coworker_target_id']['default_argument_type'] = 'fixed';
  $handler->display->display_options['arguments']['field_coworker_target_id']['default_argument_options']['argument'] = '0';
  $handler->display->display_options['arguments']['field_coworker_target_id']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['field_coworker_target_id']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['field_coworker_target_id']['summary_options']['items_per_page'] = '25';
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
    'timeshare_merit_allocation' => 'timeshare_merit_allocation',
  );

  /* Display: Block */
  $handler = $view->new_display('block', 'Block', 'block_1');
  $handler->display->display_options['defaults']['group_by'] = FALSE;
  $handler->display->display_options['defaults']['pager'] = FALSE;
  $handler->display->display_options['pager']['type'] = 'none';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['defaults']['fields'] = FALSE;
  /* Field: Content: Merit Allocation */
  $handler->display->display_options['fields']['field_merit_allocation']['id'] = 'field_merit_allocation';
  $handler->display->display_options['fields']['field_merit_allocation']['table'] = 'field_data_field_merit_allocation';
  $handler->display->display_options['fields']['field_merit_allocation']['field'] = 'field_merit_allocation';
  $handler->display->display_options['fields']['field_merit_allocation']['group_type'] = 'sum';
  $handler->display->display_options['fields']['field_merit_allocation']['label'] = '';
  $handler->display->display_options['fields']['field_merit_allocation']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_merit_allocation']['type'] = 'number_unformatted';
  $handler->display->display_options['fields']['field_merit_allocation']['settings'] = array(
    'thousand_separator' => '',
    'decimal_separator' => '.',
    'scale' => '2',
    'prefix_suffix' => 1,
  );
  /* Field: Bulk operations: Content */
  $handler->display->display_options['fields']['views_bulk_operations']['id'] = 'views_bulk_operations';
  $handler->display->display_options['fields']['views_bulk_operations']['table'] = 'views_entity_node';
  $handler->display->display_options['fields']['views_bulk_operations']['field'] = 'views_bulk_operations';
  $handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['display_type'] = '0';
  $handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['enable_select_all_pages'] = 1;
  $handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['row_clickable'] = 1;
  $handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['force_single'] = 0;
  $handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['entity_load_capacity'] = '10';
  $handler->display->display_options['defaults']['arguments'] = FALSE;
  /* Contextual filter: Content: Project (field_project) */
  $handler->display->display_options['arguments']['field_project_target_id']['id'] = 'field_project_target_id';
  $handler->display->display_options['arguments']['field_project_target_id']['table'] = 'field_data_field_project';
  $handler->display->display_options['arguments']['field_project_target_id']['field'] = 'field_project_target_id';
  $handler->display->display_options['arguments']['field_project_target_id']['default_action'] = 'not found';
  $handler->display->display_options['arguments']['field_project_target_id']['default_argument_type'] = 'fixed';
  $handler->display->display_options['arguments']['field_project_target_id']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['field_project_target_id']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['field_project_target_id']['summary_options']['items_per_page'] = '25';
  $translatables['project_user_time_shareholder'] = array(
    t('Master'),
    t('more'),
    t('Apply'),
    t('Reset'),
    t('Sort by'),
    t('Asc'),
    t('Desc'),
    t('Content'),
    t('- Choose an operation -'),
    t('All'),
    t('Block'),
  );
  $export['project_user_time_shareholder'] = $view;

	$view = new view();
	$view->name = 'project_merit_rewards_chart';
	$view->description = '';
	$view->tag = 'default';
	$view->base_table = 'node';
	$view->human_name = 'Project merit rewards chart';
	$view->core = 7;
	$view->api_version = '3.0';
	$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

	/* Display: Master */
	$handler = $view->new_display('default', 'Master', 'default');
	$handler->display->display_options['title'] = 'Project statistics';
	$handler->display->display_options['use_more_always'] = FALSE;
	$handler->display->display_options['group_by'] = TRUE;
	$handler->display->display_options['access']['type'] = 'perm';
	$handler->display->display_options['cache']['type'] = 'none';
	$handler->display->display_options['query']['type'] = 'views_query';
	$handler->display->display_options['exposed_form']['type'] = 'basic';
	$handler->display->display_options['pager']['type'] = 'none';
	$handler->display->display_options['pager']['options']['offset'] = '0';
	$handler->display->display_options['style_plugin'] = 'chart';
	$handler->display->display_options['style_options']['type'] = 'line';
	$handler->display->display_options['style_options']['library'] = '';
	$handler->display->display_options['style_options']['label_field'] = 'field_purchase_date';
	$handler->display->display_options['style_options']['data_fields'] = array(
		'field_amount_paid' => 'field_amount_paid',
		'field_purchase_date' => 0,
	);
	$handler->display->display_options['style_options']['field_colors'] = array(
		'field_purchase_date' => '#2f7ed8',
		'field_amount_paid' => '#0d233a',
	);
	$handler->display->display_options['style_options']['data_labels'] = 'TRUE';
	$handler->display->display_options['style_options']['tooltips'] = 'TRUE';
	$handler->display->display_options['style_options']['width'] = '';
	$handler->display->display_options['style_options']['height'] = '';
	$handler->display->display_options['style_options']['xaxis_title'] = 'date';
	$handler->display->display_options['style_options']['xaxis_labels_rotation'] = '0';
	$handler->display->display_options['style_options']['yaxis_title'] = 'rewards';
	$handler->display->display_options['style_options']['yaxis_suffix'] = 'ME';
	$handler->display->display_options['style_options']['yaxis_labels_rotation'] = '0';
	/* Header: Global: Text area */
	$handler->display->display_options['header']['area']['id'] = 'area';
	$handler->display->display_options['header']['area']['table'] = 'views';
	$handler->display->display_options['header']['area']['field'] = 'area';
	$handler->display->display_options['header']['area']['content'] = 'Project <strong><?php 

	$nid = \'!1\';
	$node = node_load($nid);

	$path = drupal_get_path_alias(\'node/\' . $nid);

	echo \'<a href="/\';
	echo $path;
	echo \'">\';
	echo $node->title;
	echo \'</a>\';
	?></strong>';
	$handler->display->display_options['header']['area']['format'] = 'php_code';
	$handler->display->display_options['header']['area']['tokenize'] = TRUE;
	/* Field: date */
	$handler->display->display_options['fields']['raw_field']['id'] = 'raw_field';
	$handler->display->display_options['fields']['raw_field']['table'] = 'views_raw_sql';
	$handler->display->display_options['fields']['raw_field']['field'] = 'raw_field';
	$handler->display->display_options['fields']['raw_field']['ui_name'] = 'date';
	$handler->display->display_options['fields']['raw_field']['label'] = 'date';
	$handler->display->display_options['fields']['raw_field']['element_default_classes'] = FALSE;
	$handler->display->display_options['fields']['raw_field']['raw_sql'] = 'DATE_FORMAT(FROM_UNIXTIME(field_data_field_purchase_date.field_purchase_date_value), \'%b %Y\')';
	/* Field: COUNT(Content: Purchase Date) */
	$handler->display->display_options['fields']['field_purchase_date']['id'] = 'field_purchase_date';
	$handler->display->display_options['fields']['field_purchase_date']['table'] = 'field_data_field_purchase_date';
	$handler->display->display_options['fields']['field_purchase_date']['field'] = 'field_purchase_date';
	$handler->display->display_options['fields']['field_purchase_date']['group_type'] = 'count';
	$handler->display->display_options['fields']['field_purchase_date']['label'] = '';
	$handler->display->display_options['fields']['field_purchase_date']['exclude'] = TRUE;
	$handler->display->display_options['fields']['field_purchase_date']['alter']['text'] = '1';
	$handler->display->display_options['fields']['field_purchase_date']['element_label_colon'] = FALSE;
	$handler->display->display_options['fields']['field_purchase_date']['separator'] = '';
	/* Field: SUM(Content: Amount Paid) */
	$handler->display->display_options['fields']['field_amount_paid']['id'] = 'field_amount_paid';
	$handler->display->display_options['fields']['field_amount_paid']['table'] = 'field_data_field_amount_paid';
	$handler->display->display_options['fields']['field_amount_paid']['field'] = 'field_amount_paid';
	$handler->display->display_options['fields']['field_amount_paid']['group_type'] = 'sum';
	$handler->display->display_options['fields']['field_amount_paid']['label'] = 'Amount ';
	$handler->display->display_options['fields']['field_amount_paid']['settings'] = array(
		'thousand_separator' => '',
		'decimal_separator' => '.',
		'scale' => '2',
		'prefix_suffix' => 0,
	);
	/* Sort criterion: Raw SQL: Raw sort */
	$handler->display->display_options['sorts']['raw_sort']['id'] = 'raw_sort';
	$handler->display->display_options['sorts']['raw_sort']['table'] = 'views_raw_sql';
	$handler->display->display_options['sorts']['raw_sort']['field'] = 'raw_sort';
	$handler->display->display_options['sorts']['raw_sort']['raw_sql'] = 'DATE_FORMAT(FROM_UNIXTIME(field_data_field_purchase_date.field_purchase_date_value), \'%y%c\')';
	/* Contextual filter: Content: Project (field_receipt_project) */
	$handler->display->display_options['arguments']['field_receipt_project_target_id']['id'] = 'field_receipt_project_target_id';
	$handler->display->display_options['arguments']['field_receipt_project_target_id']['table'] = 'field_data_field_receipt_project';
	$handler->display->display_options['arguments']['field_receipt_project_target_id']['field'] = 'field_receipt_project_target_id';
	$handler->display->display_options['arguments']['field_receipt_project_target_id']['default_action'] = 'default';
	$handler->display->display_options['arguments']['field_receipt_project_target_id']['default_argument_type'] = 'node';
	$handler->display->display_options['arguments']['field_receipt_project_target_id']['summary']['number_of_records'] = '0';
	$handler->display->display_options['arguments']['field_receipt_project_target_id']['summary']['format'] = 'default_summary';
	$handler->display->display_options['arguments']['field_receipt_project_target_id']['summary_options']['items_per_page'] = '25';
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
		'purchase_receipt' => 'purchase_receipt',
	);
	$handler->display->display_options['filters']['type']['group'] = 1;
	/* Filter criterion: Content: Post date */
	$handler->display->display_options['filters']['created']['id'] = 'created';
	$handler->display->display_options['filters']['created']['table'] = 'node';
	$handler->display->display_options['filters']['created']['field'] = 'created';
	$handler->display->display_options['filters']['created']['operator'] = '>';
	$handler->display->display_options['filters']['created']['value']['value'] = '- 1 year';
	$handler->display->display_options['filters']['created']['value']['type'] = 'offset';

	/* Display: Page */
	$handler = $view->new_display('page', 'Page', 'page');
	$handler->display->display_options['defaults']['style_plugin'] = FALSE;
	$handler->display->display_options['style_plugin'] = 'chart';
	$handler->display->display_options['style_options']['type'] = 'column';
	$handler->display->display_options['style_options']['library'] = '';
	$handler->display->display_options['style_options']['label_field'] = 'raw_field';
	$handler->display->display_options['style_options']['data_fields'] = array(
		'field_amount_paid' => 'field_amount_paid',
		'raw_field' => 0,
		'field_purchase_date' => 0,
	);
	$handler->display->display_options['style_options']['field_colors'] = array(
		'raw_field' => '#2f7ed8',
		'field_purchase_date' => '#d2d6d8',
		'field_amount_paid' => '#03ec20',
	);
	$handler->display->display_options['style_options']['data_labels'] = 'TRUE';
	$handler->display->display_options['style_options']['tooltips'] = 'TRUE';
	$handler->display->display_options['style_options']['width'] = '';
	$handler->display->display_options['style_options']['height'] = '';
	$handler->display->display_options['style_options']['xaxis_title'] = 'date';
	$handler->display->display_options['style_options']['xaxis_labels_rotation'] = '0';
	$handler->display->display_options['style_options']['yaxis_title'] = 'rewards';
	$handler->display->display_options['style_options']['yaxis_suffix'] = 'ME';
	$handler->display->display_options['style_options']['yaxis_labels_rotation'] = '0';
	$handler->display->display_options['defaults']['style_options'] = FALSE;
	$handler->display->display_options['defaults']['row_plugin'] = FALSE;
	$handler->display->display_options['defaults']['row_options'] = FALSE;
	$handler->display->display_options['path'] = 'projrew-chart';

	/* Display: Block */
	$handler = $view->new_display('block', 'Block', 'block_1');
	$handler->display->display_options['defaults']['style_plugin'] = FALSE;
	$handler->display->display_options['style_plugin'] = 'table';
	$handler->display->display_options['style_options']['columns'] = array(
		'raw_field' => 'raw_field',
		'field_purchase_date' => 'field_purchase_date',
		'field_amount_paid' => 'field_amount_paid',
	);
	$handler->display->display_options['style_options']['class'] = '';
	$handler->display->display_options['style_options']['default'] = '-1';
	$handler->display->display_options['style_options']['info'] = array(
		'raw_field' => array(
			'align' => '',
			'separator' => '',
			'empty_column' => 0,
		),
		'field_purchase_date' => array(
			'sortable' => 0,
			'default_sort_order' => 'asc',
			'align' => '',
			'separator' => '',
			'empty_column' => 0,
		),
		'field_amount_paid' => array(
			'sortable' => 0,
			'default_sort_order' => 'asc',
			'align' => '',
			'separator' => '',
			'empty_column' => 0,
		),
	);
	$handler->display->display_options['defaults']['style_options'] = FALSE;
	$handler->display->display_options['defaults']['row_plugin'] = FALSE;
	$handler->display->display_options['defaults']['row_options'] = FALSE;

	/* Display: Chart add-on */
	$handler = $view->new_display('chart', 'Chart add-on', 'chart_1');
	$handler->display->display_options['pager']['type'] = 'some';
	$handler->display->display_options['defaults']['style_plugin'] = FALSE;
	$handler->display->display_options['style_plugin'] = 'chart_extension';
	$handler->display->display_options['style_options']['type'] = 'column';
	$handler->display->display_options['style_options']['library'] = '';
	$handler->display->display_options['style_options']['label_field'] = 'raw_field';
	$handler->display->display_options['style_options']['data_fields'] = array(
		'field_hours_worked' => 'field_hours_worked',
		'raw_field' => 0,
		'field_date' => 0,
	);
	$handler->display->display_options['style_options']['field_colors'] = array(
		'raw_field' => '#2f7ed8',
		'field_date' => '#b8babc',
		'field_hours_worked' => '#0253df',
	);
	$handler->display->display_options['style_options']['width'] = '';
	$handler->display->display_options['style_options']['height'] = '';
	$handler->display->display_options['style_options']['yaxis_title'] = 'labor time';
	$handler->display->display_options['style_options']['yaxis_suffix'] = 'hours';
	$handler->display->display_options['style_options']['yaxis_labels_rotation'] = '0';
	$handler->display->display_options['defaults']['style_options'] = FALSE;
	$handler->display->display_options['defaults']['row_plugin'] = FALSE;
	$handler->display->display_options['defaults']['row_options'] = FALSE;
	$handler->display->display_options['defaults']['fields'] = FALSE;
	/* Field: Raw SQL: Raw field */
	$handler->display->display_options['fields']['raw_field']['id'] = 'raw_field';
	$handler->display->display_options['fields']['raw_field']['table'] = 'views_raw_sql';
	$handler->display->display_options['fields']['raw_field']['field'] = 'raw_field';
	$handler->display->display_options['fields']['raw_field']['raw_sql'] = 'DATE_FORMAT(FROM_UNIXTIME(field_data_field_date.field_date_value), \'%b %Y\')';
	/* Field: COUNT(Content: Date) */
	$handler->display->display_options['fields']['field_date']['id'] = 'field_date';
	$handler->display->display_options['fields']['field_date']['table'] = 'field_data_field_date';
	$handler->display->display_options['fields']['field_date']['field'] = 'field_date';
	$handler->display->display_options['fields']['field_date']['group_type'] = 'count';
	$handler->display->display_options['fields']['field_date']['separator'] = ', ';
	/* Field: SUM(Content: Time worked) */
	$handler->display->display_options['fields']['field_hours_worked']['id'] = 'field_hours_worked';
	$handler->display->display_options['fields']['field_hours_worked']['table'] = 'field_data_field_hours_worked';
	$handler->display->display_options['fields']['field_hours_worked']['field'] = 'field_hours_worked';
	$handler->display->display_options['fields']['field_hours_worked']['group_type'] = 'sum';
	$handler->display->display_options['fields']['field_hours_worked']['label'] = 'Time';
	$handler->display->display_options['fields']['field_hours_worked']['type'] = 'number_unformatted';
	$handler->display->display_options['fields']['field_hours_worked']['settings'] = array(
		'thousand_separator' => '',
		'decimal_separator' => '.',
		'scale' => '2',
		'prefix_suffix' => 1,
	);
	$handler->display->display_options['defaults']['sorts'] = FALSE;
	/* Sort criterion: Raw SQL: Raw sort */
	$handler->display->display_options['sorts']['raw_sort']['id'] = 'raw_sort';
	$handler->display->display_options['sorts']['raw_sort']['table'] = 'views_raw_sql';
	$handler->display->display_options['sorts']['raw_sort']['field'] = 'raw_sort';
	$handler->display->display_options['sorts']['raw_sort']['raw_sql'] = 'DATE_FORMAT(FROM_UNIXTIME(field_data_field_date.field_date_value), \'%y%c\')';
	$handler->display->display_options['defaults']['arguments'] = FALSE;
	/* Contextual filter: Content: Project (field_project) */
	$handler->display->display_options['arguments']['field_project_target_id']['id'] = 'field_project_target_id';
	$handler->display->display_options['arguments']['field_project_target_id']['table'] = 'field_data_field_project';
	$handler->display->display_options['arguments']['field_project_target_id']['field'] = 'field_project_target_id';
	$handler->display->display_options['arguments']['field_project_target_id']['default_action'] = 'default';
	$handler->display->display_options['arguments']['field_project_target_id']['default_argument_type'] = 'node';
	$handler->display->display_options['arguments']['field_project_target_id']['summary']['number_of_records'] = '0';
	$handler->display->display_options['arguments']['field_project_target_id']['summary']['format'] = 'default_summary';
	$handler->display->display_options['arguments']['field_project_target_id']['summary_options']['items_per_page'] = '25';
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
	/* Filter criterion: Content: Verification status (field_verification_status) */
	$handler->display->display_options['filters']['field_verification_status_value']['id'] = 'field_verification_status_value';
	$handler->display->display_options['filters']['field_verification_status_value']['table'] = 'field_data_field_verification_status';
	$handler->display->display_options['filters']['field_verification_status_value']['field'] = 'field_verification_status_value';
	$handler->display->display_options['filters']['field_verification_status_value']['value'] = array(
		1 => '1',
	);
	$handler->display->display_options['filters']['field_verification_status_value']['group'] = 1;
	/* Filter criterion: Content: Date (field_date) */
	$handler->display->display_options['filters']['field_date_value']['id'] = 'field_date_value';
	$handler->display->display_options['filters']['field_date_value']['table'] = 'field_data_field_date';
	$handler->display->display_options['filters']['field_date_value']['field'] = 'field_date_value';
	$handler->display->display_options['filters']['field_date_value']['operator'] = '>';
	$handler->display->display_options['filters']['field_date_value']['default_date'] = 'now - 1 year';
	$handler->display->display_options['parent_display'] = 'page';
	$handler->display->display_options['inherit_yaxis'] = '0';
	$handler->display->display_options['inherit_exposed_filters'] = FALSE;
	$translatables['project_merit_rewards_chart'] = array(
		t('Master'),
		t('Project statistics'),
		t('more'),
		t('Apply'),
		t('Reset'),
		t('Sort by'),
		t('Asc'),
		t('Desc'),
		t('Project <strong><?php 

	$nid = \'!1\';
	$node = node_load($nid);

	$path = drupal_get_path_alias(\'node/\' . $nid);

	echo \'<a href="/\';
	echo $path;
	echo \'">\';
	echo $node->title;
	echo \'</a>\';
	?></strong>'),
		t('date'),
		t('1'),
		t('Amount '),
		t('All'),
		t('Page'),
		t('Block'),
		t('Chart add-on'),
		t('Raw field'),
		t('Date'),
		t('Time'),
	);

  $export['project_merit_rewards_chart'] = $view;

  $export += _rovas_purchase_shares_views_load_default_views();
  return $export;
}


/**
 * Load additional exported Views from views/*.inc.
 */
function _rovas_purchase_shares_views_load_default_views() {
  $export = array();
  $path = drupal_get_path('module', 'rovas_purchase_shares') . '/views';
  if (!is_dir($path)) {
    return $export;
  }
  foreach (glob($path . '/*.inc') as $file) {
    $view = NULL;
    $handler = NULL;
    include $file;
    if (isset($view) && is_object($view) && !empty($view->name)) {
      $export[$view->name] = $view;
    }
    unset($view, $handler, $translatables);
  }
  return $export;
}
