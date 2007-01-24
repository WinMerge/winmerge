<?php
  include('../page.inc');

  $page = new Page;
  $page->printHead('WinMerge 2.2', WINMERGE_2_2);
?>
<p>WinMerge is an Open Source visual text file differencing and merging tool for Win32 platforms. It is highly useful for determing what has changed between project versions, and then merging changes between versions.</p>
<h2>Features</h2>
<ul>
  <li>Visual differencing and merging of text files</li>
  <li>Flexible editor with syntax highlighting</li>
  <li>Handles DOS, UNIX and MAC text file formats</li>
  <li>Unicode support</li>
  <li>Difference pane shows current difference in two vertical panes</li>
  <li>Location pane shows map of files compared</li>
  <li>Highlights differences inside lines</li>
  <li>File filters in directory diff</li>
  <li>Moved lines detection in file compare</li>
  <li>Shell Integration</li>
  <li>Rudimentary Visual SourceSafe and Rational ClearCase integration</li>
  <li>Archive file support using 7-zip</li>
  <li>Plugins</li>
  <li>Localizable interface via resource DLL</li>
  <li>HTML-based <a href="manual/index.html">Manual</a></li>
</ul>

<h2>Screenshot</h2>
<p><img src="images/screenshot.gif" alt="Screenshot of WinMerge"></p>
<p>See the <a href="screenshots.php">screenshots page</a> for more screenshots.</p>

<h2>Installation</h2>
<p>Just download the <a href="http://prdownloads.sourceforge.net/winmerge/WinMerge-2.2.4-Setup.exe">installer</a> and double-click it.</p>
<p>Or you may simply unzip the <a href="http://prdownloads.sourceforge.net/winmerge/WinMerge-2.2.4-exe.zip">binary zip package</a> into the desired location, and run
<code>WinMergeU.exe</code> (except for Windows 98/ME users who should use <code>WinMerge.exe</code>).</p>
<p>For more details please look at the <a href="manual/installing.html">Installation section</a> from the <a href="manual/index.html">Manual</a>.</p>

<h2>Using WinMerge</h2>
<p>You can use the <a href="manual/index.html">online Manual</a>, or you can
<a href="http://sourceforge.net/project/showfiles.php?group_id=13216&amp;package_id=11254&amp;release_id=331358">download</a> a copy.</p>

<p>Here is the current <a href="changes.php">change history</a>.</p>
<?php
  $page->printFoot();
?>