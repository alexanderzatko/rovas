<?php

/**
 * @file
 * Default view: chron_balance
 */

$view = new view();
$view->name = 'chron_balance';
$view->description = '';
$view->tag = 'default';
$view->base_table = 'users';
$view->human_name = 'Chron balance';
$view->core = 7;
$view->api_version = '3.0';
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

/* Display: Master */
$handler = $view->new_display('default', 'Master', 'default');
$handler->display->display_options['title'] = 'Account balance';
$handler->display->display_options['use_more_always'] = FALSE;
$handler->display->display_options['access']['type'] = 'none';
$handler->display->display_options['cache']['type'] = 'none';
$handler->display->display_options['query']['type'] = 'views_query';
$handler->display->display_options['exposed_form']['type'] = 'basic';
$handler->display->display_options['pager']['type'] = 'some';
$handler->display->display_options['pager']['options']['items_per_page'] = '5';
$handler->display->display_options['style_plugin'] = 'default';
$handler->display->display_options['row_plugin'] = 'fields';
$handler->display->display_options['row_options']['inline'] = array(
  'field_chrons_balance' => 'field_chrons_balance',
);
/* No results behavior: Global: Text area */
$handler->display->display_options['empty']['area']['id'] = 'area';
$handler->display->display_options['empty']['area']['table'] = 'views';
$handler->display->display_options['empty']['area']['field'] = 'area';
$handler->display->display_options['empty']['area']['empty'] = TRUE;
$handler->display->display_options['empty']['area']['content'] = 'You earned no Chrons yet.';
$handler->display->display_options['empty']['area']['format'] = 'filtered_html';
/* Field: User: Actually available Chrons */
$handler->display->display_options['fields']['field_actually_available_chrons']['id'] = 'field_actually_available_chrons';
$handler->display->display_options['fields']['field_actually_available_chrons']['table'] = 'field_data_field_actually_available_chrons';
$handler->display->display_options['fields']['field_actually_available_chrons']['field'] = 'field_actually_available_chrons';
$handler->display->display_options['fields']['field_actually_available_chrons']['label'] = '';
$handler->display->display_options['fields']['field_actually_available_chrons']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_actually_available_chrons']['type'] = 'computed_field_unsanitized';
/* Sort criterion: User: Created date */
$handler->display->display_options['sorts']['created']['id'] = 'created';
$handler->display->display_options['sorts']['created']['table'] = 'users';
$handler->display->display_options['sorts']['created']['field'] = 'created';
$handler->display->display_options['sorts']['created']['order'] = 'DESC';
/* Contextual filter: User: Uid */
$handler->display->display_options['arguments']['uid']['id'] = 'uid';
$handler->display->display_options['arguments']['uid']['table'] = 'users';
$handler->display->display_options['arguments']['uid']['field'] = 'uid';
$handler->display->display_options['arguments']['uid']['default_action'] = 'default';
$handler->display->display_options['arguments']['uid']['default_argument_type'] = 'php';
$handler->display->display_options['arguments']['uid']['default_argument_options']['code'] = 'if (arg(0) == \'user\' && is_numeric(arg(1))) {
return arg(1);
}
else {
global $user;
return $user->uid;
}';
$handler->display->display_options['arguments']['uid']['summary']['number_of_records'] = '0';
$handler->display->display_options['arguments']['uid']['summary']['format'] = 'default_summary';
$handler->display->display_options['arguments']['uid']['summary_options']['items_per_page'] = '25';
/* Filter criterion: User: Active status */
$handler->display->display_options['filters']['status']['id'] = 'status';
$handler->display->display_options['filters']['status']['table'] = 'users';
$handler->display->display_options['filters']['status']['field'] = 'status';
$handler->display->display_options['filters']['status']['value'] = '1';
$handler->display->display_options['filters']['status']['group'] = 1;
$handler->display->display_options['filters']['status']['expose']['operator'] = FALSE;

/* Display: Block */
$handler = $view->new_display('block', 'Block', 'block');
$translatables['chron_balance'] = array(
  t('Master'),
  t('Account balance'),
  t('more'),
  t('Apply'),
  t('Reset'),
  t('Sort by'),
  t('Asc'),
  t('Desc'),
  t('You earned no Chrons yet.'),
  t('All'),
  t('Block'),
);

