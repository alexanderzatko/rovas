<?php

/**
 * @file
 * Default view: project_merits_feed
 */

$view = new view();
$view->name = 'project_merits_feed';
$view->description = '';
$view->tag = 'default';
$view->base_table = 'node';
$view->human_name = 'project purchases';
$view->core = 7;
$view->api_version = '3.0';
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

/* Display: Master */
$handler = $view->new_display('default', 'Master', 'default');
$handler->display->display_options['use_more_always'] = FALSE;
$handler->display->display_options['group_by'] = TRUE;
$handler->display->display_options['access']['type'] = 'perm';
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
$handler->display->display_options['style_plugin'] = 'default';
$handler->display->display_options['row_plugin'] = 'fields';
/* Relationship: Entity Reference: Referenced Entity */
$handler->display->display_options['relationships']['field_receipt_project_target_id']['id'] = 'field_receipt_project_target_id';
$handler->display->display_options['relationships']['field_receipt_project_target_id']['table'] = 'field_data_field_receipt_project';
$handler->display->display_options['relationships']['field_receipt_project_target_id']['field'] = 'field_receipt_project_target_id';
$handler->display->display_options['relationships']['field_receipt_project_target_id']['label'] = 'project';
/* Field: Content: Nid */
$handler->display->display_options['fields']['nid_1']['id'] = 'nid_1';
$handler->display->display_options['fields']['nid_1']['table'] = 'node';
$handler->display->display_options['fields']['nid_1']['field'] = 'nid';
$handler->display->display_options['fields']['nid_1']['relationship'] = 'field_receipt_project_target_id';
$handler->display->display_options['fields']['nid_1']['label'] = 'project_id';
/* Field: COUNT(Content: Nid) */
$handler->display->display_options['fields']['nid']['id'] = 'nid';
$handler->display->display_options['fields']['nid']['table'] = 'node';
$handler->display->display_options['fields']['nid']['field'] = 'nid';
$handler->display->display_options['fields']['nid']['group_type'] = 'count';
$handler->display->display_options['fields']['nid']['label'] = 'purchases_count';
$handler->display->display_options['fields']['nid']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['nid']['separator'] = '';
/* Field: SUM(Content: Amount Paid) */
$handler->display->display_options['fields']['field_amount_paid']['id'] = 'field_amount_paid';
$handler->display->display_options['fields']['field_amount_paid']['table'] = 'field_data_field_amount_paid';
$handler->display->display_options['fields']['field_amount_paid']['field'] = 'field_amount_paid';
$handler->display->display_options['fields']['field_amount_paid']['group_type'] = 'sum';
$handler->display->display_options['fields']['field_amount_paid']['label'] = 'merits';
$handler->display->display_options['fields']['field_amount_paid']['settings'] = array(
  'thousand_separator' => '',
  'decimal_separator' => '.',
  'scale' => '2',
  'prefix_suffix' => 0,
);
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
/* Filter criterion: Content: System project (field_system_project) */
$handler->display->display_options['filters']['field_system_project_value']['id'] = 'field_system_project_value';
$handler->display->display_options['filters']['field_system_project_value']['table'] = 'field_data_field_system_project';
$handler->display->display_options['filters']['field_system_project_value']['field'] = 'field_system_project_value';
$handler->display->display_options['filters']['field_system_project_value']['relationship'] = 'field_receipt_project_target_id';
$handler->display->display_options['filters']['field_system_project_value']['value'] = array(
  0 => '0',
);

/* Display: Block */
$handler = $view->new_display('block', 'Block', 'block_1');
$handler->display->display_options['defaults']['use_more'] = FALSE;
$handler->display->display_options['use_more'] = TRUE;
$handler->display->display_options['defaults']['use_more_always'] = FALSE;
$handler->display->display_options['defaults']['use_more_always'] = FALSE;
$handler->display->display_options['use_more_always'] = FALSE;
$handler->display->display_options['defaults']['use_more_text'] = FALSE;
$handler->display->display_options['use_more_text'] = 'more ...';
$handler->display->display_options['defaults']['link_display'] = FALSE;
$handler->display->display_options['link_display'] = 'page_1';
$handler->display->display_options['defaults']['group_by'] = FALSE;
$handler->display->display_options['defaults']['pager'] = FALSE;
$handler->display->display_options['pager']['type'] = 'some';
$handler->display->display_options['pager']['options']['items_per_page'] = '20';
$handler->display->display_options['pager']['options']['offset'] = '0';
$handler->display->display_options['defaults']['style_plugin'] = FALSE;
$handler->display->display_options['style_plugin'] = 'table';
$handler->display->display_options['style_options']['columns'] = array(
  'created' => 'created',
  'field_buyer' => 'field_buyer',
  'field_amount_paid' => 'field_amount_paid',
);
$handler->display->display_options['style_options']['class'] = '';
$handler->display->display_options['style_options']['default'] = 'created';
$handler->display->display_options['style_options']['info'] = array(
  'created' => array(
    'sortable' => 1,
    'default_sort_order' => 'desc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_buyer' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_amount_paid' => array(
    'sortable' => 0,
    'default_sort_order' => 'desc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
);
$handler->display->display_options['style_options']['sticky'] = TRUE;
$handler->display->display_options['defaults']['style_options'] = FALSE;
$handler->display->display_options['defaults']['row_plugin'] = FALSE;
$handler->display->display_options['defaults']['row_options'] = FALSE;
$handler->display->display_options['defaults']['fields'] = FALSE;
/* Field: Content: Post date */
$handler->display->display_options['fields']['created']['id'] = 'created';
$handler->display->display_options['fields']['created']['table'] = 'node';
$handler->display->display_options['fields']['created']['field'] = 'created';
$handler->display->display_options['fields']['created']['label'] = 'Date';
$handler->display->display_options['fields']['created']['date_format'] = 'custom';
$handler->display->display_options['fields']['created']['custom_date_format'] = 'd M Y';
$handler->display->display_options['fields']['created']['second_date_format'] = 'privatemsg_current_day';
$handler->display->display_options['fields']['created']['timezone'] = 'Europe/Bratislava';
/* Field: Content: Buyer */
$handler->display->display_options['fields']['field_buyer']['id'] = 'field_buyer';
$handler->display->display_options['fields']['field_buyer']['table'] = 'field_data_field_buyer';
$handler->display->display_options['fields']['field_buyer']['field'] = 'field_buyer';
$handler->display->display_options['fields']['field_buyer']['label'] = '';
$handler->display->display_options['fields']['field_buyer']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_buyer']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_buyer']['settings'] = array(
  'bypass_access' => 0,
  'link' => 1,
);
/* Field: Global: PHP */
$handler->display->display_options['fields']['php']['id'] = 'php';
$handler->display->display_options['fields']['php']['table'] = 'views';
$handler->display->display_options['fields']['php']['field'] = 'php';
$handler->display->display_options['fields']['php']['label'] = 'Buyer';
$handler->display->display_options['fields']['php']['use_php_setup'] = 0;
$handler->display->display_options['fields']['php']['php_output'] = '<?php
if (isset($data->field_field_buyer[0][\'raw\'][\'target_id\']) && $data->field_field_buyer[0][\'raw\'][\'target_id\'] > 0) {
	echo \'<a href="/user/\' .  $data->field_field_buyer[0][\'raw\'][\'target_id\'] .\'">\';
}
if (isset($data->field_field_buyer[0][\'rendered\'][\'#label\'])) {
	echo htmlspecialchars($data->field_field_buyer[0][\'rendered\'][\'#label\'], ENT_QUOTES);
}
if (isset($data->field_field_buyer[0][\'raw\'][\'target_id\']) && $data->field_field_buyer[0][\'raw\'][\'target_id\'] > 0) {
	echo \'</a>\';
}
?>';
$handler->display->display_options['fields']['php']['use_php_click_sortable'] = '0';
$handler->display->display_options['fields']['php']['php_click_sortable'] = '';
/* Field: SUM(Content: Amount Paid) */
$handler->display->display_options['fields']['field_amount_paid']['id'] = 'field_amount_paid';
$handler->display->display_options['fields']['field_amount_paid']['table'] = 'field_data_field_amount_paid';
$handler->display->display_options['fields']['field_amount_paid']['field'] = 'field_amount_paid';
$handler->display->display_options['fields']['field_amount_paid']['group_type'] = 'sum';
$handler->display->display_options['fields']['field_amount_paid']['label'] = 'Merits';
$handler->display->display_options['fields']['field_amount_paid']['settings'] = array(
  'thousand_separator' => '',
  'decimal_separator' => '.',
  'scale' => '2',
  'prefix_suffix' => 0,
);
$handler->display->display_options['defaults']['sorts'] = FALSE;
$handler->display->display_options['defaults']['arguments'] = FALSE;
/* Contextual filter: Content: Nid */
$handler->display->display_options['arguments']['nid']['id'] = 'nid';
$handler->display->display_options['arguments']['nid']['table'] = 'node';
$handler->display->display_options['arguments']['nid']['field'] = 'nid';
$handler->display->display_options['arguments']['nid']['relationship'] = 'field_receipt_project_target_id';
$handler->display->display_options['arguments']['nid']['default_argument_type'] = 'fixed';
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
  'purchase_receipt' => 'purchase_receipt',
);
$handler->display->display_options['filters']['type']['group'] = 1;

/* Display: Page */
$handler = $view->new_display('page', 'Page', 'page_1');
$handler->display->display_options['defaults']['title'] = FALSE;
$handler->display->display_options['title'] = 'Project rewards';
$handler->display->display_options['defaults']['use_ajax'] = FALSE;
$handler->display->display_options['use_ajax'] = TRUE;
$handler->display->display_options['defaults']['group_by'] = FALSE;
$handler->display->display_options['defaults']['access'] = FALSE;
$handler->display->display_options['access']['type'] = 'role';
$handler->display->display_options['access']['role'] = array(
  2 => '2',
);
$handler->display->display_options['defaults']['style_plugin'] = FALSE;
$handler->display->display_options['style_plugin'] = 'table';
$handler->display->display_options['style_options']['columns'] = array(
  'nid' => 'nid',
  'field_purchase_date' => 'field_purchase_date',
  'field_receipt_project' => 'field_receipt_project',
  'field_buyer' => 'field_buyer',
  'language' => 'language',
  'field_amount_paid_1' => 'field_amount_paid_1',
);
$handler->display->display_options['style_options']['class'] = '';
$handler->display->display_options['style_options']['default'] = 'field_purchase_date';
$handler->display->display_options['style_options']['info'] = array(
  'nid' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_purchase_date' => array(
    'sortable' => 1,
    'default_sort_order' => 'desc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_receipt_project' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_buyer' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'language' => array(
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_amount_paid_1' => array(
    'sortable' => 1,
    'default_sort_order' => 'desc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
);
$handler->display->display_options['defaults']['style_options'] = FALSE;
$handler->display->display_options['defaults']['row_plugin'] = FALSE;
$handler->display->display_options['defaults']['row_options'] = FALSE;
$handler->display->display_options['defaults']['header'] = FALSE;
/* Header: Global: Text area */
$handler->display->display_options['header']['area']['id'] = 'area';
$handler->display->display_options['header']['area']['table'] = 'views';
$handler->display->display_options['header']['area']['field'] = 'area';
$handler->display->display_options['header']['area']['content'] = 'Project <strong>[field_receipt_project]</strong>';
$handler->display->display_options['header']['area']['format'] = 'full_html';
$handler->display->display_options['header']['area']['tokenize'] = TRUE;
$handler->display->display_options['defaults']['fields'] = FALSE;
/* Field: Content: Nid */
$handler->display->display_options['fields']['nid']['id'] = 'nid';
$handler->display->display_options['fields']['nid']['table'] = 'node';
$handler->display->display_options['fields']['nid']['field'] = 'nid';
$handler->display->display_options['fields']['nid']['label'] = '';
$handler->display->display_options['fields']['nid']['exclude'] = TRUE;
$handler->display->display_options['fields']['nid']['element_label_colon'] = FALSE;
/* Field: Content: Purchase Date */
$handler->display->display_options['fields']['field_purchase_date']['id'] = 'field_purchase_date';
$handler->display->display_options['fields']['field_purchase_date']['table'] = 'field_data_field_purchase_date';
$handler->display->display_options['fields']['field_purchase_date']['field'] = 'field_purchase_date';
$handler->display->display_options['fields']['field_purchase_date']['label'] = 'Date';
$handler->display->display_options['fields']['field_purchase_date']['alter']['alter_text'] = TRUE;
$handler->display->display_options['fields']['field_purchase_date']['alter']['text'] = '<a href="/node/[nid]">[field_purchase_date]</a>';
/* Field: Content: Project */
$handler->display->display_options['fields']['field_receipt_project']['id'] = 'field_receipt_project';
$handler->display->display_options['fields']['field_receipt_project']['table'] = 'field_data_field_receipt_project';
$handler->display->display_options['fields']['field_receipt_project']['field'] = 'field_receipt_project';
$handler->display->display_options['fields']['field_receipt_project']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_receipt_project']['settings'] = array(
  'bypass_access' => 0,
  'link' => 1,
);
/* Field: Content: Buyer */
$handler->display->display_options['fields']['field_buyer']['id'] = 'field_buyer';
$handler->display->display_options['fields']['field_buyer']['table'] = 'field_data_field_buyer';
$handler->display->display_options['fields']['field_buyer']['field'] = 'field_buyer';
$handler->display->display_options['fields']['field_buyer']['settings'] = array(
  'bypass_access' => 0,
  'link' => 1,
);
/* Field: Content: Body (body:language) */
$handler->display->display_options['fields']['language']['id'] = 'language';
$handler->display->display_options['fields']['language']['table'] = 'field_data_body';
$handler->display->display_options['fields']['language']['field'] = 'language';
$handler->display->display_options['fields']['language']['label'] = 'Note';
/* Field: Content: Amount Paid */
$handler->display->display_options['fields']['field_amount_paid_1']['id'] = 'field_amount_paid_1';
$handler->display->display_options['fields']['field_amount_paid_1']['table'] = 'field_data_field_amount_paid';
$handler->display->display_options['fields']['field_amount_paid_1']['field'] = 'field_amount_paid';
$handler->display->display_options['fields']['field_amount_paid_1']['settings'] = array(
  'thousand_separator' => '',
  'decimal_separator' => '.',
  'scale' => '2',
  'prefix_suffix' => 1,
);
$handler->display->display_options['defaults']['sorts'] = FALSE;
$handler->display->display_options['defaults']['arguments'] = FALSE;
/* Contextual filter: Content: Nid */
$handler->display->display_options['arguments']['nid']['id'] = 'nid';
$handler->display->display_options['arguments']['nid']['table'] = 'node';
$handler->display->display_options['arguments']['nid']['field'] = 'nid';
$handler->display->display_options['arguments']['nid']['relationship'] = 'field_receipt_project_target_id';
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
  'purchase_receipt' => 'purchase_receipt',
);
$handler->display->display_options['filters']['type']['group'] = 1;
$handler->display->display_options['path'] = 'project-purchases/%';

/* Display: Page 2 */
$handler = $view->new_display('page', 'Page 2', 'purchases');
$handler->display->display_options['defaults']['title'] = FALSE;
$handler->display->display_options['title'] = 'Project payments';
$handler->display->display_options['defaults']['group_by'] = FALSE;
$handler->display->display_options['defaults']['access'] = FALSE;
$handler->display->display_options['access']['type'] = 'php';
$handler->display->display_options['access']['php_access'] = '$uids = array(1); //allow admin access by default

if (is_numeric(arg(1))) {
	$nid = arg(1);
	$project = entity_metadata_wrapper(\'node\',$nid);
	$ary = $project->field_privileged_users->value();

	$uids[] = $project->author->uid->value(); //allos access to the project owner 
	
	foreach ($ary as $usr) {
		$uids[] = $usr->uid;
	}
}

global $user;
return $user->uid == 0 ? FALSE : in_array($user->uid,$uids);';
$handler->display->display_options['defaults']['style_plugin'] = FALSE;
$handler->display->display_options['style_plugin'] = 'table';
$handler->display->display_options['style_options']['columns'] = array(
  'nid' => 'nid',
  'created' => 'created',
  'field_note' => 'field_note',
  'field_usager' => 'field_usager',
  'php' => 'php',
  'php_2' => 'php_2',
  'field_asset_usage_fee' => 'field_asset_usage_fee',
  'php_1' => 'php_1',
);
$handler->display->display_options['style_options']['class'] = '';
$handler->display->display_options['style_options']['default'] = 'created';
$handler->display->display_options['style_options']['info'] = array(
  'nid' => array(
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
  'field_note' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_usager' => array(
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
  'php_2' => array(
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_asset_usage_fee' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => 'views-align-right',
    'separator' => '',
    'empty_column' => 0,
  ),
  'php_1' => array(
    'align' => 'views-align-right',
    'separator' => '',
    'empty_column' => 0,
  ),
);
$handler->display->display_options['defaults']['style_options'] = FALSE;
$handler->display->display_options['defaults']['row_plugin'] = FALSE;
$handler->display->display_options['defaults']['row_options'] = FALSE;
$handler->display->display_options['defaults']['relationships'] = FALSE;
$handler->display->display_options['defaults']['fields'] = FALSE;
/* Field: Content: Nid */
$handler->display->display_options['fields']['nid']['id'] = 'nid';
$handler->display->display_options['fields']['nid']['table'] = 'node';
$handler->display->display_options['fields']['nid']['field'] = 'nid';
$handler->display->display_options['fields']['nid']['label'] = '';
$handler->display->display_options['fields']['nid']['exclude'] = TRUE;
$handler->display->display_options['fields']['nid']['element_label_colon'] = FALSE;
/* Field: Content: Post date */
$handler->display->display_options['fields']['created']['id'] = 'created';
$handler->display->display_options['fields']['created']['table'] = 'node';
$handler->display->display_options['fields']['created']['field'] = 'created';
$handler->display->display_options['fields']['created']['label'] = 'Date';
$handler->display->display_options['fields']['created']['alter']['alter_text'] = TRUE;
$handler->display->display_options['fields']['created']['alter']['text'] = '<a href="/node/[nid]">[created]</a>';
$handler->display->display_options['fields']['created']['date_format'] = 'custom';
$handler->display->display_options['fields']['created']['custom_date_format'] = 'd M Y';
$handler->display->display_options['fields']['created']['second_date_format'] = 'privatemsg_current_day';
/* Field: Content: Note */
$handler->display->display_options['fields']['field_note']['id'] = 'field_note';
$handler->display->display_options['fields']['field_note']['table'] = 'field_data_field_note';
$handler->display->display_options['fields']['field_note']['field'] = 'field_note';
$handler->display->display_options['fields']['field_note']['label'] = '';
$handler->display->display_options['fields']['field_note']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_note']['element_label_colon'] = FALSE;
/* Field: Content: Consumer */
$handler->display->display_options['fields']['field_usager']['id'] = 'field_usager';
$handler->display->display_options['fields']['field_usager']['table'] = 'field_data_field_usager';
$handler->display->display_options['fields']['field_usager']['field'] = 'field_usager';
$handler->display->display_options['fields']['field_usager']['label'] = '';
$handler->display->display_options['fields']['field_usager']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_usager']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_usager']['settings'] = array(
  'bypass_access' => 0,
  'link' => 0,
);
/* Field: Global: PHP */
$handler->display->display_options['fields']['php']['id'] = 'php';
$handler->display->display_options['fields']['php']['table'] = 'views';
$handler->display->display_options['fields']['php']['field'] = 'php';
$handler->display->display_options['fields']['php']['label'] = 'Buyer';
$handler->display->display_options['fields']['php']['use_php_setup'] = 1;
$handler->display->display_options['fields']['php']['php_setup'] = 'function order_from_note($note) {
	preg_match(\'/Forex payment by Anonymous in order (\\d+)/\', $note, $matches, PREG_OFFSET_CAPTURE);
	$order_nid = isset($matches[1][0]) ? $matches[1][0] : \'\';

	if ($order_nid){

		return commerce_order_load($order_nid);
	}
	else {
		$blank = new stdClass();
		return $blank;
	}
}';
$handler->display->display_options['fields']['php']['php_output'] = '<?php 

$note = isset($data->field_field_note[0][\'raw\'][\'value\']) ? $data->field_field_note[0][\'raw\'][\'value\'] : \'\';

$order = order_from_note($note);

if (isset($order->commerce_customer_billing)){

	$billing_id = $order->commerce_customer_billing[LANGUAGE_NONE][0][\'profile_id\'];

	$cprofile = commerce_customer_profile_load($order->commerce_customer_billing[\'und\'][0][\'profile_id\']);
	$user = user_load_by_mail($order->field_buyer_email[LANGUAGE_NONE][0][\'value\']);

	echo isset($cprofile->commerce_customer_address[LANGUAGE_NONE][0][\'name_line\']) ? $cprofile->commerce_customer_address[LANGUAGE_NONE][0][\'name_line\'] : $cprofile->commerce_customer_address[\'und\'][0][\'first_name\'] . \' \' . $cprofile->commerce_customer_address[\'und\'][0][\'last_name\'];
}
else {
	$uid = isset($data->field_field_usager[0][\'raw\'][\'target_id\']) ? $data->field_field_usager[0][\'raw\'][\'target_id\'] : 0;
	$name = isset($data->field_field_usager[0][\'rendered\'][\'#label\']) ? $data->field_field_usager[0][\'rendered\'][\'#label\'] : t(\'Anonymous\');
	
	echo \'<a href="\' . $uid . \'">\' .  $name . \'</a>\';
}


?>';
$handler->display->display_options['fields']['php']['use_php_click_sortable'] = '0';
$handler->display->display_options['fields']['php']['php_click_sortable'] = '';
/* Field: Global: PHP */
$handler->display->display_options['fields']['php_2']['id'] = 'php_2';
$handler->display->display_options['fields']['php_2']['table'] = 'views';
$handler->display->display_options['fields']['php_2']['field'] = 'php';
$handler->display->display_options['fields']['php_2']['label'] = 'Email';
$handler->display->display_options['fields']['php_2']['use_php_setup'] = 0;
$handler->display->display_options['fields']['php_2']['php_output'] = '<?php 
$note = isset($data->field_field_note[0][\'raw\'][\'value\']) ? $data->field_field_note[0][\'raw\'][\'value\'] : \'\';
$order = order_from_note($note);

if ($data->field_field_usager[0][\'raw\'][\'target_id\'] > 0) {
	$user = user_load($data->field_field_usager[0][\'raw\'][\'target_id\']);
	echo $user->mail;
}
else {
	echo $order->mail;
}
?>';
$handler->display->display_options['fields']['php_2']['use_php_click_sortable'] = '0';
$handler->display->display_options['fields']['php_2']['php_click_sortable'] = '';
/* Field: Content: Paid */
$handler->display->display_options['fields']['field_asset_usage_fee']['id'] = 'field_asset_usage_fee';
$handler->display->display_options['fields']['field_asset_usage_fee']['table'] = 'field_data_field_asset_usage_fee';
$handler->display->display_options['fields']['field_asset_usage_fee']['field'] = 'field_asset_usage_fee';
$handler->display->display_options['fields']['field_asset_usage_fee']['label'] = 'Chrons';
$handler->display->display_options['fields']['field_asset_usage_fee']['settings'] = array(
  'thousand_separator' => '',
  'decimal_separator' => '.',
  'scale' => '2',
  'prefix_suffix' => 0,
);
/* Field: Euros */
$handler->display->display_options['fields']['php_1']['id'] = 'php_1';
$handler->display->display_options['fields']['php_1']['table'] = 'views';
$handler->display->display_options['fields']['php_1']['field'] = 'php';
$handler->display->display_options['fields']['php_1']['ui_name'] = 'Euros';
$handler->display->display_options['fields']['php_1']['label'] = 'Euros';
$handler->display->display_options['fields']['php_1']['use_php_setup'] = 0;
$handler->display->display_options['fields']['php_1']['php_output'] = '<?php 
$note = isset($data->field_field_note[0][\'raw\'][\'value\']) ? $data->field_field_note[0][\'raw\'][\'value\'] : \'\';
$order = order_from_note($note);

if (isset($order->commerce_line_items)) {
	$line_item = commerce_line_item_load($order->commerce_line_items[\'und\'][0][\'line_item_id\']);
	echo round($line_item->commerce_total[\'und\'][0][\'amount\']/100);
}
else {
	echo 0;
}
?>';
$handler->display->display_options['fields']['php_1']['use_php_click_sortable'] = '0';
$handler->display->display_options['fields']['php_1']['php_click_sortable'] = '';
$handler->display->display_options['defaults']['sorts'] = FALSE;
$handler->display->display_options['defaults']['arguments'] = FALSE;
/* Contextual filter: Content: Asset owning project (field__asset_owning_project) */
$handler->display->display_options['arguments']['field__asset_owning_project_target_id']['id'] = 'field__asset_owning_project_target_id';
$handler->display->display_options['arguments']['field__asset_owning_project_target_id']['table'] = 'field_data_field__asset_owning_project';
$handler->display->display_options['arguments']['field__asset_owning_project_target_id']['field'] = 'field__asset_owning_project_target_id';
$handler->display->display_options['arguments']['field__asset_owning_project_target_id']['default_argument_type'] = 'fixed';
$handler->display->display_options['arguments']['field__asset_owning_project_target_id']['summary']['number_of_records'] = '0';
$handler->display->display_options['arguments']['field__asset_owning_project_target_id']['summary']['format'] = 'default_summary';
$handler->display->display_options['arguments']['field__asset_owning_project_target_id']['summary_options']['items_per_page'] = '25';
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
  'asset_usage_record' => 'asset_usage_record',
);
$handler->display->display_options['path'] = 'project-payments';

/* Display: Services */
$handler = $view->new_display('services', 'Services', 'services_1');
$handler->display->display_options['defaults']['pager'] = FALSE;
$handler->display->display_options['pager']['type'] = 'some';
$handler->display->display_options['pager']['options']['items_per_page'] = '1';
$handler->display->display_options['pager']['options']['offset'] = '0';
$handler->display->display_options['defaults']['relationships'] = FALSE;
/* Relationship: Entity Reference: Referenced Entity */
$handler->display->display_options['relationships']['field_buyer_target_id']['id'] = 'field_buyer_target_id';
$handler->display->display_options['relationships']['field_buyer_target_id']['table'] = 'field_data_field_buyer';
$handler->display->display_options['relationships']['field_buyer_target_id']['field'] = 'field_buyer_target_id';
$handler->display->display_options['relationships']['field_buyer_target_id']['label'] = 'buyer';
$handler->display->display_options['defaults']['fields'] = FALSE;
/* Field: Content: Buyer */
$handler->display->display_options['fields']['field_buyer']['id'] = 'field_buyer';
$handler->display->display_options['fields']['field_buyer']['table'] = 'field_data_field_buyer';
$handler->display->display_options['fields']['field_buyer']['field'] = 'field_buyer';
$handler->display->display_options['fields']['field_buyer']['label'] = 'rovas_uid';
$handler->display->display_options['fields']['field_buyer']['type'] = 'entityreference_entity_id';
$handler->display->display_options['fields']['field_buyer']['settings'] = array(
  'bypass_access' => 0,
  'link' => 0,
);
/* Field: User: Name */
$handler->display->display_options['fields']['name']['id'] = 'name';
$handler->display->display_options['fields']['name']['table'] = 'users';
$handler->display->display_options['fields']['name']['field'] = 'name';
$handler->display->display_options['fields']['name']['relationship'] = 'field_buyer_target_id';
$handler->display->display_options['fields']['name']['label'] = 'name';
$handler->display->display_options['fields']['name']['link_to_user'] = FALSE;
/* Field: SUM(Content: Amount Paid) */
$handler->display->display_options['fields']['field_amount_paid']['id'] = 'field_amount_paid';
$handler->display->display_options['fields']['field_amount_paid']['table'] = 'field_data_field_amount_paid';
$handler->display->display_options['fields']['field_amount_paid']['field'] = 'field_amount_paid';
$handler->display->display_options['fields']['field_amount_paid']['group_type'] = 'sum';
$handler->display->display_options['fields']['field_amount_paid']['label'] = 'merits';
$handler->display->display_options['fields']['field_amount_paid']['settings'] = array(
  'thousand_separator' => '',
  'decimal_separator' => '.',
  'scale' => '2',
  'prefix_suffix' => 0,
);
$handler->display->display_options['defaults']['sorts'] = FALSE;
/* Sort criterion: SUM(Content: Amount Paid (field_amount_paid)) */
$handler->display->display_options['sorts']['field_amount_paid_value']['id'] = 'field_amount_paid_value';
$handler->display->display_options['sorts']['field_amount_paid_value']['table'] = 'field_data_field_amount_paid';
$handler->display->display_options['sorts']['field_amount_paid_value']['field'] = 'field_amount_paid_value';
$handler->display->display_options['sorts']['field_amount_paid_value']['group_type'] = 'sum';
$handler->display->display_options['sorts']['field_amount_paid_value']['order'] = 'DESC';
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
  'purchase_receipt' => 'purchase_receipt',
);
$handler->display->display_options['filters']['type']['group'] = 1;
/* Filter criterion: Content: Project (field_receipt_project) */
$handler->display->display_options['filters']['field_receipt_project_target_id']['id'] = 'field_receipt_project_target_id';
$handler->display->display_options['filters']['field_receipt_project_target_id']['table'] = 'field_data_field_receipt_project';
$handler->display->display_options['filters']['field_receipt_project_target_id']['field'] = 'field_receipt_project_target_id';
$handler->display->display_options['filters']['field_receipt_project_target_id']['exposed'] = TRUE;
$handler->display->display_options['filters']['field_receipt_project_target_id']['expose']['operator_id'] = 'field_receipt_project_target_id_op';
$handler->display->display_options['filters']['field_receipt_project_target_id']['expose']['label'] = 'Project (field_receipt_project)';
$handler->display->display_options['filters']['field_receipt_project_target_id']['expose']['operator'] = 'field_receipt_project_target_id_op';
$handler->display->display_options['filters']['field_receipt_project_target_id']['expose']['identifier'] = 'project_id';
$handler->display->display_options['filters']['field_receipt_project_target_id']['expose']['required'] = TRUE;
$handler->display->display_options['filters']['field_receipt_project_target_id']['expose']['remember_roles'] = array(
  2 => '2',
);
$handler->display->display_options['path'] = 'rest_view_project_buyers';
$translatables['project_merits_feed'] = array(
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
  t('project'),
  t('project_id'),
  t('purchases_count'),
  t('merits'),
  t('Block'),
  t('more ...'),
  t('Date'),
  t('Buyer'),
  t('Merits'),
  t('All'),
  t('Page'),
  t('Project rewards'),
  t('Project <strong>[field_receipt_project]</strong>'),
  t('<a href="/node/[nid]">[field_purchase_date]</a>'),
  t('Project'),
  t('Note'),
  t('Amount Paid'),
  t('Page 2'),
  t('Project payments'),
  t('<a href="/node/[nid]">[created]</a>'),
  t('Email'),
  t('Chrons'),
  t('Euros'),
  t('Services'),
  t('buyer'),
  t('rovas_uid'),
  t('name'),
  t('Project (field_receipt_project)'),
);

