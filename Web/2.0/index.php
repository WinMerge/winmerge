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

<p>Here is the current <a href="changes.php">change history</a>.</p>
<?php
  $page->printFoot();
?>