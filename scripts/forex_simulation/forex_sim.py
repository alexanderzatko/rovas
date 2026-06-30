#!/usr/bin/env python3
"""
Forex rate mechanism simulation (see docs/forex-rate-mechanism.md).

Models alternating EUR→CHR (donors / merit buyers) and CHR→EUR (workers cashing out).
Outputs an Excel workbook for inspection and future parameter sweeps.
"""

from __future__ import annotations

import argparse
import math
from dataclasses import dataclass, field
from datetime import datetime, timezone
from pathlib import Path
from typing import Literal

from openpyxl import Workbook
from openpyxl.styles import Font
from openpyxl.utils import get_column_letter

TradeType = Literal["EUR→CHR", "CHR→EUR"]
LiabilityMode = Literal["flat-minr", "virtual-bancor", "symmetric-bancor"]


@dataclass
class SimParams:
    initial_min_r: float = 0.1
    liability_mode: LiabilityMode = "virtual-bancor"
    minr_ratchet: bool = True
    variable_fee: float = 0.0
    fixed_fee: float = 0.0
    n_transactions: int = 20
    eur_per_donation: float = 50.0
    chr_per_eur_start: float = 3.0
    chr_per_eur_end: float = 20.0
    start_with_donor: bool = True
    ch_basket_init: float = 0.0
    eur_basket_init: float = 0.0


@dataclass
class BasketState:
    ch_basket: float = 0.0
    eur_basket: float = 0.0
    cumulative_eur_in: float = 0.0
    min_r: float = 0.1


@dataclass
class TradeResult:
    tx_num: int
    trade_type: TradeType
    regime: str
    eur_in: float
    chr_in: float
    eur_out: float
    chr_out: float
    effective_rate: float | None
    chr_per_eur_scenario: float | None
    chr_baseline: float | None
    rate_preview: float | None
    min_r_before: float
    min_r_after: float
    ch_basket_before: float
    eur_basket_before: float
    ch_basket_after: float
    eur_basket_after: float
    cumulative_eur_in: float
    status: str
    notes: str = ""


def forex(
    input_amt: float,
    input_balance: float,
    output_balance: float,
    fee: float = 0.0,
    fixed_fee: float = 0.0,
) -> dict[str, float | None]:
    """Port of rovas_forex forex() (Bancor bonding curve)."""
    if not input_balance:
        return {"output": None, "fee": None}

    input_amt = float(input_amt)
    input_balance = float(input_balance)
    output_balance = float(output_balance)
    fee = float(fee)
    fixed_fee = float(fixed_fee)

    tt_balance = 1.0
    input_weight = 0.5
    output_weight = 1.0 - input_weight

    all_fees = input_amt * fee + fixed_fee
    input_adjusted = input_amt - all_fees

    if fee < 0 or input_amt <= all_fees or fee > 1:
        calc_output = 0.0
    else:
        smartt = tt_balance * (
            math.pow(1.0 + input_adjusted / input_balance, input_weight) - 1.0
        )
        calc_output = (
            output_balance
            * (
                math.pow(
                    1.0 - smartt / (tt_balance + smartt),
                    1.0 / output_weight,
                )
                - 1.0
            )
            * -1.0
        )

    return {"output": calc_output, "fee": abs(all_fees)}


def effective_hourly_rate(eur_amount: float, chr_amount: float) -> float | None:
    if chr_amount <= 0:
        return None
    return eur_amount / (chr_amount / 10.0)


def eur_after_fees(eur_in: float, params: SimParams) -> float:
    fx = forex(eur_in, max(eur_in, 1e-9), 1.0, params.variable_fee, params.fixed_fee)
    return eur_in - (fx["fee"] or 0.0)


def synthetic_chr_depth(ch_basket: float) -> float:
    """CHR pool depth for liability-side Bancor (magnitude of outstanding liability)."""
    return abs(ch_basket) + 1.0


def synthetic_worker_chr_depth(ch_basket: float, chr_in: float) -> float:
    """CHR depth after a worker CHR→EUR trade stays in liability."""
    return abs(ch_basket + chr_in) + 1.0


def is_bootstrap_state(state: BasketState) -> bool:
    return state.ch_basket == 0 and state.eur_basket == 0


def symmetric_chr_to_eur_reward(
    state: BasketState,
    chr_in: float,
    params: SimParams,
) -> tuple[float, float]:
    """Liability CHR→EUR via Bancor on (synthetic CHR depth, real EUR basket)."""
    chr_depth = synthetic_worker_chr_depth(state.ch_basket, chr_in)
    fx = forex(
        chr_in,
        chr_depth,
        state.eur_basket,
        params.variable_fee,
        params.fixed_fee,
    )
    eur_out = fx["output"] or 0.0
    floor = (chr_in / 10.0) * state.min_r
    return max(eur_out, floor), chr_depth


def symmetric_eur_to_chr_out(
    state: BasketState,
    eur_in: float,
    eur_net: float,
    params: SimParams,
) -> tuple[float, float]:
    """Liability EUR→CHR via Bancor on (real EUR basket, synthetic CHR depth)."""
    chr_depth = synthetic_chr_depth(state.ch_basket)
    fx = forex(
        eur_in,
        state.eur_basket + eur_in,
        chr_depth,
        params.variable_fee,
        params.fixed_fee,
    )
    return fx["output"] or 0.0, chr_depth


def virtual_eur_reward(
    state: BasketState,
    chr_in: float,
    params: SimParams,
) -> tuple[float, float, float]:
    """Virtual-pool CHR→EUR. Returns (eur_reward, new_chr, new_eur)."""
    new_chr = abs(state.ch_basket + chr_in) + 1.0
    new_eur = (new_chr / 10.0) * state.min_r + state.eur_basket
    fx = forex(
        chr_in,
        new_chr,
        new_eur,
        params.variable_fee,
        params.fixed_fee,
    )
    temp_eur = fx["output"] or 0.0
    if new_eur <= 0:
        return 0.0, new_chr, new_eur
    eur_reward = state.eur_basket * (temp_eur / new_eur)
    return eur_reward, new_chr, new_eur


def preview_chr_to_eur_rate(
    state: BasketState,
    chr_amount: float,
    params: SimParams,
) -> float | None:
    """Hourly rate a worker sees before CHR→EUR for the given CHR amount."""
    if state.eur_basket <= 0 or chr_amount <= 0:
        return None

    if state.ch_basket + chr_amount > 0:
        fx = forex(
            chr_amount,
            state.ch_basket + chr_amount,
            state.eur_basket,
            params.variable_fee,
            params.fixed_fee,
        )
        return effective_hourly_rate(fx["output"] or 0.0, chr_amount)

    if params.liability_mode == "virtual-bancor" and state.ch_basket <= 0:
        eur_reward, _, _ = virtual_eur_reward(state, chr_amount, params)
        return effective_hourly_rate(eur_reward, chr_amount)

    if params.liability_mode == "symmetric-bancor" and state.ch_basket <= 0:
        eur_reward, _ = symmetric_chr_to_eur_reward(state, chr_amount, params)
        return effective_hourly_rate(eur_reward, chr_amount)

    return state.min_r


def worker_chr_amount(
    state: BasketState,
    params: SimParams,
    chr_baseline: float,
) -> tuple[float, float | None]:
    """
    Work offered (CHR) inversely proportional to preview CHR→EUR rate.
    chr = chr_baseline * (minR / R_preview), normalized at R_preview == minR.
    """
    if chr_baseline <= 0:
        return 0.0, None

    chr_amt = chr_baseline
    rate_preview: float | None = None
    for _ in range(50):
        rate_preview = preview_chr_to_eur_rate(state, chr_amt, params)
        if rate_preview is None or rate_preview <= 0:
            return chr_baseline, rate_preview
        chr_new = chr_baseline * state.min_r / rate_preview
        if abs(chr_new - chr_amt) < 1e-9:
            return chr_new, rate_preview
        chr_amt = chr_new

    return chr_amt, rate_preview


def ratchet_min_r(
    state: BasketState,
    eur_out: float,
    chr_in: float,
    params: SimParams,
) -> None:
    if not params.minr_ratchet or chr_in <= 0:
        return
    computed = 10.0 * eur_out / chr_in
    state.min_r = max(state.min_r, computed)


def execute_eur_to_chr(
    tx_num: int,
    eur_in: float,
    state: BasketState,
    params: SimParams,
    chr_per_eur_scenario: float | None,
) -> TradeResult:
    before_ch, before_eur = state.ch_basket, state.eur_basket
    min_r_before = state.min_r
    eur_net = eur_after_fees(eur_in, params)

    if state.ch_basket <= 0:
        if (
            params.liability_mode == "symmetric-bancor"
            and not is_bootstrap_state(state)
        ):
            regime = "EUR→CHR (liability / symmetric Bancor)"
            chr_out, chr_depth = symmetric_eur_to_chr_out(
                state, eur_in, eur_net, params
            )
            state.eur_basket += eur_net
            state.cumulative_eur_in += eur_net
            state.ch_basket -= chr_out
            notes = (
                f"synthetic chr_depth={chr_depth:.0f}; "
                f"donor rate={effective_hourly_rate(eur_net, chr_out):.6f} EUR/hour"
            )
        else:
            regime = "EUR→CHR (liability / flat minR bootstrap)"
            if params.liability_mode != "symmetric-bancor":
                regime = "EUR→CHR (liability / flat minR)"
            chr_out = 10.0 * eur_net / state.min_r
            state.eur_basket += eur_net
            state.cumulative_eur_in += eur_net
            if state.ch_basket == 0:
                state.ch_basket = -chr_out
            else:
                state.ch_basket -= chr_out
            notes = f"Flat minR={state.min_r:.6f} EUR/hour (donor; minR unchanged)"
    else:
        regime = "EUR→CHR (Bancor)"
        fx = forex(
            eur_in,
            state.eur_basket + eur_in,
            state.ch_basket,
            params.variable_fee,
            params.fixed_fee,
        )
        chr_out = fx["output"] or 0.0
        state.eur_basket += eur_net
        state.cumulative_eur_in += eur_net
        state.ch_basket -= chr_out
        notes = "Bancor on positive CH basket (donor; minR unchanged)"

    rate = effective_hourly_rate(eur_net, chr_out)

    return TradeResult(
        tx_num=tx_num,
        trade_type="EUR→CHR",
        regime=regime,
        eur_in=eur_in,
        chr_in=0.0,
        eur_out=0.0,
        chr_out=chr_out,
        effective_rate=rate,
        chr_per_eur_scenario=chr_per_eur_scenario,
        chr_baseline=None,
        rate_preview=None,
        min_r_before=min_r_before,
        min_r_after=state.min_r,
        ch_basket_before=before_ch,
        eur_basket_before=before_eur,
        ch_basket_after=state.ch_basket,
        eur_basket_after=state.eur_basket,
        cumulative_eur_in=state.cumulative_eur_in,
        status="ok",
        notes=notes,
    )


def execute_bancor_chr_to_eur(
    tx_num: int,
    chr_in: float,
    state: BasketState,
    params: SimParams,
    chr_per_eur_scenario: float | None,
    chr_baseline: float | None,
    rate_preview: float | None,
    regime_label: str,
) -> TradeResult:
    before_ch, before_eur = state.ch_basket, state.eur_basket
    min_r_before = state.min_r

    fx = forex(
        chr_in,
        state.ch_basket + chr_in,
        state.eur_basket,
        params.variable_fee,
        params.fixed_fee,
    )
    eur_out = fx["output"] or 0.0
    fee_chr = fx["fee"] or 0.0
    state.ch_basket += chr_in - fee_chr
    state.eur_basket -= eur_out
    ratchet_min_r(state, eur_out, chr_in, params)

    notes = regime_label
    if params.minr_ratchet:
        notes += f"; minR ratchet → {state.min_r:.6f}"
    if chr_baseline is not None and rate_preview is not None:
        notes += f"; chr={chr_in:.0f} baseline={chr_baseline:.0f} preview_R={rate_preview:.6f}"

    return TradeResult(
        tx_num=tx_num,
        trade_type="CHR→EUR",
        regime=regime_label,
        eur_in=0.0,
        chr_in=chr_in,
        eur_out=eur_out,
        chr_out=0.0,
        effective_rate=effective_hourly_rate(eur_out, chr_in),
        chr_per_eur_scenario=chr_per_eur_scenario,
        chr_baseline=chr_baseline,
        rate_preview=rate_preview,
        min_r_before=min_r_before,
        min_r_after=state.min_r,
        ch_basket_before=before_ch,
        eur_basket_before=before_eur,
        ch_basket_after=state.ch_basket,
        eur_basket_after=state.eur_basket,
        cumulative_eur_in=state.cumulative_eur_in,
        status="ok",
        notes=notes,
    )


def execute_chr_to_eur(
    tx_num: int,
    chr_in: float,
    state: BasketState,
    params: SimParams,
    chr_per_eur_scenario: float | None,
    chr_baseline: float | None = None,
    rate_preview: float | None = None,
) -> TradeResult:
    before_ch, before_eur = state.ch_basket, state.eur_basket
    min_r_before = state.min_r

    if state.eur_basket <= 0:
        return TradeResult(
            tx_num=tx_num,
            trade_type="CHR→EUR",
            regime="blocked",
            eur_in=0.0,
            chr_in=chr_in,
            eur_out=0.0,
            chr_out=0.0,
            effective_rate=None,
            chr_per_eur_scenario=chr_per_eur_scenario,
            chr_baseline=chr_baseline,
            rate_preview=rate_preview,
            min_r_before=min_r_before,
            min_r_after=state.min_r,
            ch_basket_before=before_ch,
            eur_basket_before=before_eur,
            ch_basket_after=before_ch,
            eur_basket_after=before_eur,
            cumulative_eur_in=state.cumulative_eur_in,
            status="rejected",
            notes="eur_basket <= 0",
        )

    if state.ch_basket + chr_in > 0:
        return execute_bancor_chr_to_eur(
            tx_num,
            chr_in,
            state,
            params,
            chr_per_eur_scenario,
            chr_baseline,
            rate_preview,
            "CHR→EUR (Bancor / exit liability)",
        )

    if params.liability_mode == "symmetric-bancor":
        regime = "CHR→EUR (liability / symmetric Bancor)"
        eur_out, chr_depth = symmetric_chr_to_eur_reward(state, chr_in, params)
        if eur_out <= 1e-12:
            return TradeResult(
                tx_num=tx_num,
                trade_type="CHR→EUR",
                regime=regime,
                eur_in=0.0,
                chr_in=chr_in,
                eur_out=0.0,
                chr_out=0.0,
                effective_rate=None,
                chr_per_eur_scenario=chr_per_eur_scenario,
                chr_baseline=chr_baseline,
                rate_preview=rate_preview,
                min_r_before=min_r_before,
                min_r_after=state.min_r,
                ch_basket_before=before_ch,
                eur_basket_before=before_eur,
                ch_basket_after=before_ch,
                eur_basket_after=before_eur,
                cumulative_eur_in=state.cumulative_eur_in,
                status="rejected",
                notes="EUR payout <= 0",
            )
        if eur_out > state.eur_basket + 1e-9:
            return TradeResult(
                tx_num=tx_num,
                trade_type="CHR→EUR",
                regime=regime,
                eur_in=0.0,
                chr_in=chr_in,
                eur_out=eur_out,
                chr_out=0.0,
                effective_rate=effective_hourly_rate(eur_out, chr_in),
                chr_per_eur_scenario=chr_per_eur_scenario,
                chr_baseline=chr_baseline,
                rate_preview=rate_preview,
                min_r_before=min_r_before,
                min_r_after=state.min_r,
                ch_basket_before=before_ch,
                eur_basket_before=before_eur,
                ch_basket_after=before_ch,
                eur_basket_after=before_eur,
                cumulative_eur_in=state.cumulative_eur_in,
                status="rejected",
                notes=(
                    f"need {eur_out:.2f} EUR, basket has {state.eur_basket:.2f}"
                ),
            )
        state.ch_basket += chr_in
        state.eur_basket -= eur_out
        ratchet_min_r(state, eur_out, chr_in, params)
        notes = f"synthetic chr_depth={chr_depth:.0f}"
        if params.minr_ratchet:
            notes += f"; minR ratchet → {state.min_r:.6f}"
    elif params.liability_mode == "virtual-bancor":
        regime = "CHR→EUR (liability / virtual Bancor)"
        eur_out, new_chr, new_eur = virtual_eur_reward(state, chr_in, params)
        if eur_out <= 1e-12:
            return TradeResult(
                tx_num=tx_num,
                trade_type="CHR→EUR",
                regime=regime,
                eur_in=0.0,
                chr_in=chr_in,
                eur_out=0.0,
                chr_out=0.0,
                effective_rate=None,
                chr_per_eur_scenario=chr_per_eur_scenario,
                chr_baseline=chr_baseline,
                rate_preview=rate_preview,
                min_r_before=min_r_before,
                min_r_after=state.min_r,
                ch_basket_before=before_ch,
                eur_basket_before=before_eur,
                ch_basket_after=before_ch,
                eur_basket_after=before_eur,
                cumulative_eur_in=state.cumulative_eur_in,
                status="rejected",
                notes="EUR_reward <= 0 after virtual scaling",
            )
        state.ch_basket += chr_in
        state.eur_basket -= eur_out
        ratchet_min_r(state, eur_out, chr_in, params)
        notes = (
            f"virtual newCHR={new_chr:.0f} newEUR={new_eur:.2f}; "
            f"minR ratchet → {state.min_r:.6f}"
            if params.minr_ratchet
            else f"virtual newCHR={new_chr:.0f} newEUR={new_eur:.2f}"
        )
    else:
        regime = "CHR→EUR (liability / flat minR)"
        eur_out = (chr_in / 10.0) * state.min_r
        if eur_out > state.eur_basket + 1e-9:
            return TradeResult(
                tx_num=tx_num,
                trade_type="CHR→EUR",
                regime=regime,
                eur_in=0.0,
                chr_in=chr_in,
                eur_out=eur_out,
                chr_out=0.0,
                effective_rate=state.min_r,
                chr_per_eur_scenario=chr_per_eur_scenario,
                chr_baseline=chr_baseline,
                rate_preview=rate_preview,
                min_r_before=min_r_before,
                min_r_after=state.min_r,
                ch_basket_before=before_ch,
                eur_basket_before=before_eur,
                ch_basket_after=before_ch,
                eur_basket_after=before_eur,
                cumulative_eur_in=state.cumulative_eur_in,
                status="rejected",
                notes=(
                    f"need {eur_out:.2f} EUR at minR, basket has {state.eur_basket:.2f}"
                ),
            )
        state.ch_basket += chr_in
        state.eur_basket -= eur_out
        ratchet_min_r(state, eur_out, chr_in, params)
        notes = f"Flat minR={state.min_r:.6f}"

    if chr_baseline is not None and rate_preview is not None:
        notes += f"; chr={chr_in:.0f} baseline={chr_baseline:.0f} preview_R={rate_preview:.6f}"

    return TradeResult(
        tx_num=tx_num,
        trade_type="CHR→EUR",
        regime=regime,
        eur_in=0.0,
        chr_in=chr_in,
        eur_out=eur_out,
        chr_out=0.0,
        effective_rate=effective_hourly_rate(eur_out, chr_in),
        chr_per_eur_scenario=chr_per_eur_scenario,
        chr_baseline=chr_baseline,
        rate_preview=rate_preview,
        min_r_before=min_r_before,
        min_r_after=state.min_r,
        ch_basket_before=before_ch,
        eur_basket_before=before_eur,
        ch_basket_after=state.ch_basket,
        eur_basket_after=state.eur_basket,
        cumulative_eur_in=state.cumulative_eur_in,
        status="ok",
        notes=notes,
    )


def chr_baseline_at_step(step_index: int, n_steps: int, params: SimParams) -> float:
    ratio = chr_per_eur_at_step(step_index, n_steps, params)
    return params.eur_per_donation * ratio


def chr_per_eur_at_step(step_index: int, n_steps: int, params: SimParams) -> float:
    if n_steps <= 1:
        return params.chr_per_eur_start
    t = step_index / (n_steps - 1)
    return params.chr_per_eur_start + t * (params.chr_per_eur_end - params.chr_per_eur_start)


def build_scenario(params: SimParams) -> list[tuple[TradeType, int]]:
    n = params.n_transactions
    n_pairs = n // 2
    scenario: list[tuple[TradeType, int]] = []

    for i in range(n_pairs):
        if params.start_with_donor:
            scenario.append(("EUR→CHR", i))
            scenario.append(("CHR→EUR", i))
        else:
            scenario.append(("CHR→EUR", i))
            scenario.append(("EUR→CHR", i))

    if n % 2 == 1:
        if params.start_with_donor:
            scenario.append(("EUR→CHR", n_pairs))
        else:
            scenario.append(("CHR→EUR", n_pairs))

    return scenario[:n]


def run_simulation(params: SimParams) -> tuple[list[TradeResult], BasketState]:
    state = BasketState(
        ch_basket=params.ch_basket_init,
        eur_basket=params.eur_basket_init,
        min_r=params.initial_min_r,
    )
    results: list[TradeResult] = []
    scenario = build_scenario(params)
    n_pairs = max(len(scenario) // 2, 1)

    for idx, (trade_type, step_i) in enumerate(scenario, start=1):
        ratio = chr_per_eur_at_step(step_i, n_pairs, params)

        if trade_type == "EUR→CHR":
            results.append(
                execute_eur_to_chr(
                    idx, params.eur_per_donation, state, params, ratio
                )
            )
        else:
            baseline = chr_baseline_at_step(step_i, n_pairs, params)
            chr_amt, rate_preview = worker_chr_amount(state, params, baseline)
            results.append(
                execute_chr_to_eur(
                    idx,
                    chr_amt,
                    state,
                    params,
                    ratio,
                    chr_baseline=baseline,
                    rate_preview=rate_preview,
                )
            )

    return results, state


def autosize_columns(ws) -> None:
    for col_cells in ws.columns:
        max_len = 0
        col_letter = get_column_letter(col_cells[0].column)
        for cell in col_cells:
            if cell.value is not None:
                max_len = max(max_len, len(str(cell.value)))
        ws.column_dimensions[col_letter].width = min(max_len + 2, 48)


def write_excel(
    path: Path,
    params: SimParams,
    results: list[TradeResult],
    final_state: BasketState,
) -> None:
    wb = Workbook()

    ws_p = wb.active
    ws_p.title = "Parameters"
    ws_p.append(["Parameter", "Value"])
    ws_p["A1"].font = ws_p["B1"].font = Font(bold=True)
    param_rows = [
        ("initial minR (EUR/hour)", params.initial_min_r),
        ("liability_mode", params.liability_mode),
        ("minR ratchet (workers only)", params.minr_ratchet),
        ("ch_basket init", params.ch_basket_init),
        ("eur_basket init", params.eur_basket_init),
        ("Variable fee", params.variable_fee),
        ("Fixed fee", params.fixed_fee),
        ("Transactions", params.n_transactions),
        ("EUR per donation", params.eur_per_donation),
        ("CHR per EUR (start)", params.chr_per_eur_start),
        ("CHR per EUR (end)", params.chr_per_eur_end),
        ("Worker CHR model", "chr = chr_baseline * (minR / R_preview)"),
        ("Scenario", "chr_per_eur ramp 3 → 20; start 0/0"),
        ("Generated (UTC)", datetime.now(timezone.utc).strftime("%Y-%m-%d %H:%M:%S")),
        ("Final minR", final_state.min_r),
        ("Final ch_basket", final_state.ch_basket),
        ("Final eur_basket", final_state.eur_basket),
        ("Cumulative EUR in", final_state.cumulative_eur_in),
    ]
    for row in param_rows:
        ws_p.append(row)
    autosize_columns(ws_p)

    ws_t = wb.create_sheet("Transactions")
    headers = [
        "tx",
        "type",
        "regime",
        "status",
        "eur_in",
        "chr_in",
        "eur_out",
        "chr_out",
        "rate_EUR_per_hour",
        "rate_preview",
        "minR_before",
        "minR_after",
        "chr_baseline",
        "scenario_chr_per_eur",
        "ch_basket_before",
        "eur_basket_before",
        "ch_basket_after",
        "eur_basket_after",
        "cumulative_eur_in",
        "notes",
    ]
    ws_t.append(headers)
    for cell in ws_t[1]:
        cell.font = Font(bold=True)

    for r in results:
        ws_t.append(
            [
                r.tx_num,
                r.trade_type,
                r.regime,
                r.status,
                r.eur_in or "",
                r.chr_in or "",
                r.eur_out or "",
                r.chr_out or "",
                round(r.effective_rate, 6) if r.effective_rate is not None else "",
                round(r.rate_preview, 6) if r.rate_preview is not None else "",
                round(r.min_r_before, 6),
                round(r.min_r_after, 6),
                round(r.chr_baseline, 2) if r.chr_baseline is not None else "",
                round(r.chr_per_eur_scenario, 4) if r.chr_per_eur_scenario is not None else "",
                round(r.ch_basket_before, 2),
                round(r.eur_basket_before, 2),
                round(r.ch_basket_after, 2),
                round(r.eur_basket_after, 2),
                round(r.cumulative_eur_in, 2),
                r.notes,
            ]
        )
    ws_t.freeze_panes = "A2"
    autosize_columns(ws_t)

    ws_s = wb.create_sheet("Summary")
    ws_s.append(
        [
            "tx",
            "type",
            "rate_EUR_per_hour",
            "minR_after",
            "chr_baseline",
            "chr_in",
            "ch_basket",
            "eur_basket",
        ]
    )
    for cell in ws_s[1]:
        cell.font = Font(bold=True)

    for r in results:
        ws_s.append(
            [
                r.tx_num,
                r.trade_type,
                round(r.effective_rate, 6) if r.effective_rate is not None else "",
                round(r.min_r_after, 6),
                round(r.chr_baseline, 2) if r.chr_baseline is not None else "",
                round(r.chr_in, 2) if r.chr_in else "",
                round(r.ch_basket_after, 2),
                round(r.eur_basket_after, 2),
            ]
        )
    ws_s.freeze_panes = "A2"
    autosize_columns(ws_s)

    path.parent.mkdir(parents=True, exist_ok=True)
    wb.save(path)


def write_comparison_excel(
    path: Path,
    base_params: SimParams,
    runs: list[tuple[str, LiabilityMode, list[TradeResult], BasketState]],
) -> None:
    """Write side-by-side comparison of liability modes."""
    wb = Workbook()
    ws_c = wb.active
    ws_c.title = "Comparison"
    ws_c.append(["Metric"] + [label for label, _, _, _ in runs])
    for cell in ws_c[1]:
        cell.font = Font(bold=True)

    def col_values(getter):
        return [getter(results, final) for _, _, results, final in runs]

    metrics: list[tuple[str, object]] = [
        ("liability_mode", lambda _r, f: base_params.liability_mode),
        ("initial minR", lambda _r, f: base_params.initial_min_r),
        ("minR ratchet", lambda _r, f: base_params.minr_ratchet),
        ("transactions", lambda r, _f: len(r)),
        ("ok / rejected", lambda r, _f: f"{sum(1 for x in r if x.status == 'ok')} / {sum(1 for x in r if x.status == 'rejected')}"),
        ("final minR", lambda _r, f: f.min_r),
        ("final ch_basket", lambda _r, f: f.ch_basket),
        ("final eur_basket", lambda _r, f: f.eur_basket),
        ("cumulative EUR in", lambda _r, f: f.cumulative_eur_in),
        ("avg donor rate", lambda r, _f: _avg_rate(r, "EUR→CHR")),
        ("avg worker rate", lambda r, _f: _avg_rate(r, "CHR→EUR")),
        ("last worker rate", lambda r, _f: _last_rate(r, "CHR→EUR")),
        ("last donor rate", lambda r, _f: _last_rate(r, "EUR→CHR")),
    ]

    for label, getter in metrics:
        if label == "liability_mode":
            ws_c.append([label] + [mode for _, mode, _, _ in runs])
            continue
        ws_c.append([label] + col_values(getter))

    autosize_columns(ws_c)

    for label, _mode, results, final in runs:
        sheet_base = label.replace(" ", "_")[:28]
        ws = wb.create_sheet(f"Tx_{sheet_base}")
        headers = [
            "tx", "type", "rate_EUR_per_hour", "rate_preview",
            "minR_after", "chr_in", "chr_out", "ch_basket", "eur_basket", "regime",
        ]
        ws.append(headers)
        for cell in ws[1]:
            cell.font = Font(bold=True)
        for r in results:
            ws.append([
                r.tx_num,
                r.trade_type,
                round(r.effective_rate, 6) if r.effective_rate is not None else "",
                round(r.rate_preview, 6) if r.rate_preview is not None else "",
                round(r.min_r_after, 6),
                round(r.chr_in, 2) if r.chr_in else "",
                round(r.chr_out, 2) if r.chr_out else "",
                round(r.ch_basket_after, 2),
                round(r.eur_basket_after, 2),
                r.regime,
            ])
        ws.freeze_panes = "A2"
        autosize_columns(ws)

    path.parent.mkdir(parents=True, exist_ok=True)
    wb.save(path)


def _avg_rate(results: list[TradeResult], trade_type: TradeType) -> float | str:
    rates = [
        r.effective_rate
        for r in results
        if r.trade_type == trade_type and r.effective_rate is not None
    ]
    if not rates:
        return ""
    return round(sum(rates) / len(rates), 6)


def _last_rate(results: list[TradeResult], trade_type: TradeType) -> float | str:
    for r in reversed(results):
        if r.trade_type == trade_type and r.effective_rate is not None:
            return round(r.effective_rate, 6)
    return ""


def run_comparison(base_params: SimParams) -> list[tuple[str, LiabilityMode, list[TradeResult], BasketState]]:
    modes: list[tuple[str, LiabilityMode]] = [
        ("virtual_bancor", "virtual-bancor"),
        ("symmetric_bancor", "symmetric-bancor"),
    ]
    runs: list[tuple[str, LiabilityMode, list[TradeResult], BasketState]] = []
    for label, mode in modes:
        params = SimParams(
            initial_min_r=base_params.initial_min_r,
            liability_mode=mode,
            minr_ratchet=base_params.minr_ratchet,
            variable_fee=base_params.variable_fee,
            fixed_fee=base_params.fixed_fee,
            n_transactions=base_params.n_transactions,
            eur_per_donation=base_params.eur_per_donation,
            chr_per_eur_start=base_params.chr_per_eur_start,
            chr_per_eur_end=base_params.chr_per_eur_end,
            start_with_donor=base_params.start_with_donor,
            ch_basket_init=base_params.ch_basket_init,
            eur_basket_init=base_params.eur_basket_init,
        )
        results, final = run_simulation(params)
        runs.append((label, mode, results, final))
    return runs


def parse_args() -> argparse.Namespace:
    p = argparse.ArgumentParser(description="Simulate Rovas forex rate mechanism")
    p.add_argument("--min-r", type=float, default=0.1, help="Initial minR (EUR/hour)")
    p.add_argument(
        "--liability",
        choices=["flat-minr", "virtual-bancor", "symmetric-bancor"],
        default="virtual-bancor",
        help="CHR→EUR pricing while ch_basket <= 0",
    )
    p.add_argument(
        "--minr-ratchet",
        action="store_true",
        default=False,
        help="Workers ratchet minR = max(prev, computed_newR)",
    )
    p.add_argument("--n", type=int, default=20, help="Number of transactions")
    p.add_argument("--eur", type=float, default=50.0, help="EUR per donor trade")
    p.add_argument("--chr-start", type=float, default=3.0, help="CHR per EUR at start")
    p.add_argument("--chr-end", type=float, default=20.0, help="CHR per EUR at end")
    p.add_argument("--ch-init", type=float, default=0.0, help="Initial ch_basket")
    p.add_argument("--eur-init", type=float, default=0.0, help="Initial eur_basket")
    p.add_argument("-o", "--output", type=Path, default=None, help="Output .xlsx path")
    p.add_argument(
        "--compare",
        action="store_true",
        help="Run virtual-bancor vs symmetric-bancor and write comparison workbook",
    )
    return p.parse_args()


def main() -> None:
    args = parse_args()
    params = SimParams(
        initial_min_r=args.min_r,
        liability_mode=args.liability,
        minr_ratchet=args.minr_ratchet,
        n_transactions=args.n,
        eur_per_donation=args.eur,
        chr_per_eur_start=args.chr_start,
        chr_per_eur_end=args.chr_end,
        ch_basket_init=args.ch_init,
        eur_basket_init=args.eur_init,
    )

    script_dir = Path(__file__).resolve().parent

    if args.compare:
        out = args.output or (script_dir / "output" / "forex_sim_comparison.xlsx")
        runs = run_comparison(params)
        write_comparison_excel(out, params, runs)
        print(f"Wrote comparison {out}")
        for label, mode, results, final in runs:
            ok = sum(1 for r in results if r.status == "ok")
            rejected = sum(1 for r in results if r.status == "rejected")
            print(f"  {mode}: {ok} ok, {rejected} rejected; "
                  f"final CH={final.ch_basket:,.0f}, EUR={final.eur_basket:,.2f}, "
                  f"minR={final.min_r:.6f}")
        return

    out = args.output or (
        script_dir
        / "output"
        / f"forex_sim_{datetime.now(timezone.utc).strftime('%Y%m%d_%H%M%S')}.xlsx"
    )

    results, final = run_simulation(params)
    write_excel(out, params, results, final)

    ok = sum(1 for r in results if r.status == "ok")
    rejected = sum(1 for r in results if r.status == "rejected")
    print(f"Wrote {out}")
    print(f"Transactions: {len(results)} ({ok} ok, {rejected} rejected)")
    print(f"Final minR: {final.min_r:.6f} EUR/hour")
    print(f"Final baskets: CH={final.ch_basket:,.2f}, EUR={final.eur_basket:,.2f}")
    print(f"Cumulative EUR in: {final.cumulative_eur_in:,.2f}")


if __name__ == "__main__":
    main()
