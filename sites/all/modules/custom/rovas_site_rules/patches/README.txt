Apply the Entity module patch on the server to prevent entity_defaults_rebuild()
from crashing when a default hook returns NULL or a non-object (e.g. a failed
rules import). Without it, Commerce Tax/Payment (or other code) calling
entity_defaults_rebuild() can trigger "Creating default object from empty value"
and "Field 'plugin' doesn't have a default value" for rules_config.

From the Drupal root (e.g. /var/www/rovdev):

  cd sites/all/modules/entity
  patch -p1 < /path/to/rovas_site_rules/patches/entity-defaults-rebuild-skip-non-objects.patch

If the patch fails to apply (e.g. different Entity version), add the same guard
manually in entity.module in _entity_defaults_rebuild(): inside the inner
foreach over module_invoke($module, $hook), add at the start of the loop:

  if (!is_object($entity)) {
    continue;
  }
