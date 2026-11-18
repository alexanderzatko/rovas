---
status: done
slug: civic-auth-services-error-swallow
owner_repo: rovas
created: 2026-11-18
updated: 2026-11-18
---

# Civic auth: stop swallowing `services_error` as HTTP 500

**Product:** Civic OSM login and password login against prod (`rovas.app`) are
failing with a generic 500 (`Unable to complete Civic auth session/login`) when
Rovas should return **409** (email required to link) or **401** (bad password).
Watchdog already logs the real message (`A verified email is required…`,
`Invalid login or password.`), but the HTTP response is always 500.

**Owner:** `rovas`

**Root cause:** In `rovas_oauth.auth_session.inc` and
`rovas_oauth.auth_password.inc`, intentional `return services_error(..., $code)`
calls throw `ServicesException`. Broad `catch (Exception $e)` blocks catch that
and replace it with a generic 500.

**Do not change:** Mint helper behaviour, HybridAuth lookup order, OSM
`email_verified: false` rule.

### Contract

No API shape change. Same endpoints must return the **original** status/body:

| Endpoint | Intended status | Example message |
|----------|-----------------|-----------------|
| `POST /rovas/civic_auth_session` | **409** | A verified email is required to create or link this account. |
| `POST /rovas/civic_auth_login` | **401** | Invalid login or password. |
| `POST /rovas/civic_auth_register` | (existing validation codes) | unchanged |

### Checklist

1. [x] In `rovas_oauth_civic_auth_session_create`, rethrow / ignore
   `ServicesException` (or `ServicesException` + subclasses) in the catch so
   intentional `services_error()` statuses are not rewritten to 500
2. [x] Same fix in `rovas_oauth_civic_auth_login_create` and
   `rovas_oauth_civic_auth_register_create` catch blocks
3. [x] Keep genuine unexpected exceptions as 500 + watchdog
4. [ ] Deploy **prod** `/var/www/rovas` as well as rovdev — Civic API production
   points at `https://rovas.app` (human deploy unless asked)
5. [ ] Smoke (below)

### Smoke test

```bash
API_KEY='…'
TOKEN='…'

# OSM subject with no HybridAuth row and empty email → must be 409 (not 500)
curl -si -X POST "https://rovas.app/rovas/civic_auth_session" \
  -H "Accept: application/json" \
  -H "Content-Type: application/json" \
  -H "API-KEY: $API_KEY" \
  -H "TOKEN: $TOKEN" \
  -d '{
    "provider": "openstreetmap",
    "provider_subject": "civic-smoke-osm-unlinked-999",
    "email": "",
    "email_verified": false,
    "display_name": null,
    "user_language": "sk",
    "user_country": "SK"
  }'
# Expect HTTP 409 and body mentioning verified/email required

# Wrong password → must be 401 (not 500)
curl -si -X POST "https://rovas.app/rovas/civic_auth_login" \
  -H "Accept: application/json" \
  -H "Content-Type: application/json" \
  -H "API-KEY: $API_KEY" \
  -H "TOKEN: $TOKEN" \
  -d '{"login":"orders.az@gmail.com","password":"definitely-wrong"}'
# Expect HTTP 401 Invalid login or password
```

### Civic will consume

- `POST /auth/openstreetmap` treats **409** as `needs_email` + pending token
  (email-only complete step — **not** password).
- `POST /auth/login` surfaces **401** for bad credentials.
- Civic may ship a temporary workaround that also treats a 500 whose message
  contains the verified-email text as `needs_email`; remove that once this is
  deployed on prod.
