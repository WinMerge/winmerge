<?php
  include('../engine/engine.inc');
  include('translations_branch.inc');

  $page = new Page;
  $page->addRssFeed('status_branch_rss.php');
  $page->printHead('Translations Status (Stable Branch)', TAB_TRANSLATIONS);
  
  $status = convertXml2Array('status_branch.xml');
  $page->printRssHeading('Translations Status (Stable Branch)', 'status_branch_rss.php');
  if (!empty($status)) { //If translations status available...
    printTranslationsStatus($status, 'http://winmerge.svn.sourceforge.net/viewvc/winmerge/branches/' . $stablerelease = $page->getStableRelease()->getBranchName() . '/Src/Languages/');
    printTranslationsStatusGraphLegend();
    print("<h3>Translators</h3>\n");
    printTranslationsTranslators($status);
  }
  else { //If translations status NOT available...
    print("<p>The translations status is currently not available...</p>\n");
  }
  $page->printFoot();
?>