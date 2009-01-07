<?php
  include('../page.inc');

  $page = new Page;
  $page->setDescription('WinMerge is highly useful for determining what has changed between project versions, and then merging changes between versions. WinMerge can be used as an external differencing/merging tool or as a standalone application.');
  $page->setKeywords('WinMerge, about, features, file compare, folder compare, version control, screenshots, installation');
  $page->printHead('WinMerge: About', TAB_ABOUT);
  $stablerelease = $page->getStableRelease();
?>
<h2>About WinMerge</h2>
<p>WinMerge is an <a href="license.php">Open Source</a> differencing and merging tool for Windows. WinMerge can compare both folders and files, presenting differences in a visual text format that is easy to understand and handle.</p>
<p>WinMerge is highly useful for determining what has changed between project versions, and then merging changes between versions. WinMerge can be used as an external differencing/merging tool or as a standalone application.</p>
<?php $page->printDownloadNow(); ?>

<h3>Features</h3>
<p>In addition, WinMerge has many helpful supporting features that make comparing, synchronising, and merging as easy and useful as possible:</p>
<ul>
  <li><strong>General</strong>
    <ul>
      <li>Supports Microsoft Windows 98/2000/XP/2003/Vista/2008</li>
      <li>Handles Windows, Unix and Mac text file formats</li>
      <li>Unicode support</li>
      <li>Tabbed interface</li>
    </ul>
  </li>
  <li><strong>File Compare</strong>
    <ul>
      <li>Visual differencing and merging of text files</li>
      <li>Flexible editor with syntax highlighting, line numbers and word-wrap</li>
      <li>Highlights differences inside lines</li>
      <li>Difference pane shows current difference in two vertical panes</li>
      <li>Location pane shows map of files compared</li>
      <li>Moved lines detection</li>
    </ul>
  </li>
  <li><strong>Folder Compare</strong>
    <ul>
      <li>Regular Expression based file filters allow excluding and including items</li>
      <li>Fast compare using file sizes and dates</li>
      <li>Compares one folder or includes all subfolders</li>
    </ul>
  </li>
  <li><strong>Version Control</strong>
    <ul>
      <li>Creates patch files</li>
      <li>Resolve conflict files</li>
      <li>Rudimentary Visual SourceSafe and Rational ClearCase integration</li>
    </ul>
  </li>
  <li><strong>Other</strong>
    <ul>
      <li>Shell Integration (supports 64-bit Windows versions)</li>
      <li>Archive file support using 7-Zip</li>
      <li>Plugin support</li>
      <li>Localizable interface</li>
      <li><a href="/docs/manual/">Online manual</a> and installed HTML Help manual</li>
    </ul>
  </li>
</ul>

<h3>Screenshots</h3>
<p><a href="screenshots/screenshot.png" target="_blank"><img class="thumbnail" src="screenshots/screenshot_small.png" alt="File Comparison" border="0"></a>
<a href="screenshots/foldercmp.png" target="_blank"><img class="thumbnail" src="screenshots/foldercmp_small.png" alt="Folder Comparison Results" border="0"></a></p>
<p>See the <a href="screenshots/">screenshots page</a> for more screenshots.</p>

<h3>Installation</h3>
<p>Just download the <a href="<?php echo $stablerelease->getDownload('setup.exe');?>">installer</a> and double-click it.</p>
<p>Or you may simply unzip the <a href="<?php echo $stablerelease->getDownload('exe.zip');?>">binary zip package</a> into the desired location, and run
<code>WinMergeU.exe</code> (except for Windows 98/ME users who should use <code>WinMerge.exe</code>).</p>
<p>For more information, please see the <a href="/docs/manual/Installing.html">Installation section</a> of the <a href="/docs/manual/">Manual</a>.</p>

<h3>Using WinMerge</h3>
<p>For information on using WinMerge, see our <a href="/docs/manual/">online Manual</a>,
or you can use the local help file (just press <code>F1</code> inside WinMerge) for offline use.</p>

<p>Here is the current <a href="/docs/changelog.php">change log</a>.</p>
<?php
  $page->printFoot();
?>
