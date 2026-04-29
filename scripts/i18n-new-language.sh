#!/usr/bin/env bash

set -euo pipefail

# Bootstrap translation files for a new/existing language.
#
# Usage:
#   scripts/i18n-new-language.sh [uri] <langcode> [english-name] [translations-dir]
#
# Examples:
#   scripts/i18n-new-language.sh dev.rovas.app cs Czech
#   scripts/i18n-new-language.sh dev.rovas.app de German

URI="${1:-dev.rovas.app}"
LANGCODE="${2:-}"
LANGNAME="${3:-}"
TRANSLATIONS_DIR="${4:-translations}"
PO_DIR="${TRANSLATIONS_DIR}/po"

if [ -z "${LANGCODE}" ]; then
  echo "Usage: scripts/i18n-new-language.sh [uri] <langcode> [english-name]" >&2
  exit 1
fi

if ! command -v drush74 >/dev/null 2>&1; then
  echo "Error: drush74 not found in PATH." >&2
  exit 1
fi

echo "== i18n new language bootstrap =="
echo "URI: ${URI}"
echo "Language: ${LANGCODE}"

EXISTS="$(drush74 --uri="${URI}" sqlq "SELECT COUNT(*) FROM languages WHERE language='${LANGCODE}';" | tr -d '\r' || echo 0)"
if [ "${EXISTS}" = "0" ]; then
  echo "Language ${LANGCODE} is not enabled yet."
  if [ -n "${LANGNAME}" ]; then
    echo "Trying to add language with Drush..."
    if drush74 --uri="${URI}" language-add "${LANGCODE}" --name="${LANGNAME}" >/dev/null 2>&1; then
      echo "Added language ${LANGCODE} (${LANGNAME})."
    else
      echo "Could not auto-add language via Drush."
      echo "Enable language '${LANGCODE}' in Drupal UI, then rerun this command." >&2
      exit 1
    fi
  else
    echo "Provide the language name to auto-add, e.g.:" >&2
    echo "  scripts/i18n-new-language.sh ${URI} ${LANGCODE} \"Language Name\"" >&2
    echo "Or enable '${LANGCODE}' in Drupal UI first, then rerun." >&2
    exit 1
  fi
fi

mkdir -p "${PO_DIR}"

TEXTGROUPS="$(
  drush74 --uri="${URI}" sqlq "SELECT DISTINCT textgroup FROM locales_source ORDER BY textgroup;" | tr -d '\r'
)"

for GROUP in ${TEXTGROUPS}; do
  OUT="${PO_DIR}/${LANGCODE}.${GROUP}.po"
  echo "Exporting locale scaffold ${LANGCODE}/${GROUP} -> ${OUT}"
  drush74 --uri="${URI}" scr scripts/export-locale-po.php "${LANGCODE}" "${GROUP}" "${OUT}"
done

echo "Exporting custom-modules scaffold..."
drush74 --uri="${URI}" scr scripts/export-custom-module-po.php "${LANGCODE}" "${PO_DIR}/${LANGCODE}.custom-modules.po" "sites/all/modules/custom/"

echo "Exporting themes scaffold..."
drush74 --uri="${URI}" scr scripts/export-theme-po.php "${LANGCODE}" "${PO_DIR}/${LANGCODE}.themes.po" "sites/all/themes/"

echo "Done. Send ${PO_DIR}/${LANGCODE}*.po for translation/MT."
