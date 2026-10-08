#!/usr/bin/env bash

set -euo pipefail

# Export curated Basic page nodes into content/pages/<alias>/<lang>.md
#
# Usage:
#   scripts/export-static-pages-all.sh [uri] [pages-dir] [manifest|discover]
# Examples:
#   scripts/export-static-pages-all.sh dev.rovas.app
#   scripts/export-static-pages-all.sh dev.rovas.app content/pages discover

URI="${1:-dev.rovas.app}"
PAGES_DIR="${2:-content/pages}"
MODE="${3:-manifest}"

if ! command -v drush74 >/dev/null 2>&1; then
  echo "Error: drush74 not found in PATH." >&2
  exit 1
fi

if [ "${MODE}" != "manifest" ] && [ "${MODE}" != "discover" ]; then
  echo "Error: mode must be 'manifest' or 'discover'." >&2
  exit 1
fi

echo "== static pages export start =="
echo "Using URI: ${URI}"
echo "Pages dir: ${PAGES_DIR}"
echo "Mode: ${MODE}"

drush74 --uri="${URI}" scr scripts/export-static-pages.php "${PAGES_DIR}" "${MODE}"

echo "== static pages export done =="
