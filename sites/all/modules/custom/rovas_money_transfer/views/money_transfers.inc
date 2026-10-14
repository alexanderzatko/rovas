<?php

/**
 * @file
 * Default view: money_transfers
 */

$view = new view();
$view->name = 'money_transfers';
$view->description = '';
$view->tag = 'default';
$view->base_table = 'node';
$view->human_name = 'Money transfers';
$view->core = 7;
$view->api_version = '3.0';
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

/* Display: Master */
$handler = $view->new_display('default', 'Master', 'default');
$handler->display->display_options['use_ajax'] = TRUE;
$handler->display->display_options['use_more_always'] = FALSE;
$handler->display->display_options['access']['type'] = 'role';
$handler->display->display_options['access']['role'] = array(
  2 => '2',
);
$handler->display->display_options['cache']['type'] = 'none';
$handler->display->display_options['query']['type'] = 'views_query';
$handler->display->display_options['query']['options']['pure_distinct'] = TRUE;
$handler->display->display_options['query']['options']['contextual_filters_or'] = TRUE;
$handler->display->display_options['exposed_form']['type'] = 'basic';
$handler->display->display_options['exposed_form']['options']['reset_button'] = TRUE;
$handler->display->display_options['exposed_form']['options']['expose_sort_order'] = FALSE;
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
$handler->display->display_options['style_options']['row_class'] = '[php_4]';
$handler->display->display_options['style_options']['columns'] = array(
  'field_amount' => 'field_amount',
  'field_amount_paid' => 'field_amount_paid',
  'field_chrons_to_grant' => 'field_chrons_to_grant',
  'field_verification_status' => 'field_verification_status',
  'field_buyer' => 'field_buyer',
  'field_full_name_2' => 'field_full_name_2',
  'uid' => 'uid',
  'name' => 'name',
  'field_full_name' => 'field_full_name',
  'field_receipt_project' => 'field_receipt_project',
  'name_1' => 'name_1',
  'field_full_name_1' => 'field_full_name_1',
  'type' => 'type',
  'created' => 'created',
  'title' => 'title',
  'field_conversion_type' => 'field_conversion_type',
  'php_3' => 'php_3',
  'php' => 'php',
  'php_2' => 'php_2',
  'php_4' => 'php_4',
  'field_hours_worked' => 'field_hours_worked',
  'field_input_amount' => 'field_input_amount',
  'field_output_amount' => 'field_output_amount',
  'php_1' => 'php_1',
);
$handler->display->display_options['style_options']['class'] = '';
$handler->display->display_options['style_options']['default'] = '-1';
$handler->display->display_options['style_options']['info'] = array(
  'field_amount' => array(
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
  'field_chrons_to_grant' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_verification_status' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
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
  'field_full_name_2' => array(
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
  'field_receipt_project' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'name_1' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_full_name_1' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'type' => array(
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
  'title' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_conversion_type' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'php_3' => array(
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
  'php_4' => array(
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_hours_worked' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_input_amount' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_output_amount' => array(
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
);
/* Header: Global: PHP */
$handler->display->display_options['header']['php']['id'] = 'php';
$handler->display->display_options['header']['php']['table'] = 'views';
$handler->display->display_options['header']['php']['field'] = 'php';
$handler->display->display_options['header']['php']['php_output'] = '<?php

global $user;
$viewed_uid = arg(1) ? arg(1) : 0;
if ($user->uid == $viewed_uid) {
	echo \'<div class="send-ch-text">\';
	echo t(\'Clicking the button below will open a form, which allows you to send Chrons to another person.\');
	echo \'</div><div class="send-ch-button">\';
	echo \'<a href="/node/add/money-transfer" class="btn btn-primary" role="button">\' . t(\'Send money\') . \'</a>\';
	echo \'</div>\';
}
echo \'<div class="ldgr-legend">\';
echo \'<div class="legend-title">\';
echo t(\'Legend\');
echo \'</div>\';
echo \'<div class="color debit">\';
echo \'&nbsp;\';
echo \'</div>\';
echo \'<div class="label">\';
echo t(\'Debits\');
echo \'</div>\';
echo \'<div class="color credit">\';
echo \'&nbsp;\';
echo \'</div>\';
echo \'<div class="label">\';
echo t(\'Credits\');
echo \'</div>\';
echo \'<div class="color noop">\';
echo \'&nbsp;\';
echo \'</div>\';
echo \'<div class="label">\';
echo t(\'No operation (circular)\');
echo \'</div>\';
echo \'</div>\';
?>';
/* Footer: Global: PHP */
$handler->display->display_options['footer']['php']['id'] = 'php';
$handler->display->display_options['footer']['php']['table'] = 'views';
$handler->display->display_options['footer']['php']['field'] = 'php';
$handler->display->display_options['footer']['php']['php_output'] = '<?php
echo \'<div class="transfers-note">*\' . t(\'Not verified work reports will be granted the Chron amounts shown in parentheses after they are approved\') . \'</div>\';
?>';
/* No results behavior: Global: Text area */
$handler->display->display_options['empty']['area']['id'] = 'area';
$handler->display->display_options['empty']['area']['table'] = 'views';
$handler->display->display_options['empty']['area']['field'] = 'area';
$handler->display->display_options['empty']['area']['empty'] = TRUE;
$handler->display->display_options['empty']['area']['content'] = 'There are no transactions.';
$handler->display->display_options['empty']['area']['format'] = 'plain_text';
/* Relationship: Content: Content author */
$handler->display->display_options['relationships']['uid']['id'] = 'uid';
$handler->display->display_options['relationships']['uid']['table'] = 'node';
$handler->display->display_options['relationships']['uid']['field'] = 'uid';
/* Relationship: Entity Reference: Referenced Entity */
$handler->display->display_options['relationships']['field_recipient_target_id']['id'] = 'field_recipient_target_id';
$handler->display->display_options['relationships']['field_recipient_target_id']['table'] = 'field_data_field_recipient';
$handler->display->display_options['relationships']['field_recipient_target_id']['field'] = 'field_recipient_target_id';
/* Relationship: Entity Reference: Referenced Entity */
$handler->display->display_options['relationships']['field_buyer_target_id']['id'] = 'field_buyer_target_id';
$handler->display->display_options['relationships']['field_buyer_target_id']['table'] = 'field_data_field_buyer';
$handler->display->display_options['relationships']['field_buyer_target_id']['field'] = 'field_buyer_target_id';
/* Field: Content: Amount */
$handler->display->display_options['fields']['field_amount']['id'] = 'field_amount';
$handler->display->display_options['fields']['field_amount']['table'] = 'field_data_field_amount';
$handler->display->display_options['fields']['field_amount']['field'] = 'field_amount';
$handler->display->display_options['fields']['field_amount']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_amount']['settings'] = array(
  'thousand_separator' => ' ',
  'decimal_separator' => '.',
  'scale' => '2',
  'prefix_suffix' => 0,
);
/* Field: Purchase Amount */
$handler->display->display_options['fields']['field_amount_paid']['id'] = 'field_amount_paid';
$handler->display->display_options['fields']['field_amount_paid']['table'] = 'field_data_field_amount_paid';
$handler->display->display_options['fields']['field_amount_paid']['field'] = 'field_amount_paid';
$handler->display->display_options['fields']['field_amount_paid']['ui_name'] = 'Purchase Amount';
$handler->display->display_options['fields']['field_amount_paid']['label'] = '';
$handler->display->display_options['fields']['field_amount_paid']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_amount_paid']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_amount_paid']['settings'] = array(
  'thousand_separator' => '',
  'decimal_separator' => '.',
  'scale' => '2',
  'prefix_suffix' => 0,
);
/* Field: Content: Reward */
$handler->display->display_options['fields']['field_chrons_to_grant']['id'] = 'field_chrons_to_grant';
$handler->display->display_options['fields']['field_chrons_to_grant']['table'] = 'field_data_field_chrons_to_grant';
$handler->display->display_options['fields']['field_chrons_to_grant']['field'] = 'field_chrons_to_grant';
$handler->display->display_options['fields']['field_chrons_to_grant']['label'] = '';
$handler->display->display_options['fields']['field_chrons_to_grant']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_chrons_to_grant']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_chrons_to_grant']['settings'] = array(
  'thousand_separator' => '',
  'decimal_separator' => '.',
  'scale' => '2',
  'prefix_suffix' => 0,
);
/* Field: Content: Verification status */
$handler->display->display_options['fields']['field_verification_status']['id'] = 'field_verification_status';
$handler->display->display_options['fields']['field_verification_status']['table'] = 'field_data_field_verification_status';
$handler->display->display_options['fields']['field_verification_status']['field'] = 'field_verification_status';
$handler->display->display_options['fields']['field_verification_status']['label'] = '';
$handler->display->display_options['fields']['field_verification_status']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_verification_status']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_verification_status']['type'] = 'list_key';
$handler->display->display_options['fields']['field_verification_status']['settings'] = array(
  'thousand_separator' => '',
  'prefix_suffix' => 0,
);
/* Field: Content: Buyer */
$handler->display->display_options['fields']['field_buyer']['id'] = 'field_buyer';
$handler->display->display_options['fields']['field_buyer']['table'] = 'field_data_field_buyer';
$handler->display->display_options['fields']['field_buyer']['field'] = 'field_buyer';
$handler->display->display_options['fields']['field_buyer']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_buyer']['settings'] = array(
  'bypass_access' => 0,
  'link' => 1,
);
/* Field: User: Full name */
$handler->display->display_options['fields']['field_full_name_2']['id'] = 'field_full_name_2';
$handler->display->display_options['fields']['field_full_name_2']['table'] = 'field_data_field_full_name';
$handler->display->display_options['fields']['field_full_name_2']['field'] = 'field_full_name';
$handler->display->display_options['fields']['field_full_name_2']['relationship'] = 'field_buyer_target_id';
$handler->display->display_options['fields']['field_full_name_2']['exclude'] = TRUE;
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
$handler->display->display_options['fields']['name']['label'] = '';
$handler->display->display_options['fields']['name']['exclude'] = TRUE;
$handler->display->display_options['fields']['name']['element_label_colon'] = FALSE;
/* Field: User: Full name */
$handler->display->display_options['fields']['field_full_name']['id'] = 'field_full_name';
$handler->display->display_options['fields']['field_full_name']['table'] = 'field_data_field_full_name';
$handler->display->display_options['fields']['field_full_name']['field'] = 'field_full_name';
$handler->display->display_options['fields']['field_full_name']['relationship'] = 'uid';
$handler->display->display_options['fields']['field_full_name']['label'] = 'Sender Full name';
$handler->display->display_options['fields']['field_full_name']['exclude'] = TRUE;
/* Field: Content: Project */
$handler->display->display_options['fields']['field_receipt_project']['id'] = 'field_receipt_project';
$handler->display->display_options['fields']['field_receipt_project']['table'] = 'field_data_field_receipt_project';
$handler->display->display_options['fields']['field_receipt_project']['field'] = 'field_receipt_project';
$handler->display->display_options['fields']['field_receipt_project']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_receipt_project']['settings'] = array(
  'bypass_access' => 0,
  'link' => 1,
);
/* Field: User: Name */
$handler->display->display_options['fields']['name_1']['id'] = 'name_1';
$handler->display->display_options['fields']['name_1']['table'] = 'users';
$handler->display->display_options['fields']['name_1']['field'] = 'name';
$handler->display->display_options['fields']['name_1']['relationship'] = 'field_recipient_target_id';
$handler->display->display_options['fields']['name_1']['label'] = '';
$handler->display->display_options['fields']['name_1']['exclude'] = TRUE;
$handler->display->display_options['fields']['name_1']['element_label_colon'] = FALSE;
/* Field: User: Full name */
$handler->display->display_options['fields']['field_full_name_1']['id'] = 'field_full_name_1';
$handler->display->display_options['fields']['field_full_name_1']['table'] = 'field_data_field_full_name';
$handler->display->display_options['fields']['field_full_name_1']['field'] = 'field_full_name';
$handler->display->display_options['fields']['field_full_name_1']['relationship'] = 'field_recipient_target_id';
$handler->display->display_options['fields']['field_full_name_1']['label'] = 'Recipient';
$handler->display->display_options['fields']['field_full_name_1']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_full_name_1']['alter']['text'] = '[field_full_name_1] ([name_1])';
/* Field: Content: Type */
$handler->display->display_options['fields']['type']['id'] = 'type';
$handler->display->display_options['fields']['type']['table'] = 'node';
$handler->display->display_options['fields']['type']['field'] = 'type';
$handler->display->display_options['fields']['type']['exclude'] = TRUE;
$handler->display->display_options['fields']['type']['machine_name'] = TRUE;
/* Field: Content: Post date */
$handler->display->display_options['fields']['created']['id'] = 'created';
$handler->display->display_options['fields']['created']['table'] = 'node';
$handler->display->display_options['fields']['created']['field'] = 'created';
$handler->display->display_options['fields']['created']['label'] = 'Date';
$handler->display->display_options['fields']['created']['date_format'] = 'short_timestamp_with_spelled_month';
$handler->display->display_options['fields']['created']['second_date_format'] = 'privatemsg_current_day';
/* Field: Entity translation: Title: translated */
$handler->display->display_options['fields']['title_field_et']['id'] = 'title_field_et';
$handler->display->display_options['fields']['title_field_et']['table'] = 'field_data_title_field';
$handler->display->display_options['fields']['title_field_et']['field'] = 'title_field_et';
$handler->display->display_options['fields']['title_field_et']['label'] = 'Title';
$handler->display->display_options['fields']['title_field_et']['exclude'] = TRUE;
$handler->display->display_options['fields']['title_field_et']['type'] = 'title_linked';
$handler->display->display_options['fields']['title_field_et']['settings'] = array(
  'title_style' => '_none',
  'title_link' => 'content',
  'enable_trim' => 0,
  'trim_length' => '50',
  'title_class' => '',
);
/* Field: Content: Title */
$handler->display->display_options['fields']['title']['id'] = 'title';
$handler->display->display_options['fields']['title']['table'] = 'node';
$handler->display->display_options['fields']['title']['field'] = 'title';
$handler->display->display_options['fields']['title']['label'] = 'ID';
$handler->display->display_options['fields']['title']['exclude'] = TRUE;
/* Field: Content: Conversion type */
$handler->display->display_options['fields']['field_conversion_type']['id'] = 'field_conversion_type';
$handler->display->display_options['fields']['field_conversion_type']['table'] = 'field_data_field_conversion_type';
$handler->display->display_options['fields']['field_conversion_type']['field'] = 'field_conversion_type';
$handler->display->display_options['fields']['field_conversion_type']['label'] = '';
$handler->display->display_options['fields']['field_conversion_type']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_conversion_type']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_conversion_type']['type'] = 'list_key';
/* Field: transaction type */
$handler->display->display_options['fields']['php_3']['id'] = 'php_3';
$handler->display->display_options['fields']['php_3']['table'] = 'views';
$handler->display->display_options['fields']['php_3']['field'] = 'php';
$handler->display->display_options['fields']['php_3']['ui_name'] = 'transaction type';
$handler->display->display_options['fields']['php_3']['label'] = 'Type';
$handler->display->display_options['fields']['php_3']['use_php_setup'] = 0;
$handler->display->display_options['fields']['php_3']['php_output'] = '<?php

switch ($data->node_type) {
	case \'money_transfer\':
		print t(\'transaction\') . \' <a href="/node/\'.$data->_field_data[\'nid\'][\'entity\']->nid.\'">\'.$data->_field_data[\'nid\'][\'entity\']->title.\'</a>\';
	break;
	case \'chron_adjustment\':
		print t(\'adjustment\') . \' <a href="/node/\'.$data->_field_data[\'nid\'][\'entity\']->nid.\'">\'.$data->_field_data[\'nid\'][\'entity\']->title.\'</a>\';
	break;
	case \'purchase_receipt\':
		if ($data->field_field_amount_paid < 0)
			print t(\'cancel. purchase\') . \' <a href="/node/\'.$data->_field_data[\'nid\'][\'entity\']->nid.\'">\'.$data->_field_data[\'nid\'][\'entity\']->title.\'</a>\';
		else
			print t(\'purchase\') . \' <a href="/node/\'.$data->_field_data[\'nid\'][\'entity\']->nid.\'">\'.$data->_field_data[\'nid\'][\'entity\']->title.\'</a>\';
	break;
	case \'forex_transaction\':
		if ($data->field_field_conversion_type[0][\'rendered\'][\'#markup\'] == \'chrons_to_euros\')
			print t(\'Chrons sale\') . \' <a href="/node/\'.$data->_field_data[\'nid\'][\'entity\']->nid.\'">\'.$data->_field_data[\'nid\'][\'entity\']->title.\'</a>\';
		else
			print t(\'Chrons purchase\') . \' <a href="/node/\'.$data->_field_data[\'nid\'][\'entity\']->nid.\'">\'.$data->_field_data[\'nid\'][\'entity\']->title.\'</a>\';
	break;
	default:
	print t(\'work\') . \' <a href="/node/\'.$data->_field_data[\'nid\'][\'entity\']->nid.\'">\'.$data->field_title_field_et[0][\'rendered\'][\'#markup\'].\'</a>\';
}

?>';
$handler->display->display_options['fields']['php_3']['use_php_click_sortable'] = '0';
$handler->display->display_options['fields']['php_3']['php_click_sortable'] = '';
/* Field: php */
$handler->display->display_options['fields']['php']['id'] = 'php';
$handler->display->display_options['fields']['php']['table'] = 'views';
$handler->display->display_options['fields']['php']['field'] = 'php';
$handler->display->display_options['fields']['php']['ui_name'] = 'php';
$handler->display->display_options['fields']['php']['label'] = 'Sender';
$handler->display->display_options['fields']['php']['use_php_setup'] = 0;
$handler->display->display_options['fields']['php']['php_output'] = '<?php 

switch ($data->node_type) {
	case \'purchase_receipt\':
		if ($data->field_field_amount_paid[0][\'raw\'][\'value\'] < 0)
			print t(\'Project\') . \': <a href="\'.$data->field_field_receipt_project[0][\'rendered\'][\'#item\'][\'target_id\'].\'">\' . $data->field_field_receipt_project[0][\'rendered\'][\'#label\'] . \'</a>\';
		else
			print \'<a href="/user/\'.$data->field_field_buyer[0][\'raw\'][\'target_id\'].\'">\'.$data->field_field_buyer[0][\'rendered\'][\'#label\'].\'</a>\';
	break;
	case \'money_transfer\':
		print $data->_field_data[\'users_node_uid\'][\'entity\']->uid == 0 ? t(\'Anonymous\') : \'<a href="/user/\'.$data->_field_data[\'users_node_uid\'][\'entity\']->uid.\'">\'.$data->_field_data[\'users_node_uid\'][\'entity\']->name.\'</a>\';
	break;
	case \'forex_transaction\':
		print \'<a href="/user/\'.$data->field_field_buyer[0][\'raw\'][\'target_id\'].\'">\'.$data->field_field_buyer[0][\'rendered\'][\'#label\'].\'</a>\';
	break;
	case \'chron_adjustment\':
		print \'Rovas\';
	break;
	default: //work report
		print \'Rovas\';
}
?>';
$handler->display->display_options['fields']['php']['use_php_click_sortable'] = '0';
$handler->display->display_options['fields']['php']['php_click_sortable'] = '';
/* Field: php */
$handler->display->display_options['fields']['php_2']['id'] = 'php_2';
$handler->display->display_options['fields']['php_2']['table'] = 'views';
$handler->display->display_options['fields']['php_2']['field'] = 'php';
$handler->display->display_options['fields']['php_2']['ui_name'] = 'php';
$handler->display->display_options['fields']['php_2']['label'] = 'Recipient';
$handler->display->display_options['fields']['php_2']['use_php_setup'] = 0;
$handler->display->display_options['fields']['php_2']['php_output'] = '<?php
global $user;

switch ($data->node_type) {
	case \'purchase_receipt\':
		if ($data->field_field_amount_paid[0][\'raw\'][\'value\'] < 0)
		print \'<a href="/\'.$data->field_field_buyer[0][\'rendered\'][\'#uri\'][\'path\'].\'">\'.$data->field_field_buyer[0][\'rendered\'][\'#label\'].\'</a>\';
		else {

			if (!isset($data->field_field_receipt_project[0])) {
				print t(\'Project no longer available Rovas\');
			}
			else {
				$title = isset($data->field_field_receipt_project[0][\'rendered\'][\'#item\'][\'entity\']->title_field[$user->language][0][\'value\']) ? $data->field_field_receipt_project[0][\'rendered\'][\'#item\'][\'entity\']->title_field[$user->language][0][\'value\'] : $data->field_field_receipt_project[0][\'rendered\'][\'#label\'];
				print t(\'Project\') . \': <a href="/\'.$data->field_field_receipt_project[0][\'rendered\'][\'#uri\'][\'path\'].\'">\' . $title . \'</a>\';
			}
			}
	break;
	case \'money_transfer\':
		print \'<a href="/user/\'.$data->_field_data[\'users_field_data_field_recipient_uid\'][\'entity\']->uid.\'">\'.$data->_field_data[\'users_field_data_field_recipient_uid\'][\'entity\']->name.\'</a>\';	break;
	case \'forex_transaction\':
		print \'<a href="/user/\'.$data->field_field_buyer[0][\'raw\'][\'target_id\'].\'">\'.$data->field_field_buyer[0][\'rendered\'][\'#label\'].\'</a>\';
	break;
	case \'chron_adjustment\':
		print \'<a href="/user/\'.$data->_field_data[\'users_node_uid\'][\'entity\']->uid.\'">\'.$data->_field_data[\'users_node_uid\'][\'entity\']->name.\'</a>\';
	break;
	default: //work report
		print \'<a href="/user/\'.$data->_field_data[\'users_node_uid\'][\'entity\']->uid.\'">\'.$data->_field_data[\'users_node_uid\'][\'entity\']->name.\'</a>\';
}

?>';
$handler->display->display_options['fields']['php_2']['use_php_click_sortable'] = '0';
$handler->display->display_options['fields']['php_2']['php_click_sortable'] = '';
/* Field: switch */
$handler->display->display_options['fields']['php_4']['id'] = 'php_4';
$handler->display->display_options['fields']['php_4']['table'] = 'views';
$handler->display->display_options['fields']['php_4']['field'] = 'php';
$handler->display->display_options['fields']['php_4']['ui_name'] = 'switch';
$handler->display->display_options['fields']['php_4']['label'] = 'switch';
$handler->display->display_options['fields']['php_4']['exclude'] = TRUE;
$handler->display->display_options['fields']['php_4']['use_php_setup'] = 0;
$handler->display->display_options['fields']['php_4']['php_output'] = '<?php 

//this field is used to produce classes for styling the table rows

switch ($data->node_type) {
	case \'work_report\':
		if ((module_exists(\'rovas_chron\') && (module_load_include(\'inc\', \'rovas_chron\', \'rovas_chron.ledger\') || TRUE) && function_exists(\'rovas_chron_wr_has_grant\') && rovas_chron_wr_has_grant((int) $data->nid)))
			print \'noop\';
		else
			print \'credit\';
	break;
	case \'chron_adjustment\':
		if (isset($data->field_field_amount[0][\'raw\'][\'value\']) && $data->field_field_amount[0][\'raw\'][\'value\'] < 0)
			print \'debit\';
		else
			print \'credit\';
	break;
	case \'purchase_receipt\':
		if ($data->field_field_amount_paid[0][\'raw\'][\'value\'] < 0)
			print \'credit\';
		else
			print \'debit\';
	break;
	case \'money_transfer\':
		if ($data->users_field_data_field_recipient_uid == $data->node_uid) //recipient is the same as sender
			print \'noop\';
		else if ($data->users_field_data_field_recipient_uid == arg(1)) 
			print \'credit\';
		else
			print \'debit\';
	break;
	case \'asset_usage_record\':
			print \'debit\';
	break;
	case \'forex_transaction\':
		if ($data->field_field_conversion_type[0][\'raw\'][\'value\'] == \'chrons_to_euros\')
			print \'debit\';
		else
			print \'credit\';
	break;
	default:
		print \'credit\';
}

?>';
$handler->display->display_options['fields']['php_4']['use_php_click_sortable'] = '0';
$handler->display->display_options['fields']['php_4']['php_click_sortable'] = '';
/* Field: Content: Time worked */
$handler->display->display_options['fields']['field_hours_worked']['id'] = 'field_hours_worked';
$handler->display->display_options['fields']['field_hours_worked']['table'] = 'field_data_field_hours_worked';
$handler->display->display_options['fields']['field_hours_worked']['field'] = 'field_hours_worked';
$handler->display->display_options['fields']['field_hours_worked']['label'] = '';
$handler->display->display_options['fields']['field_hours_worked']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_hours_worked']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_hours_worked']['settings'] = array(
  'thousand_separator' => '',
  'decimal_separator' => '.',
  'scale' => '2',
  'prefix_suffix' => 0,
);
/* Field: Content: Paid amount */
$handler->display->display_options['fields']['field_input_amount']['id'] = 'field_input_amount';
$handler->display->display_options['fields']['field_input_amount']['table'] = 'field_data_field_input_amount';
$handler->display->display_options['fields']['field_input_amount']['field'] = 'field_input_amount';
$handler->display->display_options['fields']['field_input_amount']['label'] = '';
$handler->display->display_options['fields']['field_input_amount']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_input_amount']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_input_amount']['settings'] = array(
  'thousand_separator' => '',
  'decimal_separator' => '.',
  'scale' => '2',
  'prefix_suffix' => 0,
);
/* Field: Content: Received amount */
$handler->display->display_options['fields']['field_output_amount']['id'] = 'field_output_amount';
$handler->display->display_options['fields']['field_output_amount']['table'] = 'field_data_field_output_amount';
$handler->display->display_options['fields']['field_output_amount']['field'] = 'field_output_amount';
$handler->display->display_options['fields']['field_output_amount']['label'] = '';
$handler->display->display_options['fields']['field_output_amount']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_output_amount']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_output_amount']['settings'] = array(
  'thousand_separator' => '',
  'decimal_separator' => '.',
  'scale' => '2',
  'prefix_suffix' => 0,
);
/* Field: php */
$handler->display->display_options['fields']['php_1']['id'] = 'php_1';
$handler->display->display_options['fields']['php_1']['table'] = 'views';
$handler->display->display_options['fields']['php_1']['field'] = 'php';
$handler->display->display_options['fields']['php_1']['ui_name'] = 'php';
$handler->display->display_options['fields']['php_1']['label'] = 'Amount';
$handler->display->display_options['fields']['php_1']['use_php_setup'] = 0;
$handler->display->display_options['fields']['php_1']['php_output'] = '<?php
//this field is used to produce classes for styling the table rows
switch ($data->node_type) {
	case \'work_report\':
		if ((module_exists(\'rovas_chron\') && (module_load_include(\'inc\', \'rovas_chron\', \'rovas_chron.ledger\') || TRUE) && function_exists(\'rovas_chron_wr_has_grant\') && rovas_chron_wr_has_grant((int) $data->nid)))
			$xfer_type = \'noop\';
		else
			$xfer_type = \'credit\';
	break;
	case \'chron_adjustment\':
		if (isset($data->field_field_amount[0][\'raw\'][\'value\']) && $data->field_field_amount[0][\'raw\'][\'value\'] < 0)
			$xfer_type = \'debit\';
		else
			$xfer_type = \'credit\';
	break;
	case \'purchase_receipt\':
		if ($data->field_field_amount_paid[0][\'raw\'][\'value\'] < 0)
			$xfer_type = \'credit\';
		else
			$xfer_type = \'debit\';
	break;
	case \'money_transfer\':
		if ($data->users_field_data_field_recipient_uid == $data->node_uid) //recipient is the same as sender
			$xfer_type = \'noop\';
		else if ($data->users_field_data_field_recipient_uid == arg(1)) 
			$xfer_type = \'credit\';
		else
			$xfer_type = \'debit\';
	break;
	case \'asset_usage_record\':
			$xfer_type = \'debit\';
	break;
	case \'forex_transaction\':
		if ($data->field_field_conversion_type[0][\'raw\'][\'value\'] == \'chrons_to_euros\')
			$xfer_type = \'debit\';
		else
			$xfer_type = \'credit\';
	break;
	default:
		$xfer_type = \'credit\';
}

//construct the sign
if ($xfer_type == \'debit\' || 
(isset($data->field_field_conversion_type[0][\'rendered\'][\'#markup\']) && $data->field_field_conversion_type[0][\'rendered\'][\'#markup\'] == \'chrons_to_euros\')
)
	$sign = \'-\';
else
	$sign = \'\';

//money sent by others to "me"
if(isset($data->field_field_amount[0][\'raw\'][\'value\'])) {
	$__amt = $data->field_field_amount[0][\'raw\'][\'value\'];
	// chron_adjustment: signed amount (neg = debit); money_transfer reimbursements flip
	if ($data->node_type == \'chron_adjustment\') {
		print ($__amt < 0 ? \'-\' : \'\') . \' \' . abs($__amt);
	}
	else if ($__amt < 0) {
		print -1 * $__amt;
	}
	else {
		print $sign . \' \' . $__amt;
	}
}
//money "I" paid for purchases
else if (isset($data->field_field_amount_paid[0][\'raw\'][\'value\'])) {
	print $sign . \' \' . $data->field_field_amount_paid[0][\'raw\'][\'value\'];
}
//money "I" paid to get Euros
else if (isset($data->field_field_conversion_type[0][\'raw\'][\'value\']) && $data->field_field_conversion_type[0][\'raw\'][\'value\'] == \'chrons_to_euros\') {
	print $sign . \' \' . $data->field_field_input_amount[0][\'raw\'][\'value\'];
}
//Euros "I" paid to get Chrons
else if (isset($data->field_field_conversion_type[0][\'raw\'][\'value\']) && $data->field_field_conversion_type[0][\'raw\'][\'value\'] == \'euros_to_chrons\') {
	print $sign . \' \' . $data->field_field_output_amount[0][\'raw\'][\'value\'];
}
//money "I" received for work
else {
	if ($xfer_type == \'noop\') {
		print \'0\';
	}
	else {
		print $sign . \' \' . $data->field_field_chrons_to_grant[0][\'rendered\'][\'#markup\'];
		if (isset($data->field_field_verification_status[0][\'raw\'][\'value\']) && $data->field_field_verification_status[0][\'raw\'][\'value\'] == 0)
			print \'&nbsp;(\' . $data->field_field_hours_worked[0][\'rendered\'][\'#markup\'] * 10 . \'*)\';
	}
}
print \' CH\';
?>';
$handler->display->display_options['fields']['php_1']['use_php_click_sortable'] = '0';
$handler->display->display_options['fields']['php_1']['php_click_sortable'] = '';
/* Sort criterion: Content: Post date */
$handler->display->display_options['sorts']['created']['id'] = 'created';
$handler->display->display_options['sorts']['created']['table'] = 'node';
$handler->display->display_options['sorts']['created']['field'] = 'created';
$handler->display->display_options['sorts']['created']['order'] = 'DESC';
/* Contextual filter: Content: Author uid */
$handler->display->display_options['arguments']['uid']['id'] = 'uid';
$handler->display->display_options['arguments']['uid']['table'] = 'node';
$handler->display->display_options['arguments']['uid']['field'] = 'uid';
$handler->display->display_options['arguments']['uid']['default_action'] = 'default';
$handler->display->display_options['arguments']['uid']['default_argument_type'] = 'user';
$handler->display->display_options['arguments']['uid']['default_argument_options']['user'] = FALSE;
$handler->display->display_options['arguments']['uid']['summary']['number_of_records'] = '0';
$handler->display->display_options['arguments']['uid']['summary']['format'] = 'default_summary';
$handler->display->display_options['arguments']['uid']['summary_options']['items_per_page'] = '25';
/* Contextual filter: Content: Recipient (field_recipient) */
$handler->display->display_options['arguments']['field_recipient_target_id']['id'] = 'field_recipient_target_id';
$handler->display->display_options['arguments']['field_recipient_target_id']['table'] = 'field_data_field_recipient';
$handler->display->display_options['arguments']['field_recipient_target_id']['field'] = 'field_recipient_target_id';
$handler->display->display_options['arguments']['field_recipient_target_id']['default_action'] = 'default';
$handler->display->display_options['arguments']['field_recipient_target_id']['default_argument_type'] = 'user';
$handler->display->display_options['arguments']['field_recipient_target_id']['default_argument_options']['user'] = FALSE;
$handler->display->display_options['arguments']['field_recipient_target_id']['summary']['number_of_records'] = '0';
$handler->display->display_options['arguments']['field_recipient_target_id']['summary']['format'] = 'default_summary';
$handler->display->display_options['arguments']['field_recipient_target_id']['summary_options']['items_per_page'] = '25';
/* Contextual filter: Content: Buyer (field_buyer) */
$handler->display->display_options['arguments']['field_buyer_target_id']['id'] = 'field_buyer_target_id';
$handler->display->display_options['arguments']['field_buyer_target_id']['table'] = 'field_data_field_buyer';
$handler->display->display_options['arguments']['field_buyer_target_id']['field'] = 'field_buyer_target_id';
$handler->display->display_options['arguments']['field_buyer_target_id']['default_action'] = 'default';
$handler->display->display_options['arguments']['field_buyer_target_id']['default_argument_type'] = 'user';
$handler->display->display_options['arguments']['field_buyer_target_id']['default_argument_options']['user'] = FALSE;
$handler->display->display_options['arguments']['field_buyer_target_id']['summary']['number_of_records'] = '0';
$handler->display->display_options['arguments']['field_buyer_target_id']['summary']['format'] = 'default_summary';
$handler->display->display_options['arguments']['field_buyer_target_id']['summary_options']['items_per_page'] = '25';
/* Filter criterion: Content: Published status */
$handler->display->display_options['filters']['status']['id'] = 'status';
$handler->display->display_options['filters']['status']['table'] = 'node';
$handler->display->display_options['filters']['status']['field'] = 'status';
$handler->display->display_options['filters']['status']['value'] = 1;
$handler->display->display_options['filters']['status']['group'] = 1;
$handler->display->display_options['filters']['status']['expose']['operator'] = FALSE;
/* Filter criterion: Content: Type (translated) */
$handler->display->display_options['filters']['type_i18n']['id'] = 'type_i18n';
$handler->display->display_options['filters']['type_i18n']['table'] = 'node';
$handler->display->display_options['filters']['type_i18n']['field'] = 'type_i18n';
$handler->display->display_options['filters']['type_i18n']['value'] = array(
  'asset_usage_record' => 'asset_usage_record',
  'chron_adjustment' => 'chron_adjustment',
  'forex_transaction' => 'forex_transaction',
  'money_transfer' => 'money_transfer',
  'purchase_receipt' => 'purchase_receipt',
  'purchase_merits' => 'purchase_merits',
  'work_report' => 'work_report',
);
$handler->display->display_options['filters']['type_i18n']['group'] = 1;
$handler->display->display_options['filters']['type_i18n']['expose']['operator_id'] = 'type_i18n_op';
$handler->display->display_options['filters']['type_i18n']['expose']['label'] = 'Type (translated)';
$handler->display->display_options['filters']['type_i18n']['expose']['operator'] = 'type_i18n_op';
$handler->display->display_options['filters']['type_i18n']['expose']['identifier'] = 'type_i18n';
$handler->display->display_options['filters']['type_i18n']['expose']['remember_roles'] = array(
  2 => '2',
);

/* Display: Page */
$handler = $view->new_display('page', 'Page', 'page');
$handler->display->display_options['defaults']['header'] = FALSE;
/* Header: Global: PHP */
$handler->display->display_options['header']['php']['id'] = 'php';
$handler->display->display_options['header']['php']['table'] = 'views';
$handler->display->display_options['header']['php']['field'] = 'php';
$handler->display->display_options['header']['php']['php_output'] = '<?php
/*
global $user;
$viewed_uid = arg(1) ? arg(1) : 0;
if ($user->uid == $viewed_uid) {
	echo \'<div class="send-ch-text">\';
	echo t(\'Clicking the button below will open a form, which allows you to send Chrons to another person.\');
	echo \'</div><div class="send-ch-button">\';
	echo \'<a href="/node/add/money-transfer" class="btn btn-primary" role="button">\' . t(\'Send Chrons\') . \'</a>\';
	echo \'</div>\';
}
echo \'<div class="ldgr-legend">\';
echo \'<div class="legend-title">\';
echo t(\'Legend\');
echo \'</div>\';
echo \'<div class="color debit">\';
echo \'&nbsp;\';
echo \'</div>\';
echo \'<div class="label">\';
echo t(\'Debits\');
echo \'</div>\';
echo \'<div class="color credit">\';
echo \'&nbsp;\';
echo \'</div>\';
echo \'<div class="label">\';
echo t(\'Credits\');
echo \'</div>\';
echo \'<div class="color noop">\';
echo \'&nbsp;\';
echo \'</div>\';
echo \'<div class="label">\';
echo t(\'No operation (circular)\');
echo \'</div>\';
echo \'</div>\';
*/
?>';
$handler->display->display_options['defaults']['fields'] = FALSE;
/* Field: Content: Amount */
$handler->display->display_options['fields']['field_amount']['id'] = 'field_amount';
$handler->display->display_options['fields']['field_amount']['table'] = 'field_data_field_amount';
$handler->display->display_options['fields']['field_amount']['field'] = 'field_amount';
$handler->display->display_options['fields']['field_amount']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_amount']['settings'] = array(
  'thousand_separator' => ' ',
  'decimal_separator' => '.',
  'scale' => '2',
  'prefix_suffix' => 0,
);
/* Field: Purchase Amount */
$handler->display->display_options['fields']['field_amount_paid']['id'] = 'field_amount_paid';
$handler->display->display_options['fields']['field_amount_paid']['table'] = 'field_data_field_amount_paid';
$handler->display->display_options['fields']['field_amount_paid']['field'] = 'field_amount_paid';
$handler->display->display_options['fields']['field_amount_paid']['ui_name'] = 'Purchase Amount';
$handler->display->display_options['fields']['field_amount_paid']['label'] = '';
$handler->display->display_options['fields']['field_amount_paid']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_amount_paid']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_amount_paid']['settings'] = array(
  'thousand_separator' => '',
  'decimal_separator' => '.',
  'scale' => '2',
  'prefix_suffix' => 0,
);
/* Field: Content: Reward */
$handler->display->display_options['fields']['field_chrons_to_grant']['id'] = 'field_chrons_to_grant';
$handler->display->display_options['fields']['field_chrons_to_grant']['table'] = 'field_data_field_chrons_to_grant';
$handler->display->display_options['fields']['field_chrons_to_grant']['field'] = 'field_chrons_to_grant';
$handler->display->display_options['fields']['field_chrons_to_grant']['label'] = '';
$handler->display->display_options['fields']['field_chrons_to_grant']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_chrons_to_grant']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_chrons_to_grant']['settings'] = array(
  'thousand_separator' => '',
  'decimal_separator' => '.',
  'scale' => '2',
  'prefix_suffix' => 0,
);
/* Field: Content: Verification status */
$handler->display->display_options['fields']['field_verification_status']['id'] = 'field_verification_status';
$handler->display->display_options['fields']['field_verification_status']['table'] = 'field_data_field_verification_status';
$handler->display->display_options['fields']['field_verification_status']['field'] = 'field_verification_status';
$handler->display->display_options['fields']['field_verification_status']['label'] = '';
$handler->display->display_options['fields']['field_verification_status']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_verification_status']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_verification_status']['type'] = 'list_key';
$handler->display->display_options['fields']['field_verification_status']['settings'] = array(
  'thousand_separator' => '',
  'prefix_suffix' => 0,
);
/* Field: Content: Buyer */
$handler->display->display_options['fields']['field_buyer']['id'] = 'field_buyer';
$handler->display->display_options['fields']['field_buyer']['table'] = 'field_data_field_buyer';
$handler->display->display_options['fields']['field_buyer']['field'] = 'field_buyer';
$handler->display->display_options['fields']['field_buyer']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_buyer']['settings'] = array(
  'bypass_access' => 0,
  'link' => 1,
);
/* Field: User: Full name */
$handler->display->display_options['fields']['field_full_name_2']['id'] = 'field_full_name_2';
$handler->display->display_options['fields']['field_full_name_2']['table'] = 'field_data_field_full_name';
$handler->display->display_options['fields']['field_full_name_2']['field'] = 'field_full_name';
$handler->display->display_options['fields']['field_full_name_2']['relationship'] = 'field_buyer_target_id';
$handler->display->display_options['fields']['field_full_name_2']['exclude'] = TRUE;
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
$handler->display->display_options['fields']['name']['label'] = '';
$handler->display->display_options['fields']['name']['exclude'] = TRUE;
$handler->display->display_options['fields']['name']['element_label_colon'] = FALSE;
/* Field: User: Full name */
$handler->display->display_options['fields']['field_full_name']['id'] = 'field_full_name';
$handler->display->display_options['fields']['field_full_name']['table'] = 'field_data_field_full_name';
$handler->display->display_options['fields']['field_full_name']['field'] = 'field_full_name';
$handler->display->display_options['fields']['field_full_name']['relationship'] = 'uid';
$handler->display->display_options['fields']['field_full_name']['label'] = 'Sender Full name';
$handler->display->display_options['fields']['field_full_name']['exclude'] = TRUE;
/* Field: Content: Project */
$handler->display->display_options['fields']['field_receipt_project']['id'] = 'field_receipt_project';
$handler->display->display_options['fields']['field_receipt_project']['table'] = 'field_data_field_receipt_project';
$handler->display->display_options['fields']['field_receipt_project']['field'] = 'field_receipt_project';
$handler->display->display_options['fields']['field_receipt_project']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_receipt_project']['settings'] = array(
  'bypass_access' => 0,
  'link' => 1,
);
/* Field: User: Name */
$handler->display->display_options['fields']['name_1']['id'] = 'name_1';
$handler->display->display_options['fields']['name_1']['table'] = 'users';
$handler->display->display_options['fields']['name_1']['field'] = 'name';
$handler->display->display_options['fields']['name_1']['relationship'] = 'field_recipient_target_id';
$handler->display->display_options['fields']['name_1']['label'] = '';
$handler->display->display_options['fields']['name_1']['exclude'] = TRUE;
$handler->display->display_options['fields']['name_1']['element_label_colon'] = FALSE;
/* Field: User: Full name */
$handler->display->display_options['fields']['field_full_name_1']['id'] = 'field_full_name_1';
$handler->display->display_options['fields']['field_full_name_1']['table'] = 'field_data_field_full_name';
$handler->display->display_options['fields']['field_full_name_1']['field'] = 'field_full_name';
$handler->display->display_options['fields']['field_full_name_1']['relationship'] = 'field_recipient_target_id';
$handler->display->display_options['fields']['field_full_name_1']['label'] = 'Recipient';
$handler->display->display_options['fields']['field_full_name_1']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_full_name_1']['alter']['text'] = '[field_full_name_1] ([name_1])';
/* Field: Content: Type */
$handler->display->display_options['fields']['type']['id'] = 'type';
$handler->display->display_options['fields']['type']['table'] = 'node';
$handler->display->display_options['fields']['type']['field'] = 'type';
$handler->display->display_options['fields']['type']['exclude'] = TRUE;
$handler->display->display_options['fields']['type']['machine_name'] = TRUE;
/* Field: Content: Post date */
$handler->display->display_options['fields']['created']['id'] = 'created';
$handler->display->display_options['fields']['created']['table'] = 'node';
$handler->display->display_options['fields']['created']['field'] = 'created';
$handler->display->display_options['fields']['created']['label'] = 'Date';
$handler->display->display_options['fields']['created']['date_format'] = 'short_timestamp_with_spelled_month';
$handler->display->display_options['fields']['created']['second_date_format'] = 'privatemsg_current_day';
/* Field: Entity translation: Title: translated */
$handler->display->display_options['fields']['title_field_et']['id'] = 'title_field_et';
$handler->display->display_options['fields']['title_field_et']['table'] = 'field_data_title_field';
$handler->display->display_options['fields']['title_field_et']['field'] = 'title_field_et';
$handler->display->display_options['fields']['title_field_et']['label'] = 'Title';
$handler->display->display_options['fields']['title_field_et']['exclude'] = TRUE;
$handler->display->display_options['fields']['title_field_et']['type'] = 'title_linked';
$handler->display->display_options['fields']['title_field_et']['settings'] = array(
  'title_style' => '_none',
  'title_link' => 'content',
  'enable_trim' => 0,
  'trim_length' => '50',
  'title_class' => '',
);
/* Field: Content: Title */
$handler->display->display_options['fields']['title']['id'] = 'title';
$handler->display->display_options['fields']['title']['table'] = 'node';
$handler->display->display_options['fields']['title']['field'] = 'title';
$handler->display->display_options['fields']['title']['label'] = 'ID';
$handler->display->display_options['fields']['title']['exclude'] = TRUE;
/* Field: Content: Conversion type */
$handler->display->display_options['fields']['field_conversion_type']['id'] = 'field_conversion_type';
$handler->display->display_options['fields']['field_conversion_type']['table'] = 'field_data_field_conversion_type';
$handler->display->display_options['fields']['field_conversion_type']['field'] = 'field_conversion_type';
$handler->display->display_options['fields']['field_conversion_type']['label'] = '';
$handler->display->display_options['fields']['field_conversion_type']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_conversion_type']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_conversion_type']['type'] = 'list_key';
/* Field: transaction type */
$handler->display->display_options['fields']['php_3']['id'] = 'php_3';
$handler->display->display_options['fields']['php_3']['table'] = 'views';
$handler->display->display_options['fields']['php_3']['field'] = 'php';
$handler->display->display_options['fields']['php_3']['ui_name'] = 'transaction type';
$handler->display->display_options['fields']['php_3']['label'] = 'Type';
$handler->display->display_options['fields']['php_3']['use_php_setup'] = 0;
$handler->display->display_options['fields']['php_3']['php_output'] = '<?php

switch ($data->node_type) {
	case \'money_transfer\':
		print t(\'transaction\') . \' <a href="/node/\'.$data->_field_data[\'nid\'][\'entity\']->nid.\'">\'.$data->_field_data[\'nid\'][\'entity\']->title.\'</a>\';
	break;
	case \'chron_adjustment\':
		print t(\'adjustment\') . \' <a href="/node/\'.$data->_field_data[\'nid\'][\'entity\']->nid.\'">\'.$data->_field_data[\'nid\'][\'entity\']->title.\'</a>\';
	break;
	case \'purchase_receipt\':
		if ($data->field_field_amount_paid < 0)
			print t(\'cancel. purchase\') . \' <a href="/node/\'.$data->_field_data[\'nid\'][\'entity\']->nid.\'">\'.$data->_field_data[\'nid\'][\'entity\']->title.\'</a>\';
		else
			print t(\'purchase\') . \' <a href="/node/\'.$data->_field_data[\'nid\'][\'entity\']->nid.\'">\'.$data->_field_data[\'nid\'][\'entity\']->title.\'</a>\';
	break;
	case \'purchase_merits\':
		if ($data->field_field_amount_paid < 0)
			print t(\'cancel. purchase\') . \' <a href="/node/\'.$data->_field_data[\'nid\'][\'entity\']->nid.\'">\'.$data->_field_data[\'nid\'][\'entity\']->title.\'</a>\';
		else
			print t(\'Merits purchase\') . \' <a href="/node/\'.$data->_field_data[\'nid\'][\'entity\']->nid.\'">\'.$data->_field_data[\'nid\'][\'entity\']->title.\'</a>\';
	break;
	case \'forex_transaction\':
		if ($data->field_field_conversion_type[0][\'rendered\'][\'#markup\'] == \'chrons_to_euros\')
			print t(\'Chrons sale\') . \' <a href="/node/\'.$data->_field_data[\'nid\'][\'entity\']->nid.\'">\'.$data->_field_data[\'nid\'][\'entity\']->title.\'</a>\';
		else
			print t(\'Chrons purchase\') . \' <a href="/node/\'.$data->_field_data[\'nid\'][\'entity\']->nid.\'">\'.$data->_field_data[\'nid\'][\'entity\']->title.\'</a>\';
	break;
	default:


		print t(\'work\') . \' <a href="/node/\'.$data->_field_data[\'nid\'][\'entity\']->nid.\'">\'.$data->field_title_field_et[0][\'rendered\'][\'#markup\'].\'</a>\';
}

?>';
$handler->display->display_options['fields']['php_3']['use_php_click_sortable'] = '0';
$handler->display->display_options['fields']['php_3']['php_click_sortable'] = '';
/* Field: php */
$handler->display->display_options['fields']['php']['id'] = 'php';
$handler->display->display_options['fields']['php']['table'] = 'views';
$handler->display->display_options['fields']['php']['field'] = 'php';
$handler->display->display_options['fields']['php']['ui_name'] = 'php';
$handler->display->display_options['fields']['php']['label'] = 'Sender';
$handler->display->display_options['fields']['php']['use_php_setup'] = 0;
$handler->display->display_options['fields']['php']['php_output'] = '<?php 

switch ($data->node_type) {
	case \'purchase_receipt\':
		if ($data->field_field_amount_paid[0][\'raw\'][\'value\'] < 0)
			print t(\'Project\') . \': <a href="\'.$data->field_field_receipt_project[0][\'rendered\'][\'#item\'][\'target_id\'].\'">\' . $data->field_field_receipt_project[0][\'rendered\'][\'#label\'] . \'</a>\';
		else
			print \'<a href="/user/\'.$data->field_field_buyer[0][\'raw\'][\'target_id\'].\'">\'.$data->field_field_buyer[0][\'rendered\'][\'#label\'].\'</a>\';
	break;
	case \'purchase_merits\':
		print \'<a href="/user/\'.$data->field_field_buyer[0][\'raw\'][\'target_id\'].\'">\'.$data->field_field_buyer[0][\'rendered\'][\'#label\'].\'</a>\';
	break;
	case \'money_transfer\':
		print $data->users_node_uid == \'0\' ? t(\'Anonymous\') : \'<a href="/user/\'.$data->users_node_uid.\'">\'.$data->users_node_name.\'</a>\';
	break;
	case \'forex_transaction\':
		print \'<a href="/user/\'.$data->field_field_buyer[0][\'raw\'][\'target_id\'].\'">\'.$data->field_field_buyer[0][\'rendered\'][\'#label\'].\'</a>\';
	break;
	case \'chron_adjustment\':
		print \'Rovas\';
	break;
	default: //work report
		print \'Rovas\';
}
?>';
$handler->display->display_options['fields']['php']['use_php_click_sortable'] = '0';
$handler->display->display_options['fields']['php']['php_click_sortable'] = '';
/* Field: php */
$handler->display->display_options['fields']['php_2']['id'] = 'php_2';
$handler->display->display_options['fields']['php_2']['table'] = 'views';
$handler->display->display_options['fields']['php_2']['field'] = 'php';
$handler->display->display_options['fields']['php_2']['ui_name'] = 'php';
$handler->display->display_options['fields']['php_2']['label'] = 'Recipient';
$handler->display->display_options['fields']['php_2']['use_php_setup'] = 0;
$handler->display->display_options['fields']['php_2']['php_output'] = '<?php
global $user;

switch ($data->node_type) {
	case \'purchase_receipt\':
		if ($data->field_field_amount_paid[0][\'raw\'][\'value\'] < 0)
		print \'<a href="/\'.$data->field_field_buyer[0][\'rendered\'][\'#uri\'][\'path\'].\'">\'.$data->field_field_buyer[0][\'rendered\'][\'#label\'].\'</a>\';
		else {

			if (!isset($data->field_field_receipt_project[0])) {
				print t(\'Project no longer available Rovas\');
			}
			else {
				$title = isset($data->field_field_receipt_project[0][\'rendered\'][\'#item\'][\'entity\']->title_field[$user->language][0][\'value\']) ? $data->field_field_receipt_project[0][\'rendered\'][\'#item\'][\'entity\']->title_field[$user->language][0][\'value\'] : $data->field_field_receipt_project[0][\'rendered\'][\'#label\'];
				print t(\'Project\') . \': <a href="/\'.$data->field_field_receipt_project[0][\'rendered\'][\'#uri\'][\'path\'].\'">\' . $title . \'</a>\';
			}
			}
	break;
	case \'money_transfer\':
		print \'<a href="/user/\'.$data->_field_data[\'users_field_data_field_recipient_uid\'][\'entity\']->uid.\'">\'.$data->_field_data[\'users_field_data_field_recipient_uid\'][\'entity\']->name.\'</a>\';	break;
	case \'forex_transaction\':
		print \'<a href="/user/\'.$data->field_field_buyer[0][\'raw\'][\'target_id\'].\'">\'.$data->field_field_buyer[0][\'rendered\'][\'#label\'].\'</a>\';
	break;
	case \'chron_adjustment\':
		print \'<a href="/user/\'.$data->users_node_uid.\'">\'.$data->users_node_name.\'</a>\';
	break;
	default: //work report
		print \'<a href="/user/\'.$data->users_node_uid.\'">\'.$data->users_node_name.\'</a>\';
}

?>';
$handler->display->display_options['fields']['php_2']['use_php_click_sortable'] = '0';
$handler->display->display_options['fields']['php_2']['php_click_sortable'] = '';
/* Field: switch */
$handler->display->display_options['fields']['php_4']['id'] = 'php_4';
$handler->display->display_options['fields']['php_4']['table'] = 'views';
$handler->display->display_options['fields']['php_4']['field'] = 'php';
$handler->display->display_options['fields']['php_4']['ui_name'] = 'switch';
$handler->display->display_options['fields']['php_4']['label'] = 'switch';
$handler->display->display_options['fields']['php_4']['exclude'] = TRUE;
$handler->display->display_options['fields']['php_4']['use_php_setup'] = 0;
$handler->display->display_options['fields']['php_4']['php_output'] = '<?php 

//this field is used to produce classes for styling the table rows

switch ($data->node_type) {
	case \'work_report\':
		if ((module_exists(\'rovas_chron\') && (module_load_include(\'inc\', \'rovas_chron\', \'rovas_chron.ledger\') || TRUE) && function_exists(\'rovas_chron_wr_has_grant\') && rovas_chron_wr_has_grant((int) $data->nid)))
			print \'noop\';
		else
			print \'credit\';
	break;
	case \'chron_adjustment\':
		if (isset($data->field_field_amount[0][\'raw\'][\'value\']) && $data->field_field_amount[0][\'raw\'][\'value\'] < 0)
			print \'debit\';
		else
			print \'credit\';
	break;
	case \'purchase_receipt\':
		if ($data->field_field_amount_paid[0][\'raw\'][\'value\'] < 0)
			print \'credit\';
		else
			print \'debit\';
	break;
	case \'money_transfer\':
		if ($data->users_field_data_field_recipient_uid == $data->node_uid) //recipient is the same as sender
			print \'noop\';
		else if ($data->users_field_data_field_recipient_uid == arg(1)) 
			print \'credit\';
		else
			print \'debit\';
	break;
	case \'asset_usage_record\':
			print \'debit\';
	break;
	case \'forex_transaction\':
		if ($data->field_field_conversion_type[0][\'raw\'][\'value\'] == \'chrons_to_euros\')
			print \'debit\';
		else
			print \'credit\';
	break;
	default:
		print \'credit\';
}

?>';
$handler->display->display_options['fields']['php_4']['use_php_click_sortable'] = '0';
$handler->display->display_options['fields']['php_4']['php_click_sortable'] = '';
/* Field: Content: Time worked */
$handler->display->display_options['fields']['field_hours_worked']['id'] = 'field_hours_worked';
$handler->display->display_options['fields']['field_hours_worked']['table'] = 'field_data_field_hours_worked';
$handler->display->display_options['fields']['field_hours_worked']['field'] = 'field_hours_worked';
$handler->display->display_options['fields']['field_hours_worked']['label'] = '';
$handler->display->display_options['fields']['field_hours_worked']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_hours_worked']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_hours_worked']['settings'] = array(
  'thousand_separator' => '',
  'decimal_separator' => '.',
  'scale' => '2',
  'prefix_suffix' => 0,
);
/* Field: Content: Paid amount */
$handler->display->display_options['fields']['field_input_amount']['id'] = 'field_input_amount';
$handler->display->display_options['fields']['field_input_amount']['table'] = 'field_data_field_input_amount';
$handler->display->display_options['fields']['field_input_amount']['field'] = 'field_input_amount';
$handler->display->display_options['fields']['field_input_amount']['label'] = '';
$handler->display->display_options['fields']['field_input_amount']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_input_amount']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_input_amount']['settings'] = array(
  'thousand_separator' => '',
  'decimal_separator' => '.',
  'scale' => '2',
  'prefix_suffix' => 0,
);
/* Field: Content: Received amount */
$handler->display->display_options['fields']['field_output_amount']['id'] = 'field_output_amount';
$handler->display->display_options['fields']['field_output_amount']['table'] = 'field_data_field_output_amount';
$handler->display->display_options['fields']['field_output_amount']['field'] = 'field_output_amount';
$handler->display->display_options['fields']['field_output_amount']['label'] = '';
$handler->display->display_options['fields']['field_output_amount']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_output_amount']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_output_amount']['settings'] = array(
  'thousand_separator' => '',
  'decimal_separator' => '.',
  'scale' => '2',
  'prefix_suffix' => 0,
);
/* Field: php */
$handler->display->display_options['fields']['php_1']['id'] = 'php_1';
$handler->display->display_options['fields']['php_1']['table'] = 'views';
$handler->display->display_options['fields']['php_1']['field'] = 'php';
$handler->display->display_options['fields']['php_1']['ui_name'] = 'php';
$handler->display->display_options['fields']['php_1']['label'] = 'Amount';
$handler->display->display_options['fields']['php_1']['use_php_setup'] = 0;
$handler->display->display_options['fields']['php_1']['php_output'] = '<?php
//this field is used to produce classes for styling the table rows
switch ($data->node_type) {
	case \'work_report\':
		if ((module_exists(\'rovas_chron\') && (module_load_include(\'inc\', \'rovas_chron\', \'rovas_chron.ledger\') || TRUE) && function_exists(\'rovas_chron_wr_has_grant\') && rovas_chron_wr_has_grant((int) $data->nid)))
			$xfer_type = \'noop\';
		else
			$xfer_type = \'credit\';
	break;
	case \'chron_adjustment\':
		if (isset($data->field_field_amount[0][\'raw\'][\'value\']) && $data->field_field_amount[0][\'raw\'][\'value\'] < 0)
			$xfer_type = \'debit\';
		else
			$xfer_type = \'credit\';
	break;
	case \'purchase_receipt\':
		if ($data->field_field_amount_paid[0][\'raw\'][\'value\'] < 0)
			$xfer_type = \'credit\';
		else
			$xfer_type = \'debit\';
	break;
	case \'purchase_merits\':
		if ($data->field_field_amount_paid[0][\'raw\'][\'value\'] < 0)
			$xfer_type = \'credit\';
		else
			$xfer_type = \'debit\';
	break;
	case \'money_transfer\':
		if ($data->users_field_data_field_recipient_uid == $data->node_uid) //recipient is the same as sender
			$xfer_type = \'noop\';
		else if ($data->users_field_data_field_recipient_uid == arg(1)) 
			$xfer_type = \'credit\';
		else
			$xfer_type = \'debit\';
	break;
	case \'asset_usage_record\':
			$xfer_type = \'debit\';
	break;
	case \'forex_transaction\':
		if ($data->field_field_conversion_type[0][\'raw\'][\'value\'] == \'chrons_to_euros\')
			$xfer_type = \'debit\';
		else
			$xfer_type = \'credit\';
	break;
	default:
		$xfer_type = \'credit\';
}

//construct the sign
if ($xfer_type == \'debit\' || 
(isset($data->field_field_conversion_type[0][\'rendered\'][\'#markup\']) && $data->field_field_conversion_type[0][\'rendered\'][\'#markup\'] == \'chrons_to_euros\')
)
	$sign = \'-\';
else
	$sign = \'\';

//money sent by others to "me"
if(isset($data->field_field_amount[0][\'raw\'][\'value\'])) {
	$__amt = $data->field_field_amount[0][\'raw\'][\'value\'];
	// chron_adjustment: signed amount (neg = debit); money_transfer reimbursements flip
	if ($data->node_type == \'chron_adjustment\') {
		print ($__amt < 0 ? \'-\' : \'\') . \' \' . abs($__amt);
	}
	else if ($__amt < 0) {
		print -1 * $__amt;
	}
	else {
		print $sign . \' \' . $__amt;
	}
}
//money "I" paid for purchases
else if (isset($data->field_field_amount_paid[0][\'raw\'][\'value\'])) {
	print $sign . \' \' . number_format($data->field_field_amount_paid[0][\'raw\'][\'value\'], 2, \'.\', \'\');
}
//money "I" paid to get Euros
else if (isset($data->field_field_conversion_type[0][\'raw\'][\'value\']) && $data->field_field_conversion_type[0][\'raw\'][\'value\'] == \'chrons_to_euros\') {
	print $sign . \' \' . $data->field_field_input_amount[0][\'raw\'][\'value\'];
}
//Euros "I" paid to get Chrons
else if (isset($data->field_field_conversion_type[0][\'raw\'][\'value\']) && $data->field_field_conversion_type[0][\'raw\'][\'value\'] == \'euros_to_chrons\') {
	print $sign . \' \' . $data->field_field_output_amount[0][\'raw\'][\'value\'];
}
//money "I" received for work
else {
	if ($xfer_type == \'noop\') {
		print \'0\';
	}
	else {
		print $sign . \' \' . $data->field_field_chrons_to_grant[0][\'rendered\'][\'#markup\'];
		if (isset($data->field_field_verification_status[0][\'raw\'][\'value\']) && $data->field_field_verification_status[0][\'raw\'][\'value\'] == 0)
			print \'&nbsp;(\' . $data->field_field_hours_worked[0][\'rendered\'][\'#markup\'] * 10 . \'*)\';
	}
}
print \' CH\';
?>';
$handler->display->display_options['fields']['php_1']['use_php_click_sortable'] = '0';
$handler->display->display_options['fields']['php_1']['php_click_sortable'] = '';
$handler->display->display_options['defaults']['filter_groups'] = FALSE;
$handler->display->display_options['defaults']['filters'] = FALSE;
/* Filter criterion: Content: Published status */
$handler->display->display_options['filters']['status']['id'] = 'status';
$handler->display->display_options['filters']['status']['table'] = 'node';
$handler->display->display_options['filters']['status']['field'] = 'status';
$handler->display->display_options['filters']['status']['value'] = 1;
$handler->display->display_options['filters']['status']['group'] = 1;
$handler->display->display_options['filters']['status']['expose']['operator'] = FALSE;
/* Filter criterion: Content: Type (translated) */
$handler->display->display_options['filters']['type_i18n']['id'] = 'type_i18n';
$handler->display->display_options['filters']['type_i18n']['table'] = 'node';
$handler->display->display_options['filters']['type_i18n']['field'] = 'type_i18n';
$handler->display->display_options['filters']['type_i18n']['value'] = array(
  'chron_adjustment' => 'chron_adjustment',
  'forex_transaction' => 'forex_transaction',
  'money_transfer' => 'money_transfer',
  'purchase_receipt' => 'purchase_receipt',
  'purchase_merits' => 'purchase_merits',
  'work_report' => 'work_report',
);
$handler->display->display_options['filters']['type_i18n']['group'] = 1;
$handler->display->display_options['filters']['type_i18n']['expose']['operator_id'] = 'type_i18n_op';
$handler->display->display_options['filters']['type_i18n']['expose']['label'] = 'Type (translated)';
$handler->display->display_options['filters']['type_i18n']['expose']['operator'] = 'type_i18n_op';
$handler->display->display_options['filters']['type_i18n']['expose']['identifier'] = 'type_i18n';
$handler->display->display_options['filters']['type_i18n']['expose']['remember_roles'] = array(
  2 => '2',
);
$handler->display->display_options['path'] = 'user/%/chrons';
$handler->display->display_options['menu']['type'] = 'tab';
$handler->display->display_options['menu']['title'] = 'Chrons';
$handler->display->display_options['menu']['weight'] = '0';
$handler->display->display_options['menu']['name'] = 'user-menu';
$handler->display->display_options['menu']['context'] = 0;
$handler->display->display_options['menu']['context_only_inline'] = 0;

/* Display: Sum Chrons */
$handler = $view->new_display('page', 'Sum Chrons', 'page_1');
$handler->display->display_options['defaults']['access'] = FALSE;
$handler->display->display_options['access']['type'] = 'php';
$handler->display->display_options['access']['php_access'] = 'global $user;
//return ($user->uid == 1) || (arg(1) == $user->uid);
return ($user->uid == 1)';
$handler->display->display_options['defaults']['pager'] = FALSE;
$handler->display->display_options['pager']['type'] = 'none';
$handler->display->display_options['pager']['options']['offset'] = '0';
$handler->display->display_options['defaults']['style_plugin'] = FALSE;
$handler->display->display_options['style_plugin'] = 'tablesummarized';
$handler->display->display_options['style_options']['default_row_class'] = FALSE;
$handler->display->display_options['style_options']['row_class_special'] = FALSE;
$handler->display->display_options['style_options']['columns'] = array(
  'field_amount' => 'field_amount',
  'field_amount_paid' => 'field_amount_paid',
  'field_chrons_to_grant' => 'field_chrons_to_grant',
  'field_verification_status' => 'field_verification_status',
  'field_buyer' => 'field_buyer',
  'field_full_name_2' => 'field_full_name_2',
  'uid' => 'uid',
  'name' => 'name',
  'field_full_name' => 'field_full_name',
  'field_receipt_project' => 'field_receipt_project',
  'name_1' => 'name_1',
  'field_full_name_1' => 'field_full_name_1',
  'type' => 'type',
  'created' => 'created',
  'title' => 'title',
  'field_conversion_type' => 'field_conversion_type',
  'php_3' => 'php_3',
  'php' => 'php',
  'php_2' => 'php_2',
  'php_4' => 'php_4',
  'field_hours_worked' => 'field_hours_worked',
  'field_input_amount' => 'field_input_amount',
  'field_output_amount' => 'field_output_amount',
  'php_1' => 'php_1',
);
$handler->display->display_options['style_options']['class'] = '';
$handler->display->display_options['style_options']['default'] = '-1';
$handler->display->display_options['style_options']['info'] = array(
  'field_amount' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
    'summarize' => 'none',
  ),
  'field_amount_paid' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
    'summarize' => 'none',
  ),
  'field_chrons_to_grant' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
    'summarize' => 'none',
  ),
  'field_verification_status' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
    'summarize' => 'none',
  ),
  'field_buyer' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
    'summarize' => 'none',
  ),
  'field_full_name_2' => array(
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
  'field_receipt_project' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
    'summarize' => 'none',
  ),
  'name_1' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
    'summarize' => 'none',
  ),
  'field_full_name_1' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
    'summarize' => 'none',
  ),
  'type' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
    'summarize' => 'none',
  ),
  'created' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
    'summarize' => 'none',
  ),
  'title' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
    'summarize' => 'none',
  ),
  'field_conversion_type' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
    'summarize' => 'none',
  ),
  'php_3' => array(
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
  'php_2' => array(
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
    'summarize' => 'none',
  ),
  'php_4' => array(
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
    'summarize' => 'none',
  ),
  'field_hours_worked' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
    'summarize' => 'none',
  ),
  'field_input_amount' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
    'summarize' => 'none',
  ),
  'field_output_amount' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
    'summarize' => 'none',
  ),
  'php_1' => array(
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
    'summarize' => 'total',
  ),
);
$handler->display->display_options['defaults']['style_options'] = FALSE;
$handler->display->display_options['defaults']['row_plugin'] = FALSE;
$handler->display->display_options['defaults']['row_options'] = FALSE;
$handler->display->display_options['defaults']['header'] = FALSE;
$handler->display->display_options['defaults']['footer'] = FALSE;
$handler->display->display_options['defaults']['fields'] = FALSE;
/* Field: Content: Amount */
$handler->display->display_options['fields']['field_amount']['id'] = 'field_amount';
$handler->display->display_options['fields']['field_amount']['table'] = 'field_data_field_amount';
$handler->display->display_options['fields']['field_amount']['field'] = 'field_amount';
$handler->display->display_options['fields']['field_amount']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_amount']['settings'] = array(
  'thousand_separator' => ' ',
  'decimal_separator' => '.',
  'scale' => '2',
  'prefix_suffix' => 0,
);
/* Field: Purchase Amount */
$handler->display->display_options['fields']['field_amount_paid']['id'] = 'field_amount_paid';
$handler->display->display_options['fields']['field_amount_paid']['table'] = 'field_data_field_amount_paid';
$handler->display->display_options['fields']['field_amount_paid']['field'] = 'field_amount_paid';
$handler->display->display_options['fields']['field_amount_paid']['ui_name'] = 'Purchase Amount';
$handler->display->display_options['fields']['field_amount_paid']['label'] = '';
$handler->display->display_options['fields']['field_amount_paid']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_amount_paid']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_amount_paid']['settings'] = array(
  'thousand_separator' => '',
  'decimal_separator' => '.',
  'scale' => '2',
  'prefix_suffix' => 0,
);
/* Field: Content: Reward */
$handler->display->display_options['fields']['field_chrons_to_grant']['id'] = 'field_chrons_to_grant';
$handler->display->display_options['fields']['field_chrons_to_grant']['table'] = 'field_data_field_chrons_to_grant';
$handler->display->display_options['fields']['field_chrons_to_grant']['field'] = 'field_chrons_to_grant';
$handler->display->display_options['fields']['field_chrons_to_grant']['label'] = '';
$handler->display->display_options['fields']['field_chrons_to_grant']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_chrons_to_grant']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_chrons_to_grant']['settings'] = array(
  'thousand_separator' => '',
  'decimal_separator' => '.',
  'scale' => '2',
  'prefix_suffix' => 0,
);
/* Field: Content: Verification status */
$handler->display->display_options['fields']['field_verification_status']['id'] = 'field_verification_status';
$handler->display->display_options['fields']['field_verification_status']['table'] = 'field_data_field_verification_status';
$handler->display->display_options['fields']['field_verification_status']['field'] = 'field_verification_status';
$handler->display->display_options['fields']['field_verification_status']['label'] = '';
$handler->display->display_options['fields']['field_verification_status']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_verification_status']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_verification_status']['type'] = 'list_key';
$handler->display->display_options['fields']['field_verification_status']['settings'] = array(
  'thousand_separator' => '',
  'prefix_suffix' => 0,
);
/* Field: Content: Buyer */
$handler->display->display_options['fields']['field_buyer']['id'] = 'field_buyer';
$handler->display->display_options['fields']['field_buyer']['table'] = 'field_data_field_buyer';
$handler->display->display_options['fields']['field_buyer']['field'] = 'field_buyer';
$handler->display->display_options['fields']['field_buyer']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_buyer']['settings'] = array(
  'bypass_access' => 0,
  'link' => 1,
);
/* Field: User: Full name */
$handler->display->display_options['fields']['field_full_name_2']['id'] = 'field_full_name_2';
$handler->display->display_options['fields']['field_full_name_2']['table'] = 'field_data_field_full_name';
$handler->display->display_options['fields']['field_full_name_2']['field'] = 'field_full_name';
$handler->display->display_options['fields']['field_full_name_2']['relationship'] = 'field_buyer_target_id';
$handler->display->display_options['fields']['field_full_name_2']['exclude'] = TRUE;
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
$handler->display->display_options['fields']['name']['label'] = '';
$handler->display->display_options['fields']['name']['exclude'] = TRUE;
$handler->display->display_options['fields']['name']['element_label_colon'] = FALSE;
/* Field: User: Full name */
$handler->display->display_options['fields']['field_full_name']['id'] = 'field_full_name';
$handler->display->display_options['fields']['field_full_name']['table'] = 'field_data_field_full_name';
$handler->display->display_options['fields']['field_full_name']['field'] = 'field_full_name';
$handler->display->display_options['fields']['field_full_name']['relationship'] = 'uid';
$handler->display->display_options['fields']['field_full_name']['label'] = 'Sender Full name';
$handler->display->display_options['fields']['field_full_name']['exclude'] = TRUE;
/* Field: Content: Project */
$handler->display->display_options['fields']['field_receipt_project']['id'] = 'field_receipt_project';
$handler->display->display_options['fields']['field_receipt_project']['table'] = 'field_data_field_receipt_project';
$handler->display->display_options['fields']['field_receipt_project']['field'] = 'field_receipt_project';
$handler->display->display_options['fields']['field_receipt_project']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_receipt_project']['settings'] = array(
  'bypass_access' => 0,
  'link' => 1,
);
/* Field: User: Name */
$handler->display->display_options['fields']['name_1']['id'] = 'name_1';
$handler->display->display_options['fields']['name_1']['table'] = 'users';
$handler->display->display_options['fields']['name_1']['field'] = 'name';
$handler->display->display_options['fields']['name_1']['relationship'] = 'field_recipient_target_id';
$handler->display->display_options['fields']['name_1']['label'] = '';
$handler->display->display_options['fields']['name_1']['exclude'] = TRUE;
$handler->display->display_options['fields']['name_1']['element_label_colon'] = FALSE;
/* Field: User: Full name */
$handler->display->display_options['fields']['field_full_name_1']['id'] = 'field_full_name_1';
$handler->display->display_options['fields']['field_full_name_1']['table'] = 'field_data_field_full_name';
$handler->display->display_options['fields']['field_full_name_1']['field'] = 'field_full_name';
$handler->display->display_options['fields']['field_full_name_1']['relationship'] = 'field_recipient_target_id';
$handler->display->display_options['fields']['field_full_name_1']['label'] = 'Recipient';
$handler->display->display_options['fields']['field_full_name_1']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_full_name_1']['alter']['text'] = '[field_full_name_1] ([name_1])';
/* Field: Content: Type */
$handler->display->display_options['fields']['type']['id'] = 'type';
$handler->display->display_options['fields']['type']['table'] = 'node';
$handler->display->display_options['fields']['type']['field'] = 'type';
$handler->display->display_options['fields']['type']['label'] = '';
$handler->display->display_options['fields']['type']['exclude'] = TRUE;
$handler->display->display_options['fields']['type']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['type']['machine_name'] = TRUE;
/* Field: Content: Post date */
$handler->display->display_options['fields']['created']['id'] = 'created';
$handler->display->display_options['fields']['created']['table'] = 'node';
$handler->display->display_options['fields']['created']['field'] = 'created';
$handler->display->display_options['fields']['created']['label'] = '';
$handler->display->display_options['fields']['created']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['created']['date_format'] = 'rovas_short';
$handler->display->display_options['fields']['created']['second_date_format'] = 'privatemsg_current_day';
/* Field: Content: Title */
$handler->display->display_options['fields']['title']['id'] = 'title';
$handler->display->display_options['fields']['title']['table'] = 'node';
$handler->display->display_options['fields']['title']['field'] = 'title';
$handler->display->display_options['fields']['title']['label'] = 'ID';
$handler->display->display_options['fields']['title']['exclude'] = TRUE;
/* Field: Content: Conversion type */
$handler->display->display_options['fields']['field_conversion_type']['id'] = 'field_conversion_type';
$handler->display->display_options['fields']['field_conversion_type']['table'] = 'field_data_field_conversion_type';
$handler->display->display_options['fields']['field_conversion_type']['field'] = 'field_conversion_type';
$handler->display->display_options['fields']['field_conversion_type']['label'] = '';
$handler->display->display_options['fields']['field_conversion_type']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_conversion_type']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_conversion_type']['type'] = 'list_key';
/* Field: transaction type */
$handler->display->display_options['fields']['php_3']['id'] = 'php_3';
$handler->display->display_options['fields']['php_3']['table'] = 'views';
$handler->display->display_options['fields']['php_3']['field'] = 'php';
$handler->display->display_options['fields']['php_3']['ui_name'] = 'transaction type';
$handler->display->display_options['fields']['php_3']['label'] = '';
$handler->display->display_options['fields']['php_3']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['php_3']['use_php_setup'] = 0;
$handler->display->display_options['fields']['php_3']['php_output'] = '<?php

switch ($row->type) {
	case \'money_transfer\':
		print t(\'transaction\') . \' \' . $row->title; // . \' [ \' . $row->name . \'] \';
	break;
	case \'chron_adjustment\':
		print t(\'adjustment\') . \' \' . $row->title;
	break;
	case \'purchase_receipt\':
		if ($row->field_amount_paid < 0)
			print t(\'cancel. purchase\') . \' \' . $row->title;
		else
			print t(\'purchase\') . \' \' . $row->title;
	break;
	case \'forex_transaction\':
		if ($row->field_conversion_type == \'chrons_to_euros\')
			print t(\'Chrons sale\') . \' \' . $row->title;
		else
			print t(\'Chrons purchase\') . \' \' . $row->title;
	break;
	default:
	print t(\'work\') . \' \' . $row->title;
}

?>';
$handler->display->display_options['fields']['php_3']['use_php_click_sortable'] = '0';
$handler->display->display_options['fields']['php_3']['php_click_sortable'] = '';
/* Field: php */
$handler->display->display_options['fields']['php']['id'] = 'php';
$handler->display->display_options['fields']['php']['table'] = 'views';
$handler->display->display_options['fields']['php']['field'] = 'php';
$handler->display->display_options['fields']['php']['ui_name'] = 'php';
$handler->display->display_options['fields']['php']['label'] = '';
$handler->display->display_options['fields']['php']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['php']['use_php_setup'] = 0;
$handler->display->display_options['fields']['php']['php_output'] = '<?php 

switch ($data->node_type) {
	case \'purchase_receipt\':
		if ($data->field_field_amount_paid[0][\'raw\'][\'value\'] < 0){
			if (isset($data->field_field_receipt_project[0][\'rendered\'][\'#item\'][\'target_id\'])) {
				print t(\'PProject\') . \': <a href="\'.$data->field_field_receipt_project[0][\'rendered\'][\'#item\'][\'target_id\'].\'">\' . $data->field_field_receipt_project[0][\'rendered\'][\'#label\'] . \'</a>\';
			}
			else {
				print \'unknown project\';
			}
		}
		else {
			print \'<a href="/user/\'.$data->field_field_buyer[0][\'raw\'][\'target_id\'].\'">\'.$data->field_field_buyer[0][\'rendered\'][\'#label\'].\'</a>\';
		}
	break;
	case \'money_transfer\':
		if (isset($data->_field_data[\'users_node_uid\'])) {
			print $data->_field_data[\'users_node_uid\'][\'entity\']->uid == 0 ? t(\'Anonymous\') : \'<a href="\'.$data->_field_data[\'users_node_uid\'][\'entity\']->uid.\'">\'.$data->_field_data[\'users_node_uid\'][\'entity\']->name.\'</a>\';
		}
		else {
			print \'sender missing\';
		}
	break;
	case \'forex_transaction\':
		print \'<a href="/user/\'.$data->field_field_buyer[0][\'raw\'][\'target_id\'].\'">\'.$data->field_field_buyer[0][\'rendered\'][\'#label\'].\'</a>\';
	break;
	case \'chron_adjustment\':
		print \'Rovas\';
	break;
	default: //work report
		print \'Rovas\';
}
?>';
$handler->display->display_options['fields']['php']['use_php_click_sortable'] = '0';
$handler->display->display_options['fields']['php']['php_click_sortable'] = '';
/* Field: php */
$handler->display->display_options['fields']['php_2']['id'] = 'php_2';
$handler->display->display_options['fields']['php_2']['table'] = 'views';
$handler->display->display_options['fields']['php_2']['field'] = 'php';
$handler->display->display_options['fields']['php_2']['ui_name'] = 'php';
$handler->display->display_options['fields']['php_2']['label'] = 'Recipient';
$handler->display->display_options['fields']['php_2']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['php_2']['use_php_setup'] = 0;
$handler->display->display_options['fields']['php_2']['php_output'] = '<?php
global $user;

switch ($data->node_type) {
	case \'purchase_receipt\':
		if ($data->field_field_amount_paid[0][\'raw\'][\'value\'] < 0)
			print \'<a href="/\'.$data->field_field_buyer[0][\'rendered\'][\'#uri\'][\'path\'].\'">\'.$data->field_field_buyer[0][\'rendered\'][\'#label\'].\'</a>\';
		else {
			if (isset($data->field_field_receipt_project[0][\'rendered\'][\'#item\'][\'entity\']->title_field[$user->language])) {
				$title = $data->field_field_receipt_project[0][\'rendered\'][\'#item\'][\'entity\']->title_field[$user->language][0][\'value\'];
			}
			else {
				$title = $data->field_field_receipt_project[0][\'rendered\'][\'#item\'][\'entity\']->title;
			}
			print t(\'Project\') . \': <a href="/\'.$data->field_field_receipt_project[0][\'rendered\'][\'#uri\'][\'path\'].\'">\' . $title . \'</a>\';
		}
	break;
	case \'money_transfer\':
		if (isset($data->_field_data[\'users_field_data_field_recipient_uid\'])) {
			print \'<a href="/user/\'.$data->_field_data[\'users_field_data_field_recipient_uid\'][\'entity\']->uid.\'">\'.$data->_field_data[\'users_field_data_field_recipient_uid\'][\'entity\']->name.\'</a>\';
		}
		else {
			print \'Missing recipient\';
		}
	break;
	case \'forex_transaction\':
		print \'<a href="/user/\'.$data->field_field_buyer[0][\'raw\'][\'target_id\'].\'">\'.$data->field_field_buyer[0][\'rendered\'][\'#label\'].\'</a>\';
	break;
	case \'chron_adjustment\':
		print \'<a href="/user/\'.$data->_field_data[\'users_node_uid\'][\'entity\']->uid.\'">\'.$data->_field_data[\'users_node_uid\'][\'entity\']->name.\'</a>\';
	break;
	default: //work report
		print \'<a href="/user/\'.$data->_field_data[\'users_node_uid\'][\'entity\']->uid.\'">\'.$data->_field_data[\'users_node_uid\'][\'entity\']->name.\'</a>\';
}

?>';
$handler->display->display_options['fields']['php_2']['use_php_click_sortable'] = '0';
$handler->display->display_options['fields']['php_2']['php_click_sortable'] = '';
/* Field: switch */
$handler->display->display_options['fields']['php_4']['id'] = 'php_4';
$handler->display->display_options['fields']['php_4']['table'] = 'views';
$handler->display->display_options['fields']['php_4']['field'] = 'php';
$handler->display->display_options['fields']['php_4']['ui_name'] = 'switch';
$handler->display->display_options['fields']['php_4']['label'] = 'switch';
$handler->display->display_options['fields']['php_4']['exclude'] = TRUE;
$handler->display->display_options['fields']['php_4']['use_php_setup'] = 0;
$handler->display->display_options['fields']['php_4']['php_output'] = '<?php 

//this field is used to produce classes for styling the table rows

switch ($data->node_type) {
	case \'work_report\':
		if ((module_exists(\'rovas_chron\') && (module_load_include(\'inc\', \'rovas_chron\', \'rovas_chron.ledger\') || TRUE) && function_exists(\'rovas_chron_wr_has_grant\') && rovas_chron_wr_has_grant((int) $data->nid)))
			print \'noop\';
		else
			print \'credit\';
	break;
	case \'chron_adjustment\':
		if (isset($data->field_field_amount[0][\'raw\'][\'value\']) && $data->field_field_amount[0][\'raw\'][\'value\'] < 0)
			print \'debit\';
		else
			print \'credit\';
	break;
	case \'purchase_receipt\':
		if ($data->field_field_amount_paid[0][\'raw\'][\'value\'] < 0)
			print \'credit\';
		else
			print \'debit\';
	break;
	case \'money_transfer\':
		if ($data->users_field_data_field_recipient_uid == $data->node_uid) //recipient is the same as sender
			print \'noop\';
		else if ($data->users_field_data_field_recipient_uid == arg(1)) 
			print \'credit\';
		else
			print \'debit\';
	break;
	case \'asset_usage_record\':
			print \'debit\';
	break;
	case \'forex_transaction\':
		if ($data->field_field_conversion_type[0][\'raw\'][\'value\'] == \'chrons_to_euros\')
			print \'debit\';
		else
			print \'credit\';
	break;
	default:
		print \'credit\';
}

?>';
$handler->display->display_options['fields']['php_4']['use_php_click_sortable'] = '0';
$handler->display->display_options['fields']['php_4']['php_click_sortable'] = '';
/* Field: Content: Time worked */
$handler->display->display_options['fields']['field_hours_worked']['id'] = 'field_hours_worked';
$handler->display->display_options['fields']['field_hours_worked']['table'] = 'field_data_field_hours_worked';
$handler->display->display_options['fields']['field_hours_worked']['field'] = 'field_hours_worked';
$handler->display->display_options['fields']['field_hours_worked']['label'] = '';
$handler->display->display_options['fields']['field_hours_worked']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_hours_worked']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_hours_worked']['settings'] = array(
  'thousand_separator' => '',
  'decimal_separator' => '.',
  'scale' => '2',
  'prefix_suffix' => 0,
);
/* Field: Content: Paid amount */
$handler->display->display_options['fields']['field_input_amount']['id'] = 'field_input_amount';
$handler->display->display_options['fields']['field_input_amount']['table'] = 'field_data_field_input_amount';
$handler->display->display_options['fields']['field_input_amount']['field'] = 'field_input_amount';
$handler->display->display_options['fields']['field_input_amount']['label'] = '';
$handler->display->display_options['fields']['field_input_amount']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_input_amount']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_input_amount']['settings'] = array(
  'thousand_separator' => '',
  'decimal_separator' => '.',
  'scale' => '2',
  'prefix_suffix' => 0,
);
/* Field: Content: Received amount */
$handler->display->display_options['fields']['field_output_amount']['id'] = 'field_output_amount';
$handler->display->display_options['fields']['field_output_amount']['table'] = 'field_data_field_output_amount';
$handler->display->display_options['fields']['field_output_amount']['field'] = 'field_output_amount';
$handler->display->display_options['fields']['field_output_amount']['label'] = '';
$handler->display->display_options['fields']['field_output_amount']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_output_amount']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_output_amount']['settings'] = array(
  'thousand_separator' => '',
  'decimal_separator' => '.',
  'scale' => '2',
  'prefix_suffix' => 0,
);
/* Field: php */
$handler->display->display_options['fields']['php_1']['id'] = 'php_1';
$handler->display->display_options['fields']['php_1']['table'] = 'views';
$handler->display->display_options['fields']['php_1']['field'] = 'php';
$handler->display->display_options['fields']['php_1']['ui_name'] = 'php';
$handler->display->display_options['fields']['php_1']['label'] = 'Amount';
$handler->display->display_options['fields']['php_1']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['php_1']['use_php_setup'] = 0;
$handler->display->display_options['fields']['php_1']['php_output'] = '<?php

$uid = 
isset($handler->last_tokens[\'%1\']) ? $handler->last_tokens[\'%1\'] :
isset($handler->last_tokens[\'%2\']) ? $handler->last_tokens[\'%2\'] :
isset($handler->last_tokens[\'%3\']) ? $handler->last_tokens[\'%3\'] : arg(1);

//this field is used to produce classes for styling the table rows

switch ($data->node_type) {
	case \'work_report\':
		if ((module_exists(\'rovas_chron\') && (module_load_include(\'inc\', \'rovas_chron\', \'rovas_chron.ledger\') || TRUE) && function_exists(\'rovas_chron_wr_has_grant\') && rovas_chron_wr_has_grant((int) $data->nid)))
			$xfer_type = \'noop\';
		else
			$xfer_type = \'credit\';
	break;
	case \'chron_adjustment\':
		if (isset($data->field_field_amount[0][\'raw\'][\'value\']) && $data->field_field_amount[0][\'raw\'][\'value\'] < 0)
			$xfer_type = \'debit\';
		else
			$xfer_type = \'credit\';
	break;
	case \'purchase_receipt\':
		if ($data->field_field_amount_paid[0][\'raw\'][\'value\'] < 0)
			$xfer_type = \'credit\';
		else
			$xfer_type = \'debit\';
	break;
	case \'money_transfer\':
		if ($data->users_field_data_field_recipient_uid == $data->node_uid) //recipient is the same as sender
			$xfer_type = \'noop\';
		else if ($data->users_field_data_field_recipient_uid == $uid) 
			$xfer_type = \'credit\';
		else
			$xfer_type = \'debit\';
	break;
	case \'asset_usage_record\':
			$xfer_type = \'debit\';
	break;
	case \'forex_transaction\':
		if ($data->field_field_conversion_type[0][\'raw\'][\'value\'] == \'chrons_to_euros\')
			$xfer_type = \'debit\';
		else
			$xfer_type = \'credit\';
	break;
	default:
		$xfer_type = \'credit\';
}

//construct the sign
if ($xfer_type == \'debit\' || 
(isset($data->field_field_conversion_type[0][\'rendered\'][\'#markup\']) && $data->field_field_conversion_type[0][\'rendered\'][\'#markup\'] == \'chrons_to_euros\')
)
	$sign = \'-\';
else
	$sign = \'\';

//money sent by others to "me"
if(isset($data->field_field_amount[0][\'raw\'][\'value\'])) {
	$__amt = $data->field_field_amount[0][\'raw\'][\'value\'];
	// chron_adjustment: signed amount (neg = debit); money_transfer reimbursements flip
	if ($data->node_type == \'chron_adjustment\') {
		print ($__amt < 0 ? \'-\' : \'\') . \' \' . abs($__amt);
	}
	else if ($__amt < 0) {
		print -1 * $__amt;
	}
	else {
		print $sign . \' \' . $__amt;
	}
}
//money "I" paid for purchases
else if (isset($data->field_field_amount_paid[0][\'raw\'][\'value\'])) {
	print $sign . \' \' . $data->field_field_amount_paid[0][\'raw\'][\'value\'];
}
//money "I" paid to get Euros
else if (isset($data->field_field_conversion_type[0][\'raw\'][\'value\']) && $data->field_field_conversion_type[0][\'raw\'][\'value\'] == \'chrons_to_euros\') {
	print $sign . \' \' . $data->field_field_input_amount[0][\'raw\'][\'value\'];
}
//Euros "I" paid to get Chrons
else if (isset($data->field_field_conversion_type[0][\'raw\'][\'value\']) && $data->field_field_conversion_type[0][\'raw\'][\'value\'] == \'euros_to_chrons\') {
	print $sign . \' \' . $data->field_field_output_amount[0][\'raw\'][\'value\'];
}
//money "I" received for work
else {
	if ($xfer_type == \'noop\') {
		print \'0\';
	}
	else {
		print $sign . \' \' . $data->field_field_chrons_to_grant[0][\'rendered\'][\'#markup\'];
		if (isset($data->field_field_verification_status[0][\'raw\'][\'value\']) && $data->field_field_verification_status[0][\'raw\'][\'value\'] == 0)
			print \'&nbsp;(\' . $data->field_field_hours_worked[0][\'rendered\'][\'#markup\'] * 10 . \'*)\';
	}
}
print \' CH\';
?>';
$handler->display->display_options['fields']['php_1']['use_php_click_sortable'] = '0';
$handler->display->display_options['fields']['php_1']['php_click_sortable'] = '';
$handler->display->display_options['defaults']['filter_groups'] = FALSE;
$handler->display->display_options['defaults']['filters'] = FALSE;
/* Filter criterion: Content: Published status */
$handler->display->display_options['filters']['status']['id'] = 'status';
$handler->display->display_options['filters']['status']['table'] = 'node';
$handler->display->display_options['filters']['status']['field'] = 'status';
$handler->display->display_options['filters']['status']['value'] = 1;
$handler->display->display_options['filters']['status']['group'] = 1;
$handler->display->display_options['filters']['status']['expose']['operator'] = FALSE;
/* Filter criterion: Content: Type (translated) */
$handler->display->display_options['filters']['type_i18n']['id'] = 'type_i18n';
$handler->display->display_options['filters']['type_i18n']['table'] = 'node';
$handler->display->display_options['filters']['type_i18n']['field'] = 'type_i18n';
$handler->display->display_options['filters']['type_i18n']['value'] = array(
  'chron_adjustment' => 'chron_adjustment',
  'forex_transaction' => 'forex_transaction',
  'money_transfer' => 'money_transfer',
  'purchase_receipt' => 'purchase_receipt',
  'work_report' => 'work_report',
);
$handler->display->display_options['filters']['type_i18n']['group'] = 1;
$handler->display->display_options['filters']['type_i18n']['expose']['operator_id'] = 'type_i18n_op';
$handler->display->display_options['filters']['type_i18n']['expose']['label'] = 'Type';
$handler->display->display_options['filters']['type_i18n']['expose']['operator'] = 'type_i18n_op';
$handler->display->display_options['filters']['type_i18n']['expose']['identifier'] = 'type_i18n';
$handler->display->display_options['filters']['type_i18n']['expose']['required'] = TRUE;
$handler->display->display_options['filters']['type_i18n']['expose']['multiple'] = TRUE;
$handler->display->display_options['filters']['type_i18n']['expose']['remember_roles'] = array(
  2 => '2',
  1 => 0,
  3 => 0,
  4 => 0,
  6 => 0,
  5 => 0,
  7 => 0,
  8 => 0,
);
$handler->display->display_options['filters']['type_i18n']['expose']['reduce'] = TRUE;
$handler->display->display_options['path'] = 'user/%/sumchrons';
$handler->display->display_options['menu']['type'] = 'tab';
$handler->display->display_options['menu']['title'] = 'Sum Chrons';
$handler->display->display_options['menu']['weight'] = '0';
$handler->display->display_options['menu']['name'] = 'user-menu';
$handler->display->display_options['menu']['context'] = 0;
$handler->display->display_options['menu']['context_only_inline'] = 0;
$translatables['money_transfers'] = array(
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
  t('There are no transactions.'),
  t('author'),
  t('User entity referenced from field_recipient'),
  t('User entity referenced from field_buyer'),
  t('Amount'),
  t('Buyer'),
  t('Full name'),
  t('Sender Full name'),
  t('Project'),
  t('Recipient'),
  t('[field_full_name_1] ([name_1])'),
  t('Type'),
  t('Date'),
  t('Title'),
  t('ID'),
  t('Sender'),
  t('switch'),
  t('All'),
  t('Type (translated)'),
  t('Page'),
  t('Sum Chrons'),
);

