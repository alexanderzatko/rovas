# Translation files workflow (Drupal 7 + Potx)

This directory stores versioned translation artifacts:

- `po/` - importable language files (`*.po`) with `msgstr` translations.
- `templates/` - extracted templates (`*.pot`) generated from source code.

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

Examples for common groups:

```bash
cd /var/www/rovdev
drush74 --uri=dev.rovas.app scr scripts/export-locale-po.php fr default translations/po/fr.default.po
drush74 --uri=dev.rovas.app scr scripts/export-locale-po.php fr custom  translations/po/fr.custom.po
drush74 --uri=dev.rovas.app scr scripts/export-locale-po.php fr views   translations/po/fr.views.po
```

You can keep separate files per text group, or merge them later into one `fr.po` if you prefer.

### One-command export for all enabled languages

Use the wrapper script:

```bash
cd /var/www/rovdev
scripts/export-locale-po-all.sh dev.rovas.app
```

This exports all enabled languages for these groups by default:

- `default`
- `custom`
- `views`

Output files are written as:

- `translations/po/<langcode>.default.po`
- `translations/po/<langcode>.custom.po`
- `translations/po/<langcode>.views.po`

You can pass custom groups as the second argument:

```bash
cd /var/www/rovdev
scripts/export-locale-po-all.sh dev.rovas.app "default custom"
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
