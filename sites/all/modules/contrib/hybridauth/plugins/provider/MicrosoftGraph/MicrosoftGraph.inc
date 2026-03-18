<?php

/**
 * @file
 * HybridAuth Microsoft Graph specific settings.
 */

/**
 * Plugin declaration.
 */
$plugin = array(
  'configuration_callback' => 'hybridauth_provider_microsoftgraph_configuration_callback',
  'configuration_form_callback' => 'hybridauth_provider_microsoftgraph_configuration_form_callback',
);

function hybridauth_provider_microsoftgraph_configuration_callback(&$config, $provider_id) {
  $config['scope'] = 'User.Read';
}

function hybridauth_provider_microsoftgraph_configuration_form_callback(&$form, $provider_id) {
  unset($form['vtabs']['application']['hybridauth_provider_' . $provider_id . '_keys_key']);
}
