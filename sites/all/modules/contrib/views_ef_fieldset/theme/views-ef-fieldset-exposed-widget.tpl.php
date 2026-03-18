<div id="<?php print $widget->id; ?>-wrapper" class="views-exposed-widget views-widget-<?php print $id; ?>">
  <?php if (!empty($widget->label)): ?>
    <label for="<?php print $widget->id; ?>">
      <?php print $widget->label; ?>
    </label>
  <?php endif; ?>
  <?php if (!empty($widget->operator)): ?>
    <div class="views-operator">
      <?php print $widget->operator; ?>
    </div>
  <?php endif; ?>
  <div class="views-widget">
    <?php print $widget->widget; ?>
  </div>
  <?php if (!empty($widget->description)): ?>
    <div class="description">
      <?php print $widget->description; ?>
    </div>
  <?php endif; ?>
  <?php if (!empty($widget->children)): ?>
    <div class="children">
      <?php print $widget->children; ?>
    </div>
  <?php endif; ?>
</div>
