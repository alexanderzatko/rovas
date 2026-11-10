# Rovas

**Rovas** (Slovak: *Rováš*) is the web application that implements **[NEO](https://neofund.sk/)**, a community economic system for voluntary work.

Live site: [https://rovas.app](https://rovas.app)  
Operator: [NeoFund s.r.o.](https://neofund.sk/) (Slovakia)

## What it does

- **Work reports** — people log voluntary labor against projects (with proofs, categories, and peer verification).
- **Chrons** — NEO’s hourly currency: **10 CHR** per verified hour of work. Chrons can reward other projects or be exchanged for other currencies.
- **Merits** — non-monetary recognition issued when projects are bought or rewarded (paid chrons are destroyed; merits accumulate as social value).
- **Projects** — public or private offerings with shareholder-style merit allocation.
- **API & Civic** — REST API for partners; [Civic](docs/integration/civic.md) uses Rovas as identity and merit ledger (OAuth2/PKCE).

See the in-app [Rules](https://rovas.app/rules) and [Welcome](https://rovas.app/welcome-to-rovas) pages for the product model.

## Stack

- **Drupal 7** application (classic docroot layout; no Composer root project)
- Custom modules: `sites/all/modules/custom/rovas_*`
- Theme: `sites/all/themes/rovas` (Bootstrap base)
- Locales: English, Slovak, Czech, Italian (`translations/`)
- Git-managed static pages: `content/pages/`

Upstream Drupal files (`README.txt`, `LICENSE.txt`, `INSTALL.txt`, …) remain the stock core docs.

## Documentation

| Doc | Purpose |
|-----|---------|
| [`docs/dev-setup.md`](docs/dev-setup.md) | Dev vs prod, deploy overview |
| [`docs/api/`](docs/api/) | Public API contracts |
| [`docs/integration/civic.md`](docs/integration/civic.md) | Civic ↔ Rovas integration index |
| [`docs/static-pages.md`](docs/static-pages.md) | Importing git-managed Basic pages |
| [`translations/README.md`](translations/README.md) | Locale / PO workflow |

## Contributing

Bug reports and feature ideas: [GitHub Issues](https://github.com/alexanderzatko/rovas/issues).

For local development you need a Drupal 7 environment, a MySQL/MariaDB database, and a `sites/default/settings.php` (use `sites/default/settings.php.example` as a starting point). Runtime files, private uploads, and per-environment settings are gitignored — they are never part of this repository.

## Security

If you find a vulnerability that could expose private user data or compromise data integrity, please email **rovas@neofund.sk** rather than filing a public issue.

## License

This repository includes Drupal core and many contrib modules under the **GNU General Public License, version 2** (see [`LICENSE.txt`](LICENSE.txt) and [`COPYRIGHT.txt`](COPYRIGHT.txt)). Custom Rovas modules and theme follow the same GPL-2 terms unless a specific file states otherwise.
