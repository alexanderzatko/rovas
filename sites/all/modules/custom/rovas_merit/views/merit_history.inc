<?php

/**
 * @file
 * Default view: merit_history
 */

$view = new view();
$view->name = 'merit_history';
$view->description = '';
$view->tag = 'default';
$view->base_table = 'node';
$view->human_name = 'Merit history';
$view->core = 7;
$view->api_version = '3.0';
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

/* Display: Master */
$handler = $view->new_display('default', 'Master', 'default');
$handler->display->display_options['title'] = 'User\'s Merits';
$handler->display->display_options['use_ajax'] = TRUE;
$handler->display->display_options['use_more_always'] = FALSE;
$handler->display->display_options['access']['type'] = 'role';
$handler->display->display_options['access']['role'] = array(
  2 => '2',
);
$handler->display->display_options['cache']['type'] = 'none';
$handler->display->display_options['query']['type'] = 'views_query';
$handler->display->display_options['query']['options']['distinct'] = TRUE;
$handler->display->display_options['query']['options']['pure_distinct'] = TRUE;
$handler->display->display_options['exposed_form']['type'] = 'better_exposed_filters';
$handler->display->display_options['exposed_form']['options']['reset_button'] = TRUE;
$handler->display->display_options['exposed_form']['options']['bef'] = array(
  'general' => array(
    'input_required' => 0,
    'text_input_required' => array(
      'text_input_required' => array(
        'value' => 'Select any filter and click on Apply to see results',
        'format' => 'filtered_html',
      ),
    ),
    'allow_secondary' => 0,
    'secondary_label' => 'Advanced options',
    'secondary_collapse_override' => '0',
  ),
  'field_reward_amount_value' => array(
    'bef_format' => 'bef_slider',
    'slider_options' => array(
      'bef_slider_min' => '0',
      'bef_slider_max' => '99999',
      'bef_slider_step' => '1',
      'bef_slider_animate' => '',
      'bef_slider_orientation' => 'horizontal',
    ),
    'more_options' => array(
      'autosubmit' => 0,
      'is_secondary' => 0,
      'any_label' => '',
      'bef_filter_description' => '',
      'tokens' => array(
        'available' => array(
          0 => 'global_types',
        ),
      ),
      'rewrite' => array(
        'filter_rewrite_values' => '',
      ),
    ),
  ),
  'uid' => array(
    'bef_format' => 'default',
    'more_options' => array(
      'bef_select_all_none' => FALSE,
      'bef_collapsible' => 0,
      'autosubmit' => 0,
      'is_secondary' => 0,
      'any_label' => '',
      'bef_filter_description' => '',
      'tokens' => array(
        'available' => array(
          0 => 'global_types',
        ),
      ),
      'rewrite' => array(
        'filter_rewrite_values' => '',
      ),
    ),
  ),
);
$handler->display->display_options['pager']['type'] = 'full';
$handler->display->display_options['pager']['options']['items_per_page'] = '10';
$handler->display->display_options['pager']['options']['offset'] = '0';
$handler->display->display_options['pager']['options']['id'] = '0';
$handler->display->display_options['pager']['options']['quantity'] = '3';
$handler->display->display_options['pager']['options']['tags']['first'] = '«';
$handler->display->display_options['pager']['options']['tags']['previous'] = '‹';
$handler->display->display_options['pager']['options']['tags']['next'] = '›';
$handler->display->display_options['pager']['options']['tags']['last'] = '»';
$handler->display->display_options['style_plugin'] = 'table';
$handler->display->display_options['style_options']['columns'] = array(
  'nid' => 'nid',
  'field_rewarded_report' => 'field_rewarded_report',
  'created' => 'created',
  'field_receipt_project' => 'field_receipt_project',
  'php' => 'php',
  'field_buyer' => 'field_buyer',
  'field_amount_paid' => 'field_amount_paid',
  'field_reward_amount' => 'field_reward_amount',
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
  'field_rewarded_report' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'created' => array(
    'sortable' => 1,
    'default_sort_order' => 'desc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_receipt_project' => array(
    'sortable' => 1,
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
  'field_buyer' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_amount_paid' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_reward_amount' => array(
    'sortable' => 1,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
);
/* No results behavior: Global: Text area */
$handler->display->display_options['empty']['area']['id'] = 'area';
$handler->display->display_options['empty']['area']['table'] = 'views';
$handler->display->display_options['empty']['area']['field'] = 'area';
$handler->display->display_options['empty']['area']['empty'] = TRUE;
$handler->display->display_options['empty']['area']['content'] = 'No records found';
$handler->display->display_options['empty']['area']['format'] = 'plain_text';
/* Relationship: Entity Reference: Referenced Entity */
$handler->display->display_options['relationships']['field_parent_purchase_receipt_target_id']['id'] = 'field_parent_purchase_receipt_target_id';
$handler->display->display_options['relationships']['field_parent_purchase_receipt_target_id']['table'] = 'field_data_field_parent_purchase_receipt';
$handler->display->display_options['relationships']['field_parent_purchase_receipt_target_id']['field'] = 'field_parent_purchase_receipt_target_id';
$handler->display->display_options['relationships']['field_parent_purchase_receipt_target_id']['label'] = 'purchase_receipt';
/* Relationship: Entity Reference: Referenced Entity */
$handler->display->display_options['relationships']['field_buyer_target_id']['id'] = 'field_buyer_target_id';
$handler->display->display_options['relationships']['field_buyer_target_id']['table'] = 'field_data_field_buyer';
$handler->display->display_options['relationships']['field_buyer_target_id']['field'] = 'field_buyer_target_id';
$handler->display->display_options['relationships']['field_buyer_target_id']['relationship'] = 'field_parent_purchase_receipt_target_id';
$handler->display->display_options['relationships']['field_buyer_target_id']['label'] = 'User';
/* Relationship: Entity Reference: Referenced Entity */
$handler->display->display_options['relationships']['field_receipt_project_target_id']['id'] = 'field_receipt_project_target_id';
$handler->display->display_options['relationships']['field_receipt_project_target_id']['table'] = 'field_data_field_receipt_project';
$handler->display->display_options['relationships']['field_receipt_project_target_id']['field'] = 'field_receipt_project_target_id';
$handler->display->display_options['relationships']['field_receipt_project_target_id']['relationship'] = 'field_parent_purchase_receipt_target_id';
$handler->display->display_options['relationships']['field_receipt_project_target_id']['label'] = 'source';
/* Relationship: Userpoints Transactions: Node */
$handler->display->display_options['relationships']['node']['id'] = 'node';
$handler->display->display_options['relationships']['node']['table'] = 'userpoints_txn';
$handler->display->display_options['relationships']['node']['field'] = 'node';
$handler->display->display_options['relationships']['node']['relationship'] = 'field_buyer_target_id';
/* Field: Content: Nid */
$handler->display->display_options['fields']['nid']['id'] = 'nid';
$handler->display->display_options['fields']['nid']['table'] = 'node';
$handler->display->display_options['fields']['nid']['field'] = 'nid';
$handler->display->display_options['fields']['nid']['label'] = 'Receipt Id';
$handler->display->display_options['fields']['nid']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['nid']['link_to_node'] = TRUE;
/* Field: Content: Rewarded report */
$handler->display->display_options['fields']['field_rewarded_report']['id'] = 'field_rewarded_report';
$handler->display->display_options['fields']['field_rewarded_report']['table'] = 'field_data_field_rewarded_report';
$handler->display->display_options['fields']['field_rewarded_report']['field'] = 'field_rewarded_report';
$handler->display->display_options['fields']['field_rewarded_report']['relationship'] = 'field_parent_purchase_receipt_target_id';
$handler->display->display_options['fields']['field_rewarded_report']['label'] = '';
$handler->display->display_options['fields']['field_rewarded_report']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_rewarded_report']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_rewarded_report']['settings'] = array(
  'bypass_access' => 0,
  'link' => 1,
);
/* Field: Content: Post date */
$handler->display->display_options['fields']['created_1']['id'] = 'created_1';
$handler->display->display_options['fields']['created_1']['table'] = 'node';
$handler->display->display_options['fields']['created_1']['field'] = 'created';
$handler->display->display_options['fields']['created_1']['label'] = 'Date';
$handler->display->display_options['fields']['created_1']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['created_1']['date_format'] = 'short_timestamp_with_spelled_month';
$handler->display->display_options['fields']['created_1']['second_date_format'] = 'privatemsg_current_day';
/* Field: Content: Project */
$handler->display->display_options['fields']['field_receipt_project']['id'] = 'field_receipt_project';
$handler->display->display_options['fields']['field_receipt_project']['table'] = 'field_data_field_receipt_project';
$handler->display->display_options['fields']['field_receipt_project']['field'] = 'field_receipt_project';
$handler->display->display_options['fields']['field_receipt_project']['relationship'] = 'field_parent_purchase_receipt_target_id';
$handler->display->display_options['fields']['field_receipt_project']['label'] = '';
$handler->display->display_options['fields']['field_receipt_project']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_receipt_project']['alter']['alter_text'] = TRUE;
$handler->display->display_options['fields']['field_receipt_project']['alter']['text'] = '[field_receipt_project] ([field_rewarded_report])';
$handler->display->display_options['fields']['field_receipt_project']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_receipt_project']['settings'] = array(
  'bypass_access' => 0,
  'link' => 1,
);
/* Field: Global: PHP */
$handler->display->display_options['fields']['php']['id'] = 'php';
$handler->display->display_options['fields']['php']['table'] = 'views';
$handler->display->display_options['fields']['php']['field'] = 'php';
$handler->display->display_options['fields']['php']['label'] = 'Source';
$handler->display->display_options['fields']['php']['use_php_setup'] = 0;
$handler->display->display_options['fields']['php']['php_output'] = '<?php
module_load_include(\'inc\', \'rovas_civic\', \'rovas_civic.me_sources\');
if (function_exists(\'rovas_civic_me_sources_merit_history_source_html\')) {
  echo rovas_civic_me_sources_merit_history_source_html($view, $data);
}
?>';
$handler->display->display_options['fields']['php']['use_php_click_sortable'] = '0';
$handler->display->display_options['fields']['php']['php_click_sortable'] = '';
/* Field: Content: Buyer */
$handler->display->display_options['fields']['field_buyer']['id'] = 'field_buyer';
$handler->display->display_options['fields']['field_buyer']['table'] = 'field_data_field_buyer';
$handler->display->display_options['fields']['field_buyer']['field'] = 'field_buyer';
$handler->display->display_options['fields']['field_buyer']['relationship'] = 'field_parent_purchase_receipt_target_id';
$handler->display->display_options['fields']['field_buyer']['settings'] = array(
  'bypass_access' => 0,
  'link' => 1,
);
/* Field: Content: Amount Paid */
$handler->display->display_options['fields']['field_amount_paid']['id'] = 'field_amount_paid';
$handler->display->display_options['fields']['field_amount_paid']['table'] = 'field_data_field_amount_paid';
$handler->display->display_options['fields']['field_amount_paid']['field'] = 'field_amount_paid';
$handler->display->display_options['fields']['field_amount_paid']['relationship'] = 'field_parent_purchase_receipt_target_id';
$handler->display->display_options['fields']['field_amount_paid']['settings'] = array(
  'thousand_separator' => '',
  'decimal_separator' => '.',
  'scale' => '2',
  'prefix_suffix' => 1,
);
/* Field: Content: Reward amount */
$handler->display->display_options['fields']['field_reward_amount']['id'] = 'field_reward_amount';
$handler->display->display_options['fields']['field_reward_amount']['table'] = 'field_data_field_reward_amount';
$handler->display->display_options['fields']['field_reward_amount']['field'] = 'field_reward_amount';
$handler->display->display_options['fields']['field_reward_amount']['label'] = 'Reward';
$handler->display->display_options['fields']['field_reward_amount']['alter']['alter_text'] = TRUE;
$handler->display->display_options['fields']['field_reward_amount']['alter']['text'] = '[field_reward_amount-value] ME';
$handler->display->display_options['fields']['field_reward_amount']['alter']['trim_whitespace'] = TRUE;
$handler->display->display_options['fields']['field_reward_amount']['type'] = 'custom_formatters_merits_formatter';
$handler->display->display_options['fields']['field_reward_amount']['settings'] = array(
  'thousand_separator' => '',
  'decimal_separator' => '.',
  'scale' => '10',
  'prefix_suffix' => 1,
);
/* Sort criterion: Content: Post date */
$handler->display->display_options['sorts']['created']['id'] = 'created';
$handler->display->display_options['sorts']['created']['table'] = 'node';
$handler->display->display_options['sorts']['created']['field'] = 'created';
$handler->display->display_options['sorts']['created']['order'] = 'DESC';
/* Contextual filter: Content: Reward recipient (field_reward_recipient) */
$handler->display->display_options['arguments']['field_reward_recipient_target_id']['id'] = 'field_reward_recipient_target_id';
$handler->display->display_options['arguments']['field_reward_recipient_target_id']['table'] = 'field_data_field_reward_recipient';
$handler->display->display_options['arguments']['field_reward_recipient_target_id']['field'] = 'field_reward_recipient_target_id';
$handler->display->display_options['arguments']['field_reward_recipient_target_id']['default_action'] = 'default';
$handler->display->display_options['arguments']['field_reward_recipient_target_id']['default_argument_type'] = 'user';
$handler->display->display_options['arguments']['field_reward_recipient_target_id']['default_argument_options']['user'] = FALSE;
$handler->display->display_options['arguments']['field_reward_recipient_target_id']['summary']['number_of_records'] = '0';
$handler->display->display_options['arguments']['field_reward_recipient_target_id']['summary']['format'] = 'default_summary';
$handler->display->display_options['arguments']['field_reward_recipient_target_id']['summary_options']['items_per_page'] = '25';
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
  'merit_penalty' => 'merit_penalty',
  'merits_disbursement' => 'merits_disbursement',
);
$handler->display->display_options['filters']['type']['group'] = 1;
/* Filter criterion: Field: Title (title_field) */
$handler->display->display_options['filters']['title_field_value']['id'] = 'title_field_value';
$handler->display->display_options['filters']['title_field_value']['table'] = 'field_data_title_field';
$handler->display->display_options['filters']['title_field_value']['field'] = 'title_field_value';
$handler->display->display_options['filters']['title_field_value']['relationship'] = 'field_receipt_project_target_id';
$handler->display->display_options['filters']['title_field_value']['operator'] = 'contains';
$handler->display->display_options['filters']['title_field_value']['group'] = 1;
$handler->display->display_options['filters']['title_field_value']['exposed'] = TRUE;
$handler->display->display_options['filters']['title_field_value']['expose']['operator_id'] = 'title_field_value_op';
$handler->display->display_options['filters']['title_field_value']['expose']['label'] = 'Project';
$handler->display->display_options['filters']['title_field_value']['expose']['description'] = 'Type the project title or its part';
$handler->display->display_options['filters']['title_field_value']['expose']['operator'] = 'title_field_value_op';
$handler->display->display_options['filters']['title_field_value']['expose']['identifier'] = 'title_field_value';
$handler->display->display_options['filters']['title_field_value']['expose']['remember_roles'] = array(
  2 => '2',
);
$handler->display->display_options['filters']['title_field_value']['expose']['autocomplete_items'] = '10';
$handler->display->display_options['filters']['title_field_value']['expose']['autocomplete_min_chars'] = '0';
$handler->display->display_options['filters']['title_field_value']['expose']['autocomplete_raw_suggestion'] = 1;
$handler->display->display_options['filters']['title_field_value']['expose']['autocomplete_raw_dropdown'] = 1;
$handler->display->display_options['filters']['title_field_value']['expose']['autocomplete_dependent'] = 0;
/* Filter criterion: User: Name */
$handler->display->display_options['filters']['uid']['id'] = 'uid';
$handler->display->display_options['filters']['uid']['table'] = 'users';
$handler->display->display_options['filters']['uid']['field'] = 'uid';
$handler->display->display_options['filters']['uid']['relationship'] = 'field_buyer_target_id';
$handler->display->display_options['filters']['uid']['value'] = '';
$handler->display->display_options['filters']['uid']['group'] = 1;
$handler->display->display_options['filters']['uid']['exposed'] = TRUE;
$handler->display->display_options['filters']['uid']['expose']['operator_id'] = 'uid_op';
$handler->display->display_options['filters']['uid']['expose']['label'] = 'Buyer';
$handler->display->display_options['filters']['uid']['expose']['operator'] = 'uid_op';
$handler->display->display_options['filters']['uid']['expose']['identifier'] = 'uid';
$handler->display->display_options['filters']['uid']['expose']['remember_roles'] = array(
  2 => '2',
  1 => 0,
  3 => 0,
  9 => 0,
  11 => 0,
  10 => 0,
  4 => 0,
  6 => 0,
  5 => 0,
  7 => 0,
  8 => 0,
);

/* Display: Page */
$handler = $view->new_display('page', 'Page', 'page_1');
$handler->display->display_options['defaults']['fields'] = FALSE;
/* Field: Content: Nid */
$handler->display->display_options['fields']['nid']['id'] = 'nid';
$handler->display->display_options['fields']['nid']['table'] = 'node';
$handler->display->display_options['fields']['nid']['field'] = 'nid';
$handler->display->display_options['fields']['nid']['label'] = 'Receipt Id';
$handler->display->display_options['fields']['nid']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['nid']['link_to_node'] = TRUE;
/* Field: Content: Rewarded report */
$handler->display->display_options['fields']['field_rewarded_report']['id'] = 'field_rewarded_report';
$handler->display->display_options['fields']['field_rewarded_report']['table'] = 'field_data_field_rewarded_report';
$handler->display->display_options['fields']['field_rewarded_report']['field'] = 'field_rewarded_report';
$handler->display->display_options['fields']['field_rewarded_report']['relationship'] = 'field_parent_purchase_receipt_target_id';
$handler->display->display_options['fields']['field_rewarded_report']['label'] = '';
$handler->display->display_options['fields']['field_rewarded_report']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_rewarded_report']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_rewarded_report']['settings'] = array(
  'bypass_access' => 0,
  'link' => 1,
);
/* Field: Content: Post date */
$handler->display->display_options['fields']['created_1']['id'] = 'created_1';
$handler->display->display_options['fields']['created_1']['table'] = 'node';
$handler->display->display_options['fields']['created_1']['field'] = 'created';
$handler->display->display_options['fields']['created_1']['label'] = 'Date';
$handler->display->display_options['fields']['created_1']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['created_1']['date_format'] = 'short_timestamp_with_spelled_month';
$handler->display->display_options['fields']['created_1']['second_date_format'] = 'privatemsg_current_day';
/* Field: Content: Project */
$handler->display->display_options['fields']['field_receipt_project']['id'] = 'field_receipt_project';
$handler->display->display_options['fields']['field_receipt_project']['table'] = 'field_data_field_receipt_project';
$handler->display->display_options['fields']['field_receipt_project']['field'] = 'field_receipt_project';
$handler->display->display_options['fields']['field_receipt_project']['relationship'] = 'field_parent_purchase_receipt_target_id';
$handler->display->display_options['fields']['field_receipt_project']['label'] = '';
$handler->display->display_options['fields']['field_receipt_project']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_receipt_project']['alter']['alter_text'] = TRUE;
$handler->display->display_options['fields']['field_receipt_project']['alter']['text'] = '[field_receipt_project] ([field_rewarded_report])';
$handler->display->display_options['fields']['field_receipt_project']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_receipt_project']['settings'] = array(
  'bypass_access' => 0,
  'link' => 1,
);
/* Field: Global: PHP */
$handler->display->display_options['fields']['php']['id'] = 'php';
$handler->display->display_options['fields']['php']['table'] = 'views';
$handler->display->display_options['fields']['php']['field'] = 'php';
$handler->display->display_options['fields']['php']['label'] = 'Source';
$handler->display->display_options['fields']['php']['use_php_setup'] = 0;
$handler->display->display_options['fields']['php']['php_output'] = '<?php
module_load_include(\'inc\', \'rovas_civic\', \'rovas_civic.me_sources\');
if (function_exists(\'rovas_civic_me_sources_merit_history_source_html\')) {
  echo rovas_civic_me_sources_merit_history_source_html($view, $data);
}
?>';
$handler->display->display_options['fields']['php']['use_php_click_sortable'] = '0';
$handler->display->display_options['fields']['php']['php_click_sortable'] = '';
/* Field: Content: Buyer */
$handler->display->display_options['fields']['field_buyer']['id'] = 'field_buyer';
$handler->display->display_options['fields']['field_buyer']['table'] = 'field_data_field_buyer';
$handler->display->display_options['fields']['field_buyer']['field'] = 'field_buyer';
$handler->display->display_options['fields']['field_buyer']['relationship'] = 'field_parent_purchase_receipt_target_id';
$handler->display->display_options['fields']['field_buyer']['settings'] = array(
  'bypass_access' => 0,
  'link' => 1,
);
/* Field: Content: Amount Paid */
$handler->display->display_options['fields']['field_amount_paid']['id'] = 'field_amount_paid';
$handler->display->display_options['fields']['field_amount_paid']['table'] = 'field_data_field_amount_paid';
$handler->display->display_options['fields']['field_amount_paid']['field'] = 'field_amount_paid';
$handler->display->display_options['fields']['field_amount_paid']['relationship'] = 'field_parent_purchase_receipt_target_id';
$handler->display->display_options['fields']['field_amount_paid']['settings'] = array(
  'thousand_separator' => '',
  'decimal_separator' => '.',
  'scale' => '2',
  'prefix_suffix' => 1,
);
/* Field: Content: Reward amount */
$handler->display->display_options['fields']['field_reward_amount']['id'] = 'field_reward_amount';
$handler->display->display_options['fields']['field_reward_amount']['table'] = 'field_data_field_reward_amount';
$handler->display->display_options['fields']['field_reward_amount']['field'] = 'field_reward_amount';
$handler->display->display_options['fields']['field_reward_amount']['label'] = 'Reward';
$handler->display->display_options['fields']['field_reward_amount']['alter']['alter_text'] = TRUE;
$handler->display->display_options['fields']['field_reward_amount']['alter']['text'] = '[field_reward_amount-value] ME';
$handler->display->display_options['fields']['field_reward_amount']['alter']['trim_whitespace'] = TRUE;
$handler->display->display_options['fields']['field_reward_amount']['type'] = 'custom_formatters_merits_formatter';
$handler->display->display_options['fields']['field_reward_amount']['settings'] = array(
  'thousand_separator' => '',
  'decimal_separator' => '.',
  'scale' => '10',
  'prefix_suffix' => 1,
);
$handler->display->display_options['path'] = 'user/%/merits';
$handler->display->display_options['menu']['type'] = 'tab';
$handler->display->display_options['menu']['title'] = 'Merits';
$handler->display->display_options['menu']['weight'] = '0';
$handler->display->display_options['menu']['name'] = 'user-menu';
$handler->display->display_options['menu']['context'] = 0;
$handler->display->display_options['menu']['context_only_inline'] = 0;

/* Display: Block */
$handler = $view->new_display('block', 'Block', 'block_1');
$handler->display->display_options['defaults']['style_plugin'] = FALSE;
$handler->display->display_options['style_plugin'] = 'table';
$handler->display->display_options['style_options']['columns'] = array(
  'created_1' => 'created_1',
  'field_buyer' => 'field_buyer',
  'field_reward_amount' => 'field_reward_amount',
);
$handler->display->display_options['style_options']['class'] = '';
$handler->display->display_options['style_options']['default'] = '-1';
$handler->display->display_options['style_options']['info'] = array(
  'created_1' => array(
    'sortable' => 1,
    'default_sort_order' => 'desc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_buyer' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_reward_amount' => array(
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
$handler->display->display_options['defaults']['relationships'] = FALSE;
/* Relationship: Entity Reference: Referenced Entity */
$handler->display->display_options['relationships']['field_parent_purchase_receipt_target_id']['id'] = 'field_parent_purchase_receipt_target_id';
$handler->display->display_options['relationships']['field_parent_purchase_receipt_target_id']['table'] = 'field_data_field_parent_purchase_receipt';
$handler->display->display_options['relationships']['field_parent_purchase_receipt_target_id']['field'] = 'field_parent_purchase_receipt_target_id';
$handler->display->display_options['relationships']['field_parent_purchase_receipt_target_id']['label'] = 'purchase_receipt';
/* Relationship: Entity Reference: Referenced Entity */
$handler->display->display_options['relationships']['field_buyer_target_id']['id'] = 'field_buyer_target_id';
$handler->display->display_options['relationships']['field_buyer_target_id']['table'] = 'field_data_field_buyer';
$handler->display->display_options['relationships']['field_buyer_target_id']['field'] = 'field_buyer_target_id';
$handler->display->display_options['relationships']['field_buyer_target_id']['relationship'] = 'field_parent_purchase_receipt_target_id';
$handler->display->display_options['relationships']['field_buyer_target_id']['label'] = 'User';
$handler->display->display_options['defaults']['fields'] = FALSE;
/* Field: Content: Post date */
$handler->display->display_options['fields']['created_1']['id'] = 'created_1';
$handler->display->display_options['fields']['created_1']['table'] = 'node';
$handler->display->display_options['fields']['created_1']['field'] = 'created';
$handler->display->display_options['fields']['created_1']['label'] = 'Date';
$handler->display->display_options['fields']['created_1']['date_format'] = 'custom';
$handler->display->display_options['fields']['created_1']['custom_date_format'] = 'd M Y';
$handler->display->display_options['fields']['created_1']['second_date_format'] = 'privatemsg_current_day';
/* Field: Content: Buyer */
$handler->display->display_options['fields']['field_buyer']['id'] = 'field_buyer';
$handler->display->display_options['fields']['field_buyer']['table'] = 'field_data_field_buyer';
$handler->display->display_options['fields']['field_buyer']['field'] = 'field_buyer';
$handler->display->display_options['fields']['field_buyer']['relationship'] = 'field_parent_purchase_receipt_target_id';
$handler->display->display_options['fields']['field_buyer']['settings'] = array(
  'bypass_access' => 0,
  'link' => 1,
);
/* Field: Content: Reward amount */
$handler->display->display_options['fields']['field_reward_amount']['id'] = 'field_reward_amount';
$handler->display->display_options['fields']['field_reward_amount']['table'] = 'field_data_field_reward_amount';
$handler->display->display_options['fields']['field_reward_amount']['field'] = 'field_reward_amount';
$handler->display->display_options['fields']['field_reward_amount']['label'] = 'Merits';
$handler->display->display_options['fields']['field_reward_amount']['settings'] = array(
  'thousand_separator' => '',
  'decimal_separator' => '.',
  'scale' => '2',
  'prefix_suffix' => 1,
);
$handler->display->display_options['defaults']['arguments'] = FALSE;
/* Contextual filter: Content: Rewarded report (field_rewarded_report) */
$handler->display->display_options['arguments']['field_rewarded_report_target_id']['id'] = 'field_rewarded_report_target_id';
$handler->display->display_options['arguments']['field_rewarded_report_target_id']['table'] = 'field_data_field_rewarded_report';
$handler->display->display_options['arguments']['field_rewarded_report_target_id']['field'] = 'field_rewarded_report_target_id';
$handler->display->display_options['arguments']['field_rewarded_report_target_id']['relationship'] = 'field_parent_purchase_receipt_target_id';
$handler->display->display_options['arguments']['field_rewarded_report_target_id']['default_argument_type'] = 'fixed';
$handler->display->display_options['arguments']['field_rewarded_report_target_id']['summary']['number_of_records'] = '0';
$handler->display->display_options['arguments']['field_rewarded_report_target_id']['summary']['format'] = 'default_summary';
$handler->display->display_options['arguments']['field_rewarded_report_target_id']['summary_options']['items_per_page'] = '25';
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
  'merits_disbursement' => 'merits_disbursement',
);
$handler->display->display_options['filters']['type']['group'] = 1;
$translatables['merit_history'] = array(
  t('Master'),
  t('User\'s Merits'),
  t('more'),
  t('Apply'),
  t('Reset'),
  t('Sort by'),
  t('Asc'),
  t('Desc'),
  t('Select any filter and click on Apply to see results'),
  t('Advanced options'),
  t('Items per page'),
  t('- All -'),
  t('Offset'),
  t('«'),
  t('‹'),
  t('›'),
  t('»'),
  t('No records found'),
  t('purchase_receipt'),
  t('User'),
  t('source'),
  t('Node'),
  t('Receipt Id'),
  t('Date'),
  t('[field_receipt_project] ([field_rewarded_report])'),
  t('Source'),
  t('Buyer'),
  t('Amount Paid'),
  t('Reward'),
  t('[field_reward_amount-value] ME'),
  t('All'),
  t('Project'),
  t('Type the project title or its part'),
  t('Page'),
  t('Block'),
  t('Merits'),
);

