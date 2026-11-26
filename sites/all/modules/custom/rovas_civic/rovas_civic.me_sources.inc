<?php

/**
 * @file
 * ME sources share preference (everyone|reciprocal|aggregate) + response-time obfuscation.
 *
 * See docs/integration/civic_tasks/done/me-sources-privacy.md.
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
 * Generic Source label when tax vs gift must stay hidden.
 *
 * @return string
 *   Translated plain text (no HTML).
 */
function rovas_civic_me_sources_aggregate_source_label() {
  return t('Taxes and gifts');
}

/**
 * Whether a project nid is a tax (State Treasury) or gift (charity) attribution target.
 *
 * @param int $nid
 *
 * @return bool
 */
function rovas_civic_me_sources_is_tax_or_gift_project($nid) {
  $nid = (int) $nid;
  if ($nid < 1) {
    return FALSE;
  }

  static $cache = array();
  if (isset($cache[$nid])) {
    return $cache[$nid];
  }

  $treasury = (int) variable_get(ROVAS_CIVIC_STATE_TREASURY_VAR, 0);
  if ($treasury > 0 && $nid === $treasury) {
    $cache[$nid] = TRUE;
    return TRUE;
  }

  $project = node_load($nid);
  if (!$project || empty($project->type) || $project->type !== 'project') {
    $cache[$nid] = FALSE;
    return FALSE;
  }

  if (!empty($project->title) && $project->title === 'State Treasury of Slovakia') {
    $cache[$nid] = TRUE;
    return TRUE;
  }

  $flag = field_get_items('node', $project, 'field_is_charity');
  if (!empty($flag[0]['value'])) {
    $cache[$nid] = TRUE;
    return TRUE;
  }

  // Linked charity org without the flag still counts as gift attribution.
  module_load_include('inc', 'rovas_civic', 'rovas_civic.charity');
  if (function_exists('rovas_civic_project_is_charity') && rovas_civic_project_is_charity($project)) {
    $cache[$nid] = TRUE;
    return TRUE;
  }

  $cache[$nid] = FALSE;
  return FALSE;
}

/**
 * Whether a ledger source value is tax or gift (not volunteer).
 *
 * @param string|null $source
 *   field_merit_source value.
 *
 * @return bool
 */
function rovas_civic_me_sources_source_is_tax_or_gift($source) {
  if ($source === NULL || $source === '') {
    return FALSE;
  }
  if (module_exists('rovas_merit') && function_exists('rovas_merit_source_to_category')) {
    $category = rovas_merit_source_to_category($source);
    return $category === 'taxes' || $category === 'charity';
  }
  return $source === 'tax_payment' || $source === 'charitable_donation';
}

/**
 * Whether a merits_disbursement / merit_penalty row is tax or gift origin.
 *
 * Prefers field_merit_source; falls back to parent receipt project attribution.
 *
 * @param object|null $entity
 *   Ledger node, or NULL.
 * @param int $project_nid
 *   Optional project nid when entity is unavailable.
 *
 * @return bool
 */
function rovas_civic_me_sources_row_is_tax_or_gift($entity = NULL, $project_nid = 0) {
  if (!empty($entity) && is_object($entity)) {
    $items = field_get_items('node', $entity, 'field_merit_source');
    if (!empty($items[0]['value']) && rovas_civic_me_sources_source_is_tax_or_gift($items[0]['value'])) {
      return TRUE;
    }
    if (empty($project_nid)) {
      $parent = field_get_items('node', $entity, 'field_parent_purchase_receipt');
      if (!empty($parent[0]['target_id'])) {
        $receipt = node_load((int) $parent[0]['target_id']);
        if ($receipt) {
          $proj = field_get_items('node', $receipt, 'field_receipt_project');
          if (!empty($proj[0]['target_id'])) {
            $project_nid = (int) $proj[0]['target_id'];
          }
        }
      }
    }
  }

  return rovas_civic_me_sources_is_tax_or_gift_project($project_nid);
}

/**
 * Render Merit history Source cell HTML (Views merit_history PHP field).
 *
 * When the viewer may only see aggregate ME sources for the profile subject,
 * tax/gift rows show a generic label with no project link.
 *
 * @param object $view
 * @param object $data
 *   Views result row.
 *
 * @return string
 *   HTML (already safe / escaped where needed).
 */
function rovas_civic_me_sources_merit_history_source_html($view, $data) {
  global $user;

  $subject_uid = 0;
  if (!empty($view->args[0]) && is_numeric($view->args[0])) {
    $subject_uid = (int) $view->args[0];
  }
  $viewer_uid = !empty($user->uid) ? (int) $user->uid : 0;

  $entity = NULL;
  if (!empty($data->_field_data['nid']['entity'])) {
    $entity = $data->_field_data['nid']['entity'];
  }

  $project_nid = 0;
  if (!empty($data->field_field_receipt_project[0]['rendered']['#item']['target_id'])) {
    $project_nid = (int) $data->field_field_receipt_project[0]['rendered']['#item']['target_id'];
  }
  elseif (!empty($data->field_field_receipt_project[0]['raw']['target_id'])) {
    $project_nid = (int) $data->field_field_receipt_project[0]['raw']['target_id'];
  }

  $aggregate = ($subject_uid > 0)
    && (rovas_civic_me_sources_detail_resolve($subject_uid, $viewer_uid) === ROVAS_CIVIC_ME_SOURCES_DETAIL_AGGREGATE);

  if ($aggregate && rovas_civic_me_sources_row_is_tax_or_gift($entity, $project_nid)) {
    return check_plain(rovas_civic_me_sources_aggregate_source_label());
  }

  if (!empty($entity) && !empty($entity->type) && $entity->type === 'merit_penalty') {
    return check_plain(t('Penalty'));
  }

  $origprojlabel = empty($data->field_field_receipt_project[0]['rendered']['#label'])
    ? ''
    : $data->field_field_receipt_project[0]['rendered']['#label'];

  if (empty($data->field_field_receipt_project) && empty($data->field_field_rewarded_report)) {
    return check_plain(t('Merits purchase'));
  }

  if (empty($data->field_field_rewarded_report[0]['rendered']['#label'])) {
    $projlabel = (drupal_strlen($origprojlabel) > 55)
      ? drupal_substr($origprojlabel, 0, 51) . '...'
      : $origprojlabel;
    $href_nid = $project_nid;
    if ($href_nid < 1 && !empty($data->field_field_receipt_project[0]['rendered']['#item']['target_id'])) {
      $href_nid = (int) $data->field_field_receipt_project[0]['rendered']['#item']['target_id'];
    }
    return '<span class="projct-type">' . check_plain(t('Project')) . '</span> '
      . '<a href="/node/' . (int) $href_nid . '" title="' . check_plain($origprojlabel) . '">'
      . check_plain($projlabel) . '</a>';
  }

  $origwreplabel = $data->field_field_rewarded_report[0]['rendered']['#label'];
  $wreplabel = (drupal_strlen($origwreplabel) > 55)
    ? drupal_substr($origwreplabel, 0, 51) . '...'
    : $origwreplabel;
  $wr_nid = 0;
  if (!empty($data->field_field_rewarded_report[0]['rendered']['#item']['target_id'])) {
    $wr_nid = (int) $data->field_field_rewarded_report[0]['rendered']['#item']['target_id'];
  }
  return '<span class="wreport-type">' . check_plain(t('Report')) . '</span> '
    . '<a href="/node/' . $wr_nid . '" title="' . check_plain($origwreplabel) . '">'
    . check_plain($wreplabel) . '</a>';
}

/**
 * SQL fragment: keep purchase_receipt rows whose buyer is visible under ME sources privacy.
 *
 * Used on tax/gift project buyer lists. Parameters: :me_src_viewer, :me_src_viewer_pref.
 *
 * @param string $receipt_nid_expression
 *   SQL expression for the purchase_receipt nid (e.g. "node.nid").
 *
 * @return string
 */
function rovas_civic_me_sources_buyer_visible_where($receipt_nid_expression) {
  return 'NOT EXISTS (
    SELECT 1
    FROM {field_data_field_buyer} me_src_b
    LEFT JOIN {civic_user_prefs} me_src_p ON me_src_p.uid = me_src_b.field_buyer_target_id
    WHERE me_src_b.entity_type = \'node\'
      AND me_src_b.deleted = 0
      AND me_src_b.entity_id = ' . $receipt_nid_expression . '
      AND me_src_b.field_buyer_target_id <> :me_src_viewer
      AND (
        me_src_p.me_sources_share = \'aggregate\'
        OR (
          me_src_p.me_sources_share = \'reciprocal\'
          AND (:me_src_viewer < 1 OR :me_src_viewer_pref = \'aggregate\')
        )
      )
  )';
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
 * Add ME sources share fieldset to the user account edit form.
 *
 * @param array $form
 * @param array $form_state
 * @param object|null $account
 *   Account being edited.
 */
function rovas_civic_attach_me_sources_share_fieldset(&$form, &$form_state, $account) {
  if (!$account || empty($account->uid) || !db_table_exists('civic_user_prefs')) {
    return;
  }

  $current = rovas_civic_me_sources_share_get((int) $account->uid);

  $form['me_sources_share_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Merit sources — privacy'),
    '#description' => t('Choose how much detail other users can see about the origin of your Merits. Your total Merits and Merits for volunteer work stay visible. You always see complete information about your Merits.'),
    '#collapsible' => FALSE,
    '#tree' => TRUE,
    '#weight' => 36,
    '#id' => 'edit-me-sources-share-settings',
  );

  $form['me_sources_share_settings']['share'] = array(
    '#type' => 'radios',
    '#title' => t('What others may see'),
    '#default_value' => $current,
    '#required' => TRUE,
    '#options' => array(
      ROVAS_CIVIC_ME_SOURCES_SHARE_EVERYONE => t('Full detail (taxes, gifts, and volunteer separately)'),
      ROVAS_CIVIC_ME_SOURCES_SHARE_RECIPROCAL => t('Full detail only for users who also share detail with others'),
      ROVAS_CIVIC_ME_SOURCES_SHARE_AGGREGATE => t('Combined taxes and gifts only (hide the split)'),
    ),
  );

  $form['#validate'][] = 'rovas_civic_me_sources_share_settings_validate';
  $form['#submit'][] = 'rovas_civic_me_sources_share_settings_submit';
}

/**
 * Validate ME sources share on account edit.
 */
function rovas_civic_me_sources_share_settings_validate($form, &$form_state) {
  if (empty($form_state['values']['me_sources_share_settings']['share'])) {
    form_set_error('me_sources_share_settings][share', t('Please choose a Merit sources privacy option.'));
    return;
  }
  $share = $form_state['values']['me_sources_share_settings']['share'];
  if ($share !== ROVAS_CIVIC_ME_SOURCES_SHARE_EVERYONE
    && $share !== ROVAS_CIVIC_ME_SOURCES_SHARE_RECIPROCAL
    && $share !== ROVAS_CIVIC_ME_SOURCES_SHARE_AGGREGATE) {
    form_set_error('me_sources_share_settings][share', t('Invalid Merit sources privacy option.'));
  }
}

/**
 * Save ME sources share from account edit.
 */
function rovas_civic_me_sources_share_settings_submit($form, &$form_state) {
  if (empty($form['#user']->uid) || empty($form_state['values']['me_sources_share_settings']['share'])) {
    return;
  }
  module_load_include('inc', 'rovas_civic', 'rovas_civic.me_sources');
  rovas_civic_me_sources_share_store(
    (int) $form['#user']->uid,
    $form_state['values']['me_sources_share_settings']['share']
  );
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
