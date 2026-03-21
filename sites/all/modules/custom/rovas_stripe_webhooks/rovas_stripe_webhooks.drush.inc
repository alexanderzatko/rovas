<?php

/**
 * @file
 * Drush commands for Rovas Stripe Webhooks.
 */

/**
 * Implements hook_drush_command().
 */
function rovas_stripe_webhooks_drush_command() {
  $items['rovas-complete-order'] = array(
    'callback' => 'drush_rovas_stripe_webhooks_complete_order',
    'description' => dt('Complete a stuck Stripe order by re-running the full workflow: payment transaction, paid_in_full Rules, checkout complete, and fee processing. Use for orders that failed mid-webhook (e.g. due to PHP deprecation).'),
    'arguments' => array(
      'order_id' => dt('The commerce order ID to complete (e.g. 2508).'),
    ),
    'examples' => array(
      'drush rovas-complete-order 2508' => 'Complete order 2508.',
    ),
    'aliases' => array('rco'),
  );
  return $items;
}

/**
 * Drush callback: Complete a stuck order by running the full workflow.
 */
function drush_rovas_stripe_webhooks_complete_order($order_id = NULL) {
  if (empty($order_id) || !ctype_digit((string) $order_id)) {
    return drush_set_error('ROVAS_COMPLETE_ORDER', dt('Provide a valid order ID, e.g. drush rovas-complete-order 2508'));
  }

  $order_id = (int) $order_id;
  $order = commerce_order_load($order_id);
  if (!$order) {
    return drush_set_error('ROVAS_COMPLETE_ORDER', dt('Order @id not found.', array('@id' => $order_id)));
  }

  drush_log(dt('Loading order @id (status: @status)...', array(
    '@id' => $order_id,
    '@status' => $order->status,
  )), 'ok');

  // Resolve payment intent ID.
  $payment_intent_id = NULL;
  if (!empty($order->data['stripe_pi']['payment_intent']['id'])) {
    $payment_intent_id = $order->data['stripe_pi']['payment_intent']['id'];
  }
  else {
    $tx = db_select('commerce_payment_transaction', 'cpt')
      ->fields('cpt', array('remote_id'))
      ->condition('cpt.order_id', $order_id)
      ->condition('cpt.payment_method', 'commerce_stripe_pi')
      ->condition('cpt.status', 'success')
      ->execute()
      ->fetchField();
    if ($tx) {
      $payment_intent_id = $tx;
    }
  }

  if (!$payment_intent_id) {
    return drush_set_error('ROVAS_COMPLETE_ORDER', dt('Order @id has no Stripe payment intent. Cannot complete.', array('@id' => $order_id)));
  }

  require_once drush_get_context('DRUSH_DRUPAL_ROOT') . '/sites/all/libraries/stripe-php/init.php';
  \Stripe\Stripe::setApiKey(variable_get('stripe_secret_key'));

  // 1. Ensure payment transaction exists.
  $pi = \Stripe\PaymentIntent::retrieve($payment_intent_id);
  if (function_exists('commerce_stripe_pi_create_transaction')) {
    $created = commerce_stripe_pi_create_transaction($order_id, $pi);
    drush_log(dt('Transaction: @status', array('@status' => $created ? 'exists/created' : 'create failed')), 'ok');
    if (!$created) {
      return drush_set_error('ROVAS_COMPLETE_ORDER', dt('Could not ensure payment transaction for order @id.', array('@id' => $order_id)));
    }
  }

  $order = commerce_order_load($order_id);

  // 2. Fire commerce_payment_order_paid_in_full if not yet invoked.
  if (empty($order->data['commerce_payment_order_paid_in_full_invoked'])) {
    $transaction = NULL;
    foreach (commerce_payment_transaction_load_multiple(array(), array('order_id' => $order_id)) as $t) {
      if ($t->payment_method === 'commerce_stripe_pi' && $t->status === 'success') {
        $transaction = $t;
        break;
      }
    }
    if ($transaction) {
      $balance = commerce_payment_order_balance($order);
      if (!empty($balance) && $balance['amount'] <= 0) {
        drush_log(dt('Firing commerce_payment_order_paid_in_full (Rules: forex node, invoice, etc.)...'), 'ok');
        if (variable_get('commerce_order_auto_revision', TRUE)) {
          $order->revision = TRUE;
          $order->log = t('Order completed via drush rovas-complete-order (recovery from stuck webhook).');
        }
        rules_invoke_all('commerce_payment_order_paid_in_full', $order, $transaction);
        $order->data['commerce_payment_order_paid_in_full_invoked'] = TRUE;
        commerce_order_save($order);
        $order = commerce_order_load($order_id);
      }
      else {
        drush_log(dt('Order balance not zero (@amt), skipping paid_in_full.', array('@amt' => isset($balance['amount']) ? $balance['amount'] : 'n/a')), 'warning');
      }
    }
    else {
      drush_log(dt('No successful transaction found, skipping paid_in_full.'), 'warning');
    }
  }
  else {
    drush_log(dt('paid_in_full already invoked.'), 'ok');
  }

  // 3. commerce_checkout_complete.
  drush_log(dt('Firing commerce_checkout_complete...'), 'ok');
  commerce_checkout_complete($order);
  $order = commerce_order_load($order_id);

  // 4. Fee processing and rules_process_recipient_order_core.
  module_load_include('inc', 'rovas_stripe_webhooks', 'includes/processor');
  $intent = array('id' => $payment_intent_id);
  if (!empty($order->data['stripe_pi']['payment_intent']) && is_array($order->data['stripe_pi']['payment_intent'])) {
    $intent = $order->data['stripe_pi']['payment_intent'];
  }
  drush_log(dt('Running fee processing and recipient core...'), 'ok');
  _rovas_stripe_webhooks_add_balance_fees_and_process($order, $intent);
  $order = commerce_order_load($order_id);

  // 5. Fallback: if still in checkout, force completed.
  $order_status = commerce_order_status_load($order->status);
  if ($order_status && $order_status['state'] === 'checkout') {
    drush_log(dt('Order still in checkout; forcing status to completed.'), 'ok');
    commerce_order_status_update($order, 'completed', FALSE, NULL, 'Order completed via drush rovas-complete-order (fallback).');
    $order = commerce_order_load($order_id);
  }

  drush_log(dt('Done. Order @id status: @status', array(
    '@id' => $order_id,
    '@status' => $order->status,
  )), 'success');
}
