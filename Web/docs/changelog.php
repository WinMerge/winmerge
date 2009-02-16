<?php
  include('../page.inc');
  
  $page = new Page;
  $page->setDescription('The change log is a more complete list of changes in the last WinMerge releases.');
  $page->setKeywords('WinMerge, change log, changes, release, tracker item, revision number');
  $page->printHead('Change Log', TAB_DOCS);
  print("<h2>Change Log</h2>\n");
  $changelog = $page->convertChangeLog2Html('ChangeLog.txt');
  if ($changelog == '')
    print("<p>The change log is currently not available...</p>\n");
  else 
    print($changelog);
  $page->printFoot();
?>