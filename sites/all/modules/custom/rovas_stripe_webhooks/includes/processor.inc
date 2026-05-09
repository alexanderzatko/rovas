<?php

/**
 * @file
 * Stripe webhook event processing.
 */

/**
 * Process payment_intent.succeeded events: create/find order, add fees, process forex.
 */
function rovas_stripe_webhooks_process_payment_intent_succeeded($event, $record) {
  if (!isset($event['data']['object'])) {
    return FALSE;
  }

  $intent = $event['data']['object'];
  $payment_intent_id = isset($intent['id']) ? $intent['id'] : NULL;
  $metadata = isset($intent['metadata']) ? $intent['metadata'] : array();
  $is_express_checkout = isset($metadata['context']) && $metadata['context'] == 'express_checkout';

  // Require Stripe PHP and Commerce for processing.
  if (!function_exists('commerce_order_load') || !class_exists('Stripe\Webhook')) {
    return FALSE;
  }

  require_once DRUPAL_ROOT . '/sites/all/libraries/stripe-php/init.php';
  \Stripe\Stripe::setApiKey(variable_get('stripe_secret_key'));

  $order = NULL;

  if ($is_express_checkout) {
    // Check for existing order (prevent duplicates).
    $existing = db_select('commerce_order', 'co')
      ->fields('co', array('order_id'))
      ->condition('co.data', '%' . db_like($payment_intent_id) . '%', 'LIKE')
      ->range(0, 1)
      ->execute()
      ->fetchField();

    if ($existing) {
      return TRUE;
    }

    $uid = isset($metadata['user_id']) ? intval($metadata['user_id']) : 0;
    $amount = isset($intent['amount']) ? intval($intent['amount']) : 0;
    $pid = isset($metadata['project_id']) ? $metadata['project_id'] : NULL;
    $paytype = isset($metadata['paytype']) ? $metadata['paytype'] : NULL;
    $paid_amt = $amount / 100;

    // Buyshares: Commerce order was created before PaymentIntent; attach PI here.
    if (!empty($metadata['paytype']) && $metadata['paytype'] === 'buyshares' && !empty($metadata['commerce_order_id']) && isset($intent['status']) && $intent['status'] === 'succeeded') {
      $order_id = (int) $metadata['commerce_order_id'];
      $order = commerce_order_load($order_id);
      if (!$order) {
        watchdog('rovas_stripe_webhooks', 'buyshares express: order @oid not found for PI @pi', array('@oid' => $order_id, '@pi' => $payment_intent_id), WATCHDOG_ERROR);
        return FALSE;
      }
      if ($uid > 0 && (int) $order->uid !== $uid) {
        watchdog('rovas_stripe_webhooks', 'buyshares express: uid mismatch for order @oid', array('@oid' => $order_id), WATCHDOG_WARNING);
        return FALSE;
      }
      $expected = isset($order->commerce_order_total[LANGUAGE_NONE][0]['amount']) ? (int) $order->commerce_order_total[LANGUAGE_NONE][0]['amount'] : 0;
      if ($expected !== $amount) {
        watchdog('rovas_stripe_webhooks', 'buyshares express: amount mismatch order=@o exp=@e got=@g', array('@o' => $order_id, '@e' => $expected, '@g' => $amount), WATCHDOG_ERROR);
        return FALSE;
      }
      // commerce_order_rules_contains_product() requires $order->commerce_line_items
      // to be populated; freshly loaded orders often omit it even when the field
      // has line items (see _rovas_purchase_shares_order_line_items_contain_sku).
      $has_timeshare = function_exists('_rovas_purchase_shares_order_line_items_contain_sku')
        && _rovas_purchase_shares_order_line_items_contain_sku($order, 'time-share');
      if (!$has_timeshare && function_exists('commerce_order_rules_contains_product')) {
        $has_timeshare = commerce_order_rules_contains_product($order, 'time-share', '>=', 1);
      }
      if (!$has_timeshare) {
        watchdog('rovas_stripe_webhooks', 'buyshares express: order @oid has no time-share line', array('@oid' => $order_id), WATCHDOG_WARNING);
        return FALSE;
      }

      $order->data['last_cart_refresh'] = REQUEST_TIME;
      $order->data['stripe_pi'] = array('payment_intent' => $intent);
      $order->data['payment_method'] = 'commerce_stripe_pi|commerce_payment_commerce_stripe_pi';
      $order->data['rovas_payment_method'] = 'commerce_stripe_pi|commerce_payment_commerce_stripe_pi';
      commerce_order_save($order);

      if (function_exists('commerce_stripe_pi_create_transaction')) {
        $pi = \Stripe\PaymentIntent::retrieve($payment_intent_id);
        $transaction = commerce_stripe_pi_create_transaction($order->order_id, $pi);
        if ($transaction) {
          $order = commerce_order_load($order->order_id);
          if (function_exists('commerce_checkout_complete')) {
            commerce_checkout_complete($order);
          }
          elseif (module_exists('rules')) {
            rules_invoke_event('commerce_checkout_complete', $order);
          }
          if (function_exists('_rovas_stripe_webhooks_add_balance_fees_and_process')) {
            _rovas_stripe_webhooks_add_balance_fees_and_process($order, $intent);
          }

          $order = commerce_order_load($order->order_id);
          $order_status = commerce_order_status_load($order->status);
          if ($order_status && $order_status['state'] == 'checkout') {
            commerce_order_status_update($order, 'completed', FALSE, NULL,
              'Order completed via Stripe buyshares express payment_intent.succeeded webhook.');
          }
          $order = commerce_order_load($order->order_id, TRUE);
          if (!isset($order->data) || !is_array($order->data)) {
            $order->data = array();
          }
          // Mark invoice as ready only after all webhook-side updates are done.
          $order->data['rovas_express_invoice_ready'] = REQUEST_TIME;
          commerce_order_save($order);

          // 3rd-party rewpro notification must not rely on session (webhook runs
          // out-of-band and rovas_stripe_webhooks suppresses session access).
          _rovas_stripe_webhooks_maybe_notify_third_party($order, $intent);

          if (function_exists('rovas_project_reward_commerce_invoice_receipt_action_mail_execute')) {
            $to = token_replace('[commerce-order:owner] <[commerce-order:mail]>', array('commerce-order' => $order), array('clear' => TRUE));
            $subject = token_replace('[site:name]: Order #[commerce-order:order-id]', array('commerce-order' => $order), array('clear' => TRUE));
            rovas_project_reward_commerce_invoice_receipt_action_mail_execute($order, $to, $subject, NULL);
          }
          elseif (function_exists('commerce_invoice_receipt_action_mail')) {
            $to = token_replace('[commerce-order:owner] <[commerce-order:mail]>', array('commerce-order' => $order), array('clear' => TRUE));
            $subject = token_replace('[site:name]: Order #[commerce-order:order-id]', array('commerce-order' => $order), array('clear' => TRUE));
            commerce_invoice_receipt_action_mail($order, $to, $subject, NULL);
          }
        }
        else {
          watchdog('rovas_stripe_webhooks', 'buyshares express: failed transaction for order @order_id', array('@order_id' => $order->order_id), WATCHDOG_ERROR);
        }
      }
      return TRUE;
    }

    // Buymerits: same order-first flow as buyshares, merits product line.
    if (!empty($metadata['paytype']) && $metadata['paytype'] === 'buymerits' && !empty($metadata['commerce_order_id']) && isset($intent['status']) && $intent['status'] === 'succeeded') {
      $order_id = (int) $metadata['commerce_order_id'];
      $order = commerce_order_load($order_id);
      if (!$order) {
        watchdog('rovas_stripe_webhooks', 'buymerits express: order @oid not found for PI @pi', array('@oid' => $order_id, '@pi' => $payment_intent_id), WATCHDOG_ERROR);
        return FALSE;
      }
      if ($uid > 0 && (int) $order->uid !== $uid) {
        watchdog('rovas_stripe_webhooks', 'buymerits express: uid mismatch for order @oid', array('@oid' => $order_id), WATCHDOG_WARNING);
        return FALSE;
      }
      $expected = isset($order->commerce_order_total[LANGUAGE_NONE][0]['amount']) ? (int) $order->commerce_order_total[LANGUAGE_NONE][0]['amount'] : 0;
      if ($expected !== $amount) {
        watchdog('rovas_stripe_webhooks', 'buymerits express: amount mismatch order=@o exp=@e got=@g', array('@o' => $order_id, '@e' => $expected, '@g' => $amount), WATCHDOG_ERROR);
        return FALSE;
      }
      $has_merits = function_exists('_rovas_purchase_shares_order_line_items_contain_sku')
        && _rovas_purchase_shares_order_line_items_contain_sku($order, 'merits');
      if (!$has_merits && function_exists('commerce_order_rules_contains_product')) {
        $has_merits = commerce_order_rules_contains_product($order, 'merits', '>=', 1);
      }
      if (!$has_merits) {
        watchdog('rovas_stripe_webhooks', 'buymerits express: order @oid has no merits line', array('@oid' => $order_id), WATCHDOG_WARNING);
        return FALSE;
      }

      $order->data['last_cart_refresh'] = REQUEST_TIME;
      $order->data['stripe_pi'] = array('payment_intent' => $intent);
      $order->data['payment_method'] = 'commerce_stripe_pi|commerce_payment_commerce_stripe_pi';
      $order->data['rovas_payment_method'] = 'commerce_stripe_pi|commerce_payment_commerce_stripe_pi';
      commerce_order_save($order);

      if (function_exists('commerce_stripe_pi_create_transaction')) {
        $pi = \Stripe\PaymentIntent::retrieve($payment_intent_id);
        $transaction = commerce_stripe_pi_create_transaction($order->order_id, $pi);
        if ($transaction) {
          $order = commerce_order_load($order->order_id);
          if (function_exists('commerce_checkout_complete')) {
            commerce_checkout_complete($order);
          }
          elseif (module_exists('rules')) {
            rules_invoke_event('commerce_checkout_complete', $order);
          }
          if (function_exists('_rovas_stripe_webhooks_add_balance_fees_and_process')) {
            _rovas_stripe_webhooks_add_balance_fees_and_process($order, $intent);
          }

          $order = commerce_order_load($order->order_id);
          $order_status = commerce_order_status_load($order->status);
          if ($order_status && $order_status['state'] == 'checkout') {
            commerce_order_status_update($order, 'completed', FALSE, NULL,
              'Order completed via Stripe buymerits express payment_intent.succeeded webhook.');
          }
          $order = commerce_order_load($order->order_id, TRUE);
          if (!isset($order->data) || !is_array($order->data)) {
            $order->data = array();
          }
          $order->data['rovas_express_invoice_ready'] = REQUEST_TIME;
          commerce_order_save($order);

          _rovas_stripe_webhooks_maybe_notify_third_party($order, $intent);

          if (function_exists('rovas_project_reward_commerce_invoice_receipt_action_mail_execute')) {
            $to = token_replace('[commerce-order:owner] <[commerce-order:mail]>', array('commerce-order' => $order), array('clear' => TRUE));
            $subject = token_replace('[site:name]: Order #[commerce-order:order-id]', array('commerce-order' => $order), array('clear' => TRUE));
            rovas_project_reward_commerce_invoice_receipt_action_mail_execute($order, $to, $subject, NULL);
          }
          elseif (function_exists('commerce_invoice_receipt_action_mail')) {
            $to = token_replace('[commerce-order:owner] <[commerce-order:mail]>', array('commerce-order' => $order), array('clear' => TRUE));
            $subject = token_replace('[site:name]: Order #[commerce-order:order-id]', array('commerce-order' => $order), array('clear' => TRUE));
            commerce_invoice_receipt_action_mail($order, $to, $subject, NULL);
          }
        }
        else {
          watchdog('rovas_stripe_webhooks', 'buymerits express: failed transaction for order @order_id', array('@order_id' => $order->order_id), WATCHDOG_ERROR);
        }
      }
      return TRUE;
    }

    $email = NULL;
    if (!empty($intent['receipt_email'])) {
      $email = $intent['receipt_email'];
    }
    elseif (!empty($intent['charges']['data'][0]['billing_details']['email'])) {
      $email = $intent['charges']['data'][0]['billing_details']['email'];
    }

    $billing = NULL;
    if (!empty($intent['charges']['data'][0]['billing_details']['address'])) {
      $bd = $intent['charges']['data'][0]['billing_details'];
      $addr = $bd['address'];
      $billing = array(
        'first_name' => $bd['name'] ? explode(' ', $bd['name'])[0] : '',
        'last_name' => $bd['name'] ? implode(' ', array_slice(explode(' ', $bd['name']), 1)) : '',
        'thoroughfare' => isset($addr['line1']) ? $addr['line1'] : '',
        'premise' => isset($addr['line2']) ? $addr['line2'] : '',
        'locality' => isset($addr['city']) ? $addr['city'] : '',
        'administrative_area' => isset($addr['state']) ? $addr['state'] : '',
        'postal_code' => isset($addr['postal_code']) ? $addr['postal_code'] : '',
        'country' => isset($addr['country']) ? $addr['country'] : '',
        'phone' => isset($bd['phone']) ? $bd['phone'] : '',
      );
      if (empty($billing['thoroughfare']) && empty($billing['locality'])) {
        $billing = NULL;
      }
    }

    if ($pid && isset($intent['status']) && $intent['status'] === 'succeeded') {
      $order = rpr_create_commerce_order($uid, $paid_amt, $paytype, $pid, $email, $billing);
      if ($order) {
        $order->data = array(
          'last_cart_refresh' => REQUEST_TIME,
          'stripe_pi' => array('payment_intent' => $intent),
          'payment_method' => 'commerce_stripe_pi|commerce_payment_commerce_stripe_pi',
          'rovas_payment_method' => 'commerce_stripe_pi|commerce_payment_commerce_stripe_pi',
        );
        commerce_order_save($order);

        if (function_exists('commerce_stripe_pi_create_transaction')) {
          $pi = \Stripe\PaymentIntent::retrieve($payment_intent_id);
          $transaction = commerce_stripe_pi_create_transaction($order->order_id, $pi);
          if ($transaction) {
            $order = commerce_order_load($order->order_id);
            if (module_exists('rules')) {
              rules_invoke_event('commerce_checkout_complete', $order);
            }
            _rovas_stripe_webhooks_add_balance_fees_and_process($order, $intent);

            // Notify 3rd party (rewpro) if the PaymentIntent carried a callback token.
            $order = commerce_order_load($order->order_id, TRUE);
            _rovas_stripe_webhooks_maybe_notify_third_party($order, $intent);
          }
          else {
            watchdog('rovas_stripe_webhooks', 'Failed to create payment transaction for order @order_id', array('@order_id' => $order->order_id), WATCHDOG_ERROR);
          }
        }
      }
    }
  }
  else {
    // Prefer an explicit order id stored on the PaymentIntent metadata.
    // For regular (non-express) card payments, commerce_stripe_pi commonly sets:
    //   - metadata.order_id
    //   - metadata.order_number
    // (and we may additionally set metadata.commerce_order_id in custom flows).
    $order = NULL;
    $meta_order_id = NULL;
    if (!empty($metadata['order_id']) && is_numeric($metadata['order_id'])) {
      $meta_order_id = (int) $metadata['order_id'];
    }
    elseif (!empty($metadata['order_number']) && is_numeric($metadata['order_number'])) {
      $meta_order_id = (int) $metadata['order_number'];
    }

    if ($meta_order_id) {
      $order = commerce_order_load($meta_order_id);
    }
    if (!$order) {
      // Do not attempt fuzzy matching by PaymentIntent id. Regular card PIs
      // are expected to carry the Commerce order id in metadata.
      watchdog('rovas_stripe_webhooks', 'No order id in PaymentIntent metadata for PI @pi; cannot correlate regular card payment.', array('@pi' => $payment_intent_id), WATCHDOG_WARNING);
      return FALSE;
    }
    if ($order) {
      // Always work with a fresh copy from the DB.
      $order = commerce_order_load($order->order_id);

      // As a secondary safeguard, check for an existing successful payment
      // transaction in addition to the in-memory flag. The flag can be wiped
      // by concurrent stale order saves. A successful transaction is
      // ground-truth evidence that commerce_stripe_pi_create_transaction()
      // already ran, which means Commerce already fired paid_in_full and set
      // the flag.
      if (empty($order->data['commerce_payment_order_paid_in_full_invoked'])
        && db_select('commerce_payment_transaction', 'cpt')
          ->condition('cpt.order_id', $order->order_id)
          ->condition('cpt.payment_method', 'commerce_stripe_pi')
          ->condition('cpt.status', 'success')
          ->countQuery()->execute()->fetchField()) {
        // A successful transaction exists but the flag was wiped by a stale
        // concurrent save. Restore it so the duplicate guard works correctly.
        $order->data['commerce_payment_order_paid_in_full_invoked'] = TRUE;
        commerce_order_save($order);
        watchdog('rovas_stripe_webhooks', 'Restored paid_in_full_invoked flag for order @order_id (was wiped by concurrent stale save).', array('@order_id' => $order->order_id), WATCHDOG_WARNING);
      }

      // Ensure a payment transaction exists for this order, mirroring the
      // express-checkout path. If the webhook arrived before the checkout-form
      // PHP handler ran, no Commerce transaction exists yet and Commerce's
      // paid_in_full mechanism has not fired, so the Rule
      // rules_process_card_payment_orders has not created the forex node that
      // _rovas_process_stripe_fees_for_order() needs to call
      // rules_process_recipient_order_core.
      //
      // commerce_stripe_pi_create_transaction() is idempotent: it checks for
      // an existing transaction by PI id and returns TRUE immediately without
      // creating a duplicate if the checkout UI already created one. When it
      // does create a new transaction Commerce fires paid_in_full via
      // hook_commerce_payment_transaction_insert, which runs the Rule and
      // creates the forex node.
      if (empty($order->data['commerce_payment_order_paid_in_full_invoked'])
        && function_exists('commerce_stripe_pi_create_transaction')) {
        $pi_obj = \Stripe\PaymentIntent::retrieve($payment_intent_id);
        commerce_stripe_pi_create_transaction($order->order_id, $pi_obj);
      }

      // Reload: Commerce's paid_in_full hook (triggered by transaction creation
      // above, or triggered earlier by the checkout UI) may have modified the
      // order, including setting paid_in_full_invoked and the order status.
      $order = commerce_order_load($order->order_id);

      // Fee processing: forex node now exists (created by the Rule above or by
      // the checkout UI that fired paid_in_full earlier). This will find the
      // node and invoke rules_process_recipient_order_core, whose last action
      // sets the order status to 'completed' and saves.
      _rovas_stripe_webhooks_add_balance_fees_and_process($order, $intent);

      // Reload after fee processing.
      $order = commerce_order_load($order->order_id);

      // Fallback: advance the order out of checkout if it is still stuck there.
      // Normally rules_process_recipient_order_core (called from fee processing)
      // has already set the status to 'completed'. This guard handles edge cases
      // where fee processing returned early (e.g. forex node not found, component
      // inactive) so the order does not remain permanently in checkout state.
      $order_status = commerce_order_status_load($order->status);
      if ($order_status && $order_status['state'] == 'checkout') {
        commerce_order_status_update($order, 'completed', FALSE, NULL,
          'Order completed via Stripe payment_intent.succeeded webhook.');
        $order = commerce_order_load($order->order_id);
      }

      _rovas_stripe_webhooks_maybe_notify_third_party($order, $intent);
    }
    else {
      watchdog('rovas_stripe_webhooks', 'No existing order found for payment intent @pi from regular card payment.', array('@pi' => $payment_intent_id), WATCHDOG_WARNING);
    }
  }

  return $order !== NULL;
}

/**
 * Notify 3rd-party app for rewpro purchases if token is present in PI metadata.
 *
 * Stripe webhooks run without browser session context and rovas_stripe_webhooks
 * suppresses session handling, so any session-gated notification must be driven
 * from persisted data (PaymentIntent metadata and order fields).
 */
function _rovas_stripe_webhooks_maybe_notify_third_party($order, array $intent) {
  if (empty($order) || empty($order->order_id) || empty($intent['metadata']) || !is_array($intent['metadata'])) {
    return;
  }

  if (!module_exists('rovas_project_reward') || !function_exists('rovas_project_reward_notify_purchase_with_token')) {
    return;
  }

  // Fresh load: Rules may have just notified and set rovas_stripe_purchase_notified.
  $order = commerce_order_load($order->order_id, TRUE);
  if (empty($order) || empty($order->order_id)) {
    return;
  }

  $meta = $intent['metadata'];
  $token = !empty($meta['callback_token']) ? (string) $meta['callback_token'] : '';
  if ($token === '') {
    return;
  }

  // Idempotency per order.
  if (!empty($order->data['rovas_stripe_purchase_notified'])) {
    watchdog('rovas_stripe_webhooks', 'Skipping 3rd-party notify: already notified for order @oid', array('@oid' => $order->order_id), WATCHDOG_DEBUG);
    return;
  }

  // Determine the rewarded node: prefer PI metadata, fallback to order field.
  $recipient_id = NULL;
  if (!empty($meta['project_id']) && is_numeric($meta['project_id'])) {
    $recipient_id = (int) $meta['project_id'];
  }
  elseif (!empty($order->field_payment_recipient[LANGUAGE_NONE][0]['value']) && is_numeric($order->field_payment_recipient[LANGUAGE_NONE][0]['value'])) {
    $recipient_id = (int) $order->field_payment_recipient[LANGUAGE_NONE][0]['value'];
  }
  if (empty($recipient_id)) {
    watchdog('rovas_stripe_webhooks', 'Skipping 3rd-party notify: missing recipient for order @oid', array('@oid' => $order->order_id), WATCHDOG_WARNING);
    return;
  }

  $rewarded = node_load($recipient_id);
  if (!$rewarded) {
    watchdog('rovas_stripe_webhooks', 'Skipping 3rd-party notify: recipient node @nid not found for order @oid', array('@nid' => $recipient_id, '@oid' => $order->order_id), WATCHDOG_WARNING);
    return;
  }

  $expiration = NULL;
  if (!empty($meta['callback_expiration']) && is_numeric($meta['callback_expiration'])) {
    $expiration = (int) $meta['callback_expiration'];
  }

  $paytype = !empty($meta['paytype']) ? (string) $meta['paytype'] : 'card_or_express';
  $email = NULL;
  if (!empty($intent['receipt_email'])) {
    $email = $intent['receipt_email'];
  }
  elseif (!empty($intent['charges']['data'][0]['billing_details']['email'])) {
    $email = $intent['charges']['data'][0]['billing_details']['email'];
  }

  $amount_minor = function_exists('rovas_project_reward_commerce_order_total_minor_units')
    ? rovas_project_reward_commerce_order_total_minor_units($order)
    : (!empty($order->commerce_order_total[LANGUAGE_NONE][0]['amount']) ? (int) $order->commerce_order_total[LANGUAGE_NONE][0]['amount'] : 0);

  $response = rovas_project_reward_notify_purchase_with_token($rewarded, $amount_minor, 'EUR', $email, $token, $expiration, $paytype, (int) $order->order_id);

  // Mark as attempted to prevent duplicates from Stripe retries / concurrent webhooks.
  if (empty($order->data) || !is_array($order->data)) {
    $order->data = array();
  }
  $order->data['rovas_stripe_purchase_notified'] = REQUEST_TIME;
  $order->data['rovas_stripe_purchase_notified_http'] = isset($response->code) ? (int) $response->code : 0;
  commerce_order_save($order);

  watchdog('rovas_stripe_webhooks', '3rd-party notify attempted for order @oid (HTTP @code).', array(
    '@oid' => $order->order_id,
    '@code' => isset($response->code) ? $response->code : '?',
  ), ((isset($response->code) && (int) $response->code >= 200 && (int) $response->code < 300) ? WATCHDOG_INFO : WATCHDOG_WARNING));
}

/**
 * Ensure intent is a plain array for safe use with isset/array access.
 * PHP 8+ deprecates array_key_exists() on objects; various code paths expect arrays.
 */
function _rovas_stripe_webhooks_intent_to_array($intent) {
  if (is_array($intent)) {
    return $intent;
  }
  if (is_object($intent) && class_exists('Stripe\Util\Util') && $intent instanceof \Stripe\StripeObject) {
    return \Stripe\Util\Util::convertStripeObjectToArray($intent);
  }
  if (is_object($intent)) {
    return json_decode(json_encode($intent), TRUE);
  }
  return array();
}

/**
 * Recursively convert any StripeObject instances in data to arrays.
 * Prevents array_key_exists() deprecation when order->data is used by Drupal/contrib.
 */
function _rovas_stripe_webhooks_sanitize_order_data($data) {
  if (!is_array($data) && !is_object($data)) {
    return $data;
  }
  if (is_object($data) && class_exists('Stripe\StripeObject') && $data instanceof \Stripe\StripeObject) {
    return \Stripe\Util\Util::convertStripeObjectToArray($data);
  }
  $out = array();
  foreach ((array) $data as $k => $v) {
    $out[$k] = is_array($v) || is_object($v) ? _rovas_stripe_webhooks_sanitize_order_data($v) : $v;
  }
  return $out;
}

/**
 * Add balance transaction fees to order and process forex (no Rules event).
 */
function _rovas_stripe_webhooks_add_balance_fees_and_process($order, $intent) {
  $intent = _rovas_stripe_webhooks_intent_to_array($intent);
  $intent_id = isset($intent['id']) ? $intent['id'] : NULL;
  if (!$intent_id) {
    watchdog('rovas_stripe_webhooks', 'add_balance_fees: intent has no id', array(), WATCHDOG_WARNING);
    return;
  }

  try {
    $pi = \Stripe\PaymentIntent::retrieve($intent_id);
    $charge = NULL;
    if (isset($pi->charges->data[0])) {
      $charge = $pi->charges->data[0];
    }
    elseif (isset($intent['charges']['data'][0])) {
      $charge = (object) $intent['charges']['data'][0];
    }

    // Newer Stripe API: charges list may be empty; fall back to latest_charge.
    if ((!$charge || empty($charge->balance_transaction)) && !empty($pi->latest_charge)) {
      try {
        $charge = \Stripe\Charge::retrieve($pi->latest_charge);
      }
      catch (Exception $ce) {
        watchdog('rovas_stripe_webhooks', 'Could not retrieve latest_charge @lc for PI @pi: @err', array(
          '@lc'  => $pi->latest_charge,
          '@pi'  => $intent_id,
          '@err' => $ce->getMessage(),
        ), WATCHDOG_WARNING);
      }
    }

    if (!$charge || empty($charge->balance_transaction)) {
      watchdog('rovas_stripe_webhooks', 'Charge or balance_transaction missing for PI @pi (charges count: @cnt, latest_charge: @lc)', array(
        '@pi'  => $intent_id,
        '@cnt' => isset($pi->charges->data) ? count($pi->charges->data) : 'n/a',
        '@lc'  => isset($pi->latest_charge) ? $pi->latest_charge : 'none',
      ), WATCHDOG_WARNING);
      return;
    }

    $bt = \Stripe\BalanceTransaction::retrieve($charge->balance_transaction);
    $fee_details = array();
    $fee_details_raw = isset($bt->fee_details) ? $bt->fee_details : NULL;
    if (!empty($fee_details_raw) && (is_array($fee_details_raw) || $fee_details_raw instanceof \Traversable)) {
      foreach ($fee_details_raw as $fd) {
        $amount = is_object($fd) ? (isset($fd->amount) ? $fd->amount : NULL) : (isset($fd['amount']) ? $fd['amount'] : NULL);
        $currency = is_object($fd) ? (isset($fd->currency) ? $fd->currency : NULL) : (isset($fd['currency']) ? $fd['currency'] : NULL);
        $type = is_object($fd) ? (isset($fd->type) ? $fd->type : NULL) : (isset($fd['type']) ? $fd['type'] : NULL);
        $description = is_object($fd) ? (isset($fd->description) ? $fd->description : NULL) : (isset($fd['description']) ? $fd['description'] : NULL);
        $fee_details[] = array(
          'amount' => $amount,
          'currency' => $currency,
          'type' => $type,
          'description' => $description,
        );
      }
    }

    $currency = isset($bt->currency) ? strtoupper($bt->currency) : NULL;
    $fee_decimal = isset($bt->fee) ? $bt->fee / 100 : NULL;
    $net_decimal = isset($bt->net) ? $bt->net / 100 : NULL;

    // Reload the order from DB right before saving. The two Stripe API calls
    // above take 4-6 seconds, during which the concurrent checkout form
    // handler may have set paid_in_full_invoked=TRUE and saved the order.
    // Saving the stale in-memory $order here would overwrite that flag,
    // causing the webhook to fire rules_process_card_payment_orders a second
    // time and produce duplicate emails and forex_transaction nodes.
    $order = commerce_order_load($order->order_id);
    if (!isset($order->data) || !is_array($order->data)) {
      $order->data = array();
    }

    // Sanitize existing order data to plain arrays (no Stripe objects).
    // PHP 8+ deprecates array_key_exists() on objects; Drupal/contrib may use it.
    $order->data = _rovas_stripe_webhooks_sanitize_order_data($order->data);

    $order->data['p_processor_balance_txn'] = array(
      'id' => $charge->balance_transaction,
      'amount' => isset($bt->amount) ? $bt->amount : NULL,
      'currency' => $currency,
      'fee' => $fee_decimal,
      'net' => $net_decimal,
      'reporting_category' => isset($bt->reporting_category) ? $bt->reporting_category : NULL,
      'fee_details' => $fee_details,
    );
    commerce_order_save($order);

    if (function_exists('_rovas_process_stripe_fees_for_order')) {
      _rovas_process_stripe_fees_for_order($order, $intent_id, $fee_decimal, $currency, $net_decimal);
    }
    else {
      watchdog('rovas_stripe_webhooks', '_rovas_process_stripe_fees_for_order not found — rovas_project_reward module may be missing.', array(), WATCHDOG_ERROR);
    }
  }
  catch (Exception $e) {
    watchdog('rovas_stripe_webhooks', 'Failed to add balance fees for PI @pi (order @order): @err', array(
      '@pi' => $intent_id,
      '@order' => $order->order_id,
      '@err' => $e->getMessage(),
    ), WATCHDOG_ERROR);
  }
}
