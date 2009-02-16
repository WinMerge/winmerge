<?php
  include('../../page.inc');

  $page = new Page;
  $page->setDescription('Manuals and user\'s guides from older WinMerge versions.');
  $page->setKeywords('WinMerge, old, documentation, manual, user\'s guide');
  $page->printHead(Older Documentation', TAB_DOCS);
?>
<h2>Older Documentation</h2>
<ul>
  <li><a href="manual-2.8/">WinMerge 2.8 - Manual</a></li>
  <li><a href="manual-2.6/">WinMerge 2.6 - Manual</a></li>
  <li><a href="manual-2.4/">WinMerge 2.4 - Manual</a></li>
  <li><a href="manual-2.2/">WinMerge 2.2 - Manual</a></li>
  <li><a href="usersguide-2.0/">WinMerge 2.0 - User's Guide</a></li>
  <li><a href="usersguide-1.7/">WinMerge 1.7 - User's Guide</a></li>
</ul>
<?php
  $page->printFoot();
?>