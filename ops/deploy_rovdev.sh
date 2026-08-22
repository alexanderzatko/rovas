#!/usr/bin/env bash
set -euo pipefail

# Deploy local Rovas tree to the *dev* docroot on the VPS.
# Usage:
#   ops/deploy_rovdev.sh <ssh_target>
# Example:
#   ops/deploy_rovdev.sh root@magura
#
# Prod deploy remains ops/deploy_magura.sh → /var/www/rovas (rovas.app).
# Dev site: https://dev.rovas.app → /var/www/rovdev
# Dev notes (email logging, settings.local.php): docs/dev-setup.md

TARGET=${1:?"Provide ssh target (e.g. root@magura)"}
DEST=/var/www/rovdev

rsync -az --delete \
  --exclude '.git/' \
  --exclude 'sites/*/files/' \
  --exclude 'sites/*/private/' \
  --exclude 'sites/*/tmp/' \
  --exclude 'sites/*/settings.php' \
  ./ "$TARGET:$DEST/"

ssh "$TARGET" "cd '$DEST' && drush74 updb -y && drush74 cc all -y"
echo "Deployed to $TARGET:$DEST (dev.rovas.app)"
