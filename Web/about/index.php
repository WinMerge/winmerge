<?php
  include('../engine/engine.inc');

  $page = new Page;
  $page->setDescription(__('WinMerge is highly useful for determining what has changed between project versions, and then merging changes between versions. WinMerge can be used as an external differencing/merging tool or as a standalone application.'));
  $page->setKeywords(__('WinMerge, about, features, file compare, folder compare, version control, screenshots, installation'));
  $page->printHead(__('About WinMerge'), TAB_ABOUT);
  $stablerelease = $page->getStableRelease();
  
  $page->printHeading(__('About WinMerge'));
  $page->printPara(__('WinMerge is an <a href="%s">Open Source</a> differencing and merging tool for Windows. WinMerge can compare both folders and files, presenting differences in a visual text format that is easy to understand and handle.', 'license.php'));
  $page->printPara(__('WinMerge is highly useful for determining what has changed between project versions, and then merging changes between versions. WinMerge can be used as an external differencing/merging tool or as a standalone application.'));
  $page->printDownloadNow();
  
  $page->printSubHeading(__('Features'));
  $page->printPara(__('In addition, WinMerge has many helpful supporting features that make comparing, synchronising, and merging as easy and useful as possible:'));
?>
<ul>
  <li><strong><?php __e('General');?></strong>
    <ul>
      <li><?php __e('Supports Microsoft Windows 98/2000/XP/2003/Vista/2008');?></li>
      <li><?php __e('Handles Windows, Unix and Mac text file formats');?></li>
      <li><?php __e('Unicode support');?></li>
      <li><?php __e('Tabbed interface');?></li>
    </ul>
  </li>
  <li><strong><?php __e('File Compare');?></strong>
    <ul>
      <li><?php __e('Visual differencing and merging of text files');?></li>
      <li><?php __e('Flexible editor with syntax highlighting, line numbers and word-wrap');?></li>
      <li><?php __e('Highlights differences inside lines');?></li>
      <li><?php __e('Difference pane shows current difference in two vertical panes');?></li>
      <li><?php __e('Location pane shows map of files compared');?></li>
      <li><?php __e('Moved lines detection');?></li>
    </ul>
  </li>
  <li><strong><?php __e('Folder Compare');?></strong>
    <ul>
      <li><?php __e('Regular Expression based file filters allow excluding and including items');?></li>
      <li><?php __e('Fast compare using file sizes and dates');?></li>
      <li><?php __e('Compares one folder or includes all subfolders');?></li>
      <li><?php __e('Can show folder compare results in a tree-style view');?></li>
    </ul>
  </li>
  <li><strong><?php __e('Version Control');?></strong>
    <ul>
      <li><?php __e('Creates patch files (Normal-, Context- and Unified formats)');?></li>
      <li><?php __e('Resolve conflict files');?></li>
      <li><?php __e('Rudimentary Visual SourceSafe and Rational ClearCase integration');?></li>
    </ul>
  </li>
  <li><strong><?php __e('Other');?></strong>
    <ul>
      <li><?php __e('Shell Integration (supports 64-bit Windows versions)');?></li>
      <li><?php __e('Archive file support using 7-Zip');?></li>
      <li><?php __e('Plugin support');?></li>
      <li><?php __e('Localizable interface');?></li>
      <li><?php __e('<a href="%s">Online manual</a> and installed HTML Help manual', '/docs/manual/');?></li>
    </ul>
  </li>
</ul>

<?php
  $page->printSubHeading(__('Screenshots'));
?>
<p><a href="screenshots/screenshot.png" target="_blank"><img class="thumbnail" src="screenshots/screenshot_small.png" alt="<?php __e('File Comparison');?>" border="0"></a>
<a href="screenshots/foldercmp.png" target="_blank"><img class="thumbnail" src="screenshots/foldercmp_small.png" alt="<?php __e('Folder Comparison Results');?>" border="0"></a></p>
<?php
  $page->printPara(__('See the <a href="%s">screenshots page</a> for more screenshots.', 'screenshots/'));
  
  $page->printSubHeading(__('Installation'));
  $page->printPara(__('Just download the <a href="%s">installer</a> and double-click it.', $stablerelease->getDownload('setup.exe')));
  $page->printPara(__('Or you may simply unzip the <a href="%s">binary zip package</a> into the desired location, and run <code>WinMergeU.exe</code> (except for Windows 98/ME users who should use <code>WinMerge.exe</code>).', $stablerelease->getDownload('exe.zip')));
  $page->printPara(__('For more information, please see the <a href="%1$s">Installation section</a> of the <a href="%2$s">Manual</a>.', '/docs/manual/Installing.html', '/docs/manual/'));
  
  $page->printSubHeading(__('Using WinMerge'));
  $page->printPara(__('For information on using WinMerge, see our <a href="%s">online Manual</a>, or you can use the local help file (just press <code>F1</code> inside WinMerge) for offline use.', '/docs/manual/'));
  $page->printPara(__('Here is the current <a href="%s">change log</a>.', '/docs/changelog.php'));

  $page->printFoot();
?>
