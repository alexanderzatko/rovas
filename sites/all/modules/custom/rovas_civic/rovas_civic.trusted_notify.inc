<?php
/**
 * @file
 * Civic Bearer dismiss for trusted-user congratulations notify.
 */

/**
 * Services: POST /rovas/civic_dismiss_trusted_notify
 *
 * Empty body OK. Idempotent: already 0 or 2 still returns 200 with current values.
 *
 * @param array|object|null $data
 *   Unused request body (optional).
 *
 * @return array
 *   keys: trusted_user (bool), trusted_notify (int).
 */
function rovas_civic_services_dismiss_trusted_notify($data = NULL) {
  $uid = rovas_civic_request_uid();
  if ($uid < 1) {
    return services_error(t('Authentication required'), 401);
  }

  $account = user_load($uid);
  if (!$account || empty($account->uid)) {
    return services_error(t('User not found'), 404);
  }

  if (!function_exists('rovas_user_dismiss_trusted_notify_for_account')) {
    return services_error(t('Trusted user module unavailable'), 503);
  }

  return rovas_user_dismiss_trusted_notify_for_account($account);
}
