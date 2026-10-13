<?php

/**
 * @file
 * Default view: user_countries
 */

$view = new view();
$view->name = 'user_countries';
$view->description = '';
$view->tag = 'default';
$view->base_table = 'users';
$view->human_name = 'user countries';
$view->core = 7;
$view->api_version = '3.0';
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

/* Display: Master */
$handler = $view->new_display('default', 'Master', 'default');
$handler->display->display_options['title'] = 'user countries';
$handler->display->display_options['use_more_always'] = FALSE;
$handler->display->display_options['access']['type'] = 'perm';
$handler->display->display_options['access']['perm'] = 'access user profiles';
$handler->display->display_options['cache']['type'] = 'none';
$handler->display->display_options['query']['type'] = 'views_query';
$handler->display->display_options['exposed_form']['type'] = 'basic';
$handler->display->display_options['pager']['type'] = 'full';
$handler->display->display_options['pager']['options']['items_per_page'] = '10';
$handler->display->display_options['style_plugin'] = 'table';
$handler->display->display_options['style_options']['columns'] = array(
  'name' => 'name',
  'views_bulk_operations' => 'views_bulk_operations',
  'field_country' => 'field_country',
  'language' => 'language',
);
$handler->display->display_options['style_options']['class'] = '';
$handler->display->display_options['style_options']['default'] = 'field_country';
$handler->display->display_options['style_options']['info'] = array(
  'name' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'views_bulk_operations' => array(
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_country' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'language' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
);
/* Field: Bulk operations: User */
$handler->display->display_options['fields']['views_bulk_operations']['id'] = 'views_bulk_operations';
$handler->display->display_options['fields']['views_bulk_operations']['table'] = 'views_entity_user';
$handler->display->display_options['fields']['views_bulk_operations']['field'] = 'views_bulk_operations';
$handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['display_type'] = '0';
$handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['enable_select_all_pages'] = 1;
$handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['row_clickable'] = 1;
$handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['force_single'] = 0;
$handler->display->display_options['fields']['views_bulk_operations']['vbo_settings']['entity_load_capacity'] = '10';
$handler->display->display_options['fields']['views_bulk_operations']['vbo_operations'] = array(
  'action::views_bulk_operations_modify_action' => array(
    'selected' => 1,
    'postpone_processing' => 0,
    'skip_confirmation' => 0,
    'skip_permission_check' => 0,
    'override_label' => 0,
    'label' => '',
    'settings' => array(
      'show_all_tokens' => 1,
      'display_values' => array(
        'language' => 'language',
        'user::field_country' => 'user::field_country',
      ),
    ),
  ),
);
/* Field: User: Name */
$handler->display->display_options['fields']['name']['id'] = 'name';
$handler->display->display_options['fields']['name']['table'] = 'users';
$handler->display->display_options['fields']['name']['field'] = 'name';
$handler->display->display_options['fields']['name']['alter']['word_boundary'] = FALSE;
$handler->display->display_options['fields']['name']['alter']['ellipsis'] = FALSE;
/* Field: Field: Country */
$handler->display->display_options['fields']['field_country']['id'] = 'field_country';
$handler->display->display_options['fields']['field_country']['table'] = 'field_data_field_country';
$handler->display->display_options['fields']['field_country']['field'] = 'field_country';
/* Field: User: Language */
$handler->display->display_options['fields']['language']['id'] = 'language';
$handler->display->display_options['fields']['language']['table'] = 'users';
$handler->display->display_options['fields']['language']['field'] = 'language';
/* Sort criterion: User: Created date */
$handler->display->display_options['sorts']['created']['id'] = 'created';
$handler->display->display_options['sorts']['created']['table'] = 'users';
$handler->display->display_options['sorts']['created']['field'] = 'created';
$handler->display->display_options['sorts']['created']['order'] = 'DESC';
/* Filter criterion: User: Active status */
$handler->display->display_options['filters']['status']['id'] = 'status';
$handler->display->display_options['filters']['status']['table'] = 'users';
$handler->display->display_options['filters']['status']['field'] = 'status';
$handler->display->display_options['filters']['status']['value'] = '1';
$handler->display->display_options['filters']['status']['group'] = 1;
$handler->display->display_options['filters']['status']['expose']['operator'] = FALSE;
/* Filter criterion: Field: Country (field_country) */
$handler->display->display_options['filters']['field_country_iso2']['id'] = 'field_country_iso2';
$handler->display->display_options['filters']['field_country_iso2']['table'] = 'field_data_field_country';
$handler->display->display_options['filters']['field_country_iso2']['field'] = 'field_country_iso2';
$handler->display->display_options['filters']['field_country_iso2']['exposed'] = TRUE;
$handler->display->display_options['filters']['field_country_iso2']['expose']['operator_id'] = 'field_country_iso2_op';
$handler->display->display_options['filters']['field_country_iso2']['expose']['label'] = 'Country';
$handler->display->display_options['filters']['field_country_iso2']['expose']['operator'] = 'field_country_iso2_op';
$handler->display->display_options['filters']['field_country_iso2']['expose']['identifier'] = 'field_country_iso2';
$handler->display->display_options['filters']['field_country_iso2']['expose']['remember_roles'] = array(
  2 => '2',
);
$handler->display->display_options['filters']['field_country_iso2']['expose']['autocomplete_items'] = '10';
$handler->display->display_options['filters']['field_country_iso2']['expose']['autocomplete_min_chars'] = '0';
$handler->display->display_options['filters']['field_country_iso2']['expose']['autocomplete_raw_suggestion'] = 1;
$handler->display->display_options['filters']['field_country_iso2']['expose']['autocomplete_raw_dropdown'] = 1;
$handler->display->display_options['filters']['field_country_iso2']['expose']['autocomplete_dependent'] = 0;
/* Filter criterion: User: Language */
$handler->display->display_options['filters']['language']['id'] = 'language';
$handler->display->display_options['filters']['language']['table'] = 'users';
$handler->display->display_options['filters']['language']['field'] = 'language';
$handler->display->display_options['filters']['language']['exposed'] = TRUE;
$handler->display->display_options['filters']['language']['expose']['operator_id'] = 'language_op';
$handler->display->display_options['filters']['language']['expose']['label'] = 'Language';
$handler->display->display_options['filters']['language']['expose']['operator'] = 'language_op';
$handler->display->display_options['filters']['language']['expose']['identifier'] = 'language';
$handler->display->display_options['filters']['language']['expose']['remember_roles'] = array(
  2 => '2',
);

/* Display: Page */
$handler = $view->new_display('page', 'Page', 'page');
$handler->display->display_options['path'] = 'user-countries';
$translatables['user_countries'] = array(
  t('Master'),
  t('user countries'),
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
  t('User'),
  t('- Choose an operation -'),
  t('Name'),
  t('Country'),
  t('Language'),
  t('Page'),
);

