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
 *
 * Internal Civic API may set $node->rovas_civic_force_charity_status to
 * approved|pending after SK register verification (owners still cannot
 * self-approve via the node form).
 */
function rovas_civic_charity_node_presave($node) {
  if ($node->type === ROVAS_CIVIC_CHARITY_BUNDLE) {
    $forced = !empty($node->rovas_civic_force_charity_status)
      ? (string) $node->rovas_civic_force_charity_status
      : '';
    if ($forced === ROVAS_CIVIC_CHARITY_STATUS_APPROVED
      || $forced === ROVAS_CIVIC_CHARITY_STATUS_PENDING) {
      $node->field_charity_status[LANGUAGE_NONE][0]['value'] = $forced;
    }
    else {
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

/* ===========================================================================
 * Donor-add: POST /rovas/civic_create_charity_org
 * ========================================================================= */

/**
 * JSON error for Civic charity create (same shape as volunteer endpoints).
 */
function rovas_civic_charity_error($http, $error, $message) {
  return services_error($message, (int) $http, array(
    'error' => $error,
    'message' => $message,
  ));
}

/**
 * Custodial NeoFund/system uid for Civic-submitted orgs.
 *
 * @return int
 */
function rovas_civic_charity_custodial_uid() {
  $uid = (int) variable_get(ROVAS_CIVIC_CHARITY_CUSTODIAL_UID_VAR, 0);
  if ($uid > 0) {
    $account = user_load($uid);
    if ($account && !empty($account->uid)) {
      return (int) $account->uid;
    }
  }
  return 1;
}

/**
 * Ownership mode for an org relative to the custodial uid.
 *
 * @param object $org
 *
 * @return string
 *   custodial|submitter
 */
function rovas_civic_charity_ownership_mode($org) {
  $custodial = rovas_civic_charity_custodial_uid();
  if (!empty($org->uid) && (int) $org->uid === $custodial) {
    return 'custodial';
  }
  return 'submitter';
}

/**
 * Find charity_organization by country + org_id (IČO).
 *
 * @param string $country
 * @param string $org_id
 *
 * @return object|null
 */
function rovas_civic_charity_find_by_country_org_id($country, $org_id) {
  $country = drupal_strtoupper(trim((string) $country));
  $org_id = trim((string) $org_id);
  if ($country === '' || $org_id === '') {
    return NULL;
  }
  $nid = (int) db_query("
    SELECT n.nid
    FROM {node} n
    INNER JOIN {field_data_field_charity_country} c
      ON c.entity_id = n.nid AND c.entity_type = 'node' AND c.deleted = 0
    INNER JOIN {field_data_field_charity_ico} i
      ON i.entity_id = n.nid AND i.entity_type = 'node' AND i.deleted = 0
    WHERE n.type = :type
      AND c.field_charity_country_value = :country
      AND i.field_charity_ico_value = :org_id
    ORDER BY n.nid ASC
    LIMIT 1
  ", array(
    ':type' => ROVAS_CIVIC_CHARITY_BUNDLE,
    ':country' => $country,
    ':org_id' => $org_id,
  ))->fetchField();
  if ($nid < 1) {
    return NULL;
  }
  $org = node_load($nid);
  if (!$org || $org->type !== ROVAS_CIVIC_CHARITY_BUNDLE) {
    return NULL;
  }
  return $org;
}

/**
 * First published project linked to an org, or 0.
 *
 * @param int $org_nid
 *
 * @return int
 */
function rovas_civic_charity_linked_project_nid($org_nid) {
  $org_nid = (int) $org_nid;
  if ($org_nid < 1) {
    return 0;
  }
  return (int) db_query("
    SELECT n.nid
    FROM {node} n
    INNER JOIN {field_data_field_is_charity} ic
      ON ic.entity_id = n.nid AND ic.entity_type = 'node' AND ic.deleted = 0
      AND ic.field_is_charity_value = 1
    INNER JOIN {field_data_field_charity_organization} ref
      ON ref.entity_id = n.nid AND ref.entity_type = 'node' AND ref.deleted = 0
      AND ref.field_charity_organization_target_id = :org
    WHERE n.type = 'project' AND n.status = 1
    ORDER BY n.nid ASC
    LIMIT 1
  ", array(':org' => $org_nid))->fetchField();
}

/**
 * Basic SEPA IBAN check (normalize + length + mod-97).
 *
 * @param string $iban
 *
 * @return string|false
 *   Normalized IBAN or FALSE.
 */
function rovas_civic_normalize_iban($iban) {
  $iban = drupal_strtoupper(preg_replace('/\s+/', '', (string) $iban));
  if (!preg_match('/^[A-Z]{2}[0-9]{2}[A-Z0-9]{11,30}$/', $iban)) {
    return FALSE;
  }
  $len = strlen($iban);
  if ($len < 15 || $len > 34) {
    return FALSE;
  }
  $rearranged = substr($iban, 4) . substr($iban, 0, 4);
  $numeric = '';
  $chars = str_split($rearranged);
  foreach ($chars as $ch) {
    if ($ch >= 'A' && $ch <= 'Z') {
      $numeric .= (string) (ord($ch) - 55);
    }
    else {
      $numeric .= $ch;
    }
  }
  // Chunked mod 97 for arbitrary-length IBAN.
  $checksum = 0;
  $chunks = str_split($numeric, 7);
  foreach ($chunks as $chunk) {
    $checksum = (int) ($checksum . $chunk) % 97;
  }
  if ($checksum !== 1) {
    return FALSE;
  }
  return $iban;
}

/**
 * Absolute http(s) URL check.
 *
 * @param string $url
 *
 * @return string|false
 */
function rovas_civic_normalize_http_url($url) {
  $url = trim((string) $url);
  if ($url === '' || drupal_strlen($url) > 512) {
    return FALSE;
  }
  if (!preg_match('#^https?://#i', $url)) {
    return FALSE;
  }
  $parts = @parse_url($url);
  if (empty($parts['scheme']) || empty($parts['host'])) {
    return FALSE;
  }
  $scheme = drupal_strtolower($parts['scheme']);
  if ($scheme !== 'http' && $scheme !== 'https') {
    return FALSE;
  }
  return $url;
}

/**
 * Contact email from a charity_organization node.
 *
 * @param object $org
 *
 * @return string|null
 */
function rovas_civic_charity_org_contact_email($org) {
  $email_items = field_get_items('node', $org, 'field_charity_contact_email');
  if (!empty($email_items[0]['email'])) {
    return trim((string) $email_items[0]['email']);
  }
  if (!empty($email_items[0]['value'])) {
    return trim((string) $email_items[0]['value']);
  }
  return NULL;
}

/**
 * SK RPO eligible source-register codes (CL010112) for auto-approve.
 *
 * 4 foundations, 5 non-investment funds, 6 n.o., 7 o.z., 9 international orgs.
 *
 * @return array
 */
function rovas_civic_sk_rpo_eligible_register_codes() {
  return array('4', '5', '6', '7', '9');
}

/**
 * Look up a Slovak IČO in the public RPO API.
 *
 * @param string $org_id
 *
 * @return array
 *   keys: ok (bool), eligible (bool), legal_name (string|null),
 *   source_register_code, source_register_label, error (string|null)
 */
function rovas_civic_sk_rpo_lookup($org_id) {
  $org_id = trim((string) $org_id);
  $out = array(
    'ok' => FALSE,
    'eligible' => FALSE,
    'legal_name' => NULL,
    'source_register_code' => NULL,
    'source_register_label' => NULL,
    'error' => NULL,
  );
  if ($org_id === '') {
    $out['error'] = 'empty_org_id';
    return $out;
  }

  $url = ROVAS_CIVIC_SK_RPO_SEARCH_URL . '?' . drupal_http_build_query(array(
    'identifier' => $org_id,
    'onlyActive' => 'true',
  ));
  $response = drupal_http_request($url, array(
    'headers' => array('Accept' => 'application/json'),
    'timeout' => 12,
  ));
  if (!empty($response->error) || (int) $response->code < 200 || (int) $response->code >= 300) {
    $out['error'] = 'register_outage';
    watchdog('rovas_civic', 'SK RPO lookup failed for @ico: @err (HTTP @code)', array(
      '@ico' => $org_id,
      '@err' => !empty($response->error) ? $response->error : 'http',
      '@code' => isset($response->code) ? $response->code : 0,
    ), WATCHDOG_WARNING);
    return $out;
  }

  $data = drupal_json_decode($response->data);
  if (!is_array($data) || empty($data['results']) || !is_array($data['results'])) {
    $out['ok'] = TRUE;
    $out['error'] = 'not_found';
    return $out;
  }

  $matches = array();
  foreach ($data['results'] as $row) {
    if (!is_array($row)) {
      continue;
    }
    $identifiers = isset($row['identifiers']) ? $row['identifiers'] : array();
    $ico_match = FALSE;
    if (is_array($identifiers)) {
      foreach ($identifiers as $id_row) {
        if (!is_array($id_row)) {
          continue;
        }
        if (isset($id_row['validTo']) && $id_row['validTo'] !== NULL && $id_row['validTo'] !== '') {
          continue;
        }
        if (isset($id_row['value']) && (string) $id_row['value'] === $org_id) {
          $ico_match = TRUE;
          break;
        }
      }
      // Fallback: first identifier value.
      if (!$ico_match && !empty($identifiers[0]['value']) && (string) $identifiers[0]['value'] === $org_id) {
        $ico_match = TRUE;
      }
    }
    if (!$ico_match) {
      continue;
    }
    $matches[] = $row;
  }

  if (count($matches) === 0) {
    $out['ok'] = TRUE;
    $out['error'] = 'not_found';
    return $out;
  }
  if (count($matches) > 1) {
    $out['ok'] = TRUE;
    $out['error'] = 'ambiguous';
    return $out;
  }

  $row = $matches[0];
  $out['ok'] = TRUE;
  $src = isset($row['sourceRegister']['value']) ? $row['sourceRegister']['value'] : array();
  $code = isset($src['code']) ? (string) $src['code'] : '';
  $label = isset($src['value']) ? (string) $src['value'] : '';
  $out['source_register_code'] = $code !== '' ? $code : NULL;
  $out['source_register_label'] = $label !== '' ? $label : NULL;
  $out['eligible'] = in_array($code, rovas_civic_sk_rpo_eligible_register_codes(), TRUE);

  $names = isset($row['fullNames']) && is_array($row['fullNames']) ? $row['fullNames'] : array();
  $legal = NULL;
  foreach ($names as $name_row) {
    if (!is_array($name_row) || empty($name_row['value'])) {
      continue;
    }
    if (isset($name_row['validTo']) && $name_row['validTo'] !== NULL && $name_row['validTo'] !== '') {
      continue;
    }
    $legal = trim((string) $name_row['value']);
    break;
  }
  if ($legal === NULL && !empty($names[0]['value'])) {
    $legal = trim((string) $names[0]['value']);
  }
  $out['legal_name'] = ($legal !== NULL && $legal !== '') ? $legal : NULL;
  return $out;
}

/**
 * Soften ALL-CAPS legal names from registers / clients for display titles.
 *
 * Slovak RPO often returns "SLOVENSKO V POHYBE"; we keep mixed-case names as-is.
 *
 * @param string $name
 *
 * @return string
 */
function rovas_civic_normalize_org_display_name($name) {
  $name = trim(preg_replace('/\s+/u', ' ', (string) $name));
  if ($name === '') {
    return $name;
  }
  $letters = preg_replace('/[^\p{L}]/u', '', $name);
  if ($letters === '') {
    return $name;
  }
  // Only rewrite when every letter is uppercase (and there is at least one
  // cased letter — avoid changing already sentence-case or lowercase strings).
  if ($letters === drupal_strtoupper($letters)
    && $letters !== drupal_strtolower($letters)) {
    return drupal_ucfirst(drupal_strtolower($name));
  }
  return $name;
}

/**
 * Default body for a Civic-created charitable project.
 *
 * @param string $org_title
 * @param string|null $langcode
 *   Locale for t(); NULL uses the current interface language.
 *
 * @return string
 *   filtered_html markup (includes mailto link).
 */
function rovas_civic_charity_default_project_description($org_title, $langcode = NULL) {
  $org_title = trim((string) $org_title);
  $options = array();
  if ($langcode !== NULL && $langcode !== '') {
    $options['langcode'] = $langcode;
  }
  if ($org_title === '') {
    $org_title = t('Nonprofit project', array(), $options);
  }
  $contact = variable_get('site_mail', '');
  if ($contact === '' || $contact === NULL) {
    $contact = 'rovas@merit.world';
  }
  $mail_link = '<a href="mailto:' . check_plain($contact) . '">' . check_plain($contact) . '</a>';
  return t('Nonprofit or charity project of organization @org, added by a donor/sponsor of this organization. If you represent this organization, send a message to !mail and we will change the owner to you.', array(
    '@org' => $org_title,
    '!mail' => $mail_link,
  ), $options);
}

/**
 * Enabled content language codes (lowercase), keyed by langcode.
 *
 * @return array
 */
function rovas_civic_charity_enabled_langcodes() {
  $out = array();
  if (!function_exists('language_list')) {
    return $out;
  }
  foreach (language_list() as $code => $lang) {
    if (!empty($lang->enabled)) {
      $out[drupal_strtolower($code)] = drupal_strtolower($code);
    }
  }
  return $out;
}

/**
 * Write per-language body (and title_field) so locale switching shows SK/IT/….
 *
 * Create-time t() alone freezes the request language into body; Entity
 * Translation needs body[$lang] + translation records for each language.
 *
 * @param object $project
 * @param string $org_title
 */
function rovas_civic_charity_seed_project_description_translations($project, $org_title) {
  if (empty($project) || empty($project->nid) || $project->type !== 'project') {
    return;
  }
  $org_title = trim((string) $org_title);
  if ($org_title === '') {
    $org_title = t('Nonprofit project');
  }

  $enabled = rovas_civic_charity_enabled_langcodes();
  if (empty($enabled)) {
    $enabled = array('en' => 'en');
  }
  $source = isset($enabled['en']) ? 'en' : reset($enabled);

  $body_field = field_info_field('body');
  $body_translatable = !empty($body_field['translatable']);
  $title_field = field_info_field('title_field');
  $title_translatable = !empty($title_field['translatable']);

  $has_et = $body_translatable
    && module_exists('entity_translation')
    && function_exists('entity_translation_enabled')
    && entity_translation_enabled('node', $project)
    && function_exists('entity_translation_get_handler');

  $project->language = $source;
  unset($project->body[LANGUAGE_NONE]);

  $langs_to_write = $body_translatable ? $enabled : array($source => $source);
  foreach ($langs_to_write as $code) {
    $project->body[$code][0] = array(
      'value' => rovas_civic_charity_default_project_description($org_title, $code),
      'summary' => '',
      'format' => 'filtered_html',
    );
    if ($title_translatable) {
      $project->title_field[$code][0] = array(
        'value' => $org_title,
        'format' => '',
        'safe_value' => check_plain($org_title),
      );
    }
  }
  $project->title = $org_title;

  $handler = NULL;
  if ($has_et) {
    $handler = entity_translation_get_handler('node', $project);
    if ($handler && method_exists($handler, 'setOriginalLanguage')) {
      $handler->setOriginalLanguage($source);
    }
    foreach ($enabled as $code) {
      if ($code === $source) {
        continue;
      }
      if (!$handler) {
        break;
      }
      $handler->setTranslation(array(
        'translate' => 0,
        'status' => 1,
        'language' => $code,
        'source' => $source,
        'uid' => !empty($project->uid) ? (int) $project->uid : 1,
        'created' => !empty($project->created) ? (int) $project->created : REQUEST_TIME,
        'changed' => REQUEST_TIME,
      ));
    }
  }

  $prev_title = NULL;
  if (function_exists('rovas_base_translation_begin_presave_title_context')) {
    $prev_title = rovas_base_translation_begin_presave_title_context($source);
  }
  node_save($project);
  if ($handler && method_exists($handler, 'saveTranslations')) {
    $handler->saveTranslations();
  }
  if (function_exists('rovas_base_translation_end_presave_title_context')) {
    rovas_base_translation_end_presave_title_context($prev_title);
  }
}

/**
 * Create or return a default charitable project linked to an org.
 *
 * Exactly one shareholder: the project owner (custodial Rovas uid, or the
 * submitter when contact_email matched their account). Public-project insert
 * Rules attach site:current-user, so create runs as that owner.
 *
 * @param object $org
 * @param int $owner_uid
 * @param string $website_url
 *
 * @return int
 *   project nid or 0
 */
function rovas_civic_charity_ensure_default_project($org, $owner_uid, $website_url = '') {
  $existing = rovas_civic_charity_linked_project_nid((int) $org->nid);
  if ($existing > 0) {
    return $existing;
  }

  $owner_uid = (int) $owner_uid;
  $author = user_load($owner_uid);
  if (!$author) {
    return 0;
  }
  $title = trim((string) $org->title);
  if ($title === '') {
    $title = t('Nonprofit project');
  }
  // English source for the initial Rules/create body; ET seed overwrites per locale.
  $description = rovas_civic_charity_default_project_description($title, 'en');
  $website_url = trim((string) $website_url);

  // Public-project Rules use site:current-user for the initial coworker row.
  // Run as the intended sole owner so the Bearer donor is not also attached.
  global $user;
  $previous_user = $user;
  $previous_session = drupal_save_session();
  drupal_save_session(FALSE);
  $user = $author;

  $nid = 0;
  try {
    if (function_exists('rules_invoke_component') && rules_config_load('rules_create_project')) {
      module_load_include('inc', 'rovas_civic', 'rovas_civic.volunteer');
      $result = rules_invoke_component(
        'rules_create_project',
        $title,
        $author,
        $website_url,
        0,
        1,
        1,
        $description,
        FALSE
      );
      $nid = function_exists('rovas_civic_volunteer_nid_from_rules')
        ? rovas_civic_volunteer_nid_from_rules($result)
        : 0;
    }

    if ($nid < 1) {
      $node = new stdClass();
      $node->type = 'project';
      $node->title = $title;
      $node->language = 'en';
      $node->uid = $owner_uid;
      $node->status = 1;
      $node->body['en'][0] = array(
        'value' => $description,
        'summary' => '',
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
      if ($website_url !== '' && field_info_field('field_asset_web_address')) {
        $node->field_asset_web_address[LANGUAGE_NONE][0]['value'] = $website_url;
      }
      node_object_prepare($node);
      node_save($node);
      $nid = (int) $node->nid;
    }

    if ($nid < 1) {
      return 0;
    }

    $project = node_load($nid);
    if (!$project) {
      return 0;
    }
    $project->uid = $owner_uid;
    $project->title = $title;
    if (field_info_field('field_is_charity')) {
      $project->field_is_charity[LANGUAGE_NONE][0]['value'] = 1;
    }
    if (field_info_field('field_charity_organization')) {
      $project->field_charity_organization[LANGUAGE_NONE][0]['target_id'] = (int) $org->nid;
    }
    if (field_info_field('field_status')) {
      $project->field_status[LANGUAGE_NONE][0]['value'] = 1;
    }
    if (field_info_field('field_participation')) {
      $project->field_participation[LANGUAGE_NONE][0]['value'] = 1;
    }
    node_save($project);

    // Idempotent: insert Rules usually already attached current-user (= owner).
    if (function_exists('rules_invoke_component') && rules_config_load('rules_check_or_add_shareholder')) {
      rules_invoke_component('rules_check_or_add_shareholder', $owner_uid, $nid);
    }

    // Reload then seed body per enabled locale (SK/IT/… via locale + ET).
    $project = node_load($nid);
    if ($project) {
      rovas_civic_charity_seed_project_description_translations($project, $title);
    }
  }
  finally {
    $user = $previous_user;
    drupal_save_session($previous_session);
  }

  return $nid;
}

/**
 * Interface language for the claim email to an org contact.
 *
 * Prefers Civic request/body lang, then org country (SK/CZ→sk, IT→it), else
 * site default. Recipient is the org, not the donor.
 *
 * @param object $org
 * @param string|null $preferred
 *   Optional lang from Civic (?lang= or body lang).
 *
 * @return string
 *   Enabled Drupal langcode.
 */
function rovas_civic_charity_claim_mail_langcode($org, $preferred = NULL) {
  $candidate = NULL;
  if ($preferred !== NULL && $preferred !== '') {
    $candidate = function_exists('rovas_civic_request_lang')
      ? rovas_civic_request_lang($preferred)
      : drupal_strtolower(trim((string) $preferred));
  }
  if ($candidate === NULL || $candidate === '') {
    $country = drupal_strtoupper(trim((string) rovas_civic_node_text_field($org, 'field_charity_country')));
    $by_country = array(
      'SK' => 'sk',
      'CZ' => 'sk',
      'IT' => 'it',
    );
    if (isset($by_country[$country])) {
      $candidate = $by_country[$country];
    }
  }
  if ($candidate === NULL || $candidate === '') {
    $default = language_default();
    $candidate = !empty($default->language) ? $default->language : 'en';
  }
  if (function_exists('rovas_base_normalize_to_enabled_language')) {
    return rovas_base_normalize_to_enabled_language($candidate);
  }
  return $candidate;
}

/**
 * Email contact_email inviting the org to claim / correct the project.
 *
 * @param object $org
 * @param int $project_nid
 * @param string $ownership
 * @param string|null $lang
 *   Optional Civic UI lang (sk|en); country is used when omitted.
 * @param object|null $submitter
 *   Civic donor user who added the organization (for @username in the body).
 *
 * @return bool
 */
function rovas_civic_notify_charity_org_claim($org, $project_nid, $ownership = 'custodial', $lang = NULL, $submitter = NULL) {
  if (empty($org) || empty($org->nid) || $org->type !== ROVAS_CIVIC_CHARITY_BUNDLE) {
    return FALSE;
  }
  $to = rovas_civic_charity_org_contact_email($org);
  if (empty($to) || !valid_email_address($to)) {
    watchdog('rovas_civic', 'Charity claim email skipped for org @nid: missing contact_email.', array(
      '@nid' => (int) $org->nid,
    ), WATCHDOG_WARNING);
    return FALSE;
  }

  $langcode = rovas_civic_charity_claim_mail_langcode($org, $lang);
  $t_opts = array('langcode' => $langcode);

  $username = (is_object($submitter) && !empty($submitter->name))
    ? '@' . $submitter->name
    : t('a Civic user', array(), $t_opts);

  $catalog_url = url('charity-projects', array('absolute' => TRUE));
  $project_url = $project_nid
    ? url('node/' . (int) $project_nid, array('absolute' => TRUE))
    : $catalog_url;
  $org_url = url('node/' . (int) $org->nid, array('absolute' => TRUE));
  $neofund_url = 'https://neofund.sk';

  $lines = array(
    t('Hello,', array(), $t_opts),
    '',
    t('We are NeoFund, a Slovak nonprofit. We develop solutions that aim to increase contributions to the volunteer, nonprofit, and charitable sector.', array(), $t_opts),
    '',
    t('At the request of user @username of our Rovas app, we today added your organization @org to the application and also created a project for it. These two steps are necessary so that our users can send you money and at the same time be rewarded by us for it.', array(
      '@username' => $username,
      '@org' => $org->title,
    ), $t_opts),
    '',
    t('Your organization page: @url', array('@url' => $org_url), $t_opts),
    t('The project we created for you: @url', array('@url' => $project_url), $t_opts),
    '',
    t('You can claim this project on Rovas (log in or register) to take ownership and add or correct information such as donation instructions. Please reply to this email if you wish to do so.', array(), $t_opts),
    '',
    t('Learn more about <a href="!url">how this works</a>.', array('!url' => $catalog_url), $t_opts),
    '',
    'NeoFund n.f.',
    $neofund_url,
  );

  $from = variable_get('site_mail', $to);

  drupal_mail(
    'rovas_civic',
    'charity_org_claim',
    $to,
    $langcode,
    array(
      'subject' => t('Rovas: Your organization was added — @title', array(
        '@title' => $org->title,
      ), $t_opts),
      'body' => implode("\n", $lines),
    ),
    $from
  );

  watchdog('rovas_civic', 'Sent charity claim email for org @nid to @to (lang=@lang).', array(
    '@nid' => (int) $org->nid,
    '@to' => $to,
    '@lang' => $langcode,
  ), WATCHDOG_NOTICE);

  return TRUE;
}

/**
 * Queue an async AI accuracy check for a newly created org.
 *
 * @param object $org
 * @param array $submitted
 */
function rovas_civic_charity_queue_ai_check($org, array $submitted) {
  if (empty($org->nid)) {
    return;
  }
  try {
    $queue = DrupalQueue::get('rovas_civic_charity_ai_check');
    $queue->createItem(array(
      'organization_nid' => (int) $org->nid,
      'submitted' => $submitted,
      'created' => REQUEST_TIME,
    ));
  }
  catch (Exception $e) {
    watchdog('rovas_civic', 'Failed to queue charity AI check for org @nid: @msg', array(
      '@nid' => (int) $org->nid,
      '@msg' => $e->getMessage(),
    ), WATCHDOG_WARNING);
  }
}

/**
 * Process queued AI accuracy check (does not change approval status).
 *
 * @param mixed $item
 */
function rovas_civic_charity_ai_check_process($item) {
  if (!is_array($item) || empty($item['organization_nid'])) {
    return;
  }
  $nid = (int) $item['organization_nid'];
  $org = node_load($nid);
  if (!$org || $org->type !== ROVAS_CIVIC_CHARITY_BUNDLE) {
    return;
  }
  if (!function_exists('rovas_ai_chat') || !function_exists('rovas_ai_is_configured') || !rovas_ai_is_configured()) {
    watchdog('rovas_civic', 'Charity AI check skipped for org @nid (rovas_ai unavailable).', array(
      '@nid' => $nid,
    ), WATCHDOG_NOTICE);
    return;
  }

  $submitted = isset($item['submitted']) && is_array($item['submitted']) ? $item['submitted'] : array();
  $payload = array(
    'legal_name' => $org->title,
    'country' => rovas_civic_node_text_field($org, 'field_charity_country'),
    'org_id' => rovas_civic_node_text_field($org, 'field_charity_ico'),
    'website_url' => rovas_civic_node_text_field($org, 'field_charity_website'),
    'contact_email' => rovas_civic_charity_org_contact_email($org),
    'iban' => rovas_civic_node_text_field($org, 'field_charity_iban'),
    'submitted' => $submitted,
    'status' => rovas_civic_charity_org_status($org),
  );

  $system = 'You review public-benefit / nonprofit organization submissions for Rovas/Civic. '
    . 'Compare entered fields for obvious inconsistencies (name vs website domain, country vs IBAN prefix, etc.). '
    . 'Reply with JSON only: {"ok":true|false,"issues":["string"],"summary":"string"}. '
    . 'Do not invent legal facts; flag only clear mismatches.';
  $messages = array(
    array(
      'role' => 'system',
      'content' => $system,
    ),
    array(
      'role' => 'user',
      'content' => drupal_json_encode($payload),
    ),
  );
  $chat = rovas_ai_chat($messages, array('task' => 'charity_org_check', 'temperature' => 0));
  if (empty($chat['ok'])) {
    watchdog('rovas_civic', 'Charity AI check failed for org @nid: @err', array(
      '@nid' => $nid,
      '@err' => isset($chat['error']) ? $chat['error'] : 'unknown',
    ), WATCHDOG_WARNING);
    return;
  }

  $parsed = NULL;
  if (function_exists('rovas_ai_parse_json_object')) {
    module_load_include('inc', 'rovas_ai', 'rovas_ai.classify');
    $parsed = rovas_ai_parse_json_object(isset($chat['content']) ? $chat['content'] : '');
  }
  $ok = is_array($parsed) && !empty($parsed['ok']);
  $summary = is_array($parsed) && isset($parsed['summary']) ? (string) $parsed['summary'] : '';
  $issues = (is_array($parsed) && !empty($parsed['issues']) && is_array($parsed['issues']))
    ? implode('; ', $parsed['issues'])
    : '';

  watchdog('rovas_civic', 'Charity AI check for org @nid: ok=@ok summary=@s issues=@i', array(
    '@nid' => $nid,
    '@ok' => $ok ? '1' : '0',
    '@s' => $summary !== '' ? $summary : '—',
    '@i' => $issues !== '' ? $issues : '—',
  ), $ok ? WATCHDOG_NOTICE : WATCHDOG_WARNING);

  if (!$ok) {
    $staff = variable_get('site_mail', '');
    if ($staff !== '' && $staff !== NULL) {
      $lang = language_default();
      $langcode = !empty($lang->language) ? $lang->language : 'en';
      drupal_mail(
        'rovas_civic',
        'charity_org_pending',
        $staff,
        $langcode,
        array(
          'subject' => t('Rovas: AI flagged charity org data — @title', array('@title' => $org->title)),
          'body' => implode("\n", array(
            t('AI accuracy check flagged issues for a public-benefit / nonprofit organization (approval status unchanged).'),
            t('Node: @nid', array('@nid' => $nid)),
            t('Legal name: @title', array('@title' => $org->title)),
            t('Summary: @s', array('@s' => $summary !== '' ? $summary : '—')),
            t('Issues: @i', array('@i' => $issues !== '' ? $issues : '—')),
            t('Edit: @url', array('@url' => url('node/' . $nid . '/edit', array('absolute' => TRUE)))),
          )),
        ),
        $staff
      );
    }
  }
}

/**
 * Services callback: POST /rovas/civic_create_charity_org
 *
 * @param array $data
 *
 * @return array
 */
function rovas_civic_services_create_charity_org($data) {
  $uid = rovas_civic_request_uid();
  if ($uid < 1) {
    return rovas_civic_charity_error(401, 'unauthorized', 'Authentication required');
  }
  if (!is_array($data)) {
    $data = array();
  }

  $name = isset($data['name']) ? trim((string) $data['name']) : '';
  $country = isset($data['country']) ? drupal_strtoupper(trim((string) $data['country'])) : '';
  $website_url = isset($data['website_url']) ? trim((string) $data['website_url']) : '';
  $contact_email = isset($data['contact_email']) ? trim((string) $data['contact_email']) : '';
  $org_id = isset($data['org_id']) ? trim((string) $data['org_id']) : '';
  $iban_raw = isset($data['iban']) ? (string) $data['iban'] : '';

  if ($name === '' || $country === '' || $website_url === '' || $contact_email === '' || $org_id === '' || trim($iban_raw) === '') {
    return rovas_civic_charity_error(400, 'invalid_input', 'name, country, website_url, contact_email, org_id, and iban are required.');
  }
  if (drupal_strlen($name) > 255) {
    return rovas_civic_charity_error(400, 'invalid_input', 'name is too long.');
  }
  if (!preg_match('/^[A-Z]{2}$/', $country)) {
    return rovas_civic_charity_error(400, 'invalid_input', 'country must be ISO 3166-1 alpha-2.');
  }
  $website_url = rovas_civic_normalize_http_url($website_url);
  if ($website_url === FALSE) {
    return rovas_civic_charity_error(400, 'invalid_input', 'website_url must be an absolute http(s) URL.');
  }
  if (!valid_email_address($contact_email)) {
    return rovas_civic_charity_error(400, 'invalid_input', 'contact_email is not a valid email address.');
  }
  if (drupal_strlen($org_id) > 64) {
    return rovas_civic_charity_error(400, 'invalid_input', 'org_id is too long.');
  }
  $iban = rovas_civic_normalize_iban($iban_raw);
  if ($iban === FALSE) {
    return rovas_civic_charity_error(400, 'invalid_input', 'iban must be a valid SEPA IBAN.');
  }

  $submitter = user_load($uid);
  if (!$submitter) {
    return rovas_civic_charity_error(401, 'unauthorized', 'Authentication required');
  }

  // Duplicate short-circuit (idempotent 200).
  $existing = rovas_civic_charity_find_by_country_org_id($country, $org_id);
  if ($existing) {
    $project_nid = rovas_civic_charity_ensure_default_project($existing, (int) $existing->uid, $website_url);
    if ($project_nid < 1) {
      return rovas_civic_charity_error(502, 'rovas_error', 'Organization exists but linked project could not be ensured.');
    }
    $status = rovas_civic_charity_org_status($existing);
    return array(
      'ok' => TRUE,
      'status' => $status ? $status : ROVAS_CIVIC_CHARITY_STATUS_PENDING,
      'created' => FALSE,
      'organization_nid' => (string) (int) $existing->nid,
      'project_nid' => (string) $project_nid,
      'organization' => trim((string) $existing->title),
      'ownership' => rovas_civic_charity_ownership_mode($existing),
    );
  }

  $custodial_uid = rovas_civic_charity_custodial_uid();
  $ownership = 'custodial';
  $owner_uid = $custodial_uid;
  if (!empty($submitter->mail)
    && drupal_strtolower(trim($submitter->mail)) === drupal_strtolower($contact_email)) {
    $ownership = 'submitter';
    $owner_uid = $uid;
  }

  $status = ROVAS_CIVIC_CHARITY_STATUS_PENDING;
  $register_name = NULL;
  if ($country === 'SK') {
    $lookup = rovas_civic_sk_rpo_lookup($org_id);
    if (!empty($lookup['ok']) && !empty($lookup['eligible'])) {
      $status = ROVAS_CIVIC_CHARITY_STATUS_APPROVED;
      if (!empty($lookup['legal_name'])) {
        $register_name = $lookup['legal_name'];
      }
    }
    // Outage / not found / ambiguous → stay pending (still create).
  }

  // Prefer register legal name when SK-verified; soften ALL-CAPS RPO strings.
  $legal_name = rovas_civic_normalize_org_display_name(
    $register_name !== NULL ? $register_name : $name
  );

  $org = new stdClass();
  $org->type = ROVAS_CIVIC_CHARITY_BUNDLE;
  $org->title = $legal_name;
  $org->language = LANGUAGE_NONE;
  $org->uid = $owner_uid;
  $org->status = 1;
  $org->rovas_civic_force_charity_status = $status;
  $org->field_charity_status[LANGUAGE_NONE][0]['value'] = $status;
  if (field_info_field('field_charity_ico')) {
    $org->field_charity_ico[LANGUAGE_NONE][0]['value'] = $org_id;
  }
  if (field_info_field('field_charity_country')) {
    $org->field_charity_country[LANGUAGE_NONE][0]['value'] = $country;
  }
  if (field_info_field('field_charity_website')) {
    $org->field_charity_website[LANGUAGE_NONE][0]['value'] = $website_url;
  }
  if (field_info_field('field_charity_iban')) {
    $org->field_charity_iban[LANGUAGE_NONE][0]['value'] = $iban;
  }
  if (field_info_field('field_charity_contact_email')) {
    $field = field_info_field('field_charity_contact_email');
    if (!empty($field['type']) && $field['type'] === 'email') {
      $org->field_charity_contact_email[LANGUAGE_NONE][0]['email'] = $contact_email;
    }
    else {
      $org->field_charity_contact_email[LANGUAGE_NONE][0]['value'] = $contact_email;
    }
  }
  node_object_prepare($org);
  // Preserve forced status after prepare.
  $org->rovas_civic_force_charity_status = $status;
  $org->field_charity_status[LANGUAGE_NONE][0]['value'] = $status;
  $org->uid = $owner_uid;

  try {
    node_save($org);
  }
  catch (Exception $e) {
    watchdog('rovas_civic', 'civic_create_charity_org node_save failed: @msg', array(
      '@msg' => $e->getMessage(),
    ), WATCHDOG_ERROR);
    return rovas_civic_charity_error(502, 'rovas_error', 'Failed to create organization.');
  }

  if (empty($org->nid)) {
    return rovas_civic_charity_error(502, 'rovas_error', 'Failed to create organization.');
  }

  // Re-assert approval if register path won (presave may have been skipped wrongly).
  $saved_status = rovas_civic_charity_org_status($org);
  if ($status === ROVAS_CIVIC_CHARITY_STATUS_APPROVED && $saved_status !== ROVAS_CIVIC_CHARITY_STATUS_APPROVED) {
    $org->rovas_civic_force_charity_status = ROVAS_CIVIC_CHARITY_STATUS_APPROVED;
    $org->field_charity_status[LANGUAGE_NONE][0]['value'] = ROVAS_CIVIC_CHARITY_STATUS_APPROVED;
    node_save($org);
  }

  $project_nid = rovas_civic_charity_ensure_default_project($org, $owner_uid, $website_url);
  if ($project_nid < 1) {
    return rovas_civic_charity_error(502, 'rovas_error', 'Organization created but linked project failed.');
  }

  // Claim invite — especially important for custodial ownership.
  // Language: optional body/query lang, else org country (SK→sk).
  $mail_lang = NULL;
  if (isset($data['lang']) && $data['lang'] !== '') {
    $mail_lang = $data['lang'];
  }
  elseif (function_exists('rovas_civic_request_lang')) {
    $mail_lang = rovas_civic_request_lang();
  }
  if ($ownership === 'custodial' || $status === ROVAS_CIVIC_CHARITY_STATUS_APPROVED) {
    rovas_civic_notify_charity_org_claim($org, $project_nid, $ownership, $mail_lang, $submitter);
  }

  if ($status === ROVAS_CIVIC_CHARITY_STATUS_APPROVED) {
    rovas_civic_charity_queue_ai_check($org, array(
      'name' => $name,
      'country' => $country,
      'website_url' => $website_url,
      'contact_email' => $contact_email,
      'org_id' => $org_id,
      'iban' => $iban,
      'submitter_uid' => $uid,
      'register_name' => $register_name,
    ));
  }

  return array(
    'ok' => TRUE,
    'status' => rovas_civic_charity_org_status($org),
    'created' => TRUE,
    'organization_nid' => (string) (int) $org->nid,
    'project_nid' => (string) $project_nid,
    'organization' => trim((string) $org->title),
    'ownership' => $ownership,
  );
}
