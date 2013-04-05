<?php

class views_handler_filter_node_current_subsite extends views_handler_filter {
  function admin_summary() { }
  function operator_form(&$form, &$form_state) { }
  function can_expose() { return FALSE; }

  function query() {
    $sid = subsites_get_current();
    $table = $this->ensure_my_table();

    if ($sid) {
      $this->query->add_where_expression($this->options['group'], "$table.sid = :sid", array(':sid' => $sid));
    }
    else {
      $this->query->add_where_expression($this->options['group'], "$table.sid IS NULL");
    }
  }
}
