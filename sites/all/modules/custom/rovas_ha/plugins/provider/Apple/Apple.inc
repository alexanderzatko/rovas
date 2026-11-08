<?php

/**
 * @file
 * HybridAuth Apple (Sign in with Apple) specific settings.
 */

/**
 * Plugin declaration.
 */
$plugin = array(
  'configuration_form_callback' => 'rovas_ha_provider_apple_configuration_form_callback',
  'configuration_callback' => 'rovas_ha_provider_apple_configuration_callback',
);

/**
 * Alters the HybridAuth Apple provider settings form.
 */
function rovas_ha_provider_apple_configuration_form_callback(&$form, $provider_id) {
  $app = &$form['vtabs']['application'];

  $redirect_uri = url('hybridauth/endpoint', array(
    'absolute' => TRUE,
    'language' => _hybridauth_language_default(),
  ));
  $origin = parse_url(url('<front>', array('absolute' => TRUE)));
  $domain = !empty($origin['host']) ? $origin['host'] : 'example.com';

  $app['#description'] = t('<p>Sign in with Apple does not use a static client secret. HybridAuth builds a JWT from your Team ID, Key ID and private key.</p>'
    . '<p>Create credentials at <a href="!apps_uri">!apps_uri</a>:</p>'
    . '<ol>'
    . '<li>App ID with <strong>Sign in with Apple</strong> enabled</li>'
    . '<li>Services ID (this is the Application / Client ID) — domains: <strong>%domain</strong>; Return URLs: <strong>%redirect_uri</strong></li>'
    . '<li>Key with Sign in with Apple enabled — note the Key ID and download the <code>.p8</code> file once</li>'
    . '<li>Team ID from your Apple Developer membership page</li>'
    . '</ol>'
    . '<p>HybridAuth generates the client secret automatically; do not create one in Apple’s portal for web.</p>',
    array(
      '!apps_uri' => 'https://developer.apple.com/account/resources',
      '%domain' => $domain,
      '%redirect_uri' => $redirect_uri,
    )
  );

  $app['hybridauth_provider_' . $provider_id . '_keys_id']['#title'] = t('Services ID (Client ID)');
  $app['hybridauth_provider_' . $provider_id . '_keys_id']['#description'] = t('Apple Services ID identifier, e.g. app.rovas.web.');
  $app['hybridauth_provider_' . $provider_id . '_keys_id']['#required'] = TRUE;

  unset($app['hybridauth_provider_' . $provider_id . '_keys_key']);
  unset($app['hybridauth_provider_' . $provider_id . '_keys_secret']);

  $app['hybridauth_provider_' . $provider_id . '_keys_team_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Team ID'),
    '#description' => t('10-character Team ID from your Apple Developer account.'),
    '#default_value' => variable_get('hybridauth_provider_' . $provider_id . '_keys_team_id', ''),
    '#required' => TRUE,
    '#size' => 12,
    '#maxlength' => 10,
  );

  $app['hybridauth_provider_' . $provider_id . '_keys_key_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Key ID'),
    '#description' => t('10-character Key ID of the Sign in with Apple key.'),
    '#default_value' => variable_get('hybridauth_provider_' . $provider_id . '_keys_key_id', ''),
    '#required' => TRUE,
    '#size' => 12,
    '#maxlength' => 10,
  );

  $app['hybridauth_provider_' . $provider_id . '_keys_key_file'] = array(
    '#type' => 'textfield',
    '#title' => t('Private key file path'),
    '#description' => t('Absolute path to the downloaded <code>.p8</code> file on the server (preferred). Keep it outside the web root when possible.'),
    '#default_value' => variable_get('hybridauth_provider_' . $provider_id . '_keys_key_file', ''),
    '#maxlength' => 512,
  );

  $app['hybridauth_provider_' . $provider_id . '_keys_key_content'] = array(
    '#type' => 'textarea',
    '#title' => t('Private key contents'),
    '#description' => t('Paste the full <code>.p8</code> contents including BEGIN/END lines if you are not using a file path. Prefer the file path on production.'),
    '#default_value' => variable_get('hybridauth_provider_' . $provider_id . '_keys_key_content', ''),
    '#rows' => 6,
  );

  $form['vtabs']['window_settings']['hybridauth_provider_' . $provider_id . '_window_type']['#description'] = t('Apple returns the authorization code via POST (<code>response_mode=form_post</code>). Use <strong>Current window</strong>.');

  $form['vtabs']['advanced'] = array(
    '#type' => 'fieldset',
    '#title' => t('Advanced settings'),
  );
  $form['vtabs']['advanced']['hybridauth_provider_' . $provider_id . '_scope'] = array(
    '#type' => 'textfield',
    '#title' => t('Scope'),
    '#default_value' => variable_get('hybridauth_provider_' . $provider_id . '_scope', rovas_ha_provider_apple_scope_default()),
    '#description' => t('Space-separated scopes. Default is <code>name email</code>.'),
    '#size' => 60,
  );
  $form['vtabs']['advanced']['hybridauth_provider_' . $provider_id . '_verify_token_signature'] = array(
    '#type' => 'checkbox',
    '#title' => t('Verify ID token signature'),
    '#default_value' => variable_get('hybridauth_provider_' . $provider_id . '_verify_token_signature', 1),
    '#description' => t('Recommended. Requires <code>firebase/php-jwt</code> and <code>phpseclib/phpseclib</code>.'),
  );

  $form['#validate'][] = 'rovas_ha_provider_apple_configuration_form_validate';
}

/**
 * Ensures Apple credentials include a private key file or pasted key.
 */
function rovas_ha_provider_apple_configuration_form_validate($form, &$form_state) {
  $values = $form_state['values'];
  $file = isset($values['hybridauth_provider_Apple_keys_key_file']) ? trim($values['hybridauth_provider_Apple_keys_key_file']) : '';
  $content = isset($values['hybridauth_provider_Apple_keys_key_content']) ? trim($values['hybridauth_provider_Apple_keys_key_content']) : '';

  if ($file === '' && $content === '') {
    form_set_error('hybridauth_provider_Apple_keys_key_file', t('Provide either a private key file path or private key contents.'));
  }
  if ($file !== '' && !is_readable($file)) {
    form_set_error('hybridauth_provider_Apple_keys_key_file', t('Private key file is not readable: %path', array('%path' => $file)));
  }
}

/**
 * Maps Drupal variables into HybridAuth Apple provider config.
 */
function rovas_ha_provider_apple_configuration_callback(&$config, $provider_id) {
  if (empty($config['scope'])) {
    $config['scope'] = rovas_ha_provider_apple_scope_default();
  }

  $config['keys']['team_id'] = trim(variable_get('hybridauth_provider_' . $provider_id . '_keys_team_id', ''));
  $config['keys']['key_id'] = trim(variable_get('hybridauth_provider_' . $provider_id . '_keys_key_id', ''));

  $key_file = trim(variable_get('hybridauth_provider_' . $provider_id . '_keys_key_file', ''));
  $key_content = trim(variable_get('hybridauth_provider_' . $provider_id . '_keys_key_content', ''));

  // Prefer file path; fall back to pasted key material.
  if ($key_file !== '') {
    $config['keys']['key_file'] = $key_file;
    unset($config['keys']['key_content']);
  }
  elseif ($key_content !== '') {
    // HybridAuth expects PEM text with real newlines.
    $config['keys']['key_content'] = str_replace(array("\r\n", "\r"), "\n", $key_content);
    unset($config['keys']['key_file']);
  }

  // Secret is generated by Hybridauth\Provider\Apple from team/key material.
  unset($config['keys']['secret']);
  unset($config['keys']['key']);

  $config['verifyTokenSignature'] = (bool) variable_get('hybridauth_provider_' . $provider_id . '_verify_token_signature', 1);
}

/**
 * Default Apple OAuth scopes.
 */
function rovas_ha_provider_apple_scope_default() {
  return 'name email';
}
