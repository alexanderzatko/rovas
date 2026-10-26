---
status: done
slug: oauth-authorize-intent
owner_repo: both
created: 2026-10-03
updated: 2026-10-03
---

# OAuth authorize intent (login vs register)

**Product:** Civic has separate Sign in and Create account screens. Both open
the same Rovas `/oauth2/authorize` URL today, so anonymous users always land on
`/user/login` (with a Create account tab). New users who tapped Create account
should open the Civic-branded **register** form first.

**Owner:** `both` — Expo already sends `intent`; Rovas must honor it.

**Do not change:**

- OAuth Authorization Code + PKCE contract (`client_id=civic`, scope
  `user_profile`, redirect `civic://oauth/callback`)
- Civic-branded chrome detection (`rovas_oauth_is_civic_authorize_context()`)
- Locale stash (`lang` / `ui_locales` — see historical
  [pass Civic locale on OAuth authorize](../done/_historical-pre-split.md#next-work-pass-civic-locale-on-oauth-authorize))
- HybridAuth providers / destination survival / automatic authorize after login
- Token lifetimes / refresh rotation

### Contract

Civic appends `intent` on the authorize request (alongside existing `lang`):

```
GET /oauth2/authorize?response_type=code&client_id=civic&…&lang=sk&intent=register
```

| Param | Values | Notes |
|-------|--------|--------|
| `intent` | `login` \| `register` | Which anonymous form to present first. Missing / invalid → `login` (today’s behavior). Never 400 authorize. |

`oauth2_server` always `drupal_goto('user/login', …)` for anonymous users
(`oauth2_server.pages.inc`). Rovas must rewrite that goto to `user/register`
when session intent is `register` (via existing `hook_drupal_goto_alter` in
`rovas_oauth`, which already attaches `lang=`).

### Rovas approach (`rovas_oauth`)

1. On `/oauth2/authorize` for `client_id=civic`, read `intent` from `$_GET`
   (and from `$_SESSION['oauth2_server_authorize']` after login return if
   present). Accept only `login` / `register`; stash
   `$_SESSION['rovas_oauth_civic_intent']`.
2. In `rovas_oauth_drupal_goto_alter()`: when `$path === 'user/login'` and
   session intent is `register`, set `$path = 'user/register'`. Keep
   `destination=oauth2/authorize` only — do **not** echo `intent=` / `lang=`
   as sibling query params (they get absorbed into destination after password
   login → `/oauth2/authorize%26intent%3D…` 404). Session remains SoT.
3. Clear the session key when leaving Civic authorize context (same lifetime
   rules as `ROVAS_OAUTH_CIVIC_UI_LANG_SESSION` — no leak to plain
   `/user/login`).
4. Tabs / in-page switches between login ↔ register remain available; intent
   only chooses the **first** landing page.

### Checklist

1. [x] Capture `intent` on `/oauth2/authorize` for client `civic`; stash in session
2. [x] Rewrite authorize → anonymous goto from `user/login` to `user/register` when intent is `register`
3. [x] Missing / invalid `intent` keeps today’s `/user/login` landing
4. [x] Session clears outside Civic authorize context
5. [x] Invalid `intent` does not 400 the authorize endpoint
6. [x] Deploy `rovas_oauth` to rovdev + smoke (human unless asked)

### Smoke test

```bash
# 1) Register intent → land on Create account (Civic chrome), destination preserved
open 'https://dev.rovas.app/oauth2/authorize?response_type=code&client_id=civic&redirect_uri=civic%3A%2F%2Foauth%2Fcallback&scope=user_profile&state=intentreg&code_challenge=x&code_challenge_method=S256&lang=sk&intent=register'
# Expect: /user/register?destination=oauth2/authorize… (Civic-branded register)

# 2) Login intent (or omit) → land on Log in
open 'https://dev.rovas.app/oauth2/authorize?response_type=code&client_id=civic&redirect_uri=civic%3A%2F%2Foauth%2Fcallback&scope=user_profile&state=intentlog&code_challenge=x&code_challenge_method=S256&lang=sk&intent=login'
# Expect: /user/login?destination=oauth2/authorize…

# 3) From Civic mobile: Create account → browser opens register; Sign in → login.
#    Complete OAuth either way → app signed in.
```

**Response shape:** unchanged OAuth token JSON.

### Civic will consume

Expo `AuthForm` passes `mode` (`login` | `register`) into `signIn(intent)`,
which sets `AuthRequest.extraParams.intent` next to `lang`. No Civic API
change. Until Rovas deploys, extra param is ignored and landing stays login.
