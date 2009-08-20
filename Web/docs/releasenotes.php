<?php
  include('../engine/engine.inc');
  
  $page = new Page;
  $page->setDescription(__('The release notes are a short summary of important changes, enhancements, bug fixes and known issues in the current WinMerge release.'));
  $page->setKeywords(__('WinMerge, release notes, summary, changes, enhancements, bug fixes, known issues, release'));
  $page->printHead(__('Release Notes'), TAB_DOCS);
  $releasenotes = $page->getContentFromHtmlFile('ReleaseNotes.html', '#href="ChangeLog.txt"#si', 'href="changelog.php"');
  if ($releasenotes == '') {
    $page->printHeading(__('Release Notes'));
    $page->printPara(__('The release notes are currently not available...'));
  }
  else
    print($releasenotes);
  $page->printFoot();
?>