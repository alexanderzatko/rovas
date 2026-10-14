<?php

/**
 * @file
 * Chron adjustment create helpers (Merit offsets + WR grant/clawback).
 */

/**
 * Create a chron_adjustment ledger node.
 *
 * Does **not** update field_chrons_balance unless the caller also invokes the
 * Chron mutator (see rovas_chron_apply_wr_delta()).
 *
 * @param int $uid
 *   Affected user (node author — Chron tab contextual filter).
 * @param float $amount
 *   Absolute Chron magnitude (direction via $context['credit']).
 * @param string $source
 *   field_chron_source machine name.
 * @param array $context
 *   Optional keys:
 *   - credit (bool) — TRUE = Chron-tab credit (default TRUE)
 *   - receipt_nid (int) — parent purchase_receipt
 *   - wr_nid (int) — parent work_report
 *   - ikey (string) — idempotency key (field_chron_ikey)
 *   - note (string)
 *   - title (string)
 *
 * @param bool|null $was_existing
 *   Set TRUE when an existing row was returned (idempotent hit).
 *
 * @return int
 *   New nid, existing nid on idempotent hit, or 0 on failure.
 */
function rovas_chron_create_adjustment($uid, $amount, $source, array $context = array(), &$was_existing = NULL) {
  $was_existing = FALSE;
  $uid = (int) $uid;
  $amount = abs((float) $amount);
  $source = (string) $source;
  if ($uid < 1 || $amount <= 0 || $source === '') {
    return 0;
  }
  $sources = rovas_chron_source_options();
  if (!isset($sources[$source])) {
    watchdog('rovas_chron', 'Unknown chron source @s', array('@s' => $source), WATCHDOG_WARNING);
    return 0;
  }

  $ikey = !empty($context['ikey']) ? truncate_utf8((string) $context['ikey'], 190, TRUE, FALSE) : '';
  if ($ikey !== '') {
    $existing = rovas_chron_find_by_ikey($ikey);
    if ($existing > 0) {
      $was_existing = TRUE;
      return $existing;
    }
  }

  $receipt_nid = !empty($context['receipt_nid']) ? (int) $context['receipt_nid'] : 0;
  if ($receipt_nid > 0 && $source === ROVAS_CHRON_SOURCE_MERIT_RECEIPT_OFFSET) {
    $existing = rovas_chron_find_merit_receipt_offset($receipt_nid);
    if ($existing > 0) {
      $was_existing = TRUE;
      return $existing;
    }
  }

  $credit = !isset($context['credit']) ? TRUE : (bool) $context['credit'];
  $signed = $credit ? $amount : -1 * $amount;
  $wr_nid = !empty($context['wr_nid']) ? (int) $context['wr_nid'] : 0;

  $node = new stdClass();
  $node->type = ROVAS_CHRON_BUNDLE;
  $node->language = LANGUAGE_NONE;
  node_object_prepare($node);
  $node->uid = $uid;
  $node->status = 1;
  $node->title = !empty($context['title'])
    ? truncate_utf8($context['title'], 128, TRUE, TRUE)
    : ('chron-adj-' . $source . '-' . REQUEST_TIME);

  if (field_info_instance('node', 'field_amount', ROVAS_CHRON_BUNDLE)) {
    $node->field_amount[LANGUAGE_NONE][0]['value'] = $signed;
  }
  if (field_info_instance('node', 'field_chron_source', ROVAS_CHRON_BUNDLE)) {
    $node->field_chron_source[LANGUAGE_NONE][0]['value'] = $source;
  }
  if ($receipt_nid > 0 && field_info_instance('node', 'field_parent_purchase_receipt', ROVAS_CHRON_BUNDLE)) {
    $node->field_parent_purchase_receipt[LANGUAGE_NONE][0]['target_id'] = $receipt_nid;
  }
  if ($wr_nid > 0 && field_info_instance('node', 'field_parent_work_report', ROVAS_CHRON_BUNDLE)) {
    $node->field_parent_work_report[LANGUAGE_NONE][0]['target_id'] = $wr_nid;
  }
  if ($ikey !== '' && field_info_instance('node', 'field_chron_ikey', ROVAS_CHRON_BUNDLE)) {
    $node->field_chron_ikey[LANGUAGE_NONE][0]['value'] = $ikey;
  }
  if (!empty($context['note'])) {
    $node->body[LANGUAGE_NONE][0]['value'] = $context['note'];
    $node->body[LANGUAGE_NONE][0]['format'] = filter_default_format();
  }
  if (function_exists('rovas_base_rules_set_fresh_timestamps')) {
    rovas_base_rules_set_fresh_timestamps($node);
  }
  $node = node_submit($node);
  node_save($node);
  return !empty($node->nid) ? (int) $node->nid : 0;
}

/**
 * Find chron_adjustment by idempotency key.
 *
 * @param string $ikey
 *
 * @return int
 */
function rovas_chron_find_by_ikey($ikey) {
  $ikey = (string) $ikey;
  if ($ikey === '' || !field_info_field('field_chron_ikey')) {
    return 0;
  }
  $nid = db_query("
    SELECT n.nid
    FROM {node} n
    INNER JOIN {field_data_field_chron_ikey} ik
      ON ik.entity_id = n.nid AND ik.entity_type = 'node' AND ik.deleted = 0
    WHERE n.type = :type
      AND n.status = 1
      AND ik.field_chron_ikey_value = :ikey
    ORDER BY n.nid ASC
  ", array(
    ':type' => ROVAS_CHRON_BUNDLE,
    ':ikey' => $ikey,
  ))->fetchField();
  return $nid ? (int) $nid : 0;
}

/**
 * Whether a work_report already has a wr_grant chron_adjustment.
 *
 * Used by the Chron tab to suppress the legacy work_report credit row.
 *
 * @param int $wr_nid
 *
 * @return bool
 */
function rovas_chron_wr_has_grant($wr_nid) {
  $wr_nid = (int) $wr_nid;
  if ($wr_nid < 1 || !field_info_field('field_parent_work_report')) {
    return FALSE;
  }
  $cid = 'rovas_chron_wr_grant:' . $wr_nid;
  $cached = &drupal_static(__FUNCTION__, array());
  if (isset($cached[$wr_nid])) {
    return $cached[$wr_nid];
  }
  $nid = db_query("
    SELECT n.nid
    FROM {node} n
    INNER JOIN {field_data_field_parent_work_report} wr
      ON wr.entity_id = n.nid AND wr.entity_type = 'node' AND wr.deleted = 0
    INNER JOIN {field_data_field_chron_source} src
      ON src.entity_id = n.nid AND src.entity_type = 'node' AND src.deleted = 0
    WHERE n.type = :type
      AND n.status = 1
      AND wr.field_parent_work_report_target_id = :wr
      AND src.field_chron_source_value = :src
    LIMIT 1
  ", array(
    ':type' => ROVAS_CHRON_BUNDLE,
    ':wr' => $wr_nid,
    ':src' => ROVAS_CHRON_SOURCE_WR_GRANT,
  ))->fetchField();
  $cached[$wr_nid] = (bool) $nid;
  return $cached[$wr_nid];
}

/**
 * Find existing merit_receipt_offset for a purchase_receipt.
 *
 * @param int $receipt_nid
 *
 * @return int
 */
function rovas_chron_find_merit_receipt_offset($receipt_nid) {
  $receipt_nid = (int) $receipt_nid;
  if ($receipt_nid < 1 || !field_info_field('field_parent_purchase_receipt')) {
    return 0;
  }
  $nid = db_query("
    SELECT n.nid
    FROM {node} n
    INNER JOIN {field_data_field_parent_purchase_receipt} pr
      ON pr.entity_id = n.nid AND pr.entity_type = 'node' AND pr.deleted = 0
    LEFT JOIN {field_data_field_chron_source} src
      ON src.entity_id = n.nid AND src.entity_type = 'node' AND src.deleted = 0
    WHERE n.type = :type
      AND n.status = 1
      AND pr.field_parent_purchase_receipt_target_id = :rid
      AND (src.field_chron_source_value = :src OR src.field_chron_source_value IS NULL)
    ORDER BY n.nid ASC
  ", array(
    ':type' => ROVAS_CHRON_BUNDLE,
    ':rid' => $receipt_nid,
    ':src' => ROVAS_CHRON_SOURCE_MERIT_RECEIPT_OFFSET,
  ))->fetchField();
  return $nid ? (int) $nid : 0;
}

/**
 * Ledger-only Chron-tab credit that cancels a Merit-only purchase_receipt debit.
 *
 * @param int $uid
 * @param float $merits_as_amount
 * @param int $receipt_nid
 * @param string $note
 *
 * @return int
 */
function rovas_chron_offset_merit_receipt($uid, $merits_as_amount, $receipt_nid, $note = '') {
  return rovas_chron_create_adjustment($uid, $merits_as_amount, ROVAS_CHRON_SOURCE_MERIT_RECEIPT_OFFSET, array(
    'credit' => TRUE,
    'receipt_nid' => (int) $receipt_nid,
    'ikey' => 'merit_offset:' . (int) $receipt_nid,
    'note' => $note !== '' ? $note : t('Chron-tab offset for Merit purchase receipt #@nid (no Chron wallet change).', array(
      '@nid' => (int) $receipt_nid,
    )),
    'title' => 'merit-offset-' . (int) $receipt_nid,
  ));
}

/**
 * Apply a WR Chron delta: optional chron_adjustment + wallet mutator.
 *
 * Idempotent on $ikey: if the adjustment already exists, the wallet mutator is
 * NOT called again.
 *
 * @param int $uid
 *   Wallet / Chron-tab user.
 * @param float $signed_amount
 *   Positive = grant (credit); negative = clawback (debit).
 * @param string $source
 *   ROVAS_CHRON_SOURCE_WR_GRANT or ROVAS_CHRON_SOURCE_WR_CLAWBACK.
 * @param int $wr_nid
 *   Work report nid.
 * @param string $ikey
 *   Unique idempotency key for this event.
 * @param array $options
 *   - note (string)
 *   - ledger_only_if_grant (bool) — for delete-WR clawback: skip ledger row when
 *     no wr_grant exists (legacy WRs: Chron tab already loses the WR credit).
 *
 * @return int
 *   Adjustment nid, or 0 when ledger skipped / failed (mutator may still run).
 */
function rovas_chron_apply_wr_delta($uid, $signed_amount, $source, $wr_nid, $ikey, array $options = array()) {
  $uid = (int) $uid;
  $signed_amount = (float) $signed_amount;
  $wr_nid = (int) $wr_nid;
  $ikey = (string) $ikey;
  if ($uid < 1 || $signed_amount == 0.0 || $wr_nid < 1 || $ikey === '') {
    return 0;
  }
  if ($source !== ROVAS_CHRON_SOURCE_WR_GRANT && $source !== ROVAS_CHRON_SOURCE_WR_CLAWBACK) {
    watchdog('rovas_chron', 'WR delta: bad source @s', array('@s' => $source), WATCHDOG_WARNING);
    return 0;
  }

  $existing = rovas_chron_find_by_ikey($ikey);
  if ($existing > 0) {
    return $existing;
  }

  $write_ledger = TRUE;
  if ($source === ROVAS_CHRON_SOURCE_WR_CLAWBACK
    && !empty($options['ledger_only_if_grant'])
    && !rovas_chron_wr_has_grant($wr_nid)) {
    $write_ledger = FALSE;
  }

  $adj_nid = 0;
  if ($write_ledger) {
    $was_existing = FALSE;
    $credit = ($signed_amount > 0);
    $adj_nid = rovas_chron_create_adjustment($uid, abs($signed_amount), $source, array(
      'credit' => $credit,
      'wr_nid' => $wr_nid,
      'ikey' => $ikey,
      'note' => !empty($options['note']) ? $options['note'] : '',
      'title' => $source . '-' . $wr_nid . '-' . substr(md5($ikey), 0, 8),
    ), $was_existing);
    if ($was_existing) {
      return $adj_nid;
    }
    if ($adj_nid < 1) {
      watchdog('rovas_chron', 'WR delta: failed to create adjustment ikey @k', array(
        '@k' => $ikey,
      ), WATCHDOG_ERROR);
      return 0;
    }
  }

  if (!function_exists('rules_invoke_component')) {
    watchdog('rovas_chron', 'WR delta: rules_invoke_component missing', array(), WATCHDOG_ERROR);
    return $adj_nid;
  }
  $account = user_load($uid);
  if (!$account) {
    return $adj_nid;
  }
  rules_invoke_component('rules_update_user_chron_account', $account, $signed_amount, NULL);
  if ($source === ROVAS_CHRON_SOURCE_WR_GRANT && $adj_nid > 0) {
    $cached = &drupal_static('rovas_chron_wr_has_grant', array());
    $cached[$wr_nid] = TRUE;
  }
  return $adj_nid;
}
