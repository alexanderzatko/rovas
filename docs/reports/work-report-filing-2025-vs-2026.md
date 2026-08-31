# Work Report Filing Analysis: 2025 vs 2026

**Generated:** 2026-08-29  
**Source:** Production Rovas (`rovas.app`), database query via Drush  
**Method:** Distinct authors (`uid > 0`) of `work_report` nodes, keyed by submission date (`node.created`)

---

## Summary

| Metric | 2025 (full year) | 2026 (YTD through Aug 29) |
|--------|------------------:|--------------------------:|
| **People who filed ≥1 report** | **94** | **84** |
| **Total reports filed** | 1,406 | 1,615 |
| **Avg reports per filer** | 15.0 | 19.2 |

2026 has already exceeded all of 2025 in total report volume, with four months remaining in the year.

---

## Same-Period Comparison (Jan 1 – Aug 29)

| Metric | 2025 YTD | 2026 YTD | Change |
|--------|--------:|---------:|-------:|
| **People** | 79 | 84 | +5 (+6%) |
| **Reports** | 826 | 1,615 | +789 (+96%) |

Activity per person roughly doubled in the same window:

- 2025 YTD: ~10.5 reports per filer
- 2026 YTD: ~19.2 reports per filer

---

## Filer Overlap (2025 vs 2026 YTD)

| Group | Count |
|-------|------:|
| Filed in both 2025 and 2026 | 50 |
| Filed in 2026 only (new filers) | 34 |
| Filed in 2025 only (not yet in 2026) | 44 |

---

## Data Quality Notes

### Published vs unpublished (2025)

| Status | Reports | Distinct users |
|--------|--------:|---------------:|
| Published | 1,392 | 92 |
| Unpublished | 14 | 4 |
| **Total** | **1,406** | **94** |

All 1,615 reports filed in 2026 YTD are published (`status = 1`).

### Work date vs submission date

Counting by the work date field (`field_date`) instead of submission date (`node.created`) yields the same distinct user counts for both years (94 in 2025, 84 in 2026 YTD).

---

## Interpretation

- **Headcount:** Slightly more unique filers in 2026 YTD (+5 vs the same period in 2025), but fewer than the full 2025 total (84 vs 94).
- **Volume:** Nearly twice as many reports in the Jan–Aug window (+96%), driven mainly by existing users filing more often.
- **Retention:** 50 of 84 current-year filers (60%) also filed in 2025; 34 are new to work reporting in 2026.
- **Drop-off:** 44 people who filed in 2025 have not filed yet in 2026.

---

## Query Reference

```sql
-- Distinct filers and report count for a calendar year (by submission date)
SELECT COUNT(*) AS reports, COUNT(DISTINCT uid) AS users
FROM node
WHERE type = 'work_report'
  AND uid > 0
  AND created >= UNIX_TIMESTAMP('YYYY-01-01 00:00:00')
  AND created < UNIX_TIMESTAMP('YYYY+1-01-01 00:00:00');

-- Same, but by work date field
SELECT COUNT(DISTINCT n.uid) AS users
FROM node n
INNER JOIN field_data_field_date fd
  ON fd.entity_id = n.nid
  AND fd.entity_type = 'node'
  AND fd.deleted = 0
WHERE n.type = 'work_report'
  AND n.uid > 0
  AND fd.field_date_value >= 'YYYY-01-01'
  AND fd.field_date_value < 'YYYY+1-01-01';
```
