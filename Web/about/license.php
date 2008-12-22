<?php
  include('../page.inc');

  $page = new Page;
  $page->printHead('WinMerge: License', TAB_ABOUT);
?>
<h2>License</h2>
<p>WinMerge is <a href="http://www.opensource.org/">Open Source</a> software under the <a href="http://www.gnu.org/licenses/gpl-2.0.html">GNU General Public License</a>.</p>
<p>This means everybody can download the <a href="/downloads/source-code.php">source code</a> and improve and modify them.
The only thing we ask is that people submit their improvements and modifications back to us so that all WinMerge users may benefit.</p>

<h3>GNU General Public License</h3>
<pre>WinMerge is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

WinMerge is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with WinMerge.  If not, see &lt;<a href="http://www.gnu.org/licenses/">http://www.gnu.org/licenses/</a>&gt;.</pre>

<h3>Links</h3>
<ul>
  <li><a href="http://www.opensource.org/">The Open Source Definition</a></li>
  <li><a href="http://www.gnu.org/licenses/gpl-2.0.html">GNU General Public License v2.0</a></li>
  <li><a href="http://en.wikipedia.org/wiki/Open_source_software">Open Source Software</a></li>
</ul>
<?php
  $page->printFoot();
?>