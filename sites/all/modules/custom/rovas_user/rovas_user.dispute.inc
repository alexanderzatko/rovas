<?php

/**
 * @file
 * Dispute judgment helpers (trusted-user camps, adverse history).
 */

/**
 * Dispute statuses that count as neither a win nor a tie for the accused.
 *
 * 2 = rectified, 4 = closed (accused lost).
 */
define('ROVAS_DISPUTE_STATUS_RECTIFIED', 2);
define('ROVAS_DISPUTE_STATUS_ACCUSED_LOST', 4);

/**
 * Number of prior adverse disputes after which a further adverse outcome
 * is always punished as a loss, even if the accused remedies.
 */
define('ROVAS_DISPUTE_THIRD_ADVERSE_THRESHOLD', 2);

/**
 * Camp score difference: trusted supporters (position 1) minus trusted
 * defenders (position 2). Positive means the complainant's camp leads.
 *
 * @param int $dispute_nid
 *
 * @return int
 */
function rovas_dispute_camp_diff($dispute_nid) {
  $dispute_nid = (int) $dispute_nid;
  if ($dispute_nid < 1) {
    return 0;
  }

  $sql = "
    SELECT
      SUM(CASE WHEN pos.field_position_value = 1 THEN 1 ELSE 0 END) AS supporters_count,
      SUM(CASE WHEN pos.field_position_value = 2 THEN 1 ELSE 0 END) AS defenders_count
    FROM {node} node
    INNER JOIN {field_data_field_dispute} d
      ON node.nid = d.entity_id
      AND d.entity_type = 'node'
      AND d.deleted = 0
    INNER JOIN {field_data_field_position} pos
      ON node.nid = pos.entity_id
      AND pos.entity_type = 'node'
      AND pos.deleted = 0
    INNER JOIN {field_data_field_trusted_user} t
      ON t.entity_type = 'user'
      AND t.entity_id = node.uid
      AND t.deleted = 0
      AND t.field_trusted_user_value = 1
    WHERE d.field_dispute_target_id = :dispute
      AND node.status = 1
      AND node.type = 'dispute_party'
  ";

  $row = db_query($sql, array(':dispute' => $dispute_nid))->fetchObject();
  $sup = isset($row->supporters_count) ? (int) $row->supporters_count : 0;
  $def = isset($row->defenders_count) ? (int) $row->defenders_count : 0;

  return $sup - $def;
}

/**
 * Count closed disputes against a user that were neither a win nor a tie.
 *
 * @param int $accused_uid
 * @param int|null $exclude_nid
 *   Optional dispute nid to exclude (usually the current dispute).
 *
 * @return int
 */
function rovas_dispute_prior_adverse_count($accused_uid, $exclude_nid = NULL) {
  $accused_uid = (int) $accused_uid;
  if ($accused_uid < 1) {
    return 0;
  }

  $query = db_select('node', 'n');
  $query->fields('n', array('nid'));
  $query->condition('n.type', 'dispute');
  $query->condition('n.status', 1);
  $query->join('field_data_field_accused', 'a',
    "a.entity_id = n.nid AND a.entity_type = 'node' AND a.deleted = 0");
  $query->condition('a.field_accused_target_id', $accused_uid);
  $query->join('field_data_field_dispute_status', 's',
    "s.entity_id = n.nid AND s.entity_type = 'node' AND s.deleted = 0");
  $query->condition('s.field_dispute_status_value', array(
    ROVAS_DISPUTE_STATUS_RECTIFIED,
    ROVAS_DISPUTE_STATUS_ACCUSED_LOST,
  ), 'IN');

  if (!empty($exclude_nid)) {
    $query->condition('n.nid', (int) $exclude_nid, '<>');
  }

  return (int) $query->countQuery()->execute()->fetchField();
}

/**
 * Whether remedying the current dispute should still apply the lost penalty.
 *
 * True when the accused already has at least two prior adverse outcomes
 * (rectified or lost), so this dispute is their third or later.
 *
 * @param object|int $dispute
 *   Dispute node or nid.
 *
 * @return bool
 */
function rovas_dispute_should_force_lost_on_remedy($dispute) {
  if (is_numeric($dispute)) {
    $dispute = node_load((int) $dispute);
  }
  if (empty($dispute) || empty($dispute->nid) || $dispute->type !== 'dispute') {
    return FALSE;
  }

  $accused_uid = 0;
  if (!empty($dispute->field_accused[LANGUAGE_NONE][0]['target_id'])) {
    $accused_uid = (int) $dispute->field_accused[LANGUAGE_NONE][0]['target_id'];
  }
  if ($accused_uid < 1) {
    return FALSE;
  }

  return rovas_dispute_prior_adverse_count($accused_uid, $dispute->nid) >= ROVAS_DISPUTE_THIRD_ADVERSE_THRESHOLD;
}

/**
 * Camp diff for Rules PHP, forcing a complainant win when remedy must still
 * apply the lost-dispute penalty (third adverse dispute).
 *
 * @param int $dispute_nid
 *
 * @return int
 */
function rovas_dispute_camp_diff_for_penalty($dispute_nid) {
  $dispute_nid = (int) $dispute_nid;
  if ($dispute_nid < 1) {
    return 0;
  }
  if (rovas_dispute_should_force_lost_on_remedy($dispute_nid)) {
    return 1;
  }
  return rovas_dispute_camp_diff($dispute_nid);
}
