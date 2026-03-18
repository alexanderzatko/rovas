<?php

/**
 * @file
 * Defines the alter query object.
 */

/**
 * Object used to create a SELECT query.
 */
class views_contextual_filters_or_search_api_query extends SearchApiViewsQuery {
  function option_definition() {
    $options = parent::option_definition();

    $options['contextual_filters_or'] = array(
      'default' => FALSE,
      'translatable' => FALSE,
      'bool' => TRUE,
    );

    return $options;
  }

  /**
   * Add settings for the ui.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $form['contextual_filters_or'] = array(
      '#title' => t('Contextual filters OR'),
      '#description' => t('Contextual filters applied to OR logic.'),
      '#type' => 'checkbox',
      '#default_value' => !empty($this->options['contextual_filters_or']),
    );
  }

  /**
   * Builds the necessary info to execute the query.
   */
  public function build(&$view) {
    if (!empty($this->where) && $this->options['contextual_filters_or']) {
      $where = array();
      foreach ($this->where as $group_id => $group) {
        if (empty($group_id) && (!empty($group['conditions']) || !empty($group['filters']))) {
          $group += array('type' => 'OR');
          if ($group_id === '') {
            $group_id = 0;
          }
        }
        $where[$group_id] = $group;
      }
      $this->where = $where;
    }
    parent::build($view);
  }
}
