<?php
  include('../engine/engine.inc');
  
  $page = new Page;
  $page->setDescription('The release notes are a short summary of important changes, enhancements, bug fixes and known issues in the current WinMerge release.');
  $page->setKeywords('WinMerge, release notes, summary, changes, enhancements, bug fixes, known issues, release');
  $page->printHead('Release Notes', TAB_DOCS);
  $releasenotes = $page->getContentFromHtmlFile('ReleaseNotes.html', '#href="ChangeLog.txt"#si', 'href="changelog.php"');
  if ($releasenotes == '') {
    print("<h2>Release Notes</h2>\n");
    print("<p>The release notes are currently not available...</p>\n");
  }
  else
    print($releasenotes);
  $page->printFoot();
?>