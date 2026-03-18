<?php

/**
 * @file
 * Template to display a view as a table.
 *
 * - $title : The title of this group of rows.  May be empty.
 * - $header: An array of header labels keyed by field id.
 * - $caption: The caption for this table. May be empty.
 * - $header_classes: An array of header classes keyed by field id.
 * - $fields: An array of CSS IDs to use for each field id.
 * - $classes: A class or classes to apply to the table, based on settings.
 * - $row_classes: An array of classes to apply to each row, indexed by row
 *   number. This matches the index in $rows.
 * - $rows: An array of row items. Each row is an array of content.
 *   $rows are keyed by row number, fields within rows are keyed by field ID.
 * - $field_classes: An array of classes to apply to each field, indexed by
 *   field id, then row number. This matches the index in $rows.
 *
 * @ingroup views_templates
 */
?>
<div class="table-responsive">
<table <?php if ($classes): ?> class="<?php print $classes; ?>"<?php endif ?><?php print $attributes; ?>>
   <?php if (!empty($title) || !empty($caption)): ?>
     <caption><?php print $caption . $title; ?></caption>
  <?php endif; ?>
  <?php if (!empty($header)) : ?>
    <thead>
      <tr>
        <?php foreach ($header as $field => $label): ?>
          <th <?php if ($header_classes[$field]): ?> class="<?php print $header_classes[$field]; ?>"<?php endif; ?> scope="col">          
            <?php print $label;
            	//provide help text for columns on the /invest page
				if (isset($row_classes[0][1]) && $row_classes[0][1] == 'table-investing') {
					$renderhelp = FALSE;
					switch ($field) {
						case 'php': //yield
						$col_help = t('The amount of Merits an hour purchased or worked would receive, if the project was rewarded with 100 Chrons');
						$renderhelp = TRUE;
						break;
						case 'field_project_total_hours': //time
						$col_help = t('The sum of labor and purchased time');
						$renderhelp = TRUE;
						break;
						case 'field_rate': //share cost
						$col_help = t('The cost of one hour share');
						$renderhelp = TRUE;
						break;
						case 'field_merit_reward':
						$col_help = t('The total number of Merits awarded to the project');
						$renderhelp = TRUE;
						break;
						default:
						$col_help = $field;
					}
					if ($renderhelp) {
						print "&nbsp;<a 
							tabindex='0'
							role='button'
							data-content='<div>".$col_help."</div>'
							data-toggle='popover'
							data-placement='top'
							data-trigger='click'
							data-html='true'><span class='help-r'><i class='glyphicon glyphicon-question-sign'></i></span>
							</a>
						";
						}
					}

            	//provide help text for columns on the /find_project page

				if (isset($row_classes[0][1]) && $row_classes[0][1] == 'table-projects') {
					$renderhelp = FALSE;
					switch ($field) {
						case 'field_merge_daily_reports':
						$col_help = t('Is the project set up to combine work reports?');
						$renderhelp = TRUE;
						break;
						default:
						$col_help = $field;
					}
					if ($renderhelp) {
						print "&nbsp;<a 
							tabindex='0'
							role='button'
							data-content='<div>".$col_help."</div>'
							data-toggle='popover'
							data-placement='top'
							data-trigger='click'
							data-html='true'><span class='help-r'><i class='glyphicon glyphicon-question-sign'></i></span>
							</a>
						";
						}
					}

				 ?>
          </th>
        <?php endforeach; ?>
      </tr>
    </thead>
  <?php endif; ?>
  <tbody>
    <?php foreach ($rows as $row_count => $row): ?>
      <tr <?php if (isset ($row_classes[$row_count]) && $row_classes[$row_count]): ?> class="<?php print implode(' ', $row_classes[$row_count]); ?>"<?php endif; ?>>
        <?php foreach ($row as $field => $content): ?>
          <td <?php if ($field_classes[$field][$row_count]): ?> class="<?php print $field_classes[$field][$row_count]; ?>"<?php endif; ?><?php print drupal_attributes($field_attributes[$field][$row_count]); ?>>
            <?php print $content; ?>
          </td>
        <?php endforeach; ?>
      </tr>
    <?php endforeach; ?>
  </tbody>
</table>
</div>