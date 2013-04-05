<?php

/**
 * Menu callback -- Show a list of all subsites.
 */
function subsites_admin_display_form($form, &$form_state)
{
	$subsites = subsites_get_all();
	$themes = _subsites_theme_options();

	$form = array();
	$form['subsites'] = array('#tree' => TRUE);
	foreach ($subsites as $subsite)
	{
		$form['subsites'][$subsite -> sid] = array();
		$form['subsites'][$subsite -> sid]['sid'] = array(
			'#type' => 'hidden',
			'#value' => $subsite -> sid,
		);
		$form['subsites'][$subsite -> sid]['name'] = array('#markup' => $subsite -> name, );
		$form['subsites'][$subsite -> sid]['status'] = array(
			'#type' => 'checkbox',
			'#default_value' => $subsite -> status,
		);
		$form['subsites'][$subsite -> sid]['theme'] = array('#markup' => htmlentities($themes[$subsite -> theme]), );
		$form['subsites'][$subsite -> sid]['weight'] = array(
			'#type' => 'weight',
			'#default_value' => $subsite -> weight,
		);
	$form['subsites'][$subsite -> sid]['owners'] = array(
			'#type' => 'textbox',
			'#default_value' => $subsite -> owners,
		);
		$form['subsites'][$subsite -> sid]['edit'] = array('#markup' => l(t('Edit'), 'admin/structure/subsites/' . $subsite -> sid . '/edit'), );
		$form['subsites'][$subsite -> sid]['delete'] = array('#markup' => l(t('Delete'), 'admin/structure/subsites/delete/' . $subsite -> sid), );
	}

	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Save configuration'),
	);

	return $form;
}

/**
 * Renders the admin_display_form
 */
function theme_subsites_admin_display_form($variables)
{
	$form = $variables['form'];
	drupal_add_tabledrag('subsites-order', 'order', 'sibling', 'subsites-order-weight');

	$header = array(
		array('data' => t('Subsite')),
		array('data' => t('Enabled')),
		array('data' => t('Theme')),
		array('data' => t('Weight')),
array('data' => t('Owners')),
		array(
			'data' => t('Operations'),
			'colspan' => '2',
		),
	);

	$rows = array();
	foreach (element_children($form['subsites']) as $key => $id)
	{
		// Don't take form control structures.
		if (is_array($form['subsites'][$id]['name']))
		{
			$form['subsites'][$id]['weight']['#attributes']['class'] = array('subsites-order-weight');
			$rows[] = array(
				'data' => array(
					drupal_render($form['subsites'][$id]['name']),
					drupal_render($form['subsites'][$id]['status']),
					drupal_render($form['subsites'][$id]['theme']),
					drupal_render($form['subsites'][$id]['weight']),
					drupal_render($form['subsites'][$id]['owners']),
					drupal_render($form['subsites'][$id]['edit']),
					drupal_render($form['subsites'][$id]['delete']),
				),
				'class' => array('draggable'),
			);
		}
	}
	if (empty($rows))
	{
		$rows[] = array( array(
				'data' => t('No subsites available.'),
				'colspan' => '6',
			));
	}

	$output = theme('table', array(
		'header' => $header,
		'rows' => $rows,
		'attributes' => array('id' => 'subsites-order')
	));

	$output .= drupal_render_children($form);

	return $output;
}

/**
 * Process subsites order configuration form submission.
 */
function subsites_admin_display_form_submit($form, &$form_state)
{
	foreach ($form_state['values']['subsites'] as $subsite)
	{
		db_update('subsites') -> fields(array(
			'status' => $subsite['status'],
			'weight' => $subsite['weight'],
			'owners' => $subsite['owners'],
		)) -> condition('sid', $subsite['sid']) -> execute();
		db_update('menu_links') -> fields(array('weight' => $subsite['weight'], )) -> condition('link_path', 'admin/structure/subsites/' . $subsite['sid']) -> execute();
	}

	menu_cache_clear_all();

	drupal_set_message(t('The subsites configuration has been saved.'));
}

/**
 * Subsite information form
 */
function subsites_info_form($form, $form_state, $subsite = NULL)
{
	$form = array();

	if ($subsite)
	{
		$form['sid'] = array(
			'#type' => 'hidden',
			'#value' => $subsite -> sid,
		);
	}
	else
	{
		$subsite = (object) array(
			'name' => NULL,
			'weight' => NULL,
			'status' => NULL
		);
	}

	$form['name'] = array(
		'#type' => 'textfield',
		'#title' => t('Name'),
		'#required' => TRUE,
		'#default_value' => $subsite -> name,
	);
	$form['weight'] = array(
		'#type' => 'weight',
		'#title' => t('Weight'),
		'#default_value' => $subsite -> weight ? $subsite -> weight : 0,
	);
	$form['status'] = array(
		'#type' => 'checkbox',
		'#title' => t('Enabled'),
		'#default_value' => $subsite -> status,
	);
	$form['owners'] = array(
		'#type' => 'textfield',
		'#title' => t('Owners'),
		'#default_value' => $subsite -> owners,
	);
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Save'),
	);

	return $form;
}

/**
 * Submit handler for the subsites_form form.
 */
function subsites_info_form_submit($form, &$form_state)
{
	global $user;

	if (isset($form_state['values']['sid']))
	{
		$old_subsite = subsites_get($form_state['values']['sid']);
		subsites_edit($form_state['values']['sid'], $form_state['values']['name'], $old_subsite -> pages, $old_subsite -> visibility, $old_subsite -> css, $old_subsite -> theme, $form_state['values']['weight'], $form_state['values']['status']);
		$sid = $form_state['values']['sid'];
	}
	else
	{
		$sid = subsites_add($form_state['values']['name'], '', 1, '', -1, 0, $form_state['values']['status'], drupal_json_encode(array($user -> uid)));
		// Allow subsequent #submit handlers to know what the subsite ID is
		$form_state['values']['sid'] = $sid;
	}

	$form_state['redirect'] = 'admin/structure/subsites/' . $sid;

	drupal_set_message(t('subsites configuration changes saved.'));
}

/**
 * Page callback, displays a table of subsite nodes
 */
function subsites_content_nodes($subsite)
{
	$output = '';

	$header = array(
		t('Title'),
		t('Actions')
	);
	$result = db_query("SELECT n.nid, n.title FROM {subsites_node} sn INNER JOIN {node} n ON n.nid = sn.nid WHERE sid = :sid", array(':sid' => $subsite -> sid));
	$rows = array();
	foreach ($result as $row)
	{
		$rows[] = array(
			l($row -> title, 'node/' . $row -> nid),
			l(t('Remove from subsite'), 'admin/structure/subsites/content/delete/' . $subsite -> sid . '/' . $row -> nid),
		);
	}

	if (empty($rows))
	{
		return t('This subsite has no nodes.');
	}

	$output .= theme('table', array(
		'header' => $header,
		'rows' => $rows
	));

	return $output;
}

/**
 * Subsite nodes form
 */
function subsites_content_nodes_form($form, $form_state, $subsite)
{
	$form = array();

	$form['sid'] = array(
		'#type' => 'hidden',
		'#value' => $subsite -> sid,
	);

	$form['existing'] = array(
		'#type' => 'fieldset',
		'#title' => t('Existing nodes'),
	);

	$form['existing']['nodes'] = array('#markup' => subsites_content_nodes($subsite), );

	$form['add'] = array(
		'#type' => 'fieldset',
		'#title' => t('Add a new node'),
		'#description' => t('Find a new node to add to this subsite.'),
	);
	$form['add']['nid'] = array(
		'#type' => 'textfield',
		'#autocomplete_path' => 'subsites/add-node',
	);

	$form['add']['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Add'),
	);

	return $form;
}

/**
 * Submit callback for subsite nodes form
 */
function subsites_content_nodes_form_submit($form, &$form_state)
{
	$matches = array();
	preg_match("%{\[nid:(.*)\]}%i", $form_state['values']['nid'], $matches);

	if (isset($matches[1]) && is_numeric($matches[1]))
	{
		$nid = $matches[1];

		$exists = db_query("SELECT nid FROM {subsites_node} WHERE nid = :nid AND sid = :sid", array(
			':nid' => $nid,
			':sid' => $form_state['values']['sid']
		)) -> fetchField();
		if (!$exists)
		{
			db_insert('subsites_node') -> fields(array(
				'nid' => $nid,
				'sid' => $form_state['values']['sid'],
			)) -> execute();
			drupal_set_message(t('Added node to subsite.'));
		}
		else
		{
			drupal_set_message(t('Node not added. Was already in subsite.'), 'error');
		}
	}
	else
	{
		drupal_set_message(t('Node not added. Not a valid node.'), 'error');
	}
}

/**
 * Autocomplete callback for subsite nodes
 */
function subsites_add_node($string)
{
	$matches = array();
	$result = db_select('node') -> fields('node', array('title')) -> condition('title', db_like($string) . '%', 'LIKE') -> range(0, 10) -> execute();
	foreach ($result as $row)
	{
		$matches['[nid:' . $row -> nid . '] ' . $row -> title] = check_plain($row -> title);
	}
	print drupal_json_encode($matches);
	exit();
}

/**
 * Menu callback -- Ask for confirmation of subsite deletion
 */
function subsites_content_nodes_delete_confirm_form($form, &$form_state, $subsite, $node)
{
	$form['sid'] = array(
		'#type' => 'value',
		'#value' => $subsite -> sid,
	);
	$form['nid'] = array(
		'#type' => 'value',
		'#value' => $node -> nid,
	);

	return confirm_form($form, t('Are you sure you want to delete %node from %subsite?', array(
		'%node' => $node -> title,
		'%subsite' => $subsite -> name
	)), isset($_GET['destination']) ? $_GET['destination'] : 'admin/structure/subsites/' . $subsite -> sid . '/content/nodes/delete/' . $node -> nid, t('This action cannot be undone.'), t('Delete'), t('Cancel'));
}

/**
 * Submit handler for executing subsite deletion after confirmation
 */
function subsites_content_nodes_delete_confirm_form_submit($form, &$form_state)
{
	db_delete('subsites_node') -> condition('sid', $form_state['values']['sid']) -> condition('nid', $form_state['values']['nid']) -> execute();

	$form_state['redirect'] = 'admin/structure/subsites/' . $form_state['values']['sid'] . '/content/nodes/delete/' . $form_state['values']['nid'];

	drupal_set_message(t('Succesfully deleted node from subsite.'));
}

/**
 * Subsite menu selection form
 */
function subsites_content_menu_form($form, $form_state, $subsite)
{
	$form = array();

	$form['sid'] = array(
		'#type' => 'hidden',
		'#value' => $subsite -> sid,
	);

	$menus = _subsites_get_menus();
	if (isset($menus[$subsite -> menu_name]))
	{
		$menu = $menus[$subsite -> menu_name];
	}

	$form['menu_name'] = array(
		'#type' => 'textfield',
		'#title' => t('Menu'),
		'#default_value' => $menu,
		'#disabled' => TRUE,
		'#suffix' => l(t('Edit menu'), 'admin/structure/menu/manage/' . $subsite -> menu_name),
	);

	return $form;
}

/**
 * Subsite content pages form
 */
function subsites_content_pages_form($form, $form_state, $subsite)
{
	$form = array();

	$form['sid'] = array(
		'#type' => 'hidden',
		'#value' => $subsite -> sid,
	);

	$access = user_access('use PHP for settings');
	$visibility = isset($subsite -> visibility) ? $subsite -> visibility : 1;
	$options = array();

	if ($visibility == 2 && !$access)
	{
		$form = array();
		$form['visibility'] = array(
			'#type' => 'value',
			'#value' => 2,
		);
		$form['pages'] = array(
			'#type' => 'value',
			'#value' => $subsite -> pages,
		);
	}
	else
	{
		$options = array(
			t('Show on every page except the listed pages.'),
			t('Show on only the listed pages.')
		);
		$description = t("Enter one page per line as Drupal paths. The '*' character is a wildcard. Example paths are %blog for the blog page and %blog-wildcard for every personal blog. %front is the front page.", array(
			'%blog' => 'blog',
			'%blog-wildcard' => 'blog/*',
			'%front' => '<front>'
		));

		if ($access)
		{
			$options[] = t('Show if the following PHP code returns <code>TRUE</code> (PHP-mode, experts only).');
			$description .= ' ' . t('If the PHP-mode is chosen, enter PHP code between %php. Note that executing incorrect PHP-code can break your Drupal site.', array('%php' => '<?php ?>'));
		}
		$form['visibility'] = array(
			'#type' => 'radios',
			'#title' => t('Activate subsite on the specific pages'),
			'#options' => $options,
			'#default_value' => $visibility,
		);
		$form['pages'] = array(
			'#type' => 'textarea',
			'#title' => t('Pages'),
			'#default_value' => $subsite -> pages,
			'#description' => $description,
		);
	}

	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Save'),
	);

	return $form;
}

/**
 * Submit handler for the subsites_form form.
 */
function subsites_content_pages_form_submit($form, &$form_state)
{
	$old_subsite = subsites_get($form_state['values']['sid']);
	subsites_edit($form_state['values']['sid'], $old_subsite -> name, $form_state['values']['pages'], $form_state['values']['visibility'], $old_subsite -> css, $old_subsite -> theme, $old_subsite -> weight, $old_subsite -> status);

	$form_state['redirect'] = 'admin/structure/subsites/' . $form_state['values']['sid'] . '/content/pages';

	drupal_set_message(t('subsites configuration changes saved.'));
}

/**
 * Subsite appearance / theme selection form
 */
function subsites_look_form($form, $form_state, $subsite)
{
	$form = array();

	$form['sid'] = array(
		'#type' => 'hidden',
		'#value' => $subsite -> sid,
	);

	$css_path = subsites_get_css_path($subsite -> sid);
	$css = NULL;
	if (file_exists($css_path))
	{
		$css = file_get_contents($css_path);
	}

	$form['css'] = array(
		'#type' => 'textarea',
		'#title' => t('CSS'),
		'#default_value' => $css,
	);

	if ($css_path)
	{
		$form['css']['#description'] = t('You can also alter this CSS code by editing the file !css_path', array('!css_path' => $css_path));
	}

	$form['theme'] = array(
		'#type' => 'select',
		'#title' => t('Theme'),
		'#options' => _subsites_theme_options(),
		'#default_value' => $subsite -> theme,
	);

	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Save'),
	);

	return $form;
}

/**
 * Submit handler for the subsites_form form.
 */
function subsites_look_form_submit($form, &$form_state)
{
	$old_subsite = subsites_get($form_state['values']['sid']);
	subsites_edit($form_state['values']['sid'], $old_subsite -> name, $old_subsite -> pages, $old_subsite -> visibility, $form_state['values']['css'], $form_state['values']['theme'], $old_subsite -> weight, $old_subsite -> status);

	$form_state['redirect'] = 'admin/structure/subsites/' . $form_state['values']['sid'] . '/look';

	drupal_set_message(t('subsites configuration changes saved.'));
}

/**
 * Menu callback -- Ask for confirmation of subsite deletion
 */
function subsites_delete_confirm_form($form, &$form_state, $subsite)
{
	$form['sid'] = array(
		'#type' => 'value',
		'#value' => $subsite -> sid,
	);

	return confirm_form($form, t('Are you sure you want to delete %name?', array('%name' => $subsite -> name)), isset($_GET['destination']) ? $_GET['destination'] : 'admin/structure/subsites', t('This action cannot be undone.'), t('Delete'), t('Cancel'));
}

/**
 * Submit handler for executing subsite deletion after confirmation
 */
function subsites_delete_confirm_form_submit($form, &$form_state)
{
	subsites_delete($form_state['values']['sid']);

	$form_state['redirect'] = 'admin/structure/subsites';
}
