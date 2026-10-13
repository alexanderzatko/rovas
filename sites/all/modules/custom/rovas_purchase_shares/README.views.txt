Rovas Purchase Shares Views
===========================

This module provides default Views for Rovas. Each view is a PHP export in
views/<machine_name>.inc, loaded via hook_views_default_views().

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

  drush74 php-eval "module_load_include('inc', 'rovas_purchase_shares', 'rovas_purchase_shares.views_migration'); rovas_purchase_shares_views_migration_run();"

What it does:
  - Deletes {views_view} / {views_display} rows for Views owned by this module
  - Clears Views caches so code defaults load again
  - Does not modify other modules' Views

Owned Views
-----------
- labor_reported_chart
- project_coworkers
- project_income_statements
- project_merit_rewards_chart
- project_project_shareholder
- project_user_shareholder
- project_user_time_shareholder
- projects_with_allocation_thresholds

See docs/views-inventory.md for the full site inventory and ownership map.
