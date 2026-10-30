---
status: done
slug: merit-spotlight-today
owner_repo: both
created: 2026-10-30
updated: 2026-10-30
---

# Merit spotlight `period=today`

**Product:** Civic Home shows a live “today” ticker (ME registered + active
earners on the current Europe/Bratislava calendar day, filtered by country).
Existing `period=day` remains **yesterday** (ME•ter / legacy spotlight chips).

**Owner:** `both`

### Contract

`GET /rovas/civic_merit_spotlight`

| Query | Required | Notes |
|-------|----------|--------|
| `period` | yes | `today` \| `day` \| `week` \| `month` |
| `country_iso2` | no | ISO-2; Civic Home ticker defaults to `SK` |
| `lang` | no | unused in payload; accept for consistency |

| `period` | Window (Europe/Bratislava) |
|----------|----------------------------|
| `today` | Current calendar day so far (`period_start` = `period_end` = today) |
| `day` | Previous calendar day (yesterday) — unchanged |
| `week` | Last 7 calendar days ending yesterday — unchanged |
| `month` | Previous calendar month — unchanged |

**Auth / aggregation / response shape:** unchanged from the existing spotlight
contract (`community_earned_me`, `active_earners`, `leads`, optional `you`).
`period_index` stays `day`\|`week`\|`month` only (no ME•ter “today” chip).

**Errors:** `400` invalid `period` (message lists `today`); `503` ledger failure.

### Checklist

1. [x] Allow `today` in `rovas_civic_services_merit_spotlight_index`
2. [x] Window math in `rovas_civic_merit_spotlight_period_window` for `today`
3. [x] Deploy rovdev + smoke (human unless asked)

### Smoke test

```bash
curl -sS "https://dev.rovas.app/rovas/civic_merit_spotlight?period=today&country_iso2=SK" \
  -H "Accept: application/json" \
  | jq '{period, period_start, period_end, community_earned_me, active_earners}'

# Expect: HTTP 200; period=today; period_start == period_end == Bratislava today;
# community_earned_me ≥ 0; active_earners ≥ 0.
# period=day still returns yesterday only.
```

### Civic will consume

Civic `GET /rank/spotlight?period=today&country_iso2=SK` → Home ticker
(`community_earned_me` + `active_earners`), polled ~1 min with manual refresh.
