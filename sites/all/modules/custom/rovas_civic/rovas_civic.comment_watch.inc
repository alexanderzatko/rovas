<?php

/**
 * @file
 * Civic pull API for pending watched-comment notices.
 *
 * Poll-based (same pattern as civic_pending_verifications). Requires
 * rovas_comment_watch. See docs/integration/civic_tasks/active/pending-comment-notices.md.
 */

/**
 * Services callback: GET /rovas/civic_pending_comment_notices
 */
function rovas_civic_services_pending_comment_notices_index() {
  module_load_include('inc', 'rovas_civic', 'rovas_civic.volunteer');
  $uid = rovas_civic_request_uid();
  if ($uid < 1) {
    return rovas_civic_volunteer_error(401, 'unauthorized', 'Authentication required');
  }
  return rovas_civic_pending_comment_notices($uid);
}

/**
 * Pending watched-comment notices for a user.
 *
 * @param int $uid
 *
 * @return array
 *   { total, items[] } with nid, node_title, cid, comment_url, created.
 */
function rovas_civic_pending_comment_notices($uid) {
  $uid = (int) $uid;
  if (!function_exists('rovas_comment_watch_pending_for_user')) {
    return array(
      'total' => 0,
      'items' => array(),
    );
  }

  $rows = rovas_comment_watch_pending_for_user($uid);
  $items = array();
  foreach ($rows as $row) {
    $items[] = array(
      'nid' => (string) ((int) $row['nid']),
      'node_title' => $row['node_title'],
      'cid' => (string) ((int) $row['cid']),
      'comment_url' => $row['comment_url'],
      'created' => (int) $row['created'],
    );
  }

  return array(
    'total' => count($items),
    'items' => $items,
  );
}
