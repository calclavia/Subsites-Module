<?php

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
}
?>