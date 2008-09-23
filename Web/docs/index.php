<?php
  include('../page.inc');

  $page = new Page;
  $page->printHead('WinMerge: Documentation', TAB_DOCS);
?>
<h2>Documentation</h2>
<h3><a href="manual/">Manual</a></h3>
<p>The <a href="manual/">manual</a> explains how to use WinMerge, and documents its capabilities and limitations.</p>
<h3><a href="releasenotes.php">Release Notes</a></h3>
<p>The <a href="releasenotes.php">release notes</a> are a short summary of important changes, enhancements, bug fixes and <a href="releasenotes.php#issues">known issues</a> in the current WinMerge release.</p>
<h3><a href="changelog.php">Change Log</a></h3>
<p>The <a href="changelog.php">change log</a> is a more complete list of changes in the last WinMerge releases.</p>
<h3><a href="/Wiki/">Developer Wiki</a></h3>
<p>The <a href="/Wiki/">Developer Wiki</a> contains many informations about the WinMerge development.</p>
<?php
  $page->printFoot();
?>