<?php
  include('page.inc');
  include('engine/simplepie/simplepie.inc');

  $page = new Page;
  $page->setDescription('WinMerge is an Open Source differencing and merging tool for Windows. WinMerge can compare both folders and files, presenting differences in a visual text format that is easy to understand and handle.');
  $page->setKeywords('WinMerge, free, open source, Windows, windiff, diff, merge, compare, tool, utility, text, file, folder, directory, compare files, compare folders, merge files, merge folders, diff tool, merge tool, compare tool');
  $page->addRssFeed('http://sourceforge.net/export/rss2_projnews.php?group_id=13216', 'Project News');
  $page->printHead('WinMerge', TAB_HOME);
  $stablerelease = $page->getStableRelease();
?>
<div class="whatisbox">
  <h2>What is WinMerge?</h2>
  <p class="whatisimage">
    <img class="thumbnail" src="about/screenshots/screenshot_small.png" alt="File Comparison" height="150" border="0"><br />
    <a href="about/screenshots/">More Screenshots...</a>
  </p>
  <div class="whatistext">
    <p>WinMerge is an <a href="about/license.php">Open Source</a> differencing and merging tool for Windows. WinMerge can compare both folders and files, presenting differences in a visual text format that is easy to understand and handle.</p>
    <p><strong><a href="about/">Learn More</a> or <a href="downloads/">Download Now!</a></strong></p>
  </div>
  <br class="whatisclear" />
</div>

<h3>WinMerge <?php echo $stablerelease->getVersionNumber();?> - latest stable version</h3>
<p><a href="downloads/">WinMerge <?php echo $stablerelease->getVersionNumber();?></a> is the latest stable version, and is recommended for most users.</p>
<?php $page->printDownloadNow(); ?>

<?php
  $page->printRssSubHeading('Project News', 'http://sourceforge.net/export/rss2_projnews.php?group_id=13216');
  $feed = new SimplePie();
  $feed->set_feed_url('http://sourceforge.net/export/rss2_projnews.php?group_id=13216');
  $feed->set_cache_location('./engine/simplepie/cache');
  $feed->init();
  print("<ul class=\"rssfeeditems\">\n");
  foreach ($feed->get_items(0, 5) as $item) { //for the last 5 news items...
    print("  <li><a href=\"".$item->get_link()."\">".$item->get_title()."</a> <em>".$item->get_date('Y-m-d')."</em></li>\n");
  }
  print("  <li><a href=\"http://sourceforge.net/news/?group_id=13216\">View all news &hellip;</a></li>\n");
  print("</ul>\n");
?>

<h3>Support</h3>
<p>If you need support, look at our <a href="support/">support page</a> for more information how you can get it.</p>

<h3>Developers</h3>
<p>WinMerge is an open source project, which means that the program is maintained and developed by
volunteers. If you''re interested in contributing work to the project, see our <a href="http://winmerge.org/Wiki/Help_Needed">developer page</a>
for further information on how you may contribute.</p>
<p>In addition, WinMerge is translated into a number of different languages. See our <a href="translations/">information on translating WinMerge</a> into your own language.</p>
<p><a href="http://winmerge.org/Wiki/Developers">The WinMerge Development Team</a></p>
<?php
  $page->printFoot();
?>
