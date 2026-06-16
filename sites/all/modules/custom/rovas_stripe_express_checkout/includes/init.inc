<?php

require_once DRUPAL_ROOT . '/sites/all/libraries/stripe-php/init.php';

/**
 * Short language label and normalized langcode for Stripe metadata.
 *
 * Prefer rewpro ?lang= (passed by express_checkout.js), then session from
 * /rewpro negotiation, then Drupal interface language, then Accept-Language.
 *
 * @return array
 *   Numeric keys 0 and 1: metadata keys "language" and "language_mapped".
 */
function _rovas_stripe_express_checkout_init_language_metadata() {
  if (!empty($_GET['lang']) && function_exists('_rovas_base_map_lang_code')) {
    $mapped = _rovas_base_map_lang_code($_GET['lang']);
    return array($mapped, $mapped);
  }
  if (session_id() && !empty($_SESSION['rovas_rewpro_ui_lang']) && function_exists('_rovas_base_map_lang_code')) {
    $mapped = _rovas_base_map_lang_code($_SESSION['rovas_rewpro_ui_lang']);
    return array($mapped, $mapped);
  }
  global $language;
  if (!empty($language->language) && function_exists('_rovas_base_map_lang_code')) {
    $mapped = _rovas_base_map_lang_code($language->language);
    return array($mapped, $mapped);
  }

  $accept_language = isset($_SERVER['HTTP_ACCEPT_LANGUAGE']) ? $_SERVER['HTTP_ACCEPT_LANGUAGE'] : '';
  $parsed = '';
  if (!empty($accept_language)) {
    $languages = explode(',', $accept_language);
    $primary_lang = trim($languages[0]);
    if (preg_match('/^([a-z]{2})/i', $primary_lang, $matches)) {
      $parsed = drupal_strtolower($matches[1]);
    }
  }
  $language_mapped = function_exists('_rovas_base_map_lang_code')
    ? _rovas_base_map_lang_code($parsed)
    : 'en';
  return array($parsed, $language_mapped);
}

function _rovas_stripe_express_checkout_init() {
  // Refuse to start a PaymentIntent when the site has fiat rails off; only
  // CHR payments are accepted in that mode (see admin/config/rovas/payment-methods).
  if (function_exists('rovas_project_reward_fiat_disabled') && rovas_project_reward_fiat_disabled()) {
    watchdog('rovas_stripe_express', 'Express init refused: fiat payments disabled.', array(), WATCHDOG_WARNING);
    drupal_json_output(array('error' => t('Card payments are disabled.')));
    drupal_exit();
  }

  $secret_key = variable_get('stripe_secret_key', '');

	$sessary = rovas_project_reward_session_get('rovas_project_reward', array());

	$callback_url = isset($sessary['callback_url']) ? $sessary['callback_url'] : '';
	$expiration = isset($sessary['expiration']) ? $sessary['expiration'] : INF;
	$curtime = time();
	
	if ($expiration < $curtime) {
		watchdog('xpress checkout init', '3rd party session expired.');
    drupal_exit();
	}

  if (!$secret_key) {
    watchdog('xpress checkout init', 'Stripe secret key not found.', [], WATCHDOG_ERROR);
    drupal_json_output(['error' => 'Stripe configuration error.']);
    drupal_exit();
  }

  \Stripe\Stripe::setApiKey($secret_key);

  global $user;
  $uid = $user->uid;

	if ($uid > 0 ) {
		$usremail = $user->mail;
	}
	else if (isset($_GET['email'])) {
		$usremail = $_GET['email'];
	}
	else {
		$usremail = '';
	}

  $currency = 'eur';

  try {
    list($language, $language_mapped) = _rovas_stripe_express_checkout_init_language_metadata();

		$metadata = [
			'user_id' => $uid,
			'context' => 'express_checkout',
			'language' => $language,
			'language_mapped' => $language_mapped,
			'product_name' => isset($sessary['name']) ? $sessary['name'] : '',
			'project_id' => isset($_GET['recipient']) ? $_GET['recipient'] : 'none', //not only project ID, see paytypes below
			'paytype' => isset($_GET['paytype']) ? $_GET['paytype'] : 'none', //project|wreport|user
		];

		// Critical for webhook-side 3rd-party notification: Stripe webhooks run
		// without browser session context (and rovas_stripe_webhooks suppresses
		// session writes). Persist the callback token/expiration on the PaymentIntent
		// so payment_intent.succeeded can notify even when the session is gone.
		if (!empty($sessary['token']) && is_string($sessary['token'])) {
			$metadata['callback_token'] = $sessary['token'];
		}
		if (!empty($sessary['expiration']) && $sessary['expiration'] !== INF && is_numeric($sessary['expiration'])) {
			$metadata['callback_expiration'] = (string) ((int) $sessary['expiration']);
		}

		$intent = \Stripe\PaymentIntent::create([
			'amount' => isset($_GET['amount']) ? (int) $_GET['amount'] : 0,
			'currency' => $currency,
			'receipt_email' => $usremail,
			'automatic_payment_methods' => ['enabled' => true],
			'metadata' => $metadata,
		]);

    watchdog('xpress checkout init', 'intent: <pre>' . print_r($intent,1) . '</pre>');

    drupal_json_output([
      'clientSecret' => $intent->client_secret,
    ]);
  }
  catch (\Stripe\Exception\ApiErrorException $e) {
    watchdog('xpress checkout init', 'PaymentIntent creation failed: @error', ['@error' => $e->getMessage()], WATCHDOG_ERROR);
    drupal_json_output(['error' => $e->getMessage()]);
  }

  drupal_exit();
}
