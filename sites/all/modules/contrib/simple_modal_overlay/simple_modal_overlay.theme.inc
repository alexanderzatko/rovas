<?php
/**
 * @file
 *   Theme hooks for the "Simple Modal Overlay" module.
 *
 *   © 2014-2016 RedBottle Design, LLC, Inveniem, and House at Work.
 *   All rights reserved.
 *
 * @author Guy Paddock (guy@redbottledesign.com)
 */

/**
 * Default theme implementation for simple modal overlays.
 *
 * @param array $variables
 *   An associative array containing:
 *     - name: The unique name for this overlay. This name appears in both the
 *       <code>Drupal.settings.simpleModalOverlay</code> namespace as well as
 *       in the <code>data-modal-name</code> attribute on the modal to make it
 *       easier to apply customizations to the overlay in custom JavaScript or
 *       theme hooks. If not provided, it is automatically populated as
 *       "modal", "modal-1", etc.
 *     - title: The title text that should appear at the top of the overlay.
 *     - content: The content of the overlay. This can be either HTML markup or
 *       a render array.
 *     - open_immediately: Whether or not the modal opens immediately upon the
 *       page loading. The default is TRUE.
 *     - dismissible: Whether or not the user can dismiss the overlay by
 *       clicking a close link in the overlay. The default is TRUE.
 *     - dispose_on_close: Whether or not the message is removed from the HTML
 *       DOM when the modal is closed. The default is TRUE.
 *
 * @return string
 *   The HTML representation of the overlay.
 */
function theme_simple_modal_overlay(array $variables) {
  $title       = $variables['title'];
  $content     = $variables['content'];
  $dismissible = $variables['dismissible'];

  // Provide a reasonable default
  if (empty($variables['name'])) {
    $modal_name = drupal_html_id('modal');
  }
  else {
    $modal_name = $variables['name'];
  }

  drupal_add_js(
    array(
      'simpleModalOverlay' => array(
        $modal_name => array(
          'openImmediately' => $variables['open_immediately'],
          'dismissible'     => $dismissible,
          'disposeOnClose'  => $variables['dispose_on_close'],
        ),
      )
    ),
    'setting');

  if (is_array($content)) {
    $content = drupal_render($content);
  }

  $output = "<div data-modal-name=\"{$modal_name}\" class=\"message-inner\">";

  if (!empty($title)) {
    $output .= '<div class="title clearfix">' . $title . '</div>';
  }

  $output .= '<div class="content">' . $content . '</div>';

  if ($dismissible) {
    $close_text = t('Close');

    $output .= '<a class="simple-overlay-close" href="#">';
    $output .= '  <span class="element-invisible">' . $close_text . '</span>';
    $output .= '</a>';
  }

  $output .= '</div>';

  return $output;
}