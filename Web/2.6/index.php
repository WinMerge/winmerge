<?php
  include('../page.inc');

  $page = new Page;
  $page->printHead('WinMerge 2.6', WINMERGE_2_6);
?>
<p>WinMerge is an Open Source visual text file differencing and merging tool for Win32 platforms. It is highly useful for determing what has changed between project versions, and then merging changes between versions.</p>
<?php $page->printDownloadNow(); ?>
<h2>Features</h2>
<ul>
  <li>Visual differencing and merging of text files</li>
  <li>Flexible editor with syntax highlighting, line numbers and word-wrap</li>
  <li>Handles DOS, UNIX and MAC text file formats</li>
  <li>Unicode support</li>
  <li>Difference pane shows current difference in two vertical panes</li>
  <li>Location pane shows map of files compared</li>
  <li>Highlights differences inside lines in file compare</li>
  <li>Regular Expression based file filters in directory compare allow excluding and including items</li>
  <li>Moved lines detection in file compare</li>
  <li>Creates patch files</li>
  <li>Shell Integration (supports 64-bit Windows versions)</li>
  <li>Rudimentary Visual SourceSafe and Rational ClearCase integration</li>
  <li>Archive file support using 7-Zip</li>
  <li>Plugin support</li>
  <li>Localizable interface via resource DLL</li>
  <li><a href="manual/index.html">Online manual</a> and installed HTML Help manual</li>
</ul>

<h2>Screenshots</h2>
<p><a href="images/screenshot.png" target="_blank"><img class="thumbnail" src="images/screenshot_small.png" alt="File Comparison" border="0"></a>
<a href="images/foldercmp.png" target="_blank"><img class="thumbnail" src="images/foldercmp_small.png" alt="Folder Comparison Results" border="0"></a></p>
<p>See the <a href="screenshots.php">screenshots page</a> for more screenshots.</p>

<h2>Installation</h2>
<p>Just download the <a href="http://downloads.sourceforge.net/winmerge/WinMerge-2.6.14-Setup.exe">installer</a> and double-click it.</p>
<p>Or you may simply unzip the <a href="http://downloads.sourceforge.net/winmerge/WinMerge-2.6.14-exe.zip">binary zip package</a> into the desired location, and run
<code>WinMergeU.exe</code> (except for Windows 98/ME users who should use <code>WinMerge.exe</code>).</p>
<p>For more information, please see the <a href="manual/Installing.html">Installation section</a> of the <a href="manual/index.html">Manual</a>.</p>

<h2>Using WinMerge</h2>
<p>For information on using WinMerge, see our <a href="manual/index.html">online Manual</a>,
or you can use the local help file (just press <code>F1</code> inside WinMerge) for offline use.</p>

<p>Here is the current <a href="changes.php">change history</a>.</p>
<?php
  $page->printFoot();
?>