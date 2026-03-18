<?php

class ViewsEFFieldsetTree {

  protected $index = array();
  protected $tree = array();

  public function __construct(array $items) {
    $index = $this->createIndex($items);
    $orphans = array();
    $tree = &$this->createTree($index, $orphans);

    $comparator = new ViewsEFFieldsetComparator('weight', ViewsEFFieldsetComparator::NULL_SORT_LAST);
    $sort_callback = $comparator->getCallback();

    // Sort children.
    foreach ($index as &$item) {
      uasort($item['children'], $sort_callback);
    }
    // Sort root items.
    uasort($tree, $sort_callback);
    uasort($orphans, $sort_callback);

    $tree += $orphans;

    $this->setWeights($tree, TRUE);
    $this->setDepth($tree);

    $this->tree = $tree;
  }

  public function getTree() {
    return $this->tree;
  }

  public function getFlattenedTree() {
    return $this->flattenTree($this->tree);
  }

  protected function flattenTree(array $tree) {
    $index = array();
    foreach ($tree as $item) {
      $index[$item['id']] = $item;
      $index += $this->flattenTree($item['children']);
    }
    return $index;
  }

  /**
   * Creates a flat normalized index of items.
   */
  protected function createIndex(array $items) {
    $index = array();
    foreach ($items as $item) {
      $item['children'] = array();
      $item['weight'] = isset($item['weight']) ? (int) $item['weight'] : NULL;
      $item['pid'] = isset($item['pid']) && strlen($item['pid'])
        ? $item['pid']
        : NULL;
      if (isset($index[$item['id']])) {
        throw new Exception('Duplicate item key "' . $item['id'] . '"');
      }
      $index[$item['id']] = $item;
    }
    return $index;
  }

  protected function &createTree(array &$index, array &$orphans = array()) {
    $tree = array();
    foreach ($index as &$item) {
      $id = $item['id'];
      $pid = $item['pid'];

      // Handle root elements.
      if (is_null($pid)) {
        $tree[$id] = &$item;
        continue;
      }
      // Handle child elements.
      if (isset($index[$pid])) {
        $index[$pid]['children'][$id] = &$item;
        continue;
      }

      // No parent was found, add to orphans stack.
      $orphans[$id] = &$item;
    }
    return $tree;
  }

  protected function setWeights(&$tree, $negative_offset = FALSE) {
    $weight = count($tree);
    if ($negative_offset) {
      $weight = 0 - $weight;
    }
    foreach ($tree as &$item) {
      $item['weight'] = $weight++;
      $this->setWeights($item['children'], $negative_offset);
    }
  }

  protected function setDepth(&$tree, $depth = 0) {
    foreach ($tree as &$item) {
      $item['depth'] = $depth;
      $this->setDepth($item['children'], $depth + 1);
    }
  }

}
