<?php
  include('../../page.inc');

  $page = new Page;
  $page->setDescription('Screenshots from WinMerge features like file comparison, folder comparison results and location pane.');
  $page->setKeywords('WinMerge, screenshots, file comparison, open-dialog, folder comparison results, highlight line diff, location pane, splash screen');
  $page->printHead('WinMerge: Screenshots', TAB_ABOUT);
?>
<h2>Screenshots</h2>
<div class="screenshots">
  <h3>File Comparison</h3>
  <a href="filecmp.png" target="_blank"><img src="filecmp_small.png" alt="File Comparison" width="305" height="200" border="0"></a>
  <p>File compare window is basically two files opened to editor into two horizontal panes. Editing allows user to easily do small changes without need to open files to other editor or development environment.</p>
  <h3>Open-dialog</h3>
  <a href="open-dialog.png" target="_blank"><img src="open-dialog_small.png" alt="Open-dialog" width="300" height="126" border="0"></a>
  <p>WinMerge allows selecting/opening paths in several ways. Using the Open-dialog is just one of them.</p>
  <h3>Folder Comparison Results</h3>
  <a href="foldercmp.png" target="_blank"><img src="foldercmp_small.png" alt="Folder Comparison Results" width="316" height="200" border="0"></a>
  <p>Folder compare shows all files and subfolders found from compared folders as list. Folder compare allows synchronising folders by copying and deleting files and subfolders. Folder compare view can be versatile customised.</p>
  <h3>Highlight Line Diff</h3>
  <a href="filecmp_inlinediff.png" target="_blank"><img src="filecmp_inlinediff_small.png" alt="Highlight Line Diff" width="294" height="200" border="0"></a>
  <p>Many times it is useful to see exact differences inside lines. WinMerge can highlight different area between lines.</p>
  <h3>Location Pane</h3>
  <a href="screenshot.png" target="_blank"><img src="screenshot_small.png" alt="Location Pane" width="300" height="200" border="0"></a>
  <p>Location pane is a handy side-pane showing a overall (difference) map of files compared.</p>
  <h3>Splash Screen</h3>
  <a href="splash_screen.png" target="_blank"><img src="splash_screen_small.png" alt="Splash Screen" width="312" height="200" border="0"></a>
  <p>WinMerge 2.8 has a new professional looking splash screen.</p>
  <br />
</div>
<?php
  $page->printFoot();
?>