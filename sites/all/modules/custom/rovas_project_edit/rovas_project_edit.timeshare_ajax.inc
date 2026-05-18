<?php

/**
 * @file
 * Timeshare rate AJAX registration helpers.
 */

/**
 * Register #ajax during form_builder once #id and #parents exist.
 */
function _rpe_timeshare_process_rate_value($element, &$form_state, $form) {
	$instance = field_widget_instance($element, $form_state);
	if (!$instance || empty($instance['field_name'])) {
		return $element;
	}
	$field_name = $instance['field_name'];
	if (!in_array($field_name, array('field_rate', 'field_time_share_cost_euros'), TRUE)) {
		return $element;
	}
	_rpe_timeshare_attach_ajax_to_widget($element, $field_name);
	if (empty($element['#ajax_processed'])) {
		$element = ajax_pre_render_element($element);
	}
	return $element;
}

/**
 * Append process callback; sliderfield breaks default textfield #process merge.
 */
function _rpe_timeshare_ensure_ajax_process(&$widget, $field_name = NULL) {
	if (!isset($widget['#process'])) {
		$widget['#process'] = array();
	}
	if (!in_array('_rpe_timeshare_process_rate_value', $widget['#process'], TRUE)) {
		$widget['#process'][] = '_rpe_timeshare_process_rate_value';
	}
}

/**
 * Register Drupal AJAX on rate widgets after the form tree is built.
 */
function _rpe_timeshare_register_ajax_after_build($form, &$form_state) {
	foreach (array('field_rate', 'field_time_share_cost_euros') as $field_name) {
		$widget = &_rpe_timeshare_value_widget($form, $field_name);
		if ($widget === NULL || !isset($widget['#ajax'])) {
			continue;
		}
		if (empty($widget['#ajax_processed'])) {
			$widget = ajax_pre_render_element($widget);
		}
	}
	return $form;
}
