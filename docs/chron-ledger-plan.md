# Plan: Chron ledger vs balance

Draft for fine-tuning. Context: clawbacks (and other balance mutators) update
`field_chrons_balance` without a ledger row, so `/user/xxx/chrons`
(`money_transfers` view) drifts from the stated Chron balance.

## Problem

- **Balance:** `field_chrons_balance` on the user (Civic `total_chr`, spending
  checks) is updated by `rules_update_user_chron_account` with **no ledger row**.
- **UI:** `money_transfers` on `/user/xxx/chrons` rebuilds history from several
  node types (`money_transfer`, purchase/forex-related records, etc.) with PHP
  fields for signed amounts. Clawbacks (and WR verification grants) never appear
  → view total drifts from balance.
- Same gap beyond clawback: verification grants, WR delete, trusted auto-approve,
  and other direct callers of `rules_update_user_chron_account`.
- Legacy `rules_chrons_batch_adjuster` treats the **view sum as truth** and can
  overwrite balance — opposite of “balance is authoritative.”

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

**Suggested default for next discussion:** B = new type; C = centralize at
mutator; A = start with grant+clawback then expand; D = balance = cache of ledger.

## Phased approach

### Phase 0 — Inventory (read-only)

- Export `money_transfers` from DB (not in git today); list bundles + PHP amount
  logic.
- Map every `rules_update_user_chron_account` caller → already has node / needs
  row / synthetic.
- Document clawback/grant guards already in Rules (status was `1`, etc.).

### Phase 1 — Minimal product fix (optional fast path)

- On WR chron **grant** and **clawback**, write an explicit ledger node + keep
  balance update.
- Extend `money_transfers` (or sibling display) to include those rows.
- Guard against double writes on delete-then-deny paths.
- Show authoritative balance at top of chron tab; optional drift banner.

### Phase 2 — Central chron ledger (preferred medium term)

- PHP helper (Merit-style: e.g. `rovas_chron.ledger.inc`) invoked from one front
  door.
- Fields: signed amount, reason/source, optional entity ref (WR, transfer,
  receipt), timestamp.
- Export chron view into repo; reconcile Drush (wallet vs ledger sum).
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
- View lives in DB until exported — plan changes depend on Phase 0 inventory.
- Deploy / Rules re-import stays human-owned (deploy-owner rule).

## Open questions for the next discussion

1. Minimal (Phase 1) now, or go straight to central ledger (Phase 2)?
2. New content type vs overload `money_transfer`?
3. Must historical clawbacks/grants be backfilled, or ledger starts at cutover?
4. Should profile UI keep reconstructing from heterogeneous nodes, or move to a
   dedicated chron ledger view?

## Related code (anchors)

| Role | Location |
|------|----------|
| Balance mutator | `rules_update_user_chron_account` |
| Clawback on verification delete | `rules_deleting_verification` |
| Clawback on deny-while-approved | deny path in `rules_verify_work_report` |
| Transfer + node | `rules_make_money_transfer` |
| Drift / batch adjuster | `rules_chrons_batch_adjuster` |
| Merit parallel | `rovas_merit.ledger.inc`, `merits_disbursement`, `merit_history` |

## How to use this doc

Lock decisions **A–G** first, then implement Phase 0 → 1 or Phase 0 → 2.
Update this file when choices are finalized.
