<?php

/**
 * @file
 * rovas_purchase_shares.features.field_base.inc
 */

/**
 * Implements hook_field_default_field_bases().
 */
function rovas_purchase_shares_field_default_field_bases() {
  $field_bases = array();

  // Exported field_base: 'field_chrons_paid'.
  $field_bases['field_chrons_paid'] = array(
    'active' => 1,
    'cardinality' => 1,
    'deleted' => 0,
    'entity_types' => array(),
    'field_name' => 'field_chrons_paid',
    'field_permissions' => array(
      'type' => 2,
    ),
    'indexes' => array(),
    'locked' => 0,
    'module' => 'number',
    'settings' => array(
      'decimal_separator' => '.',
      'entity_translation_sync' => FALSE,
      'precision' => 10,
      'scale' => 2,
    ),
    'translatable' => 0,
    'type' => 'number_decimal',
  );

  // Exported field_base: 'field_hours_purchased'.
  $field_bases['field_hours_purchased'] = array(
    'active' => 1,
    'cardinality' => 1,
    'deleted' => 0,
    'entity_types' => array(),
    'field_name' => 'field_hours_purchased',
    'field_permissions' => array(
      'type' => 2,
    ),
    'indexes' => array(),
    'locked' => 0,
    'module' => 'number',
    'settings' => array(
      'decimal_separator' => '.',
      'entity_translation_sync' => FALSE,
      'precision' => 10,
      'scale' => 5,
    ),
    'translatable' => 0,
    'type' => 'number_decimal',
  );

  // Exported field_base: 'field_purchase_proof'.
  $field_bases['field_purchase_proof'] = array(
    'active' => 1,
    'cardinality' => 1,
    'deleted' => 0,
    'entity_types' => array(),
    'field_name' => 'field_purchase_proof',
    'field_permissions' => array(
      'type' => 0,
    ),
    'indexes' => array(
      'target_id' => array(
        0 => 'target_id',
      ),
    ),
    'locked' => 0,
    'module' => 'entityreference',
    'settings' => array(
      'entity_translation_sync' => FALSE,
      'handler' => 'base',
      'handler_settings' => array(
        'behaviors' => array(
          'field_default_token' => array(
            'status' => TRUE,
          ),
          'views-select-list' => array(
            'status' => 0,
          ),
        ),
        'sort' => array(
          'type' => 'none',
        ),
        'target_bundles' => array(
          'purchase_receipt' => 'purchase_receipt',
        ),
      ),
      'target_type' => 'node',
    ),
    'translatable' => 0,
    'type' => 'entityreference',
  );

  // Exported field_base: 'field_allow_purchasing_shares'.
  $field_bases['field_allow_purchasing_shares'] = array(
    'active' => 1,
    'cardinality' => 1,
    'deleted' => 0,
    'entity_types' => array(),
    'field_name' => 'field_allow_purchasing_shares',
    'field_permissions' => array(
      'type' => 1,
    ),
    'indexes' => array(
      'value' => array(
        0 => 'value',
      ),
    ),
    'locked' => 0,
    'module' => 'list',
    'settings' => array(
      'allowed_values' => array(
        0 => 'no',
        1 => 'yes',
      ),
      'allowed_values_function' => '',
      'entity_translation_sync' => FALSE,
    ),
    'translatable' => 0,
    'type' => 'list_boolean',
  );

  // Exported field_base: 'field_minimum_time'.
  $field_bases['field_minimum_time'] = array(
    'active' => 1,
    'cardinality' => 1,
    'deleted' => 0,
    'entity_types' => array(),
    'field_name' => 'field_minimum_time',
    'field_permissions' => array(
      'type' => 0,
    ),
    'indexes' => array(),
    'locked' => 0,
    'module' => 'number',
    'settings' => array(
      'decimal_separator' => '.',
      'entity_translation_sync' => FALSE,
      'precision' => 10,
      'scale' => 2,
    ),
    'translatable' => 0,
    'type' => 'number_decimal',
  );

  // Exported field_base: 'field_rate'.
  $field_bases['field_rate'] = array(
    'active' => 1,
    'cardinality' => 1,
    'deleted' => 0,
    'entity_types' => array(),
    'field_name' => 'field_rate',
    'field_permissions' => array(
      'type' => 1,
    ),
    'indexes' => array(),
    'locked' => 0,
    'module' => 'number',
    'settings' => array(
      'decimal_separator' => '.',
      'entity_translation_sync' => FALSE,
      'precision' => 10,
      'scale' => 2,
    ),
    'translatable' => 0,
    'type' => 'number_decimal',
  );

  // Exported field_base: 'field_buy_shares_button'.
  $field_bases['field_buy_shares_button'] = array(
    'active' => 1,
    'cardinality' => 1,
    'deleted' => 0,
    'entity_types' => array(),
    'field_name' => 'field_buy_shares_button',
    'field_permissions' => array(
      'type' => 2,
    ),
    'indexes' => array(),
    'locked' => 0,
    'module' => 'computed_field',
    'settings' => array(
      'code' => '$obj = entity_metadata_wrapper(\'node\',$entity);

if($obj->field_allow_purchasing_shares->value() == 1) {
	$tip = t(\'Buy a share of Merits in this project\');
	$nid = $entity->nid;
	
	$result = \'<a class="btn btn-default" href="/buyshares/\'.$nid.\'" role="button" style="color:white; background-color:gray;" title="\'.$tip.\'">\'. t(\'Buy shares\') .\'</a>&nbsp;&nbsp;\';
//	$result = \'<a href="#" title="\'.$tip.\'">\'. t(\'Buy shares in this project\') .\'</a><br><br>\';
}
else {
	$result = \'\';
}

$entity_field[0][\'value\'] = $result;',
      'database' => array(
        'data_default' => '',
        'data_index' => 0,
        'data_length' => 128,
        'data_not_NULL' => 0,
        'data_precision' => 10,
        'data_scale' => 2,
        'data_size' => 'normal',
        'data_type' => 'text',
      ),
      'display_format' => '$display_output = $entity_field_item[\'value\'];',
      'entity_translation_sync' => FALSE,
      'recalculate' => 1,
      'store' => 1,
    ),
    'translatable' => 0,
    'type' => 'computed',
  );

  return $field_bases;
}
