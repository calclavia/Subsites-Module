<?php

class views_handler_filter_node_current_subsite extends views_handler_filter {
  function admin_summary() {}

  function can_expose() {
    return FALSE;
  }

  function query() {
    $sid = subsites_get_current();

    $query = $this->query;
    $this->ensure_my_table();
    if ($sid) {
      $query->add_where(0, "{$this->table_alias}.sid=%d", $sid);
    }
    else {
      $query->add_where(0, "{$this->table_alias}.sid IS NULL");
    }
  }
}