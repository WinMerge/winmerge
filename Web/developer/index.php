<?php
  include('../page.inc');

  $page = new Page;
  $page->printHead('WinMerge: Developers', WINMERGE_DEV);
?>
<h2>WinMerge developers</h2>
<p>This is a list of some of the current developers on WinMerge</p>
<ul>
  <li>Chris Mumford</li>
  <li>Christian List</li>
  <li>Dean Grimm</li>
  <li>Dennis Limm</li>
  <li>Jochen Tucht</li>
  <li>Kimmo Varis</li>
  <li>Laurent Ganier</li>
  <li>Perry Rapp</li>
</ul>
<p>Look how to <a href="needed.php">develop</a> and 
<a href="l10n/index.php">translate WinMerge</a>
and get some hints for <a href="wincvs.php">WinCVS</a>.</p>
<?php
  $page->printFoot();
?>