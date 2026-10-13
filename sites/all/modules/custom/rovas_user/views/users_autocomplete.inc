<?php

/**
 * @file
 * Default view: users_autocomplete
 */

$view = new view();
$view->name = 'users_autocomplete';
$view->description = '';
$view->tag = 'default';
$view->base_table = 'users';
$view->human_name = 'users-autocomplete';
$view->core = 7;
$view->api_version = '3.0';
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

/* Display: Master */
$handler = $view->new_display('default', 'Master', 'default');
$handler->display->display_options['title'] = 'users_autocomplete';
$handler->display->display_options['use_more_always'] = FALSE;
$handler->display->display_options['use_more_text'] = 'plus';
$handler->display->display_options['access']['type'] = 'none';
$handler->display->display_options['cache']['type'] = 'none';
$handler->display->display_options['query']['type'] = 'views_query';
$handler->display->display_options['query']['options']['query_comment'] = FALSE;
$handler->display->display_options['exposed_form']['type'] = 'basic';
$handler->display->display_options['exposed_form']['options']['submit_button'] = 'Search';
$handler->display->display_options['pager']['type'] = 'some';
$handler->display->display_options['pager']['options']['items_per_page'] = '15';
$handler->display->display_options['pager']['options']['offset'] = '0';
$handler->display->display_options['style_plugin'] = 'search_autocomplete';
$handler->display->display_options['style_options']['group_by'] = '0';
$handler->display->display_options['style_options']['input_label'] = 'field_full_name';
$handler->display->display_options['style_options']['input_link'] = 'mail';
$handler->display->display_options['style_options']['output_fields'] = array(
  'name' => 'name',
  'field_full_name' => 'field_full_name',
);
/* Field: User: Link to user */
$handler->display->display_options['fields']['view_user']['id'] = 'view_user';
$handler->display->display_options['fields']['view_user']['table'] = 'users';
$handler->display->display_options['fields']['view_user']['field'] = 'view_user';
$handler->display->display_options['fields']['view_user']['label'] = '';
$handler->display->display_options['fields']['view_user']['element_label_colon'] = FALSE;
/* Field: User: Picture */
$handler->display->display_options['fields']['picture']['id'] = 'picture';
$handler->display->display_options['fields']['picture']['table'] = 'users';
$handler->display->display_options['fields']['picture']['field'] = 'picture';
$handler->display->display_options['fields']['picture']['label'] = '';
$handler->display->display_options['fields']['picture']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['picture']['link_photo_to_profile'] = FALSE;
$handler->display->display_options['fields']['picture']['image_style'] = 'autocompletion_style';
/* Field: User: Name */
$handler->display->display_options['fields']['name']['id'] = 'name';
$handler->display->display_options['fields']['name']['table'] = 'users';
$handler->display->display_options['fields']['name']['field'] = 'name';
$handler->display->display_options['fields']['name']['label'] = '';
$handler->display->display_options['fields']['name']['alter']['word_boundary'] = FALSE;
$handler->display->display_options['fields']['name']['alter']['ellipsis'] = FALSE;
$handler->display->display_options['fields']['name']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['name']['link_to_user'] = FALSE;
/* Field: User: Full name */
$handler->display->display_options['fields']['field_full_name']['id'] = 'field_full_name';
$handler->display->display_options['fields']['field_full_name']['table'] = 'field_data_field_full_name';
$handler->display->display_options['fields']['field_full_name']['field'] = 'field_full_name';
$handler->display->display_options['fields']['field_full_name']['label'] = '';
$handler->display->display_options['fields']['field_full_name']['alter']['alter_text'] = TRUE;
$handler->display->display_options['fields']['field_full_name']['alter']['text'] = '[name] ([field_full_name])';
$handler->display->display_options['fields']['field_full_name']['element_label_colon'] = FALSE;
/* Field: User: E-mail */
$handler->display->display_options['fields']['mail']['id'] = 'mail';
$handler->display->display_options['fields']['mail']['table'] = 'users';
$handler->display->display_options['fields']['mail']['field'] = 'mail';
$handler->display->display_options['fields']['mail']['label'] = '';
$handler->display->display_options['fields']['mail']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['mail']['link_to_user'] = '0';
/* Sort criterion: User: Full name (field_full_name) */
$handler->display->display_options['sorts']['field_full_name_value']['id'] = 'field_full_name_value';
$handler->display->display_options['sorts']['field_full_name_value']['table'] = 'field_data_field_full_name';
$handler->display->display_options['sorts']['field_full_name_value']['field'] = 'field_full_name_value';
/* Filter criterion: User: Active status */
$handler->display->display_options['filters']['status']['id'] = 'status';
$handler->display->display_options['filters']['status']['table'] = 'users';
$handler->display->display_options['filters']['status']['field'] = 'status';
$handler->display->display_options['filters']['status']['value'] = '1';
$handler->display->display_options['filters']['status']['group'] = 1;
$handler->display->display_options['filters']['status']['expose']['operator'] = FALSE;
/* Filter criterion: User: Roles */
$handler->display->display_options['filters']['rid']['id'] = 'rid';
$handler->display->display_options['filters']['rid']['table'] = 'users_roles';
$handler->display->display_options['filters']['rid']['field'] = 'rid';
$handler->display->display_options['filters']['rid']['value'] = array(
  4 => '4',
);
$handler->display->display_options['filters']['rid']['group'] = 1;
/* Filter criterion: User: Name */
$handler->display->display_options['filters']['uid']['id'] = 'uid';
$handler->display->display_options['filters']['uid']['table'] = 'users';
$handler->display->display_options['filters']['uid']['field'] = 'uid';
$handler->display->display_options['filters']['uid']['operator'] = 'not in';
$handler->display->display_options['filters']['uid']['value'] = array(
  0 => '1',
);
$handler->display->display_options['filters']['uid']['group'] = 1;
/* Filter criterion: Global: Combine fields filter */
$handler->display->display_options['filters']['combine']['id'] = 'combine';
$handler->display->display_options['filters']['combine']['table'] = 'views';
$handler->display->display_options['filters']['combine']['field'] = 'combine';
$handler->display->display_options['filters']['combine']['operator'] = 'contains';
$handler->display->display_options['filters']['combine']['exposed'] = TRUE;
$handler->display->display_options['filters']['combine']['expose']['operator_id'] = 'combine_op';
$handler->display->display_options['filters']['combine']['expose']['label'] = 'Combine fields filter';
$handler->display->display_options['filters']['combine']['expose']['operator'] = 'combine_op';
$handler->display->display_options['filters']['combine']['expose']['identifier'] = 'combine';
$handler->display->display_options['filters']['combine']['expose']['remember_roles'] = array(
  2 => '2',
  1 => 0,
  3 => 0,
  4 => 0,
  6 => 0,
  5 => 0,
);
$handler->display->display_options['filters']['combine']['group_info']['label'] = 'Combine fields filter';
$handler->display->display_options['filters']['combine']['group_info']['identifier'] = 'combine';
$handler->display->display_options['filters']['combine']['group_info']['remember'] = FALSE;
$handler->display->display_options['filters']['combine']['group_info']['group_items'] = array(
  1 => array(),
  2 => array(),
  3 => array(),
);
$handler->display->display_options['filters']['combine']['fields'] = array(
  'name' => 'name',
  'field_full_name' => 'field_full_name',
);

/* Display: Page */
$handler = $view->new_display('page', 'Page', 'page');
$handler->display->display_options['path'] = 'users-autocomplete';
$translatables['users_autocomplete'] = array(
  t('Master'),
  t('users_autocomplete'),
  t('plus'),
  t('Search'),
  t('Reset'),
  t('Sort by'),
  t('Asc'),
  t('Desc'),
  t('[name] ([field_full_name])'),
  t('Combine fields filter'),
  t('Page'),
  t('more'),
);

