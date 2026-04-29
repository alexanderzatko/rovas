#!/usr/bin/env bash

set -euo pipefail

# Export locale DB translations for all enabled languages and selected groups.
#
# Usage:
#   scripts/export-locale-po-all.sh [uri] [groups]
# Examples:
#   scripts/export-locale-po-all.sh dev.rovas.app
#   scripts/export-locale-po-all.sh dev.rovas.app "default custom views"

URI="${1:-dev.rovas.app}"
GROUPS="${2:-default custom views}"

if ! command -v drush74 >/dev/null 2>&1; then
  echo "Error: drush74 not found in PATH." >&2
  exit 1
fi

echo "Using URI: ${URI}"
echo "Groups: ${GROUPS}"

LANGCODES="$(drush74 --uri="${URI}" sqlq "SELECT language FROM languages WHERE enabled = 1 ORDER BY weight, name;" | tr -d '\r')"

if [ -z "${LANGCODES}" ]; then
  echo "No enabled languages found."
  exit 0
fi

mkdir -p translations/po

for LANG in ${LANGCODES}; do
  for GROUP in ${GROUPS}; do
    OUT="translations/po/${LANG}.${GROUP}.po"
    echo "Exporting ${LANG} / ${GROUP} -> ${OUT}"
    drush74 --uri="${URI}" scr scripts/export-locale-po.php "${LANG}" "${GROUP}" "${OUT}"
  done
done

echo "Done."
