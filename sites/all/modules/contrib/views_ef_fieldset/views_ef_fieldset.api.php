<?php

function hook_views_ef_fieldset_item_types() {
  return array(
    'filter' => array(
      'class' => 'ViewsEFFieldsetItemTypeFilter'
    ),
    'fieldset' => array(
      'class' => 'ViewsEFFieldsetItemTypeFieldset'
    ),
  );
}
