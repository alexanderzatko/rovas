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
[Next work: users lookup (community public profile)](#next-work-users-lookup-community-public-profile),
[Next work: hide negative Merit users from Civic](#next-work-hide-negative-merit-users-from-civic),
[Next work: expose account created on /rovas/me](#next-work-expose-account-created-on-rovasme),
[Next work: expose preferred language on /rovas/me](#next-work-expose-preferred-language-on-rovasme),
[Next work: localized project and work-report text](#next-work-localized-project-and-work-report-text),
[Next work: project detail + work reports (Civic modal)](#next-work-project-detail--work-reports-civic-modal),
[Next work: project shareholders + WR photos](#next-work-project-shareholders--wr-photos),
[Next work: private projects are fully readable](#next-work-private-projects-are-fully-readable),
[Next work: merit history (profile ledger)](#next-work-merit-history-profile-ledger),
[Next work: volunteer report (Civic deferred form)](#next-work-volunteer-report-civic-deferred-form),
[Next work: pending verifications gate (assigned slots only)](#next-work-pending-verifications-gate-assigned-slots-only),
[Next work: AI NACE + title from work-report text](#next-work-ai-nace--title-from-work-report-text),
[Next work: CHR to ME on WR verification](#next-work-chr-to-me-on-wr-verification),
and [Next work: geofield on Civic work reports](#next-work-geofield-on-civic-work-reports).
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
| Hide negative Merit users | `users_search` / `users_lookup` | Implemented — `total_me < 0` omitted / 404; search response stays public fields only |
| Friend invite (Civic) | — | **Civic-client only** (`mailto:`). No Rovas invite endpoint. Signup URL in the email body should work (`/user/register` or OAuth authorize). |
| Project detail + WR list | `GET /rovas/civic_project/{nid}`, `GET /rovas/civic_project_work_reports` | Implemented (`rovas_civic_update_7007`). Private participation still returns full content — see [private projects are fully readable](#next-work-private-projects-are-fully-readable). |
| Project shareholders + WR photos | `GET /rovas/civic_project_shareholders`, WR `photo_urls[]` | Implemented in `rovas_civic` (`rovas_civic_update_7008`). |
| Localized project / WR / lookup text | `?lang=` on `civic_project`, `civic_project_work_reports`, `users_lookup` | Implemented — Entity Translation / `title_field` / `body` with source fallback. |
| Merit history (profile ledger) | `GET /rovas/civic_merit_history` | Implemented in `rovas_civic` (`rovas_civic_update_7009`). JSON of Views `merit_history` / `page_1` for the Bearer user. |
| Volunteer report (deferred form) | `GET /rovas/civic_my_projects`, `civic_projects_search`, `civic_nace_tree`; `POST /rovas/civic_create_project`, `civic_create_work_report` | Implemented in `rovas_civic` (`rovas_civic_update_7010`). Bearer only — do **not** call the partner API-KEY rules from Civic. |

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

Outgoing email on dev is logged, not sent (`DevelMailLog`). See
[`docs/dev-setup.md`](../dev-setup.md#outgoing-email-disabled-on-dev).

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
  "total_me": 1248.35,
  "created": "2019-03-12T14:22:01+00:00",
  "language": "sk"
}
```

| Field | Source |
|-------|--------|
| `uid` | Bearer → Drupal uid |
| `username` | `$user->name` |
| `display_name` | `field_full_name` if set; else username |
| `total_me` | `_rovas_merit_distribution_user_merits()` (userpoints + decimal) |
| `created` | ISO-8601 from Drupal `$account->created` (`gmdate('c')`); `null` if missing/invalid |
| `language` | `$account->language` (raw Drupal langcode); `null` if empty/`und` — see [preferred language](#next-work-expose-preferred-language-on-rovasme) |
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
    { "project_nid": "1201", "name": "Detský dom Topoľčany", "me": 120.0 }
  ],
  "top_volunteer_projects": [
    { "project_nid": "1998", "name": "Obnova lesov v Tatranskom NP", "me": 80.0 }
  ]
}
```

| Field | Notes |
|-------|--------|
| `rovas_uid` | Drupal uid as string |
| `username` | Canonical Rovas username |
| `display_name` | Prefer `field_full_name`; fall back to `username` |
| `total_me` | Same source as `/rovas/me` for that uid (`_rovas_merit_distribution_user_merits` or fallback) |
| `by_category.taxes` | `user_expertise` Merits on fallback NACE `ROVAS_CIVIC_FALLBACK_NACE_TID` (64.99 / tid **1601**) only — exclusive source |
| `by_category.charity` | `user_expertise` Merits on `ROVAS_CIVIC_CHARITY_NACE_TID` when that constant is a positive tid; **0** until a dedicated charity NACE is configured |
| `by_category.volunteer` | Sum of `user_expertise` Merits whose NACE is **neither** the fallback tid nor the charity tid |
| `taxes_last_3_months` | Last **3 calendar months** (including current) of VAT-claim Merits for this uid, keyed `YYYY-MM`; omit zero months or include with `me: 0` — prefer only months with activity; max 3 rows; `[]` if none |
| `top_charities` | Up to **3** `{ project_nid, name, me }` from `{rovas_merit_issuance}` rows with `source_type = project_reward_order` (Path-1 donor Merits from project donations). **Not** raw `purchase_receipt` Merit spends. Exclude State Treasury. Empty until real charity donations exist. |
| `top_volunteer_projects` | Up to **3** `{ project_nid, name, me }` from `project_merit_allocation` / `co_worker_merit_allocation` labor Merits (aggregated by project nid) |

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

## Next work: hide negative Merit users from Civic

**Implemented in `rovas_civic`.** Filtering belongs on Rovas (not the Civic API
proxy): `users_search` does not expose wallet balance, so Civic cannot filter
typeahead without either leaking `total_me` or N+1 lookups.

Users with a **negative** Merit balance (`total_me < 0`) are omitted from
`users_search` and return **404** from `users_lookup`. Zero and positive
balances are fine. Search response shape is unchanged
(`{ rovas_uid, username, display_name }` only). Shared helper:
`rovas_civic_user_total_me()`.

### Endpoints

| Endpoint | Behaviour when `total_me < 0` |
|----------|-------------------------------|
| `GET /rovas/users_search?query=` | Omit the user from the result list (oversample so the 10-row cap still fills) |
| `GET /rovas/users_lookup?username=` | Treat as not found → **404** (same as unknown / not searchable) |

### Checklist

1. [x] `users_search` skips uids with `total_me < 0`; response shape unchanged
2. [x] `users_lookup` returns 404 for negative-ME usernames
3. [x] Shared Merit helper (`rovas_civic_user_total_me`)
4. [ ] Smoke-test: search a known negative-ME user → absent; lookup → 404;
      positive-ME user still works
5. [ ] Deploy to rovdev when human asks (`ops/deploy_rovdev.sh` / targeted rsync)

### Smoke test

```bash
# After deploy: drush74 cc all -y
# Pick a known negative-ME username on rovdev, then:
curl -sS "https://dev.rovas.app/rovas/users_search?query=<prefix>" \
  -H "Authorization: Bearer $ACCESS_TOKEN" \
  -H "Accept: application/json"
# → user must not appear

curl -sS "https://dev.rovas.app/rovas/users_lookup?username=<negative_user>" \
  -H "Authorization: Bearer $ACCESS_TOKEN" \
  -H "Accept: application/json"
# → 404
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
rate, user-search, and project reads use machine-name paths
(`/rovas/civic_claims`, `/rovas/civic_national_hourly_rate`,
`/rovas/users_search`, `/rovas/civic_project`,
`/rovas/civic_project_work_reports`, `/rovas/civic_project_shareholders`),
not `/rovas/civic/…` or `/rovas/users/search`.

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

## Next work: expose account created on /rovas/me

**Implemented in `rovas_oauth`.** Civic fraud rule C rejects VAT claims when
the receipt `purchase_at` (Bratislava calendar day) is strictly before the
scanner’s Rovas registration day. Civic already parses `created` from
`GET /rovas/me` and **fails closed** on claim when the field is missing.

### Change

In `rovas_oauth` → `rovas_oauth_me_index()`:

1. Read Drupal `$account->created` (Unix seconds).
2. Emit ISO-8601 via `gmdate('c', (int) $account->created)` (or equivalent).
3. Include `"created"` in the JSON response alongside `uid`, `username`,
   `display_name`, `total_me`.
4. If `created` is empty/invalid, return `null` (Civic treats null as missing
   and rejects the claim with `rejected_account_age`).

No new Services resource — extend the existing `/rovas/me` payload only.

### Response (after)

```json
{
  "uid": 42,
  "username": "sano",
  "display_name": "Alexander Žaťko",
  "total_me": 1248.35,
  "created": "2019-03-12T14:22:01+00:00"
}
```

| Field | Source |
|-------|--------|
| `created` | `$account->created` → ISO-8601 (UTC/`gmdate('c')`) |

### Checklist

1. [x] `rovas_oauth_me_index()` returns `created` as ISO-8601 string
2. [x] Existing clients still work (additive field)
3. [ ] Deploy to rovdev + `drush74 cc all`
4. [ ] Smoke-test curl below

### Smoke test (after deploy)

```bash
# TOKEN = Civic OAuth access token with user_profile scope
curl -sS https://dev.rovas.app/rovas/me \
  -H "Authorization: Bearer $TOKEN" \
  -H "Accept: application/json"
# Expect: "created": "YYYY-MM-DDTHH:MM:SS+00:00" (or similar ISO-8601)
```

### Civic already consumes this

Civic `RovasClient.fetchMe()` maps `created`. Claim/verify fraud evaluation
uses it for rule C (`rejected_account_age` when purchase day < registration
day, or when `created` is absent).

## Next work: expose preferred language on /rovas/me

**Implemented in `rovas_oauth`.** Civic uses this after login to pick the UI
language (`sk` / `en`), with device preferred languages for anonymous users
and an in-app override on the Civic profile screen.

### Mapping Civic applies (do not change without Civic agreement)

| Rovas / device code | Civic UI |
|---------------------|----------|
| `sk` | `sk` |
| `cs` / `cz` | `sk` (until Civic ships a Czech catalog) |
| anything else | `en` |

Same spirit as `rovas_base_map_partner_user_language()` (cs/cz → sk).

### Change

In `rovas_oauth` → `rovas_oauth_me_index()`:

1. Read Drupal `$account->language` (user preferred interface language).
2. Include `"language"` in the JSON response (raw Drupal langcode string,
   e.g. `sk`, `en`, `cs`). Empty / unset → `null` or omit (Civic falls back
   to device language).
3. Do **not** map cs→sk on the Rovas side for this field — emit the stored
   account language; Civic maps for its catalog.

No new Services resource — extend the existing `/rovas/me` payload only.
Optional later: a write endpoint so the Civic profile language switcher can
persist back to `$account->language` (not required for v1).

### Response (after)

```json
{
  "uid": 42,
  "username": "sano",
  "display_name": "Alexander Žaťko",
  "total_me": 1248.35,
  "created": "2019-03-12T14:22:01+00:00",
  "language": "sk"
}
```

| Field | Source |
|-------|--------|
| `language` | `$account->language` (Drupal user preferred language); `null` if empty |

### Checklist

1. [x] `rovas_oauth_me_index()` returns `language` (string or null)
2. [x] Existing clients still work (additive field)
3. [ ] Deploy to rovdev + `drush74 cc all`
4. [ ] Smoke-test curl below

### Smoke test (after deploy)

```bash
# TOKEN = Civic OAuth access token with user_profile scope
curl -sS https://dev.rovas.app/rovas/me \
  -H "Authorization: Bearer $TOKEN" \
  -H "Accept: application/json"
# Expect: "language": "sk" | "en" | "cs" | … (or null)
```

### Civic already consumes this

Civic API `GET /me` proxies `language` from Rovas. Expo `LocaleProvider`
applies it after login (unless the user set an explicit Civic override).

## Next work: localized project and work-report text

**Implemented in `rovas_civic`.** Civic UI chrome is already `sk`/`en`, and the
Expo client + Civic API send `?lang=sk|en` on project/WR/lookup requests.
Rovas honors `lang` so Entity Translation / `title_field` / `body` values
match the Civic language when a translation exists.

### Civic → Rovas contract (already live on Civic side)

| Civic API | Forwards to Rovas |
|-----------|-------------------|
| `GET /projects/:nid?lang=` | `GET /rovas/civic_project/{nid}?lang=` |
| `GET /projects/:nid/work-reports?lang=` | `GET /rovas/civic_project_work_reports?project_nid=&lang=` |
| `GET /users/lookup?username=&lang=` | `GET /rovas/users_lookup?username=&lang=` |

| Param | Rules |
|-------|--------|
| `lang` | Optional. Civic sends `sk` or `en` only. Treat `cs`/`cz` as `sk` if ever received. Missing → keep current behavior (source/default). |

Shareholders endpoint stays language-agnostic (no translated copy).

### Fields that must respect `lang`

| Endpoint | Fields |
|----------|--------|
| `civic_project` | `title`, `summary`; embedded `work_reports.items[].title`, `.summary` |
| `civic_project_work_reports` | `items[].title`, `items[].summary` |
| `users_lookup` | `top_charities[].name`, `top_volunteer_projects[].name` |

### Required logic (`rovas_civic`)

1. Resolve requested langcode from `$_GET['lang']` (primary). Optionally also
   accept `Accept-Language` if `lang` is absent — same mapping as above.
2. Constrain to an **enabled** Drupal interface/content language (reuse
   `rovas_base_normalize_to_enabled_language()` when available).
3. For each project / work_report node:
   - **Title:** prefer `title_field[$lang][0]['value']` when Title + Entity
     Translation are enabled; else fall back to `$node->title`. If the
     requested lang has no title translation, fall back to Entity Translation
     source language (`rovas_base_translation_node_source_language()` /
     `rovas_base_translation_pick_source_text($node, 'title_field')`), then
     `$node->title`.
   - **Body / summary:** read `body` for `$lang` (via `field_get_items` with
     language, or direct `$node->body[$lang]`), then source, then
     `LANGUAGE_NONE`. Keep existing `rovas_civic_plain_teaser()` rules.
4. For `users_lookup` top project rows: today SQL uses `{node}.title`. After
   aggregating nids, `node_load_multiple` and resolve each `name` with the
   same title picker as above (do not leave SQL `pn.title` as the only
   source when `lang` is set).
5. Do **not** fail the request when a translation is missing — always return
   a string with source fallback.
6. Additive only: omitting `lang` must behave as today.

### Checklist

1. [x] `civic_project` + embedded WR items honor `?lang=`
2. [x] `civic_project_work_reports` honors `?lang=`
3. [x] `users_lookup` top charity/volunteer `name` honors `?lang=`
4. [x] Missing translation → source/default (no empty title)
5. [ ] Deploy to rovdev + `drush74 cc all`
6. [ ] Smoke-test curl below on a node with both `sk` and `en` title_field

### Smoke test (after deploy)

```bash
# TOKEN = Civic OAuth access token; NID = project with sk+en translations
curl -sS "https://dev.rovas.app/rovas/civic_project/$NID?lang=en" \
  -H "Authorization: Bearer $TOKEN" -H "Accept: application/json"
# Expect title/summary (and WR titles) in English when translations exist

curl -sS "https://dev.rovas.app/rovas/civic_project/$NID?lang=sk" \
  -H "Authorization: Bearer $TOKEN" -H "Accept: application/json"
# Expect Slovak variants when translations exist

curl -sS "https://dev.rovas.app/rovas/users_lookup?username=sano&lang=en" \
  -H "Authorization: Bearer $TOKEN" -H "Accept: application/json"
# Expect top_*[].name in English when project titles are translated
```

### Civic already consumes this

Expo sends `lang` from the active Civic locale on lookup + project + WR
fetches. Civic API proxies it. No further Civic change needed once Rovas
honors the param.
## Next work: project detail + work reports (Civic modal)

**Implemented in `rovas_civic`** (enable on rovdev: `drush74 updb -y`, or admin
**Enable Civic … + project APIs**). Civic opens an in-app modal when the user
taps a project name on Home / Community merit-source tables
(`top_charities`, `top_volunteer_projects`). Modal content is **JSON atomic
fields**, not embedded Rovas HTML. Work reports expand **in place** (no
nested modal).

**Auth:** Bearer required (`civic` / `user_profile`) — same as `users_lookup`.

### Additive: `project_nid` on lookup rows

`top_charities` / `top_volunteer_projects` items become
`{ project_nid, name, me }` (`project_nid` string). Backward compatible for
clients that ignore unknown fields.

### Endpoints

| Method | URL | Notes |
|--------|-----|------|
| `GET` | `https://dev.rovas.app/rovas/civic_project/{nid}` | Project summary + first page of work reports |
| `GET` | `https://dev.rovas.app/rovas/civic_project_work_reports?project_nid=&limit=&offset=` | Paginated work reports for a project |

Machine-name paths only (no slash aliases).

### `GET /rovas/civic_project/{nid}`

**200** published project:

```json
{
  "project_nid": "1998",
  "title": "Openstreetmap",
  "summary": "Plain-text body teaser…",
  "is_public": true,
  "owner_username": "alice",
  "owner_display_name": "Alice",
  "total_hours": 1234.5,
  "total_me": 980.0,
  "work_report_count": 6898,
  "rovas_url": "https://dev.rovas.app/node/1998",
  "work_reports": {
    "project_nid": "1998",
    "total": 6898,
    "limit": 20,
    "offset": 0,
    "items": [
      {
        "work_report_nid": "527201",
        "title": "Odstranovanie kriakov",
        "summary": "Plain-text body teaser…",
        "hours": 0.75,
        "author_username": "bob",
        "author_display_name": "Bob",
        "created_at": "2026-06-01T12:00:00+00:00"
      }
    ]
  }
}
```

| Field | Source / rules |
|-------|----------------|
| `summary` | `strip_tags` + decode entities on `body`; max ~800 chars; `null` if empty. Always include for published projects (do **not** omit for Private participation). |
| `is_public` | Informational only: `field_participation` ∈ {Public (1), Public-dependent (2)}. **Private (0) is still fully readable** — it only means the owner controls who may join as a shareholder (not “hide from Civic”). Full summary, WR list, photos, and shareholders for all published projects. |
| `total_hours` | `field_project_total_hours` (float or null) |
| `total_me` | `field_merit_reward` (float or null) |
| `work_report_count` / `work_reports.total` | Count of published `work_report` nodes with `field_project` → this nid |
| `created_at` (WR) | Prefer `field_date` unix; else node `created` → ISO-8601 |
| `hours` (WR) | `field_hours_worked` |
| `summary` (WR) | Same plain-text teaser rules as project body |

**404** — unknown nid, wrong type, or unpublished (`status != 1`).

Default first WR page: `limit=20` (max 50), `offset=0`.

### `GET /rovas/civic_project_work_reports`

| Param | Rules |
|-------|--------|
| `project_nid` | Required positive int |
| `limit` | Default 20, max 50 |
| `offset` | Default 0 |

Return full published WR pages for **any** published project (including Private participation). Return **404** only if project missing/unpublished.

Order work reports by `created` DESC (then nid DESC).

### Required logic

1. Resolve Bearer uid (reject anonymous)
2. Load project node; require `type=project` + published
3. Return full payload for all published projects — **do not** strip body/WRs when `field_participation` is Private
4. Enable via `rovas_civic_ensure_services_resources()` + `rovas_civic_update_7007`

### Checklist

1. [x] `project_nid` on `users_lookup` top_* rows
2. [x] Services `GET /rovas/civic_project/{nid}`
3. [x] Services `GET /rovas/civic_project_work_reports`
4. [x] Bearer + API-key disabled on both resources
5. [x] updb / ensure helper
6. [ ] Smoke-test curls below

### Smoke test (after deploy)

```bash
# TOKEN = Civic OAuth access token
curl -sS "https://dev.rovas.app/rovas/users_lookup?username=sano" \
  -H "Authorization: Bearer $TOKEN" -H "Accept: application/json"
# Expect top_* items to include project_nid when present

curl -sS "https://dev.rovas.app/rovas/civic_project/1998" \
  -H "Authorization: Bearer $TOKEN" -H "Accept: application/json"

curl -sS "https://dev.rovas.app/rovas/civic_project_work_reports?project_nid=1998&limit=5" \
  -H "Authorization: Bearer $TOKEN" -H "Accept: application/json"
```

### Civic will consume (after Rovas ships)

Civic side is wired to this contract:

- Proxy `GET /projects/:nid` and `GET /projects/:nid/work-reports`
- Expo `ProjectDetailModal` + clickable `ProjectMeTable` rows (needs `project_nid`)

**Implemented in `rovas_civic`** (Services resources + ensure helper + updb). Do not
change Civic Expo/API in this repo.

## Next work: project shareholders + WR photos

**Implemented in `rovas_civic`** (enable on rovdev: `drush74 updb -y`, or admin
enable APIs). Extends the Civic project modal with (1) a paginated
**shareholders** list matching the Rovas `project_coworkers` view user rows,
and (2) **photos** attached to work ledgers (`field_photos` on `work_report`).

**Auth:** Bearer required (`civic` / `user_profile`).

Shareholders are a non-Views SQL query in `rovas_civic` mirroring
`project_coworkers` public columns for **user** rows only
(`co_worker_merit_allocation` + `field_coworker`). Project-as-shareholder
rows are omitted.

### Additive: `photo_urls` on work-ledger items

On both `GET /rovas/civic_project/{nid}` embedded `work_reports.items[]` and
`GET /rovas/civic_project_work_reports` items, add:

```json
"photo_urls": [
  "https://dev.rovas.app/sites/default/files/…"
]
```

| Field | Rules |
|-------|--------|
| `photo_urls` | Absolute HTTPS URLs from `field_photos` via `file_create_url` (or equivalent). Empty array when none. Include for Private participation projects too. Omit broken/missing files. Prefer image styles only if a stable public style already exists; otherwise original file URL is fine. |

### Endpoint: shareholders

| Method | URL | Notes |
|--------|-----|------|
| `GET` | `https://dev.rovas.app/rovas/civic_project_shareholders?project_nid=&limit=&offset=` | Paginated; machine-name path |

Also embed the **first page** on `GET /rovas/civic_project/{nid}` as
`shareholders` (same page shape) plus `shareholder_count`.

**200** example page:

```json
{
  "project_nid": "1998",
  "total": 42,
  "limit": 20,
  "offset": 0,
  "items": [
    {
      "username": "bob",
      "share_kind": "percent",
      "share_percent": 12.5,
      "hours": null
    },
    {
      "username": "alice",
      "share_kind": "time_based",
      "share_percent": null,
      "hours": 3.5
    }
  ]
}
```

| Field | Source / rules (match Rovas shareholder view logic) |
|-------|--------|
| `username` | Rovas **username** (not full name). From coworker user for worker shares; skip pure project-as-shareholder rows **or** omit them from Civic (Civic only needs user usernames linkable to Komunita). |
| `share_kind` | `"percent"` when `field_merit_allocation` > 0; else `"time_based"` (Rovas view prints “work time share”). |
| `share_percent` | `field_merit_allocation` when percent; else `null`. |
| `hours` | `field_total_hours_worked` (float or null) — especially meaningful for time-based shares. |

Visibility: same as project detail — return shareholders for any **published**
project (including Private participation). **404** only if project
missing/unpublished.

Pagination: default `limit=20`, max `50`, `offset` ≥ 0.

Enable via `rovas_civic_ensure_services_resources()` +
`rovas_civic_update_7008`.

### Checklist

1. [x] WR items include `photo_urls[]` (project retrieve + WR list)
2. [x] `GET /rovas/civic_project_shareholders`
3. [x] First shareholders page + `shareholder_count` on `civic_project` retrieve
4. [x] Bearer + API-key disabled
5. [x] updb / ensure helper
6. [ ] Smoke-test curls

### Smoke test

```bash
curl -sS "https://dev.rovas.app/rovas/civic_project/1998" \
  -H "Authorization: Bearer $TOKEN" -H "Accept: application/json"
# Expect shareholders + photo_urls on WR items when present

curl -sS "https://dev.rovas.app/rovas/civic_project_shareholders?project_nid=1998&limit=5" \
  -H "Authorization: Bearer $TOKEN" -H "Accept: application/json"
```

### Civic already consumes

- Proxy `GET /projects/:nid/shareholders`
- Modal section Podielnici / Shareholders; WR photos in expand panel

## Next work: private projects are fully readable

**Implemented in `rovas_civic`.** Civic initially treated Rovas
`field_participation = Private (0)` as “hide project content”. That is
**wrong**.

On Rovas, **Private** only means the **owner controls who may be added as a
shareholder** (others cannot freely join). It does **not** mean the project,
summary, work ledgers, photos, or shareholders are secret.

### Fix in `rovas_civic`

1. `GET /rovas/civic_project/{nid}` — return full payload for all published
   projects regardless of `field_participation`. Keep `is_public` as an
   informational boolean only.
2. `GET /rovas/civic_project_work_reports` — return WR pages for Private
   projects too (do not empty `items` when private).
3. `GET /rovas/civic_project_shareholders` — same; include Private projects.
4. `photo_urls` — include when photos exist on Private projects.
5. Update any helpers that currently strip body / WRs / shareholders when
   `rovas_civic_project_is_public()` is false.
6. Deploy + `drush74 cc all` (updb only if a new schema hook is needed).

### Checklist

1. [x] Private published project returns summary + WR list + shareholders
2. [x] `is_public: false` still reported, but content is complete
3. [ ] Smoke-test a known Private project nid from Civic / curl

### Smoke test

```bash
# Pick a published Private project nid from the DB, then:
curl -sS "https://dev.rovas.app/rovas/civic_project/$PRIVATE_NID" \
  -H "Authorization: Bearer $TOKEN" -H "Accept: application/json"
# Expect is_public:false AND non-empty summary / work_reports when data exists
```

## Next work: merit history (profile ledger)

**Implemented in `rovas_civic`** (enable on rovdev: `drush74 updb -y`, or admin
enable APIs). Civic’s **Profil** screen needs the same Merit ledger Rovas already shows at
`/user/<uid>/merits` — Views **`merit_history`**, display **`page_1`**.

Do **not** scrape HTML or call `views_get_view()` / `views_embed_view()`. Add a
JSON Services resource in `rovas_civic` that **mirrors that view with SQL**
(same pattern as `civic_project_shareholders` vs `project_coworkers`).

**Auth:** Bearer required (`civic` / `user_profile`). History is **only for the
caller** (`field_reward_recipient` = Bearer uid). Ignore any `uid` / `username`
query param (do not expose another user’s ledger).

### What the view shows (parity)

| Views field | Role | Civic JSON |
|-------------|------|------------|
| `nid` (Receipt Id) | Disbursement / penalty node | `disbursement_nid` |
| `created` (Date) | Node created | `created_at` ISO-8601 |
| PHP **Source** | See rules below | `source_kind` + `source_title` + nids |
| `field_reward_amount` (Reward) | ME credited (can be negative) | `merits` |
| `field_receipt_project` (hidden) | Project on parent `purchase_receipt` | `project_nid` |
| `field_rewarded_report` (hidden) | Work report on parent receipt | `work_report_nid` |

HTML-only columns Civic does **not** need: Buyer (`field_buyer`), Amount paid
(`field_amount_paid`). Skip them.

**Node types** (view filter): published `merits_disbursement` **and**
`merit_penalty`.

**Source PHP on `page_1` (reproduce exactly, without HTML):**

1. No project **and** no work report → `source_kind: "purchase"`,
   `source_title: null` (Rovas label *Merits purchase* — Civic i18n’s that).
2. Project, no work report → `source_kind: "project"`,
   `source_title` = localized project title, `project_nid` set.
3. Work report present → `source_kind: "report"`,
   `source_title` = localized WR title, `work_report_nid` set,
   `project_nid` still included when known.

VAT claims and charity donations are **project** rows (State Treasury / funded
project). Volunteer work is **report**. Bought Merits are **purchase**.

### Endpoint

| Method | URL | Auth |
|--------|-----|------|
| `GET` | `https://dev.rovas.app/rovas/civic_merit_history` | Bearer required |

Machine-name path only (no slash alias).

| Param | Rules |
|-------|--------|
| `limit` | Default **20**, max **50** |
| `offset` | Default 0 |
| `lang` | Optional `sk` \| `en` — same Entity Translation / `title_field` rules as `civic_project` (source fallback). Used for `source_title` only. |

### 200 response

```json
{
  "total": 128,
  "limit": 20,
  "offset": 0,
  "items": [
    {
      "disbursement_nid": "612345",
      "node_type": "merits_disbursement",
      "created_at": "2026-08-12T14:22:00+00:00",
      "merits": 6.43,
      "source_kind": "project",
      "source_title": "State Treasury of Slovakia",
      "project_nid": "1998",
      "work_report_nid": null
    },
    {
      "disbursement_nid": "612300",
      "node_type": "merits_disbursement",
      "created_at": "2026-06-01T12:00:00+00:00",
      "merits": 7.5,
      "source_kind": "report",
      "source_title": "Trail clearing",
      "project_nid": "1998",
      "work_report_nid": "527201"
    },
    {
      "disbursement_nid": "610001",
      "node_type": "merits_disbursement",
      "created_at": "2026-01-15T09:00:00+00:00",
      "merits": 100.0,
      "source_kind": "purchase",
      "source_title": null,
      "project_nid": null,
      "work_report_nid": null
    }
  ]
}
```

| Field | Rules |
|-------|--------|
| `disbursement_nid` | String nid of the `merits_disbursement` / `merit_penalty` node |
| `node_type` | `merits_disbursement` or `merit_penalty` |
| `created_at` | Node `created` → ISO-8601 (`rovas_civic_format_created`) |
| `merits` | `field_reward_amount` float (negative allowed for penalties) |
| `source_kind` | `project` \| `report` \| `purchase` only |
| `source_title` | Localized entity title, or `null` for `purchase` |
| `project_nid` / `work_report_nid` | String nids or `null` |

Order: `created` DESC, then nid DESC. Empty ledger → `total: 0`, `items: []`
(still 200).

### SQL sketch (view relationships)

Base: `{node} n` (`status = 1`, `type IN ('merits_disbursement','merit_penalty')`).

- `{field_data_field_reward_recipient}` on `n.nid` = Bearer uid (required)
- `{field_data_field_reward_amount}` on `n.nid`
- LEFT `{field_data_field_parent_purchase_receipt}` on `n.nid` → receipt nid
- LEFT `{field_data_field_receipt_project}` on **receipt** nid
- LEFT `{field_data_field_rewarded_report}` on **receipt** nid

Do not apply the view’s **exposed** Project / Buyer filters (HTML-only).

Reuse `rovas_civic_node_localized_title()` for project + WR titles. Reuse
`rovas_civic_normalize_pager($limit, $offset, 20, 50)`.

### Required logic

1. Resolve Bearer uid (reject anonymous)
2. Count + page the SQL above
3. Map Source PHP → `source_kind` / titles / nids
4. Register `civic_merit_history` **index** on `rovas_endpoint` (Bearer,
   API-key disabled) via `rovas_civic_ensure_services_resources()`
5. Enable with `rovas_civic_update_7009` + `drush74 updb -y`

### Checklist

1. [x] Services `GET /rovas/civic_merit_history`
2. [x] Bearer-only; ledger is the caller (reward recipient)
3. [x] Types `merits_disbursement` + `merit_penalty`; published only
4. [x] Source kinds match Views PHP (`purchase` / `project` / `report`)
5. [x] `?lang=` localizes `source_title`
6. [x] Pagination `limit`/`offset` (default 20, max 50) + `total`
7. [x] `rovas_civic_ensure_services_resources()` + `rovas_civic_update_7009`
8. [ ] Smoke-test curl below after deploy

### Smoke test (after deploy)

```bash
# TOKEN = Civic OAuth access token
curl -sS "https://dev.rovas.app/rovas/civic_merit_history?limit=5&lang=sk" \
  -H "Authorization: Bearer $TOKEN" -H "Accept: application/json"
# Expect { total, limit, offset, items[] } with source_kind in
# project|report|purchase. Compare a few rows to /user/<uid>/merits.

curl -sS "https://dev.rovas.app/rovas/civic_merit_history" \
  -H "Accept: application/json"
# Expect 401 / 403 (no anonymous)
```

### Civic will consume (after Rovas ships)

- Proxy `GET /users/me/merit-history?limit=&offset=&lang=`
- Expo Profil list: date, source, whole-number ME; project rows open the
  existing project modal

## Next work: volunteer report (Civic deferred form)

Civic screen 4 (Dobrovoľnícka práca) submits a simplified work report. **Do
not** call the partner API-KEY rules (`rules_proxy_create_new_project`,
`rules_proxy_create_work_report`, `rules_proxy_check_or_add_shareholder`) from
Civic — those resolve the user via API-KEY/TOKEN. Civic only has OAuth Bearer.

New `rovas_civic` Services resources, Bearer via `$user` /
`rovas_oauth_uid_from_bearer()`, JSON errors. Internally invoke
`rules_create_project`, `rules_check_or_add_shareholder_core`,
`rules_create_work_report`. Enable with `rovas_civic_update_7010`.

Machine-name paths (no `?q=` — Drupal path conflict). Optional `?lang=sk|en`
on reads (same rules as project detail).

### Reads

| Method | URL | Returns |
|--------|-----|---------|
| `GET` | `/rovas/civic_my_projects?lang=` | `{ project_nid, title, is_public, can_report }[]` — up to **3** published projects where the Bearer user authored work reports, ordered by most recent report (`work_report` node `created` DESC). Empty when the user has never reported. `can_report`: owner, shareholder, or public join |
| `GET` | `/rovas/civic_projects_search?query=&lang=` | Same row shape. `query` ≥ 3 chars, cap 10, published `project` nodes, title LIKE. `can_report` reflects whether the Bearer user may submit (not merely `is_public`) |
| `GET` | `/rovas/civic_nace_tree?lang=` | Nested `{ tid, code, label, children[] }` from vocabulary `tags` |

### Writes

| Method | URL | Body / result |
|--------|-----|----------------|
| `POST` | `/rovas/civic_create_project` | `{ name, description, url? }` → `{ project_nid }`. Defaults: `project_status=1` (for sale), `project_participation=1` (Public), not a system project. Creator becomes shareholder via existing node-insert rules |
| `POST` | `/rovas/civic_create_work_report` | See below |

**Create work report body:**

```json
{
  "project_nid": "1998",
  "date_started": 1750000000,
  "hours": 4,
  "description": "Cleared invasive plants along the trail.",
  "title": "Trail clearing",
  "nace_tid": "860",
  "web_url": "https://example.org/proof",
  "reward_kind": "merits",
  "photos": [
    { "filename": "site.jpg", "mime": "image/jpeg", "data": "<base64>" }
  ]
}
```

| Field | Rules |
|-------|--------|
| `project_nid` | Required published project |
| `date_started` | Unix timestamp; reject if strictly before the author's Rovas `created` |
| `hours` | Required, `> 0` |
| `description` | Required non-empty |
| `title` | Optional; empty → truncated description |
| `nace_tid` | Required taxonomy tid in vocabulary `tags` |
| `web_url` | Optional proof URL |
| `photos` | Optional array of `{ filename, mime, data }` (base64). Max 5, 4 MB decoded each |
| `reward_kind` | `merits` or `chrons` (default `chrons`). Persisted in `{civic_work_report_reward}` for a later CHR→ME rule — **not** applied at create time |

At least one of `web_url` or `photos[]` is required. Photos attach to
`field_photos` after the WR node exists (partner create-WR required a URL;
Civic must accept photos without a URL).

Before create: `rules_check_or_add_shareholder_core`. Private + not already a
shareholder → **403** `{ "error": "not_shareholder" }`.

Typed errors (HTTP + `{ error, message }`):

| `error` | HTTP | Meaning |
|---------|------|---------|
| `not_shareholder` | 403 | Private project; caller cannot self-join |
| `date_before_registration` | 400 | `date_started` before account created |
| `pending_verifications` | 403 | Caller has at least one verification slot with a report assigned to verify |
| `invalid_input` | 400 | Missing/invalid fields, no proof, hours ≤ 0 |
| `project_not_found` | 404 | Unknown / unpublished project |

Success: `{ "work_report_nid": "527201", "reward_kind": "merits" }`.

### Checklist

1. [x] `GET /rovas/civic_my_projects`
2. [x] `GET /rovas/civic_projects_search` (`query`, never `q`)
3. [x] `GET /rovas/civic_nace_tree`
4. [x] `POST /rovas/civic_create_project` (status=1, public)
5. [x] `POST /rovas/civic_create_work_report` (shareholder join, photos and/or URL, `reward_kind`)
6. [x] Bearer-only; API-keys disabled; `rovas_civic_ensure_services_resources()` + `rovas_civic_update_7010`
7. [ ] Smoke-test curl below after deploy

### Smoke test (after deploy)

```bash
# TOKEN = Civic OAuth access token
curl -sS "https://dev.rovas.app/rovas/civic_my_projects?lang=sk" \
  -H "Authorization: Bearer $TOKEN" -H "Accept: application/json"

curl -sS "https://dev.rovas.app/rovas/civic_projects_search?query=les&lang=sk" \
  -H "Authorization: Bearer $TOKEN" -H "Accept: application/json"

curl -sS "https://dev.rovas.app/rovas/civic_nace_tree?lang=sk" \
  -H "Authorization: Bearer $TOKEN" -H "Accept: application/json"

curl -sS -X POST "https://dev.rovas.app/rovas/civic_create_project" \
  -H "Authorization: Bearer $TOKEN" -H "Content-Type: application/json" \
  -d '{"name":"Civic volunteer test","description":"Public test project from Civic."}'

curl -sS "https://dev.rovas.app/rovas/civic_create_project" \
  -H "Accept: application/json"
# Expect 401 / 403 (no anonymous)
```

### Civic will consume

- `GET /projects/mine`, `GET /projects/search?q=`, `POST /projects`
- `GET /nace?lang=`
- `POST /work-reports` (photos as base64)

## Next work: pending verifications gate (assigned slots only)

**Implemented in `rovas_civic`.** `POST /rovas/civic_create_work_report` was returning **403**
`pending_verifications` even when the caller has **no report assigned to verify**.

**Repro (rovdev, Aug 2026):** uid **289** (`sano`) filed two work reports
(527174, 527201). Each create added two obligation `verification_slot` nodes
(527175–527176, 527202–527203). All four slots have
`field_report_to_verify` **empty**, yet create is blocked. Civic shows
“Najprv overte výkazy…” but neither Civic nor the Rovas `pending_verifications`
view lists anything actionable (that view only shows slots with a report
assigned).

**Root cause:** `rovas_civic_pending_verify_slot_count()` in
`sites/all/modules/custom/rovas_civic/rovas_civic.volunteer.inc` counts slots
where `field_report_to_verify_target_id IS NULL OR = 0` (empty obligation
slots). The create gate fires when that count is **`> 1`**.

**Required behaviour:** only count slots that have a work report **assigned**
(`field_report_to_verify_target_id > 0`). Empty obligation slots are not
actionable and must **not** block new filings. Block when the assigned count is
**`> 0`** (any pending verification obligation with a report to act on).

### Code changes

File: `sites/all/modules/custom/rovas_civic/rovas_civic.volunteer.inc`

**1. Create gate (~line 429)** — change:

```php
if (rovas_civic_pending_verify_slot_count($uid) > 1) {
```

to:

```php
if (rovas_civic_pending_verify_slot_count($uid) > 0) {
```

**2. `rovas_civic_pending_verify_slot_count()`** — replace the function body.
Update the docblock: count slots **with an assigned report**, not “unfilled”
slots.

```php
function rovas_civic_pending_verify_slot_count($uid) {
  $uid = (int) $uid;
  if ($uid < 1 || !field_info_field('field_slip_owner') || !field_info_field('field_report_to_verify')) {
    return 0;
  }
  $sql = "
    SELECT COUNT(n.nid)
    FROM {node} n
    INNER JOIN {field_data_field_slip_owner} o
      ON o.entity_id = n.nid AND o.entity_type = 'node' AND o.deleted = 0
    INNER JOIN {field_data_field_report_to_verify} r
      ON r.entity_id = n.nid AND r.entity_type = 'node' AND r.deleted = 0
    WHERE n.type = 'verification_slot'
      AND n.status = 1
      AND o.field_slip_owner_target_id = :uid
      AND r.field_report_to_verify_target_id IS NOT NULL
      AND r.field_report_to_verify_target_id > 0
  ";
  return (int) db_query($sql, array(':uid' => $uid))->fetchField();
}
```

Remove the old fallback that counted all slots when `field_report_to_verify` is
missing — return `0` instead (cannot detect assigned reports without the field).

**No Services resource or schema change.** No Civic API/Expo change (Civic
already maps `pending_verifications` to a user-facing string).

### Error response (unchanged shape)

HTTP **403**, body:

```json
{
  "error": "pending_verifications",
  "message": "Verify outstanding work reports before filing another."
}
```

Semantics after fix: returned only when the Bearer user owns at least one
published `verification_slot` with `field_report_to_verify` pointing at a
work report.

### Checklist

1. [x] Apply both edits in `rovas_civic.volunteer.inc`
2. [ ] Deploy to **rovdev** only (`/var/www/rovdev`)
3. [ ] Smoke: uid 289 with empty slots only — `POST /rovas/civic_create_work_report` succeeds
4. [ ] Smoke: manually assign a WR to one of the caller's slots (or wait for
       `rules_find_verifier`) — same POST returns 403 `pending_verifications`
5. [ ] Confirm `pending_verifications` view rows match what blocks create

### Smoke test (after deploy)

```bash
# TOKEN = Civic OAuth access token for a user with empty obligation slots only
# (e.g. sano / uid 289 on rovdev after two prior reports)
curl -sS -X POST "https://dev.rovas.app/rovas/civic_create_work_report" \
  -H "Authorization: Bearer $TOKEN" -H "Content-Type: application/json" \
  -d '{
    "project_nid": "527225",
    "date_started": 1750000000,
    "hours": 1,
    "description": "Civic pending-verifications gate test",
    "nace_tid": "860",
    "web_url": "https://example.org/proof",
    "reward_kind": "merits"
  }'
# Expect 200: { "work_report_nid": "…", "reward_kind": "merits" }

# Optional DB check before/after:
# drush sqlq "SELECT s.nid, r.field_report_to_verify_target_id AS wr_nid
#   FROM node s
#   INNER JOIN field_data_field_slip_owner o ON o.entity_id = s.nid AND o.deleted = 0
#   LEFT JOIN field_data_field_report_to_verify r ON r.entity_id = s.nid AND r.deleted = 0
#   WHERE s.type = 'verification_slot' AND s.status = 1
#   AND o.field_slip_owner_target_id = 289"

# After a report is assigned to one of the caller's slots — repeat POST
# Expect 403: { "error": "pending_verifications", "message": "…" }
```

## Next work: AI NACE + title from work-report text

**Not implemented.** Target: a new `rovas_ai` module that classifies NACE and
suggests a WR title from the description. Low confidence → `need_more_info`
so Civic prompts. Civic already ships the NACE-tree fallback for outage or
when this module is absent.

## Next work: CHR to ME on WR verification

**Not implemented.** Civic persists `reward_kind` (`merits`|`chrons`) on create.
When a work report is approved by two verifiers, Chrons are credited as today.
If `reward_kind=merits`, convert that Chron grant to Merits (A6, 10 CHR ↔ 10
ME) for the author.

## Next work: geofield on Civic work reports

**Not implemented.** Accept `field_location` point/line GeoJSON on Civic create
WR for a future map widget / live GPS track. Out of the deferred-form pass.

## Related Civic docs

- Plan: `~/Documents/homeProjects/civic/docs/plan.md` (§6 claim SoT, §7 split claims, §8b claim rule, §15 phases)
- NeoFund: `~/Documents/Business/NeoFund_NP/` (State Treasury, Path-1 donor Merit, A12)
- Civic API/Expo: `~/Documents/homeProjects/civic/docs/rovas-integration.md`
- Mockup (home UX): `~/Documents/homeProjects/civic/mocs/civic-mockup.canvas.tsx`
