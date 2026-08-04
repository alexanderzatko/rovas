<?php

/**
 * @file
 * Admin form for Rovas OAuth (Civic) bootstrap.
 */

/**
 * Form builder: bootstrap / inspect Civic OAuth server + client.
 */
function rovas_oauth_admin_form($form, &$form_state) {
  $server = oauth2_server_load(ROVAS_OAUTH_SERVER);
  $client = oauth2_server_client_load(ROVAS_OAUTH_CLIENT_KEY);

  $form['status'] = array(
    '#type' => 'fieldset',
    '#title' => t('Status'),
  );
  $form['status']['server'] = array(
    '#type' => 'item',
    '#title' => t('OAuth2 server'),
    '#markup' => $server
      ? t('Present: @name (PKCE enforce: @pkce)', array(
        '@name' => $server->name,
        '@pkce' => !empty($server->settings['enforce_pkce']) ? t('yes') : t('no'),
      ))
      : t('Missing — click Bootstrap below.'),
  );
  $form['status']['client'] = array(
    '#type' => 'item',
    '#title' => t('Civic client'),
    '#markup' => $client
      ? t('Present: client_id=@id<br>Redirect URIs:<pre>@uris</pre>', array(
        '@id' => check_plain($client->client_key),
        '@uris' => check_plain($client->redirect_uri),
      ))
      : t('Missing — click Bootstrap below.'),
  );

  $form['endpoints'] = array(
    '#type' => 'fieldset',
    '#title' => t('Endpoints (this site)'),
  );
  $form['endpoints']['list'] = array(
    '#markup' => '<ul>'
      . '<li><code>' . check_plain(url('oauth2/authorize', array('absolute' => TRUE))) . '</code></li>'
      . '<li><code>' . check_plain(url('oauth2/token', array('absolute' => TRUE))) . '</code></li>'
      . '<li><code>' . check_plain(url('oauth2/revoke', array('absolute' => TRUE))) . '</code></li>'
      . '</ul>',
  );

  $form['expo_redirect'] = array(
    '#type' => 'textfield',
    '#title' => t('Extra Expo / dev redirect URI'),
    '#description' => t('Optional. Added to the Civic client on bootstrap (in addition to @prod).', array(
      '@prod' => ROVAS_OAUTH_REDIRECT_PROD,
    )),
    '#default_value' => variable_get('rovas_oauth_expo_redirect', ''),
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['bootstrap'] = array(
    '#type' => 'submit',
    '#value' => t('Bootstrap Civic OAuth server + client'),
  );

  return $form;
}

/**
 * Submit handler for rovas_oauth_admin_form.
 */
function rovas_oauth_admin_form_submit($form, &$form_state) {
  $extra = trim($form_state['values']['expo_redirect']);
  variable_set('rovas_oauth_expo_redirect', $extra);
  rovas_oauth_ensure_server();
  rovas_oauth_ensure_client($extra !== '' ? $extra : NULL);
  drupal_set_message(t('Civic OAuth server and client are ready. Enable OAuth2 authentication on the Services endpoint when wiring Bearer access for rules.'));
}
