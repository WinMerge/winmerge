<?php
  include('../engine/engine.inc');
  include('translations.inc');

  $page = new Page;
  $page->addRssFeed('status_branch_rss.php', 'Translations Status (Stable Branch)');
  $page->addRssFeed('status_trunk_rss.php', 'Translations Status (Unstable Trunk)');
  $page->printHead('Translations', TAB_TRANSLATIONS);
?>
<h2>Translations</h2>
<p>We currently have WinMerge translated into the languages listed below:</p>
<ul class="inline">
<?php
  try {
    $status = New TranslationsStatus('status_trunk.xml');
    
    $languages = $status->getLanguagesArray();
    foreach ($languages as $language) { //for all languages...
      print("  <li>" . $language . "</li>\n");
    }
  }
  catch (Exception $ex) { //If problems with translations status...
    print("  <li>English</li>\n");
  }
?>
</ul>
<p>To change languages, select the desired language from the <span class="guimenu">View</span>
&#8594; <span class="guimenuitem">Languages</span> menu choice.</p>

<h3>Translating</h3>
<p>If you would like to update any of these translations or add another translation, then please follow <a href="instructions.php">these instructions</a>.<br />
You can find a more technical view about the <a href="http://winmerge.org/Wiki/Translation_System">translation system</a> in our <a href="http://winmerge.org/Wiki/">Development Wiki</a>.</p>

<h3><a name="status">Status</a></h3>
<p>The following two pages inform you about the translations status of the stable and developer version of WinMerge:</p>
<ul>
  <li><a href="status_branch.php">Stable Version (Branch)</a> <?php $page->printRssFeedLink('status_branch_rss.php'); ?></li>
  <li><a href="status_trunk.php">Unstable Version (Trunk)</a> <?php $page->printRssFeedLink('status_trunk_rss.php'); ?></li>
</ul>
<?php
  $page->printFoot();
?>