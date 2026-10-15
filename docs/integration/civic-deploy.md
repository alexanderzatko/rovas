# Civic stack — deploy to a production clone

Operational checklist for enabling the Civic-era Rovas code (OAuth, Civic APIs,
Merit ledger sources, updated site Rules) on a **clone of production**.

API contracts and feature notes live in [`civic.md`](civic.md). Civic app /
API env notes: `~/Documents/homeProjects/civic/docs/rovas-integration.md`.

**Default target:** `/var/www/rovdev` → `https://dev.rovas.app`  
**Real prod:** `/var/www/rovas` → `https://rovas.app` — same Drupal steps, different
deploy script and Civic base URL. Do **not** run this against live prod unless
you intentionally mean to.

`ROVAS_BASE_URL` / `EXPO_PUBLIC_ROVAS_BASE_URL` are **Civic** env vars (API +
mobile), not Drupal. Point them at the clone under test.

---

## What you are enabling


| Layer | Modules / artifacts |
| --- | --- |
| OAuth / PKCE | `xautoload`, `libraries`, `oauth2_server` (+ PKCE patch), library `oauth2-server-php` **v1.14.2**, `rovas_oauth` |
| Civic APIs | `rovas_civic` (depends on OAuth + `rovas_merit`) |
| Merit ledger / sources | `rovas_merit` (`field_merit_source`, reconcile / backfill Drush) |
| Home Merit reads | `rovas_merit_distribution` (usually already on; needed for `/rovas/me`, aggregate, histogram) |
| Optional AI NACE | `rovas_ai` + OpenAI-compatible API key |
| Rules | Updated JSON under `rovas_site_rules` (purchase / recipient order + Merit-source rules) |

---

## 0. Preconditions

1. Deployed tree includes Civic-era modules, `sites/all/libraries/oauth2-server-php`
   **1.14.2**, and the PKCE patch on `oauth2_server`
   (`patches/oauth2_server-3015065-pkce-support-7.patch`).
2. Clone is a DB copy of prod (or equivalent): HybridAuth and Services
   `rovas_endpoint` already exist. OAuth Civic server/client and Civic DB tables
   are created on module enable / `updb`.
3. On **rovdev**, keep outgoing mail on **DevelMailLog** so a prod DB restore
   does not send real mail ([`docs/dev-setup.md`](../dev-setup.md)).
4. Prefer **`drush74`** (not plain `drush`). Prefer enabling modules via Drush
   over the Modules UI (UI can flash a one-off `service_container` / OpenLayers
   race; modules still install — see [`civic.md`](civic.md#enable-on-devrovasapp)).

---

## 1. Deploy code

Usual flow on the VPS:

```bash
cd /var/www/rovdev/
deploy-rovas
```

From a laptop (same effect as `ops/deploy_rovdev.sh`):

```bash
# from the Rovas repo root
ops/deploy_rovdev.sh <ssh_target>   # → /var/www/rovdev
# prod only when intentional:
# ops/deploy_magura.sh <ssh_target>  # → /var/www/rovas
```

`deploy_rovdev.sh` / `deploy_magura.sh` rsync the tree (excluding
`settings.php` and files), then run `drush74 updb -y && drush74 cc all -y`.
You can still open browser **update.php** afterward as a second pass.

After deploy, also re-import site Rules (step 7) — rsync alone does not update
overridden Rules in the DB.

---

## 2. Schema / updates

```bash
cd /var/www/rovdev
drush74 updb -y
# and/or open update.php in the browser
drush74 cc all -y
```

Hooks that matter on first enable / later deploys:

- `rovas_oauth_update_7001` — home Civic Services resources
- `rovas_merit_update_7001` / `7002` — create `field_merit_source` + batched backfill
- `rovas_civic_update_7001`–`7016` — Civic tables, module weight **21**, Services resources
- `rovas_ai_update_7001` — if enabling AI

---

## 3. Enable modules (order matters)

```bash
cd /var/www/rovdev

# OAuth stack
drush74 en xautoload libraries oauth2_server rovas_oauth -y

# Merit ledger (required by rovas_civic and by updated site rules)
drush74 en rovas_merit -y

# Civic APIs
drush74 en rovas_civic -y

# Optional but needed for in-app AI NACE
drush74 en rovas_ai -y

# Confirm already enabled (home Merit JSON)
drush74 pm-info rovas_merit_distribution

drush74 updb -y
drush74 cc all -y
```

On enable, `rovas_oauth` bootstraps OAuth server/client `civic` and tries to wire
Services. `rovas_civic` bootstraps State Treasury + registers Civic resources.
`rovas_civic` weight is set to **21** so `hook_node_insert` runs after Rules
(needed for CHR→ME on WR verification).

---

## 4. OAuth permissions + bootstrap

```bash
drush74 role-add-perm 'anonymous user' 'use oauth2 server'
drush74 role-add-perm 'authenticated user' 'use oauth2 server'
```

Confirm / re-run bootstrap:

- Admin: **Configuration → Web services → Rovas OAuth (Civic)**  
  (`/admin/config/services/rovas-oauth`)
- **Bootstrap Civic OAuth server + client**
- **Enable Civic home API resources**

Expect: server machine name `civic`, PKCE on, grants `authorization_code` +
`refresh_token`, client id `civic` (no secret), redirect `civic://oauth/callback`,
default scope `user_profile`.

Idempotent from CLI:

```bash
drush74 php-eval "rovas_oauth_ensure_server(); rovas_oauth_ensure_client(); rovas_oauth_ensure_civic_resources();"
```

### Session persistence (refresh tokens)

`rovas_oauth_ensure_server()` must leave the `civic` server with:

| Setting | Value | Why |
| --- | --- | --- |
| `always_issue_new_refresh_token` | **TRUE** | Otherwise mobile cannot refresh after the first rotation and users are logged out after ~1h |
| `auth_code_lifetime` | **≥ 120** seconds | Default 30s caused “authorization code has expired” on slow re-login |
| `access_lifetime` | 3600 | Access token TTL |
| `refresh_token_lifetime` | 1209600 | 14 days |

Re-run `rovas_oauth_ensure_server()` on every new clone (already applied on
rovdev 2026-09-02). Smoke: refresh grant must return a **new** `refresh_token`;
the old one must fail (`invalid_grant`). Details:
[`civic.md` — OAuth session persistence](civic.md#next-work-oauth-session-persistence).

---

## 5. Services OAuth2 + Civic resources

**Structure → Services → `rovas_endpoint` → Authentication**

- Enable **OAuth2 authentication**, server machine name **`civic`**.
- Settings must be the array form `oauth2_server => array('server' => 'civic')`,
  not a bare checkbox string (Bearer auth fails otherwise).

Register / sync all Civic resources (idempotent; also covers home `/me`):

```bash
drush74 php-eval "rovas_civic_ensure_services_resources();"
# if AI enabled:
drush74 php-eval "rovas_ai_ensure_services_resources();"
drush74 cc all -y
```

Or admin buttons:

- Rovas OAuth form → **Enable Civic home API resources**
- **Configuration → Rovas Civic** → **Enable Civic VAT claim + claim-read + rate + users + project APIs**
- **Configuration → Rovas → AI** → **Enable Civic AI APIs**

There is no Features export of `rovas_endpoint` (DB-only).

### Resources that must be enabled

API-key auth **disabled** on these; Bearer `user_profile` where required.

**Home (`rovas_oauth`):** `me`, `contributions` (aggregate), `merit_distribution`

**Civic (`rovas_civic`):**

- `rules` → action `rules_proxy_claim_vat_receipt`
- `civic_claims`, `civic_national_hourly_rate`, `users_search`, `users_lookup`
- `civic_project`, `civic_project_work_reports`, `civic_project_shareholders`
- `civic_merit_history`, `civic_my_projects`, `civic_projects_search`, `civic_nace_tree`
- `civic_create_project`, `civic_create_work_report`, `civic_work_report_map_config`
- `civic_pending_verifications`, `civic_work_report`, `civic_submit_verification`
- `civic_merit_index`, `civic_merit_index_countries`

**AI (`rovas_ai`):** `civic_ai_nace`

**Public (no Bearer):** national hourly rate, NACE tree, merit index (+ countries),
contributions aggregate, merit_distribution. Most others need Bearer.

---

## 6. Civic admin config

**Configuration → Rovas Civic** (`/admin/config/rovas/civic`)

1. **Bootstrap State Treasury project** (or set an existing project nid).
2. Set a real **SK national hourly rate** (install seed is `7.5` placeholder —
   replace before serious VAT-claim tests). Formula:  
   `ME = (eur_slice / rate) × 10`.

**If using AI:** `/admin/config/rovas/ai` — API key, model, daily budget; enable
AI resources.

**Merit history** (`GET /rovas/civic_merit_history`) uses existing Views
`merit_history` / `page_1`. No new View export if the clone already has it
(prod does).

---

## 7. Re-import site Rules

Purchase / recipient Merit-source updates and other Civic-era rule JSON only
land in the DB after migration (deploy alone does not update overridden Rules):

```bash
cd /var/www/rovdev
drush74 php-eval "module_load_include('inc', 'rovas_site_rules', 'rovas_site_rules.migration'); rovas_site_rules_migration_run();"
drush74 cc all -y
```

If Xdebug nesting blows up on `cc all`:

```bash
php -d xdebug.mode=off $(which drush74) cc all
```

Confirm key rules are **Default** (or intentionally Overridden) in the Rules UI,
especially:

- `rules_process_purchase_core`
- `rules_process_recipient_order_core`
- `rules_create_merit_disbursement`
- `rules_reward_work_report`
- `rules_give_merits_to_user`

See `sites/all/modules/custom/rovas_site_rules/README.txt`.

---

## 8. Merit ledger: backfill sources, then reconcile

`updb` / first enable create `field_merit_source` and backfill **missing** values.
Civic **`by_source`** / Prínos filters and accurate `by_category` need current
inference — including the distinction between:

| Ledger source | Counts toward | Typical origin |
| --- | --- | --- |
| `volunteer_work` | volunteer | Work-report rewards; project-reward ME to **co_worker** shareholders (any hours) |
| `volunteer` | volunteer | CHR→ME purchase |
| `charitable_donation` | charity | EUR donations / EUR merit purchase; purchased **timeshare** ME |
| `tax_payment` | taxes | Civic VAT claims |
| `penalty` | Split across non-zero taxes/charity/volunteer proportionally (`rovas_merit_user_by_category()` + Civic `categoriesFromMeritSources`) | Merit penalties |

So after code that improved source inference, always reclassify:

```bash
# Re-infer sources on all ledger nodes (can take a while on a full prod clone)
drush74 rovas-merit-backfill --reclassify

# Report drift: wallet vs user_expertise vs ledger
drush74 rovas-merit-reconcile

# One-time expertise fixes:
# - clamp bogus negative 94.99 from an old reconcile bug
# - credit missing expertise (e.g. on 64.99) or debit real legacy penalties on 94.99
# Does NOT rewrite legacy ledger-only gaps (those stay report-only)
drush74 rovas-merit-reconcile --fix
```

Optional: `--expertise-only`, `--uid=N`.

**Order:** backfill `--reclassify` → reconcile (report) → reconcile `--fix`.

Without `--reclassify`, already-filled sources stay wrong relative to the latest
inference (e.g. %-only vs earned project shares).

---

## 9. Point Civic at the clone

On the Civic API host and/or local mobile env:

```bash
ROVAS_BASE_URL=https://dev.rovas.app
EXPO_PUBLIC_ROVAS_BASE_URL=https://dev.rovas.app
# EXPO_PUBLIC_ROVAS_OAUTH_CLIENT_ID=civic   # default
```

Restart civic-api / rebuild the Expo app so the new base URL is picked up.

For a **true prod** cutover later, set both to `https://rovas.app` only after the
same Drupal steps ran on `/var/www/rovas`.

---

## 10. Smoke checks (minimal)

Replace host if not rovdev.

```bash
HOST=https://dev.rovas.app

# Public
curl -sS "$HOST/rovas/contributions/aggregate"
curl -sS "$HOST/rovas/civic_national_hourly_rate?country=SK"
curl -sS "$HOST/rovas/civic_merit_index?limit=5"
curl -sS "$HOST/rovas/civic_nace_tree?lang=sk"
# Expect merit_index items: by_category + by_source (when ledger sources exist)

# After PKCE login → TOKEN=
curl -sS "$HOST/rovas/me" -H "Authorization: Bearer $TOKEN"
curl -sS "$HOST/oauth2/tokens/$TOKEN"
curl -sS "$HOST/rovas/users_lookup?username=sano&lang=sk" -H "Authorization: Bearer $TOKEN"
# Expect users_lookup: by_category + by_source; charity/volunteer match table in §8
curl -sS "$HOST/rovas/civic_merit_history?limit=5&lang=sk" -H "Authorization: Bearer $TOKEN"

# Refresh token rotation (session persistence)
curl -sS -X POST "$HOST/oauth2/token" \
  -H "Content-Type: application/x-www-form-urlencoded" \
  -d "grant_type=refresh_token&client_id=civic&refresh_token=PASTE_REFRESH_TOKEN"
# Expect NEW access_token AND refresh_token; old refresh_token must fail
```

Also: authorize with PKCE (see [`civic.md`](civic.md#pkce-smoke-test-curl)),
confirm HybridAuth login on `/user/login?destination=oauth2/authorize`.

Optional weight check if CHR→ME on verification misbehaves:

```bash
drush74 php-eval "print db_query(\"SELECT weight FROM {system} WHERE name='rovas_civic'\")->fetchField();"
# Expect: 21
```

---

## Ordered checklist

1. [ ] Deploy code to clone (`deploy-rovas` / `ops/deploy_rovdev.sh`)
2. [ ] `drush74 updb -y` (+ browser update.php if desired)
3. [ ] Enable: `xautoload libraries oauth2_server rovas_oauth rovas_merit rovas_civic` (+ `rovas_ai`)
4. [ ] OAuth perms + bootstrap server/client `civic` (`rovas_oauth_ensure_server()` — refresh token settings)
5. [ ] Services: OAuth2 on `rovas_endpoint`, server=`civic`
6. [ ] `rovas_civic_ensure_services_resources()` (+ AI ensure if needed)
7. [ ] State Treasury + SK hourly rate (+ AI key)
8. [ ] `rovas_site_rules_migration_run()`
9. [ ] `drush74 rovas-merit-backfill --reclassify`
10. [ ] `drush74 rovas-merit-reconcile` then `--fix`
11. [ ] `drush74 cc all -y`
12. [ ] Set Civic `ROVAS_BASE_URL` / `EXPO_PUBLIC_ROVAS_BASE_URL` → clone
13. [ ] Smoke: OAuth PKCE + refresh, `/rovas/me`, public merit index `by_source`, one Bearer Civic call

---

## Production follow-up (off-peak)

On a live Magura cutover, skip `--reclassify` during the short enable window
(it can run 30–60+ minutes on a full ledger). Run later off-peak — **no
maintenance mode required**:

```bash
# On Magura
cd /var/www/rovas
nohup drush74 rovas-merit-backfill --reclassify > /tmp/merit-reclassify-prod.log 2>&1 &

# Watch progress
tail -f /tmp/merit-reclassify-prod.log

# After it finishes (look for "Backfilled field_merit_source on N ledger node(s)"):
drush74 rovas-merit-reconcile
drush74 rovas-merit-reconcile --fix
drush74 cc all -y
```

Batch size is **500**; the do-while `#finished` bug is fixed in tree. Expect
sustained CPU/DB load — prefer quiet hours. Until this runs, Civic
`by_source` may still show mostly `legacy` for older ledger rows.

For a true prod Civic point after Drupal enable:

```bash
# /opt/docker/civic-api/.env on VPS
ROVAS_BASE_URL=https://rovas.app
# then: cd /opt/docker/civic-api && docker compose up -d

# Local Civic (API + Expo Metro)
ROVAS_BASE_URL=https://rovas.app
EXPO_PUBLIC_ROVAS_BASE_URL=https://rovas.app
```

Verify: `curl -sS https://civic-api.rovas.app/health` →
`"rovas_base_url":"https://rovas.app"`.

---

## Notes often missed

- **Libraries / PKCE patch** must already be in the tree before OAuth works.
- Enable **`rovas_merit` before `rovas_civic`** and before Rules migration
  (`rovas_site_rules` depends on `rovas_merit`).
- **`rovas_ai`** is separate if you need `POST /rovas/civic_ai_nace`.
- **Services resources** are DB-only; `updb` / ensure helpers / admin buttons are
  the source of truth.
- **Rules migration** is mandatory whenever Merit / purchase JSON changed.
- Backfill **without** `--reclassify` only fills empty sources; improved
  earned vs %-only / timeshare inference needs `--reclassify`.
- Reconcile `--fix` only repairs **expertise** (including 94.99 artifacts);
  ledger-only legacy drift is reported, not rewritten.
- On a prod clone used as **dev**, keep **DevelMailLog**; do not copy that
  override to real prod.
