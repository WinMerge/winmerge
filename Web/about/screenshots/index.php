<?php
  include('../../page.inc');

  $page = new Page;
  $page->printHead('WinMerge: Screenshots', TAB_ABOUT);
?>
<h2>Screenshots</h2>
<h3>File Comparison</h3>
<p><img src="filecmp.png" alt="File Comparison"></p>
<p>File compare window is basically two files opened to editor into two horizontal panes. Editing allows user to easily do small changes without need to open files to other editor or development environment.</p>
<h3>Open-dialog</h3>
<p><img src="open-dialog.gif" alt="Open-dialog"></p>
<p>WinMerge allows selecting/opening paths in several ways. Using the Open-dialog is just one of them.</p>
<h3>Folder Comparison Results</h3>
<p><img src="foldercmp.png" alt="Folder Comparison Results"></p>
<p>Folder compare shows all files and subfolders found from compared folders as list. Folder compare allows synchronising folders by copying and deleting files and subfolders. Folder compare view can be versatile customised.</p>
<h3>Highlight Line Diff</h3>
<p><img src="filecmp_inlinediff.png" alt="Highlight Line Diff"></p>
<p>Many times it is useful to see exact differences inside lines. WinMerge can highlight different area between lines.</p>
<h3>Location Pane</h3>
<p><img src="screenshot.png" alt="Location Pane"></p>
<p>Location pane is a handy side-pane showing a overall (difference) map of files compared.</p>
<h3>Splash Screen</h3>
<p><img src="splash_screen.png" alt="Splash Screen"></p>
<p>WinMerge 2.8 has a new professional looking splash screen.</p>
<?php
  $page->printFoot();
?>