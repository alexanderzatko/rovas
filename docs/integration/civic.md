# Civic ↔ Rovas integration

Civic (`~/Documents/homeProjects/civic`) is a Slovak-first Expo app that uses
Rovas as its identity and Merit ledger. This document covers the Rovas-side
OAuth spike and how to operate it on **dev.rovas.app**.

## Status (spike)

| Piece | Location | Notes |
|-------|----------|--------|
| `oauth2_server` 7.x-1.7 | `sites/all/modules/contrib/oauth2_server` | + PKCE patch [#3015065-7](https://www.drupal.org/project/oauth2_server/issues/3015065) stored in `patches/oauth2_server-3015065-pkce-support-7.patch` |
| `xautoload` 7.x-5.9 | `sites/all/modules/contrib/xautoload` | Required by oauth2_server |
| `oauth2-server-php` **v1.14.2** | `sites/all/libraries/oauth2-server-php` | Module README lists ≤1.8; **1.14.2 is required for PKCE**. Spike must confirm no regressions on authorize/token. |
| `rovas_oauth` | `sites/all/modules/custom/rovas_oauth` | Thin wrapper: bootstrap Civic server/client, login copy, Bearer→uid helper |

## Endpoints

Once modules are enabled on dev:

- Authorize: `https://dev.rovas.app/oauth2/authorize`
- Token: `https://dev.rovas.app/oauth2/token`
- Revoke: `https://dev.rovas.app/oauth2/revoke`

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
`oauth2_server` redirects anonymous authorize requests to `/user/login?destination=oauth2/authorize`, so the existing HybridAuth widget is the authorize login UI. `rovas_oauth` only adds a short intro when the destination is authorize.

## Enable on dev.rovas.app

```bash
# From the rovas repo on your laptop:
chmod +x ops/deploy_rovdev.sh
ops/deploy_rovdev.sh root@<vps-host>

# On the server (docroot /var/www/rovdev):
cd /var/www/rovdev
drush74 en xautoload libraries oauth2_server rovas_oauth -y
drush74 updb -y
drush74 cc all -y
```

Then open `/admin/config/services/rovas-oauth` while logged in as uid 1 and run
**Bootstrap**. Grant `use oauth2 server` to authenticated users if authorize
returns access denied.

Enable **OAuth2 authentication** on the Rovas Services endpoint
(`admin/structure/services`) and select server `civic` when Civic API starts
validating Bearer tokens (Phase 2).

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

## Remaining spike checklist

- [ ] Deploy to `/var/www/rovdev` and enable modules
- [ ] Bootstrap Civic server + client
- [ ] Complete authorize → token with PKCE (curl or Expo)
- [ ] Confirm Bearer token resolves via Services / `rovas_oauth_uid_from_bearer()`
- [ ] Note any oauth2-server-php 1.14.2 incompatibilities vs module README

## Related Civic plan items

- `spike-rovas-oauth` / `rovas-oauth-server` in `civic/docs/plan.md` §8a
- Expo client: `expo-auth-session` + scheme `civic://oauth/callback` (Phase 2)
