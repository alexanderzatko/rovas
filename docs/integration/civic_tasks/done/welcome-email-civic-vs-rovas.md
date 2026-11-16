---
status: done
slug: welcome-email-civic-vs-rovas
owner_repo: rovas
created: 2026-11-16
updated: 2026-11-16
---

# Welcome email: Civic vs Rovas copy

**Product:** New accounts created via Civic get a Civic-specific welcome
email; accounts created on Rovas (web / partners) get the Rovas welcome. Both
use the same Rules component `rules_send_welcome_email`.

**Owner:** `rovas`

### Contract

- Durable marker: `$user->data['rovas_signup_source'] = 'civic'` set only at
  **create** time by:
  - `POST /rovas/civic_auth_session` (native Google/Apple)
  - Civic OAuth authorize register / HybridAuth
    (`rovas_oauth_user_presave` when Civic authorize context)
- Absent / other → Rovas welcome copy
- Mail: `rovas_site_rules_send_welcome_email()` via Rules component; subjects
  `Welcome to Civic` / `Welcome to Rovas`; bodies in `t()` with translations in
  `translations/po/{sk,cs,it,en}.custom-modules.po`

### Checklist

1. [x] Stamp `rovas_signup_source` on Civic create paths
2. [x] Branch welcome mail; new EN source + SK/CS/IT
3. [ ] Deploy rovdev + re-import `rules_send_welcome_email` + import
      `*.custom-modules.po` + cache clear (human)
4. [ ] Smoke: Civic native create vs Rovas `/user/register` → DevelMailLog
      bodies differ

### Smoke test

```bash
# After deploy on rovdev (DevelMailLog):
# 1) Civic: POST /rovas/civic_auth_session with a new email → mail body
#    starts with Civic congratulations + Rovas account note
# 2) Rovas: register a new user on /user/register → NEO Rules link body
# drush74 sqlq "SELECT uid, data FROM users WHERE mail = '…'"  # look for rovas_signup_source
```

### Civic will consume

No API change. Existing create/register paths only.
