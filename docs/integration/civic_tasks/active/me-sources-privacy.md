---
status: active
slug: me-sources-privacy
owner_repo: both
created: 2026-10-31
updated: 2026-10-31
---

# ME sources privacy (detail vs aggregate)

**Product:** Users choose how much Merit-source detail other Civic users may
see. Goal: make inferring tax burden (and therefore income) hard without hiding
total ME or volunteer contribution.

Two **detail levels**:

| Level | What others see |
|-------|-----------------|
| **detailed** (default) | Today’s three buckets (Taxes / Gifts / Volunteer) + tax months + top charities + volunteer projects / NACE |
| **aggregate** | Taxes+Gifts merged into one bucket (`taxes_and_gifts`); Volunteer unchanged with drill-downs; no tax months / top charities |

**Share preference** (who gets detailed), default **everyone**:

| Value | Meaning |
|-------|---------|
| `everyone` (A) | Others always see detailed |
| `reciprocal` (B) | Others see detailed only if the **viewer’s** share is `everyone` or `reciprocal` (not `aggregate`) |
| `aggregate` (C) | Others always see aggregate |

The subject **always** sees their own detailed data (self lookup, own merit
history, own receipts). Internally Rovas keeps full ledger detail forever;
obfuscation is response-time only.

**ME•tre:** when `source_category` is `taxes` or `charity`, omit users the
viewer may only see as aggregate; Civic shows an explainer under the filter.
Filters **all** / **volunteer** include everyone (bars obfuscated when needed).

**Owner:** `both` — Rovas stores the preference (Civic API **and** account edit
`/user/%/edit`), resolves + obfuscates public ME-mix APIs, and hides
`field_merit_source` on ledger node pages when the viewer may only see
aggregate; Civic proxies and builds Profile UI + Community / ME•tre display.
Rovas `/user/%/merits` is unchanged (no tax/gift column).

### Contract

#### 1. Preference value

Durable store: `{civic_user_prefs}.me_sources_share`.

| Value | Notes |
|-------|--------|
| missing / unset | Treat as **`everyone`** |
| `everyone` | A |
| `reciprocal` | B |
| `aggregate` | C |

#### 2. Resolve detail level (per request)

```
if subject_uid === viewer_uid → detailed
else if subjectPref === everyone (or unset) → detailed
else if subjectPref === aggregate → aggregate
else /* reciprocal */ → detailed iff viewer_uid ≥ 1 AND viewerPref ≠ aggregate
```

Anonymous / no Bearer: `viewer_uid = 0` → reciprocal subjects resolve to
**aggregate**.

#### 3. `GET /rovas/me` (extend)

| Field | Type | Notes |
|-------|------|--------|
| `me_sources_share` | `"everyone"` \| `"reciprocal"` \| `"aggregate"` | Always present; default `everyone` when unset |

#### 4. `POST /rovas/civic_me_sources_preference`

| | |
|--|--|
| Method | `POST` |
| Path | `/rovas/civic_me_sources_preference` |
| Auth | Bearer (OAuth2 `civic` client); API-key auth **disabled** |
| Content-Type | `application/json` |

**Body:** `{ "share": "everyone" | "reciprocal" | "aggregate" }`

**Response (200):** `{ "me_sources_share": "reciprocal" }`

**Errors:** `400`/`422` invalid body; `401` missing/invalid Bearer.

#### 5. Public ME-mix payloads (`users_lookup`, merit index rows, period index)

Add resolved `me_sources_detail`: `"detailed"` | `"aggregate"`.

When **aggregate**:

- `by_category.taxes` = `0`, `by_category.charity` = `0`
- `by_category.taxes_and_gifts` = taxes + charity (after existing penalty remap)
- `by_category.volunteer` unchanged
- Empty `taxes_last_3_months` and `top_charities` (lookup only)
- Strip `by_source` keys that reveal the tax/gift split (`tax_payment`,
  `charitable_donation`); keep volunteer-related keys only (or omit
  `by_source`) so Civic remapping cannot undo obfuscation
- Keep `total_me`, volunteer drill-downs, NACE

When **detailed**: current shape + `me_sources_detail: "detailed"` (omit
`taxes_and_gifts` or leave unset).

Self-lookup (Bearer username matches subject) → always detailed.

#### 6. ME•tre filters / spotlight

- Lifetime `civic_merit_index` and `civic_merit_period_index`: when
  `source_category` is `taxes` or `charity`, **exclude** subjects resolved as
  aggregate for this viewer. Set `privacy_filter_active: true` on the response
  when that filter is applied.
- Spotlight: do not elect tax/charity leads who resolve as aggregate for the
  viewer; volunteer leads unchanged.
- `GET /rovas/civic_merit_history` remains Bearer-self-only and **unchanged**.
  Any future public ledger must reuse the same resolver.

### Checklist

1. [x] Schema: `{civic_user_prefs}.me_sources_share` + update hook (`7030`)
2. [x] Get/store helpers + resolve + obfuscate helpers (`rovas_civic.me_sources.inc`)
3. [x] Expose on `GET /rovas/me`; `POST /rovas/civic_me_sources_preference`
4. [x] Obfuscate `users_lookup` for viewer
5. [x] Merit index + period index: obfuscate rows; exclude on taxes/charity filter
6. [x] Spotlight: skip aggregate tax/charity leads
7. [x] Web: hide `field_merit_source` on ledger nodes when viewer resolves aggregate (Merits tab unchanged)
8. [x] Web: account edit (`/user/%/edit`) radios for `me_sources_share` (same store as Civic API)
9. [ ] Deploy rovdev + smoke (human unless asked)

### Smoke test

```bash
TOKEN_A=…  # user A (share=everyone)
TOKEN_C=…  # user C (share=aggregate)

curl -sS -X POST https://dev.rovas.app/rovas/civic_me_sources_preference \
  -H "Authorization: Bearer $TOKEN_C" -H "Content-Type: application/json" \
  -d '{"share":"aggregate"}'
# → {"me_sources_share":"aggregate"}

curl -sS -H "Authorization: Bearer $TOKEN_A" -H "Accept: application/json" \
  "https://dev.rovas.app/rovas/users_lookup?username=USER_C"
# → me_sources_detail=aggregate; taxes=0; charity=0; taxes_and_gifts set;
#    taxes_last_3_months=[]; top_charities=[]

curl -sS -H "Authorization: Bearer $TOKEN_C" -H "Accept: application/json" \
  "https://dev.rovas.app/rovas/users_lookup?username=USER_C"
# → me_sources_detail=detailed (self)

curl -sS -H "Authorization: Bearer $TOKEN_A" \
  "https://dev.rovas.app/rovas/civic_merit_index?source_category=taxes&limit=20"
# → privacy_filter_active=true; USER_C absent from items
```

### Civic will consume

- Proxy `me_sources_share` on `GET /me`; setter `POST /me/me-sources-preference`.
- Profile A/B/C control; Community `ContributionBreakdown` aggregate tabs;
  ME•tre bar + filter explainer.
