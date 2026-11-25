---
status: active
slug: chr-me-top-volunteer-attribution
owner_repo: rovas
created: 2026-11-25
updated: 2026-11-25
---

# Attribute CHR→ME to `top_volunteer_projects`

**Product:** Civic Home / Community **Volunteer** tab shows
`by_category.volunteer` (ledger `volunteer_work` + `volunteer`) and under it
“Top projects by Merits earned” from `users_lookup.top_volunteer_projects`.

Today those diverge for users who earned Chrons on projects and then converted
them to Merits (`POST /rovas/civic_give_chrons` / A6 forex, source
`volunteer`):

| Flow | Counts in Volunteer **tab ME**? | Appears in `top_volunteer_projects`? |
|------|----------------------------------|--------------------------------------|
| WR / co_worker ledger (`volunteer_work`) | Yes | Yes (ledger via receipt / WR) |
| CHR→ME forex (`volunteer`) | Yes | **No** — intentionally excluded |

**Repro (prod, 2026-11-25):** username **`zinderMT`** (uid **3623**)

| Field | Observed |
|-------|----------|
| `total_me` | **8 ME** (~8 €) |
| Volunteer tab | **8 ME** (Taxes/Gifts 0) |
| `top_volunteer_projects` | **[]** → Civic empty “Zatiaľ žiadne verejné dobrovoľnícke projekty.” |
| NACE expertise | Present (work was reported) |
| Chron origin | All Chrons earned on **Openstreetmap**; converted to ME via Give Chrons |

Expertise icons prove WRs exist; project rows stay empty because
`rovas_civic_users_lookup_top_volunteer_projects()` only sums
`volunteer_work` (see [volunteer-tab-vs-top-projects](../done/volunteer-tab-vs-top-projects.md)
and historical “Out of scope” for CHR→ME). Product intent has changed: when
Volunteer ME comes from converted Chrons earned on a project, that project
must show under the Volunteer drawer (same class of bug as Gifts buy-ME →
[gifts-top-charities-attribution](../done/gifts-top-charities-attribution.md)).

**Owner:** `rovas` — broaden `top_volunteer_projects` attribution. Civic already
renders the array; no Expo/API shape change expected.

### Contract

No new endpoint. Existing:

- `GET /rovas/users_lookup?username=&lang=` (Bearer)
- Fields unchanged: `top_volunteer_projects[]` =
  `{ project_nid, name, me }` (up to 3), localized `name` via existing helper

**Semantics change for `top_volunteer_projects`:** include project-attributable
Merits from ledger source **`volunteer`** (CHR→ME / forex), not only
`volunteer_work`.

| ME origin | How to attribute `project_nid` |
|-----------|--------------------------------|
| `volunteer_work` (shareholder + WR-linked ledger) | Unchanged — current SQL in `rovas_civic_users_lookup_top_volunteer_projects()` |
| `volunteer` (CHR→ME / A6 forex, incl. Give Chrons and WR `reward_kind=merits` settle) | Distribute the user’s **`by_source.volunteer`** ME across projects **proportional to Chron grants** the user earned from work reports on those projects |

**Weights (Chron grants per project):** sum positive Chron amounts granted to
this uid for WRs on each project (prefer chron ledger `wr_grant` /
equivalent if reliable; else `field_chrons_to_grant` on verified/rewarded
WRs authored by the uid, falling back to `hours × 10` when the Chron field
is empty — same rule as reward helpers). Private projects stay eligible the
same way as today’s volunteer_work path (Civic already only lists what Rovas
returns).

**Merge:** for each `project_nid`, `me` = volunteer_work attribution +
proportional `volunteer` share. Sort by `me` DESC, take top 3, round as today.

**If** `by_source.volunteer` > 0 but the user has **no** Chron-grant weights
(no WR Chron history), leave that ME unattributed (do not invent a project).
Do **not** put CHR→ME into `top_charities`.

**Invariant (unchanged intent):**

| Quantity | Rule |
|----------|------|
| Without penalty | `sum(top_volunteer_projects[].me) ≤ by_category.volunteer` |
| CHR→ME-only user with all Chrons from one project | That project’s `me` ≈ `by_source.volunteer` (≈ tab ME) |
| Mix of `volunteer_work` + `volunteer` | Project rows include both; tab may still exceed sum if some `volunteer` ME remains unattributable |

No requirement to rewrite forex ledger rows or change
`field_merit_source` on Give Chrons (`volunteer` stays correct for category
tabs).

### Checklist

1. [x] Extend `rovas_civic_users_lookup_top_volunteer_projects()` to add
      proportional `by_source.volunteer` ME using WR Chron-grant weights
      (`rovas_civic_users_lookup_merge_chr_me_volunteer_projects` +
      `rovas_civic_users_lookup_wr_chron_grant_weights` — chron `wr_grant`
      first, else verified WR `field_chrons_to_grant` / hours×10)
2. [x] Keep existing `volunteer_work` shareholder + WR-linked paths
3. [x] Keep envelope `{ project_nid, name, me }` ≤ 3; `lang` titles unchanged
4. [ ] Confirm invariant `sum(top) ≤ by_category.volunteer` for zinderMT + a
      mixed user (e.g. Jožo Sečík / Filip009) — after deploy smoke
5. [ ] Deploy rovdev + smoke (human unless asked)

### Smoke test

```bash
# On the environment that serves the Civic profile (usually /var/www/rovdev
# first; prod repro is zinderMT on rovas.app).
cd /var/www/rovdev && drush74 php-eval '
$u = user_load_by_name("zinderMT");
if (!$u) { print "user not found\n"; return; }
$uid = (int) $u->uid;
module_load_include("inc", "rovas_merit", "rovas_merit.ledger");
print "uid=$uid\n";
print "by_source=" . json_encode(rovas_merit_user_by_source($uid)) . "\n";
print "by_category=" . json_encode(rovas_merit_user_by_category($uid)) . "\n";
$top = rovas_civic_users_lookup_top_volunteer_projects($uid, "sk");
print "top_volunteer_projects=" . json_encode($top, JSON_UNESCAPED_UNICODE) . "\n";
$sum = 0.0;
foreach ($top as $row) { $sum += (float) $row["me"]; }
$vol = (float) rovas_merit_user_by_category($uid)["volunteer"];
print "sum_top=$sum by_category.volunteer=$vol delta=" . ($vol - $sum) . "\n";
'
# Expect zinderMT: Openstreetmap in top_volunteer_projects with me ≈ 8
# (or ≈ by_source.volunteer); sum_top <= by_category.volunteer; empty [] gone.

TOKEN=…  # OAuth Bearer with user_profile
curl -sS "https://dev.rovas.app/rovas/users_lookup?username=zinderMT&lang=sk" \
  -H "Authorization: Bearer $TOKEN" -H "Accept: application/json" \
  | python3 -c "
import json,sys
d=json.load(sys.stdin)
print('total_me', d.get('total_me'))
print('by_category', d.get('by_category'))
print('by_source', d.get('by_source'))
for p in d.get('top_volunteer_projects') or []:
  print(p)
"
```

### Civic will consume

`ContributionBreakdown` / `ProjectMeTable` already show
`top_volunteer_projects`. After Rovas deploy, pull-to-refresh /
re-open `@zinderMT` should list **Openstreetmap** under Volunteer instead of
the empty state. No Civic contract change unless Rovas needs a new diagnostic
field (avoid).
