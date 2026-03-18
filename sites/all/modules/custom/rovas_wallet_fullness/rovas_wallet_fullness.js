/**
 * @file
 * JavaScript functionality for the wallet fullness vertical gauge with threshold indicator.
 */

(function ($) {
  Drupal.behaviors.walletFullnessGauge = {
    attach: function (context, settings) {
      $('.wallet-gauge-wrapper.vertical', context).once('wallet-gauge').each(function() {
        var $wrapper = $(this);
        var $fill = $wrapper.find('.wallet-gauge-fill');
        var $gaugeContainer = $wrapper.find('.wallet-gauge-bar-container.vertical');
        var $thresholdLine = $wrapper.find('.wallet-gauge-threshold-line');
        var $allocationLine = $wrapper.find('.wallet-gauge-allocation-line');
        var $indicator = $wrapper.find('.wallet-gauge-indicator');
        var $labels = $wrapper.find('.wallet-gauge-label');
        var $markers = $wrapper.find('.wallet-gauge-marker');
        
        // Get gauge height for calculations
        var gaugeHeight = $gaugeContainer.height();
        
        // Collect all labels with their positions and values
        var labelData = [];
        $labels.each(function() {
          var $label = $(this);
          var bottomPercent = parseFloat($label.css('bottom'));
          var value = parseFloat($label.attr('data-value'));
          var type = '';
          
          if ($label.hasClass('current')) type = 'current';
          else if ($label.hasClass('fixed')) type = 'fixed';
          else if ($label.hasClass('variable')) type = 'variable';
          else if ($label.hasClass('max')) type = 'max';
          
          labelData.push({
            element: $label,
            bottomPercent: bottomPercent,
            value: value,
            type: type,
            originalText: $label.html() // Use html() to preserve any formatting
          });
        });
        
				// Sort labels by their actual values (highest value at top to prevent line crossing)
				labelData.sort(function(a, b) {
					// Sort by value in descending order (highest value first)
					return b.value - a.value;
				});
        
        // Group labels with the same values
        var groupedLabels = [];
        var currentGroup = [];
        var lastValue = null;
        
        for (var i = 0; i < labelData.length; i++) {
          var label = labelData[i];
          if (lastValue !== null && Math.abs(label.value - lastValue) < 0.01) {
            // Same value, add to current group
            currentGroup.push(label);
          } else {
            // Different value, start new group
            if (currentGroup.length > 0) {
              groupedLabels.push(currentGroup);
            }
            currentGroup = [label];
            lastValue = label.value;
          }
        }
        if (currentGroup.length > 0) {
          groupedLabels.push(currentGroup);
        }
        
        // Position labels in stacked formation
        var stackStartTop = 10; // Start position from top
        var labelHeight = 25; // Height per label row
        
        for (var groupIndex = 0; groupIndex < groupedLabels.length; groupIndex++) {
          var group = groupedLabels[groupIndex];
          var stackTop = stackStartTop + (groupIndex * labelHeight);
          
          if (group.length === 1) {
            // Single label - position normally
            var label = group[0];
            label.element.css({
              'position': 'absolute',
              'top': stackTop + 'px',
              'bottom': 'auto'
            });
            
            // Add connecting line
            Drupal.behaviors.walletFullnessGauge.addConnectingLine(label.element, label.bottomPercent, stackTop, gaugeHeight);
          } else {
            // Multiple labels with same value - position side by side with "="
            var totalWidth = 0;
            var labelWidths = [];
            
            // Calculate total width needed
            for (var i = 0; i < group.length; i++) {
              var labelWidth = group[i].element.outerWidth();
              labelWidths.push(labelWidth);
              totalWidth += labelWidth;
              if (i > 0) totalWidth += 20; // Space for "=" between labels
            }
            
            // Position labels side by side
            var currentLeft = 0;
            for (var i = 0; i < group.length; i++) {
              var label = group[i];
              label.element.css({
                'position': 'absolute',
                'top': stackTop + 'px',
                'bottom': 'auto',
                'left': currentLeft + 'px'
              });
              
              currentLeft += labelWidths[i];
              
              // Add "=" separator (except for last label)
              if (i < group.length - 1) {
                var $separator = $('<span class="label-separator">=</span>');
                $separator.css({
                  'position': 'absolute',
                  'top': stackTop + 'px',
                  'left': (currentLeft + 5) + 'px',
                  'color': '#666',
                  'font-weight': 'bold',
                  'line-height': '20px'
                });
                $wrapper.find('.wallet-gauge-labels').append($separator);
                currentLeft += 20;
              }
            }
            
            // Add single connecting line for the group (to leftmost label, not center)
            Drupal.behaviors.walletFullnessGauge.addConnectingLine(group[0].element.parent(), group[0].bottomPercent, stackTop, gaugeHeight, 0);
          }
        }
        
        // Animation setup (keep existing animation logic)
        var originalFillHeight = $fill.css('height');
        var originalIndicatorBottom = $indicator.css('bottom');
        var originalThresholdBottom = $thresholdLine.css('bottom');
        var originalAllocationBottom = $allocationLine.length ? $allocationLine.css('bottom') : null;
        
        // Start animations from bottom
        $fill.css('height', '0');
        $indicator.css('bottom', '0');
        $thresholdLine.css('bottom', '0');
        if ($allocationLine.length) {
          $allocationLine.css('bottom', '0');
        }
        
        // Hide labels and markers initially
        $labels.css('opacity', '0');
        $markers.css('opacity', '0');
        $('.connecting-line, .label-separator').css('opacity', '0');
        
        // Animate fill
        $fill.animate({ height: originalFillHeight }, 1000);
        
        // Animate indicators
        $indicator.animate({ bottom: originalIndicatorBottom }, 1000);
        $thresholdLine.animate({ bottom: originalThresholdBottom }, 800);
        
        if ($allocationLine.length && originalAllocationBottom) {
          $allocationLine.animate({ bottom: originalAllocationBottom }, 700);
        }
        
        // Fade in labels, markers, and connecting elements
        setTimeout(function() {
          $labels.animate({ opacity: 1 }, 400);
          $markers.animate({ opacity: 1 }, 400);
          $('.connecting-line, .label-separator').animate({ opacity: 1 }, 600);
        }, 600);
      });
    },
    
    // Helper function to add diagonal connecting lines
    addConnectingLine: function(parentElement, gaugeBottomPercent, stackTop, gaugeHeight, offsetLeft) {
      offsetLeft = offsetLeft || 0;
      
      // Calculate the position on the gauge (from bottom of gauge container)
      var gaugePositionFromBottom = gaugeHeight * (gaugeBottomPercent / 100);
      var gaugeY = gaugeHeight - gaugePositionFromBottom;
      
      // Create SVG line
      var svgNS = "http://www.w3.org/2000/svg";
      var svg = document.createElementNS(svgNS, "svg");
      var line = document.createElementNS(svgNS, "line");
      
      // Line coordinates: from gauge marker to stacked label
      var x1 = 35; // Start from right edge of gauge area (30px gauge + 5px margin)
      var y1 = gaugeY;
      var x2 = 50 + offsetLeft; // End at label position (closer to gauge)
      var y2 = stackTop + 12; // Center of label (adjusted for label height)
      
      line.setAttribute("x1", x1);
      line.setAttribute("y1", y1);
      line.setAttribute("x2", x2);
      line.setAttribute("y2", y2);
      line.setAttribute("stroke", "#999");
      line.setAttribute("stroke-width", "1");
      line.setAttribute("stroke-dasharray", "3,2");
      line.setAttribute("opacity", "0.7");
      
      svg.appendChild(line);
      svg.setAttribute("class", "connecting-line");
      svg.style.position = "absolute";
      svg.style.top = "0";
      svg.style.left = "0";
      svg.style.width = "250px";
      svg.style.height = gaugeHeight + "px";
      svg.style.pointerEvents = "none";
      svg.style.zIndex = "5";
      
      // Add to the wrapper, not to individual label
      var $wrapper = $(parentElement).closest('.wallet-gauge-wrapper.vertical');
      $wrapper.append(svg);
    }
  };
})(jQuery);