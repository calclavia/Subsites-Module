<?php
/**
 * Argument handler to accept a Subsite SID.
 */
class views_handler_argument_node_subsites extends views_handler_argument_numeric {

  /**
   * Override the default actions to add an additional "subsite mainsite"
   * action.
   */
  function default_actions($which = NULL) {
    $defaults = parent::default_actions();
    $defaults['subsite mainsite'] = array(
      'title' => t('Display only content from the "Main site"'),
      'method' => 'default_only_mainsite',
      'breadcrumb' => TRUE, // generate a breadcrumb to here
    );
    if ($which) {
      if (!empty($defaults[$which])) {
        return $defaults[$which];
      }
    }
    else {
      return $defaults;
    }
  }

  /**
   * Handle the "subsite mainsite" default argument action.
   **/
  function default_only_mainsite() {
    $base_table = $this->query->base_table;
    // Dependent subquery isn't amazing for performance, but
    // shouldn't hurt in most cases
    $this->query->add_where_expression(0, "$base_table.nid NOT IN (SELECT nid FROM $this->table)");
    return TRUE;
  }

  function title() {
    $subsite = subsites_load($this->argument);
    if ($subsite->name) {
      return $subsite->name;
    }
    else {
      return t('Subsite');
    }
  }

}
