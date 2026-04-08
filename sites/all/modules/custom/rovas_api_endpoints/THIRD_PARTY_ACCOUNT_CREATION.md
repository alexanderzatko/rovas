# Third‑party apps: Create (or link) a Rovas account

This document describes how a partner / 3rd‑party app can create a Rovas account for an end user **from the partner backend**, using the Rovas Services REST API.

Sources:
- `sites/all/modules/custom/rovas_site_rules/README.txt` (section: “Third-party app: creating Rovas accounts (rules_create_user)”)
- `sites/all/modules/custom/rovas_api_endpoints/rovasApi.yml` (OpenAPI: endpoints `POST /rovas/rovas_partner_user`, `POST /rovas/rules/rules_create_user`, `GET /rovas/rovas_hybridauth_providers.json`)

## Key points (read first)

- **No anonymous signup**: end users should not call Rovas directly to register.
- **Partner backend only**: your server calls the Rovas API on behalf of your end user.
- **Technical account**: a Rovas admin must grant your dedicated Rovas technical user the **`api client`** role and generate API credentials for the `rovas_endpoint` endpoint.
- **Per-user credentials**: the account‑creation call returns **end-user** credentials (`api_key`, `token`) that you will use for subsequent calls on behalf of that user.

## 1) Obtain partner (technical account) credentials

1. A Rovas administrator creates/chooses a dedicated technical user for your integration.
2. Assign the **`api client`** role to that technical user.
3. That user opens their Rovas profile → **Rovas API** tab and generates credentials for endpoint `rovas_endpoint`.
4. Store these secrets securely in your backend:
   - Header: `API-KEY`
   - Header: `TOKEN`

## 2) (Optional) List allowed HybridAuth providers

If your integration supports multiple identity providers, fetch the allowed provider IDs:

- **Method**: `GET`
- **Path**: `/rovas/rovas_hybridauth_providers.json`
- **Auth**: usually **public** (no `API-KEY`/`TOKEN`) if the Services endpoint has “Disable API keys authentication” enabled for this resource operation. If not public on your environment, send the same headers as other authenticated calls.

Response shape:

```json
{
  "status": "success",
  "providers": [
    { "id": "Nabezky", "name": "Nabezky" }
  ]
}
```

Use `providers[].id` as the `hybridauth_provider` request field in the create/link call below.

## 3) Create or link a user (preferred endpoint)

Prefer the native Services resource:

- **Method**: `POST`
- **Path**: `/rovas/rovas_partner_user`
- **Headers** (required):
  - `API-KEY: <partner technical account api key>`
  - `TOKEN: <partner technical account token>`
  - `Accept: application/json`
  - `Content-Type: application/json`

### Request body

Send JSON with keys matching `RulesCreateUserRequest` (OpenAPI schema).

Required:
- **`user_name`** *(string)*: display name for the Rovas user
- **`user_id`** *(integer)*: your external user id (stored as HybridAuth `provider_identifier`)
- **`user_email`** *(email string)*: used to find/link an existing Rovas user by email
- **`user_country`** *(string)*: **two uppercase ASCII letters** (ISO 3166‑1 alpha‑2), must exist in site country list (e.g. `"SK"`)
- **`user_language`** *(string)*: language code; normalized to an **enabled** Drupal interface language (rule maps `sk/cs → sk`, default `en`)
- **`server_url`** *(uri string)*: your base URL used to build HybridAuth profile URLs (e.g. `"https://partner.example/"`)

Optional:
- **`hybridauth_provider`** *(string|null)*: provider machine name; omit/empty → defaults to **`Nabezky`**; must be enabled on the site (use providers list above)
- **`partner_client_id`** *(string|null)*: optional metadata stored on the user (`rovas_partner_client_id`)

### Success response

On success you get the Rovas user id plus **end-user** API credentials for `rovas_endpoint`:

```json
{
  "rovas_uid": 123,
  "api_key": "…",
  "token": "…"
}
```

Store `api_key` and `token` and use them as `API-KEY` / `TOKEN` headers when calling other Rovas endpoints on behalf of this user.

### New account vs existing email behavior

- **Email not found**: creates a new Rovas account, links the HybridAuth identity, triggers the usual post‑registration population, and sends the welcome email.
- **Email already exists**: links the identity if missing and returns credentials for the existing Rovas user. (No second welcome email for this path unless custom logic is added elsewhere.)

### Errors you should handle

For `POST /rovas/rovas_partner_user`:
- **400**: validation error (country code casing, language, URL, email, provider, etc.)
- **401**: missing/invalid `API-KEY` or `TOKEN`
- **403**: technical account lacks permission (`access services_rules rules_create_user`; granted to `api client` / administrator roles)
- **500**: server or registration failure

### Minimal cURL example

```bash
curl -sS -X POST "https://rovas.app/rovas/rovas_partner_user" \
  -H "Accept: application/json" \
  -H "Content-Type: application/json" \
  -H "API-KEY: <API_KEY_OF_API_CLIENT_TECHNICAL_ACCOUNT>" \
  -H "TOKEN: <TOKEN_OF_API_CLIENT_TECHNICAL_ACCOUNT>" \
  -d '{
    "user_name": "Jane Partner User",
    "user_id": 12345,
    "user_email": "jane@example.com",
    "user_country": "SK",
    "user_language": "en",
    "server_url": "https://partner.example/",
    "hybridauth_provider": "Nabezky",
    "partner_client_id": "partner-tenant-42"
  }'
```

## 4) Legacy endpoint (only if you must)

The older Services Rules bridge endpoint is still documented:

- **Method**: `POST`
- **Path**: `/rovas/rules/rules_create_user`

It uses the same headers and request body keys and returns the same response fields, but the native `/rovas/rovas_partner_user` endpoint is preferred for stricter validation and easier evolution.

