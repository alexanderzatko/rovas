<?php

/**
 * @file
 * Default view: total_merits
 */

$view = new view();
$view->name = 'total_merits';
$view->description = '';
$view->tag = 'default';
$view->base_table = 'userpoints_txn';
$view->human_name = 'total merits';
$view->core = 7;
$view->api_version = '3.0';
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

/* Display: Master */
$handler = $view->new_display('default', 'Master', 'default');
$handler->display->display_options['use_more_always'] = FALSE;
$handler->display->display_options['group_by'] = TRUE;
$handler->display->display_options['access']['type'] = 'none';
$handler->display->display_options['cache']['type'] = 'none';
$handler->display->display_options['query']['type'] = 'views_query';
$handler->display->display_options['exposed_form']['type'] = 'basic';
$handler->display->display_options['pager']['type'] = 'none';
$handler->display->display_options['pager']['options']['offset'] = '0';
$handler->display->display_options['style_plugin'] = 'chart';
$handler->display->display_options['style_options']['type'] = 'column';
$handler->display->display_options['style_options']['library'] = '';
$handler->display->display_options['style_options']['label_field'] = 'raw_field';
$handler->display->display_options['style_options']['data_fields'] = array(
  'raw_field' => 'raw_field',
  'points' => 'points',
  'changed' => 0,
);
$handler->display->display_options['style_options']['field_colors'] = array(
  'raw_field' => '#8bbc21',
  'points' => '#2f7ed8',
  'changed' => '#0d233a',
);
$handler->display->display_options['style_options']['title_position'] = '';
$handler->display->display_options['style_options']['data_labels'] = 'TRUE';
$handler->display->display_options['style_options']['tooltips'] = 'TRUE';
$handler->display->display_options['style_options']['width'] = '';
$handler->display->display_options['style_options']['height'] = '400';
$handler->display->display_options['style_options']['xaxis_title'] = 'month/year';
$handler->display->display_options['style_options']['xaxis_labels_rotation'] = '90';
$handler->display->display_options['style_options']['yaxis_labels_rotation'] = '0';
/* Header: Global: PHP */
$handler->display->display_options['header']['php']['id'] = 'php';
$handler->display->display_options['header']['php']['table'] = 'views';
$handler->display->display_options['header']['php']['field'] = 'php';
$handler->display->display_options['header']['php']['php_output'] = '<div class="neo-merits">
<?php
$total = 0;
foreach ($results as $grant) {
	$total += $grant->userpoints_txn_points;
}
$merdays = _rb_format_merits_to_time ($total);

echo \'<div class="start-date">\';
echo t(\'Value created since @startdate\',array(\'@startdate\' => $results[0]->raw_sql_field));
echo \'</div>\';
echo \'<div class="value">\';
echo \'<div class="merits">\';
echo format_plural(number_format($total,0,\'.\',\' \'), \'1 Merit\', \'@count Merits\');
echo \'</div>\';
echo \'<div class="mer-days">\';
echo t(\'equivalent to <span class="hum-red">@mertime</span> of labor time\',array(\'@mertime\' => $merdays));
echo \'<div>\';
echo \'</div>\';

?>
</div>
';
/* Relationship: Userpoints Transactions: User */
$handler->display->display_options['relationships']['uid']['id'] = 'uid';
$handler->display->display_options['relationships']['uid']['table'] = 'userpoints_txn';
$handler->display->display_options['relationships']['uid']['field'] = 'uid';
/* Field: Raw SQL: Raw field */
$handler->display->display_options['fields']['raw_field']['id'] = 'raw_field';
$handler->display->display_options['fields']['raw_field']['table'] = 'views_raw_sql';
$handler->display->display_options['fields']['raw_field']['field'] = 'raw_field';
$handler->display->display_options['fields']['raw_field']['raw_sql'] = 'DATE_FORMAT(FROM_UNIXTIME(userpoints_txn.changed), \'%b %Y\')';
/* Field: SUM(Userpoints Transactions: Points) */
$handler->display->display_options['fields']['points']['id'] = 'points';
$handler->display->display_options['fields']['points']['table'] = 'userpoints_txn';
$handler->display->display_options['fields']['points']['field'] = 'points';
$handler->display->display_options['fields']['points']['group_type'] = 'sum';
$handler->display->display_options['fields']['points']['label'] = 'Merits';
/* Field: COUNT(Userpoints Transactions: Changed) */
$handler->display->display_options['fields']['changed']['id'] = 'changed';
$handler->display->display_options['fields']['changed']['table'] = 'userpoints_txn';
$handler->display->display_options['fields']['changed']['field'] = 'changed';
$handler->display->display_options['fields']['changed']['group_type'] = 'count';
$handler->display->display_options['fields']['changed']['label'] = '';
$handler->display->display_options['fields']['changed']['exclude'] = TRUE;
$handler->display->display_options['fields']['changed']['element_label_colon'] = FALSE;
/* Sort criterion: Raw SQL: Raw sort */
$handler->display->display_options['sorts']['raw_sort']['id'] = 'raw_sort';
$handler->display->display_options['sorts']['raw_sort']['table'] = 'views_raw_sql';
$handler->display->display_options['sorts']['raw_sort']['field'] = 'raw_sort';
$handler->display->display_options['sorts']['raw_sort']['raw_sql'] = 'DATE_FORMAT(FROM_UNIXTIME(userpoints_txn.changed), \'%Y%m\')';
/* Filter criterion: Userpoints Transactions: Points */
$handler->display->display_options['filters']['points']['id'] = 'points';
$handler->display->display_options['filters']['points']['table'] = 'userpoints_txn';
$handler->display->display_options['filters']['points']['field'] = 'points';
$handler->display->display_options['filters']['points']['operator'] = '>';
$handler->display->display_options['filters']['points']['value']['value'] = '0';
$handler->display->display_options['filters']['points']['group'] = 1;
/* Filter criterion: User: Name */
$handler->display->display_options['filters']['uid']['id'] = 'uid';
$handler->display->display_options['filters']['uid']['table'] = 'users';
$handler->display->display_options['filters']['uid']['field'] = 'uid';
$handler->display->display_options['filters']['uid']['relationship'] = 'uid';
$handler->display->display_options['filters']['uid']['operator'] = 'not in';
$handler->display->display_options['filters']['uid']['value'] = array(
  0 => '1',
);

/* Display: Page */
$handler = $view->new_display('page', 'Page', 'page_1');
$handler->display->display_options['defaults']['style_plugin'] = FALSE;
$handler->display->display_options['style_plugin'] = 'table';
$handler->display->display_options['style_options']['columns'] = array(
  'raw_field' => 'raw_field',
  'points' => 'points',
  'changed' => 'changed',
);
$handler->display->display_options['style_options']['class'] = '';
$handler->display->display_options['style_options']['default'] = '-1';
$handler->display->display_options['style_options']['info'] = array(
  'raw_field' => array(
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'points' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'changed' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
);
$handler->display->display_options['defaults']['style_options'] = FALSE;
$handler->display->display_options['defaults']['row_plugin'] = FALSE;
$handler->display->display_options['defaults']['row_options'] = FALSE;
$handler->display->display_options['path'] = 'total-merits-history';

/* Display: Block */
$handler = $view->new_display('block', 'Block', 'block_1');
$translatables['total_merits'] = array(
  t('Master'),
  t('more'),
  t('Apply'),
  t('Reset'),
  t('Sort by'),
  t('Asc'),
  t('Desc'),
  t('Users'),
  t('Raw field'),
  t('Merits'),
  t('.'),
  t(','),
  t('Page'),
  t('Block'),
);

