<?php

/**
 * @file
 * Default view: forex_chart_chron_baskets
 */

$view = new view();
$view->name = 'forex_chart_chron_baskets';
$view->description = '';
$view->tag = 'default';
$view->base_table = 'node';
$view->human_name = 'Forex chart chron baskets';
$view->core = 7;
$view->api_version = '3.0';
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

/* Display: Master */
$handler = $view->new_display('default', 'Master', 'default');
$handler->display->display_options['title'] = 'Forex chart';
$handler->display->display_options['use_more_always'] = FALSE;
$handler->display->display_options['access']['type'] = 'role';
$handler->display->display_options['access']['role'] = array(
  2 => '2',
);
$handler->display->display_options['cache']['type'] = 'none';
$handler->display->display_options['query']['type'] = 'views_query';
$handler->display->display_options['query']['options']['distinct'] = TRUE;
$handler->display->display_options['query']['options']['pure_distinct'] = TRUE;
$handler->display->display_options['exposed_form']['type'] = 'basic';
$handler->display->display_options['pager']['type'] = 'none';
$handler->display->display_options['pager']['options']['offset'] = '0';
$handler->display->display_options['style_plugin'] = 'table';
$handler->display->display_options['style_options']['columns'] = array(
  'title' => 'title',
  'field_chron_basket' => 'field_chron_basket',
  'field_conversion_type' => 'field_conversion_type',
  'field_euro_basket' => 'field_euro_basket',
  'field_amount_paid' => 'field_amount_paid',
  'field_output_amount' => 'field_output_amount',
  'php' => 'php',
);
$handler->display->display_options['style_options']['class'] = '';
$handler->display->display_options['style_options']['default'] = '-1';
$handler->display->display_options['style_options']['info'] = array(
  'title' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_chron_basket' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_conversion_type' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_euro_basket' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_amount_paid' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_output_amount' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'php' => array(
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
);
/* Field: Content: Nid */
$handler->display->display_options['fields']['nid']['id'] = 'nid';
$handler->display->display_options['fields']['nid']['table'] = 'node';
$handler->display->display_options['fields']['nid']['field'] = 'nid';
/* Field: Content: Title */
$handler->display->display_options['fields']['title']['id'] = 'title';
$handler->display->display_options['fields']['title']['table'] = 'node';
$handler->display->display_options['fields']['title']['field'] = 'title';
$handler->display->display_options['fields']['title']['label'] = '';
$handler->display->display_options['fields']['title']['alter']['word_boundary'] = FALSE;
$handler->display->display_options['fields']['title']['alter']['ellipsis'] = FALSE;
$handler->display->display_options['fields']['title']['element_label_colon'] = FALSE;
/* Field: Content: Post date */
$handler->display->display_options['fields']['created']['id'] = 'created';
$handler->display->display_options['fields']['created']['table'] = 'node';
$handler->display->display_options['fields']['created']['field'] = 'created';
$handler->display->display_options['fields']['created']['label'] = 'Date';
$handler->display->display_options['fields']['created']['date_format'] = 'custom';
$handler->display->display_options['fields']['created']['custom_date_format'] = 'd M Y h:i:s';
$handler->display->display_options['fields']['created']['second_date_format'] = 'privatemsg_current_day';
/* Field: Content: Conversion type */
$handler->display->display_options['fields']['field_conversion_type']['id'] = 'field_conversion_type';
$handler->display->display_options['fields']['field_conversion_type']['table'] = 'field_data_field_conversion_type';
$handler->display->display_options['fields']['field_conversion_type']['field'] = 'field_conversion_type';
$handler->display->display_options['fields']['field_conversion_type']['label'] = '';
$handler->display->display_options['fields']['field_conversion_type']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_conversion_type']['element_label_colon'] = FALSE;
/* Field: Content: Chron basket */
$handler->display->display_options['fields']['field_chron_basket']['id'] = 'field_chron_basket';
$handler->display->display_options['fields']['field_chron_basket']['table'] = 'field_data_field_chron_basket';
$handler->display->display_options['fields']['field_chron_basket']['field'] = 'field_chron_basket';
$handler->display->display_options['fields']['field_chron_basket']['label'] = 'Chron CHR->EUR';
$handler->display->display_options['fields']['field_chron_basket']['settings'] = array(
  'thousand_separator' => '',
  'decimal_separator' => '.',
  'scale' => '2',
  'prefix_suffix' => 1,
);
/* Field: Content: Euro basket */
$handler->display->display_options['fields']['field_euro_basket']['id'] = 'field_euro_basket';
$handler->display->display_options['fields']['field_euro_basket']['table'] = 'field_data_field_euro_basket';
$handler->display->display_options['fields']['field_euro_basket']['field'] = 'field_euro_basket';
$handler->display->display_options['fields']['field_euro_basket']['settings'] = array(
  'thousand_separator' => '',
  'decimal_separator' => '.',
  'scale' => '2',
  'prefix_suffix' => 1,
);
/* Field: Content: Paid amount */
$handler->display->display_options['fields']['field_input_amount']['id'] = 'field_input_amount';
$handler->display->display_options['fields']['field_input_amount']['table'] = 'field_data_field_input_amount';
$handler->display->display_options['fields']['field_input_amount']['field'] = 'field_input_amount';
$handler->display->display_options['fields']['field_input_amount']['label'] = '';
$handler->display->display_options['fields']['field_input_amount']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_input_amount']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_input_amount']['type'] = 'number_unformatted';
$handler->display->display_options['fields']['field_input_amount']['settings'] = array(
  'thousand_separator' => '',
  'decimal_separator' => '.',
  'scale' => '2',
  'prefix_suffix' => 1,
);
/* Field: Content: Received amount */
$handler->display->display_options['fields']['field_output_amount']['id'] = 'field_output_amount';
$handler->display->display_options['fields']['field_output_amount']['table'] = 'field_data_field_output_amount';
$handler->display->display_options['fields']['field_output_amount']['field'] = 'field_output_amount';
$handler->display->display_options['fields']['field_output_amount']['label'] = '';
$handler->display->display_options['fields']['field_output_amount']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_output_amount']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_output_amount']['type'] = 'number_unformatted';
$handler->display->display_options['fields']['field_output_amount']['settings'] = array(
  'thousand_separator' => '',
  'decimal_separator' => '.',
  'scale' => '2',
  'prefix_suffix' => 1,
);
/* Field: Global: PHP */
$handler->display->display_options['fields']['php']['id'] = 'php';
$handler->display->display_options['fields']['php']['table'] = 'views';
$handler->display->display_options['fields']['php']['field'] = 'php';
$handler->display->display_options['fields']['php']['label'] = 'rate';
$handler->display->display_options['fields']['php']['use_php_setup'] = 0;
$handler->display->display_options['fields']['php']['php_output'] = '<?php
$static = isset($static) ? $static : 0;

$cHR_basket = $data->field_field_chron_basket[0][\'raw\'][\'value\'];
$eUR_basket = $data->field_field_euro_basket[0][\'raw\'][\'value\'];


if ($eUR_basket > 0 & $cHR_basket > 0) {
		$rate = round($cHR_basket / $eUR_basket,2);
	$static = $rate;
}
else {
	$rate = $static;
}

echo $rate;

?>';
$handler->display->display_options['fields']['php']['use_php_click_sortable'] = '0';
$handler->display->display_options['fields']['php']['php_click_sortable'] = '';
/* Sort criterion: Content: Nid */
$handler->display->display_options['sorts']['nid']['id'] = 'nid';
$handler->display->display_options['sorts']['nid']['table'] = 'node';
$handler->display->display_options['sorts']['nid']['field'] = 'nid';
$handler->display->display_options['sorts']['nid']['order'] = 'DESC';
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
  'forex_transaction' => 'forex_transaction',
);
$handler->display->display_options['filters']['type']['group'] = 1;
/* Filter criterion: Content: Conversion type (field_conversion_type) */
$handler->display->display_options['filters']['field_conversion_type_value']['id'] = 'field_conversion_type_value';
$handler->display->display_options['filters']['field_conversion_type_value']['table'] = 'field_data_field_conversion_type';
$handler->display->display_options['filters']['field_conversion_type_value']['field'] = 'field_conversion_type_value';
$handler->display->display_options['filters']['field_conversion_type_value']['value'] = array(
  'euros_to_chrons' => 'euros_to_chrons',
);
$handler->display->display_options['filters']['field_conversion_type_value']['group'] = 1;
/* Filter criterion: Content: Post date */
$handler->display->display_options['filters']['created']['id'] = 'created';
$handler->display->display_options['filters']['created']['table'] = 'node';
$handler->display->display_options['filters']['created']['field'] = 'created';
$handler->display->display_options['filters']['created']['operator'] = '>';
$handler->display->display_options['filters']['created']['value']['value'] = '- 1 year';
$handler->display->display_options['filters']['created']['value']['type'] = 'offset';
$handler->display->display_options['filters']['created']['exposed'] = TRUE;
$handler->display->display_options['filters']['created']['expose']['operator_id'] = 'created_op';
$handler->display->display_options['filters']['created']['expose']['label'] = 'Post date';
$handler->display->display_options['filters']['created']['expose']['operator'] = 'created_op';
$handler->display->display_options['filters']['created']['expose']['identifier'] = 'created';
$handler->display->display_options['filters']['created']['expose']['remember_roles'] = array(
  2 => '2',
);

/* Display: Page */
$handler = $view->new_display('page', 'Page', 'page');
$handler->display->display_options['defaults']['style_plugin'] = FALSE;
$handler->display->display_options['style_plugin'] = 'chart';
$handler->display->display_options['style_options']['type'] = 'line';
$handler->display->display_options['style_options']['library'] = '';
$handler->display->display_options['style_options']['label_field'] = 'created';
$handler->display->display_options['style_options']['data_fields'] = array(
  'field_chron_basket' => 'field_chron_basket',
  'nid' => 0,
  'title' => 0,
  'created' => 0,
  'field_conversion_type' => 0,
  'field_euro_basket' => 0,
  'field_input_amount' => 0,
  'field_output_amount' => 0,
  'php' => 0,
);
$handler->display->display_options['style_options']['field_colors'] = array(
  'nid' => '#2f7ed8',
  'title' => '#2f7ed8',
  'created' => '#0d233a',
  'field_conversion_type' => '#8bbc21',
  'field_chron_basket' => '#8bbc21',
  'field_euro_basket' => '#910000',
  'field_input_amount' => '#1aadce',
  'field_output_amount' => '#492970',
  'php' => '#f28f43',
);
$handler->display->display_options['style_options']['title'] = 'forex chart';
$handler->display->display_options['style_options']['data_labels'] = 'TRUE';
$handler->display->display_options['style_options']['tooltips'] = 'TRUE';
$handler->display->display_options['style_options']['width'] = '';
$handler->display->display_options['style_options']['height'] = '';
$handler->display->display_options['style_options']['xaxis_title'] = 'date';
$handler->display->display_options['style_options']['xaxis_labels_rotation'] = '45';
$handler->display->display_options['style_options']['yaxis_title'] = 'Chrons in basket';
$handler->display->display_options['style_options']['yaxis_labels_rotation'] = '0';
$handler->display->display_options['defaults']['style_options'] = FALSE;
$handler->display->display_options['defaults']['row_plugin'] = FALSE;
$handler->display->display_options['defaults']['row_options'] = FALSE;
$handler->display->display_options['defaults']['fields'] = FALSE;
/* Field: Content: Nid */
$handler->display->display_options['fields']['nid']['id'] = 'nid';
$handler->display->display_options['fields']['nid']['table'] = 'node';
$handler->display->display_options['fields']['nid']['field'] = 'nid';
/* Field: Content: Title */
$handler->display->display_options['fields']['title']['id'] = 'title';
$handler->display->display_options['fields']['title']['table'] = 'node';
$handler->display->display_options['fields']['title']['field'] = 'title';
$handler->display->display_options['fields']['title']['label'] = '';
$handler->display->display_options['fields']['title']['alter']['word_boundary'] = FALSE;
$handler->display->display_options['fields']['title']['alter']['ellipsis'] = FALSE;
$handler->display->display_options['fields']['title']['element_label_colon'] = FALSE;
/* Field: Content: Post date */
$handler->display->display_options['fields']['created']['id'] = 'created';
$handler->display->display_options['fields']['created']['table'] = 'node';
$handler->display->display_options['fields']['created']['field'] = 'created';
$handler->display->display_options['fields']['created']['label'] = 'Date';
$handler->display->display_options['fields']['created']['date_format'] = 'custom';
$handler->display->display_options['fields']['created']['custom_date_format'] = 'd M Y h:i:s';
$handler->display->display_options['fields']['created']['second_date_format'] = 'privatemsg_current_day';
/* Field: Content: Conversion type */
$handler->display->display_options['fields']['field_conversion_type']['id'] = 'field_conversion_type';
$handler->display->display_options['fields']['field_conversion_type']['table'] = 'field_data_field_conversion_type';
$handler->display->display_options['fields']['field_conversion_type']['field'] = 'field_conversion_type';
$handler->display->display_options['fields']['field_conversion_type']['label'] = '';
$handler->display->display_options['fields']['field_conversion_type']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_conversion_type']['element_label_colon'] = FALSE;
/* Field: Content: Chron basket */
$handler->display->display_options['fields']['field_chron_basket']['id'] = 'field_chron_basket';
$handler->display->display_options['fields']['field_chron_basket']['table'] = 'field_data_field_chron_basket';
$handler->display->display_options['fields']['field_chron_basket']['field'] = 'field_chron_basket';
$handler->display->display_options['fields']['field_chron_basket']['label'] = 'Chron EUR->CHR';
$handler->display->display_options['fields']['field_chron_basket']['settings'] = array(
  'thousand_separator' => '',
  'decimal_separator' => '.',
  'scale' => '2',
  'prefix_suffix' => 1,
);
/* Field: Content: Euro basket */
$handler->display->display_options['fields']['field_euro_basket']['id'] = 'field_euro_basket';
$handler->display->display_options['fields']['field_euro_basket']['table'] = 'field_data_field_euro_basket';
$handler->display->display_options['fields']['field_euro_basket']['field'] = 'field_euro_basket';
$handler->display->display_options['fields']['field_euro_basket']['settings'] = array(
  'thousand_separator' => '',
  'decimal_separator' => '.',
  'scale' => '2',
  'prefix_suffix' => 1,
);
/* Field: Content: Paid amount */
$handler->display->display_options['fields']['field_input_amount']['id'] = 'field_input_amount';
$handler->display->display_options['fields']['field_input_amount']['table'] = 'field_data_field_input_amount';
$handler->display->display_options['fields']['field_input_amount']['field'] = 'field_input_amount';
$handler->display->display_options['fields']['field_input_amount']['label'] = '';
$handler->display->display_options['fields']['field_input_amount']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_input_amount']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_input_amount']['type'] = 'number_unformatted';
$handler->display->display_options['fields']['field_input_amount']['settings'] = array(
  'thousand_separator' => '',
  'decimal_separator' => '.',
  'scale' => '2',
  'prefix_suffix' => 1,
);
/* Field: Content: Received amount */
$handler->display->display_options['fields']['field_output_amount']['id'] = 'field_output_amount';
$handler->display->display_options['fields']['field_output_amount']['table'] = 'field_data_field_output_amount';
$handler->display->display_options['fields']['field_output_amount']['field'] = 'field_output_amount';
$handler->display->display_options['fields']['field_output_amount']['label'] = '';
$handler->display->display_options['fields']['field_output_amount']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_output_amount']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_output_amount']['type'] = 'number_unformatted';
$handler->display->display_options['fields']['field_output_amount']['settings'] = array(
  'thousand_separator' => '',
  'decimal_separator' => '.',
  'scale' => '2',
  'prefix_suffix' => 1,
);
/* Field: Global: PHP */
$handler->display->display_options['fields']['php']['id'] = 'php';
$handler->display->display_options['fields']['php']['table'] = 'views';
$handler->display->display_options['fields']['php']['field'] = 'php';
$handler->display->display_options['fields']['php']['label'] = 'rate';
$handler->display->display_options['fields']['php']['use_php_setup'] = 0;
$handler->display->display_options['fields']['php']['php_output'] = '<?php
$static = isset($static) ? $static : 0;

$cHR_basket = $data->field_field_chron_basket[0][\'raw\'][\'value\'];
$eUR_basket = $data->field_field_euro_basket[0][\'raw\'][\'value\'];


if ($eUR_basket > 0 & $cHR_basket > 0) {
		$rate = round($cHR_basket / $eUR_basket,2);
	$static = $rate;
}
else {
	$rate = $static;
}

echo $rate;

?>';
$handler->display->display_options['fields']['php']['use_php_click_sortable'] = '0';
$handler->display->display_options['fields']['php']['php_click_sortable'] = '';
$handler->display->display_options['defaults']['sorts'] = FALSE;
/* Sort criterion: Content: Nid */
$handler->display->display_options['sorts']['nid']['id'] = 'nid';
$handler->display->display_options['sorts']['nid']['table'] = 'node';
$handler->display->display_options['sorts']['nid']['field'] = 'nid';
$handler->display->display_options['path'] = 'forex-chart-chr-baskets';

/* Display: CHR basket Chr to eur */
$handler = $view->new_display('chart', 'CHR basket Chr to eur', 'chart_3');
$handler->display->display_options['pager']['type'] = 'none';
$handler->display->display_options['pager']['options']['offset'] = '0';
$handler->display->display_options['defaults']['style_plugin'] = FALSE;
$handler->display->display_options['style_plugin'] = 'chart_extension';
$handler->display->display_options['style_options']['type'] = 'line';
$handler->display->display_options['style_options']['library'] = '';
$handler->display->display_options['style_options']['label_field'] = 'created';
$handler->display->display_options['style_options']['data_fields'] = array(
  'field_chron_basket' => 'field_chron_basket',
  'nid' => 0,
  'title' => 0,
  'created' => 0,
  'field_conversion_type' => 0,
  'field_euro_basket' => 0,
  'field_input_amount' => 0,
  'field_output_amount' => 0,
  'php' => 0,
);
$handler->display->display_options['style_options']['field_colors'] = array(
  'nid' => '#2f7ed8',
  'title' => '#0d233a',
  'created' => '#8bbc21',
  'field_conversion_type' => '#910000',
  'field_chron_basket' => '#1aadce',
  'field_euro_basket' => '#492970',
  'field_input_amount' => '#f28f43',
  'field_output_amount' => '#77a1e5',
  'php' => '#c42525',
);
$handler->display->display_options['style_options']['data_labels'] = '';
$handler->display->display_options['style_options']['tooltips'] = '';
$handler->display->display_options['style_options']['width'] = '';
$handler->display->display_options['style_options']['height'] = '';
$handler->display->display_options['style_options']['xaxis_labels_rotation'] = '0';
$handler->display->display_options['style_options']['yaxis_labels_rotation'] = '0';
$handler->display->display_options['defaults']['style_options'] = FALSE;
$handler->display->display_options['defaults']['row_plugin'] = FALSE;
$handler->display->display_options['defaults']['row_options'] = FALSE;
$handler->display->display_options['defaults']['sorts'] = FALSE;
/* Sort criterion: Content: Nid */
$handler->display->display_options['sorts']['nid']['id'] = 'nid';
$handler->display->display_options['sorts']['nid']['table'] = 'node';
$handler->display->display_options['sorts']['nid']['field'] = 'nid';
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
  'forex_transaction' => 'forex_transaction',
);
$handler->display->display_options['filters']['type']['group'] = 1;
/* Filter criterion: Content: Conversion type (field_conversion_type) */
$handler->display->display_options['filters']['field_conversion_type_value']['id'] = 'field_conversion_type_value';
$handler->display->display_options['filters']['field_conversion_type_value']['table'] = 'field_data_field_conversion_type';
$handler->display->display_options['filters']['field_conversion_type_value']['field'] = 'field_conversion_type_value';
$handler->display->display_options['filters']['field_conversion_type_value']['value'] = array(
  'chrons_to_euros' => 'chrons_to_euros',
);
$handler->display->display_options['filters']['field_conversion_type_value']['group'] = 1;
/* Filter criterion: Content: Post date */
$handler->display->display_options['filters']['created']['id'] = 'created';
$handler->display->display_options['filters']['created']['table'] = 'node';
$handler->display->display_options['filters']['created']['field'] = 'created';
$handler->display->display_options['filters']['created']['operator'] = '>';
$handler->display->display_options['filters']['created']['value']['value'] = '- 1 year';
$handler->display->display_options['filters']['created']['value']['type'] = 'offset';
$handler->display->display_options['filters']['created']['exposed'] = TRUE;
$handler->display->display_options['filters']['created']['expose']['operator_id'] = 'created_op';
$handler->display->display_options['filters']['created']['expose']['label'] = 'Post date';
$handler->display->display_options['filters']['created']['expose']['operator'] = 'created_op';
$handler->display->display_options['filters']['created']['expose']['identifier'] = 'created';
$handler->display->display_options['filters']['created']['expose']['remember_roles'] = array(
  2 => '2',
);
$handler->display->display_options['parent_display'] = 'page';
$translatables['forex_chart_chron_baskets'] = array(
  t('Master'),
  t('Forex chart'),
  t('more'),
  t('Apply'),
  t('Reset'),
  t('Sort by'),
  t('Asc'),
  t('Desc'),
  t('Nid'),
  t('Date'),
  t('Chron CHR->EUR'),
  t('Euro basket'),
  t('rate'),
  t('Post date'),
  t('Page'),
  t('Chron EUR->CHR'),
  t('CHR basket Chr to eur'),
);

