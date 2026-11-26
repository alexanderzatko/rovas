<?php

/**
 * Fix incorrect "zásluhy" translations for Merit on rules/glossary pages.
 * Run: cd /var/www/rovdev && drush php-script /path/to/fix_merit_translation.php
 *
 * Related content rules for SK/CS:
 * - Use "Rováš" (with inflections) instead of "Rovas" in visible text.
 * - Keep lowercase "rovas" in URLs, anchors, and technical identifiers.
 * - Use chron/chrony/chronov/chronom/chronmi (never kron*); vzor strom.
 * - Use merit/merity/meritov/meritom/meritmi (never zásluh*); vzor dub.
 * - SK: lowercase chron/merit mid-sentence (see .cursor/rules/sk-chron-merit-terminology.mdc).
 */

function merit_replacements_sk() {
  return [
    'odmenu za zásluhy' => 'meritovú odmenu',
    'odmene za zásluhy' => 'meritovej odmene',
    'odmeny za zásluhy' => 'meritové odmeny',
    'Oddelenie odmeny za zásluhy' => 'Oddelenie meritovej odmeny',
    'kumulatívneho skóre zásluh' => 'kumulatívneho meritového skóre',
    'od skóre zásluh' => 'od meritového skóre',
    'skóre zásluh' => 'meritové skóre',
    'rozdelenie zásluh' => 'rozdelenie meritov',
    'množstvom zásluh' => 'množstvom meritov',
    'počtom zásluh' => 'počtom meritov',
    'na zásluhy' => 'na merity',
    'nové zásluhy' => 'nové merity',
    'desať zásluh' => 'desať meritov',
    'získavať zásluhy' => 'získavať merity',
    'získať zásluhy' => 'získať merity',
    'chronogramy a zásluhy' => 'chrony a merity',
    'zásluhy sa' => 'merity sa',
    'zásluhy,' => 'merity,',
    'zásluhy.' => 'merity.',
    'zásluhy ' => 'merity ',
    'zásluhy' => 'merity',
    'zásluh' => 'meritov',
  ];
}

function merit_replacements_cs() {
  return [
    'Rozdělení odměn za zásluhy' => 'Rozdělení Meritů',
    'Přidělení odměny podle zásluh' => 'Přidělení podle Meritů',
    'Oddělení odměny za zásluhy' => 'Oddělení Meritové odměny',
    'odměnu za zásluhy' => 'Merity',
    'odměny za zásluhy' => 'Merity',
    'za zásluhy' => 'za Merit',
    'podle zásluh' => 'podle Meritů',
    'počtem zásluh' => 'počtem Meritů',
    'na zásluhy' => 'na Merity',
    'zásluhovým' => 'Meritovým',
    'zásluhách' => 'Meritech',
    'Zásluhy jsou' => 'Merit je',
    'Zásluhy' => 'Merit',
    'zásluhy získané' => 'Merity získané',
    'zásluhy,' => 'Merity,',
    'zásluhy.' => 'Merity.',
    'zásluhy ' => 'Merity ',
    'zásluhy' => 'Merity',
    'zásluh jsou' => 'Meritů jsou',
    'zásluh' => 'Meritů',
  ];
}

function apply_replacements($text, $map) {
  foreach ($map as $from => $to) {
    $text = str_replace($from, $to, $text);
  }
  return $text;
}

$targets = [
  ['nid' => 1838, 'lang' => 'sk', 'map' => merit_replacements_sk()],
  ['nid' => 1838, 'lang' => 'cs', 'map' => merit_replacements_cs()],
  ['nid' => 35379, 'lang' => 'cs', 'map' => merit_replacements_cs()],
];

foreach ($targets as $target) {
  $nid = $target['nid'];
  $lang = $target['lang'];
  $map = $target['map'];
  $rows = db_query('SELECT body_value FROM {field_data_body} WHERE entity_id = :nid AND language = :lang', [
    ':nid' => $nid,
    ':lang' => $lang,
  ]);
  foreach ($rows as $row) {
    $old = $row->body_value;
    $new = apply_replacements($old, $map);
    if ($new === $old) {
      echo "No changes for node $nid ($lang)\n";
      continue;
    }
    $before = substr_count(mb_strtolower($old, 'UTF-8'), 'zásluh');
    $after = substr_count(mb_strtolower($new, 'UTF-8'), 'zásluh');
    db_update('field_data_body')
      ->fields(['body_value' => $new])
      ->condition('entity_id', $nid)
      ->condition('language', $lang)
      ->execute();
    db_update('field_revision_body')
      ->fields(['body_value' => $new])
      ->condition('entity_id', $nid)
      ->condition('language', $lang)
      ->execute();
    echo "Updated node $nid ($lang): zásluh count $before -> $after\n";
  }
}

$remaining = db_query("SELECT entity_id, language, (LENGTH(body_value)-LENGTH(REPLACE(LOWER(body_value),'zásluh','')))/LENGTH('zásluh') AS cnt FROM {field_data_body} WHERE entity_id IN (1838,35379) AND language IN ('cs','sk') HAVING cnt > 0");
foreach ($remaining as $r) {
  echo "REMAINING node {$r->entity_id} {$r->language}: {$r->cnt}\n";
}
