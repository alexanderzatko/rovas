---
status: done
slug: update-profile
owner_repo: both
created: 2026-11-17
updated: 2026-11-17
---

# Update username and real name (Civic Profile)

**Product:** After Google/Apple signup, Rovas auto-generates a Drupal username
from the email local-part and may set `field_full_name` from the IdP. Civic’s
Profile **User profile** tab lets the signed-in user change that username and
optionally set or clear their real name (`display_name` / `field_full_name`).

**Owner:** `both` — Rovas owns the Bearer write endpoint and Drupal user update;
Civic proxies `POST /me/profile` and builds the Profile form.

### Contract

#### 1. Fields

| Civic / API field | Drupal | Notes |
|-------------------|--------|--------|
| `username` | `$user->name` | Login handle; unique site-wide |
| `display_name` | `field_full_name` | Human / real name; optional |

#### 2. Username rules

Match `rovas_oauth_civic_auth_username_from_email` sanitizer:

- Allowed charset: `[A-Za-z0-9_.-]`
- Length: 1–50
- Must be unique among Drupal users (case rules same as Drupal username uniqueness)
- No-op when equal to the caller’s current username

#### 3. Display name rules

- Optional string, max 200 characters after trim
- `null` or `""` clears `field_full_name`
- Omitting `display_name` leaves the field unchanged

#### 4. `POST /rovas/civic_update_profile`

| | |
|--|--|
| Method | `POST` |
| Path | `/rovas/civic_update_profile` |
| Auth | Bearer (OAuth2 `civic` client); API-key auth **disabled** |
| Content-Type | `application/json` |

**Body:** at least one of:

```json
{
  "username": "jane_doe",
  "display_name": "Jane Doe"
}
```

| Field | Type | Notes |
|-------|------|--------|
| `username` | string | Optional; when present must pass rules above |
| `display_name` | string \| null | Optional; `null` / `""` clears |

**Response (200):**

```json
{
  "username": "jane_doe",
  "display_name": "Jane Doe"
}
```

`display_name` in the response should match `/rovas/me`: the stored full name
when set, otherwise the username (same fallback as today).

**Errors:**

| Status | When |
|--------|------|
| `401` | Missing/invalid Bearer |
| `422` | Invalid charset/length, empty body (neither field), or other validation |
| `409` | Username already taken by another account |

Register the resource in `rovas_oauth_ensure_civic_resources()` (or equivalent)
with API-key auth disabled, same pattern as `civic_me_sources_preference`.

### Checklist

1. [x] Services resource `POST /rovas/civic_update_profile` (Bearer only)
2. [x] Validate username charset/length; uniqueness → `409`
3. [x] Update `$user->name` and/or `field_full_name`; clear on null/`""`
4. [x] Response shape `{ username, display_name }` aligned with `/rovas/me`
5. [ ] Deploy rovdev + smoke (human unless asked)

### Smoke test

```bash
TOKEN=…  # Civic Bearer for a test user

curl -sS -X POST https://dev.rovas.app/rovas/civic_update_profile \
  -H "Authorization: Bearer $TOKEN" -H "Content-Type: application/json" \
  -d '{"username":"civic_smoke_user","display_name":"Smoke Test"}'
# → {"username":"civic_smoke_user","display_name":"Smoke Test"}

curl -sS -H "Authorization: Bearer $TOKEN" -H "Accept: application/json" \
  https://dev.rovas.app/rovas/me
# → username / display_name match

curl -sS -o /dev/null -w "%{http_code}\n" -X POST \
  https://dev.rovas.app/rovas/civic_update_profile \
  -H "Authorization: Bearer $TOKEN" -H "Content-Type: application/json" \
  -d '{"username":"!!!"}'
# → 422

# Second user’s token + first user’s username → 409
```

### Civic will consume

- Proxy `POST /me/profile` → this endpoint.
- Profile **User profile** tab: editable username + optional real name; Save
  maps `409` / `422` to locale alerts.
