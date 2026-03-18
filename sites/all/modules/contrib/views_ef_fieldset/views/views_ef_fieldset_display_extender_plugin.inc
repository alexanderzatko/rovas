<?php
/**
 * @file
 * Provides a display extender plugin for View 3.x.
 */

class views_ef_fieldset_display_extender_plugin extends views_plugin_display_extender {

  /**
   * @var views_plugin_display
   */
  public $display;

  /**
   * @var ViewsEFFieldsetItemManager
   */
  protected $manager;

  public function get_manager() {
    if (!isset($this->manager)) {
      $definitions = module_invoke_all('views_ef_fieldset_item_types');
      $this->manager = new ViewsEFFieldsetItemManager($this->display, $definitions);
    }
    return $this->manager;
  }

  /**
   * {@inheritdoc}
   */
  public function options_definition_alter(&$options) {
    $options['views_ef_fieldset'] = array(
      'contains' => array(
        'enabled' => array('default' => FALSE, 'bool' => TRUE),
        'items' => array('default' => array()),
      ),
    );
  }

  /**
   * {@inheritdoc}
   */
  public function options_summary(&$categories, &$options) {
    $is_enabled = !empty($this->get_option('enabled'));
    $options['views_ef_fieldset_base'] = array(
      'category' => 'exposed',
      'title' => 'Fieldsets',
      'value' => $is_enabled ? 'Enabled' : 'Disabled',
      'desc' => 'Select basic options for filter fieldsets.',
    );
    if ($is_enabled) {
      $options['views_ef_fieldset_base'] += array(
        'setting' => 'Manage',
        'links' => array(
          'views_ef_fieldset_arrange' => 'Manage filter positions and groups.',
        ),
      );
    }
  }

  /**
   * Returns an extender plugin option.
   */
  public function get_option($name, $check_form_cache = FALSE) {
    if ($check_form_cache) {
      /** @noinspection PhpUndefinedFieldInspection */
      $form_cache = isset($this->view->form_cache) ? $this->view->form_cache : array();
      $key_exists = TRUE;
      $value = drupal_array_get_nested_value($form_cache, array('views_ef_fieldset', $name), $key_exists);
      if ($key_exists) {
        return $value;
      }
    }
    $options = $this->display->get_option('views_ef_fieldset');
    return isset($options[$name]) ? $options[$name] : NULL;
  }

  /**
   * Sets an extender plugin option.
   */
  public function set_option($name, $value) {
    $options = $this->display->get_option('views_ef_fieldset');
    $options[$name] = $value;
    $this->display->set_option('views_ef_fieldset', $options);
  }

  public function set_cached_option($name, $value) {
    /** @noinspection PhpUndefinedFieldInspection */
    $this->view->form_cache['views_ef_fieldset'][$name] = $value;
  }

  /**
   * {@inheritdoc}
   */
  public function options_form(&$form, &$form_state) {
    switch ($form_state['section']) {

      // Base settings form.
      case 'views_ef_fieldset_base':
        $this->options_form_base($form, $form_state);
        break;

      // Arrange form.
      case 'views_ef_fieldset_arrange':
        $this->options_form_arrange($form, $form_state);
        break;
    }
  }
  
  protected function options_form_base(&$form, &$form_state) {
    /** @noinspection PhpUndefinedFieldInspection */
    $display_title = $this->display->display->display_title;
    $form['#title'] = check_plain($display_title) . ': ' . 'Filter fieldsets';

    // @todo Add better title and description. Use radios to match views ui forms.
    $form['enabled'] = array(
      '#type' => 'checkbox',
      '#title' => 'Enabled',
      '#default_value' => !empty($this->get_option('enabled')),
    );
  }

  protected function options_form_arrange(&$form, &$form_state) {
    $manager = $this->get_manager();
    $valid_items = $this->manager->getItems();
    // Pull items either from the form cache or the view.
    $items = (array) $this->get_option('items', TRUE);

    foreach ($items as $key => $item) {
      // Remove items that are no longer available.
      if (!isset($valid_items[$item['id']])) {
        unset($items[$key]);
        continue;
      }
      // Add default keys.
      $items[$key] += $valid_items[$item['id']];
    }

    // Filter items that haven't been added yet.
    $backlog_items = array_diff_key($valid_items, $items);

    // Create an option list of addable items.
    $add_item_options = array();
    foreach ($backlog_items as $item) {
      $add_item_options[$item['id']] = $item['label'];
    }

    $tree = new ViewsEFFieldsetTree($items);
    $item_list = $tree->getFlattenedTree();
    $weight_delta = count($item_list);

    // Options for the (hidden) pid selects.
    $parent_options = array();
    foreach ($item_list as $item) {
      if (!empty($item['is_group'])) {
        $parent_options[$item['id']] = $item['id'];
      }
    }

    /** @noinspection PhpUndefinedFieldInspection */
    $display_title = $this->display->display->display_title;
    $form['#title'] = check_plain($display_title) . ': ' . 'Arrange filter fieldsets';

    $form['item_add'] = array(
      '#type' => 'select',
      '#options' => $add_item_options,
      '#empty_value' => '',
      '#empty_option' => t('- Add item -'),
      '#name' => 'item_add',
    );

    $form['item_add_submit'] = array(
      '#submit' => array(array($this, 'arrange_submit')),
      '#value' => 'Add',
      '#name' => 'item_add_submit',
      '#action' => 'item_add',
    );

    // Automatically trigger a submit when an item is selected.
    $form['item_add'] += array(
      '#action' => 'item_add',
      '#submit' => array(array($this, 'arrange_submit')),
      '#executes_submit_callback' => TRUE,
      '#ajax' => array(
        'path' => views_ui_build_form_url($form_state),
      ),
    );
    // Hide the "Add" button by using a #states condition that will never be
    // true.
    $form['item_add_submit'] += array(
      '#states' => array(
        'visible' => array(
          'select[name="item_add"]' => array('value' => FALSE),
        ),
      ),
    );

    $form['items'] = array(
      '#type' => 'fieldset',
      '#title' => 'Items',
      '#theme' => 'views_ef_fieldset_reorder_form',
      '#tree' => TRUE,
    );

    foreach ($item_list as $item) {
      $item_form = array(
        '#tree' => TRUE,
        '#item' => $item,
      );

      $item_form['id'] = array(
        '#type' => 'hidden',
        '#default_value' => $item['id'],
      );

      $item_form['pid'] = array(
        '#type' => 'select',
        '#options' => $parent_options,
        '#empty_value' => '',
        '#default_value' => $item['pid'],
      );
      unset($item_form['pid']['#options'][$item['id']]);

      $item_form['weight'] = array(
        '#type' => 'weight',
        '#title' => t('Weight'),
        '#default_value' => $item['weight'],
        '#delta' => $weight_delta,
        '#title_display' => 'invisible',
      );

      $item_options = isset($item['options']) ? $item['options'] : array();
      $item_form['options'] = $manager->getOptionsForm($item['id'], $item_options);

      $item_form['remove'] = array(
        '#type' => 'submit',
        '#value' => 'Remove',
        '#submit' => array(array($this, 'arrange_submit')),
        '#action' => 'item_remove',
        // Required to reliably identify the clicked button.
        '#name' => 'item_remove_' . $item['id'],
      );
      $form['items'][$item['id']] = $item_form;
    }
  }

  /**
   * Submit callback for the arrange form.
   *
   * Persists form changes without actually submitting and saving the changes.
   */
  public function arrange_submit(&$form, &$form_state) {
    $trigger = !empty($form_state['triggering_element']) ? $form_state['triggering_element'] : NULL;
    // #action is a custom element key that makes it easier to identify the
    // clicked button.
    $action = isset($trigger['#action']) ? $trigger['#action'] : NULL;
    $values = $form_state['values'] + array('items' => array());
    $items = $values['items'];

    switch ($action) {

      // Add a new item.
      case 'item_add':
        // Other submit callbacks may access this plugin using
        // $form_state['view']->display_handler->extender['views_ef_fieldset'].
        $manager = $this->get_manager();
        if ($item = $manager->getItem($values['item_add'])) {
          $items[$item['id']] = $item;
        }
        break;

      // Remove a single item.
      case 'item_remove':
        list($id) = array_slice($trigger['#parents'], -2, 1) + array(NULL);
        if (isset($items[$id])) {
          unset($items[$id]);
        }
        // @todo Reassign weights so that children of removed element are placed before next element.
        break;

      default:
        // Continue to save the form state, in case another module has added
        // its own actions.
    }

    // Remove form-specific extra values to avoid polluting the item
    // definitions.
    // @todo Consider using data objects instead of plain arrays.
    $clean_keys = array_flip(array('id', 'pid', 'weight', 'options'));
    foreach ($items as $id => $item) {
      $items[$id] = array_intersect_key($item, $clean_keys);
    }

    // form_cache allows us to persist option changes across multiple requests
    // without having to set them directly in the view (which would make
    // cancelling impossible. It will be cleared automatically if a form gets
    // cancelled (see e.g. views_ui_standard_cancel()) or if form_cache['key']
    // does not match the current form key (see views_ui_ajax_form()).
    // Clearing the form cache will only take effect once another form is
    // submitted (or cancelled) and the view gets cached again.
    //
    // Unfortunately almost all Views UI forms share the form key "display".
    // It is therefore a good idea to use your own namespace in form_cache
    // instead of relying on views_ui_ajax_form() to clear it for you.
    // Also, due to a Views bug using the form cache can temporarily break the
    // filter arrange form. See https://www.drupal.org/node/2716593.
    $form_state['view']->form_cache = array('key' => $form_state['form_key']);
    $this->set_cached_option('items', $items);

    // Add the current form identifier to the stack so that the form will be
    // loaded again. In combination with the form_cache property this enables
    // a behavior similar to standard forms with $form_state['cache'] = TRUE
    // and $form_state['rebuild'] = TRUE.
    views_ui_add_form_to_stack($form_state['form_key'], $form_state['view'], $form_state['display_id'], array($form_state['section']));
    // Persist changes to the form cache.
    views_ui_cache_set($form_state['view']);
  }

  /**
   * {@inheritdoc}
   */
  public function options_submit(&$form, &$form_state) {
    switch ($form_state['section']) {
      // Handle base settings.
      case 'views_ef_fieldset_base':
        $enabled_old = !empty($this->get_option('enabled'));
        $enabled_new = !empty($form_state['values']['enabled']);
        $this->set_option('enabled', $enabled_new);
        // If fieldsets were just enabled, step on to the arrange form.
        if (!$enabled_old && $enabled_new) {
          views_ui_add_form_to_stack($form_state['form_key'], $form_state['view'], $form_state['display_id'], array('views_ef_fieldset_arrange'));
        }
        break;

      // Handle final submission of the arrange form.
      case 'views_ef_fieldset_arrange':
        $values = $form_state['values'] + array('items' => array());
        $items = array();
        // @todo Consider using data objects instead of plain arrays.
        $clean_keys = array_flip(array('id', 'pid', 'weight', 'options'));
        foreach ($values['items'] as $id => $item) {
          $items[$id] = array_intersect_key($item, $clean_keys);
        }

        $this->set_option('items', $items);
        // Invalidate the form cache.
        unset($this->view->form_cache);
        break;
    }
  }

}
