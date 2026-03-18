<?php
/**
 * @file
 * Commerce Bank Transfer tokens.
 */

/**
 * Implements hook_token_info_alter().
 */
function commerce_bank_transfer_token_info_alter(&$data) {
  if (module_exists('commerce_checkout')) {
    $data['tokens']['commerce-order']['payment-bank-details'] = array(
      'name'        => t('Payment bank details'),
      'description' => t('The payment bank details.'),
    );
  }
  if (module_exists('commerce_payment')) {
    $data['tokens']['commerce-payment-transaction']['payment-bank-details'] = array(
      'name'        => t('Payment bank details'),
      'description' => t('The payment bank details.'),
    );
  }
}

/**
 * Implements hook_tokens().
 */
function commerce_bank_transfer_tokens($type, $tokens, array $data = array(), array $options = array()) {
  $replacements = array();

  if ($type == 'commerce-payment-transaction' && !empty($data['commerce-payment-transaction'])) {
    $transaction = $data['commerce-payment-transaction'];

    if (!empty($transaction->payment_method)) {
      $payment_method = commerce_payment_method_instance_load($transaction->instance_id);

      foreach ($tokens as $name => $original) {
        switch ($name) {
          case 'payment-bank-details':
            $details                 = commerce_bank_transfer_bank_details($transaction->order_id, $payment_method);
            $replacements[$original] = $details;
            break;
        }
      }
    }
  }

  if ($type == 'commerce-order' && !empty($data['commerce-order'])) {
    $order = $data['commerce-order'];

    if (!empty($order->data['payment_method'])) {
      $payment_method = commerce_payment_method_instance_load($order->data['payment_method']);

      if (!empty($payment_method) && !empty($payment_method['method_id'])) {
        foreach ($tokens as $name => $original) {
          switch ($name) {
            case 'payment-bank-details':
              $details                 = commerce_bank_transfer_bank_details($order->order_id, $payment_method);
              $replacements[$original] = $details;
              break;
          }
        }
      }
    }
  }

  return $replacements;
}
