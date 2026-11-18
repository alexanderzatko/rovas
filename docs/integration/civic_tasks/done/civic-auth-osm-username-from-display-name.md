---
status: done
slug: civic-auth-osm-username-from-display-name
owner_repo: rovas
created: 2026-11-18
updated: 2026-11-18
---

# Civic auth: OSM display name → username, not real name

**Product:** OpenStreetMap’s `display_name` is an OSM **username**, not a
legal/real name. On Civic OSM signup, that value currently fills Drupal
`field_full_name` (Civic Profile “real name”) while the account `name` is
derived from the email local-part. Product wants the opposite for
**new** OpenStreetMap accounts:

- Drupal **username** (`name`) ← sanitized OSM `display_name` when present
- **`field_full_name`** ← leave empty (user can set real name later in Profile)

Google / Apple / Facebook / Nabezky keep today’s rule: non-empty
`display_name` → `field_full_name`; username from email local-part.

**Owner:** `rovas`

**Do not change:** HybridAuth lookup / email-link paths for existing users
(do not rename an already-linked account). Password register/login. Other
IdPs’ `display_name` → real-name behaviour
([civic-auth-display-name-idp-only](../done/civic-auth-display-name-idp-only.md)).

### Contract

Amend **create** branch of `POST /rovas/civic_auth_session` when
`provider` is **`openstreetmap`** only:

| Input | Drupal username (`name`) | `field_full_name` |
|-------|--------------------------|-------------------|
| `display_name` non-empty | Sanitize + uniquify like `rovas_oauth_civic_auth_username_from_email` (same charset / length / `_N` collision rules), using OSM display name as the seed instead of email local-part | **do not set** (leave empty) |
| `display_name` missing / null / `""` | Unchanged: email local-part via `rovas_oauth_civic_auth_username_from_email` | do not set |

**Email-link / HybridAuth hit paths:** unchanged — do not overwrite username
or `field_full_name` from OSM `display_name`.

**Request body shape:** unchanged. Civic continues to send OSM’s
`display_name` in the existing field; only Rovas create semantics for
`openstreetmap` change.

Update OpenAPI `CivicAuthSessionRequest.display_name` description to note
the OSM exception.

### Checklist

1. [x] In `rovas_oauth.auth_session.inc` create branch: if
   `$provider_key === 'openstreetmap'` and `$display_name !== ''`, build
   username from `$display_name` (shared sanitizer / collision helper with
   email path); skip `field_full_name` edit entirely for OSM create
2. [x] Extract or reuse sanitizer so email and OSM seeds share one
   uniquify loop (avoid divergent username rules vs Profile /
   `civic_update_profile`)
3. [x] OpenAPI / Services description note for OSM
4. [ ] Deploy **prod** `/var/www/rovas` as well as rovdev — Civic API uses
   `https://rovas.app` (human deploy unless asked)
5. [ ] Smoke (below)

### Smoke test

```bash
API_KEY='…'
TOKEN='…'

# Fresh OSM subject + collected email + OSM-style display_name
curl -sS -X POST "https://rovas.app/rovas/civic_auth_session" \
  -H "Accept: application/json" \
  -H "Content-Type: application/json" \
  -H "API-KEY: $API_KEY" \
  -H "TOKEN: $TOKEN" \
  -d '{
    "provider": "openstreetmap",
    "provider_subject": "civic-smoke-osm-uname-001",
    "email": "civic-smoke-osm-uname-001@example.com",
    "email_verified": false,
    "display_name": "MapMapper42",
    "user_language": "sk",
    "user_country": "SK"
  }'
# Expect 200; then with minted Bearer:
# GET /rovas/me → username "MapMapper42" (or MapMapper42_N if taken);
# field_full_name empty so Profile real-name is blank
# (visibleDisplayName may equal username — that is fine)

# Control: google create with display_name still sets real name
# (username from email local-part) — behaviour unchanged
```

### Civic will consume

No Civic API shape change. Expo already sends OSM `display_name` on
`POST /auth/openstreetmap/complete` (and on mint-first begin when applicable).
After this ships on prod, new OSM signups show OSM name as `@username` and an
empty real-name field. Companion: `~/Documents/homeProjects/civic/docs/rovas-integration.md`.
