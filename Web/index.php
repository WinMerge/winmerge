<?php
  include('page.inc');

  $page = new Page;
  $page->printHead();
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
  <li>In-line word difference visualisation</li>
  <li>File filters in directory diff</li>
  <li>Moved lines detection in file compare</li>
  <li>Shell Integration</li>
  <li>Rudimentary Visual SourceSafe and Rational ClearCase integration</li>
  <li>Archive file support using 7-zip</li>
  <li>Plugins</li>
  <li>Localizable interface via resource DLL</li>
  <li>HTML-based <a href="2.2/manual/index.html">Manual</a></li>
</ul>

<h2>Screenshot</h2>
<p><img src="2.2/images/screenshot.gif" alt="Screenshot of WinMerge"></p>
<p>See the <a href="2.2/screenshots.php">screenshots page</a> for more screenshots.</p>

<h2>WinMerge 2.2 - latest stable version</h2>
<p><a href="2.2/index.php">WinMerge 2.2</a> is the latest stable version, and it is recommended for most users.</p>
<p><a href="http://prdownloads.sourceforge.net/winmerge/WinMerge-2.2.0-Setup.exe">Download WinMerge 2.2 Installer</a></p>

<h2>WinMerge 2.0.2 - older stable version</h2>
<p><a href="2.0/index.php">WinMerge 2.0.2</a> is an older version.</p>
<p><a href="http://prdownloads.sourceforge.net/winmerge/WinMergeSetup202.exe">Download WinMerge 2.0.2 Installer</a></p>

<!--
<h2>WinMerge 2.1 - current development version</h2>
<p>WinMerge 2.1 is the development version, and is only recommended for users that can tolerate possibly losing their files once in a while, because we do not guarantee it to be stable.</p>
<p>On the other hand, this version has the latest and greatest features in WinMerge.</p>
<p><a href="http://sourceforge.net/project/showfiles.php?group_id=13216&amp;release_id=199032">Download WinMerge 2.1</a></p>
-->

<h2>WinMerge 1.7 - much older stable version</h2>
<p><a href="1.7/index.php">WinMerge 1.7</a> is an much older and simpler version, from before the integrated editor was introduced.</p>
<p><a href="http://prdownloads.sourceforge.net/winmerge/WinMergeSetup.exe">Download WinMerge 1.7 Installer</a></p>

<h2>Installation</h2>
<p>Just download the <a href="http://prdownloads.sourceforge.net/winmerge/WinMerge-2.2.0-Setup.exe">installer</a> and double-click it.</p>
<p>Or you may simply unzip the <a href="http://prdownloads.sourceforge.net/winmerge/WinMerge-2.2.0-exe.zip">binary zip package</a> into the desired location, and run
<code>WinMerge.exe</code> or <code>WinMergeU.exe</code> (the latter is the Unicode version, only for use on NT/2000/XP).</p>
<p>For more details please look at the <a href="2.2/manual/installing.html">Installation section</a> from the <a href="2.2/manual/index.html">Manual</a>.</p>

<h2>Using WinMerge</h2>
<p>You can use the <a href="2.2/manual/index.html">online Manual</a>, or you can
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

<p>Here is the current <a href="2.2/changes.php">change history</a>.</p>

<p>Since WinMerge is an Open Source project, you may use it free of charge.
But please consider making a <a href="http://sourceforge.net/project/project_donations.php?group_id=13216">donation</a>
to support the continued development of WinMerge.</p>

<h2>Developers</h2>
<p>On the <a href="developer/index.php">developer page</a>
you get more information about the development for WinMerge.</p>
<p>You get a list from the current developers, how to
<a href="developer/needed.php">develop</a> and
<a href="developer/translate.php">translate WinMerge</a>
and some hints for <a href="developer/wincvs.php">WinCVS</a>.</p>
<?php
  $page->printFoot();
?>