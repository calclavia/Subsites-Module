<?php

/**
 * Filter handler for node subsites
 */
class views_handler_filter_node_subsites extends views_handler_filter_many_to_one {
  function get_value_options() {
    $subsites = subsites_get_all();
    foreach ($subsites as $subsite) {
      $this->value_options[$subsite->sid] = $subsite->name;
    }
  }
}
