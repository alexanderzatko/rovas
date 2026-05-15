(function ($, Drupal) {
  'use strict';

  /**
   * Whether payment controls may be used (partner-integrated rewpro skips the gate).
   */
  Drupal.rovasRewproDonationAckAllowed = function () {
    var settings = Drupal.settings || {};
    if (!settings.rovasRewproDonationAck || !settings.rovasRewproDonationAck.required) {
      return true;
    }
    var $ack = $('#edit-rewpro-donation-ack');
    return $ack.length > 0 && $ack.is(':checked');
  };

  Drupal.behaviors.rovasRewproDonationAck = {
    attach: function (context) {
      var settings = Drupal.settings || {};
      if (!settings.rovasRewproDonationAck || !settings.rovasRewproDonationAck.required) {
        return;
      }

      var $ack = $('#edit-rewpro-donation-ack', context).once('rewpro-donation-ack');
      if (!$ack.length) {
        return;
      }

      var $form = $('#rovas-project-reward-form');

      function setPaymentsEnabled(enabled) {
        $form.toggleClass('rewpro-payments-blocked', !enabled);

        $('#pay-choice-card, #pay-choice-bank').prop('disabled', !enabled);

        $('#payment-wrapper input[name="payment"]').prop('disabled', !enabled);
        $('input[name="payment-currency"]').prop('disabled', !enabled);

        if (!enabled) {
          $('#express-checkout-container').hide();
        }
        $(document).trigger('rovasRewproDonationAckChanged', [enabled]);
      }

      function update() {
        setPaymentsEnabled(Drupal.rovasRewproDonationAckAllowed());
      }

      $ack.on('change', update);
      update();
    }
  };

})(jQuery, Drupal);
