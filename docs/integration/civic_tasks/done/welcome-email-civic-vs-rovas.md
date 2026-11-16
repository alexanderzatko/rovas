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
use the same Rules component `rules_send_welcome_email`. After Rovas email
confirm, the on-site status message uses the account language (not anonymous
UI language from the link).

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
- On-site (email confirm): `rovas_site_rules_show_activation_welcome_message()`
  from `rules_show_welcome_text_on_user_activation` — `t(..., langcode =
  account language)`; bold linked “rules page”

### Checklist

1. [x] Stamp `rovas_signup_source` on Civic create paths
2. [x] Branch welcome mail; new EN source + SK/CS/IT
3. [x] Activation status message localized + new copy
4. [ ] Deploy rovdev + re-import welcome Rules + import
      `*.custom-modules.po` + cache clear (human)
5. [ ] Smoke: Civic native create vs Rovas `/user/register` → DevelMailLog
      bodies differ; Rovas email-confirm → SK (etc.) on-site message

### Smoke test

```bash
# After deploy on rovdev (DevelMailLog):
# 1) Civic: POST /rovas/civic_auth_session with a new email → mail body
#    starts with Civic congratulations + Rovas account note
# 2) Rovas: register a new user on /user/register → NEO Rules link body
# 3) Confirm email (pass-reset link) with account language sk → status
#    message “Vitajte v Rováši…” with bold rules-page link
# drush74 sqlq "SELECT uid, data FROM users WHERE mail = '…'"  # look for rovas_signup_source
```

### Civic will consume

No API change. Existing create/register paths only.
