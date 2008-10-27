<?php
  include('../page.inc');

  $page = new Page;
  $page->printHead('WinMerge 2.4: Change History', WINMERGE_2_4);
?>
<h2>Change History</h2>
<?php
  print($page->convertChanges2Html('changes.txt'));
  $page->printFoot();
?>