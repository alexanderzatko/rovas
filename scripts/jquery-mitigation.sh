#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
DRUSH_BIN="${DRUSH_BIN:-drush}"

usage() {
  cat <<'EOF'
Usage:
  scripts/jquery-mitigation.sh status
  scripts/jquery-mitigation.sh stage-1-10
  scripts/jquery-mitigation.sh rollback-1-9

Environment:
  DRUSH_BIN=/path/to/drush   Override drush executable.
EOF
}

require_drush() {
  if ! command -v "${DRUSH_BIN}" >/dev/null 2>&1; then
    echo "Error: '${DRUSH_BIN}' not found in PATH." >&2
    exit 1
  fi
}

run_drush() {
  "${DRUSH_BIN}" --root="${ROOT_DIR}" "$@"
}

show_status() {
  echo "Current jquery_update variables:"
  run_drush vget jquery_update_jquery_version
  run_drush vget jquery_update_jquery_admin_version
  run_drush vget jquery_update_jquery_cdn
}

stage_1_10() {
  echo "Applying staging hardening: jquery_update default -> 1.10"
  # IMPORTANT: must be stored as the string "1.10" (not float 1.1).
  run_drush vset --exact jquery_update_jquery_version "1.10" -y
  run_drush vset jquery_update_jquery_admin_version default -y
  run_drush cc all
  show_status
}

rollback_1_9() {
  echo "Rolling back to baseline: jquery_update default -> 1.9"
  run_drush vset jquery_update_jquery_version 1.9 -y
  run_drush vset jquery_update_jquery_admin_version default -y
  run_drush cc all
  show_status
}

main() {
  if [[ "${#}" -ne 1 ]]; then
    usage
    exit 1
  fi

  require_drush

  case "${1}" in
    status)
      show_status
      ;;
    stage-1-10)
      stage_1_10
      ;;
    rollback-1-9)
      rollback_1_9
      ;;
    *)
      usage
      exit 1
      ;;
  esac
}

main "$@"
