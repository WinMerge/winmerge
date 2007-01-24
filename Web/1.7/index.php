<?php
  include('../page.inc');

  $page = new Page;
  $page->printHead('WinMerge 1.7', WINMERGE_1_7);
?>
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
<p>Just download the <a href="http://download.sourceforge.net/winmerge/WinMergeSetup.exe">installer</a> and double-click it.</p>
<h2>Using WinMerge</h2>
<p>You can use the <a href="docs/index.html">online User's Guide</a>, or you can
<a href="http://download.sourceforge.net/winmerge/WinMergeDocs1_7.zip">download</a> a copy.</p>
<p>Here is the current <a href="changes.php">change history</a>.</p>
<?php
  $page->printFoot();
?>