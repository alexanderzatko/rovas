# Rovas API documentation

## Source of truth

**Complete endpoint reference (classic + OAuth/Civic):**

[`sites/all/modules/custom/rovas_api_endpoints/rovasApi.yml`](../../sites/all/modules/custom/rovas_api_endpoints/rovasApi.yml)

OpenAPI 3.0.1 — paths, auth (`security` / `securitySchemes`), parameters, and
schemas. When an endpoint or contract changes, update that file first.

| Doc | Role |
| --- | ---- |
| [rovasApi.yml](../../sites/all/modules/custom/rovas_api_endpoints/rovasApi.yml) | Machine-readable **complete** reference (classic + OAuth) |
| [getting-started.md](getting-started.md) | Narrative: which auth model, partner signup, Civic OAuth client |
| [reference.md](reference.md) | Short pointer to the OpenAPI file (no parallel endpoint tables) |
| [neofund-rovas-api-page-handoff.md](neofund-rovas-api-page-handoff.md) | Handoff for NeoFund site: render public docs from the YAML |
| [THIRD_PARTY_ACCOUNT_CREATION.md](../../sites/all/modules/custom/rovas_api_endpoints/THIRD_PARTY_ACCOUNT_CREATION.md) | Partner account creation deep-dive |
| [../integration/civic.md](../integration/civic.md) | Civic / OAuth internals and task index |

## Public developer page

NeoFund publishes `neofund.sk/rovas-api` (EN + SK) from:

`~/Documents/Business/NeoFund_NP/web/site/src/content/pages/rovas-api/{en,sk}.md`

Those pages should keep a **short localized narrative** and render the endpoint
catalog from a **vendored copy** of `rovasApi.yml` (see the handoff doc). Do not
maintain duplicate endpoint tables in markdown.

## Bases

| Environment | Base URL |
| ----------- | -------- |
| Production | `https://rovas.app` |
| Development | `https://dev.rovas.app` |

Services resources use the `rovas_endpoint` path prefix: `{base}/rovas/…`.
OAuth protocol paths use `{base}/oauth2/…`.
