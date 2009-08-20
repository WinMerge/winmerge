<?php
  include('../../engine/engine.inc');

  $page = new Page;
  $page->setDescription(__('Screenshots from WinMerge features like file comparison, folder comparison results and location pane.'));
  $page->setKeywords(__('WinMerge, screenshots, file comparison, open-dialog, folder comparison results, highlight line diff, location pane, splash screen'));
  $page->printHead(__('Screenshots'), TAB_ABOUT);
  
  $page->printHeading(__('Screenshots'));
?>
<div class="screenshots">
  <h3><?php __e('File Comparison');?></h3>
  <a href="filecmp.png" target="_blank"><img src="filecmp_small.png" alt="<?php __e('File Comparison');?>" width="305" height="200" border="0"></a>
  <p><?php __e('File compare window is basically two files opened to editor into two horizontal panes. Editing allows user to easily do small changes without need to open files to other editor or development environment.');?></p>
  <h3><?php __e('Open-dialog');?></h3>
  <a href="open-dialog.png" target="_blank"><img src="open-dialog_small.png" alt="<?php __e('Open-dialog');?>" width="300" height="126" border="0"></a>
  <p><?php __e('WinMerge allows selecting/opening paths in several ways. Using the Open-dialog is just one of them.');?></p>
  <h3><?php __e('Folder Comparison Results');?></h3>
  <a href="foldercmp.png" target="_blank"><img src="foldercmp_small.png" alt="<?php __e('Folder Comparison Results');?>" width="316" height="200" border="0"></a>
  <p><?php __e('Folder compare shows all files and subfolders found from compared folders as list. Folder compare allows synchronising folders by copying and deleting files and subfolders. Folder compare view can be versatile customised.');?></p>
  <h3><?php __e('Folder Compare Tree View');?></h3>
  <a href="foldercmp_treeview.png" target="_blank"><img src="foldercmp_treeview_small.png" alt="<?php __e('Folder Compare Tree View');?>" width="285" height="200" border="0"></a>
  <p><?php __e('In the tree view, folders are expandable and collapsible, containing files and subfolders. This is useful for an easier navigation in deeply nested directory structures. <em>The tree view is available only in recursive compares.');?></em></p>
  <h3><?php __e('Highlight Line Diff');?></h3>
  <a href="filecmp_inlinediff.png" target="_blank"><img src="filecmp_inlinediff_small.png" alt="<?php __e('Highlight Line Diff');?>" width="294" height="200" border="0"></a>
  <p><?php __e('Many times it is useful to see exact differences inside lines. WinMerge can highlight different area between lines.');?></p>
  <h3><?php __e('Location Pane');?></h3>
  <a href="screenshot.png" target="_blank"><img src="screenshot_small.png" alt="<?php __e('Location Pane');?>" width="300" height="200" border="0"></a>
  <p><?php __e('Location pane is a handy side-pane showing a overall (difference) map of files compared.');?></p>
  <h3><?php __e('Splash Screen');?></h3>
  <a href="splash_screen.png" target="_blank"><img src="splash_screen_small.png" alt="<?php __e('Splash Screen');?>" width="312" height="200" border="0"></a>
  <p><?php __e('WinMerge 2.8 has a new professional looking splash screen.');?></p>
  <br />
</div>
<?php
  $page->printFoot();
?>