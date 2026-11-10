# Handoff: NeoFund Rovas API developer page

**For:** NeoFund site agent (`~/Documents/Business/NeoFund_NP/web/site`)  
**From:** Rovas repo — OpenAPI is now the dual-API source of truth  
**Goal:** Keep a short localized narrative on `/en/rovas-api` and `/sk/rovas-api`;
render the detailed endpoint reference from `rovasApi.yml` instead of duplicated
markdown tables.

Do **not** invent endpoints or schemas in NeoFund. Do **not** edit Rovas Drupal
modules for this task.

---

## Why

- Classic request/response detail lived in Rovas
  [`rovasApi.yml`](../../sites/all/modules/custom/rovas_api_endpoints/rovasApi.yml)
  while NeoFund pages only showed summary tables and said schemas were
  “available on request.”
- OAuth/Civic paths were documented in NeoFund / `docs/api/reference.md` but
  missing from OpenAPI.
- As of Rovas OpenAPI **1.1.0**, that YAML is the **complete** catalog for
  classic **and** OAuth/Civic (tags, `securitySchemes`, paths, key schemas).

---

## Content split

| Layer | Owner | Lives in | Content |
| ----- | ----- | -------- | ------- |
| Narrative (i18n) | NeoFund | `src/content/pages/rovas-api/{en,sk}.md` | Intro, bases, which API to use, classic credential how-to, partner signup story + curl, OAuth how-to + Civic client table, contact CTA |
| Endpoint reference | Rovas → vendored | `src/data/rovasApi.yml` | Methods, paths, auth, params, bodies, responses, schemas, examples |

Markdown **must not** list every endpoint in tables anymore. Point readers to
the rendered reference below the narrative.

---

## Source file

Canonical (edit only in Rovas):

```text
~/Documents/homeProjects/rovas/sites/all/modules/custom/rovas_api_endpoints/rovasApi.yml
```

Vendor into NeoFund (suggested path):

```text
~/Documents/Business/NeoFund_NP/web/site/src/data/rovasApi.yml
```

Refresh the vendored copy whenever Rovas bumps `info.version` or changes
paths/schemas. Suggested local sync:

```bash
cp ~/Documents/homeProjects/rovas/sites/all/modules/custom/rovas_api_endpoints/rovasApi.yml \
   ~/Documents/Business/NeoFund_NP/web/site/src/data/rovasApi.yml
```

Optional: a small `scripts/sync-rovas-api.sh` in NeoFund that performs the copy
(and fails if the source path is missing). A sibling-path override for builds
on this machine is fine; production build should use the vendored file in git.

Offer a **raw download** link on the page to `/…/rovasApi.yml` (or a static
copy under `public/`).

---

## Implementation sketch

NeoFund stack today: Astro 7 + React, page special-case already used for
`commonsEstimate` in `src/pages/[lang]/[slug].astro`.

### 1. Dependencies

Add a YAML parser (e.g. `yaml`) **or** pre-convert to JSON at sync time
(`rovasApi.json`) so the build has no YAML dependency. Prefer vendoring YAML
and parsing at build if the dependency is acceptable.

### 2. Page wiring

For `pageKey === 'rovasApi'`:

1. Load/render markdown entry as today (`Content`).
2. Load OpenAPI spec from `src/data/rovasApi.yml`.
3. Render `<RovasApiReference spec={spec} locale={lang} />` **after** the
   narrative (Astro component or React `client:load` — Astro-only is enough
   unless you need client search/filter interactivity).

Mirror the `CommonsEstimate` branch pattern; do not replace the whole page
with Swagger UI.

### 3. `RovasApiReference` requirements

- Group operations by OpenAPI `tags` (order = `tags` array in the spec).
- Section classic vs OAuth using tag name prefixes (`classic-` / `oauth-`),
  with clear headings.
- Per operation show:
  - HTTP method + path
  - Auth badge derived from `security`:
    - `ApiKeyAuth` + `TokenAuth` → **API-KEY**
    - `BearerAuth` → **Bearer**
    - empty / `[]` → **Public**
    - (optional) if description mentions optional Bearer, show **Optional Bearer**
  - `summary` and `description`
  - Parameters (path/query/header)
  - Request body schema (resolve `#/components/schemas/…` `$ref`s one level deep
    at least)
  - Response status codes + schemas/examples when present
- UI chrome strings (section titles, “Parameters”, “Auth”, download link)
  translated for `sk` / `en`.
- **Operation text stays English** from OpenAPI unless NeoFund later adds
  `x-i18n`. Do not pretend schemas are Slovak on the SK page.
- Style with existing NeoFund layout/CSS variables; avoid embedding full
  Swagger/Redoc UI (heavy, mismatched look).

### 4. Trim markdown

Edit `src/content/pages/rovas-api/en.md` and `sk.md`:

**Keep**

- Frontmatter
- Intro + environment table + Accept/Content-Type note + contact
- “Solutions built with the API” / connectors link
- “Which API should I use?” + auth label table
- Classic “how to use” (credentials, partner signup, curl example)
- OAuth “how to use” (protocol overview, Civic client table, token curl, `/me` example)
- Support / contact

**Remove**

- Entire “Classic endpoint reference” tables
- Entire “OAuth / Civic endpoint reference” tables
- Phrases like “OpenAPI file (available on request)” / “Schémy … na vyžiadanie”

**Add**

- One short line before the component mount, e.g. “Full endpoint reference
  (from OpenAPI) follows below” / SK equivalent.

### 5. Out of scope for NeoFund agent

- Changing Rovas Services resources or Drupal code
- Adding endpoints not present in the vendored YAML
- Translating every OpenAPI `summary`/`description` into Slovak in this pass

---

## Acceptance criteria

- [ ] `npm run build` succeeds for the NeoFund site
- [ ] `/en/rovas-api` and `/sk/rovas-api` show narrative + generated reference
- [ ] Generated reference includes paths from YAML for both classic and OAuth
      (spot-check: `/rovas/rovas_partner_user`, `/rovas/me`, `/oauth2/token`,
      `/rovas/civic_create_work_report`)
- [ ] No “available on request” / “na vyžiadanie” for OpenAPI
- [ ] No duplicated full endpoint tables in `en.md` / `sk.md`
- [ ] Raw YAML (or JSON) download works
- [ ] After updating only the vendored YAML (new path) and rebuilding, the new
      path appears without editing markdown tables

---

## Related Rovas docs

- [README.md](README.md) — API docs index
- [getting-started.md](getting-started.md) — narrative source for what to keep in markdown
- [reference.md](reference.md) — pointer only
- Civic internals: [../integration/civic.md](../integration/civic.md)
