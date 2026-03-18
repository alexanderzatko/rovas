<?php

/**
 * @file
 * Admin functions for Rovas Stripe Webhooks module.
 */

/**
 * Form builder for the admin settings form.
 */
function rovas_stripe_webhooks_admin_form($form, &$form_state) {
  $form['webhook_info'] = array(
    '#type' => 'fieldset',
    '#title' => t('Webhook Information'),
    '#collapsible' => FALSE,
  );

  $webhook_url = url('stripe/webhook', array('absolute' => TRUE));
  $form['webhook_info']['webhook_url_display'] = array(
    '#type' => 'item',
    '#title' => t('Webhook URL'),
    '#markup' => '<code>' . check_plain($webhook_url) . '</code>',
    '#description' => t('Use this URL when configuring your webhook in the Stripe Dashboard.'),
  );

  $form['stripe_keys'] = array(
    '#type' => 'fieldset',
    '#title' => t('Stripe API Keys'),
    '#collapsible' => FALSE,
    '#description' => t('These keys are used by Express Checkout, webhook processing, and Commerce Stripe. Use the same values as in the <a href="@commerce-stripe">Commerce Stripe payment method</a> configuration.', array('@commerce-stripe' => url('admin/commerce/config/payment-methods/manage/commerce_payment_commerce_stripe_pi'))),
  );

  $form['stripe_keys']['stripe_secret_key'] = array(
    '#type' => 'textfield',
    '#title' => t('Stripe secret key'),
    '#default_value' => variable_get('stripe_secret_key', ''),
    '#description' => t('Your Stripe secret key (starts with sk_).'),
    '#size' => 60,
    '#maxlength' => 128,
    '#required' => TRUE,
  );

  $form['stripe_keys']['stripe_publishable_key'] = array(
    '#type' => 'textfield',
    '#title' => t('Stripe publishable key'),
    '#default_value' => variable_get('stripe_publishable_key', ''),
    '#description' => t('Your Stripe publishable key (starts with pk_).'),
    '#size' => 60,
    '#maxlength' => 128,
    '#required' => TRUE,
  );

  $form['webhook_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Webhook Settings'),
    '#collapsible' => FALSE,
  );

  $form['webhook_settings']['rovas_stripe_webhooks_webhook_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Webhook ID'),
    '#default_value' => variable_get('rovas_stripe_webhooks_webhook_id', ''),
    '#description' => t('The webhook ID from your Stripe Dashboard (optional, for reference only).'),
    '#size' => 60,
  );

  $form['webhook_settings']['rovas_stripe_webhooks_endpoint_secret'] = array(
    '#type' => 'textfield',
    '#title' => t('Webhook endpoint secret'),
    '#default_value' => variable_get('rovas_stripe_webhooks_endpoint_secret', ''),
    '#description' => t('Signing secret (whsec_...) for the webhook that sends events to the URL above. In Stripe Dashboard go to Developers → Webhooks, open the endpoint whose URL exactly matches this site (e.g. https://dev.rovas.app/stripe/webhook), click "Reveal" under Signing secret and paste it here. If you use Stripe CLI (stripe listen --forward-to ...), use the secret printed by the CLI, not the Dashboard secret. Wrong or mismatched secret causes "No signatures found matching the expected signature".'),
    '#size' => 60,
    '#maxlength' => 255,
  );

  $form['webhook_settings']['rovas_stripe_webhooks_debug'] = array(
    '#type' => 'checkbox',
    '#title' => t('Debug webhook signature verification'),
    '#default_value' => variable_get('rovas_stripe_webhooks_debug', 0),
    '#description' => t('When enabled, logs extra detail on webhook requests and signature failures (e.g. timestamp skew or HMAC mismatch). Disable in production.'),
  );

  $form['webhook_stats'] = array(
    '#type' => 'fieldset',
    '#title' => t('Webhook Statistics'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  // Get webhook statistics
  $total_webhooks = db_query("SELECT COUNT(*) FROM {rovas_stripe_webhooks}")->fetchField();
  $processed_webhooks = db_query("SELECT COUNT(*) FROM {rovas_stripe_webhooks} WHERE processed = 1")->fetchField();
  $pending_webhooks = $total_webhooks - $processed_webhooks;

  $form['webhook_stats']['stats_display'] = array(
    '#type' => 'item',
    '#markup' => theme('item_list', array(
      'items' => array(
        t('Total webhooks received: @count', array('@count' => $total_webhooks)),
        t('Processed webhooks: @count', array('@count' => $processed_webhooks)),
        t('Pending webhooks: @count', array('@count' => $pending_webhooks)),
      ),
    )),
  );

  // Recent webhooks
  if ($total_webhooks > 0) {
    $recent_webhooks = db_select('rovas_stripe_webhooks', 'rsw')
      ->fields('rsw', array('event_type', 'created'))
      ->orderBy('created', 'DESC')
      ->range(0, 5)
      ->execute()
      ->fetchAll();

    if ($recent_webhooks) {
      $recent_items = array();
      foreach ($recent_webhooks as $webhook) {
        $recent_items[] = t('@type - @time', array(
          '@type' => $webhook->event_type,
          '@time' => format_date($webhook->created, 'short'),
        ));
      }

      $form['webhook_stats']['recent_webhooks'] = array(
        '#type' => 'item',
        '#title' => t('Recent Webhooks (last 5)'),
        '#markup' => theme('item_list', array('items' => $recent_items)),
      );
    }
  }

  return system_settings_form($form);
}

/**
 * Validation handler for the admin settings form.
 */
function rovas_stripe_webhooks_admin_form_validate($form, &$form_state) {
  $webhook_id = $form_state['values']['rovas_stripe_webhooks_webhook_id'];
  $endpoint_secret = $form_state['values']['rovas_stripe_webhooks_endpoint_secret'];

  // Validate webhook ID format (if provided)
  if (!empty($webhook_id) && !preg_match('/^we_[a-zA-Z0-9]+$/', $webhook_id)) {
    form_set_error('rovas_stripe_webhooks_webhook_id', t('Webhook ID should start with "we_" and contain only alphanumeric characters.'));
  }

  // Validate endpoint secret format (if provided)
  if (!empty($endpoint_secret) && !preg_match('/^whsec_[a-zA-Z0-9]+$/', $endpoint_secret)) {
    form_set_error('rovas_stripe_webhooks_endpoint_secret', t('Webhook endpoint secret should start with "whsec_" and contain only alphanumeric characters.'));
  }
}

/**
 * Page callback for listing webhooks.
 */
function rovas_stripe_webhooks_admin_list() {
  $header = array(
    array('data' => t('Webhook ID'), 'field' => 'webhook_id'),
    array('data' => t('Event Type'), 'field' => 'event_type'),
    array('data' => t('Status'), 'field' => 'processed'),
    array('data' => t('Created'), 'field' => 'created', 'sort' => 'desc'),
    array('data' => t('Actions')),
  );

  $query = db_select('rovas_stripe_webhooks', 'rsw')
    ->fields('rsw')
    ->extend('PagerDefault')
    ->extend('TableSort')
    ->limit(25)
    ->orderByHeader($header);

  $results = $query->execute();

  $rows = array();
  foreach ($results as $webhook) {
    $status = $webhook->processed ? t('Processed') : t('Pending');
    $status_class = $webhook->processed ? 'status-processed' : 'status-pending';

    $actions = array(
      l(t('View'), 'admin/config/services/rovas-stripe-webhooks/view/' . $webhook->id),
    );

    $rows[] = array(
      'data' => array(
        check_plain($webhook->webhook_id),
        check_plain($webhook->event_type),
        array('data' => $status, 'class' => array($status_class)),
        format_date($webhook->created, 'short'),
        implode(' | ', $actions),
      ),
    );
  }

  $build = array();
  
  if (empty($rows)) {
    $build['empty'] = array(
      '#markup' => '<p>' . t('No webhooks have been received yet.') . '</p>',
    );
  } else {
    $build['table'] = array(
      '#theme' => 'table',
      '#header' => $header,
      '#rows' => $rows,
      '#attributes' => array('class' => array('rovas-stripe-webhooks-list')),
    );

    $build['pager'] = array(
      '#theme' => 'pager',
    );
  }

  // Add some basic CSS
  $build['#attached']['css'][] = array(
    'data' => '
      .status-processed { color: #0f5132; background-color: #d1e7dd; padding: 2px 6px; border-radius: 3px; }
      .status-pending { color: #664d03; background-color: #fff3cd; padding: 2px 6px; border-radius: 3px; }
      .rovas-stripe-webhooks-list { width: 100%; }
      .rovas-stripe-webhooks-list td, .rovas-stripe-webhooks-list th { padding: 8px; }
    ',
    'type' => 'inline',
  );

  return $build;
}

/**
 * Page callback for viewing a single webhook.
 */
function rovas_stripe_webhooks_view_webhook($webhook_id) {
  $webhook = db_select('rovas_stripe_webhooks', 'rsw')
    ->fields('rsw')
    ->condition('id', $webhook_id)
    ->execute()
    ->fetchObject();

  if (!$webhook) {
    drupal_not_found();
    return;
  }

  $build = array();
  
  $build['info'] = array(
    '#type' => 'fieldset',
    '#title' => t('Webhook Information'),
    '#collapsible' => FALSE,
  );

  $info_items = array(
    t('Webhook ID: @id', array('@id' => $webhook->webhook_id)),
    t('Event Type: @type', array('@type' => $webhook->event_type)),
    t('Status: @status', array('@status' => $webhook->processed ? t('Processed') : t('Pending'))),
    t('Received: @date', array('@date' => format_date($webhook->created))),
  );

  if ($webhook->stripe_created) {
    $info_items[] = t('Stripe Created: @date', array('@date' => format_date($webhook->stripe_created)));
  }

  $build['info']['items'] = array(
    '#markup' => theme('item_list', array('items' => $info_items)),
  );

  $build['data'] = array(
    '#type' => 'fieldset',
    '#title' => t('Webhook Data'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  // Pretty print the JSON data
  $json_data = json_decode($webhook->webhook_data, TRUE);
  $formatted_json = json_encode($json_data, JSON_PRETTY_PRINT | JSON_UNESCAPED_SLASHES);

  $build['data']['json'] = array(
    '#markup' => '<pre><code>' . check_plain($formatted_json) . '</code></pre>',
  );

  return $build;
}