---
status: done
slug: claim-anonymous-burn-share
owner_repo: both
created: 2026-11-13
updated: 2026-11-13
---

# Claim anonymous burn share

**Product:** When registering Merits from a Civic receipt claim, the scanner may
allocate a percentage of the VAT / Merit proceeds to an **anonymous** (not
registered in Rovas) share. That portion is **burned**: no Merits are issued to
anyone for that slice. The receipt is still fully claimed and deduped. Real
Rovas recipients (including the scanner) keep the remaining shares.

**Owner:** `both` — Rovas owns claim-rule validation + grant skip + persist;
Civic owns ShareEditor UI and API share validation against this contract.

### Contract

Existing endpoint (no new path):

| Method | URL | Auth |
|--------|-----|------|
| `POST` | `/rovas/rules/rules_proxy_claim_vat_receipt` | Bearer required |

**Delta** on `shares[]`:

| Rule | Detail |
|------|--------|
| Sentinel | `rovas_uid: "anonymous"` is allowed (string literal, case-sensitive) |
| Count | At most **one** anonymous entry |
| User check | Skip “user exists” for anonymous |
| Grants | Skip donor Merit issuer for that share; `merits_awarded: 0` |
| Persist | Store a `{civic_claim_shares}` row with `beneficiary_rovas_uid = anonymous`, `share_pct`, `merits_awarded = 0` (so submitted history can show the burn) |
| Scanner | Still required in `shares[]`; scanner **`share_pct` must be > 0** |
| Sum | All shares (including anonymous) still sum ≈ 100 |
| Residue | Flooring residue among **granted** (non-anonymous) shares only → scanner. Anonymous never receives residue |
| Response | Include anonymous in `shares[]` with `merits_awarded: 0`. `total_merits_awarded` = sum of real grants only (excludes burned slice) |

Example request body (unchanged outer shape):

```json
{
  "receipt_uid": "O-0123456789abcdef0123456789abcdef",
  "vat_eur": 4.82,
  "total_merits": 6.43,
  "merchant_name": "Tesco Bratislava",
  "purchase_at": "2026-06-11T14:22:00+02:00",
  "hourly_wage_eur": 7.5,
  "formula_version": 1,
  "shares": [
    { "rovas_uid": "42", "share_pct": 70 },
    { "rovas_uid": "anonymous", "share_pct": 30 }
  ]
}
```

Example success (illustrative numbers):

```json
{
  "total_merits_awarded": 4.5,
  "shares": [
    { "beneficiary_rovas_uid": "42", "merits_awarded": 4.5 },
    { "beneficiary_rovas_uid": "anonymous", "merits_awarded": 0 }
  ],
  "result": 0,
  "result_explain": "ok",
  "hourly_wage_eur": 7.5,
  "treasury_nid": 12345
}
```

Reject (non-zero `result` / existing explain codes as appropriate) when:

- More than one `anonymous` entry
- Scanner missing or scanner `share_pct` ≤ 0
- Non-anonymous uid does not exist
- Percentages do not sum ≈ 100

History reads (`GET /rovas/civic_claims`, `view=submitted`) should surface the
anonymous share row like any other share (uid `anonymous`, 0 ME). Received view
never lists anonymous (no user beneficiary).

### Checklist

1. [x] Accept `rovas_uid: "anonymous"` once; skip user-exists check
2. [x] Skip Merit grant for anonymous; persist share with `merits_awarded = 0`
3. [x] Enforce scanner present with `share_pct > 0`; residue only among granted shares
4. [x] `total_merits_awarded` excludes burned slice; response includes anonymous row
5. [x] Submitted claim history returns anonymous share rows
6. [x] Deploy rovdev + smoke (human unless asked)

### Smoke test

```bash
# TOKEN = Civic OAuth access token for scanner uid ROVAS_UID
# Use a fresh receipt_uid never claimed on rovdev.

curl -sS -X POST https://dev.rovas.app/rovas/rules/rules_proxy_claim_vat_receipt \
  -H "Authorization: Bearer $TOKEN" \
  -H "Content-Type: application/json" \
  -d '{
    "receipt_uid": "'"$RECEIPT_UID"'",
    "vat_eur": 4.82,
    "total_merits": 6.43,
    "merchant_name": "Civic burn smoke",
    "purchase_at": "2026-11-13T12:00:00+01:00",
    "hourly_wage_eur": 7.5,
    "formula_version": 1,
    "shares": [
      { "rovas_uid": "'"$ROVAS_UID"'", "share_pct": 70 },
      { "rovas_uid": "anonymous", "share_pct": 30 }
    ]
  }'
# Expect result 0; anonymous merits_awarded 0; total_merits_awarded ≈ 70% of full grant.
# Re-POST same receipt_uid → duplicate_receipt / 409.
# Claim with scanner share_pct 0 → rejected.
```

### Civic will consume

Expo ShareEditor adds an optional Anonymous row (`rovas_uid: "anonymous"`).
Civic `POST /receipts/claim` validates the same rules and passes the sentinel
through to this claim rule. Preview Merit math zeros the anonymous share and
does not fold the burn into the scanner.
