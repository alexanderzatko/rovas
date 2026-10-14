# Views inventory (production Rovas)

Source: `/var/www/rovas` via `drush74` on 2026-10-13. Storage is Views UI
status: **Normal** = DB-only site view; **Default** = code default;
**Overridden** = code default with DB edits.

## Already managed in git (skip re-home)

| Machine name | Module | Storage |
|--------------|--------|---------|
| `forex_purchases` | `rovas_forex` | Default |
| `project_coworkers` | `rovas_purchase_shares` | Overridden |
| `project_merit_rewards_chart` | `rovas_purchase_shares` | Overridden |
| `project_user_time_shareholder` | `rovas_purchase_shares` | Default |

Left in place (not moved to catch-all). Additional Views for those modules
live under `views/*.inc`. Their `*_views_migration_run()` helpers only revert
the new `views/*.inc` exports — not these four — so prod Overridden copies are
not wiped by a bulk migrate.

## Contrib defaults (skip — not customized)

`archive`, `charts_demo`, `comments_recent`, `commerce_addressbook`,
`commerce_addressbook_defaults`, `commerce_cart_block`, `commerce_cart_form`,
`commerce_customer_profiles`, `commerce_order_revisions`,
`commerce_payment_order`, `commerce_product_revisions`, `commerce_products`,
`frontpage`, `glossary`, `nodes_autocomplete`, `popular`, `rules_scheduler`,
`tracker`, `variable`, `words_autocomplete`

## Ownership map (exported into repo)

### `rovas_money_transfer`

| View | Storage | Paths |
|------|---------|-------|
| `money_transfers` | Normal | `user/%/chrons`, `user/%/sumchrons` (includes `chron_adjustment` as of Ship 1) |
| `chron_balance` | Normal | |
| `chron_holders_list` | Normal | `chron-holders-list` |
| `total_chrons` | Normal | `total-chrons` |

Related: `rovas_chron` owns `chron_adjustment` nodes (Merit receipt offsets;
see `docs/chron-ledger-plan.md`).

### `rovas_merit`

| View | Storage | Paths |
|------|---------|-------|
| `merit_history` | Normal | `user/%/merits` |
| `total_merits` | Normal | `total-merits-history` |
| `users_merit_rank` | Normal | `merit_index` |
| `usr_merit_rank` | Normal | |
| `projects_merits` | Normal | `rest_view_project_merits` |
| `total_project_merits` | Normal | |
| `employee_projects` | Normal | |
| `worker_merit_allocations_for_project` | Normal | |

### `rovas_forex`

| View | Storage | Paths |
|------|---------|-------|
| `forex_chart` | Normal | `forex-chart` |
| `forex_chart_chron_baskets` | Normal | `forex-chart-chr-baskets` |
| `forex_transactions` | Normal | `forex-transactions` |

(Plus existing `forex_purchases`.)

### `rovas_verify`

| View | Storage | Paths |
|------|---------|-------|
| `users_reports` | Normal | |
| `pending_verifications` | Normal | |
| `select_wr_verifier` | Normal | |
| `find_slot_for_verifiaction` | Normal | |
| `verif_slots` | Normal | `verif-slots` |
| `find_delinquent_verify_slots` | Normal | `tobe-delinquent` |
| `unverified_work_reports` | Normal | |
| `unverified_reports` | Normal | `unverified-reports` |
| `report_slots` | Normal | |
| `reference_reports` | Normal | |
| `report_exists_check` | Normal | |
| `work_report_duplicates` | Normal | |

### `rovas_report_form`

| View | Storage | Paths |
|------|---------|-------|
| `work_reports` | Normal | |
| `other_workers_work_reports` | Normal | |
| `wr_time_line` | Normal | `wr-time-line-table` |

### `rovas_purchase_shares`

| View | Storage | Paths |
|------|---------|-------|
| `project_user_shareholder` | Normal | |
| `project_project_shareholder` | Normal | |
| `labor_reported_chart` | Normal | |
| `project_income_statements` | Normal | `project-income-statements` |
| `projects_with_allocation_thresholds` | Normal | |

(Plus existing three Views in `*.views_default.inc`.)

### `rovas_project_reward`

| View | Storage | Paths |
|------|---------|-------|
| `project_merits_feed` | Normal | `project-purchases/%`, `project-payments`, `rest_view_project_buyers` |
| `purchases` | Normal | |
| `investment` | Normal | `invest` |
| `sugar_daddy` | Normal | `sugar-daddy` |

### `rovas_project_edit`

| View | Storage | Paths |
|------|---------|-------|
| `projects` | Normal | `find_project` |
| `expired_projects` | Normal | |

### `rovas_commerce_order`

| View | Storage | Paths |
|------|---------|-------|
| `users_pending_orders` | Normal | |
| `supporters_countries` | Normal | `supporters-countries` |
| `freemap_countries` | Normal | `freemap-countries` |
| `commerce_cart_summary` | Overridden (commerce_cart) | |
| `commerce_line_item_table` | Overridden (commerce_line_item) | |
| `commerce_orders` | Overridden (commerce_order_ui) | `admin/commerce/orders/list`, carts |
| `commerce_user_orders` | Overridden (commerce_order_ui) | `user/%/orders` |

Site-owned Commerce overrides are exported here so git wins after revert.

### `rovas_user`

| View | Storage | Paths |
|------|---------|-------|
| `users` | Normal | |
| `user_emails` | Normal | |
| `users_stats` | Normal | `users-stats` |
| `users_by_activity` | Normal | |
| `users_with_low_compliance_score` | Normal | |
| `top_compliant_users` | Normal | |
| `user_countries` | Normal | `user-countries` |
| `users_autocomplete` | Overridden (search_autocomplete) | `users-autocomplete` |
| `Mentions` | Overridden (mentions) | `user/%/mentions` |

### `rovas_nocat_rank`

| View | Storage | Paths |
|------|---------|-------|
| `top_10_workers_in_a_category` | Normal | `top-10-workers-in-a-category`, REST |

### `rovas_site_views` (orphans / catch-all)

| View | Storage | Paths | Notes |
|------|---------|-------|-------|
| `disputes` | Normal | `disputes` | No dispute domain module |
| `dispute_parties` | Normal | | |
| `disputed_nodes` | Normal | | |
| `disputes_without_object` | Normal | | |
| `all_comment_authors` | Normal | | |
| `detritus_deleter` | Normal | `detritus-deleter` | Ops |
| `test` | Normal | | Ops / QA |
| `updater` | Normal | `updater` | Ops |
| `user_updater` | Normal | `user-updater` | Ops |
| `taxonomy_term` | Overridden (taxonomy) | `taxonomy/term/%` | Site override |
| `uc_catalog` | Normal | `_catalog` | Legacy Ubercart |
| `uc_orders_user` | Normal | `user/%/orders` | Legacy Ubercart |

## File layout

```
sites/all/modules/custom/<module>/
  views/<machine_name>.inc
  <module>.views_default.inc
  <module>.views_migration.inc
  README.views.txt
```

Workflow and Drush re-import: see each module’s `README.views.txt` (and
`rovas_site_views/README.txt`).

## Deploy handoff (you run on the server)

1. Deploy code (rsync / usual process).
2. Enable the new catch-all module:

   ```bash
   drush74 en rovas_site_views -y
   drush74 cc all
   ```

   Domain modules that already were enabled pick up new `hook_views_api` /
   `views/*.inc` after cache clear. Confirm `views` is a dependency (already
   added in their `.info` files).

3. Revert DB overrides so code wins. Per module:

   ```bash
   drush74 php-eval "module_load_include('inc', 'MODULE', 'MODULE.views_migration'); MODULE_views_migration_run();"
   ```

   Replace `MODULE` with each of:

   - `rovas_money_transfer`
   - `rovas_merit`
   - `rovas_forex`
   - `rovas_verify`
   - `rovas_report_form`
   - `rovas_purchase_shares`
   - `rovas_project_reward`
   - `rovas_project_edit`
   - `rovas_commerce_order`
   - `rovas_user`
   - `rovas_nocat_rank`
   - `rovas_site_views`

   Or Revert individual views under Structure → Views.

4. Spot-check critical paths: `/user/1/chrons` (`money_transfers`),
   `/user/1/merits` (`merit_history`), project find (`projects`), disputes.

Ownership decisions that were ambiguous are recorded in the map above
(`users_reports` → `rovas_verify`; Commerce overrides → `rovas_commerce_order`;
disputes / ops / legacy → `rovas_site_views`).