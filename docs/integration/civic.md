# Civic ↔ Rovas integration

Civic (`~/Documents/homeProjects/civic`) is a Slovak-first Expo app that uses
Rovas as its identity and Merit ledger. This document is the **Rovas-side
source of truth** for OAuth and the APIs Civic needs next. Civic-side notes
live at `~/Documents/homeProjects/civic/docs/rovas-integration.md` (Expo +
Civic API); keep both in sync when contracts change.

**Agents working in this repo:** implement / maintain work under
[Next work: solo VAT claim](#next-work-solo-vat-claim),
[Next work: Civic claim reads + SoT](#next-work-civic-claim-reads--sot),
[Next work: enrich received claims with scanner username](#next-work-enrich-received-claims-with-scanner-username),
[Next work: public national hourly rate API](#next-work-public-national-hourly-rate-api),
[Next work: users search (split typeahead)](#next-work-users-search-split-typeahead),
and [Next work: users lookup (community public profile)](#next-work-users-lookup-community-public-profile).
Do not rely on reading the Civic tree unless you need Expo/API details.

**Dev target (until told otherwise):** all Civic integration work targets
**`/var/www/rovdev`** (`https://dev.rovas.app`). Do **not** change prod
`/var/www/rovas` (`rovas.app`) unless explicitly asked.

**Deployment:** the human deploys unless they ask the agent to. Agents should
leave code + docs ready for `ops/deploy_rovdev.sh` (or a targeted rsync) and
not run deploys on their own.

## Status

| Piece | Location | Notes |
|-------|----------|--------|
| `oauth2_server` 7.x-1.7 | `sites/all/modules/contrib/oauth2_server` | + PKCE patch [#3015065-7](https://www.drupal.org/project/oauth2_server/issues/3015065) in `patches/oauth2_server-3015065-pkce-support-7.patch` |
| `xautoload` 7.x-5.9 | `sites/all/modules/contrib/xautoload` | Required by oauth2_server |
| `oauth2-server-php` **v1.14.2** | `sites/all/libraries/oauth2-server-php` | Module README lists ≤1.8; **1.14.2 required for PKCE** |
| `rovas_oauth` | `sites/all/modules/custom/rovas_oauth` | Bootstrap Civic server/client, login copy, Bearer helpers, `/me` + `/contributions/aggregate` |
| Expo login (PKCE) | Civic mobile | **Done** against `dev.rovas.app` |
| Civic API Bearer introspect | Civic `GET /oauth2/tokens/{token}` | **Done** — resolves Drupal `user_id` (uid) |
| Services OAuth2 on `rovas_endpoint` | Admin / `rovas_oauth_ensure_civic_resources()` | Server machine name **`civic`**; API-key auth disabled on Civic resources |
| Home Merits reads | `/rovas/me`, `/rovas/contributions/aggregate`, `/rovas/merit_distribution` | Implemented |
| `rovas_civic` | `sites/all/modules/custom/rovas_civic` | National rates, donor Merit issuance, State Treasury, VAT claim |
| Claim rule + split claims | `POST /rovas/rules/rules_proxy_claim_vat_receipt` | Implemented in `rovas_civic` (enable via updb / admin) |
| Claim reads + summary | `GET /rovas/civic_claims`, `…/summary` | Implemented in `rovas_civic` (`rovas_civic_update_7002`); **Civic proxies live** (`GET /receipts`, `/me/contributions/summary`) |
| Claim status by receipt | `GET /rovas/civic_claims?receipt_uid=` | Implemented — `{ claimed, receipt_uid }` only (no scanner) |
| Received history “From” | `view=received` scanner username | Implemented — `scanner_username` + `scanner_display_name` |
| National hourly rate read | `GET /rovas/civic_national_hourly_rate` | Implemented in `rovas_civic` (`rovas_civic_update_7003`) |
| Users search (split typeahead) | `GET /rovas/users_search?query=` | Implemented in `rovas_civic` (`rovas_civic_update_7005`). **Param is `query`, not `q`** — Drupal reserves `$_GET['q']` for the request path. |
| Users lookup (community profile) | `GET /rovas/users_lookup?username=` | Implemented in `rovas_civic` (`rovas_civic_update_7006`). Exact username; Bearer required. See [users lookup](#next-work-users-lookup-community-public-profile). |
| Friend invite (Civic) | — | **Civic-client only** (`mailto:`). No Rovas invite endpoint. Signup URL in the email body should work (`/user/register` or OAuth authorize). |

## OAuth endpoints (live on dev)

- Authorize: `https://dev.rovas.app/oauth2/authorize`
- Token: `https://dev.rovas.app/oauth2/token`
- Revoke: `https://dev.rovas.app/oauth2/revoke`
- Introspect: `https://dev.rovas.app/oauth2/tokens/{access_token}` → JSON with `user_id`, `client_id`, `expires`, `scope` (404 if invalid)

## Civic home-read APIs (live on dev after enable)

| Method | URL | Auth |
|--------|-----|------|
| `GET` | `https://dev.rovas.app/rovas/me` | Bearer required (`user_profile`) |
| `GET` | `https://dev.rovas.app/rovas/contributions/aggregate` | Public |
| `GET` | `https://dev.rovas.app/rovas/merit_distribution` | Public; Bearer optional for `user_*` |

Enable / re-sync on rovdev: **Configuration → Web services → Rovas OAuth (Civic)** →
**Enable Civic home API resources**, or `drush74 updb` after deploy (`rovas_oauth_update_7001`).

That helper also ensures endpoint auth is `oauth2_server => array('server' => 'civic')`
(not a bare checkbox string) and disables API-key auth on those three resources.

## Civic OAuth client (bootstrap)

Admin UI: **Configuration → Web services → Rovas OAuth (Civic)**  
(`/admin/config/services/rovas-oauth`)

Click **Bootstrap Civic OAuth server + client**. That creates:

| Field | Value |
|-------|--------|
| Server machine name | `civic` |
| Enforce PKCE | yes |
| Grant types | authorization_code, refresh_token |
| Client ID | `civic` |
| Client secret | *(empty — public native client)* |
| Redirect URI | `civic://oauth/callback` (+ optional Expo proxy URI) |
| Default scope | `user_profile` |

HybridAuth (Google / Facebook / Drupal login) already attaches to `user_login`.
`oauth2_server` redirects anonymous authorize requests to
`/user/login?destination=oauth2/authorize`, so the existing HybridAuth widget
is the authorize login UI. `rovas_oauth` only adds a short intro when the
destination is authorize.

## Enable on dev.rovas.app

Prefer **drush** over the Modules UI. Enabling via the UI can flash a one-off
PHP error from the site’s existing `service_container` module
(`Drupal::service()` while the container is still null during rebuild). The
modules still install; the error is a race with OpenLayers’ service container,
not a failure of `oauth2_server` itself.

```bash
# After your usual rsync to /var/www/rovdev:
cd /var/www/rovdev
drush74 en xautoload libraries oauth2_server rovas_oauth -y
drush74 updb -y
drush74 role-add-perm 'anonymous user' 'use oauth2 server'
drush74 role-add-perm 'authenticated user' 'use oauth2 server'
drush74 cc all -y
```

`rovas_oauth` bootstraps the `civic` server + client on enable. Confirm at
`/admin/config/services/rovas-oauth` (or re-run **Bootstrap** there).

Docroot mapping: **dev.rovas.app** → `/var/www/rovdev` (not prod `/var/www/rovas`).

### Services OAuth2 (rovas_endpoint)

- Admin: **Structure → Services → rovas_endpoint → Authentication**
- Enable **OAuth2 authentication**, server machine name **`civic`**
- Prefer **Enable Civic home API resources** on the Rovas OAuth admin form so
  Civic resources get `disable_authentication` on API keys (Bearer-only callers
  otherwise fail at the API-keys plugin).

To mirror on prod later: same UI steps, or run
`rovas_oauth_ensure_civic_resources()` via drush php-eval after deploy.
There is no Features export of `rovas_endpoint` today (DB-only).

## PKCE smoke test (curl)

```bash
# 1) Generate verifier / challenge (S256)
VERIFIER=$(python3 - <<'PY'
import base64, hashlib, os
v = base64.urlsafe_b64encode(os.urandom(32)).rstrip(b"=").decode()
print(v)
PY
)
CHALLENGE=$(python3 - <<PY
import base64, hashlib, sys
v = """$VERIFIER""".encode()
print(base64.urlsafe_b64encode(hashlib.sha256(v).digest()).rstrip(b"=").decode())
PY
)

# 2) Open in a browser (log in via HybridAuth), approve, capture ?code=
echo "https://dev.rovas.app/oauth2/authorize?response_type=code&client_id=civic&redirect_uri=civic%3A%2F%2Foauth%2Fcallback&scope=user_profile&state=spike1&code_challenge=$CHALLENGE&code_challenge_method=S256"

# 3) Exchange code (public client + PKCE; no client_secret)
curl -sS -X POST https://dev.rovas.app/oauth2/token \
  -d grant_type=authorization_code \
  -d client_id=civic \
  -d redirect_uri=civic://oauth/callback \
  -d code=PASTE_CODE \
  -d code_verifier=$VERIFIER
```

Expect JSON with `access_token` / `refresh_token`. A missing or wrong
`code_verifier` must fail (`code_verifier_mismatch` / similar).

## OAuth spike checklist

- [x] Deploy to `/var/www/rovdev` and enable modules
- [x] Bootstrap Civic server + client
- [x] Complete authorize → token with PKCE (2026-08-04 on dev)
- [x] Confirm Bearer resolves via `/oauth2/tokens/{token}` and `rovas_oauth_uid_from_bearer()`
- [x] Expo app login against these endpoints
- [ ] Note any oauth2-server-php 1.14.2 incompatibilities vs module README (none observed in authorize/token smoke)

---

## Home reads (done) — background for agents

Civic’s home mockup needs live Merits data. The mobile app already has a Rovas
**Bearer** access token. Civic API can introspect it to a Drupal **uid**.
Home-read endpoints below are **implemented**; claim reads are under
[Next work: Civic claim reads + SoT](#next-work-civic-claim-reads--sot).

### Why existing endpoints are not enough

| Existing | Auth today | Problem for Civic |
|----------|------------|-------------------|
| `POST /rovas/rules/rules_user_s_merits` | User **API-KEY + TOKEN** | Mobile has OAuth Bearer only, not the user’s API key pair |
| `GET …/rovas_merit_distribution` | API-KEY + TOKEN; returns SVG + `meta` | Same auth issue; Civic needs **JSON buckets**, not SVG |
| Community total | — | Covered by `/rovas/contributions/aggregate` |

### Priority order

#### 1. Enable OAuth2 authentication on Services (`rovas_endpoint`) — **done on rovdev**

See [Services OAuth2](#services-oauth2-rovas_endpoint) above. Re-run
`rovas_oauth_ensure_civic_resources()` if endpoint auth is still a bare
module string instead of `array('server' => 'civic')`.

#### 2. `GET /rovas/me` (Bearer required) — **implemented**

In `rovas_oauth` (`rovas_oauth_me_index()`).

```json
{
  "uid": 42,
  "username": "sano",
  "display_name": "Alexander Žaťko",
  "total_me": 1248.35
}
```

| Field | Source |
|-------|--------|
| `uid` | Bearer → Drupal uid |
| `username` | `$user->name` |
| `display_name` | `field_full_name` if set; else username |
| `total_me` | `_rovas_merit_distribution_user_merits()` (userpoints + decimal) |

#### 3. `GET /rovas/contributions/aggregate` (public) — **implemented**

In `rovas_oauth` (`rovas_oauth_contributions_retrieve()`). Uses the cached
collect from `rovas_merit_distribution` (1h TTL). `by_category` is zeros for v1.

```json
{
  "total_me": 84320.0,
  "by_category": {
    "taxes": 0,
    "charity": 0,
    "volunteer": 0
  }
}
```

#### 4. JSON merit distribution (Bearer optional) — **implemented**

`GET /rovas/merit_distribution` via `rovas_merit_distribution_json_index()`.
Reuses `_rovas_merit_distribution_collect_merits()` /
`_rovas_merit_distribution_compute_bins()`. No SVG.

```json
{
  "buckets": [
    { "label": "1–10", "count": 48 },
    { "label": "10–100", "count": 104 }
  ],
  "user_bucket_index": 5,
  "user_merits": 1248.35,
  "username": "sano",
  "total_users": 298,
  "bin_edges_log10": [0, 1, 2, 3]
}
```

When **no** Bearer: omit `user_*` fields. When Bearer present: fill them.

## Next work: solo VAT claim

**Implemented in `rovas_civic`** (enable on rovdev after deploy). Civic verifies
eKasa receipts; Rovas grants donor Merits on the **State Treasury of Slovakia**
project. `shares[]` is already supported (each beneficiary is an individual
donor). Split-claim typeahead is a separate task — see
[Next work: users search (split typeahead)](#next-work-users-search-split-typeahead).

### Checklist

1. [x] Services action `POST /rovas/rules/rules_proxy_claim_vat_receipt`
2. [x] Auth: OAuth2 Bearer (`civic` / `user_profile`); reject anonymous
3. [x] Tables `{civic_claims}` + `{civic_claim_shares}` (UNIQUE `receipt_uid`)
4. [x] Atomic multi-beneficiary grant via shared donor issuer
   (`purchase_receipt` + `merits_disbursement` for `/user/*/merits`,
   `rules_give_merits_to_user`, `project_merit_updater`, expertise NACE
   share, `{rovas_merit_issuance}` A3); each share = separate donor attribution
5. [x] Enable: `drush74 en rovas_civic -y && drush74 updb -y`, or admin
   **Configuration → Rovas Civic → Enable Civic VAT claim API**
6. [ ] Smoke-test with curl using a Civic OAuth access token (after human deploy)

### Path-1 donor Merit (subtask, also in `rovas_civic`)

- EUR project rewards (`/rewpro` Stripe + bank success paths) now issue **donor
  Merits** to the payer at the base-fund indicative hourly rate:
  `ME = (net_eur / rate) × 10`.
- Admin: **Configuration → Rovas Civic** — national hourly rates (SK seeded)
  and State Treasury project bootstrap.
- Formula for VAT claims (decided):  
  **`ME = (vat_slice_eur / national_hourly_rate_eur) × 10`**  
  Rovas owns the math; Civic `total_merits` / `hourly_wage_eur` are preview/audit only.

### Endpoint

| Method | URL | Auth |
|--------|-----|------|
| `POST` | `https://dev.rovas.app/rovas/rules/rules_proxy_claim_vat_receipt` | Bearer required |

### Request body

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
    { "rovas_uid": "42", "share_pct": 100 }
  ]
}
```

| Field | Notes |
|-------|--------|
| `receipt_uid` | eKasa online UID (`O-` + 32 hex); global unique claim key |
| `vat_eur` | Verified by Civic against Finančná správa; Rovas grant basis |
| `total_merits` | Civic preview only — **ignored for grant** |
| `merchant_name` / `purchase_at` | Audit metadata (nullable OK) |
| `hourly_wage_eur` | Civic preview; grant uses admin **national** rate (SK) |
| `shares` | ≥1; each entry is a **donor** to State Treasury for their VAT slice; `share_pct` sums to ~100; **scanner must be included** |

### Required logic

1. Resolve **scanner** from Bearer → Drupal uid
2. **Dedup** on `receipt_uid` → `result` 409 / `duplicate_receipt`
3. Validate `shares` (distinct uids, sum ≈ 100, users exist, scanner included)
4. Per share: `vat_slice` + `ME = (vat_slice / national_rate) × 10`; floor 2dp;
   **residue to scanner**
5. One Drupal transaction: for each share call donor Merit issuer
   (`purchase_receipt` + `merits_disbursement` + `rules_give_merits_to_user`
   + project ME bump + expertise + `{rovas_merit_issuance}`) targeting State
   Treasury project; persist claim + share rows
6. Return:

```json
{
  "total_merits_awarded": 6.426667,
  "shares": [
    { "beneficiary_rovas_uid": "42", "merits_awarded": 6.426667 }
  ],
  "result": 0,
  "result_explain": "ok",
  "hourly_wage_eur": 7.5,
  "treasury_nid": 12345
}
```

Non-zero `result` + `result_explain` for failures (`duplicate_receipt`,
`shares_pct_sum`, `national_rate_missing`, …).

### Out of scope for this Rovas slice

- Full A11 charity custody / settlement
- Volunteer report proxy, charity/invest project lists
- Civic SQLite / fraud heuristics (Civic repo)
- Prod `/var/www/rovas` until explicitly requested

### Smoke test (after deploy + enable)

```bash
# Enable on rovdev (human deploy first):
#   drush74 en rovas_civic -y && drush74 updb -y && drush74 cc all -y

curl -sS -X POST https://dev.rovas.app/rovas/rules/rules_proxy_claim_vat_receipt \
  -H "Authorization: Bearer $ACCESS_TOKEN" \
  -H "Accept: application/json" \
  -H "Content-Type: application/json" \
  -d '{
    "receipt_uid": "O-…",
    "vat_eur": 1.25,
    "merchant_name": "Test",
    "purchase_at": null,
    "formula_version": 1,
    "shares": [{ "rovas_uid": "'"$ROVAS_UID"'", "share_pct": 100 }]
  }'
```

### Acceptance checks (dev / rovdev) — home reads (still valid)

```bash
curl -sS https://dev.rovas.app/rovas/me \
  -H "Authorization: Bearer $ACCESS_TOKEN" \
  -H "Accept: application/json"

curl -sS https://dev.rovas.app/rovas/contributions/aggregate \
  -H "Accept: application/json"

curl -sS https://dev.rovas.app/rovas/merit_distribution \
  -H "Accept: application/json"
```

### Later (after solo claim ships)

- Charity pass-through using national rates (A11)
- Ensure multi-beneficiary shares are exercised end-to-end from Civic

## Next work: users search (split typeahead)

**Implemented in `rovas_civic`** (enable on rovdev: `drush74 updb -y`, or admin
**Enable Civic VAT claim + claim-read + rate + users_search APIs**). Civic
split-claim UI needs a lightweight typeahead so the scanner can add other
Rovas users as Merit co-recipients. The claim rule already accepts `shares[]`;
this endpoint unblocks the add-person flow.

Civic proxies this as `GET /users/search?q=` (Bearer required). Civic keeps
the friendly `q` on its own API and maps it to Rovas `query`.

### Path note

Drupal Services resource aliases **cannot contain slashes**. Use the
machine-name path:

| Method | URL | Auth |
|--------|-----|------|
| `GET` | `https://dev.rovas.app/rovas/users_search?query=` | Bearer required (`civic` / `user_profile`) |

Do **not** register `/rovas/users/search` (slash in alias). Civic API calls
`/rovas/users_search?query=`.

**Critical:** the search string param must **not** be named `q`. Drupal uses
`$_GET['q']` as the internal request path; `?q=ann` replaces the Services
path with `ann` and returns a themed HTML 404 before Services runs (no
dblog Services entries). Use `query`.

### Query

| Param | Rules |
|-------|--------|
| `query` | Required; trim; **≥ 3** characters; reject shorter with 400. **Never `q`.** |
| (implicit) | Max **10** results |

### Response

JSON array of public profile rows only — **no** email, balance, or
contributions:

```json
[
  {
    "rovas_uid": "42",
    "username": "anna_nb",
    "display_name": "Anna Nováková"
  }
]
```

| Field | Notes |
|-------|--------|
| `rovas_uid` | Drupal uid as string |
| `username` | Rovas username |
| `display_name` | Prefer real name / display; fall back to `username` |

Empty match → `[]` (200).

### Required logic

1. Resolve caller from Bearer (reject anonymous)
2. Match `query` case-insensitively against username and display name
3. Exclude blocked / inactive users; exclude the caller (optional but
   preferred — scanner is always injected client-side)
4. Cap at 10 rows, stable order (username ASC is fine)
5. **Phase 3+:** honour a per-user “not searchable / don’t include me in
   split claims” preference — stub as no-op for v1 (`rovas_civic_user_is_searchable`)

### Checklist

1. [x] Services action `GET /rovas/users_search`
2. [x] Auth: OAuth2 Bearer (`civic` / `user_profile`); reject anonymous
3. [x] Validate `query` ≥ 3; return ≤ 10 `{ rovas_uid, username, display_name }`
4. [x] Param named `query` (not `q`) — Drupal path conflict
5. [x] API-key auth disabled on the resource (same as other Civic resources)
6. [x] Enable via updb / ensure helper after deploy (`rovas_civic_update_7005`)
7. [ ] Smoke-test curl below

### Smoke test (after deploy + enable)

```bash
# After deploy: drush74 updb -y && drush74 cc all -y
# or admin: Enable Civic VAT claim + claim-read + rate + users_search APIs

curl -sS "https://dev.rovas.app/rovas/users_search?query=ann" \
  -H "Authorization: Bearer $ACCESS_TOKEN" \
  -H "Accept: application/json"
```

## Next work: users lookup (community public profile)

**Implemented in `rovas_civic`** (enable on rovdev: `drush74 updb -y`, or admin
**Enable Civic VAT claim + claim-read + rate + users_search + users_lookup APIs**).
Civic Screen 6 (Komunita) needs a **public contribution profile** for a
Rovas username — heavier than split-claim typeahead (`users_search`). Civic
proxies this as `GET /users/lookup?username=` (Bearer required). Percentile /
rank ladder are computed on **Civic** from `/rovas/merit_distribution` +
`total_me`; do **not** return percentile from this endpoint.

### Path note

Drupal Services resource aliases **cannot contain slashes**. Use the
machine-name path:

| Method | URL | Auth |
|--------|-----|------|
| `GET` | `https://dev.rovas.app/rovas/users_lookup?username=` | Bearer required (`civic` / `user_profile`) |

Do **not** register `/rovas/users/lookup` (slash in alias). Civic API calls
`/rovas/users_lookup?username=`.

### Query

| Param | Rules |
|-------|--------|
| `username` | Required; trim; exact match, case-insensitive against `users.name`. Reject empty with **400**. |

### Response

```json
{
  "rovas_uid": "42",
  "username": "anna_nb",
  "display_name": "Anna Nováková",
  "total_me": 3410.0,
  "by_category": {
    "taxes": 980.0,
    "charity": 210.0,
    "volunteer": 148.0
  },
  "taxes_last_3_months": [
    { "month": "2026-04", "me": 310.0 },
    { "month": "2026-05", "me": 340.0 },
    { "month": "2026-06", "me": 330.0 }
  ],
  "top_charities": [
    { "name": "Detský dom Topoľčany", "me": 120.0 }
  ],
  "top_volunteer_projects": [
    { "name": "Obnova lesov v Tatranskom NP", "me": 80.0 }
  ]
}
```

| Field | Notes |
|-------|--------|
| `rovas_uid` | Drupal uid as string |
| `username` | Canonical Rovas username |
| `display_name` | Prefer `field_full_name`; fall back to `username` |
| `total_me` | Same source as `/rovas/me` for that uid (`_rovas_merit_distribution_user_merits` or fallback) |
| `by_category.taxes` | Larger of (a) `{civic_claim_shares}.merits_awarded` for this beneficiary and (b) `user_expertise` Merits on fallback NACE `ROVAS_CIVIC_FALLBACK_NACE_TID` (64.99 / tid **1601**) |
| `by_category.charity` | `user_expertise` Merits on `ROVAS_CIVIC_CHARITY_NACE_TID` when that constant is a positive tid; **0** until a dedicated charity NACE is configured |
| `by_category.volunteer` | Sum of `user_expertise` Merits whose NACE is **neither** the fallback tid nor the charity tid |
| `taxes_last_3_months` | Last **3 calendar months** (including current) of VAT-claim Merits for this uid, keyed `YYYY-MM`; omit zero months or include with `me: 0` — prefer only months with activity; max 3 rows; `[]` if none |
| `top_charities` | Up to **3** `{ name, me }` from `purchase_receipt` rows (buyer = uid, no `field_rewarded_report`, exclude State Treasury project) |
| `top_volunteer_projects` | Up to **3** `{ name, me }` from `project_merit_allocation` / `co_worker_merit_allocation` labor Merits |

### Errors

| Status | When |
|--------|------|
| 401 | No / invalid Bearer |
| 400 | Empty / missing `username` |
| 404 | Unknown username, blocked/inactive user, or not searchable (privacy) |

### Required logic

1. Resolve caller from Bearer (reject anonymous)
2. Load active user by exact username (case-insensitive); exclude uid 0 / blocked
3. Honour `rovas_civic_user_is_searchable` — if false, return **404** (same as unknown)
4. Build response fields above; missing enrichment → zeros / `[]`
5. Enable via updb / `rovas_civic_ensure_services_resources()`; disable API-key auth on the resource (same as other Civic Bearer resources)

### Checklist

1. [x] Services action `GET /rovas/users_lookup`
2. [x] Auth: OAuth2 Bearer (`civic` / `user_profile`); reject anonymous
3. [x] Validate `username` non-empty; return `UserLookup` shape above
4. [x] Machine-name path (no slash alias)
5. [x] API-key auth disabled on the resource
6. [x] Enable via updb / ensure helper after deploy (`rovas_civic_update_7006`)
7. [ ] Smoke-test curl below

### Friend invite (explicit non-work)

Civic friend invite is **client `mailto:` only**. Do **not** add a Rovas
invite endpoint. Ensure `/user/register` (and/or OAuth authorize) works so
the invite email can link to signup — no new Rovas landing page required
(unlike charity `/charity-projects`).

### Smoke test (after deploy + enable)

```bash
# After deploy: drush74 updb -y && drush74 cc all -y
# or admin: Enable Civic … + users_lookup APIs

curl -sS "https://dev.rovas.app/rovas/users_lookup?username=sano" \
  -H "Authorization: Bearer $ACCESS_TOKEN" \
  -H "Accept: application/json"
```

## Next work: Civic claim reads + SoT

**Implemented in `rovas_civic`** (enable on rovdev: `drush74 updb -y`, or admin
**Enable Civic VAT claim + claim-read APIs**). Civic is dropping its local
SQLite claim registry (`receipt_claims` / `receipt_claim_shares`). Rovas
`{civic_claims}` / `{civic_claim_shares}` is the **sole** claim source of
truth (`receipt_uid` UNIQUE, VAT, merchant, shares, Merits). Civic keeps
only `claim_attempts` for fraud/ops (GPS, rate limits, strikes) — that is
an audit log, not a second claim registry.

**Why Rovas must own this:** Merits are granted here; without
`receipt_uid` on Rovas, wiping Civic (or bypassing it) could double-pay.
Dev reset of “already scanned QRs” is manual (truncate claim tables in
phpMyAdmin + `drush74 cc all`); Merit balances are **not** reversed.

Civic will proxy these reads into `GET /receipts` and
`GET /me/contributions/summary` once they exist.

**Civic status (2026-08):** done — Expo Bločky history tabs call Civic
`GET /receipts?view=submitted|received`, and home taxes use
`GET /rovas/civic_claims/summary` via `/me/contributions/summary`. Received
items include `scanner_username` / `scanner_display_name` for the From column
(after rovdev deploy).

### Decisions (locked)

- Summary is standalone: `GET /rovas/civic_claims/summary` (not fields on `/me`)
- Summary metrics are **received**-based (shares credited to the caller)
- No built-in clear API/drush — human clears tables when re-testing

### Checklist

1. [x] `GET /rovas/civic_claims?view=submitted` — Bearer; claims where caller
   is the scanner
2. [x] `GET /rovas/civic_claims?view=received` — Bearer; share rows where
   caller is a beneficiary
3. [x] Pagination: `limit` / `offset` (default 50, max 100)
4. [x] `GET /rovas/civic_claims/summary` — `receipt_count`, `total_vat_eur`,
   `taxes_me` (all **received**)
5. [x] `GET /rovas/civic_claims?receipt_uid=` — `{ claimed, receipt_uid }`
   global SoT lookup (no scanner identity; ignores view/limit/offset)
6. [x] Services + OAuth2 (`civic` / `user_profile`); reject anonymous
7. [x] Enable / register resources (`rovas_civic_update_7002` / admin)
8. [x] Dev clear: out of band (phpMyAdmin) — Merits not reversed
9. [ ] Smoke-test curls below after deploy

### Endpoints

| Method | URL | Auth |
|--------|-----|------|
| `GET` | `https://dev.rovas.app/rovas/civic_claims?view=submitted` | Bearer required |
| `GET` | `https://dev.rovas.app/rovas/civic_claims?view=received` | Bearer required |
| `GET` | `https://dev.rovas.app/rovas/civic_claims?receipt_uid=O-…` | Bearer required — `{ claimed, receipt_uid }` |
| `GET` | `https://dev.rovas.app/rovas/civic_claims/summary` | Bearer required |

### Claim status (`?receipt_uid=`)

Global UID dedup preview for Civic verify/split UI. When `receipt_uid` is
non-empty, ignore `view` / `limit` / `offset` and return only:

```json
{ "claimed": true, "receipt_uid": "O-0123456789abcdef0123456789abcdef" }
```

Lookup is `{civic_claims}` by UNIQUE `receipt_uid`. No scanner / share data.
Implemented as `rovas_civic_claim_status_by_receipt_uid()`.

Civic calls this via `fetchClaimStatus` (optional Bearer → `already_claimed`
locks split UI + claim). Related: `users_search` uses param **`query`**, never
`q` (Drupal path conflict) — see [users search](#next-work-users-search-split-typeahead).

```bash
curl -sS "https://dev.rovas.app/rovas/civic_claims?receipt_uid=O-<known-claimed>" \
  -H "Authorization: Bearer $ACCESS_TOKEN" -H "Accept: application/json"
# → {"claimed":true,"receipt_uid":"O-…"}

curl -sS "https://dev.rovas.app/rovas/civic_claims?receipt_uid=O-<unknown>" \
  -H "Authorization: Bearer $ACCESS_TOKEN" -H "Accept: application/json"
# → {"claimed":false,"receipt_uid":"O-…"}
```

### Response shapes

**`view=submitted`** — successful claims the caller scanned (not every QR
attempt; failures stay in Civic `claim_attempts`):

```json
{
  "items": [
    {
      "receipt_uid": "O-0123456789abcdef0123456789abcdef",
      "vat_eur": 4.82,
      "total_merits_awarded": 6.43,
      "merchant_name": "Tesco Bratislava",
      "purchase_at": "2026-06-11T14:22:00+02:00",
      "created_at": "2026-06-11T15:01:00+02:00",
      "shares": [
        {
          "beneficiary_rovas_uid": "42",
          "share_pct": 100,
          "merits_awarded": 6.43
        }
      ]
    }
  ],
  "total": 1,
  "limit": 50,
  "offset": 0,
  "view": "submitted"
}
```

**`view=received`** — shares credited to the caller (may include claims
scanned by someone else):

```json
{
  "items": [
    {
      "receipt_uid": "O-…",
      "merchant_name": "Tesco Bratislava",
      "purchase_at": "2026-06-11T14:22:00+02:00",
      "scanner_rovas_uid": "7",
      "scanner_username": "sano",
      "scanner_display_name": "Sano",
      "share_pct": 50,
      "merits_awarded": 3.21,
      "created_at": "2026-06-11T15:01:00+02:00"
    }
  ],
  "total": 1,
  "limit": 50,
  "offset": 0,
  "view": "received"
}
```

**`GET …/summary`** (received-based — matches Civic home “taxes”):

```json
{
  "receipt_count": 12,
  "total_vat_eur": 48.2,
  "taxes_me": 64.3
}
```

| Field | Meaning |
|-------|---------|
| `receipt_count` | Distinct claims where caller is a beneficiary |
| `total_vat_eur` | Sum of caller’s `vat_slice` |
| `taxes_me` | Sum of caller’s `merits_awarded` from VAT shares |

### Out of scope for this Rovas slice

- Civic SQLite / `claim_attempts` / GPS fraud (Civic repo)
- Built-in truncate / Merit reversal for re-claim testing
- Prod `/var/www/rovas` until explicitly requested

### Smoke test (after deploy + enable)

```bash
# After deploy: drush74 updb -y && drush74 cc all -y
# or admin: Enable Civic VAT claim + claim-read APIs

curl -sS "https://dev.rovas.app/rovas/civic_claims?view=submitted" \
  -H "Authorization: Bearer $ACCESS_TOKEN" \
  -H "Accept: application/json"

curl -sS "https://dev.rovas.app/rovas/civic_claims?view=received" \
  -H "Authorization: Bearer $ACCESS_TOKEN" \
  -H "Accept: application/json"

curl -sS "https://dev.rovas.app/rovas/civic_claims/summary" \
  -H "Authorization: Bearer $ACCESS_TOKEN" \
  -H "Accept: application/json"

curl -sS "https://dev.rovas.app/rovas/civic_claims?receipt_uid=O-<known-or-unknown>" \
  -H "Authorization: Bearer $ACCESS_TOKEN" \
  -H "Accept: application/json"
```

## Next work: enrich received claims with scanner username

**Implemented in `rovas_civic`.** Civic screen 2 (“Bločky”) history tab
**Moje merity z bločkov** / **My Merits from receipts** shows an **Od / From**
column (who scanned the receipt that credited the caller). `view=received`
now includes `scanner_username` + `scanner_display_name` alongside
`scanner_rovas_uid` so Civic can show `@sano` instead of `#<uid>`.

Scanners are batch-loaded via `user_load_multiple()`. Missing/blocked accounts
still return the uid string as username/display_name — one bad row never 500s
the list. `view=submitted` is unchanged.

### `view=received` item shape

```json
{
  "receipt_uid": "O-…",
  "merchant_name": "Tesco Bratislava",
  "purchase_at": "2026-06-11T14:22:00+02:00",
  "scanner_rovas_uid": "7",
  "scanner_username": "sano",
  "scanner_display_name": "Sano",
  "share_pct": 50,
  "merits_awarded": 3.21,
  "created_at": "2026-06-11T15:01:00+02:00"
}
```

### Checklist

1. [x] Add `scanner_username` + `scanner_display_name` on `view=received` items
2. [x] Batch-load usernames (`user_load_multiple`)
3. [x] Document fields under [Claim reads](#next-work-civic-claim-reads--sot)
4. [ ] Smoke-test: `GET /rovas/civic_claims?view=received` after deploy + `drush74 cc all`
5. [ ] Civic: switch From column from `#uid` to `@username` (Civic agent)

### Out of scope

- Privacy / “not searchable” filtering for scanners on history (Phase 3+)
- Changing summary or `receipt_uid=` claim-status responses
- Civic mobile wiring (Civic agent picks that up after this ships)

### Smoke test

```bash
# After deploy: drush74 cc all -y
curl -sS "https://dev.rovas.app/rovas/civic_claims?view=received" \
  -H "Authorization: Bearer $ACCESS_TOKEN" \
  -H "Accept: application/json"
# Expect each item: scanner_rovas_uid + scanner_username + scanner_display_name
```

## Path note (Services)

Drupal Services resource aliases **cannot contain slashes**. Civic claim,
rate, and user-search reads use machine-name paths (`/rovas/civic_claims`,
`/rovas/civic_national_hourly_rate`, `/rovas/users_search`), not
`/rovas/civic/…` or `/rovas/users/search`.

## Next work: public national hourly rate API

**Implemented in `rovas_civic`.** Civic must stop using its hard-coded / env
`HOURLY_WAGE_EUR` as the authoritative Merit conversion wage. Rovas admin
stores the country rate in `{rovas_national_hourly_rate}` (Configuration →
Rovas Civic) and the VAT claim rule grants at that rate. This read API lets
verify-preview, home € equivalent, and claim audit fields match the grant.

**Do not** bolt this onto `GET /rovas/me` (user-scoped + Bearer) or
`/contributions/aggregate` (unrelated). Claim POST already echoes
`hourly_wage_eur` **after** grant — that does not help pre-claim UX.

### Endpoint

| Method | URL | Auth |
|--------|-----|------|
| `GET` | `https://dev.rovas.app/rovas/civic_national_hourly_rate?country=SK` | **Public** (no Bearer) |

Default `country` = `ROVAS_CIVIC_DEFAULT_COUNTRY` (`SK`). Unknown / missing
row → **404** with a clear error body.

### Response shape

```json
{
  "country_code": "SK",
  "hourly_wage_eur": 7.5,
  "merits_per_hour": 10,
  "source": "manual / Eurostat / …",
  "updated": "2026-08-01T12:00:00+02:00"
}
```

| Field | Source |
|-------|--------|
| `country_code` | Request / row PK |
| `hourly_wage_eur` | `{rovas_national_hourly_rate}.rate_eur` |
| `merits_per_hour` | `ROVAS_CIVIC_ME_PER_HOUR` (10) |
| `source` | Row `source` (may be empty string) |
| `updated` | Row `updated` as ISO-8601 (or `null`) |

### Checklist

1. [x] `GET /rovas/civic_national_hourly_rate?country=SK` returns JSON above
2. [x] Default country when `country` omitted
3. [x] 404 when country has no positive rate
4. [x] Public (anonymous) access; API keys disabled on the resource
5. [x] Enable via updb / ensure helper after deploy (`rovas_civic_update_7003`)
6. [ ] Smoke-test curl below

### Smoke test (after deploy + enable)

```bash
# After deploy: drush74 updb -y && drush74 cc all -y
# or admin: Enable Civic VAT claim + claim-read + rate APIs

curl -sS "https://dev.rovas.app/rovas/civic_national_hourly_rate?country=SK" \
  -H "Accept: application/json"

# Expect hourly_wage_eur > 0 matching admin UI for SK
```

### Civic already consumes this

Civic API `resolveMeritRate()` calls the URL above (5‑minute cache) and falls
back to `HOURLY_WAGE_EUR` if the endpoint 404s / Rovas is down. Proxied as
`GET /merit-rate` for the Expo client. No Civic changes needed once this
ships on rovdev — just deploy + enable.

## Related Civic docs

- Plan: `~/Documents/homeProjects/civic/docs/plan.md` (§6 claim SoT, §7 split claims, §8b claim rule, §15 phases)
- NeoFund: `~/Documents/Business/NeoFund_NP/` (State Treasury, Path-1 donor Merit, A12)
- Civic API/Expo: `~/Documents/homeProjects/civic/docs/rovas-integration.md`
- Mockup (home UX): `~/Documents/homeProjects/civic/mocs/civic-mockup.canvas.tsx`
