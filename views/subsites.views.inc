<?php

function subsites_views_data() {
  $data = array();

  $data['subsites']['table']['group']  = t('Subsite');

  $data['subsites']['table']['base'] = array(
    'field' => 'sid',
    'title' => t('Subsites'),
    'help' => t('Subsites are a part of your website that can have its own menu, theme, custom CSS or anything else you want.'),
    'weight' => 50,
  );

  $data['subsites']['sid'] = array(
    'title' => t('Sid'),
    'help' => t('The subsite ID of the subsite.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
  );

  $data['subsites']['name'] = array(
    'title' => t('Name'),
    'help' => t('Name of the subsite'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  $data['subsites']['weight'] = array(
    'title' => t('Weight'),
    'help' => t('The subsite weight field'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  $data['subsites']['status'] = array(
    'title' => t('Enabled'),
    'help' => t('Wether or not the subsite is enabled'),
    'field' => array(
      'handler' => 'views_handler_field_boolean',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_boolean_operator',
      'label' => t('Enabled'),
      'type' => 'yes-no',
    ),
  );

  $data['subsites_node']['table']['group']  = t('Subsite');

  $data['subsites_node']['table']['join'] = array(
    'node' => array(
      'left_field' => 'nid',
      'field' => 'nid',
    ),
  );

  $data['subsites_node']['sid'] = array(
    'title' => t('Subsite'),
    'help' => 'The subsite a node belongs to',
    'filter' => array(
      'handler' => 'views_handler_filter_node_subsites',
      'numeric' => TRUE,
    ),
    'field' => array(
      'handler' => 'views_handler_field_node_subsites',
    ),
  );

  $data['subsites_node']['current_subsite'] = array(
    'title' => t('Current subsite'),
    'help' => t('Get only nodes at the current subsite'),
    'filter' => array(
      'handler' => 'views_handler_filter_node_current_subsite',
    ),
  );

  return $data;
}

function subsites_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'subsites') . '/views/handlers',
    ),
    'handlers' => array(
      'views_handler_filter_node_subsites' => array(
        'parent' => 'views_handler_filter_many_to_one',
      ),
      'views_handler_field_node_subsites' => array(
        'parent' => 'views_handler_field_prerender_list',
      ),
      'views_handler_filter_node_current_subsite' => array(
        'parent' => 'views_handler_filter',
      ),
    ),
  );
}