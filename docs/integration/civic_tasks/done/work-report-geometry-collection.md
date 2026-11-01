---
status: done
slug: work-report-geometry-collection
owner_repo: rovas
created: 2026-11-01
updated: 2026-11-01
---

# Work-report map: GeometryCollection (multi / mixed features)

> Done — parse/validate/export `GeometryCollection` on WR `field_location`
> (`rovas_civic.geofield.inc` + `rovas_civic_wr_field_location_geojson`).

**Product:** Civic’s volunteer map proof now lets users draw **unlimited**
features and **mix** Point / LineString / Polygon on one work report. When more
than one feature is present, Civic sends GeoJSON `GeometryCollection` as
`field_location` (single features stay Point / LineString / Polygon for
backward compatibility). Rovas must accept, validate, persist, and re-export
`GeometryCollection` the same way as today’s simple geometries.

**Owner:** `rovas` — Civic API + Expo already send/normalize collections
against this contract.

### Contract

#### Write — `POST /rovas/civic_create_work_report`

Extend `field_location` acceptance in
`rovas_civic_parse_field_location_geojson()`:

| `type` | Rules |
|--------|--------|
| `Point` / `LineString` / `Polygon` | Unchanged |
| `GeometryCollection` | Required non-empty `geometries[]` of **only** Point / LineString / Polygon (no nested collections). Soft max **100** members. Validate each member (same lon/lat / ring rules as today); run geoPHP validity on polygons. Persist via existing `rovas_civic_attach_wr_field_location()` (geoPHP → WKT → geofield). |

Reject unknown types / empty collections / nested collections with **400**
`invalid_input` and a clear `error` string.

Proof rule unchanged: URL **or** photos **or** map (`field_location`).

#### Read — WR detail / list GeoJSON export

`rovas_civic_wr_field_location_geojson()` currently requires
`isset($decoded['type'], $decoded['coordinates'])`, which **drops**
`GeometryCollection` (uses `geometries`, not `coordinates`). Fix export so a
decoded object with `type === 'GeometryCollection'` and a `geometries` array
is returned as-is (after geoPHP `out('json')`).

### Checklist

1. [x] Parse + validate `GeometryCollection` in `rovas_civic.geofield.inc`
2. [x] Persist via existing geofield attach (confirm WKT `GEOMETRYCOLLECTION` stores)
3. [x] Fix GeoJSON export for collections (no `coordinates` requirement)
4. [x] Deploy rovdev + smoke (human unless asked)

### Smoke test

```bash
TOKEN=…  # Civic OAuth bearer

# Mixed collection
curl -sS -X POST "https://dev.rovas.app/rovas/civic_create_work_report" \
  -H "Authorization: Bearer $TOKEN" \
  -H "Content-Type: application/json" \
  -d '{
    "project_nid": "…",
    "date_started": 1730419200,
    "hours": 1,
    "description": "GeometryCollection smoke",
    "nace_tid": "…",
    "field_location": {
      "type": "GeometryCollection",
      "geometries": [
        { "type": "Point", "coordinates": [17.1, 48.1] },
        {
          "type": "LineString",
          "coordinates": [[17.1, 48.1], [17.11, 48.12]]
        },
        {
          "type": "Polygon",
          "coordinates": [[[17.1, 48.1], [17.11, 48.1], [17.11, 48.11], [17.1, 48.1]]]
        }
      ]
    }
  }'
# Expect work_report_nid; node field_location populated

# Round-trip export
curl -sS "https://dev.rovas.app/rovas/civic_work_report/<nid>?lang=en" \
  -H "Authorization: Bearer $TOKEN"
# field_location.type === "GeometryCollection" with 3 geometries
```

### Civic will consume

- Expo volunteer form builds `GeometryCollection` when ≥2 features are drawn.
- Civic `POST /work-reports` already proxies `field_location` after local
  parse (`GeometryCollection` allowed).
- Verify / proof map Leaflet renders `geometries[]` members.
