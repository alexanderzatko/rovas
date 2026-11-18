---
status: done
slug: civic-auth-extra-providers
owner_repo: both
created: 2026-11-18
updated: 2026-11-18
---

# Civic auth: Facebook, Nabezky, OpenStreetMap

> Done — `civic_auth_session` providers extended; OSM may use
> `email_verified: false` when email is collected in-app.

**Product:** Civic is extending native IdP login beyond Google and Apple.
Facebook, Nabezky, and OpenStreetMap already work on Rovas via HybridAuth; Civic
needs the same identities on `POST /rovas/civic_auth_session` so the Expo app
can mint Civic OAuth Bearers without the browser HybridAuth hop (which often
fails to return to `civic://`).

OpenStreetMap does not provide email. Civic collects email in-app after OSM
OAuth and calls this bridge with that address and `email_verified: false`.

**Owner:** `both` — Rovas extends the allowlist / OSM email rule; Civic verifies
or exchanges IdP tokens and calls the bridge.

**Do not change:**

- Nabezky `rovas_partner_user` partner contract
- Authorization Code + PKCE path (legacy fallback)
- Token lifetimes / refresh on the `civic` OAuth server
- Google / Apple / email-password mint behaviour for existing providers

### Contract

Same endpoint as [civic-native-auth-bridge](civic-native-auth-bridge.md):

**`POST /rovas/civic_auth_session`**

| | |
|---|---|
| **Auth** | Classic `API-KEY` + `TOKEN` of Civic technical api-client |
| **Content-Type** | `application/json` |

**Request body** (unchanged shape; expanded `provider`):

```json
{
  "provider": "facebook",
  "provider_subject": "…",
  "email": "user@example.com",
  "email_verified": true,
  "display_name": "Jane Doe",
  "user_language": "sk",
  "user_country": "SK"
}
```

| Field | Change |
|-------|--------|
| `provider` | Allow **`google` \| `apple` \| `facebook` \| `nabezky` \| `openstreetmap`** |
| HybridAuth map (`rovas_oauth_civic_auth_providers`) | Defaults: `facebook`→`Facebook`, `nabezky`→`Nabezky`, `openstreetmap`→`OpenStreetMap` (plus existing google/apple) |
| `email` / `email_verified` | Unchanged for facebook/nabezky/google/apple: create/link still requires verified email when no HybridAuth hit. **Exception:** when `provider` is **`openstreetmap`**, require a valid `email` string but **allow `email_verified: false`** so Civic can mint after in-app email collection. Lookup order remains HybridAuth identity first, then email match / create. |

**Success / errors:** same as civic-native-auth-bridge (`200` mint JSON; `400`/`401`/`403`/`409`/`500`).

Validation error copy lists the five providers.

### Checklist

1. [x] Extend default `rovas_oauth_civic_auth_providers` map (+ OpenAPI / Services description)
2. [x] Validation allowlist for the three new provider strings
3. [x] `openstreetmap`: accept `email_verified: false` when email present; still 409 if email missing on create/link without HybridAuth hit
4. [ ] Ensure HybridAuth providers Facebook, Nabezky, OpenStreetMap enabled on rovdev (ops)
5. [ ] Deploy rovdev + smoke (human unless asked)

### Smoke test

```bash
API_KEY='…'
TOKEN='…'

# Facebook / Nabezky — verified email (same as Google)
curl -sS -X POST "https://dev.rovas.app/rovas/civic_auth_session" \
  -H "Accept: application/json" \
  -H "Content-Type: application/json" \
  -H "API-KEY: $API_KEY" \
  -H "TOKEN: $TOKEN" \
  -d '{
    "provider": "nabezky",
    "provider_subject": "smoke-nabezky-001",
    "email": "civic-smoke-nabezky@example.com",
    "email_verified": true,
    "display_name": "Nabezky Smoke",
    "user_language": "sk",
    "user_country": "SK"
  }'
# Expect 200 with access_token / refresh_token

# OpenStreetMap — unverified collected email
curl -sS -X POST "https://dev.rovas.app/rovas/civic_auth_session" \
  -H "Accept: application/json" \
  -H "Content-Type: application/json" \
  -H "API-KEY: $API_KEY" \
  -H "TOKEN: $TOKEN" \
  -d '{
    "provider": "openstreetmap",
    "provider_subject": "smoke-osm-001",
    "email": "civic-smoke-osm@example.com",
    "email_verified": false,
    "display_name": "OSM Smoke",
    "user_language": "sk",
    "user_country": "SK"
  }'
# Expect 200 (not 409 for unverified email)
```

### Civic will consume

1. Facebook: Limited Login JWT or Graph access token → Civic `POST /auth/facebook` → this bridge.
2. Nabezky: Expo AuthSession PKCE → Civic `POST /auth/nabezky` (code exchange + id_token/UserInfo) → bridge.
3. OpenStreetMap: AuthSession PKCE → Civic `POST /auth/openstreetmap` (pending token) → in-app email → `POST /auth/openstreetmap/complete` → bridge with `email_verified: false`.
4. Companion: `~/Documents/homeProjects/civic/docs/rovas-integration.md`.
