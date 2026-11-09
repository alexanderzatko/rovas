#!/usr/bin/env bash
# Download Apple Sign-In PHP deps into sites/all/libraries/ (survives rsync).
# Safe to re-run; skips packages that are already present.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
LIB="$ROOT/sites/all/libraries"
mkdir -p "$LIB"
TMP="$(mktemp -d)"
trap 'rm -rf "$TMP"' EXIT

fetch() {
  local name="$1" url="$2" marker="$3" strip_dir="$4"
  if [[ -f "$LIB/$name/$marker" ]]; then
    echo "OK  $name (already present)"
    return 0
  fi
  echo "GET $name"
  curl -fsSL "$url" -o "$TMP/$name.tgz"
  rm -rf "$LIB/$name"
  mkdir -p "$LIB/$name"
  tar xzf "$TMP/$name.tgz" -C "$LIB/$name" --strip-components=1
  # If archive had a single top-level dir name different from $name, normalize.
  if [[ ! -f "$LIB/$name/$marker" && -d "$LIB/$name/$strip_dir" ]]; then
    shopt -s dotglob
    mv "$LIB/$name/$strip_dir"/* "$LIB/$name/" 2>/dev/null || true
    rmdir "$LIB/$name/$strip_dir" 2>/dev/null || true
  fi
  test -f "$LIB/$name/$marker"
  echo "OK  $name"
}

# PHP 7.4-compatible pins (JWT 7.x needs PHP 8+).
fetch php-jwt \
  'https://github.com/firebase/php-jwt/archive/refs/tags/v6.10.0.tar.gz' \
  'src/JWT.php' \
  'php-jwt-6.10.0'

fetch phpseclib \
  'https://github.com/phpseclib/phpseclib/archive/refs/tags/3.0.58.tar.gz' \
  'phpseclib/Crypt/PublicKeyLoader.php' \
  'phpseclib-3.0.58'

fetch constant_time_encoding \
  'https://github.com/paragonie/constant_time_encoding/archive/refs/tags/v2.8.2.tar.gz' \
  'src/Encoding.php' \
  'constant_time_encoding-2.8.2'

echo "Apple Sign-In libraries ready under $LIB"
