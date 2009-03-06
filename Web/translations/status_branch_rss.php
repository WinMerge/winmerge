<?php
  header('Content-type: application/rss+xml');
  
  include('../page.inc');
  include('translations.inc');
  
  $page = new Page;
  $status = $page->convertXml2Array('status_branch.xml');
  printTranslationsStatusRSS($status, 'Branch', 'http://winmerge.svn.sourceforge.net/viewvc/winmerge/branches/R2_12/Src/Languages/');
?>