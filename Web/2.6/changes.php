<?php
  include('../page.inc');

  $page = new Page;
  $page->printHead('WinMerge 2.6: Change History', WINMERGE_2_6);
?>
<h2>Change History</h2>
<?php
  print($page->convertChanges2Html('changes.txt'));
?>
<p>For older changes look at the <a href="/2.4/changes.php">Change History</a> from <a href="/2.4/index.php">WinMerge 2.4</a>.</p>
<?php
  $page->printFoot();
?>