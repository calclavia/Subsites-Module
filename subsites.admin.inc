<?php
/**
 * SUBSITE ADMINISTER MENU
 * Menu callback -- Show a list of all subsites.
 */
function subsites_admin_display_form($form, &$form_state)
{
	$subsites = Subsite::getAll();
	//$themes = _subsites_theme_options();

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
 * SUBSITE ADMINISTER MENU (INFO TAB)
 * bsite information form
 */
function subsites_info_form($form, $form_state, $subsite = NULL)
{
	$form = array();
	$subsite = new Subsite(-1, null, null, null, null, null, 1, 0);

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
		'#title' => t('Owners'),
		'#default_value' => $subsite -> owners,
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
		$old_subsite = Subsite::get($form_state['values']['sid']);
		Subsite::edit($form_state['values']['sid'], $form_state['values']['name'], $old_subsite -> pages, $old_subsite -> visibility, $old_subsite -> css, $old_subsite -> theme, $form_state['values']['weight'], $form_state['values']['status']);
		$sid = $form_state['values']['sid'];
	}
	else
	{
		$sid = Subsite::add($form_state['values']['name'], drupal_json_encode(array($user -> uid), "", $form_state['values']['css'], -1, $form_state['values']['status'], $form_state['values']['weight']));
		// Allow subsequent #submit handlers to know what the subsite ID is
		$form_state['values']['sid'] = $sid;
	}

	$form_state['redirect'] = 'admin/structure/subsites/' . $sid;

	drupal_set_message(t('subsites configuration changes saved.'));
}
