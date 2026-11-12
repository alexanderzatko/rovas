<?php

/**
 * @file
 * Civic volunteer work-report reward conversion (CHR→ME on verification).
 *
 * See docs/integration/civic.md → CHR to ME on WR verification /
 * auto-approve + same-day merge settlement + Merits net of fees.
 */

/**
 * Persist WR reward preference (chrons vs merits) for post-approval conversion.
 *
 * Used by Civic OAuth create and classic rules_proxy_create_work_report.
 * Merits is sticky: once any session stores merits for this WR, later chrons
 * submissions do not downgrade. After write, settles when the WR is already
 * fully verified (covers Trusted auto-approve: create → grant → persist).
 *
 * @param int $wr_nid
 *   Work report node id.
 * @param int $uid
 *   Author uid.
 * @param string $reward_kind
 *   'merits' or 'chrons' (invalid/empty → 'chrons').
 * @param int $usage_fee_project_id
 *   Optional app AUR project nid (0 = none).
 * @param float $usage_fee_rate
 *   Optional AUR rate in (0, 1].
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

  $has_fee_cols = db_field_exists('civic_work_report_reward', 'usage_fee_project_id');
  $existing_sql = $has_fee_cols
    ? 'SELECT reward_kind, usage_fee_project_id, usage_fee_rate, usage_fee_charged FROM {civic_work_report_reward} WHERE wr_nid = :nid'
    : 'SELECT reward_kind FROM {civic_work_report_reward} WHERE wr_nid = :nid';
  $existing = db_query($existing_sql, array(':nid' => $wr_nid))->fetchObject();
  // Merits sticky across same-day merges / repeated persist.
  if ($existing && $existing->reward_kind === 'merits') {
    $kind = 'merits';
  }

  $fee = rovas_civic_normalize_wr_usage_fee($usage_fee_project_id, $usage_fee_rate, $wr_nid);

  $fields = array(
    'reward_kind' => $kind,
    'uid' => $uid,
    'created' => REQUEST_TIME,
  );
  if ($has_fee_cols) {
    // Do not wipe a stored fee with empty params; do not change fee after charged.
    $keep_existing_fee = $existing
      && !empty($existing->usage_fee_project_id)
      && (int) $existing->usage_fee_project_id > 0
      && (float) $existing->usage_fee_rate > 0
      && $fee['project_id'] < 1;
    $already_charged = $existing && !empty($existing->usage_fee_charged);
    if (!$already_charged && !$keep_existing_fee) {
      $fields['usage_fee_project_id'] = $fee['project_id'];
      $fields['usage_fee_rate'] = $fee['rate'];
    }
  }

  db_merge('civic_work_report_reward')
    ->key(array('wr_nid' => $wr_nid))
    ->fields($fields)
    ->execute();

  $work_report = node_load($wr_nid);
  if ($work_report && $work_report->type === 'work_report' && rovas_civic_wr_is_fully_verified($work_report)) {
    rovas_civic_settle_wr_reward($work_report);
  }

  return TRUE;
}

/**
 * Normalize optional app usage-fee params for WR reward persist.
 *
 * Both project id and rate required for a fee; invalid combo → no fee + watchdog.
 *
 * @param int|string $project_id
 * @param float|string $rate
 * @param int $wr_nid
 *   For watchdog context (0 if unknown).
 *
 * @return array
 *   Keys: project_id (int), rate (float).
 */
function rovas_civic_normalize_wr_usage_fee($project_id, $rate, $wr_nid = 0) {
  $project_id = (int) $project_id;
  $rate = (float) $rate;
  if ($project_id < 1 && $rate <= 0) {
    return array('project_id' => 0, 'rate' => 0.0);
  }
  if ($project_id < 1 || $rate <= 0 || $rate > 1) {
    watchdog('rovas_civic', 'WR usage fee params invalid for work report @nid (project=@pid rate=@rate); storing no fee.', array(
      '@nid' => (int) $wr_nid,
      '@pid' => $project_id,
      '@rate' => $rate,
    ), WATCHDOG_WARNING);
    return array('project_id' => 0, 'rate' => 0.0);
  }
  $project = function_exists('rovas_civic_load_published_project')
    ? rovas_civic_load_published_project($project_id)
    : FALSE;
  if (!$project) {
    watchdog('rovas_civic', 'WR usage fee project @pid missing/unpublished for work report @nid; storing no fee.', array(
      '@pid' => $project_id,
      '@nid' => (int) $wr_nid,
    ), WATCHDOG_WARNING);
    return array('project_id' => 0, 'rate' => 0.0);
  }
  return array('project_id' => $project_id, 'rate' => $rate);
}

/**
 * Settle a verified WR: Rovas 1% (auto-approve), app fee, then net CHR→ME.
 *
 * Safe to call repeatedly (idempotent fee + delta conversion).
 *
 * @param object $work_report
 *   work_report node.
 *
 * @return bool
 *   TRUE when settlement ran or was not applicable; FALSE on hard failure paths
 *   mirrored from the underlying helpers.
 */
function rovas_civic_settle_wr_reward($work_report) {
  if (empty($work_report->nid) || empty($work_report->type) || $work_report->type !== 'work_report') {
    return FALSE;
  }
  // Reload so grant fields match DB after Rules / auto-approve / merge sync.
  $fresh = node_load((int) $work_report->nid, TRUE);
  if ($fresh && $fresh->type === 'work_report') {
    $work_report = $fresh;
  }
  rovas_civic_maybe_charge_wr_rovas_fee($work_report);
  rovas_civic_maybe_charge_wr_usage_fee($work_report);
  return rovas_civic_maybe_convert_wr_chron_to_merits($work_report);
}

/**
 * Charge Rovas 1% platform AUR for Trusted auto-approved work reports.
 *
 * Peer verify already charges via rules_verify_work_report — skipped here.
 *
 * @param object $work_report
 *
 * @return bool
 *   TRUE when charged, already charged, peer path, or not applicable.
 */
function rovas_civic_maybe_charge_wr_rovas_fee($work_report) {
  if (empty($work_report->nid) || $work_report->type !== 'work_report') {
    return FALSE;
  }
  $wr_nid = (int) $work_report->nid;

  if (!rovas_civic_wr_is_fully_verified($work_report)) {
    return FALSE;
  }
  if (!rovas_civic_wr_is_auto_approved($work_report)) {
    return TRUE;
  }
  if (!db_table_exists('civic_work_report_reward')
    || !db_field_exists('civic_work_report_reward', 'rovas_fee_charged')) {
    // Pre-7033 schema: cannot track; skip charge to avoid double risk mid-upgrade.
    return TRUE;
  }

  // Ensure a preference row exists so we can mark rovas_fee_charged.
  $row = db_query(
    'SELECT wr_nid, rovas_fee_charged FROM {civic_work_report_reward} WHERE wr_nid = :nid',
    array(':nid' => $wr_nid)
  )->fetchObject();
  if (!$row) {
    db_merge('civic_work_report_reward')
      ->key(array('wr_nid' => $wr_nid))
      ->fields(array(
        'reward_kind' => 'chrons',
        'uid' => (int) $work_report->uid,
        'created' => REQUEST_TIME,
      ))
      ->execute();
    $row = (object) array('rovas_fee_charged' => 0);
  }
  if (!empty($row->rovas_fee_charged)) {
    return TRUE;
  }

  $gross = rovas_civic_wr_chron_grant_amount($work_report);
  if ($gross <= 0) {
    return TRUE;
  }
  $fee = round($gross * 0.01, 2);
  if ($fee <= 0) {
    return TRUE;
  }

  $project_nid = defined('ROVAS_CIVIC_EUR_BUY_MERITS_PROJECT_DEFAULT')
    ? (int) ROVAS_CIVIC_EUR_BUY_MERITS_PROJECT_DEFAULT
    : 1992;
  if (defined('ROVAS_CIVIC_EUR_BUY_MERITS_PROJECT_VAR') && function_exists('variable_get')) {
    $project_nid = (int) variable_get(ROVAS_CIVIC_EUR_BUY_MERITS_PROJECT_VAR, $project_nid);
  }
  $project = function_exists('rovas_civic_load_published_project')
    ? rovas_civic_load_published_project($project_nid)
    : node_load($project_nid);
  if (!$project || (isset($project->status) && (int) $project->status !== 1)) {
    watchdog('rovas_civic', 'Rovas fee: platform project @pid missing for work report @nid.', array(
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
    watchdog('rovas_civic', 'Rovas fee: rules_create_aur unavailable for work report @nid.', array(
      '@nid' => $wr_nid,
    ), WATCHDOG_ERROR);
    return FALSE;
  }

  $note = 'Rovas use fee of ' . $fee . ' CH';
  rules_invoke_component('rules_create_aur', $project, $fee, $author, $note, $work_report);

  db_update('civic_work_report_reward')
    ->fields(array('rovas_fee_charged' => REQUEST_TIME))
    ->condition('wr_nid', $wr_nid)
    ->condition('rovas_fee_charged', 0)
    ->execute();

  watchdog('rovas_civic', 'Rovas fee: charged @fee CH to project @pid for uid @uid on work report @nid.', array(
    '@fee' => $fee,
    '@pid' => $project_nid,
    '@uid' => (int) $author->uid,
    '@nid' => $wr_nid,
  ), WATCHDOG_INFO);

  return TRUE;
}

/**
 * Charge a deferred app usage fee (AUR) for a work report with stored fee params.
 *
 * Uses project id + rate stored on {civic_work_report_reward} at create time
 * (supplied by the app; Rovas does not hardcode the app project).
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

  $update = array('usage_fee_charged' => REQUEST_TIME);
  if (db_field_exists('civic_work_report_reward', 'usage_fee_amount')) {
    $update['usage_fee_amount'] = $fee;
  }
  db_update('civic_work_report_reward')
    ->fields($update)
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
 * Convert verified WR Chron grant (net of fees) to Merits when reward_kind = merits.
 *
 * Converts target_net − converted_chrons so same-day merge top-ups convert too.
 * target_net = grant − Rovas 1% − app fee.
 * Pre-delta rows (converted > 0, converted_chrons = 0) are marked fully
 * converted for the current target without re-granting.
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

  $has_converted_chrons = db_field_exists('civic_work_report_reward', 'converted_chrons');
  $select = 'SELECT reward_kind, converted';
  if ($has_converted_chrons) {
    $select .= ', converted_chrons';
  }
  if (db_field_exists('civic_work_report_reward', 'usage_fee_project_id')) {
    $select .= ', usage_fee_project_id, usage_fee_rate, usage_fee_charged';
  }
  if (db_field_exists('civic_work_report_reward', 'usage_fee_amount')) {
    $select .= ', usage_fee_amount';
  }
  $select .= ' FROM {civic_work_report_reward} WHERE wr_nid = :nid';
  $row = db_query($select, array(':nid' => $wr_nid))->fetchObject();
  if (!$row || $row->reward_kind !== 'merits') {
    return FALSE;
  }

  $grant = rovas_civic_wr_chron_grant_amount($work_report);
  if ($grant <= 0) {
    watchdog('rovas_civic', 'CHR→ME: no Chron grant amount for work report @nid.', array(
      '@nid' => $wr_nid,
    ), WATCHDOG_WARNING);
    return FALSE;
  }

  $fees = rovas_civic_wr_fee_amounts($grant, $row);
  $target_net = $grant - $fees['rovas'] - $fees['app'];
  if ($target_net < 0) {
    $target_net = 0.0;
  }

  $already = 0.0;
  if ($has_converted_chrons && isset($row->converted_chrons)) {
    $already = (float) $row->converted_chrons;
  }

  // Pre-delta schema: converted timestamp set, converted_chrons still 0.
  // Historical rows converted gross; lock converted_chrons to target_net so we
  // do not claw back, and do not double-grant.
  if (!empty($row->converted) && $already <= 0) {
    if ($has_converted_chrons) {
      db_update('civic_work_report_reward')
        ->fields(array('converted_chrons' => $target_net > 0 ? $target_net : $grant))
        ->condition('wr_nid', $wr_nid)
        ->execute();
    }
    return TRUE;
  }

  $delta = $target_net - $already;
  if ($delta <= 0.00001) {
    return TRUE;
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

  $merits = (float) _convert_chr_2_merits($delta);
  if ($merits <= 0) {
    return FALSE;
  }

  _rovas_forex_grant_merits($author, $merits, -1 * $delta);

  $fields = array('converted' => REQUEST_TIME);
  if ($has_converted_chrons) {
    $fields['converted_chrons'] = $target_net;
  }
  db_update('civic_work_report_reward')
    ->fields($fields)
    ->condition('wr_nid', $wr_nid)
    ->execute();

  watchdog('rovas_civic', 'CHR→ME: converted @c CHR to @m ME (net of fees) for uid @uid on work report @nid.', array(
    '@c' => $delta,
    '@m' => $merits,
    '@uid' => (int) $author->uid,
    '@nid' => $wr_nid,
  ), WATCHDOG_INFO);

  return TRUE;
}

/**
 * Rovas 1% + app usage fee Chron amounts for a WR grant.
 *
 * @param float $grant
 * @param object $row
 *   {civic_work_report_reward} row (fee columns optional).
 *
 * @return array
 *   Keys: rovas (float), app (float).
 */
function rovas_civic_wr_fee_amounts($grant, $row) {
  $grant = (float) $grant;
  $rovas = $grant > 0 ? round($grant * 0.01, 2) : 0.0;
  $app = 0.0;
  if (!$row) {
    return array('rovas' => $rovas, 'app' => $app);
  }
  if (!empty($row->usage_fee_amount) && (float) $row->usage_fee_amount > 0) {
    $app = (float) $row->usage_fee_amount;
  }
  elseif (!empty($row->usage_fee_charged)
    && !empty($row->usage_fee_rate)
    && (float) $row->usage_fee_rate > 0) {
    // Charged before usage_fee_amount column: approximate from current grant×rate.
    $app = round($grant * (float) $row->usage_fee_rate, 2);
  }
  elseif (!empty($row->usage_fee_project_id)
    && (int) $row->usage_fee_project_id > 0
    && !empty($row->usage_fee_rate)
    && (float) $row->usage_fee_rate > 0) {
    $app = round($grant * (float) $row->usage_fee_rate, 2);
  }
  return array('rovas' => $rovas, 'app' => $app);
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
 * Whether a work report was Trusted auto-approved (no peer verification nodes).
 *
 * @param object $work_report
 *
 * @return bool
 */
function rovas_civic_wr_is_auto_approved($work_report) {
  if (!field_info_field('field_auto_approved')) {
    return FALSE;
  }
  $item = field_get_items('node', $work_report, 'field_auto_approved');
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
