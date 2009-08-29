<?php
  include('../engine/engine.inc');

  $page = new Page;
  $page->setDescription(__('WinMerge is Open Source software under the GNU General Public License. This means everybody can download the source code and improve and modify it.'));
  $page->setKeywords(__('WinMerge, license, open source, GNU General Public License, GPL, free software'));
  $page->printHead(__('License'), TAB_ABOUT);
  
  $page->printHeading(__('License'));
  $page->printPara(__('WinMerge is <a href="%1$s">Open Source</a> software under the <a href="%2$s">GNU General Public License</a>.', 'http://www.opensource.org/', 'http://www.gnu.org/licenses/gpl-2.0.html'));
  $page->printPara(__('This means everybody can download the <a href="%s">source code</a> and improve and modify it.
The only thing we ask is that people submit their improvements and modifications back to us so that all WinMerge users may benefit.', '/downloads/source-code.php'));
  
  $page->printSubHeading(__('GNU General Public License'));
?>
<pre lang="en">WinMerge is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

WinMerge is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with WinMerge.  If not, see &lt;<a href="http://www.gnu.org/licenses/">http://www.gnu.org/licenses/</a>&gt;.</pre>

<?php
  $page->printSubHeading(__('Links'));
?>
<ul>
  <li><a href="http://www.opensource.org/"><?php __e('The Open Source Definition');?></a></li>
  <li><a href="http://www.gnu.org/licenses/gpl-2.0.html"><?php __e('GNU General Public License v2.0');?></a></li>
  <li><a href="http://en.wikipedia.org/wiki/Open_source_software"><?php __e('Open Source Software');?></a></li>
</ul>
<?php
  $page->printFoot();
?>