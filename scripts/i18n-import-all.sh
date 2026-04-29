#!/usr/bin/env bash

set -euo pipefail

# One-command import of all .po files in translations/po.
#
# Usage:
#   scripts/i18n-import-all.sh [uri] [replace|keep] [yes|no] [po-dir]

URI="${1:-dev.rovas.app}"
MODE="${2:-replace}"
CLEAR_CACHE="${3:-yes}"
PO_DIR="${4:-translations/po}"

echo "== i18n import start =="
scripts/import-locale-po-all.sh "${URI}" "${MODE}" "${CLEAR_CACHE}" "${PO_DIR}"
echo "== i18n import done =="
