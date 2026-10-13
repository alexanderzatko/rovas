<?php

/**
 * @file
 * Default view: disputes
 */

$view = new view();
$view->name = 'disputes';
$view->description = '';
$view->tag = 'default';
$view->base_table = 'node';
$view->human_name = 'Disputes';
$view->core = 7;
$view->api_version = '3.0';
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

/* Display: Master */
$handler = $view->new_display('default', 'Master', 'default');
$handler->display->display_options['title'] = 'Disputes';
$handler->display->display_options['use_ajax'] = TRUE;
$handler->display->display_options['use_more_always'] = FALSE;
$handler->display->display_options['access']['type'] = 'perm';
$handler->display->display_options['cache']['type'] = 'none';
$handler->display->display_options['query']['type'] = 'views_query';
$handler->display->display_options['exposed_form']['type'] = 'basic';
$handler->display->display_options['exposed_form']['options']['expose_sort_order'] = FALSE;
$handler->display->display_options['exposed_form']['options']['autosubmit'] = TRUE;
$handler->display->display_options['pager']['type'] = 'full';
$handler->display->display_options['pager']['options']['items_per_page'] = '10';
$handler->display->display_options['style_plugin'] = 'table';
$handler->display->display_options['style_options']['row_class'] = '[php_1]';
$handler->display->display_options['style_options']['columns'] = array(
  'nid' => 'nid',
  'created' => 'created',
  'title' => 'title',
  'uid' => 'uid',
  'name' => 'name',
  'field_full_name' => 'field_full_name',
  'field_accused_1' => 'field_accused_1',
  'field_accused' => 'field_accused',
  'field_full_name_1' => 'field_full_name_1',
  'field_dispute_status' => 'field_dispute_status',
  'field_dispute_status_1' => 'field_dispute_status_1',
  'php' => 'php',
  'php_1' => 'php_1',
);
$handler->display->display_options['style_options']['default'] = '-1';
$handler->display->display_options['style_options']['info'] = array(
  'nid' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
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
  'title' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'uid' => array(
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'name' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_full_name' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_accused_1' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_accused' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_full_name_1' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_dispute_status' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'field_dispute_status_1' => array(
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
  'php_1' => array(
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
$handler->display->display_options['header']['area']['content'] = '<?php

$collapsed_content = "
<br/><br/>From time to time users might perform actions considered to be in violation of the economy rules. For example, a particular user might record work that another user believes was not actually performed, or the output of that work was never made available for purchase via Rovas. In cases like these, users can start a formal process called <strong>dispute</strong>, that might lead to penalization of the offender.
<br/><br/>
To file a dispute, follow these steps:
<br/><br/>
<ol>
<li>Contact the violator by sending him/her a message with request to clarify or remedy the violation voluntarily. Only if this step fails, continue with the next steps.</li>
<li>Submit a dispute. The dispute form contains the name of the accused, the object of complaint (a work report, project or another dispute) and a description of the charge against the accused.</li>
<li>Upon saving the form, Rovas changes certain application privileges for and notifies the accused about the new situation. For the duration of the dispute process he or she will not be able to file new own projects, open disputes, e.t.c.</li>
<li>Both parties are encouraged to invite other users to support their positions. Their goal is to attract supporters as disputes are won by that party, which accumulates the greatest sum of their supporters\' Merits. If nobody has any Merits, work time of parties is compared instead. Both parties can solicit help for their position for 48 hours.</li>
<li>If the accused takes an action that addresses the complaint within 48 hours, the dispute is closed without penalties</li>
<li>After the 48 hour contest period expires, Rovas tallies Merits (or work hours) for both parties and determines the winner. If the accused loses, Rovas will calculate the penalty. In particular, the algorithm will find the average merit score of all workers in the economy and divide that number by two - this forms the base of the penalty amount B. B is then multiplied by Mw divided by Md, where Mw is the sum of all merit scores of the winning party and Md is the difference between merit scores of the two parties. The negative value of the resulting number replaces the existing merit score of the offender. The object of complaint is also removed. Workers with negative merit scores have lower status in the economy, which is manifested for example by their inability to work on their own or public projects.</li>
<li>If the party of the accused wins, or the dispute ends in a tie, the Dispute is closed and no party is penalized</li>
</ol>
";
print theme(
	\'ctools_collapsible\',
	array(
		\'handle\' => \'Dispute rules (click to read)\', 
		\'content\' => $collapsed_content, 
		\'collapsed\' => TRUE
	)
);
?>';
$handler->display->display_options['header']['area']['format'] = 'php_code';
/* Footer: Global: PHP */
$handler->display->display_options['footer']['php']['id'] = 'php';
$handler->display->display_options['footer']['php']['table'] = 'views';
$handler->display->display_options['footer']['php']['field'] = 'php';
$handler->display->display_options['footer']['php']['empty'] = TRUE;
$handler->display->display_options['footer']['php']['php_output'] = '<?php

global $user;
$obj = entity_metadata_wrapper(\'user\', $user);
$vrfststus = $obj->field_verified->value();

//show only to verified users who are not banned or in dispute
print \'<br/><br/>\';
if ( $vrfststus == 1 && !(user_has_role(5) || user_has_role(6)) )
	print \'<a class="btn btn-danger" href="/node/add/dispute" role="button">\' . t(\'New Dispute\') . \'</a><br/><br/>\';
else
	print \'<a class="btn btn-default disabled" style="color: white; background-color: gray;" href="/node/add/dispute" role="button">\' . t(\'New Dispute\') . \'</a><br/><br/>\';

?>';
/* No results behavior: Global: Text area */
$handler->display->display_options['empty']['area']['id'] = 'area';
$handler->display->display_options['empty']['area']['table'] = 'views';
$handler->display->display_options['empty']['area']['field'] = 'area';
$handler->display->display_options['empty']['area']['empty'] = TRUE;
$handler->display->display_options['empty']['area']['content'] = 'There are no records corresponding to the selected filter.';
$handler->display->display_options['empty']['area']['format'] = 'filtered_html';
/* Relationship: Content: Content author */
$handler->display->display_options['relationships']['uid']['id'] = 'uid';
$handler->display->display_options['relationships']['uid']['table'] = 'node';
$handler->display->display_options['relationships']['uid']['field'] = 'uid';
/* Relationship: Entity Reference: Referenced Entity */
$handler->display->display_options['relationships']['field_accused_target_id']['id'] = 'field_accused_target_id';
$handler->display->display_options['relationships']['field_accused_target_id']['table'] = 'field_data_field_accused';
$handler->display->display_options['relationships']['field_accused_target_id']['field'] = 'field_accused_target_id';
/* Field: Content: Nid */
$handler->display->display_options['fields']['nid']['id'] = 'nid';
$handler->display->display_options['fields']['nid']['table'] = 'node';
$handler->display->display_options['fields']['nid']['field'] = 'nid';
$handler->display->display_options['fields']['nid']['label'] = '';
$handler->display->display_options['fields']['nid']['exclude'] = TRUE;
$handler->display->display_options['fields']['nid']['element_label_colon'] = FALSE;
/* Field: Content: Post date */
$handler->display->display_options['fields']['created']['id'] = 'created';
$handler->display->display_options['fields']['created']['table'] = 'node';
$handler->display->display_options['fields']['created']['field'] = 'created';
$handler->display->display_options['fields']['created']['date_format'] = 'short';
$handler->display->display_options['fields']['created']['second_date_format'] = 'privatemsg_current_day';
/* Field: Content: Title */
$handler->display->display_options['fields']['title']['id'] = 'title';
$handler->display->display_options['fields']['title']['table'] = 'node';
$handler->display->display_options['fields']['title']['field'] = 'title';
$handler->display->display_options['fields']['title']['label'] = 'Dispute Name';
$handler->display->display_options['fields']['title']['alter']['word_boundary'] = FALSE;
$handler->display->display_options['fields']['title']['alter']['ellipsis'] = FALSE;
/* Field: Content: Author uid */
$handler->display->display_options['fields']['uid']['id'] = 'uid';
$handler->display->display_options['fields']['uid']['table'] = 'node';
$handler->display->display_options['fields']['uid']['field'] = 'uid';
$handler->display->display_options['fields']['uid']['label'] = '';
$handler->display->display_options['fields']['uid']['exclude'] = TRUE;
$handler->display->display_options['fields']['uid']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['uid']['link_to_user'] = FALSE;
/* Field: User: Name */
$handler->display->display_options['fields']['name']['id'] = 'name';
$handler->display->display_options['fields']['name']['table'] = 'users';
$handler->display->display_options['fields']['name']['field'] = 'name';
$handler->display->display_options['fields']['name']['relationship'] = 'uid';
$handler->display->display_options['fields']['name']['label'] = '';
$handler->display->display_options['fields']['name']['exclude'] = TRUE;
$handler->display->display_options['fields']['name']['element_label_colon'] = FALSE;
/* Field: User: Full name */
$handler->display->display_options['fields']['field_full_name']['id'] = 'field_full_name';
$handler->display->display_options['fields']['field_full_name']['table'] = 'field_data_field_full_name';
$handler->display->display_options['fields']['field_full_name']['field'] = 'field_full_name';
$handler->display->display_options['fields']['field_full_name']['relationship'] = 'uid';
$handler->display->display_options['fields']['field_full_name']['label'] = 'Complainant';
$handler->display->display_options['fields']['field_full_name']['alter']['alter_text'] = TRUE;
$handler->display->display_options['fields']['field_full_name']['alter']['text'] = '[name] ([field_full_name])';
/* Field: accused ID */
$handler->display->display_options['fields']['field_accused_1']['id'] = 'field_accused_1';
$handler->display->display_options['fields']['field_accused_1']['table'] = 'field_data_field_accused';
$handler->display->display_options['fields']['field_accused_1']['field'] = 'field_accused';
$handler->display->display_options['fields']['field_accused_1']['ui_name'] = 'accused ID';
$handler->display->display_options['fields']['field_accused_1']['label'] = '';
$handler->display->display_options['fields']['field_accused_1']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_accused_1']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_accused_1']['type'] = 'entityreference_entity_id';
$handler->display->display_options['fields']['field_accused_1']['settings'] = array(
  'bypass_access' => 0,
  'link' => 0,
);
/* Field: accused name */
$handler->display->display_options['fields']['field_accused']['id'] = 'field_accused';
$handler->display->display_options['fields']['field_accused']['table'] = 'field_data_field_accused';
$handler->display->display_options['fields']['field_accused']['field'] = 'field_accused';
$handler->display->display_options['fields']['field_accused']['ui_name'] = 'accused name';
$handler->display->display_options['fields']['field_accused']['label'] = '';
$handler->display->display_options['fields']['field_accused']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_accused']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['field_accused']['settings'] = array(
  'bypass_access' => 0,
  'link' => 1,
);
/* Field: User: Full name */
$handler->display->display_options['fields']['field_full_name_1']['id'] = 'field_full_name_1';
$handler->display->display_options['fields']['field_full_name_1']['table'] = 'field_data_field_full_name';
$handler->display->display_options['fields']['field_full_name_1']['field'] = 'field_full_name';
$handler->display->display_options['fields']['field_full_name_1']['relationship'] = 'field_accused_target_id';
$handler->display->display_options['fields']['field_full_name_1']['label'] = 'Accused';
$handler->display->display_options['fields']['field_full_name_1']['alter']['alter_text'] = TRUE;
$handler->display->display_options['fields']['field_full_name_1']['alter']['text'] = '[field_accused] ([field_full_name_1])';
/* Field: Disp. status ID */
$handler->display->display_options['fields']['field_dispute_status']['id'] = 'field_dispute_status';
$handler->display->display_options['fields']['field_dispute_status']['table'] = 'field_data_field_dispute_status';
$handler->display->display_options['fields']['field_dispute_status']['field'] = 'field_dispute_status';
$handler->display->display_options['fields']['field_dispute_status']['ui_name'] = 'Disp. status ID';
$handler->display->display_options['fields']['field_dispute_status']['label'] = 'Status';
$handler->display->display_options['fields']['field_dispute_status']['exclude'] = TRUE;
$handler->display->display_options['fields']['field_dispute_status']['type'] = 'list_key';
/* Field: Disp. status formated */
$handler->display->display_options['fields']['field_dispute_status_1']['id'] = 'field_dispute_status_1';
$handler->display->display_options['fields']['field_dispute_status_1']['table'] = 'field_data_field_dispute_status';
$handler->display->display_options['fields']['field_dispute_status_1']['field'] = 'field_dispute_status';
$handler->display->display_options['fields']['field_dispute_status_1']['ui_name'] = 'Disp. status formated';
$handler->display->display_options['fields']['field_dispute_status_1']['label'] = 'Status';
$handler->display->display_options['fields']['field_dispute_status_1']['type'] = 'i18n_list_default';
/* Field: Global: PHP */
$handler->display->display_options['fields']['php']['id'] = 'php';
$handler->display->display_options['fields']['php']['table'] = 'views';
$handler->display->display_options['fields']['php']['field'] = 'php';
$handler->display->display_options['fields']['php']['label'] = 'Action';
$handler->display->display_options['fields']['php']['use_php_setup'] = 0;
$handler->display->display_options['fields']['php']['php_output'] = '<?php 

//make sure the dispute node is loaded only once
/*
if ($static == \'\') {

$dispute = node_load($row->field_dispute);
$dispute_w = entity_metadata_wrapper(\'node\', $dispute);

$static = array(\'accused_uid\' => $dispute_w->field_accused->uid->value(), \'complainant_uid\' => $dispute_w->author->uid->value(), \'dispute_status\' => $dispute_w->field_dispute_status->value());

}
*/

global $user;

if($row->uid == $user->uid && $data->field_field_dispute_status[0][\'raw\'][\'value\'] == 0) {
	print \'<a href="/node/\' . $row->nid . \'/edit/"><img src="/sites/default/files/pictures/edit.svg" width="16" title="\' . t(\'edit\') . \'" /></a>&nbsp;&nbsp;\';
}
else {
	print \'<img src="/sites/default/files/pictures/edit-off.svg" width="16" title="\' . t(\'edit\') . \'" />&nbsp;&nbsp;\';
}

if($row->uid == $user->uid && $data->field_field_dispute_status[0][\'raw\'][\'value\'] == 0) {
	print \'<a href="/node/\' . $row->nid . \'/delete/"><img src="/sites/default/files/pictures/delete.svg" width="16" title="\' . t(\'delete\') . \'" /></a>\';
}
else {
	print \'<img src="/sites/default/files/pictures/delete-off.svg" width="16" title="\' . t(\'delete\') . \'" />\';
}

?>';
$handler->display->display_options['fields']['php']['use_php_click_sortable'] = '0';
$handler->display->display_options['fields']['php']['php_click_sortable'] = '';
/* Sort criterion: Content: Post date */
$handler->display->display_options['sorts']['created']['id'] = 'created';
$handler->display->display_options['sorts']['created']['table'] = 'node';
$handler->display->display_options['sorts']['created']['field'] = 'created';
$handler->display->display_options['sorts']['created']['order'] = 'DESC';
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
  'dispute' => 'dispute',
);
$handler->display->display_options['views_ef_fieldset']['enabled'] = TRUE;

/* Display: Page */
$handler = $view->new_display('page', 'Page', 'page');
$handler->display->display_options['defaults']['use_ajax'] = FALSE;
$handler->display->display_options['use_ajax'] = TRUE;
$handler->display->display_options['defaults']['exposed_form'] = FALSE;
$handler->display->display_options['exposed_form']['type'] = 'basic';
$handler->display->display_options['exposed_form']['options']['reset_button'] = TRUE;
$handler->display->display_options['defaults']['header'] = FALSE;
/* Header: Global: Text area */
$handler->display->display_options['header']['area']['id'] = 'area';
$handler->display->display_options['header']['area']['table'] = 'views';
$handler->display->display_options['header']['area']['field'] = 'area';
$handler->display->display_options['header']['area']['empty'] = TRUE;
$handler->display->display_options['header']['area']['content'] = '<?php

$collapsed_content = "<br/><br/>" . t("From time to time users might perform actions considered to be in violation of the economy rules. For example, a particular user might record work that another user believes was not actually performed, or the output of that work was never made available for purchase via Rovas. In cases like these, users can start a formal process called <strong>dispute</strong>, that might lead to penalization of the offender.") . "<br/><br/>" . t("To file a dispute, follow these steps:") . "<br/><br/>" . t("<ol>
<li>Contact the violator by sending him/her a message with request to clarify or remedy the violation voluntarily. Only if this step fails, continue with the next steps.</li>
<li>Submit a dispute. The dispute form contains the name of the accused, the object of complaint (a work report or a project) and a description of the charge against the accused.</li>
<li>Upon saving the form, Rovas changes certain application privileges for and notifies the accused about the new situation. For the duration of the dispute process he or she will not be able to file new own projects, open disputes, e.t.c.</li>
<li>Both parties are encouraged to invite other users to support their positions. Their goal is to attract supporters, as disputes are won by that party, which accumulates the greatest sum of their supporters\' Merits. Both parties can solicit help for their position for 36 hours.</li>
<li>If the accused takes an action that addresses the complaint (deletes or edits the object of complaint) within 24 hours, the dispute is closed without penalties,</li>
<li>If the accused takes an action that addresses the complaint after 24, but before 36 hours since the official start of dispute, the dispute is closed, but the accused\'s Merit reward score is set to 0,</li>
<li>When the accused does not rectify the problem, after 36 hours since the start of dislpute, the contest period expires, Rovas tallies Merits for both parties and determines the winner. If the accused loses, Rovas will calculate the penalty. The algorithm will find the average merit score of all workers in the economy and divide that number by two - this forms the base of the penalty amount B. B is then multiplied by Mw divided by Md, where Mw is the sum of all merit scores of the winning party and Md is the difference between merit scores of the two parties. The negative value of the resulting number replaces the existing merit score of the accused. The object of complaint is also removed. Workers with negative merit scores have lower status in the economy, which is manifested for example by their inability to work on their own or public projects.</li>
<li>If the party of the accused wins, or the dispute ends in a tie, the Dispute is closed and no party is penalized</li>
</ol>");
print theme(
	\'ctools_collapsible\',
	array(
		\'handle\' => t(\'Dispute rules (click to read)\'), 
		\'content\' => $collapsed_content, 
		\'collapsed\' => TRUE
	)
);
?>';
$handler->display->display_options['header']['area']['format'] = 'php_code';
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
  'dispute' => 'dispute',
);
$handler->display->display_options['filters']['type']['group'] = 1;
/* Filter criterion: Content: Dispute status (field_dispute_status) (translated) */
$handler->display->display_options['filters']['field_dispute_status_value_i18n']['id'] = 'field_dispute_status_value_i18n';
$handler->display->display_options['filters']['field_dispute_status_value_i18n']['table'] = 'field_data_field_dispute_status';
$handler->display->display_options['filters']['field_dispute_status_value_i18n']['field'] = 'field_dispute_status_value_i18n';
$handler->display->display_options['filters']['field_dispute_status_value_i18n']['group'] = 1;
$handler->display->display_options['filters']['field_dispute_status_value_i18n']['exposed'] = TRUE;
$handler->display->display_options['filters']['field_dispute_status_value_i18n']['expose']['operator_id'] = 'field_dispute_status_value_i18n_op';
$handler->display->display_options['filters']['field_dispute_status_value_i18n']['expose']['label'] = 'Dispute status';
$handler->display->display_options['filters']['field_dispute_status_value_i18n']['expose']['operator'] = 'field_dispute_status_value_i18n_op';
$handler->display->display_options['filters']['field_dispute_status_value_i18n']['expose']['identifier'] = 'field_dispute_status_value_i18n';
$handler->display->display_options['filters']['field_dispute_status_value_i18n']['expose']['multiple'] = TRUE;
$handler->display->display_options['filters']['field_dispute_status_value_i18n']['expose']['remember_roles'] = array(
  2 => '2',
  1 => 0,
  3 => 0,
  4 => 0,
  6 => 0,
  5 => 0,
);
$handler->display->display_options['views_ef_fieldset']['enabled'] = TRUE;
$handler->display->display_options['views_ef_fieldset']['items'] = array(
  'fieldset:5b16b19a3444c' => array(
    'id' => 'fieldset:5b16b19a3444c',
    'pid' => '',
    'weight' => '-2',
    'options' => array(
      'label' => 'Show/hide the filters section',
      'collapse' => 'collapsed',
    ),
  ),
  'filter:field_dispute_status_value_i18n' => array(
    'id' => 'filter:field_dispute_status_value_i18n',
    'pid' => 'fieldset:5b16b19a3444c',
    'weight' => '-4',
  ),
  'button:reset' => array(
    'id' => 'button:reset',
    'pid' => 'fieldset:5b16b19a3444c',
    'weight' => '-3',
  ),
  'button:submit' => array(
    'id' => 'button:submit',
    'pid' => 'fieldset:5b16b19a3444c',
    'weight' => '-2',
  ),
);
$handler->display->display_options['path'] = 'disputes';
$handler->display->display_options['menu']['type'] = 'normal';
$handler->display->display_options['menu']['title'] = 'Disputes';
$handler->display->display_options['menu']['name'] = 'main-menu';

/* Display: Entity Reference */
$handler = $view->new_display('entityreference', 'Entity Reference', 'entityreference_1');
$handler->display->display_options['defaults']['title'] = FALSE;
$handler->display->display_options['pager']['type'] = 'some';
$handler->display->display_options['defaults']['style_plugin'] = FALSE;
$handler->display->display_options['style_plugin'] = 'entityreference_style';
$handler->display->display_options['style_options']['search_fields'] = array(
  'title' => 'title',
  'nid' => 0,
);
$handler->display->display_options['defaults']['style_options'] = FALSE;
$handler->display->display_options['defaults']['row_plugin'] = FALSE;
$handler->display->display_options['row_plugin'] = 'entityreference_fields';
$handler->display->display_options['defaults']['row_options'] = FALSE;
$handler->display->display_options['defaults']['header'] = FALSE;
$handler->display->display_options['defaults']['footer'] = FALSE;
$handler->display->display_options['defaults']['relationships'] = FALSE;
$handler->display->display_options['defaults']['fields'] = FALSE;
/* Field: Content: Title */
$handler->display->display_options['fields']['title']['id'] = 'title';
$handler->display->display_options['fields']['title']['table'] = 'node';
$handler->display->display_options['fields']['title']['field'] = 'title';
$handler->display->display_options['fields']['title']['label'] = '';
$handler->display->display_options['fields']['title']['alter']['word_boundary'] = FALSE;
$handler->display->display_options['fields']['title']['alter']['ellipsis'] = FALSE;
$handler->display->display_options['fields']['title']['element_label_colon'] = FALSE;
$handler->display->display_options['fields']['title']['link_to_node'] = FALSE;
$translatables['disputes'] = array(
  t('Master'),
  t('Disputes'),
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
  t('<?php

$collapsed_content = "
<br/><br/>From time to time users might perform actions considered to be in violation of the economy rules. For example, a particular user might record work that another user believes was not actually performed, or the output of that work was never made available for purchase via Rovas. In cases like these, users can start a formal process called <strong>dispute</strong>, that might lead to penalization of the offender.
<br/><br/>
To file a dispute, follow these steps:
<br/><br/>
<ol>
<li>Contact the violator by sending him/her a message with request to clarify or remedy the violation voluntarily. Only if this step fails, continue with the next steps.</li>
<li>Submit a dispute. The dispute form contains the name of the accused, the object of complaint (a work report, project or another dispute) and a description of the charge against the accused.</li>
<li>Upon saving the form, Rovas changes certain application privileges for and notifies the accused about the new situation. For the duration of the dispute process he or she will not be able to file new own projects, open disputes, e.t.c.</li>
<li>Both parties are encouraged to invite other users to support their positions. Their goal is to attract supporters as disputes are won by that party, which accumulates the greatest sum of their supporters\' Merits. If nobody has any Merits, work time of parties is compared instead. Both parties can solicit help for their position for 48 hours.</li>
<li>If the accused takes an action that addresses the complaint within 48 hours, the dispute is closed without penalties</li>
<li>After the 48 hour contest period expires, Rovas tallies Merits (or work hours) for both parties and determines the winner. If the accused loses, Rovas will calculate the penalty. In particular, the algorithm will find the average merit score of all workers in the economy and divide that number by two - this forms the base of the penalty amount B. B is then multiplied by Mw divided by Md, where Mw is the sum of all merit scores of the winning party and Md is the difference between merit scores of the two parties. The negative value of the resulting number replaces the existing merit score of the offender. The object of complaint is also removed. Workers with negative merit scores have lower status in the economy, which is manifested for example by their inability to work on their own or public projects.</li>
<li>If the party of the accused wins, or the dispute ends in a tie, the Dispute is closed and no party is penalized</li>
</ol>
";
print theme(
	\'ctools_collapsible\',
	array(
		\'handle\' => \'Dispute rules (click to read)\', 
		\'content\' => $collapsed_content, 
		\'collapsed\' => TRUE
	)
);
?>'),
  t('There are no records corresponding to the selected filter.'),
  t('author'),
  t('User entity referenced from field_accused'),
  t('Post date'),
  t('Dispute Name'),
  t('Complainant'),
  t('[name] ([field_full_name])'),
  t('Accused'),
  t('[field_accused] ([field_full_name_1])'),
  t('Status'),
  t('Action'),
  t('Page'),
  t('<?php

$collapsed_content = "<br/><br/>" . t("From time to time users might perform actions considered to be in violation of the economy rules. For example, a particular user might record work that another user believes was not actually performed, or the output of that work was never made available for purchase via Rovas. In cases like these, users can start a formal process called <strong>dispute</strong>, that might lead to penalization of the offender.") . "<br/><br/>" . t("To file a dispute, follow these steps:") . "<br/><br/>" . t("<ol>
<li>Contact the violator by sending him/her a message with request to clarify or remedy the violation voluntarily. Only if this step fails, continue with the next steps.</li>
<li>Submit a dispute. The dispute form contains the name of the accused, the object of complaint (a work report or a project) and a description of the charge against the accused.</li>
<li>Upon saving the form, Rovas changes certain application privileges for and notifies the accused about the new situation. For the duration of the dispute process he or she will not be able to file new own projects, open disputes, e.t.c.</li>
<li>Both parties are encouraged to invite other users to support their positions. Their goal is to attract supporters, as disputes are won by that party, which accumulates the greatest sum of their supporters\' Merits. Both parties can solicit help for their position for 36 hours.</li>
<li>If the accused takes an action that addresses the complaint (deletes or edits the object of complaint) within 24 hours, the dispute is closed without penalties,</li>
<li>If the accused takes an action that addresses the complaint after 24, but before 36 hours since the official start of dispute, the dispute is closed, but the accused\'s Merit reward score is set to 0,</li>
<li>When the accused does not rectify the problem, after 36 hours since the start of dislpute, the contest period expires, Rovas tallies Merits for both parties and determines the winner. If the accused loses, Rovas will calculate the penalty. The algorithm will find the average merit score of all workers in the economy and divide that number by two - this forms the base of the penalty amount B. B is then multiplied by Mw divided by Md, where Mw is the sum of all merit scores of the winning party and Md is the difference between merit scores of the two parties. The negative value of the resulting number replaces the existing merit score of the accused. The object of complaint is also removed. Workers with negative merit scores have lower status in the economy, which is manifested for example by their inability to work on their own or public projects.</li>
<li>If the party of the accused wins, or the dispute ends in a tie, the Dispute is closed and no party is penalized</li>
</ol>");
print theme(
	\'ctools_collapsible\',
	array(
		\'handle\' => t(\'Dispute rules (click to read)\'), 
		\'content\' => $collapsed_content, 
		\'collapsed\' => TRUE
	)
);
?>'),
  t('Dispute status'),
  t('Entity Reference'),
);

