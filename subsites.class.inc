<?php

/**
 * The Subsite Class. Every variable defined in this class will be saved into SQL database.
 */
class Subsite
{

	//The ID of the subsite.
	public $id = 0;

	//The name of the subsite. Will also be the title of the subsite.
	public $name = "";

	//An array of userIDs who own this subsite.
	public $owners = array();

	//A text field containing CSS code to be injected after the current theme's CSS gets injected.
	public $css = "";

	//The Theme ID. -1 will use the default theme of the main site.
	public $theme = -1;

	//The pages that this subsite contains.
	public $pages = array();

	//Is the subsite enabled or disabled?
	public $enabled = false;

	/**
	 * Constructor. Used by MySQL to retrieve data. Do not use this to create new subsites.
	 */
	public function __construct($id, $name, $owners, $css, $theme, $pages, $enabled)
	{
		$this -> id = $id;
		$this -> name = $name;
		$this -> owners = $owners;
		$this -> css = $css;
		$this -> theme = $theme;
		$this -> pages = $pages;
		$this -> enabled = $enabled;
	}

	/**
	 * Create a new subsite.
	 *
	 * @param (string) $name
	 *   The name of the subsite.
	 * @param (string) $pages
	 *   The pages of the subsite.
	 * @param (int) $visibility
	 *   The page visibility mode, similar to block visibility.
	 * @param (string) $css
	 *   CSS text to update.  If set to FALSE the CSS will not be updated.
	 * @param (string) $theme
	 *   The name of theme used for this subsite
	 * @param (int) $weight
	 *   The weight of the subsite
	 * @param (int) $status
	 *   The status of the subsite, 1 = enabled, 0 = disabled.
	 * @return
	 *   An instance of a subsite.
	 */
	public static function create($name, $visibility, $css, $theme, $weight, $status, $owners)
	{
		// insert in db and fetch sid
		$sid = db_insert('subsites') -> fields(array(
			'name' => $name,
			'pages' => $pages,
			'visibility' => $visibility,
			'css_fid' => NULL,
			'theme' => $theme,
			'weight' => $weight,
			'status' => $status,
			'owners' => $owners
		)) -> execute();

		// save css
		$file = file_save_data($css, subsites_get_css_path($sid), FILE_EXISTS_REPLACE);

		// create menu
		$menu_name = _subsites_create_menu($sid, $name, $name);
		db_update('subsites') -> fields(array(
			'menu_name' => $menu_name,
			'css_fid' => $file -> fid,
		)) -> condition('sid', $sid) -> execute();

		// create subsite menu item
		$path = 'admin/structure/subsites/';

		$menu['title'] = $name;
		$menu['description'] = '';

		$link['link_title'] = $menu['title'];
		$link['link_path'] = $path . $sid;
		$link['router_path'] = $path . '%';
		$link['module'] = 'subsites';
		$link['weight'] = $weight;
		$link['plid'] = db_query("SELECT mlid FROM {menu_links} WHERE link_path = :link_path AND module = :module", array(
			':link_path' => 'admin/structure/subsites',
			':module' => 'system'
		)) -> fetchField();

		menu_link_save($link);

		module_invoke_all('subsites_add', subsites_get($sid));

		return $sid;
	}

}
?>