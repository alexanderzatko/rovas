Apple Sign-In PHP libraries
===========================

Required by HybridAuth Apple provider (via rovas_ha):

  php-jwt/                  firebase/php-jwt 6.10.x
  phpseclib/                phpseclib 3.0.x
  constant_time_encoding/   paragonie/constant_time_encoding 2.x

Fetch/update locally:

  ops/fetch_apple_signin_libs.sh

Production/dev deploy scripts also download these on the server if missing,
so Apple login does not depend on Composer Manager vendor packages.
