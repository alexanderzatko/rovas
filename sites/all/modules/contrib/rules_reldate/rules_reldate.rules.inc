<?php

/**
 * Implement hook_rules_action_info().
 */
function rules_reldate_rules_action_info() {
  return array(
    'rules_reldate_rules_action_calculate_relative_date' => array(
      'label' => t('Calculate a relative date'),
      'group' => t('Data'),
      'parameter' => array(
        'date' => array(
          'type' => 'date',
          'label' => t('Initial date value'),
          'description' => t('This is the anchor value that will be used to determine the relative date'),
        ),
        'offset' => array(
          'type' => 'text',
          'label' => t('Relative offset'),
          'description' => t('This value will be made relative to the anchor date'),
        ),
      ),
      'provides' => array(
        'adjusted_date' => array(
          'type' => 'date',
          'label' => t('Adjusted Date'),
        ),
      ),
    ),
  );
}
function rules_reldate_rules_action_calculate_relative_date($date, $offset) {
  $calculated_date = strtotime($offset, $date);
  return array('adjusted_date' => $calculated_date);
}
