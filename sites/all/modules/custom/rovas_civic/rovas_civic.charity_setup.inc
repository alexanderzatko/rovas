<?php

/**
 * @file
 * Ensure charity_organization type + charity fields (called from update 7018).
 *
 * Kept separate from hook_schema updates so the definitions stay readable.
 */

/**
 * Create content type, fields, instances, permissions, and ET settings.
 */
function rovas_civic_ensure_charity_content() {
  module_load_include('inc', 'rovas_civic', 'rovas_civic.charity');

  _rovas_civic_ensure_charity_organization_type();
  _rovas_civic_ensure_charity_field_bases();
  _rovas_civic_ensure_charity_field_instances();
  _rovas_civic_refresh_charity_user_labels();
  _rovas_civic_ensure_charity_permissions();
  _rovas_civic_ensure_charity_entity_translation();
  field_cache_clear();
  menu_rebuild();
}

/**
 * Update human labels on existing charity type + project field instances.
 *
 * Wire names stay charity_*; product framing is public-benefit / nonprofit.
 */
function _rovas_civic_refresh_charity_user_labels() {
  $updates = array(
    array(
      'entity_type' => 'node',
      'bundle' => 'project',
      'field_name' => 'field_is_charity',
      'label' => 'Public-benefit / nonprofit project',
      'description' => 'This is a project of a registered public-benefit organization (nonprofit).',
    ),
    array(
      'entity_type' => 'node',
      'bundle' => 'project',
      'field_name' => 'field_charity_organization',
      'label' => 'Public-benefit / nonprofit organization',
      'description' => 'Required when the public-benefit / nonprofit checkbox is on. Create the organization once and reuse it.',
    ),
    array(
      'entity_type' => 'node',
      'bundle' => ROVAS_CIVIC_CHARITY_BUNDLE,
      'field_name' => 'field_charity_status',
      'label' => NULL,
      'description' => 'NeoFund review for public-benefit / nonprofit eligibility. Owners cannot self-approve.',
    ),
  );
  foreach ($updates as $spec) {
    $instance = field_info_instance($spec['entity_type'], $spec['field_name'], $spec['bundle']);
    if (!$instance) {
      continue;
    }
    $changed = FALSE;
    if ($spec['label'] !== NULL && $instance['label'] !== $spec['label']) {
      $instance['label'] = $spec['label'];
      $changed = TRUE;
    }
    if ($spec['description'] !== NULL
      && (!isset($instance['description']) || $instance['description'] !== $spec['description'])) {
      $instance['description'] = $spec['description'];
      $changed = TRUE;
    }
    if ($changed) {
      field_update_instance($instance);
    }
  }
}

/**
 * Node type charity_organization (human label: public-benefit / nonprofit).
 */
function _rovas_civic_ensure_charity_organization_type() {
  $name = 'Public-benefit / nonprofit organization';
  $description = 'Verified public-benefit / nonprofit organization. Linked from projects; NeoFund approves once for all linked projects.';
  $types = node_type_get_types();
  if (isset($types[ROVAS_CIVIC_CHARITY_BUNDLE])) {
    $type = $types[ROVAS_CIVIC_CHARITY_BUNDLE];
    if ($type->name !== $name || $type->description !== $description) {
      $type->name = $name;
      $type->description = $description;
      node_type_save($type);
      node_types_rebuild();
    }
    return;
  }
  $type = array(
    'type' => ROVAS_CIVIC_CHARITY_BUNDLE,
    'name' => $name,
    'base' => 'node_content',
    'description' => $description,
    'custom' => 1,
    'modified' => 1,
    'locked' => 0,
    'disabled' => 0,
    'module' => 'node',
    'has_title' => 1,
    'title_label' => 'Legal name',
    'help' => '',
  );
  $type = node_type_set_defaults($type);
  node_type_save($type);
  node_types_rebuild();
  // No body field — org identity is field-based.
}

/**
 * Field bases shared by org + project where applicable.
 */
function _rovas_civic_ensure_charity_field_bases() {
  $bases = array(
    'field_is_charity' => array(
      'field_name' => 'field_is_charity',
      'type' => 'list_boolean',
      'module' => 'list',
      'active' => 1,
      'locked' => 0,
      'cardinality' => 1,
      'translatable' => 0,
      'settings' => array(
        'allowed_values' => array(
          0 => '',
          1 => '',
        ),
        'allowed_values_function' => '',
      ),
    ),
    'field_charity_organization' => array(
      'field_name' => 'field_charity_organization',
      'type' => 'entityreference',
      'module' => 'entityreference',
      'active' => 1,
      'locked' => 0,
      'cardinality' => 1,
      'translatable' => 0,
      'settings' => array(
        'target_type' => 'node',
        'handler' => 'base',
        'handler_settings' => array(
          'target_bundles' => array(
            ROVAS_CIVIC_CHARITY_BUNDLE => ROVAS_CIVIC_CHARITY_BUNDLE,
          ),
          'sort' => array(
            'type' => 'property',
            'property' => 'title',
            'direction' => 'ASC',
          ),
        ),
      ),
    ),
    'field_charity_status' => array(
      'field_name' => 'field_charity_status',
      'type' => 'list_text',
      'module' => 'list',
      'active' => 1,
      'locked' => 0,
      'cardinality' => 1,
      'translatable' => 0,
      'settings' => array(
        'allowed_values' => array(
          'pending' => 'Pending NeoFund review',
          'approved' => 'Approved',
        ),
        'allowed_values_function' => '',
      ),
    ),
    'field_charity_ico' => array(
      'field_name' => 'field_charity_ico',
      'type' => 'text',
      'module' => 'text',
      'active' => 1,
      'locked' => 0,
      'cardinality' => 1,
      'translatable' => 0,
      'settings' => array(
        'max_length' => 64,
      ),
    ),
    'field_charity_country' => array(
      'field_name' => 'field_charity_country',
      'type' => 'text',
      'module' => 'text',
      'active' => 1,
      'locked' => 0,
      'cardinality' => 1,
      'translatable' => 0,
      'settings' => array(
        'max_length' => 2,
      ),
    ),
    'field_charity_website' => array(
      'field_name' => 'field_charity_website',
      'type' => 'text',
      'module' => 'text',
      'active' => 1,
      'locked' => 0,
      'cardinality' => 1,
      'translatable' => 0,
      'settings' => array(
        'max_length' => 255,
      ),
    ),
    'field_charity_contact_email' => array(
      'field_name' => 'field_charity_contact_email',
      'type' => 'email',
      'module' => 'email',
      'active' => 1,
      'locked' => 0,
      'cardinality' => 1,
      'translatable' => 0,
      'settings' => array(),
    ),
    'field_charity_iban' => array(
      'field_name' => 'field_charity_iban',
      'type' => 'text',
      'module' => 'text',
      'active' => 1,
      'locked' => 0,
      'cardinality' => 1,
      'translatable' => 0,
      'settings' => array(
        'max_length' => 34,
      ),
    ),
    'field_charity_iban_verified' => array(
      'field_name' => 'field_charity_iban_verified',
      'type' => 'list_boolean',
      'module' => 'list',
      'active' => 1,
      'locked' => 0,
      'cardinality' => 1,
      'translatable' => 0,
      'settings' => array(
        'allowed_values' => array(
          0 => '',
          1 => '',
        ),
        'allowed_values_function' => '',
      ),
    ),
    'field_charity_donate_text' => array(
      'field_name' => 'field_charity_donate_text',
      'type' => 'text_long',
      'module' => 'text',
      'active' => 1,
      'locked' => 0,
      'cardinality' => 1,
      'translatable' => 1,
      'settings' => array(),
    ),
    'field_charity_donate_url' => array(
      'field_name' => 'field_charity_donate_url',
      'type' => 'text',
      'module' => 'text',
      'active' => 1,
      'locked' => 0,
      'cardinality' => 1,
      'translatable' => 1,
      'settings' => array(
        'max_length' => 512,
      ),
    ),
  );

  // Fall back to text if email module is absent.
  if (!module_exists('email')) {
    $bases['field_charity_contact_email'] = array(
      'field_name' => 'field_charity_contact_email',
      'type' => 'text',
      'module' => 'text',
      'active' => 1,
      'locked' => 0,
      'cardinality' => 1,
      'translatable' => 0,
      'settings' => array(
        'max_length' => 255,
      ),
    );
  }

  foreach ($bases as $name => $def) {
    if (!field_info_field($name)) {
      field_create_field($def);
    }
  }
}

/**
 * Attach field instances to charity_organization and project.
 */
function _rovas_civic_ensure_charity_field_instances() {
  $org_instances = array(
    'field_charity_status' => array(
      'label' => 'Approval status',
      'required' => 1,
      'default_value' => array(
        array('value' => 'pending'),
      ),
      'widget' => array(
        'type' => 'options_select',
        'weight' => 1,
      ),
      'display' => array(
        'default' => array(
          'label' => 'above',
          'type' => 'list_default',
          'weight' => 1,
        ),
      ),
      'description' => 'NeoFund review for public-benefit / nonprofit eligibility. Owners cannot self-approve.',
    ),
    'field_charity_ico' => array(
      'label' => 'IČO / registration number',
      'required' => 0,
      'widget' => array(
        'type' => 'text_textfield',
        'weight' => 2,
      ),
      'display' => array(
        'default' => array(
          'label' => 'above',
          'type' => 'text_default',
          'weight' => 2,
        ),
      ),
    ),
    'field_charity_country' => array(
      'label' => 'Country of registered seat (ISO)',
      'required' => 0,
      'widget' => array(
        'type' => 'text_textfield',
        'weight' => 3,
        'settings' => array('size' => 4),
      ),
      'display' => array(
        'default' => array(
          'label' => 'above',
          'type' => 'text_default',
          'weight' => 3,
        ),
      ),
      'description' => 'ISO 3166-1 alpha-2 (e.g. SK). Used later for national hourly rate.',
    ),
    'field_charity_website' => array(
      'label' => 'Website',
      'required' => 0,
      'widget' => array(
        'type' => 'text_textfield',
        'weight' => 4,
      ),
      'display' => array(
        'default' => array(
          'label' => 'above',
          'type' => 'text_default',
          'weight' => 4,
        ),
      ),
    ),
    'field_charity_contact_email' => array(
      'label' => 'Contact email',
      'required' => 0,
      'widget' => array(
        'type' => module_exists('email') ? 'email_default' : 'text_textfield',
        'weight' => 5,
      ),
      'display' => array(
        'default' => array(
          'label' => 'above',
          'type' => module_exists('email') ? 'email_default' : 'text_default',
          'weight' => 5,
        ),
      ),
    ),
    'field_charity_iban' => array(
      'label' => 'SEPA IBAN',
      'required' => 0,
      'widget' => array(
        'type' => 'text_textfield',
        'weight' => 6,
      ),
      'display' => array(
        'default' => array(
          'label' => 'above',
          'type' => 'text_default',
          'weight' => 6,
        ),
      ),
      'description' => 'For later pass-through remittance. Not shown in Civic catalog.',
    ),
    'field_charity_iban_verified' => array(
      'label' => 'IBAN verified',
      'required' => 0,
      'default_value' => array(
        array('value' => 0),
      ),
      'widget' => array(
        'type' => 'options_onoff',
        'weight' => 7,
        'settings' => array(
          'display_label' => 1,
        ),
      ),
      'display' => array(
        'default' => array(
          'label' => 'above',
          'type' => 'list_default',
          'weight' => 7,
        ),
      ),
      'description' => 'NeoFund staff only. Marks that the org controls this IBAN (e.g. challenge payment or manual review). Cleared automatically when the IBAN changes.',
    ),
    'field_charity_donate_text' => array(
      'label' => 'Default donate instructions',
      'required' => 0,
      'widget' => array(
        'type' => 'text_textarea',
        'weight' => 8,
        'settings' => array('rows' => 4),
      ),
      'display' => array(
        'default' => array(
          'label' => 'above',
          'type' => 'text_default',
          'weight' => 8,
        ),
      ),
      'description' => 'Shown in Civic when a project does not override donate instructions.',
      'settings' => array(
        'text_processing' => 0,
      ),
    ),
    'field_charity_donate_url' => array(
      'label' => 'Default donate URL',
      'required' => 0,
      'widget' => array(
        'type' => 'text_textfield',
        'weight' => 9,
      ),
      'display' => array(
        'default' => array(
          'label' => 'above',
          'type' => 'text_default',
          'weight' => 9,
        ),
      ),
      'description' => 'Absolute URL; used when a project has no override.',
    ),
  );

  foreach ($org_instances as $field_name => $extra) {
    if (!field_info_instance('node', $field_name, ROVAS_CIVIC_CHARITY_BUNDLE)) {
      $instance = array(
        'field_name' => $field_name,
        'entity_type' => 'node',
        'bundle' => ROVAS_CIVIC_CHARITY_BUNDLE,
        'deleted' => 0,
      ) + $extra;
      if (!isset($instance['settings'])) {
        $instance['settings'] = array('user_register_form' => FALSE);
      }
      field_create_instance($instance);
    }
  }

  $project_instances = array(
    'field_is_charity' => array(
      'label' => 'Public-benefit / nonprofit project',
      'required' => 0,
      'default_value' => array(
        array('value' => 0),
      ),
      'widget' => array(
        'type' => 'options_onoff',
        'weight' => 35,
        'settings' => array(
          'display_label' => 1,
        ),
      ),
      'display' => array(
        'default' => array(
          'label' => 'above',
          'type' => 'list_default',
          'weight' => 35,
        ),
      ),
      'description' => 'This is a project of a registered public-benefit organization (nonprofit).',
    ),
    'field_charity_organization' => array(
      'label' => 'Public-benefit / nonprofit organization',
      'required' => 0,
      'widget' => array(
        'type' => 'entityreference_autocomplete',
        'weight' => 36,
        'settings' => array(
          'match_operator' => 'CONTAINS',
          'size' => 60,
          'path' => '',
        ),
      ),
      'display' => array(
        'default' => array(
          'label' => 'above',
          'type' => 'entityreference_label',
          'weight' => 36,
        ),
      ),
      'description' => 'Required when the public-benefit / nonprofit checkbox is on. Create the organization once and reuse it.',
    ),
    'field_charity_donate_text' => array(
      'label' => 'Donate instructions (project override)',
      'required' => 0,
      'widget' => array(
        'type' => 'text_textarea',
        'weight' => 37,
        'settings' => array('rows' => 3),
      ),
      'display' => array(
        'default' => array(
          'label' => 'above',
          'type' => 'text_default',
          'weight' => 37,
        ),
      ),
      'description' => 'Optional. Overrides the organization’s default donate instructions for this project.',
      'settings' => array(
        'text_processing' => 0,
        'user_register_form' => FALSE,
      ),
    ),
    'field_charity_donate_url' => array(
      'label' => 'Donate URL (project override)',
      'required' => 0,
      'widget' => array(
        'type' => 'text_textfield',
        'weight' => 38,
      ),
      'display' => array(
        'default' => array(
          'label' => 'above',
          'type' => 'text_default',
          'weight' => 38,
        ),
      ),
      'description' => 'Optional absolute URL. Overrides the organization’s default donate URL.',
    ),
  );

  foreach ($project_instances as $field_name => $extra) {
    if (!field_info_instance('node', $field_name, 'project')) {
      $instance = array(
        'field_name' => $field_name,
        'entity_type' => 'node',
        'bundle' => 'project',
        'deleted' => 0,
      ) + $extra;
      if (!isset($instance['settings'])) {
        $instance['settings'] = array('user_register_form' => FALSE);
      }
      field_create_instance($instance);
    }
  }
}

/**
 * Grant authenticated users create/edit own charity_organization.
 */
function _rovas_civic_ensure_charity_permissions() {
  $perms = array(
    'create ' . ROVAS_CIVIC_CHARITY_BUNDLE . ' content',
    'edit own ' . ROVAS_CIVIC_CHARITY_BUNDLE . ' content',
    'delete own ' . ROVAS_CIVIC_CHARITY_BUNDLE . ' content',
  );
  foreach (array('authenticated user') as $role_name) {
    $role = user_role_load_by_name($role_name);
    if ($role) {
      user_role_grant_permissions($role->rid, $perms);
    }
  }
  // Staff editors (if present).
  foreach (array('administrator', 'editor', 'content editor') as $role_name) {
    $role = user_role_load_by_name($role_name);
    if ($role) {
      user_role_grant_permissions($role->rid, array_merge($perms, array(
        'edit any ' . ROVAS_CIVIC_CHARITY_BUNDLE . ' content',
        'delete any ' . ROVAS_CIVIC_CHARITY_BUNDLE . ' content',
      )));
    }
  }
}

/**
 * Enable Entity Translation for charity_organization when available.
 */
function _rovas_civic_ensure_charity_entity_translation() {
  if (!module_exists('entity_translation')) {
    return;
  }
  // Mirror common D7 ET setup: content-type language setting.
  if (defined('ENTITY_TRANSLATION_ENABLED')) {
    variable_set('language_content_type_' . ROVAS_CIVIC_CHARITY_BUNDLE, ENTITY_TRANSLATION_ENABLED);
  }
  else {
    // ENTITY_TRANSLATION_ENABLED === 4 in entity_translation.module.
    variable_set('language_content_type_' . ROVAS_CIVIC_CHARITY_BUNDLE, 4);
  }
}
