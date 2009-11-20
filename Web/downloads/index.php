<?php
  include('../engine/engine.inc');
  include('../engine/simplepie/simplepie.inc');

  $page = new Page;
  $stablerelease = $page->getStableRelease();
  $page->setDescription(__('Download the current WinMerge version %1$s, which was released at %2$s. For detailed info on what is new, read the change log and the release notes.', $stablerelease->getVersionNumber(), $stablerelease->getDate()));
  $page->setKeywords(__('WinMerge, free, download, Windows, setup, installer, binaries, runtimes, stable, beta, experimental, portable'));
  $page->addRssFeed('http://sourceforge.net/api/file/index/project-id/13216/rss', __('Project File Releases'));
  $page->printHead(__('Download WinMerge'), TAB_DOWNLOADS, 'toggle(\'checksumslist\');');
  
  $page->printHeading(__('Download WinMerge'));
  $page->printSubHeading(__('WinMerge %s', $stablerelease->getVersionNumber()));
  $page->printDownloadNow();
  $page->printPara(__('The current WinMerge version is <strong>%1$s</strong> and was released at <strong>%2$s</strong>.', $stablerelease->getVersionNumber(), $stablerelease->getDate()),
                   __('For detailed info on what is new, read the <a href="%1$s">change log</a> and the <a href="%2$s">release notes</a>.', '/docs/changelog.php', '/docs/releasenotes.php'));
?>
<div class="downloadmatrix">
<ul>
  <li><strong><?php __e('Installer');?></strong>
    <ul>
      <li><a href="<?php echo $stablerelease->getDownload('setup.exe');?>"><?php __e('Exe-Format (%s MB)', $stablerelease->getDownloadSizeMb('setup.exe'));?></a></li>
    </ul>
  </li>
  <li><strong><?php __e('Binaries');?></strong>
    <ul>
      <li><a href="<?php echo $stablerelease->getDownload('exe.zip');?>"><?php __e('Zip-Format (%s MB)', $stablerelease->getDownloadSizeMb('exe.zip'));?></a></li>
      <li><a href="<?php echo $stablerelease->getDownload('exe.7z');?>"><?php __e('7z-Format (%s MB)', $stablerelease->getDownloadSizeMb('exe.7z'));?></a></li>
    </ul>
  </li>
  <li><strong><?php __e('Runtimes');?></strong>
    <ul>
      <li><a href="<?php echo $stablerelease->getDownload('rt.zip');?>"><?php __e('Zip-Format (%s MB)', $stablerelease->getDownloadSizeMb('rt.zip'));?></a></li>
      <li><a href="<?php echo $stablerelease->getDownload('rt.7z');?>"><?php __e('7z-Format (%s MB)', $stablerelease->getDownloadSizeMb('rt.7z'));?></a></li>
    </ul>
  </li>
</ul>
</div> <!-- .downloadmatrix -->
<div id="checksums">
  <h4><a href="javascript:toggle('checksumslist')"><?php __e('SHA-1 Checksums');?></a></h4>
<pre id="checksumslist">
<?php
  echo $stablerelease->getDownloadSha1Sum('setup.exe') . ' ' . $stablerelease->getDownloadFileName('setup.exe') . "\n";
  echo $stablerelease->getDownloadSha1Sum('exe.zip') . ' ' . $stablerelease->getDownloadFileName('exe.zip') . "\n";
  echo $stablerelease->getDownloadSha1Sum('exe.7z') . ' ' . $stablerelease->getDownloadFileName('exe.7z') . "\n";
  echo $stablerelease->getDownloadSha1Sum('rt.zip') . ' ' . $stablerelease->getDownloadFileName('rt.zip') . "\n";
  echo $stablerelease->getDownloadSha1Sum('rt.7z') . ' ' . $stablerelease->getDownloadFileName('rt.7z') . "\n";
?>
</pre>
</div> <!-- #checksums -->
<?php
  $page->printPara(__('The easiest way to install WinMerge is to download and run the Installer. Read the <a href="%s">online manual</a> for help using it.', '/docs/manual/Installing.html'));
  $page->printPara(__('You can also download additional <a href="%1$s">plugins</a> and the whole <a href="%2$s">source code</a> from WinMerge.', 'plugins.php', 'source-code.php'));

  $page->printSubSubHeading(__('Requirements'));
?>
<ul>
  <li><?php __e('Microsoft Windows 98/ME/2000/XP/2003/Vista/2008');?></li>
  <li><?php __e('Microsoft Visual C++ 2003 Runtime Components (included in the installer)');?></li>
  <li><?php __e('Admin rights for the installer');?></li>
</ul>
<?php
  $page->printSubHeading(__('Other Versions'));
?>
<ul>
  <li><a href="http://sourceforge.net/project/showfiles.php?group_id=13216&amp;package_id=11248"><?php __e('Stable Versions');?></a></li>
  <li><a href="http://sourceforge.net/project/showfiles.php?group_id=13216&amp;package_id=29158"><?php __e('Beta Versions');?></a></li>
  <li><a href="http://sourceforge.net/project/showfiles.php?group_id=13216&amp;package_id=92246"><?php __e('Experimental Builds');?></a></li>
  <li><a href="http://portableapps.com/apps/utilities/winmerge_portable"><?php __e('WinMerge Portable');?></a> <?php __e('(by PortableApps.com)');?></li>
  <li><a href="http://www.geocities.co.jp/SiliconValley-SanJose/8165/winmerge.html"><?php __e('Japanese WinMerge Version');?></a> (by Takashi Sawanaka)</li>
</ul>
<?php
  $page->printRssSubHeading(__('Project File Releases'), 'http://sourceforge.net/api/file/index/project-id/13216/rss');
  $feed = new SimplePie();
  $feed->set_feed_url('http://sourceforge.net/api/file/index/project-id/13216/rss');
  $feed->set_cache_location('../engine/simplepie/cache');
  $feed->init();
  print("<ul class=\"rssfeeditems\">\n");
  foreach ($feed->get_items(0, 10) as $item) { //for the last 10 file releases...
    $title = $item->get_title();
    $title = preg_replace('#(\([A-Z][a-z][a-z],.*GMT\))#si', '', $title);
    $title = str_replace('1. Stable versions', 'Stable version', $title);
    $title = str_replace('2. Documentation', 'Documentation', $title);
    $title = str_replace('3. 7-Zip plugin', '7-Zip plugin', $title);
    $title = str_replace('4. Beta versions', 'Beta version', $title);
    $title = str_replace('5. Experimental builds', 'Experimental build', $title);
    $title = str_replace('6. Developer tools', 'Developer tool', $title);
    print("  <li><a href=\"".$item->get_link()."\">".$title."</a> <em>".$item->get_date(__('Y-m-d'))."</em></li>\n");
  }
  print("  <li><a href=\"http://sourceforge.net/project/showfiles.php?group_id=13216\">" . __('View all file releases&hellip;') . "</a></li>\n");
  print("</ul>\n");

  $page->printFoot();
?>