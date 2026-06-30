#!/usr/bin/env python3
"""
Verify forex regime behavior against docs/forex-rate-mechanism.md worked examples.

Self-contained (no openpyxl / forex_sim import). Mirrors symmetric-bancor logic.
Run: python3 scripts/forex_simulation/verify_regime_parity.py
"""

from __future__ import annotations

import math
from dataclasses import dataclass


@dataclass
class BasketState:
    ch_basket: float = 0.0
    eur_basket: float = 0.0
    min_r: float = 0.1


def forex(input_amt, input_balance, output_balance, fee=0.0, fixed_fee=0.0):
    if not input_balance:
        return {"output": None, "fee": None}
    all_fees = input_amt * fee + fixed_fee
    input_adjusted = input_amt - all_fees
    if fee < 0 or input_amt <= all_fees or fee > 1:
        return {"output": 0.0, "fee": abs(all_fees)}
    smartt = math.pow(1.0 + input_adjusted / input_balance, 0.5) - 1.0
    calc_output = output_balance * (math.pow(1.0 - smartt / (1.0 + smartt), 2.0) - 1.0) * -1.0
    return {"output": calc_output, "fee": abs(all_fees)}


def synthetic_chr_depth(ch_basket: float) -> float:
    return abs(ch_basket) + 1.0


def synthetic_worker_chr_depth(ch_basket: float, chr_in: float) -> float:
    return abs(ch_basket + chr_in) + 1.0


def execute_eur_to_chr(eur_in: float, state: BasketState) -> tuple[float, str]:
    eur_net = eur_in
    if state.ch_basket == 0 and state.eur_basket == 0:
        chr_out = 10.0 * eur_net / state.min_r
        state.eur_basket += eur_net
        state.ch_basket = -chr_out
        return chr_out, "bootstrap"
    if state.ch_basket <= 0:
        chr_depth = synthetic_chr_depth(state.ch_basket)
        fx = forex(eur_in, state.eur_basket + eur_in, chr_depth, 0.0, 0.0)
        chr_out = fx["output"] or 0.0
        state.eur_basket += eur_net
        state.ch_basket -= chr_out
        return chr_out, "liability_symmetric"
    fx = forex(eur_in, state.eur_basket + eur_in, state.ch_basket, 0.0, 0.0)
    chr_out = fx["output"] or 0.0
    state.eur_basket += eur_net
    state.ch_basket -= chr_out
    return chr_out, "positive"


def execute_chr_to_eur(chr_in: float, state: BasketState) -> tuple[float, str]:
    if state.eur_basket <= 0:
        raise AssertionError("eur_basket <= 0 should reject")
    if state.ch_basket + chr_in > 0:
        fx = forex(chr_in, state.ch_basket + chr_in, state.eur_basket, 0.0, 0.0)
        eur_out = fx["output"] or 0.0
        state.ch_basket += chr_in
        state.eur_basket -= eur_out
        return eur_out, "positive"
    chr_depth = synthetic_worker_chr_depth(state.ch_basket, chr_in)
    fx = forex(chr_in, chr_depth, state.eur_basket, 0.0, 0.0)
    eur_raw = fx["output"] or 0.0
    floor = (chr_in / 10.0) * state.min_r
    eur_out = max(eur_raw, floor)
    state.ch_basket += chr_in
    state.eur_basket -= eur_out
    computed = 10.0 * eur_out / chr_in
    state.min_r = max(state.min_r, computed)
    return eur_out, "liability_symmetric"


def approx(a: float, b: float, tol: float = 0.05) -> bool:
    return abs(a - b) <= tol


def test_bootstrap() -> None:
    state = BasketState()
    chr_out, regime = execute_eur_to_chr(100.0, state)
    assert regime == "bootstrap"
    assert chr_out == 10_000
    assert state.ch_basket == -10_000 and state.eur_basket == 100
    print("bootstrap: OK")


def test_liability_worker_and_donor() -> None:
    state = BasketState(ch_basket=-10_000, eur_basket=100, min_r=0.1)
    eur_out, _ = execute_chr_to_eur(200.0, state)
    assert approx(eur_out, 2.0), eur_out
    assert state.ch_basket == -9_800 and approx(state.eur_basket, 98.0)

    donor = BasketState(ch_basket=-10_000, eur_basket=100, min_r=0.1)
    chr_out, _ = execute_eur_to_chr(50.0, donor)
    expected_chr = forex(50, 150, 10_001, 0.0, 0.0)["output"] or 0.0
    assert approx(chr_out, expected_chr, tol=1e-3), (chr_out, expected_chr)
    print("liability symmetric trades: OK")


def test_positive_bancor_matches_legacy_forex() -> None:
    ch_basket = 50_000.0
    eur_basket = 5_000.0
    eur_in = 100.0
    legacy = forex(eur_in, eur_basket + eur_in, ch_basket, 0.0, 0.0)
    state = BasketState(ch_basket=ch_basket, eur_basket=eur_basket, min_r=0.1)
    chr_out, regime = execute_eur_to_chr(eur_in, state)
    assert regime == "positive"
    assert approx(chr_out, legacy["output"] or 0.0, tol=1e-6)
    print("positive-basket regression: OK")


def test_chr_to_eur_blocked_without_eur() -> None:
    state = BasketState(ch_basket=-1_000, eur_basket=0, min_r=0.1)
    try:
        execute_chr_to_eur(100.0, state)
        raise AssertionError("expected rejection")
    except AssertionError as exc:
        if "eur_basket" not in str(exc):
            raise
    print("chr_to_eur blocked when eur_basket<=0: OK")


def main() -> int:
    test_bootstrap()
    test_liability_worker_and_donor()
    test_positive_bancor_matches_legacy_forex()
    test_chr_to_eur_blocked_without_eur()
    print("\nAll regime parity checks passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
