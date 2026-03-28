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

Re-importing all rules from the JSON files (migration)
--------------------------------------------------------
Use drush74 (not plain drush) on Rovas servers: the default "drush" binary may
use a different PHP or bootstrap context, so "drush pm-info" can show the wrong
status (e.g. module "not installed" while the admin UI shows it enabled). If
commands or counts do not match what you see in the browser, you are almost
certainly using the wrong Drush.

Use this after you change JSON files in rules/ and want the site to match them,
or when Rules entries for this module look wrong or out of date.

What it does (plain language):
  - Removes the saved copies of this module's rules from the database (only the
    rules that come from these JSON files; other modules' rules are untouched).
  - Re-loads every JSON file and writes fresh copies into the database with
    "Default" status in the Rules UI.

How to run it

  A) Drush on the server (recommended — one step, no batch progress page)
     From the Drupal root (the directory that contains index.php), run:

       drush74 php-eval "module_load_include('inc', 'rovas_site_rules', 'rovas_site_rules.migration'); rovas_site_rules_migration_run();"

     What you should see:
     - A short summary line with how many JSON files were processed and how many
       old database rows were removed. "0 rows removed" often means the rules
       were not in the database yet (for example after a fresh deploy); the
       second half of the process still re-imports from JSON.
     - If something is wrong, check Administration > Reports > Recent log
       messages for lines like "Skipped invalid rule" (type may show as
       rovas_site_rules). Those lines explain which JSON file failed and why.

  B) Browser (Devel > Execute PHP)
     Paste and run:

       module_load_include('inc', 'rovas_site_rules', 'rovas_site_rules.migration');
       rovas_site_rules_migration_run();

     The site will show a batch progress screen. Wait until it finishes; you
     should see a green status message when done. Do not close the tab early.

After a code deploy to the server
  - If the migration or the site behaves oddly right after uploading new files,
    clear caches (drush74 cc all) or ask your host to reload PHP (OPcache
    can briefly serve old code). Then run the migration again using method A or B.

"Maximum function nesting level" (e.g. on "drush74 cc all" or after migration):
  During cache clears, entity_defaults_rebuild() calls
  hook_default_rules_configuration(), which runs rules_import() for each of the
  139 JSON files. For complex rules this creates a deep — but finite — call
  stack. PHP itself handles this fine, but Xdebug's default nesting limit (100)
  is too low.

  Option 1 — raise the limit for the command:
    php -d xdebug.max_nesting_level=1000 $(which drush74) cc all

  Option 2 — disable Xdebug for CLI cache clears:
    php -d xdebug.mode=off $(which drush74) cc all

  Option 3 — raise the limit permanently in php.ini (CLI section):
    xdebug.max_nesting_level = 1000
