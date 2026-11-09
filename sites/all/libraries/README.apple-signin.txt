Apple Sign-In PHP libraries
===========================

Required by HybridAuth Apple provider (via rovas_ha):

  php-jwt/                  firebase/php-jwt 6.10.x
  phpseclib/                phpseclib 3.0.x
  constant_time_encoding/   paragonie/constant_time_encoding 2.x

Fetch/update locally:

  ops/fetch_apple_signin_libs.sh

On-box deploys (`/usr/local/bin/deploy-rovas`, see ops/deploy-rovas) re-fetch
these into the target docroot after rsync --delete. Laptop→VPS scripts
(ops/deploy_magura.sh, ops/deploy_rovdev.sh) do the same. Apple login does
not depend on Composer Manager vendor packages.
