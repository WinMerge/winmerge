<?php
  header('Content-type: application/rss+xml');
  
  include('translations.inc');
  
  try {
    $status = New TranslationsStatus('status_trunk.xml');
    $status->version = 'Unstable Trunk';
    $status->svnUrl = 'http://winmerge.svn.sourceforge.net/viewvc/winmerge/trunk/Translations/';
    $status->rssLink = 'http://winmerge.org/translations/status_trunk.php';
    $status->printRSS();
  }
  catch (Exception $ex) { //If problems with translations status...
    print("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    print("<rss version=\"2.0\">\n");
    print("</rss>\n");
  }
?>