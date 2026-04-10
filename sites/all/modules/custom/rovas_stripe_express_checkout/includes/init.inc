<?php

require_once DRUPAL_ROOT . '/sites/all/libraries/stripe-php/init.php';

function _rovas_stripe_express_checkout_init() {
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
    $accept_language = isset($_SERVER['HTTP_ACCEPT_LANGUAGE']) ? $_SERVER['HTTP_ACCEPT_LANGUAGE'] : '';
    $language = '';
    
    if (!empty($accept_language)) {
			$languages = explode(',', $accept_language);
			$primary_lang = trim($languages[0]);
			
			if (preg_match('/^([a-z]{2})/i', $primary_lang, $matches)) {
				$language = strtolower($matches[1]);
			}
    }

		$language_mapped = _rovas_base_map_lang_code ($language);

		$intent = \Stripe\PaymentIntent::create([
			'amount' => isset($_GET['amount']) ? (int) $_GET['amount'] : 0,
			'currency' => $currency,
			'receipt_email' => $usremail,
			'automatic_payment_methods' => ['enabled' => true],
			'metadata' => [
				'user_id' => $uid,
				'context' => 'express_checkout',
				'language' => $language,
				'language_mapped' => $language_mapped,
				'product_name' => isset($sessary['name']) ? $sessary['name'] : '',
				'project_id' => isset($_GET['recipient']) ? $_GET['recipient'] : 'none', //not only project ID, see paytypes below
				'paytype' => isset($_GET['paytype']) ? $_GET['paytype'] : 'none', //project|wreport|user
			],
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
