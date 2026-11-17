---
status: done
slug: civic-email-password-auth
owner_repo: both
created: 2026-11-17
updated: 2026-11-17
---

# Civic email / username + password auth

> Done — `POST /rovas/civic_auth_register` + `POST /rovas/civic_auth_login`
> (`rovas_oauth_update_7005`); reuse `rovas_oauth_mint_civic_tokens()`.

**Product:** Civic Phase 2 auth. Users register with **username + email +
password** and log in with **username or email + password** inside the Expo
app. Civic API forwards credentials to Rovas; Rovas creates or verifies the
Drupal account and **mints** the same Civic OAuth Bearer tokens as
[`civic-native-auth-bridge`](civic-native-auth-bridge.md)
(`POST /rovas/civic_auth_session`). Password reset stays on Rovas web
(`/user/password`); Civic only links there.

**Owner:** `both` — Rovas implements sibling register/login + mint; Civic
exposes `/auth/register` + `/auth/login` and AuthForm fields.

**Do not change:**

- Existing `POST /rovas/civic_auth_session` social contract (google/apple)
- Token lifetimes / refresh rotation on the `civic` OAuth server
- Nabezky `rovas_partner_user` contract
- Bearer-protected Civic resources (`/rovas/me`, etc.)

### Contract

Reuse the **same mint helper** as `civic_auth_session`. Prefer **sibling**
endpoints (clearer validation than overloading `provider=email` on the social
body). Auth: classic `API-KEY` + `TOKEN` of the Civic technical account
(permission **mint civic auth session** or equivalent).

---

#### `POST /rovas/civic_auth_register`

**Content-Type:** `application/json`

**Request body:**

```json
{
  "username": "jana.novak",
  "email": "jana@example.com",
  "password": "correct-horse-battery",
  "user_language": "sk",
  "user_country": "SK"
}
```

| Field | Type | Required | Notes |
|-------|------|----------|--------|
| `username` | string | yes | Letters, numbers, `.` `_` `-` only; max 50 (Civic profile rules) |
| `email` | email | yes | Unique; normalize as Drupal does |
| `password` | string | yes | Set as the Drupal password (not random). Min length ≥ 8 |
| `user_language` | string | yes | Same normalization as partner / auth session |
| `user_country` | string | yes | Uppercase ISO 3166-1 alpha-2 |

**Behavior:**

1. Validate username / email / password.
2. If email or username already exists → **409** with a machine-readable
   `code` when practical: `email_taken` | `username_taken`.
3. Create Drupal user with the **provided** password, `status=1` (same as
   social create). Set language/country. Stamp `rovas_signup_source=civic` if
   that flag already exists on bridge creates.
4. **Do not** attach a HybridAuth identity.
5. **Mint** OAuth access + refresh for client `civic`, scope `user_profile`
   (same lifetimes / refresh rotation as authorize / `civic_auth_session`).

**Success `200`:** same shape as `civic_auth_session`:

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

**Errors:**

| Status | When |
|--------|------|
| 400 | Validation (username pattern, email, weak/short password, country/language) |
| 401 | Bad technical `API-KEY` / `TOKEN` |
| 403 | Technical account lacks permission |
| 409 | Email or username taken (`code`: `email_taken` \| `username_taken`) |
| 500 | User create or token mint failure |

Optional error body: `{ "error": "…", "code": "email_taken" }`.

---

#### `POST /rovas/civic_auth_login`

**Content-Type:** `application/json`

**Request body:**

```json
{
  "login": "jana.novak",
  "password": "correct-horse-battery"
}
```

| Field | Type | Required | Notes |
|-------|------|----------|--------|
| `login` | string | yes | Drupal **username** or **email** |
| `password` | string | yes | Plain password to verify |

**Behavior:**

1. Resolve `login` as username **or** email to a Drupal account.
2. Verify password (`user_check_password`). On unknown user **or**
   bad password → **401** with the **same** generic message (avoid account
   enumeration).
3. Accounts created via Google/Apple (random password) fail until the user
   sets a password on Rovas (`/user/password`) — expected.
4. **Mint** the same Civic OAuth token pair as register / `civic_auth_session`.

**Success `200`:** identical mint JSON.

**Errors:**

| Status | When |
|--------|------|
| 400 | Missing/empty `login` or `password` |
| 401 | Bad technical creds **or** invalid login/password (app-facing auth failure) |
| 403 | Technical account lacks permission |
| 500 | Token mint failure |

**Not acceptable:** returning classic end-user `api_key`/`token` only.

### Checklist

1. [x] Services resources `POST /rovas/civic_auth_register` and
       `POST /rovas/civic_auth_login` (OpenAPI in `rovasApi.yml`)
2. [x] Reuse mint helper from `civic_auth_session` (client `civic`,
       `user_profile`)
3. [x] Register: set provided password; username rules; no HybridAuth row;
       `rovas_signup_source=civic` when applicable
4. [x] Register 409 with `email_taken` / `username_taken` when easy
5. [x] Login: resolve username **or** email; generic 401 on failure
6. [ ] Deploy rovdev + smoke (human unless asked)

### Smoke test

```bash
API_KEY='…'
TOKEN='…'
BASE='https://dev.rovas.app'

# Register
curl -sS -X POST "$BASE/rovas/civic_auth_register" \
  -H "Accept: application/json" \
  -H "Content-Type: application/json" \
  -H "API-KEY: $API_KEY" \
  -H "TOKEN: $TOKEN" \
  -d '{
    "username": "civic.pw.smoke",
    "email": "civic-pw-smoke@example.com",
    "password": "SmokeTestPass1",
    "user_language": "sk",
    "user_country": "SK"
  }'
# Expect 200 with rovas_uid, access_token, refresh_token, expires_in

# Login by username
curl -sS -X POST "$BASE/rovas/civic_auth_login" \
  -H "Accept: application/json" \
  -H "Content-Type: application/json" \
  -H "API-KEY: $API_KEY" \
  -H "TOKEN: $TOKEN" \
  -d '{
    "login": "civic.pw.smoke",
    "password": "SmokeTestPass1"
  }'

# Login by email
curl -sS -X POST "$BASE/rovas/civic_auth_login" \
  -H "Accept: application/json" \
  -H "Content-Type: application/json" \
  -H "API-KEY: $API_KEY" \
  -H "TOKEN: $TOKEN" \
  -d '{
    "login": "civic-pw-smoke@example.com",
    "password": "SmokeTestPass1"
  }'

# Bad password → 401
curl -sS -o /dev/null -w "%{http_code}\n" -X POST "$BASE/rovas/civic_auth_login" \
  -H "Accept: application/json" \
  -H "Content-Type: application/json" \
  -H "API-KEY: $API_KEY" \
  -H "TOKEN: $TOKEN" \
  -d '{"login":"civic.pw.smoke","password":"wrong"}'
```

### Civic will consume

1. Expo AuthForm → Civic `POST /auth/register` / `POST /auth/login`.
2. Civic API calls these endpoints with technical keys; returns Expo-shaped
   tokens; SecureStore + refresh via `{ROVAS}/oauth2/token` unchanged.
3. Forgot password: open `https://rovas.app/user/password` (no in-app reset).
4. Companion: `~/Documents/homeProjects/civic/docs/rovas-integration.md`.
