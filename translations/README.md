# Translation files workflow (Drupal 7 + Potx)

This directory stores versioned translation artifacts:

- `po/` - importable language files (`*.po`) with `msgstr` translations.
- `templates/` - extracted templates (`*.pot`) generated from source code.

## Quickstart (easy mode)

From Drupal root (`/var/www/rovdev`):

1. Export everything to files:

```bash
scripts/i18n-export-all.sh dev.rovas.app
```

Export directly into another checkout (example staging git checkout):

```bash
scripts/i18n-export-all.sh dev.rovas.app /var/www/rovas-staging/translations
```

2. Bootstrap files for a new language (example `cs`):

```bash
scripts/i18n-new-language.sh dev.rovas.app cs Czech
```

3. Import all translated files back:

```bash
scripts/i18n-import-all.sh dev.rovas.app replace yes
```

Import from a custom PO directory:

```bash
scripts/i18n-import-all.sh dev.rovas.app replace yes /var/www/rovas-staging/translations/po
```

## Prerequisites

- Potx module enabled.
- Use `drush74` for this project.
- Run commands from Drupal root: `/var/www/rovdev`.

## Populate `templates/` (`.pot`)

Extract source strings from custom modules into a single template file:

```bash
cd /var/www/rovdev
drush74 --uri=dev.rovas.app potx single \
  --folder=sites/all/modules/custom
```

Potx writes output into the current working directory by default (for single mode, usually `general.pot`).
Move/rename it into this repo directory:

```bash
mv /var/www/rovdev/general.pot /var/www/rovdev/translations/templates/custom-modules.pot
```

## Populate `po/` (`.po`) from existing DB translations

Use the included Drush script to export translations from locale tables.
This produces real importable `.po` files and avoids Potx translation-export edge cases.

Export one language (French in this case) + one text group:

```bash
cd /var/www/rovdev
drush74 --uri=dev.rovas.app scr scripts/export-locale-po.php fr default translations/po/fr.default.po
```

Examples for common groups (group names are Drupal locale `textgroup` values):

```bash
cd /var/www/rovdev
drush74 --uri=dev.rovas.app scr scripts/export-locale-po.php fr default translations/po/fr.default.po
drush74 --uri=dev.rovas.app scr scripts/export-locale-po.php fr views   translations/po/fr.views.po
```

You can keep separate files per text group, or merge them later into one `fr.po` if you prefer.

### One-command export for all enabled languages

Use the wrapper script:

```bash
cd /var/www/rovdev
scripts/export-locale-po-all.sh dev.rovas.app
```

By default it exports all enabled languages for every Drupal `textgroup` that currently has translations in the DB.

Output files are written as:

- `translations/po/<langcode>.<textgroup>.po`

You can pass an explicit whitespace-separated list of textgroups as the second argument (e.g. to export only a subset):

```bash
cd /var/www/rovdev
scripts/export-locale-po-all.sh dev.rovas.app "default views taxonomy"
```

To see which `textgroup` values currently have translations (for enabled languages), you can run:

```bash
cd /var/www/rovdev
drush74 --uri=dev.rovas.app sqlq "SELECT DISTINCT s.textgroup FROM locales_target t JOIN locales_source s ON s.lid=t.lid WHERE t.language IN (SELECT language FROM languages WHERE enabled=1) ORDER BY s.textgroup;"
```

## Export only custom module strings (for translation/MT)

If you need only strings whose source location is in custom modules,
use the dedicated exporter (location filter):

```bash
cd /var/www/rovdev
drush74 --uri=dev.rovas.app scr scripts/export-custom-module-po.php sk translations/po/sk.custom-modules.po
```

Bulk all enabled languages:

```bash
cd /var/www/rovdev
scripts/export-custom-module-po-all.sh dev.rovas.app
```

Optional custom location prefix:

```bash
cd /var/www/rovdev
scripts/export-custom-module-po-all.sh dev.rovas.app sites/all/modules/custom/
```

## Export only theme strings (for translation/MT)

If you need only strings whose source location is in theme files,
use the dedicated exporter:

```bash
cd /var/www/rovdev
drush74 --uri=dev.rovas.app scr scripts/export-theme-po.php sk translations/po/sk.themes.po
```

Bulk all enabled languages:

```bash
cd /var/www/rovdev
scripts/export-theme-po-all.sh dev.rovas.app
```

Optional custom location prefix:

```bash
cd /var/www/rovdev
scripts/export-theme-po-all.sh dev.rovas.app sites/all/themes/
```

## Import versioned `.po` files

```bash
cd /var/www/rovdev
drush74 --uri=dev.rovas.app language-import fr translations/po/fr.po --replace
drush74 --uri=dev.rovas.app cc all
```

### One-command import for all `.po` files

Import everything in `translations/po/*.po` using filename convention
`<langcode>.<group>.po`:

```bash
cd /var/www/rovdev
scripts/import-locale-po-all.sh dev.rovas.app replace yes
```

Arguments:

- `uri` (default: `dev.rovas.app`)
- `replace-mode` (`replace` or `keep`, default: `replace`)
- `clear-cache` (`yes` or `no`, default: `yes`)

## Recommended repeatable flow

1. Update code strings in custom modules.
2. Regenerate `templates/custom-modules.pot`.
3. Regenerate/update language `po/<lang>.po`.
4. Commit `translations/templates/*.pot` and `translations/po/*.po`.

## About contrib modules (Drupal 7)

There is no single D7 contrib module that cleanly does this full
versioned file workflow (export-all scopes + machine-translation handoff + bulk re-import)
for locale interface strings and custom/theme file-location scoped strings together.

What helps in D7:

- `potx` - excellent for extracting source templates (`.pot`) from code.
- `l10n_client` - useful for in-context translation editing, less ideal for versioned file pipelines.

For your requirements, the script-based flow in this folder is the most predictable and CI-friendly.
