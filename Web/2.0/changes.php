<?php
  include('../page.inc');

  $page = new Page;
  $page->printHead('WinMerge 2.0: Change History', WINMERGE_2_0);
?>
<h2>Change History</h2>
<?php
  print($page->convertChanges2Html('changes.txt'));
?>
<p>For older changes look at the <a href="/1.7/changes.php">Change History</a> from <a href="/1.7/index.php">WinMerge 1.7</a>.</p>
<?php
  $page->printFoot();
?>