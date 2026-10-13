<?php

/**
 * @file
 * Default view: uc_catalog
 */

$view = new view();
$view->name = 'uc_catalog';
$view->description = 'Catalog';
$view->tag = '';
$view->base_table = 'node';
$view->human_name = '';
$view->core = 0;
$view->api_version = '3.0';
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

/* Display: Defaults */
$handler = $view->new_display('default', 'Defaults', 'default');
$handler->display->display_options['title'] = 'Catalog';
$handler->display->display_options['use_more_always'] = FALSE;
$handler->display->display_options['access']['type'] = 'none';
$handler->display->display_options['cache']['type'] = 'none';
$handler->display->display_options['query']['type'] = 'views_query';
$handler->display->display_options['exposed_form']['type'] = 'basic';
$handler->display->display_options['pager']['type'] = 'full';
$handler->display->display_options['style_plugin'] = 'table';
$handler->display->display_options['style_options']['columns'] = array(
  'title' => 'title',
  'buyitnowbutton' => 'buyitnowbutton',
);
$handler->display->display_options['style_options']['default'] = '-1';
$handler->display->display_options['style_options']['info'] = array(
  'title' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'buyitnowbutton' => array(
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
);
/* Field: Content: Title */
$handler->display->display_options['fields']['title']['id'] = 'title';
$handler->display->display_options['fields']['title']['table'] = 'node';
$handler->display->display_options['fields']['title']['field'] = 'title';
$handler->display->display_options['fields']['title']['label'] = 'Product';
/* Field: Broken/missing handler */
$handler->display->display_options['fields']['buyitnowbutton']['id'] = 'buyitnowbutton';
$handler->display->display_options['fields']['buyitnowbutton']['table'] = 'uc_products';
$handler->display->display_options['fields']['buyitnowbutton']['field'] = 'buyitnowbutton';
$handler->display->display_options['fields']['buyitnowbutton']['label'] = '';
$handler->display->display_options['fields']['buyitnowbutton']['element_label_colon'] = FALSE;
/* Contextual filter: Content: Has taxonomy term ID (with depth) */
$handler->display->display_options['arguments']['term_node_tid_depth']['id'] = 'term_node_tid_depth';
$handler->display->display_options['arguments']['term_node_tid_depth']['table'] = 'node';
$handler->display->display_options['arguments']['term_node_tid_depth']['field'] = 'term_node_tid_depth';
$handler->display->display_options['arguments']['term_node_tid_depth']['default_action'] = 'empty';
$handler->display->display_options['arguments']['term_node_tid_depth']['title_enable'] = TRUE;
$handler->display->display_options['arguments']['term_node_tid_depth']['title'] = '%1';
$handler->display->display_options['arguments']['term_node_tid_depth']['breadcrumb_enable'] = TRUE;
$handler->display->display_options['arguments']['term_node_tid_depth']['breadcrumb'] = 'Catalog';
$handler->display->display_options['arguments']['term_node_tid_depth']['default_argument_type'] = 'taxonomy_tid';
$handler->display->display_options['arguments']['term_node_tid_depth']['summary']['format'] = 'default_summary';
$handler->display->display_options['arguments']['term_node_tid_depth']['specify_validation'] = TRUE;
$handler->display->display_options['arguments']['term_node_tid_depth']['validate']['type'] = 'taxonomy_term';
$handler->display->display_options['arguments']['term_node_tid_depth']['validate_options']['vocabularies'] = array(
  'catalog' => 'catalog',
);
$handler->display->display_options['arguments']['term_node_tid_depth']['validate']['fail'] = 'empty';
$handler->display->display_options['arguments']['term_node_tid_depth']['depth'] = '0';
$handler->display->display_options['arguments']['term_node_tid_depth']['set_breadcrumb'] = TRUE;
/* Filter criterion: Content: Published status */
$handler->display->display_options['filters']['status']['id'] = 'status';
$handler->display->display_options['filters']['status']['table'] = 'node';
$handler->display->display_options['filters']['status']['field'] = 'status';
$handler->display->display_options['filters']['status']['value'] = '1';
/* Filter criterion: Broken/missing handler */
$handler->display->display_options['filters']['is_product']['id'] = 'is_product';
$handler->display->display_options['filters']['is_product']['table'] = 'uc_products';
$handler->display->display_options['filters']['is_product']['field'] = 'is_product';
$handler->display->display_options['filters']['is_product']['value'] = '1';
/* Filter criterion: Content: Language */
$handler->display->display_options['filters']['language']['id'] = 'language';
$handler->display->display_options['filters']['language']['table'] = 'node';
$handler->display->display_options['filters']['language']['field'] = 'language';
$handler->display->display_options['filters']['language']['value'] = array(
  '***CURRENT_LANGUAGE***' => '***CURRENT_LANGUAGE***',
);

/* Display: Table */
$handler = $view->new_display('page', 'Table', 'catalog');
$handler->display->display_options['path'] = '_catalog';

/* Display: Grid */
$handler = $view->new_display('page', 'Grid', 'catalog_grid');
$handler->display->display_options['defaults']['css_class'] = FALSE;
$handler->display->display_options['css_class'] = 'uc-catalog';
$handler->display->display_options['defaults']['style_plugin'] = FALSE;
$handler->display->display_options['style_plugin'] = 'grid';
$handler->display->display_options['style_options']['columns'] = '3';
$handler->display->display_options['defaults']['style_options'] = FALSE;
$handler->display->display_options['defaults']['row_plugin'] = FALSE;
$handler->display->display_options['row_plugin'] = 'fields';
$handler->display->display_options['defaults']['row_options'] = FALSE;
$handler->display->display_options['defaults']['fields'] = FALSE;
/* Field: Content: Title */
$handler->display->display_options['fields']['title']['id'] = 'title';
$handler->display->display_options['fields']['title']['table'] = 'node';
$handler->display->display_options['fields']['title']['field'] = 'title';
$handler->display->display_options['fields']['title']['label'] = '';
/* Field: Broken/missing handler */
$handler->display->display_options['fields']['model']['id'] = 'model';
$handler->display->display_options['fields']['model']['table'] = 'uc_products';
$handler->display->display_options['fields']['model']['field'] = 'model';
$handler->display->display_options['fields']['model']['label'] = '';
/* Field: Broken/missing handler */
$handler->display->display_options['fields']['entity_id']['id'] = 'entity_id';
$handler->display->display_options['fields']['entity_id']['table'] = 'field_data_uc_product_image';
$handler->display->display_options['fields']['entity_id']['field'] = 'uc_product_image';
$handler->display->display_options['fields']['entity_id']['label'] = '';
$handler->display->display_options['fields']['entity_id']['hide_empty'] = TRUE;
/* Field: Broken/missing handler */
$handler->display->display_options['fields']['display_price']['id'] = 'display_price';
$handler->display->display_options['fields']['display_price']['table'] = 'uc_products';
$handler->display->display_options['fields']['display_price']['field'] = 'display_price';
$handler->display->display_options['fields']['display_price']['label'] = '';
/* Field: Broken/missing handler */
$handler->display->display_options['fields']['buyitnowbutton']['id'] = 'buyitnowbutton';
$handler->display->display_options['fields']['buyitnowbutton']['table'] = 'uc_products';
$handler->display->display_options['fields']['buyitnowbutton']['field'] = 'buyitnowbutton';
$handler->display->display_options['fields']['buyitnowbutton']['label'] = '';
$handler->display->display_options['path'] = '_catalog_grid';
$translatables['uc_catalog'] = array(
  t('Defaults'),
  t('Catalog'),
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
  t('Product'),
  t('All'),
  t('%1'),
  t('Table'),
  t('Grid'),
);

