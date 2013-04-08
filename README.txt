
### Brief
Subsites are a part of your website that can have its own menu, theme, custom
CSS or anything else you want.

Subsites can be defined in a few manners:
- Marking individual nodes as belonging to the subsite.
- All pages in the subsite menu belong to the section.
- Defining paths that belong to a the section. This is done in the same way
block visibility works. So you can use PHP too!

Subsites can have ...
... their own page template : page-subsites-{subsite-id} or
    page-subsites-{subsite-slug-name}.tpl.php
... their own CSS file
... their own theme, different from the default theme
... their own menu.

Subsites can be ...
... a condition in Context module (http://drupal.org/project/context)
... used to define block visibility

That's all? Nope! Themers and module writers can react differently for each
subsite using subsites_get_current and subsites_get.

Remark! Each path/node/... can have only one subsite. If multiple subsites'
conditions apply for the path, the subsite with the lowest weight is the
active one.

### Requirements
This module requires Drupal 7.

### License
This software is distributed under the Lesser General Public License (LGPL) version 3. The license can be found here: 
http://www.gnu.org/copyleft/lesser.html

### Author
Davy Van Den Bremt <info@davyvandenbremt.be>
http://www.davyvandenbremt.be
http://www.drupalcoder.com

### Credits
Drupal 7 upgrade by James Andres (http://drupal.org/user/33827)