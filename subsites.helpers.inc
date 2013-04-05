<?php
/* $Id: subsites.helpers.inc,v 1.5 2010/10/05 10:38:59 davyvandenbremt Exp $ */

/**
 * Retrieve the subsite record for a certain subsite id.
 *
 * @param $sid
 *   The subsite id of the requested subsite.
 * @return
 *   The requested subsite.
 */
function subsites_get($sid) {
  if (!$sid) return FALSE;
  return db_fetch_object(db_query("SELECT * FROM {subsites} WHERE sid = %d", $sid));
}

/**
 * Retrieve all subsites.
 *
 * @return
 *   An array of subsites.
 */
function subsites_get_all($only_active = FALSE) {
  $subsites = array();

  if ($only_active) {
    $result = db_query("SELECT * FROM {subsites} WHERE status = 1 ORDER BY weight ASC");
  }
  else {
    $result = db_query("SELECT * FROM {subsites} ORDER BY weight ASC");
  }

  while ($subsite = db_fetch_object($result)) {
    $subsites[] = $subsite;
  }
  return $subsites;
}

/**
 * Create a new subsite.
 *
 * @param $name
 *   String; The name of the subsite.
 * @param $pages
 *   String; The pages of the subsite.
 * @return
 *   The requested subsite.
 */
function subsites_add($name, $pages, $visibility, $css, $theme, $weight, $status) {
  // insert in db
  db_query("INSERT INTO {subsites} (name, pages, visibility, theme, weight, status) VALUES('%s', '%s', %d, '%s', %d, %d)", $name, $pages, $visibility, $theme, $weight, $status);

  // fetch sid
  $sid = db_last_insert_id('subsites', 'sid');

  // create menu
  $menu_name = _subsites_create_menu($sid, $name, $name);
  db_query("UPDATE {subsites} SET menu_name = '%s' WHERE sid = %d", $menu_name, $sid);

  // create subsite menu item
  $path = 'admin/build/subsites/';

  $menu['title'] = $name;
  $menu['description'] = '';

  $link['link_title'] = $menu['title'];
  $link['link_path'] = $path . $sid;
  $link['router_path'] = $path .'%';
  $link['module'] = 'subsites';
  $link['weight'] = $weight;
  $link['plid'] = db_result(db_query("SELECT mlid FROM {menu_links} WHERE link_path = '%s' AND module = '%s'", 'admin/build/subsites', 'system'));

  menu_link_save($link);

  // save css
  file_check_directory(file_create_path('subsites'), FILE_CREATE_DIRECTORY);
  file_check_directory(file_create_path('subsites/'. $sid), FILE_CREATE_DIRECTORY);
  file_save_data($css, subsites_get_css_path($sid), FILE_EXISTS_REPLACE);

  return $sid;
}

/**
 * Edit an existing subsite.
 *
 * @param $sid
 *   Int; The subsite id of the subsite.
 * @param $name
 *   String; The name of the subsite.
 * @param $pages
 *   String; The pages of the subsite.
 * @return
 *   The requested subsite.
 */
function subsites_edit($sid, $name, $pages, $visibility, $css, $theme, $weight, $status) {
  // update db
  db_query("UPDATE {subsites} SET name = '%s', pages = '%s', visibility = %d, theme = '%s', weight = %d, status = %d WHERE sid = %d", $name, $pages, $visibility, $theme, $weight, $status, $sid);

  // update menu item weight
  db_query("UPDATE {menu_links} SET weight = %d WHERE link_path = '%s'", $weight, 'admin/build/subsites/'. $sid);

  // save css
  file_check_directory(file_create_path('subsites'), FILE_CREATE_DIRECTORY);
  file_check_directory(file_create_path('subsites/'. $sid), FILE_CREATE_DIRECTORY);
  file_save_data($css, subsites_get_css_path($sid), FILE_EXISTS_REPLACE);
}

/**
 * Delete an existing subsite.
 *
 * @param $sid
 *   Int; The subsite id of the subsite.
 */
function subsites_delete($sid) {
  $subsite = subsites_get($sid);

  // delete menu
  _subsites_delete_menu($subsite->menu_name);

  // delete subsite menu item
  $path = 'admin/build/subsites/'. $sid;
  menu_link_delete(NULL, $path);

  // delete css
  file_delete(subsites_get_css_path($sid));

  // delete db entry
  db_query("DELETE FROM {subsites} WHERE sid = %d", $sid);
}

function subsites_get_current() {
  static $sid;

  if ($sid === NULL) {
    $detection_methods = module_invoke_all('subsite_detection_methods');
    drupal_alter('subsite_detection_methods', $detection_methods);

    foreach ($detection_methods as $method => $method_info) {
      if (isset($method_info['file'])) {
        include_once($method_info['file']);
      }
      $sid = call_user_func($method);
      if ($sid) {
        break;
      }
    }
  }

  return $sid;
}

function _subsites_get_current_by_pages() {
  $subsites = subsites_get_all(TRUE);

  foreach ($subsites as $subsite) {
    if (_subsites_match_path($subsite->pages, $subsite->visibility)) {
      return $subsite->sid;
    }
  }

  return FALSE;
}

function _subsites_get_current_by_menu() {
  $item = menu_get_item();

  $active_menu_name = db_result(db_query("SELECT menu_name FROM {menu_links} WHERE link_path = '%s'", $item['href']));

  $subsites = subsites_get_all(TRUE);

  if ($active_menu_name) {
    foreach ($subsites as $subsite) {
      if ($subsite->menu_name == $active_menu_name) {
        return $subsite->sid;
      }
    }
  }

  return FALSE;
}

function _subsites_get_current_by_node() {
  $node = menu_get_object();

  if ($node && !arg(2) && $node->subsite) {
    $subsite = subsites_load($node->subsite);
    if ($subsite->status) {
      return $node->subsite;
    }
  }

  return FALSE;
}

/**
 * Get a list of subsite options to use in select fields, checkboxes, ....
 *
 * @return
 *   Array; Keys are subsite ids. Values are subsite names.
 */
function _subsites_select_options($add_default = TRUE) {
  $options = array();

  if ($add_default) {
    $options[-1] = t('<None>');
  }

  $subsites = subsites_get_all();
  foreach ($subsites as $subsite) {
    $options[$subsite->sid] = $subsite->name;
  }
  return $options;
}

/**
 * Retrieve a list of all menus.
 *
 * @return
 *   Array; Keys are the 'menu_name's. Values are the Menu titles.
 */
function _subsites_get_menus() {
  $result = db_query("SELECT * FROM {menu_custom} ORDER BY title");
  $content = array();
  while ($menu = db_fetch_object($result)) {
    $content[$menu->menu_name] = $menu->title;
  }

  return $content;
}

function _subsites_match_path($pages, $visibility) {
  if ($visibility < 2) {
    $path = drupal_get_path_alias($_GET['q']);
    // Compare with the internal and path alias (if any).
    $page_match = drupal_match_path($path, $pages);
    if ($path != $_GET['q']) {
      $page_match = $page_match || drupal_match_path($_GET['q'], $pages);
    }
    // When $visibility has a value of 0, the block is displayed on
    // all pages except those listed in $pages. When set to 1, it
    // is displayed only on those pages listed in $pages.
    if ($page_match = !($visibility xor $page_match)) {
      return TRUE;
    }
  }
  else {
    if (drupal_eval($pages)) {
      return TRUE;
    }
  }

  return FALSE;
}

/**
 * Create a menu for a subsite.
 *
 * @param $sid
 *   Integer; The subsite id to create a menu for.
 * @param $title
 *   String; The title of the menu.
 * @return
 *   Integer; Menu ID
 */
function _subsites_create_menu($sid, $title, $description) {
  $path = 'admin/build/menu-customize/';

  $menu['menu_name'] = sprintf('subsite-%d-%s', $sid, _subsites_slug($title, 32)); // maximum menu_name length is 32
  $menu['title'] = $title;
  $menu['description'] = '';

  $link['link_title'] = $menu['title'];
  $link['link_path'] = $path . $menu['menu_name'];
  $link['router_path'] = $path .'%';
  $link['module'] = 'menu';
  $link['plid'] = db_result(db_query("SELECT mlid FROM {menu_links} WHERE link_path = '%s' AND module = '%s'", 'admin/build/menu', 'system'));

  menu_link_save($link);

  db_query("INSERT INTO {menu_custom} (menu_name, title, description) VALUES ('%s', '%s', '%s')", $menu['menu_name'], $menu['title'], $menu['description']);

  return $menu['menu_name'];
}

function _subsites_delete_menu($menu_name) {
  // Reset all the menu links defined by the system via hook_menu.
  $result = db_query("SELECT * FROM {menu_links} ml INNER JOIN {menu_router} m ON ml.router_path = m.path WHERE ml.menu_name = '%s' AND ml.module = 'system' ORDER BY m.number_parts ASC", $menu_name);
  while ($item = db_fetch_array($result)) {
    menu_reset_item($item);
  }

  // Delete all links to the overview page for this menu.
  $result = db_query("SELECT mlid FROM {menu_links} ml WHERE ml.link_path = '%s'", 'admin/build/menu-customize/'. $menu_name);
  while ($m = db_fetch_array($result)) {
    menu_link_delete($m['mlid']);
  }

  // Delete all the links in the menu and the menu from the list of custom menus.
  db_query("DELETE FROM {menu_links} WHERE menu_name = '%s'", $menu_name);
  db_query("DELETE FROM {menu_custom} WHERE menu_name = '%s'", $menu_name);

  // Delete all the blocks for this menu.
  db_query("DELETE FROM {blocks} WHERE module = 'menu' AND delta = '%s'", $menu_name);
  db_query("DELETE FROM {blocks_roles} WHERE module = 'menu' AND delta = '%s'", $menu_name);

  // Clear caches
  menu_cache_clear_all();
  cache_clear_all();
}

/**
 * Calculate a slug with a maximum length for a string.
 *
 * @param $string
 *   The string you want to calculate a slug for.
 * @param $length
 *   The maximum length the slug can have.
 * @return
 *   A string representing the slug
 */
function _subsites_slug($string, $length = 50, $separator = '-') {
  // transliterate
  $string = _subsites_transliterate($string);

  // lowercase
  $string = strtolower($string);

  // replace non alphanumeric and non underscore charachters by separator
  $string = preg_replace('/[^a-z0-9]/i', $separator, $string);

  // replace multiple occurences of separator by one instance
  $string = preg_replace('/'. preg_quote($separator) .'['. preg_quote($separator) .']*/', $separator, $string);

  // cut off to maximum length
  if ($length > -1 && strlen($string) > $length) {
    $string = substr($string, 0, $length);
  }

  // remove separator from start and end of string
  $string = preg_replace('/'. preg_quote($separator) .'$/', '', $string);
  $string = preg_replace('/^'. preg_quote($separator) .'/', '', $string);

  return $string;
}

/**
 * Transliterate a given string.
 *
 * @param $string
 *   The string you want to transliterate.
 * @return
 *   A string representing the transliterated version of the input string.
 */
function _subsites_transliterate($string) {
  static $charmap;
  if (!$charmap) {
    $charmap = array(
      // Decompositions for Latin-1 Supplement
      chr(195) . chr(128) => 'A', chr(195) . chr(129) => 'A',
      chr(195) . chr(130) => 'A', chr(195) . chr(131) => 'A',
      chr(195) . chr(132) => 'A', chr(195) . chr(133) => 'A',
      chr(195) . chr(135) => 'C', chr(195) . chr(136) => 'E',
      chr(195) . chr(137) => 'E', chr(195) . chr(138) => 'E',
      chr(195) . chr(139) => 'E', chr(195) . chr(140) => 'I',
      chr(195) . chr(141) => 'I', chr(195) . chr(142) => 'I',
      chr(195) . chr(143) => 'I', chr(195) . chr(145) => 'N',
      chr(195) . chr(146) => 'O', chr(195) . chr(147) => 'O',
      chr(195) . chr(148) => 'O', chr(195) . chr(149) => 'O',
      chr(195) . chr(150) => 'O', chr(195) . chr(153) => 'U',
      chr(195) . chr(154) => 'U', chr(195) . chr(155) => 'U',
      chr(195) . chr(156) => 'U', chr(195) . chr(157) => 'Y',
      chr(195) . chr(159) => 's', chr(195) . chr(160) => 'a',
      chr(195) . chr(161) => 'a', chr(195) . chr(162) => 'a',
      chr(195) . chr(163) => 'a', chr(195) . chr(164) => 'a',
      chr(195) . chr(165) => 'a', chr(195) . chr(167) => 'c',
      chr(195) . chr(168) => 'e', chr(195) . chr(169) => 'e',
      chr(195) . chr(170) => 'e', chr(195) . chr(171) => 'e',
      chr(195) . chr(172) => 'i', chr(195) . chr(173) => 'i',
      chr(195) . chr(174) => 'i', chr(195) . chr(175) => 'i',
      chr(195) . chr(177) => 'n', chr(195) . chr(178) => 'o',
      chr(195) . chr(179) => 'o', chr(195) . chr(180) => 'o',
      chr(195) . chr(181) => 'o', chr(195) . chr(182) => 'o',
      chr(195) . chr(182) => 'o', chr(195) . chr(185) => 'u',
      chr(195) . chr(186) => 'u', chr(195) . chr(187) => 'u',
      chr(195) . chr(188) => 'u', chr(195) . chr(189) => 'y',
      chr(195) . chr(191) => 'y',
      // Decompositions for Latin Extended-A
      chr(196) . chr(128) => 'A', chr(196) . chr(129) => 'a',
      chr(196) . chr(130) => 'A', chr(196) . chr(131) => 'a',
      chr(196) . chr(132) => 'A', chr(196) . chr(133) => 'a',
      chr(196) . chr(134) => 'C', chr(196) . chr(135) => 'c',
      chr(196) . chr(136) => 'C', chr(196) . chr(137) => 'c',
      chr(196) . chr(138) => 'C', chr(196) . chr(139) => 'c',
      chr(196) . chr(140) => 'C', chr(196) . chr(141) => 'c',
      chr(196) . chr(142) => 'D', chr(196) . chr(143) => 'd',
      chr(196) . chr(144) => 'D', chr(196) . chr(145) => 'd',
      chr(196) . chr(146) => 'E', chr(196) . chr(147) => 'e',
      chr(196) . chr(148) => 'E', chr(196) . chr(149) => 'e',
      chr(196) . chr(150) => 'E', chr(196) . chr(151) => 'e',
      chr(196) . chr(152) => 'E', chr(196) . chr(153) => 'e',
      chr(196) . chr(154) => 'E', chr(196) . chr(155) => 'e',
      chr(196) . chr(156) => 'G', chr(196) . chr(157) => 'g',
      chr(196) . chr(158) => 'G', chr(196) . chr(159) => 'g',
      chr(196) . chr(160) => 'G', chr(196) . chr(161) => 'g',
      chr(196) . chr(162) => 'G', chr(196) . chr(163) => 'g',
      chr(196) . chr(164) => 'H', chr(196) . chr(165) => 'h',
      chr(196) . chr(166) => 'H', chr(196) . chr(167) => 'h',
      chr(196) . chr(168) => 'I', chr(196) . chr(169) => 'i',
      chr(196) . chr(170) => 'I', chr(196) . chr(171) => 'i',
      chr(196) . chr(172) => 'I', chr(196) . chr(173) => 'i',
      chr(196) . chr(174) => 'I', chr(196) . chr(175) => 'i',
      chr(196) . chr(176) => 'I', chr(196) . chr(177) => 'i',
      chr(196) . chr(178) => 'IJ', chr(196) . chr(179) => 'ij',
      chr(196) . chr(180) => 'J', chr(196) . chr(181) => 'j',
      chr(196) . chr(182) => 'K', chr(196) . chr(183) => 'k',
      chr(196) . chr(184) => 'k', chr(196) . chr(185) => 'L',
      chr(196) . chr(186) => 'l', chr(196) . chr(187) => 'L',
      chr(196) . chr(188) => 'l', chr(196) . chr(189) => 'L',
      chr(196) . chr(190) => 'l', chr(196) . chr(191) => 'L',
      chr(197) . chr(128) => 'l', chr(197) . chr(129) => 'L',
      chr(197) . chr(130) => 'l', chr(197) . chr(131) => 'N',
      chr(197) . chr(132) => 'n', chr(197) . chr(133) => 'N',
      chr(197) . chr(134) => 'n', chr(197) . chr(135) => 'N',
      chr(197) . chr(136) => 'n', chr(197) . chr(137) => 'N',
      chr(197) . chr(138) => 'n', chr(197) . chr(139) => 'N',
      chr(197) . chr(140) => 'O', chr(197) . chr(141) => 'o',
      chr(197) . chr(142) => 'O', chr(197) . chr(143) => 'o',
      chr(197) . chr(144) => 'O', chr(197) . chr(145) => 'o',
      chr(197) . chr(146) => 'OE', chr(197) . chr(147) => 'oe',
      chr(197) . chr(148) => 'R', chr(197) . chr(149) => 'r',
      chr(197) . chr(150) => 'R', chr(197) . chr(151) => 'r',
      chr(197) . chr(152) => 'R', chr(197) . chr(153) => 'r',
      chr(197) . chr(154) => 'S', chr(197) . chr(155) => 's',
      chr(197) . chr(156) => 'S', chr(197) . chr(157) => 's',
      chr(197) . chr(158) => 'S', chr(197) . chr(159) => 's',
      chr(197) . chr(160) => 'S', chr(197) . chr(161) => 's',
      chr(197) . chr(162) => 'T', chr(197) . chr(163) => 't',
      chr(197) . chr(164) => 'T', chr(197) . chr(165) => 't',
      chr(197) . chr(166) => 'T', chr(197) . chr(167) => 't',
      chr(197) . chr(168) => 'U', chr(197) . chr(169) => 'u',
      chr(197) . chr(170) => 'U', chr(197) . chr(171) => 'u',
      chr(197) . chr(172) => 'U', chr(197) . chr(173) => 'u',
      chr(197) . chr(174) => 'U', chr(197) . chr(175) => 'u',
      chr(197) . chr(176) => 'U', chr(197) . chr(177) => 'u',
      chr(197) . chr(178) => 'U', chr(197) . chr(179) => 'u',
      chr(197) . chr(180) => 'W', chr(197) . chr(181) => 'w',
      chr(197) . chr(182) => 'Y', chr(197) . chr(183) => 'y',
      chr(197) . chr(184) => 'Y', chr(197) . chr(185) => 'Z',
      chr(197) . chr(186) => 'z', chr(197) . chr(187) => 'Z',
      chr(197) . chr(188) => 'z', chr(197) . chr(189) => 'Z',
      chr(197) . chr(190) => 'z', chr(197) . chr(191) => 's',
      // Euro Sign
      chr(226) . chr(130) . chr(172) => 'E'
    );
  }

  // transliterate
  return strtr($string, $charmap);
}

/**
 * Loads the options for the themes select form element.
 */
function _subsites_theme_options($add_default = TRUE) {
  $options = array();

  if ($add_default) {
    $options[-1] = '<'. t('System default') .'>';
  }

  foreach (list_themes() as $key => $theme) {
    $options[$theme->name] = t('@name (@status)', array('@name' => $theme->name, '@status' => ($theme->status ? t('Enabled') : t('Disabled'))));
  }

  return $options;
}

function subsites_get_css_path($sid) {
  return file_create_path(file_directory_path() .'/subsites/'. $sid .'/subsites-'. $sid .'.css');;
}