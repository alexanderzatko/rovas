#!/usr/bin/env python3
"""One-shot expander: make rovasApi.yml the dual-API OpenAPI SoT.

Already applied (OpenAPI 1.1.0). Re-running fails if OAuth paths already exist;
restore a classic-only YAML backup before using again. Prefer hand-editing
rovasApi.yml for incremental contract changes.
"""

from __future__ import annotations

from copy import deepcopy
from pathlib import Path

import yaml

ROOT = Path(__file__).resolve().parents[1]
YML = ROOT / "sites/all/modules/custom/rovas_api_endpoints/rovasApi.yml"

CLASSIC_SECURITY = [{"ApiKeyAuth": [], "TokenAuth": []}]
BEARER_SECURITY = [{"BearerAuth": []}]
PUBLIC_SECURITY: list = []

TAG_MAP = {
    "/rovas/node/{id}": "classic-nodes",
    "/rovas/rovas_partner_user": "classic-accounts",
    "/rovas/rules/rules_create_user": "classic-accounts",
    "/rovas/rules/rules_fetch_user_data": "classic-accounts",
    "/rovas/rules/rules_users_chrons": "classic-accounts",
    "/rovas/rules/rules_user_s_merits": "classic-accounts",
    "/rovas/rules/rules_check_enabled_language": "classic-accounts",
    "/rovas/rovas_hybridauth_providers.json": "classic-accounts",
    "/rovas/rules/rules_proxy_create_new_project": "classic-projects",
    "/rovas/rules/rules_proxy_delete_project": "classic-projects",
    "/rovas/rules/rules_proxy_create_work_report": "classic-projects",
    "/rovas/rules/rules_proxy_delete_work_report": "classic-projects",
    "/rovas/rules/rules_proxy_report_editability": "classic-projects",
    "/rovas/rules/rules_proxy_work_report_verifiability": "classic-projects",
    "/rovas/rules/rules_proxy_report_verification_status": "classic-projects",
    "/rovas/rules/rules_create_verification": "classic-projects",
    "/rovas/rules/rules_create_verification_with_acccess_token": "classic-projects",
    "/rovas/rules/rules_proxy_check_verifier_by_access_tokens": "classic-projects",
    "/rovas/rules/rules_proxy_reward_work_report": "classic-projects",
    "/rovas/rovas_verifications.json": "classic-projects",
    "/rovas/rovas_comment_notices.json": "classic-projects",
    "/rovas/rules/rules_proxy_check_or_add_shareholder": "classic-commerce",
    "/rovas/rules/rules_proxy_check_or_add_project_shareholder": "classic-commerce",
    "/rovas/rules/rules_proxy_purchase_product": "classic-commerce",
    "/rovas/rules/rules_proxy_create_aur": "classic-commerce",
    "/rovas/rules/rules_get_merits_for_projects": "classic-commerce",
    "/rovas/rules/rules_convert_euros_to_chrons": "classic-commerce",
    "/rovas/rules/rules_convert_euros_to_chrons_reverse": "classic-commerce",
    "/rovas/views/top_10_workers_in_a_category": "classic-views",
    "/rovas/views/project_merits_feed": "classic-views",
    "/rovas/views/projects_merits": "classic-views",
}

PUBLIC_CLASSIC = {
    ("/rovas/rovas_hybridauth_providers.json", "get"),
    ("/rovas/rules/rules_create_verification_with_acccess_token", "post"),
}

INFO_DESCRIPTION = """\
Rovas REST API for partners, plugins, and apps (including NeoFund Civic).

## Environments

| Environment | Base URL |
| ----------- | -------- |
| Production | `https://rovas.app` |
| Development | `https://dev.rovas.app` |

Service paths use the prefix `/rovas` (Drupal Services endpoint `rovas_endpoint`).
OAuth protocol paths use `/oauth2`.

Send `Accept: application/json` and, for POST/PUT bodies, `Content-Type: application/json`
unless an operation notes otherwise (OAuth token exchange uses form body).

## Authentication models

Do **not** mix models on one request.

| Model | How | Best for |
| ----- | --- | -------- |
| Classic | Headers `API-KEY` + `TOKEN` | Partner backends, website plugins, server-to-server |
| OAuth 2 (Bearer) | `Authorization: Bearer <access_token>` (scope `user_profile`) | Mobile / SPA with browser login (e.g. Civic) |
| Public | No credentials (API-key auth disabled on the resource) | Catalogs, some leaderboards, HybridAuth provider list |

Classic calls act as the owner of the API key pair. Bearer calls act as the
OAuth-authenticated Drupal user. Civic-oriented resources have API-key auth
disabled.

Contact: info@neofund.sk — partner access and new OAuth clients are provisioned
by NeoFund (do not reuse Civic's `client_id` from third-party apps).
"""

TAGS = [
    {"name": "classic-accounts", "description": "Classic API — accounts and identity (API-KEY + TOKEN)"},
    {"name": "classic-projects", "description": "Classic API — projects, work reports, verification"},
    {"name": "classic-commerce", "description": "Classic API — shares, purchases, conversions"},
    {"name": "classic-views", "description": "Classic API — leaderboard / feed Views"},
    {"name": "classic-nodes", "description": "Classic API — allowlisted node retrieve/update/delete"},
    {"name": "oauth-protocol", "description": "OAuth 2 protocol (authorize, token, revoke, introspect)"},
    {"name": "oauth-profile", "description": "OAuth / Civic — home, profile, preferences"},
    {"name": "oauth-users", "description": "OAuth / Civic — user search and community lookup"},
    {"name": "oauth-claims", "description": "OAuth / Civic — tax claims (Path 1) and national rate"},
    {"name": "oauth-projects", "description": "OAuth / Civic — project detail, charity catalog, gifts"},
    {"name": "oauth-volunteer", "description": "OAuth / Civic — volunteer work reports"},
    {"name": "oauth-verify", "description": "OAuth / Civic — verification and notifications"},
    {"name": "oauth-merit-index", "description": "OAuth / Civic — Merit index and spotlight (public)"},
]


def accept_header():
    return {
        "in": "header",
        "name": "Accept",
        "schema": {"type": "string"},
        "required": False,
        "example": "application/json",
    }


def bearer_param_note():
    return {
        "description": "API-key authentication is disabled on this resource. Use Bearer where required.",
    }


def op(
    *,
    tag: str,
    summary: str,
    description: str = "",
    security,
    parameters=None,
    request_body=None,
    responses=None,
    extra=None,
):
    out = {
        "tags": [tag],
        "summary": summary,
        "security": security,
    }
    if description:
        out["description"] = description
    if parameters:
        out["parameters"] = parameters
    if request_body is not None:
        out["requestBody"] = request_body
    if responses:
        out["responses"] = responses
    else:
        out["responses"] = {"200": {"description": "Success"}}
    if extra:
        out.update(extra)
    return out


def json_body(schema_ref: str, description: str = "", required: bool = True):
    body = {
        "required": required,
        "content": {
            "application/json": {
                "schema": {"$ref": schema_ref},
            }
        },
    }
    if description:
        body["description"] = description
    return body


def json_response(code: str, description: str, schema_ref: str = None, example=None):
    content = {}
    media = {}
    if schema_ref:
        media["schema"] = {"$ref": schema_ref}
    if example is not None:
        media["example"] = example
    if media:
        content = {"application/json": media}
    resp = {"description": description}
    if content:
        resp["content"] = content
    return {code: resp}


def build_oauth_paths():
    paths = {}

    # --- OAuth protocol ---
    paths["/oauth2/authorize"] = {
        "get": op(
            tag="oauth-protocol",
            summary="OAuth 2 authorization endpoint (browser)",
            description=(
                "Browser authorization with PKCE. Civic supports query extras "
                "`lang=sk|en` and `intent=login|register`.\n\n"
                "Civic client (reference): `client_id=civic`, public (no secret), "
                "grants `authorization_code` + `refresh_token`, PKCE `S256`, "
                "scope `user_profile`, redirect `civic://oauth/callback`."
            ),
            security=PUBLIC_SECURITY,
            parameters=[
                {"in": "query", "name": "response_type", "required": True, "schema": {"type": "string", "enum": ["code"]}},
                {"in": "query", "name": "client_id", "required": True, "schema": {"type": "string", "example": "civic"}},
                {"in": "query", "name": "redirect_uri", "required": True, "schema": {"type": "string", "example": "civic://oauth/callback"}},
                {"in": "query", "name": "scope", "required": False, "schema": {"type": "string", "example": "user_profile"}},
                {"in": "query", "name": "state", "required": False, "schema": {"type": "string"}},
                {"in": "query", "name": "code_challenge", "required": True, "schema": {"type": "string"}},
                {
                    "in": "query",
                    "name": "code_challenge_method",
                    "required": True,
                    "schema": {"type": "string", "enum": ["S256"]},
                },
                {"in": "query", "name": "lang", "required": False, "schema": {"type": "string", "enum": ["sk", "en"]}},
                {
                    "in": "query",
                    "name": "intent",
                    "required": False,
                    "description": "Land on login vs register.",
                    "schema": {"type": "string", "enum": ["login", "register"]},
                },
            ],
            responses={
                "302": {"description": "Redirect to login/consent or back to redirect_uri with code"},
            },
        )
    }

    paths["/oauth2/token"] = {
        "post": op(
            tag="oauth-protocol",
            summary="Exchange authorization code or refresh token",
            description="Public client + PKCE. Use `application/x-www-form-urlencoded` body.",
            security=PUBLIC_SECURITY,
            request_body={
                "required": True,
                "content": {
                    "application/x-www-form-urlencoded": {
                        "schema": {"$ref": "#/components/schemas/OAuthTokenRequest"},
                    }
                },
            },
            responses={
                **json_response("200", "Token pair", "#/components/schemas/OAuthTokenResponse"),
                "400": {"description": "Invalid grant or request"},
            },
        )
    }

    paths["/oauth2/revoke"] = {
        "post": op(
            tag="oauth-protocol",
            summary="Revoke access or refresh token",
            security=PUBLIC_SECURITY,
            request_body={
                "required": True,
                "content": {
                    "application/x-www-form-urlencoded": {
                        "schema": {
                            "type": "object",
                            "required": ["token"],
                            "properties": {
                                "token": {"type": "string"},
                                "token_type_hint": {
                                    "type": "string",
                                    "enum": ["access_token", "refresh_token"],
                                },
                                "client_id": {"type": "string", "example": "civic"},
                            },
                        }
                    }
                },
            },
            responses={"200": {"description": "Revoked (or already invalid)"}},
        )
    }

    paths["/oauth2/tokens/{access_token}"] = {
        "get": op(
            tag="oauth-protocol",
            summary="Introspect access token",
            description="Returns `user_id`, `client_id`, `expires`, `scope`. 404 if invalid.",
            security=PUBLIC_SECURITY,
            parameters=[
                {
                    "in": "path",
                    "name": "access_token",
                    "required": True,
                    "schema": {"type": "string"},
                }
            ],
            responses={
                **json_response("200", "Token metadata", "#/components/schemas/OAuthTokenIntrospection"),
                "404": {"description": "Invalid or unknown token"},
            },
        )
    }

    # --- Profile ---
    paths["/rovas/me"] = {
        "get": op(
            tag="oauth-profile",
            summary="Current user profile (Bearer)",
            description=(
                "Profile for the OAuth-authenticated user. Includes Merit/Chron totals, "
                "receipt history retention, ME sources share preference, and Trusted user notify state."
            ),
            security=BEARER_SECURITY,
            parameters=[accept_header()],
            responses={
                **json_response(
                    "200",
                    "Profile",
                    "#/components/schemas/MeResponse",
                    example={
                        "uid": 42,
                        "username": "example_user",
                        "display_name": "Example User",
                        "total_me": 1248.35,
                        "total_chr": 320.0,
                        "created": "2019-03-12T14:22:01+00:00",
                        "language": "sk",
                        "receipt_history_retention": None,
                        "me_sources_share": "everyone",
                        "trusted_user": False,
                        "trusted_notify": 0,
                    },
                ),
                "401": {"description": "Missing or invalid Bearer"},
            },
        )
    }

    paths["/rovas/contributions/aggregate"] = {
        "get": op(
            tag="oauth-profile",
            summary="Community Merit total",
            description="Public aggregate community Merit total (+ category placeholders).",
            security=PUBLIC_SECURITY,
            parameters=[accept_header()],
            responses={"200": {"description": "Aggregate Merits JSON"}},
        )
    }

    paths["/rovas/merit_distribution"] = {
        "get": op(
            tag="oauth-profile",
            summary="Merit histogram",
            description=(
                "Public Merit distribution histogram. Optional Bearer adds `user_*` fields "
                "for the authenticated caller."
            ),
            security=PUBLIC_SECURITY,
            parameters=[accept_header()],
            responses={"200": {"description": "Histogram JSON; may include user_* when Bearer present"}},
        )
    }

    paths["/rovas/civic_receipt_history_preference"] = {
        "post": op(
            tag="oauth-profile",
            summary="Set receipt history retention",
            description="Body: `{ \"retention\": \"keep\" | \"policy\" }` (90-day policy when `policy`).",
            security=BEARER_SECURITY,
            request_body=json_body("#/components/schemas/ReceiptHistoryPreferenceRequest"),
            responses={
                **json_response("200", "Updated preference", "#/components/schemas/ReceiptHistoryPreferenceResponse"),
                "401": {"description": "Missing or invalid Bearer"},
            },
        )
    }

    paths["/rovas/civic_me_sources_preference"] = {
        "post": op(
            tag="oauth-profile",
            summary="Set ME sources share preference",
            description="Who may see detailed tax/gift ME mix: `everyone` | `reciprocal` | `aggregate`.",
            security=BEARER_SECURITY,
            request_body=json_body("#/components/schemas/MeSourcesPreferenceRequest"),
            responses={
                **json_response("200", "Updated preference", "#/components/schemas/MeSourcesPreferenceResponse"),
                "401": {"description": "Missing or invalid Bearer"},
            },
        )
    }

    paths["/rovas/civic_give_chrons"] = {
        "post": op(
            tag="oauth-profile",
            summary="Convert Chrons to Merits 1:1 (A6)",
            description="Voluntary CHR→ME conversion. Body `{ \"amount\": <positive number> }`.",
            security=BEARER_SECURITY,
            request_body=json_body("#/components/schemas/GiveChronsRequest"),
            responses={
                **json_response("200", "Conversion result", "#/components/schemas/GiveChronsResponse"),
                "401": {"description": "Missing or invalid Bearer"},
                "422": {"description": "Invalid amount or insufficient_chrons"},
            },
        )
    }

    paths["/rovas/civic_dismiss_trusted_notify"] = {
        "post": op(
            tag="oauth-profile",
            summary="Dismiss Trusted user congratulations banner",
            description="Sets `trusted_notify` to `2`. Empty JSON body OK.",
            security=BEARER_SECURITY,
            request_body={
                "required": False,
                "content": {"application/json": {"schema": {"type": "object"}}},
            },
            responses={
                **json_response("200", "Notify state", "#/components/schemas/TrustedNotifyDismissResponse"),
                "401": {"description": "Missing or invalid Bearer"},
            },
        )
    }

    paths["/rovas/civic_merit_history"] = {
        "get": op(
            tag="oauth-profile",
            summary="Caller Merit ledger rows",
            security=BEARER_SECURITY,
            parameters=[accept_header()],
            responses={"200": {"description": "Merit history rows for the Bearer user"}, "401": {"description": "Missing or invalid Bearer"}},
        )
    }

    # --- Users ---
    paths["/rovas/users_search"] = {
        "get": op(
            tag="oauth-users",
            summary="User typeahead search",
            description=(
                "Param is **`query`**, not `q` (Drupal reserves `$_GET['q']` for the request path). "
                "Users with `total_me < 0` are omitted."
            ),
            security=BEARER_SECURITY,
            parameters=[
                accept_header(),
                {"in": "query", "name": "query", "required": True, "schema": {"type": "string"}},
            ],
            responses={"200": {"description": "Search hits (public fields)"}, "401": {"description": "Missing or invalid Bearer"}},
        )
    }

    paths["/rovas/users_lookup"] = {
        "get": op(
            tag="oauth-users",
            summary="Community profile by username",
            description=(
                "Exact username. Includes `nace_expertise`, tab aggregates (`top_charities`, "
                "`top_volunteer_projects`, …). ME-mix detail respects `me_sources_share`. "
                "Users with `total_me < 0` → 404. Optional `lang=sk|en`."
            ),
            security=BEARER_SECURITY,
            parameters=[
                accept_header(),
                {"in": "query", "name": "username", "required": True, "schema": {"type": "string"}},
                {"in": "query", "name": "lang", "required": False, "schema": {"type": "string", "enum": ["sk", "en"]}},
            ],
            responses={
                "200": {"description": "Public community profile"},
                "401": {"description": "Missing or invalid Bearer"},
                "404": {"description": "Unknown user or total_me < 0"},
            },
        )
    }

    # --- Claims ---
    paths["/rovas/rules/rules_proxy_claim_vat_receipt"] = {
        "post": op(
            tag="oauth-claims",
            summary="Claim eKasa VAT as donor Merits",
            description=(
                "Bearer-only Rules proxy. Split Merits across projects via `shares[]`. "
                "API-key auth is disabled on this resource."
            ),
            security=BEARER_SECURITY,
            request_body=json_body("#/components/schemas/ClaimVatReceiptRequest"),
            responses={
                **json_response("200", "Claim result", "#/components/schemas/ClaimVatReceiptResponse"),
                "401": {"description": "Missing or invalid Bearer"},
                "400": {"description": "Validation error"},
            },
        )
    }

    paths["/rovas/civic_claims"] = {
        "get": op(
            tag="oauth-claims",
            summary="Claim lists or receipt status",
            description=(
                "With `view=submitted|received`: claim lists (optional merchant / date / VAT filters). "
                "With `receipt_uid=`: returns `{ claimed, receipt_uid }` only."
            ),
            security=BEARER_SECURITY,
            parameters=[
                accept_header(),
                {
                    "in": "query",
                    "name": "view",
                    "required": False,
                    "schema": {"type": "string", "enum": ["submitted", "received"]},
                },
                {"in": "query", "name": "receipt_uid", "required": False, "schema": {"type": "string"}},
                {"in": "query", "name": "merchant", "required": False, "schema": {"type": "string"}},
                {"in": "query", "name": "date_from", "required": False, "schema": {"type": "string"}},
                {"in": "query", "name": "date_to", "required": False, "schema": {"type": "string"}},
            ],
            responses={"200": {"description": "Claim list or receipt status"}, "401": {"description": "Missing or invalid Bearer"}},
        )
    }

    paths["/rovas/civic_claims/summary"] = {
        "get": op(
            tag="oauth-claims",
            summary="Claim totals for caller",
            security=BEARER_SECURITY,
            parameters=[accept_header()],
            responses={"200": {"description": "Summary totals"}, "401": {"description": "Missing or invalid Bearer"}},
        )
    }

    paths["/rovas/civic_national_hourly_rate"] = {
        "get": op(
            tag="oauth-claims",
            summary="National hourly rate for ME formula",
            security=PUBLIC_SECURITY,
            parameters=[
                accept_header(),
                {"in": "query", "name": "country", "required": True, "schema": {"type": "string", "example": "SK"}},
            ],
            responses={"200": {"description": "Hourly rate for country"}},
        )
    }

    # --- Projects / charity ---
    paths["/rovas/civic_project/{nid}"] = {
        "get": op(
            tag="oauth-projects",
            summary="Project detail (Civic)",
            description=(
                "Includes `donate_iban`, org fields (`org_id`, `country`, `website_url`, `iban_verified`), … "
                "Anonymous allowed for NeoFund-approved public-benefit projects; otherwise Bearer / access rules apply. "
                "Optional `lang=`."
            ),
            security=PUBLIC_SECURITY,
            parameters=[
                accept_header(),
                {"in": "path", "name": "nid", "required": True, "schema": {"type": "integer"}},
                {"in": "query", "name": "lang", "required": False, "schema": {"type": "string", "enum": ["sk", "en"]}},
            ],
            responses={"200": {"description": "Project detail"}, "404": {"description": "Not found or not readable"}},
        )
    }

    paths["/rovas/civic_project_work_reports"] = {
        "get": op(
            tag="oauth-projects",
            summary="Work reports for a project",
            description="Anonymous allowed for approved public-benefit projects; otherwise Bearer. Optional `lang=`.",
            security=PUBLIC_SECURITY,
            parameters=[
                accept_header(),
                {"in": "query", "name": "project_nid", "required": True, "schema": {"type": "integer"}},
                {"in": "query", "name": "lang", "required": False, "schema": {"type": "string", "enum": ["sk", "en"]}},
            ],
            responses={"200": {"description": "Work report list (may include photo_urls[])"}},
        )
    }

    paths["/rovas/civic_project_shareholders"] = {
        "get": op(
            tag="oauth-projects",
            summary="Project shareholders",
            description="Anonymous allowed for approved public-benefit projects; otherwise Bearer.",
            security=PUBLIC_SECURITY,
            parameters=[
                accept_header(),
                {"in": "query", "name": "project_nid", "required": True, "schema": {"type": "integer"}},
            ],
            responses={"200": {"description": "Shareholder list"}},
        )
    }

    paths["/rovas/civic_charity_projects"] = {
        "get": op(
            tag="oauth-projects",
            summary="Approved public-benefit project catalog",
            security=PUBLIC_SECURITY,
            parameters=[accept_header()],
            responses={"200": {"description": "Charity project catalog"}},
        )
    }

    paths["/rovas/civic_create_charity_org"] = {
        "post": op(
            tag="oauth-projects",
            summary="Donor submits a public-benefit organization",
            description="Bearer required. SK orgs may auto-verify; claim email flow applies.",
            security=BEARER_SECURITY,
            request_body={
                "required": True,
                "content": {"application/json": {"schema": {"type": "object", "description": "Organization fields (name, country, identifiers, …)"}}},
            },
            responses={"200": {"description": "Created org / project linkage"}, "401": {"description": "Missing or invalid Bearer"}},
        )
    }

    # --- Volunteer ---
    paths["/rovas/civic_my_projects"] = {
        "get": op(
            tag="oauth-volunteer",
            summary="Recent projects the user reported on",
            security=BEARER_SECURITY,
            parameters=[accept_header()],
            responses={"200": {"description": "Project list"}, "401": {"description": "Missing or invalid Bearer"}},
        )
    }

    paths["/rovas/civic_projects_search"] = {
        "get": op(
            tag="oauth-volunteer",
            summary="Search publishable projects",
            description="`query` must be at least 3 characters.",
            security=BEARER_SECURITY,
            parameters=[
                accept_header(),
                {"in": "query", "name": "query", "required": True, "schema": {"type": "string", "minLength": 3}},
            ],
            responses={"200": {"description": "Matching projects"}, "401": {"description": "Missing or invalid Bearer"}},
        )
    }

    paths["/rovas/civic_nace_tree"] = {
        "get": op(
            tag="oauth-volunteer",
            summary="NACE activity tree",
            security=PUBLIC_SECURITY,
            parameters=[accept_header()],
            responses={"200": {"description": "NACE tree JSON"}},
        )
    }

    paths["/rovas/civic_create_project"] = {
        "post": op(
            tag="oauth-volunteer",
            summary="Create project (Bearer)",
            description="Body: `name`, `description`, optional `url`. Do not use classic API-KEY project rules from Civic.",
            security=BEARER_SECURITY,
            request_body=json_body("#/components/schemas/CivicCreateProjectRequest"),
            responses={
                **json_response("200", "Created project", "#/components/schemas/CivicCreateProjectResponse"),
                "401": {"description": "Missing or invalid Bearer"},
            },
        )
    }

    paths["/rovas/civic_create_work_report"] = {
        "post": op(
            tag="oauth-volunteer",
            summary="Create work report (Bearer)",
            description=(
                "Proof required: URL and/or photos and/or map (`field_location` GeoJSON Point, "
                "LineString, Polygon, or GeometryCollection). Clients should redact photos before "
                "upload; `photo_redact_mode` is ignored if present."
            ),
            security=BEARER_SECURITY,
            request_body=json_body("#/components/schemas/CivicCreateWorkReportRequest"),
            responses={
                **json_response("200", "Created work report", "#/components/schemas/CivicCreateWorkReportResponse"),
                "401": {"description": "Missing or invalid Bearer"},
                "400": {"description": "Validation error"},
            },
        )
    }

    paths["/rovas/civic_work_report_map_config"] = {
        "get": op(
            tag="oauth-volunteer",
            summary="Map widget config for location proof",
            security=BEARER_SECURITY,
            parameters=[accept_header()],
            responses={"200": {"description": "Basemap / centre / geometry types"}, "401": {"description": "Missing or invalid Bearer"}},
        )
    }

    paths["/rovas/civic_ai_nace"] = {
        "post": op(
            tag="oauth-volunteer",
            summary="AI NACE + title suggestion",
            description="Optional `rovas_ai` module. Clients should fall back to NACE tree when unavailable.",
            security=BEARER_SECURITY,
            request_body={
                "required": True,
                "content": {"application/json": {"schema": {"type": "object", "description": "Work description / context for suggestion"}}},
            },
            responses={"200": {"description": "Suggestion or unavailable"}, "401": {"description": "Missing or invalid Bearer"}},
        )
    }

    # --- Verify ---
    paths["/rovas/civic_pending_verifications"] = {
        "get": op(
            tag="oauth-verify",
            summary="Work reports assigned to caller to verify",
            security=BEARER_SECURITY,
            parameters=[
                accept_header(),
                {"in": "query", "name": "lang", "required": False, "schema": {"type": "string", "enum": ["sk", "en"]}},
            ],
            responses={"200": {"description": "Pending verification slots"}, "401": {"description": "Missing or invalid Bearer"}},
        )
    }

    paths["/rovas/civic_work_report/{nid}"] = {
        "get": op(
            tag="oauth-verify",
            summary="Verifier-facing work-report detail",
            security=BEARER_SECURITY,
            parameters=[
                accept_header(),
                {"in": "path", "name": "nid", "required": True, "schema": {"type": "integer"}},
                {"in": "query", "name": "lang", "required": False, "schema": {"type": "string", "enum": ["sk", "en"]}},
            ],
            responses={
                "200": {"description": "Work report detail for verifier"},
                "401": {"description": "Missing or invalid Bearer"},
                "403": {"description": "No assigned verification slot"},
            },
        )
    }

    paths["/rovas/civic_submit_verification"] = {
        "post": op(
            tag="oauth-verify",
            summary="Approve / reject / decline verification",
            security=BEARER_SECURITY,
            request_body=json_body("#/components/schemas/CivicSubmitVerificationRequest"),
            responses={"200": {"description": "Verification submitted"}, "401": {"description": "Missing or invalid Bearer"}},
        )
    }

    paths["/rovas/civic_pending_comment_notices"] = {
        "get": op(
            tag="oauth-verify",
            summary="Polled watched-comment notices",
            description="Same Comment Notify audience as email; poll-based (`rovas_comment_watch`). Classic twin: `GET /rovas/rovas_comment_notices.json`.",
            security=BEARER_SECURITY,
            parameters=[accept_header()],
            responses={"200": {"description": "Pending notices"}, "401": {"description": "Missing or invalid Bearer"}},
        )
    }

    # --- Merit index ---
    paths["/rovas/civic_merit_index"] = {
        "get": op(
            tag="oauth-merit-index",
            summary="Snapshot Merit leaderboard",
            description="Includes `by_category`, `by_source`, … Snapshot rebuilt periodically (cron).",
            security=PUBLIC_SECURITY,
            parameters=[accept_header()],
            responses={"200": {"description": "Leaderboard snapshot"}},
        )
    }

    paths["/rovas/civic_merit_index_countries"] = {
        "get": op(
            tag="oauth-merit-index",
            summary="Country filter options for Merit index",
            security=PUBLIC_SECURITY,
            parameters=[accept_header()],
            responses={"200": {"description": "Country list"}},
        )
    }

    paths["/rovas/civic_merit_spotlight"] = {
        "get": op(
            tag="oauth-merit-index",
            summary="Period leads by Civic area",
            security=PUBLIC_SECURITY,
            parameters=[
                accept_header(),
                {
                    "in": "query",
                    "name": "period",
                    "required": False,
                    "schema": {"type": "string", "enum": ["today", "day", "week", "month"]},
                },
            ],
            responses={"200": {"description": "Spotlight leads"}},
        )
    }

    paths["/rovas/civic_merit_period_index"] = {
        "get": op(
            tag="oauth-merit-index",
            summary="ME•ter period leaderboard",
            security=PUBLIC_SECURITY,
            parameters=[accept_header()],
            responses={"200": {"description": "Period leaderboard"}},
        )
    }

    return paths


def oauth_schemas():
    return {
        "OAuthTokenRequest": {
            "type": "object",
            "properties": {
                "grant_type": {
                    "type": "string",
                    "enum": ["authorization_code", "refresh_token"],
                },
                "client_id": {"type": "string", "example": "civic"},
                "redirect_uri": {"type": "string", "example": "civic://oauth/callback"},
                "code": {"type": "string", "description": "Required for authorization_code"},
                "code_verifier": {"type": "string", "description": "PKCE verifier for authorization_code"},
                "refresh_token": {"type": "string", "description": "Required for refresh_token"},
            },
            "required": ["grant_type", "client_id"],
        },
        "OAuthTokenResponse": {
            "type": "object",
            "properties": {
                "access_token": {"type": "string"},
                "token_type": {"type": "string", "example": "Bearer"},
                "expires_in": {"type": "integer"},
                "refresh_token": {"type": "string"},
                "scope": {"type": "string", "example": "user_profile"},
            },
        },
        "OAuthTokenIntrospection": {
            "type": "object",
            "properties": {
                "user_id": {"type": "integer", "description": "Drupal uid"},
                "client_id": {"type": "string"},
                "expires": {"type": "integer", "description": "Unix expiry"},
                "scope": {"type": "string"},
            },
        },
        "MeResponse": {
            "type": "object",
            "properties": {
                "uid": {"type": "integer"},
                "username": {"type": "string"},
                "display_name": {"type": "string"},
                "total_me": {"type": "number", "format": "float"},
                "total_chr": {"type": "number", "format": "float"},
                "created": {"type": "string", "format": "date-time"},
                "language": {"type": "string"},
                "receipt_history_retention": {
                    "description": "`null` | `keep` | `policy`",
                    "nullable": True,
                    "oneOf": [
                        {"type": "string", "enum": ["keep", "policy"]},
                        {"type": "null"},
                    ],
                },
                "me_sources_share": {
                    "type": "string",
                    "enum": ["everyone", "reciprocal", "aggregate"],
                },
                "trusted_user": {"type": "boolean"},
                "trusted_notify": {
                    "type": "integer",
                    "enum": [0, 1, 2],
                    "description": "0=no banner, 1=show, 2=dismissed",
                },
            },
        },
        "ReceiptHistoryPreferenceRequest": {
            "type": "object",
            "required": ["retention"],
            "properties": {
                "retention": {"type": "string", "enum": ["keep", "policy"]},
            },
        },
        "ReceiptHistoryPreferenceResponse": {
            "type": "object",
            "properties": {
                "receipt_history_retention": {"type": "string", "enum": ["keep", "policy"]},
            },
        },
        "MeSourcesPreferenceRequest": {
            "type": "object",
            "required": ["share"],
            "properties": {
                "share": {"type": "string", "enum": ["everyone", "reciprocal", "aggregate"]},
            },
        },
        "MeSourcesPreferenceResponse": {
            "type": "object",
            "properties": {
                "me_sources_share": {"type": "string", "enum": ["everyone", "reciprocal", "aggregate"]},
            },
        },
        "GiveChronsRequest": {
            "type": "object",
            "required": ["amount"],
            "properties": {
                "amount": {
                    "type": "number",
                    "format": "float",
                    "description": "Positive Chrons to convert (1:1 to Merits)",
                    "exclusiveMinimum": 0,
                },
            },
        },
        "GiveChronsResponse": {
            "type": "object",
            "properties": {
                "chrons_given": {"type": "number", "format": "float"},
                "merits_gained": {"type": "number", "format": "float"},
                "total_chr": {"type": "number", "format": "float"},
                "total_me": {"type": "number", "format": "float"},
            },
        },
        "TrustedNotifyDismissResponse": {
            "type": "object",
            "properties": {
                "trusted_user": {"type": "boolean"},
                "trusted_notify": {"type": "integer", "enum": [0, 1, 2]},
            },
        },
        "ClaimVatReceiptRequest": {
            "type": "object",
            "description": "eKasa VAT claim with optional Merit split across projects",
            "properties": {
                "receipt_uid": {"type": "string"},
                "shares": {
                    "type": "array",
                    "items": {
                        "type": "object",
                        "properties": {
                            "project_nid": {"type": "integer"},
                            "share": {"type": "number", "format": "float"},
                        },
                    },
                },
            },
        },
        "ClaimVatReceiptResponse": {
            "type": "object",
            "description": "Claim outcome (fields vary by implementation)",
            "properties": {
                "claimed": {"type": "boolean"},
                "receipt_uid": {"type": "string"},
                "merits": {"type": "number", "format": "float"},
            },
        },
        "CivicCreateProjectRequest": {
            "type": "object",
            "required": ["name", "description"],
            "properties": {
                "name": {"type": "string"},
                "description": {"type": "string"},
                "url": {"type": "string"},
            },
        },
        "CivicCreateProjectResponse": {
            "type": "object",
            "properties": {
                "nid": {"type": "integer"},
            },
        },
        "CivicCreateWorkReportRequest": {
            "type": "object",
            "required": ["project_nid", "date_started", "hours", "description", "nace_tid"],
            "properties": {
                "project_nid": {"type": "integer"},
                "date_started": {"type": "string", "description": "Work start date"},
                "hours": {"type": "number", "format": "float"},
                "description": {"type": "string"},
                "nace_tid": {"type": "integer"},
                "title": {"type": "string"},
                "web_url": {"type": "string", "description": "Work evidence URL"},
                "photos": {
                    "type": "array",
                    "items": {"type": "string"},
                    "description": "Base64 proof photos (client-redacted)",
                },
                "reward_kind": {"type": "string", "description": "e.g. merits vs chrons payout preference"},
                "field_location": {
                    "type": "object",
                    "description": "GeoJSON geometry: Point, LineString, Polygon, or GeometryCollection",
                },
                "usage_fee_project_id": {"type": "integer"},
                "usage_fee_rate": {"type": "number", "format": "float"},
                "photo_redact_mode": {
                    "type": "string",
                    "description": "Ignored if present; redact on the client before upload",
                },
            },
        },
        "CivicCreateWorkReportResponse": {
            "type": "object",
            "properties": {
                "nid": {"type": "integer"},
            },
        },
        "CivicSubmitVerificationRequest": {
            "type": "object",
            "properties": {
                "work_report_nid": {"type": "integer"},
                "decision": {
                    "type": "string",
                    "enum": ["approve", "reject", "decline"],
                },
                "comment": {"type": "string"},
            },
        },
    }


def strip_classic_header_params(operation: dict) -> None:
    """Drop explicit API-KEY/TOKEN header params; securitySchemes replace them."""
    params = operation.get("parameters")
    if not params:
        return
    kept = [
        p
        for p in params
        if not (
            isinstance(p, dict)
            and p.get("in") == "header"
            and p.get("name") in ("API-KEY", "TOKEN")
        )
    ]
    if kept:
        operation["parameters"] = kept
    else:
        operation.pop("parameters", None)


def main():
    with YML.open(encoding="utf-8") as f:
        doc = yaml.safe_load(f)

    doc["info"]["title"] = "Rovas API"
    doc["info"]["description"] = INFO_DESCRIPTION
    doc["info"]["version"] = "1.1.0"
    doc["tags"] = TAGS

    # Remap classic ops
    for path, methods in doc.get("paths", {}).items():
        if not isinstance(methods, dict):
            continue
        for method, operation in methods.items():
            if method.startswith("x-") or not isinstance(operation, dict):
                continue
            if path in TAG_MAP:
                operation["tags"] = [TAG_MAP[path]]
            key = (path, method)
            if key in PUBLIC_CLASSIC:
                operation["security"] = PUBLIC_SECURITY
            else:
                # Classic default: API key pair (views may still require keys depending on server config)
                operation["security"] = CLASSIC_SECURITY
            strip_classic_header_params(operation)

    # Merge OAuth paths (do not overwrite existing classic paths)
    oauth_paths = build_oauth_paths()
    for path, methods in oauth_paths.items():
        if path in doc["paths"]:
            raise SystemExit(f"Refusing to overwrite existing path: {path}")
        doc["paths"][path] = methods

    components = doc.setdefault("components", {})
    components["securitySchemes"] = {
        "ApiKeyAuth": {
            "type": "apiKey",
            "in": "header",
            "name": "API-KEY",
            "description": "From My account → Rovas API for endpoint `rovas_endpoint`.",
        },
        "TokenAuth": {
            "type": "apiKey",
            "in": "header",
            "name": "TOKEN",
            "description": "Paired with API-KEY for the same user.",
        },
        "BearerAuth": {
            "type": "http",
            "scheme": "bearer",
            "bearerFormat": "OAuth2 access_token",
            "description": "OAuth 2 access token with scope `user_profile` (Civic client uses PKCE).",
        },
    }
    schemas = components.setdefault("schemas", {})
    for name, schema in oauth_schemas().items():
        if name in schemas:
            raise SystemExit(f"Schema name collision: {name}")
        schemas[name] = schema

    # Enrich RetrieveNodeResponse example already present — leave as-is.

    class LiteralStr(str):
        pass

    def literal_representer(dumper, data):
        return dumper.represent_scalar("tag:yaml.org,2002:str", data, style="|")

    yaml.add_representer(LiteralStr, literal_representer)

    # Keep multiline descriptions as block scalars where we set them
    doc["info"]["description"] = LiteralStr(doc["info"]["description"])

    def walk_descriptions(obj):
        if isinstance(obj, dict):
            for k, v in obj.items():
                if k == "description" and isinstance(v, str) and "\n" in v:
                    obj[k] = LiteralStr(v)
                else:
                    walk_descriptions(v)
        elif isinstance(obj, list):
            for item in obj:
                walk_descriptions(item)

    walk_descriptions(doc)

    with YML.open("w", encoding="utf-8") as f:
        yaml.dump(
            doc,
            f,
            default_flow_style=False,
            sort_keys=False,
            allow_unicode=True,
            width=100,
        )

    # Stats
    n_paths = len(doc["paths"])
    n_ops = sum(
        1
        for methods in doc["paths"].values()
        for m, op in methods.items()
        if m in ("get", "post", "put", "delete", "patch") and isinstance(op, dict)
    )
    print(f"Wrote {YML} — {n_paths} paths, {n_ops} operations, version {doc['info']['version']}")


if __name__ == "__main__":
    main()
