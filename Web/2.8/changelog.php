<?php
  include('../page.inc');
  
  $page = new Page;
  $page->printHead('WinMerge 2.8: Change Log', WINMERGE_2_8);
  print("<h2>Change Log</h2>\n");
  $changelog = $page->convertChangeLog2Html('ChangeLog.txt');
  if ($changelog == '')
    print("<p>The change log is currently not available...</p>\n");
  else 
    print($changelog);
  $page->printFoot();
?>