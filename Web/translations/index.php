<?php
  include('../page.inc');

  $page = new Page;
  $page->addRssFeed('status_branch_rss.php', 'Translations Status (Branch)');
  $page->addRssFeed('status_trunk_rss.php', 'Translations Status (Trunk)');
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
<p>If you would like to update any of these translations or add another translation, then please follow <a href="instructions.php">these instructions</a>.<br />
You can find a more technical view about the <a href="http://winmerge.org/Wiki/Translation_System">translation system</a> in our <a href="http://winmerge.org/Wiki/">Developer Wiki</a>.</p>
<h3><a name="status">Status</a></h3>
<p>The following two pages inform you about the translations status of the stable and developer version of WinMerge:</p>
<ul>
  <li><a href="status_branch.php">Stable Version (Branch)</a> <?php $page->printRssFeedLink('status_branch_rss.php'); ?></li>
  <li><a href="status_trunk.php">Developer Version (Trunk)</a> <?php $page->printRssFeedLink('status_trunk_rss.php'); ?></li>
</ul>
<?php
  $page->printFoot();
?>