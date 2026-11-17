---
status: done
slug: civic-auth-display-name-idp-only
owner_repo: rovas
created: 2026-11-17
updated: 2026-11-17
---

# Civic auth: real name only from IdP

**Product:** On native Google/Apple signup, Civic only sends `display_name`
when the IdP actually provides a human name (Google `name`, Apple full name).
Rovas previously fell back to the **email local-part** when `display_name` was
empty/null, and that value was written to `field_full_name`. Civic then showed
it as “real name” on Profile — users did not choose it.

**Fix:** Never invent a real name from email. Set `field_full_name` only when
Civic sends a non-empty `display_name`. Leave the field empty otherwise
(Profile real-name input stays blank until the user fills it).

**Owner:** `rovas`

### Contract

Amend `POST /rovas/civic_auth_session` behavior (create path):

| `display_name` from Civic | Drupal `field_full_name` |
|---------------------------|--------------------------|
| non-empty string | set to that value |
| missing, `null`, or `""` | **do not set** (leave empty) |

**Removed** the fallback in `rovas_oauth.auth_session.inc`:

```php
if ($display_name === '' && $email !== '') {
  $display_name = preg_replace('/@.*/', '', $email);
}
```

Username generation from email local-part (`rovas_oauth_civic_auth_username_from_email`) is **unchanged**.

HybridAuth identity `username` metadata may still use `$account->name` when
no IdP display name is present — that must **not** write `field_full_name`.

Updated the done task note in
[`civic-native-auth-bridge`](civic-native-auth-bridge.md) (display_name
row: drop “Fallback: email local-part”).

### Checklist

1. [x] Remove email local-part → `display_name` fallback on create
2. [x] Only set `field_full_name` when request `display_name` is non-empty
3. [x] Confirm HybridAuth attach does not populate `field_full_name` from email
4. [ ] Deploy rovdev + smoke (human unless asked)

### Smoke test

```bash
# Create (or use a fresh IdP subject) with display_name omitted / null:
curl -sS -X POST https://dev.rovas.app/rovas/civic_auth_session \
  -H "API-KEY: …" -H "TOKEN: …" -H "Content-Type: application/json" \
  -d '{"provider":"google","provider_subject":"…","email":"…","email_verified":true,"user_language":"sk","user_country":"SK"}'

# Then as the minted Bearer:
curl -sS -H "Authorization: Bearer …" https://dev.rovas.app/rovas/me
# → username from email local-part; display_name equals username (me fallback)
#    and field_full_name empty so Civic Profile real-name field is blank
#    (visibleDisplayName hides name-equal-to-username)
```

### Civic will consume

No API shape change. Profile real-name field stays empty unless Google/Apple
provided a name or the user sets it via `POST /rovas/civic_update_profile`.
