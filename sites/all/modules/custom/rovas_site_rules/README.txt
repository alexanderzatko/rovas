Rovas Site Rules
================

This module provides default Rules configurations for the Rovas site. Rules are
stored as JSON files in the rules/ subdirectory and loaded via
hook_default_rules_configuration().

Rules excluded (provided by other modules):
- rules_check_or_add_time_shareholder_core (rovas_purchase_shares)
- rules_create_time_share (rovas_purchase_shares)

Rule status in the Rules UI:
  Rules are provided via hook_default_rules_configuration() so Entity API
  assigns them "Default" status. If a rule is edited in the UI it becomes
  "Overridden" and can be reverted to the JSON-file version at any time.

Re-importing all rules from the JSON files (migration)
--------------------------------------------------------
Use drush74 (not plain drush) on Rovas servers: the default "drush" binary may
use a different PHP or bootstrap context, so "drush pm-info" can show the wrong
status (e.g. module "not installed" while the admin UI shows it enabled). If
commands or counts do not match what you see in the browser, you are almost
certainly using the wrong Drush.

Use this after you change JSON files in rules/ and want the site to match them,
or when Rules entries for this module look wrong or out of date.

What it does (plain language):
  - Removes the saved copies of this module's rules from the database (only the
    rules that come from these JSON files; other modules' rules are untouched).
  - Re-loads every JSON file and writes fresh copies into the database with
    "Default" status in the Rules UI.

How to run it

  A) Drush on the server (recommended — one step, no batch progress page)
     From the Drupal root (the directory that contains index.php), run:

       drush74 php-eval "module_load_include('inc', 'rovas_site_rules', 'rovas_site_rules.migration'); rovas_site_rules_migration_run();"

     What you should see:
     - A short summary line with how many JSON files were processed and how many
       old database rows were removed. "0 rows removed" often means the rules
       were not in the database yet (for example after a fresh deploy); the
       second half of the process still re-imports from JSON.
     - If something is wrong, check Administration > Reports > Recent log
       messages for lines like "Skipped invalid rule" (type may show as
       rovas_site_rules). Those lines explain which JSON file failed and why.

  B) Browser (Devel > Execute PHP)
     Paste and run:

       module_load_include('inc', 'rovas_site_rules', 'rovas_site_rules.migration');
       rovas_site_rules_migration_run();

     The site will show a batch progress screen. Wait until it finishes; you
     should see a green status message when done. Do not close the tab early.

After a code deploy to the server
  - If the migration or the site behaves oddly right after uploading new files,
    clear caches (drush74 cc all) or ask your host to reload PHP (OPcache
    can briefly serve old code). Then run the migration again using method A or B.

"Maximum function nesting level" (e.g. on "drush74 cc all" or after migration):
  During cache clears, entity_defaults_rebuild() calls
  hook_default_rules_configuration(), which runs rules_import() for each of the
  139 JSON files. For complex rules this creates a deep — but finite — call
  stack. PHP itself handles this fine, but Xdebug's default nesting limit (100)
  is too low.

  Option 1 — raise the limit for the command:
    php -d xdebug.max_nesting_level=1000 $(which drush74) cc all

  Option 2 — disable Xdebug for CLI cache clears:
    php -d xdebug.mode=off $(which drush74) cc all

  Option 3 — raise the limit permanently in php.ini (CLI section):
    xdebug.max_nesting_level = 1000


Third-party app: creating Rovas accounts (rules_create_user)
----------------------------------------------------------
This flow is for integrators whose *backend* calls the Rovas Services REST API
with API credentials. End users do not call Rovas anonymously for signup.

Trust model
  - A Rovas administrator assigns the **api client** role to a dedicated Rovas
    user account used by the partner (automation / “app admin”), not to every
    end user.
  - That user opens their Rovas profile, **Rovas API** tab, and generates
    **API key** and **TOKEN** for the `rovas_endpoint` endpoint (see
    services_auth_apikeys).
  - The partner backend stores these secrets and sends them on each request:
    HTTP headers `API-KEY` and `TOKEN` (as configured for the endpoint).

Permission
  - The Services Rules bridge exposes Rules components as
    `POST /rovas/rules/{rule_name}` (path prefix `rovas` is the endpoint path).
  - Calling `rules_create_user` requires permission
    **access services_rules rules_create_user**, which is granted to the
    **api client** and **administrator** roles (verify under
    People > Permissions). The api client role is the intended choice for
    partner technical accounts.

HTTP request (conceptual)
  - Method: POST to the Services action for rule `rules_create_user`.
  - Headers: `API-KEY`, `TOKEN` (and `Content-Type` / `Accept` as required by
    your client).
  - Body: JSON with parameters matching the rule variables (see below).

Rule parameters (JSON keys map to Rules variable names)
  - **user_name** — Display name for the new Rovas user.
  - **user_id** — External user id at the partner (stored as HybridAuth
    provider_identifier; integer).
  - **user_email** — Email; used to find an existing Rovas account by mail.
  - **user_country** — Country reference as elsewhere on the site.
  - **user_language** — Language code; rule maps sk/cs to sk, default en.
  - **server_url** — Partner base URL; used to build profileURL for HybridAuth
    data (e.g. `https://partner.example/`).
  - **hybridauth_provider** — Optional. HybridAuth *provider* machine name
    (e.g. `Nabezky`). If omitted or empty, **Nabezky** is used for backward
    compatibility. The value must be **enabled** in HybridAuth (Administration
    > HybridAuth, or the `hybridauth_providers` variable). Validation uses
    `hybridauth_get_enabled_providers()` in code (`rovas_site_rules_hybridauth_provider_is_allowed()`).
    To allow a provider for API linking, enable it in HybridAuth first; optional
    `hook_rovas_site_rules_hybridauth_providers_alter()` can narrow the list.
  - **partner_client_id** — Optional arbitrary string stored under
    `$user->data['rovas_partner_client_id']` for your bookkeeping.

Response (provided variables returned to the client)
  - **rovas_uid** — Rovas user id.
  - **api_key** — `rovas_endpoint` API key for that user (after user_insert
    handling and key generation).
  - **token** — Matching token for `rovas_endpoint`.

New account vs existing email
  - If no user exists with the given **user_email**, a new account is created,
    HybridAuth identity is inserted for **hybridauth_provider** + external
    **user_id**, **rules_registered_user_populator** runs on user_insert
    (roles, system project, API keys, etc.), and **component_rules_send_welcome_email**
    sends the welcome mail (same as before).
  - If a user **already exists** with that email, the rule sets **rovas_uid** to
    that account, ensures API keys exist, links HybridAuth identity if missing
    for this provider, and returns the same **api_key** / **token** fields.
    No second welcome email is sent for the duplicate-email path unless you
    add separate logic (not part of this rule).

Email verification
  - Site-wide **user_email_verification** and registration settings still apply
    to *other* registration paths. This rule creates users via entity API and
    unblocks them; confirm your product expectations against Drupal
    user settings and any custom validation.

After deploy
  - Run `drush updatedb` (or update.php); `7002` removes the obsolete
    `rovas_site_rules_hybridauth_providers` variable (allowed providers now
    follow HybridAuth configuration).
  - Re-import the updated rule from JSON (migration script in this README) so
    the database copy matches `rules/rules_create_user.json`.

Public documentation
  - Keep external docs (e.g. NeoFund Rovas API pages) aligned with the
    parameters and headers above; they are not stored automatically in the DB.
