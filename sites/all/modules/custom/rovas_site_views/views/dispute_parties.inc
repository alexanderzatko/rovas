<?php

/**
 * @file
 * Default view: dispute_parties
 */

$view = new view();
$view->name = 'dispute_parties';
$view->description = '';
$view->tag = 'default';
$view->base_table = 'node';
$view->human_name = 'Dispute parties';
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
$handler->display->display_options['pager']['type'] = 'full';
$handler->display->display_options['style_plugin'] = 'tablesummarized';
$handler->display->display_options['style_options']['grouping'] = array(
  0 => array(
    'field' => 'php',
    'rendered' => 1,
    'rendered_strip' => 0,
  ),
);
$handler->display->display_options['style_options']['default_row_class'] = FALSE;
$handler->display->display_options['style_options']['columns'] = array(
  'field_dispute' => 'field_dispute',
  'nid' => 'nid',
  'uid' => 'uid',
  'name' => 'name',
  'field_full_name' => 'field_full_name',
  'points' => 'points',
  'field_date_rectified' => 'field_date_rectified',
  'php_2' => 'php_2',
  'field_position' => 'field_position',
  'php' => 'php',
  'php_1' => 'php_1',
);
$handler->display->display_options['style_options']['default'] = 'points';
$handler->display->display_options['style_options']['info'] = array(
  'field_dispute' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
    'summarize' => 'none',
  ),
  'nid' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
    'summarize' => 'none',
  ),
  'uid' => array(
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
    'summarize' => 'none',
  ),
  'name' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
    'summarize' => 'none',
  ),
  'field_full_name' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
    'summarize' => 'none',
  ),
  'points' => array(
    'sortable' => 1,
    'default_sort_order' => 'desc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
    'summarize' => 'none',
  ),
  'field_date_rectified' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
    'summarize' => 'none',
  ),
  'php_2' => array(
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
    'summarize' => 'currency',
  ),
  'field_position' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
    'summarize' => 'none',
  ),
  'php' => array(
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
    'summarize' => 'none',
  ),
  'php_1' => array(
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
    'summarize' => 'none',
  ),
);
$handler->display->display_options['style_options']['override'] = FALSE;
/* Footer: Global: PHP */
$handler->display->display_options['footer']['php']['id'] = 'php';
$handler->display->display_options['footer']['php']['table'] = 'views';
$handler->display->display_options['footer']['php']['field'] = 'php';
$handler->display->display_options['footer']['php']['php_output'] = '<?php
if (is_numeric($view->args[0])) {
	$dsid = $view->args[0];
	$obj = entity_metadata_wrapper(\'node\', $dsid);
	$dstatus = $obj->field_dispute_status->value();
}
else
	$dsid = 1; //made-up number


global $user;
$uid_found = \'no\';

if(isset($results)) {
	foreach ($results as $rec) {
		if ($rec->users_node_uid == $user->uid) {
			$uid_found = \'yes\';
			break;
			}
	}
}
//if user is not banned or in dispute and does not have a record yet, app should allow him/her to add him/herself.
if($uid_found == \'no\' && !(user_has_role(5) || user_has_role(6)) && $dstatus == 0) {
	print \'<a class="btn btn-default" style="color: white; background-color: green;" role="button" href="/node/add/dispute-party/\'.$dsid . \'">\' . t(\'Join Dispute\') . \'</a><br/><br/>\';
}
else {
	print \'<a class="btn btn-default disabled" style="color: white; background-color: gray;" role="button" href="#">\'. t(\'Join Dispute\') . \'</a>\';
	if ($dstatus != 0)
		print \' <span style="color: lightgray;">\' . t(\'The dispute is closed.\') . \'</span><br/><br/>\';
	else if ($uid_found == \'yes\')
		print \' <span style="color: lightgray;">\' . t(\'Your are already a party to the dispute.\') . \'</span><br/><br/>\';
	else
		print \' <span style="color: lightgray;">\' . t(\'Your current status does not allow you to participate.\') . \'</span><br/><br/>\';
}
?>';
/* Relationship: Content: Content author */
$handler->display->display_options['relationships']['uid']['id'] = 'uid';
$handler->display->display_options['relationships']['uid']['table'] = 'node';
$handler->display->display_options['relationships']['uid']['field'] = 'uid';
/* Relationship: Entity Reference: Referenced Entity */
$handler->display->display_options['relationships']['field_dispute_target_id']['id'] = 'field_dispute_target_id';
$handler->display->display_options['relationships']['field_dispute_target_id']['table'] = 'field_data_field_dispute';
$handler->display->display_options['relationships']['field_dispute_target_id']['field'] = 'field_dispute_target_id';
$handler->display->display_options['relationships']['field_dispute_target_id']['label'] = 'dispute';
/* Field: Content: Dispute */
$handler->display->display_options['fields']['field_dispute']['id'] = 'field_dispute';
$handler->display->display_options['fields']['field_dispute']['table'] = 'field_data_field_dispute';
$handler->display->display_options['fields']['field_dispute']['field'] = 'field_dispute';
$handler->display->display_options['fields']['field_dispute']['label'] = '';
$handler->display->display_options['fields']['field_dispute']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_dispute']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_dispute']['type'] = 'entityreference_entity_id';
$handler->display->display_options['fields']['field_dispute']['settings'] = array(
  'bypass_access' => 0,
  'link' => 0,
);
/* Field: Content: Nid */
$handler->display->display_options['fields']['nid']['id'] = 'nid';
$handler->display->display_options['fields']['nid']['table'] = 'node';
$handler->display->display_options['fields']['nid']['field'] = 'nid';
$handler->display->display_options['fields']['nid']['label'] = '';
$handler->display->display_options['fields']['nid']['exclude'] = TRUE;
$handler->display->display_options['fields']['nid']['element_label_colon'] = FALSE;
/* Field: Content: Author uid */
$handler->display->display_options['fields']['uid']['id'] = 'uid';
$handler->display->display_options['fields']['uid']['table'] = 'node';
$handler->display->display_options['fields']['uid']['field'] = 'uid';
$handler->display->display_options['fields']['uid']['label'] = '';
$handler->display->display_options['fields']['uid']['exclude'] = TRUE;
$handler->display->display_options['fields']['uid']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['uid']['link_to_user'] = FALSE;
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
$handler->display->display_options['fields']['field_full_name']['label'] = 'Party Name';
$handler->display->display_options['fields']['field_full_name']['alter']['alter_text'] = TRUE;
$handler->display->display_options['fields']['field_full_name']['alter']['text'] = '[name] ([field_full_name])';
/* Field: Userpoints total: Current total merits */
$handler->display->display_options['fields']['points']['id'] = 'points';
$handler->display->display_options['fields']['points']['table'] = 'userpoints_total';
$handler->display->display_options['fields']['points']['field'] = 'points';
$handler->display->display_options['fields']['points']['label'] = '';
$handler->display->display_options['fields']['points']['exclude'] = TRUE;
$handler->display->display_options['fields']['points']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['points']['separator'] = ' ';
/* Field: Content: End date */
$handler->display->display_options['fields']['field_date_rectified']['id'] = 'field_date_rectified';
$handler->display->display_options['fields']['field_date_rectified']['table'] = 'field_data_field_date_rectified';
$handler->display->display_options['fields']['field_date_rectified']['field'] = 'field_date_rectified';
$handler->display->display_options['fields']['field_date_rectified']['relationship'] = 'field_dispute_target_id';
$handler->display->display_options['fields']['field_date_rectified']['label'] = '';
$handler->display->display_options['fields']['field_date_rectified']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_date_rectified']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_date_rectified']['type'] = 'date_plain';
$handler->display->display_options['fields']['field_date_rectified']['settings'] = array(
  'format_type' => 'custom',
  'custom_date_format' => 'U',
  'fromto' => 'both',
  'multiple_number' => '',
  'multiple_from' => '',
  'multiple_to' => '',
  'show_remaining_days' => 0,
);
/* Field: Global: PHP */
$handler->display->display_options['fields']['php_2']['id'] = 'php_2';
$handler->display->display_options['fields']['php_2']['table'] = 'views';
$handler->display->display_options['fields']['php_2']['field'] = 'php';
$handler->display->display_options['fields']['php_2']['label'] = 'Merit score';
$handler->display->display_options['fields']['php_2']['alter']['text'] = '[php_2] (current: [points])';
$handler->display->display_options['fields']['php_2']['use_php_setup'] = 0;
$handler->display->display_options['fields']['php_2']['php_output'] = '<?php 
$rd = isset($data->field_field_date_rectified) ? $data->field_field_date_rectified : null;
$uid = $row->uid;
$user = user_load($uid);

$decpart = round($user->field_merit_decimal_part[LANGUAGE_NONE][0][\'value\'],2);

if(is_numeric($rd))
	$date = $rd;
else
	$date = time();

$sql="SELECT SUM(u.points) AS merits
FROM {userpoints_txn} u
WHERE u.uid = :uid
AND u.time_stamp < :ts";

$result = db_query($sql, array(\':uid\' => $uid, \':ts\' => $date));
$resarray = $result->fetchAll();

$wholemerits =  is_numeric($resarray[0]->merits) ? $resarray[0]->merits : 0;

print $decpart + $wholemerits;
?>
';
$handler->display->display_options['fields']['php_2']['use_php_click_sortable'] = '0';
$handler->display->display_options['fields']['php_2']['php_click_sortable'] = '';
/* Field: Content: Position */
$handler->display->display_options['fields']['field_position']['id'] = 'field_position';
$handler->display->display_options['fields']['field_position']['table'] = 'field_data_field_position';
$handler->display->display_options['fields']['field_position']['field'] = 'field_position';
$handler->display->display_options['fields']['field_position']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_position']['type'] = 'list_key';
/* Field: Global: PHP */
$handler->display->display_options['fields']['php']['id'] = 'php';
$handler->display->display_options['fields']['php']['table'] = 'views';
$handler->display->display_options['fields']['php']['field'] = 'php';
$handler->display->display_options['fields']['php']['label'] = '';
$handler->display->display_options['fields']['php']['exclude'] = TRUE;
$handler->display->display_options['fields']['php']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['php']['use_php_setup'] = 0;
$handler->display->display_options['fields']['php']['php_output'] = '<?php 
if($data->field_field_position[0][\'raw\'][\'value\'] == 1)
	print t(\'Supporters of the Complainant\');
else
	print t(\'Defenders of the Accused\');
?>';
$handler->display->display_options['fields']['php']['use_php_click_sortable'] = '0';
$handler->display->display_options['fields']['php']['php_click_sortable'] = '';
/* Field: Global: PHP */
$handler->display->display_options['fields']['php_1']['id'] = 'php_1';
$handler->display->display_options['fields']['php_1']['table'] = 'views';
$handler->display->display_options['fields']['php_1']['field'] = 'php';
$handler->display->display_options['fields']['php_1']['label'] = 'Action';
$handler->display->display_options['fields']['php_1']['use_php_setup'] = 0;
$handler->display->display_options['fields']['php_1']['php_output'] = '<?php 
//make sure the dispute node is loaded only once
if ($static == \'\') {

$dispute = $data->field_field_dispute[0][\'raw\'][\'target_id\'];
$dispute_w = entity_metadata_wrapper(\'node\', $dispute);

$static = [
\'accused_uid\' => $dispute_w->field_accused->uid->value(),
\'complainant_uid\' => $dispute_w->author->uid->value(),
\'dispute_status\' => $dispute_w->field_dispute_status->value()
];

}

global $user;

if($row->uid == $user->uid && $row->uid != $static[\'accused_uid\'] && $row->uid != $static[\'complainant_uid\'] && $static[\'dispute_status\'] == 0) {
	print \'<a href="/node/\' . $row->nid . \'/edit"><img src="/sites/default/files/pictures/edit.svg" width="16" title="\' . t(\'edit\') . \'" /></a>&nbsp;&nbsp;\';
	print \'<a href="/node/\' . $row->nid . \'/delete"><img src="/sites/default/files/pictures/delete.svg" width="16" title="\' . t(\'delete\') . \'" /></a>\';
}
else {
	print \'<img src="/sites/default/files/pictures/edit-off.svg" width="16" title="\' . t(\'edit\') . \'" />&nbsp;&nbsp;\';
	print \'<img src="/sites/default/files/pictures/delete-off.svg" width="16" title="\' . t(\'delete\') . \'" />\';
}
?>';
$handler->display->display_options['fields']['php_1']['use_php_click_sortable'] = '0';
$handler->display->display_options['fields']['php_1']['php_click_sortable'] = '';
/* Contextual filter: Content: Dispute (field_dispute) */
$handler->display->display_options['arguments']['field_dispute_target_id']['id'] = 'field_dispute_target_id';
$handler->display->display_options['arguments']['field_dispute_target_id']['table'] = 'field_data_field_dispute';
$handler->display->display_options['arguments']['field_dispute_target_id']['field'] = 'field_dispute_target_id';
$handler->display->display_options['arguments']['field_dispute_target_id']['default_action'] = 'empty';
$handler->display->display_options['arguments']['field_dispute_target_id']['default_argument_type'] = 'php';
$handler->display->display_options['arguments']['field_dispute_target_id']['default_argument_options']['code'] = 'return arg(1);';
$handler->display->display_options['arguments']['field_dispute_target_id']['summary']['number_of_records'] = '0';
$handler->display->display_options['arguments']['field_dispute_target_id']['summary']['format'] = 'default_summary';
$handler->display->display_options['arguments']['field_dispute_target_id']['summary_options']['items_per_page'] = '25';
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
  'dispute_party' => 'dispute_party',
);
$translatables['dispute_parties'] = array(
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
  t('« first'),
  t('‹ previous'),
  t('next ›'),
  t('last »'),
  t('author'),
  t('dispute'),
  t('Name'),
  t('Party Name'),
  t('[name] ([field_full_name])'),
  t('.'),
  t(' '),
  t('Merit score'),
  t('[php_2] (current: [points])'),
  t('Position'),
  t('Action'),
  t('All'),
);

