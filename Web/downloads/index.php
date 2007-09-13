<?php
  include('../page.inc');

  $page = new Page;
  $page->printHead('WinMerge: Downloads', TAB_DOWNLOADS);
  $stablerelease = $page->getStableRelease();
?>
<h2>Downloads</h2>
<?php $page->printDownloadNow(); ?>
<h3>Current Version</h3>
<p>The current version is <strong>2.6.8</strong> and was released at <strong>2007-06-19</strong>. For detailed info on what's new, read the <a href="/2.6/changes.php">changelog</a> and the <a href="http://sourceforge.net/project/shownotes.php?release_id=517309">release notes</a>.</p>
<div class="downloadmatrix">
<ul>
  <li><strong>Installer</strong>
    <ul>
      <li><a href="<?php echo $stablerelease->getSetupDownload();?>">Exe-Format (<?php echo $stablerelease->getSetupSizeFormatted('mb');?> MB)</a></li>
    </ul>
  </li>
  <li><strong>Binaries</strong>
    <ul>
      <li><a href="http://prdownloads.sourceforge.net/winmerge/WinMerge-2.6.8-exe.zip">Zip-Format (5.18 MB)</a></li>
      <li><a href="http://prdownloads.sourceforge.net/winmerge/WinMerge-2.6.8-exe.7z">7z-Format (1.67 MB)</a></li>
    </ul>
  </li>
  <li><strong>Runtimes</strong>
    <ul>
      <li><a href="http://prdownloads.sourceforge.net/winmerge/Runtimes-2.6.8.zip">Zip-Format (1.23 MB)</a></li>
      <li><a href="http://prdownloads.sourceforge.net/winmerge/Runtimes-2.6.8.7z">7z-Format (0.72 MB)</a></li>
    </ul>
  </li>
  <li><strong>Source Code</strong>
    <ul>
      <li><a href="http://prdownloads.sourceforge.net/winmerge/WinMerge-2.6.8-src.zip">Zip-Format (5.06 MB)</a></li>
      <li><a href="http://prdownloads.sourceforge.net/winmerge/WinMerge-2.6.8-src.7z">7z-Format (2.78 MB)</a></li>
    </ul>
  </li>
</ul>
</div>
<p>The easiest way to install WinMerge is to download and run the Installer. Read the <a href="2.6/manual/Installing.html">online manual</a> for help using it.</p>
<h3>Other Versions</h3>
<ul>
  <li><a href="http://sourceforge.net/project/showfiles.php?group_id=13216&amp;package_id=11248">Stable Versions</a></li>
  <li><a href="http://sourceforge.net/project/showfiles.php?group_id=13216&amp;package_id=29158">Beta Versions</a></li>
  <li><a href="http://sourceforge.net/project/showfiles.php?group_id=13216&amp;package_id=92246">Experimental Builds</a></li>
</ul>
<h3>Plugins</h3>
<h4>7-Zip Plugin</h4>
<p>This is the 7-Zip plugin for WinMerge and the current version <strong>DllBuild 0025</strong> was released at <strong>2007-09-07</strong>.</p>
<div class="downloadmatrix">
<ul>
  <li><strong>Installer</strong>
    <ul>
      <li><a href="http://prdownloads.sourceforge.net/winmerge/Merge7zInstaller0025-432-455.exe">Exe-Format (1.04 MB)</a></li>
    </ul>
  </li>
  <li><strong>Binaries</strong>
    <ul>
      <li><a href="http://prdownloads.sourceforge.net/winmerge/Merge7z0025-432-455.7z">7z-Format (0.89 MB)</a></li>
    </ul>
  </li>
</ul>
</div>
<p>You can also read the <a href="http://sourceforge.net/project/shownotes.php?release_id=537779&amp;group_id=13216">Release Notes</a> or search for <a href="http://sourceforge.net/project/showfiles.php?group_id=13216&amp;package_id=143957">other versions</a>.</p>
<?php
  $page->printFoot();
?>