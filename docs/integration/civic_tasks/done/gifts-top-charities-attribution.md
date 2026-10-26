---
status: active
slug: gifts-top-charities-attribution
owner_repo: both
created: 2026-10-13
updated: 2026-10-13
---

# Gifts `top_charities`: EUR buy-ME project + timeshare attribution

**Product:** Civic’s Community / Home contribution drawer shows a **Gifts**
(Dary) tab total from ledger `charitable_donation`, and under it “Top
organisations by contribution” from `users_lookup.top_charities`.

Today those two diverge:

| Flow | Counts in Gifts **tab ME**? | Appears in `top_charities`? |
|------|-----------------------------|-----------------------------|
| Path-1 EUR **project donation** | Yes | Yes (`rovas_merit_issuance` / `project_reward_order`) |
| EUR **buy-Merits** (`purchase_merits`) | Yes | **No** — no project, no issuance row |
| EUR **timeshare** ME (shareholder split) | Yes | **No** — ledger only, no issuance |

Example: `@emenky` (uid 3626) has **250 ME** Gifts from one EUR buy-Merits
receipt (`purchase_merits` / `MP - …`) and `top_charities: []` → Civic empty
state “Zatiaľ žiadne verejné neziskové projekty.”

**Needed:**

1. **EUR buy-Merits** — Merits purchased with EUR should be attributed to an
   **admin-configurable Rovas project** (default: the platform **Rovas**
   project). After grant, that project must show up in `top_charities` for
   the buyer.
2. **EUR timeshare** — When a user earns Merits from a purchased timeshare
   on a project, that **project** must appear in `top_charities` (summed
   with other Gifts attributions). Purchasing the share alone does not
   create ME; listing happens when timeshare ME is actually disbursed.

**Owner:** `both` (Rovas implements attribution + `top_charities`; Civic
already renders `top_charities` — no API shape change expected if the array
is populated. Optional Civic copy tweak if empty state still overstates
“public nonprofit”.)

### Contract

No new endpoint. Existing:

- `GET /rovas/users_lookup?username=&lang=` (Bearer)
- Fields unchanged: `by_source.charitable_donation` / remapped
  `by_category.charity`, `top_charities[]` =
  `{ project_nid, name, me }` (up to 3, exclude State Treasury)

**Semantics change for `top_charities`:** include every project-attributable
`charitable_donation` Merit that Civic should show under Gifts, not only
Path-1 `project_reward_order` issuance:

| Source | `project_nid` |
|--------|----------------|
| Path-1 donor ME | Project that received the EUR donation (existing) |
| EUR buy-Merits | Admin-configured buy-ME destination project |
| Timeshare ME | Project on the `timeshare_merit_allocation` / reward |

Do **not** put CHR→ME (`volunteer`) or co_worker labor ME into
`top_charities`.

### Checklist

1. [x] **Admin setting** — `rovas_civic_eur_buy_merits_project_nid` on
       `admin/config/rovas/civic`. Default / seeded = platform **Rovas**
       project **nid 1992** via `rovas_civic_update_7027` /
       `rovas_civic_ensure_eur_buy_merits_project()`.
2. [x] **EUR buy-Merits grant** — after `_rovas_forex_grant_merits(...,
       'charitable_donation')` ledger apply, `rovas_civic_attribute_eur_buy_merits()`
       inserts `{rovas_merit_issuance}` with `source_type = eur_buy_merits`,
       `source_id = purchase_merits:{nid}` (idempotent). Does **not**
       overload Path-1 `project_reward_order` or re-grant Merits.
3. [x] **Broaden `rovas_civic_users_lookup_top_charities()`** — union / sum:
       - existing `project_reward_order` issuance (unchanged)
       - `eur_buy_merits` issuance for the configured project
       - timeshare `field_merits_earned` on `timeshare_merit_allocation`
       Still: `amount > 0`, top 3 by ME, exclude State Treasury, localized
       `name` via existing title helper. CHR→ME / co_worker excluded.
4. [x] **Historical backfill (buy-ME)** — `rovas_civic_update_7027` +
       `drush rovas-civic-backfill-eur-buy-merits` (`--uid=` optional).
       Writes issuance to the **current** configured project for EUR
       `purchase_merits` → `charitable_donation` leaves with no attribution.
5. [x] **Timeshare:** same-project Path-1 + timeshare ME summed once per
       `project_nid` (PHP merge in `top_charities`).
6. [ ] Deploy rovdev + smoke (human unless asked)

### Smoke test

```bash
# After deploy + drush74 updb (7027) on /var/www/rovdev:

# 1) Admin: confirm buy-ME project nid 1992 (or set on admin/config/rovas/civic).

# 2) Historical: @emenky should populate after backfill.
TOKEN=… # Civic OAuth Bearer
curl -sS -H "Authorization: Bearer $TOKEN" \
  "https://dev.rovas.app/rovas/users_lookup?username=emenky&lang=sk" \
  | jq '{ charity: .by_source.charitable_donation, top_charities }'
# Expect: charitable_donation ≈ 250 and top_charities with project_nid "1992".

# 3) EUR buy-Merits (test user): after paid order / grant,
#    users_lookup includes top_charities entry for configured project.
curl -sS -H "Authorization: Bearer $TOKEN" \
  "https://dev.rovas.app/rovas/users_lookup?username=BUYER&lang=sk" \
  | jq '{ charity: .by_source.charitable_donation, top_charities }'

# 4) EUR timeshare: buy time-share on project P, then cause project reward
#    so timeshare ME is disbursed as charitable_donation.
#    users_lookup for shareholder: top_charities contains P (or P in top 3).
#    Purchasing the share alone with no ME yet → no requirement to list P.

# 5) Regression: Path-1 project donation still appears; State Treasury
#    still excluded; CHR→ME never in top_charities.

# Optional re-backfill:
#   drush74 -r /var/www/rovdev rovas-civic-backfill-eur-buy-merits
#   drush74 -r /var/www/rovdev rovas-civic-backfill-eur-buy-merits --uid=3626
```

### Response shape (unchanged)

```json
{
  "by_source": { "charitable_donation": 250.0 },
  "top_charities": [
    { "project_nid": "1992", "name": "Rovas", "me": 250.0 }
  ]
}
```

### Civic will consume

- Mobile already binds Gifts drawer to `profile.top_charities`
  (`ContributionBreakdown` + `ProjectMeTable`). No client change required
  once Rovas returns rows.
- Optional follow-up in Civic: soften `community.charity_empty` copy
  (it currently implies “no public nonprofits” even when the gap is
  attribution, not catalog privacy).
- Civic companion: `~/Documents/homeProjects/civic/docs/rovas-integration.md`
  — `GET /users/lookup` note for this task.

### Out of scope

- Changing what counts as `charitable_donation` in the ledger / tab total.
- Showing projects for zero-ME timeshare holdings (share purchased, not yet
  rewarded).
- CHR-funded Merit purchase (`volunteer` source).
