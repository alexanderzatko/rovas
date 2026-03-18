/**
 * @file
 * JavaScript to switch OpenLayers 3 tile sources based on zoom level.
 */

(function ($, Drupal) {
  'use strict';
  
  Drupal.behaviors.rovasOlZoomSourceSwitcher = {
    attach: function (context, settings) {
      
      if (!settings.rovasOlZoomSourceSwitcher) {
        return;
      }
      
      // Configuration values
      var config = settings.rovasOlZoomSourceSwitcher;

      var lowZoomUrl = config.lowZoomUrl;
      var highZoomUrl = config.highZoomUrl;
      var zoomThreshold = config.zoomThreshold || 15;
      
      // Wait for OpenLayers to be fully loaded
      if (typeof ol === 'undefined') {
        return;
      }
      
      // Function to process maps after they're created
      var processMap = function(map) {
        
        if (!map || typeof map.getLayers !== 'function') {
          return;
        }
        
        // Find XYZ layers
        var xyzLayers = [];
        var allLayers = [];
        
        map.getLayers().forEach(function(layer) {
          allLayers.push(layer);
          if (layer instanceof ol.layer.Tile) {
            var source = layer.getSource();
            
            if (source instanceof ol.source.XYZ && source.getUrls) {
              var urls = source.getUrls();
              if (urls && urls.length > 0) {
                // Check for both opentopomap and other related URLs
                if (urls[0].indexOf('opentopomap.org') !== -1 || 
                    urls[0].indexOf('tile.opentopomap') !== -1) {
                  xyzLayers.push(layer);
                }
              }
            }
          }
        });
        
        if (xyzLayers.length === 0) {
          return;
        }
        
        // For each OpenTopo layer, set up zoom switching
        xyzLayers.forEach(function(layer) {
          var originalSource = layer.getSource();
          
          // Create high zoom source
          var highZoomSource = new ol.source.XYZ({
            url: highZoomUrl,
            attributions: originalSource.getAttributions ? originalSource.getAttributions() : undefined
          });
          
          // Set up view change listener
          map.getView().on('change:resolution', function() {
            var zoomLevel = map.getView().getZoom();
            var currentSource = layer.getSource();
            var currentUrl = '';
            
            if (currentSource.getUrls && currentSource.getUrls()[0]) {
              currentUrl = currentSource.getUrls()[0];
            }
            
            if (zoomLevel > zoomThreshold) {
              layer.setSource(highZoomSource);
            } else if (zoomLevel <= zoomThreshold) {
              layer.setSource(originalSource);
            }
          });
          
          // Initial check
          var initialZoom = map.getView().getZoom();
          if (initialZoom > zoomThreshold) {
            layer.setSource(highZoomSource);
          }
        });
      };
      

	// Add a function to check for maps periodically
	function checkForMaps() {
	  
	  if (Drupal.openlayers && Drupal.openlayers.instances) {
		// Check if there are instances available yet
		var instancesAvailable = false;
		
		// Check all properties of Drupal.openlayers
		for (var prop in Drupal.openlayers) {
		  if (prop === 'instances') {
			// If this has maps, process them
			if (typeof Drupal.openlayers[prop] === 'object') {
			  // Check if the instances property actually has any maps
			  for (var mapId in Drupal.openlayers[prop]) {
				instancesAvailable = true;
				
				if (Drupal.openlayers[prop][mapId] && Drupal.openlayers[prop][mapId].map) {
				  processMap(Drupal.openlayers[prop][mapId].map);
				}
			  }
			}
		  }
		}
		
		if (!instancesAvailable) {
		  setTimeout(checkForMaps, 1000); // Check again in 1 second
		}
	  } else {
		setTimeout(checkForMaps, 1000); // Check again in 1 second
	  }
	}
	
	// Start checking for maps
	setTimeout(checkForMaps, 1000);

    }
  };
})(jQuery, Drupal);