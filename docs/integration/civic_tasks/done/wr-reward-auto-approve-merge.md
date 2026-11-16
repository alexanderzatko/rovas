---
status: done
slug: wr-reward-auto-approve-merge
owner_repo: rovas
created: 2026-11-11
updated: 2026-11-16
---

# WR reward: auto-approve + merge Merits settlement

**Product:** When a third-party or Civic work report elects `reward_kind=merits`, Chron grants must convert to Merits (A6) after approval. Trusted auto-approve never creates verification nodes, so the old verification-only hook left authors with Chrons. Same-day merges can also change `reward_kind` across sessions that fold into one WR. App fees charged via a separate AUR after Merits conversion could drive Chron negative; deferred fee on create + net-of-fees convert fixes that.

**Owner:** `rovas`

### Contract

| Rule | Behavior |
|------|----------|
| Merits sticky | Once any session stores `merits` on `{civic_work_report_reward}` for a WR, later `chrons` cannot downgrade |
| Settle after approve | Auto-approve (persist after create) and peer verification both call `rovas_civic_settle_wr_reward()` |
| Merge top-ups | Chron deltas from `rovas_repmerger_sync_auto_approved_chrons()` convert when sticky merits; tracked via `converted_chrons` |
| Deferred app fee | Classic + Civic: optional `usage_fee_project_id` + `usage_fee_rate` on create; AUR on settle |
| Net Merits | Convert `grant − Rovas 1% − app fee`; auto-approve also charges Rovas 1% AUR (peer path already does in Rules) |

Schema: `converted_chrons` (`7032`); `rovas_fee_charged` + `usage_fee_amount` (`7033`).

Classic: prefer deferred fee on create over `rules_proxy_create_aur` when electing Merits.

### Checklist

1. [x] Sticky persist + settle when WR already verified
2. [x] Delta CHR→ME via `converted_chrons`; guard pre-delta `converted` rows
3. [x] Settle from verification insert + auto-approve Chron sync
4. [x] Classic create accepts `usage_fee_*`; Merits convert net of Rovas 1% + app fee
5. [ ] Deploy rovdev + re-import `rules_proxy_create_work_report` + `drush74 updb` (7032/7033) + smoke (human)

### Smoke test

```bash
# Trusted user with auto-approve; TOKEN / API-KEY as appropriate.
# 1) Create WR with reward_kind=merits → ME ≈ grant×0.99; Chron ≈ 0 from this WR
# 2) Classic create merits + usage_fee_rate=0.03 → ME ≈ grant×0.96; Chron ≈ 0
# 3) Same-day merge: first chrons, second merits → sticky merits; net convert
# 4) reward_kind=chrons + deferred fee → Chron net of fees; no ME
# Peer-verify merits: single Rovas AUR (Rules); ME still net of 1% + app fee

# drush74 sqlq "SELECT wr_nid, reward_kind, converted, converted_chrons, usage_fee_amount, rovas_fee_charged FROM civic_work_report_reward WHERE wr_nid = WR_NID"
```

### Civic will consume

No breaking API shape change. Civic already sends `usage_fee_*` when enabled; settle now converts Merits **net** of Rovas 1% and that fee. Trusted auto-approve authors get Rovas 1% charged in settle.
