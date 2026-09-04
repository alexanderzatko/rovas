<?php

/**
 * @file
 * Admin forms: national hourly rates + State Treasury project.
 */

/**
 * Form: national hourly rates + State Treasury nid.
 */
function rovas_civic_admin_form($form, &$form_state) {
  $treasury_nid = (int) variable_get('rovas_civic_state_treasury_nid', 0);
  $treasury = $treasury_nid ? node_load($treasury_nid) : FALSE;

  $form['treasury'] = array(
    '#type' => 'fieldset',
    '#title' => t('State Treasury project'),
  );
  $form['treasury']['status'] = array(
    '#type' => 'item',
    '#title' => t('Current project'),
    '#markup' => $treasury
      ? t('Nid @nid: !link', array(
        '@nid' => $treasury_nid,
        '!link' => l(check_plain($treasury->title), 'node/' . $treasury_nid),
      ))
      : t('Not set — click Bootstrap below.'),
  );
  $form['treasury']['rovas_civic_state_treasury_nid'] = array(
    '#type' => 'textfield',
    '#title' => t('State Treasury project nid'),
    '#default_value' => $treasury_nid ? $treasury_nid : '',
    '#description' => t('Override if the project already exists. Leave empty and use Bootstrap to create one.'),
    '#size' => 10,
  );
  $form['treasury']['bootstrap'] = array(
    '#type' => 'submit',
    '#value' => t('Bootstrap State Treasury project'),
    '#submit' => array('rovas_civic_admin_bootstrap_treasury_submit'),
  );

  $form['rates'] = array(
    '#type' => 'fieldset',
    '#title' => t('National hourly rates (EUR / hour)'),
    '#description' => t('Used for pass-through charity donor Merit and Civic VAT claims: ME = (eur_slice / rate) × 10.'),
  );

  $rows = db_select('rovas_national_hourly_rate', 'r')
    ->fields('r')
    ->orderBy('country_code')
    ->execute();
  $header = array(t('Country'), t('Rate (EUR/h)'), t('Source'), t('Updated'), t('Operations'));
  $table_rows = array();
  foreach ($rows as $row) {
    $table_rows[] = array(
      check_plain($row->country_code),
      check_plain(number_format((float) $row->rate_eur, 4, '.', '')),
      check_plain($row->source),
      $row->updated ? format_date($row->updated, 'short') : '—',
      l(t('edit'), 'admin/config/rovas/civic/rate/' . $row->country_code . '/edit'),
    );
  }
  $form['rates']['table'] = array(
    '#markup' => theme('table', array(
      'header' => $header,
      'rows' => $table_rows,
      'empty' => t('No rates defined.'),
    )),
  );
  $form['rates']['add'] = array(
    '#markup' => '<p>' . l(t('Add country rate'), 'admin/config/rovas/civic/rate/add') . '</p>',
  );

  module_load_include('inc', 'rovas_civic', 'rovas_civic.merit_index');
  $built = (int) variable_get(ROVAS_CIVIC_MERIT_INDEX_SNAPSHOT_BUILT_VAR, 0);
  $interval = function_exists('rovas_civic_merit_index_snapshot_interval')
    ? rovas_civic_merit_index_snapshot_interval()
    : ROVAS_CIVIC_MERIT_INDEX_SNAPSHOT_INTERVAL;
  $count = 0;
  if (function_exists('rovas_civic_merit_index_snapshot_load')) {
    $snap = rovas_civic_merit_index_snapshot_load();
    if (!empty($snap['items']) && is_array($snap['items'])) {
      $count = count($snap['items']);
    }
  }

  $form['merit_index'] = array(
    '#type' => 'fieldset',
    '#title' => t('Merit Index snapshot (Civic Prínos)'),
    '#description' => t('Civic Merit Index reads from a cached snapshot. Cron rebuilds it every @hours hours (site cron runs more often). Rebuild after large Merit changes if needed sooner.', array(
      '@hours' => max(1, (int) round($interval / 3600)),
    )),
  );
  $form['merit_index']['status'] = array(
    '#type' => 'item',
    '#title' => t('Last rebuilt'),
    '#markup' => $built
      ? t('@when (@count users)', array(
        '@when' => format_date($built, 'short'),
        '@count' => $count,
      ))
      : t('Never — rebuild now or wait for cron.'),
  );
  $form['merit_index']['rovas_civic_merit_index_snapshot_interval_hours'] = array(
    '#type' => 'textfield',
    '#title' => t('Rebuild interval (hours)'),
    '#default_value' => max(1, (int) round($interval / 3600)),
    '#size' => 4,
    '#description' => t('Minimum hours between automatic cron rebuilds (default 12).'),
  );
  $form['merit_index']['rebuild'] = array(
    '#type' => 'submit',
    '#value' => t('Rebuild Merit Index snapshot'),
    '#submit' => array('rovas_civic_admin_merit_index_rebuild_submit'),
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['save'] = array(
    '#type' => 'submit',
    '#value' => t('Save settings'),
    '#submit' => array('rovas_civic_admin_form_submit'),
  );
  $form['actions']['resources'] = array(
    '#type' => 'submit',
    '#value' => t('Enable Civic VAT claim + claim-read + rate + users + project APIs'),
    '#submit' => array('rovas_civic_admin_resources_submit'),
  );

  return $form;
}

/**
 * Save Treasury nid + Merit Index snapshot interval.
 */
function rovas_civic_admin_form_submit($form, &$form_state) {
  $nid = (int) $form_state['values']['rovas_civic_state_treasury_nid'];
  if ($nid > 0) {
    $node = node_load($nid);
    if (!$node || $node->type !== 'project') {
      form_set_error('rovas_civic_state_treasury_nid', t('Nid must be a project node.'));
      return;
    }
    variable_set('rovas_civic_state_treasury_nid', $nid);
    drupal_set_message(t('State Treasury project nid saved.'));
  }
  else {
    variable_del('rovas_civic_state_treasury_nid');
    drupal_set_message(t('State Treasury nid cleared.'));
  }

  if (isset($form_state['values']['rovas_civic_merit_index_snapshot_interval_hours'])) {
    $hours = (int) $form_state['values']['rovas_civic_merit_index_snapshot_interval_hours'];
    if ($hours < 1) {
      form_set_error('rovas_civic_merit_index_snapshot_interval_hours', t('Interval must be at least 1 hour.'));
      return;
    }
    variable_set(ROVAS_CIVIC_MERIT_INDEX_SNAPSHOT_INTERVAL_VAR, $hours * 3600);
    drupal_set_message(t('Merit Index snapshot interval set to @h hours.', array('@h' => $hours)));
  }
}

/**
 * Force-rebuild Merit Index snapshot (admin).
 */
function rovas_civic_admin_merit_index_rebuild_submit($form, &$form_state) {
  module_load_include('inc', 'rovas_civic', 'rovas_civic.merit_index');
  $started = microtime(TRUE);
  $n = rovas_civic_merit_index_snapshot_rebuild();
  $secs = round(microtime(TRUE) - $started, 2);
  drupal_set_message(t('Merit Index snapshot rebuilt: @count users in @s s.', array(
    '@count' => $n,
    '@s' => $secs,
  )));
}

/**
 * Bootstrap submit.
 */
function rovas_civic_admin_bootstrap_treasury_submit($form, &$form_state) {
  $nid = rovas_civic_ensure_state_treasury();
  if ($nid) {
    drupal_set_message(t('State Treasury project ready (nid @nid).', array('@nid' => $nid)));
  }
  else {
    drupal_set_message(t('Could not create State Treasury project.'), 'error');
  }
}

/**
 * Enable Services Civic resources on rovas_endpoint (claims, rates, users, project, shareholders, merit history).
 */
function rovas_civic_admin_resources_submit($form, &$form_state) {
  module_load_include('inc', 'rovas_civic', 'rovas_civic.charity_setup');
  if (function_exists('rovas_civic_ensure_charity_content')) {
    rovas_civic_ensure_charity_content();
  }
  if (rovas_civic_ensure_services_resources()) {
    drupal_set_message(t('Civic APIs enabled on rovas_endpoint (claims, rates, users, project, charity catalog, shareholders, merit history).'));
  }
  else {
    drupal_set_message(t('Could not update Services endpoint.'), 'error');
  }
}

/**
 * Add / edit a national hourly rate.
 */
function rovas_civic_rate_form($form, &$form_state, $country_code = NULL) {
  $existing = NULL;
  if ($country_code) {
    $existing = db_query("SELECT * FROM {rovas_national_hourly_rate} WHERE country_code = :c", array(
      ':c' => drupal_strtoupper($country_code),
    ))->fetchObject();
    if (!$existing) {
      drupal_not_found();
      drupal_exit();
    }
  }

  $form['country_code'] = array(
    '#type' => 'textfield',
    '#title' => t('Country code'),
    '#description' => t('ISO 3166-1 alpha-2 (e.g. SK).'),
    '#default_value' => $existing ? $existing->country_code : '',
    '#required' => TRUE,
    '#size' => 4,
    '#maxlength' => 2,
    '#disabled' => (bool) $existing,
  );
  $form['rate_eur'] = array(
    '#type' => 'textfield',
    '#title' => t('Hourly rate (EUR)'),
    '#default_value' => $existing ? $existing->rate_eur : '',
    '#required' => TRUE,
  );
  $form['source'] = array(
    '#type' => 'textfield',
    '#title' => t('Source'),
    '#default_value' => $existing ? $existing->source : 'manual',
    '#maxlength' => 255,
  );
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  $form['#existing'] = $existing;

  return $form;
}

/**
 * Validate rate form.
 */
function rovas_civic_rate_form_validate($form, &$form_state) {
  $code = drupal_strtoupper(trim($form_state['values']['country_code']));
  if (!preg_match('/^[A-Z]{2}$/', $code)) {
    form_set_error('country_code', t('Country code must be two letters.'));
  }
  $rate = $form_state['values']['rate_eur'];
  if (!is_numeric($rate) || (float) $rate <= 0) {
    form_set_error('rate_eur', t('Rate must be a positive number.'));
  }
  $form_state['values']['country_code'] = $code;
}

/**
 * Submit rate form.
 */
function rovas_civic_rate_form_submit($form, &$form_state) {
  db_merge('rovas_national_hourly_rate')
    ->key(array('country_code' => $form_state['values']['country_code']))
    ->fields(array(
      'rate_eur' => (float) $form_state['values']['rate_eur'],
      'source' => trim($form_state['values']['source']),
      'updated' => REQUEST_TIME,
    ))
    ->execute();
  drupal_set_message(t('National hourly rate saved for @c.', array(
    '@c' => $form_state['values']['country_code'],
  )));
  $form_state['redirect'] = 'admin/config/rovas/civic';
}
