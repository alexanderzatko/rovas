<?php

/**
 * @file
 * Civic volunteer work-report reward conversion (CHR→ME on verification).
 *
 * See docs/integration/civic.md → CHR to ME on WR verification /
 * Next work: Civic WR usage fee (app AUR).
 */

/**
 * Persist WR reward preference (chrons vs merits) for post-verification conversion.
 *
 * Used by Civic OAuth create and classic rules_proxy_create_work_report.
 * Conversion runs in rovas_civic_maybe_convert_wr_chron_to_merits() after approve.
 *
 * @param int $wr_nid
 *   Work report node id.
 * @param int $uid
 *   Author uid.
 * @param string $reward_kind
 *   'merits' or 'chrons' (invalid/empty → 'chrons').
 * @param int $usage_fee_project_id
 *   Optional app AUR project nid (Civic only; classic leaves 0).
 * @param float $usage_fee_rate
 *   Optional AUR rate in (0, 1] (Civic only).
 *
 * @return bool
 *   TRUE when row written; FALSE if skipped.
 */
function rovas_civic_persist_wr_reward_preference($wr_nid, $uid, $reward_kind, $usage_fee_project_id = 0, $usage_fee_rate = 0.0) {
  $wr_nid = (int) $wr_nid;
  $uid = (int) $uid;
  if ($wr_nid < 1 || $uid < 1 || !db_table_exists('civic_work_report_reward')) {
    return FALSE;
  }

  $kind = drupal_strtolower(trim((string) $reward_kind));
  if ($kind !== 'merits' && $kind !== 'chrons') {
    $kind = 'chrons';
  }

  $fields = array(
    'reward_kind' => $kind,
    'uid' => $uid,
    'created' => REQUEST_TIME,
  );
  if (db_field_exists('civic_work_report_reward', 'usage_fee_project_id')) {
    $fields['usage_fee_project_id'] = (int) $usage_fee_project_id;
    $fields['usage_fee_rate'] = (float) $usage_fee_rate;
  }

  db_merge('civic_work_report_reward')
    ->key(array('wr_nid' => $wr_nid))
    ->fields($fields)
    ->execute();

  return TRUE;
}

/**
 * Charge a deferred app usage fee (AUR) for a Civic-filed work report.
 *
 * Uses project id + rate stored on {civic_work_report_reward} at create time
 * (supplied by the app; Rovas does not hardcode Civic's project). Runs after
 * rules_verify_work_report has credited Chrons and taken the Rovas 1% fee.
 *
 * @param object $work_report
 *   work_report node (approved: field_verification_status = 1).
 *
 * @return bool
 *   TRUE when fee charged, already charged, or not applicable; FALSE on error.
 */
function rovas_civic_maybe_charge_wr_usage_fee($work_report) {
  if (empty($work_report->nid) || $work_report->type !== 'work_report') {
    return FALSE;
  }
  $wr_nid = (int) $work_report->nid;

  if (!rovas_civic_wr_is_fully_verified($work_report)) {
    return FALSE;
  }
  if (!db_table_exists('civic_work_report_reward')
    || !db_field_exists('civic_work_report_reward', 'usage_fee_project_id')) {
    return FALSE;
  }

  $row = db_query(
    'SELECT usage_fee_project_id, usage_fee_rate, usage_fee_charged FROM {civic_work_report_reward} WHERE wr_nid = :nid',
    array(':nid' => $wr_nid)
  )->fetchObject();
  if (!$row) {
    return FALSE;
  }
  $project_nid = (int) $row->usage_fee_project_id;
  $rate = (float) $row->usage_fee_rate;
  if ($project_nid < 1 || $rate <= 0) {
    return TRUE;
  }
  if (!empty($row->usage_fee_charged)) {
    return TRUE;
  }

  $gross = rovas_civic_wr_chron_grant_amount($work_report);
  if ($gross <= 0) {
    watchdog('rovas_civic', 'App usage fee: no Chron grant amount for work report @nid.', array(
      '@nid' => $wr_nid,
    ), WATCHDOG_WARNING);
    return FALSE;
  }

  $fee = round($gross * $rate, 2);
  if ($fee <= 0) {
    return TRUE;
  }

  $project = rovas_civic_load_published_project($project_nid);
  if (!$project) {
    watchdog('rovas_civic', 'App usage fee: project @pid missing for work report @nid.', array(
      '@pid' => $project_nid,
      '@nid' => $wr_nid,
    ), WATCHDOG_ERROR);
    return FALSE;
  }

  $author = user_load($work_report->uid);
  if (!$author) {
    return FALSE;
  }

  if (!function_exists('rules_invoke_component') || !rules_config_load('rules_create_aur')) {
    watchdog('rovas_civic', 'App usage fee: rules_create_aur unavailable for work report @nid.', array(
      '@nid' => $wr_nid,
    ), WATCHDOG_ERROR);
    return FALSE;
  }

  $note = 'App usage fee of ' . $fee . ' CH';
  rules_invoke_component('rules_create_aur', $project, $fee, $author, $note, $work_report);

  db_update('civic_work_report_reward')
    ->fields(array('usage_fee_charged' => REQUEST_TIME))
    ->condition('wr_nid', $wr_nid)
    ->condition('usage_fee_charged', 0)
    ->execute();

  watchdog('rovas_civic', 'App usage fee: charged @fee CH to project @pid for uid @uid on work report @nid.', array(
    '@fee' => $fee,
    '@pid' => $project_nid,
    '@uid' => (int) $author->uid,
    '@nid' => $wr_nid,
  ), WATCHDOG_INFO);

  return TRUE;
}

/**
 * Convert a verified Civic work-report Chron grant to Merits (NeoFund A6).
 *
 * Called after rules_verify_work_report credits Chrons when
 * {civic_work_report_reward}.reward_kind = merits. Uses the existing forex
 * A6 path (_rovas_forex_grant_merits): 10 CHR ↔ 10 ME, debiting CHR only.
 *
 * @param object $work_report
 *   work_report node (approved: field_verification_status = 1).
 *
 * @return bool
 *   TRUE when conversion ran or was already done; FALSE when not applicable.
 */
function rovas_civic_maybe_convert_wr_chron_to_merits($work_report) {
  if (empty($work_report->nid) || $work_report->type !== 'work_report') {
    return FALSE;
  }
  $wr_nid = (int) $work_report->nid;

  if (!rovas_civic_wr_is_fully_verified($work_report)) {
    return FALSE;
  }
  if (!db_table_exists('civic_work_report_reward')) {
    return FALSE;
  }

  $row = db_query(
    'SELECT reward_kind, converted FROM {civic_work_report_reward} WHERE wr_nid = :nid',
    array(':nid' => $wr_nid)
  )->fetchObject();
  if (!$row || $row->reward_kind !== 'merits') {
    return FALSE;
  }
  if (!empty($row->converted)) {
    return TRUE;
  }

  $chrons = rovas_civic_wr_chron_grant_amount($work_report);
  if ($chrons <= 0) {
    watchdog('rovas_civic', 'CHR→ME: no Chron grant amount for work report @nid.', array(
      '@nid' => $wr_nid,
    ), WATCHDOG_WARNING);
    return FALSE;
  }

  if (!function_exists('_rovas_forex_grant_merits') || !function_exists('_convert_chr_2_merits')) {
    watchdog('rovas_civic', 'CHR→ME: forex helpers unavailable for work report @nid.', array(
      '@nid' => $wr_nid,
    ), WATCHDOG_ERROR);
    return FALSE;
  }

  $author = user_load($work_report->uid);
  if (!$author) {
    return FALSE;
  }

  $merits = (float) _convert_chr_2_merits($chrons);
  if ($merits <= 0) {
    return FALSE;
  }

  _rovas_forex_grant_merits($author, $merits, -1 * $chrons);

  db_update('civic_work_report_reward')
    ->fields(array('converted' => REQUEST_TIME))
    ->condition('wr_nid', $wr_nid)
    ->condition('converted', 0)
    ->execute();

  watchdog('rovas_civic', 'CHR→ME: converted @c CHR to @m ME for uid @uid on work report @nid.', array(
    '@c' => $chrons,
    '@m' => $merits,
    '@uid' => (int) $author->uid,
    '@nid' => $wr_nid,
  ), WATCHDOG_INFO);

  return TRUE;
}

/**
 * Whether a work report passed verification (both verifiers approved).
 *
 * @param object $work_report
 *
 * @return bool
 */
function rovas_civic_wr_is_fully_verified($work_report) {
  if (!field_info_field('field_verification_status')) {
    return FALSE;
  }
  $item = field_get_items('node', $work_report, 'field_verification_status');
  return !empty($item[0]['value']) && (int) $item[0]['value'] === 1;
}

/**
 * Chron amount granted on verification for a work report.
 *
 * @param object $work_report
 *
 * @return float
 */
function rovas_civic_wr_chron_grant_amount($work_report) {
  if (field_info_field('field_chrons_to_grant')) {
    $item = field_get_items('node', $work_report, 'field_chrons_to_grant');
    if (!empty($item[0]['value']) && (float) $item[0]['value'] > 0) {
      return (float) $item[0]['value'];
    }
  }
  if (field_info_field('field_hours_worked')) {
    $item = field_get_items('node', $work_report, 'field_hours_worked');
    if (!empty($item[0]['value']) && (float) $item[0]['value'] > 0) {
      return (float) $item[0]['value'] * 10;
    }
  }
  return 0.0;
}
