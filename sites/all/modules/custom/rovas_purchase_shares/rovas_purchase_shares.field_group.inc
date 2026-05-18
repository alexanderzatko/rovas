<?php

/**
 * @file
 * rovas_purchase_shares.field_group.inc
 */

/**
 * Implements hook_field_group_info().
 */
function rovas_purchase_shares_field_group_info() {
  $field_groups = array();

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_purchasing_shares|node|project|form';
  $field_group->group_name = 'group_purchasing_shares';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'project';
  $field_group->mode = 'form';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'Purchasing shares',
    'weight' => '26',
    'children' => array(
      0 => 'field_allow_purchasing_shares',
      1 => 'field_rate',
      2 => 'field_time_share_cost_euros',
      3 => 'field_minimum_time',
    ),
    'format_type' => 'fieldset',
    'format_settings' => array(
      'formatter' => 'collapsible',
      'instance_settings' => array(
        'description' => '',
        'classes' => 'group-purchasing-shares field-group-fieldset',
        'required_fields' => 1,
        'id' => '',
      ),
    ),
  );
  $field_groups['group_purchasing_shares|node|project|form'] = $field_group;

  // Translatables
  // Included for use with string extractors like potx.
  t('Purchasing shares');

  return $field_groups;
}
