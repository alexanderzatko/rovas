---
status: done
slug: civic-native-auth-bridge
owner_repo: both
created: 2026-11-16
updated: 2026-11-16
---

# Civic native auth session bridge

> Done — Trusted `POST /rovas/civic_auth_session` (`rovas_oauth_update_7003`);
> create/link + mint Civic OAuth tokens for native Google/Apple.

**Product:** Civic is moving login/signup out of the Rovas browser OAuth hop
into native Google + Apple Sign-In in the Expo app. Users get stuck when
HybridAuth (Google/FB) does not return to `civic://`. Civic will verify IdP
tokens on its API, then need Rovas to **create or link** the Drupal user and
**mint** Civic OAuth Bearer tokens without `/oauth2/authorize`.

The existing partner endpoint `POST /rovas/rovas_partner_user` is the wrong
shape: integer `user_id`, returns classic `api_key`/`token`, no OAuth mint.
Civic APIs require Bearer (`client_id=civic`, scope `user_profile`).

**Owner:** `both` — Rovas implements the bridge; Civic verifies IdP tokens and
calls it with a technical api-client account.

**Do not change:**

- Existing Authorization Code + PKCE path (keep as legacy/fallback)
- Token lifetimes / refresh rotation on the `civic` OAuth server
- Nabezky `rovas_partner_user` contract
- Bearer-protected Civic resources (`/rovas/me`, etc.)

### Contract

**`POST /rovas/civic_auth_session`**

| | |
|---|---|
| **Auth** | Classic headers `API-KEY` + `TOKEN` of a dedicated Civic **api client** technical account (server-only) |
| **Permission** | Same family as partner create (`api client` / administrator), or a dedicated Civic auth permission |
| **Content-Type** | `application/json` |

**Request body** (Civic has already verified the IdP token):

```json
{
  "provider": "google",
  "provider_subject": "108234567890123456789",
  "email": "user@example.com",
  "email_verified": true,
  "display_name": "Jane Doe",
  "user_language": "sk",
  "user_country": "SK"
}
```

| Field | Type | Required | Notes |
|-------|------|----------|--------|
| `provider` | string | yes | `google` \| `apple` (later `facebook`, `email`) |
| `provider_subject` | **string** | yes | IdP `sub` (not integer — unlike Nabezky `user_id`) |
| `email` | email | yes* | Required unless product allows anonymous Apple (prefer require) |
| `email_verified` | boolean | yes | From IdP claims |
| `display_name` | string | no | Set `field_full_name` only when non-empty; omit/null/"" leaves it empty (no email local-part fallback) — see [civic-auth-display-name-idp-only](civic-auth-display-name-idp-only.md) |
| `user_language` | string | yes | Normalized like partner create (`sk`/`cs` → `sk`, else enabled / `en`) |
| `user_country` | string | yes | Uppercase ISO 3166-1 alpha-2; Civic defaults to `SK` |

**Behavior:**

1. Map `provider` → HybridAuth machine name (default `google`→`Google`,
   `apple`→`Apple`; override via `rovas_oauth_civic_auth_providers`);
   providers must be enabled in HybridAuth.
2. Lookup order: HybridAuth (`provider` + `provider_subject`) → else verified
   email match → else create Drupal user (status=1, random password OK for
   social).
3. Attach HybridAuth identity if missing; set language/country on create.
4. **Mint** OAuth access + refresh for client `civic`, scope `user_profile`,
   same lifetimes and `always_issue_new_refresh_token` as authorize path.
   No consent screen; no redirect URI required for this grant.

**Success `200`:**

```json
{
  "rovas_uid": 123,
  "access_token": "…",
  "refresh_token": "…",
  "expires_in": 3600,
  "token_type": "Bearer",
  "scope": "user_profile"
}
```

Shape should match what Expo already stores from `/oauth2/token` so Civic can
persist without a second exchange.

**Errors:**

| Status | When |
|--------|------|
| 400 | Validation (provider, country casing, language, missing subject) |
| 401 | Bad technical `API-KEY` / `TOKEN` |
| 403 | Technical account lacks permission |
| 409 | Missing/unverified email when link would be ambiguous |
| 500 | User create or token mint failure |

**Not acceptable:** returning classic end-user `api_key`/`token` only.

### Provisioning (ops)

1. Dedicated Rovas technical user with permission **mint civic auth session**
   (and enough Services access for `rovas_endpoint` API keys).
2. Generate `API-KEY` / `TOKEN` for `rovas_endpoint`; give to Civic API env
   (`ROVAS_API_KEY` / `ROVAS_TOKEN`) — never Expo/`EXPO_PUBLIC_*`.
3. Enable HybridAuth providers **Google** and **Apple**.
4. After deploy: `drush74 updb` (`rovas_oauth_update_7003`) + `drush74 cc all`.
5. Document keys for rovdev vs prod separately.

### Checklist

1. [x] Services resource `POST /rovas/civic_auth_session` (OpenAPI in `rovasApi.yml`)
2. [x] String `provider_subject` + provider allowlist (`google`, `apple`)
3. [x] Create or link user + HybridAuth identity (email link when verified)
4. [x] Mint `civic` OAuth access + refresh (same lifetimes/rotation as authorize)
5. [x] Reject unverified/missing email with 409 when link is ambiguous
6. [x] Provision rovdev technical account + keys; enable HybridAuth providers
7. [x] Deploy rovdev + smoke (human unless asked)

**Rovas code:** `rovas_oauth.auth_session.inc` + `rovas_oauth_update_7003`. Permission
`mint civic auth session`. HybridAuth map defaults `google→Google`, `apple→Apple`
(`rovas_oauth_civic_auth_providers` variable).

### Smoke test

```bash
# Technical account credentials (rovdev)
API_KEY='…'
TOKEN='…'

curl -sS -X POST "https://dev.rovas.app/rovas/civic_auth_session" \
  -H "Accept: application/json" \
  -H "Content-Type: application/json" \
  -H "API-KEY: $API_KEY" \
  -H "TOKEN: $TOKEN" \
  -d '{
    "provider": "google",
    "provider_subject": "smoke-test-subject-001",
    "email": "civic-smoke-auth@example.com",
    "email_verified": true,
    "display_name": "Civic Smoke",
    "user_language": "sk",
    "user_country": "SK"
  }'
# Expect 200 with rovas_uid, access_token, refresh_token, expires_in, token_type

# Introspect minted access token
ACCESS='…'  # from response
curl -sS "https://dev.rovas.app/oauth2/tokens/$ACCESS" -H "Accept: application/json"
# Expect user_id matching rovas_uid, client_id civic, scope user_profile

# Bearer /rovas/me
curl -sS "https://dev.rovas.app/rovas/me" \
  -H "Accept: application/json" \
  -H "Authorization: Bearer $ACCESS"
```

### Civic will consume

1. Expo native Google / Apple → ID token.
2. Civic API `POST /auth/google` / `POST /auth/apple` verifies JWKS, then calls
   this endpoint with technical keys.
3. API returns the mint JSON to the app; SecureStore + refresh via
   `{ROVAS}/oauth2/token` unchanged.
4. Companion: `~/Documents/homeProjects/civic/docs/rovas-integration.md`.

### Phase 2 (out of scope here)

Email/password: extend with `provider=email` (password set/verify) or sibling
login/register endpoints that reuse the same mint helper. Facebook: add
`provider=facebook` on the same bridge.
