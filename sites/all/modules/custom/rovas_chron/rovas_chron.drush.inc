<?php

/**
 * @file
 * Drush commands for Chron ledger offsets.
 */

/**
 * Implements hook_drush_command().
 */
function rovas_chron_drush_command() {
  $items['rovas-chron-backfill-merit-offsets'] = array(
    'callback' => 'drush_rovas_chron_backfill_merit_offsets',
    'description' => dt('Create ledger-only chron_adjustment credits for Merit-only purchase_receipts (VAT tax_payment + Civic Path-1 notes) missing an offset.'),
    'options' => array(
      'uid' => dt('Limit to one buyer uid.'),
      'dry-run' => dt('Report only; do not create nodes.'),
      'limit' => dt('Max receipts to process (default 500).'),
    ),
    'examples' => array(
      'drush rovas-chron-backfill-merit-offsets' => dt('Backfill missing Merit receipt offsets.'),
      'drush rovas-chron-backfill-merit-offsets --uid=42 --dry-run' => dt('Preview for one user.'),
    ),
  );
  $items['rovas-chron-reconcile'] = array(
    'callback' => 'drush_rovas_chron_reconcile',
    'description' => dt('Report drift between field_chrons_balance and money_transfers sumchrons view sum (read-only; does not overwrite balances).'),
    'options' => array(
      'uid' => dt('Limit to one uid (recommended).'),
      'min-diff' => dt('Only report when abs(wallet - view) exceeds this (default 0.01).'),
      'limit' => dt('Max users when scanning all (default 50).'),
    ),
    'examples' => array(
      'drush rovas-chron-reconcile --uid=42' => dt('Compare one user.'),
      'drush rovas-chron-reconcile --limit=20' => dt('Sample active users.'),
    ),
  );
  return $items;
}

/**
 * Drush: backfill merit_receipt_offset adjustments.
 */
function drush_rovas_chron_backfill_merit_offsets() {
  module_load_include('inc', 'rovas_chron', 'rovas_chron.ledger');
  module_load_include('install', 'rovas_chron');
  _rovas_chron_ensure_bundle_and_fields();

  $only_uid = drush_get_option('uid', NULL);
  $dry_run = (bool) drush_get_option('dry-run', FALSE);
  $limit = (int) drush_get_option('limit', 500);
  if ($limit < 1) {
    $limit = 500;
  }

  $rows = _rovas_chron_merit_receipts_needing_offset($only_uid, $limit);
  $created = 0;
  $skipped = 0;
  foreach ($rows as $row) {
    $receipt_nid = (int) $row->receipt_nid;
    $uid = (int) $row->uid;
    $amount = abs((float) $row->amount_paid);
    if ($receipt_nid < 1 || $uid < 1 || $amount <= 0) {
      $skipped++;
      continue;
    }
    if ($dry_run) {
      drush_log(dt('Would offset receipt @r uid @u amount @a', array(
        '@r' => $receipt_nid,
        '@u' => $uid,
        '@a' => $amount,
      )), 'ok');
      $created++;
      continue;
    }
    $nid = rovas_chron_offset_merit_receipt($uid, $amount, $receipt_nid);
    if ($nid > 0) {
      $created++;
    }
    else {
      $skipped++;
    }
  }
  drush_log(dt('Merit receipt offsets: @n processed/created, @s skipped (dry-run=@d).', array(
    '@n' => $created,
    '@s' => $skipped,
    '@d' => $dry_run ? 'yes' : 'no',
  )), 'success');
}

/**
 * Merit-only receipts that still need a chron_adjustment offset.
 *
 * Includes:
 * - Parent of merits_disbursement with merit source tax_payment (VAT claims)
 * - purchase_receipt notes matching Civic Path-1 donor Merit text
 *
 * Excludes receipts that already have a merit_receipt_offset adjustment.
 *
 * @param int|null $only_uid
 * @param int $limit
 *
 * @return object[]
 */
function _rovas_chron_merit_receipts_needing_offset($only_uid = NULL, $limit = 500) {
  $args = array();
  $uid_sql = '';
  if ($only_uid !== NULL && $only_uid !== '') {
    $uid_sql = ' AND buyer.field_buyer_target_id = :uid ';
    $args[':uid'] = (int) $only_uid;
  }

  // tax_payment ME disbursements → parent receipt.
  $q1 = "
    SELECT DISTINCT
      pr.field_parent_purchase_receipt_target_id AS receipt_nid,
      buyer.field_buyer_target_id AS uid,
      amt.field_amount_paid_value AS amount_paid
    FROM {field_data_field_parent_purchase_receipt} pr
    INNER JOIN {node} dn ON dn.nid = pr.entity_id AND dn.type = 'merits_disbursement' AND dn.status = 1
    INNER JOIN {field_data_field_merit_source} src
      ON src.entity_id = dn.nid AND src.entity_type = 'node' AND src.deleted = 0
      AND src.field_merit_source_value = 'tax_payment'
    INNER JOIN {node} rn ON rn.nid = pr.field_parent_purchase_receipt_target_id AND rn.type = 'purchase_receipt'
    INNER JOIN {field_data_field_buyer} buyer
      ON buyer.entity_id = rn.nid AND buyer.entity_type = 'node' AND buyer.deleted = 0
    INNER JOIN {field_data_field_amount_paid} amt
      ON amt.entity_id = rn.nid AND amt.entity_type = 'node' AND amt.deleted = 0
    WHERE pr.entity_type = 'node' AND pr.deleted = 0
      AND amt.field_amount_paid_value > 0
      $uid_sql
      AND NOT EXISTS (
        SELECT 1 FROM {node} adj
        INNER JOIN {field_data_field_parent_purchase_receipt} apr
          ON apr.entity_id = adj.nid AND apr.entity_type = 'node' AND apr.deleted = 0
        LEFT JOIN {field_data_field_chron_source} csrc
          ON csrc.entity_id = adj.nid AND csrc.entity_type = 'node' AND csrc.deleted = 0
        WHERE adj.type = 'chron_adjustment' AND adj.status = 1
          AND apr.field_parent_purchase_receipt_target_id = pr.field_parent_purchase_receipt_target_id
          AND (csrc.field_chron_source_value = 'merit_receipt_offset' OR csrc.field_chron_source_value IS NULL)
      )
  ";

  // Path-1 Civic donor notes on the receipt itself.
  $q2 = "
    SELECT DISTINCT
      rn.nid AS receipt_nid,
      buyer.field_buyer_target_id AS uid,
      amt.field_amount_paid_value AS amount_paid
    FROM {node} rn
    INNER JOIN {field_data_field_buyer} buyer
      ON buyer.entity_id = rn.nid AND buyer.entity_type = 'node' AND buyer.deleted = 0
    INNER JOIN {field_data_field_amount_paid} amt
      ON amt.entity_id = rn.nid AND amt.entity_type = 'node' AND amt.deleted = 0
    INNER JOIN {field_data_field_note} note
      ON note.entity_id = rn.nid AND note.entity_type = 'node' AND note.deleted = 0
    WHERE rn.type = 'purchase_receipt' AND rn.status = 1
      AND amt.field_amount_paid_value > 0
      AND (
        note.field_note_value LIKE 'Civic VAT claim%'
        OR note.field_note_value LIKE 'Donor Merit for project reward%'
      )
      $uid_sql
      AND NOT EXISTS (
        SELECT 1 FROM {node} adj
        INNER JOIN {field_data_field_parent_purchase_receipt} apr
          ON apr.entity_id = adj.nid AND apr.entity_type = 'node' AND apr.deleted = 0
        LEFT JOIN {field_data_field_chron_source} csrc
          ON csrc.entity_id = adj.nid AND csrc.entity_type = 'node' AND csrc.deleted = 0
        WHERE adj.type = 'chron_adjustment' AND adj.status = 1
          AND apr.field_parent_purchase_receipt_target_id = rn.nid
          AND (csrc.field_chron_source_value = 'merit_receipt_offset' OR csrc.field_chron_source_value IS NULL)
      )
  ";

  $seen = array();
  $out = array();
  foreach (array($q1, $q2) as $sql) {
    $result = db_query_range($sql, 0, $limit, $args);
    foreach ($result as $row) {
      $rid = (int) $row->receipt_nid;
      if (isset($seen[$rid])) {
        continue;
      }
      $seen[$rid] = TRUE;
      $out[] = $row;
      if (count($out) >= $limit) {
        return $out;
      }
    }
  }
  return $out;
}

/**
 * Drush: report wallet vs Chron-tab (sumchrons) drift. Never overwrites.
 */
function drush_rovas_chron_reconcile() {
  $only_uid = drush_get_option('uid', NULL);
  $min_diff = (float) drush_get_option('min-diff', 0.01);
  $limit = (int) drush_get_option('limit', 50);
  if ($limit < 1) {
    $limit = 50;
  }

  if ($only_uid !== NULL && $only_uid !== '') {
    $uids = array((int) $only_uid);
  }
  else {
    $uids = db_query_range('SELECT uid FROM {users} WHERE uid > 0 AND status = 1 ORDER BY access DESC', 0, $limit)->fetchCol();
  }

  $drift_count = 0;
  foreach ($uids as $uid) {
    $uid = (int) $uid;
    $account = user_load($uid);
    if (!$account) {
      continue;
    }
    $wallet = 0.0;
    $items = field_get_items('user', $account, 'field_chrons_balance');
    if (!empty($items[0]['value'])) {
      $wallet = (float) $items[0]['value'];
    }
    $view_sum = _rovas_chron_sumchrons_view_total($uid);
    if ($view_sum === NULL) {
      drush_log(dt('uid @u: wallet=@w view=unparsed', array(
        '@u' => $uid,
        '@w' => $wallet,
      )), 'warning');
      continue;
    }
    $diff = abs($wallet - $view_sum);
    if ($diff > $min_diff) {
      $drift_count++;
      drush_log(dt('uid @u: wallet=@w view=@v diff=@d', array(
        '@u' => $uid,
        '@w' => round($wallet, 2),
        '@v' => round($view_sum, 2),
        '@d' => round($wallet - $view_sum, 2),
      )), 'ok');
    }
  }
  drush_log(dt('Reconcile done: @n user(s) with |diff| > @m (read-only).', array(
    '@n' => $drift_count,
    '@m' => $min_diff,
  )), 'success');
}

/**
 * Parse money_transfers|page_1 embedded sum for a uid (same scrape as old batch adjuster).
 *
 * @param int $uid
 *
 * @return float|null
 */
function _rovas_chron_sumchrons_view_total($uid) {
  $uid = (int) $uid;
  if ($uid < 1 || !function_exists('views_embed_view')) {
    return NULL;
  }
  $vdata = views_embed_view('money_transfers', 'page_1', $uid, $uid, $uid);
  if (!is_string($vdata) || $vdata === '') {
    return NULL;
  }
  if (preg_match('/<div class="label">.+<\/div>(.+)<\/td>/', $vdata, $matches)) {
    return (float) round((float) $matches[1], 2);
  }
  return NULL;
}
