<?php

function _rovas_project_reward_installed_fields() {
  $field_bases = array();

  // Exported field_base: 'field_payer_type'.
  $field_bases['field_payer_type'] = array(
    'active' => 1,
    'cardinality' => 1,
    'deleted' => 0,
    'entity_types' => array(),
    'field_name' => 'field_payer_type',
    'field_permissions' => array(
      'type' => 2,
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
        'user' => 'user',
        'project' => 'project',
      ),
      'allowed_values_function' => '',
      'entity_translation_sync' => FALSE,
    ),
    'translatable' => 0,
    'type' => 'list_text',
  );

  // Exported field_base: 'field_payment_recipient'.
  $field_bases['field_payment_recipient'] = array(
    'active' => 1,
    'cardinality' => 1,
    'deleted' => 0,
    'entity_types' => array(),
    'field_name' => 'field_payment_recipient',
    'field_permissions' => array(
      'type' => 2,
    ),
    'indexes' => array(),
    'locked' => 0,
    'module' => 'number',
    'settings' => array(
      'entity_translation_sync' => FALSE,
    ),
    'translatable' => 0,
    'type' => 'number_integer',
  );

	$field_bases['field_price_in_eur'] = array(
		'active' => '1',
		'cardinality' => '1',
		'deleted' => '0',
		'entity_types' => array(),
		'field_name' => 'field_price_in_eur',
		'field_permissions' => array(
			'type' => '1',
		),
		'indexes' => array(),
		'locked' => '0',
		'module' => 'number',
		'settings' => array(
			'precision' => '10',
			'scale' => '2',
			'decimal_separator' => '.',
			'entity_translation_sync' => FALSE,
		),
		'translatable' => '0',
		'type' => 'number_decimal',
		'storage' => array(
			'type' => 'field_sql_storage',
			'settings' => array(),
			'module' => 'field_sql_storage',
			'active' => '1',
			'details' => array(
				'sql' => array(
					'FIELD_LOAD_CURRENT' => array(
						'field_data_field_price_in_eur' => array(
							'value' => 'field_price_in_eur_value',
						),
					),
					'FIELD_LOAD_REVISION' => array(
						'field_revision_field_price_in_eur' => array(
							'value' => 'field_price_in_eur_value',
						),
					),
				),
			),
		),
		'foreign keys' => array(),
		'columns' => array(
			'value' => array(
				'type' => 'numeric',
				'precision' => '10',
				'scale' => '2',
				'not null' => FALSE,
			),
		),
		'bundles' => array(
			'node' => array(
				'project',
			),
		),
	);

	$field_bases['field_reward_merit_allocation'] = array(
		'translatable' => '0',
		'entity_types' => array(),
		'settings' => array(
			'entity_translation_sync' => FALSE,
		),
		'storage' => array(
			'type' => 'field_sql_storage',
			'settings' => array(),
			'module' => 'field_sql_storage',
			'active' => '1',
			'details' => array(
				'sql' => array(
					'FIELD_LOAD_CURRENT' => array(
						'field_data_field_reward_merit_allocation' => array(
							'value' => 'field_reward_merit_allocation_value',
						),
					),
					'FIELD_LOAD_REVISION' => array(
						'field_revision_field_reward_merit_allocation' => array(
							'value' => 'field_reward_merit_allocation_value',
						),
					),
				),
			),
		),
		'foreign keys' => array(),
		'indexes' => array(),
		'field_permissions' => array(
			'type' => '0',
		),
		'field_name' => 'field_reward_merit_allocation',
		'type' => 'number_integer',
		'module' => 'number',
		'active' => '1',
		'locked' => '0',
		'cardinality' => '1',
		'deleted' => '0',
		'columns' => array(
			'value' => array(
				'type' => 'int',
				'not null' => FALSE,
			),
		),
		'bundles' => array(
			'node' => array(
				'project',
			),
		),
	);

	$field_bases['field_chron_beneficiary'] = array(
		'translatable' => '0',
		'entity_types' => array(),
		'settings' => array(
			'target_type' => 'user',
			'handler' => 'base',
			'handler_settings' => array(
				'target_bundles' => array(),
				'sort' => array(
					'type' => 'none',
				),
				'behaviors' => array(
					'field_default_token' => array(
						'status' => TRUE,
					),
					'views-select-list' => array(
						'status' => 0,
					),
				),
			),
			'entity_translation_sync' => FALSE,
		),
		'storage' => array(
			'type' => 'field_sql_storage',
			'settings' => array(),
			'module' => 'field_sql_storage',
			'active' => '1',
			'details' => array(
				'sql' => array(
					'FIELD_LOAD_CURRENT' => array(
						'field_data_field_chron_beneficiary' => array(
							'target_id' => 'field_chron_beneficiary_target_id',
						),
					),
					'FIELD_LOAD_REVISION' => array(
						'field_revision_field_chron_beneficiary' => array(
							'target_id' => 'field_chron_beneficiary_target_id',
						),
					),
				),
			),
		),
		'foreign keys' => array(
			'users' => array(
				'table' => 'users',
				'columns' => array(
					'target_id' => 'uid',
				),
			),
		),
		'indexes' => array(
			'target_id' => array(
				'target_id',
			),
		),
		'field_permissions' => array(
			'type' => '1',
		),
		'field_name' => 'field_chron_beneficiary',
		'type' => 'entityreference',
		'module' => 'entityreference',
		'active' => '1',
		'locked' => '0',
		'cardinality' => '1',
		'deleted' => '0',
		'columns' => array(
			'target_id' => array(
				'description' => 'The id of the target entity.',
				'type' => 'int',
				'unsigned' => TRUE,
				'not null' => TRUE,
			),
		),
		'bundles' => array(
			'node' => array(
				'project',
			),
		),
	);

  return $field_bases;
}
