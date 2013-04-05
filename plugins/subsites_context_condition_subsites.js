(function ($) {

Drupal.behaviors.subsitesContextConditionSubsites = {
  attach: function (context) {
    var $all    = $('#edit-conditions-plugins-subsites-values-all'),
        $values = $('#edit-conditions-plugins-subsites-values-values');

    if ($all.val() == 'all') {
      $values.hide();
    }

    $all.change(function () {
      if ($all.val() == 'all') {
        $values.slideUp();
      }
      else {
        $values.slideDown();
      }
    });
  }
};

})(jQuery);