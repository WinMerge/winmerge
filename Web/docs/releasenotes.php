<?php
  include('../page.inc');
  
  $page = new Page;
  $page->printHead('WinMerge: Release Notes', TAB_DOCS);
  $releasenotes = $page->getContentFromHtmlFile('ReleaseNotes.html', '#href="ChangeLog.txt"#si', 'href="changelog.php"');
  if ($releasenotes == '') {
    print("<h2>Release Notes</h2>\n");
    print("<p>The release notes are currently not available...</p>\n");
  }
  else
    print($releasenotes);
  $page->printFoot();
?>