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
  scripts/jquery-mitigation.sh pilot-status
  scripts/jquery-mitigation.sh pilot-on    [--include-admin] [--no-migrate]
  scripts/jquery-mitigation.sh pilot-off

Subcommands:
  status         Print current jquery_update variables.
  stage-1-10     Move jquery_update default version to 1.10 (staging hardening).
  rollback-1-9   Revert jquery_update default version to 1.9.
  pilot-status   Print current rovas_jquery_pilot variables.
  pilot-on       Enable the jQuery 3 pilot override. Defaults to safest stage:
                   include_admin=0, use_migrate=1 (public-facing only, Migrate on).
                 --include-admin  Also override jQuery on admin pages.
                 --no-migrate     Do not load jQuery Migrate (final cutover form).
  pilot-off      Disable the jQuery 3 pilot override (rollback to baseline).

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

show_pilot_status() {
  echo "Current rovas_jquery_pilot variables:"
  run_drush vget rovas_jquery_pilot_enabled
  run_drush vget rovas_jquery_pilot_include_admin
  run_drush vget rovas_jquery_pilot_use_migrate
  run_drush vget rovas_jquery_pilot_jquery_url
  run_drush vget rovas_jquery_pilot_migrate_url
}

pilot_on() {
  local include_admin=0
  local use_migrate=1

  while [[ "${#}" -gt 0 ]]; do
    case "${1}" in
      --include-admin)
        include_admin=1
        ;;
      --no-migrate)
        use_migrate=0
        ;;
      *)
        echo "Error: unknown flag for pilot-on: ${1}" >&2
        usage
        exit 1
        ;;
    esac
    shift
  done

  echo "Enabling jQuery 3 pilot override (include_admin=${include_admin}, use_migrate=${use_migrate})"
  run_drush vset --exact rovas_jquery_pilot_enabled 1 -y
  run_drush vset --exact rovas_jquery_pilot_include_admin "${include_admin}" -y
  run_drush vset --exact rovas_jquery_pilot_use_migrate "${use_migrate}" -y
  run_drush cc all
  show_pilot_status
}

pilot_off() {
  echo "Disabling jQuery 3 pilot override (rollback to baseline jquery_update)"
  run_drush vset --exact rovas_jquery_pilot_enabled 0 -y
  run_drush cc all
  show_pilot_status
}

main() {
  if [[ "${#}" -lt 1 ]]; then
    usage
    exit 1
  fi

  local cmd="${1}"
  shift

  require_drush

  case "${cmd}" in
    status)
      [[ "${#}" -eq 0 ]] || { usage; exit 1; }
      show_status
      ;;
    stage-1-10)
      [[ "${#}" -eq 0 ]] || { usage; exit 1; }
      stage_1_10
      ;;
    rollback-1-9)
      [[ "${#}" -eq 0 ]] || { usage; exit 1; }
      rollback_1_9
      ;;
    pilot-status)
      [[ "${#}" -eq 0 ]] || { usage; exit 1; }
      show_pilot_status
      ;;
    pilot-on)
      pilot_on "$@"
      ;;
    pilot-off)
      [[ "${#}" -eq 0 ]] || { usage; exit 1; }
      pilot_off
      ;;
    *)
      usage
      exit 1
      ;;
  esac
}

main "$@"
