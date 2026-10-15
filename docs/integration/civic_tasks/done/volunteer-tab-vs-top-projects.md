---
status: active
slug: volunteer-tab-vs-top-projects
owner_repo: rovas
created: 2026-10-15
updated: 2026-10-15
---

# Investigate Volunteer tab ME < top_volunteer_projects sum

**Product:** On Civic Community / Home `ContributionBreakdown`, the
**Volunteer** tab total (`by_category.volunteer`) should not be less than the
sum of `top_volunteer_projects[].me` (modulo rounding). Product intent from
[top_volunteer_projects include WR rewards](../done/_historical-pre-split.md#next-work-top_volunteer_projects-include-wr-rewards)
and [co_worker %-shares…](../done/_historical-pre-split.md#next-work-co_worker-shares-misclassified-as-charity):
tab ME ≈ project list (CHR→ME may make the **tab larger**; it must not make
projects larger than the tab without an explained cause).

**Repro (Civic UI, 2026-10-15):** username **`Filip009`**

| Field | Observed (prod screenshot) | rovdev uid **2281** |
|-------|----------------------------|---------------------|
| `total_me` | **155** | **158.67** |
| `by_category` | taxes **0**, charity **148**, volunteer **7** | taxes **0**, charity **147.28**, volunteer **11.7** |
| `top_volunteer_projects` | Openstreetmap **17** ME | Openstreetmap **21.39** (before fix) |
| `by_source.penalty` | — | **0** |

Volunteer tab &lt; project row — inverted vs expected. Staff: never
Merit-penalized; confirmed `penalty_ledger == 0` on rovdev.

**Owner:** `rovas` — investigate + fix query / ledger inference as needed.
Civic already displays whatever `users_lookup` returns; no Expo contract
change unless the response shape must grow (e.g. diagnostic fields — avoid
unless useful).

### Root cause (rovdev, 2026-10-15)

Not penalty. Not double-count of WR + allocation.

| Source | ME |
|--------|-----|
| `co_worker_merit_allocation` on Openstreetmap (nid 1998) `field_merits_earned` | **21.39** (152.9 h, share 0%) |
| Ledger `volunteer_work` attributed to OSM via `field_receipt_project` | **7.48** |
| `timeshare_merit_allocation` on “Rovas connector pre JOSM” (35259) | **147.28** (= `by_source.charitable_donation`) |
| WR-linked OSM disbursements | **0** |

`top_volunteer_projects` summed stale/inflated allocation
`field_merits_earned`, which can exceed what the ledger actually credited.
Charity tab was correct (timeshare). Volunteer tab matched the ledger;
the project row was wrong.

Jožo Sečík (uid 1481): old allocation+WR totals already matched ledger
(~490); switching attribution to ledger keeps the same top rows.

### Fix

`rovas_civic_users_lookup_top_volunteer_projects()` now attributes **ledger**
`volunteer_work` (or empty source) to projects via:

1. Shareholder path: parent receipt `field_receipt_project` (exclude receipts
   that have `field_rewarded_report`)
2. WR path: parent receipt `field_rewarded_report` → WR → `field_project`

No longer reads `project_merit_allocation` /
`co_worker_merit_allocation.field_merits_earned`. Invariant:
`sum(top_volunteer_projects[].me) ≤ by_source.volunteer_work` (and ≤
`by_category.volunteer` when CHR→ME is zero).

### Contract (invariant)

No new endpoint. Expected relationship on
`GET /rovas/users_lookup?username=` (and Civic `GET /users/lookup`):

| Quantity | Rule |
|----------|------|
| `by_source.penalty` | If present and ≠ 0, document how much hits `volunteer` after proportional split ([penalty-proportional-by-category](../done/penalty-proportional-by-category.md)). |
| Without penalty | `sum(top_volunteer_projects[].me)` ≤ `by_source.volunteer_work` and ≤ `by_category.volunteer`. |
| With penalty only | Tab may drop below gross project rows after remap; document if needed. **Not** Filip009. |

### Checklist

1. [x] Resolve Filip009 → uid on **rovdev** (uid **2281**).
2. [x] Dump `by_source`, `by_category`, `total_me`, `top_volunteer_projects`, and raw `merit_penalty` ledger sum — penalty **0**.
3. [x] Break down Openstreetmap: co_worker allocation **21.39**, WR ledger **0**, ledger OSM volunteer_work **7.48** — allocation drift, not double-count.
4. [x] Charity **147.28** = timeshare on project 35259; OSM ME not misclassified as charity.
5. [x] Fix: ledger-based `top_volunteer_projects` (code in `rovas_civic.module`).
6. [ ] Re-smoke Filip009 + Jožo Sečík on rovdev after deploy.
7. [ ] Deploy rovdev + smoke (human unless asked).

### Smoke test

```bash
# On the environment that serves the Civic profile (usually /var/www/rovdev).
cd /var/www/rovdev && drush74 php-eval '
module_load_include("inc", "rovas_merit", "rovas_merit.ledger");
$u = user_load_by_name("Filip009");
if (!$u) { print "user not found\n"; return; }
$uid = (int) $u->uid;
print "uid=$uid total_me wallet check separately\n";
print "by_source=" . json_encode(rovas_merit_user_by_source($uid)) . "\n";
print "by_category=" . json_encode(rovas_merit_user_by_category($uid)) . "\n";
print "penalty_ledger=" . json_encode(rovas_merit_user_penalty_ledger_sum($uid)) . "\n";
$top = rovas_civic_users_lookup_top_volunteer_projects($uid, "sk");
print "top_volunteer_projects=" . json_encode($top) . "\n";
$sum = 0.0;
foreach ($top as $row) { $sum += (float) $row["me"]; }
$vol = (float) rovas_merit_user_by_category($uid)["volunteer"];
print "sum_top=$sum by_category.volunteer=$vol delta=" . ($vol - $sum) . "\n";
'
# Expect Filip009: OSM me ≈ 7.48 (not 21); sum_top <= volunteer (~11.7); penalty 0.
# Also check Jožo Sečík: top projects still ~451 + ~38 ≈ volunteer_work ~490.
```

### Civic will consume

`ContributionBreakdown` shows `by_category.volunteer` on the tab and
`top_volunteer_projects` in the drawer (`ProjectMeTable`). After the Rovas
deploy, pull-to-refresh / re-open `@Filip009` should show Volunteer tab ME
≥ Openstreetmap (and any other) project ME. No Civic code change.
