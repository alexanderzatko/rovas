<?php

/**
 * @file
 * Public-benefit / nonprofit organizations + Civic catalog / enrollment.
 *
 * Content type charity_organization holds org identity; projects link via
 * field_charity_organization. Wire names stay charity_*; product framing is
 * Gifts / public-benefit. See docs/integration/civic.md.
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
 *
 * Audience: a public-benefit / nonprofit org (often linked here by a donor)
 * that is new to Rovas / Civic / Merit. Locale affects wording only — content
 * stays country-agnostic. SK/CS use accented Rováš.
 */
function rovas_civic_charity_projects_page() {
  $lang = drupal_strtolower(substr($GLOBALS['language']->language, 0, 2));

  if ($lang === 'sk') {
    $title = 'Neziskové projekty v Rováši';
    drupal_set_title($title);
    $body = ''
      . '<p>Ak vám niekto poslal odkaz na túto stránku, zvyčajne ide o darcu, ktorý chce podporiť vašu organizáciu a zároveň získať <strong>Merity</strong> — nefinančné uznanie za prínos spoločnosti. Merity nie sú peniaze; nedajú sa predať ani vymeniť za hotovosť. Vznikajú napríklad darom, dobrovoľníckou prácou alebo platením daní.</p>'
      . '<p><strong>Rováš</strong> je platforma, na ktorej organizácie vedú projekty. <strong>Civic</strong> je mobilná aplikácia, v ktorej darcovia projekty hľadajú a podporujú. <strong>NeoFund</strong> je verejnoprospešný fond, ktorý organizácie overuje a neskôr bude sprostredkúvať peňažné dary tak, aby darca dostal Merity a vaša organizácia prostriedky.</p>'
      . '<p><strong>Čo platí dnes:</strong> po overení NeoFundom sa váš projekt môže zobraziť v Civic a darcovia uvidia pokyny na darovanie (napríklad bankový prevod alebo odkaz na vašu darovaciu stránku). <strong>Čo príde neskôr:</strong> dary priamo cez NeoFund / Civic s automatickým udelením Meritov darcovi. Zápis organizácie a projektu je bez poplatku a pripraví vás na oboje.</p>'
      . '<h2>Kto sa môže zapísať</h2>'
      . '<p>Registrované verejnoprospešné a neziskové organizácie, ktoré môžu zákonne prijímať dary na spoločné dobro — bez ohľadu na krajinu sídla. Typicky ide o charity, občianske združenia, nadácie, fondy, cirkevné právnické osoby pri verejnoprospešnej činnosti, kultúrne a vzdelávacie neziskovky, environmentálne a advokačné organizácie a podobné subjekty.</p>'
      . '<p><strong>Nezahŕňa:</strong> jednotlivcov, politické strany a komerčný predaj.</p>'
      . '<h2>Ako sa zapísať</h2>'
      . '<ol>'
      . '<li>Vytvorte si účet v Rováši (ak ešte nemáte) a založte projekt — alebo použite existujúci.</li>'
      . '<li>Pri úprave projektu zaškrtnite „Toto je projekt registrovanej verejnoprospešnej (neziskovej) organizácie“.</li>'
      . '<li>Vytvorte alebo vyberte záznam organizácie (právny názov, registračné číslo, kontakt, IBAN). Údaje zadáte raz a môžete ich použiť na viacerých projektoch.</li>'
      . '<li>Počkajte na schválenie NeoFundom. Kým nie je organizácia schválená, projekt sa v Civic nezobrazí.</li>'
      . '<li>Po schválení doplňte pokyny na darovanie (text alebo URL), aby darcovia vedeli, ako vás podporiť už teraz. Ďalšie projekty pripojte k tej istej organizácii.</li>'
      . '</ol>';
  }
  elseif ($lang === 'cs') {
    $title = 'Neziskové projekty v Rováši';
    drupal_set_title($title);
    $body = ''
      . '<p>Pokud vám někdo poslal odkaz na tuto stránku, obvykle jde o dárce, který chce podpořit vaši organizaci a zároveň získat <strong>Merity</strong> — nefinanční uznání za přínos společnosti. Merity nejsou peníze; nedají se prodat ani vyměnit za hotovost. Vznikají například darem, dobrovolnickou prací nebo placením daní.</p>'
      . '<p><strong>Rováš</strong> je platforma, na které organizace vedou projekty. <strong>Civic</strong> je mobilní aplikace, ve které dárci projekty hledají a podporují. <strong>NeoFund</strong> je veřejně prospěšný fond, který organizace ověřuje a později bude zprostředkovávat peněžní dary tak, aby dárce dostal Merity a vaše organizace prostředky.</p>'
      . '<p><strong>Co platí dnes:</strong> po ověření NeoFundem se váš projekt může zobrazit v Civic a dárci uvidí pokyny k darování (například bankovní převod nebo odkaz na vaši darovací stránku). <strong>Co přijde později:</strong> dary přímo přes NeoFund / Civic s automatickým udělením Meritů dárci. Zápis organizace a projektu je bez poplatku a připraví vás na obojí.</p>'
      . '<h2>Kdo se může zapsat</h2>'
      . '<p>Registrované veřejně prospěšné a neziskové organizace, které mohou zákonně přijímat dary pro společné dobro — bez ohledu na zemi sídla. Typicky jde o charity, spolky, nadace, fondy, církevní právnické osoby při veřejně prospěšné činnosti, kulturní a vzdělávací neziskovky, environmentální a advokační organizace a podobné subjekty.</p>'
      . '<p><strong>Nezahrnuje:</strong> jednotlivce, politické strany a komerční prodej.</p>'
      . '<h2>Jak se zapsat</h2>'
      . '<ol>'
      . '<li>Vytvořte si účet v Rováši (pokud ještě nemáte) a založte projekt — nebo použijte existující.</li>'
      . '<li>Při úpravě projektu zaškrtněte „Toto je projekt registrované veřejně prospěšné (neziskové) organizace“.</li>'
      . '<li>Vytvořte nebo vyberte záznam organizace (právní název, registrační číslo, kontakt, IBAN). Údaje zadáte jednou a můžete je použít u více projektů.</li>'
      . '<li>Počkejte na schválení NeoFundem. Dokud není organizace schválena, projekt se v Civic nezobrazí.</li>'
      . '<li>Po schválení doplňte pokyny k darování (text nebo URL), aby dárci věděli, jak vás podpořit už teď. Další projekty připojte ke stejné organizaci.</li>'
      . '</ol>';
  }
  else {
    // English and other locales (country-agnostic).
    $title = 'Nonprofit projects on Rovas';
    drupal_set_title($title);
    $body = ''
      . '<p>If someone sent you this link, it is usually a donor who wants to support your organization and also earn <strong>Merits</strong> — non-financial recognition for contribution to the commons. Merits are not money; they cannot be sold or cashed out. People earn them by giving, volunteering, paying taxes, and similar contributions.</p>'
      . '<p><strong>Rovas</strong> is the platform where organizations list projects. <strong>Civic</strong> is the mobile app where donors discover and support those projects. <strong>NeoFund</strong> is the public-benefit foundation that verifies organizations and will later channel monetary gifts so the donor receives Merits and your organization receives the funds.</p>'
      . '<p><strong>What works today:</strong> after NeoFund verifies your organization, your project can appear in Civic and donors will see your donate instructions (for example a bank transfer or a link to your donation page). <strong>What comes later:</strong> gifts paid through NeoFund / Civic with Merits awarded to the donor automatically. Enrolling your organization and project is free and prepares you for both.</p>'
      . '<h2>Who can enroll</h2>'
      . '<p>Registered public-benefit and nonprofit organizations that may lawfully receive gifts for the commons — regardless of country of registration. Typical examples include charities, civic associations, foundations, funds, church legal entities for public-benefit activity, cultural and educational nonprofits, environmental and advocacy groups, and similar entities.</p>'
      . '<p><strong>Excluded:</strong> individuals, political parties, and commercial sales.</p>'
      . '<h2>How to enroll</h2>'
      . '<ol>'
      . '<li>Create a Rovas account (if you do not have one) and a project — or use an existing project.</li>'
      . '<li>On the project form, check “This is a project of a registered public-benefit organization (nonprofit).”</li>'
      . '<li>Create or select an organization record (legal name, registration number, contact, IBAN). Enter the details once and reuse them on further projects.</li>'
      . '<li>Wait for NeoFund approval. Until the organization is approved, the project does not appear in Civic.</li>'
      . '<li>After approval, add donate instructions (text or URL) so donors can support you already. Attach further projects to the same organization.</li>'
      . '</ol>';
  }

  $cta = '<p>' . l(t('Create a project'), 'node/add/project') . ' · ' . l(t('Register a public-benefit / nonprofit organization'), 'node/add/' . ROVAS_CIVIC_CHARITY_BUNDLE) . '</p>';

  return array(
    '#type' => 'markup',
    '#markup' => $body . $cta,
  );
}

/**
 * Form alter: project charitable subsection in a collapsible fieldset.
 */
function rovas_civic_form_project_node_form_alter(&$form, &$form_state) {
  if (!isset($form['field_is_charity'])) {
    return;
  }

  $is_charity = FALSE;
  if (!empty($form['field_is_charity'][LANGUAGE_NONE]['#default_value'])) {
    $is_charity = (bool) $form['field_is_charity'][LANGUAGE_NONE]['#default_value'];
  }
  elseif (!empty($form['#entity']->field_is_charity[LANGUAGE_NONE][0]['value'])) {
    $is_charity = TRUE;
  }

  $weight = isset($form['field_is_charity']['#weight'])
    ? $form['field_is_charity']['#weight']
    : 35;

  $form['field_is_charity'][LANGUAGE_NONE]['#title'] = t('This is a project of a registered public-benefit organization (nonprofit)');
  $form['field_is_charity'][LANGUAGE_NONE]['#description'] = t('When checked, link a public-benefit / nonprofit organization. NeoFund must approve the organization before the project appears in Civic and before EUR donations or share purchases are allowed.');

  // Same pattern as project ownership / purchasing-shares: collapsible fieldset
  // + #states on the dependent block (id selector matches options_onoff).
  $form['charity_project'] = array(
    '#type' => 'fieldset',
    '#title' => t('Public-benefit / nonprofit project'),
    '#collapsible' => TRUE,
    '#collapsed' => !$is_charity,
    '#weight' => $weight,
    '#attributes' => array('class' => array('field-group-fieldset')),
    '#tree' => FALSE,
  );

  $form['charity_project']['field_is_charity'] = $form['field_is_charity'];
  // Keep a top-level #language stub: locale_field_entity_form_submit() reads
  // $form[$field_name]['#language'] for every instance (Drupal core #2356283).
  $form['field_is_charity'] = array(
    '#language' => isset($form['charity_project']['field_is_charity']['#language'])
      ? $form['charity_project']['field_is_charity']['#language']
      : LANGUAGE_NONE,
  );

  // options_onoff renders id edit-field-is-charity-und (same as allow-purchasing-shares).
  $visible = array(
    ':input[id="edit-field-is-charity-und"]' => array('checked' => TRUE),
  );

  $form['charity_project']['charity_details'] = array(
    '#type' => 'container',
    '#states' => array(
      'visible' => $visible,
    ),
  );

  foreach (array(
    'field_charity_organization',
    'field_charity_donate_text',
    'field_charity_donate_url',
  ) as $fname) {
    if (isset($form[$fname])) {
      $form['charity_project']['charity_details'][$fname] = $form[$fname];
      $form[$fname] = array(
        '#language' => isset($form['charity_project']['charity_details'][$fname]['#language'])
          ? $form['charity_project']['charity_details'][$fname]['#language']
          : LANGUAGE_NONE,
      );
    }
  }

  if (isset($form['charity_project']['charity_details']['field_charity_organization'][LANGUAGE_NONE][0]['target_id'])) {
    $org_widget = &$form['charity_project']['charity_details']['field_charity_organization'][LANGUAGE_NONE][0]['target_id'];
    $org_widget['#title'] = t('Public-benefit / nonprofit organization');
    $org_widget['#description'] = t('Select an organization you manage, or !create first.', array(
      '!create' => l(t('create a public-benefit / nonprofit organization'), 'node/add/' . ROVAS_CIVIC_CHARITY_BUNDLE, array(
        'query' => array('destination' => current_path()),
      )),
    ));
  }

  if (isset($form['charity_project']['charity_details']['field_charity_donate_text'][LANGUAGE_NONE][0]['value'])) {
    $donate_text = &$form['charity_project']['charity_details']['field_charity_donate_text'][LANGUAGE_NONE][0]['value'];
    $donate_text['#title'] = t('Donate instructions (project override)');
    $donate_text['#description'] = t("Optional. Overrides the organization's default donate instructions for this project.");
  }

  if (isset($form['charity_project']['charity_details']['field_charity_donate_url'][LANGUAGE_NONE][0]['value'])) {
    $donate_url = &$form['charity_project']['charity_details']['field_charity_donate_url'][LANGUAGE_NONE][0]['value'];
    $donate_url['#title'] = t('Donate URL (project override)');
    $donate_url['#description'] = t("Optional absolute URL. Overrides the organization's default donate URL.");
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
      $form['field_charity_status'][LANGUAGE_NONE]['#description'] = t('Only NeoFund staff should set Approved. Review for public-benefit / nonprofit eligibility (not charity-only). Owners leave this as Pending.');
    }
    else {
      $form['field_charity_status']['#access'] = FALSE;
    }
  }
  if (isset($form['title'])) {
    $form['title']['#title'] = t('Legal name');
    $form['title']['#description'] = t('Official registered name of the public-benefit / nonprofit organization.');
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
    form_set_error('field_charity_organization', t('Select or create a public-benefit / nonprofit organization for this project.'));
    return;
  }
  $org = node_load($tid);
  if (!$org || $org->type !== ROVAS_CIVIC_CHARITY_BUNDLE) {
    form_set_error('field_charity_organization', t('Invalid public-benefit / nonprofit organization.'));
    return;
  }
  global $user;
  if (!user_access('administer rovas civic') && (int) $org->uid !== (int) $user->uid) {
    form_set_error('field_charity_organization', t('You may only link public-benefit / nonprofit organizations you own.'));
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
 * Email site mail when a charity_organization is created (pending review).
 *
 * Recipient and From use variable site_mail (default site email).
 *
 * @param object $node
 *   Newly inserted charity_organization node.
 *
 * @return bool
 */
function rovas_civic_notify_charity_org_pending($node) {
  if (empty($node) || empty($node->nid) || $node->type !== ROVAS_CIVIC_CHARITY_BUNDLE) {
    return FALSE;
  }

  $to = variable_get('site_mail', '');
  if ($to === '' || $to === NULL) {
    $su = user_load(1);
    if (!empty($su->mail)) {
      $to = $su->mail;
    }
  }
  if (empty($to)) {
    watchdog('rovas_civic', 'Charity org @nid pending but site_mail is empty; no review email sent.', array(
      '@nid' => (int) $node->nid,
    ), WATCHDOG_WARNING);
    return FALSE;
  }

  $owner = user_load((int) $node->uid);
  $owner_name = $owner ? format_username($owner) : t('Unknown');
  $owner_mail = $owner && !empty($owner->mail) ? $owner->mail : '—';

  $ico = rovas_civic_node_text_field($node, 'field_charity_ico');
  $country = rovas_civic_node_text_field($node, 'field_charity_country');
  $website = rovas_civic_node_text_field($node, 'field_charity_website');
  $contact = NULL;
  $email_items = field_get_items('node', $node, 'field_charity_contact_email');
  if (!empty($email_items[0]['email'])) {
    $contact = $email_items[0]['email'];
  }
  elseif (!empty($email_items[0]['value'])) {
    $contact = $email_items[0]['value'];
  }

  $admin_url = url('admin/config/rovas/civic/charities', array('absolute' => TRUE));
  $node_url = url('node/' . (int) $node->nid . '/edit', array('absolute' => TRUE));

  $lines = array(
    t('A new public-benefit / nonprofit organization was submitted and is pending NeoFund review.'),
    t('Confirm eligibility for the Gifts path (charities, o.z., n.o., foundations, etc. — not individuals, political parties, or commercial sales).'),
    '',
    t('Legal name: @title', array('@title' => $node->title)),
    t('Node: @nid', array('@nid' => (int) $node->nid)),
    t('Owner: @name (@mail)', array('@name' => $owner_name, '@mail' => $owner_mail)),
    t('IČO / registration: @v', array('@v' => $ico ? $ico : '—')),
    t('Country: @v', array('@v' => $country ? $country : '—')),
    t('Website: @v', array('@v' => $website ? $website : '—')),
    t('Contact email: @v', array('@v' => $contact ? $contact : '—')),
    '',
    t('Review list: @url', array('@url' => $admin_url)),
    t('Edit organization: @url', array('@url' => $node_url)),
  );

  $lang = language_default();
  $langcode = !empty($lang->language) ? $lang->language : 'en';
  $from = variable_get('site_mail', $to);

  drupal_mail(
    'rovas_civic',
    'charity_org_pending',
    $to,
    $langcode,
    array(
      'subject' => t('Rovas: New public-benefit / nonprofit organization pending review — @title', array(
        '@title' => $node->title,
      )),
      'body' => implode("\n", $lines),
    ),
    $from
  );

  watchdog('rovas_civic', 'Sent charity review email for org @nid to @to.', array(
    '@nid' => (int) $node->nid,
    '@to' => $to,
  ), WATCHDOG_NOTICE);

  return TRUE;
}

/**
 * Set charity organization approval status (staff).
 *
 * @param object $node
 * @param string $status
 *   pending|approved
 *
 * @return bool
 */
function rovas_civic_charity_org_set_status($node, $status) {
  if (empty($node) || $node->type !== ROVAS_CIVIC_CHARITY_BUNDLE) {
    return FALSE;
  }
  if ($status !== ROVAS_CIVIC_CHARITY_STATUS_PENDING && $status !== ROVAS_CIVIC_CHARITY_STATUS_APPROVED) {
    return FALSE;
  }
  $node->field_charity_status[LANGUAGE_NONE][0]['value'] = $status;
  node_save($node);
  return TRUE;
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
    '#markup' => '<div class="messages warning">' . t('EUR donations and EUR share purchases are unavailable until NeoFund approves this project’s public-benefit / nonprofit organization. You can still pay with Chrons.') . '</div>',
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
    form_set_error('payment', t('EUR payments are blocked until NeoFund approves this public-benefit / nonprofit organization.'));
  }
}
