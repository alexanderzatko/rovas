---
status: done
slug: civic-claims-filters
owner_repo: both
created: 2026-10-31
updated: 2026-10-31
---

# Civic claims list filters (receipt history search)

**Product:** Civic’s receipt history screen needs server-side search over saved
claims by **merchant name**, **purchase date range**, and (submitted view only)
**VAT € range**. Filters must run in Rovas before `limit`/`offset` so `total`
stays correct. Only fields already stored on `{civic_claims}` — no gross total
or line-item search.

**Owner:** `both` — Rovas filters `GET /rovas/civic_claims`; Civic proxies the
same params on `GET /receipts` and builds the mobile filter UI.

### Contract

Extend existing `GET /rovas/civic_claims` (Bearer; keep `view`, `limit`,
`offset`, `receipt_uid`). Empty / omitted filter params are ignored. Unknown
query params remain ignored (forward-compatible).

| Param | Type | Rules |
|-------|------|--------|
| `merchant` | string | Optional. Case-insensitive substring match on `merchant_name`. Rows with `NULL` merchant never match. |
| `date_from` | string | Optional. ISO date `YYYY-MM-DD`. Inclusive via string compare on stored varchar ISO `purchase_at`: `purchase_at >= YYYY-MM-DD` (no TZ reinterpretation). Rows with `NULL` `purchase_at` never match. |
| `date_to` | string | Optional. ISO date `YYYY-MM-DD`. Inclusive via exclusive upper bound: `purchase_at <` day-after as `YYYY-MM-DD`. |
| `min_vat_eur` | number | Optional. `vat_eur >=` value. Apply only when `view=submitted`. When `view=received`, ignore (do not 400). |
| `max_vat_eur` | number | Optional. `vat_eur <=` value. Same received-view ignore rule. |

**AND** all provided filters together with the existing view scope.

**Date bounds:** `{civic_claims}.purchase_at` is a client-supplied ISO string
(varchar), not a DB datetime. Filters compare that string lexicographically
against calendar `YYYY-MM-DD` prefixes — consistent with how values are stored,
without converting to UTC or site TZ.

**Response (200):** unchanged shape:

```json
{
  "items": [ /* same submitted or received item shape as today */ ],
  "total": 12,
  "limit": 50,
  "offset": 0,
  "view": "submitted"
}
```

| Field | Notes |
|-------|--------|
| `total` | Count of rows matching **filters + view**, not the unfiltered claim count |
| `items` | Page after filter, then `LIMIT`/`OFFSET` |

Invalid `date_from` / `date_to` (not `YYYY-MM-DD`) or non-numeric VAT bounds:
return **400** with a short message so Civic can surface bad input.

`receipt_uid=` lookup mode is unchanged (status-only payload); filter params
have no effect in that mode.

### Checklist

1. [x] Parse optional filter query params on `GET /rovas/civic_claims` list mode
2. [x] Apply merchant / date / VAT (`submitted` only) filters before count + page
3. [x] Ensure scrubbed (`NULL`) merchant/purchase_at rows do not match those filters
4. [x] Deploy rovdev + smoke (human unless asked)

### Smoke test

```bash
# Replace TOKEN; use a uid that has several claims
curl -sS -H "Authorization: Bearer TOKEN" \
  'https://dev.rovas.app/rovas/civic_claims?view=submitted&limit=10&merchant=Lidl'

curl -sS -H "Authorization: Bearer TOKEN" \
  'https://dev.rovas.app/rovas/civic_claims?view=submitted&date_from=2026-01-01&date_to=2026-12-31&min_vat_eur=1&max_vat_eur=50'

# received: merchant/date work; VAT params ignored
curl -sS -H "Authorization: Bearer TOKEN" \
  'https://dev.rovas.app/rovas/civic_claims?view=received&merchant=Lidl&min_vat_eur=1'
```

Expect `total` ≤ unfiltered total; every returned row satisfies the filters.

### Civic will consume

- Proxies filters on `GET /receipts?view=&merchant=&date_from=&date_to=&min_vat_eur=&max_vat_eur=&limit=&offset=`
- Mobile receipts history filter strip (merchant, dates, VAT on submitted tab)
- Companion notes: `~/Documents/homeProjects/civic/docs/rovas-integration.md`
