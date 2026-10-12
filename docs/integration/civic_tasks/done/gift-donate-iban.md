---
status: done
slug: gift-donate-iban
owner_repo: both
created: 2026-10-11
updated: 2026-10-11
---

# Expose verified donate IBAN on civic_project

**Product:** Civic’s Gifts org-detail modal lets donors start a SEPA transfer
(Pay by Square share/preview) to a nonprofit. For the demo slice, the
beneficiary is the organisation’s own IBAN.

> **Superseded (gating):** [charity-org-basic-info](../active/charity-org-basic-info.md)
> now returns `donate_iban` whenever a valid IBAN is stored, plus
> `iban_verified`, so Civic can show unverified IBANs with at-own-risk copy.
> The verified-only / approved-only rules below describe the original demo
> contract and are no longer enforced on Rovas.

**Owner:** `both` — Rovas adds `donate_iban` on `GET /rovas/civic_project/{nid}`;
Civic proxies it on `GET /projects/:nid` and builds the Expo Pay by Square UI.
No Merit grant in this slice.

### Contract

| | |
|--|--|
| Method | `GET` |
| Path | `/rovas/civic_project/{nid}` |
| Auth | Unchanged (anonymous allowed for NeoFund-**approved** public-benefit projects; Bearer otherwise) |

**New response field** (alongside existing gift-path fields):

| Field | Type | Rules |
|-------|------|--------|
| `donate_iban` | string \| omitted/`null` | Compact SEPA IBAN (no spaces, uppercase). **Include only when** the linked `charity_organization` has `field_charity_status = approved`, `field_charity_iban_verified` is true, and `field_charity_iban` is non-empty after normalize. Otherwise omit or send `null`. |

Do **not** expose:

- IBAN when `field_charity_iban_verified` is false/empty
- IBAN when org status is `pending`
- A separate `iban_verified` flag (presence of `donate_iban` is the gate)

Other gift-path fields (`is_charity`, `charity_status`, `organization`,
`donate_instructions`, `donate_url`) stay unchanged.

### Checklist

1. [x] On `civic_project` serialize: resolve linked org; if approved + IBAN verified + IBAN present → set `donate_iban` (strip spaces, uppercase)
2. [x] Never include `donate_iban` for unverified / pending / missing IBAN
3. [x] Deploy rovdev + smoke (human unless asked)

### Smoke test

```bash
# Approved org with staff-verified IBAN → donate_iban present (compact)
curl -sS "https://dev.rovas.app/rovas/civic_project/<APPROVED_VERIFIED_NID>" \
  | jq '{ project_nid, charity_status, organization, donate_iban }'
# Expect: donate_iban matches /^[A-Z]{2}[0-9]{2}[A-Z0-9]{11,30}$/

# Approved org without IBAN verification → donate_iban absent/null
curl -sS "https://dev.rovas.app/rovas/civic_project/<APPROVED_UNVERIFIED_NID>" \
  | jq '{ project_nid, charity_status, donate_iban }'
# Expect: donate_iban == null or key omitted
```

### Civic will consume

- Shared `ProjectDetail.donate_iban`
- Civic API `normalizeProjectDetail` passthrough (format check only)
- Expo: amount field + Pay by Square encode → Share to banking app / Preview
  when `donate_iban` is set and `charity_status === "approved"`. No ME credit.
