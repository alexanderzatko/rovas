# Forex rate simulation

Simulates the mechanism in [docs/forex-rate-mechanism.md](../../docs/forex-rate-mechanism.md).

## Default run (virtual Bancor + minR ratchet, start 0/0)

```bash
python forex_sim.py --liability virtual-bancor --minr-ratchet \
  -o output/forex_sim_virtual_bancor.xlsx
```

## Modes

| Flag | Meaning |
|------|---------|
| `--liability virtual-bancor` | CHR→EUR in liability uses virtual-pool Bancor + EUR scaling; donors flat `minR` |
| `--liability symmetric-bancor` | Both directions use Bancor on synthetic CHR depth + real EUR (bootstrap donor flat `minR`) |
| `--liability flat-minr` | Legacy symmetric flat `minR` both directions |
| `--minr-ratchet` | Workers update `minR = max(prev, computed_newR)` |
| `--compare` | Run virtual-bancor vs symmetric-bancor into one comparison workbook |

## Symmetric Bancor comparison

```bash
python forex_sim.py --compare --minr-ratchet -o output/forex_sim_comparison.xlsx
python forex_sim.py --liability symmetric-bancor --minr-ratchet \
  -o output/forex_sim_symmetric_bancor.xlsx
```

## Scenario

- ~20 transactions, donor EUR→CHR then worker CHR→EUR
- Baskets start at **0 / 0** (override with `--ch-init`, `--eur-init`)
- `chr_baseline` ramps CHR-per-EUR reference **3 → 20**
- Worker CHR: `chr = chr_baseline × (minR / R_preview)`

## Setup

```bash
cd scripts/forex_simulation
python3 -m venv .venv && source .venv/bin/activate
pip install -r requirements.txt
```

## Output

Excel: **Parameters**, **Transactions** (incl. `minR_before` / `minR_after`), **Summary**.
