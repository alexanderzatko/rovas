<?php

/**
 * @file
 * Civic Bearer profile update: username + field_full_name (display_name).
 *
 * Services: POST /rovas/civic_update_profile
 */

/**
 * Validate a Civic profile username (charset + length).
 *
 * Matches the sanitizer charset used by
 * rovas_oauth_civic_auth_username_from_seed(): [A-Za-z0-9_.-], length 1–50.
 *
 * @param string $username
 *   Trimmed candidate.
 *
 * @return string|null
 *   Translated error message, or NULL if valid.
 */
function rovas_oauth_civic_profile_username_error($username) {
  if ($username === '') {
    return t('username must be 1–50 characters of letters, digits, underscore, period, or hyphen.');
  }
  if (drupal_strlen($username) > 50) {
    return t('username must be 1–50 characters of letters, digits, underscore, period, or hyphen.');
  }
  if (!preg_match('/^[A-Za-z0-9_.\-]+$/', $username)) {
    return t('username must be 1–50 characters of letters, digits, underscore, period, or hyphen.');
  }
  // Extra Drupal safety (charset above is a subset of allowed names).
  if (user_validate_name($username) !== NULL) {
    return t('username is not a valid Drupal username.');
  }
  return NULL;
}

/**
 * Build display_name the same way as GET /rovas/me.
 *
 * @param object $account
 *   Loaded user.
 *
 * @return string
 */
function rovas_oauth_civic_profile_display_name($account) {
  $display_name = $account->name;
  $items = field_get_items('user', $account, 'field_full_name');
  if (!empty($items[0]['value'])) {
    $display_name = $items[0]['value'];
  }
  return $display_name;
}

/**
 * Apply username and/or display_name updates for a uid.
 *
 * @param int $uid
 * @param array $data
 *   Body keys: username?, display_name? (string|null).
 *
 * @return array|object
 *   Success payload {username, display_name} or services_error result.
 */
function rovas_oauth_civic_profile_apply($uid, array $data) {
  $uid = (int) $uid;
  $has_username = array_key_exists('username', $data);
  $has_display_name = array_key_exists('display_name', $data);

  if (!$has_username && !$has_display_name) {
    return services_error(t('Body must include username and/or display_name.'), 422);
  }

  $account = user_load($uid);
  if (!$account || empty($account->uid)) {
    return services_error(t('User not found'), 404);
  }

  $edit = array();

  if ($has_username) {
    if (!is_string($data['username']) && !is_numeric($data['username'])) {
      return services_error(t('username must be a string.'), 422);
    }
    $username = trim((string) $data['username']);
    $uname_err = rovas_oauth_civic_profile_username_error($username);
    if ($uname_err !== NULL) {
      return services_error($uname_err, 422);
    }
    // No-op when equal to current name (exact); still check uniqueness if casing differs.
    if ($username !== $account->name) {
      $existing = user_load_by_name($username);
      if ($existing && (int) $existing->uid !== $uid) {
        return services_error(t('username is already taken.'), 409);
      }
      $edit['name'] = $username;
    }
  }

  if ($has_display_name) {
    $raw = $data['display_name'];
    if ($raw === NULL) {
      $edit['field_full_name'] = array(LANGUAGE_NONE => array());
    }
    elseif (is_string($raw) || is_numeric($raw)) {
      $trimmed = trim((string) $raw);
      if ($trimmed === '') {
        $edit['field_full_name'] = array(LANGUAGE_NONE => array());
      }
      elseif (drupal_strlen($trimmed) > 200) {
        return services_error(t('display_name must be at most 200 characters.'), 422);
      }
      else {
        $edit['field_full_name'] = array(
          LANGUAGE_NONE => array(0 => array('value' => $trimmed)),
        );
      }
    }
    else {
      return services_error(t('display_name must be a string or null.'), 422);
    }
  }

  if (!empty($edit)) {
    $saved = user_save($account, $edit);
    if (!$saved || empty($saved->uid)) {
      return services_error(t('Could not update profile.'), 500);
    }
    $account = $saved;
  }
  else {
    // Reload in case of concurrent changes; account already loaded.
    $account = user_load($uid);
  }

  return array(
    'username' => $account->name,
    'display_name' => rovas_oauth_civic_profile_display_name($account),
  );
}

/**
 * Services: POST /rovas/civic_update_profile
 *
 * @param array|object $data
 *   Body with optional username and/or display_name.
 *
 * @return array
 */
function rovas_oauth_civic_update_profile_create($data) {
  global $user;

  $uid = !empty($user->uid) ? (int) $user->uid : (int) rovas_oauth_uid_from_bearer('user_profile');
  if ($uid < 1) {
    return services_error(t('Authentication required'), 401);
  }
  if (!is_array($data)) {
    $data = (array) $data;
  }

  return rovas_oauth_civic_profile_apply($uid, $data);
}
