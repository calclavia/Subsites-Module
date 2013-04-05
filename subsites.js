(function ($) {

Drupal.behaviors.subsites_visibility = function() {
  var pages = $('#edit-pages-wrapper');
  var subsites_visibility = $('#advanced-visibility');
  var visibility = $("input[name='visibility']:checked").val();

  if (visibility >= 3) {
    pages.hide();
  }
  else {
    subsites_visibility.hide();
  }

  $("input[name='visibility']").change(function() {
    var visibility = $("input[name='visibility']:checked").val();
    if (visibility >= 3) {
      pages.hide();
      subsites_visibility.show();
    }
    else {
      subsites_visibility.hide();
      pages.show();
    }
  });
};

Drupal.behaviors.vertical_tabs_subsiteFieldsetSummary = {
  attach: function (context) {
    $('fieldset#edit-subsites', context).drupalSetSummary(function (context) {
      return Drupal.checkPlain($('#edit-subsite option:selected', context).text());
    });
  }
};

})(jQuery);