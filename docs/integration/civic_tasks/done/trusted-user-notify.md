---
status: done
slug: trusted-user-notify
owner_repo: both
created: 2026-11-01
updated: 2026-11-01
---

# Trusted user dismissable notification (Civic)

**Product:** When a Rovas account becomes a **Trusted user**, the Rovas web UI
shows a site-wide dismissable congratulations message (`field_trusted_notify =
1`). Civic should show the same notice in-app (header banner alongside
verification / comment notices) and honor the same permanent dismiss so web and
app stay in sync.

Existing Rovas behavior (`rovas_user`): grant sets `field_trusted_user = 1` and
`field_trusted_notify = 1`; banner shows while notify is `1`;
`GET /dismiss_trusted_notify` sets notify to `2`. Civic cannot see any of this
today — `GET /rovas/me` omits trusted fields, and there is no Bearer dismiss.

This task is **independent of** the deferred Slovak eID Trusted path
([`trusted-user-eid.md`](../../../trusted-user-eid.md)). Expose the sticky
Trusted flag + notify state that already exist for compliance / reward
eligibility. Do not wait for eID.

**Owner:** `both` — Rovas extends `/rovas/me` + Bearer dismiss; Civic proxies
and builds the banner.

### Contract

#### 1. Notify state (existing field)

| Value | Meaning |
|-------|---------|
| `0` | Default / after Trusted revoke — do **not** show banner |
| `1` | Show congratulations banner (just granted, not yet dismissed) |
| `2` | Permanently dismissed — do **not** show banner |

Store remains `field_trusted_notify` on the user. Web dismiss
(`/dismiss_trusted_notify`) and the Civic dismiss below must write the **same**
value (`2`).

#### 2. `GET /rovas/me` (extend)

Add:

| Field | Type | Notes |
|-------|------|--------|
| `trusted_user` | `boolean` | `true` when `field_trusted_user == 1` |
| `trusted_notify` | `0` \| `1` \| `2` | Exact notify field; Civic shows banner only when `trusted_user === true` **and** `trusted_notify === 1` |

Always include both fields for signed-in Bearer callers (do not omit when
false/`0`).

#### 3. `POST /rovas/civic_dismiss_trusted_notify`

| | |
|--|--|
| Method | `POST` |
| Path | `/rovas/civic_dismiss_trusted_notify` |
| Auth | Bearer (OAuth2 `civic` client); API-key auth **disabled** |
| Content-Type | `application/json` (empty body OK) |

**Response (200):**

```json
{
  "trusted_user": true,
  "trusted_notify": 2
}
```

| Field | Notes |
|-------|--------|
| `trusted_user` | Current sticky Trusted flag after the call |
| `trusted_notify` | Echo of stored value (`2` when dismiss applied) |

**Side effects:**

- If the Bearer user is Trusted and `field_trusted_notify == 1`, set it to `2`
  and `user_save` (same as `rovas_user_dismiss_trusted_notify`).
- Idempotent: already `2` (or `0`) → still `200` with current values; no error.
- Do **not** clear `field_trusted_user` or Trusted prefs
  (`field_trusted_auto_approve`, `field_trusted_verify_level`).

**Errors:** `401` missing/invalid Bearer.

Reuse `rovas_user_dismiss_trusted_notify` logic (shared helper preferred) so web
session dismiss and Civic Bearer dismiss cannot drift.

#### 4. Out of scope (this task)

- Exposing or editing Trusted prefs (`auto_approve`, verify level) in Civic —
  banner body may deep-link to Rovas account settings
  (`/user/{uid}/edit#edit-trusted-user-settings`) until a later prefs task.
- Slovak eID Trusted eligibility (`trusted-user-eid.md`).
- Push / email — web banner + in-app poll of `/me` only.

### Checklist

1. [x] Extend `rovas_oauth_me_index()` (or equivalent) with `trusted_user` + `trusted_notify`
2. [x] Services resource `POST /rovas/civic_dismiss_trusted_notify` (Bearer only; enable in Civic resources helper / update hook)
3. [x] Shared dismiss helper used by web menu callback + Civic resource
4. [x] Deploy rovdev + smoke (human unless asked)

### Smoke test

```bash
TOKEN=…  # OAuth access token for a Trusted test user with notify=1 on dev.rovas.app

curl -sS -H "Authorization: Bearer $TOKEN" -H "Accept: application/json" \
  https://dev.rovas.app/rovas/me
# → "trusted_user": true, "trusted_notify": 1

curl -sS -X POST https://dev.rovas.app/rovas/civic_dismiss_trusted_notify \
  -H "Authorization: Bearer $TOKEN" -H "Content-Type: application/json" \
  -d '{}'
# → {"trusted_user":true,"trusted_notify":2}

curl -sS -H "Authorization: Bearer $TOKEN" -H "Accept: application/json" \
  https://dev.rovas.app/rovas/me
# → trusted_notify: 2

# Idempotent second dismiss
curl -sS -X POST https://dev.rovas.app/rovas/civic_dismiss_trusted_notify \
  -H "Authorization: Bearer $TOKEN" -H "Content-Type: application/json" \
  -d '{}'
# → still trusted_notify: 2
```

(Non-Trusted user: `trusted_user: false`, `trusted_notify: 0` or `2`; dismiss
returns current values without error.)

### Civic will consume

Civic side **implemented** (awaits rovdev deploy of `7031` for E2E):

- Map `trusted_user` / `trusted_notify` on Civic `GET /me` (`MeProfile`).
- Proxy `POST /me/dismiss-trusted-notify` → Rovas dismiss.
- Header banner (above verification / comment notices) when
  `trusted_user && trusted_notify === 1`.
- Copy aligned with Rovas EN/SK site banner (Trusted user / dôveryhodný
  používateľ terminology). Dismiss calls the API (not session-only SecureStore).
- Settings CTA: open Rovas account edit Trusted settings fragment in the system
  browser until in-app Trusted prefs exist.
