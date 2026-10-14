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

(If that helper takes a component list, import only the four changed rules —
see rovas_site_rules/README.txt.)

Verify:
1. Peer-approve a WR: wallet +CH; Chron tab shows adjustment credit; WR row 0 CH.
2. Deny while approved: wallet −CH; clawback adjustment debit.
3. Delete verification while approved: same as deny.
4. Delete approved WR that never had wr_grant (legacy): wallet −CH; Chron tab
   does not gain an extra debit beyond losing the WR credit.
5. Trusted auto-approve: one wr_grant, one wallet credit.
6. Re-run same event (same ikey): no second wallet change.
