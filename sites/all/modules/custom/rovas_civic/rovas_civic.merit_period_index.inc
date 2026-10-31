<?php

/**
 * @file
 * Civic Merit period index — GET /rovas/civic_merit_period_index.
 *
 * ME•ter time dimension (day/week/month). Reuses spotlight ledger aggregate;
 * does not touch the lifetime Merit Index snapshot. See civic.md.
 */

/**
 * Services callback: GET /rovas/civic_merit_period_index.
 */
function rovas_civic_services_merit_period_index_index(
  $period = '',
  $limit = ROVAS_CIVIC_MERIT_INDEX_DEFAULT_LIMIT,
  $offset = 0,
  $sort = 'total_me',
  $order = 'desc',
  $country_iso2 = '',
  $source_category = '',
  $lang = ''
) {
  module_load_include('inc', 'rovas_civic', 'rovas_civic.merit_spotlight');
  module_load_include('inc', 'rovas_civic', 'rovas_civic.merit_index');

  $period = drupal_strtolower(trim((string) $period));
  $allowed_period = array('day', 'week', 'month');
  if (!in_array($period, $allowed_period, TRUE)) {
    return services_error(t('Invalid period; use day, week, or month.'), 400);
  }

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
    'total_me',
    'taxes_me',
    'charity_me',
    'volunteer_me',
    'position',
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

  $country = strtoupper(trim((string) $country_iso2));
  if ($country !== '' && !preg_match('/^[A-Z]{2}$/', $country)) {
    return services_error(t('Invalid country_iso2; use ISO-2.'), 400);
  }
  $country_filter = ($country === '') ? NULL : $country;

  $source_category = drupal_strtolower(trim((string) $source_category));
  if ($source_category !== '') {
    $allowed_source = array('taxes', 'charity', 'volunteer');
    if (!in_array($source_category, $allowed_source, TRUE)) {
      return services_error(t('Invalid source_category; use taxes, charity, or volunteer.'), 400);
    }
    if ($sort === 'total_me') {
      $sort = $source_category . '_me';
    }
  }

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
    watchdog('rovas_civic', 'Merit period index ledger query failed: @m', array(
      '@m' => $e->getMessage(),
    ), WATCHDOG_ERROR);
    return services_error(t('Merit period index temporarily unavailable.'), 503);
  }

  return rovas_civic_merit_period_index_page(array(
    'period' => $period,
    'period_start' => $window['period_start'],
    'period_end' => $window['period_end'],
    'limit' => $pager['limit'],
    'offset' => $pager['offset'],
    'sort' => $sort,
    'order' => $order,
    'source_category' => $source_category,
    'lang' => rovas_civic_request_lang($lang),
    'by_uid' => isset($agg['by_uid']) && is_array($agg['by_uid']) ? $agg['by_uid'] : array(),
  ));
}

/**
 * Rank / filter / paginate period earners into Merit Index–shaped rows.
 *
 * @param array $opts
 *
 * @return array
 */
function rovas_civic_merit_period_index_page(array $opts) {
  module_load_include('inc', 'rovas_civic', 'rovas_civic.me_sources');
  $viewer_uid = function_exists('rovas_civic_request_uid') ? (int) rovas_civic_request_uid() : 0;
  $privacy_filter = (!empty($opts['source_category'])
    && ($opts['source_category'] === 'taxes' || $opts['source_category'] === 'charity'));

  $rows = array();
  foreach ($opts['by_uid'] as $uid => $data) {
    if (!is_array($data)) {
      continue;
    }
    $uid = (int) $uid;
    $earned = isset($data['earned_me']) ? (float) $data['earned_me'] : 0.0;
    // Omit users whose period ME rounds to ≤ 0 (integer round per contract).
    if (round($earned) <= 0) {
      continue;
    }

    $by_category = isset($data['by_category']) && is_array($data['by_category'])
      ? $data['by_category']
      : array('taxes' => 0.0, 'charity' => 0.0, 'volunteer' => 0.0);

    if (!empty($opts['source_category'])) {
      $cat = $opts['source_category'];
      $cat_me = isset($by_category[$cat]) ? (float) $by_category[$cat] : 0.0;
      if (round($cat_me) <= 0) {
        continue;
      }
      if ($privacy_filter
        && function_exists('rovas_civic_me_sources_omit_from_tax_gift_filter')
        && rovas_civic_me_sources_omit_from_tax_gift_filter($uid, $viewer_uid)) {
        continue;
      }
    }

    $country_iso2 = isset($data['country_iso2']) && $data['country_iso2'] !== NULL && $data['country_iso2'] !== ''
      ? strtoupper((string) $data['country_iso2'])
      : '';

    $rows[] = array(
      'uid' => $uid,
      'rovas_uid' => (string) $uid,
      'total_me' => round($earned, 2),
      'value_eur' => 0.0,
      'country_iso2' => $country_iso2,
      'by_category' => array(
        'taxes' => round(isset($by_category['taxes']) ? (float) $by_category['taxes'] : 0.0, 2),
        'charity' => round(isset($by_category['charity']) ? (float) $by_category['charity'] : 0.0, 2),
        'volunteer' => round(isset($by_category['volunteer']) ? (float) $by_category['volunteer'] : 0.0, 2),
      ),
    );
  }

  $sort = ($opts['sort'] === 'position') ? 'total_me' : $opts['sort'];
  $order = $opts['order'];
  usort($rows, function ($a, $b) use ($sort, $order, $viewer_uid) {
    return rovas_civic_merit_period_index_compare_rows($a, $b, $sort, $order, $viewer_uid);
  });

  $total = count($rows);
  $slice = array_slice($rows, (int) $opts['offset'], (int) $opts['limit']);
  $opts['viewer_uid'] = $viewer_uid;
  $items = rovas_civic_merit_period_index_enrich_slice($slice, $opts);

  $out = array(
    'total' => (int) $total,
    'limit' => (int) $opts['limit'],
    'offset' => (int) $opts['offset'],
    'sort' => $opts['sort'],
    'order' => $opts['order'],
    'period' => $opts['period'],
    'period_start' => $opts['period_start'],
    'period_end' => $opts['period_end'],
    'items' => $items,
  );
  if ($privacy_filter) {
    $out['privacy_filter_active'] = TRUE;
  }
  return $out;
}

/**
 * Compare period-index rows; ties → lower uid.
 *
 * @param array $a
 * @param array $b
 * @param string $sort
 * @param string $order
 *
 * @return int
 */
function rovas_civic_merit_period_index_compare_rows(array $a, array $b, $sort, $order, $viewer_uid = 0) {
  $dir = ($order === 'asc') ? 1 : -1;
  $viewer_uid = (int) $viewer_uid;

  switch ($sort) {
    case 'taxes_me':
      $va = function_exists('rovas_civic_merit_index_sort_category_me')
        ? rovas_civic_merit_index_sort_category_me($a, 'taxes', $viewer_uid)
        : (float) $a['by_category']['taxes'];
      $vb = function_exists('rovas_civic_merit_index_sort_category_me')
        ? rovas_civic_merit_index_sort_category_me($b, 'taxes', $viewer_uid)
        : (float) $b['by_category']['taxes'];
      break;

    case 'charity_me':
      $va = function_exists('rovas_civic_merit_index_sort_category_me')
        ? rovas_civic_merit_index_sort_category_me($a, 'charity', $viewer_uid)
        : (float) $a['by_category']['charity'];
      $vb = function_exists('rovas_civic_merit_index_sort_category_me')
        ? rovas_civic_merit_index_sort_category_me($b, 'charity', $viewer_uid)
        : (float) $b['by_category']['charity'];
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
    $ua = (int) $a['uid'];
    $ub = (int) $b['uid'];
    if ($ua == $ub) {
      return 0;
    }
    // Stable tie-break: lower uid first (independent of asc/desc).
    return ($ua < $ub) ? -1 : 1;
  }
  return ($va < $vb ? -1 : 1) * $dir;
}

/**
 * Enrich a page slice with user labels, member_since_year, country_label.
 *
 * @param array $slice
 * @param array $opts
 *
 * @return array
 */
function rovas_civic_merit_period_index_enrich_slice(array $slice, array $opts) {
  if (empty($slice)) {
    return array();
  }

  $uids = array();
  foreach ($slice as $row) {
    $uids[] = (int) $row['uid'];
  }
  $accounts = user_load_multiple(array_values(array_unique($uids)));
  $lang = isset($opts['lang']) ? $opts['lang'] : NULL;
  $offset = (int) $opts['offset'];
  $viewer_uid = isset($opts['viewer_uid']) ? (int) $opts['viewer_uid'] : 0;
  if (!function_exists('rovas_civic_me_sources_apply_payload')) {
    module_load_include('inc', 'rovas_civic', 'rovas_civic.me_sources');
  }

  $out = array();
  foreach ($slice as $i => $row) {
    $uid = (int) $row['uid'];
    $account = isset($accounts[$uid]) ? $accounts[$uid] : NULL;
    $labels = rovas_civic_user_public_labels($uid, $account);
    $created = ($account && !empty($account->created)) ? (int) $account->created : 0;
    $member_since_year = $created > 0 ? (int) format_date($created, 'custom', 'Y', 'UTC') : 0;
    $country_iso2 = isset($row['country_iso2']) ? (string) $row['country_iso2'] : '';

    $item = array(
      'position' => $offset + $i + 1,
      'rovas_uid' => (string) $uid,
      'username' => $labels['username'],
      'display_name' => $labels['display_name'],
      'member_since_year' => $member_since_year,
      'total_me' => (float) $row['total_me'],
      'value_eur' => 0.0,
      'country_iso2' => $country_iso2,
      'country_label' => rovas_civic_merit_index_country_label($country_iso2, $lang),
      'by_category' => $row['by_category'],
      'by_source' => array(),
      'nace_expertise' => array(),
    );
    if (function_exists('rovas_civic_me_sources_apply_payload')) {
      rovas_civic_me_sources_apply_payload($item, $viewer_uid, $uid);
    }
    $out[] = $item;
  }

  return $out;
}
