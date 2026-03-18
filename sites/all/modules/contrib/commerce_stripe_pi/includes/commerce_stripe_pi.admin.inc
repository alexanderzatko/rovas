<?php

/**
 * @file
 * Provides admin forms and functions for commerce_stripe_pi.
 */

use Stripe\Refund;
use Stripe\Stripe;
use Stripe\Charge;

/**
 * Form callback for processing refunds.
 */
function commerce_stripe_pi_refund_form($form, &$form_state, $order, $transaction) {
  $form = array();
  $payment_method = commerce_payment_method_instance_load($transaction->instance_id);
  $form_state['order'] = $order;
  $form_state['transaction'] = $transaction;
  $form_state['payment_method'] = $payment_method;

  // Make sure the library is available.
  if (!commerce_stripe_pi_load_library()) {
    drupal_set_message(t('Cannot load the Stripe PHP library'), 'error');
    return $form;
  }

  if (!commerce_stripe_pi_advanced_fraud_enabled()) {
    $form['#attached']['js'][COMMERCE_STRIPE_PI_JS] = array(
      'type' => 'external',
    );
  }

  // Make sure we can load the original charge object.
  try {
    Stripe::setApiKey(trim($payment_method['settings']['secret_key']));
    $payment_intent = \Stripe\PaymentIntent::retrieve($transaction->remote_id);
    $form_state['stripe_pi_payment_intent'] = $payment_intent;
  }
  catch (Exception $e) {
    drupal_set_message(t('The original transaction could not be loaded. The error was: @error', array('@error' => $e->getMessage())), 'error');
    return $form;
  }
    /**
     * @var \Stripe\Charge $charge
     */
    $charge = $payment_intent->charges->data[0];
  // Calculate the amount left available for a refund.
  $amount_refunded = !empty($charge->amount_refunded) ? $charge->amount_refunded : 0;
  $remaining = $payment_intent->amount - $amount_refunded;

  $form['amount'] = array(
    '#type' => 'textfield',
    '#title' => t('Refund amount'),
    '#description' => t('Enter any amount to refund up to @txn_amount', array('@txn_amount' => commerce_currency_format($remaining, $transaction->currency_code))),
    '#required' => TRUE,
    '#size' => 8,
  );

  $options = array(
    'requested_by_customer' => t('Requested by customer'),
    'duplicate' => t('Duplicate'),
    'fraudulent' => t('Fraduluent'),
  );

  $form['reason'] = array(
    '#type' => 'select',
    '#title' => t('Refund reason'),
    '#description' => t('Select the most appropriate reason for the refund.'),
    '#options' => $options,
  );

  $form['actions'] = array(
    '#type' => 'container',
  );

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Process refund'),
  );

  $form['actions']['cancel'] = array(
    '#type' => 'button',
    '#value' => t('Cancel'),
  );
  return $form;
}

/**
 * Validation callback for submitting refunds to Stripe.
 */
function commerce_stripe_pi_refund_form_validate($form, &$form_state) {
  $transaction = $form_state['transaction'];
  $amount = commerce_currency_decimal_to_amount($form_state['values']['amount'], $transaction->currency_code);

  /**
   * @var \Stripe\Charge $charge
   */
  //Charge from Payment intent.
  $charge = $form_state['stripe_pi_payment_intent']->charges->data[0];

  // Calculate the amount left available for a refund.
  $amount_refunded = !empty($charge->amount_refunded) ? $charge->amount_refunded : 0;
  $remaining = $form_state['stripe_pi_payment_intent']->amount - $amount_refunded;

  // Make sure the refund amount is valid and available.
  if ($amount <= 0 || $amount > $remaining || !is_numeric($amount)) {
    form_set_error('amount', t('Please enter a valid return amount that is less than or equal to the remaining balance available for refund of @remaining.', array('@remaining' => commerce_currency_format($remaining, $transaction->currency_code))));
  }
}

/**
 * Submit callback for submitting refunds to Stripe.
 */
function commerce_stripe_pi_refund_form_submit($form, &$form_state) {
  // Don't rely on form_state objects to be fresh.
  $order = commerce_order_load($form_state['order']->order_id);
  $transaction = commerce_payment_transaction_load($form_state['transaction']->transaction_id);
  $payment_method = $form_state['payment_method'];

  // Make sure the library is available.
  if (!commerce_stripe_pi_load_library()) {
    drupal_set_message(t('Cannot load the Stripe PHP library'), 'error');
    return FALSE;
  }
  $payment_intent = $form_state['stripe_pi_payment_intent'];
  // Create the refund object.
  $data = array(
    'charge' => $payment_intent->charges->data[0]->id,
    'amount' => commerce_currency_decimal_to_amount($form_state['values']['amount'], $transaction->currency_code),
    'reason' => $form_state['values']['reason'],
  );

  try {
    Stripe::setApiKey(trim($payment_method['settings']['secret_key']));
    $refund = Refund::create($data);

    if (is_object($refund)) {
      // Copy the refund object into our own payload so we don't save API keys
      // included in the response object.
      $payload = array(
        'id' => $refund->id,
        'amount' => $refund->amount,
        'currency' => $refund->currency,
        'created' => $refund->created,
        'object' => $refund->object,
        'balance_transaction' => $refund->balance_transaction,
        'charge' => $refund->charge,
        'receipt_number' => $refund->receipt_number,
        'reason' => $refund->reason,
      );

      // Create the new commerce payment transation and set appropriate values.
      $refund_transaction = commerce_payment_transaction_new($transaction->payment_method, $order->order_id);
      $refund_transaction->instance_id = $payment_method['instance_id'];
      $refund_transaction->payload[REQUEST_TIME] = json_encode($payload);
      $refund_transaction->status = COMMERCE_PAYMENT_STATUS_SUCCESS;
      $refund_transaction->remote_id = $refund->id;
      $refund_transaction->message = t('Refund issued.');

      // Save data on the initial charge and flag this transaction as a refund.
      $refund_transaction->data = array(
        'stripe' => array(
          'stripe_charge' => array(
            'id' => $refund->id,
            'balance_transaction' => $refund->balance_transaction,
            'amount_refunded' => $refund->amount,
          ),
          'stripe_refund' => TRUE,
        ),
      );

      // Save the amount as a negative integer.
      $refund_transaction->amount = $refund->amount * -1;
      $refund_transaction->currency_code = strtoupper($refund->currency);
      commerce_payment_transaction_save($refund_transaction);

      // Inform the user of the success and redirect them back to payments.
      drupal_set_message(t('Refund processed successfully'));
      $form_state['redirect'] = 'admin/commerce/orders/' . $order->order_id . '/payment';
    }
  }
  catch (Exception $e) {
    drupal_set_message(t('The transaction could not be refunded. The error was: @error',
      array('@error' => $e->getMessage())), 'error');
  }
}

/**
 * Form callback for capturing a authorized payment.
 */
function commerce_stripe_pi_capture_form($form, &$form_state, $order, $transaction) {
  $form_state['order'] = $order;
  $form_state['transaction'] = $transaction;

  // Load and store the payment method instance for this transaction.
  $payment_method = commerce_payment_method_instance_load($transaction->instance_id);
  $form_state['payment_method'] = $payment_method;

  $balance = commerce_payment_order_balance($order);

  if ($balance['amount'] > 0 && $balance['amount'] < $transaction->amount) {
    $default_amount = $balance['amount'];
  }
  else {
    $default_amount = $transaction->amount;
  }

  // Convert the price amount to a user friendly decimal value.
  $default_amount = number_format(commerce_currency_amount_to_decimal($default_amount, $transaction->currency_code), 2, '.', '');

  $description = implode('<br />', array(
    t('Authorization: @amount', array('@amount' => commerce_currency_format($transaction->amount, $transaction->currency_code))),
    t('Order balance: @balance', array('@balance' => commerce_currency_format($balance['amount'], $balance['currency_code']))),
  ));

  $form['amount'] = array(
    '#type' => 'textfield',
    '#title' => t('Capture amount'),
    '#description' => $description,
    '#default_value' => $default_amount,
    '#field_suffix' => check_plain($transaction->currency_code),
    '#size' => 16,
  );

  $form = confirm_form($form,
    t('What amount do you want to capture?'),
    'admin/commerce/orders/' . $order->order_id . '/payment',
    '',
    t('Capture'),
    t('Cancel'),
    'confirm'
  );

  return $form;
}

/**
 * Validate handler: ensure a valid amount is given.
 */
function commerce_stripe_pi_capture_form_validate($form, &$form_state) {
  $transaction = $form_state['transaction'];
  $amount = $form_state['values']['amount'];

  // Verify this transaction is authorization only (AUTH_ONLY).
  if ($transaction->remote_status != 'AUTH_ONLY') {
    drupal_set_message(t('This operation can only be done on authorization only transactions.'), 'error');
    drupal_goto('admin/commerce/orders/' . $form_state['order']->order_id . '/payment');
  }

  // Verify a positive numeric amount has been entered for capture.
  if (!is_numeric($amount) || $amount <= 0) {
    form_set_error('amount', t('You must specify a positive numeric amount to capture.'));
  }

  // Verify the amount is less than or equal to the authorization amount.
  if ($amount > commerce_currency_amount_to_decimal($transaction->amount, $transaction->currency_code)) {
    form_set_error('amount', t('You cannot capture more than you authorized through Commerce Stripe.'));
  }

  // If the authorization has expired, display an error message and redirect.
  if (time() - $transaction->created > 86400 * 7) {
    drupal_set_message(t('This authorization has passed its 7 day limit and cannot be captured.'), 'error');
    drupal_goto('admin/commerce/orders/' . $form_state['order']->order_id . '/payment');
  }
}

/**
 * Submit handler: process a prior authorization capture via AIM.
 */
function commerce_stripe_pi_capture_form_submit($form, &$form_state) {
  $transaction = $form_state['transaction'];

  if (!commerce_stripe_pi_load_library()) {
    drupal_set_message(t('Error capturing payment. Please contact shop admin to proceed.'), 'error');
    watchdog('commerce_stripe_pi', 'Error capturing payment from admin : @transaction', array(
      '@transaction' => print_r($transaction, TRUE),
    ), WATCHDOG_ERROR);
    drupal_goto('admin/commerce/orders/' . $form_state['order']->order_id . '/payment');
  }

  commerce_stripe_pi_capture($transaction, $form_state['values']['amount'], TRUE);

  $form_state['redirect'] = 'admin/commerce/orders/' . $form_state['order']->order_id . '/payment';
}

/**
 * Form callback: allows the user to void a transaction.
 */
function commerce_stripe_pi_void_form($form, &$form_state, $order, $transaction) {
  $form_state['order'] = $order;
  $form_state['transaction'] = $transaction;

  // Load and store the payment method instance for this transaction.
  $payment_method = commerce_payment_method_instance_load($transaction->instance_id);
  $form_state['payment_method'] = $payment_method;

  $form['markup'] = array(
    '#markup' => t('Are you sure that you want to void this transaction?'),
  );

  $form = confirm_form($form,
    t('Are you sure that you want to void this transaction?'),
    'admin/commerce/orders/' . $order->order_id . '/payment',
    '',
    t('Void'),
    t('Cancel'),
    'confirm'
  );

  return $form;
}

/**
 * Submit handler: process the void request.
 *
 * This is only accessible on uncaptured charge. If a user wishes to
 * cancel a captured transaction, they need to credit the account.
 *
 * According to stripe, you can perform a refund on an uncaptured charge,
 * however, it must be for the full amount.
 */
function commerce_stripe_pi_void_form_submit($form, &$form_state) {
  $transaction = $form_state['transaction'];
  $order = $form_state['order'];

  if (!commerce_stripe_pi_load_library()) {
    drupal_set_message(t('Error voiding payment. Please contact shop admin to proceed.'), 'error');
    drupal_goto('admin/commerce/orders/' . $form_state['order']->order_id . '/payment');
  }

  try {
    $charge = Charge::retrieve($transaction->remote_id);

    $response = $charge->refunds->create();

    if (method_exists($response, '__toJSON')) {
      $transaction->payload[REQUEST_TIME] = $response->__toJSON();
    }
    else {
      $transaction->payload[REQUEST_TIME] = $response->toJSON();
    }

    $transaction->remote_status = commerce_stripe_pi_get_remote_status(NULL, $transaction, 'void');
    $transaction->message .= '<br />' . t('Voided: @date', array('@date' => format_date(REQUEST_TIME, 'short')));
    // Set the status to failure so that it isn't used for order balance
    // calculations.
    $transaction->status = COMMERCE_PAYMENT_STATUS_FAILURE;
    $transaction->amount = 0;
    commerce_payment_transaction_save($transaction);
  }
  catch (Exception $e) {
    drupal_set_message(t('We received the following error when trying to capture the transaction.'), 'error');
    drupal_set_message(check_plain($e->getMessage()), 'error');
    watchdog('commerce_stripe_pi', 'Following error received when processing card for capture @stripe_pi_error.', array('@stripe_pi_error' => $e->getMessage()), WATCHDOG_NOTICE);

    $transaction->payload[REQUEST_TIME] = $e->json_body;
    $transaction->message = t('Capture processing error: @stripe_pi_error', array('@stripe_pi_error' => $e->getMessage()));
    $transaction->status = COMMERCE_PAYMENT_STATUS_FAILURE;
    $transaction->remote_status = 'FAILED';
    commerce_payment_transaction_save($transaction);
  }

  $form_state['redirect'] = 'admin/commerce/orders/' . $order->order_id . '/payment';
}
