# Civic ↔ Rovas integration

Civic (`~/Documents/homeProjects/civic`) is a Slovak-first Expo app that uses
Rovas as its identity and Merit ledger. This document is the **Rovas-side
source of truth** for OAuth and the APIs Civic needs next. Civic-side notes
live at `~/Documents/homeProjects/civic/docs/rovas-integration.md` (Expo +
Civic API); keep both in sync when contracts change.

**Agents working in this repo:** implement the work under
[Next work for Rovas (home + Phase 2)](#next-work-for-rovas-home--phase-2)
below. Do not rely on reading the Civic tree unless you need Expo/API details.

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
| Home Merits reads | `/rovas/me`, `/rovas/contributions/aggregate`, `/rovas/merit_distribution` | Implemented in code; enable via updb / admin button on rovdev |
| Claim rule + split claims | — | Phase 2 (after home reads or in parallel) |

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

## Next work for Rovas (home + Phase 2)

Civic’s home mockup needs live Merits data. The mobile app already has a Rovas
**Bearer** access token. Civic API can introspect it to a Drupal **uid**.

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

### Later (Phase 2 claim path — not blocking home reads)

- `POST /rovas/rules/rules_proxy_claim_vat_receipt` — atomic multi-beneficiary
  Merit grant; Bearer preferred (see Civic `docs/plan.md` §7–§8)
- `GET /rovas/users/search?q=` — split-claim typeahead
- Ensure Services OAuth2 covers these when they ship

### Acceptance checks (dev / rovdev)

```bash
# After deploy + updb (or Enable Civic home API resources):
curl -sS https://dev.rovas.app/rovas/me \
  -H "Authorization: Bearer $ACCESS_TOKEN" \
  -H "Accept: application/json"

curl -sS https://dev.rovas.app/rovas/contributions/aggregate \
  -H "Accept: application/json"

curl -sS https://dev.rovas.app/rovas/merit_distribution \
  -H "Accept: application/json"
```

### Out of scope for this slice

- Changing Expo redirect / client id
- Civic claim fraud / SQLite (Civic repo)
- Full category breakdown if Rovas cannot source it yet
- Prod (`/var/www/rovas`) until explicitly requested

## Related Civic docs

- Plan: `~/Documents/homeProjects/civic/docs/plan.md` (§8a OAuth, §8c reads, §15 phases)
- Civic API/Expo: `~/Documents/homeProjects/civic/docs/rovas-integration.md`
- Mockup (home UX): `~/Documents/homeProjects/civic/mocs/civic-mockup.canvas.tsx`
