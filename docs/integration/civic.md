# Civic ↔ Rovas integration

Civic (`~/Documents/homeProjects/civic`) is a Slovak-first Expo app that uses
Rovas as its identity and Merit ledger. This document is the **Rovas-side
index and evergreen source of truth** for OAuth and live Civic APIs.
Per-task specs live under [`civic_tasks/`](civic_tasks/). Civic-side notes
live at `~/Documents/homeProjects/civic/docs/rovas-integration.md` (Expo +
Civic API); keep both in sync when contracts change.

**Dev target (until told otherwise):** all Civic integration work targets
`**/var/www/rovdev**` (`https://dev.rovas.app`). Do **not** change prod
`/var/www/rovas` (`rovas.app`) unless explicitly asked.

**Deployment:** the human deploys unless they ask the agent to. Agents should
leave code + docs ready for `ops/deploy_rovdev.sh` (or a targeted rsync) and
not run deploys on their own. Full enable checklist for a production clone
(modules, OAuth/Services, Rules migration, Merit backfill/reconcile, Civic
env): [`civic-deploy.md`](civic-deploy.md).

## Agent playbook

1. Read **this file** (index + evergreen contracts) and only the relevant
   task under [`civic_tasks/active/`](civic_tasks/active/) (or `@` that file).
   Do not load the historical archive unless you need an old contract.
2. **New work:** copy [`civic_tasks/_template.md`](civic_tasks/_template.md) to
   `civic_tasks/active/<slug>.md`; fill Product / Contract / Checklist / Smoke;
   add a row to [Task directory](#task-directory) below. Do **not** append a
   new `## Next work` section to this file.
3. **Implement:** edit Rovas code + mark checklist boxes in the task file;
   update the [Status](#status) table here when an API becomes live.
4. **Finish:** set frontmatter `status: done`, move the file to
   `civic_tasks/done/`, update the task directory row. Leave deploy/import to
   the human (see deploy-owner rule).
5. Do **not** use Agent Stores or chat history as the source of truth.
   Do not rely on reading the Civic tree unless you need Expo/API details.

## Task directory

| Status | Slug | File | Notes |
|--------|------|------|-------|
| active | penalty-proportional-by-category | [`active/penalty-proportional-by-category.md`](civic_tasks/active/penalty-proportional-by-category.md) | Rovas `by_category` matches Civic proportional penalty; deploy + merit-index snapshot rebuild + smoke pending |
| active | gifts-top-charities-attribution | [`active/gifts-top-charities-attribution.md`](civic_tasks/active/gifts-top-charities-attribution.md) | EUR buy-ME → project 1992 (configurable); `top_charities` += buy-ME + timeshare; deploy/smoke pending |
| done | charity-org-basic-info | [`done/charity-org-basic-info.md`](civic_tasks/done/charity-org-basic-info.md) | `org_id`, `country`, `website_url`, `iban_verified` + ungated `donate_iban` on `civic_project`; smoked on rovdev |
| done | receipt-history-retention | [`done/receipt-history-retention.md`](civic_tasks/done/receipt-history-retention.md) | Keep vs 90-day policy on `/rovas/me` + `POST /rovas/civic_receipt_history_preference`; scrub merchant/purchase_at; smoked on rovdev |
| done | gift-donate-iban | [`done/gift-donate-iban.md`](civic_tasks/done/gift-donate-iban.md) | Org IBAN as `donate_iban` on `civic_project` (initially verified-only; gating relaxed in [charity-org-basic-info](civic_tasks/done/charity-org-basic-info.md)) |
| done | donor-add-charity-org | [`done/donor-add-charity-org.md`](civic_tasks/done/donor-add-charity-org.md) | Donor adds public-benefit org from Civic; SK auto-verify; claim email; smoked on rovdev |
| done | oauth-authorize-intent | [`done/oauth-authorize-intent.md`](civic_tasks/done/oauth-authorize-intent.md) | `intent=login\|register` on authorize → land on login vs register; deploy/smoke on rovdev |
| done | merit-period-index | [`done/merit-period-index.md`](civic_tasks/done/merit-period-index.md) | ME•ter period leaderboard; deploy/smoke on rovdev when ready |
| done | historical-pre-split | [`done/_historical-pre-split.md`](civic_tasks/done/_historical-pre-split.md) | Pre-split “Next work” archive — not open work |

Open tasks: see `civic_tasks/active/`. Naming and archival rules:
[`civic_tasks/README.md`](civic_tasks/README.md).

## Status


| Piece                                | Location                                                                                                                                   | Notes                                                                                                                                                                                  |
| ------------------------------------ | ------------------------------------------------------------------------------------------------------------------------------------------ | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `oauth2_server` 7.x-1.7              | `sites/all/modules/contrib/oauth2_server`                                                                                                  | + PKCE [#3015065-7](https://www.drupal.org/project/oauth2_server/issues/3015065); + last_access deadlock [#2859214](https://www.drupal.org/project/oauth2_server/issues/2859214) (`patches/oauth2_server-2859214-last-access-deadlock.patch`) |
| `xautoload` 7.x-5.9                  | `sites/all/modules/contrib/xautoload`                                                                                                      | Required by oauth2_server                                                                                                                                                              |
| `oauth2-server-php` **v1.14.2**      | `sites/all/libraries/oauth2-server-php`                                                                                                    | Module README lists ≤1.8; **1.14.2 required for PKCE**                                                                                                                                 |
| `rovas_oauth`                        | `sites/all/modules/custom/rovas_oauth`                                                                                                     | Bootstrap Civic server/client, login copy, Bearer helpers, `/me` + `/contributions/aggregate`                                                                                          |
| Expo login (PKCE)                    | Civic mobile                                                                                                                               | **Done** against `dev.rovas.app`                                                                                                                                                       |
| Civic API Bearer introspect          | Civic `GET /oauth2/tokens/{token}`                                                                                                         | **Done** — resolves Drupal `user_id` (uid)                                                                                                                                             |
| Services OAuth2 on `rovas_endpoint`  | Admin / `rovas_oauth_ensure_civic_resources()`                                                                                             | Server machine name `**civic**`; API-key auth disabled on Civic resources                                                                                                              |
| Home Merits reads                    | `/rovas/me`, `/rovas/contributions/aggregate`, `/rovas/merit_distribution`                                                                 | Implemented; `/rovas/me` includes `total_chr` + `receipt_history_retention` (`null`\|`keep`\|`policy`) — see [receipt-history-retention](civic_tasks/done/receipt-history-retention.md) |
| `rovas_civic`                        | `sites/all/modules/custom/rovas_civic`                                                                                                     | National rates, donor Merit issuance, State Treasury, VAT claim                                                                                                                        |
| Claim rule + split claims            | `POST /rovas/rules/rules_proxy_claim_vat_receipt`                                                                                          | Implemented in `rovas_civic` (enable via updb / admin)                                                                                                                                 |
| Claim reads + summary                | `GET /rovas/civic_claims`, `…/summary`                                                                                                     | Implemented in `rovas_civic` (`rovas_civic_update_7002`); **Civic proxies live** (`GET /receipts`, `/me/contributions/summary`)                                                        |
| Claim status by receipt              | `GET /rovas/civic_claims?receipt_uid=`                                                                                                     | Implemented — `{ claimed, receipt_uid }` only (no scanner)                                                                                                                             |
| Received history “From”              | `view=received` scanner username                                                                                                           | Implemented — `scanner_username` + `scanner_display_name`                                                                                                                              |
| National hourly rate read            | `GET /rovas/civic_national_hourly_rate`                                                                                                    | Implemented in `rovas_civic` (`rovas_civic_update_7003`)                                                                                                                               |
| Users search (split typeahead)       | `GET /rovas/users_search?query=`                                                                                                           | Implemented in `rovas_civic` (`rovas_civic_update_7005`). **Param is `query`, not `q`** — Drupal reserves `$_GET['q']` for the request path.                                           |
| Users lookup (community profile)     | `GET /rovas/users_lookup?username=`                                                                                                        | Implemented in `rovas_civic` (`rovas_civic_update_7006`). Exact username; Bearer required. Includes `nace_expertise[]` via `rovas_meritscope_user_expertise_json()`.                   |
| `top_charities` buy-ME + timeshare   | `users_lookup.top_charities`                                                                                                               | **Code ready** (`7027`) — Path-1 + `eur_buy_merits` (default project **1992**) + timeshare allocations; deploy/smoke pending — see [gifts-top-charities-attribution](civic_tasks/active/gifts-top-charities-attribution.md) |
| `top_volunteer_projects` WR rewards  | `users_lookup.top_volunteer_projects`                                                                                                      | **Implemented** — allocations + WR-linked ledger ME; CHR→ME stays unattributed — see [top_volunteer_projects include WR rewards](civic_tasks/done/_historical-pre-split.md#next-work-top_volunteer_projects-include-wr-rewards) |
| co_worker %-shares → volunteer         | ledger `field_merit_source` + `users_lookup` tabs                                                                                          | **Implemented** — co_worker (0h included) → `volunteer_work`; timeshare → charity — see [co_worker %-shares misclassified as charity](civic_tasks/done/_historical-pre-split.md#next-work-co_worker-shares-misclassified-as-charity) |
| Hide negative Merit users            | `users_search` / `users_lookup`                                                                                                            | Implemented — `total_me < 0` omitted / 404; search response stays public fields only                                                                                                   |
| Friend invite (Civic)                | —                                                                                                                                          | **Civic-client only** (`mailto:`). No Rovas invite endpoint. Signup URL in the email body should work (`/user/register` or OAuth authorize).                                           |
| Project detail + WR list             | `GET /rovas/civic_project/{nid}`, `GET /rovas/civic_project_work_reports`                                                                  | Implemented (`rovas_civic_update_7007`). Gift-path `donate_iban` (any valid stored IBAN) + Basic info `org_id` / `country` / `website_url` / `iban_verified` — see [charity-org-basic-info](civic_tasks/done/charity-org-basic-info.md) (supersedes verified-only rule in [gift-donate-iban](civic_tasks/done/gift-donate-iban.md)). Private participation still returns full content — see [private projects are fully readable](civic_tasks/done/_historical-pre-split.md#next-work-private-projects-are-fully-readable). |
| Project shareholders + WR photos     | `GET /rovas/civic_project_shareholders`, WR `photo_urls[]`                                                                                 | Implemented in `rovas_civic` (`rovas_civic_update_7008`).                                                                                                                              |
| Localized project / WR / lookup text | `?lang=` on `civic_project`, `civic_project_work_reports`, `users_lookup`                                                                  | Implemented — Entity Translation / `title_field` / `body` with source fallback.                                                                                                        |
| Merit history (profile ledger)       | `GET /rovas/civic_merit_history`                                                                                                           | Implemented in `rovas_civic` (`rovas_civic_update_7009`). JSON of Views `merit_history` / `page_1` for the Bearer user.                                                                |
| Volunteer report (deferred form)     | `GET /rovas/civic_my_projects`, `civic_projects_search`, `civic_nace_tree`; `POST /rovas/civic_create_project`, `civic_create_work_report` | Implemented in `rovas_civic` (`rovas_civic_update_7010`). Bearer only — do **not** call the partner API-KEY rules from Civic.                                                          |
| CHR→ME on WR verification            | `{civic_work_report_reward}` + verification hook                                                                                           | Implemented in `rovas_civic` (`rovas_civic_update_7011`). When `reward_kind=merits`, A6 converts the Chron grant after approval.                                                       |
| AI NACE + title                      | `POST /rovas/civic_ai_nace`                                                                                                                | Implemented in `rovas_ai` (`rovas_ai_update_7001`). Bearer; keep NACE-tree fallback when `unavailable` or module absent.                                                               |
| Work-report map proof                | `GET /rovas/civic_work_report_map_config`; `field_location` on create WR                                                                   | Implemented in `rovas_civic` (`rovas_civic_update_7012`). GeoJSON Point/LineString/Polygon; proof = URL **or** photos **or** map.                                                      |
| Civic WR verification (in-app)       | `GET /rovas/civic_pending_verifications`, `civic_work_report/{nid}`, `POST /rovas/civic_submit_verification`                               | Implemented in `rovas_civic` (`rovas_civic_update_7013`). Pending list + verifier detail + approve/reject/decline.                                                                     |
| Merit index (Civic Prínos / ranks)   | `GET /rovas/civic_merit_index`, `civic_merit_index_countries`                                                                             | Snapshot-backed (`7017`); cron rebuild every **12h** (configurable) + admin rebuild. Warm reads ≪1s; items include `by_category` + `by_source`.                                                                      |
| Charitable projects catalog          | `GET /rovas/civic_charity_projects`; org type `charity_organization`; `/charity-projects`                                                  | Catalog API implemented (`7018`). Product framing → Gifts / public-benefit orgs — see [broaden Gifts path labels](civic_tasks/done/_historical-pre-split.md#next-work-broaden-gifts-path-labels-public-benefit-orgs).          |
| Donor add charity org                | `POST /rovas/civic_create_charity_org`                                                                                                     | **Live on rovdev** (`rovas_civic_update_7023`) — SK RPO auto-approve; custodial uid; claim email (locale from `lang` or org country); AI queue. See [donor-add-charity-org](civic_tasks/done/donor-add-charity-org.md).                                              |
| Civic-branded OAuth login/register | `/user/login`, `/user/register` when authorizing `client_id=civic`                                                                        | Implemented in `rovas_oauth` — Civic chrome + copy; HybridAuth destination fix + automatic authorize (`7002`). Deploy + `drush74 updb` + smoke.                                      |
| Civic locale on OAuth authorize      | `GET /oauth2/authorize?lang=sk\|en`                                                                                                        | Implemented in `rovas_oauth` — session stash + force `$GLOBALS['language']` for Civic login/register. Deploy + smoke.                                                                  |
| OAuth authorize intent               | `GET /oauth2/authorize?intent=login\|register`                                                                                             | **Implemented** in `rovas_oauth` — session stash + goto rewrite to `/user/register` when `intent=register`. Deploy + smoke. See [oauth-authorize-intent](civic_tasks/done/oauth-authorize-intent.md).   |


## OAuth endpoints (live on dev)

- Authorize: `https://dev.rovas.app/oauth2/authorize`
- Token: `https://dev.rovas.app/oauth2/token`
- Revoke: `https://dev.rovas.app/oauth2/revoke`
- Introspect: `https://dev.rovas.app/oauth2/tokens/{access_token}` → JSON with `user_id`, `client_id`, `expires`, `scope` (404 if invalid)

## Civic home-read APIs (live on dev after enable)


| Method | URL                                                   | Auth                                 |
| ------ | ----------------------------------------------------- | ------------------------------------ |
| `GET`  | `https://dev.rovas.app/rovas/me`                      | Bearer required (`user_profile`)     |
| `GET`  | `https://dev.rovas.app/rovas/contributions/aggregate` | Public                               |
| `GET`  | `https://dev.rovas.app/rovas/merit_distribution`      | Public; Bearer optional for `user_*` |


Enable / re-sync on rovdev: **Configuration → Web services → Rovas OAuth (Civic)** →
**Enable Civic home API resources**, or `drush74 updb` after deploy (`rovas_oauth_update_7001`).

That helper also ensures endpoint auth is `oauth2_server => array('server' => 'civic')`
(not a bare checkbox string) and disables API-key auth on those three resources.

## Civic OAuth client (bootstrap)

Admin UI: **Configuration → Web services → Rovas OAuth (Civic)**  
(`/admin/config/services/rovas-oauth`)

Click **Bootstrap Civic OAuth server + client**. That creates:


| Field               | Value                                                |
| ------------------- | ---------------------------------------------------- |
| Server machine name | `civic`                                              |
| Enforce PKCE        | yes                                                  |
| Grant types         | authorization_code, refresh_token                    |
| Client ID           | `civic`                                              |
| Client secret       | *(empty — public native client)*                     |
| Redirect URI        | `civic://oauth/callback` (+ optional Expo proxy URI) |
| Automatic authorize | yes (no consent screen; `rovas_oauth_update_7002`)   |
| Default scope       | `user_profile`                                       |


HybridAuth (Google / Facebook / Drupal login) already attaches to `user_login`.
`oauth2_server` redirects anonymous authorize requests to
`/user/login?destination=oauth2/authorize`, so the existing HybridAuth widget
is the authorize login UI. When `client_id=civic`, `rovas_oauth` applies
Civic-branded chrome and copy (see
[Civic-branded OAuth login/register](civic_tasks/done/_historical-pre-split.md#next-work-civic-branded-oauth-loginregister)).
Non-Civic `/user/login` is unchanged.

After Google/Facebook, HybridAuth must return to `oauth2/authorize` (not `/user`).
That needs the destination-aware close handler in
`patches/hybridauth-destination-survive-oauth-roundtrip.patch` (applied under
`sites/all/modules/contrib/hybridauth`). With automatic authorization on, authorize
then immediately redirects to `civic://oauth/callback`.

## Enable on dev.rovas.app

Prefer **drush** over the Modules UI. Enabling via the UI can flash a one-off
PHP error from the site’s existing `service_container` module
(`Drupal::service()` while the container is still null during rebuild). The
modules still install; the error is a race with OpenLayers’ service container,
not a failure of `oauth2_server` itself.

```bash
# After your usual rsync to /var/www/rovdev:
cd /var/www/rovdev
drush74 en xautoload libraries oauth2_server rovas_oauth -y
drush74 updb -y
drush74 role-add-perm 'anonymous user' 'use oauth2 server'
drush74 role-add-perm 'authenticated user' 'use oauth2 server'
drush74 cc all -y
```

`rovas_oauth` bootstraps the `civic` server + client on enable. Confirm at
`/admin/config/services/rovas-oauth` (or re-run **Bootstrap** there).

Docroot mapping: **dev.rovas.app** → `/var/www/rovdev` (not prod `/var/www/rovas`).

Outgoing email on dev is logged, not sent (`DevelMailLog`). See
`[docs/dev-setup.md](../dev-setup.md#outgoing-email-disabled-on-dev)`.

### Services OAuth2 (rovas_endpoint)

- Admin: **Structure → Services → rovas_endpoint → Authentication**
- Enable **OAuth2 authentication**, server machine name `**civic`**
- Prefer **Enable Civic home API resources** on the Rovas OAuth admin form so
Civic resources get `disable_authentication` on API keys (Bearer-only callers
otherwise fail at the API-keys plugin).

To mirror on prod later: same UI steps, or run
`rovas_oauth_ensure_civic_resources()` via drush php-eval after deploy.
There is no Features export of `rovas_endpoint` today (DB-only).

## PKCE smoke test (curl)

```bash
# 1) Generate verifier / challenge (S256)
VERIFIER=$(python3 - <<'PY'
import base64, hashlib, os
v = base64.urlsafe_b64encode(os.urandom(32)).rstrip(b"=").decode()
print(v)
PY
)
CHALLENGE=$(python3 - <<PY
import base64, hashlib, sys
v = """$VERIFIER""".encode()
print(base64.urlsafe_b64encode(hashlib.sha256(v).digest()).rstrip(b"=").decode())
PY
)

# 2) Open in a browser (log in via HybridAuth), approve, capture ?code=
echo "https://dev.rovas.app/oauth2/authorize?response_type=code&client_id=civic&redirect_uri=civic%3A%2F%2Foauth%2Fcallback&scope=user_profile&state=spike1&code_challenge=$CHALLENGE&code_challenge_method=S256"

# 3) Exchange code (public client + PKCE; no client_secret)
curl -sS -X POST https://dev.rovas.app/oauth2/token \
  -d grant_type=authorization_code \
  -d client_id=civic \
  -d redirect_uri=civic://oauth/callback \
  -d code=PASTE_CODE \
  -d code_verifier=$VERIFIER
```

Expect JSON with `access_token` / `refresh_token`. A missing or wrong
`code_verifier` must fail (`code_verifier_mismatch` / similar).

## OAuth spike checklist

- [x] Deploy to `/var/www/rovdev` and enable modules
- [x] Bootstrap Civic server + client
- [x] Complete authorize → token with PKCE (2026-08-04 on dev)
- [x] Confirm Bearer resolves via `/oauth2/tokens/{token}` and `rovas_oauth_uid_from_bearer()`
- [x] Expo app login against these endpoints
- [ ] Note any oauth2-server-php 1.14.2 incompatibilities vs module README (none observed in authorize/token smoke)

---

## Home reads (done) — background for agents

Civic’s home mockup needs live Merits data. The mobile app already has a Rovas
**Bearer** access token. Civic API can introspect it to a Drupal **uid**.
Home-read endpoints below are **implemented**; claim reads are under
[Next work: Civic claim reads + SoT](civic_tasks/done/_historical-pre-split.md#next-work-civic-claim-reads--sot).

### Why existing endpoints are not enough


| Existing                                | Auth today                            | Problem for Civic                                         |
| --------------------------------------- | ------------------------------------- | --------------------------------------------------------- |
| `POST /rovas/rules/rules_user_s_merits` | User **API-KEY + TOKEN**              | Mobile has OAuth Bearer only, not the user’s API key pair |
| `GET …/rovas_merit_distribution`        | API-KEY + TOKEN; returns SVG + `meta` | Same auth issue; Civic needs **JSON buckets**, not SVG    |
| Community total                         | —                                     | Covered by `/rovas/contributions/aggregate`               |


### Priority order

#### 1. Enable OAuth2 authentication on Services (`rovas_endpoint`) — **done on rovdev**

See [Services OAuth2](#services-oauth2-rovas_endpoint) above. Re-run
`rovas_oauth_ensure_civic_resources()` if endpoint auth is still a bare
module string instead of `array('server' => 'civic')`.

#### 2. `GET /rovas/me` (Bearer required) — **implemented**

In `rovas_oauth` (`rovas_oauth_me_index()`).

```json
{
  "uid": 42,
  "username": "sano",
  "display_name": "Alexander Žaťko",
  "total_me": 1248.35,
  "total_chr": 320.0,
  "created": "2019-03-12T14:22:01+00:00",
  "language": "sk",
  "receipt_history_retention": null
}
```


| Field          | Source                                                                                                                                        |
| -------------- | --------------------------------------------------------------------------------------------------------------------------------------------- |
| `uid`          | Bearer → Drupal uid                                                                                                                           |
| `username`     | `$user->name`                                                                                                                                 |
| `display_name` | `field_full_name` if set; else username                                                                                                       |
| `total_me`     | `_rovas_merit_distribution_user_merits()` (userpoints + decimal)                                                                              |
| `total_chr`    | `field_chrons_balance` on the user account (numeric; `0` when empty)                                                                          |
| `created`      | ISO-8601 from Drupal `$account->created` (`gmdate('c')`); `null` if missing/invalid                                                           |
| `language`     | `$account->language` (raw Drupal langcode); `null` if empty/`und` — see [preferred language](civic_tasks/done/_historical-pre-split.md#next-work-expose-preferred-language-on-rovasme) |
| `receipt_history_retention` | `null` \| `keep` \| `policy` from `{civic_user_prefs}` — see [receipt-history-retention](civic_tasks/done/receipt-history-retention.md) |


#### 3. `GET /rovas/contributions/aggregate` (public) — **implemented**

In `rovas_oauth` (`rovas_oauth_contributions_retrieve()`). Uses the cached
collect from `rovas_merit_distribution` (1h TTL). `by_category` is zeros for v1.

```json
{
  "total_me": 84320.0,
  "by_category": {
    "taxes": 0,
    "charity": 0,
    "volunteer": 0
  }
}
```

#### 4. JSON merit distribution (Bearer optional) — **implemented**

`GET /rovas/merit_distribution` via `rovas_merit_distribution_json_index()`.
Reuses `_rovas_merit_distribution_collect_merits()` /
`_rovas_merit_distribution_compute_bins()`. No SVG.

```json
{
  "buckets": [
    { "label": "1–10", "count": 48 },
    { "label": "10–100", "count": 104 }
  ],
  "user_bucket_index": 5,
  "user_merits": 1248.35,
  "username": "sano",
  "total_users": 298,
  "bin_edges_log10": [0, 1, 2, 3]
}
```

When **no** Bearer: omit `user_`* fields. When Bearer present: fill them.

## Related Civic docs

- Task specs: [`civic_tasks/`](civic_tasks/)

- Deploy / enable on a prod clone: [`civic-deploy.md`](civic-deploy.md)
- Plan: `~/Documents/homeProjects/civic/docs/plan.md` (§6 claim SoT, §7 split claims, §8b claim rule, §15 phases)
- NeoFund: `~/Documents/Business/NeoFund_NP/` (State Treasury, Path-1 donor Merit, A12)
- Civic API/Expo: `~/Documents/homeProjects/civic/docs/rovas-integration.md`
- Mockup (home UX): `~/Documents/homeProjects/civic/mocs/civic-mockup.canvas.tsx`
