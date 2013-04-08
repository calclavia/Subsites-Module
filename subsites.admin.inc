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
		'Name',
		'Owners',
		'Page Count',
		'Status',
		'Edit',
		'Delete'
	);

	$rows = array();

	foreach ($subsites as $subsite)
	{
		if ($subsite != null)
		{
			//String of listed owners.
			$owners = "";

			foreach ($subsite->owners as $ownerID)
			{
				$user = user_load($ownerID);
				$owners .= $username = $user -> name . ", ";
			}

			$edit = array('#markup' => l(t('Edit'), 'admin/structure/subsites/' . $subsite -> id . '/edit'));
			$delete = array('#markup' => l(t('Delete'), 'admin/structure/subsites/' . $subsite -> id . '/delete'));
			$rows[] = array(
				$subsite -> name,
				$owners,
				count($subsite -> pages),
				$subsite -> status,
				render($edit),
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
function subsites_info_form($form, $form_state, $subsite = NULL)
{
	if ($subsite)
	{
		$subsite = Subsite::get($subsite);
		$form['sid'] = array(
			'#type' => 'hidden',
			'#value' => $subsite -> id,
		);
	}
	else
	{
		$subsite = new Subsite(-1, null, null, null, null, null, 1, 0);
	}

	$form = array();

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
			'#default_value' => $subsite -> owners,
		);
		$form['css'] = array(
			'#type' => 'textfield',
			'#title' => t('CSS File'),
			'#default_value' => $subsite -> css,
		);
		$form['theme'] = array(
			'#type' => 'textfield',
			'#title' => t('Owners'),
			'#default_value' => $subsite -> owners,
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

	/**
	 * Check to see if this is an old site and we want to edit the information,
	 * otherwise, add a new subsite.
	 */
	if (isset($form_state['values']['sid']))
	{
		$subsite = Subsite::get($form_state['values']['sid']);
		$subsite -> name = $form_state['values']['name'];
		$subsite -> theme = $form_state['values']['theme'];
		$subsite -> status = $form_state['values']['status'];
		$subsite -> weight = $form_state['values']['weight'];
		$subsite -> save();
	}
	else
	{
		$sid = Subsite::add($form_state['values']['name'], drupal_json_encode(array($user -> uid)), "", $form_state['values']['css'], -1, $form_state['values']['status'], $form_state['values']['weight']);
		// Allow subsequent #submit handlers to know what the subsite ID is
		$form_state['values']['sid'] = $sid;
	}

	$form_state['redirect'] = 'admin/structure/subsites/';
	drupal_set_message(t('subsites configuration changes saved.'));
}
