# Rovas API reference

The **complete** endpoint catalog (classic API-KEY + OAuth/Bearer) lives in
OpenAPI:

[`rovasApi.yml`](../../sites/all/modules/custom/rovas_api_endpoints/rovasApi.yml)

Do not maintain parallel markdown tables here — they drift. For human narrative
(credentials, partner signup, Civic client settings), see
[getting-started.md](getting-started.md).

## How to read the OpenAPI file

| OpenAPI field | Meaning |
| ------------- | ------- |
| `servers` | `https://dev.rovas.app/` and `https://rovas.app/` |
| `tags` | Grouping: `classic-*` vs `oauth-*` |
| `security` | `ApiKeyAuth`+`TokenAuth` (classic), `BearerAuth` (OAuth), `[]` (public) |
| `components.schemas` | Request/response bodies (classic Rules + key Civic surfaces) |

Auth labels used in prose elsewhere:

| Label | OpenAPI |
| ----- | ------- |
| **API-KEY** | `security: [ApiKeyAuth + TokenAuth]` — headers `API-KEY` + `TOKEN` |
| **Bearer** | `security: [BearerAuth]` — `Authorization: Bearer` scope `user_profile` |
| **Public** | `security: []` |
| **Optional Bearer** | Public operation; description notes extra `user_*` fields when Bearer is sent |

## Public site

NeoFund developer pages should render this file (vendored). Implementation
notes: [neofund-rovas-api-page-handoff.md](neofund-rovas-api-page-handoff.md).

## Related

- Usage: [getting-started.md](getting-started.md)
- Civic integration index: [../integration/civic.md](../integration/civic.md)
- Partner signup: [THIRD_PARTY_ACCOUNT_CREATION.md](../../sites/all/modules/custom/rovas_api_endpoints/THIRD_PARTY_ACCOUNT_CREATION.md)
