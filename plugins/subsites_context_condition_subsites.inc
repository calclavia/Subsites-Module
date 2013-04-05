<?php

/**
 * Exposes subsites as a context condition.
 */
class subsites_context_condition_subsites extends context_condition {

  function condition_values() {
    $options = array(-1 => t('- Main site -'));
    $subsites = subsites_get_all();
    foreach ($subsites as $subsite) {
      $options[$subsite->sid] = $subsite->name;
    }
    return $options;
  }

  /**
   * Override of condition_form.
   **/
  function condition_form($context) {
    $defaults = $this->fetch_from_context($context, 'values');

    $form = array();
    $form['all'] = array(
      '#type' => 'select',
      '#title' => $this->title,
      '#default_value' => isset($defaults['all']) ? $defaults['all'] : '',
      '#options' => array('' => t('Apply to the selected subsites below'), 'all' => t('Apply to all subsites, except the "- Main site -"')),
      '#attached' => array('js' => array(drupal_get_path('module', 'subsites') . '/plugins/subsites_context_condition_subsites.js')),
    );
    $form['subsites'] = array(
      '#title' => t('Match on these subsites'),
      '#options' => $this->condition_values(),
      '#type' => 'checkboxes',
      '#default_value' => array_diff_key($defaults, array('all' => '')),
      '#prefix' => '<div id="edit-conditions-plugins-subsites-values-values">',
      '#suffix' => '</div>'
    );
    return $form;
  }

  /**
   * Override of condition_form_submit.
   **/
  function condition_form_submit($values) {
    // Editor forms are generally checkboxes -- do some checkbox processing.
    if (isset($values['subsites'])) {
      ksort($values['subsites']);
      $values['subsites'] = drupal_map_assoc(array_keys(array_filter($values['subsites'])));
    }
    return array_merge($values['subsites'], array('all' => $values['all']));
  }

  function execute($subsite) {
    foreach ($this->get_contexts() as $context) {
      // Skip all contexts that do not respond to this condition
      if (!isset($context->conditions['subsites']) || !isset($context->conditions['subsites']['values'])) {
        continue;
      }

      $values = $context->conditions['subsites']['values'];

      if (isset($values['all']) && $values['all'] == 'all') {
        if ($subsite && $subsite->sid > 0) {
          $this->condition_met($context, 'all');
        }
      }
      else if (in_array($subsite->sid, $values)) {
        $this->condition_met($context, $subsite->sid);
      }
    }
  }

}
