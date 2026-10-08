# Git-managed static pages

Curated **Basic page** (`page`) titles and bodies live here as Markdown with
YAML frontmatter. Git is the source of truth; import into Drupal Entity
Translation after deploy. Google on-view auto-translation is disabled for
`page` nodes (see `docs/static-pages.md`).

## Layout

```text
content/pages/
  manifest.yml          # allowlist of path aliases
  <alias>/
    en.md
    sk.md
    …
```

Each language file:

```markdown
---
title: Page title
format: filtered_html
---

Body in Markdown and/or raw HTML.
```

- Directory name = Drupal path alias (e.g. `rules` → `/rules`).
- Filename stem = langcode (`sk.md` → Slovak).
- First export may leave existing Drupal HTML inside the Markdown body
  (valid CommonMark). Tidy to idiomatic Markdown in later PRs if you want.

## Workflow

From Drupal root (e.g. `/var/www/rovdev`), with `drush74` on `PATH`:

1. **Seed / refresh files from DB** (first time, or to re-export):

```bash
scripts/export-static-pages-all.sh dev.rovas.app content/pages discover
```

`discover` fills `manifest.yml` from published `page` nodes that have a path
alias, then writes `.md` files. Subsequent exports without `discover` only
touch aliases already listed in the manifest.

2. Edit `.md` files in Git; commit and deploy/rsync.

3. **Import into Drupal** (after deploy):

```bash
scripts/import-static-pages-all.sh dev.rovas.app replace yes
```

Arguments: `[uri] [replace|keep] [yes|no clear-cache] [pages-dir]`.

- `replace` (default): overwrite title/body for languages present on disk.
- `keep`: only fill empty title/body for that language.
- Missing language files are skipped (existing ET values left alone).
- Missing alias/node fails the import for that alias.

## Related

- UI / interface strings: [`translations/`](../../translations/README.md) (PO files).
- Deploy notes: [`docs/static-pages.md`](../../docs/static-pages.md).
