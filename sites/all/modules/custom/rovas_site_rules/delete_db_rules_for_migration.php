<?php
/**
 * @file
 * One-off snippet: delete from DB all rules that rovas_site_rules provides,
 * then re-create them from the module's default configs.
 *
 * Uses Batch API when run from the web (Devel > Execute PHP) to avoid lock
 * timeouts. When run via Drush CLI, executes directly.
 *
 * Usage:
 *   - Devel Execute PHP: paste and run the line below.
 *   - Drush: drush php-eval "module_load_include('inc', 'rovas_site_rules', 'rovas_site_rules.migration'); rovas_site_rules_migration_run();"
 *   - Or: drush php-script delete_db_rules_for_migration.php (if wrapped in bootstrap)
 */

module_load_include('inc', 'rovas_site_rules', 'rovas_site_rules.migration');
rovas_site_rules_migration_run();
