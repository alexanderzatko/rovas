<?php

/**
 * @file
 * This code is never called, it's just information about to use the hooks.
 */

/**
 * Add information to the metadata sent to Stripe.
 */
function hook_commerce_stripe_pi_metadata($order) {
  return array(
    'order_number' => $order->order_number,
  );
}

/**
 * Add information to the payment intent sent to Stripe.
 */
function hook_commerce_stripe_pi_payment_intent_data_alter(array &$payment_intent_data, EntityMetadataWrapper $order_wrapper, array $payment_method, $cardonfile_id = NULL) {
  $payment_intent_data['description'] = sprintf('Order %s', $order_wrapper->order_id->value());
}

/**
 * Make action after webhook call.
 *
 * @param object $event
 *   Webhook event object.
 * @param bool $unexpected
 *   TRUE if event is unexpected and should return error.
 */
function hook_commerce_stripe_pi_webhook_alter($event, &$unexpected) {
  // Handle the event.
  switch ($event->type) {
    case 'payment_intent.succeeded':
    case 'payment_intent.payment_failed':
      // Contains a \Stripe\PaymentIntent.
      $paymentIntent = $event->data->object;
      // Get order id from metadata.
      $order_id = $paymentIntent->metadata['order_id'];
      commerce_stripe_pi_create_transaction($order_id, $paymentIntent);
      break;

    default:
      $unexpected = TRUE;
  }
}
