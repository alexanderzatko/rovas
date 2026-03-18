<?php

/**
 * Implements hook_field_widget_info().
 */
function term_reference_tree_field_widget_info() {
  return array(
    'term_reference_tree' => array(
      'label' => t('Term reference tree'),
      'field types' => array('taxonomy_term_reference'),
      'behaviors' => array(
        'multiple values' => FIELD_BEHAVIOR_CUSTOM,
        'default value' => FIELD_BEHAVIOR_DEFAULT,
      ),
      'settings' => array(
        'start_minimized' => 0,
        'leaves_only' => 0,
        'filter_view' => '',
        'select_parents' => 0,
        'cascading_selection' => 0,
        'track_list' => 0,
        'token_display' => '',
        'parent_term_id' => '',
        'max_depth' => '',
        'search' => 0,
        'use_ajax' => 0,
      ),
    ),
  );
}

/**
 * Implements hook_field_widget_settings_form().
 */
function term_reference_tree_field_widget_settings_form($field, $instance) {
  $widget = $instance['widget'];
  $settings = $widget['settings'];
  $form = array();

  if ($widget['type'] == 'term_reference_tree') {
    $form['start_minimized'] = array(
      '#type' => 'checkbox',
      '#title' => t('Start minimized'),
      '#description' => t('Make the tree appear minimized on the form by default'),
      '#default_value' => $settings['start_minimized'],
      '#return_value' => 1,
    );

    $form['leaves_only'] = array(
      '#type' => 'checkbox',
      '#title' => t('Leaves only'),
      '#description' => t("Don't allow the user to select items that have children"),
      '#default_value' => $settings['leaves_only'],
      '#return_value' => 1,
    );

    $form['select_parents'] = array(
      '#type' => 'checkbox',
      '#title' => t('Select parents automatically'),
      '#description' => t("When turned on, this option causes the widget to automatically select the ancestors of all selected items. In Leaves Only mode, the parents will be added invisibly to the selected value.  <em>This option is only valid if an unlimited number of values can be selected.</em>"),
      '#default_value' => $settings['select_parents'],
      '#element_validate' => array('_term_reference_tree_select_parents_validate'),
      '#return_value' => 1,
    );

    $form['use_ajax'] = array(
      '#type' => 'checkbox',
      '#title' => t('Load children with ajax'),
      '#description' => t('This option is recommended for large taxonomy lists for better performance.'),
      '#default_value' => isset($settings['use_ajax']) ? $settings['use_ajax'] : 0,
      '#return_value' => 1,
    );

    $form['cascading_selection'] = array(
      '#type' => 'checkbox',
      '#title' => t('Cascading selection'),
      '#description' => t('On parent selection, automatically select children if none were selected. Some may then be manually unselected. In the same way, on parent unselection, unselect children if all were selected. <em>This option is only valid if an unlimited number of values can be selected.</em>'),
      '#default_value' => $settings['cascading_selection'],
      '#element_validate' => array('_term_reference_tree_cascading_selection_validate'),
      '#return_value' => 1,
    );

    if (module_exists('views')) {
      $views = views_get_all_views();
      $options = array('' => 'none');

      foreach ($views as $name => $view) {
        if ($view->base_table == 'taxonomy_term_data') {
          foreach ($view->display as $display) {
            $options["$name:{$display->id}"] = "{$view->human_name}: {$display->display_title}";
          }
        }
      }

      $form['filter_view'] = array(
        '#type' => 'select',
        '#title' => t('Filter by view'),
        '#description' => t('Filter the available options based on whether they appear in the selected view.'),
        '#default_value' => $settings['filter_view'],
        '#options' => $options,
      );
    }
    else {
      $form['filter_view'] = array(
        '#type' => 'hidden',
        '#value' => $settings['filter_view'],
      );
    }

    if (module_exists('token')) {
      $form['token_display'] = array(
        '#type' => 'textarea',
        '#title' => t('Custom Term Label'),
        '#description' => t('Use tokens to change the term labels for the checkboxes and/or radio buttons. Leave this field blank to use the term name.'),
        '#default_value' => $settings['token_display'],
      );

      $form['tokens_list'] = array(
        '#theme' => 'token_tree',
        '#token_types' => array('term'),
        '#dialog' => TRUE,
      );
    }
    else {
      $form['token_display'] = array(
        '#type' => 'hidden',
        '#value' => $settings['token_display'],
      );
    }

    $form['track_list'] = array(
      '#type' => 'checkbox',
      '#title' => t('Track list'),
      '#description' => t('Track what the user has chosen in a list below the tree. Useful when the tree is large, with many levels.'),
      '#default_value' => $settings['track_list'],
      '#return_value' => 1,
    );

    $form['max_depth'] = array(
      '#type' => 'textfield',
      '#title' => t('Maximum Depth'),
      '#description' => t('Only show items up to this many levels deep.'),
      '#default_value' => $settings['max_depth'],
      '#size' => 2,
      '#return_value' => 1,
    );

    $form['parent_term_id'] = array(
      '#type' => 'textfield',
      '#title' => t('Parent Term ID'),
      '#description' => t('Only show items underneath the taxonomy term with this ID number. Leave this field blank to not limit terms by parent.'),
      '#default_value' => $settings['parent_term_id'],
      '#size' => 8,
      '#return_value' => 1,
    );

    $form['search'] = array(
      '#type' => 'checkbox',
      '#title' => t('Display a search field to filter results'),
      '#default_value' => $settings['search'],
    );
  }

  return $form;
}

/**
 * Themes the term tree display (as opposed to the select widget).
 */
function theme_term_tree_list($variables) {
  $element = &$variables['element'];
  $data = &$element['#data'];

  $tree = array();

  // For each selected term.
  foreach ($data as $item) {
    // Loop if the term ID is not zero.
    $values = array();
    $tid = $item['tid'];
    $original_tid = $tid;
    while ($tid != 0) {
      // Unshift the term onto an array.
      array_unshift($values, $tid);

      // Repeat with parent term.
      $tid = _term_reference_tree_get_parent($tid);
    }

    $current = &$tree;
    // For each term in the above array.
    foreach ($values as $tid) {
      if (!isset($current['children'][$tid])) {
        $current['children'][$tid] = array('selected' => FALSE);
      }

      // If this is the last value in the array,
      // tree[children][term_id][selected] = TRUE.
      if ($tid == $original_tid) {
        $current['children'][$tid]['selected'] = TRUE;
      }

      $current['children'][$tid]['tid'] = $tid;
      $current = &$current['children'][$tid];
    }
  }

  $output = '<div class="term-tree-list">';
  $output .= _term_reference_tree_output_list_level($element, $tree);
  $output .= '</div>';
  return $output;
}

/**
 * Helper function to output a single level of the term reference tree display.
 */
function _term_reference_tree_output_list_level(&$element, &$tree) {
  if (isset($tree['children']) && is_array($tree['children']) && count($tree['children']) > 0) {
    $output = '<ul class="term">';
    $settings = $element['#display']['settings'];
    $tokens_selected = $settings['token_display_selected'];
    $tokens_unselected = ($settings['token_display_unselected'] != '') ? $settings['token_display_unselected'] : $tokens_selected;

    $taxonomy_term_info = entity_get_info('taxonomy_term');
    foreach ($tree['children'] as $itemno => &$item) {
      $term = $taxonomy_term_info['load hook']($item['tid']);
      // Do not output terms that do not exist.
      if ($term) {
        $uri = $taxonomy_term_info['uri callback']($term);
        $uri['options']['html'] = TRUE;
        $class = $item['selected'] ? 'selected' : 'unselected';
        $output_fragment[$term->weight][$itemno] = '<li class="' . $class . '">';
        if ($tokens_selected != '' && module_exists('token')) {
          $replace = $item['selected'] ? $tokens_selected : $tokens_unselected;
          $output_fragment[$term->weight][$itemno] .= token_replace($replace, array('term' => $term), array('clear' => TRUE));
        }
        else {
          $output_fragment[$term->weight][$itemno] .= l(filter_xss(entity_label('taxonomy_term', $term)), $uri['path'], $uri['options']);
        }
        if (isset($item['children'])) {
          $output_fragment[$term->weight][$itemno] .= _term_reference_tree_output_list_level($element, $item);
        }
        $output_fragment[$term->weight][$itemno] .= '</li>';
      }
    }

    // Using output fragments with weights as the array key
    // should force this into current sort order once we sort the array.
    if (isset($output_fragment)) {
      ksort($output_fragment);
      foreach ($output_fragment as $this_fragment) {
        $output .= implode('', $this_fragment);
      }
    }

    $output .= '</ul>';
    return $output;
  }
}

/**
 * Makes sure that cardinality is unlimited if auto-select parents is enabled.
 */
function _term_reference_tree_select_parents_validate($element, &$form_state) {
  if ($form_state['values']['instance']['widget']['settings']['select_parents'] == 1 && $form_state['values']['field']['cardinality'] != -1) {
    // This is pretty wonky syntax for the field name in form_set_error,
    // but it's correct.
    form_set_error('field][cardinality', t('You must select an Unlimited number of values if Select Parents Automatically is enabled.'));
  }
}

/**
 * Makes sure that cardinality is unlimited if cascading selection is enabled.
 */
function _term_reference_tree_cascading_selection_validate($element, &$form_state) {
  if ($form_state['values']['instance']['widget']['settings']['cascading_selection'] == 1) {
    if ($form_state['values']['field']['cardinality'] != -1) {
      // This is pretty wonky syntax for the field name in form_set_error,
      // but it's correct.
      form_set_error('field][cardinality', t('You must select an Unlimited number of values if Cascading selection is enabled.'));
    }
    if ($form_state['values']['instance']['widget']['settings']['use_ajax'] == 1) {
      form_set_error('instance[widget][settings][use_ajax', t('You must deselect the use of ajax for loading children if you need cascading selection.'));
    }
  }
}

/**
 * Process the checkbox_tree widget.
 *
 * This function processes the checkbox_tree widget.
 *
 * @param $element
 *   The element to be drawn.$element['#field_name']
 * @param $form_state
 *   The form state.
 *
 * @return
 *   The processed element.
 */
function term_reference_tree_process_checkbox_tree($element, $form_state) {
  if (is_array($form_state)) {
    $allowed = !empty($element['#filter_view']) ? _term_reference_tree_get_allowed_values($element['#filter_view']) : '';
    $default = !empty($element['#default_value']) && is_array($element['#default_value']) ? $element['#default_value'] : array();
    $opened = !empty($form_state['children']) ? $form_state['children'] : array();
    $max_depth = !empty($element['#max_depth']) ? $element['#max_depth'] : NULL;

    // Build the terms hierarchy.
    $default_parents = _term_reference_tree_taxonomy_get_parents_all($element['#vocabulary']->vid, $default);
    $element['#expanded'] = $opened + $default_parents;
    $element['#options_tree'] = _term_reference_tree_get_term_hierarchy($element['#parent_tid'], $element['#vocabulary']->vid, $allowed, $element['#expanded'], $element['#use_ajax'], $max_depth);

    // Flatten the hierarchy.
    $terms_flat = _term_reference_tree_hierarchy_flatten($element['#options_tree']);

    // Give other modules a chance to alter the terms.
    drupal_alter('term_reference_tree_process_terms', $terms_flat, $element);

    // Resolve labels.
    _term_reference_tree_taxonomy_resolve_labels($terms_flat, $element['#token_display']);

    $max_choices = !empty($element['#max_choices']) ? $element['#max_choices'] : 1;
    if ($max_choices != '-1') {
      drupal_add_js(array('term_reference_tree' => array('trees' => array($element['#id'] => array('max_choices' => $element['#max_choices'])))), 'setting');
    }

    if ($max_choices == 1 && !$element['#required']) {
      array_unshift($element['#options_tree'], (object) array(
        'tid' => '',
        'name' => t('N/A'),
        'depth' => 0,
        'vocabulary_machine_name' => $element['#vocabulary']->machine_name,
        'has_children' => FALSE,
      ));
    }

    if ($max_choices != 1) {
      $element['#tree'] = TRUE;
    }

    if (!empty($element['#select_parents'])) {
      $element['#attributes']['class'][] = 'select-parents';
    }

    $tree = new stdClass();
    $tree->children = $element['#options_tree'];
    $tree->has_children = TRUE;
    $element[] = _term_reference_tree_build_level($element, $tree, $form_state, $default, $max_choices, array(), 1);

    // Add a track list element?
    if (!empty($element['#track_list'])) {
      $element[] = array(
        '#type' => 'checkbox_tree_track_list',
        '#max_choices' => $max_choices,
      );
    }
  }

  return $element;
}

/**
 * Returns HTML for a checkbox_tree form element.
 *
 * @param $variables
 *   An associative array containing:
 *   - element: An associative array containing the properties of the element.
 *
 * @ingroup themeable
 */
function theme_checkbox_tree($variables) {
  $element = $variables['element'];
  $element['#children'] = drupal_render_children($element);

  $attributes = array();
  if (isset($element['#id'])) {
    $attributes['id'] = $element['#id'];
  }
  $attributes['class'][] = 'term-reference-tree';

  if (form_get_error($element)) {
    $attributes['class'][] = 'error';
  }

  if (!empty($element['#required'])) {
    $attributes['class'][] = 'required';
  }

  if (array_key_exists('#start_minimized', $element) && $element['#start_minimized']) {
    $attributes['class'][] = 'term-reference-tree-collapsed';
  }

  if (array_key_exists('#cascading_selection', $element) && $element['#cascading_selection']) {
    $attributes['class'][] = 'term-reference-tree-cascading-selection';
  }

  if (array_key_exists('#track_list', $element) && $element['#track_list']) {
    $attributes['class'][] = 'term-reference-tree-track-list-shown';
  }

  if (!empty($element['#attributes']['class'])) {
    $attributes['class'] = array_merge($attributes['class'], $element['#attributes']['class']);
  }
  return
    '<div' . drupal_attributes($attributes) . '>'
    . (!empty($element['#children']) ? $element['#children'] : '')
    . '</div>';
}


/**
 * This function prints a list item with a checkbox and an unordered list
 * of all the elements inside it.
 */
function theme_checkbox_tree_level($variables) {
  $element = $variables['element'];
  $sm = '';
  if (array_key_exists('#level_start_minimized', $element) && $element['#level_start_minimized']) {
    $sm = ' style="display: none;"';
  }

  $output = '<ul class="term-reference-tree-level "' . $sm . '>';
  $children = element_children($element);
  foreach ($children as $child) {
    $output .= '<li>';
    $output .= drupal_render($element[$child]);
    $output .= '</li>';
  }

  $output .= '</ul>';

  return $output;
}

/**
 * This function prints a single item in the tree, followed by that item's
 * children
 * (which may be another checkbox_tree_level).
 */
function theme_checkbox_tree_item($variables) {
  $element = $variables['element'];
  $children = element_children($element);

  $output = '';
  if ($element['#has_children']) {
    if ($element['#level_start_minimized']) {
      $id = drupal_html_id('term-reference-tree-' . $children[0]);
      $output .= '<div id="' . $id . '" data-tid="' . $children[0] . '" class="term-reference-tree-button term-reference-tree-collapsed"></div>';
    }
    else {
      $output .= '<div class="term-reference-tree-button"></div>';
    }
  }
  elseif (!$element['#leaves_only']) {
    $output .= '<div class="no-term-reference-tree-button"></div>';
  }

  foreach ($children as $child) {
    $output .= drupal_render($element[$child]);
  }

  return $output;
}

/**
 * This function prints a label that cannot be selected.
 */
function theme_checkbox_tree_label($variables) {
  $element = $variables['element'];
  $output = '<div class="parent-term">' . $element['#value'] . '</div>';
  return $output;
}

/**
 * Shows a list of items that have been checked.
 * The display happens on the client-side.
 * Use this function to theme the element's label,
 * and the "nothing selected" message.
 *
 * @param $variables
 *   Variables available for theming.
 */
function theme_checkbox_tree_track_list($variables) {
  // Should the label be singular or plural?
  // Depends on cardinality of term field.
  static $nothingselected;

  if (!$nothingselected) {
    $nothingselected = t('[Nothing selected]');
    // Add the "Nothing selected" text. To style it,
    // replace it with whatever you want. Could do this with a file instead.
    drupal_add_js(
      'var termReferenceTreeNothingSelectedText = "' . $nothingselected . '";',
      'inline'
    );
  }

  $label = format_plural(
    $variables['element']['#max_choices'],
    t('Selected item (click the item to uncheck it)'),
    t('Selected items (click an item to uncheck it)')
  );
  $output = '<div class="term-reference-track-list-container">';
  $output .= '<div class="term-reference-track-list-label">' . $label . '</div>';
  $output .= '<ul class="term-reference-tree-track-list"><li class="term_ref_tree_nothing_message">' . $nothingselected . '</li></ul>';
  $output .= '</div>';

  return $output;
}

/**
 * Implements hook_field_widget_form().
 */
function term_reference_tree_field_widget_form(&$form, &$form_state, $field, $instance, $langcode, $items, $delta, $element) {
  $settings = $instance['widget']['settings'];
  $vocabulary = taxonomy_vocabulary_machine_name_load($field['settings']['allowed_values'][0]['vocabulary']);
  $path = drupal_get_path('module', 'term_reference_tree');
  $value_key = key($field['columns']);

  $default_value = array();
  foreach ($items as $item) {
    $key = $item[$value_key];
    if ($key === 0) {
      $default_value[$key] = '0';
    }
    else {
      $default_value[$key] = $key;
    }
  }

  $multiple = $field['cardinality'] > 1 || $field['cardinality'] == FIELD_CARDINALITY_UNLIMITED;
  $properties = array();

  if (!array_key_exists('#value', $element)) {
    $element['#value'] = array();
  }

  // A switch statement, in case we ever add more widgets to this module.
  switch ($instance['widget']['type']) {
    case 'term_reference_tree':
      $element['#attached']['js'] = array($path . '/term_reference_tree.js');
      $element['#attached']['css'] = array($path . '/term_reference_tree.css');
      $element['#type'] = 'checkbox_tree';
      $element['#default_value'] = $multiple ? $default_value : array(reset($default_value) => reset($default_value));
      $element['#max_choices'] = $field['cardinality'];
      $element['#max_depth'] = $settings['max_depth'];
      $element['#start_minimized'] = $settings['start_minimized'];
      $element['#leaves_only'] = $settings['leaves_only'];
      $element['#filter_view'] = module_exists('views') ? $settings['filter_view'] : '';
      $element['#select_parents'] = $settings['select_parents'];
      $element['#cascading_selection'] = $settings['cascading_selection'];
      $element['#use_ajax'] = isset($settings['use_ajax']) ? (bool) $settings['use_ajax'] : FALSE;
      $element['#track_list'] = $settings['track_list'];
      $element['#parent_tid'] = $settings['parent_term_id'] ? $settings['parent_term_id'] : $field['settings']['allowed_values'][0]['parent'];
      $element['#vocabulary'] = $vocabulary;
      $element['#token_display'] = module_exists('token') ? $settings['token_display'] : '';
      break;
  }

  if (isset($form_state['triggering_element']['#term'])) {
    $term = $form_state['triggering_element']['#term'];
    $form_state['children'][$term->tid] = $term->tid;
  }

  $element += array(
    '#value_key' => $value_key,
    '#element_validate' => array('_term_reference_tree_widget_validate'),
    '#properties' => $properties,
  );

  $element['filter'] = array(
    '#type' => module_exists('elements') ? 'searchfield' : 'textfield',
    '#title' => t('Filter terms'),
    '#size' => 30,
    '#weight' => -1,
    '#attributes' => array('class' => array('term-reference-tree-filter')),
    '#access' => $settings['search'],
  );

  return $element;
}


/**
 * Validates the term reference tree widgets.
 *
 * This function sets the value of the tree widgets into a form that Drupal
 * can understand, and also checks if the field is required and has been
 * left empty.
 *
 * @param $element
 *   The element to be validated.
 * @param $form_state
 *   The state of the form.
 *
 * @return
 *   The validated element.
 */
function _term_reference_tree_widget_validate(&$element, &$form_state) {
  $items = _term_reference_tree_flatten($element, $form_state);
  $value = array();

  if ($element['#max_choices'] != 1) {
    foreach ($items as $child) {
      if (!empty($child['#value'])) {
        array_push($value, array($element['#value_key'] => $child['#value']));

        // If the element is leaves only and select parents is on, then automatically
        // add all the parents of each selected value.
        if ($element['#select_parents'] && $element['#leaves_only']) {
          foreach ($child['#parent_values'] as $parent_tid) {
            if (!in_array(array($element['#value_key'] => $parent_tid), $value)) {
              array_push($value, array($element['#value_key'] => $parent_tid));
            }
          }
        }
      }
    }
  }
  else {
    // If it's a tree of radio buttons, they all have the same value,
    // so we can just grab the value of the first one.
    if (count($items) > 0) {
      $child = reset($items);
      if (!empty($child['#value'])) {
        array_push($value, array($element['#value_key'] => $child['#value']));
      }
    }
  }

  if ($element['#required'] && empty($value)) {
    // The title is already check_plained so it's appropriate to use !.
    form_error($element, t('!name field is required.', array('!name' => $element['#title'])));
  }

  form_set_value($element, $value, $form_state);
  return $element;
}

/**
 * Returns an array of allowed values defined by the given view.
 *
 * @param $filter
 *   A view, in the format VIEWNAME:DISPLAYNAME
 *
 * @return
 *   An array of term IDs (tid => true) returned by the view.
 */
function _term_reference_tree_get_allowed_values($filter) {
  $viewname = '';
  $displayname = '';
  $allowed = array();

  if (module_exists('views') && $filter != '') {
    list($viewname, $displayname) = explode(':', $filter);
    $view = views_get_view($viewname);
    if (is_object($view)) {
      if ($view->access($displayname)) {
        // Save the page title first, since execute_display() will reset this
        // to the display title.
        $title = drupal_get_title();
        $view->execute_display($displayname);
        $title = drupal_set_title($title, PASS_THROUGH);
        foreach ($view->result as $item) {
          $allowed[$item->tid] = TRUE;
        }
      }
      else {
        drupal_set_message(t('Cannot access view for term reference tree widget.'), 'warning');
      }
    }
    else {
      drupal_set_message(t('Term reference tree: no view named !view_name', array('!view_name' => $viewname)), 'warning');
    }
  }

  return $allowed;
}


/**
 * Builds a single item in the term reference tree widget.
 *
 * This function returns an element with a checkbox for a single taxonomy term.
 * If that term has children, it appends checkbox_tree_level element that
 * contains the children.  It is meant to be called recursively when the widget
 * is built.
 *
 * @param $element
 *   The main checkbox_tree element.
 * @param $term
 *   A taxonomy term object.  $term->children should be an array of the term
 *   objects that are that term's children.
 * @param $form_state
 *   The form state.
 * @param $value
 *   The value of the element.
 * @param $max_choices
 *   The maximum number of allowed selections.
 *
 * @return
 *   A completed checkbox_tree_item element, which contains a checkbox and
 *   possibly a checkbox_tree_level element as well.
 */
function _term_reference_tree_build_item($element, $term, $form_state, $value, $max_choices, $parent_tids, $parent, $depth) {
  $term_name = $term->name;
  if (!empty($element['#token_display']) && module_exists('token') && !empty($term->term_reference_tree_token)) {
    $term_name = check_plain($term->term_reference_tree_token);
  }
  elseif (!empty($term->term_reference_tree_label)) {
    $term_name = check_plain($term->term_reference_tree_label);
  }

  $container = array(
    '#type' => 'checkbox_tree_item',
    '#max_choices' => $max_choices,
    '#leaves_only' => isset($element['#leaves_only']) ? $element['#leaves_only'] : FALSE,
    '#term_name' => $term_name,
    '#level_start_minimized' => FALSE,
    '#depth' => $depth,
    '#has_children' => FALSE,
  );

  if (!$element['#leaves_only'] || empty($term->children)) {
    $e = array(
      '#type' => ($max_choices == 1) ? 'radio' : 'checkbox',
      '#title' => $term_name,
      '#on_value' => $term->tid,
      '#off_value' => 0,
      '#return_value' => $term->tid,
      '#parent_values' => $parent_tids,
      '#default_value' => isset($value[$term->tid]) ? $term->tid : NULL,
      '#attributes' => isset($element['#attributes']) ? $element['#attributes'] : NULL,
      '#ajax' => isset($element['#ajax']) ? $element['#ajax'] : NULL,
    );

    if ($e['#type'] == 'radio') {
      $parents_for_id = array_merge($element['#parents'], array($term->tid));
      $e['#id'] = drupal_html_id('edit-' . implode('-', $parents_for_id));
      $e['#parents'] = $element['#parents'];
    }
    $context = array(
      'element' => $element,
      'term' => $term,
      'form_state' => $form_state,
    );
    drupal_alter('term_reference_tree_element', $e, $context);
  }
  else {
    $e = array(
      '#type' => 'checkbox_tree_label',
      '#value' => $term_name,
    );
  }

  $container[$term->tid] = $e;

  $max_depth_reached = !empty($element['#max_depth']) && $depth >= $element['#max_depth'];
  if ($term->has_children && !$max_depth_reached) {
    $container['#has_children'] = TRUE;
    $parents = $parent_tids;
    $parents[] = $term->tid;
    $container[$term->tid . '-children'] = _term_reference_tree_build_level($element, $term, $form_state, $value, $max_choices, $parents, $depth + 1);
    $container['#level_start_minimized'] = $container[$term->tid . '-children']['#level_start_minimized'];
  }

  return $container;
}

/**
 * Builds a level in the term reference tree widget.
 *
 * This function returns an element that has a number of checkbox_tree_item
 * elements as children.  It is meant to be called recursively when the widget
 * is built.
 *
 * @param $element
 *   The main checkbox_tree element.
 * @param $term
 *   A taxonomy term object.  $term->children should be an array of the term
 *   objects that are that term's children.
 * @param $form_state
 *   The form state.
 * @param $value
 *   The value of the element.
 * @param $max_choices
 *   The maximum number of allowed selections.
 *
 * @return
 *   A completed checkbox_tree_level element.
 */
function _term_reference_tree_build_level($element, $term, $form_state, $value, $max_choices, $parent_tids, $depth) {
  $must_expand = (!$element['#start_minimized'] && !$element['#use_ajax']) || (isset($term->tid) && in_array($term->tid, $element['#expanded']));
  $container = array(
    '#max_choices' => $max_choices,
    '#leaves_only' => isset($element['#leaves_only']) ? $element['#leaves_only'] : FALSE,
    '#start_minimized' => isset($element['#start_minimized']) ? $element['#start_minimized'] : FALSE,
    '#level_start_minimized' => $depth > 1 && !$must_expand,
    '#depth' => $depth,
  );

  // Adds checkbox_tree_level when children exists or ajax wrapper when children
  // are not loaded (has_children = TRUE but children is empty).
  if (!empty($term->children)) {
    $container['#type'] = 'checkbox_tree_level';
    foreach ($term->children as $child) {
      $container[$child->tid] = _term_reference_tree_build_item($element, $child, $form_state, $value, $max_choices, $parent_tids, $container, $depth);
    }
  }
  else {
    $id = drupal_html_id('term-reference-tree-widget-level-ajax-' . $term->tid);
    $container['#type'] = 'container';
    $container['#attributes']['id'] = $id;
    $container['#attributes']['class'][] = 'term-reference-tree-widget-level-ajax';
    $container['load_children_' . $term->tid] = array(
      '#type' => 'button',
      '#attributes' => array('class' => array('term-reference-tree-widget-level-ajax-button')),
      '#value' => 'load children ' . $term->tid,
      '#limit_validation_errors' => array(),
      '#term' => $term,
      '#ajax' => array(
        'wrapper' => $id,
        'callback' => 'term_reference_tree_children_callback',
      ),
    );
  }

  return $container;
}

/**
 * Ajax callback to load children for a list.
 */
function term_reference_tree_children_callback($form, $form_state) {
  $button = $form_state['triggering_element'];
  $element = drupal_array_get_nested_value($form, array_slice($button['#array_parents'], 0, -1));
  $commands[] = array(
    'command' => 'term_reference_tree_append_children',
    'data' => drupal_render($element),
  );
  return array('#type' => 'ajax', '#commands' => $commands);
}
