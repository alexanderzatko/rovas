<?php

/**
 * @file
 * Default view: top_compliant_users
 */

$view = new view();
$view->name = 'top_compliant_users';
$view->description = '';
$view->tag = 'default';
$view->base_table = 'users';
$view->human_name = 'top compliant users';
$view->core = 7;
$view->api_version = '3.0';
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

/* Display: Master */
$handler = $view->new_display('default', 'Master', 'default');
$handler->display->display_options['use_more_always'] = FALSE;
$handler->display->display_options['access']['type'] = 'perm';
$handler->display->display_options['access']['perm'] = 'access user profiles';
$handler->display->display_options['cache']['type'] = 'none';
$handler->display->display_options['query']['type'] = 'views_query';
$handler->display->display_options['exposed_form']['type'] = 'basic';
$handler->display->display_options['pager']['type'] = 'some';
$handler->display->display_options['pager']['options']['items_per_page'] = '100';
$handler->display->display_options['pager']['options']['offset'] = '0';
$handler->display->display_options['style_plugin'] = 'default';
$handler->display->display_options['row_plugin'] = 'fields';
/* Field: Bulk operations: User */
$handler->display->display_options['fields']['views_bulk_operations']['id'] = 'views_bulk_operations';
$handler->display->display_options['fields']['views_bulk_operations']['table'] = 'views_entity_user';
$handler->display->display_options['fields']['views_bulk_operations']['field'] = 'views_bulk_operations';
$handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['display_type'] = '0';
$handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['enable_select_all_pages'] = 1;
$handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['row_clickable'] = 1;
$handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['force_single'] = 0;
$handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['entity_load_capacity'] = '10';
/* Field: User: Uid */
$handler->display->display_options['fields']['uid']['id'] = 'uid';
$handler->display->display_options['fields']['uid']['table'] = 'users';
$handler->display->display_options['fields']['uid']['field'] = 'uid';
$handler->display->display_options['fields']['uid']['label'] = '';
$handler->display->display_options['fields']['uid']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['uid']['link_to_user'] = FALSE;
/* Contextual filter: User: Uid */
$handler->display->display_options['arguments']['uid']['id'] = 'uid';
$handler->display->display_options['arguments']['uid']['table'] = 'users';
$handler->display->display_options['arguments']['uid']['field'] = 'uid';
$handler->display->display_options['arguments']['uid']['default_argument_type'] = 'fixed';
$handler->display->display_options['arguments']['uid']['summary']['number_of_records'] = '0';
$handler->display->display_options['arguments']['uid']['summary']['format'] = 'default_summary';
$handler->display->display_options['arguments']['uid']['summary_options']['items_per_page'] = '25';
$handler->display->display_options['arguments']['uid']['break_phrase'] = TRUE;
$handler->display->display_options['arguments']['uid']['not'] = TRUE;
/* Contextual filter: User: Language */
$handler->display->display_options['arguments']['language']['id'] = 'language';
$handler->display->display_options['arguments']['language']['table'] = 'users';
$handler->display->display_options['arguments']['language']['field'] = 'language';
$handler->display->display_options['arguments']['language']['default_argument_type'] = 'fixed';
$handler->display->display_options['arguments']['language']['summary']['number_of_records'] = '0';
$handler->display->display_options['arguments']['language']['summary']['format'] = 'default_summary';
$handler->display->display_options['arguments']['language']['summary_options']['items_per_page'] = '25';
/* Filter criterion: User: Active status */
$handler->display->display_options['filters']['status']['id'] = 'status';
$handler->display->display_options['filters']['status']['table'] = 'users';
$handler->display->display_options['filters']['status']['field'] = 'status';
$handler->display->display_options['filters']['status']['value'] = '1';
$handler->display->display_options['filters']['status']['group'] = 1;
$handler->display->display_options['filters']['status']['expose']['operator'] = FALSE;
/* Filter criterion: User: Compliance score (field_verification_score) */
$handler->display->display_options['filters']['field_verification_score_value']['id'] = 'field_verification_score_value';
$handler->display->display_options['filters']['field_verification_score_value']['table'] = 'field_data_field_verification_score';
$handler->display->display_options['filters']['field_verification_score_value']['field'] = 'field_verification_score_value';
$handler->display->display_options['filters']['field_verification_score_value']['operator'] = '>';
$handler->display->display_options['filters']['field_verification_score_value']['value']['value'] = '10';
/* Filter criterion: has less than 2 slots */
$handler->display->display_options['filters']['raw_filter']['id'] = 'raw_filter';
$handler->display->display_options['filters']['raw_filter']['table'] = 'views_raw_sql';
$handler->display->display_options['filters']['raw_filter']['field'] = 'raw_filter';
$handler->display->display_options['filters']['raw_filter']['ui_name'] = 'has less than 2 slots';
$handler->display->display_options['filters']['raw_filter']['raw_sql'] = '(
                SELECT
                    COUNT(node.nid) AS nid
                FROM
                    {node} node
                LEFT JOIN
                    {field_data_field_slip_owner} slpown ON node.nid = slpown.entity_id AND (slpown.entity_type = \'node\' AND slpown.deleted = \'0\')
                LEFT JOIN
                    {field_data_field_verify_date} vdate ON node.nid = vdate.entity_id AND (vdate.entity_type = \'node\' AND vdate.deleted = \'0\')
                WHERE
                    node.status = \'1\'
                    AND node.type = \'verification_slot\'
                    AND slpown.field_slip_owner_target_id = users.uid
                    AND DATE_FORMAT(ADDTIME(FROM_UNIXTIME(vdate.field_verify_date_value), SEC_TO_TIME(3600)), \'%Y-%m-%d\') >= NOW()
) < 2';
/* Filter criterion: User: Verification requests (field_verification_requests) */
$handler->display->display_options['filters']['field_verification_requests_value']['id'] = 'field_verification_requests_value';
$handler->display->display_options['filters']['field_verification_requests_value']['table'] = 'field_data_field_verification_requests';
$handler->display->display_options['filters']['field_verification_requests_value']['field'] = 'field_verification_requests_value';
$handler->display->display_options['filters']['field_verification_requests_value']['value'] = array(
  1 => '1',
);
$translatables['top_compliant_users'] = array(
  t('Master'),
  t('more'),
  t('Apply'),
  t('Reset'),
  t('Sort by'),
  t('Asc'),
  t('Desc'),
  t('User'),
  t('- Choose an operation -'),
  t('All'),
);

