<?php
  include('../page.inc');

  $page = new Page;
  $page->printHead('WinMerge: Downloads', WINMERGE_2_4);
?>
<h2>Downloads</h2>
<p class="important">This is an old version of WinMerge, for the latest version go to the <a href="../index.php">main site</a>.</p>
<h3><a href="http://sourceforge.net/project/showfiles.php?group_id=13216&amp;package_id=11248&amp;release_id=438554">WinMerge 2.4.10</a></h3>
<dl>
  <dt><a href="http://prdownloads.sourceforge.net/winmerge/WinMerge-2.4.10-Setup.exe">Installer</a></dt>
  <dd><p>The easiest way to install WinMerge is to download and run this installer. Read the <a href="manual/installing.html">online manual</a> for help using it.</p></dd>
  <dt><a href="http://prdownloads.sourceforge.net/winmerge/WinMerge-2.4.10-exe.zip">Binaries</a></dt>
  <dd><p>WinMerge binaries are also available in archive files (<a href="http://prdownloads.sourceforge.net/winmerge/WinMerge-2.4.10-exe.zip">.zip</a> and <a href="http://prdownloads.sourceforge.net/winmerge/WinMerge-2.4.10-exe.7z">.7z</a>).</p></dd>
  <dt><a href="http://prdownloads.sourceforge.net/winmerge/WinMerge-2.4.10-src.zip">Source code</a></dt>
  <dd><p>Source code is available in archive files (<a href="http://prdownloads.sourceforge.net/winmerge/WinMerge-2.4.10-src.zip">.zip</a> and <a href="http://prdownloads.sourceforge.net/winmerge/WinMerge-2.4.10-src.7z">.7z</a>).</p></dd>
  <dt><a href="http://prdownloads.sourceforge.net/winmerge/Merge7zInstaller0018-311-442.exe ">7-Zip Plugin</a> (DllBuild 0018)</dt>
  <dd>
    <p>This is the 7-Zip plugin for WinMerge.</p>
    <p>You can also read the <a href="http://sourceforge.net/project/shownotes.php?group_id=13216&amp;release_id=428331">Release Notes</a> or search for <a href="http://sourceforge.net/project/showfiles.php?group_id=13216&amp;package_id=143957">other versions</a>.</p>
  </dd>
  <dt><a href="http://prdownloads.sourceforge.net/winmerge/WinMerge-2.4.10-manual.zip">Manual</a></dt>
  <dd><p>This is the offline version of the manual in archived format (<a href="http://prdownloads.sourceforge.net/winmerge/WinMerge-2.4.10-manual.zip">.zip</a> and <a href="http://prdownloads.sourceforge.net/winmerge/WinMerge-2.4.10-manual.7z">.7z</a>).</p></dd>
</dl>
<?php
  $page->printFoot();
?>