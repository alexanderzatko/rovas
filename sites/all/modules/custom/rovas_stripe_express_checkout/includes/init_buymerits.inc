<?php

/**
 * @file
 * PaymentIntent creation for buymerits Express Checkout (order-first).
 */

require_once DRUPAL_ROOT . '/sites/all/libraries/stripe-php/init.php';

/**
 * JSON callback: create Commerce merits order then Stripe PaymentIntent.
 */
function _rovas_stripe_express_checkout_init_buymerits() {
  global $user;

  // Refuse to start a PaymentIntent when the site has fiat rails off; only
  // CHR payments are accepted in that mode (see admin/config/rovas/payment-methods).
  if (function_exists('rovas_project_reward_fiat_disabled') && rovas_project_reward_fiat_disabled()) {
    watchdog('rovas_stripe_express', 'buymerits init refused: fiat payments disabled.', array(), WATCHDOG_WARNING);
    drupal_json_output(array('error' => t('Card payments are disabled.')));
    drupal_exit();
  }

  if (!$user->uid) {
    drupal_json_output(array('error' => 'Authentication required.'));
    drupal_exit();
  }

  $token = isset($_GET['token']) ? $_GET['token'] : '';
  if (!drupal_valid_token($token, 'rovas_express_buymerits')) {
    watchdog('rovas_stripe_express', 'buymerits init: invalid token for uid @u', array('@u' => $user->uid), WATCHDOG_WARNING);
    drupal_json_output(array('error' => 'Invalid security token.'));
    drupal_exit();
  }

  $recipient = isset($_GET['recipient']) ? $_GET['recipient'] : '';
  if (!is_numeric($recipient) || (int) $recipient !== (int) $user->uid) {
    drupal_json_output(array('error' => 'Invalid recipient.'));
    drupal_exit();
  }

  $amount_cents = isset($_GET['amount']) ? (int) $_GET['amount'] : 0;
  if ($amount_cents < 1) {
    drupal_json_output(array('error' => 'Invalid amount.'));
    drupal_exit();
  }
  $paid_eur = $amount_cents / 100.0;

  if (!function_exists('rovas_forex_create_merits_eur_order')) {
    drupal_json_output(array('error' => 'Merits purchase is unavailable.'));
    drupal_exit();
  }

  $order = rovas_forex_create_merits_eur_order($user->uid, $paid_eur, 'pay-choice-card');
  if (!$order) {
    drupal_json_output(array('error' => 'Could not create order for this amount.'));
    drupal_exit();
  }

  $order = commerce_order_load($order->order_id);
  $expected_cents = isset($order->commerce_order_total[LANGUAGE_NONE][0]['amount']) ? (int) $order->commerce_order_total[LANGUAGE_NONE][0]['amount'] : 0;
  if ($expected_cents < 1) {
    watchdog('rovas_stripe_express', 'buymerits init: invalid order total order=@o', array('@o' => $order->order_id), WATCHDOG_ERROR);
    drupal_json_output(array('error' => 'Invalid order total.'));
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
      'amount' => $expected_cents,
      'currency' => 'eur',
      'receipt_email' => $user->mail,
      'automatic_payment_methods' => array('enabled' => TRUE),
      'metadata' => array(
        'user_id' => (string) $user->uid,
        'context' => 'express_checkout',
        'paytype' => 'buymerits',
        'commerce_order_id' => (string) $order->order_id,
      ),
    ));

    drupal_json_output(array(
      'clientSecret' => $intent->client_secret,
    ));
  }
  catch (\Stripe\Exception\ApiErrorException $e) {
    watchdog('rovas_stripe_express', 'buymerits PaymentIntent failed: @m', array('@m' => $e->getMessage()), WATCHDOG_ERROR);
    drupal_json_output(array('error' => $e->getMessage()));
  }

  drupal_exit();
}
