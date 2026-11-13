---
status: done
slug: merit-distribution-decade-bins
owner_repo: rovas
created: 2026-11-13
updated: 2026-11-13
---

# Merit distribution: power-of-ten bin edges

**Product:** Civic Home (guest + signed-in) and Community profile histograms
show Merit-band labels under each bar (e.g. today `1.6 – 25.1 Merits`). Those
fractions come from `_rovas_merit_distribution_compute_bins()`: it floors /
ceils the log10 range of positive Merits, then **splits that range into a
fixed 10 equal-width log bins**. Civic wants **readable decade boundaries**:
`0, 1, 10, 100, 1000, …` (and matching integer labels).

**Owner:** `rovas` (binning lives only in `rovas_merit_distribution`). Civic
already formats labels from `bin_edges_log10` via `spelledBucketLabel` — no
API path change if the JSON shape stays the same.

### Contract

`GET /rovas/merit_distribution` (unchanged path / auth).

**Binning change** in `_rovas_merit_distribution_compute_bins()`:

1. Collect positive Merits as today (`merits <= 0` still excluded from bins;
   `excluded_zero` unchanged).
2. Let `max_merits = max(positive)`. Build **linear** edges:
   `[0, 1, 10, 100, …, 10^k]` where `k = max(0, ceil(log10(max_merits)))`
   and the last edge is at least one decade above the previous (so there is
   always a top open-ended / last bin).
3. Assign each positive Merit `v` to the unique interval
   `(edges[i], edges[i+1]]` (or `[edges[i], edges[i+1])` — pick one and
   document it; last bin includes everything up to / above the final edge
   as today).
4. Encode `bin_edges_log10` as log10 of those linear edges, with the first
   edge (linear `0`) as a **JSON-safe sentinel** `<= -20` (Civic treats
   `pow(10, edge) < 1e-9` as “zero lower bound”). Remaining edges are
   integers `0, 1, 2, …, k`.
5. **Variable bin count** — do **not** force
   `ROVAS_MERIT_DISTRIBUTION_BIN_COUNT` (10). Bar count = `len(edges) - 1`
   (typically ~4–8 for current community spans). Keep the constant only if
   still useful as a max cap (optional; prefer uncapped decades).
6. Bucket `label` strings should read as integers / decades (e.g. `0–1`,
   `1–10`, `10–100`, last `1000+` or `1000–10000` — match existing
   `_rovas_merit_distribution_bucket_label` style). Civic prefers its own
   spelled labels from edges; keep Rovas labels consistent for the SVG /
   admin preview.
7. `user_bucket_index` must use the same edges (`_rovas_merit_distribution_user_bin`).
8. SVG admin preview (`rovas_merit_distribution_render`) already plots
   `counts.length` bars — verify it still looks sane with fewer/more bars.
9. Bust merits cache after deploy
   (`cache_clear_all('rovas_merit_distribution:merits', 'cache')`).

**Response shape:** unchanged keys (`buckets`, `bin_edges_log10`,
`total_users`, optional `user_*`). Only edge values and `buckets.length`
change.

**Errors:** unchanged.

### Representation notes (for Civic / product)

Decade bins do **not** break the histogram; they make bands meaningful.

| Today (10 equal log slices) | After (decades) |
|-----------------------------|-----------------|
| Always 10 bars | Variable bar count (~ span of community Merits) |
| Labels like `1.6 – 25.1` | Labels like `1 – 10`, `10 – 100` |
| Can split one decade across several bars | One bar = one order of magnitude |
| Smooth-looking mid bulge | Heights may look “lumpier” (honest) |

Empty or near-empty high decades are OK if a few whales set `max_merits`.

### Checklist

1. [x] Rewrite `_rovas_merit_distribution_compute_bins` for decade edges as above
2. [x] Align `_rovas_merit_distribution_user_bin` + JSON + SVG paths
3. [x] Update module file header / README (no longer “10 equal-width bins”)
4. [x] Deploy rovdev + smoke (human unless asked); clear merits cache

### Smoke test

```bash
curl -sS "https://dev.rovas.app/rovas/merit_distribution" \
  -H "Accept: application/json" \
  | jq '{
      n: (.buckets|length),
      edges: .bin_edges_log10,
      labels: [.buckets[].label],
      counts: [.buckets[].count]
    }'

# Expect: HTTP 200; bin_edges_log10 first ≤ -20 (linear 0), then consecutive
# integers 0,1,2,…; linear 10^edge for i≥1 are 1,10,100,…; bucket counts
# sum to previous positive-Merit population; no fractional labels like 1.6–25.1.
```

### Civic will consume

`GET /rank/distribution` → Home `RankDistributionChart` / `RankPanel` and
Community profile. Labels via `@civic/shared` `spelledBucketLabel` (show
`0 – 1 Merits` when lower edge ≈ 0). No Civic API change required once
edges are decades.
