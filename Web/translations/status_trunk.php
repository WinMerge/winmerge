<?php
  include('../page.inc');
  include('translations.inc');

  $page = new Page;
  $page->printHead('WinMerge: Translations Status (Trunk)', TAB_TRANSLATIONS);
  
  $status = $page->convertXml2Array('status_trunk.xml');
  print("<h2>Translations Status (Trunk)</h2>\n");
  if (!empty($status)) {
    printTranslationsStatus($status, 'http://winmerge.svn.sourceforge.net/viewvc/winmerge/trunk/Src/Languages/');
    printTranslationsStatusGraphLegend();
    print("<h3>Translators</h3>\n");
    printTranslationsTranslators($status);
  }
  else {
    print("<p>The translations status is currently not available...</p>\n");
  }
  $page->printFoot();
?>