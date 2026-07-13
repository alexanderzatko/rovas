---
name: Forex Rate Mechanism
overview: Basket-based CHR/EUR exchange. Liability regime (ch_basket <= 0) uses symmetric Bancor on synthetic CHR depth and real EUR; bootstrap donor at flat minR. Bancor on real baskets when ch_basket > 0. minR ratchets up on worker CHR→EUR only.
todos:
  - id: liability-symmetric-bancor
    content: Implement symmetric synthetic-depth Bancor for both directions when ch_basket <= 0 (bootstrap donor flat minR).
    status: completed
  - id: minr-ratchet
    content: Worker CHR→EUR updates minR = max(previous_minR, computed_newR).
    status: completed
  - id: integrate-forex-call-sites
    content: Route calculate_forex_transaction_params and UI previews through regime-aware pricing and settlement.
    status: completed
  - id: forex-reverse-consistency
    content: Add matching inverse/preview helpers for each regime so project pricing and checkout match settlement.
    status: completed
  - id: admin-minr
    content: Add initial minR admin setting.
    status: completed
  - id: project-pool-forex-audit
    content: Align or document project sandbox forex (project_forex) vs base-pool liability regime.
    status: pending
  - id: neofund-a2-settlement
    content: Wire discretionary-support approvals to rovas_forex_settle_trade chr_to_eur with worker ratchet.
    status: pending
isProject: false
---

# Forex Rate Mechanism

## Conventions

- **Hour unit:** `10 CHR = 1 reported hour`. Effective hourly rate in EUR/hour:
  ```
  rate = eur_amount / (chr_amount / 10)
  ```
- **`minR`** — EUR/hour floor anchor. **Admin sets initial `minR`.** Only **worker CHR→EUR** trades may raise it: `minR := max(previous_minR, computed_newR)` (ratchet up only).
- **`forex()` never receives negative basket balances.** Negative `ch_basket` is ledger-only; liability pricing uses **synthetic CHR depth** `|ch_basket| + 1`.
- **Include CHR input in depth** for Bancor when `ch_basket > 0`.

## Regime summary

| Condition | EUR → CHR | CHR → EUR |
|-----------|-----------|-----------|
| `eur_basket <= 0` | Allowed if `ch_basket <= 0` (bootstrap) | **Reject** |
| `ch_basket <= 0` and `ch_basket + chr_in <= 0` | **Symmetric Bancor** (below); bootstrap `0/0` flat **`minR`** | **Symmetric Bancor** (below) |
| `ch_basket + chr_in > 0` (before pricing) | **Bancor** (real baskets) | **Bancor** (real baskets) |
| `ch_basket > 0` | **Bancor** (real baskets) | **Bancor** (real baskets) |

**Bootstrap (`0/0`):** CHR→EUR blocked (no EUR). First trade is always donor **EUR→CHR** at flat `minR`.

### Liability feedback (design intent)

In liability, both directions share the same synthetic pool:

- **EUR scarce / liability high** → low CHR→EUR rate (workers wait), generous EUR→CHR (donors fund).
- **EUR accumulates / liability shrinks** → CHR→EUR rate rises (workers attracted), EUR→CHR tightens (donors pay more per hour).
- Worker redemptions drain EUR → rate falls for the marginal trade → natural equilibrium toward exiting liability (`ch_basket → 0`).

---

## Synthetic depth (liability only)

Outstanding liability magnitude stands in for CHR pool depth:

```
chr_depth_donor  = |ch_basket| + 1
chr_depth_worker = |ch_basket + chr_requested| + 1
```

EUR side uses the **real** `eur_basket` only (no virtual EUR term).

---

## EUR → CHR

### Bootstrap (`ch_basket == 0` and `eur_basket == 0`)

```
chr_out = 10 * eur_after_fees / minR
eur_basket += eur_after_fees
ch_basket := -chr_out
```

One-time seed at admin `minR`. All subsequent liability trades use symmetric Bancor.

### Liability (`ch_basket < 0`)

```
chr_depth = |ch_basket| + 1
chr_out = forex(eur_in, eur_basket + eur_in, chr_depth, fees…)
eur_basket += eur_net
ch_basket -= chr_out
```

`minR` is **not** updated by donor trades. Effective donor rate rises as `eur_basket` grows and `|ch_basket|` falls.

### Bancor (`ch_basket > 0`)

```
forex(eur_in, eur_basket + eur_in, ch_basket, fees…)
```

Settlement: `eur_basket += eur_net`, `ch_basket -= chr_out`.

---

## CHR → EUR

### Blocked

```
if eur_basket <= 0: reject
```

### Regime switch (before pricing)

```
if ch_basket + chr_requested > 0:
  use Bancor on real baskets (same as positive regime)
```

### Liability — symmetric Bancor (`ch_basket <= 0` and trade stays in liability)

```
chr_depth = |ch_basket + chr_requested| + 1
eur_raw = forex(chr_requested, chr_depth, eur_basket, fees…)
EUR_reward = max(eur_raw, (chr_requested / 10) * minR)
```

Settlement:

```
ch_basket += chr_requested
eur_basket -= EUR_reward
```

**Ratchet (workers only):**

```
computed_newR = 10 * EUR_reward / chr_requested
minR = max(previous_minR, computed_newR)
```

Reject if `EUR_reward <= 0`, `EUR_reward > eur_basket`, or insufficient liquidity (optional future: minimum `eur_basket` guard).

### Bancor (`ch_basket + chr_requested > 0` or `ch_basket > 0`)

```
forex(chr_in, ch_basket + chr_in, eur_basket, fees…)
```

Settlement per `rovas_forex.module`. Worker trades apply the same **minR ratchet** from `computed_newR`.

---

## Merit-motivated workers (non-arbitrage)

Direct path: **CHR → ME at 1:1**.

Merit-motivated users need not use CHR→EUR unless:

```
ME from (CHR → EUR → spend EUR on merits)  >  ME from direct CHR → ME
```

If that never holds, EUR/hour on CHR→EUR only affects **fiat-motivated** workers. Re-check if `minR` ratchets up and merit purchase from EUR becomes more favorable.

---

## Worked example — bootstrap EUR→CHR

```
ch_basket = 0, eur_basket = 0, minR = 0.1
Donor: 100 EUR → chr_out = 10 000 CHR
End: ch_basket = -10 000, eur_basket = 100
```

---

## Worked example — symmetric liability trades

After bootstrap (`ch_basket = -10 000`, `eur_basket = 100`, `minR = 0.1`):

**Worker** `chr_requested = 200`:

```
chr_depth = |-10 000 + 200| + 1 = 9 801
eur_raw = forex(200, 9 801, 100, 0, 0)
EUR_reward = max(eur_raw, 200/10 * 0.1) = max(eur_raw, 2.0)
→ rate ≈ 0.10 EUR/hour (thin EUR pool; floor binds)
ch_basket = -9 800, eur_basket ≈ 98
```

**Donor** `50 EUR` (same state before worker, for illustration):

```
chr_depth = 10 000 + 1 = 10 001
chr_out = forex(50, 100 + 50, 10 001, 0, 0) ≈ 1 667 CHR
→ rate ≈ 0.30 EUR/hour (fewer CHR per EUR than flat minR)
ch_basket ≈ -11 667, eur_basket = 150
```

As EUR accumulates and workers redeem CHR, CHR→EUR rates rise and EUR→CHR rates rise with them — donors fund when cheap, workers cash out when the pool can support it.

---

## Future extensions

- Scale `minR` with cumulative EUR inflow and/or participant counts.
- Optional minimum `eur_basket` before CHR→EUR.
- **Project-pool forex audit** — sandbox/project paths may still use legacy `project_forex()` linear pricing; align or document separately (see NeoFund [`rovas-app-agent-brief.md`](../../../Business/NeoFund_NP/rovas-app-agent-brief.md) A5).

## NeoFund Path 1 integration

Under NeoFund Path 1 ([`HANDOFF.md`](../../../Business/NeoFund_NP/HANDOFF.md) §3.5):

- Users do **not** execute CHR→EUR at the displayed rate. The forex UI is replaced by a **discretionary-support application** flow; approved payouts settle via `rovas_forex_settle_trade('chr_to_eur', …)` with worker `minR` ratchet.
- The computed **`effective_rate`** may be **published as an indicative support reference** (EUR per reported hour) to help users decide whether to report work — with explicit disclaimer that the governing body decides each payment per case.
- **A6** (Chron→Merit election) debits only the user's CHR balance; it does not update pool baskets.
- The **správna rada** may manually adjust `minR` and basket balances via the forex admin UI.

Glossary cross-reference: NeoFund [`glossary.md`](../../../Business/NeoFund_NP/glossary.md) — **Indicative support reference rate**, **minR**, **Pool rate**.

## Implementation touchpoints

- [`rovas_forex.module`](../sites/all/modules/custom/rovas_forex/rovas_forex.module)
- Simulation: [`scripts/forex_simulation/forex_sim.py`](../scripts/forex_simulation/forex_sim.py)
  - `--liability symmetric-bancor --minr-ratchet`
  - `--compare` — virtual-bancor vs symmetric-bancor comparison workbook

## Acceptance criteria

- Bootstrap from `0/0` via flat-`minR` EUR→CHR only.
- Symmetric synthetic-depth Bancor for both directions while `ch_basket <= 0` (after bootstrap).
- Bancor on real baskets when `ch_basket + chr_in > 0`.
- `minR` ratchets up on worker trades only; `minR` never decreases.
- Worker CHR→EUR payout floored at `minR`; donor and worker rates both respond to basket state in liability.
- CHR→EUR rejected when `eur_basket <= 0`.
