# Rovas API documentation

Public developer docs for NeoFund (`neofund.sk/rovas-api`) are published from
the NeoFund site content collection:

`~/Documents/Business/NeoFund_NP/web/site/src/content/pages/rovas-api/{en,sk}.md`

This folder is the **Rovas-repo source of truth** for contracts and usage.
When endpoints change, update these files first, then refresh the NeoFund
pages (EN + SK) so the public site stays aligned.

| Doc | Audience |
| --- | -------- |
| [getting-started.md](getting-started.md) | How to authenticate and call the APIs |
| [reference.md](reference.md) | Endpoint catalog (classic + OAuth/Bearer) |
| OpenAPI (classic) | [`sites/all/modules/custom/rovas_api_endpoints/rovasApi.yml`](../../sites/all/modules/custom/rovas_api_endpoints/rovasApi.yml) |
| Partner account creation | [`THIRD_PARTY_ACCOUNT_CREATION.md`](../../sites/all/modules/custom/rovas_api_endpoints/THIRD_PARTY_ACCOUNT_CREATION.md) |
| Civic / OAuth internals | [`../integration/civic.md`](../integration/civic.md) |

**Bases**

| Environment | Base URL |
| ----------- | -------- |
| Production | `https://rovas.app` |
| Development | `https://dev.rovas.app` |

All Services resources below are under the `rovas_endpoint` path prefix:
`{base}/rovas/…`.
