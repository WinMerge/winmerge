<?php
  include('../engine/engine.inc');

  $page = new Page;
  $page->setDescription(__('Download additional plugins for WinMerge, like the 7-Zip plugin or the xdocdiff Plugin.'));
  $page->setKeywords(__('WinMerge, free, download, plugins, plugin, plug-in, setup, installer, binaries, 7-Zip, xdocdiff, Word, Excel, PowerPoint, PDF'));
  $page->printHead(__('Download Plugins'), TAB_DOWNLOADS);
  
  $page->printHeading(__('Download Plugins'));
  $page->printSubHeading(__('7-Zip Plugin'));
  $page->printPara(__('This is the 7-Zip plugin for WinMerge and the current version <strong>%1$s</strong> was released at <strong>%2$s</strong>.', 'DllBuild 0026', '2007-12-23'));
?>
<div class="downloadmatrix">
<ul>
  <li><strong><?php __e('Installer');?></strong>
    <ul>
      <li><a href="http://downloads.sourceforge.net/winmerge/Merge7zInstaller0026-432-457.exe"><?php __e('Exe-Format (%s MB)', '1.05');?></a></li>
    </ul>
  </li>
  <li><strong><?php __e('Binaries');?></strong>
    <ul>
      <li><a href="http://downloads.sourceforge.net/winmerge/Merge7z0026-432-457.7z"><?php __e('7z-Format (%s MB)', '0.91');?></a></li>
    </ul>
  </li>
</ul>
</div>
<?php
  $page->printPara(__('You can also read the <a href="%1$s">Release Notes</a> or search for <a href="%2$s">other versions</a>.', 'http://sourceforge.net/project/shownotes.php?release_id=563695&amp;group_id=13216', 'http://sourceforge.net/project/showfiles.php?group_id=13216&amp;package_id=143957'));
  
  $page->printSubHeading(__('xdocdiff Plugin'));
  $page->printPara(__('With this plugin you can compare Word, Excel, PowerPoint, PDF and some more files.'));
  $page->printPara(__('For more details and the download go to the <a href="%s">xdocdiff plugin homepage</a>.', 'http://freemind.s57.xrea.com/xdocdiffPlugin/en/index.html'));
  
  $page->printFoot();
?>