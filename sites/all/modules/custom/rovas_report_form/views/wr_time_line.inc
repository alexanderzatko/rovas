<?php

/**
 * @file
 * Default view: wr_time_line
 */

$view = new view();
$view->name = 'wr_time_line';
$view->description = '';
$view->tag = 'default';
$view->base_table = 'node';
$view->human_name = 'WR time line';
$view->core = 7;
$view->api_version = '3.0';
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

/* Display: Master */
$handler = $view->new_display('default', 'Master', 'default');
$handler->display->display_options['title'] = 'Work reports';
$handler->display->display_options['use_more_always'] = FALSE;
$handler->display->display_options['access']['type'] = 'role';
$handler->display->display_options['access']['role'] = array(
  6 => '6',
  3 => '3',
  2 => '2',
  5 => '5',
  4 => '4',
);
$handler->display->display_options['cache']['type'] = 'none';
$handler->display->display_options['query']['type'] = 'views_query';
$handler->display->display_options['query']['options']['distinct'] = TRUE;
$handler->display->display_options['query']['options']['pure_distinct'] = TRUE;
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
  'field_classification_tid_i18n' => array(
    'bef_format' => 'default',
    'more_options' => array(
      'bef_select_all_none' => 0,
      'bef_select_all_none_nested' => 0,
      'bef_term_description' => 0,
      'bef_collapsible' => 0,
      'autosubmit' => 0,
      'is_secondary' => 0,
      'any_label' => 'all',
      'bef_filter_description' => '',
      'tokens' => array(
        'available' => array(
          0 => 'global_types',
          1 => 'vocabulary',
        ),
      ),
      'rewrite' => array(
        'filter_rewrite_values' => '',
      ),
    ),
  ),
  'language' => array(
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
  'field_verification_status_value_i18n' => array(
    'bef_format' => 'default',
    'more_options' => array(
      'bef_select_all_none' => 0,
      'bef_collapsible' => 0,
      'autosubmit' => 0,
      'is_secondary' => 0,
      'any_label' => 'all',
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
  'field_system_report_value_i18n' => array(
    'bef_format' => 'bef',
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
  'title' => array(
    'bef_format' => 'default',
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
  'field_verification_note_value' => array(
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
);
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
$handler->display->display_options['style_options']['row_class'] = 'wr-row-[field_verification_status]';
$handler->display->display_options['row_plugin'] = 'fields';
$handler->display->display_options['row_options']['inline'] = array(
  'changed' => 'changed',
  'field_verification_status' => 'field_verification_status',
  'php_3' => 'php_3',
);
/* Relationship: Content: Content author */
$handler->display->display_options['relationships']['uid']['id'] = 'uid';
$handler->display->display_options['relationships']['uid']['table'] = 'node';
$handler->display->display_options['relationships']['uid']['field'] = 'uid';
$handler->display->display_options['relationships']['uid']['label'] = 'user';
/* Relationship: Entity Reference: Referenced Entity */
$handler->display->display_options['relationships']['field_project_target_id']['id'] = 'field_project_target_id';
$handler->display->display_options['relationships']['field_project_target_id']['table'] = 'field_data_field_project';
$handler->display->display_options['relationships']['field_project_target_id']['field'] = 'field_project_target_id';
$handler->display->display_options['relationships']['field_project_target_id']['label'] = 'project';
/* Relationship: Entity Reference: Referenced Entity */
$handler->display->display_options['relationships']['field_verification_slip_target_id']['id'] = 'field_verification_slip_target_id';
$handler->display->display_options['relationships']['field_verification_slip_target_id']['table'] = 'field_data_field_verification_slip';
$handler->display->display_options['relationships']['field_verification_slip_target_id']['field'] = 'field_verification_slip_target_id';
$handler->display->display_options['relationships']['field_verification_slip_target_id']['label'] = 'verification';
/* Field: Content: Nid */
$handler->display->display_options['fields']['nid']['id'] = 'nid';
$handler->display->display_options['fields']['nid']['table'] = 'node';
$handler->display->display_options['fields']['nid']['field'] = 'nid';
$handler->display->display_options['fields']['nid']['label'] = '';
$handler->display->display_options['fields']['nid']['exclude'] = TRUE;
$handler->display->display_options['fields']['nid']['element_label_colon'] = FALSE;
/* Field: Content: Updated date */
$handler->display->display_options['fields']['changed']['id'] = 'changed';
$handler->display->display_options['fields']['changed']['table'] = 'node';
$handler->display->display_options['fields']['changed']['field'] = 'changed';
$handler->display->display_options['fields']['changed']['label'] = '';
$handler->display->display_options['fields']['changed']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['changed']['date_format'] = 'short_timestamp_with_spelled_month';
$handler->display->display_options['fields']['changed']['second_date_format'] = 'privatemsg_current_day';
/* Field: Content: Verification status */
$handler->display->display_options['fields']['field_verification_status']['id'] = 'field_verification_status';
$handler->display->display_options['fields']['field_verification_status']['table'] = 'field_data_field_verification_status';
$handler->display->display_options['fields']['field_verification_status']['field'] = 'field_verification_status';
$handler->display->display_options['fields']['field_verification_status']['label'] = '';
$handler->display->display_options['fields']['field_verification_status']['element_type'] = 'p';
$handler->display->display_options['fields']['field_verification_status']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_verification_status']['type'] = 'i18n_list_default';
/* Field: verification status icons */
$handler->display->display_options['fields']['php_3']['id'] = 'php_3';
$handler->display->display_options['fields']['php_3']['table'] = 'views';
$handler->display->display_options['fields']['php_3']['field'] = 'php';
$handler->display->display_options['fields']['php_3']['ui_name'] = 'verification status icons';
$handler->display->display_options['fields']['php_3']['label'] = '';
$handler->display->display_options['fields']['php_3']['element_class'] = 'verf-icons';
$handler->display->display_options['fields']['php_3']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['php_3']['element_default_classes'] = FALSE;
$handler->display->display_options['fields']['php_3']['use_php_setup'] = 0;
$handler->display->display_options['fields']['php_3']['php_output'] = '<?php 

$wr = entity_metadata_wrapper(\'node\', $row->nid);
$slips = $wr->field_verification_slip->value();


for ($i = 0; $i <= 2; $i++) {
	if (isset($slips[$i])) {
		$ver = entity_metadata_wrapper(\'node\', $slips[$i]->nid);
		$outcome = $ver->field_verdict->value();
		if ($outcome == 1) {
			echo \'<span style="color: green;">◼︎</span>\';
		}
		else {
			echo \'<span style="color: red;">◼︎</span>\';
		}
	}
	else
		echo \'<span style="color: lightgray;">◼︎</span>\';
}

?>';
$handler->display->display_options['fields']['php_3']['use_php_click_sortable'] = '0';
$handler->display->display_options['fields']['php_3']['php_click_sortable'] = '';
/* Field: date php */
$handler->display->display_options['fields']['php_2']['id'] = 'php_2';
$handler->display->display_options['fields']['php_2']['table'] = 'views';
$handler->display->display_options['fields']['php_2']['field'] = 'php';
$handler->display->display_options['fields']['php_2']['ui_name'] = 'date php';
$handler->display->display_options['fields']['php_2']['label'] = '';
$handler->display->display_options['fields']['php_2']['exclude'] = TRUE;
$handler->display->display_options['fields']['php_2']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['php_2']['use_php_setup'] = 0;
$handler->display->display_options['fields']['php_2']['php_output'] = '<?php 
/*
echo \'<div>\';
echo $row->changed;
echo \'</div>\';
echo \'<div>\';
if ($row->field_verification_status == 1)
	echo \'&nbsp; (\' . t(\'verified\') . \')\';
else
	echo \'&nbsp; (\' . t(\'not verified\') . \')\';
echo \'</div>\';
echo \'<div>\';
echo $row->php_3;
echo \'</div>\';
*/
?>';
$handler->display->display_options['fields']['php_2']['use_php_click_sortable'] = '0';
$handler->display->display_options['fields']['php_2']['php_click_sortable'] = '';
/* Field: Content: Title */
$handler->display->display_options['fields']['title']['id'] = 'title';
$handler->display->display_options['fields']['title']['table'] = 'node';
$handler->display->display_options['fields']['title']['field'] = 'title';
$handler->display->display_options['fields']['title']['label'] = '';
$handler->display->display_options['fields']['title']['alter']['word_boundary'] = FALSE;
$handler->display->display_options['fields']['title']['alter']['ellipsis'] = FALSE;
$handler->display->display_options['fields']['title']['element_label_colon'] = FALSE;
/* Field: User: Name */
$handler->display->display_options['fields']['name']['id'] = 'name';
$handler->display->display_options['fields']['name']['table'] = 'users';
$handler->display->display_options['fields']['name']['field'] = 'name';
$handler->display->display_options['fields']['name']['relationship'] = 'uid';
$handler->display->display_options['fields']['name']['label'] = '';
$handler->display->display_options['fields']['name']['element_label_colon'] = FALSE;
/* Field: Content: Time worked */
$handler->display->display_options['fields']['field_hours_worked']['id'] = 'field_hours_worked';
$handler->display->display_options['fields']['field_hours_worked']['table'] = 'field_data_field_hours_worked';
$handler->display->display_options['fields']['field_hours_worked']['field'] = 'field_hours_worked';
$handler->display->display_options['fields']['field_hours_worked']['label'] = '';
$handler->display->display_options['fields']['field_hours_worked']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_hours_worked']['alter']['text'] = '[field_hours_worked]';
$handler->display->display_options['fields']['field_hours_worked']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_hours_worked']['settings'] = array(
  'thousand_separator' => '',
  'decimal_separator' => '.',
  'scale' => '2',
  'prefix_suffix' => 0,
);
/* Field: chrons */
$handler->display->display_options['fields']['php_1']['id'] = 'php_1';
$handler->display->display_options['fields']['php_1']['table'] = 'views';
$handler->display->display_options['fields']['php_1']['field'] = 'php';
$handler->display->display_options['fields']['php_1']['ui_name'] = 'chrons';
$handler->display->display_options['fields']['php_1']['label'] = '';
$handler->display->display_options['fields']['php_1']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['php_1']['use_php_setup'] = 0;
$handler->display->display_options['fields']['php_1']['php_output'] = '<?php 
echo $row->field_hours_worked * 10 . \' \' . t(\'CH\');
?>';
$handler->display->display_options['fields']['php_1']['use_php_click_sortable'] = '0';
$handler->display->display_options['fields']['php_1']['php_click_sortable'] = '';
/* Field: Content: Photos */
$handler->display->display_options['fields']['field_photos']['id'] = 'field_photos';
$handler->display->display_options['fields']['field_photos']['table'] = 'field_data_field_photos';
$handler->display->display_options['fields']['field_photos']['field'] = 'field_photos';
$handler->display->display_options['fields']['field_photos']['label'] = '';
$handler->display->display_options['fields']['field_photos']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_photos']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_photos']['click_sort_column'] = 'fid';
$handler->display->display_options['fields']['field_photos']['type'] = 'lightbox2';
$handler->display->display_options['fields']['field_photos']['settings'] = array(
  'type' => 'lightbox',
  'image_style' => '60x60_thumb',
  'lightbox_style' => '0',
  'caption' => 'hidden',
);
$handler->display->display_options['fields']['field_photos']['delta_offset'] = '0';
/* Field: Content: Body */
$handler->display->display_options['fields']['body']['id'] = 'body';
$handler->display->display_options['fields']['body']['table'] = 'field_data_body';
$handler->display->display_options['fields']['body']['field'] = 'body';
$handler->display->display_options['fields']['body']['label'] = '';
$handler->display->display_options['fields']['body']['alter']['max_length'] = '160';
$handler->display->display_options['fields']['body']['alter']['trim'] = TRUE;
$handler->display->display_options['fields']['body']['element_label_colon'] = FALSE;
/* Field: wkt */
$handler->display->display_options['fields']['field_location_1']['id'] = 'field_location_1';
$handler->display->display_options['fields']['field_location_1']['table'] = 'field_data_field_location';
$handler->display->display_options['fields']['field_location_1']['field'] = 'field_location';
$handler->display->display_options['fields']['field_location_1']['ui_name'] = 'wkt';
$handler->display->display_options['fields']['field_location_1']['label'] = '';
$handler->display->display_options['fields']['field_location_1']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_location_1']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_location_1']['click_sort_column'] = 'geom';
$handler->display->display_options['fields']['field_location_1']['settings'] = array(
  'data' => 'full',
);
/* Field: Content: Location */
$handler->display->display_options['fields']['field_location']['id'] = 'field_location';
$handler->display->display_options['fields']['field_location']['table'] = 'field_data_field_location';
$handler->display->display_options['fields']['field_location']['field'] = 'field_location';
$handler->display->display_options['fields']['field_location']['label'] = '';
$handler->display->display_options['fields']['field_location']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_location']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_location']['click_sort_column'] = 'geom';
$handler->display->display_options['fields']['field_location']['type'] = 'geofield_openlayers';
$handler->display->display_options['fields']['field_location']['settings'] = array(
  'map_preset' => 'thumbnail_map',
  'data' => 'full',
);
/* Field: Content: Web Address */
$handler->display->display_options['fields']['field_web_address']['id'] = 'field_web_address';
$handler->display->display_options['fields']['field_web_address']['table'] = 'field_data_field_web_address';
$handler->display->display_options['fields']['field_web_address']['field'] = 'field_web_address';
$handler->display->display_options['fields']['field_web_address']['label'] = '';
$handler->display->display_options['fields']['field_web_address']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_web_address']['alter']['alter_text'] = TRUE;
$handler->display->display_options['fields']['field_web_address']['alter']['text'] = '<a href="[field_web_address-value]" target="-_blank">[field_web_address-value]</a>';
$handler->display->display_options['fields']['field_web_address']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_web_address']['settings'] = array(
  'title_style' => '_none',
  'title_link' => '',
  'title_class' => '',
);
/* Field: Global: PHP */
$handler->display->display_options['fields']['php']['id'] = 'php';
$handler->display->display_options['fields']['php']['table'] = 'views';
$handler->display->display_options['fields']['php']['field'] = 'php';
$handler->display->display_options['fields']['php']['label'] = '';
$handler->display->display_options['fields']['php']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['php']['use_php_setup'] = 0;
$handler->display->display_options['fields']['php']['php_output'] = '<?php
if ($row->field_photos) {
print \'<div class="wrt-photos">\' . $row->field_photos . \'</div>\';
}
if ($row->field_web_address) {
print \'<div class="web-address">\' . $row->field_web_address . \'</div>\';
}
if ($row->field_location_1) {
print \'<iframe src="\' . $row->field_location . \'" width="60px" height="60px" scrollong="no" frameborder="0">whatever</iframe>\';
}
?>';
$handler->display->display_options['fields']['php']['use_php_click_sortable'] = '0';
$handler->display->display_options['fields']['php']['php_click_sortable'] = '';
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
/* Filter criterion: Field: Classification (field_classification) (translated) */
$handler->display->display_options['filters']['field_classification_tid_i18n']['id'] = 'field_classification_tid_i18n';
$handler->display->display_options['filters']['field_classification_tid_i18n']['table'] = 'field_data_field_classification';
$handler->display->display_options['filters']['field_classification_tid_i18n']['field'] = 'field_classification_tid_i18n';
$handler->display->display_options['filters']['field_classification_tid_i18n']['group'] = 1;
$handler->display->display_options['filters']['field_classification_tid_i18n']['exposed'] = TRUE;
$handler->display->display_options['filters']['field_classification_tid_i18n']['expose']['operator_id'] = 'field_classification_tid_i18n_op';
$handler->display->display_options['filters']['field_classification_tid_i18n']['expose']['label'] = 'Classification';
$handler->display->display_options['filters']['field_classification_tid_i18n']['expose']['operator'] = 'field_classification_tid_i18n_op';
$handler->display->display_options['filters']['field_classification_tid_i18n']['expose']['identifier'] = 'field_classification_tid_i18n';
$handler->display->display_options['filters']['field_classification_tid_i18n']['expose']['remember'] = TRUE;
$handler->display->display_options['filters']['field_classification_tid_i18n']['expose']['multiple'] = TRUE;
$handler->display->display_options['filters']['field_classification_tid_i18n']['expose']['remember_roles'] = array(
  2 => '2',
  3 => '3',
  4 => '4',
  6 => '6',
  5 => '5',
  1 => 0,
);
$handler->display->display_options['filters']['field_classification_tid_i18n']['type'] = 'select';
$handler->display->display_options['filters']['field_classification_tid_i18n']['vocabulary'] = 'tags';
$handler->display->display_options['filters']['field_classification_tid_i18n']['hierarchy'] = 1;
$handler->display->display_options['filters']['field_classification_tid_i18n']['error_message'] = FALSE;
/* Filter criterion: Content: Language */
$handler->display->display_options['filters']['language']['id'] = 'language';
$handler->display->display_options['filters']['language']['table'] = 'node';
$handler->display->display_options['filters']['language']['field'] = 'language';
$handler->display->display_options['filters']['language']['group'] = 1;
$handler->display->display_options['filters']['language']['exposed'] = TRUE;
$handler->display->display_options['filters']['language']['expose']['operator_id'] = 'language_op';
$handler->display->display_options['filters']['language']['expose']['label'] = 'Language';
$handler->display->display_options['filters']['language']['expose']['operator'] = 'language_op';
$handler->display->display_options['filters']['language']['expose']['identifier'] = 'language';
$handler->display->display_options['filters']['language']['expose']['remember'] = TRUE;
$handler->display->display_options['filters']['language']['expose']['multiple'] = TRUE;
$handler->display->display_options['filters']['language']['expose']['remember_roles'] = array(
  2 => '2',
  3 => '3',
  4 => '4',
  6 => '6',
  5 => '5',
  1 => 0,
);
/* Filter criterion: Content: Verification status (field_verification_status) (translated) */
$handler->display->display_options['filters']['field_verification_status_value_i18n']['id'] = 'field_verification_status_value_i18n';
$handler->display->display_options['filters']['field_verification_status_value_i18n']['table'] = 'field_data_field_verification_status';
$handler->display->display_options['filters']['field_verification_status_value_i18n']['field'] = 'field_verification_status_value_i18n';
$handler->display->display_options['filters']['field_verification_status_value_i18n']['group'] = 1;
$handler->display->display_options['filters']['field_verification_status_value_i18n']['exposed'] = TRUE;
$handler->display->display_options['filters']['field_verification_status_value_i18n']['expose']['operator_id'] = 'field_verification_status_value_i18n_op';
$handler->display->display_options['filters']['field_verification_status_value_i18n']['expose']['label'] = 'Verification status';
$handler->display->display_options['filters']['field_verification_status_value_i18n']['expose']['operator'] = 'field_verification_status_value_i18n_op';
$handler->display->display_options['filters']['field_verification_status_value_i18n']['expose']['identifier'] = 'field_verification_status_value_i18n';
$handler->display->display_options['filters']['field_verification_status_value_i18n']['expose']['remember'] = TRUE;
$handler->display->display_options['filters']['field_verification_status_value_i18n']['expose']['multiple'] = TRUE;
$handler->display->display_options['filters']['field_verification_status_value_i18n']['expose']['remember_roles'] = array(
  2 => '2',
  3 => '3',
  4 => '4',
  6 => '6',
  5 => '5',
  1 => 0,
);
/* Filter criterion: User: Name */
$handler->display->display_options['filters']['uid']['id'] = 'uid';
$handler->display->display_options['filters']['uid']['table'] = 'users';
$handler->display->display_options['filters']['uid']['field'] = 'uid';
$handler->display->display_options['filters']['uid']['relationship'] = 'uid';
$handler->display->display_options['filters']['uid']['value'] = '';
$handler->display->display_options['filters']['uid']['group'] = 1;
$handler->display->display_options['filters']['uid']['exposed'] = TRUE;
$handler->display->display_options['filters']['uid']['expose']['operator_id'] = 'uid_op';
$handler->display->display_options['filters']['uid']['expose']['label'] = 'Worker Name';
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
/* Filter criterion: Content: System report (field_system_report) (translated) */
$handler->display->display_options['filters']['field_system_report_value_i18n']['id'] = 'field_system_report_value_i18n';
$handler->display->display_options['filters']['field_system_report_value_i18n']['table'] = 'field_data_field_system_report';
$handler->display->display_options['filters']['field_system_report_value_i18n']['field'] = 'field_system_report_value_i18n';
$handler->display->display_options['filters']['field_system_report_value_i18n']['group'] = 1;
$handler->display->display_options['filters']['field_system_report_value_i18n']['exposed'] = TRUE;
$handler->display->display_options['filters']['field_system_report_value_i18n']['expose']['operator_id'] = 'field_system_report_value_i18n_op';
$handler->display->display_options['filters']['field_system_report_value_i18n']['expose']['label'] = 'System report (field_system_report) (translated)';
$handler->display->display_options['filters']['field_system_report_value_i18n']['expose']['operator'] = 'field_system_report_value_i18n_op';
$handler->display->display_options['filters']['field_system_report_value_i18n']['expose']['identifier'] = 'field_system_report_value_i18n';
$handler->display->display_options['filters']['field_system_report_value_i18n']['is_grouped'] = TRUE;
$handler->display->display_options['filters']['field_system_report_value_i18n']['group_info']['label'] = 'System reports';
$handler->display->display_options['filters']['field_system_report_value_i18n']['group_info']['identifier'] = 'field_system_report_value_i18n';
$handler->display->display_options['filters']['field_system_report_value_i18n']['group_info']['optional'] = FALSE;
$handler->display->display_options['filters']['field_system_report_value_i18n']['group_info']['default_group'] = '1';
$handler->display->display_options['filters']['field_system_report_value_i18n']['group_info']['group_items'] = array(
  1 => array(
    'title' => 'hide',
    'operator' => 'or',
    'value' => array(
      0 => '0',
    ),
  ),
  2 => array(
    'title' => 'show',
    'operator' => 'or',
    'value' => array(
      1 => '1',
    ),
  ),
);
/* Filter criterion: Content: Project (field_project) */
$handler->display->display_options['filters']['field_project_target_id']['id'] = 'field_project_target_id';
$handler->display->display_options['filters']['field_project_target_id']['table'] = 'field_data_field_project';
$handler->display->display_options['filters']['field_project_target_id']['field'] = 'field_project_target_id';
$handler->display->display_options['filters']['field_project_target_id']['exposed'] = TRUE;
$handler->display->display_options['filters']['field_project_target_id']['expose']['operator_id'] = 'field_project_target_id_op';
$handler->display->display_options['filters']['field_project_target_id']['expose']['label'] = 'Project ID';
$handler->display->display_options['filters']['field_project_target_id']['expose']['operator'] = 'field_project_target_id_op';
$handler->display->display_options['filters']['field_project_target_id']['expose']['identifier'] = 'field_project_target_id';
$handler->display->display_options['filters']['field_project_target_id']['expose']['remember'] = TRUE;
$handler->display->display_options['filters']['field_project_target_id']['expose']['remember_roles'] = array(
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
$handler->display->display_options['filters']['title']['relationship'] = 'field_project_target_id';
$handler->display->display_options['filters']['title']['operator'] = 'contains';
$handler->display->display_options['filters']['title']['exposed'] = TRUE;
$handler->display->display_options['filters']['title']['expose']['operator_id'] = 'title_op';
$handler->display->display_options['filters']['title']['expose']['label'] = 'Project';
$handler->display->display_options['filters']['title']['expose']['operator'] = 'title_op';
$handler->display->display_options['filters']['title']['expose']['identifier'] = 'title';
$handler->display->display_options['filters']['title']['expose']['remember'] = TRUE;
$handler->display->display_options['filters']['title']['expose']['remember_roles'] = array(
  2 => '2',
  3 => '3',
  4 => '4',
  6 => '6',
  5 => '5',
  1 => 0,
);
$handler->display->display_options['filters']['title']['expose']['autocomplete_items'] = '10';
$handler->display->display_options['filters']['title']['expose']['autocomplete_min_chars'] = '0';
$handler->display->display_options['filters']['title']['expose']['autocomplete_field'] = 'title';
$handler->display->display_options['filters']['title']['expose']['autocomplete_raw_suggestion'] = 1;
$handler->display->display_options['filters']['title']['expose']['autocomplete_raw_dropdown'] = 1;
$handler->display->display_options['filters']['title']['expose']['autocomplete_dependent'] = 0;
$handler->display->display_options['views_ef_fieldset']['enabled'] = TRUE;

/* Display: Page */
$handler = $view->new_display('page', 'Page', 'page_1');
$handler->display->display_options['defaults']['style_plugin'] = FALSE;
$handler->display->display_options['style_plugin'] = 'table';
$handler->display->display_options['style_options']['row_class'] = 'wr-row-[field_verification_status_1]';
$handler->display->display_options['style_options']['columns'] = array(
  'nid' => 'nid',
  'changed' => 'changed',
  'created' => 'created',
  'title_field_et' => 'title_field_et',
  'title_1' => 'title_field_et',
  'php_2' => 'title_field_et',
  'name' => 'title_field_et',
  'php_1' => 'title_field_et',
  'body' => 'title_field_et',
  'field_hours_worked' => 'field_hours_worked',
  'php' => 'php',
  'field_verification_status' => 'field_verification_status',
  'field_verification_status_icons' => 'field_verification_status',
  'field_verification_status_1' => 'field_verification_status_1',
  'field_classification' => 'field_classification',
  'php_5' => 'php_5',
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
  'changed' => array(
    'sortable' => 0,
    'default_sort_order' => 'desc',
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
  'title_field_et' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '<br>',
    'empty_column' => 0,
  ),
  'title_1' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'php_2' => array(
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
  'php_1' => array(
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'body' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_hours_worked' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => 'views-align-right',
    'separator' => '',
    'empty_column' => 0,
  ),
  'php' => array(
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
  'field_verification_status_1' => array(
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
  'php_5' => array(
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
);
$handler->display->display_options['defaults']['style_options'] = FALSE;
$handler->display->display_options['defaults']['row_plugin'] = FALSE;
$handler->display->display_options['defaults']['row_options'] = FALSE;
$handler->display->display_options['defaults']['fields'] = FALSE;
/* Field: Content: Nid */
$handler->display->display_options['fields']['nid']['id'] = 'nid';
$handler->display->display_options['fields']['nid']['table'] = 'node';
$handler->display->display_options['fields']['nid']['field'] = 'nid';
$handler->display->display_options['fields']['nid']['label'] = '';
$handler->display->display_options['fields']['nid']['exclude'] = TRUE;
$handler->display->display_options['fields']['nid']['element_label_colon'] = FALSE;
/* Field: Content: Updated date */
$handler->display->display_options['fields']['changed']['id'] = 'changed';
$handler->display->display_options['fields']['changed']['table'] = 'node';
$handler->display->display_options['fields']['changed']['field'] = 'changed';
$handler->display->display_options['fields']['changed']['label'] = 'Date';
$handler->display->display_options['fields']['changed']['exclude'] = TRUE;
$handler->display->display_options['fields']['changed']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['changed']['date_format'] = 'custom';
$handler->display->display_options['fields']['changed']['custom_date_format'] = 'd M Y';
$handler->display->display_options['fields']['changed']['second_date_format'] = 'privatemsg_current_day';
/* Field: Content: Post date */
$handler->display->display_options['fields']['created']['id'] = 'created';
$handler->display->display_options['fields']['created']['table'] = 'node';
$handler->display->display_options['fields']['created']['field'] = 'created';
$handler->display->display_options['fields']['created']['label'] = 'Date';
$handler->display->display_options['fields']['created']['date_format'] = 'custom';
$handler->display->display_options['fields']['created']['custom_date_format'] = 'd M Y';
$handler->display->display_options['fields']['created']['second_date_format'] = 'privatemsg_current_day';
/* Field: Entity translation: Title: translated */
$handler->display->display_options['fields']['title_field_et']['id'] = 'title_field_et';
$handler->display->display_options['fields']['title_field_et']['table'] = 'field_data_title_field';
$handler->display->display_options['fields']['title_field_et']['field'] = 'title_field_et';
$handler->display->display_options['fields']['title_field_et']['label'] = 'Name / description...';
$handler->display->display_options['fields']['title_field_et']['type'] = 'title_linked';
$handler->display->display_options['fields']['title_field_et']['settings'] = array(
  'title_style' => '_none',
  'title_link' => 'content',
  'enable_trim' => 0,
  'trim_length' => '50',
  'title_class' => '',
);
/* Field: Project title */
$handler->display->display_options['fields']['title_1']['id'] = 'title_1';
$handler->display->display_options['fields']['title_1']['table'] = 'node';
$handler->display->display_options['fields']['title_1']['field'] = 'title';
$handler->display->display_options['fields']['title_1']['relationship'] = 'field_project_target_id';
$handler->display->display_options['fields']['title_1']['ui_name'] = 'Project title';
$handler->display->display_options['fields']['title_1']['label'] = '';
$handler->display->display_options['fields']['title_1']['exclude'] = TRUE;
$handler->display->display_options['fields']['title_1']['element_label_colon'] = FALSE;
/* Field: Global: PHP */
$handler->display->display_options['fields']['php_2']['id'] = 'php_2';
$handler->display->display_options['fields']['php_2']['table'] = 'views';
$handler->display->display_options['fields']['php_2']['field'] = 'php';
$handler->display->display_options['fields']['php_2']['label'] = '';
$handler->display->display_options['fields']['php_2']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['php_2']['use_php_setup'] = 0;
$handler->display->display_options['fields']['php_2']['php_output'] = '<?php 
$alias = drupal_get_path_alias(\'node/\'.$data->node_field_data_field_project_nid.\'\');
echo t(\'project\') . \': <a href="/\' . $alias . \'">\' . $data->node_field_data_field_project_title . \'</a>\';
?>';
$handler->display->display_options['fields']['php_2']['use_php_click_sortable'] = '0';
$handler->display->display_options['fields']['php_2']['php_click_sortable'] = '';
/* Field: User: Name */
$handler->display->display_options['fields']['name']['id'] = 'name';
$handler->display->display_options['fields']['name']['table'] = 'users';
$handler->display->display_options['fields']['name']['field'] = 'name';
$handler->display->display_options['fields']['name']['relationship'] = 'uid';
$handler->display->display_options['fields']['name']['label'] = '';
$handler->display->display_options['fields']['name']['exclude'] = TRUE;
$handler->display->display_options['fields']['name']['element_label_colon'] = FALSE;
/* Field: user php */
$handler->display->display_options['fields']['php_1']['id'] = 'php_1';
$handler->display->display_options['fields']['php_1']['table'] = 'views';
$handler->display->display_options['fields']['php_1']['field'] = 'php';
$handler->display->display_options['fields']['php_1']['ui_name'] = 'user php';
$handler->display->display_options['fields']['php_1']['use_php_setup'] = 0;
$handler->display->display_options['fields']['php_1']['php_output'] = '<?php
$uid = $data->users_node_uid;
echo \'<a href="/user/\'.$uid.\'">\' . $data->users_node_name . \'</a>\';
$user = user_load($uid);
echo render_merit_badge($user);
?>';
$handler->display->display_options['fields']['php_1']['use_php_click_sortable'] = '0';
$handler->display->display_options['fields']['php_1']['php_click_sortable'] = '';
/* Field: Content: Body */
$handler->display->display_options['fields']['body']['id'] = 'body';
$handler->display->display_options['fields']['body']['table'] = 'field_data_body';
$handler->display->display_options['fields']['body']['field'] = 'body';
$handler->display->display_options['fields']['body']['label'] = '';
$handler->display->display_options['fields']['body']['alter']['max_length'] = '160';
$handler->display->display_options['fields']['body']['alter']['trim'] = TRUE;
$handler->display->display_options['fields']['body']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['body']['type'] = 'custom_formatters_process_html';
/* Field: Content: Time worked */
$handler->display->display_options['fields']['field_hours_worked']['id'] = 'field_hours_worked';
$handler->display->display_options['fields']['field_hours_worked']['table'] = 'field_data_field_hours_worked';
$handler->display->display_options['fields']['field_hours_worked']['field'] = 'field_hours_worked';
$handler->display->display_options['fields']['field_hours_worked']['label'] = 'Time';
$handler->display->display_options['fields']['field_hours_worked']['alter']['text'] = '[field_hours_worked]';
$handler->display->display_options['fields']['field_hours_worked']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_hours_worked']['type'] = 'custom_formatters_labor_time_formatter';
$handler->display->display_options['fields']['field_hours_worked']['settings'] = array(
  'thousand_separator' => '',
  'decimal_separator' => '.',
  'scale' => '2',
  'prefix_suffix' => 0,
);
/* Field: default label for missing status */
$handler->display->display_options['fields']['php']['id'] = 'php';
$handler->display->display_options['fields']['php']['table'] = 'views';
$handler->display->display_options['fields']['php']['field'] = 'php';
$handler->display->display_options['fields']['php']['ui_name'] = 'default label for missing status';
$handler->display->display_options['fields']['php']['label'] = '';
$handler->display->display_options['fields']['php']['exclude'] = TRUE;
$handler->display->display_options['fields']['php']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['php']['use_php_setup'] = 0;
$handler->display->display_options['fields']['php']['php_output'] = '<?php 
echo t(\'not verified\');
?>';
$handler->display->display_options['fields']['php']['use_php_click_sortable'] = '0';
$handler->display->display_options['fields']['php']['php_click_sortable'] = '';
/* Field: Content: Verification status */
$handler->display->display_options['fields']['field_verification_status']['id'] = 'field_verification_status';
$handler->display->display_options['fields']['field_verification_status']['table'] = 'field_data_field_verification_status';
$handler->display->display_options['fields']['field_verification_status']['field'] = 'field_verification_status';
$handler->display->display_options['fields']['field_verification_status']['element_type'] = 'p';
$handler->display->display_options['fields']['field_verification_status']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_verification_status']['empty'] = '[php] ';
$handler->display->display_options['fields']['field_verification_status']['type'] = 'i18n_list_default';
/* Field: Content: Verification status icons */
$handler->display->display_options['fields']['field_verification_status_icons']['id'] = 'field_verification_status_icons';
$handler->display->display_options['fields']['field_verification_status_icons']['table'] = 'field_data_field_verification_status_icons';
$handler->display->display_options['fields']['field_verification_status_icons']['field'] = 'field_verification_status_icons';
$handler->display->display_options['fields']['field_verification_status_icons']['label'] = '';
$handler->display->display_options['fields']['field_verification_status_icons']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_verification_status_icons']['type'] = 'computed_field_unsanitized';
/* Field: ver status key */
$handler->display->display_options['fields']['field_verification_status_1']['id'] = 'field_verification_status_1';
$handler->display->display_options['fields']['field_verification_status_1']['table'] = 'field_data_field_verification_status';
$handler->display->display_options['fields']['field_verification_status_1']['field'] = 'field_verification_status';
$handler->display->display_options['fields']['field_verification_status_1']['ui_name'] = 'ver status key';
$handler->display->display_options['fields']['field_verification_status_1']['label'] = '';
$handler->display->display_options['fields']['field_verification_status_1']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_verification_status_1']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_verification_status_1']['empty'] = '0';
$handler->display->display_options['fields']['field_verification_status_1']['type'] = 'list_key';
/* Field: Field: Classification */
$handler->display->display_options['fields']['field_classification']['id'] = 'field_classification';
$handler->display->display_options['fields']['field_classification']['table'] = 'field_data_field_classification';
$handler->display->display_options['fields']['field_classification']['field'] = 'field_classification';
$handler->display->display_options['fields']['field_classification']['label'] = '';
$handler->display->display_options['fields']['field_classification']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_classification']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_classification']['delta_offset'] = '0';
$handler->display->display_options['fields']['field_classification']['separator'] = '|';
/* Field: Global: PHP */
$handler->display->display_options['fields']['php_5']['id'] = 'php_5';
$handler->display->display_options['fields']['php_5']['table'] = 'views';
$handler->display->display_options['fields']['php_5']['field'] = 'php';
$handler->display->display_options['fields']['php_5']['label'] = 'Classification';
$handler->display->display_options['fields']['php_5']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['php_5']['use_php_setup'] = 0;
$handler->display->display_options['fields']['php_5']['php_output'] = '<?php 

$classes = $data->field_field_classification;
$allcats = \'\';

foreach ($classes as $trieda) {
//	preg_match(\'/<a href="\\/taxonomy\\/term\\/([0-9]+)" .+">(.+)<\\/a>/\', $trieda, $matches, PREG_OFFSET_CAPTURE);
$tid = $trieda[\'raw\'][\'tid\'];
$labl = $trieda[\'rendered\'][\'#title\'];
	$kids = taxonomy_get_children($tid);
	$parents = taxonomy_get_parents($tid);
	//if the this category is the leaf (no more kids)
	if (!$kids)
		$allcats = $allcats . \'<span><a href="taxonomy/term/\'.$tid.\'">\'.$labl.\'</a></span><br>\';
	//if the only parent is the top category
	else if (isset($parents[1856]))
		$allcats = \'<span style="font-size: larger"><a href="taxonomy/term/\'.$tid.\'">\'.$labl.\'</a></span><br>\' . $allcats;
}
echo $allcats;
?>';
$handler->display->display_options['fields']['php_5']['use_php_click_sortable'] = '0';
$handler->display->display_options['fields']['php_5']['php_click_sortable'] = '';
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
/* Filter criterion: Field: Classification (field_classification) (translated) */
$handler->display->display_options['filters']['field_classification_tid_i18n']['id'] = 'field_classification_tid_i18n';
$handler->display->display_options['filters']['field_classification_tid_i18n']['table'] = 'field_data_field_classification';
$handler->display->display_options['filters']['field_classification_tid_i18n']['field'] = 'field_classification_tid_i18n';
$handler->display->display_options['filters']['field_classification_tid_i18n']['group'] = 1;
$handler->display->display_options['filters']['field_classification_tid_i18n']['exposed'] = TRUE;
$handler->display->display_options['filters']['field_classification_tid_i18n']['expose']['operator_id'] = 'field_classification_tid_i18n_op';
$handler->display->display_options['filters']['field_classification_tid_i18n']['expose']['label'] = 'Classification';
$handler->display->display_options['filters']['field_classification_tid_i18n']['expose']['operator'] = 'field_classification_tid_i18n_op';
$handler->display->display_options['filters']['field_classification_tid_i18n']['expose']['identifier'] = 'field_classification_tid_i18n';
$handler->display->display_options['filters']['field_classification_tid_i18n']['expose']['remember'] = TRUE;
$handler->display->display_options['filters']['field_classification_tid_i18n']['expose']['multiple'] = TRUE;
$handler->display->display_options['filters']['field_classification_tid_i18n']['expose']['remember_roles'] = array(
  2 => '2',
  3 => '3',
  4 => '4',
  6 => '6',
  5 => '5',
  1 => 0,
);
$handler->display->display_options['filters']['field_classification_tid_i18n']['type'] = 'select';
$handler->display->display_options['filters']['field_classification_tid_i18n']['vocabulary'] = 'tags';
$handler->display->display_options['filters']['field_classification_tid_i18n']['hierarchy'] = 1;
$handler->display->display_options['filters']['field_classification_tid_i18n']['error_message'] = FALSE;
/* Filter criterion: Content: Language */
$handler->display->display_options['filters']['language']['id'] = 'language';
$handler->display->display_options['filters']['language']['table'] = 'node';
$handler->display->display_options['filters']['language']['field'] = 'language';
$handler->display->display_options['filters']['language']['group'] = 1;
$handler->display->display_options['filters']['language']['exposed'] = TRUE;
$handler->display->display_options['filters']['language']['expose']['operator_id'] = 'language_op';
$handler->display->display_options['filters']['language']['expose']['label'] = 'Language';
$handler->display->display_options['filters']['language']['expose']['operator'] = 'language_op';
$handler->display->display_options['filters']['language']['expose']['identifier'] = 'language';
$handler->display->display_options['filters']['language']['expose']['remember'] = TRUE;
$handler->display->display_options['filters']['language']['expose']['multiple'] = TRUE;
$handler->display->display_options['filters']['language']['expose']['remember_roles'] = array(
  2 => '2',
  3 => '3',
  4 => '4',
  6 => '6',
  5 => '5',
  1 => 0,
);
/* Filter criterion: Content: Verification status (field_verification_status) (translated) */
$handler->display->display_options['filters']['field_verification_status_value_i18n']['id'] = 'field_verification_status_value_i18n';
$handler->display->display_options['filters']['field_verification_status_value_i18n']['table'] = 'field_data_field_verification_status';
$handler->display->display_options['filters']['field_verification_status_value_i18n']['field'] = 'field_verification_status_value_i18n';
$handler->display->display_options['filters']['field_verification_status_value_i18n']['group'] = 1;
$handler->display->display_options['filters']['field_verification_status_value_i18n']['exposed'] = TRUE;
$handler->display->display_options['filters']['field_verification_status_value_i18n']['expose']['operator_id'] = 'field_verification_status_value_i18n_op';
$handler->display->display_options['filters']['field_verification_status_value_i18n']['expose']['label'] = 'Verification status';
$handler->display->display_options['filters']['field_verification_status_value_i18n']['expose']['operator'] = 'field_verification_status_value_i18n_op';
$handler->display->display_options['filters']['field_verification_status_value_i18n']['expose']['identifier'] = 'field_verification_status_value_i18n';
$handler->display->display_options['filters']['field_verification_status_value_i18n']['expose']['remember'] = TRUE;
$handler->display->display_options['filters']['field_verification_status_value_i18n']['expose']['multiple'] = TRUE;
$handler->display->display_options['filters']['field_verification_status_value_i18n']['expose']['remember_roles'] = array(
  2 => '2',
  3 => '3',
  4 => '4',
  6 => '6',
  5 => '5',
  1 => 0,
);
/* Filter criterion: User: Name */
$handler->display->display_options['filters']['uid']['id'] = 'uid';
$handler->display->display_options['filters']['uid']['table'] = 'users';
$handler->display->display_options['filters']['uid']['field'] = 'uid';
$handler->display->display_options['filters']['uid']['relationship'] = 'uid';
$handler->display->display_options['filters']['uid']['value'] = '';
$handler->display->display_options['filters']['uid']['group'] = 1;
$handler->display->display_options['filters']['uid']['exposed'] = TRUE;
$handler->display->display_options['filters']['uid']['expose']['operator_id'] = 'uid_op';
$handler->display->display_options['filters']['uid']['expose']['label'] = 'Worker Name';
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
/* Filter criterion: Content: System report (field_system_report) (translated) */
$handler->display->display_options['filters']['field_system_report_value_i18n']['id'] = 'field_system_report_value_i18n';
$handler->display->display_options['filters']['field_system_report_value_i18n']['table'] = 'field_data_field_system_report';
$handler->display->display_options['filters']['field_system_report_value_i18n']['field'] = 'field_system_report_value_i18n';
$handler->display->display_options['filters']['field_system_report_value_i18n']['operator'] = 'empty';
$handler->display->display_options['filters']['field_system_report_value_i18n']['value'] = array(
  1 => '1',
);
$handler->display->display_options['filters']['field_system_report_value_i18n']['group'] = 1;
$handler->display->display_options['filters']['field_system_report_value_i18n']['exposed'] = TRUE;
$handler->display->display_options['filters']['field_system_report_value_i18n']['expose']['operator_id'] = 'field_system_report_value_i18n_op';
$handler->display->display_options['filters']['field_system_report_value_i18n']['expose']['label'] = 'Include System reports';
$handler->display->display_options['filters']['field_system_report_value_i18n']['expose']['operator'] = 'field_system_report_value_i18n_op';
$handler->display->display_options['filters']['field_system_report_value_i18n']['expose']['identifier'] = 'field_system_report_value_i18n';
$handler->display->display_options['filters']['field_system_report_value_i18n']['expose']['remember_roles'] = array(
  2 => '2',
  1 => 0,
  3 => 0,
  4 => 0,
  6 => 0,
  5 => 0,
  7 => 0,
);
$handler->display->display_options['filters']['field_system_report_value_i18n']['is_grouped'] = TRUE;
$handler->display->display_options['filters']['field_system_report_value_i18n']['group_info']['label'] = 'System reports';
$handler->display->display_options['filters']['field_system_report_value_i18n']['group_info']['identifier'] = 'field_system_report_value_i18n';
$handler->display->display_options['filters']['field_system_report_value_i18n']['group_info']['optional'] = FALSE;
$handler->display->display_options['filters']['field_system_report_value_i18n']['group_info']['default_group'] = '1';
$handler->display->display_options['filters']['field_system_report_value_i18n']['group_info']['group_items'] = array(
  1 => array(
    'title' => 'hide',
    'operator' => 'or',
    'value' => array(
      0 => '0',
    ),
  ),
  2 => array(
    'title' => 'show',
    'operator' => 'or',
    'value' => array(
      1 => '1',
    ),
  ),
);
/* Filter criterion: Content: Title */
$handler->display->display_options['filters']['title']['id'] = 'title';
$handler->display->display_options['filters']['title']['table'] = 'node';
$handler->display->display_options['filters']['title']['field'] = 'title';
$handler->display->display_options['filters']['title']['relationship'] = 'field_project_target_id';
$handler->display->display_options['filters']['title']['operator'] = 'contains';
$handler->display->display_options['filters']['title']['exposed'] = TRUE;
$handler->display->display_options['filters']['title']['expose']['operator_id'] = 'title_op';
$handler->display->display_options['filters']['title']['expose']['label'] = 'Project';
$handler->display->display_options['filters']['title']['expose']['operator'] = 'title_op';
$handler->display->display_options['filters']['title']['expose']['identifier'] = 'title';
$handler->display->display_options['filters']['title']['expose']['remember'] = TRUE;
$handler->display->display_options['filters']['title']['expose']['remember_roles'] = array(
  2 => '2',
  3 => '3',
  4 => '4',
  6 => '6',
  5 => '5',
  1 => 0,
);
$handler->display->display_options['filters']['title']['expose']['autocomplete_filter'] = 1;
$handler->display->display_options['filters']['title']['expose']['autocomplete_items'] = '10';
$handler->display->display_options['filters']['title']['expose']['autocomplete_min_chars'] = '0';
$handler->display->display_options['filters']['title']['expose']['autocomplete_field'] = 'title_1';
$handler->display->display_options['filters']['title']['expose']['autocomplete_raw_suggestion'] = 1;
$handler->display->display_options['filters']['title']['expose']['autocomplete_raw_dropdown'] = 1;
$handler->display->display_options['filters']['title']['expose']['autocomplete_dependent'] = 0;
/* Filter criterion: Content: Status (field_status) */
$handler->display->display_options['filters']['field_status_value']['id'] = 'field_status_value';
$handler->display->display_options['filters']['field_status_value']['table'] = 'field_data_field_status';
$handler->display->display_options['filters']['field_status_value']['field'] = 'field_status_value';
$handler->display->display_options['filters']['field_status_value']['relationship'] = 'field_project_target_id';
$handler->display->display_options['filters']['field_status_value']['value'] = array(
  0 => '0',
  1 => '1',
  2 => '2',
  3 => '3',
);
$handler->display->display_options['views_ef_fieldset']['enabled'] = TRUE;
$handler->display->display_options['views_ef_fieldset']['items'] = array(
  'fieldset:5af167a3c028a' => array(
    'id' => 'fieldset:5af167a3c028a',
    'pid' => '',
    'weight' => '-2',
    'options' => array(
      'label' => 'Show/hide the filters section',
      'collapse' => 'collapsed',
    ),
  ),
  'filter:field_classification_tid_i18n' => array(
    'id' => 'filter:field_classification_tid_i18n',
    'pid' => 'fieldset:5af167a3c028a',
    'weight' => '-10',
  ),
  'filter:language' => array(
    'id' => 'filter:language',
    'pid' => 'fieldset:5af167a3c028a',
    'weight' => '-9',
  ),
  'filter:field_verification_status_value_i18n' => array(
    'id' => 'filter:field_verification_status_value_i18n',
    'pid' => 'fieldset:5af167a3c028a',
    'weight' => '-8',
  ),
  'filter:uid' => array(
    'id' => 'filter:uid',
    'pid' => 'fieldset:5af167a3c028a',
    'weight' => '-7',
  ),
  'filter:title' => array(
    'id' => 'filter:title',
    'pid' => 'fieldset:5af167a3c028a',
    'weight' => '-6',
  ),
  'filter:field_system_report_value_i18n' => array(
    'id' => 'filter:field_system_report_value_i18n',
    'pid' => 'fieldset:5af167a3c028a',
    'weight' => '-5',
  ),
  'button:submit' => array(
    'id' => 'button:submit',
    'pid' => 'fieldset:5af167a3c028a',
    'weight' => '-3',
  ),
  'button:reset' => array(
    'id' => 'button:reset',
    'pid' => 'fieldset:5af167a3c028a',
    'weight' => '-2',
  ),
  'filter:field_verification_note_value' => array(
    'id' => 'filter:field_verification_note_value',
    'pid' => 'fieldset:5af167a3c028a',
    'weight' => '-4',
  ),
);
$handler->display->display_options['path'] = 'wr-time-line-table';
$handler->display->display_options['menu']['type'] = 'normal';
$handler->display->display_options['menu']['title'] = 'Reports';
$handler->display->display_options['menu']['description'] = 'The work reports timeline';
$handler->display->display_options['menu']['weight'] = '0';
$handler->display->display_options['menu']['name'] = 'main-menu';
$handler->display->display_options['menu']['context'] = 0;
$handler->display->display_options['menu']['context_only_inline'] = 0;
$translatables['wr_time_line'] = array(
  t('Master'),
  t('Work reports'),
  t('more'),
  t('Apply'),
  t('Reset'),
  t('Sort by'),
  t('Asc'),
  t('Desc'),
  t('Select any filter and click on Apply to see results'),
  t('Advanced options'),
  t('all'),
  t('Items per page'),
  t('- All -'),
  t('Offset'),
  t('«'),
  t('‹'),
  t('›'),
  t('»'),
  t('user'),
  t('project'),
  t('verification'),
  t('[field_hours_worked]'),
  t('<a href="[field_web_address-value]" target="-_blank">[field_web_address-value]</a>'),
  t('Classification'),
  t('Language'),
  t('Verification status'),
  t('Worker Name'),
  t('System report (field_system_report) (translated)'),
  t('System reports'),
  t('Project ID'),
  t('Project'),
  t('Page'),
  t('Date'),
  t('Name / description...'),
  t('PHP'),
  t('Time'),
  t('[php] '),
  t('Include System reports'),
);

