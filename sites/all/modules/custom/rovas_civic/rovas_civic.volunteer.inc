<?php

/**
 * @file
 * Civic volunteer-report APIs (my recent projects, search, NACE, create project/WR).
 *
 * Loaded via Services resource 'file' keys. See docs/integration/civic.md
 * → Next work: volunteer report (Civic deferred form).
 */

/**
 * JSON error for Civic volunteer endpoints.
 *
 * @param int $http
 * @param string $error
 *   Machine code (not_shareholder, invalid_input, …).
 * @param string $message
 *
 * @return mixed
 *   Never returns — throws ServicesException.
 */
function rovas_civic_volunteer_error($http, $error, $message) {
  return services_error($message, (int) $http, array(
    'error' => $error,
    'message' => $message,
  ));
}

/**
 * Services callback: GET /rovas/civic_my_projects?lang=
 */
function rovas_civic_services_my_projects_index($lang = '') {
  $uid = rovas_civic_request_uid();
  if ($uid < 1) {
    return rovas_civic_volunteer_error(401, 'unauthorized', 'Authentication required');
  }
  return rovas_civic_my_projects($uid, rovas_civic_request_lang($lang));
}

/**
 * Up to ROVAS_CIVIC_MY_PROJECTS_LIMIT published projects where the user filed
 * work reports most recently (by work_report node created time).
 *
 * @param int $uid
 * @param string|null $lang
 *
 * @return array
 *   List of {project_nid, title, is_public, can_report}, newest report first.
 */
function rovas_civic_my_projects($uid, $lang = NULL) {
  $uid = (int) $uid;
  $limit = (int) ROVAS_CIVIC_MY_PROJECTS_LIMIT;
  if ($limit < 1) {
    $limit = 3;
  }

  $rows = db_query("
    SELECT p.field_project_target_id AS nid, MAX(n.created) AS last_reported
    FROM {node} n
    INNER JOIN {field_data_field_project} p
      ON p.entity_id = n.nid AND p.entity_type = 'node' AND p.deleted = 0
    INNER JOIN {node} proj
      ON proj.nid = p.field_project_target_id
      AND proj.type = 'project'
      AND proj.status = 1
    WHERE n.type = 'work_report'
      AND n.status = 1
      AND n.uid = :uid
    GROUP BY p.field_project_target_id
    ORDER BY last_reported DESC, nid DESC
    LIMIT " . $limit . "
  ", array(':uid' => $uid))->fetchAll();

  if (empty($rows)) {
    return array();
  }

  $nids = array();
  foreach ($rows as $row) {
    $nids[] = (int) $row->nid;
  }

  $nodes = node_load_multiple($nids);
  $out = array();
  foreach ($nids as $nid) {
    if (empty($nodes[$nid]) || $nodes[$nid]->type !== 'project' || (int) $nodes[$nid]->status !== 1) {
      continue;
    }
    $title = trim(rovas_civic_node_localized_title($nodes[$nid], $lang));
    if ($title === '') {
      $title = (string) $nodes[$nid]->title;
    }
    $out[] = array(
      'project_nid' => (string) $nid,
      'title' => $title,
      'is_public' => rovas_civic_project_is_public($nodes[$nid]),
      'can_report' => rovas_civic_user_can_report_on_project($uid, $nodes[$nid]),
    );
  }

  return $out;
}

/**
 * Services callback: GET /rovas/civic_projects_search?query=&lang=
 */
function rovas_civic_services_projects_search_index($query = '', $lang = '') {
  $uid = rovas_civic_request_uid();
  if ($uid < 1) {
    return rovas_civic_volunteer_error(401, 'unauthorized', 'Authentication required');
  }

  $query = trim((string) $query);
  if (drupal_strlen($query) < ROVAS_CIVIC_USERS_SEARCH_MIN_Q) {
    return rovas_civic_volunteer_error(400, 'invalid_input', 'Query must be at least 3 characters.');
  }

  return rovas_civic_projects_search($query, rovas_civic_request_lang($lang), $uid);
}

/**
 * Typeahead published projects by title.
 *
 * @param string $q
 * @param string|null $lang
 * @param int $uid
 *   Bearer user — used for can_report on each row.
 *
 * @return array
 */
function rovas_civic_projects_search($q, $lang = NULL, $uid = 0) {
  $like = '%' . db_like($q) . '%';
  $nids = db_query("
    SELECT n.nid
    FROM {node} n
    WHERE n.type = 'project'
      AND n.status = 1
      AND n.title LIKE :like
    ORDER BY n.title ASC
    LIMIT 10
  ", array(':like' => $like))->fetchCol();

  if (empty($nids)) {
    return array();
  }

  $nodes = node_load_multiple($nids);
  $out = array();
  foreach ($nids as $nid) {
    $nid = (int) $nid;
    if (empty($nodes[$nid])) {
      continue;
    }
    $title = trim(rovas_civic_node_localized_title($nodes[$nid], $lang));
    if ($title === '') {
      $title = (string) $nodes[$nid]->title;
    }
    $out[] = array(
      'project_nid' => (string) $nid,
      'title' => $title,
      'is_public' => rovas_civic_project_is_public($nodes[$nid]),
      'can_report' => rovas_civic_user_can_report_on_project($uid, $nodes[$nid]),
    );
  }
  return $out;
}

/**
 * Services callback: GET /rovas/civic_nace_tree?lang=
 */
function rovas_civic_services_nace_tree_index($lang = '') {
  $uid = rovas_civic_request_uid();
  if ($uid < 1) {
    return rovas_civic_volunteer_error(401, 'unauthorized', 'Authentication required');
  }
  return rovas_civic_nace_tree(rovas_civic_request_lang($lang));
}

/**
 * Nested NACE taxonomy (vocabulary machine name `tags`).
 *
 * @param string|null $lang
 *
 * @return array
 */
function rovas_civic_nace_tree($lang = NULL) {
  $vocab = taxonomy_vocabulary_machine_name_load('tags');
  if (!$vocab) {
    return array();
  }
  $terms = taxonomy_get_tree($vocab->vid);
  if (empty($terms)) {
    return array();
  }

  if (!$lang) {
    global $user;
    $lang = !empty($user->language) ? $user->language : 'en';
  }

  $items = array();
  foreach ($terms as $term) {
    $name = $term->name;
    if (function_exists('_rb_translate_term')) {
      $translated = _rb_translate_term($term->tid, $lang);
      if ($translated) {
        $name = $translated;
      }
    }
    $parsed = rovas_civic_nace_code_label($name);
    $parent = 0;
    if (!empty($term->parents[0])) {
      $parent = (int) $term->parents[0];
    }
    $items[(int) $term->tid] = array(
      'tid' => (string) ((int) $term->tid),
      'code' => $parsed[0],
      'label' => $parsed[1],
      'parent' => $parent,
      'children' => array(),
    );
  }

  $roots = array();
  foreach ($items as $tid => &$item) {
    $parent = $item['parent'];
    unset($item['parent']);
    if ($parent && isset($items[$parent])) {
      $items[$parent]['children'][] = &$item;
    }
    else {
      $roots[] = &$item;
    }
  }
  unset($item);

  return $roots;
}

/**
 * Split a NACE term name into a short code + label.
 *
 * Handles both dash-separated names ("F – Construction", "41.10 – …") and
 * space-separated names used on this site ("41.10 Development of …").
 *
 * @param string $name
 *
 * @return array
 *   array(code, label).
 */
function rovas_civic_nace_code_label($name) {
  $name = trim((string) $name);
  // "F – Construction" / "41.10 – Development..." (en-dash, hyphen, or colon).
  if (preg_match('/^([A-Z0-9][A-Z0-9.]{0,7})\s*[–\-:]+\s*(.+)$/u', $name, $m)) {
    return array($m[1], $m[2]);
  }
  // "F Construction" / "41 Construction…" / "41.10 Development…" / "43.21 …"
  if (preg_match('/^([A-Z]|[0-9]{1,2}(?:\.[0-9]{1,2}){0,2})\s+(.+)$/u', $name, $m)) {
    return array($m[1], $m[2]);
  }
  return array('', $name);
}

/**
 * Services callback: POST /rovas/civic_create_project
 *
 * @param array $data
 */
function rovas_civic_services_create_project($data) {
  $uid = rovas_civic_request_uid();
  if ($uid < 1) {
    return rovas_civic_volunteer_error(401, 'unauthorized', 'Authentication required');
  }
  if (!is_array($data)) {
    $data = array();
  }

  $name = isset($data['name']) ? trim((string) $data['name']) : '';
  $description = isset($data['description']) ? trim((string) $data['description']) : '';
  $url = isset($data['url']) ? trim((string) $data['url']) : '';
  if ($name === '' || $description === '') {
    return rovas_civic_volunteer_error(400, 'invalid_input', 'name and description are required.');
  }
  if (drupal_strlen($name) > 255) {
    return rovas_civic_volunteer_error(400, 'invalid_input', 'name is too long.');
  }

  $author = user_load($uid);
  if (!$author) {
    return rovas_civic_volunteer_error(401, 'unauthorized', 'Authentication required');
  }

  $nid = 0;
  if (function_exists('rules_invoke_component') && rules_config_load('rules_create_project')) {
    $result = rules_invoke_component(
      'rules_create_project',
      $name,
      $author,
      $url,
      0,
      1,
      1,
      $description,
      FALSE
    );
    $nid = rovas_civic_volunteer_nid_from_rules($result);
  }

  if ($nid < 1) {
    $node = new stdClass();
    $node->type = 'project';
    $node->title = $name;
    $node->language = !empty($author->language) ? $author->language : LANGUAGE_NONE;
    $node->uid = $uid;
    $node->status = 1;
    $node->body[LANGUAGE_NONE][0] = array(
      'value' => $description,
      'format' => 'filtered_html',
    );
    if (field_info_field('field_status')) {
      $node->field_status[LANGUAGE_NONE][0]['value'] = 1;
    }
    if (field_info_field('field_participation')) {
      $node->field_participation[LANGUAGE_NONE][0]['value'] = 1;
    }
    if (field_info_field('field_system_project')) {
      $node->field_system_project[LANGUAGE_NONE][0]['value'] = 0;
    }
    if ($url !== '' && field_info_field('field_asset_web_address')) {
      $node->field_asset_web_address[LANGUAGE_NONE][0]['value'] = $url;
    }
    node_object_prepare($node);
    node_save($node);
    $nid = (int) $node->nid;
  }

  if ($nid < 1) {
    return rovas_civic_volunteer_error(502, 'rovas_error', 'Failed to create project.');
  }

  // Ensure the creator can file work reports (public self-join / owner share).
  if (function_exists('rules_invoke_component') && rules_config_load('rules_check_or_add_shareholder')) {
    rules_invoke_component('rules_check_or_add_shareholder', $uid, $nid);
  }

  return array('project_nid' => (string) $nid);
}

/**
 * Pull a node nid out of a rules_invoke_component return value.
 *
 * @param mixed $result
 *
 * @return int
 */
function rovas_civic_volunteer_nid_from_rules($result) {
  if (is_object($result) && !empty($result->nid)) {
    return (int) $result->nid;
  }
  if (is_array($result)) {
    foreach ($result as $item) {
      if (is_object($item) && !empty($item->nid)) {
        return (int) $item->nid;
      }
      if (is_numeric($item) && (int) $item > 0) {
        return (int) $item;
      }
    }
  }
  if (is_numeric($result) && (int) $result > 0) {
    return (int) $result;
  }
  return 0;
}

/**
 * Services callback: POST /rovas/civic_create_work_report
 *
 * @param array $data
 */
function rovas_civic_services_create_work_report($data) {
  $uid = rovas_civic_request_uid();
  if ($uid < 1) {
    return rovas_civic_volunteer_error(401, 'unauthorized', 'Authentication required');
  }
  if (!is_array($data)) {
    $data = array();
  }

  $project_nid = isset($data['project_nid']) ? (int) $data['project_nid'] : 0;
  $date_started = isset($data['date_started']) ? (int) $data['date_started'] : 0;
  $hours = isset($data['hours']) ? (float) $data['hours'] : 0;
  $description = isset($data['description']) ? trim((string) $data['description']) : '';
  $title = isset($data['title']) ? trim((string) $data['title']) : '';
  $nace_tid = isset($data['nace_tid']) ? (int) $data['nace_tid'] : 0;
  $web_url = isset($data['web_url']) ? trim((string) $data['web_url']) : '';
  $reward_kind = isset($data['reward_kind']) ? drupal_strtolower(trim((string) $data['reward_kind'])) : 'chrons';
  $photos = isset($data['photos']) && is_array($data['photos']) ? $data['photos'] : array();

  if ($reward_kind !== 'merits' && $reward_kind !== 'chrons') {
    $reward_kind = 'chrons';
  }
  if ($project_nid < 1 || $hours <= 0 || $description === '' || $nace_tid < 1) {
    return rovas_civic_volunteer_error(400, 'invalid_input', 'project_nid, hours, description, and nace_tid are required.');
  }
  if ($date_started < 1) {
    return rovas_civic_volunteer_error(400, 'invalid_input', 'date_started must be a unix timestamp.');
  }
  if ($web_url === '' && empty($photos)) {
    return rovas_civic_volunteer_error(400, 'invalid_input', 'Provide a web_url and/or photos as proof of work.');
  }
  if ($title === '') {
    $title = drupal_substr(preg_replace('/\s+/u', ' ', $description), 0, 80);
  }

  $author = user_load($uid);
  if (!$author) {
    return rovas_civic_volunteer_error(401, 'unauthorized', 'Authentication required');
  }
  if (!empty($author->created) && (int) $author->created > $date_started) {
    return rovas_civic_volunteer_error(400, 'date_before_registration', 'Work date is before the Rovas account was created.');
  }

  $project = rovas_civic_load_published_project($project_nid);
  if (!$project) {
    return rovas_civic_volunteer_error(404, 'project_not_found', 'Project not found.');
  }

  $term = taxonomy_term_load($nace_tid);
  if (!$term) {
    return rovas_civic_volunteer_error(400, 'invalid_input', 'Unknown nace_tid.');
  }

  if (rovas_civic_pending_verify_slot_count($uid) > 0) {
    return rovas_civic_volunteer_error(403, 'pending_verifications', 'Verify outstanding work reports before filing another.');
  }

  $share = 0;
  if (function_exists('rules_invoke_component') && rules_config_load('rules_check_or_add_shareholder')) {
    $share_result = rules_invoke_component('rules_check_or_add_shareholder', $uid, $project_nid);
    $share = rovas_civic_volunteer_int_from_rules($share_result);
  }
  if ($share === 0) {
    return rovas_civic_volunteer_error(403, 'not_shareholder', 'This project is private; the owner must add you as a shareholder.');
  }
  if ($share < 0) {
    return rovas_civic_volunteer_error(404, 'project_not_found', 'Project not found.');
  }

  $lang = !empty($author->language) ? $author->language : LANGUAGE_NONE;
  $evidence = ($web_url !== '') ? array('1') : array();
  $wr_nid = 0;

  if (function_exists('rules_invoke_component') && rules_config_load('rules_create_work_report')) {
    $result = rules_invoke_component(
      'rules_create_work_report',
      $title,
      $web_url,
      $hours,
      $project,
      $description,
      $evidence,
      FALSE,
      0,
      '',
      $lang,
      $date_started,
      1,
      $author,
      $term
    );
    $wr_nid = rovas_civic_volunteer_wr_nid_from_rules($result);
    $rule_code = rovas_civic_volunteer_int_from_rules($result);
    if ($wr_nid < 1 && $rule_code === -3) {
      return rovas_civic_volunteer_error(400, 'date_before_registration', 'Work date is before the Rovas account was created.');
    }
  }

  if ($wr_nid < 1) {
    return rovas_civic_volunteer_error(502, 'rovas_error', 'Failed to create work report.');
  }

  $wr = node_load($wr_nid);
  if ($wr && !empty($photos)) {
    rovas_civic_attach_wr_photos($wr, $photos);
  }

  db_merge('civic_work_report_reward')
    ->key(array('wr_nid' => $wr_nid))
    ->fields(array(
      'reward_kind' => $reward_kind,
      'uid' => $uid,
      'created' => REQUEST_TIME,
    ))
    ->execute();

  return array(
    'work_report_nid' => (string) $wr_nid,
    'reward_kind' => $reward_kind,
  );
}

/**
 * Count verification slots with an assigned report owned by the user.
 *
 * Empty obligation slots (no field_report_to_verify) are not actionable and
 * must not block new work-report filings.
 *
 * @param int $uid
 *
 * @return int
 */
function rovas_civic_pending_verify_slot_count($uid) {
  $uid = (int) $uid;
  if ($uid < 1 || !field_info_field('field_slip_owner') || !field_info_field('field_report_to_verify')) {
    return 0;
  }
  $sql = "
    SELECT COUNT(n.nid)
    FROM {node} n
    INNER JOIN {field_data_field_slip_owner} o
      ON o.entity_id = n.nid AND o.entity_type = 'node' AND o.deleted = 0
    INNER JOIN {field_data_field_report_to_verify} r
      ON r.entity_id = n.nid AND r.entity_type = 'node' AND r.deleted = 0
    WHERE n.type = 'verification_slot'
      AND n.status = 1
      AND o.field_slip_owner_target_id = :uid
      AND r.field_report_to_verify_target_id IS NOT NULL
      AND r.field_report_to_verify_target_id > 0
  ";
  return (int) db_query($sql, array(':uid' => $uid))->fetchField();
}

/**
 * Integer from a rules_invoke_component return (provided `result`).
 *
 * @param mixed $result
 *
 * @return int
 */
function rovas_civic_volunteer_int_from_rules($result) {
  if (is_int($result) || is_float($result) || (is_string($result) && is_numeric($result))) {
    return (int) $result;
  }
  if (is_array($result)) {
    if (isset($result['result']) && is_numeric($result['result'])) {
      return (int) $result['result'];
    }
    foreach ($result as $item) {
      if (is_numeric($item)) {
        return (int) $item;
      }
    }
  }
  return 0;
}

/**
 * Work-report nid from rules_create_work_report return.
 *
 * @param mixed $result
 *
 * @return int
 */
function rovas_civic_volunteer_wr_nid_from_rules($result) {
  $nid = rovas_civic_volunteer_nid_from_rules($result);
  if ($nid > 0) {
    return $nid;
  }
  if (is_array($result)) {
    foreach (array('created_wr', 'created-wr', 'created_work_report') as $key) {
      if (!empty($result[$key]) && is_object($result[$key]) && !empty($result[$key]->nid)) {
        return (int) $result[$key]->nid;
      }
    }
  }
  return 0;
}

/**
 * Attach Civic-uploaded photos to field_photos.
 *
 * @param object $node
 * @param array $photos
 */
function rovas_civic_attach_wr_photos($node, array $photos) {
  if (!field_info_field('field_photos')) {
    return;
  }
  $dir = 'public://civic_wr';
  file_prepare_directory($dir, FILE_CREATE_DIRECTORY);
  $added = 0;
  foreach ($photos as $photo) {
    if ($added >= 5) {
      break;
    }
    if (!is_array($photo) || empty($photo['data'])) {
      continue;
    }
    $bin = base64_decode((string) $photo['data'], TRUE);
    if ($bin === FALSE || strlen($bin) < 32) {
      continue;
    }
    if (strlen($bin) > 4 * 1024 * 1024) {
      continue;
    }
    $filename = isset($photo['filename']) ? (string) $photo['filename'] : 'photo.jpg';
    $filename = preg_replace('/[^a-zA-Z0-9._-]/', '_', $filename);
    if ($filename === '' || $filename === '_') {
      $filename = 'photo.jpg';
    }
    $uri = file_unmanaged_save_data($bin, $dir . '/' . $filename, FILE_EXISTS_RENAME);
    if (!$uri) {
      continue;
    }
    $file = new stdClass();
    $file->uri = $uri;
    $file->filename = drupal_basename($uri);
    $file->filemime = !empty($photo['mime']) ? (string) $photo['mime'] : file_get_mimetype($uri);
    $file->uid = $node->uid;
    $file->status = FILE_STATUS_PERMANENT;
    file_save($file);
    if (empty($file->fid)) {
      continue;
    }
    $node->field_photos[LANGUAGE_NONE][] = array(
      'fid' => $file->fid,
      'display' => 1,
      'description' => '',
    );
    file_usage_add($file, 'rovas_civic', 'node', $node->nid);
    $added++;
  }
  if ($added > 0) {
    node_save($node);
  }
}
