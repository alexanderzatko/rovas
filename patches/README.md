# Patches

This directory contains patches applied/required for this Drupal 7 codebase.

## Conventions
- Keep patch filenames stable and descriptive.
- If a patch is applied to code in the repo, keep the patch here as documentation of the delta.
- Rebase/refresh patches when upgrading the affected component.

## Patches present
- oauth2_server-php74-notices.patch (guard missing scope and non-router paths)
- oauth2_server-2859214-last-access-deadlock.patch (narrow last_access UPDATE + catch deadlock on concurrent token use)
- oauth2_server-3015065-pkce-support-7.patch (PKCE for Civic OAuth)
- hybridauth-destination-survive-oauth-roundtrip.patch (honor widget `destination=` after Google/FB; stash across provider callback)
- CORE-entity.inc-hardening-1803048-63.patch (core hardening)
- 86287-translate-reset-password-page-global-xx.patch (custom/behavioral fix)
- commerce-stripe-pi-translate-please-wait.patch (card checkout submit button loading text)
- services_rules-honor-allow-null-optional.patch (mark Rules params with allow null/optional as optional in Services so omitted body fields do not 401)
