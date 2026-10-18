# Rovas API reference

Base: `https://rovas.app` (prod) or `https://dev.rovas.app` (dev).  
Prefix: `/rovas` unless the path is under `/oauth2`.

Auth column:

| Label | Meaning |
| ----- | ------- |
| **API-KEY** | Headers `API-KEY` + `TOKEN` (classic) |
| **Bearer** | `Authorization: Bearer` with scope `user_profile` |
| **Public** | No credentials (API-key auth disabled on the resource) |
| **Optional Bearer** | Public read; Bearer adds user-specific fields |

Schema details for classic Rules bodies: [`rovasApi.yml`](../../sites/all/modules/custom/rovas_api_endpoints/rovasApi.yml).

---

## OAuth 2 protocol

| Method | Path | Auth | Notes |
| ------ | ---- | ---- | ----- |
| `GET` | `/oauth2/authorize` | browser session | PKCE; Civic supports `lang`, `intent` |
| `POST` | `/oauth2/token` | public client + PKCE | `authorization_code` / `refresh_token` |
| `POST` | `/oauth2/revoke` | token | revoke access/refresh |
| `GET` | `/oauth2/tokens/{access_token}` | — | introspect → `user_id`, `client_id`, `expires`, `scope` (404 if invalid) |

---

## Classic API (`API-KEY` + `TOKEN`)

### Accounts & identity

| Method | Path | Auth | Summary |
| ------ | ---- | ---- | ------- |
| `POST` | `/rovas/rovas_partner_user` | API-KEY | **Preferred** create/link partner user; returns `rovas_uid`, `api_key`, `token` |
| `POST` | `/rovas/rules/rules_create_user` | API-KEY | Legacy create/link (same body/response) |
| `GET` | `/rovas/rovas_hybridauth_providers.json` | Public* | List HybridAuth providers for `hybridauth_provider` |
| `POST` | `/rovas/rules/rules_fetch_user_data` | API-KEY | Profile info for the key owner |
| `POST` | `/rovas/rules/rules_users_chrons` | API-KEY | Chron balances |
| `POST` | `/rovas/rules/rules_user_s_merits` | API-KEY | Merit totals (optional NACE filter) |
| `POST` | `/rovas/rules/rules_check_enabled_language` | API-KEY | Whether a 2-letter language code is enabled |

\*Usually public when “Disable API keys authentication” is set on the resource.

### Projects & work reports

| Method | Path | Auth | Summary |
| ------ | ---- | ---- | ------- |
| `POST` | `/rovas/rules/rules_proxy_create_new_project` | API-KEY | Create project |
| `POST` | `/rovas/rules/rules_proxy_delete_project` | API-KEY | Delete project |
| `POST` | `/rovas/rules/rules_proxy_create_work_report` | API-KEY | Create work report |
| `POST` | `/rovas/rules/rules_proxy_delete_work_report` | API-KEY | Delete work report |
| `POST` | `/rovas/rules/rules_proxy_report_editability` | API-KEY | Is work report editable by caller |
| `POST` | `/rovas/rules/rules_proxy_work_report_verifiability` | API-KEY | Is work report verifiable by caller |
| `POST` | `/rovas/rules/rules_proxy_report_verification_status` | API-KEY | Verification status |
| `POST` | `/rovas/rules/rules_create_verification` | API-KEY | Create verification |
| `POST` | `/rovas/rules/rules_create_verification_with_acccess_token` | Public† | Create verification via access token |
| `POST` | `/rovas/rules/rules_proxy_check_verifier_by_access_tokens` | see OpenAPI | Verifiability via access tokens |
| `POST` | `/rovas/rules/rules_proxy_reward_work_report` | API-KEY | Reward a work report |
| `GET` | `/rovas/rovas_verifications.json` | API-KEY | List work-report verifications |
| `GET` | `/rovas/rovas_comment_notices.json` | API-KEY | Pending watched-comment notices (Comment Notify audience; same store as Civic Bearer twin) |
| `PUT` | `/rovas/node/{id}` | API-KEY | Update node owned by key owner |
| `DELETE` | `/rovas/node/{id}` | API-KEY | Delete node owned by key owner |

†Typo in rule machine name (`acccess`) is intentional in the live path.

### Shares, purchases, conversions

| Method | Path | Auth | Summary |
| ------ | ---- | ---- | ------- |
| `POST` | `/rovas/rules/rules_proxy_check_or_add_shareholder` | API-KEY | Ensure user share on project |
| `POST` | `/rovas/rules/rules_proxy_check_or_add_project_shareholder` | API-KEY | Ensure project-as-shareholder |
| `POST` | `/rovas/rules/rules_proxy_purchase_product` | API-KEY | Process product purchase |
| `POST` | `/rovas/rules/rules_proxy_create_aur` | API-KEY | Asset usage record (usage fee) |
| `POST` | `/rovas/rules/rules_get_merits_for_projects` | API-KEY | Merit values for pipe-separated project IDs |
| `POST` | `/rovas/rules/rules_convert_euros_to_chrons` | API-KEY | EUR → Chron quote |
| `POST` | `/rovas/rules/rules_convert_euros_to_chrons_reverse` | API-KEY | Chron → EUR quote |

### Views (leaderboards / feeds)

| Method | Path | Auth | Summary |
| ------ | ---- | ---- | ------- |
| `GET` | `/rovas/views/top_10_workers_in_a_category` | see OpenAPI | Top workers in a category |
| `GET` | `/rovas/views/project_merits_feed` | see OpenAPI | Top rewarders of a project |
| `GET` | `/rovas/views/projects_merits` | see OpenAPI | Top users across same-site projects |

---

## OAuth / Bearer API (Civic-oriented)

API-key authentication is **disabled** on these resources. Use Bearer where
required. Implemented mainly in `rovas_oauth`, `rovas_civic`, `rovas_merit_distribution`, `rovas_ai`.

### Home & profile

| Method | Path | Auth | Summary |
| ------ | ---- | ---- | ------- |
| `GET` | `/rovas/me` | Bearer | Profile: `uid`, `username`, `display_name`, `total_me`, `total_chr`, `created`, `language`, `receipt_history_retention` |
| `GET` | `/rovas/contributions/aggregate` | Public | Community Merit total (+ category placeholders) |
| `GET` | `/rovas/merit_distribution` | Optional Bearer | JSON Merit histogram; `user_*` when Bearer present |
| `POST` | `/rovas/civic_receipt_history_preference` | Bearer | Set receipt history retention (`keep` \| `policy`) |
| `GET` | `/rovas/civic_merit_history` | Bearer | Caller’s Merit ledger rows |

### Users

| Method | Path | Auth | Summary |
| ------ | ---- | ---- | ------- |
| `GET` | `/rovas/users_search?query=` | Bearer | Typeahead search (**param is `query`, not `q`**) |
| `GET` | `/rovas/users_lookup?username=` | Bearer | Public community profile (+ `nace_expertise`, tab aggregates). Users with `total_me < 0` → 404 |

Optional: `lang=sk|en` on lookup for localized fields.

### Tax claims (Path 1)

| Method | Path | Auth | Summary |
| ------ | ---- | ---- | ------- |
| `POST` | `/rovas/rules/rules_proxy_claim_vat_receipt` | Bearer | Claim eKasa VAT as donor Merits (split `shares[]`) |
| `GET` | `/rovas/civic_claims?view=submitted\|received` | Bearer | Claim lists |
| `GET` | `/rovas/civic_claims?receipt_uid=` | Bearer | `{ claimed, receipt_uid }` only |
| `GET` | `/rovas/civic_claims/summary` | Bearer | Totals for caller |
| `GET` | `/rovas/civic_national_hourly_rate?country=` | Public | National hourly rate used in ME formula |

### Projects, charity catalog, gifts

| Method | Path | Auth | Summary |
| ------ | ---- | ---- | ------- |
| `GET` | `/rovas/civic_project/{nid}` | Public‡ | Project detail (`donate_iban`, org fields, …) |
| `GET` | `/rovas/civic_project_work_reports?project_nid=` | Public‡ | Work reports for project |
| `GET` | `/rovas/civic_project_shareholders?project_nid=` | Public‡ | Shareholders |
| `GET` | `/rovas/civic_charity_projects` | Public | Approved public-benefit project catalog |
| `POST` | `/rovas/civic_create_charity_org` | Bearer | Donor submits a public-benefit org |

‡Anonymous allowed for NeoFund-approved public-benefit projects; otherwise Bearer / access rules apply in the callback.

### Volunteer work reports

| Method | Path | Auth | Summary |
| ------ | ---- | ---- | ------- |
| `GET` | `/rovas/civic_my_projects` | Bearer | Recent projects the user reported on |
| `GET` | `/rovas/civic_projects_search?query=` | Bearer | Search publishable projects (`query` ≥ 3 chars) |
| `GET` | `/rovas/civic_nace_tree` | Public | NACE activity tree |
| `POST` | `/rovas/civic_create_project` | Bearer | Create project |
| `POST` | `/rovas/civic_create_work_report` | Bearer | Create work report (proof: URL, photos, and/or map) |
| `GET` | `/rovas/civic_work_report_map_config` | Bearer | Map widget config for location proof |
| `POST` | `/rovas/civic_ai_nace` | Bearer | AI NACE + title suggestion (optional module) |

### Verification & notifications

| Method | Path | Auth | Summary |
| ------ | ---- | ---- | ------- |
| `GET` | `/rovas/civic_pending_verifications` | Bearer | Work reports assigned to caller to verify |
| `GET` | `/rovas/civic_work_report/{nid}` | Bearer | Verifier-facing work-report detail |
| `POST` | `/rovas/civic_submit_verification` | Bearer | Approve / reject / decline |
| `GET` | `/rovas/civic_pending_comment_notices` | Bearer | Polled watched-comment notices (`rovas_comment_watch`) |

### Merit index & spotlight

| Method | Path | Auth | Summary |
| ------ | ---- | ---- | ------- |
| `GET` | `/rovas/civic_merit_index` | Public | Snapshot leaderboard (`by_category`, `by_source`, …) |
| `GET` | `/rovas/civic_merit_index_countries` | Public | Country filter options |
| `GET` | `/rovas/civic_merit_spotlight` | Public | Period leads by Civic area |
| `GET` | `/rovas/civic_merit_period_index` | Public | ME•ter period leaderboard |

---

## Example responses

### `GET /rovas/me`

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

### `POST /rovas/rovas_partner_user` (success)

```json
{
  "rovas_uid": 123,
  "api_key": "…",
  "token": "…"
}
```

---

## Related internal docs

- Usage: [getting-started.md](getting-started.md)
- Civic integration index: [../integration/civic.md](../integration/civic.md)
- Partner signup deep-dive: [THIRD_PARTY_ACCOUNT_CREATION.md](../../sites/all/modules/custom/rovas_api_endpoints/THIRD_PARTY_ACCOUNT_CREATION.md)
