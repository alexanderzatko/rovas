<?php
/**
 * @files
 * Documentation for hooks defined in term_reference_tree project
 */

/**
 * Hook to allow the form elements of a tree to be modified by other code.
 *
 * @param array $element
 *   The checkbox/radio button element for a single term.
 * @param array $context
 *   An array with the following keys:
 *     'element' => Array of the main checkbox_tree element.
 *     'term' => A taxonomy term object.  $term->children should be an array of the term
 *               objects that are that term's children.
 *     'form_state' => The form state array.
 */
function hook_term_reference_tree_element_alter( &$element, $context ) {
  global $user; // Used by example below.

  // Some examples of info that can be found in the context
  $form_id = $context['form_state']['build_info']['form_id'];
  $vocab_obj = $context['element']['#vocabulary'];
  $field_name = $context['element']['#field_name'];
  $term = $context['term'];

  // Example of disabling some of the terms in the tree.
  if ( $field_name = "field_my_tags" ) {
    // Call some custom code to get user specific tags to disable.
    // Returns an array of tids.
    $disabled = my_module_get_disabled_tags( $user );
    if ( in_array($term->tid, $disabled ) ) {
      $element['#disabled'] = TRUE;
    }
  }
}

