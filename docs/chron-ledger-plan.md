# Plan: Chron ledger vs balance

Draft for fine-tuning. Context: clawbacks (and other balance mutators) update
`field_chrons_balance` without a ledger row, so `/user/xxx/chrons`
(`money_transfers` view) drifts from the stated Chron balance.

## Problem

Drift goes both ways:

1. **Balance changes with no ledger row** — `field_chrons_balance` (Civic
   `total_chr`, spending checks) is updated by `rules_update_user_chron_account`
   with **no ledger row**. Clawbacks, WR verification grants, WR delete, trusted
   auto-approve, and other direct callers never appear on `/user/xxx/chrons`
   → view total drifts from balance.
2. **Ledger rows that are not Chron** — `money_transfers` rebuilds history from
   heterogeneous node types. Every positive `purchase_receipt` is shown as a
   Chron **purchase debit** (`−X CH`), but Civic Merit-only issuers reuse
   `purchase_receipt` as Merit paper trail with `field_amount_paid` = **Merit
   units**, and never call the Chron mutator. Result: VAT claims (and Path-1
   EUR gifts via the same issuer) appear to subtract Chrons on the Chron tab
   even though `field_chrons_balance` is unchanged. The false debit amount
   matches Merits awarded, not EUR/VAT.

   **Preferred fix (aligned with B = `chron_adjustment`):** keep the Merit
   `purchase_receipt` (Merit tab / Source = Project stays intact) and create a
   matching **ledger-only** `chron_adjustment` credit that counter-balances the
   Chron-tab purchase debit (same magnitude as `field_amount_paid`). Do **not**
   call `rules_update_user_chron_account` for these offsets — wallet is already
   correct. Reason/source e.g. `merit_receipt_offset` with optional ref to the
   receipt. Same pattern for Path-1 gift receipts from
   `rovas_civic_issue_donor_merits`. Wire `chron_adjustment` into
   `money_transfers.inc` as a credit (and later for real balance mutators too).

   Caveats: Chron tab shows two canceling lines (noisier than filtering the
   receipt out). Do not combine offset **and** exclude the same receipt from the
   Chron view. Filtering Merit-only receipts remains a valid alternative if UX
   prefers one fewer row; offset is preferred because it reuses the planned
   adjustment type and does not special-case Civic receipts in the view.

- Legacy `rules_chrons_batch_adjuster` treats the **view sum as truth** and can
  overwrite balance — opposite of “balance is authoritative.” Pollution from
  (2) makes that overwrite especially dangerous if the adjuster is still run.

## Goals (to confirm)

1. Every chron delta that changes user balance has an auditable ledger line.
2. Profile chron tab (and later Civic) can reconcile to `field_chrons_balance`.
3. Clawback paths stay idempotent (no double debit / double ledger row).

## Non-goals (initial)

- Full double-entry accounting.
- Civic chron-history API (follow-on).
- Perfect reconstruction of all historical gaps before cutover (unless we choose
  backfill).

## Decision points (fine-tune next)

| # | Choice | Candidates |
|---|--------|------------|
| A | Scope | Clawback-only → grant+clawback → all `rules_update_user_chron_account` callers |
| B | Record type | Reuse `money_transfer` vs new `chron_adjustment` (or similar) |
| C | Where to write | Call sites only vs inside central mutator (`rules_update_user_chron_account` / PHP helper) |
| D | Source of truth | Balance cached + ledger append-only vs view-inferred |
| E | Counterparty | uid 1 / system user / note-only |
| F | History | Opening-balance row at deploy vs backfill from WR + known transfers |
| G | Batch adjuster | Disable, invert (ledger→balance), or leave until ledger exists |

**Suggested default for next discussion:** B = new `chron_adjustment` type;
C = centralize at mutator for real Chron deltas, plus **ledger-only** writes for
Merit-receipt offsets (no balance change); A = start with grant+clawback then
expand; D = balance = cache of ledger. Merit-pollution offsets are an early
consumer of B even before every mutator is centralized.

## Phased approach (deploy / review between ships)

Prefer shipping and reviewing each ship on rovdev before the next.

### Phase 0 — Inventory (read-only)

- Read in-repo Chron views under `rovas_money_transfer/views/` (primary:
  `money_transfers.inc`); list bundles + PHP amount logic — especially the
  `purchase_receipt` → debit/credit branch. No DB export or Views UI pass
  required for inventory.
- Map every `rules_update_user_chron_account` caller → already has node / needs
  row / synthetic.
- Map every `purchase_receipt` creator → Chron spend (`rules_process_purchase_core`)
  vs Merit-only paper trail (`rovas_civic_issue_donor_merits` and any peers).
- Document clawback/grant guards already in Rules (status was `1`, etc.).

### Ship 1 — Merit-receipt Chron-tab offsets (**verified on rovdev 2026-10-14**)

Scope: fix VAT / Path-1 false Chron debits only. No WR grant/clawback yet.

- New module `rovas_chron`: `chron_adjustment` type +
  `rovas_chron_offset_merit_receipt()` (ledger-only; never touches
  `field_chrons_balance`).
- `rovas_civic_issue_donor_merits` creates the offset after the Merit receipt.
- `money_transfers.inc` includes `chron_adjustment` on all displays.
- Drush: `rovas-chron-backfill-merit-offsets` (+ `--dry-run` / `--uid`).
- Deploy notes: `sites/all/modules/custom/rovas_chron/README.txt`.

**Review gate (passed):** offset records created in the correct amounts against
the correct purchase receipts; Chron wallet unchanged by design.

### Ship 2 — WR grant / clawback ledger rows (**implemented in code; awaiting deploy**)

- `rovas_chron_apply_wr_delta()`: chron_adjustment **then** wallet mutator;
  idempotent on `field_chron_ikey` (no double wallet hit).
- Sources: `wr_grant`, `wr_clawback`; ER `field_parent_work_report`.
- Wired: peer verify grant/deny, auto-approve, system WR grant, verification
  delete clawbacks (3 arms), delete approved WR (`ledger_only_if_grant` so
  legacy WRs without `wr_grant` do not double-debit the Chron tab).
- Chron view: when a WR has a `wr_grant` adjustment, the `work_report` row is
  treated as noop (0 CH) so grants are not double-counted.
- Rules JSON changed — re-import via `rovas_site_rules` migration after deploy.
- Optional: authoritative balance / drift banner deferred.

**Review gate:** approve → wallet +CH + `wr_grant` line; deny/del-verif while
approved → wallet −CH + `wr_clawback`; WR row shows 0 when grant exists;
delete approved WR without prior `wr_grant` → wallet −CH, no extra Chron debit;
idempotent re-fire of same ikey does not double-mutate.

### Ship 3 / Phase 2 — Central chron ledger (after Ship 2)

- Expand `rovas_chron.ledger.inc` as the one front door for all real Chron
  deltas (balance mutator + ledger row).
- Extend `field_chron_source` values; optional WR/transfer refs as needed.
- Keep Chron UI views in `rovas_money_transfer/views/*.inc`; revise in repo.
- Reconcile Drush (wallet vs ledger sum).
- Retire or invert `rules_chrons_batch_adjuster`.

### Phase 3 — Polish / Civic

- Opening balance or backfill strategy.
- Optional `GET /rovas/civic_chron_history`.
- Clear UX copy: full chron history vs “transfers & purchases” only.

## Risks / constraints

- `rules_make_money_transfer` **already updates balance** — do not stack with
  `rules_update_user_chron_account` without `skip_charge` / split “record vs
  mutate.”
- Clawback in both `rules_deleting_verification` and deny branch of
  `rules_verify_work_report` — ledger must use the same guards as balance.
- Dual-use `purchase_receipt`: Chron tab and Merit tab both read it. Do not
  remove receipt create in the Civic issuer (Merit Source = Project). Preferred
  Chron fix: ledger-only `chron_adjustment` offset (see Problem §2). Offset
  rows must never touch `field_chrons_balance`. Do not also filter the same
  receipts out of `money_transfers` or the Chron sum double-credits.
- Chron views are code-owned (`docs/views-inventory.md`,
  `rovas_money_transfer/README.views.txt`). Target `views/*.inc` in git; after
  deploy, clear DB overrides so defaults load. Phase 0 still needs the
  caller/creator maps above.
- Deploy / Rules re-import / views-migration stays human-owned (deploy-owner
  rule).

## Open questions for the next discussion

1. After Ship 1 review: proceed to Ship 2 (WR grant/clawback), or jump to
   Ship 3 (central mutator)?
2. ~~New content type vs overload `money_transfer`?~~ → **decided:** `chron_adjustment`
   (`rovas_chron`).
3. Must historical clawbacks/grants be backfilled, or ledger starts at cutover?
4. Should profile UI keep reconstructing from heterogeneous nodes, or move to a
   dedicated chron ledger view (still as a `views/*.inc`)?
5. ~~Merit-only Chron pollution strategy~~ → **decided for Ship 1:** ledger-only
   `chron_adjustment` offset (do not also filter those receipts from the Chron
   view).

## Related code (anchors)

| Role | Location |
|------|----------|
| Balance mutator | `rules_update_user_chron_account` |
| Clawback on verification delete | `rules_deleting_verification` |
| Clawback on deny-while-approved | deny path in `rules_verify_work_report` |
| Transfer + node | `rules_make_money_transfer` |
| Real Chron purchase (debit + receipt) | `rules_process_purchase_core` |
| Merit-only receipt (no Chron debit) | `rovas_civic_issue_donor_merits` (VAT claim + Path-1 gifts) |
| Merit receipt Chron offset (Ship 1) | `rovas_chron` — `rovas_chron_offset_merit_receipt()` |
| Chron tab / purchase debit + adjustment | `rovas_money_transfer/views/money_transfers.inc` |
| Chron views (code defaults) | `rovas_money_transfer/views/*.inc`, `README.views.txt`, `docs/views-inventory.md` |
| Drift / batch adjuster | `rules_chrons_batch_adjuster` |
| Merit parallel | `rovas_merit.ledger.inc`, `merits_disbursement`, `merit_history` |

## How to use this doc

Ship 1 is coded and waiting on deploy/review. Then Ship 2 → Ship 3.
View work lands in git (`views/*.inc`); human deploys code and reverts
overrides — no Rovas admin Views UI required to revise the Chron tab.
