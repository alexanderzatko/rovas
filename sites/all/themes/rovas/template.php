<?php

// SANO: overriding display on summaries rendered by the Views Summarize module on the Dispute (parties) pages

/**
 * Theme: Total value for a currency amount
 */
function rovas_views_summarize_type_currency($variables) {
  $data = $variables['data'];
  $total = 0;
  foreach ($data as $val) {
    // This allows for negative values and values w/commas to be added
    // correctly.
    $total += (float) preg_replace('/[^0-9\.\-]/', '', $val);
  }
  return '<strong>' . number_format($total, 0) . ' ME</strong>';
}

//get rid of the "reply" link in comments
function rovas_preprocess_comment(&$variables) {
  unset($variables['content']['links']['comment']['#links']['comment-reply']);
}

//submenu functions
// source of code: https://www.zyxware.com/articles/4757/drupal-how-to-create-multi-level-responsive-drop-down-menu-in-drupal-using-bootstrap

function rovas_preprocess_page(&$vars) {
	//Setting menu in a tree structure
	$main_menu_tree = menu_tree(variable_get('menu_main_links_source', 'main-menu'));
	$vars['main_menu'] =  $main_menu_tree;
	// Get the entire main menu tree
	$main_menu_tree = menu_tree_all_data('main-menu');
	// Add the rendered output to the $main_menu_expanded variable
	$vars['main_menu_expanded'] = menu_tree_output($main_menu_tree);

	if (isset($vars['node']->type) && !empty($vars['node']->type)){
		$vars['theme_hook_suggestions'][] = 'page__'. $vars['node']->type;
	}
}


function rovas_menu_tree__main_menu(&$vars) {        
  return '<ul class="nav navbar-nav">' . $vars['tree'] . '</ul>';
}


function rovas_menu_tree__main_menu_inner($vars) {	
    return '<ul class="dropdown-menu">' . $vars['tree'] . '</ul>';
}


function rovas_menu_link__main_menu($vars) {
	$element = $vars['element'];
	$sub_menu = '';

	if ($element['#below']) {
		foreach ($element['#below'] as $key => $val) {
		if (is_numeric($key)) {
			$element['#below'][$key]['#theme'] = 'menu_link__main_menu_inner'; // 2 level <li>
		}
		}
		$element['#below']['#theme_wrappers'][0] = 'menu_tree__main_menu_inner';  // 2 level <ul>
		$sub_menu = drupal_render($element['#below']);
	}

	$element['#localized_options']['attributes']['class'][] = " dropdown-toggle ";
	$element['#localized_options']['attributes']['role'][] = "menu";
	$element['#localized_options']['attributes']['data-toggle'][] = "dropdown";
	$element['#localized_options']['attributes']['aria-expanded'][] = "true";  

	$output = l($element['#title'], $element['#href'], $element['#localized_options']);

	if ($sub_menu) {
		return '<li class="dropdown">' . $output . $sub_menu . '</li>';
	}
	else {
		$output = l($element['#title'], $element['#href']);
		return '<li class="leaf">' . $output . '</li>';
	}
}


	function rovas_menu_link__main_menu_inner($vars) {
		$element = $vars['element'];
		$sub_menu = '';

		if ($element['#below']) {
			$sub_menu = drupal_render($element['#below']);
		}

		$output = l($element['#title'], $element['#href'], $element['#localized_options']);

		return '<li' . drupal_attributes($element['#attributes']) . '>' . $output . $sub_menu . '</li>';
	}


/**
 * Implements template_preprocess_node().
* hide the field_project_exchange_rate field if it is empty
 */
function rovas_preprocess_node(&$variables) {
  if ($variables['type'] == 'project') {
    if (empty($variables['field_project_exchange_rate'][0]['value'])) {
      hide($variables['content']['field_project_exchange_rate']);
    }
  }
}
