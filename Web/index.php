<?php
  include('page.inc');

  $page = new Page;
  $page->printHead();
?>
<h1><img src="images/winmerge.gif" height="32" width="32" align="top" alt="WinMerge logo"> WinMerge</h1>
<p>WinMerge is an Open Source visual text file differencing and merging tool for Win32 platforms. It is highly useful for determing what has changed between project versions, and then merging changes between versions.</p>
<h2>Features</h2>
<ul>
  <li class="toc">Visual differencing of text files
  <li class="toc">Syntax highlighting
  <li class="toc">Diff a single file, or entire directories
  <li class="toc">File filters in directory diff
  <li class="toc">Merge differences
  <li class="toc">Directory recursion
  <li class="toc">Integrated editor (with multi-level undo/redo)
  <li class="toc">Find &amp; Replace
  <li class="toc">Drag &amp; Drop support
  <li class="toc">Option for integration with Windows Explorer
  <li class="toc">Rudimentary Visual SourceSafe and Rational ClearCase integration
  <li class="toc">Handles DOS, UNIX and MAC text file formats
  <li class="toc">Localizable interface via resource DLL
  <li class="toc">HTML-based <a href="2.0/docs/index.html">User's Guide</a></li>
</ul>

<h2>Screenshot</h2>
<img src="images/screenshot.png" alt="Screenshot of WinMerge">

<h2>WinMerge 2.0.2 - latest stable version</h2>
<p><a href="2.0/index.php">WinMerge 2.0.2</a> is the latest stable version, and it is recommended for most users.</p>
<p><a href="http://prdownloads.sourceforge.net/winmerge/WinMergeSetup202.exe">Download WinMerge 2.0.2 Installer</a></p>

<h2>WinMerge 2.2RC - release candidate version</h2>
<p><a href="2.2/index.php">WinMerge 2.2RC</a> is the release candidate version from the next stable version with many new and great features.</p>
<p><a href="http://sourceforge.net/project/showfiles.php?group_id=13216&amp;package_id=29158&amp;release_id=273507">Download WinMerge 2.2RC</a></p>

<!--
<h2>WinMerge 2.1 - current development version</h2>
<p>WinMerge 2.1 is the development version, and is only recommended for users that can tolerate possibly losing their files once in a while, because we do not guarantee it to be stable.</p>
<p>On the other hand, this version has the latest and greatest features in WinMerge.</p>
<p><a href="http://sourceforge.net/project/showfiles.php?group_id=13216&amp;release_id=199032">Download WinMerge 2.1</a></p>
-->

<h2>WinMerge 1.7 - older stable version</h2>
<p><a href="1.7/index.php">WinMerge 1.7</a> is an older and simpler version, from before the integrated editor was introduced.</p>
<p><a href="http://prdownloads.sourceforge.net/winmerge/WinMergeSetup.exe">Download WinMerge 1.7 Installer</a></p>

<h2>Installation</h2>
<p>Just download the installer and double-click it.</p>
<p>Or you may simply unzip the binary zip package (eg, <code>WinMerge202-exe.zip</code>) into the desired location, and run
<code>WinMerge.exe</code> or <code>WinMergeU.exe</code> (the latter is the Unicode version, only for use on NT/2000/XP).</p>

<h2>Using WinMerge</h2>
<p>You can use the <a href="2.0/docs/index.html">online User's Guide</a>, or you can
<a href="http://sourceforge.net/project/showfiles.php?group_id=13216&amp;package_id=11254">download</a> a copy.</p>

<h2>Support</h2>
<p>WinMerge is an Open Source project, that is covered
under the <a href="http://www.gnu.org/copyleft/gpl.html">GNU Public License</a>.
As such, your main form of support for WinMerge will be through
subscription to the <a href="http://lists.sourceforge.net/lists/listinfo/winmerge-support">WinMerge-support mailing list</a>.
In addition, SourceForge has bug tracking capabilities, so please report any
bugs <a href="http://sourceforge.net/bugs/?group_id=13216">here</a>. Wish list items on the
<a href="http://sourceforge.net/tracker/?group_id=13216&amp;atid=363216">RFE list</a> will also be considered, but we make
absolutely no promises.</p>

<p>Here is the current <a href="2.0/changes.php">change history</a>.</p>

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