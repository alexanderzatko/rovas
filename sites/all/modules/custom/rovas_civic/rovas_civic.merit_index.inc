<?php

/**
 * @file
 * Civic Merit Index (Prínos / ranks) — GET /rovas/civic_merit_index.
 *
 * Mirrors Views users_merit_rank without views_get_view(); see civic.md.
 */

/**
 * Services callback: GET /rovas/civic_merit_index.
 */
function rovas_civic_services_merit_index_index(
  $limit = ROVAS_CIVIC_MERIT_INDEX_DEFAULT_LIMIT,
  $offset = 0,
  $sort = 'total_me',
  $order = 'desc',
  $country_iso2 = '',
  $since_from = '',
  $min_me = NULL,
  $max_me = NULL,
  $min_value_eur = NULL,
  $max_value_eur = NULL,
  $nace_section_tid = NULL,
  $source_category = '',
  $lang = ''
) {
  $pager = rovas_civic_normalize_pager(
    $limit,
    $offset,
    ROVAS_CIVIC_MERIT_INDEX_DEFAULT_LIMIT,
    ROVAS_CIVIC_MERIT_INDEX_MAX_LIMIT
  );

  $sort = drupal_strtolower(trim((string) $sort));
  if ($sort === '') {
    $sort = 'total_me';
  }
  $allowed_sort = array(
    'position',
    'total_me',
    'value_eur',
    'member_since',
    'country',
    'taxes_me',
    'charity_me',
    'volunteer_me',
  );
  if (!in_array($sort, $allowed_sort, TRUE)) {
    return services_error(t('Invalid sort parameter.'), 400);
  }

  $order = drupal_strtolower(trim((string) $order));
  if ($order === '') {
    $order = 'desc';
  }
  if ($order !== 'asc' && $order !== 'desc') {
    return services_error(t('Invalid order parameter; use asc or desc.'), 400);
  }

  $since_ts = NULL;
  $since_from = trim((string) $since_from);
  if ($since_from !== '') {
    if (!preg_match('/^\d{4}-\d{2}-\d{2}$/', $since_from)) {
      return services_error(t('Invalid since_from; use YYYY-MM-DD.'), 400);
    }
    $since_ts = strtotime($since_from . ' 00:00:00 UTC');
    if ($since_ts === FALSE) {
      return services_error(t('Invalid since_from; use YYYY-MM-DD.'), 400);
    }
  }

  $source_category = drupal_strtolower(trim((string) $source_category));
  if ($source_category !== '') {
    $allowed_source = array('taxes', 'charity', 'volunteer');
    if (!in_array($source_category, $allowed_source, TRUE)) {
      return services_error(t('Invalid source_category; use taxes, charity, or volunteer.'), 400);
    }
    // When filtering by ME source, default sort to that category unless caller chose another column.
    if ($sort === 'total_me') {
      $sort = $source_category . '_me';
      if ($order === '') {
        $order = 'desc';
      }
    }
  }

  $nace_section_tid = ($nace_section_tid === NULL || $nace_section_tid === '')
    ? 0
    : (int) $nace_section_tid;

  return rovas_civic_merit_index_page(array(
    'limit' => $pager['limit'],
    'offset' => $pager['offset'],
    'sort' => $sort,
    'order' => $order,
    'country_iso2' => trim((string) $country_iso2),
    'since_ts' => $since_ts,
    'min_me' => rovas_civic_merit_index_optional_float($min_me),
    'max_me' => rovas_civic_merit_index_optional_float($max_me),
    'min_value_eur' => rovas_civic_merit_index_optional_float($min_value_eur),
    'max_value_eur' => rovas_civic_merit_index_optional_float($max_value_eur),
    'nace_section_tid' => $nace_section_tid,
    'source_category' => $source_category,
    'lang' => rovas_civic_request_lang($lang),
  ));
}

/**
 * Parse optional numeric filter param.
 *
 * @param mixed $value
 *
 * @return float|null
 */
function rovas_civic_merit_index_optional_float($value) {
  if ($value === NULL || $value === '') {
    return NULL;
  }
  if (!is_numeric($value)) {
    return NULL;
  }
  return (float) $value;
}

/**
 * Build paginated Merit Index response.
 *
 * @param array $opts
 *   Filter/sort/pagination options from the Services callback.
 *
 * @return array
 */
function rovas_civic_merit_index_page(array $opts) {
  $hourly_rate = rovas_civic_national_hourly_rate(ROVAS_CIVIC_DEFAULT_COUNTRY);
  $rows = rovas_civic_merit_index_base_rows();
  if (empty($rows)) {
    return rovas_civic_merit_index_response($opts, 0, array());
  }

  $uids = array();
  foreach ($rows as $row) {
    $uids[] = (int) $row->uid;
  }
  $expertise = rovas_civic_merit_index_expertise_by_uid($uids);

  $country_filter = drupal_strtoupper(trim((string) $opts['country_iso2']));
  $items = array();
  foreach ($rows as $row) {
    $uid = (int) $row->uid;
    $total_me = round((float) $row->total_me, 2);
    $value_eur = round(($total_me / ROVAS_CIVIC_ME_PER_HOUR) * $hourly_rate, 2);

    if ($country_filter !== '') {
      $row_iso = drupal_strtoupper(trim((string) $row->country_iso2));
      if ($row_iso !== $country_filter) {
        continue;
      }
    }
    if (!empty($opts['since_ts']) && (int) $row->created < (int) $opts['since_ts']) {
      continue;
    }
    if ($opts['min_me'] !== NULL && $total_me < $opts['min_me']) {
      continue;
    }
    if ($opts['max_me'] !== NULL && $total_me > $opts['max_me']) {
      continue;
    }
    if ($opts['min_value_eur'] !== NULL && $value_eur < $opts['min_value_eur']) {
      continue;
    }
    if ($opts['max_value_eur'] !== NULL && $value_eur > $opts['max_value_eur']) {
      continue;
    }

    $by_category = isset($expertise[$uid]['by_category'])
      ? $expertise[$uid]['by_category']
      : array('taxes' => 0.0, 'charity' => 0.0, 'volunteer' => 0.0);

    if (!empty($opts['source_category'])) {
      $cat = $opts['source_category'];
      if (empty($by_category[$cat]) || (float) $by_category[$cat] <= 0) {
        continue;
      }
    }

    if (!empty($opts['nace_section_tid'])) {
      $sections = isset($expertise[$uid]['sections']) ? $expertise[$uid]['sections'] : array();
      if (empty($sections[(int) $opts['nace_section_tid']])) {
        continue;
      }
    }

    $username = (string) $row->name;
    $display_name = $username;
    if (!empty($row->full_name)) {
      $display_name = (string) $row->full_name;
    }

    $country_iso2 = drupal_strtoupper(trim((string) $row->country_iso2));
    $items[] = array(
      'rovas_uid' => (string) $uid,
      'username' => $username,
      'display_name' => $display_name,
      'member_since_year' => (int) date('Y', (int) $row->created),
      'created' => (int) $row->created,
      'total_me' => $total_me,
      'value_eur' => $value_eur,
      'country_iso2' => $country_iso2,
      'country_label' => rovas_civic_merit_index_country_label($country_iso2, $opts['lang']),
      'by_category' => array(
        'taxes' => round($by_category['taxes'], 2),
        'charity' => round($by_category['charity'], 2),
        'volunteer' => round($by_category['volunteer'], 2),
      ),
      'nace_expertise' => array(),
    );
  }

  usort($items, function ($a, $b) use ($opts) {
    return rovas_civic_merit_index_compare_rows($a, $b, $opts['sort'], $opts['order']);
  });

  $total = count($items);
  $slice = array_slice($items, (int) $opts['offset'], (int) $opts['limit']);

  $lang = $opts['lang'];
  foreach ($slice as $i => &$item) {
    $uid = (int) $item['rovas_uid'];
    if (function_exists('rovas_meritscope_user_expertise_json')) {
      $item['nace_expertise'] = rovas_meritscope_user_expertise_json($uid, $lang, 5);
    }
    unset($item['created']);
    $item['position'] = (int) $opts['offset'] + $i + 1;
  }
  unset($item);

  return rovas_civic_merit_index_response($opts, $total, $slice);
}

/**
 * Base SQL rows for eligible Merit Index users.
 *
 * @return array
 *   stdClass rows: uid, name, created, total_me, country_iso2, full_name.
 */
function rovas_civic_merit_index_base_rows() {
  $has_full_name = (bool) field_info_field('field_full_name');
  $has_country = (bool) field_info_field('field_country');

  $sql = "
    SELECT u.uid AS uid,
      u.name AS name,
      u.created AS created,
      (IFNULL(pt.points, 0) + IFNULL(d.field_merit_decimal_part_value, 0)) AS total_me";
  if ($has_country) {
    $sql .= ",
      c.field_country_iso2 AS country_iso2";
  }
  else {
    $sql .= ",
      '' AS country_iso2";
  }
  if ($has_full_name) {
    $sql .= ",
      fn.field_full_name_value AS full_name";
  }
  else {
    $sql .= ",
      '' AS full_name";
  }
  $sql .= "
    FROM {users} u
    LEFT JOIN {userpoints_total} pt ON u.uid = pt.uid
    LEFT JOIN {field_data_field_merit_decimal_part} d
      ON u.uid = d.entity_id
      AND d.entity_type = 'user'
      AND d.deleted = 0";
  if ($has_country) {
    $sql .= "
    LEFT JOIN {field_data_field_country} c
      ON c.entity_id = u.uid
      AND c.entity_type = 'user'
      AND c.deleted = 0
      AND c.delta = 0";
  }
  if ($has_full_name) {
    $sql .= "
    LEFT JOIN {field_data_field_full_name} fn
      ON fn.entity_id = u.uid
      AND fn.entity_type = 'user'
      AND fn.deleted = 0
      AND fn.delta = 0";
  }
  $sql .= "
    WHERE u.status = 1
      AND u.uid > 1
    HAVING total_me >= 0
    ORDER BY total_me DESC";

  return db_query($sql)->fetchAll();
}

/**
 * Batch-load ME-by-category and top-level NACE sections per uid.
 *
 * @param array $uids
 *
 * @return array
 *   uid => { by_category: {taxes,charity,volunteer}, sections: { section_tid: TRUE } }.
 */
function rovas_civic_merit_index_expertise_by_uid(array $uids) {
  if (module_exists('rovas_merit') && field_info_field('field_merit_source')) {
    module_load_include('inc', 'rovas_merit', 'rovas_merit.ledger');
    return rovas_merit_users_lookup_data_batch($uids);
  }

  $out = array();
  $uids = array_values(array_unique(array_filter(array_map('intval', $uids))));
  if (empty($uids)) {
    return $out;
  }

  $fallback = (int) ROVAS_CIVIC_FALLBACK_NACE_TID;
  $charity_nace = (int) ROVAS_CIVIC_CHARITY_NACE_TID;

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
      AND m.field_merits_value > 0
    GROUP BY e.field_expert_target_id, c.field_category_tid", array(
    ':uids' => $uids,
  ));

  foreach ($uids as $uid) {
    $out[$uid] = array(
      'by_category' => array('taxes' => 0.0, 'charity' => 0.0, 'volunteer' => 0.0),
      'by_source' => array(),
      'sections' => array(),
    );
  }

  foreach ($result as $row) {
    $uid = (int) $row->uid;
    $tid = (int) $row->tid;
    $me = (float) $row->me;
    if ($uid < 1 || $tid < 1 || $me <= 0 || empty($out[$uid])) {
      continue;
    }

    if ($tid === $fallback) {
      $out[$uid]['by_category']['taxes'] += $me;
    }
    elseif ($charity_nace > 0 && $tid === $charity_nace) {
      $out[$uid]['by_category']['charity'] += $me;
    }
    else {
      $out[$uid]['by_category']['volunteer'] += $me;
    }

    $section_tid = rovas_civic_merit_index_nace_section_tid($tid);
    if ($section_tid > 0) {
      $out[$uid]['sections'][$section_tid] = TRUE;
    }
  }

  return $out;
}

/**
 * Top-level NACE letter-section tid for a category tid.
 *
 * Same ancestor depth as rovas_meritscope_user_expertise_json() section_tid.
 *
 * @param int $tid
 *
 * @return int
 */
function rovas_civic_merit_index_nace_section_tid($tid) {
  $tid = (int) $tid;
  if ($tid < 1) {
    return 0;
  }
  $cat = taxonomy_get_parents_all($tid);
  if (empty($cat)) {
    return 0;
  }
  $catindex = count($cat) > 1 ? count($cat) - 2 : 0;
  return isset($cat[$catindex]->tid) ? (int) $cat[$catindex]->tid : 0;
}

/**
 * Services callback: GET /rovas/civic_merit_index_countries.
 */
function rovas_civic_services_merit_index_countries_index($lang = '') {
  return rovas_civic_merit_index_countries(rovas_civic_request_lang($lang));
}

/**
 * Distinct countries for Merit Index users with total_me > 0.
 *
 * @param string|null $lang
 *
 * @return array
 *   { countries: [ { iso2, label, user_count }, … ] } sorted by label asc.
 */
function rovas_civic_merit_index_countries($lang = NULL) {
  $rows = rovas_civic_merit_index_base_rows();
  $counts = array();
  foreach ($rows as $row) {
    if ((float) $row->total_me <= 0) {
      continue;
    }
    $iso2 = drupal_strtoupper(trim((string) $row->country_iso2));
    if ($iso2 === '' || strlen($iso2) !== 2) {
      continue;
    }
    if (!isset($counts[$iso2])) {
      $counts[$iso2] = 0;
    }
    $counts[$iso2]++;
  }

  $countries = array();
  foreach ($counts as $iso2 => $user_count) {
    $countries[] = array(
      'iso2' => $iso2,
      'label' => rovas_civic_merit_index_country_label($iso2, $lang),
      'user_count' => (int) $user_count,
    );
  }

  usort($countries, function ($a, $b) {
    return strcasecmp($a['label'], $b['label']);
  });

  return array('countries' => $countries);
}

/**
 * Compare two Merit Index rows for sorting.
 */
function rovas_civic_merit_index_compare_rows(array $a, array $b, $sort, $order) {
  $sort = ($sort === 'position') ? 'total_me' : $sort;
  $dir = ($order === 'asc') ? 1 : -1;

  switch ($sort) {
    case 'value_eur':
      $va = (float) $a['value_eur'];
      $vb = (float) $b['value_eur'];
      break;

    case 'member_since':
      $va = (int) $a['created'];
      $vb = (int) $b['created'];
      break;

    case 'country':
      $va = (string) $a['country_iso2'];
      $vb = (string) $b['country_iso2'];
      break;

    case 'taxes_me':
      $va = (float) $a['by_category']['taxes'];
      $vb = (float) $b['by_category']['taxes'];
      break;

    case 'charity_me':
      $va = (float) $a['by_category']['charity'];
      $vb = (float) $b['by_category']['charity'];
      break;

    case 'volunteer_me':
      $va = (float) $a['by_category']['volunteer'];
      $vb = (float) $b['by_category']['volunteer'];
      break;

    case 'total_me':
    default:
      $va = (float) $a['total_me'];
      $vb = (float) $b['total_me'];
      break;
  }

  if ($va == $vb) {
    $tie = strcasecmp((string) $a['username'], (string) $b['username']);
    return $tie * $dir;
  }
  if ($sort === 'country') {
    return ($va < $vb ? -1 : 1) * $dir;
  }
  return ($va < $vb ? -1 : 1) * $dir;
}

/**
 * Localized country label for Merit Index rows.
 */
function rovas_civic_merit_index_country_label($iso2, $lang = NULL) {
  $iso2 = drupal_strtoupper(trim((string) $iso2));
  if ($iso2 === '') {
    return '';
  }
  if (function_exists('_addressfield_country_options_list')) {
    $countries = _addressfield_country_options_list();
    if (isset($countries[$iso2])) {
      return (string) $countries[$iso2];
    }
  }
  return $iso2;
}

/**
 * Wrap list payload with pagination metadata.
 */
function rovas_civic_merit_index_response(array $opts, $total, array $items) {
  return array(
    'total' => (int) $total,
    'limit' => (int) $opts['limit'],
    'offset' => (int) $opts['offset'],
    'sort' => $opts['sort'],
    'order' => $opts['order'],
    'items' => $items,
  );
}
