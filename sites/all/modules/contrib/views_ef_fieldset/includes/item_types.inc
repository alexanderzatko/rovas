<?php

/**
 * Base class for type plugins.
 */
abstract class ViewsEFFieldsetItemType {

  /**
   * @var views_plugin_display
   */
  protected $display;

  public function __construct(views_plugin_display $display) {
    $this->display = $display;
  }

  public function getItem($id) {
    return NULL;
  }

  public function getItems() {
    return array();
  }

  public function getOptionsForm($id, array $options) {
    return array();
  }

  public function getWidget($id, array &$form, array $children) {
    return array();
  }

}

/**
 * Exposes filters as items.
 */
class ViewsEFFieldsetItemTypeFilter extends ViewsEFFieldsetItemType {

  /**
   * {@inheritdoc}
   */
  public function getItem($id) {
    if ($handler = $this->getHandler($id)) {
      return array(
        'id' => $id,
        'is_group' => TRUE,
        'label' => 'Filter: ' . $handler->options['expose']['label'],
      );
    }
    return NULL;
  }

  /**
   * {@inheritdoc}
   */
  public function getItems() {
    $items = array();
    foreach ($this->display->get_handlers('filter') as $handler_id => $handler) {
      if ($item = $this->getItem($handler_id)) {
        $items[] = $item;
      }
    }
    return $items;
  }

  public function getWidget($id, array &$form, array $children) {
    if (($handler = $this->getHandler($id)) && $info = $handler->exposed_info()) {
      $info += array('operator' => NULL);
      $operator = $info['operator'];
      $name = $info['value'];

      if (isset($form[$name])) {
        $element = array(
          '#theme' => 'views_ef_fieldset_exposed_widget',
          '#label' => $info['label'],
          '#description' => $info['description'],
          '#widget' => $form[$name],
        );
        $form[$name]['#printed'] = TRUE;
        unset($form['#info']["filter-$name"]);

        if ($operator && isset($form[$operator])) {
          $element['#operator'] = $form[$operator];
          $form[$operator]['#printed'] = TRUE;
        }

        return $element + $children;
      }
    }
    return array();
  }

  protected function getHandler($id) {
    /** @var views_handler_filter $handler */
    $handler = $this->display->get_handler('filter', $id);
    return $handler && $handler->is_exposed() ? $handler : NULL;
  }

}

/**
 * Exposes sorts as items.
 */
class ViewsEFFieldsetItemTypeSort extends ViewsEFFieldsetItemType {

  /**
   * {@inheritdoc}
   */
  public function getItem($id) {
    $items = $this->getItems();
    return isset($items[$id]) ? $items[$id] : NULL;
  }

  /**
   * {@inheritdoc}
   */
  public function getItems() {
    $items = array();

    $sort_by_exposed = FALSE;
    // Check if at least one sort handler is exposed.
    /** @var views_handler_sort $handler */
    foreach ($this->display->get_handlers('sort') as $handler) {
      if ($handler->can_expose() && !empty($handler->options['exposed'])) {
        $sort_by_exposed = TRUE;
        break;
      }
    }

    if ($sort_by_exposed) {
      $items['sort_by'] = array(
        'id' => 'sort_by',
        'label' => 'Sort: ' . $this->getExposedOption('exposed_sorts_label') . ' (sort by)',
      );
    }

    if ($this->getExposedOption('expose_sort_order')) {
      $items['sort_order'] = array(
        'id' => 'sort_order',
        'label' => 'Sort: Order',
      );
    }

    return $items;
  }

  /**
   * {@inheritdoc}
   */
  public function getWidget($id, array &$form, array $children) {
    $element = array();

    switch ($id) {
      case 'sort_by':
      case 'sort_order':
        if (isset($form[$id])) {
          $element = $form[$id];
          $form[$id]['#printed'] = TRUE;
        }
        break;
    }

    return $element;
  }

  /**
   * Returns an exposed form option.
   *
   * @param string $name
   *   The option name.
   *
   * @return mixed|NULL
   *   The option value or NULL if the option key does not exist.
   */
  protected function getExposedOption($name) {
    $options = $this->display->get_option('exposed_form');
    return isset($options['options'][$name]) ? $options['options'][$name] : NULL;
  }

}

/**
 * Exposes buttons as items.
 */
class ViewsEFFieldsetItemTypeButton extends ViewsEFFieldsetItemType {

  /**
   * {@inheritdoc}
   */
  public function getItem($id) {
    $items = $this->getItems();
    return isset($items[$id]) ? $items[$id] : NULL;
  }

  /**
   * {@inheritdoc}
   */
  public function getItems() {
    $items = array();

    $items['submit'] = array(
      'id' => 'submit',
      'label' => 'Button: ' . $this->getExposedOption('submit_button') . ' (submit)',
    );

    if ($this->getExposedOption('reset_button')) {
      $items['reset'] = array(
        'id' => 'reset',
        'label' => 'Button: ' . $this->getExposedOption('reset_button_label') . ' (reset)',
      );
    }

    return $items;
  }

  /**
   * {@inheritdoc}
   */
  public function getWidget($id, array &$form, array $children) {
    $element = array();

    switch ($id) {
      case 'submit':
      case 'reset':
        if (isset($form[$id])) {
          $element = $form[$id];
          $form[$id]['#printed'] = TRUE;
        }
        break;
    }
    return $element;
  }

  /**
   * Returns an exposed form option.
   *
   * @param string $name
   *   The option name.
   *
   * @return mixed|NULL
   *   The option value or NULL if the option key does not exist.
   */
  protected function getExposedOption($name) {
    $options = $this->display->get_option('exposed_form');
    return isset($options['options'][$name]) ? $options['options'][$name] : NULL;
  }

}

/**
 * Base class for dynamically generated item types.
 */
abstract class ViewsEFFieldsetItemTypeDynamic extends ViewsEFFieldsetItemType {

  /**
   * Returns the plugin type identifier.
   *
   * @return string
   *   The plugin type.
   */
  abstract protected function getType();

  /**
   * {@inheritdoc}
   */
  public function getItem($id) {
    $items = $this->getItems();
    if (isset($items[$id])) {
      return $this->prepareItem($items[$id]);
    }
    return NULL;
  }

  /**
   * {@inheritdoc}
   */
  public function getItems() {
    /** @var views_ef_fieldset_display_extender_plugin $extender */
    $extender = $this->display->extender['views_ef_fieldset'];
    // Pull items either from the form cache or the view.
    $prefixed_items = $extender->get_option('items', TRUE);
    $unprefixed_items = $extender->get_manager()->getUnprefixedItems($prefixed_items, $this->getType());

    $items = array();
    foreach ($unprefixed_items as $item) {
      $item = $this->prepareItem($item);
      $items[$item['id']] = $item;
    }
    $items += $this->getVirtualItems();

    return $items;
  }

  protected function prepareItem($item) {
    if ($item['id'] === 'new') {
      $item['id'] = uniqid();
    }
    return $item;
  }

  protected function getVirtualItems() {
    return array(
      'new' => array(
        'id' => 'new',
        'label' => t('New'),
      ),
    );
  }

}

/**
 * Provides fieldset items.
 */
class ViewsEFFieldsetItemTypeFieldset extends ViewsEFFieldsetItemTypeDynamic {

  /**
   * {@inheritdoc}
   */
  protected function getType() {
    return 'fieldset';
  }

  /**
   * {@inheritdoc}
   */
  protected function getVirtualItems() {
    $items = parent::getVirtualItems();
    $items['new']['label'] = t('New fieldset');
    return $items;
  }

  /**
   * {@inheritdoc}
   */
  protected function prepareItem($item) {
    $item['label'] = 'Fieldset';
    $item['is_group'] = TRUE;
    return parent::prepareItem($item);
  }

  public function getWidget($id, array &$form, array $children) {
    if (($item = $this->getItem($id)) && $children) {

      $element = array(
        '#type' => 'fieldset',
        '#title' => t($item['options']['label']),
      );

      switch ($item['options']['collapse']) {
        case 'collapsible':
          $element['#collapsible'] = TRUE;
          $element['#collapsed'] = FALSE;
          break;
        case 'collapsed':
          $element['#collapsible'] = TRUE;
          $element['#collapsed'] = TRUE;
          break;
      }

      $element['#parents'] = array();
      $fake_state = array();
      form_process_fieldset($element, $fake_state);

      return $element + $children;
    }
    return array();
  }

  /**
   * {@inheritdoc}
   */
  public function getOptionsForm($id, array $options) {
    $options += array(
      'label' => '',
      'collapse' => '',
    );
    $form = array(
      'label' => array(
        '#title' => 'Label',
        '#type' => 'textfield',
        '#default_value' => $options['label'],
      ),
      'collapse' => array(
        '#title' => 'Mode',
        '#type' => 'select',
        '#options' => array(
          '' => 'Static',
          'collapsible' => 'Collapsible',
          'collapsed' => 'Collapsed',
        ),
        '#default_value' => $options['collapse'],
      ),
    );
    return $form;
  }

}

/**
 * Provides vertical tabs items.
 */
class ViewsEFFieldsetItemTypeVerticalTabs extends ViewsEFFieldsetItemTypeDynamic {

  /**
   * {@inheritdoc}
   */
  protected function getType() {
    return 'vertical_tabs';
  }

  /**
   * {@inheritdoc}
   */
  protected function getVirtualItems() {
    $items = parent::getVirtualItems();
    $items['new']['label'] = t('New vertical tabs group');
    return $items;
  }

  /**
   * {@inheritdoc}
   */
  protected function prepareItem($item) {
    $item['label'] = 'Vertical tabs';
    $item['is_group'] = TRUE;
    return parent::prepareItem($item);
  }

  public function getWidget($id, array &$form, array $children) {
    if (($item = $this->getItem($id)) && $children) {

      $element = array(
        '#type' => 'vertical_tabs',
      );

      $element['#parents'] = array();

      return $element + $children;
    }
    return array();
  }

}

/**
 * Provides simple container items.
 */
class ViewsEFFieldsetItemTypeContainer extends ViewsEFFieldsetItemTypeDynamic {

  /**
   * {@inheritdoc}
   */
  protected function getType() {
    return 'container';
  }

  /**
   * {@inheritdoc}
   */
  protected function getVirtualItems() {
    $items = parent::getVirtualItems();
    $items['new']['label'] = t('New container');
    return $items;
  }

  /**
   * {@inheritdoc}
   */
  protected function prepareItem($item) {
    $item['label'] = 'Container';
    $item['is_group'] = TRUE;
    return parent::prepareItem($item);
  }

  public function getWidget($id, array &$form, array $children) {
    if (($item = $this->getItem($id)) && $children) {

      $element = array(
        '#type' => 'container',
        '#attributes' => array(
          'class' => array($item['options']['class'])
        )
      );

      $element['#parents'] = array();

      return $element + $children;
    }
    return array();
  }

  /**
   * {@inheritdoc}
   */
  public function getOptionsForm($id, array $options) {
    $options += array(
      'class' => '',
    );
    $form = array(
      'class' => array(
        '#title' => 'class',
        '#type' => 'textfield',
        '#default_value' => $options['class'],
      ),
    );
    return $form;
  }

}
