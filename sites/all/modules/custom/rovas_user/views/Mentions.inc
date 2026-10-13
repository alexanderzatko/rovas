<?php

/**
 * @file
 * Default view: Mentions
 */

$view = new view();
$view->name = 'Mentions';
$view->description = 'Displays @username style Mentions made in all Nodes and Comments.';
$view->tag = '';
$view->base_table = 'mentions';
$view->human_name = '';
$view->core = 0;
$view->api_version = '3.0';
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

/* Display: Defaults */
$handler = $view->new_display('default', 'Defaults', 'default');
$handler->display->display_options['use_more_always'] = FALSE;
$handler->display->display_options['access']['type'] = 'php';
$handler->display->display_options['access']['php_access'] = 'global $user;
return $user->uid == arg(1) || $user->uid ==1;';
$handler->display->display_options['cache']['type'] = 'none';
$handler->display->display_options['query']['type'] = 'views_query';
$handler->display->display_options['exposed_form']['type'] = 'basic';
$handler->display->display_options['pager']['type'] = 'none';
$handler->display->display_options['style_plugin'] = 'table';
$handler->display->display_options['style_options']['columns'] = array(
  'entity_type' => 'entity_type',
  'entity_id' => 'entity_id',
  'title' => 'title',
  'created' => 'created',
  'auid' => 'auid',
  'link' => 'link',
);
$handler->display->display_options['style_options']['default'] = '-1';
$handler->display->display_options['style_options']['info'] = array(
  'entity_type' => array(
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'entity_id' => array(
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'title' => array(
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
  'auid' => array(
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'link' => array(
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
);
/* Header: Global: Text area */
$handler->display->display_options['header']['area']['id'] = 'area';
$handler->display->display_options['header']['area']['table'] = 'views';
$handler->display->display_options['header']['area']['field'] = 'area';
$handler->display->display_options['header']['area']['empty'] = TRUE;
$handler->display->display_options['header']['area']['content'] = 'List of all events when your name was mentioned in a post or a comment.';
$handler->display->display_options['header']['area']['format'] = 'plain_text';
/* Footer: Global: Text area */
$handler->display->display_options['footer']['area']['id'] = 'area';
$handler->display->display_options['footer']['area']['table'] = 'views';
$handler->display->display_options['footer']['area']['field'] = 'area';
$handler->display->display_options['footer']['area']['empty'] = TRUE;
$handler->display->display_options['footer']['area']['content'] = 'How to use mentions: Wrap username of the user you want to mention into brackets and prefix it with the @ sign like this <strong>[@username]</strong>. The mentioned user will be notified with a link to the content where they were mentioned. ';
$handler->display->display_options['footer']['area']['format'] = 'full_html';
/* No results behavior: Global: Text area */
$handler->display->display_options['empty']['area']['id'] = 'area';
$handler->display->display_options['empty']['area']['table'] = 'views';
$handler->display->display_options['empty']['area']['field'] = 'area';
$handler->display->display_options['empty']['area']['empty'] = TRUE;
$handler->display->display_options['empty']['area']['content'] = 'Nobody mentioned you yet.';
$handler->display->display_options['empty']['area']['format'] = 'plain_text';
/* Field: Mentions: Date */
$handler->display->display_options['fields']['created']['id'] = 'created';
$handler->display->display_options['fields']['created']['table'] = 'mentions';
$handler->display->display_options['fields']['created']['field'] = 'created';
$handler->display->display_options['fields']['created']['label'] = 'Time';
$handler->display->display_options['fields']['created']['alter']['text'] = '[created] ago';
$handler->display->display_options['fields']['created']['date_format'] = 'short';
$handler->display->display_options['fields']['created']['second_date_format'] = 'ip2country_date';
/* Field: Mentions: Entity type */
$handler->display->display_options['fields']['entity_type']['id'] = 'entity_type';
$handler->display->display_options['fields']['entity_type']['table'] = 'mentions';
$handler->display->display_options['fields']['entity_type']['field'] = 'entity_type';
$handler->display->display_options['fields']['entity_type']['label'] = '';
$handler->display->display_options['fields']['entity_type']['exclude'] = TRUE;
$handler->display->display_options['fields']['entity_type']['element_label_colon'] = FALSE;
/* Field: Mentions: Entity id */
$handler->display->display_options['fields']['entity_id']['id'] = 'entity_id';
$handler->display->display_options['fields']['entity_id']['table'] = 'mentions';
$handler->display->display_options['fields']['entity_id']['field'] = 'entity_id';
$handler->display->display_options['fields']['entity_id']['label'] = '';
$handler->display->display_options['fields']['entity_id']['exclude'] = TRUE;
$handler->display->display_options['fields']['entity_id']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['entity_id']['separator'] = '';
/* Field: Mentions: Title */
$handler->display->display_options['fields']['title']['id'] = 'title';
$handler->display->display_options['fields']['title']['table'] = 'mentions';
$handler->display->display_options['fields']['title']['field'] = 'title';
$handler->display->display_options['fields']['title']['element_type'] = 'strong';
$handler->display->display_options['fields']['title']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['title']['empty'] = '[entity_type] [entity_id]';
$handler->display->display_options['fields']['title']['hide_alter_empty'] = FALSE;
/* Field: Mentions: Author user id */
$handler->display->display_options['fields']['auid']['id'] = 'auid';
$handler->display->display_options['fields']['auid']['table'] = 'mentions';
$handler->display->display_options['fields']['auid']['field'] = 'auid';
$handler->display->display_options['fields']['auid']['label'] = 'Author';
$handler->display->display_options['fields']['auid']['alter']['text'] = 'by [auid]';
/* Field: Mentions: Link */
$handler->display->display_options['fields']['link']['id'] = 'link';
$handler->display->display_options['fields']['link']['table'] = 'mentions';
$handler->display->display_options['fields']['link']['field'] = 'link';
$handler->display->display_options['fields']['link']['label'] = 'Action';
$handler->display->display_options['fields']['link']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['link']['text'] = 'View';
/* Sort criterion: Mentions: Date */
$handler->display->display_options['sorts']['created']['id'] = 'created';
$handler->display->display_options['sorts']['created']['table'] = 'mentions';
$handler->display->display_options['sorts']['created']['field'] = 'created';
$handler->display->display_options['sorts']['created']['order'] = 'DESC';
/* Contextual filter: Mentions: Mentioned user id */
$handler->display->display_options['arguments']['uid']['id'] = 'uid';
$handler->display->display_options['arguments']['uid']['table'] = 'mentions';
$handler->display->display_options['arguments']['uid']['field'] = 'uid';
$handler->display->display_options['arguments']['uid']['default_action'] = 'default';
$handler->display->display_options['arguments']['uid']['exception']['title_enable'] = TRUE;
$handler->display->display_options['arguments']['uid']['default_argument_type'] = 'current_user';
$handler->display->display_options['arguments']['uid']['summary']['format'] = 'default_summary';
$handler->display->display_options['arguments']['uid']['specify_validation'] = TRUE;
$handler->display->display_options['arguments']['uid']['validate']['type'] = 'user';

/* Display: Page */
$handler = $view->new_display('page', 'Page', 'page_1');
$handler->display->display_options['path'] = 'user/%/mentions';
$handler->display->display_options['menu']['type'] = 'tab';
$handler->display->display_options['menu']['title'] = 'Mentions';
$handler->display->display_options['menu']['description'] = 'Mentions';
$handler->display->display_options['menu']['weight'] = '0';
$translatables['Mentions'] = array(
  t('Defaults'),
  t('more'),
  t('Apply'),
  t('Reset'),
  t('Sort by'),
  t('Asc'),
  t('Desc'),
  t('List of all events when your name was mentioned in a post or a comment.'),
  t('How to use mentions: Wrap username of the user you want to mention into brackets and prefix it with the @ sign like this <strong>[@username]</strong>. The mentioned user will be notified with a link to the content where they were mentioned. '),
  t('Nobody mentioned you yet.'),
  t('Time'),
  t('[created] ago'),
  t('.'),
  t('Title'),
  t('[entity_type] [entity_id]'),
  t('Author'),
  t('by [auid]'),
  t('Action'),
  t('View'),
  t('All'),
  t('Page'),
);

