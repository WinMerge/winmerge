<?php
  header('Content-type: application/rss+xml');
  
  include('../page.inc');
  include('translations.inc');
  
  $page = new Page;
  $status = $page->convertXml2Array('status_trunk.xml');
  printTranslationsStatusRSS($status, 'Trunk', 'http://winmerge.svn.sourceforge.net/viewvc/winmerge/trunk/Src/Languages/');
?>