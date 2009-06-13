<?php
  include('../page.inc');
  include('translations.inc');

  $page = new Page;
  $page->addRssFeed('status_trunk_rss.php');
  $page->printHead('Translations Status (Unstable Trunk)', TAB_TRANSLATIONS);
  
  $status = $page->convertXml2Array('status_trunk.xml');
  $page->printRssHeading('Translations Status (Unstable Trunk)', 'status_trunk_rss.php');
  if (!empty($status)) { //If translations status available...
    printTranslationsTOC($status);
    printTranslationsStatus($status, 'http://winmerge.svn.sourceforge.net/viewvc/winmerge/trunk/Translations/');
    printTranslationsTranslators($status);
  }
  else { //If translations status NOT available...
    print("<p>The translations status is currently not available...</p>\n");
  }
  $page->printFoot();
?>