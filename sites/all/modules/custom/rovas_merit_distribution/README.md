# Rovas Merit Distribution

Drupal 7 module that produces an SVG histogram of all active users' merit
scores split into 10 equal-width bins on a log10 axis, optionally
highlighting one user's bin with a triangle pointer and their username.

## What it provides

- **Render helper** `rovas_merit_distribution_render($opts)` — returns
  `['svg' => string, 'meta' => array]`. Other modules can embed the SVG
  inline.
- **Services resource** `rovas_merit_distribution` (operation `index`) —
  registers via `hook_services_resources()` for any Services endpoint that
  opts in. Returns JSON `{status, svg, meta}`.
- **Admin preview** at `admin/reports/merit-distribution` (permission
  `access site reports`) — renders the chart inline with a small
  uid/width/height form and a cache-bust link.

## Data source

The chart pulls per-user totals using the same expression the existing
user-rank popover uses (`_base_user_me_rank()` in `rovas_base`):

```sql
SELECT (IFNULL(pt.points, 0) + IFNULL(d.field_merit_decimal_part_value, 0)) AS merits
FROM {users} u
LEFT JOIN {userpoints_total} pt ON u.uid = pt.uid
LEFT JOIN {field_data_field_merit_decimal_part} d
  ON u.uid = d.entity_id AND d.entity_type = 'user' AND d.deleted = 0
WHERE u.status <> 0 AND u.uid > 1
```

Results are cached in the `cache` bin under
`rovas_merit_distribution:merits` for 1 hour. The cache can be busted from
the admin preview page (`?reset=1`) or programmatically via
`cache_clear_all('rovas_merit_distribution:merits', 'cache')`.

## Binning

- Users with `merits <= 0` are excluded from the log axis and reported in
  `meta.excluded_zero`.
- Bin edges: `floor(log10(min_positive))` to `ceil(log10(max))` in 10 equal
  log10-wide steps. The highest bin is right-inclusive.
- `meta.bin_edges_log10` holds the 11 edge values; `meta.bin_counts` holds
  the 10 counts.

## Enabling the Services resource

After enabling the module (`drush en -y rovas_merit_distribution`), the
new resource must be activated for whichever Services endpoint should
serve it (typically `rovas_endpoint`):

1. Go to **Admin → Structure → Services → rovas_endpoint → Edit resources**.
2. Tick **rovas_merit_distribution → index**, save.
3. (If using session/API key auth defaults) make sure the API key + token
   headers are required for this operation, matching the other
   `rovas_api_endpoints` resources.

## Request format

```
GET /<endpoint-base>/rovas_merit_distribution
    ?uid=<int>          # optional, defaults to authenticated user
    &width=<int>        # optional, default 720
    &height=<int>       # optional, default 360
    &x_labels=1         # optional, show rounded merit values under bin edges
    &count_labels=1     # optional, show rounded user counts above each bar
Headers:
    X-API-Key: <api key>
    X-Token:   <token>
```

Both `x_labels` and `count_labels` default to off. Pass `1` (truthy) to
enable. Labels are rendered with a "reasonable" rounding scheme: integers
below 1000 (e.g. `316`), `k` suffix for thousands (`32k`), `M` for
millions, `B` for billions, `T` for trillions. The render helper
`rovas_merit_distribution_render($opts)` accepts the same flags as boolean
keys `x_labels` and `count_labels`.

## Response

```json
{
  "status": "success",
  "svg": "<svg ...>...</svg>",
  "meta": {
    "bin_edges_log10": [0.0, 0.3, 0.6, ...],
    "bin_counts":      [12, 38, 91, 140, 210, 175, 95, 40, 18, 4],
    "min_merits":      1.0,
    "max_merits":      980000.0,
    "user_bin":        4,
    "user_merits":     2153.45,
    "user_name":       "example_user",
    "total_users":     823,
    "excluded_zero":   17,
    "degenerate":      false
  }
}
```

## Notes

- Highlight uses Rovas brand blue `#3D5A99`; bars are mid-grey `#A8A8A8`.
  Override via the `bar_color`, `highlight_color`, `border_color`,
  `triangle_color`, `label_color` options to `rovas_merit_distribution_render()`.
- A user with `merits <= 0` is rendered without a highlight; `meta.user_bin`
  is `null`.
- The SVG is fully self-contained (no JS, no external resources).
