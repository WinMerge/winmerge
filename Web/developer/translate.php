<?php
  include('../page.inc');

  $page = new Page;
  $page->printHead('WinMerge: Translate WinMerge!');
?>
<h2>Localization</h2>
<p>If you are interested in translating WinMerge's resources to your own native language,
we'd be happy to provide you with the .RC file. All the resources are moved into a
DLL, so it should be fairly straightforward to support most languages.
<a href="mailto:winmerge-support@lists.sourceforge.net">Let us know</a> if you're interested.</p>

<p>We currently have WinMerge translated into the languages listed below.
To change languages, all you have to do is unzip the DLL in the same directory as WinMerge.exe,
then select View / Language and choose your language.</p>

<h3>Current translations of WinMerge</h3>
<ul>
  <li class="toc">Brazilian Portuguese translated by Wender Firmino</li>
  <li class="toc">Catalan translated by Jordi Vilar</li>
  <li class="toc">Simplified Chinese translated by Liaobin</li>
  <li class="toc">Traditional Chinese translated by Koko</li>
  <li class="toc">Czech translated by Jiri Tax</li>
  <li class="toc">Danish translated by Christian List</li>
  <li class="toc">Dutch translated by Ronald Schaap</li>
  <li class="toc">French translated by Gil Andre, continued by Laurent Ganier</li>
  <li class="toc">German translated by Winfried Peter, continued by Joerg Schneider and Tim Gerundt</li>
  <li class="toc">Italian translated by Michele Merega</li>
  <li class="toc">Korean translated by Sukjoon</li>
  <li class="toc">Norwegian translated by Hans Fredrik Nordhaug</li>
  <li class="toc">Polish translated by Pawe Wawrzyszko</li>
  <li class="toc">Russian translated by Dmitriy S. Aleshkowskiy</li>
  <li class="toc">Slovak translated by majvan</li>
  <li class="toc">Spanish translated by Dean Grimm</li>
  <li class="toc">Swedish translated by Göran Håkansson</li>
</ul>
<?php
  $page->printFoot();
?>