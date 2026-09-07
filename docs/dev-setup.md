# Dev instance setup (`dev.rovas.app`)

Rovas runs a separate Drupal docroot for development. Prod is **not** touched
unless explicitly requested.

| | Dev | Prod |
|---|-----|------|
| URL | `https://dev.rovas.app` | `https://rovas.app` |
| Docroot | `/var/www/rovdev` | `/var/www/rovas` |
| Deploy | `ops/deploy_rovdev.sh <ssh_target>` | `ops/deploy_magura.sh` |

`deploy_rovdev.sh` rsyncs the repo to `/var/www/rovdev`, runs `drush74 updb`,
and clears caches. It does **not** overwrite `sites/*/settings.php` or
`sites/*/files/`.

To refresh rovdev as a full production clone (code/modules/libraries + DB +
files) before a deploy rehearsal, see
[`clone-prod-to-rovdev.md`](clone-prod-to-rovdev.md). Civic cutover steps on
that clone: [`integration/civic-deploy.md`](integration/civic-deploy.md).

## Outgoing email (disabled on dev)

Event mail (work reports, verifications, Rules notifications, etc.) must **not**
leave the dev server. Use **DevelMailLog** (Devel contrib module) so messages
are written to disk instead of PHP `mail()`.

### Current setup

- **Devel** module: enabled on dev.
- **Mail System** (`admin/config/system/mailsystem`): default sender =
  `DevelMailLog` (also set for mimemail / commerce invoice receipt).
- **`sites/default/settings.local.php`** on the server (gitignored): persists
  the same `mail_system` override so a DB import from prod does not re-enable
  real sending.
- **Log directory:** `sites/default/files/devel-mails/` (`public://devel-mails`).

After triggering an event (e.g. publish a work report), check that directory for
`.mail.txt` files with To/Subject/body — no inbox delivery.

### Re-applying after a fresh install or DB restore

1. Enable **Devel** (`drush74 en devel -y`).
2. **Configuration → Mail System** → set default **Sender** to `DevelMailLog`.
3. Ensure `settings.php` loads local overrides (see `settings.php.example`):

   ```php
   if (file_exists(dirname(__FILE__) . '/settings.local.php')) {
     include dirname(__FILE__) . '/settings.local.php';
   }
   ```

4. Create `sites/default/settings.local.php`:

   ```php
   <?php
   $conf['mail_system'] = array(
     'default-system' => 'DevelMailLog',
     'mimemail' => 'DevelMailLog',
     'commerce_invoice_receipt' => 'DevelMailLog',
   );
   $conf['devel_debug_mail_directory'] = 'public://devel-mails';
   ```

5. Ensure `sites/default/files/devel-mails/` exists and is writable by
   `www-data` (web-triggered mail runs as the web user, not root).

### Quick check (drush on server)

```bash
cd /var/www/rovdev
drush php-eval "print get_class(drupal_mail_system('rules', 'mail'));"
# Expected: DevelMailLog
```

Prod should keep the normal **Mime Mail** / server MTA configuration — do not
copy `settings.local.php` mail overrides to prod.
