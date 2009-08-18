<?php
  include('engine/engine.inc');
  include('engine/simplepie/simplepie.inc');

  $page = new Page;
  $page->setDescription(__('WinMerge is an Open Source differencing and merging tool for Windows. WinMerge can compare both folders and files, presenting differences in a visual text format that is easy to understand and handle.'));
  $page->setKeywords(__('WinMerge, free, open source, Windows, windiff, diff, merge, compare, tool, utility, text, file, folder, directory, compare files, compare folders, merge files, merge folders, diff tool, merge tool, compare tool'));
  $page->addRssFeed('http://sourceforge.net/export/rss2_projnews.php?group_id=13216', __('Project News'));
  $page->printHead('', TAB_HOME);
  $stablerelease = $page->getStableRelease();
?>
<div class="whatisbox">
  <h2><?php __e('What is WinMerge?');?></h2>
  <p class="whatisimage">
    <img class="thumbnail" src="about/screenshots/screenshot_small.png" alt="<?php __e('File Comparison');?>" height="150" border="0"><br />
    <a href="about/screenshots/"><?php __e('More Screenshots&hellip;');?></a>
  </p>
  <div class="whatistext">
    <p><?php __e('WinMerge is an <a href="%s">Open Source</a> differencing and merging tool for Windows. WinMerge can compare both folders and files, presenting differences in a visual text format that is easy to understand and handle.', 'about/license.php');?></p>
    <p><strong><?php __e('<a href="%1$s">Learn More</a> or <a href="%2$s">Download Now!</a>', 'about/', 'downloads/');?></strong></p>
  </div>
  <br class="whatisclear" />
</div>

<?php
  $page->printSubHeading(__('WinMerge %s - latest stable version', $stablerelease->getVersionNumber()));
  $page->printPara(__('<a href="%1$s">WinMerge %2$s</a> is the latest stable version, and is recommended for most users.', 'downloads/', $stablerelease->getVersionNumber()));
  $page->printDownloadNow();
  
  $page->printRssSubHeading(__('Project News'), 'http://sourceforge.net/export/rss2_projnews.php?group_id=13216');
  $feed = new SimplePie();
  $feed->set_feed_url('http://sourceforge.net/export/rss2_projnews.php?group_id=13216');
  $feed->set_cache_location('./engine/simplepie/cache');
  $feed->init();
  print("<ul class=\"rssfeeditems\">\n");
  foreach ($feed->get_items(0, 5) as $item) { //for the last 5 news items...
    print("  <li><a href=\"".$item->get_link()."\">".$item->get_title()."</a> <em>".$item->get_date(__('Y-m-d'))."</em></li>\n");
  }
  print("  <li><a href=\"http://sourceforge.net/news/?group_id=13216\">" . __('View all news&hellip;') . "</a></li>\n");
  print("</ul>\n");

  $page->printSubHeading(__('Support'));
  $page->printPara(__('If you need support, look at our <a href="%s">support page</a> for more information how you can get it.', 'support/'));

  $page->printSubHeading(__('Developers'));
  $page->printPara(__('WinMerge is an open source project, which means that the program is maintained and developed by volunteers.'),
                   __('If you are interested in contributing work to the project, see our <a href="%s">developer page</a> for further information on how you may contribute.', 'http://winmerge.org/Wiki/Help_Needed'));
  $page->printPara(__('In addition, WinMerge is translated into a number of different languages. See our <a href="%s">information on translating WinMerge</a> into your own language.', 'translations/'));
  $page->printPara('<a href="http://winmerge.org/Wiki/Developers">' . __('The WinMerge Development Team') . '</a>');

  $page->printFoot();
?>
