<?php

/**
 * @file
 * JSON: Stripe-normalized EUR buyshares amount for Express Checkout UI alignment.
 */

/**
 * Menu callback: normalized line-total cents for a typed EUR buyshares amount.
 */
function _rovas_stripe_express_checkout_normalize_buyshares_amount() {
  global $user;

  if (!$user->uid) {
    drupal_json_output(array('ok' => FALSE, 'error' => t('Authentication required.')));
    drupal_exit();
  }

  $token = isset($_GET['token']) ? $_GET['token'] : '';
  if (!drupal_valid_token($token, 'rovas_express_buyshares')) {
    drupal_json_output(array('ok' => FALSE, 'error' => t('Invalid security token.')));
    drupal_exit();
  }

  $recipient = isset($_GET['recipient']) ? $_GET['recipient'] : '';
  if (!is_numeric($recipient) || (int) $recipient < 1) {
    drupal_json_output(array('ok' => FALSE, 'error' => t('Invalid recipient.')));
    drupal_exit();
  }
  $recipient = (int) $recipient;

  $amount_cents = isset($_GET['amount']) ? (int) $_GET['amount'] : 0;
  if ($amount_cents < 1) {
    drupal_json_output(array('ok' => FALSE, 'error' => t('Invalid amount.')));
    drupal_exit();
  }

  if (!function_exists('rovas_purchase_shares_project_allows_buyshares')) {
    drupal_json_output(array('ok' => FALSE, 'error' => t('Buyshares module unavailable.')));
    drupal_exit();
  }

  $proj = node_load($recipient);
  if (!rovas_purchase_shares_project_allows_buyshares($proj)) {
    drupal_json_output(array('ok' => FALSE, 'error' => t('This project does not allow buying shares.')));
    drupal_exit();
  }

  $paid_eur = $amount_cents / 100.0;
  $proj_wrap = entity_metadata_wrapper('node', $proj);
  $line_cents = rovas_purchase_shares_eur_buyshares_line_total_cents($proj_wrap, $paid_eur);

  if ($line_cents === FALSE || $line_cents < 1) {
    drupal_json_output(array('ok' => FALSE, 'error' => t('The amount paid cannot be processed for this project.')));
    drupal_exit();
  }

  $adjusted = ($line_cents !== $amount_cents);
  $normalized_eur = $line_cents / 100.0;
  $message = '';
  if ($adjusted) {
    $message = t('The amount you entered (@orig €) was adjusted to @adj € to accommodate accounting constraints.', array(
      '@orig' => number_format($paid_eur, 2, '.', ''),
      '@adj' => number_format($normalized_eur, 2, '.', ''),
    ));
  }

  drupal_json_output(array(
    'ok' => TRUE,
    'normalizedCents' => $line_cents,
    'normalizedEuro' => $normalized_eur,
    'adjusted' => $adjusted,
    'message' => $message,
  ));
  drupal_exit();
}
