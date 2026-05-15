(function ($) {
  'use strict';

  // Global variables to persist across AJAX calls
  let expressCheckoutElement = null;
  let eceInitialized = false;
  let eceInitializing = false;
  let stripe = null;
  let elements = null;
  let debounceTimeout = null;
  let eceStateKey = null;

  Drupal.behaviors.expressCheckout = {
    attach: function (context, settings) {
      // Search in document - Drupal #states may hide the container in a wrapper
      // not included in context (e.g. when payment is empty, #states hides it)
      const eceContainer = $('#express-checkout-container');
      const hasContainer = eceContainer.length > 0;
      console.log('[ECE] attach: context is', context === document ? 'document' : (context.nodeName || 'fragment'), 'container in DOM:', hasContainer);

      if (!hasContainer) {
        return;
      }

      const loadingIndicator = $('#ece-loading-indicator');

      function showLoading() {
        if (loadingIndicator.length) loadingIndicator.show();
      }

      function hideLoading() {
        if (loadingIndicator.length) loadingIndicator.hide();
      }

      /** Rewpro amount lives in #payment-wrapper; avoid a stale duplicate input[name=payment] elsewhere on the page. */
      function getRewproPaymentInput() {
        const $scoped = $('#payment-wrapper input[name="payment"]');
        return $scoped.length ? $scoped.first() : $('input[name="payment"]').first();
      }

      function getSubmitButtons() {
        return $('#submit-buttons button');
      }

      function getCardButton() {
        return $('#pay-choice-card');
      }

      function getBankButton() {
        return $('#pay-choice-bank');
      }

      function normalizeLangToStripeLocale(rawLang) {
        if (!rawLang) return null;
        const v = String(rawLang).trim().toLowerCase().replace('_', '-');
        if (!v) return null;
        // Stripe locales are BCP47-ish; prefer base language (e.g. "cs" from "cs-CZ").
        const base = v.split('-')[0];
        // Keep a small allow-list to avoid passing an unsupported locale.
        const supported = {
          ar: true, bg: true, cs: true, da: true, de: true, el: true, en: true, es: true, et: true, fi: true,
          fr: true, he: true, hr: true, hu: true, id: true, it: true, ja: true, ko: true, lt: true, lv: true,
          ms: true, mt: true, nb: true, nl: true, pl: true, pt: true, ro: true, ru: true, sk: true, sl: true,
          sv: true, th: true, tr: true, vi: true, zh: true
        };
        return supported[base] ? base : null;
      }

      function getStripeLocale() {
        const urlParams = new URLSearchParams(window.location.search);
        const urlLang = urlParams.get('lang');
        const isAnon = !settings || !settings.rovasStripeExpressCheckout || !settings.rovasStripeExpressCheckout.uid;

        // Requirement: only enforce locale from ?lang for anonymous users.
        if (isAnon && urlLang) {
          return normalizeLangToStripeLocale(urlLang);
        }

        // Safe fallback: use negotiated interface lang if exposed (works for anon rewpro too).
        if (settings && settings.rovasStripeExpressCheckout && settings.rovasStripeExpressCheckout.uiLang) {
          return normalizeLangToStripeLocale(settings.rovasStripeExpressCheckout.uiLang);
        }

        return null;
      }

      function donationAckBlocksPayment() {
        return typeof Drupal.rovasRewproDonationAckAllowed === 'function' &&
          !Drupal.rovasRewproDonationAckAllowed();
      }

      function setSubmitButtonsDisabled(disabled) {
        const cardButton = getCardButton();
        const bankButton = getBankButton();
        if (cardButton.length) {
          cardButton.prop('disabled', disabled);
        }
        if (bankButton.length) {
          bankButton.prop('disabled', disabled);
        }
      }

      function getECEStateKey() {
        const paymentRaw = (getRewproPaymentInput().val() || '').trim();
        const currency = ($('input[name="payment-currency"]:checked').val() || 'eur').toLowerCase();
        const email = ($('input[name="email"]').val() || '').trim().toLowerCase();
        const urlParams = new URLSearchParams(window.location.search);
        const paytype = urlParams.get('paytype') || '';
        const recipient = urlParams.get('recipient') || '';
        return [paymentRaw, currency, email, paytype, recipient].join('|');
      }

      /** buyshares / buymerits: order-first express init + EUR decimals + payment AJAX refresh. */
      function isRewproOrderFirstContext() {
        const urlParams = new URLSearchParams(window.location.search);
        const p = urlParams.get('paytype');
        return p === 'buyshares' || p === 'buymerits';
      }

      function resetECEState() {
        if (expressCheckoutElement) {
          try {
            expressCheckoutElement.unmount();
          } catch (eUn) {
            console.log('[ECE] unmount on state reset:', eUn);
          }
          expressCheckoutElement = null;
        }
        eceInitialized = false;
        eceInitializing = false;
        eceStateKey = null;
      }

      function debouncedToggleECE() {
        clearTimeout(debounceTimeout);
        debounceTimeout = setTimeout(() => {
          toggleECEVisibility();
        }, 500);
      }

      function checkEmailFieldRequirement() {
        const urlParams = new URLSearchParams(window.location.search);
        const pt = urlParams.get('paytype');
        if (pt === 'buyshares' || pt === 'buymerits') {
          return true;
        }
        const emailField = $('input[name="email"]');
        
        if (emailField.length === 0) {
          return true;
        }
        
        return emailField.val().trim() !== '';
      }

      function isValidEmail(email) {
        const emailRegex = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;
        return emailRegex.test(email);
      }

      function getPpriceValue() {
        const urlParams = new URLSearchParams(window.location.search);
        const pt = urlParams.get('paytype');
        if (pt === 'buyshares' || pt === 'buymerits') {
          const ppriceField = $('#payment-wrapper input[name="pprice"]');
          if (ppriceField.length === 0) {
            return null;
          }
          const ppriceValue = parseFloat(ppriceField.val());
          return isNaN(ppriceValue) ? null : ppriceValue;
        }
        // Only enforce pprice if there's a callback parameter in the URL
        const hasCallback = urlParams.has('callback_url');

        if (!hasCallback) {
          return null;
        }
        
        const ppriceField = $('input[name="pprice"]');
        if (ppriceField.length === 0) {
          return null;
        }
        const ppriceValue = parseFloat(ppriceField.val());
        return isNaN(ppriceValue) ? null : ppriceValue;
      }

      function toggleECEVisibility() {
        const paymentInputLive = getRewproPaymentInput();
        const currencyInput = $('input[name="payment-currency"]:checked');
        const eceContainerLive = $('#express-checkout-container');
        console.log('[ECE] toggleECEVisibility called, paymentInputs:', paymentInputLive.length, 'currencyInputs:', currencyInput.length, 'containers:', eceContainerLive.length);

        if (donationAckBlocksPayment()) {
          if (eceContainerLive.length) {
            eceContainerLive.hide();
          }
          if (expressCheckoutElement) {
            expressCheckoutElement.unmount();
            expressCheckoutElement = null;
          }
          eceInitialized = false;
          eceInitializing = false;
          eceStateKey = null;
          setSubmitButtonsDisabled(true);
          return;
        }
				const currencyLabel = currencyInput.val() === 'CHR' ? Drupal.t('CHR') : Drupal.t('€');

				const rawVal = (getRewproPaymentInput().val() || '').trim();
        const pprice = getPpriceValue();
        const urlParamsPay = new URLSearchParams(window.location.search);
        const currency = (currencyInput.val() || 'eur').toLowerCase();
        const ptPay = urlParamsPay.get('paytype');
        const isRewproDecimalsEur = (ptPay === 'buyshares' || ptPay === 'buymerits') && currency === 'eur';
        const isValidPaymentFormat = rawVal === '' ? false : (isRewproDecimalsEur
          ? /^\d+(\.\d{1,2})?$/.test(rawVal)
          : /^[0-9]+$/.test(rawVal));

        const $errorDiv = $('#payment-error');

        // Clear any existing error messages first
        $errorDiv.hide();

        // Check for amount format: whole euros elsewhere; buyshares/buymerits EUR allow up to 2 decimals.
				if (rawVal !== '' && !isValidPaymentFormat) {
					$errorDiv
						.text(isRewproDecimalsEur
              ? Drupal.t('Enter a valid amount (up to two decimal places).')
              : Drupal.t('The amount must be a whole number (no decimals).'))
						.show();
				}
        // Check for minimum amount if pprice exists
        else if (pprice !== null && rawVal !== '' && isValidPaymentFormat) {
          const amtForMin = isRewproDecimalsEur ? parseFloat(rawVal, 10) : parseInt(rawVal, 10);
          if (amtForMin < pprice) {
						$errorDiv
							.text(Drupal.t('The amount must be at least @amount @currency.', {
								'@amount': pprice.toFixed(2),
								'@currency': currencyLabel
							}))
							.show();
          }
        }

        const paymentValue = parseFloat(paymentInputLive.val() || '0');
        const actualAmt = isValidPaymentFormat
          ? (isRewproDecimalsEur ? parseFloat(rawVal, 10) : parseInt(rawVal, 10))
          : 0;

        const emailRequirementMet = checkEmailFieldRequirement();

        // ECE visibility logic
        const hasPprice = pprice !== null;
        const meetsMinimumAmount = hasPprice ? actualAmt >= pprice : actualAmt > 0;

        const shouldShowECE = !donationAckBlocksPayment() &&
                             meetsMinimumAmount &&
                             isValidPaymentFormat &&
                             (currency === 'eur') &&
                             emailRequirementMet;

        console.log('[ECE] visibility calc:', {
          rawVal: rawVal,
          pprice: pprice,
          actualAmt: actualAmt,
          currency: currency,
          hasPprice: hasPprice,
          meetsMinimumAmount: meetsMinimumAmount,
          isValidPaymentFormat: isValidPaymentFormat,
          isRewproDecimalsEur: isRewproDecimalsEur,
          emailRequirementMet: emailRequirementMet,
          shouldShowECE: shouldShowECE,
          containerInDOM: eceContainerLive.length,
          containerVisible: eceContainerLive.length ? eceContainerLive.is(':visible') : 'n/a',
          containerParent: eceContainerLive.length ? eceContainerLive.parent().attr('id') || eceContainerLive.parent().attr('class') : 'n/a'
        });

        // Reset only when hidden. Re-init while visible is handled by state-key changes.
        if (!shouldShowECE) {
          eceInitialized = false;
          eceStateKey = null;
        }

        if (eceContainerLive.length) {
          if (shouldShowECE) {
            console.log('[ECE] scheduling container show (setTimeout 0)');
            setTimeout(function() {
              eceContainerLive.toggle(true);
              console.log('[ECE] container toggled visible, isVisible:', eceContainerLive.is(':visible'), 'parentVisible:', eceContainerLive.parent().is(':visible'));
            }, 0);
          } else {
            eceContainerLive.toggle(false);
            console.log('[ECE] container hidden (shouldShowECE false)');
          }
        } else {
          console.log('[ECE] no container to toggle');
        }

        const cardButton = getCardButton();
        const bankButton = getBankButton();
        
        if (cardButton.length || bankButton.length) {
          // Disable submit buttons if:
          // - Amount is <= 0
          // - Email requirement not met
          // - Not a whole integer
          // - Amount is less than pprice (when pprice exists)
					const disableSubmit = donationAckBlocksPayment() ||
                               (actualAmt <= 0) ||
                               !emailRequirementMet ||
                               !isValidPaymentFormat ||
                               (pprice !== null && actualAmt < pprice);
          
          // Apply the same logic to both buttons
          setSubmitButtonsDisabled(disableSubmit);
        }

        if (!shouldShowECE && expressCheckoutElement) {
					expressCheckoutElement.unmount();
					expressCheckoutElement = null;
					eceInitialized = false;
					eceInitializing = false;
					if (paymentValue > 0 && !donationAckBlocksPayment()) {
            // Re-enable both buttons when ECE is hidden and payment value is valid
            setSubmitButtonsDisabled(false);
					}
				}

        function expressCheckoutMountEmpty() {
          const el = document.getElementById('express-checkout-element');
          return !el || el.childElementCount === 0;
        }

        // Stripe mount can be lost while flags still say "ready" (e.g. after Drupal AJAX replaces markup).
        if (shouldShowECE && eceInitialized && expressCheckoutMountEmpty()) {
          eceInitialized = false;
          eceInitializing = false;
        }

        const nextStateKey = shouldShowECE ? getECEStateKey() : null;
        const stateChanged = shouldShowECE && eceStateKey !== nextStateKey;

        if (stateChanged && expressCheckoutElement) {
          expressCheckoutElement.unmount();
          expressCheckoutElement = null;
          eceInitialized = false;
          eceInitializing = false;
        }

				if (shouldShowECE && (!eceInitialized || stateChanged) && !eceInitializing) {
					console.log('[ECE] calling initializeStripeECE');
					eceInitializing = true;
          eceStateKey = nextStateKey;
					showLoading();
					// Disable both buttons while ECE is loading
          const cardButton = getCardButton();
          const bankButton = getBankButton();
          if (cardButton.length) cardButton.prop('disabled', true);
          if (bankButton.length) bankButton.prop('disabled', true);
					initializeStripeECE();
				}
      }

      async function initializeStripeECE() {
        if (donationAckBlocksPayment()) {
          console.log('[ECE] initializeStripeECE aborted: donation ack not checked');
          eceInitialized = false;
          eceInitializing = false;
          eceStateKey = null;
          hideLoading();
          setSubmitButtonsDisabled(true);
          return;
        }

        const paymentInput = getRewproPaymentInput();
        const amount = parseFloat(paymentInput.val() || '0');
        console.log('[ECE] initializeStripeECE: amount=', amount, 'cents=', Math.round(amount * 100));
        if (amount <= 0) {
          console.log('[ECE] initializeStripeECE aborted: amount <= 0');
          eceInitialized = false;
			    eceInitializing = false;
          eceStateKey = null;
          hideLoading();
          if (!donationAckBlocksPayment()) {
            setSubmitButtonsDisabled(false);
          }
          return;
        }

        let cents = Math.round(amount * 100);
        const selectedCurrencyInput = $('input[name="payment-currency"]:checked');
        const currency = (selectedCurrencyInput.val() || 'eur').toLowerCase();

        try {
          // Check for paytype and recipient URL parameters
          const urlParams = new URLSearchParams(window.location.search);
          const paytype = urlParams.get('paytype');
          const recipient = urlParams.get('recipient');
          const rewproLang = urlParams.get('lang');
          const langQuery = rewproLang
            ? ('&lang=' + encodeURIComponent(rewproLang))
            : '';

          let fetchUrl;

          if (paytype === 'buyshares' && recipient && Drupal.settings.rovasExpressBuyshares && Drupal.settings.rovasExpressBuyshares.token) {
            fetchUrl = '/stripe/express-checkout/init-buyshares?amount=' + cents
              + '&recipient=' + encodeURIComponent(recipient)
              + '&token=' + encodeURIComponent(Drupal.settings.rovasExpressBuyshares.token)
              + langQuery;
          }
          else if (paytype === 'buyshares') {
            console.log('[ECE] buyshares init aborted: missing Drupal.settings.rovasExpressBuyshares.token');
            hideLoading();
            eceInitializing = false;
            eceStateKey = null;
            if (!donationAckBlocksPayment()) {
              setSubmitButtonsDisabled(false);
            }
            return;
          }
          else if (paytype === 'buymerits' && recipient && Drupal.settings.rovasExpressBuymerits && Drupal.settings.rovasExpressBuymerits.token) {
            fetchUrl = '/stripe/express-checkout/init-buymerits?amount=' + cents
              + '&recipient=' + encodeURIComponent(recipient)
              + '&token=' + encodeURIComponent(Drupal.settings.rovasExpressBuymerits.token)
              + langQuery;
          }
          else if (paytype === 'buymerits') {
            console.log('[ECE] buymerits init aborted: missing Drupal.settings.rovasExpressBuymerits.token');
            hideLoading();
            eceInitializing = false;
            eceStateKey = null;
            if (!donationAckBlocksPayment()) {
              setSubmitButtonsDisabled(false);
            }
            return;
          }
          else {
          // Check for email field value
          const emailField = $('input.form-control.form-text.required[type="text"][id="edit-email"][name="email"]');
          const emailFieldAlt = $('input[name="email"]');
          const emailValue = (emailField.val() || emailFieldAlt.val() || '').trim();
          console.log('[ECE] init: emailField(strict)=', emailField.length, 'emailFieldAlt(any)=', emailFieldAlt.length, 'emailValue=', emailValue ? emailValue.substring(0, 5) + '...' : '(empty)');
          
          // Validate email before making the fetch request
          if (emailValue && !isValidEmail(emailValue)) {
            console.log('[ECE] init aborted: invalid email');
            hideLoading();
            eceInitializing = false;
            eceStateKey = null;
            const cEm = getCardButton();
            const bEm = getBankButton();
            if (cEm.length) cEm.prop('disabled', false);
            if (bEm.length) bEm.prop('disabled', false);
            return;
          }
          
          fetchUrl = `/stripe/express-checkout/init?amount=${cents}&currency=${currency}`;
          
          // Add recipient and paytype parameters if both exist
          if (paytype && recipient) {
            fetchUrl += `&recipient=${encodeURIComponent(recipient)}&paytype=${encodeURIComponent(paytype)}`;
          }

          fetchUrl += langQuery;
          
          // Add email parameter if email field exists and has a valid value
          if (emailValue && isValidEmail(emailValue)) {
            fetchUrl += `&email=${encodeURIComponent(emailValue)}`;
          }
          }
          
          console.log('[ECE] fetch URL:', fetchUrl);
          
          const response = await fetch(fetchUrl);
          const data = await response.json();
          console.log('[ECE] fetch response:', data.clientSecret ? 'has clientSecret' : 'NO clientSecret', data);

          if (!data.clientSecret) {
            console.log('[ECE] init aborted: no clientSecret in response');
            hideLoading();
            eceInitializing = false;
            eceInitialized = false;
            eceStateKey = null;
            const c1 = getCardButton();
            const b1 = getBankButton();
            if (c1.length) c1.prop('disabled', false);
            if (b1.length) b1.prop('disabled', false);
            return;
          }

          const stripeLocale = getStripeLocale();
          stripe = stripeLocale ? Stripe(stripePublicKey, { locale: stripeLocale }) : Stripe(stripePublicKey);
          elements = stripe.elements({ clientSecret: data.clientSecret });

          if (expressCheckoutElement) {
            expressCheckoutElement.unmount();
          }

          expressCheckoutElement = elements.create('expressCheckout', {
            layout: { type: 'auto' }
          });

          expressCheckoutElement.mount('#express-checkout-element');
          console.log('[ECE] Stripe element mounted');

          expressCheckoutElement.on('ready', function () {
            hideLoading();
            eceInitialized = true;
				    eceInitializing = false;
            // Re-enable both buttons when ECE is ready
            const cardButton = getCardButton();
            const bankButton = getBankButton();
            if (cardButton.length) cardButton.prop('disabled', false);
            if (bankButton.length) bankButton.prop('disabled', false);
          });

          expressCheckoutElement.on('confirm', async (event) => {
            showLoading();
          
            try {
              const result = await stripe.confirmPayment({
                elements: elements,
                confirmParams: {
                  return_url: window.location.origin + "/stripe/express-checkout/thank-you"
                }
              });
          
              if (result.error) {
                alert("Payment failed: " + result.error.message);
              }
            } catch (err) {
              // Handle error
            } finally {
              hideLoading();
            }
          });

        } catch (err) {
          console.log('[ECE] init error:', err);
          hideLoading();
			    eceInitializing = false;
          const cardErr = getCardButton();
          const bankErr = getBankButton();
          if (cardErr.length) cardErr.prop('disabled', false);
          if (bankErr.length) bankErr.prop('disabled', false);
        }
      }

      $(document).off('rovasRewproPaymentPreviewed.rovasExpressEce').on('rovasRewproPaymentPreviewed.rovasExpressEce', function () {
        const urlParamsAjax = new URLSearchParams(window.location.search);
        const ptAjax = urlParamsAjax.get('paytype');
        if (ptAjax !== 'buyshares' && ptAjax !== 'buymerits') {
          return;
        }
        if (expressCheckoutElement) {
          try {
            expressCheckoutElement.unmount();
          } catch (eUn) {
            console.log('[ECE] unmount after rewpro payment ajax:', eUn);
          }
          expressCheckoutElement = null;
        }
        eceInitialized = false;
        eceInitializing = false;
        eceStateKey = null;
        setTimeout(function () {
          toggleECEVisibility();
        }, 0);
      });

      // Attach event listeners using context to avoid duplicate bindings
      $('#payment-wrapper input[name="payment"]').once('express-checkout-payment')
        .on('input', function() {
          console.log('[ECE] payment input');
          if (isRewproOrderFirstContext()) {
            return;
          }
          debouncedToggleECE();
        })
        .on('blur', function() {
          console.log('[ECE] payment blur');
          if (isRewproOrderFirstContext()) {
            return;
          }
          toggleECEVisibility();
        })
        .on('change', function() {
          console.log('[ECE] payment change');
          if (isRewproOrderFirstContext()) {
            return;
          }
          toggleECEVisibility();
        });

      $('input[name="email"]', context).once('express-checkout-email').on('input', function() {
        debouncedToggleECE();
      });

      $('input[name="payment-currency"]', context).once('express-checkout-currency').on('change', function() {
        if (isRewproOrderFirstContext()) {
          resetECEState();
          const eceContainerLive = $('#express-checkout-container');
          if (eceContainerLive.length) {
            eceContainerLive.toggle(false);
          }
          const selectedCurrency = (($('input[name="payment-currency"]:checked').val() || '') + '').toUpperCase();
          const amountRaw = (getRewproPaymentInput().val() || '').trim();
          // Order-first rewpro: only refresh ECE on currency switch when moving to EUR
          // and there is an entered amount to evaluate.
          if (selectedCurrency === 'EUR' && amountRaw !== '') {
            setTimeout(() => toggleECEVisibility(), 0);
          }
          return;
        }
        setTimeout(() => toggleECEVisibility(), 0);
      });

      $(document).once('rewpro-donation-ack-ece').on('rovasRewproDonationAckChanged', function () {
        if (isRewproOrderFirstContext()) {
          return;
        }
        toggleECEVisibility();
      });

      // Initial visibility check.
      // On buyshares/buymerits, let payment preview AJAX drive ECE refresh.
      if (!isRewproOrderFirstContext()) {
        toggleECEVisibility();
      }
    },

    detach: function (context, settings, trigger) {
      // Clean up when elements are removed (e.g., during AJAX refresh)
      if (trigger === 'unload') {
        if (expressCheckoutElement) {
          expressCheckoutElement.unmount();
          expressCheckoutElement = null;
        }
        eceInitialized = false;
        eceStateKey = null;
        clearTimeout(debounceTimeout);
      }
    }
  };

})(jQuery);