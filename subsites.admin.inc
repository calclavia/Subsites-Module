<?php

/**
 * This PHP file is responsible for all interface and form generation as well as management that happen in the subsite's administeration panel and dashboard.
 */

/**
 * The main admin panel dashboard.
 * Displays a list of subsites in a table, providing an "edit"
 * and a "delete" button allowing the user to alter the subsite.
 */
function subsites_admin_dashboard()
{
	/**
	 * Create a table that lists out all subsites avaliable.
	 */
	$subsites = Subsite::getAll();

	$header = array(
		t('Name'),
		t('Owners'),
		t('Page Count'),
		t('Status'),
		t('Delete')
	);

	$rows = array();

	foreach ($subsites as $subsite)
	{
		if ($subsite != null)
		{
			//String of listed owners.
			$name = array('#markup' => l(t($subsite -> name), 'admin/structure/subsites/' . $subsite -> id));
			$delete = array('#markup' => l(t('Delete'), 'admin/structure/subsites/' . $subsite -> id . '/delete'));
			$rows[] = array(
				render($name),
				$subsite -> getOwnersAsString(),
				count($subsite -> pages),
				t($subsite -> status ? "Enabled" : "Disabled"),
				render($delete)
			);
		}
	}

	$table_attributes = array('id' => 'table_dashboard');

	return t("There are a total of " . count($subsite) . " subsite(s).<br />") . theme("table", array(
		'header' => $header,
		'rows' => $rows,
		'attributes' => $table_attributes
	));
}

/**
 * SUBSITE ADMINISTER MENU (INFO TAB)
 * bsite information form
 */
function subsites_info_form($form, $form_state, $subsiteID = null)
{
	$form = array();

	if ($subsiteID != null)
	{
		$subsite = Subsite::get($subsiteID);
		$form['sid'] = array(
			'#type' => 'hidden',
			'#value' => $subsite -> id,
		);
	}
	else
	{
		$subsite = new Subsite(-1, null, null, null, null, null, 1, 0);
	}

	if ($subsite != null)
	{
		$form['name'] = array(
			'#type' => 'textfield',
			'#title' => t('Name'),
			'#required' => TRUE,
			'#default_value' => $subsite -> name,
		);
		$form['owners'] = array(
			'#type' => 'textfield',
			'#title' => t('Owners'),
			'#default_value' => $subsite -> getOwnersAsString(),
		);
		$form['css'] = array(
			'#type' => 'textarea',
			'#title' => t('CSS'),
			'#default_value' => $subsite -> css,
		);
		$form['theme'] = array(
			'#type' => 'select',
			'#title' => t('Theme'),
			'#options' => getThemeOptions(),
			'#default_value' => $subsite -> theme,
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
		$form['submit'] = array(
			'#type' => 'submit',
			'#value' => t('Save'),
		);
	}

	return $form;
}

/**
 * AD<SUBSITE ADMINISTER MENU (INFO TAB)
 * Submit handler for the subsites_form form.
 */
function subsites_info_form_submit($form, &$form_state)
{
	global $user;

	$subsite = Subsite::get($form_state['values']['sid']);

	/**
	 * Check to see if this is an old site and we want to edit the information,
	 * otherwise, add a new subsite.
	 */
	if ($subsite != null)
	{
		$subsite -> name = $form_state['values']['name'];
		//$subsite -> theme = $form_state['values']['theme'];
		$subsite -> status = $form_state['values']['status'];
		$subsite -> weight = $form_state['values']['weight'];
		$subsite -> enabled = $form_state['values']['status'];
		$subsite -> save();
		drupal_set_message(t('Subsites configuration changes saved.'));

	}
	else
	{
		$sid = Subsite::add($form_state['values']['name'], drupal_json_encode(array($user -> uid)), "", $form_state['values']['css'], -1, $form_state['values']['status'], $form_state['values']['weight']);
		// Allow subsequent #submit handlers to know what the subsite ID is
		$form_state['values']['sid'] = $sid;
		drupal_set_message(t('Successfully added new subsite.'));
	}

	$form_state['redirect'] = 'admin/structure/subsites/';
}

/**
 * Menu callback -- Ask for confirmation of subsite deletion
 */
function subsites_delete_confirm_form($form, &$form_state, $subsiteID)
{
	$subsite = Subsite::get($subsiteID);

	if ($subsite != null)
	{
		$form['sid'] = array(
			'#type' => 'value',
			'#value' => $subsite -> id,
		);

		return confirm_form($form, t('Are you sure you want to delete %name?', array('%name' => $subsite -> name)), isset($_GET['destination']) ? $_GET['destination'] : 'admin/structure/subsites', t('This action cannot be undone.'), t('Delete'), t('Cancel'));
	}
	else
	{
		die("Error! Cannot find subsite with the ID: " . $subsiteID . "!");
	}

}

function subsites_delete_confirm_form_submit($form, &$form_state)
{
	$subsite = Subsite::get($form_state['values']['sid']);

	if ($subsite != null)
	{
		$subsite -> delete();
	}

	$form_state['redirect'] = 'admin/structure/subsites';
}

/**
 * Menu callback -- Ask for confirmation of subsite node deletion
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
