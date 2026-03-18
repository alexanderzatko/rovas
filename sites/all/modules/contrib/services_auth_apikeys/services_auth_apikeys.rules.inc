<?php

/**
 * Implements hook_rules_action_info().
 */
function services_auth_apikeys_rules_action_info() {
  return array(
    'services_auth_apikeys_rules_action_generate_token_api_key' => array(
      'label' => t('Generate token and api key on account creation'),
      'group' => t('Services Auth Api Keys'),
      'parameter' => array(
        'user_id' => array(
          'type' => 'integer',
          'label' => t('account uid'),
          'description' => t('Enter token user id.'),
        ),
        'service_endpoint' => array(
          'type' => 'text',
          'label' => t('Services Endpoint'),
          'description' => t('Input services endpoint.'),
        ),
      ),
      'provides' => array(
        'generated_api_key' => array(
          'type' => 'text',
          'label' => t('API Key Generated'),
        ),
        'generated_token' => array(
          'type' => 'text',
          'label' => t('Token Generated'),
        ),
      ),
    ),
  );
}

// This callback generates token and api-key.
function services_auth_apikeys_rules_action_generate_token_api_key($user_id, $service_endpoint) {
  services_auth_apikeys_update_apikeys($user_id, $service_endpoint);

  $usrkeys = services_auth_apikeys_load_by_user($user_id);
  $api_key = $usrkeys[$service_endpoint]->api_key;
  $token = $usrkeys[$service_endpoint]->token;

  return array('generated_token' => $token,'generated_api_key' => $api_key);
}
