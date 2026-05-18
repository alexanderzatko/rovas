/**
 * @file
 * Ensures Chrons/euro timeshare rate fields trigger Drupal AJAX on change.
 *
 * Field groups and #states can leave inputs without the ajax-processed class
 * even when Drupal.settings.ajax contains their callbacks.
 */
(function ($) {
  'use strict';

  function fireTimeshareAjax(input) {
    var id = input.id;
    if (!id || typeof Drupal === 'undefined' || typeof Drupal.ajax === 'undefined') {
      return;
    }
    if (!Drupal.settings.ajax || !Drupal.settings.ajax[id]) {
      return;
    }
    if (!Drupal.ajax[id]) {
      Drupal.ajax[id] = new Drupal.ajax(id, input, Drupal.settings.ajax[id]);
    }
    Drupal.ajax[id].eventResponse(input, $.Event('change'));
  }

  function bindTimeshareInput(input) {
    var $input = $(input);
    if ($input.data('rpeTimeshareBound')) {
      return;
    }
    $input.data('rpeTimeshareBound', 1);
    $input.on('change.rpeTimeshare', function () {
      if (!$input.hasClass('ajax-processed')) {
        fireTimeshareAjax(input);
      }
    });
  }

  Drupal.behaviors.rovasProjectEditTimeshareRates = {
    attach: function (context) {
      var selectors = [
        'input.timeshare-rate-chr',
        'input.timeshare-rate-eur',
        'input[name*="[field_rate]"][name$="[value]"]',
        'input[name*="[field_time_share_cost_euros]"][name$="[value]"]'
      ];
      $(selectors.join(','), context).each(function () {
        bindTimeshareInput(this);
      });
    }
  };
})(jQuery);
