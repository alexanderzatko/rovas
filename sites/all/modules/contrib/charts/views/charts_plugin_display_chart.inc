<?php
/**
 * @file
 * Contains the Chart display type (similar to Page, Block, Attachment, etc.)
 */

/**
 * Display plugin to attach multiple chart configurations to the same chart.
 *
 * @ingroup views_style_plugins
 */
class charts_plugin_display_chart extends views_plugin_display {
  function get_style_type() {
    return 'chart';
  }

  function option_definition() {
    $options = parent::option_definition();

    // Overrides of standard options.
    $options['style_plugin']['default'] = 'chart_extension';
    $options['row_plugin']['default'] = 'fields';
    $options['defaults']['default']['style_plugin'] = FALSE;
    $options['defaults']['default']['style_options'] = FALSE;
    $options['defaults']['default']['row_plugin'] = FALSE;
    $options['defaults']['default']['row_options'] = FALSE;

    $options['parent_display'] = array('default' => '');
    $options['inherit_yaxis'] = array('default' => '1');

    $options['inherit_arguments'] = array('default' => TRUE, 'bool' => TRUE);
    $options['inherit_exposed_filters'] = array('default' => TRUE, 'bool' => TRUE);

    return $options;
  }

  /**
   * Provide the summary for page options in the views UI.
   *
   * This output is returned as an array.
   */
  function options_summary(&$categories, &$options) {
    // It is very important to call the parent function here:
    parent::options_summary($categories, $options);

    $categories['chart'] = array(
      'title' => t('Chart settings'),
      'column' => 'second',
      'build' => array(
        '#weight' => -10,
      ),
    );

    $parent_title = NULL;
    $parent_display = $this->get_option('parent_display');
    if (!empty($this->view->display[$parent_display])) {
      $parent_title = check_plain($this->view->display[$parent_display]->display_title);
    }
    $options['parent_display'] = array(
      'category' => 'chart',
      'title' => t('Combine with parent chart'),
      'value' => $parent_title ? $parent_title : t('None'),
    );
    $options['inherit_yaxis'] = array(
      'category' => 'chart',
      'title' => t('Axis settings'),
      'value' => $this->get_option('inherit_yaxis') ? t('Use primary Y-axis') : t('Create secondary axis'),
    );
    $options['inherit_arguments'] = array(
      'category' => 'chart',
      'title' => t('Inherit contextual filters'),
      'value' => $this->get_option('inherit_arguments') ? t('Yes') : t('No'),
    );
    $options['inherit_exposed_filters'] = array(
      'category' => 'chart',
      'title' => t('Inherit exposed filters'),
      'value' => $this->get_option('inherit_exposed_filters') ? t('Yes') : t('No'),
    );
  }

  /**
   * Provide the default form for setting options.
   */
  function options_form(&$form, &$form_state) {
    // It is very important to call the parent function here:
    parent::options_form($form, $form_state);

    switch ($form_state['section']) {
      case 'parent_display':
        $form['#title'] .= t('Parent display');

        // Filter down the list of displays to include only those that use
        // the chart display style.
        $display_options = array();
        foreach ($this->view->display as $display_name => $display) {
          if ($display->handler->get_option('style_plugin') === 'chart' && $display_name !== $this->view->current_display) {
            $display_options[$display_name] = $display->display_title;
          }
        }
        $form['parent_display'] = array(
          '#title' => t('Parent display'),
          '#type' => 'select',
          '#options' => $display_options,
          '#empty_option' => t('- None - '),
          '#required' => TRUE,
          '#default_value' => $this->get_option('parent_display'),
          '#description' => t('Select a parent display onto which this chart will be overlaid. Only other displays using a "Chart" format are included here. This option may be used to create charts with several series of data or to create combination charts.'),
        );
        break;
      case 'inherit_yaxis':
        $form['#title'] .= t('Axis settings');
        $form['inherit_yaxis'] = array(
          '#title' => t('Y-Axis settings'),
          '#type' => 'radios',
          '#options' => array(
            1 => t('Inherit primary of parent display'),
            0 => t('Create a secondary axis'),
          ),
          '#default_value' => $this->get_option('inherit_yaxis'),
          '#description' => t('In most charts, the X and Y axis from the parent display are both shared with each attached child chart. However, if this chart is going to use a different unit of measurement, a secondary axis may be added on the opposite side of the normal Y-axis.'),
        );
        break;
      case 'inherit_arguments':
        $form['#title'] .= t('Inherit contextual filters');
        $form['inherit_arguments'] = array(
          '#type' => 'checkbox',
          '#title' => t('Inherit'),
          '#description' => t('Should this display inherit its contextual filter values from the parent display to which it is attached?'),
          '#default_value' => $this->get_option('inherit_arguments'),
        );
        break;
      case 'inherit_exposed_filters':
        $form['#title'] .= t('Inherit exposed filters');
        $form['inherit_exposed_filters'] = array(
          '#type' => 'checkbox',
          '#title' => t('Inherit'),
          '#description' => t('Should this display inherit its exposed filter values from the parent display to which it is attached?'),
          '#default_value' => $this->get_option('inherit_exposed_filters'),
        );
        break;
    }
  }

  /**
   * Perform any necessary changes to the form values prior to storage.
   * There is no need for this function to actually store the data.
   */
  function options_submit(&$form, &$form_state) {
    // It is very important to call the parent function here:
    parent::options_submit($form, $form_state);
    switch ($form_state['section']) {
      case 'parent_display':
      case 'inherit_yaxis':
      case 'inherit_arguments':
      case 'inherit_exposed_filters':
        $this->set_option($form_state['section'], $form_state['values'][$form_state['section']]);
        break;
    }
  }

  /**
   * Determine if this display uses exposed filters.
   */
  function uses_exposed() {
    if (!empty($this->options['inherit_exposed_filters']) && parent::uses_exposed()) {
      return TRUE;
    }
    return FALSE;
  }

  /**
   * If an attachment is set to inherit the exposed filter
   * settings from its parent display, then don't render and
   * display a second set of exposed filter widgets.
   */
  function displays_exposed() {
    return $this->options['inherit_exposed_filters'] ? FALSE : TRUE;
  }
}

