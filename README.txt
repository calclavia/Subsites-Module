/* $Id: README.txt,v 1.2 2009/11/14 09:53:06 davyvandenbremt Exp $ */

Description
-----------

Subsites are a part of your website that can have its own menu, theme, custom CSS or anything else you want.

Subsites can be defined in a few manners:
- Marking individual nodes as belonging to the subsite.
- All pages in the subsite menu belong to the section.
- Defining paths that belong to a the section. This is done in the same way block visibility works. So you can use PHP too!

Subsites can have ...
... their own page template : page-subsites-{subsite-id} or page-subsites-{subsite-slug-name}.tpl.php
... their own CSS file
... their own theme, different from the default theme
... their own menu.

Subsites can be ...
... a condition in Context module (http://drupal.org/project/context)
... used to define block visibility

That's all? Nope! Themers and module writers can react differently for each subsite using subsites_get_current and subsites_get.

Remark! Each path/node/... can have only one subsite. If multiple subsites' conditions apply for the path, the subsite with the lowest weight is the active one.


Requirements
------------

This module requires Drupal 6.


Installation
------------

1) Copy/upload the subsites module folder to the sites/all/modules directory of your Drupal installation. 

2) Enable the Subsites module in Drupal (administer -> modules).


Configuration
-------------

- Go to Admininster > Site building > Subsites.
- Choose "Add subsite".
- Fill in details. After saving the details the subsite will be created and a companion menu (you can verify this by going to Administer > Site building > Menu).
- You can now :
  - Configure the subsite by choosing "Edit" in the subsites overview page.
  - Add nodes, views, ... to the subsite menu.
  - Mark nodes on their edit form to belong to a certain subsite.
  - Put the "Subsite menu" block in some region using the block admin on Administer > Site building > Blocks.
  - Restrict visibility for some blocks to only show up in specific subsites.
  
Developers
----------

Themers and module writers can react differently for each subsite using subsites_get_current and subsites_get.

Alternatives
------------

This module is a pretty easy one allowing webmasters to create "parts" in their site with their own theme, css and menu.

If you're a more experienced user and need more flexibility, sections (http://drupal.org/project/sections) and context (http://drupal.org/project/context) are your thing.

Author
------

Davy Van Den Bremt <info@davyvandenbremt.be>
http://www.davyvandenbremt.be
http://www.drupalcoder.com