<?php
  include('../page.inc');

  $page = new Page;
  $page->printHead('WinMerge 2.4: Screenshots', WINMERGE_2_4);
?>
<h2>Screenshots</h2>
<p><img src="images/file-compare.gif" alt="File Comparison"></p>
<p>File compare window is basically two files opened to editor into two horizontal panes. Editing allows user to easily do small changes without need to open files to other editor or development environment.</p>
<p><img src="images/open-dialog.gif" alt="Open-dialog"></p>
<p>WinMerge allows selecting/opening paths in several ways. Using the Open-dialog is just one of them.</p>
<p><img src="images/dir-compare-status.gif" alt="Compare Statepane"></p>
<p>Statuspane always shows current status of compare so it can be used to verify there are no different files left to handle.</p>
<p><img src="images/dir-compare.gif" alt="Directory Comparison Results"></p>
<p>Directory compare shows all files and subdirectries found from compared directories as list. Directory compare allows synchronising directories by copying and deleting files and subdirectories. Directory compare view can be versatile customised.</p>
<p><img src="images/line-diff.gif" alt="Highlight Line Diff"></p>
<p>Many times it is useful to see exact differences inside lines. WinMerge can highlight different area between lines.</p>
<?php
  $page->printFoot();
?>