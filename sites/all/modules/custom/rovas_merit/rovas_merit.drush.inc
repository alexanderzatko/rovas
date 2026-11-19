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
    'options' => array(
      'reclassify' => dt('Also re-infer sources already set (EUR vs CHR purchases; project-reward co_worker vs timeshare shares).'),
    ),
    'examples' => array(
      'drush rovas-merit-backfill' => dt('Backfill missing merit sources.'),
      'drush rovas-merit-backfill --reclassify' => dt('Re-infer all ledger sources.'),
    ),
  );
  $items['rovas-merit-reconcile'] = array(
    'callback' => 'drush_rovas_merit_reconcile',
    'description' => dt('Check or fix Merit invariant drift (total_me vs user_expertise vs ledger).'),
    'options' => array(
      'fix' => dt('Clamp bogus 94.99 negatives, then credit missing expertise on 64.99 (or debit real legacy penalties on 94.99).'),
      'uid' => dt('Limit to one uid.'),
      'expertise-only' => dt('Only report/fix wallet vs user_expertise drift (ignore legacy ledger gaps).'),
    ),
    'examples' => array(
      'drush rovas-merit-reconcile' => dt('Report users with drift.'),
      'drush rovas-merit-reconcile --expertise-only' => dt('Report expertise drift only.'),
      'drush rovas-merit-reconcile --fix' => dt('Fix expertise drift per user.'),
      'drush rovas-merit-reconcile --uid=3767 --fix' => dt('Fix one user.'),
    ),
  );
  $items['rovas-merit-sync-roles'] = array(
    'callback' => 'drush_rovas_merit_sync_roles',
    'description' => dt('Sync full-access / banned roles to Merit balance (negative → banned).'),
    'options' => array(
      'fix' => dt('Apply role changes (default is report only).'),
      'uid' => dt('Limit to one uid.'),
      'notify' => dt('Send privileges-lost mail when demoting (only with --fix).'),
    ),
    'examples' => array(
      'drush rovas-merit-sync-roles' => dt('Report mismatches.'),
      'drush rovas-merit-sync-roles --fix' => dt('Fix all mismatches.'),
      'drush rovas-merit-sync-roles --uid=3853 --fix --notify' => dt('Fix one user and email.'),
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
  $reclassify = (bool) drush_get_option('reclassify', FALSE);
  $count = _rovas_merit_backfill_sources($reclassify);
  drush_log(dt('Backfilled field_merit_source on @count ledger node(s).', array(
    '@count' => $count,
  )), 'success');
}

/**
 * Drush callback: Merit reconcile report / fix.
 */
function drush_rovas_merit_reconcile() {
  module_load_include('inc', 'rovas_merit', 'rovas_merit.ledger');
  $fix = (bool) drush_get_option('fix', FALSE);
  $only_uid = drush_get_option('uid', NULL);
  $expertise_only = (bool) drush_get_option('expertise-only', FALSE);

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
    $bogus = rovas_merit_repair_bogus_penalty_expertise($uid, FALSE);
    $expertise_problem = empty($check['expertise_ok']) || !empty($bogus['applied_delta']);
    $ledger_problem = empty($check['ledger_ok']);
    if ($expertise_only) {
      $needs_attention = $expertise_problem;
    }
    else {
      $needs_attention = !$check['ok'] || !empty($bogus['applied_delta']);
    }
    if (!$needs_attention) {
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
    if ($expertise_problem && $ledger_problem) {
      drush_print(dt('  (expertise + ledger drift)'));
    }
    elseif ($ledger_problem && !$expertise_problem) {
      drush_print(dt('  (ledger-only legacy drift; --fix does not rewrite ledger)'));
    }
    if (!empty($bogus['applied_delta'])) {
      drush_print(dt('  bogus 94.99 expertise=@b expected=@x (would adjust by @d)', array(
        '@b' => round($bogus['before'], 4),
        '@x' => round($bogus['expected'], 4),
        '@d' => round($bogus['applied_delta'], 4),
      )));
    }
    if ($fix) {
      if (!$expertise_problem) {
        continue;
      }
      $result = rovas_merit_reconcile_user($uid, TRUE);
      if (!empty($result['fixed'])) {
        $fixed++;
        if (!empty($result['repair']['clamped'])) {
          drush_print(dt('  clamped 94.99 by @d', array(
            '@d' => round($result['repair']['applied_delta'], 4),
          )));
        }
        if (isset($result['fix_delta'])) {
          drush_print(dt('  adjusted nace @n by @d', array(
            '@n' => !empty($result['fix_nace']) ? $result['fix_nace'] : '?',
            '@d' => round($result['fix_delta'], 4),
          )));
        }
        if (!empty($result['after'])) {
          drush_print(dt('  after: total=@t expertise=@e ledger=@l expertise_ok=@eo ledger_ok=@lo', array(
            '@t' => round($result['after']['total_me'], 2),
            '@e' => round($result['after']['expertise_sum'], 2),
            '@l' => round($result['after']['ledger_sum'], 2),
            '@eo' => !empty($result['after']['expertise_ok']) ? 'yes' : 'no',
            '@lo' => !empty($result['after']['ledger_ok']) ? 'yes' : 'no',
          )));
        }
      }
      elseif (!empty($result['error'])) {
        drush_print(dt('  error: @e', array('@e' => $result['error'])));
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

/**
 * Drush callback: sync full-access / banned to Merit balance.
 */
function drush_rovas_merit_sync_roles() {
  module_load_include('inc', 'rovas_merit', 'rovas_merit.ledger');
  $fix = (bool) drush_get_option('fix', FALSE);
  $notify = (bool) drush_get_option('notify', FALSE);
  $only_uid = drush_get_option('uid', NULL);

  $ids = rovas_merit_access_role_ids();
  $rid_full = $ids['full_access'];
  $rid_banned = $ids['banned'];

  if ($only_uid) {
    $uids = array((int) $only_uid);
  }
  else {
    // Candidates: negative Merits without banned, or banned with non-negative.
    $uids = db_query("
      SELECT DISTINCT u.uid
      FROM {users} u
      INNER JOIN {userpoints} up ON up.uid = u.uid
      LEFT JOIN {users_roles} banned ON banned.uid = u.uid AND banned.rid = :banned
      LEFT JOIN {field_data_field_merit_decimal_part} d
        ON d.entity_id = u.uid AND d.entity_type = 'user' AND d.deleted = 0
      WHERE u.uid > 0
        AND (
          ((up.points + COALESCE(d.field_merit_decimal_part_value, 0)) < 0 AND banned.uid IS NULL)
          OR
          ((up.points + COALESCE(d.field_merit_decimal_part_value, 0)) >= 0 AND banned.uid IS NOT NULL)
        )
      ORDER BY u.uid
    ", array(':banned' => $rid_banned))->fetchCol();
  }

  $mismatch = 0;
  $changed = 0;
  foreach ($uids as $uid) {
    $uid = (int) $uid;
    $account = user_load($uid);
    if (!$account) {
      continue;
    }
    $total = rovas_merit_user_total_me($uid);
    $has_full = isset($account->roles[$rid_full]);
    $has_banned = isset($account->roles[$rid_banned]);
    $expected = ($total < 0) ? 'banned' : 'full_access';
    if ($has_banned && !$has_full) {
      $actual = 'banned';
    }
    elseif ($has_full && !$has_banned) {
      $actual = 'full_access';
    }
    elseif ($has_banned && $has_full) {
      $actual = 'both';
    }
    else {
      $actual = 'neither';
    }
    $needs = ($total < 0 && (!$has_banned || $has_full))
      || ($total >= 0 && (!$has_full || $has_banned));
    if (!$needs) {
      if ($only_uid) {
        drush_print(dt('uid @uid (@name): total=@t roles ok (@a).', array(
          '@uid' => $uid,
          '@name' => $account->name,
          '@t' => round($total, 4),
          '@a' => $actual,
        )));
      }
      continue;
    }
    $mismatch++;
    drush_print(dt('uid @uid (@name): total=@t actual=@a expected=@e', array(
      '@uid' => $uid,
      '@name' => $account->name,
      '@t' => round($total, 4),
      '@a' => $actual,
      '@e' => $expected,
    )));
    if ($fix) {
      $result = rovas_merit_sync_access_role($uid, $notify && ($expected === 'banned'));
      if ($result) {
        $changed++;
        drush_print(dt('  -> synced to @r', array('@r' => $result)));
      }
      else {
        drush_print(dt('  -> no change'));
      }
    }
  }

  if ($mismatch === 0) {
    drush_log(dt('No full-access / banned role mismatches found.'), 'success');
  }
  else {
    drush_log(dt('@n mismatch(es); @c changed.', array(
      '@n' => $mismatch,
      '@c' => $changed,
    )), $fix ? 'success' : 'warning');
  }
}
