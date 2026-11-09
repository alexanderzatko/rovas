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
# settings.local.php is excluded so rsync --delete cannot remove DevelMailLog.

TARGET=${1:?"Provide ssh target (e.g. root@magura)"}
DEST=/var/www/rovdev

rsync -az --delete \
  --exclude '.git/' \
  --exclude 'sites/*/files/' \
  --exclude 'sites/*/private/' \
  --exclude 'sites/*/tmp/' \
  --exclude 'sites/*/settings.php' \
  --exclude 'sites/*/settings.local.php' \
  ./ "$TARGET:$DEST/"

# sites/*/files/ is excluded, so Composer Manager's lockfile survives rsync.
# Reinstall vendor packages afterward — rsync --delete would otherwise wipe
# Apple Sign-In deps (firebase/php-jwt, phpseclib) not present in the local tree.
# Rebuild composer.json first so newly deployed module requires are picked up.
ssh "$TARGET" "cd '$DEST' && drush74 composer-json-rebuild && cd '$DEST/sites/default/files/composer' && composer install --no-interaction --prefer-dist && chown -R www-data:www-data '$DEST/sites/all/vendor' && cd '$DEST' && drush74 updb -y && drush74 cc all -y"
echo "Deployed to $TARGET:$DEST (dev.rovas.app)"
