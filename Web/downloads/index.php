<?php
  include('../page.inc');

  $page = new Page;
  $page->printHead('WinMerge: Downloads', TAB_DOWNLOADS);
?>
<h2>Downloads</h2>
<?php $page->printDownloadNow(); ?>
<h3><a href="http://sourceforge.net/project/showfiles.php?group_id=13216&amp;package_id=11248&amp;release_id=517309">WinMerge 2.6.8</a> (<a href="http://sourceforge.net/project/shownotes.php?release_id=517309">Release Notes</a>)</h3>
<dl>
  <dt><a href="http://prdownloads.sourceforge.net/winmerge/WinMerge-2.6.8-Setup.exe">Installer</a></dt>
  <dd><p>The easiest way to install WinMerge is to download and run this installer. Read the <a href="2.6/manual/Installing.html">online manual</a> for help using it.</p></dd>
  <dt><a href="http://prdownloads.sourceforge.net/winmerge/WinMerge-2.6.8-exe.zip">Binaries</a></dt>
  <dd><p>WinMerge binaries are also available in archive files (<a href="http://prdownloads.sourceforge.net/winmerge/WinMerge-2.6.8-exe.zip">.zip</a> and <a href="http://prdownloads.sourceforge.net/winmerge/WinMerge-2.6.8-exe.7z">.7z</a>).</p></dd>
  <dt><a href="http://prdownloads.sourceforge.net/winmerge/WinMerge-2.6.8-src.zip">Source code</a></dt>
  <dd><p>Source code is available in archive files (<a href="http://prdownloads.sourceforge.net/winmerge/WinMerge-2.6.8-src.zip">.zip</a> and <a href="http://prdownloads.sourceforge.net/winmerge/WinMerge-2.6.8-src.7z">.7z</a>).</p></dd>
  <dt><a href="http://prdownloads.sourceforge.net/winmerge/Merge7zInstaller0019-311-444.exe">7-Zip Plugin</a> (DllBuild 0019)</dt>
  <dd>
    <p>This is the 7-Zip plugin for WinMerge.</p>
    <p>You can also read the <a href="http://sourceforge.net/project/shownotes.php?group_id=13216&amp;release_id=482019">Release Notes</a> or search for <a href="http://sourceforge.net/project/showfiles.php?group_id=13216&amp;package_id=143957">other versions</a>.</p>
  </dd>
</dl>
<h3>Other Versions</h3>
<ul>
  <li><a href="http://sourceforge.net/project/showfiles.php?group_id=13216&amp;package_id=11248">Stable Versions</a></li>
  <li><a href="http://sourceforge.net/project/showfiles.php?group_id=13216&amp;package_id=29158">Beta Versions</a></li>
  <li><a href="http://sourceforge.net/project/showfiles.php?group_id=13216&amp;package_id=92246">Experimental Builds</a></li>
</ul>
<?php
  $page->printFoot();
?>