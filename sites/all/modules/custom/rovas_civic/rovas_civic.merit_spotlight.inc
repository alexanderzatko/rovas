<?php

/**
 * @file
 * Civic Merit spotlight — GET /rovas/civic_merit_spotlight.
 *
 * Period leads by Civic area (taxes / charity / volunteer). See civic.md.
 */

define('ROVAS_CIVIC_MERIT_SPOTLIGHT_TZ', 'Europe/Bratislava');

/**
 * Services callback: GET /rovas/civic_merit_spotlight.
 */
function rovas_civic_services_merit_spotlight_index($period = '', $country_iso2 = '', $lang = '') {
  // lang accepted for Civic consistency; unused in this payload.
  unset($lang);

  $period = drupal_strtolower(trim((string) $period));
  $allowed = array('today', 'day', 'week', 'month');
  if (!in_array($period, $allowed, TRUE)) {
    return services_error(t('Invalid period; use today, day, week, or month.'), 400);
  }

  $country = strtoupper(trim((string) $country_iso2));
  if ($country !== '' && !preg_match('/^[A-Z]{2}$/', $country)) {
    return services_error(t('Invalid country_iso2; use ISO-2.'), 400);
  }
  $country_filter = ($country === '') ? NULL : $country;

  $window = rovas_civic_merit_spotlight_period_window($period);
  if ($window === FALSE) {
    return services_error(t('Could not resolve period window.'), 503);
  }

  try {
    $agg = rovas_civic_merit_spotlight_aggregate(
      $window['start_ts'],
      $window['end_excl_ts'],
      $country_filter
    );
  }
  catch (Exception $e) {
    watchdog('rovas_civic', 'Merit spotlight ledger query failed: @m', array(
      '@m' => $e->getMessage(),
    ), WATCHDOG_ERROR);
    return services_error(t('Merit spotlight temporarily unavailable.'), 503);
  }

  $caller_uid = rovas_civic_request_uid();
  $leads = rovas_civic_merit_spotlight_leads($agg['by_uid'], $caller_uid);
  $response = array(
    'period' => $period,
    'period_start' => $window['period_start'],
    'period_end' => $window['period_end'],
    'country_iso2' => $country_filter,
    'community_earned_me' => round((float) $agg['community_earned_me'], 2),
    'active_earners' => (int) $agg['active_earners'],
    'leads' => $leads,
  );

  if ($caller_uid > 0) {
    $response['you'] = rovas_civic_merit_spotlight_you($caller_uid, $window, $leads);
  }

  return $response;
}

/**
 * Inclusive calendar dates + unix bounds for a spotlight period.
 *
 * Timezone: Europe/Bratislava.
 *
 * - today: current calendar day so far
 * - day: previous calendar day (yesterday)
 * - week: last 7 calendar days ending yesterday (inclusive)
 * - month: previous calendar month
 *
 * @param string $period
 *   today|day|week|month.
 *
 * @return array|false
 *   keys: period_start, period_end (Y-m-d), start_ts, end_excl_ts, today_start_ts,
 *   today_end_excl_ts; FALSE on failure.
 */
function rovas_civic_merit_spotlight_period_window($period) {
  try {
    $tz = new DateTimeZone(ROVAS_CIVIC_MERIT_SPOTLIGHT_TZ);
    $now = new DateTime('now', $tz);
  }
  catch (Exception $e) {
    return FALSE;
  }

  $today_start = clone $now;
  $today_start->setTime(0, 0, 0);

  $yesterday_start = clone $today_start;
  $yesterday_start->modify('-1 day');

  if ($period === 'today') {
    $start = clone $today_start;
    $end_day = clone $today_start;
  }
  elseif ($period === 'day') {
    $start = clone $yesterday_start;
    $end_day = clone $yesterday_start;
  }
  elseif ($period === 'week') {
    // Seven days ending yesterday inclusive: yesterday-6 … yesterday.
    $end_day = clone $yesterday_start;
    $start = clone $yesterday_start;
    $start->modify('-6 days');
  }
  elseif ($period === 'month') {
    $first_of_this_month = clone $today_start;
    $first_of_this_month->modify('first day of this month');
    $end_day = clone $first_of_this_month;
    $end_day->modify('-1 day');
    $start = clone $end_day;
    $start->modify('first day of this month');
  }
  else {
    return FALSE;
  }

  $end_excl = clone $end_day;
  $end_excl->modify('+1 day');

  $today_end_excl = clone $today_start;
  $today_end_excl->modify('+1 day');

  return array(
    'period_start' => $start->format('Y-m-d'),
    'period_end' => $end_day->format('Y-m-d'),
    'start_ts' => (int) $start->format('U'),
    'end_excl_ts' => (int) $end_excl->format('U'),
    'today_start_ts' => (int) $today_start->format('U'),
    'today_end_excl_ts' => (int) $today_end_excl->format('U'),
  );
}

/**
 * Aggregate positive period Merit credits per uid × category.
 *
 * Excludes blocked/inactive users (users.status = 1) and lifetime
 * total_me < 0 — same hide rules as Merit Index. Optional country filter
 * uses field_country (same source as Merit Index).
 *
 * @param int $start_ts
 * @param int $end_excl_ts
 * @param string|null $country_iso2
 *
 * @return array
 *   keys: by_uid (uid => {by_category, earned_me, country_iso2}),
 *   community_earned_me, active_earners.
 */
function rovas_civic_merit_spotlight_aggregate($start_ts, $end_excl_ts, $country_iso2 = NULL) {
  $start_ts = (int) $start_ts;
  $end_excl_ts = (int) $end_excl_ts;

  $has_country = (bool) field_info_field('field_country');
  $has_decimal = (bool) field_info_field('field_merit_decimal_part');

  $sql = "
    SELECT rec.field_reward_recipient_target_id AS uid,
      n.type AS node_type,
      MAX(src.field_merit_source_value) AS merit_source,
      SUM(amt.field_reward_amount_value) AS earned_me";
  if ($has_country) {
    $sql .= ",
      MAX(c.field_country_iso2) AS country_iso2";
  }
  else {
    $sql .= ",
      '' AS country_iso2";
  }
  $sql .= "
    FROM {node} n
    INNER JOIN {field_data_field_reward_recipient} rec
      ON rec.entity_id = n.nid AND rec.entity_type = 'node' AND rec.deleted = 0
    INNER JOIN {users} u
      ON u.uid = rec.field_reward_recipient_target_id
      AND u.status = 1
    INNER JOIN {field_data_field_reward_amount} amt
      ON amt.entity_id = n.nid AND amt.entity_type = 'node' AND amt.deleted = 0
    LEFT JOIN {field_data_field_merit_source} src
      ON src.entity_id = n.nid AND src.entity_type = 'node' AND src.deleted = 0
    LEFT JOIN {userpoints_total} pt
      ON pt.uid = rec.field_reward_recipient_target_id";
  if ($has_decimal) {
    $sql .= "
    LEFT JOIN {field_data_field_merit_decimal_part} d
      ON d.entity_id = rec.field_reward_recipient_target_id
      AND d.entity_type = 'user'
      AND d.deleted = 0";
  }
  if ($has_country) {
    $sql .= "
    LEFT JOIN {field_data_field_country} c
      ON c.entity_id = rec.field_reward_recipient_target_id
      AND c.entity_type = 'user'
      AND c.deleted = 0
      AND c.delta = 0";
  }
  $sql .= "
    WHERE n.status = 1
      AND n.type IN ('merits_disbursement', 'merit_penalty')
      AND n.created >= :start_ts
      AND n.created < :end_excl_ts
      AND amt.field_reward_amount_value > 0
      AND rec.field_reward_recipient_target_id > 1
      AND (IFNULL(pt.points, 0)";
  if ($has_decimal) {
    $sql .= " + IFNULL(d.field_merit_decimal_part_value, 0)";
  }
  $sql .= ") >= 0";

  $args = array(
    ':start_ts' => $start_ts,
    ':end_excl_ts' => $end_excl_ts,
  );
  if ($country_iso2 !== NULL) {
    if (!$has_country) {
      return array(
        'by_uid' => array(),
        'community_earned_me' => 0.0,
        'active_earners' => 0,
      );
    }
    $sql .= "
      AND UPPER(c.field_country_iso2) = :country";
    $args[':country'] = strtoupper($country_iso2);
  }

  $sql .= "
    GROUP BY rec.field_reward_recipient_target_id, n.type, src.field_merit_source_value";

  $rows = db_query($sql, $args)->fetchAll();

  $by_uid = array();
  foreach ($rows as $row) {
    $uid = (int) $row->uid;
    if ($uid < 1) {
      continue;
    }
    $category = rovas_civic_merit_spotlight_source_category(
      isset($row->merit_source) ? $row->merit_source : NULL,
      isset($row->node_type) ? $row->node_type : ''
    );
    if ($category === NULL) {
      continue;
    }
    $earned = (float) $row->earned_me;
    if ($earned <= 0) {
      continue;
    }
    if (!isset($by_uid[$uid])) {
      $by_uid[$uid] = array(
        'by_category' => array(
          'taxes' => 0.0,
          'charity' => 0.0,
          'volunteer' => 0.0,
        ),
        'earned_me' => 0.0,
        'country_iso2' => isset($row->country_iso2) && $row->country_iso2 !== NULL && $row->country_iso2 !== ''
          ? strtoupper((string) $row->country_iso2)
          : NULL,
      );
    }
    $by_uid[$uid]['by_category'][$category] += $earned;
    $by_uid[$uid]['earned_me'] += $earned;
  }

  $community = 0.0;
  $active = 0;
  foreach ($by_uid as $uid => $data) {
    foreach ($data['by_category'] as $cat => $val) {
      $by_uid[$uid]['by_category'][$cat] = round((float) $val, 2);
    }
    $by_uid[$uid]['earned_me'] = round((float) $data['earned_me'], 2);
    if ($by_uid[$uid]['earned_me'] > 0) {
      $community += $by_uid[$uid]['earned_me'];
      $active++;
    }
  }

  return array(
    'by_uid' => $by_uid,
    'community_earned_me' => round($community, 2),
    'active_earners' => $active,
  );
}

/**
 * Map ledger source (+ node type fallback) to spotlight category.
 *
 * Spotlight includes penalty → volunteer (unlike lifetime Merit Index).
 *
 * @param string|null $source
 * @param string $node_type
 *
 * @return string|null
 *   taxes|charity|volunteer or NULL.
 */
function rovas_civic_merit_spotlight_source_category($source, $node_type = '') {
  $source = trim((string) $source);
  if ($source === '' && $node_type === 'merit_penalty') {
    $source = 'penalty';
  }

  switch ($source) {
    case 'tax_payment':
      return 'taxes';

    case 'charitable_donation':
      return 'charity';

    case 'volunteer_work':
    case 'volunteer':
    case 'penalty':
      return 'volunteer';
  }

  return NULL;
}

/**
 * Build leads[] (0–3) from per-uid aggregation.
 *
 * Per category: highest earned_me; ties → lower uid. Omit category when ≤ 0.
 * Order: taxes, charity, volunteer.
 *
 * @param array $by_uid
 * @param int $viewer_uid
 *   Bearer viewer (0 = anonymous). Tax/charity leads that resolve aggregate
 *   for this viewer are skipped; volunteer leads are unchanged.
 *
 * @return array
 */
function rovas_civic_merit_spotlight_leads(array $by_uid, $viewer_uid = 0) {
  $categories = array('taxes', 'charity', 'volunteer');
  $winners = array();
  $viewer_uid = (int) $viewer_uid;
  module_load_include('inc', 'rovas_civic', 'rovas_civic.me_sources');

  foreach ($categories as $cat) {
    $best_uid = NULL;
    $best_me = 0.0;
    foreach ($by_uid as $uid => $data) {
      $me = isset($data['by_category'][$cat]) ? round((float) $data['by_category'][$cat], 2) : 0.0;
      if ($me <= 0) {
        continue;
      }
      $uid = (int) $uid;
      if (($cat === 'taxes' || $cat === 'charity')
        && function_exists('rovas_civic_me_sources_omit_from_tax_gift_filter')
        && rovas_civic_me_sources_omit_from_tax_gift_filter($uid, $viewer_uid)) {
        continue;
      }
      if ($best_uid === NULL
        || $me > $best_me
        || ($me == $best_me && $uid < $best_uid)) {
        $best_uid = $uid;
        $best_me = $me;
      }
    }
    if ($best_uid !== NULL && $best_me > 0) {
      $winners[$cat] = array(
        'uid' => $best_uid,
        'earned_me' => round($best_me, 2),
        'country_iso2' => isset($by_uid[$best_uid]['country_iso2'])
          ? $by_uid[$best_uid]['country_iso2']
          : NULL,
      );
    }
  }

  if (empty($winners)) {
    return array();
  }

  $lead_uids = array();
  foreach ($winners as $w) {
    $lead_uids[] = (int) $w['uid'];
  }
  $accounts = user_load_multiple(array_values(array_unique($lead_uids)));

  $leads = array();
  foreach ($categories as $cat) {
    if (!isset($winners[$cat])) {
      continue;
    }
    $uid = $winners[$cat]['uid'];
    $account = isset($accounts[$uid]) ? $accounts[$uid] : NULL;
    $labels = rovas_civic_user_public_labels($uid, $account);
    $leads[] = array(
      'category' => $cat,
      'earned_me' => $winners[$cat]['earned_me'],
      'rovas_uid' => (string) $uid,
      'username' => $labels['username'],
      'display_name' => $labels['display_name'],
      'country_iso2' => $winners[$cat]['country_iso2'],
    );
  }

  return $leads;
}

/**
 * Optional Bearer `you` block for the spotlight response.
 *
 * @param int $uid
 * @param array $window
 *   From rovas_civic_merit_spotlight_period_window().
 * @param array $leads
 *
 * @return array
 */
function rovas_civic_merit_spotlight_you($uid, array $window, array $leads) {
  $uid = (int) $uid;
  $by_category = array(
    'taxes' => 0.0,
    'charity' => 0.0,
    'volunteer' => 0.0,
  );

  // Caller's period totals are personal (not country-filtered).
  try {
    $period = rovas_civic_merit_spotlight_uid_credits(
      $uid,
      $window['start_ts'],
      $window['end_excl_ts']
    );
    $by_category = $period['by_category'];
    $earned_me = $period['earned_me'];

    $today = rovas_civic_merit_spotlight_uid_credits(
      $uid,
      $window['today_start_ts'],
      $window['today_end_excl_ts']
    );
    $earned_today_me = $today['earned_me'];
  }
  catch (Exception $e) {
    watchdog('rovas_civic', 'Merit spotlight you-block query failed: @m', array(
      '@m' => $e->getMessage(),
    ), WATCHDOG_ERROR);
    $earned_me = 0.0;
    $earned_today_me = 0.0;
  }

  $is_lead_for = array();
  foreach ($leads as $lead) {
    if (isset($lead['rovas_uid']) && (int) $lead['rovas_uid'] === $uid) {
      $is_lead_for[] = $lead['category'];
    }
  }

  return array(
    'earned_me' => round((float) $earned_me, 2),
    'by_category' => array(
      'taxes' => round((float) $by_category['taxes'], 2),
      'charity' => round((float) $by_category['charity'], 2),
      'volunteer' => round((float) $by_category['volunteer'], 2),
    ),
    'earned_today_me' => round((float) $earned_today_me, 2),
    'is_lead_for' => $is_lead_for,
  );
}

/**
 * Sum positive Merit credits for one uid in a unix window.
 *
 * @param int $uid
 * @param int $start_ts
 * @param int $end_excl_ts
 *
 * @return array
 *   keys: by_category, earned_me.
 */
function rovas_civic_merit_spotlight_uid_credits($uid, $start_ts, $end_excl_ts) {
  $uid = (int) $uid;
  $by_category = array(
    'taxes' => 0.0,
    'charity' => 0.0,
    'volunteer' => 0.0,
  );
  $earned_me = 0.0;

  if ($uid < 1) {
    return array('by_category' => $by_category, 'earned_me' => $earned_me);
  }

  $rows = db_query("
    SELECT n.type AS node_type,
      MAX(src.field_merit_source_value) AS merit_source,
      SUM(amt.field_reward_amount_value) AS earned_me
    FROM {node} n
    INNER JOIN {field_data_field_reward_recipient} rec
      ON rec.entity_id = n.nid AND rec.entity_type = 'node' AND rec.deleted = 0
    INNER JOIN {field_data_field_reward_amount} amt
      ON amt.entity_id = n.nid AND amt.entity_type = 'node' AND amt.deleted = 0
    LEFT JOIN {field_data_field_merit_source} src
      ON src.entity_id = n.nid AND src.entity_type = 'node' AND src.deleted = 0
    WHERE n.status = 1
      AND n.type IN ('merits_disbursement', 'merit_penalty')
      AND n.created >= :start_ts
      AND n.created < :end_excl_ts
      AND amt.field_reward_amount_value > 0
      AND rec.field_reward_recipient_target_id = :uid
    GROUP BY n.type, src.field_merit_source_value
  ", array(
    ':start_ts' => (int) $start_ts,
    ':end_excl_ts' => (int) $end_excl_ts,
    ':uid' => $uid,
  ))->fetchAll();

  foreach ($rows as $row) {
    $category = rovas_civic_merit_spotlight_source_category(
      isset($row->merit_source) ? $row->merit_source : NULL,
      isset($row->node_type) ? $row->node_type : ''
    );
    if ($category === NULL) {
      continue;
    }
    $me = (float) $row->earned_me;
    if ($me <= 0) {
      continue;
    }
    $by_category[$category] += $me;
    $earned_me += $me;
  }

  return array(
    'by_category' => $by_category,
    'earned_me' => $earned_me,
  );
}
