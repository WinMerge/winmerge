<?php
  include('../page.inc');

  $page = new Page;
  $page->printHead('WinMerge 2.0', WINMERGE_2_0);
?>
<p>WinMerge is an Open Source visual text file differencing and merging tool for Win32 platforms. It is highly useful for determing what has changed between project versions, and then merging changes between versions.</p>
<h2>Features</h2>
<ul>
  <li>Visual differencing of text files</li>
  <li>Syntax highlighting</li>
  <li>Diff a single file, or entire directories</li>
  <li>File filters in directory diff</li>
  <li>Merge differences</li>
  <li>Directory recursion</li>
  <li>Integrated editor (with multi-level undo/redo)</li>
  <li>Find &amp; Replace</li>
  <li>Drag &amp; Drop support</li>
  <li>Option for integration with Windows Explorer</li>
  <li>Rudimentary Visual SourceSafe and Rational ClearCase integration</li>
  <li>Handles DOS, UNIX and MAC text file formats</li>
  <li>Localizable interface via resource DLL</li>
  <li>HTML-based <a href="docs/index.html">User's Guide</a></li>
</ul>

<h2>Screenshot</h2>
<img src="images/screenshot.png" alt="Screenshot of WinMerge">

<h2>Installation</h2>
<p>Just download the <a href="http://prdownloads.sourceforge.net/winmerge/WinMergeSetup202.exe">installer</a> and double-click it.</p>
<p>Or you may simply unzip the <a href="http://prdownloads.sourceforge.net/winmerge/WinMerge202-exe.zip">binary zip package</a> into the desired location, and run
<code>WinMergeU.exe</code> (except for Windows 98/ME users who should use <code>WinMerge.exe</code>).</p>

<h2>Using WinMerge</h2>
<p>You can use the <a href="docs/index.html">online User's Guide</a>, or you can
<a href="http://prdownloads.sourceforge.net/winmerge/WinMergeDocs2_0.zip">download</a> a copy.</p>

<h2>Support</h2>
<p>WinMerge is an Open Source project, that is covered
under the <a href="http://www.gnu.org/copyleft/gpl.html">GNU Public License</a>.
As such, your main form of support for WinMerge will be through
subscription to the <a href="http://lists.sourceforge.net/lists/listinfo/winmerge-support">WinMerge-support mailing list</a>.
In addition, SourceForge has bug tracking capabilities, so please report any
bugs <a href="http://sourceforge.net/bugs/?group_id=13216">here</a>. Wish list items on the
<a href="http://sourceforge.net/tracker/?group_id=13216&amp;atid=363216">feature request list</a> will also be considered, but we make
absolutely no promises.</p>

<p>Here is the current <a href="changes.php">change history</a>.</p>

<p>Since WinMerge is an Open Source project, you may use it free of charge.
But please consider making a <a href="http://sourceforge.net/project/project_donations.php?group_id=13216">donation</a>
to support the continued development of WinMerge.</p>
<?php
  $page->printFoot();
?>