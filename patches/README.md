# Patches

This directory contains patches applied/required for this Drupal 7 codebase.

## Conventions
- Keep patch filenames stable and descriptive.
- If a patch is applied to code in the repo, keep the patch here as documentation of the delta.
- Rebase/refresh patches when upgrading the affected component.

## Patches present
- oauth2_server-php74-notices.patch (guard missing scope and non-router paths)
- CORE-entity.inc-hardening-1803048-63.patch (core hardening)
- 86287-translate-reset-password-page-global-xx.patch (custom/behavioral fix)
- commerce-stripe-pi-translate-please-wait.patch (card checkout submit button loading text)
