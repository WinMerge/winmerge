<?php
  include('../page.inc');

  $page = new Page;
  $page->printHead('WinMerge 1.7', WINMERGE_1_7);
?>
<h2>WinMerge 1.7</h2>
<p class="important">This is an old version of WinMerge, for the latest version go to the <a href="http://winmerge.org/">main site</a>.</p>
<p>WinMerge is the result of my dissatisfaction with existing diff / merge tools available
on the web.&nbsp; They always seemed so lacking, and the one that ships with SourceSafe
doesn't work in standalone mode.&nbsp; So one day, I bit the bullet, grabbed a copy of the
GNU diff code, and wrote a MFC front end for it.&nbsp; Since then, I have added quite a
few features, including SourceSafe integration.&nbsp;&nbsp;Note that if you are in search of a directory
synchronizing tool only (ie: Briefcase), WinMerge is probably not the right tool for you.</p>
<h2>Features</h2>
<ul>
  <li>Visual differencing of text files</li>
  <li>Diff a single file, or entire directories</li>
  <li>Merge differences</li>
  <li>Multi-level undo</li>
  <li>Directory recursion</li>
  <li>Integrated editor</li>
  <li>Rudimentary Visual SourceSafe integration</li>
  <li>Localizable interface via/ resource DLL</li>
  <li>HTML-based <a href="docs/index.html">User's Guide</a></li>
</ul>
<h2>Installation</h2>
<p>Just download the installer and double-click <code>WinMergeSetup.exe</code>.</p>
<h2>Using WinMerge</h2>
<p>I actually dug up some time and created a simple User's Guide.
<a href="docs/index.html">Check it out</a>.</p>
<h2>Support</h2>
<p>WinMerge is now an Open Source project, that is covered
under the <a href="http://www.gnu.org/copyleft/gpl.html">GNU Public License</a>.&nbsp;
As such, your main form of support for WinMerge will be through
subscription to the <a href="http://sourceforge.net/mail/?group_id=13216">WinMerge-dev mailing list</a>.
In addition, SourceForge has bug tracking capabilities, so please report any
bugs <a href="http://sourceforge.net/bugs/?group_id=13216">here</a>.&nbsp; Wish list items will also be considered, but I make
absolutely no promises.</p>
<p>Here is the current <a href="changes.php">change history</a>.</p>
<?php
  $page->printFoot();
?>