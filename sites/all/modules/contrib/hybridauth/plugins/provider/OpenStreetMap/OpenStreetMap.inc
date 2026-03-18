<?php

/**
 * @file
 * HybridAuth OpenStreetMap specific settings.
 */

/**
 * Plugin declaration.
 */
$plugin = array(
  'configuration_form_callback' => 'hybridauth_provider_openstreetmap_configuration_form_callback',
  'configuration_callback' => 'hybridauth_provider_openstreetmap_configuration_callback',
);

function hybridauth_provider_openstreetmap_configuration_form_callback(&$form, $provider_id) {
  $form['vtabs']['application']['hybridauth_provider_' . $provider_id . '_keys_id']['#title'] = t('Client ID');
  unset($form['vtabs']['application']['hybridauth_provider_' . $provider_id . '_keys_id']['#description']);
  unset($form['vtabs']['application']['hybridauth_provider_' . $provider_id . '_keys_key']);
  $form['vtabs']['application']['hybridauth_provider_' . $provider_id . '_keys_secret']['#title'] = t('Client secret');
  unset($form['vtabs']['application']['hybridauth_provider_' . $provider_id . '_keys_secret']['#description']);

  $origin_uri_parts = parse_url(url('<front>', array('absolute' => TRUE)));
  $form['vtabs']['application']['#description'] = t('<p>Enter your Client ID and Client secret. You can get these from <a href="https://www.openstreetmap.org/oauth2/applications">your profile at OpenStreetMap</a>.</p>'
    . '<p>You must set <strong>Authorized Redirect URIs</strong> to <strong>%redirect_uri</strong>.</p>',
    [
      '%redirect_uri' => url('hybridauth/endpoint', array('absolute' => TRUE, 'language' => _hybridauth_language_default())),
      '%origin_uri' => $origin_uri_parts['scheme'] . '://' . $origin_uri_parts['host'],
    ]
  );

  $form['vtabs']['advanced'] = array(
    '#type' => 'fieldset',
    '#title' => 'Advanced settings',
  );
  $form['vtabs']['advanced']['hybridauth_provider_' . $provider_id . '_scope'] = array(
    '#type' => 'textfield',
    '#title' => t('Scope'),
    '#default_value' => variable_get('hybridauth_provider_' . $provider_id . '_scope', hybridauth_provider_openstreetmap_scope_default()),
    '#size' => 150,
  );
}

function hybridauth_provider_openstreetmap_configuration_callback(&$config, $provider_id) {
  if (empty($config['scope'])) {
    $config['scope'] = hybridauth_provider_openstreetmap_scope_default();
  }
}

function hybridauth_provider_openstreetmap_scope_default() {
  return 'openid read_prefs';
}
