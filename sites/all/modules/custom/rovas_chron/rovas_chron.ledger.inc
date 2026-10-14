<?php

/**
 * @file
 * Chron adjustment create helpers and Merit-receipt offset API.
 */

/**
 * Create a chron_adjustment ledger node.
 *
 * Does **not** update field_chrons_balance. Callers that also mutate the wallet
 * must invoke rules_update_user_chron_account separately (later phases).
 *
 * @param int $uid
 *   Affected user (node author — Chron tab contextual filter).
 * @param float $amount
 *   Absolute Chron magnitude (always store positive; direction via $credit).
 * @param string $source
 *   field_chron_source machine name.
 * @param array $context
 *   Optional keys:
 *   - credit (bool) — TRUE = Chron-tab credit (default TRUE); FALSE = debit
 *   - receipt_nid (int) — parent purchase_receipt for offsets / audit
 *   - note (string) — body text
 *   - title (string)
 *
 * @return int
 *   New nid, existing nid on idempotent hit, or 0 on failure.
 */
function rovas_chron_create_adjustment($uid, $amount, $source, array $context = array()) {
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

  $receipt_nid = !empty($context['receipt_nid']) ? (int) $context['receipt_nid'] : 0;
  if ($receipt_nid > 0 && $source === ROVAS_CHRON_SOURCE_MERIT_RECEIPT_OFFSET) {
    $existing = rovas_chron_find_merit_receipt_offset($receipt_nid);
    if ($existing > 0) {
      return $existing;
    }
  }

  // Direction: money_transfers treats positive field_amount + credit class as +CH.
  // For debit lines store positive amount and rely on PHP debit class (Phase 1+).
  // Merit offsets are always credits; optional credit flag kept for future.
  $credit = !isset($context['credit']) ? TRUE : (bool) $context['credit'];
  $signed = $credit ? $amount : -1 * $amount;

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
    // Store signed delta: positive = credit on Chron tab.
    $node->field_amount[LANGUAGE_NONE][0]['value'] = $signed;
  }
  if (field_info_instance('node', 'field_chron_source', ROVAS_CHRON_BUNDLE)) {
    $node->field_chron_source[LANGUAGE_NONE][0]['value'] = $source;
  }
  if ($receipt_nid > 0 && field_info_instance('node', 'field_parent_purchase_receipt', ROVAS_CHRON_BUNDLE)) {
    $node->field_parent_purchase_receipt[LANGUAGE_NONE][0]['target_id'] = $receipt_nid;
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
 * Find existing merit_receipt_offset for a purchase_receipt.
 *
 * @param int $receipt_nid
 *
 * @return int
 *   Adjustment nid or 0.
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
 * Never touches field_chrons_balance.
 *
 * @param int $uid
 * @param float $merits_as_amount
 *   Same magnitude as purchase_receipt.field_amount_paid (Merit units).
 * @param int $receipt_nid
 * @param string $note
 *
 * @return int
 *   Adjustment nid or 0.
 */
function rovas_chron_offset_merit_receipt($uid, $merits_as_amount, $receipt_nid, $note = '') {
  return rovas_chron_create_adjustment($uid, $merits_as_amount, ROVAS_CHRON_SOURCE_MERIT_RECEIPT_OFFSET, array(
    'credit' => TRUE,
    'receipt_nid' => (int) $receipt_nid,
    'note' => $note !== '' ? $note : t('Chron-tab offset for Merit purchase receipt #@nid (no Chron wallet change).', array(
      '@nid' => (int) $receipt_nid,
    )),
    'title' => 'merit-offset-' . (int) $receipt_nid,
  ));
}
