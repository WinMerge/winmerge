<?php
  include('../page.inc');

  $page = new Page;
  $page->printHead('WinMerge: Developers', WINMERGE_DEV);
?>
<h2>WinMerge developers</h2>
<p>This is a list of some of the current developers on WinMerge</p>
<ul>
  <li class="toc">Chris Mumford</li>
  <li class="toc">Christian List</li>
  <li class="toc">Dean Grimm</li>
  <li class="toc">Dennis Limm</li>
  <li class="toc">Jochen Tucht</li>
  <li class="toc">Kimmo Varis</li>
  <li class="toc">Laurent Ganier</li>
  <li class="toc">Perry Rapp</li>
</ul>
<p>Look how to <a href="needed.php">develop</a> and 
<a href="l10n/index.php">translate WinMerge</a>
and get some hints for <a href="wincvs.php">WinCVS</a>.</p>
<?php
  $page->printFoot();
?>