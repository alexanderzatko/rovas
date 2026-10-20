# Trusted User via Slovak government ID (slovensko.sk) — deferred design

**Status:** deferred — documented for later; not scheduled for implementation.  
**Related:** [`trusted-user-deploy.md`](trusted-user-deploy.md) (current Trusted User ship notes).  
**Civic pointer:** `~/Documents/homeProjects/civic/docs/rovas-integration.md` (Deferred section).

## Verdict

Viable on Rovas. Private-sector legal entities (PO) may reuse slovensko.sk WebSSO
(eID / Slovensko v mobile) for state-guaranteed identity. Add eID verification as a
**third OR path** into existing Trusted User eligibility. Hard parts: **NASES
onboarding**, **SAML SP**, and **PII handling**. Civic only consumes `/rovas/me`
flags later.

## Context on Rovas today

**Trusted User** — sticky `field_trusted_user` in `rovas_user.module`:

- Compliance (`field_verification_score`) ≥ 50, **or**
- Owner of a published project with **>500 Merits** from rewards to that project

Benefits: optional auto-approve of own work reports; opt-out of verification
obligations. Revocation after dispute maximal penalty when eligibility fails.

**Real names** — optional profile field `field_full_name` (not required). eID remains
an **opt-in** verification step; it may offer to fill or confirm `field_full_name`
from the SAML assertion without making real names mandatory for other users.

**Login today** — HybridAuth (Google / Facebook / Drupal). eID is **not** a
replacement primary login for v1; it is a **verify identity** action for signed-in
users.

**Related NeoFund concepts** — AML/KYC for EUR payouts is separate (may later reuse
eID LoA). Civic deferred “income-tax Merits / ÚPVS” can share the same SP later.

## Target architecture (Rovas-centric)

```text
User → Rovas Profile “Verify with eID”
     → UPVS SAML AuthnRequest
     → eID / Slovensko v mobile
     → SAML Assertion to Rovas ACS
     → hash subject URI, set eid_verified
     → maybe_grant_trusted via eID path
```

- Protocol: **SAML 2.0 WebSSO** (not OAuth). Samples:
  [NASES-Slovakia/slovensko-sk-login](https://github.com/NASES-Slovakia/slovensko-sk-login).
  Optional Docker proxy: [slovensko-sk-api](https://github.com/slovensko-digital/slovensko-sk-api) —
  note `GET /api/upvs/identity` is **OVM-only**; private SP should use SAML assertion
  attributes (FO URI `rc://sk/...`).
- Operator: Slovak **PO** with IČO (NeoFund / Rovas operator). Free for private
  sector; months of Partner Framework / FIX / PROD process.

## Product defaults

- **Eligibility:** `eid_verified` is a third OR in `rovas_user_trusted_conditions_met()`;
  sticky grant same as today.
- **Opt-in:** Verify with eID from profile; not required to use Rovas.
- **Name handling:** On success, optionally propose filling empty `field_full_name`
  from assertion; do not overwrite a user-set name without confirmation.
- **Storage:** `field_eid_verified`, `field_eid_verified_at`, hashed subject URI
  (avoid plain rodné číslo unless counsel requires it).
- **Revocation:** Keep dispute reevaluate; define whether clearing eID also revokes
  Trusted when other paths fail.
- **Geography:** SK-first; eIDAS later.
- **AML:** Keep separate in v1; document optional reuse for counsel.

## Implementation checklist (when scheduled)

### 1. Org / legal (blocking, start first)

- [ ] Confirm PO (IČO) that owns the Service Provider registration
- [ ] Partner Framework Portal registration; FIX tunnel; test identities; SP metadata (ACS, SLO)
- [ ] Privacy / processing purpose for Trusted grant; counsel on legal basis and retention
- [ ] Glossary: document Trusted User (missing today) and eID path

### 2. Schema + grant logic

- [ ] New fields on user: `field_eid_verified`, `field_eid_verified_at`, `field_eid_subject_hash` (names flexible)
- [ ] Extend `rovas_user_trusted_conditions_met()` and grant/notify path used by `rovas_user_maybe_grant_trusted`
- [ ] Update dispute reevaluate if eID path should keep Trusted after compliance reset
- [ ] Deploy notes alongside [`trusted-user-deploy.md`](trusted-user-deploy.md)

### 3. SAML Service Provider

- [ ] Verify start URL (authenticated user only) + Assertion Consumer Service + SP- and IdP-initiated logout (all three scenarios required for PROD)
- [ ] Prefer PHP/Drupal-native SP or NASES PHP sample behind Rovas routes
- [ ] Bind assertion to **currently logged-in** Drupal uid; reject if subject hash already linked to another account
- [ ] FIX smoke, then PROD metadata cutover

### 4. Profile UI + i18n

- [ ] Fieldset near Trusted user settings: CTA “Verify with eID / slovensko.sk”, verified badge, date
- [ ] On grant via eID: same site message / dismiss flow as other grant paths
- [ ] EN/SK (and other locales) PO strings; product terms Merit / Chron / Rováš unchanged

### 5. API contract for consumers (Civic and others)

- [ ] Extend `GET /rovas/me` with `trusted_user`, `eid_verified` (optional: Compliance)
- [ ] When ready to build for Civic: create `docs/integration/civic_tasks/active/<slug>.md` from the template and add a Task directory row in [`integration/civic.md`](integration/civic.md)

### 6. Docs

- [ ] Public glossary / rules mention of Trusted User + eID eligibility
- [ ] Deploy + smoke: FIX login, grant Trusted, duplicate-subject rejection, logout scenarios, dispute revoke behavior

## Downstream: Civic (not blocking Rovas ship)

After `/rovas/me` exposes the flags, Civic can:

- Surface status on Profile and open Rovas verify URL in browser
- Adjust volunteer / WR copy when Trusted + auto-approve applies

## Out of scope for v1

- Replacing HybridAuth social login with eID as primary login
- Full ÚPVS filings / income-tax Merits (reuse SP later)
- Non-SK eIDs / eIDAS node
- Replacing NeoFund AML document collection with eID alone

## Risks

- NASES calendar dominates (often 2–6+ months to PROD)
- FO URI embeds rodné číslo — hash + minimal retention
- Account-linking abuse (one eID → one Rovas account)
- OVM-only REST identity APIs — do not depend on them for private SP
- Trusted ≠ AML until counsel unifies them

## Suggested order (when scheduled)

1. Confirm PO + start NASES / FIX (parallel with counsel)
2. Spec fields, grant OR-path, `/rovas/me` shape in a Civic integration task file
3. Spike SAML SP on FIX; harden ACS + subject uniqueness
4. Profile UI + grant notify + deploy notes
5. PROD cutover; then Civic consumer work
