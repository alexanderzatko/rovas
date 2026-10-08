# Git-managed static pages

Curated **Basic page** copy (titles + bodies and their Entity Translation
languages) is stored under [`content/pages/`](../content/pages/) as Markdown
with YAML frontmatter. UI/locale strings remain in
[`translations/`](../translations/README.md) (PO files).

Google on-view auto-translation is **disabled for `page` nodes**:

- Reaction rule `rules_translate_page_on_view` is inactive (`ACTIVE: false`).
- Component `rules_translate_node` no longer accepts bundle `page`
  (`project` / `work_report` unchanged).

After changing those JSON files, re-sync Rules from disk (you run this on the
server):

```bash
drush74 php-eval "module_load_include('inc', 'rovas_site_rules', 'rovas_site_rules.migration'); rovas_site_rules_migration_run();"
```

See [`rovas_site_rules/README.txt`](../sites/all/modules/custom/rovas_site_rules/README.txt).

## Deploy checklist

From Drupal root (typically `/var/www/rovdev` or `/var/www/rovas`):

1. Deploy / rsync the repo (includes `content/pages/`, scripts, Rules JSON).
2. If Rules JSON for page MT changed, run the migration command above.
3. Import static pages:

```bash
scripts/import-static-pages-all.sh dev.rovas.app replace yes
```

4. Smoke-check a few aliases (e.g. `/rules`, `/privacy`) in each language you
   maintain in Git.

## Seed from the database (one-time / refresh)

On an environment that has the current page copy (prod or a fresh clone):

```bash
cd /var/www/rovdev   # or the checkout you will commit from
scripts/export-static-pages-all.sh dev.rovas.app content/pages discover
```

Then commit `content/pages/manifest.yml` and the new
`content/pages/<alias>/*.md` files. Bodies may still be HTML inside Markdown
after the first export; that is expected.

Later exports (no discover) only refresh aliases listed in the manifest:

```bash
scripts/export-static-pages-all.sh dev.rovas.app content/pages manifest
```

Full edit workflow: [`content/pages/README.md`](../content/pages/README.md).
