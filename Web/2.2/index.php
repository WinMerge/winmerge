<?php
  include('../page.inc');

  $page = new Page;
  $page->printHead('WinMerge 2.2', WINMERGE_2_2);
?>
<p>WinMerge is an Open Source visual text file differencing and merging tool for Win32 platforms. It is highly useful for determing what has changed between project versions, and then merging changes between versions.</p>
<h2>Features</h2>
<ul>
  <li class="toc">Visual differencing and merging of text files</li>
  <li class="toc">Flexible editor with syntax highlighting</li>
  <li class="toc">Handles DOS, UNIX and MAC text file formats</li>
  <li class="toc">Unicode support</li>
  <li class="toc">Difference pane shows current difference in two vertical panes</li>
  <li class="toc">Location pane shows map of files compared</li>
  <li class="toc">In-line word difference visualisation</li>
  <li class="toc">File filters in directory diff</li>
  <li class="toc">Moved lines detection in file compare</li>
  <li class="toc">Shell Integration</li>
  <li class="toc">Rudimentary Visual SourceSafe and Rational ClearCase integration</li>
  <li class="toc">Archive file support using 7-zip</li>
  <li class="toc">Plugins</li>
  <li class="toc">Localizable interface via resource DLL</li>
  <li class="toc">HTML-based <a href="manual/index.html">Manual</a></li>
</ul>

<h2>Screenshot</h2>
<img src="images/filecomp1.gif" alt="Screenshot of WinMerge">

<h2>Installation</h2>
<p>Just download the <a href="http://prdownloads.sourceforge.net/winmerge/WinMerge-2.2.0-Setup.exe">installer</a> and double-click it.</p>
<p>Or you may simply unzip the <a href="http://prdownloads.sourceforge.net/winmerge/WinMerge-2.2.0-exe.zip">binary zip package</a> into the desired location, and run
<code>WinMerge.exe</code> or <code>WinMergeU.exe</code> (the latter is the Unicode version, only for use on NT/2000/XP).</p>
<p>For more details please look at the <a href="manual/installing.html">Installation section</a> from the <a href="manual/index.html">Manual</a>.</p>

<h2>Using WinMerge</h2>
<p>You can use the <a href="manual/index.html">online Manual</a>, or you can
<a href="http://sourceforge.net/project/showfiles.php?group_id=13216&amp;package_id=11254&amp;release_id=281681">download</a> a copy.</p>

<h2>Support</h2>
<p>WinMerge is an Open Source project, that is covered
under the <a href="http://www.gnu.org/copyleft/gpl.html">GNU Public License</a>.
As such, your main form of support for WinMerge will be through
subscription to the <a href="http://lists.sourceforge.net/lists/listinfo/winmerge-support">WinMerge-support mailing list</a>.
In addition, SourceForge has bug tracking capabilities, so please report any
bugs <a href="http://sourceforge.net/bugs/?group_id=13216">here</a>. Wish list items on the
<a href="http://sourceforge.net/tracker/?group_id=13216&amp;atid=363216">RFE list</a> will also be considered, but we make
absolutely no promises.</p>

<p>Here is the current <a href="changes.php">change history</a>.</p>

<p>Since WinMerge is an Open Source project, you may use it free of charge.
But please consider making a <a href="http://sourceforge.net/project/project_donations.php?group_id=13216">donation</a>
to support the continued development of WinMerge.</p>
<?php
  $page->printFoot();
?>