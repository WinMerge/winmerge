<?php
  include('page.inc');

  $page = new Page;
  $page->printHead('WinMerge', WINMERGE_USR);
?>
<p>WinMerge is an Open Source visual text file differencing and merging tool for Win32 platforms. It is highly useful for determing what has changed between project versions, and then merging changes between versions.</p>
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
  <li><a href="2.6/manual/index.html">Online manual</a> and installed HTML Help manual</li>
</ul>

<h2>Screenshots</h2>
<p><a href="2.6/images/screenshot.png" target="_blank"><img class="thumbnail" src="2.6/images/screenshot_small.png" alt="File Comparison" border="0"></a>
<a href="2.6/images/foldercmp.png" target="_blank"><img class="thumbnail" src="2.6/images/foldercmp_small.png" alt="Folder Comparison Results" border="0"></a></p>
<p>See the <a href="2.6/screenshots.php">screenshots page</a> for more screenshots.</p>

<h2>WinMerge 2.6.6 - latest stable version</h2>
<p><a href="2.6/index.php">WinMerge 2.6.6</a> is the latest stable version, and is recommended for most users.</p>
<p><a href="http://prdownloads.sourceforge.net/winmerge/WinMerge-2.6.6-Setup.exe">Download WinMerge 2.6.6 Installer</a></p>

<h2>Installation</h2>
<p>Just download the <a href="http://prdownloads.sourceforge.net/winmerge/WinMerge-2.6.6-Setup.exe">installer</a> and double-click it.</p>
<p>Or you may simply unzip the <a href="http://prdownloads.sourceforge.net/winmerge/WinMerge-2.6.6-exe.zip">binary zip package</a> into the desired location, and run
<code>WinMergeU.exe</code> (except for Windows 98/ME users who should use <code>WinMerge.exe</code>).</p>
<p>For more information, please see the <a href="2.6/manual/Installing.html">Installation section</a> of the <a href="2.6/manual/index.html">Manual</a>.</p>

<h2>Using WinMerge</h2>
<p>For information on using WinMerge, see our <a href="2.6/manual/index.html">online Manual</a>,
or you can use the local help file (just press <code>F1</code> inside WinMerge) for offline use.</p>

<p>Here is the current <a href="2.6/changes.php">change history</a>.</p>

<h2>Support</h2>
<p>If you need support, look at our <a href="support/index.php">support page</a> for more information how you can get it.</p>

<h2>Developers</h2>
<p>WinMerge is an open source project, which means that the program is maintained and developed by
volunteers. If you''re interested in contributing work to the project, see our <a href="developer/needed.php">developer page</a>
for further information on how you may contribute.</p>
<p>In addition, WinMerge is translated into a number of different languages. See our <a href="developer/translate.php">information on translating WinMerge</a> into your own language.</p>
<p><a href="developer/index.php">The WinMerge Development Team</a></p>
<?php
  $page->printFoot();
?>