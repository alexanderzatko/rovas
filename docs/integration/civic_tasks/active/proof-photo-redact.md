---
status: active
slug: proof-photo-redact
owner_repo: both
created: 2026-10-25
updated: 2026-10-25
---

# Proof photo PII / face redaction

**Product:** When filing a work report with photo proof, users can opt in to have
Rovas detect and black out sensitive text (emails, phones, personal names) and
optionally faces/people before the file is stored publicly. Civic and the Rovas
web form show the same controls; after an edit, clients show
“Your image was edited by Rovas”. Unredacted originals are never written to
`public://` when redaction was requested (fail-closed if AI is unavailable).

**Owner:** `both` (Rovas API + web; Civic UI consumes the contract)

### Contract

#### Input — `POST /rovas/civic_create_work_report` (Bearer)

Existing body plus:

| Field | Type | Notes |
|-------|------|--------|
| `photo_redact_mode` | string | Optional. `none` (default) \| `text` \| `text_faces`. Applies to all `photos[]` in the request. |

When `photo_redact_mode` is `text` or `text_faces` and `rovas_ai` cannot redact
(not configured, budget exceeded, vision error), the request fails with
**502** `photo_redact_failed` and **no work report is created** (redaction runs
before Rules create). Client should retry or set mode to `none`.

#### Output — create WR response (additive)

When at least one photo had regions painted:

```json
{
  "work_report_nid": "123",
  "reward_kind": "chrons",
  "photos_edited": true,
  "photo_edits": [
    {
      "filename": "site_rovas_edited.jpg",
      "url": "https://dev.rovas.app/sites/default/files/civic_wr/site_rovas_edited.jpg",
      "mode": "text",
      "regions_redacted": 3,
      "message": "Your image was edited by Rovas"
    }
  ]
}
```

When mode ≠ `none` but nothing was found to paint: `photos_edited: false`,
`photo_edits: []` (optional clarity). When mode is `none`, omit these fields.

#### Output — WR list / detail (when `photo_urls` included)

Same `photos_edited` + `photo_edits[]` when rows exist in `{rovas_ai_photo_edit}`.

### Checklist

1. [x] `rovas_ai`: vision detect + GD paint; table `rovas_ai_photo_edit`; admin model override `proof_pii_redact`
2. [x] `rovas_civic`: `photo_redact_mode` on create; redact before public save; evidence bit 4; create + list/detail `photo_edits`
3. [x] `rovas_report_form`: opt-in checkboxes under Photos; status message
4. [ ] Deploy rovdev: enable/update `rovas_ai` (`drush updb` for `rovas_ai_update_7002`), configure vision model if needed
5. [ ] Civic UI: checkboxes under photo upload; send `photo_redact_mode`; show `photo_edits[].message`
6. [ ] Smoke (human)

### Smoke test

```bash
# Opt-in text redact (replace TOKEN / tiny JPEG base64)
curl -sS -X POST "https://dev.rovas.app/rovas/civic_create_work_report" \
  -H "Authorization: Bearer $TOKEN" \
  -H "Content-Type: application/json" \
  -d '{
    "project_nid": "…",
    "date_started": 1730000000,
    "hours": 1,
    "description": "Smoke test proof redact",
    "nace_tid": "…",
    "photo_redact_mode": "text",
    "photos": [{"filename":"proof.jpg","mime":"image/jpeg","data":"<base64>"}]
  }'
# Expect work_report_nid; if PII detected: photos_edited true + photo_edits[].message
# If AI down: 502 photo_redact_failed and no new WR
```

### Civic will consume

- Under photo upload: “Automatically remove” with checkboxes **Sensitive text** and
  **Sensitive text and faces** (mutually exclusive; default both off → `none`).
- Map to `photo_redact_mode`: `text` | `text_faces` | omit/`none`.
- After create (and when showing WR proof): if `photos_edited`, show
  `photo_edits[0].message` (or per-photo).
