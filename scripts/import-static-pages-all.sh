#!/usr/bin/env bash

set -euo pipefail

# Import content/pages Markdown into Basic page Entity Translation fields.
#
# Usage:
#   scripts/import-static-pages-all.sh [uri] [replace|keep] [yes|no] [pages-dir]
# Examples:
#   scripts/import-static-pages-all.sh dev.rovas.app replace yes
#   scripts/import-static-pages-all.sh dev.rovas.app keep no content/pages

URI="${1:-dev.rovas.app}"
MODE="${2:-replace}"
CLEAR_CACHE="${3:-yes}"
PAGES_DIR="${4:-content/pages}"

if ! command -v drush74 >/dev/null 2>&1; then
  echo "Error: drush74 not found in PATH." >&2
  exit 1
fi

if [ "${MODE}" != "replace" ] && [ "${MODE}" != "keep" ]; then
  echo "Error: mode must be 'replace' or 'keep'." >&2
  exit 1
fi

if [ ! -d "${PAGES_DIR}" ]; then
  echo "Error: ${PAGES_DIR} directory not found." >&2
  exit 1
fi

echo "== static pages import start =="
echo "Using URI: ${URI}"
echo "Import mode: ${MODE}"
echo "Pages dir: ${PAGES_DIR}"

drush74 --uri="${URI}" scr scripts/import-static-pages.php "${PAGES_DIR}" "${MODE}"

if [ "${CLEAR_CACHE}" = "yes" ]; then
  echo "Clearing Drupal caches..."
  drush74 --uri="${URI}" cc all
fi

echo "== static pages import done =="
