<?php

/**
 * @file
 * Receipt scanning-history retention preference + 90-day scrub.
 *
 * See docs/integration/civic_tasks/done/receipt-history-retention.md.
 */

/**
 * Read retention preference for a user.
 *
 * @param int $uid
 *
 * @return string|null
 *   'keep', 'policy', or NULL when unanswered.
 */
function rovas_civic_receipt_history_retention_get($uid) {
  $uid = (int) $uid;
  if ($uid < 1 || !db_table_exists('civic_user_prefs')) {
    return NULL;
  }
  $value = db_query('SELECT receipt_history_retention FROM {civic_user_prefs} WHERE uid = :u', array(
    ':u' => $uid,
  ))->fetchField();
  if ($value === FALSE || $value === NULL || $value === '') {
    return NULL;
  }
  $value = (string) $value;
  if ($value === ROVAS_CIVIC_RECEIPT_HISTORY_KEEP || $value === ROVAS_CIVIC_RECEIPT_HISTORY_POLICY) {
    return $value;
  }
  return NULL;
}

/**
 * Persist retention preference (does not scrub).
 *
 * @param int $uid
 * @param string $retention
 *   'keep' or 'policy'.
 *
 * @return bool
 */
function rovas_civic_receipt_history_retention_store($uid, $retention) {
  $uid = (int) $uid;
  if ($uid < 1) {
    return FALSE;
  }
  if ($retention !== ROVAS_CIVIC_RECEIPT_HISTORY_KEEP
    && $retention !== ROVAS_CIVIC_RECEIPT_HISTORY_POLICY) {
    return FALSE;
  }
  if (!db_table_exists('civic_user_prefs')) {
    return FALSE;
  }
  db_merge('civic_user_prefs')
    ->key(array('uid' => $uid))
    ->fields(array(
      'receipt_history_retention' => $retention,
      'updated' => REQUEST_TIME,
    ))
    ->execute();
  return TRUE;
}

/**
 * Anchor timestamp for the 90-day scrub rule.
 *
 * Uses purchase_at when parseable; else claim created unix time.
 *
 * @param object $row
 *   civic_claims row with purchase_at + created.
 *
 * @return int
 */
function rovas_civic_receipt_history_claim_anchor_ts($row) {
  if (!empty($row->purchase_at)) {
    $ts = strtotime((string) $row->purchase_at);
    if ($ts !== FALSE && $ts > 0) {
      return (int) $ts;
    }
  }
  return !empty($row->created) ? (int) $row->created : 0;
}

/**
 * Whether a claim still has scrubbable scanning-history detail.
 *
 * @param object $row
 *
 * @return bool
 */
function rovas_civic_receipt_history_claim_has_detail($row) {
  if (isset($row->merchant_name) && $row->merchant_name !== NULL && $row->merchant_name !== '') {
    return TRUE;
  }
  if (isset($row->purchase_at) && $row->purchase_at !== NULL && $row->purchase_at !== '') {
    return TRUE;
  }
  return FALSE;
}

/**
 * Null merchant_name / purchase_at on this scanner's aged claims.
 *
 * @param int $uid
 *   Scanner Drupal uid.
 * @param int|null $limit
 *   Optional max rows to scrub in this call.
 *
 * @return int
 *   Number of claim rows scrubbed.
 */
function rovas_civic_receipt_history_scrub_user($uid, $limit = NULL) {
  $uid = (int) $uid;
  if ($uid < 1 || !db_table_exists('civic_claims')) {
    return 0;
  }

  $cutoff = REQUEST_TIME - (ROVAS_CIVIC_RECEIPT_HISTORY_RETENTION_DAYS * 86400);
  $query = db_select('civic_claims', 'c')
    ->fields('c', array('id', 'merchant_name', 'purchase_at', 'created'))
    ->condition('scanner_uid', $uid)
    // Prefer oldest created first so cron budget hits aged rows.
    ->orderBy('created', 'ASC')
    ->orderBy('id', 'ASC');
  // Still has scrubbable detail (NULL-safe via OR of non-empty checks in PHP).
  $or = db_or()
    ->isNotNull('merchant_name')
    ->isNotNull('purchase_at');
  $query->condition($or);
  if ($limit !== NULL && (int) $limit > 0) {
    // Over-fetch a little; purchase_at age is applied in PHP.
    $query->range(0, max((int) $limit * 5, 50));
  }

  $ids = array();
  foreach ($query->execute() as $row) {
    if (!rovas_civic_receipt_history_claim_has_detail($row)) {
      continue;
    }
    $anchor = rovas_civic_receipt_history_claim_anchor_ts($row);
    if ($anchor < 1 || $anchor >= $cutoff) {
      continue;
    }
    $ids[] = (int) $row->id;
    if ($limit !== NULL && count($ids) >= (int) $limit) {
      break;
    }
  }

  if (!$ids) {
    return 0;
  }

  db_update('civic_claims')
    ->fields(array(
      'merchant_name' => NULL,
      'purchase_at' => NULL,
    ))
    ->condition('id', $ids, 'IN')
    ->execute();

  return count($ids);
}

/**
 * Set preference; scrub immediately when target is policy.
 *
 * @param int $uid
 * @param string $retention
 *
 * @return array
 *   keys: receipt_history_retention, scrubbed_claims
 */
function rovas_civic_receipt_history_preference_apply($uid, $retention) {
  $scrubbed = 0;
  if ($retention === ROVAS_CIVIC_RECEIPT_HISTORY_POLICY) {
    $scrubbed = rovas_civic_receipt_history_scrub_user($uid);
  }
  rovas_civic_receipt_history_retention_store($uid, $retention);
  return array(
    'receipt_history_retention' => $retention,
    'scrubbed_claims' => $scrubbed,
  );
}

/**
 * Cron: scrub aged detail for users on the 90-day policy.
 *
 * Users still at null (unanswered) and users on "keep" are skipped.
 */
function rovas_civic_receipt_history_cron() {
  if (!db_table_exists('civic_user_prefs') || !db_table_exists('civic_claims')) {
    return;
  }

  $budget = (int) variable_get('rovas_civic_receipt_history_scrub_budget', ROVAS_CIVIC_RECEIPT_HISTORY_CRON_BUDGET);
  if ($budget < 1) {
    return;
  }

  $uids = db_query("SELECT uid FROM {civic_user_prefs} WHERE receipt_history_retention = :p ORDER BY updated ASC, uid ASC", array(
    ':p' => ROVAS_CIVIC_RECEIPT_HISTORY_POLICY,
  ))->fetchCol();
  if (!$uids) {
    return;
  }

  $remaining = $budget;
  foreach ($uids as $uid) {
    if ($remaining < 1) {
      break;
    }
    $n = rovas_civic_receipt_history_scrub_user((int) $uid, $remaining);
    $remaining -= $n;
  }
}

/**
 * Services: POST /rovas/civic_receipt_history_preference
 *
 * @param array|object $data
 *   Body with retention: keep|policy.
 *
 * @return array
 */
function rovas_civic_services_receipt_history_preference($data) {
  $uid = rovas_civic_request_uid();
  if ($uid < 1) {
    return services_error(t('Authentication required'), 401);
  }
  if (!is_array($data)) {
    $data = (array) $data;
  }
  $retention = isset($data['retention']) ? trim((string) $data['retention']) : '';
  if ($retention !== ROVAS_CIVIC_RECEIPT_HISTORY_KEEP
    && $retention !== ROVAS_CIVIC_RECEIPT_HISTORY_POLICY) {
    return services_error(t('Body must include retention: "keep" or "policy".'), 400);
  }
  return rovas_civic_receipt_history_preference_apply($uid, $retention);
}
