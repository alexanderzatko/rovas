---
status: active
slug: pending-comment-notices
owner_repo: rovas
created: 2026-10-16
updated: 2026-10-16
---

# Pending watched-comment notices (pull API)

**Product:** When someone comments on a node a user is watching (same audience
as Comment Notify email), Rovas stores a pending notice so the user does not
miss it if they never open the email. Web shows a header block; Civic and
partner apps **poll** a list endpoint (same pattern as
`civic_pending_verifications`). This is **not** an outbound push webhook.

**Owner:** `rovas` (API + in-app); Civic client can consume later.

### Contract

**Method / path:** `GET /rovas/civic_pending_comment_notices`

**Auth:** Bearer required (`civic` / `user_profile`). API-key auth disabled on
this resource (same as other Civic Bearer reads).

**Query:** none.

**Response JSON:**

```json
{
  "total": 1,
  "items": [
    {
      "nid": "123",
      "node_title": "Example post",
      "cid": "456",
      "comment_url": "https://dev.rovas.app/comment/456",
      "created": 1729100000
    }
  ]
}
```

| Field | Notes |
|-------|--------|
| `total` | Count of pending notice rows (one per unread watched comment) |
| `nid` / `cid` | Strings (Civic numeric-id convention) |
| `comment_url` | Absolute permalink; opening it auto-clears that `(uid, cid)` on the web |
| `created` | Unix timestamp when the notice was created |

**Errors:** `401` `{ "error": "unauthorized", "message": "…" }` when no Bearer user.

**Empty / module off:** `{ "total": 0, "items": [] }` if `rovas_comment_watch`
is disabled or the user has no pending rows.

**Partner twin:** `GET /rovas/rovas_comment_notices` (API-key session) returns
`{ status, count, notices[] }` with int `nid`/`cid`.

**Web behaviour (same store):** header block “New comments you are watching”;
explicit dismiss clears all pending for that `uid`+`nid`; visiting
`/comment/{cid}` clears that cid only.

### Checklist

1. [x] `rovas_comment_watch` module + `{rovas_comment_watch_notice}` table
2. [x] Header block + dismiss + permalink auto-clear
3. [x] `GET /rovas/civic_pending_comment_notices` + `rovas_civic_update_7028`
4. [x] Partner `GET /rovas/rovas_comment_notices`
5. [ ] Deploy rovdev: enable `rovas_comment_watch`, `drush74 updb`, clear caches; enable partner resource on `rovas_endpoint` if needed
6. [ ] Smoke (human unless asked)

### Smoke test

```bash
# After a comment that would email the Bearer user via Comment Notify:
curl -sS "https://dev.rovas.app/rovas/civic_pending_comment_notices" \
  -H "Authorization: Bearer $TOKEN"
# Expect total >= 1 and comment_url pointing at /comment/{cid}

# After opening that comment_url while logged in as the same user (web),
# or after dismiss — re-fetch; that cid should be gone.
```

### Civic will consume

Optional later: poll on home/resume and deep-link to the comment (or show an
in-app list). Outbound Expo push on notice create is out of scope (same as
verify Phase 4).
