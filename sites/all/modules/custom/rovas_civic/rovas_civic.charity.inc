<?php

/**
 * @file
 * Charitable organizations + Civic catalog / enrollment.
 *
 * Content type charity_organization holds org identity; projects link via
 * field_charity_organization. See docs/integration/civic.md.
 */

if (!defined('ROVAS_CIVIC_CHARITY_BUNDLE')) {
  define('ROVAS_CIVIC_CHARITY_BUNDLE', 'charity_organization');
}
if (!defined('ROVAS_CIVIC_CHARITY_STATUS_PENDING')) {
  define('ROVAS_CIVIC_CHARITY_STATUS_PENDING', 'pending');
}
if (!defined('ROVAS_CIVIC_CHARITY_STATUS_APPROVED')) {
  define('ROVAS_CIVIC_CHARITY_STATUS_APPROVED', 'approved');
}

/**
 * Whether a project is marked charitable and linked to an org.
 *
 * @param object $project
 *
 * @return bool
 */
function rovas_civic_project_is_charity($project) {
  if (empty($project) || empty($project->type) || $project->type !== 'project') {
    return FALSE;
  }
  $flag = field_get_items('node', $project, 'field_is_charity');
  if (empty($flag[0]['value'])) {
    return FALSE;
  }
  $ref = field_get_items('node', $project, 'field_charity_organization');
  return !empty($ref[0]['target_id']);
}

/**
 * Load the charity_organization node linked from a project, or NULL.
 *
 * @param object $project
 *
 * @return object|null
 */
function rovas_civic_project_charity_org($project) {
  if (empty($project)) {
    return NULL;
  }
  $ref = field_get_items('node', $project, 'field_charity_organization');
  if (empty($ref[0]['target_id'])) {
    return NULL;
  }
  $org = node_load((int) $ref[0]['target_id']);
  if (!$org || $org->type !== ROVAS_CIVIC_CHARITY_BUNDLE) {
    return NULL;
  }
  return $org;
}

/**
 * Org approval status string, or NULL.
 *
 * @param object $org
 *
 * @return string|null
 *   pending|approved or NULL.
 */
function rovas_civic_charity_org_status($org) {
  if (empty($org)) {
    return NULL;
  }
  $items = field_get_items('node', $org, 'field_charity_status');
  if (empty($items[0]['value'])) {
    return ROVAS_CIVIC_CHARITY_STATUS_PENDING;
  }
  $status = (string) $items[0]['value'];
  if ($status === ROVAS_CIVIC_CHARITY_STATUS_APPROVED || $status === ROVAS_CIVIC_CHARITY_STATUS_PENDING) {
    return $status;
  }
  return ROVAS_CIVIC_CHARITY_STATUS_PENDING;
}

/**
 * Whether anonymous Civic reads + catalog inclusion are allowed.
 *
 * @param object $project
 *
 * @return bool
 */
function rovas_civic_project_is_approved_charity($project) {
  if (!rovas_civic_project_is_charity($project)) {
    return FALSE;
  }
  $org = rovas_civic_project_charity_org($project);
  return rovas_civic_charity_org_status($org) === ROVAS_CIVIC_CHARITY_STATUS_APPROVED;
}

/**
 * A12: block EUR donations / EUR share purchases while charity org is pending.
 *
 * Ordinary (non-charity) projects are never blocked here. CHR rails stay open.
 *
 * @param object|null $project
 *
 * @return bool
 *   TRUE when EUR rails must be refused.
 */
function rovas_civic_project_blocks_eur_rails($project) {
  if (empty($project) || empty($project->type) || $project->type !== 'project') {
    return FALSE;
  }
  if (!rovas_civic_project_is_charity($project)) {
    return FALSE;
  }
  return !rovas_civic_project_is_approved_charity($project);
}

/**
 * Services access: always allow; callbacks gate anonymous vs Bearer.
 */
function rovas_civic_project_read_access() {
  return TRUE;
}

/**
 * Assert the caller may read this published project via Civic APIs.
 *
 * Bearer (or session) may read any published project. Anonymous only when the
 * project is linked to an approved charity_organization.
 *
 * @param object $project
 *
 * @return true|mixed
 *   TRUE, or services_error() result.
 */
function rovas_civic_assert_project_readable($project) {
  $uid = rovas_civic_request_uid();
  if ($uid >= 1) {
    return TRUE;
  }
  if (rovas_civic_project_is_approved_charity($project)) {
    return TRUE;
  }
  return services_error(t('Authentication required'), 401);
}

/**
 * Plain text field value with optional language (source fallback).
 *
 * @param object $node
 * @param string $field_name
 * @param string|null $lang
 *
 * @return string|null
 */
function rovas_civic_node_text_field($node, $field_name, $lang = NULL) {
  if ($lang !== NULL && $lang !== '') {
    $lang = drupal_strtolower(trim((string) $lang));
    if (!empty($node->{$field_name}[$lang][0]['value'])) {
      return trim((string) $node->{$field_name}[$lang][0]['value']);
    }
    $items = field_get_items('node', $node, $field_name, $lang);
    if (!empty($items[0]['value'])) {
      return trim((string) $items[0]['value']);
    }
    if (function_exists('rovas_base_translation_pick_source_text')) {
      $src = rovas_base_translation_pick_source_text($node, $field_name);
      if ($src !== '') {
        return trim($src);
      }
    }
  }
  $items = field_get_items('node', $node, $field_name);
  if (!empty($items[0]['value'])) {
    return trim((string) $items[0]['value']);
  }
  return NULL;
}

/**
 * Donate instructions for a project (project override, else org default).
 *
 * @param object $project
 * @param object|null $org
 * @param string|null $lang
 *
 * @return string|null
 */
function rovas_civic_project_donate_instructions($project, $org = NULL, $lang = NULL) {
  $override = rovas_civic_node_text_field($project, 'field_charity_donate_text', $lang);
  if ($override !== NULL && $override !== '') {
    return $override;
  }
  if ($org === NULL) {
    $org = rovas_civic_project_charity_org($project);
  }
  if ($org) {
    return rovas_civic_node_text_field($org, 'field_charity_donate_text', $lang);
  }
  return NULL;
}

/**
 * Donate URL for a project (project override, else org default).
 *
 * @param object $project
 * @param object|null $org
 * @param string|null $lang
 *
 * @return string|null
 */
function rovas_civic_project_donate_url($project, $org = NULL, $lang = NULL) {
  $override = rovas_civic_node_text_field($project, 'field_charity_donate_url', $lang);
  if ($override !== NULL && $override !== '') {
    return $override;
  }
  if ($org === NULL) {
    $org = rovas_civic_project_charity_org($project);
  }
  if ($org) {
    return rovas_civic_node_text_field($org, 'field_charity_donate_url', $lang);
  }
  return NULL;
}

/**
 * Charity fields for civic_project JSON (only when charitable).
 *
 * @param object $project
 * @param string|null $lang
 *
 * @return array
 */
function rovas_civic_project_charity_json($project, $lang = NULL) {
  if (!rovas_civic_project_is_charity($project)) {
    return array(
      'is_charity' => FALSE,
      'charity_status' => NULL,
      'organization' => NULL,
      'donate_instructions' => NULL,
      'donate_url' => NULL,
    );
  }
  $org = rovas_civic_project_charity_org($project);
  $status = rovas_civic_charity_org_status($org);
  $legal = $org ? trim((string) $org->title) : '';
  return array(
    'is_charity' => TRUE,
    'charity_status' => $status,
    'organization' => $legal !== '' ? $legal : NULL,
    'donate_instructions' => rovas_civic_project_donate_instructions($project, $org, $lang),
    'donate_url' => rovas_civic_project_donate_url($project, $org, $lang),
  );
}

/**
 * Services: GET /rovas/civic_charity_projects
 */
function rovas_civic_services_charity_projects_index($query = '', $limit = ROVAS_CIVIC_CHARITY_CATALOG_DEFAULT_LIMIT, $lang = '') {
  return rovas_civic_charity_projects_catalog($query, $limit, rovas_civic_request_lang($lang));
}

/**
 * Approved-charity project catalog.
 *
 * @param string $query
 * @param int $limit
 * @param string|null $lang
 *
 * @return array
 *   keys: total, projects
 */
function rovas_civic_charity_projects_catalog($query = '', $limit = ROVAS_CIVIC_CHARITY_CATALOG_DEFAULT_LIMIT, $lang = NULL) {
  $query = trim((string) $query);
  $has_query = ($query !== '');
  if ($has_query && drupal_strlen($query) < 1) {
    return array('total' => 0, 'projects' => array());
  }

  $limit = (int) $limit;
  if ($limit < 1) {
    $limit = ROVAS_CIVIC_CHARITY_CATALOG_DEFAULT_LIMIT;
  }
  if ($limit > ROVAS_CIVIC_CHARITY_CATALOG_MAX_LIMIT) {
    $limit = ROVAS_CIVIC_CHARITY_CATALOG_MAX_LIMIT;
  }

  $total = (int) db_query("
    SELECT COUNT(DISTINCT n.nid)
    FROM {node} n
    INNER JOIN {field_data_field_is_charity} ic
      ON ic.entity_id = n.nid AND ic.entity_type = 'node' AND ic.deleted = 0
      AND ic.field_is_charity_value = 1
    INNER JOIN {field_data_field_charity_organization} ref
      ON ref.entity_id = n.nid AND ref.entity_type = 'node' AND ref.deleted = 0
    INNER JOIN {node} org ON org.nid = ref.field_charity_organization_target_id
      AND org.type = :bundle AND org.status = 1
    INNER JOIN {field_data_field_charity_status} st
      ON st.entity_id = org.nid AND st.entity_type = 'node' AND st.deleted = 0
      AND st.field_charity_status_value = :approved
    WHERE n.type = 'project' AND n.status = 1
  ", array(
    ':bundle' => ROVAS_CIVIC_CHARITY_BUNDLE,
    ':approved' => ROVAS_CIVIC_CHARITY_STATUS_APPROVED,
  ))->fetchField();

  $args = array(
    ':bundle' => ROVAS_CIVIC_CHARITY_BUNDLE,
    ':approved' => ROVAS_CIVIC_CHARITY_STATUS_APPROVED,
  );
  $search_sql = '';
  if ($has_query) {
    $search_sql = ' AND (n.title LIKE :like OR org.title LIKE :like) ';
    $args[':like'] = '%' . db_like($query) . '%';
  }

  $rows = db_query_range("
    SELECT n.nid, org.title AS org_title, COALESCE(me.field_merit_reward_value, 0) AS total_me
    FROM {node} n
    INNER JOIN {field_data_field_is_charity} ic
      ON ic.entity_id = n.nid AND ic.entity_type = 'node' AND ic.deleted = 0
      AND ic.field_is_charity_value = 1
    INNER JOIN {field_data_field_charity_organization} ref
      ON ref.entity_id = n.nid AND ref.entity_type = 'node' AND ref.deleted = 0
    INNER JOIN {node} org ON org.nid = ref.field_charity_organization_target_id
      AND org.type = :bundle AND org.status = 1
    INNER JOIN {field_data_field_charity_status} st
      ON st.entity_id = org.nid AND st.entity_type = 'node' AND st.deleted = 0
      AND st.field_charity_status_value = :approved
    LEFT JOIN {field_data_field_merit_reward} me
      ON me.entity_id = n.nid AND me.entity_type = 'node' AND me.deleted = 0
    WHERE n.type = 'project' AND n.status = 1
    $search_sql
    ORDER BY total_me DESC, n.nid DESC
  ", 0, $limit, $args);

  $projects = array();
  foreach ($rows as $row) {
    $project = node_load((int) $row->nid);
    if (!$project) {
      continue;
    }
    $title = rovas_civic_node_localized_title($project, $lang);
    $org_name = trim((string) $row->org_title);
    if ($org_name === '') {
      $org_name = $title;
    }
    $me = isset($row->total_me) ? (float) $row->total_me : 0.0;
    $projects[] = array(
      'project_nid' => (string) $project->nid,
      'title' => $title,
      'organization' => $org_name,
      'summary' => rovas_civic_plain_teaser(rovas_civic_node_body_value($project, $lang)),
      'total_me' => $me,
      'rovas_url' => url('node/' . $project->nid, array('absolute' => TRUE)),
    );
  }

  return array(
    'total' => $total,
    'projects' => $projects,
  );
}

/**
 * Public enrollment page: /charity-projects
 */
function rovas_civic_charity_projects_page() {
  $lang = $GLOBALS['language']->language;
  $is_sk = (drupal_strtolower(substr($lang, 0, 2)) === 'sk' || drupal_strtolower(substr($lang, 0, 2)) === 'cs');

  if ($is_sk) {
    $title = 'Charitatívne projekty na Rovase';
    drupal_set_title($title);
    $intro = '<p>Rovas umožňuje registrovaným charitám a verejnoprospešným organizáciám zverejniť projekty v aplikácii Civic — bez poplatkov za zápis. Darovanie v eurách a súvisiace Merit prídu neskôr; teraz ide o objavenie projektov a zápis organizácie.</p>';
    $steps_title = 'Ako sa zapojiť';
    $steps = array(
      'Vytvorte projekt na Rovase (alebo použite existujúci).',
      'Pri úprave projektu zaškrtnite „Toto je projekt registrovanej charity“.',
      'Vytvorte alebo vyberte organizáciu (právny názov, IČO, kontakt, IBAN) — údaje zadáte raz a môžete ich použiť na viacerých projektoch.',
      'Pošlite na schválenie NeoFundu. Kým nie je organizácia schválená, projekt sa v Civic katalógu nezobrazí a EUR platby sú blokované.',
      'Ďalšie projekty pripojte k tej istej organizácii — nemusíte znova vypĺňať IČO ani účet.',
      'Neskôr: platobná integrácia a voliteľný widget na vašom webe.',
    );
    $cta = '<p>' . l(t('Create a project'), 'node/add/project') . ' · ' . l(t('Register a charity organization'), 'node/add/' . ROVAS_CIVIC_CHARITY_BUNDLE) . '</p>';
  }
  else {
    $title = 'Charitable projects on Rovas';
    drupal_set_title($title);
    $intro = '<p>Rovas lets registered charities and public-benefit organizations list projects in the Civic app — no listing fee. EUR donations and related Merit come later; this step is discovery and enrollment only.</p>';
    $steps_title = 'How to enroll';
    $steps = array(
      'Create a Rovas project (or use an existing one).',
      'On the project form, check “This is a project of a registered charity.”',
      'Create or select a charity organization (legal name, registration id, contact, IBAN) — enter once, reuse across projects.',
      'Wait for NeoFund approval. Until the organization is approved, the project stays out of the Civic catalog and EUR rails stay blocked.',
      'Attach further projects to the same organization — no need to re-enter legal or bank details.',
      'Later: payment integration and an optional site widget.',
    );
    $cta = '<p>' . l(t('Create a project'), 'node/add/project') . ' · ' . l(t('Register a charity organization'), 'node/add/' . ROVAS_CIVIC_CHARITY_BUNDLE) . '</p>';
  }

  $list = '<ol>';
  foreach ($steps as $step) {
    $list .= '<li>' . check_plain($step) . '</li>';
  }
  $list .= '</ol>';

  return array(
    '#type' => 'markup',
    '#markup' => $intro . '<h2>' . check_plain($steps_title) . '</h2>' . $list . $cta,
  );
}

/**
 * Form alter: project charitable subsection + #states.
 */
function rovas_civic_form_project_node_form_alter(&$form, &$form_state) {
  if (!isset($form['field_is_charity'])) {
    return;
  }

  $form['field_is_charity'][LANGUAGE_NONE]['#title'] = t('This is a project of a registered charity');
  $form['field_is_charity'][LANGUAGE_NONE]['#description'] = t('When checked, link a charity organization. NeoFund must approve the organization before the project appears in Civic and before EUR donations or share purchases are allowed.');

  // options_onoff uses …[und][value]; also accept radios-style name.
  $visible = array(
    ':input[name="field_is_charity[' . LANGUAGE_NONE . '][value]"]' => array('checked' => TRUE),
  );

  foreach (array(
    'field_charity_organization',
    'field_charity_donate_text',
    'field_charity_donate_url',
  ) as $fname) {
    if (isset($form[$fname])) {
      $form[$fname]['#states'] = array('visible' => $visible);
    }
  }

  if (isset($form['field_charity_organization'])) {
    $form['field_charity_organization'][LANGUAGE_NONE][0]['target_id']['#description'] = t('Select an organization you manage, or !create first.', array(
      '!create' => l(t('create a charity organization'), 'node/add/' . ROVAS_CIVIC_CHARITY_BUNDLE, array(
        'query' => array('destination' => current_path()),
      )),
    ));
  }

  $form['#validate'][] = 'rovas_civic_project_node_form_validate';
}

/**
 * Form alter: charity_organization — hide approval from non-staff.
 */
function rovas_civic_form_charity_organization_node_form_alter(&$form, &$form_state) {
  $can_approve = user_access('administer rovas civic');
  if (isset($form['field_charity_status'])) {
    if ($can_approve) {
      $form['field_charity_status'][LANGUAGE_NONE]['#description'] = t('Only NeoFund staff should set Approved. Owners leave this as Pending.');
    }
    else {
      $form['field_charity_status']['#access'] = FALSE;
    }
  }
  if (isset($form['title'])) {
    $form['title']['#title'] = t('Legal name');
    $form['title']['#description'] = t('Official registered name of the charity / public-benefit organization.');
  }
}

/**
 * Validate: charitable projects must link an organization.
 */
function rovas_civic_project_charity_form_validate_impl($form, &$form_state) {
  $values = $form_state['values'];
  $is_charity = 0;
  if (isset($values['field_is_charity'][LANGUAGE_NONE][0]['value'])) {
    $is_charity = (int) $values['field_is_charity'][LANGUAGE_NONE][0]['value'];
  }
  if (!$is_charity) {
    return;
  }
  $tid = 0;
  if (!empty($values['field_charity_organization'][LANGUAGE_NONE][0]['target_id'])) {
    $raw = $values['field_charity_organization'][LANGUAGE_NONE][0]['target_id'];
    // Autocomplete may pass "Title (nid)".
    if (is_numeric($raw)) {
      $tid = (int) $raw;
    }
    elseif (preg_match('/\((\d+)\)\s*$/', (string) $raw, $m)) {
      $tid = (int) $m[1];
    }
  }
  if ($tid < 1) {
    form_set_error('field_charity_organization', t('Select or create a charity organization for this charitable project.'));
    return;
  }
  $org = node_load($tid);
  if (!$org || $org->type !== ROVAS_CIVIC_CHARITY_BUNDLE) {
    form_set_error('field_charity_organization', t('Invalid charity organization.'));
    return;
  }
  global $user;
  if (!user_access('administer rovas civic') && (int) $org->uid !== (int) $user->uid) {
    form_set_error('field_charity_organization', t('You may only link charity organizations you own.'));
  }
}

/**
 * Presave: force pending status for non-staff; default pending on insert.
 */
function rovas_civic_charity_node_presave($node) {
  if ($node->type === ROVAS_CIVIC_CHARITY_BUNDLE) {
    $can_approve = user_access('administer rovas civic');
    $current = NULL;
    if (!empty($node->field_charity_status[LANGUAGE_NONE][0]['value'])) {
      $current = $node->field_charity_status[LANGUAGE_NONE][0]['value'];
    }
    if (!$can_approve) {
      // Owners cannot self-approve; keep previous status if editing.
      if (!empty($node->original) && !empty($node->nid)) {
        $prev = field_get_items('node', $node->original, 'field_charity_status');
        $prev_status = !empty($prev[0]['value']) ? $prev[0]['value'] : ROVAS_CIVIC_CHARITY_STATUS_PENDING;
        $node->field_charity_status[LANGUAGE_NONE][0]['value'] = $prev_status;
      }
      else {
        $node->field_charity_status[LANGUAGE_NONE][0]['value'] = ROVAS_CIVIC_CHARITY_STATUS_PENDING;
      }
    }
    elseif ($current === NULL || $current === '') {
      $node->field_charity_status[LANGUAGE_NONE][0]['value'] = ROVAS_CIVIC_CHARITY_STATUS_PENDING;
    }
  }

  if ($node->type === 'project') {
    $flag = !empty($node->field_is_charity[LANGUAGE_NONE][0]['value']);
    if (!$flag) {
      $node->field_charity_organization[LANGUAGE_NONE] = array();
    }
  }
}

/**
 * Form alter: block EUR on rewpro when charity org is pending (A12).
 */
function rovas_civic_form_rovas_project_reward_form_alter(&$form, &$form_state) {
  $paytype = isset($form_state['values']['paytype'])
    ? $form_state['values']['paytype']
    : (isset($form['paytype']['#value']) ? $form['paytype']['#value'] : '');
  $recipient = isset($form_state['values']['recipient'])
    ? $form_state['values']['recipient']
    : (isset($form['recipient']['#value']) ? $form['recipient']['#value'] : 0);

  if (!in_array($paytype, array('project', 'buyshares'), TRUE) || !is_numeric($recipient)) {
    return;
  }
  $project = node_load((int) $recipient);
  if (!rovas_civic_project_blocks_eur_rails($project)) {
    return;
  }

  $form['rovas_civic_eur_blocked'] = array(
    '#type' => 'markup',
    '#markup' => '<div class="messages warning">' . t('EUR donations and EUR share purchases are unavailable until NeoFund approves this project’s charity organization. You can still pay with Chrons.') . '</div>',
    '#weight' => -50,
  );
  $form['#validate'][] = 'rovas_civic_rewpro_form_validate';
}

/**
 * Validate: refuse EUR when charity org pending.
 */
function rovas_civic_rewpro_eur_charity_validate_impl($form, &$form_state) {
  $paytype = isset($form_state['values']['paytype']) ? $form_state['values']['paytype'] : '';
  $currency = isset($form_state['values']['payment-currency']) ? $form_state['values']['payment-currency'] : 'CHR';
  $pid = isset($form_state['values']['recipient']) ? $form_state['values']['recipient'] : 0;
  if ($currency !== 'EUR' || !in_array($paytype, array('project', 'buyshares'), TRUE) || !is_numeric($pid)) {
    return;
  }
  $project = node_load((int) $pid);
  if (rovas_civic_project_blocks_eur_rails($project)) {
    form_set_error('payment', t('EUR payments are blocked until NeoFund approves this charity organization.'));
  }
}
