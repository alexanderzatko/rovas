---
status: done
slug: charity-org-basic-info
owner_repo: both
created: 2026-10-12
updated: 2026-10-13
---


# Expose charity org basic info on civic_project

**Product:** Civic’s Nonprofit projects screen expands a catalog card in place
and shows a **Basic info** block: IČO / registration number, country of
registered seat, website, and SEPA IBAN (with verification state). Those org
fields are stored on the linked `charity_organization` at donor-add time but
are not yet returned on `GET /rovas/civic_project/{nid}`. Civic needs them on
the same payload as existing gift-path fields so expand does not require a
second org endpoint.

**Owner:** `both` — Rovas adds `org_id`, `country`, `website_url`,
`iban_verified` and relaxes `donate_iban` (include whenever a valid IBAN is
stored) on `GET /rovas/civic_project/{nid}`; Civic proxies them on
`GET /projects/:nid` and renders Basic info (localized country name via
ISO→name lookup on the client; at-own-risk copy when showing unverified IBAN).

### Contract

| | |
|--|--|
| Method | `GET` |
| Path | `/rovas/civic_project/{nid}` |
| Auth | Unchanged (anonymous allowed for NeoFund-**approved** public-benefit projects; Bearer otherwise) |

**New / updated response fields** (alongside existing gift-path fields):

| Field | Type | Rules |
|-------|------|--------|
| `org_id` | string \| omitted/`null` | Registration number / IČO from linked org (`field_charity_ico`). Trim; omit/`null` when empty or no linked org. |
| `country` | string \| omitted/`null` | ISO 3166-1 alpha-2 from linked org (`field_charity_country`), uppercase. Omit/`null` when empty or no linked org. |
| `website_url` | string \| omitted/`null` | Absolute `http(s)` website from linked org. Trim; omit/`null` when empty or invalid/missing. |
| `iban_verified` | bool \| omitted/`null` | Staff IBAN verification from linked org (`field_charity_iban_verified`, and IBAN present). `true`/`false` when linked org exists; omit/`null` when no linked org. |
| `donate_iban` | string \| omitted/`null` | Compact SEPA IBAN (no spaces, uppercase) from linked org whenever stored value normalizes. **Not** gated on approval or staff verification. Omit/`null` when empty/invalid/no org. Supersedes the verified-only rule in [gift-donate-iban](gift-donate-iban.md). |

Include these whenever the project has a linked `charity_organization` and
Civic is already allowed to read the project (same visibility as
`organization` / other gift-path fields). Do **not** gate them on IBAN
verification or org approval — Civic decides whether to show unverified IBAN
and any at-own-risk warning via `iban_verified`.

Other gift-path fields (`is_charity`, `charity_status`, `organization`,
`donate_instructions`, `donate_url`) stay unchanged.

### Checklist

1. [x] On `civic_project` serialize: resolve linked org; set `org_id`, `country`, `website_url`, `iban_verified` from org fields when present
2. [x] Normalize `country` to uppercase ISO2; trim `org_id` / `website_url`; omit or null when empty
3. [x] Return `donate_iban` whenever a valid IBAN is stored (not gated on verification/approval)
4. [x] Deploy rovdev + smoke (human unless asked)

### Smoke test

```bash
# Org with registry fields + IBAN → org_id, country, website_url, donate_iban, iban_verified
curl -sS "https://dev.rovas.app/rovas/civic_project/<APPROVED_ORG_PROJECT_NID>" \
  | jq '{ project_nid, organization, org_id, country, website_url, iban_verified, donate_iban }'
# Expect: country matches /^[A-Z]{2}$/; org_id non-empty; website_url http(s) when set;
#         donate_iban compact SEPA when stored; iban_verified true|false independently

# Project without linked charity org → fields absent/null
curl -sS "https://dev.rovas.app/rovas/civic_project/<NON_CHARITY_NID>" \
  -H "Authorization: Bearer <TOKEN>" \
  | jq '{ project_nid, is_charity, org_id, country, website_url, iban_verified, donate_iban }'
# Expect: org_id/country/website_url/iban_verified/donate_iban null or omitted
```

### Civic will consume

- Shared `ProjectDetail.org_id` / `country` / `website_url` / `iban_verified` /
  `donate_iban` (unverified allowed)
- Civic API `normalizeProjectDetail` passthrough
- Expo Nonprofit projects expand card → Basic info pillbox (country shown via
  `countryLabel(iso2, locale)`; at-own-risk when `iban_verified === false`)
