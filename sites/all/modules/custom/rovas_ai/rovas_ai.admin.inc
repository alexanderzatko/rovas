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

  $form['rovas_ai_api_key'] = array(
    '#type' => 'password',
    '#title' => t('API key'),
    '#description' => t('Leave blank to keep the current key. Stored in the database; never logged.'),
    '#maxlength' => 512,
  );
  $key = variable_get('rovas_ai_api_key', '');
  if ($key !== '') {
    $form['rovas_ai_api_key']['#description'] = t('A key is already stored. Leave blank to keep it, or enter a new key to replace it.');
  }

  $form['rovas_ai_base_url'] = array(
    '#type' => 'textfield',
    '#title' => t('API base URL'),
    '#default_value' => variable_get('rovas_ai_base_url', ROVAS_AI_DEFAULT_BASE_URL),
    '#description' => t('OpenAI-compatible base ending in /v1 (e.g. https://api.openai.com/v1). Azure/proxies: set the compatible base URL.'),
    '#maxlength' => 255,
  );

  $form['rovas_ai_model'] = array(
    '#type' => 'textfield',
    '#title' => t('Default model'),
    '#default_value' => variable_get('rovas_ai_model', ROVAS_AI_DEFAULT_MODEL),
    '#description' => t('Used when a task has no override. Prefer inexpensive models.'),
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
 * Save AI settings.
 */
function rovas_ai_admin_form_submit($form, &$form_state) {
  $v = $form_state['values'];
  variable_set('rovas_ai_provider', $v['rovas_ai_provider']);
  $new_key = isset($v['rovas_ai_api_key']) ? trim((string) $v['rovas_ai_api_key']) : '';
  if ($new_key !== '') {
    variable_set('rovas_ai_api_key', $new_key);
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
