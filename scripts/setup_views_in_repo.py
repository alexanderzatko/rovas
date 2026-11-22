#!/usr/bin/env python3
"""Place exported Views into domain modules and scaffold hooks/helpers."""
from __future__ import print_function

import os
import shutil
import textwrap

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
SRC = os.path.join(ROOT, 'tmp', 'rovas_views_export')
CUSTOM = os.path.join(ROOT, 'sites', 'all', 'modules', 'custom')

# view machine name -> owner module
OWNERSHIP = {
    # rovas_money_transfer
    'money_transfers': 'rovas_money_transfer',
    'chron_balance': 'rovas_money_transfer',
    'chron_holders_list': 'rovas_money_transfer',
    'total_chrons': 'rovas_money_transfer',
    # rovas_merit
    'merit_history': 'rovas_merit',
    'total_merits': 'rovas_merit',
    'users_merit_rank': 'rovas_merit',
    'usr_merit_rank': 'rovas_merit',
    'projects_merits': 'rovas_merit',
    'total_project_merits': 'rovas_merit',
    'employee_projects': 'rovas_merit',
    'worker_merit_allocations_for_project': 'rovas_merit',
    # rovas_forex
    'forex_chart': 'rovas_forex',
    'forex_chart_chron_baskets': 'rovas_forex',
    'forex_transactions': 'rovas_forex',
    # rovas_verify
    'users_reports': 'rovas_verify',
    'pending_verifications': 'rovas_verify',
    'select_wr_verifier': 'rovas_verify',
    'find_slot_for_verifiaction': 'rovas_verify',
    'verif_slots': 'rovas_verify',
    'find_delinquent_verify_slots': 'rovas_verify',
    'unverified_work_reports': 'rovas_verify',
    'unverified_reports': 'rovas_verify',
    'report_slots': 'rovas_verify',
    'reference_reports': 'rovas_verify',
    'report_exists_check': 'rovas_verify',
    'work_report_duplicates': 'rovas_verify',
    # rovas_report_form
    'work_reports': 'rovas_report_form',
    'other_workers_work_reports': 'rovas_report_form',
    'wr_time_line': 'rovas_report_form',
    # rovas_purchase_shares
    'project_user_shareholder': 'rovas_purchase_shares',
    'project_project_shareholder': 'rovas_purchase_shares',
    'labor_reported_chart': 'rovas_purchase_shares',
    'project_income_statements': 'rovas_purchase_shares',
    'projects_with_allocation_thresholds': 'rovas_purchase_shares',
    # rovas_project_reward
    'project_merits_feed': 'rovas_project_reward',
    'purchases': 'rovas_project_reward',
    'investment': 'rovas_project_reward',
    'sugar_daddy': 'rovas_project_reward',
    # rovas_project_edit
    'projects': 'rovas_project_edit',
    'expired_projects': 'rovas_project_edit',
    # rovas_commerce_order
    'users_pending_orders': 'rovas_commerce_order',
    'supporters_countries': 'rovas_commerce_order',
    'commerce_cart_summary': 'rovas_commerce_order',
    'commerce_line_item_table': 'rovas_commerce_order',
    'commerce_orders': 'rovas_commerce_order',
    'commerce_user_orders': 'rovas_commerce_order',
    # rovas_user
    'users': 'rovas_user',
    'user_emails': 'rovas_user',
    'users_stats': 'rovas_user',
    'users_by_activity': 'rovas_user',
    'users_with_low_compliance_score': 'rovas_user',
    'top_compliant_users': 'rovas_user',
    'users_autocomplete': 'rovas_user',
    'Mentions': 'rovas_user',
    # rovas_nocat_rank
    'top_10_workers_in_a_category': 'rovas_nocat_rank',
    # rovas_site_views (orphans)
    'disputes': 'rovas_site_views',
    'dispute_parties': 'rovas_site_views',
    'disputed_nodes': 'rovas_site_views',
    'disputes_without_object': 'rovas_site_views',
    'all_comment_authors': 'rovas_site_views',
    'detritus_deleter': 'rovas_site_views',
    'test': 'rovas_site_views',
    'updater': 'rovas_site_views',
    'user_updater': 'rovas_site_views',
    'taxonomy_term': 'rovas_site_views',
    'uc_catalog': 'rovas_site_views',
    'uc_orders_user': 'rovas_site_views',
}

ALREADY_MANAGED = {
    'forex_purchases': 'rovas_forex',
    'project_coworkers': 'rovas_purchase_shares',
    'project_merit_rewards_chart': 'rovas_purchase_shares',
    'project_user_time_shareholder': 'rovas_purchase_shares',
}

SKIP_CONTRIB_DEFAULT = [
    'archive', 'charts_demo', 'comments_recent', 'commerce_addressbook',
    'commerce_addressbook_defaults', 'commerce_cart_block', 'commerce_cart_form',
    'commerce_customer_profiles', 'commerce_order_revisions', 'commerce_payment_order',
    'commerce_product_revisions', 'commerce_products', 'frontpage', 'glossary',
    'nodes_autocomplete', 'popular', 'rules_scheduler', 'tracker', 'variable',
    'words_autocomplete',
]

VIEWS_DEFAULT_LOADER = '''<?php

/**
 * @file
 * Default Views for {module} (one file per view under views/).
 */

/**
 * Implements hook_views_default_views().
 */
function {module}_views_default_views() {{
  return _{module}_views_load_default_views();
}}

/**
 * Load exported Views from views/*.inc.
 *
 * Each file defines $view (Views UI export). Returns keyed by machine name.
 */
function _{module}_views_load_default_views() {{
  $export = array();
  $path = drupal_get_path('module', '{module}') . '/views';
  if (!is_dir($path)) {{
    return $export;
  }}
  foreach (glob($path . '/*.inc') as $file) {{
    $view = NULL;
    $handler = NULL;
    include $file;
    if (isset($view) && is_object($view) && !empty($view->name)) {{
      $export[$view->name] = $view;
    }}
    unset($view, $handler, $translatables);
  }}
  return $export;
}}
'''

MIGRATION_INC = '''<?php

/**
 * @file
 * Revert DB overrides for Views owned by {module}.
 *
 * After deploy, run:
 *   drush74 php-eval "module_load_include('inc', '{module}', '{module}.views_migration'); {module}_views_migration_run();"
 */

/**
 * Delete {{views_view}} / {{views_display}} rows for this module's default Views.
 *
 * Code defaults then apply after Views cache clear. Does not touch other modules.
 */
function {module}_views_migration_run() {{
  module_load_include('inc', '{module}', '{module}.views_default');
  if (!function_exists('_{module}_views_load_default_views')) {{
    drupal_set_message(t('{module}: views loader missing.'), 'error');
    return;
  }}
  $names = array_keys(_{module}_views_load_default_views());
{extra_names}
  if (empty($names)) {{
    drupal_set_message(t('{module}: no default Views to migrate.'), 'warning');
    return;
  }}

  $vids = db_select('views_view', 'v')
    ->fields('v', array('vid'))
    ->condition('name', $names, 'IN')
    ->execute()
    ->fetchCol();

  $removed = 0;
  if (!empty($vids)) {{
    $removed = db_delete('views_display')
      ->condition('vid', $vids, 'IN')
      ->execute();
    db_delete('views_view')
      ->condition('vid', $vids, 'IN')
      ->execute();
  }}

  if (function_exists('views_invalidate_cache')) {{
    views_invalidate_cache();
  }}
  cache_clear_all('*', 'cache_views', TRUE);
  cache_clear_all('*', 'cache_views_data', TRUE);

  $msg = t('{module}: removed overrides for @count view(s) (@names). Cleared Views caches.', array(
    '@count' => count($names),
    '@names' => implode(', ', $names),
  ));
  if (drupal_is_cli()) {{
    print $msg . "\\n";
    print t('Display rows deleted: @n', array('@n' => (int) $removed)) . "\\n";
  }}
  else {{
    drupal_set_message($msg);
  }}
}}
'''

def build_readme(title, module, owned_list):
    bar = '=' * len(title)
    return '''{title}
{bar}

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

  drush74 php-eval "module_load_include('inc', '{module}', '{module}.views_migration'); {module}_views_migration_run();"

What it does:
  - Deletes {{views_view}} / {{views_display}} rows for Views owned by this module
  - Clears Views caches so code defaults load again
  - Does not modify other modules' Views

Owned Views
-----------
{owned_list}

See docs/views-inventory.md for the full site inventory and ownership map.
'''.format(title=title, bar=bar, module=module, owned_list=owned_list)


def ensure_views_dep(info_path):
    with open(info_path) as f:
        content = f.read()
    if 'dependencies[] = views\n' in content or 'dependencies[] = views\r\n' in content:
        return
    if not content.endswith('\n'):
        content += '\n'
    content += 'dependencies[] = views\n'
    with open(info_path, 'w') as f:
        f.write(content)


def ensure_views_api_in_module(module_path, module):
    marker = 'function {0}_views_api('.format(module)
    # Prefer existing hook in .module or included .inc files.
    for root, _dirs, files in os.walk(module_path):
        for fname in files:
            if not fname.endswith(('.module', '.inc', '.php')):
                continue
            path = os.path.join(root, fname)
            # Skip generated Views export files under views/.
            if os.path.basename(root) == 'views':
                continue
            with open(path) as f:
                if marker in f.read():
                    return
    module_file = os.path.join(module_path, module + '.module')
    if not os.path.isfile(module_file):
        with open(module_file, 'w') as f:
            f.write("<?php\n\n/**\n * @file\n * {0} module.\n */\n\n".format(module))
    with open(module_file) as f:
        content = f.read()
    snippet = textwrap.dedent('''

    /**
     * Implements hook_views_api().
     */
    function {module}_views_api($module = NULL, $api = NULL) {{
      return array('api' => '3.0');
    }}
    ''').format(module=module)
    if not content.endswith('\n'):
        content += '\n'
    content += snippet
    with open(module_file, 'w') as f:
        f.write(content)


def write_loader(module_path, module, extra_merge_fn=None):
    path = os.path.join(module_path, module + '.views_default.inc')
    if extra_merge_fn:
        # Caller handles special cases (forex / purchase_shares).
        return
    with open(path, 'w') as f:
        f.write(VIEWS_DEFAULT_LOADER.format(module=module))


def write_migration(module_path, module, extra_names=None):
    extra = ''
    if extra_names:
        lines = ["  $names = array_values(array_unique(array_merge($names, array("]
        for n in extra_names:
            lines.append("    '{0}',".format(n))
        lines.append('  ))));')
        extra = '\n'.join(lines) + '\n'
    path = os.path.join(module_path, module + '.views_migration.inc')
    with open(path, 'w') as f:
        f.write(MIGRATION_INC.format(module=module, extra_names=extra))


def write_readme(module_path, module, views):
    title = module.replace('_', ' ').title() + ' Views'
    owned = '\n'.join('- ' + v for v in sorted(views))
    path = os.path.join(module_path, 'README.views.txt')
    with open(path, 'w') as f:
        f.write(build_readme(title, module, owned))


def create_site_views_module():
    module = 'rovas_site_views'
    path = os.path.join(CUSTOM, module)
    os.makedirs(path, exist_ok=True)
    os.makedirs(os.path.join(path, 'views'), exist_ok=True)
    info = textwrap.dedent('''\
        name = Rovas Site Views
        description = Catch-all default Views that do not belong to a domain module. One PHP export per file under views/.
        core = 7.x
        package = Rovas

        dependencies[] = views
        ''')
    with open(os.path.join(path, module + '.info'), 'w') as f:
        f.write(info)
    with open(os.path.join(path, module + '.module'), 'w') as f:
        f.write(textwrap.dedent('''\
            <?php

            /**
             * @file
             * Catch-all site Views defaults (orphans not owned by a domain module).
             */

            /**
             * Implements hook_views_api().
             */
            function rovas_site_views_views_api($module = NULL, $api = NULL) {
              return array('api' => '3.0');
            }
            '''))
    write_loader(path, module)
    return path


def patch_existing_views_default(module, keep_inline_names):
    """For forex/purchase_shares: keep inline exports, also load views/*.inc."""
    path = os.path.join(CUSTOM, module, module + '.views_default.inc')
    with open(path) as f:
        content = f.read()
    if '_{0}_views_load_default_views'.format(module) in content:
        return
    # Replace final `return $export;` with merge + return.
    old = '  return $export;\n}'
    # Find last occurrence
    idx = content.rfind(old)
    if idx < 0:
        raise SystemExit('Could not patch {0}'.format(path))
    loader_fn = textwrap.dedent('''

    /**
     * Load additional exported Views from views/*.inc.
     */
    function _{module}_views_load_default_views() {{
      $export = array();
      $path = drupal_get_path('module', '{module}') . '/views';
      if (!is_dir($path)) {{
        return $export;
      }}
      foreach (glob($path . '/*.inc') as $file) {{
        $view = NULL;
        $handler = NULL;
        include $file;
        if (isset($view) && is_object($view) && !empty($view->name)) {{
          $export[$view->name] = $view;
        }}
        unset($view, $handler, $translatables);
      }}
      return $export;
    }}
    ''').format(module=module)
    new_return = (
        '  $export += _{module}_views_load_default_views();\n'
        '  return $export;\n'
        '}}\n'
    ).format(module=module)
    content = content[:idx] + new_return + loader_fn
    # Avoid double closing if we replaced `}` already in new_return
    with open(path, 'w') as f:
        f.write(content)


def main():
    if not os.path.isdir(SRC):
        raise SystemExit('Missing exports at {0}'.format(SRC))

    create_site_views_module()

    by_module = {}
    for name, module in OWNERSHIP.items():
        by_module.setdefault(module, []).append(name)

    missing = []
    for name, module in sorted(OWNERSHIP.items()):
        src = os.path.join(SRC, name + '.inc')
        if not os.path.isfile(src):
            missing.append(name)
            continue
        dest_dir = os.path.join(CUSTOM, module, 'views')
        os.makedirs(dest_dir, exist_ok=True)
        shutil.copy2(src, os.path.join(dest_dir, name + '.inc'))

    if missing:
        raise SystemExit('Missing export files: {0}'.format(', '.join(missing)))

    for module, views in sorted(by_module.items()):
        module_path = os.path.join(CUSTOM, module)
        info_path = os.path.join(module_path, module + '.info')
        ensure_views_dep(info_path)
        ensure_views_api_in_module(module_path, module)

        if module in ('rovas_forex', 'rovas_purchase_shares'):
            patch_existing_views_default(module, ALREADY_MANAGED)
            # Migration only clears views/*.inc; leave inline legacy exports alone.
            write_migration(module_path, module)
            extra = [n for n, m in ALREADY_MANAGED.items() if m == module]
            all_views = sorted(set(views) | set(extra))
        else:
            write_loader(module_path, module)
            write_migration(module_path, module)
            all_views = sorted(views)

        write_readme(module_path, module, all_views)

    print('Placed {0} views into {1} modules.'.format(len(OWNERSHIP), len(by_module)))
    for module, views in sorted(by_module.items()):
        print('  {0}: {1}'.format(module, len(views)))


if __name__ == '__main__':
    main()
