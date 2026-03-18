<?php

class ViewsEFFieldsetItemManager {

  /**
   * @var array
   */
  protected $plugins = array();

  public function __construct(views_plugin_display $display, $type_definitions) {
    foreach ($type_definitions as $type => $definition) {
      $this->plugins[$type] = new $definition['class']($display);
    }
  }

  public function getItems($type = NULL) {
    $items = array();
    $types = is_null($type) ? NULL : array($type);
    foreach ($this->getPlugins($types) as $plugin_type => $plugin) {
      foreach ($plugin->getItems() as $item) {
        $item = $this->prepareItem($item, $plugin_type);
        $items[$item['id']] = $item;
      }
    }
    return $items;
  }

  public function getItem($id) {
    list($type, $item_id) = $this->splitItemId($id);
    if (($plugin = $this->getPlugin($type)) && $item = $plugin->getItem($item_id)) {
      return $this->prepareItem($item, $type);
    }
    return NULL;
  }

  /**
   * Filters and unprefixes items.
   */
  public function getUnprefixedItems($items, $type) {
    $raw_items = array();
    foreach ($items as $item) {
      list($item_type, $id) = $this->splitItemId($item['id']);
      if ($item_type === $type) {
        $item['id'] = $id;
        $raw_items[$id] = $item;
      }
    }
    return $raw_items;
  }

  public function getOptionsForm($id, array $options) {
    list($type, $item_id) = $this->splitItemId($id);
    if ($plugin = $this->getPlugin($type)) {
      return $this->plugins[$type]->getOptionsForm($item_id, $options);
    }
    return array();
  }

  public function getWidget($id, array &$form, array $children) {
    list($type, $item_id) = $this->splitItemId($id);
    if ($plugin = $this->getPlugin($type)) {
      return $this->plugins[$type]->getWidget($item_id, $form, $children);
    }
    return array();
  }

  protected function getPlugin($type) {
    return isset($this->plugins[$type]) ? $this->plugins[$type] : NULL;
  }

  protected function getPlugins(array $types = NULL) {
    if (is_null($types)) {
      return $this->plugins;
    }
    $plugins = array();
    foreach ($types as $type) {
      if (isset($this->plugins[$type])) {
        $plugins[$type] = $this->plugins[$type];
      }
    }
    return $plugins;
  }

  /**
   * Prefixes item ID and adds default keys.
   *
   * @todo Consider using data objects instead.
   */
  protected function prepareItem($item, $type) {
    $item['id'] = $this->prefixItemId($item['id'], $type);
    $item['is_group'] = !empty($item['is_group']);
    $item['is_root'] = !empty($item['is_root']);
    $item += array('options' => array());
    return $item;
  }

  /**
   * Prefixes item ID with item type.
   */
  protected function prefixItemId($id, $type) {
    return $type . ':' . $id;
  }

  /**
   * Splits a prefixed item ID in type and unprefixed ID.
   */
  protected function splitItemId($id) {
    return explode(':', $id, 2) + array(NULL, NULL);
  }

}
