<?php

/**
 * @file
 * Merit ledger helpers: wallet, user_expertise, disbursement nodes.
 */

/** @var float Balance invariant tolerance (ME). */
define('ROVAS_MERIT_BALANCE_TOLERANCE', 0.02);

/** Merit source: verified volunteer work report reward. */
define('ROVAS_MERIT_SOURCE_VOLUNTEER_WORK', 'volunteer_work');

/** Merit source: CHR→ME purchase (forex). */
define('ROVAS_MERIT_SOURCE_VOLUNTEER', 'volunteer');

/** Merit source: VAT / tax claim (Civic). */
define('ROVAS_MERIT_SOURCE_TAX_PAYMENT', 'tax_payment');

/** Merit source: EUR donation, EUR buy-merits, timeshare ME. */
define('ROVAS_MERIT_SOURCE_CHARITABLE_DONATION', 'charitable_donation');

/** Merit source: dispute / compliance penalty. */
define('ROVAS_MERIT_SOURCE_PENALTY', 'penalty');

/** Merit source: legacy / unknown (backfill only). */
define('ROVAS_MERIT_SOURCE_LEGACY', 'legacy');

/**
 * Allowed field_merit_source values.
 *
 * @return array
 *   machine_name => label.
 */
function rovas_merit_source_options() {
  return array(
    ROVAS_MERIT_SOURCE_VOLUNTEER_WORK => t('Volunteer work'),
    ROVAS_MERIT_SOURCE_VOLUNTEER => t('Volunteer (CHR purchase)'),
    ROVAS_MERIT_SOURCE_TAX_PAYMENT => t('Tax payment'),
    ROVAS_MERIT_SOURCE_CHARITABLE_DONATION => t('Charitable donation'),
    ROVAS_MERIT_SOURCE_PENALTY => t('Penalty'),
    ROVAS_MERIT_SOURCE_LEGACY => t('Legacy'),
  );
}

/**
 * Map Civic A3 audit source_type to field_merit_source.
 *
 * @param string $source_type
 *
 * @return string
 */
function rovas_merit_source_from_civic_audit($source_type) {
  switch ($source_type) {
    case 'civic_vat_claim':
      return ROVAS_MERIT_SOURCE_TAX_PAYMENT;

    case 'project_reward_order':
      return ROVAS_MERIT_SOURCE_CHARITABLE_DONATION;

    default:
      return ROVAS_MERIT_SOURCE_LEGACY;
  }
}

/**
 * Map field_merit_source to Civic by_category bucket.
 *
 * @param string $source
 *
 * @return string|null
 *   taxes|charity|volunteer or NULL when not in by_category (e.g. penalty).
 */
function rovas_merit_source_to_category($source) {
  switch ($source) {
    case ROVAS_MERIT_SOURCE_TAX_PAYMENT:
      return 'taxes';

    case ROVAS_MERIT_SOURCE_CHARITABLE_DONATION:
      return 'charity';

    case ROVAS_MERIT_SOURCE_VOLUNTEER_WORK:
    case ROVAS_MERIT_SOURCE_VOLUNTEER:
      return 'volunteer';

    default:
      return NULL;
  }
}

/**
 * Resolve governance NACE tid for penalties (94.99).
 *
 * @return int
 *   Taxonomy tid or 0 when not found.
 */
function rovas_merit_penalty_nace_tid() {
  $cached = &drupal_static(__FUNCTION__);
  if (isset($cached)) {
    return $cached;
  }

  $override = (int) variable_get('rovas_merit_penalty_nace_tid', 0);
  if ($override > 0) {
    $cached = $override;
    return $cached;
  }

  if (function_exists('rovas_ai_nace_leaf_by_code')) {
    $leaf = rovas_ai_nace_leaf_by_code(ROVAS_MERIT_PENALTY_NACE_CODE);
    if (!empty($leaf['tid'])) {
      $cached = (int) $leaf['tid'];
      return $cached;
    }
  }

  // Fallback: match term name containing the code in vocabulary tags.
  $vids = taxonomy_get_vocabularies();
  foreach ($vids as $vid => $vocab) {
    if ($vocab->machine_name !== 'tags') {
      continue;
    }
    $terms = taxonomy_get_tree($vid);
    foreach ($terms as $term) {
      if (strpos($term->name, '94.99') !== FALSE) {
        $cached = (int) $term->tid;
        return $cached;
      }
    }
  }

  $cached = 0;
  return $cached;
}

/**
 * Total ME for a user (userpoints + decimal part).
 *
 * @param int $uid
 *
 * @return float
 */
function rovas_merit_user_total_me($uid) {
  $uid = (int) $uid;
  if ($uid < 1) {
    return 0.0;
  }
  if (function_exists('_rovas_merit_distribution_user_merits')) {
    return (float) _rovas_merit_distribution_user_merits($uid);
  }
  $points = (float) db_query('SELECT points FROM {userpoints} WHERE uid = :uid', array(
    ':uid' => $uid,
  ))->fetchField();
  $decimal = 0.0;
  if ($uid > 0 && field_info_field('field_merit_decimal_part')) {
    $decimal = (float) db_query('SELECT field_merit_decimal_part_value FROM {field_data_field_merit_decimal_part}
      WHERE entity_type = :et AND entity_id = :uid AND deleted = 0 LIMIT 1', array(
      ':et' => 'user',
      ':uid' => $uid,
    ))->fetchField();
  }
  return $points + $decimal;
}

/**
 * Sum of user_expertise.field_merits for a user.
 *
 * @param int $uid
 *
 * @return float
 */
function rovas_merit_user_expertise_sum($uid) {
  $uid = (int) $uid;
  if ($uid < 1) {
    return 0.0;
  }
  return (float) db_query("
    SELECT COALESCE(SUM(m.field_merits_value), 0)
    FROM {node} n
    INNER JOIN {field_data_field_expert} e
      ON e.entity_id = n.nid AND e.entity_type = 'node' AND e.deleted = 0
    INNER JOIN {field_data_field_merits} m
      ON m.entity_id = n.nid AND m.entity_type = 'node' AND m.deleted = 0
    WHERE n.type = 'user_expertise'
      AND n.status = 1
      AND e.field_expert_target_id = :uid
  ", array(':uid' => $uid))->fetchField();
}

/**
 * Sum of ledger field_reward_amount for a user.
 *
 * @param int $uid
 *
 * @return float
 */
function rovas_merit_user_ledger_sum($uid) {
  $uid = (int) $uid;
  if ($uid < 1 || !field_info_field('field_merit_source')) {
    return 0.0;
  }
  return (float) db_query("
    SELECT COALESCE(SUM(amt.field_reward_amount_value), 0)
    FROM {node} n
    INNER JOIN {field_data_field_reward_recipient} rec
      ON rec.entity_id = n.nid AND rec.entity_type = 'node' AND rec.deleted = 0
    INNER JOIN {field_data_field_reward_amount} amt
      ON amt.entity_id = n.nid AND amt.entity_type = 'node' AND amt.deleted = 0
    WHERE n.status = 1
      AND n.type IN ('merits_disbursement', 'merit_penalty')
      AND rec.field_reward_recipient_target_id = :uid
  ", array(':uid' => $uid))->fetchField();
}

/**
 * Aggregate ME by field_merit_source for a user.
 *
 * @param int $uid
 *
 * @return array
 *   source => float (includes negative penalty totals).
 */
function rovas_merit_user_by_source($uid) {
  $uid = (int) $uid;
  $out = array();
  foreach (array_keys(rovas_merit_source_options()) as $key) {
    $out[$key] = 0.0;
  }
  if ($uid < 1 || !field_info_field('field_merit_source')) {
    return $out;
  }

  $result = db_query("
    SELECT src.field_merit_source_value AS source,
      COALESCE(SUM(amt.field_reward_amount_value), 0) AS me
    FROM {node} n
    INNER JOIN {field_data_field_reward_recipient} rec
      ON rec.entity_id = n.nid AND rec.entity_type = 'node' AND rec.deleted = 0
    INNER JOIN {field_data_field_reward_amount} amt
      ON amt.entity_id = n.nid AND amt.entity_type = 'node' AND amt.deleted = 0
    LEFT JOIN {field_data_field_merit_source} src
      ON src.entity_id = n.nid AND src.entity_type = 'node' AND src.deleted = 0
    WHERE n.status = 1
      AND n.type IN ('merits_disbursement', 'merit_penalty')
      AND rec.field_reward_recipient_target_id = :uid
    GROUP BY src.field_merit_source_value
  ", array(':uid' => $uid));

  foreach ($result as $row) {
    $source = $row->source ? $row->source : ROVAS_MERIT_SOURCE_LEGACY;
    if (!isset($out[$source])) {
      $out[$source] = 0.0;
    }
    $out[$source] += (float) $row->me;
  }
  return $out;
}

/**
 * Derive Civic by_category buckets from ledger sources.
 *
 * @param int $uid
 *
 * @return array
 *   keys taxes, charity, volunteer (non-negative floats).
 */
function rovas_merit_user_by_category($uid) {
  $by_source = rovas_merit_user_by_source($uid);
  $out = array('taxes' => 0.0, 'charity' => 0.0, 'volunteer' => 0.0);
  foreach ($by_source as $source => $me) {
    if ($me <= 0) {
      continue;
    }
    $cat = rovas_merit_source_to_category($source);
    if ($cat) {
      $out[$cat] += $me;
    }
  }
  return $out;
}

/**
 * Check wallet / expertise / ledger invariant for one user.
 *
 * @param int $uid
 *
 * @return array
 *   ok, total_me, expertise_sum, ledger_sum, delta_expertise, delta_ledger.
 */
function rovas_merit_balance_check($uid) {
  $total = rovas_merit_user_total_me($uid);
  $expertise = rovas_merit_user_expertise_sum($uid);
  $ledger = rovas_merit_user_ledger_sum($uid);
  $tol = ROVAS_MERIT_BALANCE_TOLERANCE;
  $delta_e = $total - $expertise;
  $delta_l = $total - $ledger;
  return array(
    'ok' => (abs($delta_e) <= $tol && abs($delta_l) <= $tol),
    'total_me' => $total,
    'expertise_sum' => $expertise,
    'ledger_sum' => $ledger,
    'delta_expertise' => $delta_e,
    'delta_ledger' => $delta_l,
  );
}

/**
 * Watchdog when invariant fails (development aid).
 *
 * @param int $uid
 */
function rovas_merit_assert_balance($uid) {
  $check = rovas_merit_balance_check($uid);
  if (!$check['ok']) {
    watchdog('rovas_merit', 'Merit invariant failed for uid @uid: total=@t expertise=@e ledger=@l', array(
      '@uid' => $uid,
      '@t' => $check['total_me'],
      '@e' => $check['expertise_sum'],
      '@l' => $check['ledger_sum'],
    ), WATCHDOG_WARNING);
  }
}

/**
 * Credit or debit wallet (userpoints + decimal part).
 *
 * Mirrors rules_give_merits_to_user with correct negative decimal handling.
 *
 * @param object $account
 * @param float $delta
 * @param object $reward_entity
 * @param string $description
 * @param string $operation
 *   userpoints operation label.
 *
 * @return bool
 */
function rovas_merit_credit_wallet($account, $delta, $reward_entity, $description, $operation = 'Insert') {
  if (empty($account->uid) || $delta == 0) {
    return FALSE;
  }

  $loaded = user_load($account->uid);
  $points = (float) userpoints_get_current_points($account->uid);
  $decimal = 0.0;
  if ($loaded && field_info_field('field_merit_decimal_part')
    && isset($loaded->field_merit_decimal_part[LANGUAGE_NONE][0]['value'])) {
    $decimal = (float) $loaded->field_merit_decimal_part[LANGUAGE_NONE][0]['value'];
  }

  $new_total = $points + $decimal + (float) $delta;
  $new_points_whole = (int) floor($new_total);
  $new_decimal = $new_total - $new_points_whole;
  $grant_whole = $new_points_whole - (int) floor($points);

  if (field_info_field('field_merit_decimal_part') && $loaded) {
    $wrapper = entity_metadata_wrapper('user', $loaded);
    $wrapper->field_merit_decimal_part->set($new_decimal);
    $wrapper->save();
  }

  if (!function_exists('userpoints_userpointsapi')) {
    return FALSE;
  }

  $entity_type = 'user';
  $entity_id = $account->uid;
  if (is_object($reward_entity)) {
    if (isset($reward_entity->nid)) {
      $entity_type = 'node';
      $entity_id = $reward_entity->nid;
    }
    elseif (method_exists($reward_entity, 'getIdentifier')) {
      list($entity_type, $entity_id) = entity_extract_ids('entity', $reward_entity->value());
    }
  }

  if ($grant_whole != 0) {
    userpoints_userpointsapi(array(
      'uid' => (int) $account->uid,
      'points' => $grant_whole,
      'operation' => $operation,
      'description' => $description,
      'entity_id' => $entity_id,
      'entity_type' => $entity_type,
      'moderate' => FALSE,
    ));
  }

  return TRUE;
}

/**
 * Adjust user_expertise for one NACE category (positive or negative delta).
 *
 * @param int $uid
 * @param float $delta
 * @param int $nace_tid
 * @param float $share
 *   When < 1, apply share * delta (labor mix paths).
 *
 * @return bool
 */
function rovas_merit_adjust_user_expertise($uid, $delta, $nace_tid, $share = 1.0) {
  $uid = (int) $uid;
  $nace_tid = (int) $nace_tid;
  $delta = (float) $delta;
  $share = (float) $share;
  if ($uid < 1 || $nace_tid < 1 || $delta == 0) {
    return FALSE;
  }

  $user = user_load($uid);
  if (!$user) {
    return FALSE;
  }
  $roles = isset($user->roles) ? $user->roles : array();
  if (in_array(5, array_keys($roles))) {
    return FALSE;
  }

  $merits2add = ($share == 1.0) ? $delta : ($share * $delta);

  $record = db_query("SELECT node.nid AS nid, m.field_merits_value AS merits
    FROM {node} node
    LEFT JOIN {field_data_field_expert} x ON node.nid = x.entity_id AND x.entity_type = 'node' AND x.deleted = '0'
    LEFT JOIN {field_data_field_category} c ON node.nid = c.entity_id AND c.entity_type = 'node' AND c.deleted = '0'
    LEFT JOIN {field_data_field_merits} m ON node.nid = m.entity_id AND m.entity_type = 'node' AND m.deleted = '0'
    WHERE node.type = 'user_expertise'
      AND x.field_expert_target_id = :uid
      AND c.field_category_tid = :cat
  ", array(':uid' => $uid, ':cat' => $nace_tid))->fetchAssoc();

  if (empty($record)) {
    $node = new stdClass();
    $node->type = 'user_expertise';
    $node->title = 'UX-' . $uid . '-' . $nace_tid;
    $node->language = LANGUAGE_NONE;
    node_object_prepare($node);
    $node->uid = $uid;
    $node->field_expert[LANGUAGE_NONE][0]['target_id'] = $uid;
    $node->field_category[LANGUAGE_NONE][0]['tid'] = $nace_tid;
    $node->field_merits[LANGUAGE_NONE][0]['value'] = 0;
    $node = node_submit($node);
    node_save($node);
    $nid = $node->nid;
    $old_merits = 0.0;
  }
  else {
    $nid = (int) $record['nid'];
    $old_merits = (float) $record['merits'];
  }

  $cur_merits = $old_merits + $merits2add;
  $wrapper = entity_metadata_wrapper('node', node_load($nid));
  $wrapper->field_merits = $cur_merits;
  $wrapper->save();

  return TRUE;
}

/**
 * Create merits_disbursement or merit_penalty ledger node.
 *
 * @param int $uid
 * @param float $amount
 *   Signed amount (negative for penalty).
 * @param string $source
 * @param int $receipt_id
 * @param string $body
 *
 * @return int
 *   Node nid or 0.
 */
function rovas_merit_create_ledger_node($uid, $amount, $source, $receipt_id = 0, $body = '') {
  $uid = (int) $uid;
  $amount = (float) $amount;
  if ($uid < 1 || $amount == 0) {
    return 0;
  }

  $bundle = ($amount < 0) ? 'merit_penalty' : 'merits_disbursement';
  $node = new stdClass();
  $node->type = $bundle;
  $node->title = $bundle . '-' . REQUEST_TIME;
  $node->language = LANGUAGE_NONE;
  node_object_prepare($node);
  $node->uid = 1;
  $node->field_reward_recipient[LANGUAGE_NONE][0]['target_id'] = $uid;
  $node->field_reward_amount[LANGUAGE_NONE][0]['value'] = $amount;
  if ($receipt_id > 0) {
    $node->field_parent_purchase_receipt[LANGUAGE_NONE][0]['target_id'] = (int) $receipt_id;
  }
  if (field_info_field('field_merit_source')) {
    $node->field_merit_source[LANGUAGE_NONE][0]['value'] = $source;
  }
  if ($body !== '') {
    $node->body[LANGUAGE_NONE][0]['value'] = $body;
    $node->body[LANGUAGE_NONE][0]['format'] = filter_default_format();
  }
  if (function_exists('rovas_base_rules_set_fresh_timestamps')) {
    rovas_base_rules_set_fresh_timestamps($node);
  }
  $node = node_submit($node);
  node_save($node);
  return (int) $node->nid;
}

/**
 * Apply a Merit credit or debit (wallet + expertise + ledger).
 *
 * @param int $uid
 * @param float $delta
 *   Signed ME change.
 * @param string $source
 *   field_merit_source value.
 * @param int $nace_tid
 *   Primary NACE for user_expertise (ignored when nace_splits set).
 * @param array $context
 *   Optional keys:
 *   - description (string)
 *   - reward_entity (object)
 *   - receipt_id (int)
 *   - project_nid (int) — bump field_merit_reward when delta > 0
 *   - body (string) — penalty HTML body
 *   - operation (string) — userpoints operation
 *   - nace_splits (array) — list of array(tid, share) for multi-NACE
 *   - skip_ledger (bool) — when caller creates disbursement via Rules
 *   - skip_wallet (bool)
 *   - skip_expertise (bool)
 *
 * @return bool
 */
function rovas_merit_apply($uid, $delta, $source, $nace_tid, array $context = array()) {
  $uid = (int) $uid;
  $delta = (float) $delta;
  if ($uid < 1 || $delta == 0) {
    return FALSE;
  }

  $account = user_load($uid);
  if (!$account) {
    return FALSE;
  }

  $description = !empty($context['description']) ? $context['description'] : '';
  $reward_entity = !empty($context['reward_entity']) ? $context['reward_entity'] : $account;
  $receipt_id = !empty($context['receipt_id']) ? (int) $context['receipt_id'] : 0;
  $operation = !empty($context['operation']) ? $context['operation'] : 'Insert';
  $body = !empty($context['body']) ? $context['body'] : '';

  if (empty($context['skip_wallet'])) {
    if (!rovas_merit_credit_wallet($account, $delta, $reward_entity, $description, $operation)) {
      return FALSE;
    }
  }

  if (empty($context['skip_expertise'])) {
    if (!empty($context['nace_splits']) && is_array($context['nace_splits'])) {
      foreach ($context['nace_splits'] as $split) {
        $tid = (int) $split['tid'];
        $share = isset($split['share']) ? (float) $split['share'] : 1.0;
        if ($tid > 0 && $share != 0) {
          rovas_merit_adjust_user_expertise($uid, $delta, $tid, $share);
        }
      }
    }
    elseif ($nace_tid > 0) {
      rovas_merit_adjust_user_expertise($uid, $delta, $nace_tid, 1.0);
    }
  }

  if (empty($context['skip_ledger'])) {
    rovas_merit_create_ledger_node($uid, $delta, $source, $receipt_id, $body);
  }

  if ($delta > 0 && !empty($context['project_nid']) && function_exists('project_merit_updater')) {
    project_merit_updater((int) $context['project_nid'], $delta);
  }

  rovas_merit_assert_balance($uid);
  return TRUE;
}

/**
 * Build nace_splits array from labor_merit_rewarder-style category rows.
 *
 * @param array $categories
 *   Rows with ->cat and ->hours or ->percent.
 * @param float $total_hours
 *
 * @return array
 */
function rovas_merit_nace_splits_from_hours(array $categories, $total_hours = 0) {
  $splits = array();
  if ($total_hours <= 0) {
    foreach ($categories as $row) {
      if (!empty($row->hours)) {
        $total_hours += (float) $row->hours;
      }
    }
  }
  if ($total_hours <= 0) {
    return $splits;
  }
  foreach ($categories as $row) {
    $tid = (int) $row->cat;
    $hours = isset($row->hours) ? (float) $row->hours : 0;
    if ($tid > 0 && $hours > 0) {
      $splits[] = array(
        'tid' => $tid,
        'share' => $hours / $total_hours,
      );
    }
  }
  return $splits;
}

/**
 * Batch-load by_category (from ledger source) and NACE sections per uid.
 *
 * @param array $uids
 *
 * @return array
 *   uid => { by_category, by_source, sections }.
 */
function rovas_merit_users_lookup_data_batch(array $uids) {
  $out = array();
  $uids = array_values(array_unique(array_filter(array_map('intval', $uids))));
  if (empty($uids)) {
    return $out;
  }

  foreach ($uids as $uid) {
    $out[$uid] = array(
      'by_category' => rovas_merit_user_by_category($uid),
      'by_source' => rovas_merit_user_by_source($uid),
      'sections' => array(),
    );
  }

  if (!field_info_field('field_merit_source')) {
    return $out;
  }

  $result = db_query("
    SELECT e.field_expert_target_id AS uid,
      c.field_category_tid AS tid,
      COALESCE(SUM(m.field_merits_value), 0) AS me
    FROM {node} n
    INNER JOIN {field_data_field_expert} e
      ON e.entity_id = n.nid AND e.entity_type = 'node' AND e.deleted = 0
    INNER JOIN {field_data_field_category} c
      ON c.entity_id = n.nid AND c.entity_type = 'node' AND c.deleted = 0
    INNER JOIN {field_data_field_merits} m
      ON m.entity_id = n.nid AND m.entity_type = 'node' AND m.deleted = 0
    WHERE n.type = 'user_expertise'
      AND n.status = 1
      AND e.field_expert_target_id IN (:uids)
      AND m.field_merits_value <> 0
    GROUP BY e.field_expert_target_id, c.field_category_tid", array(
    ':uids' => $uids,
  ));

  foreach ($result as $row) {
    $uid = (int) $row->uid;
    $tid = (int) $row->tid;
    $me = (float) $row->me;
    if ($uid < 1 || $tid < 1 || $me == 0 || empty($out[$uid])) {
      continue;
    }
    if (function_exists('rovas_civic_merit_index_nace_section_tid')) {
      $section_tid = rovas_civic_merit_index_nace_section_tid($tid);
      if ($section_tid > 0) {
        $out[$uid]['sections'][$section_tid] = TRUE;
      }
    }
  }

  return $out;
}

/**
 * Reconcile one user: apply expertise correction on penalty NACE if needed.
 *
 * @param int $uid
 * @param bool $apply_fix
 *
 * @return array
 *   Status details.
 */
function rovas_merit_reconcile_user($uid, $apply_fix = FALSE) {
  $check = rovas_merit_balance_check($uid);
  $result = $check;
  $result['fixed'] = FALSE;

  if ($check['ok'] || !$apply_fix) {
    return $result;
  }

  $delta = $check['delta_expertise'];
  if (abs($delta) <= ROVAS_MERIT_BALANCE_TOLERANCE) {
    return $result;
  }

  $penalty_tid = rovas_merit_penalty_nace_tid();
  if ($penalty_tid < 1) {
    $result['error'] = 'penalty_nace_missing';
    return $result;
  }

  // Adjust expertise on governance NACE to absorb wallet vs expertise drift.
  rovas_merit_adjust_user_expertise($uid, -1 * $delta, $penalty_tid, 1.0);
  $result['fixed'] = TRUE;
  $result['fix_delta'] = -1 * $delta;
  $result['after'] = rovas_merit_balance_check($uid);
  return $result;
}

/**
 * Infer merit source from ledger row fields (no node_load).
 *
 * @param object $row
 *   At least: type, created; for disbursements also uid, amount, receipt_nid.
 *
 * @return string
 */
function _rovas_merit_infer_source_from_row($row) {
  if ($row->type === 'merit_penalty') {
    return ROVAS_MERIT_SOURCE_PENALTY;
  }

  if ($row->type !== 'merits_disbursement') {
    return ROVAS_MERIT_SOURCE_LEGACY;
  }

  $uid = !empty($row->uid) ? (int) $row->uid : 0;
  $amount = isset($row->amount) ? (float) $row->amount : 0;
  $created = !empty($row->created) ? (int) $row->created : 0;

  if ($uid > 0 && $created > 0 && db_table_exists('rovas_merit_issuance')) {
    $audit = db_query("SELECT source_type FROM {rovas_merit_issuance}
      WHERE uid = :uid AND amount = :amt AND created BETWEEN :lo AND :hi
      ORDER BY ABS(created - :created) ASC LIMIT 1", array(
      ':uid' => $uid,
      ':amt' => $amount,
      ':lo' => $created - 120,
      ':hi' => $created + 120,
      ':created' => $created,
    ))->fetchField();
    if ($audit) {
      return rovas_merit_source_from_civic_audit($audit);
    }
  }

  $receipt_nid = !empty($row->receipt_nid) ? (int) $row->receipt_nid : 0;
  if ($receipt_nid > 0) {
    $receipt_type = db_query('SELECT type FROM {node} WHERE nid = :nid', array(
      ':nid' => $receipt_nid,
    ))->fetchField();
    if ($receipt_type === 'purchase_merits') {
      return ROVAS_MERIT_SOURCE_VOLUNTEER;
    }
    if ($receipt_type) {
      $has_report = db_query('SELECT 1 FROM {field_data_field_rewarded_report}
        WHERE entity_id = :nid AND entity_type = :et AND deleted = 0 LIMIT 1', array(
        ':nid' => $receipt_nid,
        ':et' => 'node',
      ))->fetchField();
      if ($has_report) {
        return ROVAS_MERIT_SOURCE_VOLUNTEER_WORK;
      }
      $has_project = db_query('SELECT 1 FROM {field_data_field_receipt_project}
        WHERE entity_id = :nid AND entity_type = :et AND deleted = 0 LIMIT 1', array(
        ':nid' => $receipt_nid,
        ':et' => 'node',
      ))->fetchField();
      if ($has_project) {
        return ROVAS_MERIT_SOURCE_CHARITABLE_DONATION;
      }
    }
  }

  return ROVAS_MERIT_SOURCE_LEGACY;
}

/**
 * Infer merit source for a disbursement/penalty node (backfill).
 *
 * @param int $nid
 *
 * @return string
 */
function rovas_merit_infer_source_for_node($nid) {
  $nid = (int) $nid;
  $row = db_query("
    SELECT n.nid, n.type, n.created,
      rec.field_reward_recipient_target_id AS uid,
      amt.field_reward_amount_value AS amount,
      pr.field_parent_purchase_receipt_target_id AS receipt_nid
    FROM {node} n
    LEFT JOIN {field_data_field_reward_recipient} rec
      ON rec.entity_id = n.nid AND rec.entity_type = 'node' AND rec.deleted = 0
    LEFT JOIN {field_data_field_reward_amount} amt
      ON amt.entity_id = n.nid AND amt.entity_type = 'node' AND amt.deleted = 0
    LEFT JOIN {field_data_field_parent_purchase_receipt} pr
      ON pr.entity_id = n.nid AND pr.entity_type = 'node' AND pr.deleted = 0
    WHERE n.nid = :nid
  ", array(':nid' => $nid))->fetchObject();
  if (!$row) {
    return ROVAS_MERIT_SOURCE_LEGACY;
  }
  return _rovas_merit_infer_source_from_row($row);
}
