# Clone production → rovdev (full baseline)

Refresh `https://dev.rovas.app` (`/var/www/rovdev`) as a faithful copy of
production before rehearsing a deploy/Civic cutover. Live prod
(`/var/www/rovas`, DB `rovas`) is **read-only**.

After this clone, deploy Civic-era code with `ops/deploy_rovdev.sh` and follow
[`integration/civic-deploy.md`](integration/civic-deploy.md). Mail hardening:
[`dev-setup.md`](dev-setup.md).

**SSH:** Magura root (e.g. `sano-vps` → `root@37.205.11.79`). Prefer `drush74`.

## What gets mirrored

| Asset | Source → target |
| --- | --- |
| Code (core, themes, vendor) | `/var/www/rovas/` → `/var/www/rovdev/` |
| Modules | `sites/all/modules/` (contrib + custom) |
| Libraries | `sites/all/libraries/` |
| Public files | `sites/default/files/` |
| Database | MySQL `rovas` → `rovdev` |

**Never** overwrite rovdev’s `sites/default/settings.php` or
`settings.local.php` with prod’s.

## Procedure (on Magura)

### 0. Preflight

```bash
df -h /
du -sh /var/www/rovas /var/www/rovdev \
  /var/www/rovas/sites/default/files /var/www/rovdev/sites/default/files
# Confirm trees exist: modules/{contrib,custom}, libraries, themes, vendor
```

### 1. Maintenance on clone only

```bash
cd /var/www/rovdev && drush74 vset maintenance_mode 1 -y
cp -a sites/default/settings.php /tmp/rovdev-settings.php.bak
cp -a sites/default/settings.local.php /tmp/rovdev-settings.local.php.bak
```

### 2. Code / modules / libraries

```bash
rsync -a --delete \
  --exclude 'sites/default/settings.php' \
  --exclude 'sites/default/settings.local.php' \
  --exclude 'sites/*/files/' \
  --exclude 'sites/*/private/' \
  --exclude 'sites/*/tmp/' \
  --exclude '.git/' \
  /var/www/rovas/ /var/www/rovdev/
```

Parity check (expect empty):

```bash
diff -rq /var/www/rovas/sites/all/modules /var/www/rovdev/sites/all/modules
diff -rq /var/www/rovas/sites/all/libraries /var/www/rovdev/sites/all/libraries
```

### 3. Public files

```bash
rsync -a --delete \
  /var/www/rovas/sites/default/files/ \
  /var/www/rovdev/sites/default/files/
```

### 4. Database

```bash
DUMP=/tmp/rovas-prod-$(date +%Y%m%d-%H%M).sql.gz
mysqldump --single-transaction --routines --triggers rovas | gzip -1 > "$DUMP"

mysql -e "DROP DATABASE rovdev;
  CREATE DATABASE rovdev CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci;
  GRANT ALL ON rovdev.* TO 'rovdev'@'localhost';
  FLUSH PRIVILEGES;"

gunzip -c "$DUMP" | mysql rovdev
```

DB credentials stay in each docroot’s `settings.php` (rovdev user/db name
unchanged).

### 5. Caches + mail harden

```bash
mysql rovdev -e '
  TRUNCATE cache; TRUNCATE cache_bootstrap; TRUNCATE cache_block;
  TRUNCATE cache_field; TRUNCATE cache_filter; TRUNCATE cache_form;
  TRUNCATE cache_menu; TRUNCATE cache_page; TRUNCATE cache_path;
  TRUNCATE cache_rules; TRUNCATE cache_token; TRUNCATE cache_update;
  TRUNCATE cache_variable; TRUNCATE cache_views; TRUNCATE cache_views_data;'

mkdir -p /var/www/rovdev/sites/default/files/devel-mails
chown www-data:www-data /var/www/rovdev/sites/default/files/devel-mails
chmod 775 /var/www/rovdev/sites/default/files/devel-mails

cd /var/www/rovdev
drush74 en devel -y
drush74 vset maintenance_mode 1 -y
drush74 php-eval "print get_class(drupal_mail_system('rules', 'mail'));"
# Expected: DevelMailLog  (via settings.local.php — see dev-setup.md)
drush74 status
```

### 6. Deploy rehearsal + Civic checklist

From the laptop (Civic-era tree):

```bash
ops/deploy_rovdev.sh sano-vps
```

Then [`integration/civic-deploy.md`](integration/civic-deploy.md) (enable modules,
OAuth/Services ensure, Rules migration, merit backfill/reconcile, smoke curls).

## After laptop deploy

`ops/deploy_rovdev.sh` uses `rsync --delete` and previously could remove
server-only `settings.local.php`. The script now excludes
`sites/*/settings.local.php`. Still verify mail after every deploy:

```bash
cd /var/www/rovdev
drush74 php-eval "print get_class(drupal_mail_system('rules', 'mail'));"
# Expected: DevelMailLog
```

## Safety

- No writes under `/var/www/rovas`; never import into DB `rovas`
- Code rsync is **prod → rovdev only**
- Confirm `DevelMailLog` before browsing the clone
- Do not run live payment tests against production Stripe keys on the clone
