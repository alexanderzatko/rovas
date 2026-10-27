<?php
/**
 * @file
 * Voluntary Chron → Merit conversion for Civic (Get Merits → Give Chrons).
 *
 * NeoFund A6 forex path: 1 Chron = 1 Merit, merit source volunteer (CHR purchase).
 */

/**
 * Normalize and validate a Chron amount for give-chrons.
 *
 * @param mixed $raw
 *   Request body amount.
 *
 * @return float|false
 *   Amount rounded to 2 decimal places, or FALSE when invalid.
 */
function rovas_civic_give_chrons_normalize_amount($raw) {
  if ($raw === NULL || $raw === '' || is_bool($raw) || is_array($raw) || is_object($raw)) {
    return FALSE;
  }
  if (!is_numeric($raw)) {
    return FALSE;
  }
  $amount = (float) $raw;
  if (is_nan($amount) || $amount === INF || $amount === -INF || $amount <= 0) {
    return FALSE;
  }
  // Reject more than 2 decimal places (cent precision).
  $cents = round($amount * 100);
  if (abs(($amount * 100) - $cents) > 0.0001) {
    return FALSE;
  }
  return $cents / 100.0;
}

/**
 * Read Chron wallet balance for a user account.
 *
 * @param object $account
 *
 * @return float
 */
function rovas_civic_give_chrons_balance($account) {
  $items = field_get_items('user', $account, 'field_chrons_balance');
  if (!empty($items[0]['value']) && is_numeric($items[0]['value'])) {
    return (float) $items[0]['value'];
  }
  return 0.0;
}

/**
 * Read Merit wallet total for a uid.
 *
 * @param int $uid
 *
 * @return float
 */
function rovas_civic_give_chrons_total_me($uid) {
  if (function_exists('_rovas_merit_distribution_user_merits')) {
    $merits = _rovas_merit_distribution_user_merits($uid);
    return $merits !== NULL ? (float) $merits : 0.0;
  }
  if (function_exists('rovas_merit_user_balance')) {
    return (float) rovas_merit_user_balance($uid);
  }
  return 0.0;
}

/**
 * Convert Chrons to Merits (A6) for a signed-in user.
 *
 * @param int $uid
 * @param float $amount
 *   Positive Chron amount (≤ 2 decimal places).
 *
 * @return array|object
 *   Success payload, or services_error() result.
 */
function rovas_civic_give_chrons_apply($uid, $amount) {
  $uid = (int) $uid;
  $amount = (float) $amount;
  if ($uid < 1 || $amount <= 0) {
    return services_error(t('Invalid amount.'), 400);
  }

  if (!function_exists('_rovas_forex_grant_merits') || !function_exists('_convert_chr_2_merits')) {
    return services_error(t('Chron to Merit conversion is temporarily unavailable.'), 503);
  }

  $account = user_load($uid);
  if (!$account || empty($account->uid)) {
    return services_error(t('User not found'), 404);
  }

  $balance = rovas_civic_give_chrons_balance($account);
  // Cent compare avoids float edge cases on the wallet field.
  if ((int) round($amount * 100) > (int) round($balance * 100)) {
    return services_error(t('Insufficient Chrons.'), 422);
  }

  $merits = (float) _convert_chr_2_merits($amount);
  if ($merits <= 0) {
    return services_error(t('Invalid amount.'), 400);
  }

  // Default merit source = volunteer (CHR purchase); do not pass volunteer_work.
  _rovas_forex_grant_merits($account, $merits, -1 * $amount);

  $account = user_load($uid);
  $total_chr = $account ? rovas_civic_give_chrons_balance($account) : 0.0;
  $total_me = rovas_civic_give_chrons_total_me($uid);

  watchdog('rovas_civic', 'Give Chrons: uid @uid converted @c CHR to @m ME.', array(
    '@uid' => $uid,
    '@c' => $amount,
    '@m' => $merits,
  ), WATCHDOG_INFO);

  return array(
    'chrons_given' => $amount,
    'merits_gained' => $merits,
    'total_chr' => $total_chr,
    'total_me' => $total_me,
  );
}

/**
 * Services: POST /rovas/civic_give_chrons
 *
 * @param array|object $data
 *   Body with amount (positive Chron quantity).
 *
 * @return array
 */
function rovas_civic_services_give_chrons($data) {
  $uid = rovas_civic_request_uid();
  if ($uid < 1) {
    return services_error(t('Authentication required'), 401);
  }
  if (!is_array($data)) {
    $data = (array) $data;
  }
  if (!array_key_exists('amount', $data)) {
    return services_error(t('Body must include amount (positive number of Chrons).'), 400);
  }

  $amount = rovas_civic_give_chrons_normalize_amount($data['amount']);
  if ($amount === FALSE) {
    return services_error(t('amount must be a positive number with at most 2 decimal places.'), 422);
  }

  return rovas_civic_give_chrons_apply($uid, $amount);
}
