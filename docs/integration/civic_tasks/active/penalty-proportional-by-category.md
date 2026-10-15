---
status: active
slug: penalty-proportional-by-category
owner_repo: both
created: 2026-10-15
updated: 2026-10-15
---

# Proportional Merit penalty across ME sources

**Product:** Merit penalties must not be attributed to a single ME source
(Volunteer). Split `penalty` across every **non-zero** Civic bucket
(taxes / charity / volunteer) in proportion to that bucket’s pre-penalty
Merits. Civic already remaps `by_source` this way; Rovas `by_category` must
match so fallbacks and merit-index snapshots stay consistent.

**Owner:** `both` — Civic remap done in `@civic/shared` `categoriesFromMeritSources`;
Rovas still needs `rovas_merit_user_by_category()` (+ snapshot rebuild).

### Contract

No new endpoint. Same fields:

| Field | Behaviour |
|-------|-----------|
| `by_source.penalty` | Unchanged (signed ledger sum; usually negative). |
| `by_category` | After mapping earned sources (`tax_payment`→taxes, `charitable_donation`→charity, `volunteer_work`+`volunteer`→volunteer), allocate `penalty` **proportionally** across buckets with pre-penalty value **≠ 0**. Prefer positive bases as weights; if none, use `abs` of non-zero bases; if still none, equal thirds. Bucket totals **may be negative**. |
| `total_me` | Unchanged (wallet). Sum of remapped `by_category` should match ledger net (modulo rounding). |

Do **not** fold all of `penalty` into `volunteer`.

**Example (prod uid 2138 / `LAO`, 2026-10-15):**

| `by_source` | ME |
|-------------|-----|
| `charitable_donation` | 8.93 |
| `volunteer_work` | 7.53 |
| `penalty` | −10 |

→ `by_category`: taxes **0**, charity **≈3.50**, volunteer **≈2.96** (not volunteer −2.47).

### Checklist

1. [x] Update `rovas_merit_user_by_category()` in `rovas_merit.ledger.inc` to apply the same proportional penalty rules (allow negative category values; stop skipping `penalty` / `me <= 0` wholesale without allocation).
2. [x] Align any doc comments / `civic-deploy.md` table that still say “Civic folds penalty into volunteer” / “Rovas `by_category` excludes penalty”.
3. [ ] Rebuild merit-index snapshot on rovdev after deploy so stored `by_category` matches (cron or admin rebuild).
4. [ ] Smoke-test below on rovdev (and optionally prod uid 2138 after prod deploy).
5. [ ] Deploy rovdev + smoke (human unless asked)

### Smoke test

```bash
cd /var/www/rovdev && drush74 php-eval '
module_load_include("inc", "rovas_merit", "rovas_merit.ledger");
$uid = 2138; // LAO — or pick a rovdev user with penalty + ≥2 non-zero sources
print "by_source=" . json_encode(rovas_merit_user_by_source($uid)) . "\n";
print "by_category=" . json_encode(rovas_merit_user_by_category($uid)) . "\n";
$lookup = rovas_civic_users_lookup("LAO", "sk");
if ($lookup) {
  print "lookup.by_category=" . json_encode($lookup["by_category"]) . "\n";
  print "lookup.by_source=" . json_encode($lookup["by_source"]) . "\n";
}
'
# Expect: penalty not entirely in volunteer; non-zero earned sources share penalty
# by weight; taxes+charity+volunteer ≈ total_me / ledger net
```

### Civic will consume

Civic API remaps `by_source` → `by_category` in
`packages/shared/src/merit-sources.ts` (proportional penalty) and prefers that
over Rovas `by_category` when `by_source` is present. After this Rovas change,
native `by_category` and merit-index stored categories match the same rule when
`by_source` is missing or when reading Rovas JSON directly.
