---
status: done
slug: merit-period-index
owner_repo: both
created: 2026-09-26
updated: 2026-10-02
---

# Merit period index (ME•ter time dimension)

**Product:** Civic ME•ter (Impact) ranks contributors in the main table with a
**time** dimension: All time | Yesterday | Last week | Last month. Default =
**All time** (existing `civic_merit_index` snapshot). Period tabs need a
paginated leaderboard of Merits *earned in* the window (same windows as
spotlight), not lifetime wallet.

Reuse the ledger aggregation already used by
`civic_merit_spotlight` (`rovas_civic_merit_spotlight_aggregate` + period
windows). Do **not** change the lifetime Merit Index snapshot path.

**Timezone / windows:** identical to spotlight (`Europe/Bratislava`):

| `period` | Window |
|----------|--------|
| `day` | Previous calendar day (yesterday) |
| `week` | Last 7 calendar days ending yesterday (inclusive) |
| `month` | Previous calendar month |

### Contract

`GET /rovas/civic_merit_period_index`

| Query | Required | Notes |
|-------|----------|--------|
| `period` | yes | `day` \| `week` \| `month` |
| `limit` | no | Default 50, max 100 |
| `offset` | no | Default 0 |
| `sort` | no | `total_me` (default) \| `taxes_me` \| `charity_me` \| `volunteer_me` \| `position` |
| `order` | no | `asc` \| `desc` (default `desc`) |
| `country_iso2` | no | ISO-2; same country source as Merit Index / spotlight |
| `source_category` | no | `taxes` \| `charity` \| `volunteer` — keep only users with that category’s period ME rounding to &gt; 0; when sorting by `total_me` with a source set, sort key is that category’s ME |
| `lang` | no | `sk` \| `en` for `country_label` |

**Auth:** public read (same as Merit Index). Bearer optional / unused for v1.

**Aggregation:** same as spotlight — positive ledger credits in the window;
category map `tax_payment`→taxes, `charitable_donation`→charity,
`volunteer_work`+`volunteer`+`penalty`→volunteer; hide blocked/inactive
users and lifetime `total_me < 0`. Omit users whose **period** ME rounds to
≤ 0 (`round(earned_me) <= 0`) so Civic does not show “0 ME” rows.

**Row shape:** same fields as `civic_merit_index` items so Civic can reuse the
Merit Index table UI:

| Field | Period meaning |
|-------|----------------|
| `total_me` | ME earned in the window (not lifetime balance) |
| `by_category` | Period ME by taxes / charity / volunteer |
| `by_source` | Optional; empty `{}` is fine for v1 |
| `nace_expertise` | Empty `[]` for v1 (NACE filter disabled on Civic when period ≠ all-time) |
| `value_eur` | `0` for v1 (optional later) |
| `member_since_year` | Account creation year (same as index) |
| `username` / `display_name` / `country_*` | Same helpers as spotlight / index |

**Response 200:**

```json
{
  "total": 12,
  "limit": 50,
  "offset": 0,
  "sort": "total_me",
  "order": "desc",
  "period": "day",
  "period_start": "2026-09-26",
  "period_end": "2026-09-26",
  "items": [
    {
      "position": 1,
      "rovas_uid": "1481",
      "username": "ann",
      "display_name": "Ann",
      "member_since_year": 2024,
      "total_me": 42.5,
      "value_eur": 0,
      "country_iso2": "SK",
      "country_label": "Slovakia",
      "by_category": { "taxes": 42.5, "charity": 0, "volunteer": 0 },
      "by_source": {},
      "nace_expertise": []
    }
  ]
}
```

**Errors:** `400` invalid `period` / `source_category` / `country_iso2`;
`503` ledger query failure.

### Checklist

1. [x] Services resource `civic_merit_period_index` on `rovas_endpoint` (OAuth2 `civic`; API-key off; public GET)
2. [x] Reuse spotlight period window + aggregate; rank / paginate / filter as above
3. [x] Enrich page slice with usernames, display names, member_since_year, country labels
4. [x] Enable via `rovas_civic_ensure_services_resources` + `drush74 updb` hook
5. [x] Deploy rovdev (and prod when asked)

### Smoke test

```bash
curl -sS "https://dev.rovas.app/rovas/civic_merit_period_index?period=week&country_iso2=SK&limit=5" \
  -H "Accept: application/json" \
  | jq '{total, period, period_start, period_end, first: .items[0] | {username, total_me, by_category}}'

# Expect: HTTP 200; items[].total_me rounds to ≥ 1; positions 1..n.
# With source_category=taxes: only taxes earners; sort by taxes_me when requested.
```

### Civic will consume

Civic `GET /rank/index?period=day|week|month&…` proxies this endpoint when
`period` is set; without `period` it keeps proxying lifetime
`civic_merit_index`. Mobile ME•ter: time chips (All time default) + existing
table; NACE filter only for All time. Home spotlight unchanged.
