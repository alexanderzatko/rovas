#!/usr/bin/env bash
set -euo pipefail

# Usage:
#   ops/deploy_magura.sh <ssh_target>
# Example:
#   ops/deploy_magura.sh root@magura

TARGET=${1:?"Provide ssh target (e.g. root@magura)"}
DEST=/var/www/rovas

# Sync docroot, but do NOT overwrite runtime content and secrets.
rsync -az --delete \
  --exclude '.git/' \
  --exclude 'sites/*/files/' \
  --exclude 'sites/*/private/' \
  --exclude 'sites/*/tmp/' \
  --exclude 'sites/*/settings.php' \
  --exclude 'sites/*/settings.local.php' \
  ./ "$TARGET:$DEST/"

# First deploy after moving modules into contrib/custom requires cache rebuild.
ssh "$TARGET" "cd '$DEST' && drush74 updb -y && drush74 cc all -y"
