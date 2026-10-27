---
status: active
slug: give-chrons-to-merits
owner_repo: both
created: 2026-10-27
updated: 2026-10-27
---

# Give Chrons → Merits (voluntary conversion)

> Rovas implementation is in tree (`rovas_civic_update_7029`); awaiting rovdev deploy + smoke.

**Product:** Civic users who hold Chrons (from volunteer work reports that paid
out as CHR) can convert some or all of that balance into Merits from the Get
Merits → Volunteering card (“Give Chrons”). Conversion is **1 Chron = 1 Merit**
(existing A6 forex path). Merits are credited as volunteer / CHR-purchase
source so they count toward the Volunteering Merit category. Success copy in
Civic: “You gained XXX Merits for your volunteer work.”

**Owner:** `both` — Rovas exposes Bearer `POST /rovas/civic_give_chrons`; Civic
proxies and builds the modal UI.

### Contract

#### `POST /rovas/civic_give_chrons`

| | |
|--|--|
| Method | `POST` |
| Path | `/rovas/civic_give_chrons` |
| Auth | Bearer (OAuth2 `civic` client); API-key auth **disabled** |
| Content-Type | `application/json` |

**Body:**

| Field | Rules |
|-------|--------|
| `amount` | Required positive number (Chrons to convert). Accept float; reject `≤ 0`, non-finite, or more than 2 decimal places if you normalize to cents. Must not exceed the user’s current `field_chrons_balance`. |

**Response (200):**

```json
{
  "chrons_given": 25.5,
  "merits_gained": 25.5,
  "total_chr": 10.0,
  "total_me": 1234.5
}
```

| Field | Notes |
|-------|--------|
| `chrons_given` | Amount debited (echo of accepted `amount`, after any rounding you apply) |
| `merits_gained` | Merits credited — **equal to** `chrons_given` (1:1 via `_convert_chr_2_merits`) |
| `total_chr` | User Chron balance after debit |
| `total_me` | User Merit wallet after grant |

**Implementation notes (Rovas):**

1. Resolve Bearer uid; load account; read Chron balance from
   `field_chrons_balance`.
2. Validate `amount`; if `amount > balance` → `422` with a clear message
   (Civic maps to `insufficient_chrons`).
3. Call existing A6 helpers used by WR conversion:
   - `$merits = _convert_chr_2_merits($amount);`
   - `_rovas_forex_grant_merits($account, $merits, -1 * $amount);`
     (default merit source `volunteer` / CHR purchase — do **not** use
     `volunteer_work`, which is WR-linked).
4. Re-read balances; return the JSON above.
5. Register Services resource + `rovas_civic_ensure_services_resources()` /
   update hook (same pattern as `civic_receipt_history_preference`).

**Errors:**

| Code | When |
|------|------|
| `400` / `422` | Missing/invalid `amount` |
| `422` | `insufficient_chrons` (amount &gt; balance) |
| `401` | Missing/invalid Bearer |
| `503` | Forex helpers unavailable |

### Checklist

1. [x] Services resource `POST /rovas/civic_give_chrons` (Bearer only)
2. [x] Validate amount + balance; 1:1 via `_convert_chr_2_merits` +
      `_rovas_forex_grant_merits`
3. [x] Return updated `total_chr` / `total_me`
4. [x] Update hook / `ensure_services_resources` (`rovas_civic_update_7029`)
5. [ ] Deploy rovdev + smoke (human unless asked)

### Smoke test

```bash
TOKEN=…  # OAuth access token for a user with total_chr > 0 on dev.rovas.app

# Balance before
curl -sS -H "Authorization: Bearer $TOKEN" -H "Accept: application/json" \
  https://dev.rovas.app/rovas/me
# → "total_chr": N  (N > 0)

# Convert 1 Chron
curl -sS -X POST https://dev.rovas.app/rovas/civic_give_chrons \
  -H "Authorization: Bearer $TOKEN" -H "Content-Type: application/json" \
  -H "Accept: application/json" \
  -d '{"amount":1}'
# → {"chrons_given":1,"merits_gained":1,"total_chr":…,"total_me":…}

# Over-balance
curl -sS -X POST https://dev.rovas.app/rovas/civic_give_chrons \
  -H "Authorization: Bearer $TOKEN" -H "Content-Type: application/json" \
  -d '{"amount":999999}'
# → HTTP 422
```

### Civic will consume

- Civic API: `POST /me/give-chrons` → proxies this endpoint.
- Expo Get Merits → Volunteering card: “Give Chrons” button when
  `total_chr > 0`; modal form with balance + amount; on success show
  “You gained XXX Merits for your volunteer work.” then Close.
