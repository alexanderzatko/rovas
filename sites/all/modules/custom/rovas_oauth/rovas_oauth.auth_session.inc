<?php

/**
 * @file
 * Trusted Civic native auth bridge: create/link user + mint OAuth tokens.
 *
 * POST /rovas/civic_auth_session — classic API-KEY + TOKEN of a technical
 * account. Civic has already verified the IdP token.
 */

/**
 * Default Civic provider id → HybridAuth machine name map.
 *
 * @return array
 *   Keys: google|apple|facebook|nabezky|openstreetmap.
 */
function rovas_oauth_civic_auth_provider_defaults() {
  return array(
    'google' => 'Google',
    'apple' => 'Apple',
    'facebook' => 'Facebook',
    'nabezky' => 'Nabezky',
    'openstreetmap' => 'OpenStreetMap',
  );
}

/**
 * Map Civic provider id → HybridAuth machine name.
 *
 * @param string $provider
 *   Request body provider (google|apple|facebook|nabezky|openstreetmap).
 *
 * @return string|false
 *   HybridAuth provider id, or FALSE when unknown.
 */
function rovas_oauth_civic_auth_hybridauth_provider($provider) {
  $provider = drupal_strtolower(trim((string) $provider));
  $map = rovas_oauth_civic_auth_provider_defaults();
  $override = variable_get('rovas_oauth_civic_auth_providers', array());
  if (is_array($override)) {
    foreach ($override as $key => $value) {
      $map[drupal_strtolower(trim((string) $key))] = $value;
    }
  }
  if (!isset($map[$provider]) || $map[$provider] === '' || $map[$provider] === NULL) {
    return FALSE;
  }
  return (string) $map[$provider];
}

/**
 * Normalize email_verified from JSON (bool, 0/1, "true"/"false").
 *
 * @param mixed $raw
 *
 * @return bool|null
 *   TRUE/FALSE, or NULL when missing/invalid.
 */
function rovas_oauth_civic_auth_parse_email_verified($raw) {
  if ($raw === TRUE || $raw === 1 || $raw === '1') {
    return TRUE;
  }
  if ($raw === FALSE || $raw === 0 || $raw === '0') {
    return FALSE;
  }
  if (is_string($raw)) {
    $lower = drupal_strtolower(trim($raw));
    if ($lower === 'true') {
      return TRUE;
    }
    if ($lower === 'false') {
      return FALSE;
    }
  }
  return NULL;
}

/**
 * Build a unique Drupal username from a raw seed string.
 *
 * Shared sanitizer / collision loop for email local-part and OSM display_name
 * seeds. Charset [A-Za-z0-9_.-], length ≤50, `_N` suffixes on collision.
 *
 * @param string $seed
 *   Candidate source (e.g. email local-part or OSM display_name).
 * @param string $collision_salt
 *   Extra entropy for the rare md5 fallback when `_N` exhausts.
 *
 * @return string
 */
function rovas_oauth_civic_auth_username_from_seed($seed, $collision_salt = '') {
  $local = preg_replace('/[^a-zA-Z0-9_\-\.]/', '', (string) $seed);
  if ($local === '' || user_validate_name($local) !== NULL) {
    $local = 'civic_user';
  }
  // USERNAME_MAX_LENGTH is 60; leave room for _N suffix.
  $base = drupal_substr($local, 0, 50);
  $candidate = $base;
  $i = 0;
  while (user_load_by_name($candidate)) {
    $i++;
    $candidate = $base . '_' . $i;
    if ($i > 9999) {
      $candidate = 'civic_' . drupal_substr(md5($seed . $collision_salt . microtime(TRUE)), 0, 12);
      break;
    }
  }
  return $candidate;
}

/**
 * Build a unique Drupal username from email local-part.
 *
 * @param string $email
 *
 * @return string
 */
function rovas_oauth_civic_auth_username_from_email($email) {
  $local = preg_replace('/@.*/', '', (string) $email);
  return rovas_oauth_civic_auth_username_from_seed($local, (string) $email);
}

/**
 * Mint Civic OAuth access + refresh tokens for a Drupal uid.
 *
 * Uses the same lifetimes as the authorize / token path on the civic server.
 *
 * @param int $uid
 *
 * @return array|false
 *   Token payload (rovas_uid, access_token, refresh_token, expires_in,
 *   token_type, scope) or FALSE on failure.
 */
function rovas_oauth_mint_civic_tokens($uid) {
  $uid = (int) $uid;
  if ($uid < 1) {
    return FALSE;
  }
  $account = user_load($uid);
  if (!$account || empty($account->uid) || empty($account->status)) {
    return FALSE;
  }

  if (function_exists('rovas_oauth_ensure_client')) {
    rovas_oauth_ensure_client();
  }

  $server = oauth2_server_load(ROVAS_OAUTH_SERVER);
  $client = oauth2_server_client_load(ROVAS_OAUTH_CLIENT_KEY);
  if (!$server || !$client) {
    watchdog('rovas_oauth', 'civic_auth_session: civic OAuth server/client missing.', array(), WATCHDOG_ERROR);
    return FALSE;
  }

  $storage = new Drupal\oauth2_server\Storage();
  $config = array(
    'access_lifetime' => !empty($server->settings['access_lifetime'])
      ? (int) $server->settings['access_lifetime'] : 3600,
    'refresh_token_lifetime' => !empty($server->settings['refresh_token_lifetime'])
      ? (int) $server->settings['refresh_token_lifetime'] : 1209600,
    'token_type' => 'Bearer',
  );

  try {
    $issuer = new OAuth2\ResponseType\AccessToken($storage, $storage, $config);
    $token = $issuer->createAccessToken(
      ROVAS_OAUTH_CLIENT_KEY,
      $uid,
      'user_profile',
      TRUE
    );
  }
  catch (Exception $e) {
    watchdog_exception('rovas_oauth', $e);
    return FALSE;
  }

  if (empty($token['access_token']) || empty($token['refresh_token'])) {
    return FALSE;
  }

  return array(
    'rovas_uid' => $uid,
    'access_token' => $token['access_token'],
    'refresh_token' => $token['refresh_token'],
    'expires_in' => isset($token['expires_in']) ? (int) $token['expires_in'] : $config['access_lifetime'],
    'token_type' => !empty($token['token_type']) ? $token['token_type'] : 'Bearer',
    'scope' => !empty($token['scope']) ? $token['scope'] : 'user_profile',
  );
}

/**
 * Attach HybridAuth identity for Civic native auth (idempotent per uid+provider+sub).
 *
 * @param object $account
 * @param string $ha_provider
 * @param string $provider_subject
 * @param string $email
 * @param string $display_name
 * @param string $user_language_raw
 */
function rovas_oauth_civic_auth_attach_identity($account, $ha_provider, $provider_subject, $email, $display_name, $user_language_raw) {
  $data = array(
    'provider' => $ha_provider,
    'identifier' => $provider_subject,
    'email' => $email,
    'emailVerified' => $email,
    'username' => $display_name !== '' ? $display_name : $account->name,
    'language' => $user_language_raw,
    'photoURL' => '',
    'profileURL' => '',
  );

  if (function_exists('_hybridauth_identity_save')) {
    _hybridauth_identity_save($data, $account->uid);
  }
  else {
    db_merge('hybridauth_identity')
      ->key(array(
        'uid' => $account->uid,
        'provider' => $ha_provider,
        'provider_identifier' => $provider_subject,
      ))
      ->fields(array('data' => serialize($data)))
      ->execute();
  }

  $ud = array();
  if (isset($account->data)) {
    if (is_array($account->data)) {
      $ud = $account->data;
    }
    elseif (is_string($account->data)) {
      $un = @unserialize($account->data);
      if (is_array($un)) {
        $ud = $un;
      }
    }
  }
  if (empty($ud['hybridauth'])) {
    $ud['hybridauth'] = $data;
    user_save($account, array('data' => $ud));
  }
}

/**
 * Services callback: POST /rovas/civic_auth_session.
 *
 * @param array $data
 *   Request body.
 *
 * @return array|object
 *   Mint JSON or services_error().
 */
function rovas_oauth_civic_auth_session_create($data) {
  if (!is_array($data)) {
    return services_error(t('Request body must be a JSON object.'), 400);
  }

  $provider_raw = isset($data['provider']) ? trim((string) $data['provider']) : '';
  $provider_key = drupal_strtolower($provider_raw);
  $ha_provider = rovas_oauth_civic_auth_hybridauth_provider($provider_raw);
  if ($ha_provider === FALSE) {
    return services_error(t('provider must be google, apple, facebook, nabezky, or openstreetmap.'), 400);
  }

  if (function_exists('rovas_site_rules_hybridauth_provider_is_allowed')
    && !rovas_site_rules_hybridauth_provider_is_allowed($ha_provider)) {
    watchdog('rovas_oauth', 'civic_auth_session: HybridAuth provider @p is not enabled.', array(
      '@p' => $ha_provider,
    ), WATCHDOG_WARNING);
    return services_error(t('HybridAuth provider @p is not enabled on this site.', array(
      '@p' => $ha_provider,
    )), 400);
  }

  $provider_subject = isset($data['provider_subject']) ? trim((string) $data['provider_subject']) : '';
  if ($provider_subject === '' || drupal_strlen($provider_subject) > 255) {
    return services_error(t('provider_subject must be a non-empty string (max 255).'), 400);
  }

  $email_verified = rovas_oauth_civic_auth_parse_email_verified(
    array_key_exists('email_verified', $data) ? $data['email_verified'] : NULL
  );
  if ($email_verified === NULL) {
    return services_error(t('email_verified must be a boolean.'), 400);
  }

  $email = isset($data['email']) ? trim((string) $data['email']) : '';
  if ($email !== '' && !valid_email_address($email)) {
    return services_error(t('Invalid email address.'), 400);
  }

  if (!isset($data['user_language']) || trim((string) $data['user_language']) === '') {
    return services_error(t('Missing required field: @field.', array('@field' => 'user_language')), 400);
  }
  if (!isset($data['user_country']) || trim((string) $data['user_country']) === '') {
    return services_error(t('Missing required field: @field.', array('@field' => 'user_country')), 400);
  }

  if (!function_exists('rovas_base_validate_iso3166_alpha2_uppercase')) {
    return services_error(t('Server configuration error.'), 500);
  }
  $iso2 = rovas_base_validate_iso3166_alpha2_uppercase($data['user_country']);
  if ($iso2 === FALSE) {
    return services_error(t('user_country must be a two-letter ISO 3166-1 alpha-2 code using uppercase ASCII letters only (e.g. SK).'), 400);
  }
  if (!function_exists('country_load') || !country_load($iso2)) {
    return services_error(t('Unknown or unavailable country code.'), 400);
  }

  $user_language_raw = trim((string) $data['user_language']);
  $mapped_language = function_exists('rovas_base_map_partner_user_language')
    ? rovas_base_map_partner_user_language($user_language_raw)
    : 'en';

  // Real name only when Civic/IdP sent one — never invent from email local-part.
  $display_name = isset($data['display_name']) ? trim((string) $data['display_name']) : '';

  try {
    $account = NULL;

    // 1) HybridAuth identity (provider + subject).
    if (function_exists('_hybridauth_identity_load')) {
      $identity = _hybridauth_identity_load(array(
        'provider' => $ha_provider,
        'identifier' => $provider_subject,
      ));
      if (!empty($identity['uid'])) {
        $account = user_load((int) $identity['uid']);
        if (!$account || empty($account->uid) || empty($account->status)) {
          return services_error(t('Linked account is unavailable.'), 500);
        }
      }
    }

    // 2) Email match, or 3) create.
    // OpenStreetMap: Civic collects email in-app; allow email_verified=false.
    // Other providers still require a verified email when no HybridAuth hit.
    if (!$account) {
      $allow_unverified_email = ($provider_key === 'openstreetmap');
      if ($email === '' || (!$email_verified && !$allow_unverified_email)) {
        return services_error(t('A verified email is required to create or link this account.'), 409);
      }

      $existing = user_load_by_mail($email);
      if ($existing && !empty($existing->uid)) {
        if (empty($existing->status)) {
          return services_error(t('Linked account is unavailable.'), 500);
        }
        // Guard: same IdP subject must not already belong to another uid.
        if (function_exists('_hybridauth_identity_load')) {
          $taken = _hybridauth_identity_load(array(
            'provider' => $ha_provider,
            'identifier' => $provider_subject,
          ));
          if (!empty($taken['uid']) && (int) $taken['uid'] !== (int) $existing->uid) {
            return services_error(t('provider_subject is already linked to another account.'), 409);
          }
        }
        $account = $existing;
        rovas_oauth_civic_auth_attach_identity(
          $account,
          $ha_provider,
          $provider_subject,
          $email,
          $display_name,
          $user_language_raw
        );
      }
      else {
        // OpenStreetMap display_name is an OSM username, not a legal name:
        // seed Drupal name from it and leave field_full_name empty.
        if ($provider_key === 'openstreetmap' && $display_name !== '') {
          $username = rovas_oauth_civic_auth_username_from_seed($display_name, $display_name);
        }
        else {
          $username = rovas_oauth_civic_auth_username_from_email($email);
        }
        $new = new stdClass();
        $new->is_new = TRUE;
        $new->name = $username;
        $new->mail = $email;
        $new->init = $email;
        $new->status = 1;
        $new->timezone = date_default_timezone_get();
        $new->language = $mapped_language;
        require_once DRUPAL_ROOT . '/' . variable_get('password_inc', 'includes/password.inc');
        $new->pass = user_hash_password(user_password(32));

        $edit = array(
          'field_country' => array(LANGUAGE_NONE => array(0 => array('iso2' => $iso2))),
          'data' => array(
            ROVAS_OAUTH_SIGNUP_SOURCE_KEY => ROVAS_OAUTH_SIGNUP_SOURCE_CIVIC,
          ),
        );
        // Other IdPs: non-empty display_name → real name. OSM: never set.
        if ($provider_key !== 'openstreetmap' && $display_name !== '') {
          $edit['field_full_name'] = array(LANGUAGE_NONE => array(0 => array('value' => $display_name)));
        }

        $account = user_save($new, $edit);
        if (!$account || empty($account->uid)) {
          return services_error(t('Could not create user account.'), 500);
        }

        rovas_oauth_civic_auth_attach_identity(
          $account,
          $ha_provider,
          $provider_subject,
          $email,
          $display_name !== '' ? $display_name : $account->name,
          $user_language_raw
        );

        // Reload after attach; re-stamp signup source in case attach's
        // user_save saw serialized $account->data and dropped the key.
        $account = user_load($account->uid);
        $ud = (is_object($account) && is_array($account->data)) ? $account->data : array();
        if (empty($ud[ROVAS_OAUTH_SIGNUP_SOURCE_KEY])) {
          $ud[ROVAS_OAUTH_SIGNUP_SOURCE_KEY] = ROVAS_OAUTH_SIGNUP_SOURCE_CIVIC;
          user_save($account, array('data' => $ud));
          $account = user_load($account->uid);
        }

        if (function_exists('rules_invoke_component')) {
          rules_invoke_component('rules_send_welcome_email', $account);
        }
      }
    }
    else {
      // Existing identity: attach is already present; ensure identity row is fresh.
      rovas_oauth_civic_auth_attach_identity(
        $account,
        $ha_provider,
        $provider_subject,
        $email !== '' ? $email : $account->mail,
        $display_name !== '' ? $display_name : $account->name,
        $user_language_raw
      );
    }

    $mint = rovas_oauth_mint_civic_tokens((int) $account->uid);
    if ($mint === FALSE) {
      return services_error(t('Unable to mint OAuth tokens.'), 500);
    }
    return $mint;
  }
  catch (ServicesException $e) {
    // Intentional services_error() statuses (e.g. 409 needs email) must reach
    // the REST server unchanged — do not rewrite as a generic 500.
    throw $e;
  }
  catch (Exception $e) {
    watchdog('rovas_oauth', 'civic_auth_session: @msg', array('@msg' => $e->getMessage()), WATCHDOG_ERROR);
    return services_error(t('Unable to complete Civic auth session.'), 500);
  }
}
