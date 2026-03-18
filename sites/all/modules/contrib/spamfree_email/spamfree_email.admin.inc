<?php

/**
 * @file
 * Admin configuration form for Spamfree Email.
 */

/**
 * Admin settings form implements hook__form().
 *
 * @return array
 *         returns form array
 */
function spamfree_email_settings_form() {
  $fields_formatter_types = field_info_field_types();
  $fields_formats = array();
  foreach ($fields_formatter_types as $key => $fields) {
    $fields_formats[$key] = $fields['label'];
  }

  if (!empty($fields_formats)) {
    if (isset($fields_formats['datetime'])) {
      unset($fields_formats['datetime']);
    }
    if (isset($fields_formats['date'])) {
      unset($fields_formats['date']);
    }
    if (isset($fields_formats['datestamp'])) {
      unset($fields_formats['datestamp']);
    }
    if (isset($fields_formats['field_collection'])) {
      unset($fields_formats['field_collection']);
    }
    if (isset($fields_formats['node_reference'])) {
      unset($fields_formats['node_reference']);
    }
    if (isset($fields_formats['file'])) {
      unset($fields_formats['file']);
    }
    if (isset($fields_formats['image'])) {
      unset($fields_formats['image']);
    }
    if (isset($fields_formats['list_integer'])) {
      unset($fields_formats['list_integer']);
    }
    if (isset($fields_formats['list_float'])) {
      unset($fields_formats['list_float']);
    }
    if (isset($fields_formats['list_boolean'])) {
      unset($fields_formats['list_boolean']);
    }
    if (isset($fields_formats['number_integer'])) {
      unset($fields_formats['number_integer']);
    }
    if (isset($fields_formats['number_decimal'])) {
      unset($fields_formats['number_decimal']);
    }
    if (isset($fields_formats['number_float'])) {
      unset($fields_formats['number_float']);
    }
    if (isset($fields_formats['taxonomy_term_reference'])) {
      unset($fields_formats['taxonomy_term_reference']);
    }
    if (isset($fields_formats['viewfield'])) {
      unset($fields_formats['viewfield']);
    }
  }

  $fonts_arr = array("" => "Select");
  $font_files = file_scan_directory('public://spamfree_email_fonts', '/.*\.ttf|.TTF$/');
  if (!empty($font_files)) {
    foreach ($font_files as $font_key => $font_value) {
      $fonts_arr[$font_key] = $font_value->name;
    }
  }
  $form = array();
  $form['mark_1_open'] = array(
    '#markup' => "<fieldset class='form-wrapper'>
      <legend><span class=\"fieldset-legend\">Content Settings</span></legend>
      <div class='fieldset-wrapper'>",
  );

  $form['spamfree_email'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Apply Spamfree Email protection for below fields:'),
    '#options' => $fields_formats,
    '#default_value' => variable_get('spamfree_email', array()),
    '#description' => t("Email addresses will be obfuscated for the above fields."),
  );

  $form['mark_1_close'] = array(
    '#markup' => "</div></fieldset>",
  );

  $form['spamfree_email_nofollow'] = array(
    '#type' => 'checkbox',
    '#title' => t('Add rel=“nofollow”'),
    '#default_value' => variable_get('spamfree_email_nofollow', 1),
    '#return_value' => 1,
    '#description' => t('Instruct search engines that they don’t need to follow these email links.'),
  );

  $form['mark_3_open'] = array(
    '#markup' => "<fieldset class='form-wrapper'>
      <legend><span class=\"fieldset-legend\">" . t('Non JS Enabled browser settings') . "</span></legend>
      <div class='fieldset-wrapper'>",
  );

  $form['spamfree_email_nojs'] = array(
    '#type' => 'checkbox',
    '#title' => t("Enable Spamfree email for Javascript disabled browsers?"),
    '#default_value' => variable_get('spamfree_email_nojs', 0),
    '#return_value' => 1,
    '#description' => t('Emails will be converted to images and shown for non Js browsers. Gd library support should be enabled if you need to use this feature.'),
  );

  $form['spamfree_email_text_color'] = array(
    '#type' => 'textfield',
    '#size' => 6,
    '#title' => t('Text color code'),
    '#default_value' => variable_get('spamfree_email_text_color', '#000000'),
    '#description' => t('Enter hex code for text color.'),
  );

  $form['spamfree_email_bg_color'] = array(
    '#type' => 'textfield',
    '#size' => 6,
    '#title' => t('Background color code'),
    '#default_value' => variable_get('spamfree_email_bg_color', '#ffffff'),
    '#description' => t('Enter hex code for text background color.'),
  );

  $form['spamfree_email_font_size'] = array(
    '#type' => 'textfield',
    '#size' => 3,
    '#title' => t('Font Size'),
    '#default_value' => variable_get('spamfree_email_font_size', '12'),
    '#description' => t('Enter the font size in pixels.'),
  );

  $form['spamfree_email_font_upload'] = array(
    '#type' => 'managed_file',
    '#name' => 'spamfree_email_font_upload',
    '#title' => t('Upload Font'),
    '#upload_validators' => array(
      'file_validate_extensions' => array('ttf'),
    ),
    '#upload_location' => 'public://spamfree_email_fonts/',
    '#description' => t('If a font is uploaded you must click "Save Configuration" button, then only you will see the font in below list.'),
  );

  $form['spamfree_email_font_family'] = array(
    '#type' => 'select',
    '#title' => t("Select font family"),
    '#default_value' => variable_get('spamfree_email_font_family', 0),
    '#options' => $fonts_arr,
  );

  $form['mark_3_close'] = array(
    '#markup' => "</div></fieldset>",
  );

  $form['#submit'][] = 'spamfree_email_settings_form_submit';

  return system_settings_form($form);
}

/**
 * Function validating submitte form and checking Gd library.
 */
function spamfree_email_settings_form_validate($form, &$form_state) {
  if (!variable_get('spamfree_email_nojs', 0)) {
    $gd_needed = $form_state['values']['spamfree_email_nojs'];
    if ($gd_needed) {
      // Checking whether gd is enabled.
      if (!extension_loaded('gd') || !function_exists('gd_info')) {
        $form_state['values']['spamfree_email_nojs'] = 0;
        form_set_error('spamfree_email_nojs', t("PHP GD library is NOT installed on your web server"));
      }
    }
  }
}

/**
 * Submit function for font upload.
 */
function spamfree_email_settings_form_submit($form, &$form_state) {
  $file = file_save_upload('spamfree_email_font_upload', array(), 'public://spamfree_email_fonts/', FILE_EXISTS_RENAME);

  if ($file) {
    // Set the status of the uploaded file.
    $file->status = FILE_STATUS_PERMANENT;
    file_save($file);
  }
}
