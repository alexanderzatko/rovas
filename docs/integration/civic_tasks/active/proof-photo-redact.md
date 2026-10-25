---
slug: proof-photo-redact
status: active
owner: sano
updated: 2026-10-25
---

# Proof photo client-side redaction

## Goal

Proof photos are public. Uploaders should black out sensitive areas **before**
the image is stored, using paint tools on the client (Rovas web and Civic app).
Server-side AI auto-redaction was retired (unreliable bounding boxes on arbitrary
proofs).

## Rovas web (this repo)

- After the first proof photo is attached on the work-report form, show:

  > Proofs of work are visible to the public, so we encourage you to review and
  > redact sensitive information from the images using the paint tools provided
  > here.

- Per photo: **Black out areas** opens a canvas editor with variable marker
  thickness, undo, clear, apply (overwrites the managed file in place).

## Civic API

- `photos[]` on `POST /rovas/civic_create_work_report` are stored as sent.
- `photo_redact_mode` is **ignored** if present (backward compatible).
- `photos_edited` / `photo_edits` are no longer emitted.

## Civic app (Expo — outside this repo)

- [ ] Same public-proofs note after first photo attach
- [ ] Paint / blackout tool (variable thickness) before upload
- [ ] Upload already-painted bytes in `photos[]`

## Checklist

1. [x] Remove AI vision auto-redact from Rovas web + Civic create path
2. [x] Web form: note + blackout paint editor
3. [ ] Civic Expo: note + paint before upload
4. [x] Docs / API reference updated

## Deploy / test (web)

```bash
# Deploy rovas_report_form (+ civic volunteer.inc / rovas_ai cleanup)
# Clear caches (menu for /rovas/report-photo-paint)
drush cc all   # on rovdev when you deploy

# Manual: add WR → check Photograph → upload image → see note + Black out areas
# Paint → Apply → submit; confirm public file is painted
```
