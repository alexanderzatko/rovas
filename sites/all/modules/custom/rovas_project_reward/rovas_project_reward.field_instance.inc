<?php

function _rovas_project_reward_installed_instances() {
  $field_instances = array();

  // Exported field_instance: 'commerce_order-commerce_order-field_payer_type'.
  $field_instances['commerce_order-commerce_order-field_payer_type'] = array(
    'bundle' => 'commerce_order',
    'default_value' => NULL,
    'default_value_label_token' => 0,
    'deleted' => 0,
    'description' => 'paytype can have values "project" or "user," depending on the type of payment that is being done',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'module' => 'list',
        'settings' => array(),
        'type' => 'list_default',
        'weight' => -3,
      ),
    ),
    'entity_type' => 'commerce_order',
    'field_name' => 'field_payer_type',
    'label' => 'Payer type',
    'required' => 0,
    'settings' => array(
      'entity_translation_sync' => FALSE,
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'options',
      'settings' => array(),
      'type' => 'options_select',
      'weight' => -2,
    ),
  );

  // Exported field_instance:
  // 'commerce_order-commerce_order-field_payment_recipient'.
  $field_instances['commerce_order-commerce_order-field_payment_recipient'] = array(
    'bundle' => 'commerce_order',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => 'the ID of the user or project that is receiving the payment',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'module' => 'number',
        'settings' => array(
          'decimal_separator' => '.',
          'prefix_suffix' => TRUE,
          'scale' => 0,
          'thousand_separator' => '',
        ),
        'type' => 'number_integer',
        'weight' => -2,
      ),
    ),
    'entity_type' => 'commerce_order',
    'field_name' => 'field_payment_recipient',
    'label' => 'Payment recipient',
    'required' => 0,
    'settings' => array(
      'entity_translation_sync' => FALSE,
      'max' => '',
      'min' => 0,
      'prefix' => '',
      'suffix' => '',
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 0,
      'module' => 'number',
      'settings' => array(),
      'type' => 'number',
      'weight' => -1,
    ),
  );

  $field_instances['node-project-field_price_in_eur'] = array(
		'bundle' => 'project',
		'default_value' => NULL,
		'deleted' => '0',
		'description' => 'Price in euro. Will be used if the Price field is empty',
		'display' => array(
			'default' => array(
				'label' => 'above',
				'type' => 'number_decimal',
				'settings' => array(
					'thousand_separator' => '',
					'decimal_separator' => '.',
					'scale' => 2,
					'prefix_suffix' => TRUE,
				),
				'module' => 'number',
				'weight' => 19,
			),
			'teaser' => array(
				'type' => 'hidden',
				'label' => 'above',
				'settings' => array(),
				'weight' => 0,
			),
		),
		'entity_type' => 'node',
		'field_name' => 'field_price_in_eur',
		'label' => 'Price in Eur',
		'required' => 0,
		'settings' => array(
			'min' => '0',
			'max' => '',
			'prefix' => '',
			'suffix' => ' €',
			'user_register_form' => FALSE,
			'entity_translation_sync' => FALSE,
		),
		'widget' => array(
			'weight' => '20',
			'type' => 'number',
			'module' => 'number',
			'active' => 0,
			'settings' => array(),
		),
	);


  $field_instances['node-project-field_reward_merit_allocation'] = array(
		'label' => 'Reward merit allocation',
		'widget' => array(
			'weight' => '21',
			'type' => 'number',
			'module' => 'number',
			'active' => 0,
			'settings' => array(),
		),
		'settings' => array(
			'min' => '0',
			'max' => '100',
			'prefix' => '',
			'suffix' => ' %',
			'user_register_form' => FALSE,
			'entity_translation_sync' => FALSE,
		),
		'display' => array(
			'default' => array(
				'label' => 'above',
				'type' => 'number_integer',
				'settings' => array(
					'thousand_separator' => '',
					'decimal_separator' => '.',
					'scale' => 0,
					'prefix_suffix' => TRUE,
				),
				'module' => 'number',
				'weight' => 20,
			),
			'teaser' => array(
				'type' => 'hidden',
				'label' => 'above',
				'settings' => array(),
				'weight' => 0,
			),
		),
		'required' => 0,
		'description' => 'The portion of reward the project is granted that is converted to merits. The rest is given to the project owner.',
		'default_value' => array(
			array(
				'value' => '100',
			),
		),
		'field_name' => 'field_reward_merit_allocation',
		'entity_type' => 'node',
		'bundle' => 'project',
		'deleted' => '0',
	);
	
	$field_instances['node-project-field_chron_beneficiary'] = array(
		'label' => 'Chron beneficiary',
		'widget' => array(
			'weight' => '22',
			'type' => 'entityreference_autocomplete',
			'module' => 'entityreference',
			'active' => 1,
			'settings' => array(
				'match_operator' => 'CONTAINS',
				'size' => '60',
				'path' => '',
			),
		),
		'settings' => array(
			'behaviors' => array(
				'prepopulate' => array(
					'status' => 0,
				),
			),
			'user_register_form' => FALSE,
			'entity_translation_sync' => FALSE,
		),
		'display' => array(
			'default' => array(
				'label' => 'above',
				'type' => 'entityreference_label',
				'settings' => array(
					'link' => FALSE,
					'bypass_access' => FALSE,
				),
				'module' => 'entityreference',
				'weight' => 21,
			),
			'teaser' => array(
				'type' => 'hidden',
				'label' => 'above',
				'settings' => array(),
				'weight' => 0,
			),
		),
		'required' => 0,
		'description' => 'Chron allocation beneficiary from a reward paid to the project. If left empty, the project owner is the beneficiary.',
		'default_value' => NULL,
		'default_value_function' => '',
		'field_name' => 'field_chron_beneficiary',
		'entity_type' => 'node',
		'bundle' => 'project',
		'deleted' => '0',
	);

  // Translatables
  // Included for use with string extractors like potx.
  t('Payer type');
  t('Payment recipient');
  t('paytype can have values "project" or "user," depending on the type of payment that is being done');
  t('the ID of the user or project that is receiving the payment');

  return $field_instances;
}
