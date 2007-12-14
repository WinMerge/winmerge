<?php
  include('../page.inc');

  $page = new Page;
  $page->printHead('WinMerge: Translations', TAB_TRANSLATIONS);
?>
<h2>Translations</h2>
<p>We currently have WinMerge translated into the languages listed below:</p>
<?php
  $status = $page->convertXml2Array('status_trunk.xml');
  print("<ul class=\"inline\">\n");
  $translations = $status['TRANSLATIONS'][0]['TRANSLATION'];
  $translations = $page->multisortArray($translations, 'LANGUAGE', SORT_ASC);
  for ($i = 0; $i < count($translations); $i++) { //For all translations...
    $translation = $translations[$i];
    $language_name = $translation['LANGUAGE'][0]['VALUE'];
    
    print("  <li>" . $language_name . "</li>\n");
  }
  print("</ul>\n");
?>
<p>To change languages, select the desired language from the <em>View-&gt;Languages</em> menu choice.</p>
<h3>Translating</h3>
<p>If you would like to update any of these translations or add another translation, then please follow <a href="http://winmerge.svn.sourceforge.net/viewvc/*checkout*/winmerge/trunk/Docs/Developers/Translations.html">these instructions</a>.</p>
<h3>Status</h3>
<p>The following two pages inform you about the translations status of the stable and developer version of WinMerge:</p>
<ul>
  <li><a href="http://winmerge.org/Wiki/Translations">Stable Version (Branch)</a></li>
  <!--<li><a href="status_branch.php">Stable Version (Branch)</a></li>-->
  <li><a href="status_trunk.php">Developer Version (Trunk)</a></li>
</ul>
<?php
  $page->printFoot();
?>