#!/usr/bin/env bash

set -euo pipefail

# Export theme-origin strings for all enabled languages.
#
# Usage:
#   scripts/export-theme-po-all.sh [uri] [location-prefix] [output-dir]
#
# Examples:
#   scripts/export-theme-po-all.sh dev.rovas.app
#   scripts/export-theme-po-all.sh dev.rovas.app sites/all/themes/

URI="${1:-dev.rovas.app}"
LOCATION_PREFIX="${2:-sites/all/themes/}"
OUTPUT_DIR="${3:-translations/po}"

if ! command -v drush74 >/dev/null 2>&1; then
  echo "Error: drush74 not found in PATH." >&2
  exit 1
fi

echo "Using URI: ${URI}"
echo "Location prefix: ${LOCATION_PREFIX}"

LANGCODES="$(drush74 --uri="${URI}" sqlq "SELECT language FROM languages WHERE enabled = 1 ORDER BY weight, name;" | tr -d '\r')"

if [ -z "${LANGCODES}" ]; then
  echo "No enabled languages found."
  exit 0
fi

mkdir -p "${OUTPUT_DIR}"

for LANG in ${LANGCODES}; do
  OUT="${OUTPUT_DIR}/${LANG}.themes.po"
  echo "Exporting ${LANG} -> ${OUT}"
  drush74 --uri="${URI}" scr scripts/export-theme-po.php "${LANG}" "${OUT}" "${LOCATION_PREFIX}"
done

echo "Done."
