<?php
/**
 * @file
 * Module file containing theme implementations.
 */

/**
 * Theme callback to render the option form as a draggable table.
 */
function theme_views_ef_fieldset_reorder_form($vars) {
  $form = $vars['form'];
  $table_rows = array();

  foreach (element_children($form) as $key) {
    $classes = array('draggable');
    $element = $form[$key];
    $element['id']['#attributes']['class'][] = 'item-id';
    $element['pid']['#attributes']['class'][] = 'item-pid';
    $element['weight']['#attributes']['class'][] = 'item-weight';
    $item = $element['#item'];

    if(!empty($item['is_root'])) {
      $classes[] = 'tabledrag-root';
    }
    if(empty($item['is_group'])) {
      $classes[] = 'tabledrag-leaf';
    }

    $indent = theme('indentation', array('size' => $item['depth']));
    $label = check_plain($item['label']);

    $row = array(
      $indent . $label . drupal_render($element['id']) . ' (' . $item['id'] .')',
      drupal_render($element['pid']),
      drupal_render($element['weight']),
      // @todo Having additional form elements seems to cause invalid indentations with tabledrag.
      drupal_render($element['options']),
      drupal_render($element['remove']),
    );

    $table_rows[] = array('data' => $row, 'class' => $classes, 'id' => 'display-row-' . $key);
  }

  if(!$table_rows) {
    return t('<p><em>No items have been added. To get started, select an item to add.</em></p>');
  }

  drupal_add_tabledrag('reorder-fieldset-filters', 'match', 'parent', 'item-pid', 'item-pid', 'item-id');
  drupal_add_tabledrag('reorder-fieldset-filters', 'order', 'sibling', 'item-weight');

  $header = array(
    t('Item'),
    t('Parent'),
    t('Weight'),
    t('Options'),
    '',
  );

  return theme('table', array(
    'header' => $header,
    'rows' => $table_rows,
    'attributes' => array('id' => 'reorder-fieldset-filters'),
  ));
}

function template_preprocess_views_ef_fieldset_exposed_widget(&$vars) {
  $element = $vars['element'];
  $element += array(
    '#label' => '',
    '#description' => '',
    '#operator' => NULL,
    '#widget' => NULL,
  );
  $widget = $element['#widget'];
  
  $vars['widget'] = (object) array(
    'label' => $element['#label'],
    'id' => isset($widget['#id']) ? $widget['#id'] : '',
    'description' => $element['#description'],
    'operator' => $element['#operator'] ? drupal_render($element['#operator']) : '',
    'widget' => drupal_render($widget),
    'children' => drupal_render_children($element),
  );
  return;
}