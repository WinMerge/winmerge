<?php
  include('../page.inc');

  $page = new Page;
  $page->printHead('WinMerge 2.2: Change History', WINMERGE_2_2);
?>
<h2>Change History</h2>
<?php
  print($page->convertChanges2Html('changes.txt'));
?>
<p>For older changes look at the <a href="/2.0/changes.php">Change History</a> from <a href="/2.0/index.php">WinMerge 2.0</a>.</p>
<?php
  $page->printFoot();
?>