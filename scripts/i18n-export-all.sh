#!/usr/bin/env bash

set -euo pipefail

# One-command export of all translation files used by this project.
#
# Usage:
#   scripts/i18n-export-all.sh [uri] [translations-dir]
#
# What it exports:
# 1) Locale textgroups from DB -> translations/po/<lang>.<textgroup>.po
# 2) Custom-module location scoped -> translations/po/<lang>.custom-modules.po
# 3) Theme location scoped -> translations/po/<lang>.themes.po
# 4) Optional POT templates from source code -> translations/templates/custom-modules.pot

URI="${1:-dev.rovas.app}"
TRANSLATIONS_DIR="${2:-translations}"
PO_DIR="${TRANSLATIONS_DIR}/po"
TEMPLATES_DIR="${TRANSLATIONS_DIR}/templates"

echo "== i18n export start =="
echo "URI: ${URI}"

scripts/export-locale-po-all.sh "${URI}" all "${PO_DIR}"
scripts/export-custom-module-po-all.sh "${URI}" "sites/all/modules/custom/" "${PO_DIR}"
scripts/export-theme-po-all.sh "${URI}" "sites/all/themes/" "${PO_DIR}"

# POT template extraction is useful for MT tooling and source drift checks.
if command -v drush74 >/dev/null 2>&1; then
  drush74 --uri="${URI}" potx single --folder=sites/all/modules/custom >/dev/null 2>&1 || true
  if [ -f "general.pot" ]; then
    mkdir -p "${TEMPLATES_DIR}"
    mv -f general.pot "${TEMPLATES_DIR}/custom-modules.pot"
    echo "Updated ${TEMPLATES_DIR}/custom-modules.pot"
  fi
fi

echo "== i18n export done =="
