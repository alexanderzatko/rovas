<?php

/**
 * @file
 * Civic work-report map proof: map config export + field_location GeoJSON.
 *
 * See docs/integration/civic.md → Next work: geofield on Civic work reports.
 */

/**
 * Services callback: GET /rovas/civic_work_report_map_config
 */
function rovas_civic_services_work_report_map_config_index() {
  $uid = rovas_civic_request_uid();
  if ($uid < 1) {
    return rovas_civic_volunteer_error(401, 'unauthorized', 'Authentication required');
  }
  return rovas_civic_work_report_map_config();
}

/**
 * Map widget DTO for Civic (basemap tiles, centre, allowed geometry types).
 *
 * @return array
 */
function rovas_civic_work_report_map_config() {
  $widget = rovas_civic_work_report_geofield_widget_settings();
  $geometry_types = rovas_civic_work_report_geometry_types_from_widget($widget);
  $map_name = !empty($widget['openlayers_map']) ? $widget['openlayers_map'] : 'freemap';

  $built = rovas_civic_work_report_map_config_from_openlayers($map_name, $geometry_types);
  if ($built) {
    return $built;
  }

  return rovas_civic_work_report_map_config_fallback($geometry_types);
}

/**
 * Field instance widget settings for work_report.field_location.
 *
 * @return array
 */
function rovas_civic_work_report_geofield_widget_settings() {
  $instances = field_info_instances('node', 'work_report');
  if (empty($instances['field_location']['widget']['settings'])
    || !is_array($instances['field_location']['widget']['settings'])) {
    return array();
  }
  return $instances['field_location']['widget']['settings'];
}

/**
 * Map widget feature_types (point/path/polygon) → Civic GeoJSON type names.
 *
 * @param array $widget
 *
 * @return array
 *   e.g. array('Point', 'LineString', 'Polygon')
 */
function rovas_civic_work_report_geometry_types_from_widget(array $widget) {
  $map = array(
    'point' => 'Point',
    'path' => 'LineString',
    'polygon' => 'Polygon',
  );
  $out = array();
  if (!empty($widget['feature_types']) && is_array($widget['feature_types'])) {
    foreach ($widget['feature_types'] as $key => $enabled) {
      if (!$enabled || !isset($map[$key])) {
        continue;
      }
      $out[] = $map[$key];
    }
  }
  if (empty($out)) {
    return array_values($map);
  }
  return $out;
}

/**
 * Try OpenLayers 3 export for the work-report geofield widget preset.
 *
 * @param string $map_name
 * @param array $geometry_types
 *
 * @return array|null
 */
function rovas_civic_work_report_map_config_from_openlayers($map_name, array $geometry_types) {
  if (!class_exists('\Drupal\openlayers\Openlayers')) {
    return NULL;
  }

  /** @var \Drupal\openlayers\Types\MapInterface|false $map */
  $map = \Drupal\openlayers\Openlayers::load('Map', $map_name);
  if (!$map) {
    return NULL;
  }

  $opts = $map->getOptions();
  $center = array(
    'latitude' => 48.7164,
    'longitude' => 19.6611,
  );
  $zoom = 8;
  if (!empty($opts['view']['center']['lat']) && isset($opts['view']['center']['lon'])) {
    $center = array(
      'latitude' => (float) $opts['view']['center']['lat'],
      'longitude' => (float) $opts['view']['center']['lon'],
    );
  }
  if (isset($opts['view']['zoom']) && $opts['view']['zoom'] !== '') {
    $zoom = (int) $opts['view']['zoom'];
  }

  $raster_layers = array();
  foreach ($map->getCollection()->getObjects('layer') as $layer) {
    if (!method_exists($layer, 'getSource')) {
      continue;
    }
    $source = $layer->getSource();
    if (!$source || $source->getFactoryService() !== 'openlayers.Source:XYZ') {
      continue;
    }
    $url_raw = trim((string) $source->getOption('url', ''));
    if ($url_raw === '') {
      continue;
    }
    $url_lines = preg_split('/\r\n|\r|\n/', $url_raw);
    $url_template = rovas_civic_normalize_xyz_url_template(trim((string) reset($url_lines)));
    if ($url_template === '' || strpos($url_template, '{z}') === FALSE) {
      continue;
    }
    $layer_id = $layer->getMachineName();
    $raster_layers[] = array(
      'id' => $layer_id,
      'name' => $layer->getName(),
      'url_template' => $url_template,
      'attribution' => trim((string) $layer->getOption('attribution', '')),
      'max_zoom' => rovas_civic_xyz_max_zoom_from_url($url_template),
    );
  }

  if (empty($raster_layers)) {
    return NULL;
  }

  $default_layer_id = $raster_layers[0]['id'];
  if (!empty($opts['layers']) && is_array($opts['layers'])) {
    foreach ($opts['layers'] as $layer_machine_name) {
      foreach ($raster_layers as $row) {
        if ($row['id'] === $layer_machine_name) {
          $default_layer_id = $layer_machine_name;
          break 2;
        }
      }
    }
  }

  return array(
    'center' => $center,
    'zoom' => $zoom,
    'geometry_types' => $geometry_types,
    'raster_layers' => $raster_layers,
    'default_layer_id' => $default_layer_id,
  );
}

/**
 * Documented freemap fallback (matches rovdev WR widget when OL export fails).
 *
 * @param array $geometry_types
 *
 * @return array
 */
function rovas_civic_work_report_map_config_fallback(array $geometry_types) {
  return array(
    'center' => array(
      'latitude' => 48.7164,
      'longitude' => 19.6611,
    ),
    'zoom' => 8,
    'geometry_types' => $geometry_types,
    'raster_layers' => array(
      array(
        'id' => 'freemap',
        'name' => 'Freemap',
        'url_template' => 'https://tile.freemap.sk/T/{z}/{x}/{y}.jpeg',
        'attribution' => '© Freemap Slovakia',
        'max_zoom' => 18,
      ),
    ),
    'default_layer_id' => 'freemap',
  );
}

/**
 * Normalize an XYZ tile URL for Civic ({z}/{x}/{y}, https scheme).
 *
 * @param string $url
 *
 * @return string
 */
function rovas_civic_normalize_xyz_url_template($url) {
  $url = trim((string) $url);
  if ($url === '') {
    return '';
  }
  if (strpos($url, '//') === 0) {
    $url = 'https:' . $url;
  }
  return $url;
}

/**
 * Guess max zoom from common tile URL patterns.
 *
 * @param string $url
 *
 * @return int
 */
function rovas_civic_xyz_max_zoom_from_url($url) {
  if (stripos($url, 'freemap.sk') !== FALSE) {
    return 18;
  }
  if (stripos($url, 'openstreetmap.org') !== FALSE) {
    return 19;
  }
  return 18;
}

/**
 * Parse and validate Civic field_location GeoJSON geometry.
 *
 * @param mixed $raw
 *
 * @return array|null
 *   Normalized geometry array, NULL when omitted.
 * @return array
 *   With key `error` (string) on validation failure.
 */
function rovas_civic_parse_field_location_geojson($raw) {
  if ($raw === NULL || $raw === '') {
    return NULL;
  }
  if (!is_array($raw)) {
    return array('error' => 'field_location must be a GeoJSON geometry object.');
  }

  $type = isset($raw['type']) ? (string) $raw['type'] : '';
  if ($type === 'Point') {
    return rovas_civic_parse_field_location_point($raw);
  }
  if ($type === 'LineString') {
    return rovas_civic_parse_field_location_linestring($raw);
  }
  if ($type === 'Polygon') {
    return rovas_civic_parse_field_location_polygon($raw);
  }

  return array('error' => 'field_location type must be Point, LineString, or Polygon.');
}

/**
 * @param array $raw
 *
 * @return array
 */
function rovas_civic_parse_field_location_point(array $raw) {
  if (empty($raw['coordinates']) || !is_array($raw['coordinates'])) {
    return array('error' => 'field_location Point coordinates must be [lon, lat].');
  }
  $pos = rovas_civic_parse_lon_lat_pair($raw['coordinates']);
  if (isset($pos['error'])) {
    return $pos;
  }
  return array(
    'type' => 'Point',
    'coordinates' => $pos,
  );
}

/**
 * @param array $raw
 *
 * @return array
 */
function rovas_civic_parse_field_location_linestring(array $raw) {
  if (empty($raw['coordinates']) || !is_array($raw['coordinates'])) {
    return array('error' => 'field_location LineString needs at least two coordinates.');
  }
  if (count($raw['coordinates']) < 2) {
    return array('error' => 'field_location LineString needs at least two coordinates.');
  }
  $line = array();
  foreach ($raw['coordinates'] as $pos) {
    $pair = rovas_civic_parse_lon_lat_pair($pos);
    if (isset($pair['error'])) {
      return array('error' => 'field_location LineString coordinates must be [lon, lat].');
    }
    $line[] = $pair;
  }
  return array(
    'type' => 'LineString',
    'coordinates' => $line,
  );
}

/**
 * @param array $raw
 *
 * @return array
 */
function rovas_civic_parse_field_location_polygon(array $raw) {
  if (empty($raw['coordinates']) || !is_array($raw['coordinates'])) {
    return array('error' => 'field_location Polygon needs at least one ring.');
  }
  if (empty($raw['coordinates'][0]) || !is_array($raw['coordinates'][0])) {
    return array('error' => 'field_location Polygon needs at least one ring.');
  }
  $outer = $raw['coordinates'][0];
  if (count($outer) < 4) {
    return array('error' => 'field_location Polygon outer ring needs at least four positions.');
  }

  $ring = array();
  foreach ($outer as $pos) {
    $pair = rovas_civic_parse_lon_lat_pair($pos);
    if (isset($pair['error'])) {
      return array('error' => 'field_location Polygon coordinates must be [lon, lat].');
    }
    $ring[] = $pair;
  }

  $first = $ring[0];
  $last = $ring[count($ring) - 1];
  if ($first[0] !== $last[0] || $first[1] !== $last[1]) {
    $ring[] = array($first[0], $first[1]);
  }
  if (count($ring) < 4) {
    return array('error' => 'field_location Polygon outer ring needs at least four positions.');
  }

  return array(
    'type' => 'Polygon',
    'coordinates' => array($ring),
  );
}

/**
 * @param mixed $pos
 *
 * @return array
 *   [lon, lat] or array(error => …)
 */
function rovas_civic_parse_lon_lat_pair($pos) {
  if (!is_array($pos) || count($pos) < 2) {
    return array('error' => 'invalid');
  }
  $lon = $pos[0];
  $lat = $pos[1];
  if (!is_numeric($lon) || !is_numeric($lat) || !is_finite((float) $lon) || !is_finite((float) $lat)) {
    return array('error' => 'invalid');
  }
  $lon = (float) $lon;
  $lat = (float) $lat;
  if ($lon < -180 || $lon > 180 || $lat < -90 || $lat > 90) {
    return array('error' => 'invalid');
  }
  return array($lon, $lat);
}

/**
 * Validate geometry with geoPHP; reject invalid/self-intersecting polygons.
 *
 * @param array $geometry
 *   Normalized GeoJSON geometry from rovas_civic_parse_field_location_geojson().
 *
 * @return true|string
 *   TRUE when valid, else error message.
 */
function rovas_civic_validate_field_location_geometry(array $geometry) {
  if (!function_exists('geophp_load')) {
    return 'field_location geometry is invalid.';
  }
  geophp_load();

  $json = drupal_json_encode($geometry);
  $geom = geoPHP::load($json, 'geojson');
  if (!$geom || (method_exists($geom, 'isEmpty') && $geom->isEmpty())) {
    return 'field_location geometry is invalid.';
  }

  if ($geometry['type'] === 'Polygon' && method_exists($geom, 'checkValidity')) {
    $validity = $geom->checkValidity();
    if (is_array($validity) && empty($validity['valid'])) {
      return 'field_location Polygon is invalid or self-intersecting.';
    }
  }

  return TRUE;
}

/**
 * Persist field_location + map work-evidence bit on a work_report node.
 *
 * @param object $node
 * @param array $geometry
 *   Normalized GeoJSON geometry.
 */
function rovas_civic_attach_wr_field_location($node, array $geometry) {
  if (!field_info_field('field_location')) {
    return;
  }

  $check = rovas_civic_validate_field_location_geometry($geometry);
  if ($check !== TRUE) {
    return;
  }

  geophp_load();
  $wkt = geoPHP::load(drupal_json_encode($geometry), 'geojson')->out('wkt');
  if ($wkt === '') {
    return;
  }

  $values = geofield_compute_values($wkt);
  if (empty($values)) {
    return;
  }

  $lang = field_language('node', $node, 'field_location');
  if ($lang === NULL) {
    $lang = LANGUAGE_NONE;
  }
  $node->field_location[$lang] = array($values);
  rovas_civic_wr_set_work_evidence_bit($node, 3);
  node_save($node);
}

/**
 * Ensure field_work_evidence includes a proof-type list value.
 *
 * Map proof uses list value "3" (same as Drupal work-report form checkbox).
 *
 * @param object $node
 * @param int|string $bit
 */
function rovas_civic_wr_set_work_evidence_bit($node, $bit) {
  if (!field_info_field('field_work_evidence')) {
    return;
  }
  $bit = (string) $bit;
  $lang = field_language('node', $node, 'field_work_evidence');
  if ($lang === NULL) {
    $lang = !empty($node->language) ? $node->language : LANGUAGE_NONE;
  }

  $existing = field_get_items('node', $node, 'field_work_evidence', $lang);
  $values = array();
  if ($existing) {
    foreach ($existing as $item) {
      if (isset($item['value']) && $item['value'] !== '') {
        $values[(string) $item['value']] = TRUE;
      }
    }
  }
  $values[$bit] = TRUE;

  $node->field_work_evidence[$lang] = array();
  foreach (array_keys($values) as $value) {
    $node->field_work_evidence[$lang][] = array('value' => $value);
  }
}
