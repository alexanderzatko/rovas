# Production cron (responsiveness)

How Drupal cron is run on **prod** (`/var/www/rovas`, `rovas.app`) so it does
not steal PHP-FPM workers from Civic / the Rovas API.

Dev (`/var/www/rovdev`) is separate; this note is for prod unless stated.

## Current setup (2026-10-08)

| Piece | Value |
|-------|--------|
| Module | `elysia_cron` **7.x-2.5** (enabled) |
| Automated Drupal cron | Off (`cron_safe_threshold = 0`) |
| Trigger | `www-data` crontab → **niced Drush** + **`flock -n`** (not `wget cron.php`) |
| Lock file | `/run/lock/rovas-elysia-cron.lock` (`flock -n`: skip tick if still running) |
| PHP-FPM | `php7.4-fpm` pool `www`: `pm.max_children = 8` (was 5) |
| UI / stats | `/admin/config/system/cron` |

### www-data crontab

```cron
# Matomo archive (niced so it yields to web/API)
*/5 * * * * /usr/bin/nice -n 19 /usr/bin/php /var/www/matomo/console core:archive --url=https://analytics.rovas.app > /dev/null 2>&1

# Drupal / Elysia Cron via Drush (CLI, niced, flock so slow runs cannot stack)
* * * * * /usr/bin/flock -n /run/lock/rovas-elysia-cron.lock /bin/sh -c 'cd /var/www/rovas && /usr/bin/nice -n 19 /usr/local/bin/drush74 elysia-cron run' >/dev/null 2>&1
```

The **1-minute** tick only asks Elysia which jobs are due. If a previous run is
still holding the lock, that minute is skipped (`flock -n`). Heavy work is not
scheduled every minute (see below).

On 2026-10-08 a Rules/`semaphore` deadlock during a long Elysia run stacked many
minute ticks and saturated PHP-FPM; `flock -n` was added to stop that pile-up.

### Elysia job rules (initial)

| Cadence | Channel | Jobs |
|---------|---------|------|
| `*/5 * * * *` | `default` | `rovas_fio_reconcile_cron`, `rovas_project_reward_cron`, `rules_scheduler_cron`, `oauth2_server_cron`, `queue_rules_scheduler_tasks`, `queue_rovas_civic_charity_ai_check` |
| `0 * * * *` | `default` | Light cleanup (`system`, `dblog`, `captcha`, …), `rovas_civic_cron` (Merit Index snapshot still self-gates ~12h in code) |
| `0 4 * * *` | `heavy` | `backup_migrate_cron` (BAM’s own schedule is also nightly) |

Tune further from Elysia’s per-job timing stats after a day or two.

## Why this shape

Previously, `wget https://rovas.app/cron.php` every minute ran through nginx →
**php7.4-fpm** (`pm.max_children = 5`), shared with `rovas.app`, `dev.rovas.app`,
and `zajezka.sk`. Matomo archive also burned CPU every 5 minutes. Civic API
requests queued when cron or Matomo overlapped that tiny pool.

## Rollback

Crontab backup on the VPS (example):

```bash
# List backups
ls -la /root/crontab-www-data.bak.*

# Restore previous crontab
crontab -u www-data /root/crontab-www-data.bak.YYYYMMDD-HHMMSS
```

The pre-change line looked like:

```cron
* * * * * /usr/bin/wget -O - -q -t 1 "https://rovas.app/cron.php?cron_key=…" > /dev/null 2>&1
```

FPM pool backup:

```bash
ls /etc/php/7.4/fpm/pool.d/www.conf.bak.*
# Restore then: systemctl reload php7.4-fpm
```

To disable Elysia without restoring wget: `drush74 dis elysia_cron -y` and point
crontab at `drush74 core-cron` (still prefer CLI over HTTP).

## Useful commands

```bash
cd /var/www/rovas
drush74 elysia-cron list --elysia-cron-verbose
drush74 elysia-cron run
# Force one job:
drush74 elysia-cron run rovas_fio_reconcile_cron --ignore-time
```

## Apply schedule script (reference)

A one-shot was used on the server as `/tmp/elysia_schedule_rovas.php` via
`drush74 php-script`. Re-apply by setting rules with:

- `elysia_cron_set_job_rule($job, $crontab)`
- `elysia_cron_set_job_channel($job, $channel)`
