#!/usr/bin/env bash

set -euo pipefail

# Import all translations/po/*.po files using filename convention:
#   <langcode>.<group>.po
#
# Usage:
#   scripts/import-locale-po-all.sh [uri] [replace-mode] [clear-cache] [po-dir]
# Examples:
#   scripts/import-locale-po-all.sh dev.rovas.app replace yes
#   scripts/import-locale-po-all.sh dev.rovas.app keep no

URI="${1:-dev.rovas.app}"
MODE="${2:-replace}"   # replace|keep
CLEAR_CACHE="${3:-yes}" # yes|no
PO_DIR="${4:-translations/po}"

if ! command -v drush74 >/dev/null 2>&1; then
  echo "Error: drush74 not found in PATH." >&2
  exit 1
fi

if [ ! -d "${PO_DIR}" ]; then
  echo "Error: ${PO_DIR} directory not found."
  exit 1
fi

shopt -s nullglob
FILES=("${PO_DIR}"/*.po)
shopt -u nullglob

if [ ${#FILES[@]} -eq 0 ]; then
  echo "No .po files found in ${PO_DIR}."
  exit 0
fi

echo "Using URI: ${URI}"
echo "Import mode: ${MODE}"

for FILE in "${FILES[@]}"; do
  BASE="$(basename "${FILE}" .po)"
  LANG="${BASE%%.*}"
  GROUP_PART="${BASE#*.}"

  if [ -z "${LANG}" ]; then
    echo "Skipping malformed filename: ${FILE}"
    continue
  fi

  # Default textgroup when filename does not carry one.
  GROUP="default"
  if [ "${GROUP_PART}" != "${BASE}" ] && [ -n "${GROUP_PART}" ]; then
    GROUP="${GROUP_PART}"
  fi

  # Location-scoped files are for translator convenience and can contain
  # entries from multiple internal textgroups. Import them into default
  # to avoid creating ad-hoc textgroups.
  if [ "${GROUP}" = "custom-modules" ] || [ "${GROUP}" = "themes" ]; then
    GROUP="default"
  fi

  if [ "${MODE}" = "replace" ]; then
    echo "Importing ${FILE} -> ${LANG} (replace, group=${GROUP})"
    drush74 --uri="${URI}" scr scripts/import-locale-po.php "${LANG}" "${FILE}" replace "${GROUP}"
  else
    echo "Importing ${FILE} -> ${LANG} (keep, group=${GROUP})"
    drush74 --uri="${URI}" scr scripts/import-locale-po.php "${LANG}" "${FILE}" keep "${GROUP}"
  fi
done

if [ "${CLEAR_CACHE}" = "yes" ]; then
  echo "Clearing Drupal caches..."
  drush74 --uri="${URI}" cc all
fi

echo "Done."
