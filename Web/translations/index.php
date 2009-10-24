<?php
  include('../engine/engine.inc');
  include('translations.inc');

  $page = new Page;
  $page->addRssFeed('status_branch_rss.php', __('Translations Status (Stable Branch)'));
  $page->addRssFeed('status_trunk_rss.php', __('Translations Status (Unstable Trunk)'));
  $page->printHead(__('Translations'), TAB_TRANSLATIONS);
  
  $page->printHeading(__('Translations'));
  $page->printPara(__('We currently have WinMerge translated into the languages listed below:'));
?>
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
    print("  <li>" . __('English') . "</li>\n");
  }
?>
</ul>
<?php
  $page->printPara(__('To change language, select the desired language from the <span class="guimenu">View</span>&#8594; <span class="guimenuitem">Language</span> menu choice.'));
  
  $page->printSubHeading(__('Translating'));
  $page->printPara(__('If you would like to update any of these translations or add another translation, then please follow <a href="%s">these instructions</a>.', 'instructions.php'),
                   __('You can find a more technical view about the <a href="%1$s">translation system</a> in our <a href="%2$s">Development Wiki</a>.', 'http://winmerge.org/Wiki/Translation_System', 'http://winmerge.org/Wiki/'));
?>

<h3><a name="status"><?php __e('Status');?></a></h3>
<p><?php __e('The following two pages inform you about the translations status of the stable and developer version of WinMerge:');?></p>
<ul>
  <li><a href="status_branch.php"><?php __e('Stable Version (Branch)');?></a> <?php $page->printRssFeedLink('status_branch_rss.php'); ?></li>
  <li><a href="status_trunk.php"><?php __e('Unstable Version (Trunk)');?></a> <?php $page->printRssFeedLink('status_trunk_rss.php'); ?></li>
</ul>
<?php
  $page->printFoot();
?>