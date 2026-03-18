<?php

class ViewsEFFieldsetComparator {

  /**
   * Skips sort for NULL values.
   */
  const NULL_SORT_SKIP = 1;

  /**
   * Positions NULL values first.
   */
  const NULL_SORT_FIRST = 2;

  /**
   * Positions NULL values last.
   */
  const NULL_SORT_LAST = 3;

  protected $nullA;
  protected $nullB;
  protected $key;

  /**
   * Constructor.
   *
   * @param mixed $key
   *   The array key used for comparison.
   * @param NULL $nullBehavior
   *   The behavior used to compare NULL values.
   */
  public function __construct($key, $nullBehavior = NULL) {
    $this->key = $key;

    switch ($nullBehavior) {
      case self::NULL_SORT_SKIP:
        $this->nullA = 0;
        $this->nullB = 0;
        break;

      case self::NULL_SORT_FIRST:
        $this->nullA = -1;
        $this->nullB = 1;
        break;

      case self::NULL_SORT_LAST:
        $this->nullA = 1;
        $this->nullB = -1;
    }
  }

  public function getCallback() {
    return array($this, 'sort');
  }

  /**
   * Sort callback.
   */
  public function sort(array $array_a, array $array_b) {
    $a = $array_a[$this->key];
    $b = $array_b[$this->key];

    if ($a === $b) {
      return 0;
    }
    if ($a === NULL && $this->nullA !== NULL) {
      return $this->nullA;
    }
    if ($b === NULL && $this->nullB !== NULL) {
      return $this->nullB;
    }
    if ($a == $b) {
      return 0;
    }
    return $a < $b ? -1 : 1;
  }

}
