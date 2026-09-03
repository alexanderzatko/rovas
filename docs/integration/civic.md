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
[Next work: Chron balance on /rovas/me](#next-work-chron-balance-on-rovasme),
[Next work: NACE expertise on users_lookup](#next-work-nace-expertise-on-users_lookup),
[Next work: localized project and work-report text](#next-work-localized-project-and-work-report-text),
[Next work: project detail + work reports (Civic modal)](#next-work-project-detail--work-reports-civic-modal),
[Next work: project shareholders + WR photos](#next-work-project-shareholders--wr-photos),
[Next work: private projects are fully readable](#next-work-private-projects-are-fully-readable),
[Next work: merit history (profile ledger)](#next-work-merit-history-profile-ledger),
[Next work: merit index (Civic Prínos / ranks screen)](#next-work-merit-index-civic-prínos--ranks-screen),
[Next work: merit index Prínos filter fixes](#next-work-merit-index-prínos-filter-fixes),
[Next work: by_source on merit index](#next-work-by_source-on-merit-index),
[Next work: merit index filter latency](#next-work-merit-index-filter-latency),
[Next work: volunteer report (Civic deferred form)](#next-work-volunteer-report-civic-deferred-form),
[Next work: pending verifications gate (assigned slots only)](#next-work-pending-verifications-gate-assigned-slots-only),
[Next work: Civic WR verification (in-app)](#next-work-civic-wr-verification-in-app),
[AI NACE + title from work-report text](#ai-nace--title-from-work-report-text),
[CHR to ME on WR verification](#chr-to-me-on-wr-verification),
[Next work: geofield on Civic work reports](#next-work-geofield-on-civic-work-reports),
and [Next work: OAuth session persistence](#next-work-oauth-session-persistence).
Do not rely on reading the Civic tree unless you need Expo/API details.

**Dev target (until told otherwise):** all Civic integration work targets
`**/var/www/rovdev**` (`https://dev.rovas.app`). Do **not** change prod
`/var/www/rovas` (`rovas.app`) unless explicitly asked.

**Deployment:** the human deploys unless they ask the agent to. Agents should
leave code + docs ready for `ops/deploy_rovdev.sh` (or a targeted rsync) and
not run deploys on their own. Full enable checklist for a production clone
(modules, OAuth/Services, Rules migration, Merit backfill/reconcile, Civic
env): [`civic-deploy.md`](civic-deploy.md).

## Status


| Piece                                | Location                                                                                                                                   | Notes                                                                                                                                                                                  |
| ------------------------------------ | ------------------------------------------------------------------------------------------------------------------------------------------ | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `oauth2_server` 7.x-1.7              | `sites/all/modules/contrib/oauth2_server`                                                                                                  | + PKCE patch [#3015065-7](https://www.drupal.org/project/oauth2_server/issues/3015065) in `patches/oauth2_server-3015065-pkce-support-7.patch`                                         |
| `xautoload` 7.x-5.9                  | `sites/all/modules/contrib/xautoload`                                                                                                      | Required by oauth2_server                                                                                                                                                              |
| `oauth2-server-php` **v1.14.2**      | `sites/all/libraries/oauth2-server-php`                                                                                                    | Module README lists ≤1.8; **1.14.2 required for PKCE**                                                                                                                                 |
| `rovas_oauth`                        | `sites/all/modules/custom/rovas_oauth`                                                                                                     | Bootstrap Civic server/client, login copy, Bearer helpers, `/me` + `/contributions/aggregate`                                                                                          |
| Expo login (PKCE)                    | Civic mobile                                                                                                                               | **Done** against `dev.rovas.app`                                                                                                                                                       |
| Civic API Bearer introspect          | Civic `GET /oauth2/tokens/{token}`                                                                                                         | **Done** — resolves Drupal `user_id` (uid)                                                                                                                                             |
| Services OAuth2 on `rovas_endpoint`  | Admin / `rovas_oauth_ensure_civic_resources()`                                                                                             | Server machine name `**civic**`; API-key auth disabled on Civic resources                                                                                                              |
| Home Merits reads                    | `/rovas/me`, `/rovas/contributions/aggregate`, `/rovas/merit_distribution`                                                                 | Implemented; `/rovas/me` includes `total_chr` (Chron wallet)                                                                                                                           |
| `rovas_civic`                        | `sites/all/modules/custom/rovas_civic`                                                                                                     | National rates, donor Merit issuance, State Treasury, VAT claim                                                                                                                        |
| Claim rule + split claims            | `POST /rovas/rules/rules_proxy_claim_vat_receipt`                                                                                          | Implemented in `rovas_civic` (enable via updb / admin)                                                                                                                                 |
| Claim reads + summary                | `GET /rovas/civic_claims`, `…/summary`                                                                                                     | Implemented in `rovas_civic` (`rovas_civic_update_7002`); **Civic proxies live** (`GET /receipts`, `/me/contributions/summary`)                                                        |
| Claim status by receipt              | `GET /rovas/civic_claims?receipt_uid=`                                                                                                     | Implemented — `{ claimed, receipt_uid }` only (no scanner)                                                                                                                             |
| Received history “From”              | `view=received` scanner username                                                                                                           | Implemented — `scanner_username` + `scanner_display_name`                                                                                                                              |
| National hourly rate read            | `GET /rovas/civic_national_hourly_rate`                                                                                                    | Implemented in `rovas_civic` (`rovas_civic_update_7003`)                                                                                                                               |
| Users search (split typeahead)       | `GET /rovas/users_search?query=`                                                                                                           | Implemented in `rovas_civic` (`rovas_civic_update_7005`). **Param is `query`, not `q`** — Drupal reserves `$_GET['q']` for the request path.                                           |
| Users lookup (community profile)     | `GET /rovas/users_lookup?username=`                                                                                                        | Implemented in `rovas_civic` (`rovas_civic_update_7006`). Exact username; Bearer required. Includes `nace_expertise[]` via `rovas_meritscope_user_expertise_json()`.                   |
| Hide negative Merit users            | `users_search` / `users_lookup`                                                                                                            | Implemented — `total_me < 0` omitted / 404; search response stays public fields only                                                                                                   |
| Friend invite (Civic)                | —                                                                                                                                          | **Civic-client only** (`mailto:`). No Rovas invite endpoint. Signup URL in the email body should work (`/user/register` or OAuth authorize).                                           |
| Project detail + WR list             | `GET /rovas/civic_project/{nid}`, `GET /rovas/civic_project_work_reports`                                                                  | Implemented (`rovas_civic_update_7007`). Private participation still returns full content — see [private projects are fully readable](#next-work-private-projects-are-fully-readable). |
| Project shareholders + WR photos     | `GET /rovas/civic_project_shareholders`, WR `photo_urls[]`                                                                                 | Implemented in `rovas_civic` (`rovas_civic_update_7008`).                                                                                                                              |
| Localized project / WR / lookup text | `?lang=` on `civic_project`, `civic_project_work_reports`, `users_lookup`                                                                  | Implemented — Entity Translation / `title_field` / `body` with source fallback.                                                                                                        |
| Merit history (profile ledger)       | `GET /rovas/civic_merit_history`                                                                                                           | Implemented in `rovas_civic` (`rovas_civic_update_7009`). JSON of Views `merit_history` / `page_1` for the Bearer user.                                                                |
| Volunteer report (deferred form)     | `GET /rovas/civic_my_projects`, `civic_projects_search`, `civic_nace_tree`; `POST /rovas/civic_create_project`, `civic_create_work_report` | Implemented in `rovas_civic` (`rovas_civic_update_7010`). Bearer only — do **not** call the partner API-KEY rules from Civic.                                                          |
| CHR→ME on WR verification            | `{civic_work_report_reward}` + verification hook                                                                                           | Implemented in `rovas_civic` (`rovas_civic_update_7011`). When `reward_kind=merits`, A6 converts the Chron grant after approval.                                                       |
| AI NACE + title                      | `POST /rovas/civic_ai_nace`                                                                                                                | Implemented in `rovas_ai` (`rovas_ai_update_7001`). Bearer; keep NACE-tree fallback when `unavailable` or module absent.                                                               |
| Work-report map proof                | `GET /rovas/civic_work_report_map_config`; `field_location` on create WR                                                                   | Implemented in `rovas_civic` (`rovas_civic_update_7012`). GeoJSON Point/LineString/Polygon; proof = URL **or** photos **or** map.                                                      |
| Civic WR verification (in-app)       | `GET /rovas/civic_pending_verifications`, `civic_work_report/{nid}`, `POST /rovas/civic_submit_verification`                               | Implemented in `rovas_civic` (`rovas_civic_update_7013`). Pending list + verifier detail + approve/reject/decline.                                                                     |
| Merit index (Civic Prínos / ranks)   | `GET /rovas/civic_merit_index`, `civic_merit_index_countries`                                                                             | Snapshot-backed (`7017`); cron rebuild every **12h** (configurable) + admin rebuild. Warm reads ≪1s; items include `by_category` + `by_source`.                                                                      |
| Chron balance on `/rovas/me`         | `GET /rovas/me` → `total_chr`                                                                                                              | Pending — see [Chron balance on /rovas/me](#next-work-chron-balance-on-rovasme).                                                                                                       |
| NACE expertise icons                 | `GET /rovas/users_lookup` → `nace_expertise[]`                                                                                             | Pending — see [NACE expertise on users_lookup](#next-work-nace-expertise-on-users_lookup).                                                                                             |


## OAuth endpoints (live on dev)

- Authorize: `https://dev.rovas.app/oauth2/authorize`
- Token: `https://dev.rovas.app/oauth2/token`
- Revoke: `https://dev.rovas.app/oauth2/revoke`
- Introspect: `https://dev.rovas.app/oauth2/tokens/{access_token}` → JSON with `user_id`, `client_id`, `expires`, `scope` (404 if invalid)

## Civic home-read APIs (live on dev after enable)


| Method | URL                                                   | Auth                                 |
| ------ | ----------------------------------------------------- | ------------------------------------ |
| `GET`  | `https://dev.rovas.app/rovas/me`                      | Bearer required (`user_profile`)     |
| `GET`  | `https://dev.rovas.app/rovas/contributions/aggregate` | Public                               |
| `GET`  | `https://dev.rovas.app/rovas/merit_distribution`      | Public; Bearer optional for `user_*` |


Enable / re-sync on rovdev: **Configuration → Web services → Rovas OAuth (Civic)** →
**Enable Civic home API resources**, or `drush74 updb` after deploy (`rovas_oauth_update_7001`).

That helper also ensures endpoint auth is `oauth2_server => array('server' => 'civic')`
(not a bare checkbox string) and disables API-key auth on those three resources.

## Civic OAuth client (bootstrap)

Admin UI: **Configuration → Web services → Rovas OAuth (Civic)**  
(`/admin/config/services/rovas-oauth`)

Click **Bootstrap Civic OAuth server + client**. That creates:


| Field               | Value                                                |
| ------------------- | ---------------------------------------------------- |
| Server machine name | `civic`                                              |
| Enforce PKCE        | yes                                                  |
| Grant types         | authorization_code, refresh_token                    |
| Client ID           | `civic`                                              |
| Client secret       | *(empty — public native client)*                     |
| Redirect URI        | `civic://oauth/callback` (+ optional Expo proxy URI) |
| Default scope       | `user_profile`                                       |


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
`[docs/dev-setup.md](../dev-setup.md#outgoing-email-disabled-on-dev)`.

### Services OAuth2 (rovas_endpoint)

- Admin: **Structure → Services → rovas_endpoint → Authentication**
- Enable **OAuth2 authentication**, server machine name `**civic`**
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


| Existing                                | Auth today                            | Problem for Civic                                         |
| --------------------------------------- | ------------------------------------- | --------------------------------------------------------- |
| `POST /rovas/rules/rules_user_s_merits` | User **API-KEY + TOKEN**              | Mobile has OAuth Bearer only, not the user’s API key pair |
| `GET …/rovas_merit_distribution`        | API-KEY + TOKEN; returns SVG + `meta` | Same auth issue; Civic needs **JSON buckets**, not SVG    |
| Community total                         | —                                     | Covered by `/rovas/contributions/aggregate`               |


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
  "total_chr": 320.0,
  "created": "2019-03-12T14:22:01+00:00",
  "language": "sk"
}
```


| Field          | Source                                                                                                                                        |
| -------------- | --------------------------------------------------------------------------------------------------------------------------------------------- |
| `uid`          | Bearer → Drupal uid                                                                                                                           |
| `username`     | `$user->name`                                                                                                                                 |
| `display_name` | `field_full_name` if set; else username                                                                                                       |
| `total_me`     | `_rovas_merit_distribution_user_merits()` (userpoints + decimal)                                                                              |
| `total_chr`    | `field_chrons_balance` on the user account (numeric; `0` when empty)                                                                          |
| `created`      | ISO-8601 from Drupal `$account->created` (`gmdate('c')`); `null` if missing/invalid                                                           |
| `language`     | `$account->language` (raw Drupal langcode); `null` if empty/`und` — see [preferred language](#next-work-expose-preferred-language-on-rovasme) |


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

When **no** Bearer: omit `user_`* fields. When Bearer present: fill them.

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
`**ME = (vat_slice_eur / national_hourly_rate_eur) × 10**`  
Rovas owns the math; Civic `total_merits` / `hourly_wage_eur` are preview/audit only.

### Endpoint


| Method | URL                                                               | Auth            |
| ------ | ----------------------------------------------------------------- | --------------- |
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


| Field                           | Notes                                                                                                                       |
| ------------------------------- | --------------------------------------------------------------------------------------------------------------------------- |
| `receipt_uid`                   | eKasa online UID (`O-` + 32 hex); global unique claim key                                                                   |
| `vat_eur`                       | Verified by Civic against Finančná správa; Rovas grant basis                                                                |
| `total_merits`                  | Civic preview only — **ignored for grant**                                                                                  |
| `merchant_name` / `purchase_at` | Audit metadata (nullable OK)                                                                                                |
| `hourly_wage_eur`               | Civic preview; grant uses admin **national** rate (SK)                                                                      |
| `shares`                        | ≥1; each entry is a **donor** to State Treasury for their VAT slice; `share_pct` sums to ~100; **scanner must be included** |


### Required logic

1. Resolve **scanner** from Bearer → Drupal uid
2. **Dedup** on `receipt_uid` → `result` 409 / `duplicate_receipt`
3. Validate `shares` (distinct uids, sum ≈ 100, users exist, scanner included)
4. Per share: `vat_slice` + `ME = (vat_slice / national_rate) × 10`; floor 2dp;
  **residue to scanner**
5. One Drupal transaction: for each share call donor Merit issuer
  (`purchase_receipt` + `merits_disbursement` + `rules_give_merits_to_user`
  - project ME bump + expertise + `{rovas_merit_issuance}`) targeting State
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


| Method | URL                                               | Auth                                       |
| ------ | ------------------------------------------------- | ------------------------------------------ |
| `GET`  | `https://dev.rovas.app/rovas/users_search?query=` | Bearer required (`civic` / `user_profile`) |


Do **not** register `/rovas/users/search` (slash in alias). Civic API calls
`/rovas/users_search?query=`.

**Critical:** the search string param must **not** be named `q`. Drupal uses
`$_GET['q']` as the internal request path; `?q=ann` replaces the Services
path with `ann` and returns a themed HTML 404 before Services runs (no
dblog Services entries). Use `query`.

### Query


| Param      | Rules                                                                       |
| ---------- | --------------------------------------------------------------------------- |
| `query`    | Required; trim; **≥ 3** characters; reject shorter with 400. **Never `q`.** |
| (implicit) | Max **10** results                                                          |


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


| Field          | Notes                                               |
| -------------- | --------------------------------------------------- |
| `rovas_uid`    | Drupal uid as string                                |
| `username`     | Rovas username                                      |
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


| Method | URL                                                  | Auth                                       |
| ------ | ---------------------------------------------------- | ------------------------------------------ |
| `GET`  | `https://dev.rovas.app/rovas/users_lookup?username=` | Bearer required (`civic` / `user_profile`) |


Do **not** register `/rovas/users/lookup` (slash in alias). Civic API calls
`/rovas/users_lookup?username=`.

### Query


| Param      | Rules                                                                                          |
| ---------- | ---------------------------------------------------------------------------------------------- |
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
  "by_source": {
    "tax_payment": 980.0,
    "charitable_donation": 210.0,
    "volunteer_work": 120.0,
    "volunteer": 28.0
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


| Field                    | Notes                                                                                                                                                                                                                                                                             |
| ------------------------ | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `rovas_uid`              | Drupal uid as string                                                                                                                                                                                                                                                              |
| `username`               | Canonical Rovas username                                                                                                                                                                                                                                                          |
| `display_name`           | Prefer `field_full_name`; fall back to `username`                                                                                                                                                                                                                                 |
| `total_me`               | Same source as `/rovas/me` for that uid (`_rovas_merit_distribution_user_merits` or fallback)                                                                                                                                                                                     |
| `by_category.taxes`      | Sum of ledger `field_merit_source = tax_payment` (positive ME only)                                                                                                                                                                                                                |
| `by_category.charity`    | Sum of ledger sources `charitable_donation` (EUR donations, EUR merit purchase, timeshare ME, project-reward ME to %-only shareholders)                                                                                                                                           |
| `by_category.volunteer`  | Sum of ledger sources `volunteer_work` + `volunteer` (work-report rewards, project-reward ME to earned/labor shareholders, CHR→ME purchase)                                                                                                                                       |
| `by_source`              | Full breakdown keyed by `field_merit_source` (`volunteer_work`, `volunteer`, `tax_payment`, `charitable_donation`, `penalty`, …); omits zero keys; negative `penalty` included when present. **Civic** remaps this into its displayed `by_category` with `penalty` folded into `volunteer` (Rovas `by_category` still excludes penalty). |
| `taxes_last_3_months`    | Last **3 calendar months** (including current) of VAT-claim Merits for this uid, keyed `YYYY-MM`; omit zero months or include with `me: 0` — prefer only months with activity; max 3 rows; `[]` if none                                                                           |
| `top_charities`          | Up to **3** `{ project_nid, name, me }` from `{rovas_merit_issuance}` rows with `source_type = project_reward_order` (Path-1 donor Merits from project donations). **Not** raw `purchase_receipt` Merit spends. Exclude State Treasury. Empty until real charity donations exist. |
| `top_volunteer_projects` | Up to **3** `{ project_nid, name, me }` from `project_merit_allocation` / `co_worker_merit_allocation` labor Merits (aggregated by project nid)                                                                                                                                   |


### Errors


| Status | When                                                                 |
| ------ | -------------------------------------------------------------------- |
| 401    | No / invalid Bearer                                                  |
| 400    | Empty / missing `username`                                           |
| 404    | Unknown username, blocked/inactive user, or not searchable (privacy) |


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


| Endpoint                            | Behaviour when `total_me < 0`                                                 |
| ----------------------------------- | ----------------------------------------------------------------------------- |
| `GET /rovas/users_search?query=`    | Omit the user from the result list (oversample so the 10-row cap still fills) |
| `GET /rovas/users_lookup?username=` | Treat as not found → **404** (same as unknown / not searchable)               |


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


| Method | URL                                                        | Auth                                         |
| ------ | ---------------------------------------------------------- | -------------------------------------------- |
| `GET`  | `https://dev.rovas.app/rovas/civic_claims?view=submitted`  | Bearer required                              |
| `GET`  | `https://dev.rovas.app/rovas/civic_claims?view=received`   | Bearer required                              |
| `GET`  | `https://dev.rovas.app/rovas/civic_claims?receipt_uid=O-…` | Bearer required — `{ claimed, receipt_uid }` |
| `GET`  | `https://dev.rovas.app/rovas/civic_claims/summary`         | Bearer required                              |


### Claim status (`?receipt_uid=`)

Global UID dedup preview for Civic verify/split UI. When `receipt_uid` is
non-empty, ignore `view` / `limit` / `offset` and return only:

```json
{ "claimed": true, "receipt_uid": "O-0123456789abcdef0123456789abcdef" }
```

Lookup is `{civic_claims}` by UNIQUE `receipt_uid`. No scanner / share data.
Implemented as `rovas_civic_claim_status_by_receipt_uid()`.

Civic calls this via `fetchClaimStatus` (optional Bearer → `already_claimed`
locks split UI + claim). Related: `users_search` uses param `**query**`, never
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

`**view=submitted**` — successful claims the caller scanned (not every QR
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

`**view=received**` — shares credited to the caller (may include claims
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

`**GET …/summary**` (received-based — matches Civic home “taxes”):

```json
{
  "receipt_count": 12,
  "total_vat_eur": 48.2,
  "taxes_me": 64.3
}
```


| Field           | Meaning                                          |
| --------------- | ------------------------------------------------ |
| `receipt_count` | Distinct claims where caller is a beneficiary    |
| `total_vat_eur` | Sum of caller’s `vat_slice`                      |
| `taxes_me`      | Sum of caller’s `merits_awarded` from VAT shares |


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


| Method | URL                                                                 | Auth                   |
| ------ | ------------------------------------------------------------------- | ---------------------- |
| `GET`  | `https://dev.rovas.app/rovas/civic_national_hourly_rate?country=SK` | **Public** (no Bearer) |


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


| Field             | Source                                  |
| ----------------- | --------------------------------------- |
| `country_code`    | Request / row PK                        |
| `hourly_wage_eur` | `{rovas_national_hourly_rate}.rate_eur` |
| `merits_per_hour` | `ROVAS_CIVIC_ME_PER_HOUR` (10)          |
| `source`          | Row `source` (may be empty string)      |
| `updated`         | Row `updated` as ISO-8601 (or `null`)   |


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


| Field     | Source                                             |
| --------- | -------------------------------------------------- |
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


| Rovas / device code | Civic UI                                 |
| ------------------- | ---------------------------------------- |
| `sk`                | `sk`                                     |
| `cs` / `cz`         | `sk` (until Civic ships a Czech catalog) |
| anything else       | `en`                                     |


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
  "total_chr": 320.0,
  "created": "2019-03-12T14:22:01+00:00",
  "language": "sk"
}
```


| Field      | Source                                                                 |
| ---------- | ---------------------------------------------------------------------- |
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

## Next work: Chron balance on /rovas/me

**Implemented in `rovas_oauth`.** Civic Profile screen shows the signed-in user's Chron wallet alongside Merits.
Chrons are stored on the Drupal user as `field_chrons_balance` (suffix `" CH"`).
Extend the existing Bearer-only home-read — **no new Services resource**.

### Change

In `rovas_oauth` → `rovas_oauth_me_index()`:

1. After loading `$account`, read `field_chrons_balance` via `field_get_items('user', $account, 'field_chrons_balance')`.
2. Emit `"total_chr"` as a float. Use `0` when the field is empty or missing.
3. Additive only — existing fields (`uid`, `username`, `display_name`, `total_me`, `created`, `language`) unchanged.

### Response (after)

```json
{
  "uid": 42,
  "username": "sano",
  "display_name": "Alexander Žaťko",
  "total_me": 1248.35,
  "total_chr": 320.0,
  "created": "2019-03-12T14:22:01+00:00",
  "language": "sk"
}
```


| Field       | Source                                                                               |
| ----------- | ------------------------------------------------------------------------------------ |
| `total_chr` | `field_chrons_balance` on the user account (numeric value only; omit `" CH"` suffix) |


### Checklist

1. [x] `rovas_oauth_me_index()` returns `total_chr` as number
2. [x] Missing / empty balance → `0`
3. [x] Existing clients still work (additive field)
4. [ ] Deploy to rovdev + `drush74 cc all`
5. [ ] Smoke-test curl below

### Smoke test (after deploy)

```bash
# TOKEN = Civic OAuth access token with user_profile scope
curl -sS https://dev.rovas.app/rovas/me \
  -H "Authorization: Bearer $TOKEN" \
  -H "Accept: application/json"
# Expect: "total_chr": <number> (0 when wallet empty)
```

### Civic will consume

Civic API `GET /me` proxies `total_chr`. Expo Profile screen shows `{total_chr} CHR`
below the Merit balance. Chron balance is **not** shown on Home in v1.

## Next work: NACE expertise on users_lookup

**Implemented in `rovas_meritscope` + `rovas_civic`.** Civic Home (signed-in) and Community (public profile) show clickable NACE section
icons beside the user's Merit total — matching the Rovas Merit Index row
(`rovas_meritscope_display_user_expertise()`). Data comes from `user_expertise`
nodes grouped by top-level NACE parent.

Extend `rovas_civic_users_lookup()` — **no new Services resource**.

### Path note

Same machine-name path as existing users lookup:


| Method | URL                                                        | Auth                                       |
| ------ | ---------------------------------------------------------- | ------------------------------------------ |
| `GET`  | `https://dev.rovas.app/rovas/users_lookup?username=&lang=` | Bearer required (`civic` / `user_profile`) |


### Response addition

Add `nace_expertise` array to the existing `users_lookup` JSON (after
`by_category`, before `taxes_last_3_months` is fine):

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
  "nace_expertise": [
    {
      "section_tid": 942,
      "section_label": "Construction",
      "section_letter": "f",
      "icon_url": "https://dev.rovas.app/sites/default/files/pictures/nace_icons/f.svg",
      "me": 420.0,
      "share_percent": 34,
      "hourly_rate_eur": 12.5
    }
  ],
  "taxes_last_3_months": [],
  "top_charities": [],
  "top_volunteer_projects": []
}
```


| Field             | Rules                                                                                                              |
| ----------------- | ------------------------------------------------------------------------------------------------------------------ |
| `section_tid`     | Top-level NACE taxonomy term id (parent section)                                                                   |
| `section_label`   | Localized parent name with `"X – "` / `"X - "` letter prefix stripped (meritscope uses `substr($parent_label, 2)`) |
| `section_letter`  | Lowercase first character of the **unlocalized** parent name — used for icon file name                             |
| `icon_url`        | Absolute URL: `{base_url}/sites/default/files/pictures/nace_icons/{section_letter}.svg`                            |
| `me`              | ME in that section (float, rounded to 2 dp)                                                                        |
| `share_percent`   | Integer % of total grouped ME across all sections                                                                  |
| `hourly_rate_eur` | Parent term `field_hourly_reward`, default **10** when unset (same as Merit Index popover)                         |


Return `"nace_expertise": []` when the user has no positive expertise rows (do
not 404). Omitting the field on older deploys is OK — Civic defaults to `[]`.

### Required logic

1. **Extract** a shared JSON builder from `rovas_meritscope_display_user_expertise()`
  (e.g. `rovas_meritscope_user_expertise_json($uid, $lang)` in
   `rovas_meritscope.module`). Refactor the HTML renderer to call it so web and
   Civic stay in sync.
2. **Fix expert filter:** the current meritscope SQL filters on `node.uid`;
  expertise nodes use `**field_expert**` (`field_expert_target_id`). The JSON
   builder must join/filter like `group_nace_merit_sum()` and
   `rovas_civic_users_lookup_expertise_categories()` — `field_expert_target_id = :uid`.
3. Load all `user_expertise` nodes with `field_merits > 0` for the user.
4. Group by **top-level NACE parent** (`taxonomy_get_parents_all`, same index
  logic as `rovas_meritscope_display_user_expertise()` step 3).
5. Sum ME per section; apply proportional scaling when userpoints total >
  summed expertise (meritscope step 4).
6. Sort descending by `me`.
7. Honor `?lang=` on `users_lookup` for localized `section_label`
  (`i18n_string("taxonomy:term:{tid}:name", …)`).
8. Wire into `rovas_civic_users_lookup()` before return.

### Checklist

1. [x] Shared `rovas_meritscope_user_expertise_json($uid, $lang)` returns array of section objects
2. [x] HTML merit index row refactored to use the shared builder (no drift)
3. [x] Expert filter uses `field_expert`, not `node.uid`
4. [x] `users_lookup` includes `nace_expertise` (empty array when none)
5. [x] `icon_url` is absolute (uses `$GLOBALS['base_url']` or equivalent)
6. [ ] Deploy to rovdev + `drush74 cc all`
7. [ ] Smoke-test curl below

### Smoke test (after deploy)

```bash
# TOKEN = Civic OAuth access token; USERNAME = user with known user_expertise rows
curl -sS "https://dev.rovas.app/rovas/users_lookup?username=${USERNAME}&lang=sk" \
  -H "Authorization: Bearer $TOKEN" \
  -H "Accept: application/json"
# Expect: "nace_expertise": [ { "section_tid", "section_label", "icon_url", "me", ... } ]
# User with no expertise: "nace_expertise": []
```

### Civic will consume

Civic API `GET /users/lookup` proxies `nace_expertise`. Expo shows icon row on
Home (signed-in, via self lookup) and Community profile hero. Icons load from
`icon_url` returned by Rovas (same SVG assets as Merit Index web UI).

**Out of scope:** Meritscope circular chart, AJAX drill-down, bundling icons
into the Expo app.

## Next work: localized project and work-report text

**Implemented in `rovas_civic`.** Civic UI chrome is already `sk`/`en`, and the
Expo client + Civic API send `?lang=sk|en` on project/WR/lookup requests.
Rovas honors `lang` so Entity Translation / `title_field` / `body` values
match the Civic language when a translation exists.

### Civic → Rovas contract (already live on Civic side)


| Civic API                               | Forwards to Rovas                                          |
| --------------------------------------- | ---------------------------------------------------------- |
| `GET /projects/:nid?lang=`              | `GET /rovas/civic_project/{nid}?lang=`                     |
| `GET /projects/:nid/work-reports?lang=` | `GET /rovas/civic_project_work_reports?project_nid=&lang=` |
| `GET /users/lookup?username=&lang=`     | `GET /rovas/users_lookup?username=&lang=`                  |



| Param  | Rules                                                                                                                                |
| ------ | ------------------------------------------------------------------------------------------------------------------------------------ |
| `lang` | Optional. Civic sends `sk` or `en` only. Treat `cs`/`cz` as `sk` if ever received. Missing → keep current behavior (source/default). |


Shareholders endpoint stays language-agnostic (no translated copy).

### Fields that must respect `lang`


| Endpoint                     | Fields                                                                |
| ---------------------------- | --------------------------------------------------------------------- |
| `civic_project`              | `title`, `summary`; embedded `work_reports.items[].title`, `.summary` |
| `civic_project_work_reports` | `items[].title`, `items[].summary`                                    |
| `users_lookup`               | `top_charities[].name`, `top_volunteer_projects[].name`               |


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


| Method | URL                                                                                  | Notes                                        |
| ------ | ------------------------------------------------------------------------------------ | -------------------------------------------- |
| `GET`  | `https://dev.rovas.app/rovas/civic_project/{nid}`                                    | Project summary + first page of work reports |
| `GET`  | `https://dev.rovas.app/rovas/civic_project_work_reports?project_nid=&limit=&offset=` | Paginated work reports for a project         |


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


| Field                                      | Source / rules                                                                                                                                                                                                                                                                                |
| ------------------------------------------ | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `summary`                                  | `strip_tags` + decode entities on `body`; max ~800 chars; `null` if empty. Always include for published projects (do **not** omit for Private participation).                                                                                                                                 |
| `is_public`                                | Informational only: `field_participation` ∈ {Public (1), Public-dependent (2)}. **Private (0) is still fully readable** — it only means the owner controls who may join as a shareholder (not “hide from Civic”). Full summary, WR list, photos, and shareholders for all published projects. |
| `total_hours`                              | `field_project_total_hours` (float or null)                                                                                                                                                                                                                                                   |
| `total_me`                                 | `field_merit_reward` (float or null)                                                                                                                                                                                                                                                          |
| `work_report_count` / `work_reports.total` | Count of published `work_report` nodes with `field_project` → this nid                                                                                                                                                                                                                        |
| `created_at` (WR)                          | Prefer `field_date` unix; else node `created` → ISO-8601                                                                                                                                                                                                                                      |
| `hours` (WR)                               | `field_hours_worked`                                                                                                                                                                                                                                                                          |
| `summary` (WR)                             | Same plain-text teaser rules as project body                                                                                                                                                                                                                                                  |


**404** — unknown nid, wrong type, or unpublished (`status != 1`).

Default first WR page: `limit=20` (max 50), `offset=0`.

### `GET /rovas/civic_project_work_reports`


| Param         | Rules                 |
| ------------- | --------------------- |
| `project_nid` | Required positive int |
| `limit`       | Default 20, max 50    |
| `offset`      | Default 0             |


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


| Field        | Rules                                                                                                                                                                                                                                                                                   |
| ------------ | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `photo_urls` | Absolute HTTPS URLs from `field_photos` via `file_create_url` (or equivalent). Empty array when none. Include for Private participation projects too. Omit broken/missing files. Prefer image styles only if a stable public style already exists; otherwise original file URL is fine. |


### Endpoint: shareholders


| Method | URL                                                                                  | Notes                        |
| ------ | ------------------------------------------------------------------------------------ | ---------------------------- |
| `GET`  | `https://dev.rovas.app/rovas/civic_project_shareholders?project_nid=&limit=&offset=` | Paginated; machine-name path |


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


| Field           | Source / rules (match Rovas shareholder view logic)                                                                                                                                                 |
| --------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `username`      | Rovas **username** (not full name). From coworker user for worker shares; skip pure project-as-shareholder rows **or** omit them from Civic (Civic only needs user usernames linkable to Komunita). |
| `share_kind`    | `"percent"` when `field_merit_allocation` > 0; else `"time_based"` (Rovas view prints “work time share”).                                                                                           |
| `share_percent` | `field_merit_allocation` when percent; else `null`.                                                                                                                                                 |
| `hours`         | `field_total_hours_worked` (float or null) — especially meaningful for time-based shares.                                                                                                           |


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
`/user/<uid>/merits` — Views `**merit_history**`, display `**page_1**`.

Do **not** scrape HTML or call `views_get_view()` / `views_embed_view()`. Add a
JSON Services resource in `rovas_civic` that **mirrors that view with SQL**
(same pattern as `civic_project_shareholders` vs `project_coworkers`).

**Auth:** Bearer required (`civic` / `user_profile`). History is **only for the
caller** (`field_reward_recipient` = Bearer uid). Ignore any `uid` / `username`
query param (do not expose another user’s ledger).

### What the view shows (parity)


| Views field                      | Role                                 | Civic JSON                            |
| -------------------------------- | ------------------------------------ | ------------------------------------- |
| `nid` (Receipt Id)               | Disbursement / penalty node          | `disbursement_nid`                    |
| `created` (Date)                 | Node created                         | `created_at` ISO-8601                 |
| PHP **Source**                   | See rules below                      | `source_kind` + `source_title` + nids |
| `field_reward_amount` (Reward)   | ME credited (can be negative)        | `merits`                              |
| `field_receipt_project` (hidden) | Project on parent `purchase_receipt` | `project_nid`                         |
| `field_rewarded_report` (hidden) | Work report on parent receipt        | `work_report_nid`                     |


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


| Method | URL                                               | Auth            |
| ------ | ------------------------------------------------- | --------------- |
| `GET`  | `https://dev.rovas.app/rovas/civic_merit_history` | Bearer required |


Machine-name path only (no slash alias).


| Param    | Rules                                                                                                                                    |
| -------- | ---------------------------------------------------------------------------------------------------------------------------------------- |
| `limit`  | Default **20**, max **50**                                                                                                               |
| `offset` | Default 0                                                                                                                                |
| `lang`   | Optional `sk` | `en` — same Entity Translation / `title_field` rules as `civic_project` (source fallback). Used for `source_title` only. |


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


| Field                             | Rules                                                          |
| --------------------------------- | -------------------------------------------------------------- |
| `disbursement_nid`                | String nid of the `merits_disbursement` / `merit_penalty` node |
| `node_type`                       | `merits_disbursement` or `merit_penalty`                       |
| `created_at`                      | Node `created` → ISO-8601 (`rovas_civic_format_created`)       |
| `merits`                          | `field_reward_amount` float (negative allowed for penalties)   |
| `merit_source`                    | `field_merit_source` on the ledger node (`volunteer_work`, `volunteer`, `tax_payment`, `charitable_donation`, `penalty`, …) or `null` when unset (legacy rows) |
| `source_kind`                     | `project` | `report` | `purchase` only                         |
| `source_title`                    | Localized entity title, or `null` for `purchase`               |
| `project_nid` / `work_report_nid` | String nids or `null`                                          |


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


| Method | URL                                         | Returns                                                                                                                                                                                                                                                                                     |
| ------ | ------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `GET`  | `/rovas/civic_my_projects?lang=`            | `{ project_nid, title, is_public, can_report }[]` — up to **3** published projects where the Bearer user authored work reports, ordered by most recent report (`work_report` node `created` DESC). Empty when the user has never reported. `can_report`: owner, shareholder, or public join |
| `GET`  | `/rovas/civic_projects_search?query=&lang=` | Same row shape. `query` ≥ 3 chars, cap 10, published `project` nodes, title LIKE. `can_report` reflects whether the Bearer user may submit (not merely `is_public`)                                                                                                                         |
| `GET`  | `/rovas/civic_nace_tree?lang=`              | Nested `{ tid, code, label, children[] }` from vocabulary `tags`                                                                                                                                                                                                                            |


### Writes


| Method | URL                               | Body / result                                                                                                                                                                                                    |
| ------ | --------------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `POST` | `/rovas/civic_create_project`     | `{ name, description, url? }` → `{ project_nid }`. Defaults: `project_status=1` (for sale), `project_participation=1` (Public), not a system project. Creator becomes shareholder via existing node-insert rules |
| `POST` | `/rovas/civic_create_work_report` | See below                                                                                                                                                                                                        |
| `POST` | `/rovas/civic_ai_nace`            | See [AI NACE + title](#ai-nace--title-from-work-report-text) — classify from description (Bearer)                                                                                                                |


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


| Field            | Rules                                                                                                                                   |
| ---------------- | --------------------------------------------------------------------------------------------------------------------------------------- |
| `project_nid`    | Required published project                                                                                                              |
| `date_started`   | Unix timestamp; reject if strictly before the author's Rovas `created`                                                                  |
| `hours`          | Required, `> 0`                                                                                                                         |
| `description`    | Required non-empty                                                                                                                      |
| `title`          | Optional; empty → truncated description                                                                                                 |
| `nace_tid`       | Required taxonomy tid in vocabulary `tags`                                                                                              |
| `web_url`        | Optional proof URL                                                                                                                      |
| `photos`         | Optional array of `{ filename, mime, data }` (base64). Max 5, 4 MB decoded each                                                         |
| `field_location` | Optional GeoJSON Geometry (`Point`, `LineString`, `Polygon`); lon/lat order; persisted to `field_location` + map work-evidence bit      |
| `reward_kind`    | `merits` or `chrons` (default `chrons`). Persisted in `{civic_work_report_reward}`; when `merits`, converted to ME on verification (A6) |


At least one of `web_url`, `photos[]`, or `field_location` is required. Photos attach to
`field_photos` after the WR node exists (partner create-WR required a URL;
Civic must accept photos without a URL).

Before create: `rules_check_or_add_shareholder_core`. Private + not already a
shareholder → **403** `{ "error": "not_shareholder" }`.

Typed errors (HTTP + `{ error, message }`):


| `error`                    | HTTP | Meaning                                                                    |
| -------------------------- | ---- | -------------------------------------------------------------------------- |
| `not_shareholder`          | 403  | Private project; caller cannot self-join                                   |
| `date_before_registration` | 400  | `date_started` before account created                                      |
| `pending_verifications`    | 403  | Caller has at least one verification slot with a report assigned to verify |
| `invalid_input`            | 400  | Missing/invalid fields, no proof, hours ≤ 0                                |
| `project_not_found`        | 404  | Unknown / unpublished project                                              |


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
slots). The create gate fires when that count is `**> 1**`.

**Required behaviour:** only count slots that have a work report **assigned**
(`field_report_to_verify_target_id > 0`). Empty obligation slots are not
actionable and must **not** block new filings. Block when the assigned count is
`**> 0**` (any pending verification obligation with a report to act on).

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
2. [x] Exclude declined slots from `rovas_civic_pending_verify_slot_count()` (gate matches pending list)
3. [ ] Deploy to **rovdev** only (`/var/www/rovdev`)
4. [ ] Smoke: uid 289 with empty slots only — `POST /rovas/civic_create_work_report` succeeds
5. [ ] Smoke: manually assign a WR to one of the caller's slots (or wait for
  `rules_find_verifier`) — same POST returns 403 `pending_verifications`
6. [ ] Confirm `pending_verifications` view rows match what blocks create

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

## AI NACE + title from work-report text

**Implemented** in `sites/all/modules/custom/rovas_ai`. Classifies NACE and
suggests a work-report title from a free-text description. Low confidence →
`need_more_info` so Civic can prompt. Civic must keep the `civic_nace_tree`
manual picker as fallback when the module is absent, not configured, over
budget, or returns `unavailable`.

Internal PHP API (same module): `rovas_ai_classify_work($description, $lang, $options)`.

### Enable on rovdev

```bash
cd /var/www/rovdev
drush74 en rovas_ai -y
drush74 updb -y
drush74 cc all -y
```

Admin: **Configuration → Rovas → AI** (`/admin/config/rovas/ai`) — set API key,
optional base URL (OpenAI-compatible), model (default `gpt-4o-mini`), confidence
threshold, daily per-user call budget. Click **Enable Civic AI APIs** (or rely
on `rovas_ai_update_7001` / `hook_enable`).

### Contract


| Method | URL                    | Auth                                           |
| ------ | ---------------------- | ---------------------------------------------- |
| `POST` | `/rovas/civic_ai_nace` | Bearer (`user_profile`), API-key auth disabled |


**Request body:**

```json
{
  "description": "Cleared invasive plants along the trail.",
  "lang": "sk",
  "project_nid": "1998"
}
```


| Field                                   | Rules                                                                                                                                                          |
| --------------------------------------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `description`                           | Required for a useful result; empty or a bare one-word label (e.g. `Buildings`) → `need_more_info`                                                             |
| `lang`                                  | Optional (`sk`|`en`; `cs`/`cz` → `sk`). Clarifying questions + title language                                                                                  |
| `project_nid`                           | Optional published project. Rovas loads localized title + body and sends them to the LLM as background context (classify the work done, not the whole project) |
| `project_title` / `project_description` | Optional plain-text overrides or client-side context when `project_nid` is omitted                                                                             |


**Response** — HTTP **200** with a `status` field (except **401** when unauthenticated). Civic should branch on `status`, not only on HTTP code:

```json
{
  "status": "ok",
  "nace_tid": "860",
  "nace_code": "43.21",
  "nace_label": "…",
  "title": "Trail clearing",
  "clarifying_question": "",
  "confidence": 0.82,
  "error": ""
}
```


| `status`         | Meaning                                           | Civic behaviour                                      |
| ---------------- | ------------------------------------------------- | ---------------------------------------------------- |
| `ok`             | NACE tid + title ready                            | Prefill form; user can still edit                    |
| `need_more_info` | Ambiguous / low confidence                        | Show `clarifying_question`; keep NACE tree available |
| `unavailable`    | Not configured, LLM error, budget, empty taxonomy | Fall back to NACE tree; optional soft message        |
| `error`          | Unexpected                                        | Fall back to NACE tree                               |


`need_more_info` example:

```json
{
  "status": "need_more_info",
  "nace_tid": "",
  "nace_code": "",
  "nace_label": "",
  "title": "",
  "clarifying_question": "Can you clarify the industry or type of activity?",
  "confidence": 0.4,
  "error": ""
}
```

### Cost / behaviour notes (Rovas-side)

- Provider: pluggable; v1 ships **OpenAI-compatible** (`drupal_http_request`, JSON mode).
- Two-pass classification: top-level NACE section(s) → leaves under that section only (never the full tree in one prompt). When pass 1 returns two section candidates with similar confidence (tie margin, default 0.20), their leaf lists are merged into one catalog and a single leaf pass compares them directly (e.g. Education P vs Recreation R for teaching sports) — two isolated pass-2 calls are not used because LLM confidence isn't comparable across separate, unrelated prompts. Typical latency is ~8–15s (two sequential OpenRouter round-trips). If the model returns a code missing from the pass-2 catalog but present as a real leaf in the taxonomy, Rovas still accepts it.
- Both the section catalog (pass 1) and leaf catalog (pass 2) append short disambiguation notes for a curated list of codes/sections the model tends to confuse — bare NACE labels are terse and don't say what's excluded (e.g. section `E` water/waste/remediation vs section `N` cleaning/business-support services; `81.29` "Other cleaning activities" explicitly covers routine snow/ice/street cleaning, which the model otherwise sometimes misfiles under `39.00` remediation/hazardous-waste, or under `84.12` public-administration because the project text mentioned "municipal"/"public"). Section-level hints matter because if pass 1 excludes the right section, its leaves (however well-hinted) never reach pass 2. See `rovas_ai_nace_disambiguation_hints()` in `rovas_ai.nace.inc`; extend it as new confusions are observed instead of switching to a slower/pricier reasoning model.
- Reasoning models are intentionally **not** used: for OpenRouter requests the provider sets `reasoning.effort=none` / `exclude=true` on every call (reasoning tokens ate the completion budget and often returned empty `content`). A reasoning model would add real latency/cost on top of the existing two-pass flow without fixing label-ambiguity misses like the one above — targeted hints are the cheaper, more reliable fix for those.
- Every classify run logs to `watchdog('rovas_ai', …, WATCHDOG_NOTICE)`: pass-1 section candidates with confidences, and (on success) the final leaf code/label/confidence plus which section(s) were searched. Check **Reports → Recent log messages** filtered to type `rovas_ai` when a result looks wrong — it shows whether pass 1 excluded the right section before pass 2 ever ran.
- Optional **project context** (`project_nid` or title/description) is appended to both passes so purpose/setting from the parent project can disambiguate short work texts. Platform/reward boilerplate is stripped, but mapping/GIS domain sentences (OpenStreetMap, JOSM, iD, changesets, …) are kept even when they also mention Rovas rewards. The model must classify the **activity performed**, not municipal/public ownership (avoids false O/84.x hits). Bare noun labels without an action still return `need_more_info` — project domain alone must not invent construction or map-editing. It is also instructed not to ask clarifying questions already answered by the work text or project setting.
- Per-user daily budget (default 50 classify calls); over budget → `unavailable`.
- Unknown model codes are never trusted as tids — must resolve against vocabulary `tags`.

### Smoke test

```bash
# After OAuth token + API key configured:
curl -sS -X POST "https://dev.rovas.app/rovas/civic_ai_nace" \
  -H "Authorization: Bearer $TOKEN" \
  -H "Content-Type: application/json" \
  -d '{"description":"Repaired a wooden bridge railing","lang":"en","project_nid":"1998"}'
```

## CHR to ME on WR verification

**Implemented in `rovas_civic`.** Civic persists `reward_kind` (`merits`|`chrons`) on create
in `{civic_work_report_reward}`. When a work report is approved by two verifiers,
Chrons are credited as today (`rules_verify_work_report`). If `reward_kind=merits`,
`rovas_civic_node_insert()` (verification bundle, module weight **21** — after Rules)
calls `rovas_civic_maybe_convert_wr_chron_to_merits()` which runs the existing forex
**A6** path (`_rovas_forex_grant_merits` / `_convert_chr_2_merits`, 10 CHR ↔ 10 ME)
for the author. Idempotent via `{civic_work_report_reward}.converted`.

### Code


| File                     | Role                                                                   |
| ------------------------ | ---------------------------------------------------------------------- |
| `rovas_civic.reward.inc` | `rovas_civic_maybe_convert_wr_chron_to_merits()`, grant amount helpers |
| `rovas_civic.module`     | `hook_node_insert()` on `verification`                                 |
| `rovas_civic.install`    | `converted` column; `rovas_civic_update_7011`; module weight 21        |


Chron amount: `field_chrons_to_grant` when set, else `field_hours_worked × 10`.
Requires `rovas_forex` helpers at runtime (`function_exists` guard + watchdog if missing).
Rovas usage fee (AUR) still debits CHR before conversion, unchanged.

### Checklist

1. [x] `converted` column on `{civic_work_report_reward}`
2. [x] Post-verification hook + A6 conversion in `rovas_civic.reward.inc`
3. [ ] Deploy to **rovdev** only (`/var/www/rovdev`); `drush74 updb -y`
4. [ ] Smoke: Civic WR with `reward_kind=merits` → verify → author ME +CHR ledger as A6
5. [ ] Smoke: `reward_kind=chrons` (default) → verify → CHR only, `converted` stays 0

### Smoke test (after deploy)

```bash
# TOKEN = Civic OAuth access token
# 1) Create WR electing Merits
curl -sS -X POST "https://dev.rovas.app/rovas/civic_create_work_report" \
  -H "Authorization: Bearer $TOKEN" -H "Content-Type: application/json" \
  -d '{
    "project_nid": "YOUR_PROJECT_NID",
    "date_started": 1750000000,
    "hours": 1,
    "description": "CHR→ME verification test",
    "nace_tid": "860",
    "web_url": "https://example.org/proof",
    "reward_kind": "merits"
  }'
# Note work_report_nid from response

# 2) After two verifiers approve (rules_verify_work_report):
# drush sqlq "SELECT wr_nid, reward_kind, converted FROM civic_work_report_reward WHERE wr_nid = WR_NID"
# Expect converted > 0 when reward_kind = merits

# 3) Author balances — ME increased by hours×10; CHR net −Rovas fee only
curl -sS "https://dev.rovas.app/rovas/me" -H "Authorization: Bearer $AUTHOR_TOKEN"
curl -sS "https://dev.rovas.app/rovas/civic_merit_history?limit=5" \
  -H "Authorization: Bearer $AUTHOR_TOKEN"
```

## Next work: geofield on Civic work reports

**Implemented in `rovas_civic`** (enable on rovdev: `drush74 updb -y`, or admin
enable APIs). Civic screen 4 (Dobrovoľnícka práca) adds an expandable **map proof** section:
users draw a **Point**, **LineString**, or **Polygon** on a basemap (same proof
type as Rovas `field_location` / `field_work_evidence` map checkbox on the
Drupal work-report form). Civic proxies map layer definitions from Rovas and
submits GeoJSON on create.

**Civic client:** implemented against this contract; uses a local OSM fallback
when the map-config endpoint is not yet deployed.

### Read: map widget config


| Method | URL                                   | Auth   |
| ------ | ------------------------------------- | ------ |
| `GET`  | `/rovas/civic_work_report_map_config` | Bearer |


Export the **same** raster layers, default view, and allowed geometry types as
the OpenLayers preset used by the `work_report` `field_location` geofield widget
(on rovdev this is typically the `**freemap**` map export — basemap tiles Civic
should render). Do **not** ship raw Drupal OpenLayers PHP objects; return a small
JSON DTO Civic can feed to any map SDK.

**Response shape:**

```json
{
  "center": { "latitude": 48.7164, "longitude": 19.6611 },
  "zoom": 8,
  "geometry_types": ["Point", "LineString", "Polygon"],
  "raster_layers": [
    {
      "id": "freemap",
      "name": "Freemap",
      "url_template": "https://tile.freemap.sk/T/{z}/{x}/{y}.jpeg",
      "attribution": "© Freemap Slovakia",
      "max_zoom": 18
    }
  ],
  "default_layer_id": "freemap"
}
```


| Field              | Rules                                                           |
| ------------------ | --------------------------------------------------------------- |
| `center`           | Default map centre (WGS-84 degrees)                             |
| `zoom`             | Initial zoom (WebMercator / XYZ tile zoom)                      |
| `geometry_types`   | Subset of `Point`, `LineString`, `Polygon` the widget allows    |
| `raster_layers[]`  | At least one XYZ template with `{z}`, `{x}`, `{y}` placeholders |
| `default_layer_id` | Must match one `raster_layers[].id`                             |


### Write: `field_location` on create work report

Extend `POST /rovas/civic_create_work_report`:

```json
{
  "project_nid": "1998",
  "date_started": 1750000000,
  "hours": 4,
  "description": "Marked trail maintenance along the ridge.",
  "nace_tid": "860",
  "field_location": {
    "type": "LineString",
    "coordinates": [
      [19.6611, 48.7164],
      [19.6700, 48.7200]
    ]
  }
}
```


| Field            | Rules                                                                                                                                                                                                                                                                                                                                  |
| ---------------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `field_location` | Optional GeoJSON **Geometry** object (`Point`, `LineString`, or `Polygon`). Coordinates **WGS-84**, order `**[longitude, latitude]`** per RFC 7946. Persist to `field_location` on the created `work_report` node (same as the Drupal map widget WKT). Reject self-intersecting polygons / invalid rings with **400** `invalid_input`. |


**Proof rule (update):** at least one of `web_url`, `photos[]`, or
`field_location` is required (same as Drupal: any one evidence type suffices).

After `rules_create_work_report` succeeds (same as photos), set geofield on the
WR node when `field_location` is present. Set `field_work_evidence` map bit so
verification UI shows the map proof.

### Checklist

1. [x] `GET /rovas/civic_work_report_map_config` — Bearer; JSON per above; layers from `freemap` (or active WR widget preset)
2. [x] `POST /rovas/civic_create_work_report` — accept optional `field_location` GeoJSON; update proof rule (URL **or** photos **or** map)
3. [x] Persist geometry to `field_location`; set work-evidence map flag
4. [x] `rovas_civic_ensure_services_resources()` + `rovas_civic_update_7012`
5. [ ] Smoke test below after deploy

### Smoke test (after deploy)

```bash
# TOKEN = Civic OAuth access token
curl -sS "https://dev.rovas.app/rovas/civic_work_report_map_config" \
  -H "Authorization: Bearer $TOKEN" -H "Accept: application/json"

curl -sS -X POST "https://dev.rovas.app/rovas/civic_create_work_report" \
  -H "Authorization: Bearer $TOKEN" -H "Content-Type: application/json" \
  -d '{
    "project_nid": "PROJECT_NID",
    "date_started": 1750000000,
    "hours": 1,
    "description": "Map proof smoke test",
    "nace_tid": "860",
    "field_location": {
      "type": "Point",
      "coordinates": [19.6611, 48.7164]
    }
  }'
# Expect work_report_nid; WR node field_location populated
```

### Civic will consume

- `GET /work-reports/map-config` → proxy above (Civic API fallback until live)
- `POST /work-reports` with optional `field_location` geometry

## Next work: Civic WR verification (in-app)

**Implemented in `rovas_civic`** (enable on rovdev: `drush74 updb -y`, or admin
enable APIs). Civic surfaces **in-app notifications** (not a dedicated list screen) when the
Bearer user has assigned `verification_slot` nodes to clear. Each notification
links to a per-report verify screen in Civic. Civic API proxies three new
Rovas Services resources; **implement on rovdev before Civic mobile can ship
verifier UX.**

**Prerequisite:** [pending verifications gate](#next-work-pending-verifications-gate-assigned-slots-only)
deployed and smoke-tested — gate count must match pending list `total`.

### 1. `GET /rovas/civic_pending_verifications?lang=`

Bearer required. Minimal rows for notification copy and deep links — **not** a
full report payload.

**Response shape:**

```json
{
  "total": 1,
  "items": [{
    "slot_nid": "527175",
    "work_report_nid": "527174",
    "title": "Trail maintenance",
    "author_username": "jane",
    "author_display_name": "Jane Doe",
    "project_nid": "1998",
    "assigned_at": "2026-08-30T10:00:00Z"
  }]
}
```


| Field     | Rules                                                                                                                                                                    |
| --------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| Predicate | Same as `rovas_civic_pending_verify_slot_count()` — published `verification_slot`, `field_slip_owner` = Bearer uid, `field_report_to_verify_target_id > 0`, not declined |
| `total`   | Must equal count that blocks `POST /rovas/civic_create_work_report` (403 `pending_verifications`)                                                                        |
| `lang`    | Optional `sk|en`; translate WR `title` like `civic_project_work_reports`                                                                                                 |


### 2. `GET /rovas/civic_work_report/{nid}?lang=`

Bearer required. **Reuse** the item builder from
`rovas_civic_project_work_reports_fetch()` (title, summary, hours, author,
`created_at`, `photo_urls`) and extend with verifier fields:


| Additional field              | Notes                                                                  |
| ----------------------------- | ---------------------------------------------------------------------- |
| `project_nid`, `project_name` | From `field_project`                                                   |
| `web_urls[]`                  | `field_web_address` + content-access token (`cbt=`) like web verify UI |
| `field_location`              | GeoJSON geometry export from `field_location` geofield                 |
| `nace_tid`, `nace_label`      | From `field_classification`                                            |
| `reward_kind`                 | `merits` | `chrons` from `{civic_work_report_reward}`                  |
| `date_started`                | Unix timestamp from `field_date`                                       |


**Verification context** — include only when Bearer user owns an assigned,
non-declined slot for this WR:

```json
{
  "verification": {
    "slot_nid": "527175",
    "can_decline": true,
    "verification_count": 0,
    "verification_needed": 2
  }
}
```


| Case                                    | HTTP                                  |
| --------------------------------------- | ------------------------------------- |
| WR not found / unpublished              | **404**                               |
| Caller has no assigned slot for this WR | **403** `{ "error": "not_assigned" }` |


Prefer shared helper `rovas_civic_work_report_item($node, $lang, $include_proof, $verifier_uid)` used by both `civic_project_work_reports` and
`civic_work_report/{nid}`.

### 3. `POST /rovas/civic_submit_verification`

Bearer required. Body:

```json
{
  "slot_nid": "527175",
  "verdict": "approve",
  "verified_hours": 4,
  "note": "optional string"
}
```


| Field            | Rules                                                          |
| ---------------- | -------------------------------------------------------------- |
| `verdict`        | `approve` | `reject` | `decline`                               |
| `verified_hours` | Optional; default = WR `field_hours_worked`; required positive |
| `note`           | Optional for approve; **required for reject**                  |


**Behaviour** (mirror existing Drupal — do not invent new rules):

- `**approve` / `reject`:** `rules_verifiability` + create `verification` node
(like `rules_create_verification`) with `field_verdict` boolean,
`field_verified_time`, optional `field_verification_note`,
`field_verification_source = 1` (triggers `rules_verify_work_report`).
- `**decline`:** only when slot `field_voluntary`; set `field_declined` on slot
(like `node_form_decline` in `rovas_verify.module`); optional note email.


| Error              | HTTP |
| ------------------ | ---- |
| `not_assigned`     | 403  |
| `already_verified` | 409  |
| `not_verifiable`   | 403  |
| `cannot_decline`   | 403  |
| `invalid_input`    | 400  |


**Success:**

```json
{
  "ok": true,
  "verification_nid": "527300",
  "work_report_nid": "527174",
  "verdict": "approve"
}
```

(`verification_nid` omitted for `decline`.)

### Checklist

1. [x] Shared `rovas_civic_work_report_item()` helper (+ geofield GeoJSON export, web URLs with token)
2. [x] `GET /rovas/civic_pending_verifications` — Bearer; register in Services + update hook
3. [x] `GET /rovas/civic_work_report/{nid}` — Bearer; verification block when assigned
4. [x] `POST /rovas/civic_submit_verification` — Bearer; approve/reject/decline
5. [ ] Deploy to **rovdev** only
6. [ ] Smoke tests below

### Smoke test (after deploy)

```bash
# TOKEN = Civic OAuth access token for a user with at least one assigned slot
curl -sS "https://dev.rovas.app/rovas/civic_pending_verifications" \
  -H "Authorization: Bearer $TOKEN" -H "Accept: application/json"
# Expect { "total": N, "items": [ { "slot_nid", "work_report_nid", ... } ] }

WR_NID="<work_report_nid from items[0]>"
SLOT_NID="<slot_nid from items[0]>"

curl -sS "https://dev.rovas.app/rovas/civic_work_report/${WR_NID}" \
  -H "Authorization: Bearer $TOKEN" -H "Accept: application/json"
# Expect full WR + verification { slot_nid, can_decline, verification_count, verification_needed }

curl -sS -X POST "https://dev.rovas.app/rovas/civic_submit_verification" \
  -H "Authorization: Bearer $TOKEN" -H "Content-Type: application/json" \
  -d "{\"slot_nid\":\"${SLOT_NID}\",\"verdict\":\"approve\",\"verified_hours\":1}"
# Expect { "ok": true, "verification_nid": "...", "verdict": "approve" }

# Re-fetch pending — total decreased by 1; create WR no longer blocked when total=0
curl -sS -X POST "https://dev.rovas.app/rovas/civic_create_work_report" \
  -H "Authorization: Bearer $TOKEN" -H "Content-Type: application/json" \
  -d '{ "project_nid": "…", "date_started": 1750000000, "hours": 1,
        "description": "gate test", "nace_tid": "860", "web_url": "https://example.org/x" }'
```

### Civic will consume

- `GET /verifications/pending?lang=` → proxy above
- `GET /work-reports/:nid?lang=` → proxy `civic_work_report/{nid}`
- `POST /verifications/:slot_nid` → proxy submit

See `~/Documents/homeProjects/civic/docs/rovas-integration.md`.

### Follow-up: OS push on verifier assignment (Phase 4)

Optional later: when `rules_find_verifier` assigns a report to a slot, POST to
Civic internal webhook so Civic can send Expo push with deep link
`civic://earn/verify/{slot_nid}`. Civic will register push tokens via
`POST /push-tokens`. Not blocking in-app notifications.

## Next work: merit index (Civic Prínos / ranks screen)

**Implemented in `rovas_civic`.** Civic’s **Prínos** tab needs a JSON Merit Index
(leaderboard) matching the Rovas web page `/users-merit-rank` (Views
`users_merit_rank`), plus Civic-only `**by_category**` ME-source shares for a
stacked bar column.

Do **not** scrape HTML or call `views_get_view()`. Mirror the view with SQL
(same merit total as `_base_user_me_rank()` / `rovas_merit_distribution`).

**Auth:** Bearer required (`civic` / `user_profile`). No username filter param.

### Parity with web Merit Index


| Views column              | Civic JSON                                      |
| ------------------------- | ----------------------------------------------- |
| Position (#)              | `position` (1-based within filtered/sorted set) |
| Worker + Since            | `username`, `display_name`, `member_since_year` |
| Merits                    | `total_me`                                      |
| Value (€)*                | `value_eur`                                     |
| Country                   | `country_iso2`, `country_label`                 |
| NACE icons row            | `nace_expertise[]` (top 5, shared builder)      |
| *(Civic only)* ME sources | `by_category` `{ taxes, charity, volunteer }`   |


Web exposed filters: `**country_iso2**`, **From date** (`since_from`). Civic
adds numeric ME/€ ranges, NACE section, and ME source category filter.

### Endpoint


| Method | URL                                             | Auth            |
| ------ | ----------------------------------------------- | --------------- |
| `GET`  | `https://dev.rovas.app/rovas/civic_merit_index` | Public (optional Bearer) |


Machine-name path only.


| Param                             | Rules                                                                                                                                |
| --------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------ |
| `limit`                           | Default **50**, max **100**                                                                                                          |
| `offset`                          | Default 0                                                                                                                            |
| `sort`                            | `position` | `total_me` | `value_eur` | `member_since` | `country` | `taxes_me` | `charity_me` | `volunteer_me` — default `total_me` |
| `order`                           | `asc` | `desc` — default `desc`                                                                                                      |
| `country_iso2`                    | ISO-2 (case-insensitive), parity with web                                                                                            |
| `since_from`                      | `YYYY-MM-DD` — user `created` ≥ start of that day (UTC)                                                                              |
| `min_me` / `max_me`               | Optional floats on `total_me`                                                                                                        |
| `min_value_eur` / `max_value_eur` | Optional floats on `value_eur`                                                                                                       |
| `nace_section_tid`                | Top-level NACE section tid; user must have expertise in that section                                                                 |
| `source_category`                 | ME source filter: users with `by_category.{taxes\|charity\|volunteer} > 0`; default sort → `{category}_me` desc when `sort=total_me`                  |
| `lang`                            | Optional `sk` | `en` — localized `country_label` + `nace_expertise[].section_label`                                                  |


### 200 response

```json
{
  "total": 298,
  "limit": 50,
  "offset": 0,
  "sort": "total_me",
  "order": "desc",
  "items": [
    {
      "position": 1,
      "rovas_uid": "42",
      "username": "anna_nb",
      "display_name": "Anna Nováková",
      "member_since_year": 2019,
      "total_me": 3410.0,
      "value_eur": 2557.5,
      "country_iso2": "SK",
      "country_label": "Slovakia",
      "by_category": {
        "taxes": 980.0,
        "charity": 210.0,
        "volunteer": 148.0
      },
      "nace_expertise": [
        {
          "section_tid": 942,
          "section_label": "Construction",
          "section_letter": "f",
          "icon_url": "https://dev.rovas.app/sites/default/files/pictures/nace_icons/f.svg",
          "me": 420.0,
          "share_percent": 34,
          "hourly_rate_eur": 12.5
        }
      ]
    }
  ]
}
```

### Required logic

1. Eligible users: `status = 1`, `uid > 1`, `total_me >= 0` (same hide rule as
  `users_lookup`).
2. `**total_me`:** `userpoints_total.points + field_merit_decimal_part` (same as
  `/rovas/me`).
3. `**value_eur`:** `(total_me / ROVAS_CIVIC_ME_PER_HOUR) *
  rovas_civic_national_hourly_rate()` hourly wage.
4. `**by_category`:** `rovas_civic_users_lookup_expertise_categories($uid)`.
5. `**nace_expertise`:** `rovas_meritscope_user_expertise_json($uid, $lang, 5)`
  when available; else `[]`.
6. `**position`:** index in filtered/sorted list (`offset + row_index + 1`).
7. Sort `position` alias → sort by `total_me` (desc = rank 1 first when
  `order=asc` on position — Civic sends explicit sort/order).

### Checklist

1. [x] `GET /rovas/civic_merit_index` Services resource in `rovas_civic`
2. [x] SQL base list + PHP enrich/sort/filter/paginate
3. [x] Public read: `services_access_menu` + `oauth2_server.require_authentication=0` on `rovas_endpoint` (`7016`) **and** no `rovas_civic_request_uid()` gate in Services callbacks
4. [x] `rovas_civic_update_7014` + ensure helper
5. [ ] Deploy to rovdev + `drush74 cc all`
6. [ ] Smoke-test curl below

### Smoke test (after deploy)

```bash
# Anonymous (Prínos without sign-in)
curl -sS "https://dev.rovas.app/rovas/civic_merit_index?limit=5&min_me=1&sort=total_me&order=desc" \
  -H "Accept: application/json"
# Expect: 200, total + items[]

curl -sS "https://dev.rovas.app/rovas/civic_merit_index_countries?lang=sk" \
  -H "Accept: application/json"
# Expect: 200, countries[]

# Signed-in (still works with Bearer)
# TOKEN = Civic OAuth access token with user_profile scope
curl -sS "https://dev.rovas.app/rovas/civic_merit_index?limit=5&country_iso2=SK&sort=total_me&order=desc" \
  -H "Authorization: Bearer $TOKEN" \
  -H "Accept: application/json"
# Expect: total, items[] with by_category + nace_expertise

curl -sS "https://dev.rovas.app/rovas/civic_merit_index?source_category=taxes&limit=3" \
  -H "Authorization: Bearer $TOKEN" -H "Accept: application/json"
```

### Civic will consume

- Civic API `GET /rank/index` → proxy above (optional Bearer; anonymous OK after `7016`)
- Expo **Prínos** tab: horizontal Merit Index table + ME-source stacked bar;
column filters on all columns except `@username`

See `~/Documents/homeProjects/civic/docs/rovas-integration.md`.

## Next work: merit index Prínos filter fixes

**Implemented in `rovas_civic`.** Civic **Prínos** tab filter UX is wired; three Rovas fixes are required for
correct behaviour. Civic API proxies are ready (`GET /rank/index/countries` →
`civic_merit_index_countries`).

### 1. `source_category` — include any ME in category (not dominant only)

**Bug:** `rovas_civic_merit_index_page()` filters with
`rovas_civic_merit_index_dominant_category()` — only users whose **largest**
`by_category` bucket matches are returned. Civic expects users with **any**
Merits in that source (e.g. filter `taxes` → all users with `by_category.taxes > 0`).

**Example:** `@sano` has 338 ME from taxes on his profile but is omitted when
filtering `source_category=taxes` because another category dominates.

**Change** in `rovas_civic.merit_index.inc`:

```php
// Replace dominant check with:
if (!empty($opts['source_category'])) {
  $cat = $opts['source_category'];
  if (empty($by_category[$cat]) || (float) $by_category[$cat] <= 0) {
    continue;
  }
}
```

When `source_category` is set and `sort` is still `total_me`, default sort to
`{category}_me` desc (or document that Civic already sends `sort=taxes_me` etc.).

**Smoke test:**

```bash
curl -sS "https://dev.rovas.app/rovas/civic_merit_index?source_category=taxes&limit=50&sort=taxes_me&order=desc" \
  -H "Authorization: Bearer $TOKEN" -H "Accept: application/json"
# Expect: @sano (and every user with taxes ME > 0), each item by_category.taxes > 0
```

### 2. `GET /rovas/civic_merit_index_countries`

Distinct countries for Merit Index users (`total_me > 0`, same eligibility as
index). Civic Prínos country dropdown — default “all countries”.

| Method | URL | Auth |
|--------|-----|------|
| `GET` | `https://dev.rovas.app/rovas/civic_merit_index_countries` | Public (optional Bearer) |

| Param | Rules |
|-------|-------|
| `lang` | Optional `sk` \| `en` — localized `label` |

**200 response:**

```json
{
  "countries": [
    { "iso2": "SK", "label": "Slovensko", "user_count": 142 },
    { "iso2": "CZ", "label": "Česko", "user_count": 8 }
  ]
}
```

- Only ISO-2 codes present on at least one eligible user with `total_me > 0`.
- Sort by `label` ascending (locale-aware if easy; else `label` string).
- Reuse `field_country` / `rovas_civic_merit_index_country_label()` for labels.

**Checklist:**

1. [x] Services resource `civic_merit_index_countries` in `rovas_civic`
2. [x] SQL: distinct `country_iso2` from same base query as merit index
3. [x] OAuth2-only; updb hook + `rovas_civic_ensure_services_resources()`
4. [ ] Deploy rovdev + smoke curl

```bash
curl -sS "https://dev.rovas.app/rovas/civic_merit_index_countries?lang=sk" \
  -H "Authorization: Bearer $TOKEN" -H "Accept: application/json"
```

### 3. `nace_section_tid` — letter sections (A–U), not vocabulary root

**Bug risk:** Civic filter picker uses **letter sections** (children of the NACE
vocabulary root, e.g. “F — Stavebníctvo”), matching
`rovas_meritscope_user_expertise_json()` `section_tid` and
`rovas_civic_merit_index_nace_section_tid()`.

Confirm `nace_section_tid` filter matches the **section letter term tid** (one
level below root “Pracovné činnosti”), not the vocabulary root tid. If
`rovas_civic_merit_index_nace_section_tid()` returns the wrong ancestor depth,
align with Meritscope section tid used in `nace_expertise[].section_tid`.

**Smoke test:** pick section tid from a row’s `nace_expertise[0].section_tid`;
same user must appear when filtering `nace_section_tid={that_tid}`.

**Checklist:**

1. [x] `rovas_civic_merit_index_nace_section_tid()` matches Meritscope `section_tid` depth
2. [ ] Deploy rovdev + smoke test with `nace_section_tid` from a row’s `nace_expertise[0]`

### Civic will consume

- `GET /rank/index/countries` (already proxied)
- Existing `source_category` + `nace_section_tid` params once fixed above

## Next work: by_source on merit index

**Implemented in `rovas_civic`.** Civic Prínos (Impact) ME-source bars remap ledger sources into Civic
buckets: `tax_payment`→taxes, `charitable_donation`→charity,
`volunteer_work`+`volunteer`+`penalty`→volunteer. Rovas `by_category` still
**excludes** `penalty`. `users_lookup` already returns `by_source`; Merit Index
items now do too so Civic can apply the same remap on Prínos.

**Already available:** `rovas_merit_users_lookup_data_batch()` / expertise batch
loads `by_source` per uid; `rovas_civic_merit_index_page()` serializes both
`by_category` and `by_source`.

### Change

On each `civic_merit_index` item, add `by_source` (same shape as
`users_lookup.by_source`):

```json
"by_source": {
  "tax_payment": 980.0,
  "charitable_donation": 210.0,
  "volunteer_work": 120.0,
  "volunteer": 28.0,
  "penalty": -15.0
}
```

Rules:

- Keyed by `field_merit_source`; omit zero keys
- Include negative `penalty` when present
- Round floats like `by_category` (2 dp)
- Keep existing `by_category` unchanged (Rovas semantics; Civic remaps from
  `by_source` when present)

### Checklist

1. [x] Include `by_source` from expertise batch on each merit-index item
2. [x] Omit empty object when user has no sourced ledger rows (`{}` when none)
3. [ ] Deploy rovdev

### Smoke test

```bash
curl -sS "https://dev.rovas.app/rovas/civic_merit_index?limit=5" \
  -H "Accept: application/json"
# Expect: each item has by_category AND by_source (when ledger sources exist)
# Pick a uid with merit_penalty rows — by_source.penalty < 0
```

### Response shape (Civic)

Civic API `GET /rank/index` keeps returning only remapped `by_category` to Expo
(does not need to forward `by_source` to the client).

## Next work: merit index filter latency

**Implemented in `rovas_civic` (snapshot).** Civic Prínos filter chips
(`source_category`, `country_iso2`, `nace_section_tid`) were taking **~6–7s**
on `dev.rovas.app` because every request rebuilt the full leaderboard in PHP
(including per-uid ledger aggregates). Baseline (2026-09-03): unfiltered /
taxes / SK all ~6.5–7.1s even when `taxes` returned only 2 rows.

**Approach:** precompute a **Merit Index snapshot** (all eligible users with
`by_category`, `by_source`, section tids, `nace_expertise` for `en`+`sk`).
Request path only filters / sorts / paginates the snapshot. Rebuild:

- On cron when older than interval (default **12 hours**; site cron may run
  every 5 minutes — the gate skips until due)
- On admin **Rebuild Merit Index snapshot**
  (`/admin/config/rovas/civic`)
- On cache miss (first request after `cc all`)
- Via `rovas_civic_update_7017` after deploy

Interval is configurable on the admin form (hours). Same public JSON contract.

### Checklist

1. [x] Profiled on rovdev — ~7s cold for filtered and unfiltered first page
2. [x] Snapshot build + load; request path filters from snapshot only
3. [x] Precompute `nace_expertise` for sk/en in snapshot (no N+1 on read)
4. [x] Cron interval gate (default 12h) + admin rebuild button
5. [ ] Deploy rovdev + `drush74 updb` (7017 builds initial snapshot)
6. [ ] Re-measure — warm first page should be **≪ 1s**

### Smoke test

```bash
# After deploy + updb (or admin Rebuild):
time curl -sS -o /tmp/mi.json -w "%{http_code} %{time_total}\n" \
  "https://dev.rovas.app/rovas/civic_merit_index?limit=50&min_me=1&sort=total_me&order=desc" \
  -H "Accept: application/json"

time curl -sS -o /tmp/mi-taxes.json -w "%{http_code} %{time_total}\n" \
  "https://dev.rovas.app/rovas/civic_merit_index?limit=50&min_me=1&source_category=taxes&sort=taxes_me&order=desc" \
  -H "Accept: application/json"

# Expect: HTTP 200; time_total well under 1s when snapshot is warm;
# JSON still has items[].by_category, by_source, nace_expertise, position.
```

### Civic will consume

No API change required. Civic already proxies `GET /rank/index` → this
endpoint. Snapshot freshness is up to the rebuild interval (default 12h).

## Next work: OAuth session persistence

**Problem:** Civic mobile users were logged out after ~1 hour of inactivity. Rovas
access tokens expire after **3600s**. The OAuth server consumed refresh tokens
on use (`unset_refresh_token_after_use`) but did **not** issue replacements
(`always_issue_new_refresh_token: false`), so the mobile app could not refresh
after the first token rotation. Authorization codes also defaulted to **30s**,
causing “authorization code has expired” when re-login was delayed.

**Change (implemented in `rovas_oauth_ensure_server()`):**

1. Set `always_issue_new_refresh_token` = **TRUE** on the `civic` OAuth server.
2. Set `auth_code_lifetime` = **120** seconds (minimum; idempotent upgrade).

Existing servers are updated on the next `rovas_oauth_ensure_server()` call
(admin bootstrap or deploy hook).

**Checklist:**

1. [x] `always_issue_new_refresh_token` in `rovas_oauth_ensure_server()` (create + migrate)
2. [x] `auth_code_lifetime` = 120 in same helper
3. [x] Deploy rovdev + run ensure (2026-09-02: `always_issue_new_refresh_token` was `false` → now `true`; `auth_code_lifetime` = 120)

**Smoke test:**

```bash
# 1. Login in Civic mobile — note refresh_token from token response (or DB).
# 2. Refresh access token:
curl -sS -X POST "https://dev.rovas.app/oauth2/token" \
  -H "Content-Type: application/x-www-form-urlencoded" \
  -d "grant_type=refresh_token&client_id=civic&refresh_token=PASTE_REFRESH_TOKEN"
# Expect 200 with NEW access_token AND refresh_token in JSON body.
# Old refresh_token must no longer work (one-time use).
```

Civic mobile (`apps/mobile/lib/auth.tsx`) preserves `refreshToken` when the
server omits it, refreshes on app foreground, and only clears the session on
`invalid_grant` (not transient network errors).

## Related Civic docs

- Deploy / enable on a prod clone: [`civic-deploy.md`](civic-deploy.md)
- Plan: `~/Documents/homeProjects/civic/docs/plan.md` (§6 claim SoT, §7 split claims, §8b claim rule, §15 phases)
- NeoFund: `~/Documents/Business/NeoFund_NP/` (State Treasury, Path-1 donor Merit, A12)
- Civic API/Expo: `~/Documents/homeProjects/civic/docs/rovas-integration.md`
- Mockup (home UX): `~/Documents/homeProjects/civic/mocs/civic-mockup.canvas.tsx`

