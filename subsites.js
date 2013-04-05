/* $Id: subsites.js,v 1.2 2010/10/05 10:16:26 davyvandenbremt Exp $ */
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