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

      function getSubmitButtons() {
        return $('#submit-buttons button');
      }

      function getCardButton() {
        return $('#pay-choice-card');
      }

      function getBankButton() {
        return $('#pay-choice-bank');
      }

      function getECEStateKey() {
        const paymentRaw = $('input[name="payment"]').val().trim();
        const currency = ($('input[name="payment-currency"]:checked').val() || 'eur').toLowerCase();
        const email = ($('input[name="email"]').val() || '').trim().toLowerCase();
        const urlParams = new URLSearchParams(window.location.search);
        const paytype = urlParams.get('paytype') || '';
        const recipient = urlParams.get('recipient') || '';
        return [paymentRaw, currency, email, paytype, recipient].join('|');
      }

      function debouncedToggleECE() {
        clearTimeout(debounceTimeout);
        debounceTimeout = setTimeout(() => {
          toggleECEVisibility();
        }, 500);
      }

      function checkEmailFieldRequirement() {
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
        // Only enforce pprice if there's a callback parameter in the URL
        const urlParams = new URLSearchParams(window.location.search);
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
        const paymentInputLive = $('input[name="payment"]');
        const currencyInput = $('input[name="payment-currency"]:checked');
        const eceContainerLive = $('#express-checkout-container');
        console.log('[ECE] toggleECEVisibility called, paymentInputs:', paymentInputLive.length, 'currencyInputs:', currencyInput.length, 'containers:', eceContainerLive.length);
				const currencyLabel = currencyInput.val() === 'CHR' ? Drupal.t('CHR') : Drupal.t('€');

				const rawVal = $('input[name="payment"]').val().trim();
        const pprice = getPpriceValue();

				const isWholeInteger = /^[0-9]+$/.test(rawVal);
				const $errorDiv = $('#payment-error');
        
        // Clear any existing error messages first
        $errorDiv.hide();
        
        // Check for decimal validation first
				if (rawVal !== '' && !isWholeInteger) {
					$errorDiv
						.text(Drupal.t('The amount must be a whole number (no decimals).'))
						.show();
				}
        // Check for minimum amount if pprice exists
        else if (pprice !== null && rawVal !== '' && isWholeInteger) {
          const actualAmt = parseInt(rawVal, 10);
          if (actualAmt < pprice) {
//            $errorDiv
//              .text(Drupal.t('The amount must be at least @min euro.', {'@min': pprice.toFixed(2)}))
//              .show();
						$errorDiv
							.text(Drupal.t('The amount must be at least @amount @currency.', {
								'@amount': pprice.toFixed(2),
								'@currency': currencyLabel
							}))
							.show();
          }
        }

        const paymentValue = parseFloat(paymentInputLive.val() || '0');
        const actualAmt = isWholeInteger ? parseInt(rawVal, 10) : 0;
        
        const currency = (currencyInput.val() || 'eur').toLowerCase();
        const emailRequirementMet = checkEmailFieldRequirement();

        // ECE visibility logic
        const hasPprice = pprice !== null;
        const meetsMinimumAmount = hasPprice ? actualAmt >= pprice : actualAmt > 0;
        
        const shouldShowECE = meetsMinimumAmount && 
                             isWholeInteger && 
                             (currency === 'eur') && 
                             emailRequirementMet;

        console.log('[ECE] visibility calc:', {
          rawVal: rawVal,
          pprice: pprice,
          actualAmt: actualAmt,
          currency: currency,
          hasPprice: hasPprice,
          meetsMinimumAmount: meetsMinimumAmount,
          isWholeInteger: isWholeInteger,
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
					const disableSubmit = (actualAmt <= 0) || 
                               !emailRequirementMet || 
                               !isWholeInteger ||
                               (pprice !== null && actualAmt < pprice);
          
          // Apply the same logic to both buttons
					if (cardButton.length) {
            cardButton.prop('disabled', disableSubmit);
          }
          if (bankButton.length) {
            bankButton.prop('disabled', disableSubmit);
          }
        }

				if (!shouldShowECE && expressCheckoutElement) {
					expressCheckoutElement.unmount();
					expressCheckoutElement = null;
					eceInitialized = false;
					eceInitializing = false;
					if (paymentValue > 0) {
            // Re-enable both buttons when ECE is hidden and payment value is valid
            const cardButton = getCardButton();
            const bankButton = getBankButton();
            if (cardButton.length) cardButton.prop('disabled', false);
            if (bankButton.length) bankButton.prop('disabled', false);
					}
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
        const paymentInput = $('input[name="payment"]');
        const amount = parseFloat(paymentInput.val() || '0');
        console.log('[ECE] initializeStripeECE: amount=', amount, 'cents=', Math.round(amount * 100));
        if (amount <= 0) {
          console.log('[ECE] initializeStripeECE aborted: amount <= 0');
          eceInitialized = false;
			    eceInitializing = false;
          return;
        }

        const cents = Math.round(amount * 100);
        const selectedCurrencyInput = $('input[name="payment-currency"]:checked');
        const currency = (selectedCurrencyInput.val() || 'eur').toLowerCase();

        try {
          // Check for paytype and recipient URL parameters
          const urlParams = new URLSearchParams(window.location.search);
          const paytype = urlParams.get('paytype');
          const recipient = urlParams.get('recipient');
          
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
            return;
          }
          
          let fetchUrl = `/stripe/express-checkout/init?amount=${cents}&currency=${currency}`;
          
          // Add recipient and paytype parameters if both exist
          if (paytype && recipient) {
            fetchUrl += `&recipient=${encodeURIComponent(recipient)}&paytype=${encodeURIComponent(paytype)}`;
          }
          
          // Add email parameter if email field exists and has a valid value
          if (emailValue && isValidEmail(emailValue)) {
            fetchUrl += `&email=${encodeURIComponent(emailValue)}`;
          }
          
          console.log('[ECE] fetch URL:', fetchUrl);
          
          const response = await fetch(fetchUrl);
          const data = await response.json();
          console.log('[ECE] fetch response:', data.clientSecret ? 'has clientSecret' : 'NO clientSecret', data);

          if (!data.clientSecret) {
            console.log('[ECE] init aborted: no clientSecret in response');
            hideLoading();
            eceInitializing = false;
            return;
          }

          stripe = Stripe(stripePublicKey);
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
        }
      }

      // Attach event listeners using context to avoid duplicate bindings
      $('input[name="payment"]', context).once('express-checkout-payment')
        .on('input', function() { console.log('[ECE] payment input'); debouncedToggleECE(); })
        .on('blur', function() { console.log('[ECE] payment blur'); toggleECEVisibility(); })
        .on('change', function() { console.log('[ECE] payment change'); toggleECEVisibility(); });

      $('input[name="email"]', context).once('express-checkout-email').on('input', function() {
        debouncedToggleECE();
      });

      $('input[name="payment-currency"]', context).once('express-checkout-currency').on('change', function() {
        setTimeout(() => toggleECEVisibility(), 0);
      });

      // Initial visibility check
      toggleECEVisibility();
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