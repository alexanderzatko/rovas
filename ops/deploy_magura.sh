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

# sites/*/files/ is excluded, so Composer Manager's lockfile survives rsync.
# Reinstall vendor packages afterward — rsync --delete would otherwise wipe
# Apple Sign-In deps (firebase/php-jwt, phpseclib) not present in the local tree.
# Rebuild composer.json, then update those packages into the lock/vendor
# (plain `composer install` will not add packages that are only in composer.json).
ssh "$TARGET" "cd '$DEST' && drush74 composer-json-rebuild && cd '$DEST/sites/default/files/composer' && composer config platform.php 7.4.33 && composer config audit.ignore PKSA-y2cr-5h3j-g3ys && composer update --no-interaction --prefer-dist --with-all-dependencies firebase/php-jwt phpseclib/phpseclib paragonie/constant_time_encoding && chown -R www-data:www-data '$DEST/sites/all/vendor' && cd '$DEST' && drush74 updb -y && drush74 cc all -y"
echo "Deployed to $TARGET:$DEST (rovas.app)"
