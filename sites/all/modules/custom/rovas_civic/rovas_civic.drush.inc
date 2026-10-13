<?php

/**
 * @file
 * Drush commands for Rovas Civic.
 */

/**
 * Implements hook_drush_command().
 */
function rovas_civic_drush_command() {
  $items['rovas-civic-backfill-eur-buy-merits'] = array(
    'callback' => 'drush_rovas_civic_backfill_eur_buy_merits',
    'description' => dt('Backfill eur_buy_merits issuance for EUR purchase_merits → top_charities.'),
    'options' => array(
      'uid' => dt('Limit to one Drupal uid.'),
    ),
    'examples' => array(
      'drush rovas-civic-backfill-eur-buy-merits' => dt('Backfill all historical EUR buy-Merits attributions.'),
      'drush rovas-civic-backfill-eur-buy-merits --uid=3626' => dt('Backfill one user (e.g. @emenky).'),
    ),
  );
  return $items;
}

/**
 * Drush: backfill EUR buy-Merits A3 attribution rows.
 */
function drush_rovas_civic_backfill_eur_buy_merits() {
  $nid = rovas_civic_ensure_eur_buy_merits_project();
  if ($nid < 1) {
    return drush_set_error('ROVAS_CIVIC_BUY_ME_PROJECT', dt('EUR buy-Merits project nid is missing or invalid.'));
  }
  $only_uid = drush_get_option('uid', NULL);
  $only_uid = $only_uid !== NULL && $only_uid !== '' ? (int) $only_uid : NULL;
  $inserted = rovas_civic_backfill_eur_buy_merits_attribution($only_uid);
  drush_log(dt('Attributed EUR buy-Merits to project @nid; inserted @n issuance row(s).', array(
    '@nid' => $nid,
    '@n' => $inserted,
  )), 'success');
}
