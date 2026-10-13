<?php

/**
 * @file
 * Default view: verif_slots
 */

$view = new view();
$view->name = 'verif_slots';
$view->description = '';
$view->tag = 'default';
$view->base_table = 'node';
$view->human_name = 'verif. slots';
$view->core = 7;
$view->api_version = '3.0';
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

/* Display: Master */
$handler = $view->new_display('default', 'Master', 'default');
$handler->display->display_options['title'] = 'verif. slots';
$handler->display->display_options['use_more_always'] = FALSE;
$handler->display->display_options['access']['type'] = 'perm';
$handler->display->display_options['cache']['type'] = 'none';
$handler->display->display_options['query']['type'] = 'views_query';
$handler->display->display_options['exposed_form']['type'] = 'basic';
$handler->display->display_options['pager']['type'] = 'full';
$handler->display->display_options['pager']['options']['items_per_page'] = '10';
$handler->display->display_options['style_plugin'] = 'table';
$handler->display->display_options['style_options']['columns'] = array(
  'nid' => 'nid',
  'field_slip_owner' => 'field_slip_owner',
  'field_report_to_verify' => 'field_report_to_verify',
  'field_verify_date' => 'field_verify_date',
  'field_voluntary' => 'field_voluntary',
  'field_declined' => 'field_declined',
  'created' => 'created',
);
$handler->display->display_options['style_options']['class'] = '';
$handler->display->display_options['style_options']['default'] = 'nid';
$handler->display->display_options['style_options']['info'] = array(
  'nid' => array(
    'sortable' => 1,
    'default_sort_order' => 'desc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_slip_owner' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_report_to_verify' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_verify_date' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_voluntary' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_declined' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'created' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
);
/* Relationship: Content: Content author */
$handler->display->display_options['relationships']['uid']['id'] = 'uid';
$handler->display->display_options['relationships']['uid']['table'] = 'node';
$handler->display->display_options['relationships']['uid']['field'] = 'uid';
/* Relationship: Entity Reference: Referenced Entity */
$handler->display->display_options['relationships']['field_report_to_verify_target_id']['id'] = 'field_report_to_verify_target_id';
$handler->display->display_options['relationships']['field_report_to_verify_target_id']['table'] = 'field_data_field_report_to_verify';
$handler->display->display_options['relationships']['field_report_to_verify_target_id']['field'] = 'field_report_to_verify_target_id';
$handler->display->display_options['relationships']['field_report_to_verify_target_id']['label'] = 'report';
/* Field: Content: Nid */
$handler->display->display_options['fields']['nid']['id'] = 'nid';
$handler->display->display_options['fields']['nid']['table'] = 'node';
$handler->display->display_options['fields']['nid']['field'] = 'nid';
$handler->display->display_options['fields']['nid']['link_to_node'] = TRUE;
/* Field: Content: Slot owner */
$handler->display->display_options['fields']['field_slip_owner']['id'] = 'field_slip_owner';
$handler->display->display_options['fields']['field_slip_owner']['table'] = 'field_data_field_slip_owner';
$handler->display->display_options['fields']['field_slip_owner']['field'] = 'field_slip_owner';
$handler->display->display_options['fields']['field_slip_owner']['settings'] = array(
  'bypass_access' => 0,
  'link' => 1,
);
/* Field: Content: Report to verify */
$handler->display->display_options['fields']['field_report_to_verify']['id'] = 'field_report_to_verify';
$handler->display->display_options['fields']['field_report_to_verify']['table'] = 'field_data_field_report_to_verify';
$handler->display->display_options['fields']['field_report_to_verify']['field'] = 'field_report_to_verify';
$handler->display->display_options['fields']['field_report_to_verify']['settings'] = array(
  'bypass_access' => 0,
  'link' => 1,
);
/* Field: Content: Verify date */
$handler->display->display_options['fields']['field_verify_date']['id'] = 'field_verify_date';
$handler->display->display_options['fields']['field_verify_date']['table'] = 'field_data_field_verify_date';
$handler->display->display_options['fields']['field_verify_date']['field'] = 'field_verify_date';
$handler->display->display_options['fields']['field_verify_date']['settings'] = array(
  'format_type' => 'rovas_short',
  'custom_date_format' => '',
  'fromto' => 'both',
  'multiple_number' => '',
  'multiple_from' => '',
  'multiple_to' => '',
  'show_remaining_days' => 0,
);
/* Field: Content: Voluntary */
$handler->display->display_options['fields']['field_voluntary']['id'] = 'field_voluntary';
$handler->display->display_options['fields']['field_voluntary']['table'] = 'field_data_field_voluntary';
$handler->display->display_options['fields']['field_voluntary']['field'] = 'field_voluntary';
/* Field: Content: Declined */
$handler->display->display_options['fields']['field_declined']['id'] = 'field_declined';
$handler->display->display_options['fields']['field_declined']['table'] = 'field_data_field_declined';
$handler->display->display_options['fields']['field_declined']['field'] = 'field_declined';
/* Field: Content: Post date */
$handler->display->display_options['fields']['created']['id'] = 'created';
$handler->display->display_options['fields']['created']['table'] = 'node';
$handler->display->display_options['fields']['created']['field'] = 'created';
$handler->display->display_options['fields']['created']['date_format'] = 'uc_store';
$handler->display->display_options['fields']['created']['second_date_format'] = 'privatemsg_current_day';
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
$handler->display->display_options['filters']['type']['group'] = 1;
/* Filter criterion: User: Name */
$handler->display->display_options['filters']['uid']['id'] = 'uid';
$handler->display->display_options['filters']['uid']['table'] = 'users';
$handler->display->display_options['filters']['uid']['field'] = 'uid';
$handler->display->display_options['filters']['uid']['relationship'] = 'uid';
$handler->display->display_options['filters']['uid']['value'] = '';
$handler->display->display_options['filters']['uid']['group'] = 1;
$handler->display->display_options['filters']['uid']['exposed'] = TRUE;
$handler->display->display_options['filters']['uid']['expose']['operator_id'] = 'uid_op';
$handler->display->display_options['filters']['uid']['expose']['label'] = 'Verifier name';
$handler->display->display_options['filters']['uid']['expose']['operator'] = 'uid_op';
$handler->display->display_options['filters']['uid']['expose']['identifier'] = 'uid';
$handler->display->display_options['filters']['uid']['expose']['remember_roles'] = array(
  2 => '2',
);
/* Filter criterion: Content: Declined (field_declined) */
$handler->display->display_options['filters']['field_declined_value']['id'] = 'field_declined_value';
$handler->display->display_options['filters']['field_declined_value']['table'] = 'field_data_field_declined';
$handler->display->display_options['filters']['field_declined_value']['field'] = 'field_declined_value';
$handler->display->display_options['filters']['field_declined_value']['value'] = array(
  'all' => 'all',
  0 => '0',
  1 => '1',
);
$handler->display->display_options['filters']['field_declined_value']['group'] = 1;
$handler->display->display_options['filters']['field_declined_value']['exposed'] = TRUE;
$handler->display->display_options['filters']['field_declined_value']['expose']['operator_id'] = 'field_declined_value_op';
$handler->display->display_options['filters']['field_declined_value']['expose']['label'] = 'Declined (field_declined)';
$handler->display->display_options['filters']['field_declined_value']['expose']['operator'] = 'field_declined_value_op';
$handler->display->display_options['filters']['field_declined_value']['expose']['identifier'] = 'field_declined_value';
$handler->display->display_options['filters']['field_declined_value']['expose']['remember_roles'] = array(
  2 => '2',
);
/* Filter criterion: Content: Voluntary (field_voluntary) */
$handler->display->display_options['filters']['field_voluntary_value']['id'] = 'field_voluntary_value';
$handler->display->display_options['filters']['field_voluntary_value']['table'] = 'field_data_field_voluntary';
$handler->display->display_options['filters']['field_voluntary_value']['field'] = 'field_voluntary_value';
$handler->display->display_options['filters']['field_voluntary_value']['value'] = array(
  'all' => 'all',
  0 => '0',
  1 => '1',
);
$handler->display->display_options['filters']['field_voluntary_value']['group'] = 1;
$handler->display->display_options['filters']['field_voluntary_value']['exposed'] = TRUE;
$handler->display->display_options['filters']['field_voluntary_value']['expose']['operator_id'] = 'field_voluntary_value_op';
$handler->display->display_options['filters']['field_voluntary_value']['expose']['label'] = 'Voluntary';
$handler->display->display_options['filters']['field_voluntary_value']['expose']['operator'] = 'field_voluntary_value_op';
$handler->display->display_options['filters']['field_voluntary_value']['expose']['identifier'] = 'field_voluntary_value';
$handler->display->display_options['filters']['field_voluntary_value']['expose']['remember_roles'] = array(
  2 => '2',
);
/* Filter criterion: Content: Nid */
$handler->display->display_options['filters']['nid']['id'] = 'nid';
$handler->display->display_options['filters']['nid']['table'] = 'node';
$handler->display->display_options['filters']['nid']['field'] = 'nid';
$handler->display->display_options['filters']['nid']['relationship'] = 'field_report_to_verify_target_id';
$handler->display->display_options['filters']['nid']['exposed'] = TRUE;
$handler->display->display_options['filters']['nid']['expose']['operator_id'] = 'nid_op';
$handler->display->display_options['filters']['nid']['expose']['label'] = 'Nid';
$handler->display->display_options['filters']['nid']['expose']['operator'] = 'nid_op';
$handler->display->display_options['filters']['nid']['expose']['identifier'] = 'nid';
$handler->display->display_options['filters']['nid']['expose']['remember_roles'] = array(
  2 => '2',
);

/* Display: Page */
$handler = $view->new_display('page', 'Page', 'page');
$handler->display->display_options['path'] = 'verif-slots';
$translatables['verif_slots'] = array(
  t('Master'),
  t('verif. slots'),
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
  t('report'),
  t('Nid'),
  t('Slot owner'),
  t('Report to verify'),
  t('Verify date'),
  t('Voluntary'),
  t('Declined'),
  t('Post date'),
  t('Verifier name'),
  t('Declined (field_declined)'),
  t('Page'),
);

