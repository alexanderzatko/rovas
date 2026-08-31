<?php

/**
 * @file
 * Civic in-app work-report verification APIs.
 *
 * See docs/integration/civic.md → Next work: Civic WR verification (in-app).
 */

define('ROVAS_CIVIC_VERIFICATION_NEEDED', 2);

/**
 * Services callback: GET /rovas/civic_pending_verifications?lang=
 */
function rovas_civic_services_pending_verifications_index($lang = '') {
  $uid = rovas_civic_request_uid();
  if ($uid < 1) {
    return rovas_civic_volunteer_error(401, 'unauthorized', 'Authentication required');
  }
  return rovas_civic_pending_verifications($uid, rovas_civic_request_lang($lang));
}

/**
 * Services callback: GET /rovas/civic_work_report/{nid}?lang=
 */
function rovas_civic_services_work_report_retrieve($nid, $lang = '') {
  $uid = rovas_civic_request_uid();
  if ($uid < 1) {
    return rovas_civic_volunteer_error(401, 'unauthorized', 'Authentication required');
  }

  $wr_nid = (int) $nid;
  if ($wr_nid < 1) {
    return rovas_civic_volunteer_error(404, 'not_found', 'Work report not found.');
  }

  $node = node_load($wr_nid);
  if (!$node || $node->type !== 'work_report' || (int) $node->status !== 1) {
    return rovas_civic_volunteer_error(404, 'not_found', 'Work report not found.');
  }

  $slot = rovas_civic_verifier_assigned_slot($uid, $wr_nid);
  if (!$slot) {
    return rovas_civic_volunteer_error(403, 'not_assigned', 'You are not assigned to verify this work report.');
  }

  $lang = rovas_civic_request_lang($lang);
  $item = rovas_civic_work_report_item($node, $lang, TRUE, $uid);
  $item['verification'] = rovas_civic_work_report_verification_context($node, $slot);

  return $item;
}

/**
 * Services callback: POST /rovas/civic_submit_verification
 *
 * @param array $data
 */
function rovas_civic_services_submit_verification_create($data) {
  $uid = rovas_civic_request_uid();
  if ($uid < 1) {
    return rovas_civic_volunteer_error(401, 'unauthorized', 'Authentication required');
  }
  if (!is_array($data)) {
    $data = array();
  }

  $slot_nid = isset($data['slot_nid']) ? (int) $data['slot_nid'] : 0;
  $verdict = isset($data['verdict']) ? strtolower(trim((string) $data['verdict'])) : '';
  $note = isset($data['note']) ? trim((string) $data['note']) : '';
  $verified_hours = isset($data['verified_hours']) ? $data['verified_hours'] : NULL;

  if ($slot_nid < 1 || $verdict === '') {
    return rovas_civic_volunteer_error(400, 'invalid_input', 'slot_nid and verdict are required.');
  }
  if (!in_array($verdict, array('approve', 'reject', 'decline'), TRUE)) {
    return rovas_civic_volunteer_error(400, 'invalid_input', 'verdict must be approve, reject, or decline.');
  }

  $slot = node_load($slot_nid);
  if (!$slot || $slot->type !== 'verification_slot' || (int) $slot->status !== 1) {
    return rovas_civic_volunteer_error(403, 'not_assigned', 'Verification slot not found.');
  }

  $owner_items = field_get_items('node', $slot, 'field_slip_owner');
  $owner_uid = !empty($owner_items[0]['target_id']) ? (int) $owner_items[0]['target_id'] : 0;
  if ($owner_uid !== $uid) {
    return rovas_civic_volunteer_error(403, 'not_assigned', 'You are not assigned to this verification slot.');
  }

  if (rovas_civic_slot_is_declined($slot)) {
    return rovas_civic_volunteer_error(403, 'not_assigned', 'This verification slot was declined.');
  }

  $wr_items = field_get_items('node', $slot, 'field_report_to_verify');
  $wr_nid = !empty($wr_items[0]['target_id']) ? (int) $wr_items[0]['target_id'] : 0;
  if ($wr_nid < 1) {
    return rovas_civic_volunteer_error(403, 'not_assigned', 'No work report is assigned to this slot.');
  }

  $work_report = node_load($wr_nid);
  if (!$work_report || $work_report->type !== 'work_report' || (int) $work_report->status !== 1) {
    return rovas_civic_volunteer_error(404, 'not_found', 'Work report not found.');
  }

  if ($verdict === 'decline') {
    return rovas_civic_submit_verification_decline($slot, $work_report, $uid, $note);
  }

  if ($verdict === 'reject' && $note === '') {
    return rovas_civic_volunteer_error(400, 'invalid_input', 'note is required when rejecting a work report.');
  }

  if (rovas_civic_user_verified_work_report($wr_nid, $uid)) {
    return rovas_civic_volunteer_error(409, 'already_verified', 'You have already verified this work report.');
  }

  if (!rovas_civic_wr_is_verifiable_by_user($work_report, $uid)) {
    return rovas_civic_volunteer_error(403, 'not_verifiable', 'This work report cannot be verified by you.');
  }

  $hours_default = rovas_civic_node_float_field($work_report, 'field_hours_worked');
  if ($verified_hours === NULL || $verified_hours === '') {
    $verified_hours = $hours_default;
  }
  else {
    $verified_hours = (float) $verified_hours;
  }
  if ($verified_hours <= 0) {
    return rovas_civic_volunteer_error(400, 'invalid_input', 'verified_hours must be a positive number.');
  }

  $approve = ($verdict === 'approve');
  $verification_nid = rovas_civic_create_verification_node($work_report, $uid, $approve, $verified_hours, $note);
  if ($verification_nid < 1) {
    return rovas_civic_volunteer_error(403, 'not_verifiable', 'Failed to create verification.');
  }

  return array(
    'ok' => TRUE,
    'verification_nid' => (string) $verification_nid,
    'work_report_nid' => (string) $wr_nid,
    'verdict' => $verdict,
  );
}

/**
 * Pending assigned verification slots for the Bearer user.
 *
 * @param int $uid
 * @param string|null $lang
 *
 * @return array
 *   { total, items[] }
 */
function rovas_civic_pending_verifications($uid, $lang = NULL) {
  $uid = (int) $uid;
  $rows = rovas_civic_pending_assigned_slots_query($uid)->fetchAll();
  if (empty($rows)) {
    return array(
      'total' => 0,
      'items' => array(),
    );
  }

  $wr_nids = array();
  $author_uids = array();
  foreach ($rows as $row) {
    $wr_nids[(int) $row->wr_nid] = (int) $row->wr_nid;
    if (!empty($row->author_uid)) {
      $author_uids[(int) $row->author_uid] = (int) $row->author_uid;
    }
  }

  $reports = node_load_multiple(array_values($wr_nids));
  $authors = $author_uids ? user_load_multiple(array_values($author_uids)) : array();
  $items = array();

  foreach ($rows as $row) {
    $wr_nid = (int) $row->wr_nid;
    if (empty($reports[$wr_nid])) {
      continue;
    }
    if (rovas_civic_user_verified_work_report($wr_nid, $uid)) {
      continue;
    }
    $report = $reports[$wr_nid];
    $author_uid = (int) $report->uid;
    $labels = rovas_civic_user_public_labels($author_uid, isset($authors[$author_uid]) ? $authors[$author_uid] : NULL);
    $project_nid = rovas_civic_wr_project_nid($report);

    $items[] = array(
      'slot_nid' => (string) ((int) $row->slot_nid),
      'work_report_nid' => (string) $wr_nid,
      'title' => rovas_civic_node_localized_title($report, $lang),
      'author_username' => $labels['username'],
      'author_display_name' => $labels['display_name'],
      'project_nid' => $project_nid ? (string) $project_nid : NULL,
      'assigned_at' => rovas_civic_format_created((int) $row->assigned_at),
    );
  }

  return array(
    'total' => count($items),
    'items' => $items,
  );
}

/**
 * Base query for assigned, non-declined verification slots owned by a user.
 *
 * @param int $uid
 *
 * @return DatabaseStatementInterface
 */
function rovas_civic_pending_assigned_slots_query($uid) {
  $sql = "
    SELECT n.nid AS slot_nid,
      r.field_report_to_verify_target_id AS wr_nid,
      wr.uid AS author_uid,
      COALESCE(vd.field_verify_date_value, n.changed) AS assigned_at
    FROM {node} n
    INNER JOIN {field_data_field_slip_owner} o
      ON o.entity_id = n.nid AND o.entity_type = 'node' AND o.deleted = 0
    INNER JOIN {field_data_field_report_to_verify} r
      ON r.entity_id = n.nid AND r.entity_type = 'node' AND r.deleted = 0
    INNER JOIN {node} wr
      ON wr.nid = r.field_report_to_verify_target_id
      AND wr.type = 'work_report'
      AND wr.status = 1
    LEFT JOIN {field_data_field_declined} d
      ON d.entity_id = n.nid AND d.entity_type = 'node' AND d.deleted = 0
    LEFT JOIN {field_data_field_verify_date} vd
      ON vd.entity_id = n.nid AND vd.entity_type = 'node' AND vd.deleted = 0
    WHERE n.type = 'verification_slot'
      AND n.status = 1
      AND o.field_slip_owner_target_id = :uid
      AND r.field_report_to_verify_target_id IS NOT NULL
      AND r.field_report_to_verify_target_id > 0
      AND (d.field_declined_value IS NULL OR d.field_declined_value = 0)
    ORDER BY assigned_at DESC, n.nid DESC
  ";
  return db_query($sql, array(':uid' => (int) $uid));
}

/**
 * Assigned verification slot for a verifier and work report.
 *
 * @param int $verifier_uid
 * @param int $wr_nid
 *
 * @return object|null
 *   verification_slot node.
 */
function rovas_civic_verifier_assigned_slot($verifier_uid, $wr_nid) {
  $verifier_uid = (int) $verifier_uid;
  $wr_nid = (int) $wr_nid;
  if ($verifier_uid < 1 || $wr_nid < 1) {
    return NULL;
  }

  $slot_nid = db_query("
    SELECT n.nid
    FROM {node} n
    INNER JOIN {field_data_field_slip_owner} o
      ON o.entity_id = n.nid AND o.entity_type = 'node' AND o.deleted = 0
    INNER JOIN {field_data_field_report_to_verify} r
      ON r.entity_id = n.nid AND r.entity_type = 'node' AND r.deleted = 0
    LEFT JOIN {field_data_field_declined} d
      ON d.entity_id = n.nid AND d.entity_type = 'node' AND d.deleted = 0
    WHERE n.type = 'verification_slot'
      AND n.status = 1
      AND o.field_slip_owner_target_id = :uid
      AND r.field_report_to_verify_target_id = :wr
      AND (d.field_declined_value IS NULL OR d.field_declined_value = 0)
    ORDER BY n.nid ASC
    LIMIT 1
  ", array(
    ':uid' => $verifier_uid,
    ':wr' => $wr_nid,
  ))->fetchField();

  if (!$slot_nid) {
    return NULL;
  }
  $slot = node_load((int) $slot_nid);
  return ($slot && $slot->type === 'verification_slot') ? $slot : NULL;
}

/**
 * Whether a verification slot was declined.
 *
 * @param object $slot
 *
 * @return bool
 */
function rovas_civic_slot_is_declined($slot) {
  $items = field_get_items('node', $slot, 'field_declined');
  return !empty($items[0]['value']);
}

/**
 * Verification UI context for an assigned verifier.
 *
 * @param object $work_report
 * @param object $slot
 *
 * @return array
 */
function rovas_civic_work_report_verification_context($work_report, $slot) {
  $voluntary = FALSE;
  $vol_items = field_get_items('node', $slot, 'field_voluntary');
  if (!empty($vol_items[0]['value'])) {
    $voluntary = TRUE;
  }

  return array(
    'slot_nid' => (string) ((int) $slot->nid),
    'can_decline' => $voluntary,
    'verification_count' => rovas_civic_wr_verification_count($work_report),
    'verification_needed' => (int) ROVAS_CIVIC_VERIFICATION_NEEDED,
  );
}

/**
 * Count published verification nodes linked to a work report.
 *
 * Mirrors rules_determine_verification_outcome count.
 *
 * @param object $work_report
 *
 * @return int
 */
function rovas_civic_wr_verification_count($work_report) {
  if (function_exists('rules_invoke_component') && rules_config_load('rules_determine_verification_outcome')) {
    $result = rules_invoke_component('rules_determine_verification_outcome', $work_report);
    if (is_array($result)) {
      if (isset($result['count']) && is_numeric($result['count'])) {
        return (int) $result['count'];
      }
      foreach ($result as $value) {
        if (is_numeric($value) && (int) $value >= 0 && (int) $value <= 10) {
          return (int) $value;
        }
      }
    }
  }

  $count = 0;
  $items = field_get_items('node', $work_report, 'field_verification_slip');
  if (empty($items)) {
    return 0;
  }
  foreach ($items as $item) {
    if (empty($item['target_id'])) {
      continue;
    }
    $slip = node_load((int) $item['target_id']);
    if ($slip && $slip->type === 'verification' && (int) $slip->status === 1) {
      $count++;
    }
  }
  return $count;
}

/**
 * Whether the user already submitted a verification for this work report.
 *
 * @param int $wr_nid
 * @param int $verifier_uid
 *
 * @return bool
 */
function rovas_civic_user_verified_work_report($wr_nid, $verifier_uid) {
  $wr_nid = (int) $wr_nid;
  $verifier_uid = (int) $verifier_uid;
  if ($wr_nid < 1 || $verifier_uid < 1 || !field_info_field('field_verifier')) {
    return FALSE;
  }

  return (bool) db_query("
    SELECT 1
    FROM {node} v
    INNER JOIN {field_data_field_work_report} wr
      ON wr.entity_id = v.nid AND wr.entity_type = 'node' AND wr.deleted = 0
    INNER JOIN {field_data_field_verifier} vf
      ON vf.entity_id = v.nid AND vf.entity_type = 'node' AND vf.deleted = 0
    WHERE v.type = 'verification'
      AND v.status = 1
      AND wr.field_work_report_target_id = :wr
      AND vf.field_verifier_target_id = :uid
    LIMIT 1
  ", array(
    ':wr' => $wr_nid,
    ':uid' => $verifier_uid,
  ))->fetchField();
}

/**
 * Rules verifiability check for Civic Bearer user.
 *
 * @param object $work_report
 * @param int $verifier_uid
 *
 * @return bool
 */
function rovas_civic_wr_is_verifiable_by_user($work_report, $verifier_uid) {
  $verifier_uid = (int) $verifier_uid;
  if ($verifier_uid < 1) {
    return FALSE;
  }
  if ((int) $work_report->uid === $verifier_uid) {
    return FALSE;
  }

  if (!function_exists('rules_invoke_component') || !rules_config_load('rules_verifiability')) {
    return (bool) rovas_civic_verifier_assigned_slot($verifier_uid, (int) $work_report->nid);
  }

  $verifier = user_load($verifier_uid);
  if (!$verifier) {
    return FALSE;
  }

  $result = rules_invoke_component('rules_verifiability', $work_report, $verifier, FALSE);
  if (is_array($result)) {
    if (isset($result['verifiable'])) {
      return (bool) $result['verifiable'];
    }
    if (isset($result[0])) {
      return (bool) $result[0];
    }
    foreach ($result as $value) {
      if (is_bool($value) || is_numeric($value)) {
        return (bool) $value;
      }
    }
  }
  return (bool) $result;
}

/**
 * Create a verification node (approve/reject) for Civic.
 *
 * @param object $work_report
 * @param int $verifier_uid
 * @param bool $approve
 * @param float $verified_hours
 * @param string $note
 *
 * @return int
 *   verification nid or 0.
 */
function rovas_civic_create_verification_node($work_report, $verifier_uid, $approve, $verified_hours, $note = '') {
  $verifier_uid = (int) $verifier_uid;
  $verifier = user_load($verifier_uid);
  if (!$verifier) {
    return 0;
  }

  $lang = LANGUAGE_NONE;
  $verification = (object) array(
    'type' => 'verification',
    'uid' => $verifier_uid,
    'title' => 'Verification by Civic user',
    'status' => 1,
    'language' => LANGUAGE_NONE,
  );
  node_object_prepare($verification);

  $verification->field_work_report[$lang][0]['target_id'] = (int) $work_report->nid;
  if (field_info_field('field_verifier')) {
    $verification->field_verifier[$lang][0]['target_id'] = $verifier_uid;
  }
  if (field_info_field('field_verdict')) {
    $verification->field_verdict[$lang][0]['value'] = $approve ? 1 : 0;
  }
  if (field_info_field('field_verification_source')) {
    $verification->field_verification_source[$lang][0]['value'] = 1;
  }
  if (field_info_field('field_verified_time')) {
    $verification->field_verified_time[$lang][0]['value'] = (float) $verified_hours;
  }
  if ($note !== '' && field_info_field('field_verification_note')) {
    $verification->field_verification_note[$lang][0]['value'] = $note;
  }

  node_save($verification);
  return !empty($verification->nid) ? (int) $verification->nid : 0;
}

/**
 * Decline a voluntary verification slot (mirror node_form_decline).
 *
 * @param object $slot
 * @param object $work_report
 * @param int $verifier_uid
 * @param string $note
 *
 * @return array
 */
function rovas_civic_submit_verification_decline($slot, $work_report, $verifier_uid, $note = '') {
  $voluntary = FALSE;
  $vol_items = field_get_items('node', $slot, 'field_voluntary');
  if (!empty($vol_items[0]['value'])) {
    $voluntary = TRUE;
  }
  if (!$voluntary) {
    return rovas_civic_volunteer_error(403, 'cannot_decline', 'This verification cannot be declined.');
  }

  $lang = field_language('node', $slot, 'field_declined');
  if ($lang === NULL) {
    $lang = LANGUAGE_NONE;
  }
  $slot->field_declined[$lang][0]['value'] = 1;
  node_save($slot);

  if ($note !== '' && function_exists('rules_invoke_component') && rules_config_load('rules_send_email')) {
    $verifier = user_load((int) $verifier_uid);
    $author = user_load((int) $work_report->uid);
    if ($verifier && $author && !empty($author->mail)) {
      $replink = '/node/' . (int) $work_report->nid;
      $usrlink = '/user/' . (int) $verifier_uid;
      $message = t('Hello @user,', array('@user' => $author->name));
      $message .= '<br>';
      $message .= t('Recently we asked user <a href="@usrlink">@user</a> to verify your report <a href="@replink">@repname</a>.', array(
        '@user' => $verifier->name,
        '@usrlink' => $usrlink,
        '@repname' => check_plain($work_report->title),
        '@replink' => $replink,
      ));
      $message .= '<br>';
      $message .= t('The report requires expertise that @user does not have, and <strong>can therefore refuse to perform the verification, which they did.</strong>', array('@user' => $verifier->name));
      $message .= '<br>';
      $message .= t('@user left the following commentary when asked to verify the report:', array('@user' => $verifier->name));
      $message .= '<p>' . check_plain($note) . '</p>';
      $message .= t('If you need, you might still be able to <a href="@editlink">edit the work report</a>. In any case, we will try to find another verifier for your report.', array(
        '@editlink' => $replink . '/edit',
      ));
      $message .= '<br><br>';
      $message .= t('Thank you for your patience.');
      $wr_lang = !empty($work_report->language) ? $work_report->language : 'en';
      rules_invoke_component('rules_send_email', $author->mail, t('Your work report verification status'), $message, $wr_lang);
    }
  }

  return array(
    'ok' => TRUE,
    'work_report_nid' => (string) ((int) $work_report->nid),
    'verdict' => 'decline',
  );
}

/**
 * Project nid from a work report node.
 *
 * @param object $work_report
 *
 * @return int
 */
function rovas_civic_wr_project_nid($work_report) {
  $items = field_get_items('node', $work_report, 'field_project');
  return !empty($items[0]['target_id']) ? (int) $items[0]['target_id'] : 0;
}

/**
 * Proof web URLs with content-access token (cbt=).
 *
 * @param object $node
 *
 * @return array
 */
function rovas_civic_wr_web_urls($node) {
  $web_items = field_get_items('node', $node, 'field_web_address');
  if ($web_items === FALSE || empty($web_items)) {
    return array();
  }

  $token = '';
  $token_items = field_get_items('node', $node, 'field_content_access_token');
  if (!empty($token_items[0]['value'])) {
    $token = (string) $token_items[0]['value'];
  }

  $urls = array();
  foreach ($web_items as $item) {
    if (empty($item['value'])) {
      continue;
    }
    $web = trim((string) $item['value']);
    if ($web === '') {
      continue;
    }
    if ($token !== '') {
      $separator = (strpos($web, '?') !== FALSE) ? '&' : '?';
      $web .= $separator . 'cbt=' . rawurlencode($token);
    }
    $urls[] = $web;
  }
  return $urls;
}

/**
 * Export work_report field_location as GeoJSON geometry.
 *
 * @param object $node
 *
 * @return array|null
 */
function rovas_civic_wr_field_location_geojson($node) {
  if (!field_info_field('field_location')) {
    return NULL;
  }
  $items = field_get_items('node', $node, 'field_location');
  if (empty($items[0]['geom'])) {
    return NULL;
  }
  if (!function_exists('geophp_load')) {
    return NULL;
  }
  geophp_load();
  $geom = geoPHP::load($items[0]['geom']);
  if (!$geom || (method_exists($geom, 'isEmpty') && $geom->isEmpty())) {
    return NULL;
  }

  $json = $geom->out('json');
  if ($json === '') {
    return NULL;
  }
  $decoded = drupal_json_decode($json);
  if (!is_array($decoded)) {
    return NULL;
  }
  if (isset($decoded['type'], $decoded['coordinates'])) {
    return $decoded;
  }
  if (isset($decoded['geometry']) && is_array($decoded['geometry'])) {
    return $decoded['geometry'];
  }
  return NULL;
}

/**
 * Reward kind persisted by Civic create WR.
 *
 * @param int $wr_nid
 *
 * @return string
 *   merits|chrons
 */
function rovas_civic_wr_reward_kind($wr_nid) {
  $wr_nid = (int) $wr_nid;
  if ($wr_nid < 1 || !db_table_exists('civic_work_report_reward')) {
    return 'chrons';
  }
  $kind = db_query('SELECT reward_kind FROM {civic_work_report_reward} WHERE wr_nid = :nid', array(
    ':nid' => $wr_nid,
  ))->fetchField();
  return ($kind === 'merits') ? 'merits' : 'chrons';
}

/**
 * NACE tid + localized label for a work report.
 *
 * @param object $node
 * @param string|null $lang
 *
 * @return array
 *   keys: nace_tid (string|null), nace_label (string|null)
 */
function rovas_civic_wr_nace_fields($node, $lang = NULL) {
  $items = field_get_items('node', $node, 'field_classification');
  if (empty($items[0]['target_id'])) {
    return array(
      'nace_tid' => NULL,
      'nace_label' => NULL,
    );
  }

  $tid = (int) $items[0]['target_id'];
  $term = taxonomy_term_load($tid);
  if (!$term) {
    return array(
      'nace_tid' => (string) $tid,
      'nace_label' => NULL,
    );
  }

  $name = $term->name;
  if ($lang && function_exists('_rb_translate_term')) {
    $translated = _rb_translate_term($tid, $lang);
    if ($translated) {
      $name = $translated;
    }
  }
  $parsed = rovas_civic_nace_code_label($name);
  $label = $parsed[1] !== '' ? $parsed[1] : $name;

  return array(
    'nace_tid' => (string) $tid,
    'nace_label' => $label,
  );
}

/**
 * Unix timestamp from field_date on a work report.
 *
 * @param object $node
 *
 * @return int|null
 */
function rovas_civic_wr_date_started($node) {
  $items = field_get_items('node', $node, 'field_date');
  if (empty($items[0]['value'])) {
    return NULL;
  }
  $raw = $items[0]['value'];
  if (is_numeric($raw)) {
    $ts = (int) $raw;
    return $ts > 0 ? $ts : NULL;
  }
  $ts = (int) strtotime((string) $raw);
  return $ts > 0 ? $ts : NULL;
}
