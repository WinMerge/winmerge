<?php
  include('../page.inc');

  $page = new Page;
  $page->printHead('WinMerge: Developers');
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
<p>Look how to <a href="http://winmerge.org/developer/needed.php" target="_top">develop</a> and 
<a href="http://winmerge.org/developer/translate.php" target="_top">translate WinMerge</a>
and get some hints for <a href="http://winmerge.org/developer/wincvs.php" target="_top">WinCVS</a>.</p>
<?php
  $page->printFoot();
?>