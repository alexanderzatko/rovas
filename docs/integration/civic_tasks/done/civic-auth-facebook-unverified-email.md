---
status: done
slug: civic-auth-facebook-unverified-email
owner_repo: rovas
created: 2026-11-18
updated: 2026-11-18
---

# Civic auth: allow unverified email for Facebook (in-app collect)

> Done — `facebook` may pass `email_verified: false` like OSM when Civic
> collected email in-app. Deploy + smoke left to human.

**Product:** Meta often grants the Facebook Login `email` permission but
**omits** `email` from Graph `/me` (and Limited Login JWTs) for some accounts.
Civic already collects email in-app for OpenStreetMap and now does the same
when Facebook returns no address. Completing that flow calls
`POST /rovas/civic_auth_session` with `provider: "facebook"` and
`email_verified: false`.

Previously only **`openstreetmap`** could use `email_verified: false`. Facebook
required a verified email on create/link, so the Civic complete step failed
with 409 until this exception was extended.

**Owner:** `rovas`

**Do not change:** Google / Apple / Nabezky verified-email rules; HybridAuth
lookup order; token mint shape.

### Contract

**`POST /rovas/civic_auth_session`** (unchanged path / auth)

| Field | Change |
|-------|--------|
| `provider` | Unchanged allowlist |
| `email` / `email_verified` | When `provider` is **`facebook`** *or* **`openstreetmap`**, require a valid `email` string but **allow `email_verified: false`**. Other providers unchanged (still require verified email when no HybridAuth hit). |

Empty email + no HybridAuth hit → still **409** (Civic uses that to decide
whether to show the email form after Facebook OAuth).

### Checklist

1. [x] Extend the `email_verified: false` exception from `openstreetmap` only to **`facebook` \| `openstreetmap`**
2. [x] OpenAPI / Services description if it documents the OSM-only rule
3. [ ] Deploy rovdev + smoke (human unless asked)

### Smoke test

```bash
API_KEY='…'
TOKEN='…'

# Facebook — collected (unverified) email, new subject
curl -sS -X POST "https://dev.rovas.app/rovas/civic_auth_session" \
  -H "Accept: application/json" \
  -H "Content-Type: application/json" \
  -H "API-KEY: $API_KEY" \
  -H "TOKEN: $TOKEN" \
  -d '{
    "provider": "facebook",
    "provider_subject": "smoke-fb-email-001",
    "email": "civic-smoke-fb-email@example.com",
    "email_verified": false,
    "display_name": "FB Smoke",
    "user_language": "sk",
    "user_country": "SK"
  }'
# Expect 200 with access_token / refresh_token (not 409 for unverified email)

# Facebook — empty email, unknown subject → still 409
curl -sS -X POST "https://dev.rovas.app/rovas/civic_auth_session" \
  -H "Accept: application/json" \
  -H "Content-Type: application/json" \
  -H "API-KEY: $API_KEY" \
  -H "TOKEN: $TOKEN" \
  -d '{
    "provider": "facebook",
    "provider_subject": "smoke-fb-no-email-001",
    "email": "",
    "email_verified": false,
    "display_name": "FB No Email",
    "user_language": "sk",
    "user_country": "SK"
  }'
# Expect HTTP 409
```

### Civic will consume

1. `POST /auth/facebook` — mint if Meta email present or HybridAuth hit; else
   `needs_email` + pending JWT.
2. `POST /auth/facebook/complete` — pending JWT + user email → this bridge with
   `email_verified: false`.
