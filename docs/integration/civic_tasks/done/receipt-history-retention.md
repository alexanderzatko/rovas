---
status: done
slug: receipt-history-retention
owner_repo: both
created: 2026-10-12
updated: 2026-10-12
---

# Receipt history retention preference (Civic)

**Product:** Civic must ask users whether to **keep** unlimited receipt scanning
history (merchant / purchase time / place detail on `{civic_claims}`) or follow
the NeoFund personal-data policy (**90-day** scrub of those fields, Merit-audit
fields retained). The choice is bound to the Rovas account so it survives Civic
app delete/install. Users may reverse the decision later; switching **keep →
policy** immediately scrubs eligible aged detail and is irreversible for scrubbed
rows.

Aligned with NeoFund privacy draft §3.1 / §5: line-item / shop / time / place
detail retained 90 days then deleted or aggregated; UID (or hash), VAT, claim
outcome kept longer for audit.

**Owner:** `both` — Rovas stores the preference, exposes it on `/rovas/me`,
provides a setter + scrub/cron; Civic proxies and builds the consent modal +
Profile Taxes settings UI.

### Contract

#### 1. Preference value

| Value | Meaning |
|-------|---------|
| `null` / omitted | Not answered yet — Civic blocks claim until set; **do not** scrub yet |
| `"keep"` | Retain full scanning-history detail on claims indefinitely |
| `"policy"` | After 90 days from `purchase_at` (fallback: claim `created_at`), scrub PII history fields |

**Scrub target columns** (set to NULL): `merchant_name`, `purchase_at`, and any
future richer detail columns on `{civic_claims}`. **Do not** scrub
`receipt_uid`, VAT, shares, Merits, claim outcome, or Merit ledger rows.

Durable store: `{civic_user_prefs}` (per-uid row; survives fresh Civic install).

#### 2. `GET /rovas/me` (extend)

Add:

| Field | Type | Notes |
|-------|------|--------|
| `receipt_history_retention` | `null` \| `"keep"` \| `"policy"` | Omit or `null` until the user has answered |

#### 3. `POST /rovas/civic_receipt_history_preference`

| | |
|--|--|
| Method | `POST` |
| Path | `/rovas/civic_receipt_history_preference` |
| Auth | Bearer (OAuth2 `civic` client); API-key auth **disabled** |
| Content-Type | `application/json` |

**Body:**

| Field | Rules |
|-------|--------|
| `retention` | Required; `"keep"` \| `"policy"` |

**Response (200):**

```json
{
  "receipt_history_retention": "policy",
  "scrubbed_claims": 3
}
```

| Field | Notes |
|-------|--------|
| `receipt_history_retention` | Echo of stored value |
| `scrubbed_claims` | Count of claim rows whose detail fields were nulled in this request |

**Side effects:**

- **`keep → policy`:** immediately scrub this user’s claims whose
  `purchase_at` (else `created_at`) is strictly older than **90 days**; then
  store `"policy"`.
- **`policy → keep`:** flip flag only; **do not** restore scrubbed fields.
- **First set** (`null → keep` or `null → policy`): store value; on
  `null → policy`, scrub eligible aged claims the same way as keep→policy.
- Idempotent re-set of the same value: `scrubbed_claims` may be `0`.

**Errors:** `400`/`422` invalid body; `401` missing/invalid Bearer.

#### 4. Periodic scrub (cron)

For users with `receipt_history_retention = "policy"` only, periodically scrub
eligible claim detail (same 90-day rule). Users still at `null` are **not**
scrubbed (so the first-claim consent modal remains meaningful). Users with
`"keep"` are never scrubbed by this job.

### Checklist

1. [x] Persist `receipt_history_retention` per user (schema / field + update hook)
2. [x] Expose on `GET /rovas/me`
3. [x] Services resource `POST /rovas/civic_receipt_history_preference` (Bearer only)
4. [x] Immediate scrub on transition to `policy` (incl. first set to policy)
5. [x] Cron / periodic scrub for `policy` users
6. [x] Deploy rovdev + smoke (human unless asked)

### Smoke test

```bash
TOKEN=…  # OAuth access token for a test user on dev.rovas.app

# Before answer
curl -sS -H "Authorization: Bearer $TOKEN" -H "Accept: application/json" \
  https://dev.rovas.app/rovas/me
# → receipt_history_retention null or omitted

# Choose keep
curl -sS -X POST https://dev.rovas.app/rovas/civic_receipt_history_preference \
  -H "Authorization: Bearer $TOKEN" -H "Content-Type: application/json" \
  -H "Accept: application/json" \
  -d '{"retention":"keep"}'
# → {"receipt_history_retention":"keep","scrubbed_claims":0}

# Switch to policy (seed an aged claim with merchant_name first if testing scrub)
curl -sS -X POST https://dev.rovas.app/rovas/civic_receipt_history_preference \
  -H "Authorization: Bearer $TOKEN" -H "Content-Type: application/json" \
  -H "Accept: application/json" \
  -d '{"retention":"policy"}'
# → scrubbed_claims >= 0; aged merchant_name / purchase_at nulled

# Keep users: cron leaves merchant_name intact
```

### Civic will consume

- Proxy `receipt_history_retention` on `GET /me`.
- Proxy setter as `POST /me/receipt-history-preference`.
- Gate receipt claim until set; Profile Taxes settings pillbox; consent modal
  with privacy policy link.
