<?php

/**
 * @file
 * ME sources share preference (everyone|reciprocal|aggregate) + response-time obfuscation.
 *
 * See docs/integration/civic_tasks/active/me-sources-privacy.md.
 */

/**
 * Read ME sources share preference for a user.
 *
 * @param int $uid
 *
 * @return string
 *   everyone|reciprocal|aggregate (unset → everyone).
 */
function rovas_civic_me_sources_share_get($uid) {
  $uid = (int) $uid;
  if ($uid < 1 || !db_table_exists('civic_user_prefs')) {
    return ROVAS_CIVIC_ME_SOURCES_SHARE_EVERYONE;
  }
  $value = db_query('SELECT me_sources_share FROM {civic_user_prefs} WHERE uid = :u', array(
    ':u' => $uid,
  ))->fetchField();
  if ($value === FALSE || $value === NULL || $value === '') {
    return ROVAS_CIVIC_ME_SOURCES_SHARE_EVERYONE;
  }
  $value = (string) $value;
  if ($value === ROVAS_CIVIC_ME_SOURCES_SHARE_EVERYONE
    || $value === ROVAS_CIVIC_ME_SOURCES_SHARE_RECIPROCAL
    || $value === ROVAS_CIVIC_ME_SOURCES_SHARE_AGGREGATE) {
    return $value;
  }
  return ROVAS_CIVIC_ME_SOURCES_SHARE_EVERYONE;
}

/**
 * Persist ME sources share preference.
 *
 * @param int $uid
 * @param string $share
 *   everyone|reciprocal|aggregate.
 *
 * @return bool
 */
function rovas_civic_me_sources_share_store($uid, $share) {
  $uid = (int) $uid;
  if ($uid < 1) {
    return FALSE;
  }
  if ($share !== ROVAS_CIVIC_ME_SOURCES_SHARE_EVERYONE
    && $share !== ROVAS_CIVIC_ME_SOURCES_SHARE_RECIPROCAL
    && $share !== ROVAS_CIVIC_ME_SOURCES_SHARE_AGGREGATE) {
    return FALSE;
  }
  if (!db_table_exists('civic_user_prefs')) {
    return FALSE;
  }
  db_merge('civic_user_prefs')
    ->key(array('uid' => $uid))
    ->fields(array(
      'me_sources_share' => $share,
      'updated' => REQUEST_TIME,
    ))
    ->execute();
  return TRUE;
}

/**
 * Resolve whether the viewer may see detailed ME sources for the subject.
 *
 * @param int $subject_uid
 * @param int $viewer_uid
 *   0 = anonymous / no Bearer.
 *
 * @return string
 *   detailed|aggregate.
 */
function rovas_civic_me_sources_detail_resolve($subject_uid, $viewer_uid) {
  $subject_uid = (int) $subject_uid;
  $viewer_uid = (int) $viewer_uid;
  if ($subject_uid < 1) {
    return ROVAS_CIVIC_ME_SOURCES_DETAIL_AGGREGATE;
  }
  if ($subject_uid === $viewer_uid) {
    return ROVAS_CIVIC_ME_SOURCES_DETAIL_DETAILED;
  }

  $subject_pref = rovas_civic_me_sources_share_get($subject_uid);
  if ($subject_pref === ROVAS_CIVIC_ME_SOURCES_SHARE_EVERYONE) {
    return ROVAS_CIVIC_ME_SOURCES_DETAIL_DETAILED;
  }
  if ($subject_pref === ROVAS_CIVIC_ME_SOURCES_SHARE_AGGREGATE) {
    return ROVAS_CIVIC_ME_SOURCES_DETAIL_AGGREGATE;
  }

  // reciprocal: detailed iff viewer is signed in and viewer's share ≠ aggregate.
  if ($viewer_uid < 1) {
    return ROVAS_CIVIC_ME_SOURCES_DETAIL_AGGREGATE;
  }
  $viewer_pref = rovas_civic_me_sources_share_get($viewer_uid);
  if ($viewer_pref === ROVAS_CIVIC_ME_SOURCES_SHARE_AGGREGATE) {
    return ROVAS_CIVIC_ME_SOURCES_DETAIL_AGGREGATE;
  }
  return ROVAS_CIVIC_ME_SOURCES_DETAIL_DETAILED;
}

/**
 * Whether taxes/charity ME•tre filters should omit this subject for the viewer.
 *
 * @param int $subject_uid
 * @param int $viewer_uid
 *
 * @return bool
 */
function rovas_civic_me_sources_omit_from_tax_gift_filter($subject_uid, $viewer_uid) {
  return rovas_civic_me_sources_detail_resolve($subject_uid, $viewer_uid)
    === ROVAS_CIVIC_ME_SOURCES_DETAIL_AGGREGATE;
}

/**
 * Obfuscate an in-place ME-mix payload when detail is aggregate.
 *
 * Mutates by_category / by_source / taxes_last_3_months / top_charities and
 * sets me_sources_detail. Safe no-op shape when keys are missing (index rows).
 *
 * @param array $row
 *   Payload with optional by_category, by_source, taxes_last_3_months, top_charities.
 * @param string $detail
 *   detailed|aggregate.
 */
function rovas_civic_me_sources_obfuscate_payload(array &$row, $detail) {
  if ($detail === ROVAS_CIVIC_ME_SOURCES_DETAIL_DETAILED) {
    $row['me_sources_detail'] = ROVAS_CIVIC_ME_SOURCES_DETAIL_DETAILED;
    if (isset($row['by_category']) && is_array($row['by_category'])) {
      unset($row['by_category']['taxes_and_gifts']);
    }
    return;
  }

  $row['me_sources_detail'] = ROVAS_CIVIC_ME_SOURCES_DETAIL_AGGREGATE;

  if (!isset($row['by_category']) || !is_array($row['by_category'])) {
    $row['by_category'] = array(
      'taxes' => 0.0,
      'charity' => 0.0,
      'volunteer' => 0.0,
    );
  }

  $taxes = isset($row['by_category']['taxes']) ? (float) $row['by_category']['taxes'] : 0.0;
  $charity = isset($row['by_category']['charity']) ? (float) $row['by_category']['charity'] : 0.0;
  $volunteer = isset($row['by_category']['volunteer']) ? (float) $row['by_category']['volunteer'] : 0.0;

  $row['by_category'] = array(
    'taxes' => 0.0,
    'charity' => 0.0,
    'volunteer' => round($volunteer, 2),
    'taxes_and_gifts' => round($taxes + $charity, 2),
  );

  if (isset($row['by_source']) && is_array($row['by_source'])) {
    $kept = array();
    foreach ($row['by_source'] as $key => $me) {
      if ($key === 'volunteer_work' || $key === 'volunteer') {
        $kept[$key] = $me;
      }
    }
    $row['by_source'] = $kept;
  }

  if (array_key_exists('taxes_last_3_months', $row)) {
    $row['taxes_last_3_months'] = array();
  }
  if (array_key_exists('top_charities', $row)) {
    $row['top_charities'] = array();
  }
}

/**
 * Resolve detail for viewer/subject and apply to a payload row.
 *
 * @param array $row
 * @param int $viewer_uid
 * @param int $subject_uid
 *
 * @return string
 *   detailed|aggregate.
 */
function rovas_civic_me_sources_apply_payload(array &$row, $viewer_uid, $subject_uid) {
  $detail = rovas_civic_me_sources_detail_resolve($subject_uid, $viewer_uid);
  rovas_civic_me_sources_obfuscate_payload($row, $detail);
  return $detail;
}

/**
 * Services: POST /rovas/civic_me_sources_preference
 *
 * @param array|object $data
 *   Body with share: everyone|reciprocal|aggregate.
 *
 * @return array
 */
function rovas_civic_services_me_sources_preference($data) {
  $uid = rovas_civic_request_uid();
  if ($uid < 1) {
    return services_error(t('Authentication required'), 401);
  }
  if (!is_array($data)) {
    $data = (array) $data;
  }
  $share = isset($data['share']) ? trim((string) $data['share']) : '';
  if ($share !== ROVAS_CIVIC_ME_SOURCES_SHARE_EVERYONE
    && $share !== ROVAS_CIVIC_ME_SOURCES_SHARE_RECIPROCAL
    && $share !== ROVAS_CIVIC_ME_SOURCES_SHARE_AGGREGATE) {
    return services_error(t('Body must include share: "everyone", "reciprocal", or "aggregate".'), 400);
  }
  if (!rovas_civic_me_sources_share_store($uid, $share)) {
    return services_error(t('Could not store preference.'), 500);
  }
  return array(
    'me_sources_share' => $share,
  );
}
