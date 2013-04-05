<?php

/**
 * Field handler to provide a list of subsites.
 */
class views_handler_field_node_subsites extends views_handler_field_prerender_list {
  function construct() {
    parent::construct();
    $this->additional_fields['nid'] = array('table' => 'node', 'field' => 'nid');
  }

  function query() {
    $this->add_additional_fields();
    $this->field_alias = $this->aliases['nid'];
  }

  function pre_render($values) {
    $this->items = array();

    foreach ($values as $result) {
      $nids[] = $result->{$this->field_alias};
    }

    if ($nids) {
      $placeholders = db_placeholders($nids, 'int');
      $sql = "SELECT n.nid, s.sid, s.name FROM {subsites} s INNER JOIN {subsites_node} n ON s.sid = n.sid WHERE n.nid IN ({$placeholders}) ORDER BY s.name";
      $result = db_query($sql, $nids);
      while ($subsite = db_fetch_object($result)) {
        $this->items[$subsite->nid][$subsite->sid] = array(
          'subsite' => check_plain($subsite->name),
          'sid' => $subsite->sid,
        );
      }
    }
  }

  function render_item($count, $item) {
    return $item['subsite'];
  }

  function document_self_tokens(&$tokens) {
    $tokens['[' . $this->options['id'] . '-subsite' . ']'] = t('The name of the subsite.');
    $tokens['[' . $this->options['id'] . '-sid' . ']'] = t('The subsite ID of the subsite.');
  }

  function add_self_tokens(&$tokens, $item) {
    $tokens['[' . $this->options['id'] . '-subsite' . ']'] = $item['subsite'];
    $tokens['[' . $this->options['id'] . '-sid' . ']'] = $item['sid'];
  }
}