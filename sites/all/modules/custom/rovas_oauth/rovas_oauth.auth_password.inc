<?php

/**
 * @file
 * Civic email/username + password auth: register, login, mint OAuth tokens.
 *
 * POST /rovas/civic_auth_register
 * POST /rovas/civic_auth_login
 *
 * Classic API-KEY + TOKEN of a technical account (same permission as
 * civic_auth_session). Reuses rovas_oauth_mint_civic_tokens().
 */

/**
 * Minimum password length for Civic email/password register.
 */
define('ROVAS_OAUTH_CIVIC_PASSWORD_MIN_LENGTH', 8);

/**
 * Maximum password length accepted on Civic register/login.
 */
define('ROVAS_OAUTH_CIVIC_PASSWORD_MAX_LENGTH', 128);

/**
 * Validate language + country fields shared with civic_auth_session create.
 *
 * @param array $data
 *   Request body.
 *
 * @return array|object
 *   array('iso2' => string, 'mapped_language' => string) or services_error().
 */
function rovas_oauth_civic_auth_password_validate_locale(array $data) {
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

  return array(
    'iso2' => $iso2,
    'mapped_language' => $mapped_language,
  );
}

/**
 * Validate Civic register password (length only).
 *
 * @param mixed $raw
 *
 * @return string|null
 *   Translated error, or NULL if acceptable.
 */
function rovas_oauth_civic_auth_password_error($raw) {
  if (!is_string($raw)) {
    return t('password must be a string.');
  }
  $len = drupal_strlen($raw);
  if ($len < ROVAS_OAUTH_CIVIC_PASSWORD_MIN_LENGTH) {
    return t('password must be at least @min characters.', array(
      '@min' => ROVAS_OAUTH_CIVIC_PASSWORD_MIN_LENGTH,
    ));
  }
  if ($len > ROVAS_OAUTH_CIVIC_PASSWORD_MAX_LENGTH) {
    return t('password must be at most @max characters.', array(
      '@max' => ROVAS_OAUTH_CIVIC_PASSWORD_MAX_LENGTH,
    ));
  }
  return NULL;
}

/**
 * Services callback: POST /rovas/civic_auth_register.
 *
 * @param array $data
 *   Request body.
 *
 * @return array|object
 *   Mint JSON or services_error().
 */
function rovas_oauth_civic_auth_register_create($data) {
  module_load_include('inc', 'rovas_oauth', 'rovas_oauth.auth_session');
  module_load_include('inc', 'rovas_oauth', 'rovas_oauth.profile');

  if (!is_array($data)) {
    return services_error(t('Request body must be a JSON object.'), 400);
  }

  if (!isset($data['username']) || (!is_string($data['username']) && !is_numeric($data['username']))) {
    return services_error(t('Missing required field: @field.', array('@field' => 'username')), 400);
  }
  $username = trim((string) $data['username']);
  $uname_err = rovas_oauth_civic_profile_username_error($username);
  if ($uname_err !== NULL) {
    return services_error($uname_err, 400);
  }

  if (!isset($data['email']) || (!is_string($data['email']) && !is_numeric($data['email']))) {
    return services_error(t('Missing required field: @field.', array('@field' => 'email')), 400);
  }
  $email = trim((string) $data['email']);
  if ($email === '' || !valid_email_address($email)) {
    return services_error(t('Invalid email address.'), 400);
  }

  if (!array_key_exists('password', $data)) {
    return services_error(t('Missing required field: @field.', array('@field' => 'password')), 400);
  }
  $pass_err = rovas_oauth_civic_auth_password_error($data['password']);
  if ($pass_err !== NULL) {
    return services_error($pass_err, 400);
  }
  $password = $data['password'];

  $locale = rovas_oauth_civic_auth_password_validate_locale($data);
  if (!is_array($locale)) {
    return $locale;
  }

  if (user_load_by_name($username)) {
    $msg = t('username is already taken.');
    return services_error($msg, 409, array(
      'error' => $msg,
      'code' => 'username_taken',
    ));
  }
  if (user_load_by_mail($email)) {
    $msg = t('email is already taken.');
    return services_error($msg, 409, array(
      'error' => $msg,
      'code' => 'email_taken',
    ));
  }

  try {
    $new = new stdClass();
    $new->is_new = TRUE;
    $new->name = $username;
    $new->mail = $email;
    $new->init = $email;
    $new->status = 1;
    $new->timezone = date_default_timezone_get();
    $new->language = $locale['mapped_language'];

    $edit = array(
      // Plaintext — user_save hashes via password.inc.
      'pass' => $password,
      'field_country' => array(LANGUAGE_NONE => array(0 => array('iso2' => $locale['iso2']))),
      'data' => array(
        ROVAS_OAUTH_SIGNUP_SOURCE_KEY => ROVAS_OAUTH_SIGNUP_SOURCE_CIVIC,
      ),
    );

    $account = user_save($new, $edit);
    if (!$account || empty($account->uid)) {
      return services_error(t('Could not create user account.'), 500);
    }

    // Re-stamp signup source if user_save dropped serialized data.
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

    $mint = rovas_oauth_mint_civic_tokens((int) $account->uid);
    if ($mint === FALSE) {
      return services_error(t('Unable to mint OAuth tokens.'), 500);
    }
    return $mint;
  }
  catch (Exception $e) {
    watchdog('rovas_oauth', 'civic_auth_register: @msg', array('@msg' => $e->getMessage()), WATCHDOG_ERROR);
    return services_error(t('Unable to complete Civic auth register.'), 500);
  }
}

/**
 * Resolve login string to a Drupal account (username or email).
 *
 * @param string $login
 *
 * @return object|null
 *   Loaded active account, or NULL.
 */
function rovas_oauth_civic_auth_login_load_account($login) {
  $login = trim((string) $login);
  if ($login === '') {
    return NULL;
  }

  $account = user_load_by_name($login);
  if (!$account || empty($account->uid)) {
    if (valid_email_address($login)) {
      $account = user_load_by_mail($login);
    }
  }

  if (!$account || empty($account->uid) || empty($account->status)) {
    return NULL;
  }
  return $account;
}

/**
 * Services callback: POST /rovas/civic_auth_login.
 *
 * @param array $data
 *   Request body.
 *
 * @return array|object
 *   Mint JSON or services_error().
 */
function rovas_oauth_civic_auth_login_create($data) {
  module_load_include('inc', 'rovas_oauth', 'rovas_oauth.auth_session');

  if (!is_array($data)) {
    return services_error(t('Request body must be a JSON object.'), 400);
  }

  if (!isset($data['login']) || (!is_string($data['login']) && !is_numeric($data['login']))) {
    return services_error(t('Missing required field: @field.', array('@field' => 'login')), 400);
  }
  $login = trim((string) $data['login']);
  if ($login === '') {
    return services_error(t('Missing required field: @field.', array('@field' => 'login')), 400);
  }

  if (!array_key_exists('password', $data) || !is_string($data['password']) || $data['password'] === '') {
    return services_error(t('Missing required field: @field.', array('@field' => 'password')), 400);
  }
  $password = $data['password'];
  if (drupal_strlen($password) > ROVAS_OAUTH_CIVIC_PASSWORD_MAX_LENGTH) {
    return services_error(t('Invalid login or password.'), 401);
  }

  // Generic failure — do not reveal whether the account exists.
  $fail = function () {
    return services_error(t('Invalid login or password.'), 401);
  };

  try {
    $account = rovas_oauth_civic_auth_login_load_account($login);
    if (!$account) {
      return $fail();
    }

    require_once DRUPAL_ROOT . '/' . variable_get('password_inc', 'includes/password.inc');
    if (!user_check_password($password, $account)) {
      return $fail();
    }

    $mint = rovas_oauth_mint_civic_tokens((int) $account->uid);
    if ($mint === FALSE) {
      return services_error(t('Unable to mint OAuth tokens.'), 500);
    }
    return $mint;
  }
  catch (Exception $e) {
    watchdog('rovas_oauth', 'civic_auth_login: @msg', array('@msg' => $e->getMessage()), WATCHDOG_ERROR);
    return services_error(t('Unable to complete Civic auth login.'), 500);
  }
}
