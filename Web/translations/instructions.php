<?php
  include('../page.inc');

  $page = new Page;
  $page->printHead('Translations instructions', TAB_TRANSLATIONS);
  $instructions = $page->getContentFromHtmlFile('Translations.html', '#/winmerge/trunk/#si', '/winmerge/branches/R2_10/');
  if ($instructions == '') {
    print("<h2>Translations instructions</h2>\n");
    print("<p>The translations instructions are currently not available...</p>\n");
  }
  else
    print($instructions);
?>
<hr>
<p><em>Source: <a href="http://winmerge.svn.sourceforge.net/viewvc/*checkout*/winmerge/branches/R2_10/Docs/Developers/Translations.html">WinMerge\Docs\Developers\Translations.html</a></em></p>
<?php
  $page->printFoot();
?>