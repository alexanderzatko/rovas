#!/usr/bin/env bash

set -euo pipefail

# Export locale DB translations for all enabled languages and selected groups.
#
# Usage:
#   scripts/export-locale-po-all.sh [uri] [groups]
# Examples:
#   scripts/export-locale-po-all.sh dev.rovas.app
#   scripts/export-locale-po-all.sh dev.rovas.app all
#   scripts/export-locale-po-all.sh dev.rovas.app "default views taxonomy"

URI="${1:-dev.rovas.app}"
# Avoid using the variable name "GROUPS" because some shells/environments define it
# as a special/internal array (it may not behave like a normal string).
TEXTGROUPS_STRING="${2:-all}"

if ! command -v drush74 >/dev/null 2>&1; then
  echo "Error: drush74 not found in PATH." >&2
  exit 1
fi

echo "Using URI: ${URI}"
if [ "${TEXTGROUPS_STRING}" = "all" ]; then
  echo "Groups: auto (detected from locales_target)"
else
  echo "Groups: ${TEXTGROUPS_STRING}"
fi

LANGCODES="$(drush74 --uri="${URI}" sqlq "SELECT language FROM languages WHERE enabled = 1 ORDER BY weight, name;" | tr -d '\r')"

if [ -z "${LANGCODES}" ]; then
  echo "No enabled languages found."
  exit 0
fi

mkdir -p translations/po

GROUP_LIST="${TEXTGROUPS_STRING}"
if [ "${TEXTGROUPS_STRING}" = "all" ]; then
  # Export every textgroup that currently has translations for any enabled language.
  GROUP_LIST="$(
    drush74 --uri="${URI}" sqlq "SELECT DISTINCT s.textgroup FROM locales_target t JOIN locales_source s ON s.lid=t.lid WHERE t.language IN (SELECT language FROM languages WHERE enabled=1) ORDER BY s.textgroup;" | tr -d '\r'
  )"
fi

for LANG in ${LANGCODES}; do
  # Shell word-splitting is intentional here: TEXTGROUPS_STRING is a whitespace-separated list.
  for GROUP in ${GROUP_LIST}; do
    OUT="translations/po/${LANG}.${GROUP}.po"
    echo "Exporting ${LANG} / ${GROUP} -> ${OUT}"
    drush74 --uri="${URI}" scr scripts/export-locale-po.php "${LANG}" "${GROUP}" "${OUT}"
  done
done

echo "Done."
