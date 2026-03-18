/**
 * @file
 *   Behaviors for the Simple Modal Overlay module.
 *
 *   © 2014-2016 RedBottle Design, LLC, Inveniem, and House at Work.
 *   All rights reserved.
 *
 * @author Guy Paddock (guy@redbottledesign.com)
 */
(function ($) {
  Drupal.behaviors.simpleModalOverlay = {
    /**
     * Implements Drupal's standard attach() method for behaviors.
     */
    attach: function (context, settings) {
      var _this = this;

      $('.simple-overlay').each(function (index, element) {
        _this._setupOverlay(element);
      });
    },

    /**
     * Displays the modal with the specified name.
     *
     * The modal must already be present in the HTML DOM, and settings for it
     * must already be defined in Drupal.settings.simpleModalOverlay.
     *
     * @param modalName
     *   The name of the modal to show.
     */
    showModal: function (modalName) {
      var overlaySettings = this._getModalSettings(modalName),
          overlayElement  = this._getModalElement(modalName);

      (function (_this, _modalName, _overlayElement, _overlaySettings) {
        // Fade in the background first
        _overlayElement.fadeIn(300, function() {
          // Then fade in the actual modal
          _overlayElement.find('.message-inner').fadeIn(300, function () {
            if (_overlaySettings.dismissible) {
              $(document).on('keyup.' + _modalName, function(e) {
                if (e.keyCode == jQuery.ui.keyCode.ESCAPE) {
                  _this.dismissModal(_modalName);
                }
              });
            }
          });
        });
      })(this, modalName, overlayElement, overlaySettings);
    },

    /**
     * Dismisses / hides the modal with the specified name.
     *
     * The modal must already be present in the HTML DOM, and settings for it
     * must already be defined in Drupal.settings.simpleModalOverlay.
     *
     * @param modalName
     *   The name of the modal to show.
     */
    dismissModal: function (modalName) {
      var overlaySettings = this._getModalSettings(modalName),
          overlayElement  = this._getModalElement(modalName);

      (function (_modalName, _overlayElement, _overlaySettings) {
        // If disposeOnClose is set, when the overlay is closed, remove it and
        // the background behind it.
        _overlayElement.find('.message-inner').fadeOut(300, function() {
          if (_overlaySettings.disposeOnClose) {
            $(this).remove();
          }

          // Fade out the background last
          _overlayElement.fadeOut(300, function() {
            if (_overlaySettings.disposeOnClose) {
              $(this).remove();
            }

            if (_overlaySettings.dismissible) {
              $(document).unbind('keyup.' + _modalName);
            }
          });
        });
      })(modalName, overlayElement, overlaySettings);
    },

    /**
     * Sets up the specified overlay element with appropriate callbacks for
     * being dismissed or opened immediately upon page load.
     *
     * @private
     * @param overlayElement
     *   The element that is being configured. This is the outer
     *   '.simple-overlay' element.
     */
    _setupOverlay: function (overlayElement) {
      (function (_this, _overlayElement) {
        var innerElement    = $(overlayElement).find('.message-inner')[0],
            modalName       = $(innerElement).attr('data-modal-name'),
            overlaySettings = _this._getModalSettings(modalName);

        // Enable the close link.
        $(_overlayElement).find('.simple-overlay-close').click(function(e) {
          e.preventDefault();
          _this.dismissModal(modalName);
        });

        if (overlaySettings.openImmediately) {
          _this.showModal(modalName);
        }
      })(this, overlayElement);
    },

    /**
     * Gets settings for the modal with the specified name.
     *
     * @private
     *
     * @param modalName
     *   The name of the modal.
     *
     * @returns {*}
     *   An object containing the settings for the modal.
     */
    _getModalSettings: function (modalName) {
      var defaults = {
        openImmediately: true,
        dismissible:     true,
        disposeOnClose:  true
      };

      if ((Drupal.settings.simpleModalOverlay != undefined) &&
          Drupal.settings.simpleModalOverlay.hasOwnProperty(modalName)) {
        settings = Drupal.settings.simpleModalOverlay[modalName];
      }
      else {
        settings = {};
      }

      return $.extend({}, defaults, settings);
    },

    /**
     * Gets the element that contains the modal with the specified name.
     *
     * @private
     *
     * @param modalName
     *   The name of the modal.
     *
     * @returns {XML|*}
     *   The outer '.simple-overlay' element for the specified modal.
     */
    _getModalElement: function (modalName) {
      var innerSelector,
          innerElement,
          outerElement;

      innerSelector
        = '.simple-overlay .message-inner[data-modal-name="' + modalName + '"]';

      innerElement = $(innerSelector);
      outerElement = innerElement.parent('.simple-overlay');

      return outerElement;
    }
  };

  // jQuery UI shim
  $.extend(true, jQuery, { ui: { keyCode: { ESCAPE: 27 } } });
})(jQuery);
