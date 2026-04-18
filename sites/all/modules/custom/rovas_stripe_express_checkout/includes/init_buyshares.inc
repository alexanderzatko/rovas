<?php

/**
 * @file
 * PaymentIntent creation for buyshares Express Checkout (order-first).
 */

require_once DRUPAL_ROOT . '/sites/all/libraries/stripe-php/init.php';

/**
 * JSON callback: create Commerce time-share order then Stripe PaymentIntent.
 */
function _rovas_stripe_express_checkout_init_buyshares() {
  global $user;

  if (!$user->uid) {
    drupal_json_output(array('error' => 'Authentication required.'));
    drupal_exit();
  }

  $token = isset($_GET['token']) ? $_GET['token'] : '';
  if (!drupal_valid_token($token, 'rovas_express_buyshares')) {
    watchdog('rovas_stripe_express', 'buyshares init: invalid token for uid @u', array('@u' => $user->uid), WATCHDOG_WARNING);
    drupal_json_output(array('error' => 'Invalid security token.'));
    drupal_exit();
  }

  $recipient = isset($_GET['recipient']) ? $_GET['recipient'] : '';
  if (!is_numeric($recipient) || (int) $recipient < 1) {
    drupal_json_output(array('error' => 'Invalid recipient.'));
    drupal_exit();
  }
  $recipient = (int) $recipient;

  $amount_cents = isset($_GET['amount']) ? (int) $_GET['amount'] : 0;
  if ($amount_cents < 1) {
    drupal_json_output(array('error' => 'Invalid amount.'));
    drupal_exit();
  }
  $paid_eur = $amount_cents / 100.0;

  if (!function_exists('rovas_purchase_shares_project_allows_buyshares')) {
    drupal_json_output(array('error' => 'Buyshares module unavailable.'));
    drupal_exit();
  }

  $proj = node_load($recipient);
  if (!rovas_purchase_shares_project_allows_buyshares($proj)) {
    drupal_json_output(array('error' => 'This project does not allow buying shares.'));
    drupal_exit();
  }

  $proj_wrap = entity_metadata_wrapper('node', $proj);
  $share = rovas_purchase_shares_compute_share_hours_for_payment($proj_wrap, 'EUR', $paid_eur);

  $order = rovas_purchase_shares_create_eur_time_share_order($user->uid, $recipient, $paid_eur, $share, 'pay-choice-card');
  if (!$order) {
    drupal_json_output(array('error' => 'Could not create order for this amount.'));
    drupal_exit();
  }

  $order = commerce_order_load($order->order_id);
  $expected_cents = isset($order->commerce_order_total[LANGUAGE_NONE][0]['amount']) ? (int) $order->commerce_order_total[LANGUAGE_NONE][0]['amount'] : 0;
  if ($expected_cents !== $amount_cents) {
    watchdog('rovas_stripe_express', 'buyshares init: amount mismatch order=@o expected=@e got=@g', array(
      '@o' => $order->order_id,
      '@e' => $expected_cents,
      '@g' => $amount_cents,
    ), WATCHDOG_ERROR);
    drupal_json_output(array('error' => 'Order amount mismatch.'));
    drupal_exit();
  }

  $secret_key = variable_get('stripe_secret_key', '');
  if (!$secret_key) {
    drupal_json_output(array('error' => 'Stripe configuration error.'));
    drupal_exit();
  }

  \Stripe\Stripe::setApiKey($secret_key);

  try {
    $intent = \Stripe\PaymentIntent::create(array(
      'amount' => $amount_cents,
      'currency' => 'eur',
      'receipt_email' => $user->mail,
      'automatic_payment_methods' => array('enabled' => TRUE),
      'metadata' => array(
        'user_id' => (string) $user->uid,
        'context' => 'express_checkout',
        'paytype' => 'buyshares',
        'commerce_order_id' => (string) $order->order_id,
        'project_id' => (string) $recipient,
      ),
    ));

    drupal_json_output(array(
      'clientSecret' => $intent->client_secret,
    ));
  }
  catch (\Stripe\Exception\ApiErrorException $e) {
    watchdog('rovas_stripe_express', 'buyshares PaymentIntent failed: @m', array('@m' => $e->getMessage()), WATCHDOG_ERROR);
    drupal_json_output(array('error' => $e->getMessage()));
  }

  drupal_exit();
}
