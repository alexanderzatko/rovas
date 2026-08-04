<?php
$v = rtrim(strtr(base64_encode(random_bytes(32)), '+/', '-_'), '=');
$ch = rtrim(strtr(base64_encode(hash('sha256', $v, TRUE)), '+/', '-_'), '=');
$code = bin2hex(random_bytes(20));
$storage = new Drupal\oauth2_server\Storage();
$storage->setAuthorizationCode($code, 'civic', 1, 'civic://oauth/callback', REQUEST_TIME + 300, 'user_profile', NULL, $ch, 'S256');
file_put_contents('/tmp/pkce.env', "VERIFIER=$v\nCODE=$code\n");
echo "CODE=$code\nVERIFIER=$v\n";
