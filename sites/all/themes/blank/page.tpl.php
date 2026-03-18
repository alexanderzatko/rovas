<!-- $page is the content variable -->

<div id="page">
	<div id="content" class="clearfix">
		<div class="element-invisible"><a id="main-content"></a></div>
		<img src="http://www.merit.world/sites/default/files/me_logo_bw.svg" style="display: block;float: left;width: 4%;padding: 7pt;">
		<?php 

		$result = db_query("SELECT count(node.nid) AS count
		FROM {node} node
		LEFT JOIN {field_data_field_project} p ON node.nid = p.entity_id 
		WHERE p.field_project_target_id = :node
		AND p.entity_type = 'node' 
		AND p.deleted = '0'
		AND node.status = '1'
		AND node.type IN  ('co_worker_merit_allocation')", array(':node' => arg(1)));

		$resarray = $result->fetchAll();
		$workers = $resarray[0]->count;

		$obj = entity_metadata_wrapper('node', node_load(arg(1)));
		$merits = $obj->field_merit_reward->value();

		$result2 = db_query("SELECT SUM(field_total_hours_worked_value) AS sum
		FROM {node} node
		LEFT JOIN {field_data_field_project} p ON node.nid = p.entity_id 
		LEFT JOIN {field_data_field_total_hours_worked} h ON node.nid = h.entity_id 
		AND p.entity_type = 'node' 
		AND h.entity_type = 'node' 
		AND p.deleted = '0'
		AND h.deleted = '0'
		WHERE p.field_project_target_id = :node
		AND node.status = '1'
		AND node.type IN  ('co_worker_merit_allocation')", array(':node' => arg(1)));

		$resarray2 = $result2->fetchAll();
		$hours = $resarray2[0]->sum;

		print '<strong>' . t('NEO project') . '</strong>: <a href = "http://rovas.merit.world/node/'.arg(1).'" target="_blank">' . arg(1) . '</a><br/>' . 
		'<strong>' . t('Hours') . '</strong>: ' . round($hours) . '<br/>' .
		'<strong>' . t('Workers') . '</strong>: ' . $workers . '<br/>' . 
		'<strong>' . t('Merits') . '</strong>: ' . round($merits);
		 ?>
	</div>

</div>
