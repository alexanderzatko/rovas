# Trusted user status — deploy notes

**Deferred design (not scheduled):** government ID (slovensko.sk / eID) as a
third Trusted eligibility path — [`trusted-user-eid.md`](trusted-user-eid.md).

Local code is ready. On each environment (typically **dev** first, then **prod**):

1. Deploy / rsync the repo (including `rovas_user`, `rovas_verify`, `rovas_site_rules` JSON, `translations/po`).
2. Run Drupal updates so new fields are created:
   ```bash
   drush74 updb -y
   ```
   Expect `rovas_user_update_7001` (trusted user fields + `field_auto_approved` on work reports).
3. Rebuild / re-import default Rules so the updated JSON is live (site-specific process), e.g. the existing `rovas_site_rules` migration / `entity_defaults_rebuild` path used for other rule changes. Affected configs:
   - `rules_work_report_created`
   - `rules_create_work_report`
   - `rules_proxy_create_work_report`
   - `rules_find_verifier`
   - `rules_ask_user_to_verify_report`
   - `rules_process_outstanding_verifications`
   - `rules_dispute_maximal_penalty`
4. Clear caches: `drush74 cc all`
5. Import English (and other locale) PO as you usually do for Rovas translations (`translations/po/en.custom-modules.po`, `en.field.po`, then CS/SK/IT when translated).
6. If `content/pages/` changed, import static page Markdown (see [`static-pages.md`](static-pages.md)):
   ```bash
   scripts/import-static-pages-all.sh dev.rovas.app replace yes
   ```
7. Smoke-test:
   - Profile → **Trusted user settings** visible (disabled until eligible).
   - Trusted + auto-approve → new published WR has status approved, `Auto-approved` shown, no new obligation slots.
   - Verification request email to a trusted user starts with the trusted notice.
   - Site-wide status message on grant; `/dismiss_trusted_notify` clears it.
   - Accused losing a dispute → compliance 0; trusted revoked only if eligibility fails.
   - Same-day proxy filings on a project with daily merge: second+ sessions merge into the auto-approved WR (hours/PoW updated); Chron tops up to `hours × 10` (`wr_grant:{nid}:auto:sync:{cents}`). Concurrent submits are serialized with a same-day lock; lock failure returns `created_wr_nid = -5`.

Also re-import / rebuild **`rules_proxy_create_work_report`** when deploying the auto-approved same-day merge change (lock + mergeability + Chron sync).
