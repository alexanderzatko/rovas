Rovas Site Rules
================

This module provides default Rules configurations for the Rovas site. Rules are
stored as JSON files in the rules/ subdirectory and loaded via
hook_default_rules_configuration().

Rules excluded (provided by other modules):
- rules_check_or_add_time_shareholder_core (rovas_purchase_shares)
- rules_create_time_share (rovas_purchase_shares)

Rule status in the Rules UI:
  Rules are provided via hook_default_rules_configuration() so Entity API
  assigns them "Default" status. If a rule is edited in the UI it becomes
  "Overridden" and can be reverted to the JSON-file version at any time.

Migration (re-import all rules from JSON files):
  Via Drush (recommended):
    drush php-eval "module_load_include('inc', 'rovas_site_rules', 'rovas_site_rules.migration'); rovas_site_rules_migration_run();"

  Via browser (Devel > Execute PHP):
    module_load_include('inc', 'rovas_site_rules', 'rovas_site_rules.migration');
    rovas_site_rules_migration_run();

  The migration deletes all DB entries for this module's rules, then calls
  entity_defaults_rebuild() which re-imports them from the JSON files and saves
  them with "Default" status.

"Maximum function nesting level" (e.g. on "drush cc all" or after migration):
  During cache clears, entity_defaults_rebuild() calls
  hook_default_rules_configuration(), which runs rules_import() for each of the
  139 JSON files. For complex rules this creates a deep — but finite — call
  stack. PHP itself handles this fine, but Xdebug's default nesting limit (100)
  is too low.

  Option 1 — raise the limit for the command:
    php -d xdebug.max_nesting_level=1000 $(which drush) cc all

  Option 2 — disable Xdebug for CLI cache clears:
    php -d xdebug.mode=off $(which drush) cc all

  Option 3 — raise the limit permanently in php.ini (CLI section):
    xdebug.max_nesting_level = 1000
