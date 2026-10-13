<?php

/**
 * @file
 * Default view: users_with_low_compliance_score
 */

$view = new view();
$view->name = 'users_with_low_compliance_score';
$view->description = '';
$view->tag = 'default';
$view->base_table = 'users';
$view->human_name = 'Users with low compliance score';
$view->core = 7;
$view->api_version = '3.0';
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

/* Display: Master */
$handler = $view->new_display('default', 'Master', 'default');
$handler->display->display_options['title'] = 'Users with low compliance score';
$handler->display->display_options['use_more_always'] = FALSE;
$handler->display->display_options['access']['type'] = 'perm';
$handler->display->display_options['access']['perm'] = 'access user profiles';
$handler->display->display_options['cache']['type'] = 'none';
$handler->display->display_options['query']['type'] = 'views_query';
$handler->display->display_options['exposed_form']['type'] = 'basic';
$handler->display->display_options['pager']['type'] = 'some';
$handler->display->display_options['pager']['options']['items_per_page'] = '5';
$handler->display->display_options['style_plugin'] = 'table';
/* Field: User: Name */
$handler->display->display_options['fields']['name']['id'] = 'name';
$handler->display->display_options['fields']['name']['table'] = 'users';
$handler->display->display_options['fields']['name']['field'] = 'name';
$handler->display->display_options['fields']['name']['alter']['word_boundary'] = FALSE;
$handler->display->display_options['fields']['name']['alter']['ellipsis'] = FALSE;
/* Field: Bulk operations: User */
$handler->display->display_options['fields']['views_bulk_operations']['id'] = 'views_bulk_operations';
$handler->display->display_options['fields']['views_bulk_operations']['table'] = 'views_entity_user';
$handler->display->display_options['fields']['views_bulk_operations']['field'] = 'views_bulk_operations';
$handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['display_type'] = '0';
$handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['enable_select_all_pages'] = 1;
$handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['row_clickable'] = 1;
$handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['force_single'] = 0;
$handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['entity_load_capacity'] = '10';
/* Sort criterion: User: Compliance score (field_verification_score) */
$handler->display->display_options['sorts']['field_verification_score_value']['id'] = 'field_verification_score_value';
$handler->display->display_options['sorts']['field_verification_score_value']['table'] = 'field_data_field_verification_score';
$handler->display->display_options['sorts']['field_verification_score_value']['field'] = 'field_verification_score_value';
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
$handler->display->display_options['filters']['field_verification_score_value']['operator'] = '<';
$handler->display->display_options['filters']['field_verification_score_value']['value']['value'] = '0';

/* Display: Block */
$handler = $view->new_display('block', 'Block', 'block');
$handler->display->display_options['defaults']['access'] = FALSE;
$handler->display->display_options['access']['type'] = 'role';
$handler->display->display_options['access']['role'] = array(
  3 => '3',
);
$handler->display->display_options['defaults']['fields'] = FALSE;
/* Field: User: Name */
$handler->display->display_options['fields']['name']['id'] = 'name';
$handler->display->display_options['fields']['name']['table'] = 'users';
$handler->display->display_options['fields']['name']['field'] = 'name';
$handler->display->display_options['fields']['name']['alter']['word_boundary'] = FALSE;
$handler->display->display_options['fields']['name']['alter']['ellipsis'] = FALSE;
/* Field: Bulk operations: User */
$handler->display->display_options['fields']['views_bulk_operations']['id'] = 'views_bulk_operations';
$handler->display->display_options['fields']['views_bulk_operations']['table'] = 'views_entity_user';
$handler->display->display_options['fields']['views_bulk_operations']['field'] = 'views_bulk_operations';
$handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['display_type'] = '0';
$handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['enable_select_all_pages'] = 1;
$handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['row_clickable'] = 1;
$handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['force_single'] = 0;
$handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['entity_load_capacity'] = '10';
/* Field: User: Compliance score */
$handler->display->display_options['fields']['field_verification_score']['id'] = 'field_verification_score';
$handler->display->display_options['fields']['field_verification_score']['table'] = 'field_data_field_verification_score';
$handler->display->display_options['fields']['field_verification_score']['field'] = 'field_verification_score';
$handler->display->display_options['fields']['field_verification_score']['label'] = 'Score';
$handler->display->display_options['fields']['field_verification_score']['settings'] = array(
  'thousand_separator' => '',
  'prefix_suffix' => 1,
);
$handler->display->display_options['block_description'] = 'Score warcher';
$translatables['users_with_low_compliance_score'] = array(
  t('Master'),
  t('Users with low compliance score'),
  t('more'),
  t('Apply'),
  t('Reset'),
  t('Sort by'),
  t('Asc'),
  t('Desc'),
  t('Name'),
  t('User'),
  t('- Choose an operation -'),
  t('Block'),
  t('Score'),
  t('Score warcher'),
);

