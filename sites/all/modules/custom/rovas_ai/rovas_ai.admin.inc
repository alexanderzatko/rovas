<?php

/**
 * @file
 * Admin form: LLM provider, model, keys, thresholds.
 */

/**
 * Settings form: Configuration → Rovas → AI.
 */
function rovas_ai_admin_form($form, &$form_state) {
  $providers = rovas_ai_providers();
  $provider_options = array();
  foreach ($providers as $id => $info) {
    $provider_options[$id] = $info['label'];
  }

  $form['intro'] = array(
    '#markup' => '<p>' . t('Prefer cheaper chat models (default gpt-4o-mini). NACE classification uses a two-pass prompt so the full taxonomy is never sent in one request.') . '</p>',
  );

  $form['rovas_ai_provider'] = array(
    '#type' => 'select',
    '#title' => t('Active provider'),
    '#options' => $provider_options,
    '#default_value' => variable_get('rovas_ai_provider', 'openai'),
    '#required' => TRUE,
  );

  $key = trim((string) variable_get('rovas_ai_api_key', ''));
  $form['api_key_status'] = array(
    '#type' => 'item',
    '#title' => t('Current API key'),
    '#markup' => $key !== ''
      ? t('Stored (@hint). Leave the field below empty to keep it.', array(
        '@hint' => rovas_ai_api_key_hint($key),
      ))
      : t('Not set — enter a key below.'),
  );
  // Field machine name must NOT be rovas_ai_api_key: password managers and any
  // blanket variable_set of form values would overwrite the stored key with "".
  $form['api_key_new'] = array(
    '#type' => 'textfield',
    '#title' => t('New API key'),
    '#default_value' => '',
    '#description' => t('Leave blank to keep the current key. Stored in the database; never logged.'),
    '#maxlength' => 512,
    '#attributes' => array(
      'autocomplete' => 'off',
      'autocorrect' => 'off',
      'autocapitalize' => 'off',
      'spellcheck' => 'false',
    ),
  );

  $form['rovas_ai_base_url'] = array(
    '#type' => 'textfield',
    '#title' => t('API base URL'),
    '#default_value' => variable_get('rovas_ai_base_url', ROVAS_AI_DEFAULT_BASE_URL),
    '#description' => t('OpenAI-compatible base ending in /v1 (e.g. https://api.openai.com/v1 or https://openrouter.ai/api/v1).'),
    '#maxlength' => 255,
  );

  $form['rovas_ai_model'] = array(
    '#type' => 'textfield',
    '#title' => t('Default model'),
    '#default_value' => variable_get('rovas_ai_model', ROVAS_AI_DEFAULT_MODEL),
    '#description' => t('Used when a task has no override. Prefer inexpensive models. OpenRouter: use slug like openai/gpt-4o-mini.'),
    '#required' => TRUE,
    '#maxlength' => 128,
  );

  $task_models = variable_get('rovas_ai_task_models', array());
  if (!is_array($task_models)) {
    $task_models = array();
  }
  $form['rovas_ai_task_model_nace'] = array(
    '#type' => 'textfield',
    '#title' => t('Model override: NACE classify'),
    '#default_value' => isset($task_models['nace_classify']) ? $task_models['nace_classify'] : '',
    '#description' => t('Optional. Empty = use default model. Task id: nace_classify.'),
    '#maxlength' => 128,
  );

  $form['rovas_ai_confidence_threshold'] = array(
    '#type' => 'textfield',
    '#title' => t('Confidence threshold'),
    '#default_value' => variable_get('rovas_ai_confidence_threshold', ROVAS_AI_DEFAULT_CONFIDENCE),
    '#description' => t('Below this (0–1), the API returns need_more_info. Default @d.', array(
      '@d' => ROVAS_AI_DEFAULT_CONFIDENCE,
    )),
    '#size' => 6,
    '#required' => TRUE,
  );

  $form['rovas_ai_daily_budget'] = array(
    '#type' => 'textfield',
    '#title' => t('Daily call budget per user'),
    '#default_value' => variable_get('rovas_ai_daily_budget', ROVAS_AI_DEFAULT_DAILY_BUDGET),
    '#description' => t('Each classify request counts as one call (both LLM passes share the same budget slot). Over budget → status unavailable so Civic can fall back to the NACE tree. 0 disables the limit.'),
    '#size' => 6,
    '#element_validate' => array('element_validate_integer'),
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
    '#submit' => array('rovas_ai_admin_form_submit'),
  );
  $form['actions']['resources'] = array(
    '#type' => 'submit',
    '#value' => t('Enable Civic AI APIs'),
    '#submit' => array('rovas_ai_admin_resources_submit'),
  );

  return $form;
}

/**
 * Short non-secret hint for a stored API key (length + last 4 chars).
 *
 * @param string $key
 *
 * @return string
 */
function rovas_ai_api_key_hint($key) {
  $key = (string) $key;
  $len = drupal_strlen($key);
  if ($len <= 4) {
    return t('@n chars', array('@n' => $len));
  }
  return t('@n chars, ends with @s', array(
    '@n' => $len,
    '@s' => drupal_substr($key, -4),
  ));
}

/**
 * Save AI settings.
 */
function rovas_ai_admin_form_submit($form, &$form_state) {
  $v = $form_state['values'];
  variable_set('rovas_ai_provider', $v['rovas_ai_provider']);

  // Never overwrite the stored key with an empty field value.
  $new_key = isset($v['api_key_new']) ? trim((string) $v['api_key_new']) : '';
  if ($new_key !== '') {
    variable_set('rovas_ai_api_key', $new_key);
    drupal_set_message(t('API key updated.'));
  }

  variable_set('rovas_ai_base_url', rovas_ai_normalize_base_url($v['rovas_ai_base_url']));
  variable_set('rovas_ai_model', trim((string) $v['rovas_ai_model']));

  $task_models = variable_get('rovas_ai_task_models', array());
  if (!is_array($task_models)) {
    $task_models = array();
  }
  $override = trim((string) $v['rovas_ai_task_model_nace']);
  if ($override === '') {
    unset($task_models['nace_classify']);
  }
  else {
    $task_models['nace_classify'] = $override;
  }
  variable_set('rovas_ai_task_models', $task_models);

  $threshold = (float) $v['rovas_ai_confidence_threshold'];
  if ($threshold < 0) {
    $threshold = 0;
  }
  if ($threshold > 1) {
    $threshold = 1;
  }
  variable_set('rovas_ai_confidence_threshold', $threshold);
  variable_set('rovas_ai_daily_budget', max(0, (int) $v['rovas_ai_daily_budget']));

  drupal_set_message(t('AI configuration saved.'));
}

/**
 * Enable Services resource on rovas_endpoint.
 */
function rovas_ai_admin_resources_submit($form, &$form_state) {
  if (rovas_ai_ensure_services_resources()) {
    drupal_set_message(t('Civic AI resource (civic_ai_nace) enabled on rovas_endpoint.'));
  }
  else {
    drupal_set_message(t('Could not enable Civic AI resource. Is Services / rovas_endpoint available?'), 'error');
  }
}
