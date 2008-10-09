<?php
  include('../../page.inc');

  $page = new Page;
  $page->printHead('WinMerge: Older Documentation', TAB_DOCS);
?>
<h2>Older Documentation</h2>
<ul>
  <li><a href="usersguide-2.0/">WinMerge 2.0 - User's Guide</a></li>
  <li><a href="usersguide-1.7/">WinMerge 1.7 - User's Guide</a></li>
</ul>
<?php
  $page->printFoot();
?>