Rovas Chron Ledger
==================

Ship 1 (Merit receipt Chron-tab offsets)
----------------------------------------
Introduces content type chron_adjustment and a ledger-only create API.
Civic Merit issuers (VAT claims + Path-1 gifts) create a matching Chron-tab
credit that cancels the false purchase debit from purchase_receipt, without
changing field_chrons_balance.

See docs/chron-ledger-plan.md.

Deploy (human-owned)
--------------------
On rovdev (or prod when ready):

  drush74 en rovas_chron -y
  drush74 updb -y
  drush74 cc all

If Drush prints a service_container / Drupal::service() error after enable,
check watchdog: "rovas_chron module installed/enabled" means success; that
PHP error is a known secondary crash on this stack, not a failed install.
Confirm with: drush74 pm-info rovas_chron  (Status: enabled, schema 7001)

Revert Chron view overrides so money_transfers.inc wins (skip if already code
default — no row in {views_view} for money_transfers):

  drush74 php-eval "module_load_include('inc', 'rovas_money_transfer', 'rovas_money_transfer.views_migration'); rovas_money_transfer_views_migration_run();"

Optional backfill for existing Merit-only receipts:

  drush74 rovas-chron-backfill-merit-offsets --dry-run
  drush74 rovas-chron-backfill-merit-offsets

Verify (Ship 1)
---------------
1. Issue a VAT claim (or Path-1 gift).
2. field_chrons_balance unchanged.
3. /user/<uid>/chrons shows purchase debit and matching "adjustment" credit.
4. /user/<uid>/merits still shows the Merit credit.
5. Re-issue same source_id → no duplicate receipt/offset.

Ship 2 (WR grant / clawback)
----------------------------
After code deploy:

  drush74 updb -y
  drush74 cc all

Revert Chron view overrides if needed (same as Ship 1).

Re-import changed Rules (verify / deleting verification / delete WR / create WR):

  drush74 php-eval "module_load_include('inc', 'rovas_site_rules', 'rovas_site_rules.migration'); rovas_site_rules_migration_run();"

Verify:
1. Peer-approve a WR: wallet +CH; Chron tab shows adjustment credit; WR row 0 CH.
2. Deny while approved: wallet −CH; clawback adjustment debit.
3. Delete verification while approved: same as deny.
4. Delete approved WR that never had wr_grant (legacy): wallet −CH; Chron tab
   does not gain an extra debit beyond losing the WR credit.
5. Trusted auto-approve: one wr_grant, one wallet credit.
6. Re-run same event (same ikey): no second wallet change.

Ship 3 (central front door + safety)
------------------------------------
  drush74 cc all
  # Rules: batch adjuster disabled (re-import required)
  drush74 php-eval "module_load_include('inc', 'rovas_site_rules', 'rovas_site_rules.migration'); rovas_site_rules_migration_run();"
  # Chron views: sumchrons now includes purchase_merits
  drush74 php-eval "module_load_include('inc', 'rovas_money_transfer', 'rovas_money_transfer.views_migration'); rovas_money_transfer_views_migration_run();"

Report-only drift check (never overwrites balances):

  drush74 rovas-chron-reconcile --uid=YOUR_UID

Verify:
1. Invoking "chrons batch adjuster" only shows a warning; balances unchanged.
2. Ship 2 WR grant/clawback still works (uses rovas_chron_mutate under the hood).
3. /user/<uid>/sumchrons includes Merits-purchase rows when present.
4. reconcile reports wallet vs view sum without writing.

Phase 3 (opening balance + Chron-tab UX)
----------------------------------------
  drush74 updb -y
  drush74 cc all
  # Chron views: send-money CTA removed from Chron tab header
  drush74 php-eval "module_load_include('inc', 'rovas_money_transfer', 'rovas_money_transfer.views_migration'); rovas_money_transfer_views_migration_run();"

Cutover rows so Chron-tab sum matches wallet (ledger-only; never changes
field_chrons_balance). Idempotent on opening_balance:{uid}:

  drush74 rovas-chron-opening-balance --uid=YOUR_UID --dry-run
  drush74 rovas-chron-opening-balance --uid=YOUR_UID
  drush74 rovas-chron-reconcile --uid=YOUR_UID

Verify:
1. Chron tab shows an "adjustment" opening-balance row; wallet unchanged.
2. Re-run opening-balance for same uid: no second row.
3. reconcile |diff| within min-diff (default 0.01).
4. Send-money CTA is not shown on the Chron tab.

Import Chron ledger locale strings (sk/cs/it) after deploy, e.g.
sk.custom-modules.po (and cs/it peers), then clear caches.

Not in Phase 3: Civic chron history API; forex / fiat CHR wallet wiring.
