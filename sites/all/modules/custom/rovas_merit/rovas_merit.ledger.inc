<?php

/**
 * @file
 * Merit ledger helpers: wallet, user_expertise, disbursement nodes.
 */

/** @var float Balance invariant tolerance (ME). */
define('ROVAS_MERIT_BALANCE_TOLERANCE', 0.02);

/** Merit source: verified volunteer work report reward / earned project share. */
define('ROVAS_MERIT_SOURCE_VOLUNTEER_WORK', 'volunteer_work');

/** Merit source: CHR→ME purchase (forex). */
define('ROVAS_MERIT_SOURCE_VOLUNTEER', 'volunteer');

/** Merit source: VAT / tax claim (Civic). */
define('ROVAS_MERIT_SOURCE_TAX_PAYMENT', 'tax_payment');

/** Merit source: EUR donation, EUR buy-merits, purchased timeshare project-reward ME. */
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
      // Donor Path-1 ME for paying into a project (not shareholder split).
      return ROVAS_MERIT_SOURCE_CHARITABLE_DONATION;

    default:
      return ROVAS_MERIT_SOURCE_LEGACY;
  }
}

/**
 * Merit source for ME a user receives when a project is rewarded.
 *
 * - Purchased timeshare → always charitable_donation.
 * - co_worker shares (any hours, including 0% / %-only trail shares) → volunteer_work.
 *
 * @param string $allocation_bundle
 *   co_worker_merit_allocation|timeshare_merit_allocation.
 * @param float $hours
 *   Unused for classification; kept for call-site compatibility.
 *
 * @return string
 *   field_merit_source value.
 */
function rovas_merit_source_for_project_share_reward($allocation_bundle, $hours = 0) {
  if ($allocation_bundle === 'timeshare_merit_allocation') {
    return ROVAS_MERIT_SOURCE_CHARITABLE_DONATION;
  }
  // co_worker_merit_allocation (and unknown non-timeshare bundles): volunteering.
  return ROVAS_MERIT_SOURCE_VOLUNTEER_WORK;
}

/**
 * User co_worker / timeshare allocations on a project.
 *
 * @param int $uid
 * @param int $project_nid
 *
 * @return array
 *   List of objects with bundle, hours, share.
 */
function _rovas_merit_user_share_allocs($uid, $project_nid) {
  $uid = (int) $uid;
  $project_nid = (int) $project_nid;
  if ($uid < 1 || $project_nid < 1) {
    return array();
  }
  return db_query("
    SELECT n.type AS bundle,
      COALESCE(hw.field_total_hours_worked_value, 0) AS hours,
      COALESCE(ma.field_merit_allocation_value, 0) AS share
    FROM {node} n
    INNER JOIN {field_data_field_project} p
      ON p.entity_id = n.nid AND p.entity_type = 'node' AND p.deleted = 0
    INNER JOIN {field_data_field_coworker} cw
      ON cw.entity_id = n.nid AND cw.entity_type = 'node' AND cw.deleted = 0
    LEFT JOIN {field_data_field_total_hours_worked} hw
      ON hw.entity_id = n.nid AND hw.entity_type = 'node' AND hw.deleted = 0
    LEFT JOIN {field_data_field_merit_allocation} ma
      ON ma.entity_id = n.nid AND ma.entity_type = 'node' AND ma.deleted = 0
    WHERE n.status = 1
      AND p.field_project_target_id = :prid
      AND n.type IN ('co_worker_merit_allocation', 'timeshare_merit_allocation')
      AND cw.field_coworker_target_id = :uid
  ", array(
    ':prid' => $project_nid,
    ':uid' => $uid,
  ))->fetchAll();
}

/**
 * Infer shareholder ME source from allocation rows (and amount when dual).
 *
 * @param array $allocs
 * @param float $amount
 * @param int $receipt_nid
 * @param int $project_nid
 *
 * @return string
 */
function _rovas_merit_source_from_share_allocs(array $allocs, $amount, $receipt_nid = 0, $project_nid = 0) {
  $labor_hours = 0.0;
  $labor_share = 0.0;
  $timeshare_hours = 0.0;
  $timeshare_share = 0.0;
  $has_labor = FALSE;
  $has_timeshare = FALSE;

  foreach ($allocs as $row) {
    $bundle = isset($row->bundle) ? $row->bundle : '';
    $hours = isset($row->hours) ? (float) $row->hours : 0.0;
    $share = isset($row->share) ? (float) $row->share : 0.0;
    if ($bundle === 'timeshare_merit_allocation') {
      $has_timeshare = TRUE;
      $timeshare_hours += $hours;
      $timeshare_share += $share;
    }
    elseif ($bundle === 'co_worker_merit_allocation') {
      $has_labor = TRUE;
      $labor_hours += $hours;
      $labor_share += $share;
    }
  }

  if ($has_timeshare && !$has_labor) {
    return ROVAS_MERIT_SOURCE_CHARITABLE_DONATION;
  }
  if ($has_labor && !$has_timeshare) {
    return rovas_merit_source_for_project_share_reward('co_worker_merit_allocation', $labor_hours);
  }
  if (!($has_labor && $has_timeshare)) {
    return ROVAS_MERIT_SOURCE_CHARITABLE_DONATION;
  }

  // Dual: purchased portion is always charity; labor/earned → volunteer_work.
  // Match this disbursement amount to the closer expected split.
  $expected = _rovas_merit_expected_user_share_merits($project_nid, $labor_share, $labor_hours, $timeshare_share, $timeshare_hours, $receipt_nid);
  if ($expected === NULL) {
    // Dual without a reliable amount split: co_worker portion is volunteering.
    return ROVAS_MERIT_SOURCE_VOLUNTEER_WORK;
  }

  $d_labor = abs($amount - $expected['labor']);
  $d_share = abs($amount - $expected['timeshare']);
  if ($d_share < $d_labor) {
    return ROVAS_MERIT_SOURCE_CHARITABLE_DONATION;
  }
  return rovas_merit_source_for_project_share_reward('co_worker_merit_allocation', $labor_hours);
}

/**
 * Expected labor vs timeshare ME for a user from a project-reward receipt.
 *
 * Uses the same split as rules_process_purchase_core mrewarder.
 *
 * @param int $project_nid
 * @param float $labor_share
 * @param float $labor_hours
 * @param float $timeshare_share
 * @param float $timeshare_hours
 * @param int $receipt_nid
 *
 * @return array|null
 *   keys labor, timeshare — or NULL when amount_paid cannot be inferred.
 */
function _rovas_merit_expected_user_share_merits($project_nid, $labor_share, $labor_hours, $timeshare_share, $timeshare_hours, $receipt_nid) {
  $project_nid = (int) $project_nid;
  $receipt_nid = (int) $receipt_nid;
  if ($project_nid < 1) {
    return NULL;
  }

  // Approximate amount_paid as sum of shareholder disbursements on this receipt.
  $amount_paid = 0.0;
  if ($receipt_nid > 0) {
    $amount_paid = (float) db_query("
      SELECT COALESCE(SUM(amt.field_reward_amount_value), 0)
      FROM {node} n
      INNER JOIN {field_data_field_parent_purchase_receipt} pr
        ON pr.entity_id = n.nid AND pr.entity_type = 'node' AND pr.deleted = 0
      INNER JOIN {field_data_field_reward_amount} amt
        ON amt.entity_id = n.nid AND amt.entity_type = 'node' AND amt.deleted = 0
      WHERE n.status = 1
        AND n.type = 'merits_disbursement'
        AND pr.field_parent_purchase_receipt_target_id = :rid
        AND amt.field_reward_amount_value > 0
    ", array(':rid' => $receipt_nid))->fetchField();
  }
  if ($amount_paid <= 0) {
    return NULL;
  }

  $sum = db_query("
    SELECT COALESCE(SUM(ma.field_merit_allocation_value), 0) AS shares,
      COALESCE(SUM(hw.field_total_hours_worked_value), 0) AS hours
    FROM {node} node
    INNER JOIN {field_data_field_project} fpr
      ON node.nid = fpr.entity_id AND fpr.entity_type = 'node' AND fpr.deleted = 0
    LEFT JOIN {field_data_field_merit_allocation} ma
      ON node.nid = ma.entity_id AND ma.entity_type = 'node' AND ma.deleted = 0
    LEFT JOIN {field_data_field_total_hours_worked} hw
      ON node.nid = hw.entity_id AND hw.entity_type = 'node' AND hw.deleted = 0
    WHERE fpr.field_project_target_id = :prid
      AND node.status = 1
      AND node.type IN ('project_merit_allocation', 'co_worker_merit_allocation', 'timeshare_merit_allocation')
  ", array(':prid' => $project_nid))->fetchAssoc();

  $tsdiff = 100.0 - (float) $sum['shares'];
  $total_hours = (float) $sum['hours'];

  $labor = ($labor_share / 100.0) * $amount_paid;
  $timeshare = ($timeshare_share / 100.0) * $amount_paid;
  if ($total_hours > 0) {
    $labor += ($tsdiff / 100.0) * $amount_paid * ($labor_hours / $total_hours);
    $timeshare += ($tsdiff / 100.0) * $amount_paid * ($timeshare_hours / $total_hours);
  }

  return array(
    'labor' => $labor,
    'timeshare' => $timeshare,
  );
}

/**
 * Whether allocation rows include a co_worker (volunteer) share.
 *
 * @param array $allocs
 *
 * @return bool
 */
function _rovas_merit_allocs_have_coworker(array $allocs) {
  foreach ($allocs as $row) {
    $bundle = isset($row->bundle) ? $row->bundle : '';
    if ($bundle === 'co_worker_merit_allocation') {
      return TRUE;
    }
  }
  return FALSE;
}

/**
 * All co_worker / timeshare projects for a user, keyed by project nid.
 *
 * @param int $uid
 *
 * @return array
 *   project_nid => list of alloc objects (bundle, hours, share).
 */
function _rovas_merit_user_share_projects_map($uid) {
  $uid = (int) $uid;
  $out = array();
  if ($uid < 1) {
    return $out;
  }
  $result = db_query("
    SELECT p.field_project_target_id AS project_nid,
      n.type AS bundle,
      COALESCE(hw.field_total_hours_worked_value, 0) AS hours,
      COALESCE(ma.field_merit_allocation_value, 0) AS share
    FROM {node} n
    INNER JOIN {field_data_field_project} p
      ON p.entity_id = n.nid AND p.entity_type = 'node' AND p.deleted = 0
    INNER JOIN {field_data_field_coworker} cw
      ON cw.entity_id = n.nid AND cw.entity_type = 'node' AND cw.deleted = 0
    LEFT JOIN {field_data_field_total_hours_worked} hw
      ON hw.entity_id = n.nid AND hw.entity_type = 'node' AND hw.deleted = 0
    LEFT JOIN {field_data_field_merit_allocation} ma
      ON ma.entity_id = n.nid AND ma.entity_type = 'node' AND ma.deleted = 0
    WHERE n.status = 1
      AND n.type IN ('co_worker_merit_allocation', 'timeshare_merit_allocation')
      AND cw.field_coworker_target_id = :uid
  ", array(':uid' => $uid));
  foreach ($result as $row) {
    $prid = (int) $row->project_nid;
    if ($prid < 1) {
      continue;
    }
    if (!isset($out[$prid])) {
      $out[$prid] = array();
    }
    $out[$prid][] = (object) array(
      'bundle' => $row->bundle,
      'hours' => $row->hours,
      'share' => $row->share,
    );
  }
  return $out;
}

/**
 * project_merit_allocation children for a batch of parent projects.
 *
 * @param array $parent_nids
 *
 * @return array
 *   parent_nid => list of child project nids.
 */
function _rovas_merit_shareholder_children_map(array $parent_nids) {
  $out = array();
  $parent_nids = array_values(array_unique(array_filter(array_map('intval', $parent_nids))));
  if (empty($parent_nids)) {
    return $out;
  }
  $result = db_query("
    SELECT p.field_project_target_id AS parent_nid,
      sh.field_shareholder_project_target_id AS child_nid
    FROM {node} n
    INNER JOIN {field_data_field_project} p
      ON p.entity_id = n.nid AND p.entity_type = 'node' AND p.deleted = 0
    INNER JOIN {field_data_field_shareholder_project} sh
      ON sh.entity_id = n.nid AND sh.entity_type = 'node' AND sh.deleted = 0
    WHERE n.status = 1
      AND n.type = 'project_merit_allocation'
      AND p.field_project_target_id IN (:parents)
  ", array(':parents' => $parent_nids));
  foreach ($result as $row) {
    $parent = (int) $row->parent_nid;
    $child = (int) $row->child_nid;
    if ($parent < 1 || $child < 1 || $child === $parent) {
      continue;
    }
    if (!isset($out[$parent])) {
      $out[$parent] = array();
    }
    $out[$parent][$child] = $child;
  }
  foreach ($out as $parent => $children) {
    $out[$parent] = array_values($children);
  }
  return $out;
}

/**
 * Reachable projects where the user has personal share allocs (mirrors mrewarder).
 *
 * Walks project_merit_allocation edges with a loop guard. Includes the root
 * when the user holds shares there directly.
 *
 * @param int $uid
 * @param int $root_project_nid
 *   Originally rewarded project (receipt field_receipt_project).
 *
 * @return array
 *   List of array(project_nid, allocs).
 */
function _rovas_merit_cascade_share_candidates($uid, $root_project_nid) {
  $uid = (int) $uid;
  $root_project_nid = (int) $root_project_nid;
  $candidates = array();
  if ($uid < 1 || $root_project_nid < 1) {
    return $candidates;
  }

  $user_map = _rovas_merit_user_share_projects_map($uid);
  if (empty($user_map)) {
    return $candidates;
  }

  $found = array();
  if (isset($user_map[$root_project_nid])) {
    $found[$root_project_nid] = $user_map[$root_project_nid];
    // Still walk nested project shareholders: same receipt can also pay the
    // user via a deeper leaf (mrewarder pays co_worker on root and recurses).
  }

  $visited = array();
  $queued = array($root_project_nid => TRUE);
  $queue = array($root_project_nid);
  $max_nodes = 5000;
  $seen = 0;

  while (!empty($queue) && $seen < $max_nodes) {
    $batch = array();
    foreach ($queue as $nid) {
      $nid = (int) $nid;
      if ($nid < 1 || isset($visited[$nid])) {
        continue;
      }
      $visited[$nid] = TRUE;
      $batch[] = $nid;
      $seen++;
    }
    $queue = array();
    if (empty($batch)) {
      break;
    }

    $children_map = _rovas_merit_shareholder_children_map($batch);
    foreach ($batch as $parent) {
      if (empty($children_map[$parent])) {
        continue;
      }
      foreach ($children_map[$parent] as $child) {
        $child = (int) $child;
        if ($child < 1 || isset($visited[$child]) || isset($queued[$child])) {
          continue;
        }
        if (isset($user_map[$child]) && !isset($found[$child])) {
          $found[$child] = $user_map[$child];
        }
        // Continue past user leaves: another nested path may also pay them.
        $queued[$child] = TRUE;
        $queue[] = $child;
      }
    }
  }

  foreach ($found as $prid => $allocs) {
    $candidates[] = array(
      'project_nid' => (int) $prid,
      'allocs' => $allocs,
    );
  }
  return $candidates;
}

/**
 * Pick merit source from one or more cascade leaf candidates.
 *
 * When several leaves match, prefer co_worker leaves over timeshare-only
 * (avoids purchased-share siblings forcing charity). Dual labor+timeshare
 * amount matching still applies within a leaf.
 *
 * @param array $candidates
 * @param float $amount
 * @param int $receipt_nid
 *
 * @return string
 */
function _rovas_merit_source_from_cascade_candidates(array $candidates, $amount, $receipt_nid = 0) {
  if (empty($candidates)) {
    return ROVAS_MERIT_SOURCE_CHARITABLE_DONATION;
  }

  $coworker = array();
  $other = array();
  foreach ($candidates as $c) {
    $allocs = isset($c['allocs']) ? $c['allocs'] : array();
    if (_rovas_merit_allocs_have_coworker($allocs)) {
      $coworker[] = $c;
    }
    else {
      $other[] = $c;
    }
  }
  // Prefer co_worker leaves when present (volunteering, including 0h %-shares).
  $pool = !empty($coworker) ? $coworker : $other;

  if (count($pool) === 1) {
    $c = reset($pool);
    return _rovas_merit_source_from_share_allocs($c['allocs'], $amount, $receipt_nid, $c['project_nid']);
  }

  $sources = array();
  foreach ($pool as $c) {
    $sources[] = _rovas_merit_source_from_share_allocs($c['allocs'], $amount, $receipt_nid, $c['project_nid']);
  }
  $sources = array_values(array_unique($sources));
  if (count($sources) === 1) {
    return $sources[0];
  }

  // Mixed pool: prefer volunteer when any co_worker leaf remains in play.
  if (!empty($coworker)) {
    return ROVAS_MERIT_SOURCE_VOLUNTEER_WORK;
  }
  return ROVAS_MERIT_SOURCE_CHARITABLE_DONATION;
}

/**
 * Infer source for shareholder ME from a project-reward purchase_receipt.
 *
 * Walks the full project_merit_allocation graph (like live mrewarder), not a
 * single hop. Disbursements only store the top rewarded project on the parent
 * receipt, so nested labor ME must be recovered by cascade.
 *
 * @param int $uid
 * @param int $project_nid
 * @param float $amount
 * @param int $receipt_nid
 *
 * @return string
 */
function rovas_merit_infer_project_shareholder_source($uid, $project_nid, $amount, $receipt_nid = 0) {
  $uid = (int) $uid;
  $project_nid = (int) $project_nid;
  $candidates = _rovas_merit_cascade_share_candidates($uid, $project_nid);
  if (empty($candidates)) {
    // No allocation found: default charity (capital-like / unknown).
    return ROVAS_MERIT_SOURCE_CHARITABLE_DONATION;
  }
  return _rovas_merit_source_from_cascade_candidates($candidates, $amount, $receipt_nid);
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
 * Financial / non-labor fallback NACE tid (64.99).
 *
 * @return int
 */
function rovas_merit_financial_nace_tid() {
  if (defined('ROVAS_CIVIC_FALLBACK_NACE_TID') && (int) ROVAS_CIVIC_FALLBACK_NACE_TID > 0) {
    return (int) ROVAS_CIVIC_FALLBACK_NACE_TID;
  }
  return (int) ROVAS_MERIT_FINANCIAL_NACE_TID;
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
  $expertise_ok = abs($delta_e) <= $tol;
  $ledger_ok = abs($delta_l) <= $tol;
  return array(
    'ok' => ($expertise_ok && $ledger_ok),
    'expertise_ok' => $expertise_ok,
    'ledger_ok' => $ledger_ok,
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
 * @param bool $force
 *   When TRUE, also update banned users (reconcile / repair only).
 *
 * @return bool
 */
function rovas_merit_adjust_user_expertise($uid, $delta, $nace_tid, $share = 1.0, $force = FALSE) {
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
  if (!$force && in_array(5, array_keys($roles))) {
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
 * Sum of merit_penalty ledger amounts for a user (usually negative or zero).
 *
 * @param int $uid
 *
 * @return float
 */
function rovas_merit_user_penalty_ledger_sum($uid) {
  $uid = (int) $uid;
  if ($uid < 1) {
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
      AND n.type = 'merit_penalty'
      AND rec.field_reward_recipient_target_id = :uid
  ", array(':uid' => $uid))->fetchField();
}

/**
 * Current user_expertise ME on one NACE category.
 *
 * @param int $uid
 * @param int $nace_tid
 *
 * @return float|null
 *   Merits value, or NULL when no expertise node exists.
 */
function rovas_merit_user_expertise_on_nace($uid, $nace_tid) {
  $uid = (int) $uid;
  $nace_tid = (int) $nace_tid;
  if ($uid < 1 || $nace_tid < 1) {
    return NULL;
  }
  $val = db_query("
    SELECT m.field_merits_value
    FROM {node} node
    INNER JOIN {field_data_field_expert} x
      ON node.nid = x.entity_id AND x.entity_type = 'node' AND x.deleted = 0
    INNER JOIN {field_data_field_category} c
      ON node.nid = c.entity_id AND c.entity_type = 'node' AND c.deleted = 0
    INNER JOIN {field_data_field_merits} m
      ON node.nid = m.entity_id AND m.entity_type = 'node' AND m.deleted = 0
    WHERE node.type = 'user_expertise'
      AND node.status = 1
      AND x.field_expert_target_id = :uid
      AND c.field_category_tid = :cat
    LIMIT 1
  ", array(':uid' => $uid, ':cat' => $nace_tid))->fetchField();
  return ($val === FALSE || $val === NULL) ? NULL : (float) $val;
}

/**
 * Clamp bogus negative governance-NACE expertise left by the old reconcile bug.
 *
 * Old --fix used sign-flipped deltas on 94.99, so buyers with missing expertise
 * got large negative scores there without matching merit_penalty ledger rows.
 *
 * Only clamp when 94.99 is more negative than the penalty ledger explains AND
 * overall expertise is still below the wallet (the buyer/reconcile artifact case).
 * Do not touch correctly applied legacy penalty expertise that already matches
 * total_me.
 *
 * @param int $uid
 * @param bool $apply_fix
 *
 * @return array
 *   keys: clamped (bool), before, after, expected, applied_delta.
 */
function rovas_merit_repair_bogus_penalty_expertise($uid, $apply_fix = FALSE) {
  $uid = (int) $uid;
  $out = array(
    'clamped' => FALSE,
    'before' => NULL,
    'after' => NULL,
    'expected' => 0.0,
    'applied_delta' => 0.0,
  );
  $penalty_tid = rovas_merit_penalty_nace_tid();
  if ($uid < 1 || $penalty_tid < 1) {
    return $out;
  }

  $before = rovas_merit_user_expertise_on_nace($uid, $penalty_tid);
  if ($before === NULL) {
    return $out;
  }
  $out['before'] = $before;

  // Penalty ledger amounts are negative; expertise on 94.99 should not be
  // more negative than that sum (and never negative with no penalty rows).
  $expected = rovas_merit_user_penalty_ledger_sum($uid);
  if ($expected > 0) {
    $expected = 0.0;
  }
  $out['expected'] = $expected;

  if ($before >= ($expected - ROVAS_MERIT_BALANCE_TOLERANCE)) {
    $out['after'] = $before;
    return $out;
  }

  $delta = $expected - $before;
  // Artifact signature: excess negative on 94.99 while wallet > expertise sum.
  $check = rovas_merit_balance_check($uid);
  if ($check['delta_expertise'] <= ROVAS_MERIT_BALANCE_TOLERANCE) {
    // Expertise already matches (or exceeds) the wallet — leave 94.99 alone.
    $out['after'] = $before;
    return $out;
  }

  $out['applied_delta'] = $delta;
  if ($apply_fix) {
    rovas_merit_adjust_user_expertise($uid, $delta, $penalty_tid, 1.0, TRUE);
    $out['clamped'] = TRUE;
    $out['after'] = rovas_merit_user_expertise_on_nace($uid, $penalty_tid);
  }
  else {
    $out['after'] = $before;
  }
  return $out;
}

/**
 * Reconcile one user: close wallet vs expertise drift on the correct NACE.
 *
 * - Positive drift (wallet > expertise): credit financial fallback NACE (64.99).
 * - Negative drift (expertise > wallet): debit governance penalty NACE (94.99)
 *   for legacy penalties that only hit the wallet.
 *
 * Never sign-flips the delta (previous bug parked buyers as −ME on 94.99).
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
  $result['repair'] = NULL;

  if (!$apply_fix) {
    return $result;
  }

  // First undo bogus negative 94.99 from the previous reconcile implementation.
  $repair = rovas_merit_repair_bogus_penalty_expertise($uid, TRUE);
  $result['repair'] = $repair;
  if (!empty($repair['clamped'])) {
    $check = rovas_merit_balance_check($uid);
    $result = array_merge($result, $check);
    $result['fixed'] = TRUE;
    $result['repair'] = $repair;
  }

  $delta = $check['delta_expertise'];
  if (abs($delta) <= ROVAS_MERIT_BALANCE_TOLERANCE) {
    $result['after'] = rovas_merit_balance_check($uid);
    // Expertise is aligned; remaining failure (if any) is ledger-only legacy drift.
    return $result;
  }

  if ($delta > 0) {
    // Expertise too low (e.g. forex grant with no work-report mix).
    $nace_tid = rovas_merit_financial_nace_tid();
    if ($nace_tid < 1) {
      $result['error'] = 'financial_nace_missing';
      return $result;
    }
    if (!rovas_merit_adjust_user_expertise($uid, $delta, $nace_tid, 1.0, TRUE)) {
      $result['error'] = 'expertise_adjust_failed';
      return $result;
    }
    $result['fixed'] = TRUE;
    $result['fix_delta'] = $delta;
    $result['fix_nace'] = $nace_tid;
  }
  else {
    // Expertise too high — legacy wallet-only penalties.
    $nace_tid = rovas_merit_penalty_nace_tid();
    if ($nace_tid < 1) {
      $result['error'] = 'penalty_nace_missing';
      return $result;
    }
    if (!rovas_merit_adjust_user_expertise($uid, $delta, $nace_tid, 1.0, TRUE)) {
      $result['error'] = 'expertise_adjust_failed';
      return $result;
    }
    $result['fixed'] = TRUE;
    $result['fix_delta'] = $delta;
    $result['fix_nace'] = $nace_tid;
  }

  $result['after'] = rovas_merit_balance_check($uid);
  return $result;
}

/**
 * Whether a purchase_merits receipt was funded via an EUR Commerce merits order.
 *
 * @param int $uid
 * @param int $created
 *   Unix timestamp of the purchase_merits / disbursement.
 * @param float $amount
 *   ME amount (≈ converted CHR).
 *
 * @return bool
 */
function _rovas_merit_purchase_merits_was_eur_funded($uid, $created, $amount) {
  $uid = (int) $uid;
  $created = (int) $created;
  if ($uid < 1 || $created < 1 || !db_table_exists('commerce_order')) {
    return FALSE;
  }

  $product_id = (int) db_query("SELECT product_id FROM {commerce_product} WHERE sku = :sku", array(
    ':sku' => 'merits',
  ))->fetchField();
  if ($product_id < 1 || !db_table_exists('field_data_commerce_product')) {
    return FALSE;
  }

  // Merits EUR orders use product SKU "merits" (see rovas_forex_create_merits_eur_order).
  $sql = "
    SELECT o.order_id
    FROM {commerce_order} o
    INNER JOIN {field_data_commerce_line_items} fcli
      ON fcli.entity_id = o.order_id AND fcli.entity_type = 'commerce_order' AND fcli.deleted = 0
    INNER JOIN {field_data_commerce_product} fcp
      ON fcp.entity_id = fcli.commerce_line_items_line_item_id
      AND fcp.entity_type = 'commerce_line_item' AND fcp.deleted = 0
    WHERE o.uid = :uid
      AND o.status = 'completed'
      AND fcp.commerce_product_product_id = :pid
      AND COALESCE(o.changed, o.created) BETWEEN :lo AND :hi
  ";
  $args = array(
    ':uid' => $uid,
    ':pid' => $product_id,
    ':lo' => $created - 600,
    ':hi' => $created + 600,
  );
  if (field_info_field('field_payer_type') && db_table_exists('field_data_field_payer_type')) {
    $sql .= " AND EXISTS (
      SELECT 1 FROM {field_data_field_payer_type} pt
      WHERE pt.entity_id = o.order_id AND pt.entity_type = 'commerce_order'
        AND pt.deleted = 0 AND pt.field_payer_type_value = 'user'
    )";
  }
  $sql .= ' ORDER BY ABS(COALESCE(o.changed, o.created) - :created) ASC LIMIT 1';
  $args[':created'] = $created;

  return (bool) db_query($sql, $args)->fetchField();
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
      if (_rovas_merit_purchase_merits_was_eur_funded($uid, $created, $amount)) {
        return ROVAS_MERIT_SOURCE_CHARITABLE_DONATION;
      }
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
      $project_nid = (int) db_query('SELECT field_receipt_project_target_id
        FROM {field_data_field_receipt_project}
        WHERE entity_id = :nid AND entity_type = :et AND deleted = 0 LIMIT 1', array(
        ':nid' => $receipt_nid,
        ':et' => 'node',
      ))->fetchField();
      if ($project_nid > 0) {
        // Shareholder split from a project reward: co_worker vs timeshare.
        return rovas_merit_infer_project_shareholder_source($uid, $project_nid, $amount, $receipt_nid);
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
