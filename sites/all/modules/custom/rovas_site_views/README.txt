Rovas Site Views
================

Catch-all module for site-owned Views that do not fit a domain module
(disputes, ops helpers, legacy Ubercart, overridden taxonomy term, etc.).

Each view is a PHP export in views/<machine_name>.inc, loaded via
hook_views_default_views(). Domain-owned Views live in their domain modules
instead (see docs/views-inventory.md).

View status in the Views UI
---------------------------
Defaults from code show as "Default". Editing in the UI makes them
"Overridden". Revert in the UI, or run the migration below, so the site
matches the files in git.

Updating a view
---------------
1. Edit on rovdev (or export from prod).
2. Views UI -> export the view.
3. Replace views/<machine_name>.inc (keep the <?php header and @file docblock).
4. Deploy code, then revert overrides (UI or migration).

Re-import (clear DB overrides for this module's Views)
------------------------------------------------------
Use drush74 on Rovas servers.

  drush74 php-eval "module_load_include('inc', 'rovas_site_views', 'rovas_site_views.views_migration'); rovas_site_views_views_migration_run();"

What it does:
  - Deletes {views_view} / {views_display} rows for Views owned by this module
  - Clears Views caches so code defaults load again
  - Does not modify other modules' Views

Enable
------
This module is new. After deploy:

  drush74 en rovas_site_views -y
  drush74 cc all

Then run the migration (or Revert each view in the UI) so DB copies do not
override the code defaults.

See also README.views.txt and docs/views-inventory.md.
