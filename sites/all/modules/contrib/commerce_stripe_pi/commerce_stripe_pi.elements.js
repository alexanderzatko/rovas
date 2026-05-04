(function ($) {

  String.prototype.capitalize = function() {
    return this.charAt(0).toUpperCase() + this.slice(1);
  };

  Drupal.behaviors.commerce_stripe_pi_elements = {
    /**
     * Attach Stripe behavior to form elements.
     *
     * @param context
     *   An element to attach behavior to.
     * @param settings
     *   An object containing settings for the current context.
     */
    attach: function (context, settings)  {
      var self = this;

      if (typeof settings.stripe_pi !== 'undefined') {

        var style = {
          base: {
            // Add your base input styles here. For example:
            fontSize: '16px',
            lineHeight: '24px'
          }
        };

        $("#card-element").once('elements', function() {
          // Create an instance of Stripe Elements
          self.stripe_pi = Stripe(settings.stripe_pi.publicKey);
          self.elements = self.stripe_pi.elements();
          self.card = {};

          // If we are not using an exsiting cardonfile, load stripe card.
          if (!($("input[type='radio'][name='commerce_payment[payment_details][cardonfile]']").length
              && $("input[type='radio'][name='commerce_payment[payment_details][cardonfile]']:checked").val() != 'new')) {
            // Create an instance of the card Element
            self.card = self.elements.create('card', {style: style, hidePostalCode: settings.stripe_pi.hide_postal_code});
            self.card.mount(this);

            self.card.addEventListener('change', function (event) {
              var submitButton$ = $('#edit-submit');
              var submit_text = submitButton$.val();
              var cardErrors$ = $("#card-errors");
              if (event.error) {
                cardErrors$.text(event.error.message);
                cardErrors$.addClass('messages');
                cardErrors$.addClass('error');
                submitButton$.val(submit_text);
                submitButton$.removeAttr("disabled");
                submitButton$.removeClass('disabled');
              }
              else {
                cardErrors$.text('');
                cardErrors$.removeClass('messages');
                cardErrors$.removeClass('error');
              }
            });
          }

          // Remove previous attached events.
          $('body').undelegate('.checkout-buttons #edit-continue', 'click');
          // Attach the JS behaviors just once to the available card element.
          $('body').delegate('.checkout-buttons #edit-continue', 'click', function(event) {
            // Prevent the Stripe actions from being triggered if Stripe is not the selected payment method.
            if ($("input[value*='commerce_stripe_pi|']").is(':checked')) {
              // Do not fetch the token if cardonfile is enabled and the customer has selected an existing card.
              if ($('.form-item-commerce-payment-payment-details-cardonfile').length) {
                // If select list enabled in card on file settings
                if ($("select[name='commerce_payment[payment_details][cardonfile]']").length
                  && $("select[name='commerce_payment[payment_details][cardonfile]'] option:selected").val() != 'new') {
                  return;
                }
              }

              // Do not submit if no radio buttons are selected and there is no
              // cardonfile.
              if ($("input[type='radio'][name='commerce_payment[payment_details][cardonfile]']").length !== 0 && $("input[type='radio'][name='commerce_payment[payment_details][cardonfile]']:checked").length === 0) {
                // Inform customer he should select a radio.
                var cardErrors$ = $("#card-errors");
                cardErrors$.text(Drupal.t("Please select a payment before submitting the order."));
                cardErrors$.addClass("messages");
                cardErrors$.addClass("error");
                return false;
              }

              $('.checkout-processing').hide();
              var form$ = $(this).closest("form", context);
              var submitButton$ = $('.checkout-buttons #edit-continue');
              var submit_text = submitButton$.val();

              submitButton$.removeAttr("disabled");
              submitButton$.removeClass('disabled');

              // Tell the form to let Stripe Elements handle the click event.
              // Prevent the Stripe actions from being triggered if Stripe is not the selected payment method.
              if ($("input[value*='commerce_stripe_pi|']").is(':checked')) {
                var submitButton$ = $('.checkout-buttons #edit-continue');
                var submit_text = submitButton$.val();

                submitButton$.text('Please wait...');
                submitButton$.attr("disabled", "disabled");
                submitButton$.addClass('disabled');
                submitButton$.addClass('auth-processing');
                $('.checkout-processing').show();

                self.stripe_pi.handleCardPayment(Drupal.settings.stripe_pi.payment_intent.client_secret, self.card, Drupal.behaviors.commerce_stripe_pi_elements.extractTokenData(form$)).then(function (result) {
                  var cardErrors$ = $("#card-errors");
                  if (result.error) {
                    // In case of cardonfile authentication failure, uncheck the
                    // radio to force the reload of payment intent on next
                    // change. Otherwise, customer will be able to re-submit the
                    // same cardonfile without reloading payment intent which will
                    // always fail.
                    var cardonfileSelected = form$.find("input[name='commerce_payment[payment_details][cardonfile]']:checked");
                    if (cardonfileSelected.length === 1 && form$.find("input[name='commerce_payment[payment_details][cardonfile]']:checked").val() !== "new") {
                      cardonfileSelected.removeAttr('checked');
                    }
                    var submitButton$ = $('.checkout-buttons #edit-continue');
                    var submit_text = submitButton$.val();
                    submitButton$.text(submit_text);
                    submitButton$.removeAttr("disabled");
                    submitButton$.removeClass("disabled");
                    // Inform the user if there was an error
                    if (result.error.code === "payment_intent_unexpected_state" && result.error.payment_intent.status === "canceled") {
                      cardErrors$.text(Drupal.t("Your payment has been canceled. Please refresh your page or change payment method."));
                    }
                    else {
                      cardErrors$.text(result.error.message);
                    }
                    $(document).trigger( "commerce_stripe_pi:error", [result.error]);
                    cardErrors$.addClass("messages");
                    cardErrors$.addClass("error");
                    // // Prevent duplicate submissions to stripe from multiple clicks
                    // if ($(this).hasClass('auth-processing')) {
                    //   return false;
                    // }
                    submitButton$.removeClass("auth-processing");
                    $(".checkout-processing").hide();

                    Drupal.attachBehaviors(context);
                  }
                  else {

                    cardErrors$.text('');
                    cardErrors$.removeClass('messages');
                    cardErrors$.removeClass('error');
                    // Send the payment intent id to your server.
                    $('#stripe_pi_payment_intent').val(result.paymentIntent.id);
                    var submitButton$ = $('.checkout-buttons #edit-continue');
                    var submit_text = submitButton$.val();
                    // Set a triggering element for the form.
                    var trigger$ = $("<input type='hidden' />").attr('name', submitButton$.attr('name')).attr('value', submitButton$.attr('value'));
                    form$.append(trigger$);

                    // And submit.
                    form$.get(0).submit(form$);
                  }
                });

                var cardErrors$ = $("#card-errors");
                cardErrors$.text('');
                cardErrors$.removeClass('messages');
                cardErrors$.removeClass('error');

                event.preventDefault();
                return false;
              }
            }
          });

          $('#commerce-stripe-pi-cardonfile-create-form').delegate('#edit-submit', 'click', function (event) {

            var form$ = $(this).closest("form", context);
            var submitButton$ = $('#edit-submit');
            var submit_text = submitButton$.val();

            submitButton$.removeAttr("disabled");
            submitButton$.removeClass('disabled');

            // Tell the form to let Stripe Elements handle the click event.

            // Prevent the Stripe actions from being triggered if Stripe is not the selected payment method.

            var submitButton$ = $('#edit-submit');
            // console.log('Initiating Checkout.');
            submitButton$.text('Please wait...');
            submitButton$.attr("disabled", "disabled");
            submitButton$.addClass('disabled');
            submitButton$.addClass('auth-processing');
            $('.checkout-processing').show();

            // Do not fetch the token if cardonfile is enabled and the customer has selected an existing card.
            if ($('.form-item-commerce-payment-payment-details-cardonfile').length) {
              // If select list enabled in card on file settings
              if ($("select[name='commerce_payment[payment_details][cardonfile]']").length
                && $("select[name='commerce_payment[payment_details][cardonfile]'] option:selected").val() != 'new') {
                return;
              }

              // Do not submit if no radio buttons are selected and there is no
              // cardonfile.
              if ($("input[type='radio'][name='commerce_payment[payment_details][cardonfile]']").length !== 0 && $("input[type='radio'][name='commerce_payment[payment_details][cardonfile]']:checked").length === 0) {
                // Inform customer he should select a radio.
                var cardErrors$ = $("#card-errors");
                cardErrors$.text(Drupal.t("Please select a payment before submitting the order."));
                cardErrors$.addClass("messages");
                cardErrors$.addClass("error");
                return false;
              }

              // If radio buttons are enabled in card on file settings
              if ($("input[type='radio'][name='commerce_payment[payment_details][cardonfile]']").length
                && $("input[type='radio'][name='commerce_payment[payment_details][cardonfile]']:checked").val() != 'new') {
                return;
              }
            }

            self.stripe_pi.handleCardSetup(Drupal.settings.stripe_pi.payment_intent.client_secret, self.card, Drupal.behaviors.commerce_stripe_pi_elements.extractTokenData(form$)).then(function(result) {
              var cardErrors$ = $("#card-errors");
              if (result.error) {
                // In case of cardonfile authentication failure, uncheck the
                // radio to force the reload of payment intent on next
                // change. Otherwise, customer will be able to re-submit the
                // same cardonfile without reloading payment intent which will
                // always fail.
                var cardonfileSelected = form$.find("input[name='commerce_payment[payment_details][cardonfile]']:checked");
                if (cardonfileSelected.length === 1 && form$.find("input[name='commerce_payment[payment_details][cardonfile]']:checked").val() !== "new") {
                  cardonfileSelected.removeAttr('checked');
                }
                var submitButton$ = $('#edit-submit');
                submitButton$.text(submit_text);
                submitButton$.removeAttr("disabled");
                submitButton$.removeClass('disabled');
                // Inform the user if there was an error
                if (result.error.code === 'payment_intent_unexpected_state' && result.error.payment_intent.status === 'canceled') {
                  cardErrors$.text(Drupal.t('Your payment has been canceled. Please refresh your page or change payment method.'));
                }
                else {
                  cardErrors$.text(result.error.message);
                }
                $(document).trigger( "commerce_stripe_pi:error", [result.error]);
                cardErrors$.addClass('messages');
                cardErrors$.addClass('error');
                // // Prevent duplicate submissions to stripe from multiple clicks
                // if ($(this).hasClass('auth-processing')) {
                //   return false;
                // }
                submitButton$.removeClass('auth-processing');
                $('.checkout-processing').hide();

                Drupal.attachBehaviors(context);
              }
              else {
                cardErrors$.text('');
                cardErrors$.removeClass('messages');
                cardErrors$.removeClass('error');
                // Send the payement intent id to your server.
                $('#stripe_pi_payment_intent').val(result.setupIntent.id);
                var submitButton$ = $('#edit-submit');
                // Set a triggering element for the form.
                var trigger$ = $("<input type='hidden' />").attr('name', submitButton$.attr('name')).attr('value', submitButton$.attr('value'));
                form$.append(trigger$);

                // And submit.
                form$.get(0).submit(form$);
              }
            });

            var cardErrors$ = $("#card-errors");
            cardErrors$.text('');
            cardErrors$.removeClass('messages');
            cardErrors$.removeClass('error');

            event.preventDefault();
            return false;


          });

          $('.page-admin-commerce-orders-payment').delegate('#edit-submit', 'click', function(event) {
            // Prevent the Stripe actions to be triggered if hidden field hasn't been set
            var cs_terminal = $('input[name=commerce_stripe_pi_terminal]').val();
            if ( cs_terminal > 0) {
              var submitButton$ = $('#edit-submit');
              submitButton$.addClass('auth-processing');
              var form$ = $(this).closest("form", context);

              // Prevent the form from submitting with the default action.
              event.preventDefault();

              // Disable the submit button to prevent repeated clicks.
              $('.form-submit').attr("disabled", "disabled");

              var cardFields = {
                number: 'edit-payment-details-credit-card-number',
                cvc: 'edit-payment-details-credit-card-code',
                exp_month: 'edit-payment-details-credit-card-exp-month',
                exp_year: 'edit-payment-details-credit-card-exp-year',
                name: 'edit-payment-details-credit-card-owner'
              };

              // add3DSecure();

              self.stripe_pi['create' + self.creationType.capitalize()](self.card, Drupal.behaviors.commerce_stripe_pi_elements.extractTokenData(form$)).then(function(result) {
                var cardErrors$ = $("#card-errors");
                if (result.error) {
                  // In case of cardonfile authentication failure, uncheck the
                  // radio to force the reload of payment intent on next
                  // change. Otherwise, customer will be able to re-submit the
                  // same cardonfile without reloading payment intent which will
                  // always fail.
                  var cardonfileSelected = form$.find("input[name='commerce_payment[payment_details][cardonfile]']:checked");
                  if (cardonfileSelected.length === 1 && form$.find("input[name='commerce_payment[payment_details][cardonfile]']:checked").val() !== "new") {
                    cardonfileSelected.removeAttr('checked');
                  }
                  var submitButton$ = $('#edit-submit');
                  var submit_text = submitButton$.val();
                  submitButton$.text(submit_text);
                  submitButton$.removeAttr("disabled");
                  submitButton$.removeClass('disabled');
                  // Inform the user if there was an error
                  if (result.error.code === 'payment_intent_unexpected_state' && result.error.payment_intent.status === 'canceled') {
                    cardErrors$.text(Drupal.t('Your payment has been canceled. Please refresh your page or change payment method.'));
                  }
                  else {
                    cardErrors$.text(result.error.message);
                  }
                  $(document).trigger( "commerce_stripe_pi:error", [result.error]);
                  cardErrors$.addClass('messages');
                  cardErrors$.addClass('error');
                  // // Prevent duplicate submissions to stripe from multiple clicks
                  if (submitButton$.hasClass('auth-processing')) {
                    return false;
                  }
                  submitButton$.removeClass('auth-processing');
                  $('.checkout-processing').hide();

                  Drupal.attachBehaviors(context);
                }
                else {
                  cardErrors$.text('');
                  cardErrors$.removeClass('messages');
                  cardErrors$.removeClass('error');
                  // Send the token to your server
                  $('#stripe_pi_token').val(result.token.id);
                  var submitButton$ = $('#edit-submit');
                  var submit_text = submitButton$.val();
                  // Set a triggering element for the form.
                  var trigger$ = $("<input type='hidden' />").attr('name', submitButton$.attr('name')).attr('value', submitButton$.attr('value'));
                  form$.append(trigger$);

                  // And submit.
                  form$.get(0).submit(form$);
                }
              });

              var cardErrors$ = $("#card-errors");
              cardErrors$.text('');
              cardErrors$.removeClass('messages');
              cardErrors$.removeClass('error');

              event.preventDefault();
              // Prevent the form from submitting with the default action.
              return false;
            }
          });
        });
      }

    },
    detach: function(context, settings, trigger) {

      if (trigger === 'unload') {
        // Remove error message for unloaded Stripe inputs.
        var cardErrors$ = $("#card-errors");
        cardErrors$.removeClass('messages');
        cardErrors$.removeClass('error');
      }
    },
    /**
     * Extract token creation data from a form.
     *
     * Stripe_pi.createToken() should support the form as first argument and pull the information
     * from inputs marked up with the data-stripe attribute. But it does not seems to properly
     * pull value from <select> elements for the 'exp_month' and 'exp_year' fields.
     *
     */
    extractTokenData: function(form) {
      var data = {};
      // Do not extract token data if creation type is not token.
      if (this.creationType !== 'token') {
        return data;
      }

      $(':input[data-stripe]').not('[data-stripe="token"]').each(function() {
        var input = $(this);
        data[input.attr('data-stripe')] = input.val();
      });

      var optionalFieldMap = {
        address_line1: 'commerce-stripe-pi-thoroughfare',
        address_line2: 'commerce-stripe-pi-premise',
        address_city: 'commerce-stripe-pi-locality',
        address_state: 'commerce-stripe-pi-administrative-area',
        address_zip: 'commerce-stripe-pi-postal-code',
        address_country: 'commerce-stripe-pi-country',
        name: 'commerce-stripe-pi-name'
      };

      for (var stripeName in optionalFieldMap) {
        if (optionalFieldMap.hasOwnProperty(stripeName)) {
          var formInputElement = $('.' + optionalFieldMap[stripeName]);
          if (formInputElement.length) {
            data[stripeName] = formInputElement.val();
          }
          else if (typeof Drupal.settings.commerce_stripe_pi_address !== 'undefined') {
            // Load the values from settings if the billing address isn't on
            // the same checkout pane as the address form.
            if (Drupal.settings.commerce_stripe_pi_address[stripeName]) {
              data[stripeName] = Drupal.settings.commerce_stripe_pi_address[stripeName];
            }
          }
        }
      }
      return data;
    },
    /**
     * Define stripe creation type to call: source or token.
     */
    creationType: 'payment_intent'
  };
})(jQuery);
