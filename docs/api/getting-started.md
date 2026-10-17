# Getting started with the Rovas API

Rovas exposes a REST API on Drupal Services (`rovas_endpoint`). There are two
authentication models. Pick one based on your integration shape.

| Model | Headers | Best for |
| ----- | ------- | -------- |
| **Classic** | `API-KEY` + `TOKEN` | Partner backends, plugins, server-to-server |
| **OAuth 2 (Bearer)** | `Authorization: Bearer <access_token>` | First-party mobile (Civic) and other OAuth clients NeoFund provisions |

Do **not** mix models on the same call. Most classic Rules proxies require
API-key auth; Civic/OAuth resources have API-key auth **disabled** and expect
Bearer (or are public).

## Conventions

- **Content type:** send `Content-Type: application/json` and `Accept: application/json` unless noted.
- **Path prefix:** `/rovas/…` (endpoint path of `rovas_endpoint`).
- **Format suffix:** some clients append `.json` (e.g. `/rovas/rovas_hybridauth_providers.json`). Both forms may work depending on the REST server config.
- **Errors:** HTTP `4xx`/`5xx` with a JSON body when possible (`error`, `message`, or Services/Rules error payloads).
- **User identity:** classic calls act as the owner of the API key pair (or the end-user pair you store after account creation). Bearer calls act as the OAuth-authenticated Drupal user.

---

## Classic API (`API-KEY` + `TOKEN`)

### 1. Get credentials

1. Create or choose a dedicated Rovas user for your integration (or for each end user).
2. Open **My account → Rovas API** and generate keys for endpoint **`rovas_endpoint`**.
3. Store the **API key** and **TOKEN** securely. Send them on every authenticated request:

```http
API-KEY: <api_key>
TOKEN: <token>
```

### 2. Partner technical account (recommended for signup)

Partners that create or link accounts for end users need:

1. A dedicated technical user with the **`api client`** role (granted by a Rovas admin).
2. API credentials for that technical user.
3. Permission **`access services_rules rules_create_user`** (included for `api client`).

Preferred create/link endpoint:

`POST /rovas/rovas_partner_user`

Legacy equivalent: `POST /rovas/rules/rules_create_user`.

Success returns **end-user** credentials for subsequent calls:

```json
{
  "rovas_uid": 123,
  "api_key": "…",
  "token": "…"
}
```

Full field list, validation, and curl: [THIRD_PARTY_ACCOUNT_CREATION.md](../../sites/all/modules/custom/rovas_api_endpoints/THIRD_PARTY_ACCOUNT_CREATION.md).

### 3. Minimal authenticated call

```bash
curl -sS "https://rovas.app/rovas/rules/rules_fetch_user_data" \
  -H "Accept: application/json" \
  -H "Content-Type: application/json" \
  -H "API-KEY: $API_KEY" \
  -H "TOKEN: $TOKEN" \
  -d '{}'
```

(Exact body parameters vary by rule — see OpenAPI / [reference.md](reference.md).)

### 4. OpenAPI

Machine-readable classic contract:

[`rovasApi.yml`](../../sites/all/modules/custom/rovas_api_endpoints/rovasApi.yml)

---

## OAuth 2 / Bearer API

Used by the **Civic** mobile app and any other client NeoFund registers on the
`civic` OAuth2 server (PKCE public clients today).

### Endpoints

| Step | URL |
| ---- | --- |
| Authorize | `{base}/oauth2/authorize` |
| Token | `{base}/oauth2/token` |
| Revoke | `{base}/oauth2/revoke` |
| Introspect | `{base}/oauth2/tokens/{access_token}` |

### Civic client (current production shape)

| Field | Value |
| ----- | ----- |
| `client_id` | `civic` |
| Client secret | empty (public native client) |
| Grant types | `authorization_code`, `refresh_token` |
| PKCE | required (`S256`) |
| Default scope | `user_profile` |
| Redirect URI | `civic://oauth/callback` (+ optional Expo proxy) |

Query extras on authorize (Civic):

- `lang=sk|en` — force login/register locale
- `intent=login|register` — land on login vs register

### Token request (authorization code + PKCE)

```bash
curl -sS -X POST "https://rovas.app/oauth2/token" \
  -d grant_type=authorization_code \
  -d client_id=civic \
  -d redirect_uri=civic://oauth/callback \
  -d code=PASTE_CODE \
  -d code_verifier=$VERIFIER
```

Expect `access_token` and `refresh_token`. Call Services with:

```http
Authorization: Bearer <access_token>
```

### Example: current user profile

```bash
curl -sS "https://rovas.app/rovas/me" \
  -H "Accept: application/json" \
  -H "Authorization: Bearer $ACCESS_TOKEN"
```

### Accessing OAuth APIs as a new product

OAuth clients are not self-serve. Contact NeoFund to register a client, redirect
URIs, and scopes. Do not reuse the `civic` client id from third-party products.

Internal enablement notes: [civic.md](../integration/civic.md), [civic-deploy.md](../integration/civic-deploy.md).

---

## Choosing endpoints

| Goal | Prefer |
| ---- | ------ |
| Create/link users from a charity or partner site | Classic `POST /rovas/rovas_partner_user` |
| File projects / work reports from a desktop plugin | Classic Rules proxies (`rules_proxy_*`) |
| Mobile app with browser login | OAuth Bearer + `/rovas/me`, Civic resources |
| Public Merit leaderboards / charity catalog | OAuth-era public GETs (no Bearer required) |
| VAT receipt claim / split | Bearer `POST /rovas/rules/rules_proxy_claim_vat_receipt` |

Classic Rules proxies that identify the actor via API keys are **not** suitable
for Civic mobile (no per-user API key on device). Use the Bearer Civic resources
instead.
