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

Revert Chron view overrides so money_transfers.inc wins:

  drush74 php-eval "module_load_include('inc', 'rovas_money_transfer', 'rovas_money_transfer.views_migration'); rovas_money_transfer_views_migration_run();"

Optional backfill for existing Merit-only receipts:

  drush74 rovas-chron-backfill-merit-offsets --dry-run
  drush74 rovas-chron-backfill-merit-offsets

Verify
------
1. Issue a VAT claim (or Path-1 gift).
2. field_chrons_balance unchanged.
3. /user/<uid>/chrons shows purchase debit and matching "adjustment" credit.
4. /user/<uid>/merits still shows the Merit credit.
5. Re-issue same source_id → no duplicate receipt/offset.
