/**
 * @file
 * JavaScript integration between Highcharts and Drupal.
 */
(function ($) {

  Drupal.behaviors.chartsHighcharts = {
    attach: function (context, settings) {
      $('.charts-highchart', context).once('charts-highchart', function() {
        if ($(this).attr('data-chart')) {
          var config = JSON.parse($(this).attr('data-chart'));
          $(this).highcharts(config);
        }
      });
    },
    detach: function (context, settings, trigger) {
      if (trigger === 'unload') {
        var $highcharts_in_context = $('.charts-highchart', context).highcharts();
        if ($highcharts_in_context) {
          $highcharts_in_context.destroy();
        }
      }
    }
  };

})(jQuery);
