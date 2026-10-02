# Civic task files

Per-task specs for Civic ↔ Rovas work. The evergreen index is
[`../civic.md`](../civic.md).

## Layout

| Path | Purpose |
|------|---------|
| `_template.md` | Skeleton for new tasks |
| `active/` | Open work only |
| `done/` | Finished specs (contracts + checklists kept for history) |

## Naming

- File: `kebab-case-slug.md` (e.g. `merit-spotlight.md`)
- Frontmatter `slug` must match the filename without `.md`
- Prefer one concern per file

## Status

| `status` | Meaning |
|----------|---------|
| `active` | Ready to implement (file in `active/`) |
| `blocked` | Waiting on product / Civic / external (still in `active/`) |
| `done` | Implemented; file moved to `done/` |

## When to archive

After the checklist’s code items are done (deploy/smoke may remain for the
human): set `status: done`, move `active/<slug>.md` → `done/<slug>.md`, and
update the Task directory table in `civic.md`.

Do not append new `## Next work` sections to `civic.md`.
