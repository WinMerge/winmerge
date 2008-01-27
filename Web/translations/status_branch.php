<?php
  include('../page.inc');
  include('translations.inc');

  $page = new Page;
  $page->addRssFeed('status_branch_rss.php');
  $page->printHead('WinMerge: Translations Status (Branch)', TAB_TRANSLATIONS);
  
  $status = $page->convertXml2Array('status_branch.xml');
  print("<h2>Translations Status (Branch) ");
  $page->printRssFeedLink('status_branch_rss.php');
  print("</h2>\n");
  if (!empty($status)) {
    printTranslationsStatus($status, 'http://winmerge.svn.sourceforge.net/viewvc/winmerge/branch/R2_8/Src/Languages/');
    printTranslationsStatusGraphLegend();
    print("<h3>Translators</h3>\n");
    printTranslationsTranslators($status);
  }
  else {
    print("<p>The translations status is currently not available...</p>\n");
  }
  $page->printFoot();
?>