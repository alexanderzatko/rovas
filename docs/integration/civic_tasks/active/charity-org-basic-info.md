---
status: active
slug: charity-org-basic-info
owner_repo: both
created: 2026-10-12
updated: 2026-10-12
---


# Expose charity org basic info on civic_project

**Product:** Civic’s Nonprofit projects screen expands a catalog card in place
and shows a **Basic info** block: IČO / registration number, country of
registered seat, website, and (when already gated) SEPA IBAN. Those org fields
are stored on the linked `charity_organization` at donor-add time but are not
yet returned on `GET /rovas/civic_project/{nid}`. Civic needs them on the same
payload as existing gift-path fields so expand does not require a second org
endpoint.

**Owner:** `both` — Rovas adds `org_id`, `country`, `website_url`,
`iban_verified` on `GET /rovas/civic_project/{nid}`; Civic proxies them on
`GET /projects/:nid` and renders Basic info (localized country name via
ISO→name lookup on the client).

### Contract

| | |
|--|--|
| Method | `GET` |
| Path | `/rovas/civic_project/{nid}` |
| Auth | Unchanged (anonymous allowed for NeoFund-**approved** public-benefit projects; Bearer otherwise) |

**New response fields** (alongside existing gift-path fields):

| Field | Type | Rules |
|-------|------|--------|
| `org_id` | string \| omitted/`null` | Registration number / IČO from linked org (`field_charity_ico`). Trim; omit/`null` when empty or no linked org. |
| `country` | string \| omitted/`null` | ISO 3166-1 alpha-2 from linked org (`field_charity_country`), uppercase. Omit/`null` when empty or no linked org. |
| `website_url` | string \| omitted/`null` | Absolute `http(s)` website from linked org. Trim; omit/`null` when empty or invalid/missing. |
| `iban_verified` | bool \| omitted/`null` | Staff IBAN verification from linked org (`field_charity_iban_verified`, and IBAN present). `true`/`false` when linked org exists; omit/`null` when no linked org. Not gated on org approval. |

Include these whenever the project has a linked `charity_organization` and
Civic is already allowed to read the project (same visibility as
`organization` / other gift-path fields). Do **not** gate `org_id` /
`country` / `website_url` / `iban_verified` on IBAN verification or approval.

`donate_iban` rules stay unchanged (approved + staff IBAN-verified only) —
see [gift-donate-iban](../done/gift-donate-iban.md). Presence of
`donate_iban` still implies verified; `iban_verified` lets Civic show
verification state when the IBAN itself is withheld (e.g. pending approval).

Other gift-path fields (`is_charity`, `charity_status`, `organization`,
`donate_instructions`, `donate_url`, `donate_iban`) stay unchanged.

### Checklist

1. [x] On `civic_project` serialize: resolve linked org; set `org_id`, `country`, `website_url`, `iban_verified` from org fields when present
2. [x] Normalize `country` to uppercase ISO2; trim `org_id` / `website_url`; omit or null when empty
3. [x] Do not change `donate_iban` gating
4. [ ] Deploy rovdev + smoke (human unless asked)

### Smoke test

```bash
# Approved org with registry fields → org_id, country, website_url, iban_verified present
curl -sS "https://dev.rovas.app/rovas/civic_project/<APPROVED_ORG_PROJECT_NID>" \
  | jq '{ project_nid, organization, org_id, country, website_url, iban_verified, donate_iban }'
# Expect: country matches /^[A-Z]{2}$/; org_id non-empty; website_url http(s) when set;
#         iban_verified true|false; donate_iban only when verified + approved

# Project without linked charity org → fields absent/null
curl -sS "https://dev.rovas.app/rovas/civic_project/<NON_CHARITY_NID>" \
  -H "Authorization: Bearer <TOKEN>" \
  | jq '{ project_nid, is_charity, org_id, country, website_url, iban_verified }'
# Expect: org_id/country/website_url/iban_verified null or omitted
```

### Civic will consume

- Shared `ProjectDetail.org_id` / `country` / `website_url` / `iban_verified`
- Civic API `normalizeProjectDetail` passthrough
- Expo Nonprofit projects expand card → Basic info pillbox (country shown via
  `countryLabel(iso2, locale)`)
