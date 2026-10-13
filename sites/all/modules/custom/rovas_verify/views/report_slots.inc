<?php

/**
 * @file
 * Default view: report_slots
 */

$view = new view();
$view->name = 'report_slots';
$view->description = '';
$view->tag = 'default';
$view->base_table = 'node';
$view->human_name = 'report slots';
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
$handler->display->display_options['pager']['options']['items_per_page'] = '10';
$handler->display->display_options['pager']['options']['offset'] = '0';
$handler->display->display_options['style_plugin'] = 'default';
$handler->display->display_options['row_plugin'] = 'fields';
/* Field: Bulk operations: Content */
$handler->display->display_options['fields']['views_bulk_operations']['id'] = 'views_bulk_operations';
$handler->display->display_options['fields']['views_bulk_operations']['table'] = 'views_entity_node';
$handler->display->display_options['fields']['views_bulk_operations']['field'] = 'views_bulk_operations';
$handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['display_type'] = '0';
$handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['enable_select_all_pages'] = 1;
$handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['row_clickable'] = 1;
$handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['force_single'] = 0;
$handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['entity_load_capacity'] = '10';
/* Field: Content: Report to verify */
$handler->display->display_options['fields']['field_report_to_verify']['id'] = 'field_report_to_verify';
$handler->display->display_options['fields']['field_report_to_verify']['table'] = 'field_data_field_report_to_verify';
$handler->display->display_options['fields']['field_report_to_verify']['field'] = 'field_report_to_verify';
$handler->display->display_options['fields']['field_report_to_verify']['label'] = '';
$handler->display->display_options['fields']['field_report_to_verify']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_report_to_verify']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_report_to_verify']['type'] = 'entityreference_entity_id';
$handler->display->display_options['fields']['field_report_to_verify']['settings'] = array(
  'bypass_access' => 0,
  'link' => 0,
);
/* Field: Content: Verify date */
$handler->display->display_options['fields']['field_verify_date']['id'] = 'field_verify_date';
$handler->display->display_options['fields']['field_verify_date']['table'] = 'field_data_field_verify_date';
$handler->display->display_options['fields']['field_verify_date']['field'] = 'field_verify_date';
$handler->display->display_options['fields']['field_verify_date']['label'] = '';
$handler->display->display_options['fields']['field_verify_date']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_verify_date']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_verify_date']['settings'] = array(
  'format_type' => 'short_timestamp_with_spelled_month',
  'custom_date_format' => '',
  'fromto' => 'both',
  'multiple_number' => '',
  'multiple_from' => '',
  'multiple_to' => '',
  'show_remaining_days' => 0,
);
/* Field: Content: Slot owner */
$handler->display->display_options['fields']['field_slip_owner']['id'] = 'field_slip_owner';
$handler->display->display_options['fields']['field_slip_owner']['table'] = 'field_data_field_slip_owner';
$handler->display->display_options['fields']['field_slip_owner']['field'] = 'field_slip_owner';
$handler->display->display_options['fields']['field_slip_owner']['label'] = '';
$handler->display->display_options['fields']['field_slip_owner']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_slip_owner']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_slip_owner']['settings'] = array(
  'bypass_access' => 0,
  'link' => 1,
);
/* Field: Global: PHP */
$handler->display->display_options['fields']['php']['id'] = 'php';
$handler->display->display_options['fields']['php']['table'] = 'views';
$handler->display->display_options['fields']['php']['field'] = 'php';
$handler->display->display_options['fields']['php']['label'] = '';
$handler->display->display_options['fields']['php']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['php']['use_php_setup'] = 0;
$handler->display->display_options['fields']['php']['php_output'] = '<?php 
echo \'<a href="/\';
echo isset($data->field_field_slip_owner[0][\'rendered\'][\'#uri\'][\'path\']) ? $data->field_field_slip_owner[0][\'rendered\'][\'#uri\'][\'path\'] : \'-\';
echo \'">\';
echo isset($data->field_field_slip_owner[0][\'rendered\'][\'#label\']) ? $data->field_field_slip_owner[0][\'rendered\'][\'#label\'] : \'-\';
echo \'</a> \';
echo t(\'deadline\',array(),array(\'context\' => \'report_slots view\'));
echo \': \';
echo isset($data->field_field_verify_date[0][\'rendered\'][\'#markup\']) ? $data->field_field_verify_date[0][\'rendered\'][\'#markup\'] : \'-\';
?>
';
$handler->display->display_options['fields']['php']['use_php_click_sortable'] = '0';
$handler->display->display_options['fields']['php']['php_click_sortable'] = '';
/* Contextual filter: Content: Report to verify (field_report_to_verify) */
$handler->display->display_options['arguments']['field_report_to_verify_target_id']['id'] = 'field_report_to_verify_target_id';
$handler->display->display_options['arguments']['field_report_to_verify_target_id']['table'] = 'field_data_field_report_to_verify';
$handler->display->display_options['arguments']['field_report_to_verify_target_id']['field'] = 'field_report_to_verify_target_id';
$handler->display->display_options['arguments']['field_report_to_verify_target_id']['default_action'] = 'default';
$handler->display->display_options['arguments']['field_report_to_verify_target_id']['default_argument_type'] = 'fixed';
$handler->display->display_options['arguments']['field_report_to_verify_target_id']['summary']['number_of_records'] = '0';
$handler->display->display_options['arguments']['field_report_to_verify_target_id']['summary']['format'] = 'default_summary';
$handler->display->display_options['arguments']['field_report_to_verify_target_id']['summary_options']['items_per_page'] = '25';
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
  'verification_slot' => 'verification_slot',
);
/* Filter criterion: Content: Declined (field_declined) */
$handler->display->display_options['filters']['field_declined_value']['id'] = 'field_declined_value';
$handler->display->display_options['filters']['field_declined_value']['table'] = 'field_data_field_declined';
$handler->display->display_options['filters']['field_declined_value']['field'] = 'field_declined_value';
$handler->display->display_options['filters']['field_declined_value']['operator'] = 'not';
$handler->display->display_options['filters']['field_declined_value']['value'] = array(
  1 => '1',
);
$translatables['report_slots'] = array(
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
);

