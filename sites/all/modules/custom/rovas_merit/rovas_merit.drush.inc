<?php

/**
 * @file
 * Drush commands for Rovas Merit ledger reconcile.
 */

/**
 * Implements hook_drush_command().
 */
function rovas_merit_drush_command() {
  $items['rovas-merit-backfill'] = array(
    'callback' => 'drush_rovas_merit_backfill',
    'description' => dt('Backfill field_merit_source on ledger nodes (batched, low memory).'),
    'examples' => array(
      'drush rovas-merit-backfill' => dt('Backfill missing merit sources.'),
    ),
  );
  $items['rovas-merit-reconcile'] = array(
    'callback' => 'drush_rovas_merit_reconcile',
    'description' => dt('Check or fix Merit invariant drift (total_me vs user_expertise vs ledger).'),
    'options' => array(
      'fix' => dt('Apply expertise corrections on governance NACE for drifting users.'),
      'uid' => dt('Limit to one uid.'),
    ),
    'examples' => array(
      'drush rovas-merit-reconcile' => dt('Report users with drift.'),
      'drush rovas-merit-reconcile --fix' => dt('Fix expertise drift per user.'),
    ),
  );
  return $items;
}

/**
 * Drush callback: backfill field_merit_source on ledger nodes.
 */
function drush_rovas_merit_backfill() {
  module_load_include('install', 'rovas_merit');
  module_load_include('inc', 'rovas_merit', 'rovas_merit.ledger');
  _rovas_merit_ensure_merit_source_field();
  $count = _rovas_merit_backfill_sources();
  drush_log(dt('Backfilled field_merit_source on @count ledger node(s).', array(
    '@count' => $count,
  )), 'success');
}

/**
 * Drush callback: Merit reconcile report / fix.
 */
function drush_rovas_merit_reconcile() {
  module_load_include('inc', 'rovas_merit', 'rovas_merit.ledger');
  $fix = drush_get_option('fix', FALSE);
  $only_uid = drush_get_option('uid', NULL);

  if ($only_uid) {
    $uids = array((int) $only_uid);
  }
  else {
    $uids = db_query('SELECT uid FROM {users} WHERE uid > 0 AND status = 1 ORDER BY uid')->fetchCol();
  }

  $drift = 0;
  $fixed = 0;
  foreach ($uids as $uid) {
    $uid = (int) $uid;
    $check = rovas_merit_balance_check($uid);
    if ($check['ok']) {
      continue;
    }
    $drift++;
    $account = user_load($uid);
    drush_print(dt('uid @uid (@name): total=@t expertise=@e ledger=@l de=@de dl=@dl', array(
      '@uid' => $uid,
      '@name' => $account ? $account->name : '?',
      '@t' => round($check['total_me'], 2),
      '@e' => round($check['expertise_sum'], 2),
      '@l' => round($check['ledger_sum'], 2),
      '@de' => round($check['delta_expertise'], 4),
      '@dl' => round($check['delta_ledger'], 4),
    )));
    if ($fix) {
      $result = rovas_merit_reconcile_user($uid, TRUE);
      if (!empty($result['fixed'])) {
        $fixed++;
        drush_print(dt('  fixed expertise by @d', array('@d' => $result['fix_delta'])));
      }
    }
  }

  if ($drift === 0) {
    drush_log(dt('All checked users pass the Merit invariant.'), 'success');
  }
  else {
    drush_log(dt('@drift user(s) with drift; @fixed fixed.', array(
      '@drift' => $drift,
      '@fixed' => $fixed,
    )), $fix ? 'success' : 'warning');
  }
}
