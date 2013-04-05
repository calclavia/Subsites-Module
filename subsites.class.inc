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

	//The pages that this subsite contains.
	public $pages = array();

	//A text field containing CSS code to be injected after the current theme's CSS gets injected.
	public $css = "";

	//The Theme ID. -1 will use the default theme of the main site.
	public $theme = -1;

	//Is the subsite enabled or disabled? 0 for disabled, 1 for enabled.
	public $status = false;

	//Is the subsite enabled or disabled? 0 for disabled, 1 for enabled.
	public $weight = 0;

	/**
	 * Constructor. Used by MySQL to retrieve data. Do not use this to create new subsites.
	 */
	public function __construct($id, $name, $owners, $css, $theme, $pages, $status, $weight)
	{
		$this -> id = $id;
		$this -> name = $name;
		$this -> owners = $owners;
		$this -> css = $css;
		$this -> theme = $theme;
		$this -> pages = $pages;
		$this -> status = $status;
		$this -> weight = $weight;

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
	public static function create($name, $owners, $css, $theme, $pages, $status, $weight)
	{
		// insert in db and fetch sid
		$sid = db_insert('subsites') -> fields(array(
			'name' => $name,
			'pages' => $pages,
			'css_fid' => $css,
			'theme' => $theme,
			'status' => $status,
			'owners' => $owners,
			'weight' => $weight
		)) -> execute();

		/*
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

		 menu_link_save($link);*/

		$subsite = Subsites::get($sid);

		module_invoke_all('subsites_add', $subsite);

		return $subsite;
	}

	/**
	 * Gets a subsite from the database.
	 *
	 * @param $sid
	 *   Int; The subsite id of the subsite.
	 */
	public static function get($sid)
	{
		if (!$sid)
		{
			return false;
		}

		$subsite = db_query("SELECT * FROM {subsites} WHERE sid = :sid", array(':sid' => $sid)) -> fetchObject();
		$path = subsites_get_css_path($sid);
		if (is_file($path))
		{
			$subsite -> css = file_get_contents($path);
		}
		else
		{
			$subsite -> css = '';
		}

		return new Subsite($subsite -> sid, $subsite -> name, drupal_json_decode($subsite -> owners), $subsite -> css, $subsite -> theme, drupal_json_decode($subsite -> pages), $subsite -> status, $subsite -> weight);
	}

	public static function getAll($only_active = FALSE)
	{
		$subsites = array();

		if ($only_active)
		{
			$result = db_query("SELECT * FROM {subsites} WHERE status = :status ORDER BY weight ASC", array(':status' => 1));
		}
		else
		{
			$result = db_query("SELECT * FROM {subsites} ORDER BY weight ASC");
		}

		foreach ($result as $subsite)
		{
			$subsites[] = $subsite;
		}

		return $subsites;
	}

	/**
	 * Saves the existing subsite into the database.
	 */

	public function save()
	{
		// save css
		// $file = file_save_data($this->css, subsites_get_css_path($sid), FILE_EXISTS_REPLACE);

		// update db
		db_update('subsites') -> fields(array(
			'name' => $this -> name,
			'owners' => $this -> owners,
			'css_fid' => $this -> css,
			'theme' => $this -> theme,
			'pages' => $this -> pages,
			'status' => $this -> status,
		)) -> condition('sid', $sid) -> execute();

		// update menu item weight
		// db_update('menu_links') -> fields(array('weight' => $weight, )) -> condition('link_path', 'admin/structure/subsites/' . $sid) -> execute();

		module_invoke_all('subsites_edit', $this);
	}

	/**
	 * Delete an existing subsite.
	 */
	public function delete()
	{
		// Delete db entry
		db_delete('subsites') -> condition('sid', $sid) -> execute();

		module_invoke_all('subsites_delete', $subsite);
		unset($this);
	}

}
?>