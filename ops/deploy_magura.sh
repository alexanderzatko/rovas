#!/usr/bin/env bash
set -euo pipefail

# Usage:
#   ops/deploy_magura.sh <ssh_target>
# Example:
#   ops/deploy_magura.sh root@magura
#
# Apple Sign-In deps are Drupal libraries under sites/all/libraries/
# (php-jwt, phpseclib, constant_time_encoding). After rsync, this script
# ensures they exist on the server (download if missing) so Apple login
# does not depend on Composer Manager / sites/all/vendor.

TARGET=${1:?"Provide ssh target (e.g. root@magura)"}
DEST=/var/www/rovas

rsync -az --delete \
  --exclude '.git/' \
  --exclude 'sites/*/files/' \
  --exclude 'sites/*/private/' \
  --exclude 'sites/*/tmp/' \
  --exclude 'sites/*/settings.php' \
  --exclude 'sites/*/settings.local.php' \
  ./ "$TARGET:$DEST/"

# Ensure Apple Sign-In PHP libraries on the server (idempotent).
ssh "$TARGET" "bash -s" <<EOF
set -euo pipefail
LIB='$DEST/sites/all/libraries'
mkdir -p "\$LIB"
TMP=\$(mktemp -d)
trap 'rm -rf "\$TMP"' EXIT
fetch() {
  local name="\$1" url="\$2" marker="\$3"
  if [[ -f "\$LIB/\$name/\$marker" ]]; then
    echo "OK  \$name"
    return 0
  fi
  echo "GET \$name"
  curl -fsSL "\$url" -o "\$TMP/\$name.tgz"
  rm -rf "\$LIB/\$name"
  mkdir -p "\$LIB/\$name"
  tar xzf "\$TMP/\$name.tgz" -C "\$LIB/\$name" --strip-components=1
  test -f "\$LIB/\$name/\$marker"
  chown -R www-data:www-data "\$LIB/\$name"
  echo "OK  \$name"
}
fetch php-jwt 'https://github.com/firebase/php-jwt/archive/refs/tags/v6.10.0.tar.gz' 'src/JWT.php'
fetch phpseclib 'https://github.com/phpseclib/phpseclib/archive/refs/tags/3.0.58.tar.gz' 'phpseclib/Crypt/PublicKeyLoader.php'
fetch constant_time_encoding 'https://github.com/paragonie/constant_time_encoding/archive/refs/tags/v2.8.2.tar.gz' 'src/Encoding.php'
cd '$DEST' && drush74 updb -y && drush74 cc all -y
EOF

echo "Deployed to $TARGET:$DEST (rovas.app)"
