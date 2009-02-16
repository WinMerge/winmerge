<?php
  include('../page.inc');
  include('../engine/simplepie/simplepie.inc');

  $page = new Page;
  $stablerelease = $page->getStableRelease();
  $page->setDescription('Download the current WinMerge version ' . $stablerelease->getVersionNumber() . ', which was released at ' . $stablerelease->getDate() . '. For detailed info on what\'s new, read the change log and the release notes.');
  $page->setKeywords('WinMerge, free, download, Windows, setup, installer, binaries, runtimes, stable, beta, experimental, portable');
  $page->addRssFeed('http://sourceforge.net/export/rss2_projfiles.php?group_id=13216', 'Project File Releases');
  $page->printHead('Download WinMerge', TAB_DOWNLOADS, 'toggle(\'checksumslist\');');
?>
<h2>Download WinMerge</h2>
<h3>WinMerge <?php echo $stablerelease->getVersionNumber();?></h3>
<?php $page->printDownloadNow(); ?>
<p>The current WinMerge version is <strong><?php echo $stablerelease->getVersionNumber();?></strong> and was released at <strong><?php echo $stablerelease->getDate();?></strong>. For detailed info on what's new, read the <a href="/docs/changelog.php">change log</a> and the <a href="/docs/releasenotes.php">release notes</a>.</p>
<div class="downloadmatrix">
<ul>
  <li><strong>Installer</strong>
    <ul>
      <li><a href="<?php echo $stablerelease->getDownload('setup.exe');?>">Exe-Format (<?php echo $stablerelease->getDownloadSizeMb('setup.exe');?> MB)</a></li>
    </ul>
  </li>
  <li><strong>Binaries</strong>
    <ul>
      <li><a href="<?php echo $stablerelease->getDownload('exe.zip');?>">Zip-Format (<?php echo $stablerelease->getDownloadSizeMb('exe.zip');?> MB)</a></li>
      <li><a href="<?php echo $stablerelease->getDownload('exe.7z');?>">7z-Format (<?php echo $stablerelease->getDownloadSizeMb('exe.7z');?> MB)</a></li>
    </ul>
  </li>
  <li><strong>Runtimes</strong>
    <ul>
      <li><a href="<?php echo $stablerelease->getDownload('rt.zip');?>">Zip-Format (<?php echo $stablerelease->getDownloadSizeMb('rt.zip');?> MB)</a></li>
      <li><a href="<?php echo $stablerelease->getDownload('rt.7z');?>">7z-Format (<?php echo $stablerelease->getDownloadSizeMb('rt.7z');?> MB)</a></li>
    </ul>
  </li>
</ul>
</div> <!-- .downloadmatrix -->
<div id="checksums">
  <h4><a href="javascript:toggle('checksumslist')">SHA-1 Checksums</a></h4>
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
<p>The easiest way to install WinMerge is to download and run the Installer. Read the <a href="/docs/manual/Installing.html">online manual</a> for help using it.</p>
<p>You can also download additional <a href="plugins.php">plugins</a> and the whole <a href="source-code.php">source code</a> from WinMerge.</p>
<h3><a name="other">Other Versions</a></h3>
<ul>
  <li><a href="http://sourceforge.net/project/showfiles.php?group_id=13216&amp;package_id=11248">Stable Versions</a></li>
  <li><a href="http://sourceforge.net/project/showfiles.php?group_id=13216&amp;package_id=29158">Beta Versions</a></li>
  <li><a href="http://sourceforge.net/project/showfiles.php?group_id=13216&amp;package_id=92246">Experimental Builds</a></li>
  <li><a href="http://portableapps.com/apps/utilities/winmerge_portable">WinMerge Portable</a> (by PortableApps.com)</li>
  <li><a href="http://www.geocities.co.jp/SiliconValley-SanJose/8165/winmerge.html">Japanese WinMerge Version</a> (by Takashi Sawanaka)</li>
</ul>
<?php
  $page->printRssSubHeading('Project File Releases', 'http://sourceforge.net/export/rss2_projfiles.php?group_id=13216');
  $feed = new SimplePie();
  $feed->set_feed_url('http://sourceforge.net/export/rss2_projfiles.php?group_id=13216');
  $feed->set_cache_location('../engine/simplepie/cache');
  $feed->init();
  print("<ul class=\"rssfeeditems\">\n");
  foreach ($feed->get_items(0, 5) as $item) { //for the last 5 file releases...
    $title = $item->get_title();
    $title = preg_replace('#(\([A-Z][a-z][a-z],.*GMT\))#si', '', $title);
    $title = str_replace('1. Stable versions', 'Stable version', $title);
    $title = str_replace('2. Documentation', 'Documentation', $title);
    $title = str_replace('3. 7-Zip plugin', '7-Zip plugin', $title);
    $title = str_replace('4. Beta versions', 'Beta version', $title);
    $title = str_replace('5. Experimental builds', 'Experimental build', $title);
    $title = str_replace('6. Developer tools', 'Developer tool', $title);
    print("  <li><a href=\"".$item->get_link()."\">".$title."</a> <em>".$item->get_date('Y-m-d')."</em></li>\n");
  }
  print("  <li><a href=\"http://sourceforge.net/project/showfiles.php?group_id=13216\">View all file releases &hellip;</a></li>\n");
  print("</ul>\n");
?>

<?php
  $page->printFoot();
?>