---
status: active
slug: wr-reward-auto-approve-merge
owner_repo: rovas
created: 2026-11-11
updated: 2026-11-11
---

# WR reward: auto-approve + merge Merits settlement

**Product:** When a third-party or Civic work report elects `reward_kind=merits`, Chron grants must convert to Merits (A6) after approval. Trusted auto-approve never creates verification nodes, so the old verification-only hook left authors with Chrons. Same-day merges can also change `reward_kind` across sessions that fold into one WR.

**Owner:** `rovas`

### Contract

Unchanged request fields. Behavior changes:

| Rule | Behavior |
|------|----------|
| Merits sticky | Once any session stores `merits` on `{civic_work_report_reward}` for a WR, later `chrons` cannot downgrade |
| Settle after approve | Auto-approve (persist after create) and peer verification both call `rovas_civic_settle_wr_reward()` |
| Merge top-ups | Chron deltas from `rovas_repmerger_sync_auto_approved_chrons()` convert when sticky merits; tracked via `converted_chrons` |

Schema: `{civic_work_report_reward}.converted_chrons` (`rovas_civic_update_7032`).

### Checklist

1. [x] Sticky persist + settle when WR already verified
2. [x] Delta CHR→ME via `converted_chrons`; guard pre-delta `converted` rows
3. [x] Settle from verification insert + auto-approve Chron sync
4. [ ] Deploy rovdev + `drush74 updb` (7032) + smoke (human)

### Smoke test

```bash
# Trusted user with auto-approve; TOKEN / API-KEY as appropriate.
# 1) Create WR with reward_kind=merits → expect ME (not leftover CHR from this WR)
# 2) Same-day merge: first chrons, second merits → sticky merits; full grant converted
# 3) Third merge adds hours → only Chron delta converts (converted_chrons rises)
# Peer-verify path: still settles on second approving verification insert.

# drush74 sqlq "SELECT wr_nid, reward_kind, converted, converted_chrons FROM civic_work_report_reward WHERE wr_nid = WR_NID"
```

### Civic will consume

No API shape change. Civic `civic_create_work_report` already persists `reward_kind`; settle now also covers Trusted auto-approve authors.
