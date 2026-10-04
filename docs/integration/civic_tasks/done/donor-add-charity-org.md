---
status: done
slug: donor-add-charity-org
owner_repo: both
created: 2026-10-04
updated: 2026-10-04
---

# Donor add public-benefit organisation (Civic)

**Product:** Civic’s Nonprofit projects (Gifts) screen lets signed-in donors
**add** a public-benefit / nonprofit organisation themselves, in addition to
the existing client `mailto:` invite. On submit, Rovas creates (or returns) a
`charity_organization` plus a linked charitable project. Slovak orgs can be
auto-approved via the nonprofit register (IČO); others stay `pending` for
NeoFund staff. Claim email invites the org to take ownership when records are
custodial.

**Owner:** `both` — Rovas implements `POST /rovas/civic_create_charity_org`;
Civic proxies as `POST /charities/organizations` and builds the Expo form.

### Contract

| | |
|--|--|
| Method | `POST` |
| Path | `/rovas/civic_create_charity_org` |
| Auth | Bearer (OAuth2 `civic` client); API-key auth **disabled** |
| Content-Type | `application/json` |

**Body:**

| Field | Rules |
|-------|--------|
| `name` | Legal name; required; trim; non-empty |
| `country` | ISO 3166-1 alpha-2; required; uppercase |
| `website_url` | Absolute `http(s)` URL; required |
| `contact_email` | Valid email; required |
| `org_id` | Registration number / IČO; required; trim; uniqueness with `country` |
| `iban` | SEPA IBAN; required; normalize (strip spaces); basic format check |

**Uniqueness:** `(country, org_id)` on `field_charity_country` +
`field_charity_ico`. If a matching `charity_organization` already exists:

- Return **200** with `created: false` and that org’s linked charitable
  project (create a default project only if none exists yet).
- `status` mirrors the existing org’s `field_charity_status`
  (`approved` \| `pending`).
- Do **not** create a second org.

**Ownership (new creates):**

1. Default: author/owner of org + project = **custodial NeoFund/system uid**
   (configurable under Configuration → Rovas Civic → Public-benefit /
   nonprofit organizations, e.g. `rovas_civic_charity_custodial_uid`).
2. If the Bearer user’s Drupal `mail` equals `contact_email`
   (case-insensitive) → author/owner = **submitter**.

**SK auto-approve:** when `country=SK`, look up `org_id` in the Slovak
register of nonprofit / public-benefit organizations. If found and eligible:

1. Set `field_charity_status` = `approved`.
2. Optionally reconcile legal name (`title`) from the register.
3. Invoke Rovas AI module to check entered data accuracy (async/queue OK;
   failure must not block the HTTP response once org+project exist —
   leave `approved` if register passed, log AI issues for staff).
4. Email `contact_email` that a donor added the org and invite them to
   **claim** the project on Rovas (link to `/charity-projects` and/or the
   project node) to add or correct info — especially when
   `ownership=custodial`.

If SK register lookup fails, is ambiguous, or country is not `SK`: leave
`pending`, keep existing insert → `site_mail` staff notify. Do **not** let
owners self-serve the approved badge outside this automated path.

**200 body:**

```json
{
  "ok": true,
  "status": "approved",
  "created": true,
  "organization_nid": "123",
  "project_nid": "456",
  "organization": "Example n.o.",
  "ownership": "custodial"
}
```

| Field | Rules |
|-------|--------|
| `status` | `approved` \| `pending` |
| `created` | `true` if new org was inserted; `false` on duplicate short-circuit |
| `organization_nid` | Org node nid (string) |
| `project_nid` | Linked charitable project nid (string) |
| `organization` | Legal name returned to Civic |
| `ownership` | `custodial` \| `submitter` (for this create; on duplicate, report current owner mode if known, else omit or use best-effort) |

**Errors:**

| HTTP | `error` | When |
|------|---------|------|
| 400 | `invalid_input` | Missing/invalid fields |
| 401 | `unauthorized` | Missing/invalid Bearer |
| 502 | `rovas_error` / register outage after pending create | Prefer still returning 200 with `status: pending` if org+project were saved; only 502 when nothing durable was written |

Prefer **idempotent 200** with `created: false` for duplicates — **not** 409.

**New project defaults (create path):** Public, for sale, `field_is_charity`
on, `field_charity_organization` → new/existing org, title = org legal name
(claimant can edit later).

### Checklist

1. [x] Services resource `civic_create_charity_org` (POST, Bearer, no API key)
2. [x] Lookup by `(country, org_id)`; short-circuit duplicate with linked project
3. [x] Create `charity_organization` + charitable project; field mapping as above
4. [x] Custodial uid variable + admin UI; email-match → submitter owner
5. [x] SK nonprofit register verification → auto `approved` + name reconcile
6. [x] Claim email to `contact_email` when custodial (and optionally always)
7. [x] Rovas AI accuracy check (async OK); staff notify on pending
8. [x] Deploy rovdev + smoke (human unless asked)

**Rovas notes (2026-10-04):** Implemented in `rovas_civic` — `POST /rovas/civic_create_charity_org`
(`rovas_civic_update_7023`). SK lookup via ŠÚ SR RPO
(`https://api.statistics.sk/rpo/v1/search`); eligible source-register codes
4/5/6/7/9 (nadácie, neinvestičné fondy, n.o., o.z., medzinárodný prvok).
Custodial uid: `rovas_civic_charity_custodial_uid` on Configuration → Rovas Civic.
AI check queue `rovas_civic_charity_ai_check` (does not revoke auto-approval).

### Smoke test

```bash
# After deploy: drush74 updb -y && drush74 cc all -y

TOKEN=… # Civic OAuth access token

# 1) New SK org (known n.o. IČO — Človek v ohrození; use a unique test IČO on shared env)
curl -sS "https://dev.rovas.app/rovas/civic_create_charity_org" \
  -H "Authorization: Bearer $TOKEN" \
  -H "Content-Type: application/json" \
  -H "Accept: application/json" \
  -d '{
    "name": "Test Public Benefit Org",
    "country": "SK",
    "website_url": "https://clovekvohrozeni.sk",
    "contact_email": "org@example.sk",
    "org_id": "50082001",
    "iban": "SK3112000000198742637541"
  }'
# Expect: { "ok": true, "status": "approved"|"pending", "created": true,
#           "organization_nid", "project_nid", "organization", "ownership" }

# 2) Duplicate same country+org_id
# Expect: 200, created: false, same organization_nid

# 3) Non-SK → status pending
# 4) contact_email matching Bearer user mail → ownership: "submitter"

# Catalog (only if approved):
curl -sS "https://dev.rovas.app/rovas/civic_charity_projects?query=Test" \
  -H "Accept: application/json"
```

### Deferred (not this increment)

- Real email verification / ownership-transfer token for Civic’s **Verify**
  button next to contact email (UI stub only on Civic for now).
- Automated registers for countries other than SK.

### Civic will consume

- Expo Nonprofit projects screen: Add organisation form → Civic
  `POST /charities/organizations` (Bearer) → this endpoint.
- On `status=approved`: refresh `GET /charities/projects` / open project modal.
- On `status=pending`: under-review copy (NeoFund will contact when done).
- Companion: `~/Documents/homeProjects/civic/docs/rovas-integration.md`.
