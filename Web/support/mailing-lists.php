<?php
  include('../page.inc');

  $page = new Page;
  $page->printHead('WinMerge: Mailing lists', TAB_SUPPORT);
?>
<h2>Mailing lists</h2>
<ul>
  <li><a href="#announce">Announce List</a></li>
  <li><a href="#support">Support List</a></li>
  <li><a href="#user">User List</a></li>
  <li><a href="#translate">Translate List</a></li>
  <li><a href="#development">Development List</a></li>
  <li><a href="#svn">SVN List</a></li>
</ul>
<p>You should read the "<a href="http://sourceforge.net/docman/display_doc.php?docid=12983&amp;group_id=1">Guide to SourceForge.net Mailing List Services</a>" before subscribing and posting to any of the lists below.</p>
<h3><a name="announce">Announce List</a></h3>
<ul>
  <li><strong>Low Traffic</strong></li>
  <li><a href="http://lists.sourceforge.net/mailman/listinfo/winmerge-announce">Subscribe</a></li>
  <li><a href="http://lists.sourceforge.net/mailman/listinfo/winmerge-announce">Unsubscribe</a></li>
  <li><a href="http://sourceforge.net/mailarchive/forum.php?forum_name=winmerge-announce">Archive</a></li>
</ul>
<p>This list is only for announcing new releases of WinMerge.</p>
<h3><a name="support">Support List</a></h3>
<ul>
  <li><strong>Normal Traffic</strong></li>
  <li><a href="http://lists.sourceforge.net/mailman/listinfo/winmerge-support">Subscribe</a></li>
  <li><a href="http://lists.sourceforge.net/mailman/listinfo/winmerge-support">Unsubscribe</a></li>
  <li><a href="http://sourceforge.net/mailarchive/forum.php?forum_name=winmerge-support">Archive</a></li>
</ul>
<p>You should ask all your support questions on this list.</p>
<h3><a name="user">User List</a></h3>
<ul>
  <li><strong>Normal Traffic</strong></li>
  <li><a href="http://lists.sourceforge.net/mailman/listinfo/winmerge-user">Subscribe</a></li>
  <li><a href="http://lists.sourceforge.net/mailman/listinfo/winmerge-user">Unsubscribe</a></li>
  <li><a href="http://sourceforge.net/mailarchive/forum.php?forum_name=winmerge-user">Archive</a></li>
</ul>
<p>This list is for user related questions. Having questions posted to this list may allow others having the same problem to solve it on their own.</p>
<h3><a name="translate">Translate List</a></h3>
<ul>
  <li><strong>Low Traffic</strong></li>
  <li><a href="http://lists.sourceforge.net/mailman/listinfo/winmerge-translate">Subscribe</a></li>
  <li><a href="http://lists.sourceforge.net/mailman/listinfo/winmerge-translate">Unsubscribe</a></li>
  <li><a href="http://sourceforge.net/mailarchive/forum.php?forum_name=winmerge-translate">Archive</a></li>
</ul>
<p>This list is used for coordinating the translations.</p>
<h3><a name="development">Development List</a></h3>
<ul>
  <li><strong>Normal Traffic</strong></li>
  <li><a href="http://lists.sourceforge.net/mailman/listinfo/winmerge-development">Subscribe</a></li>
  <li><a href="http://lists.sourceforge.net/mailman/listinfo/winmerge-development">Unsubscribe</a></li>
  <li><a href="http://sourceforge.net/mailarchive/forum.php?forum_name=winmerge-development">Archive</a></li>
</ul>
<p>This is the list where participating developers of the WinMerge meet and discuss issues, code changes/additions, etc.</p>
<h3><a name="svn">SVN List</a></h3>
<ul>
  <li><strong>High Traffic</strong></li>
  <li><a href="http://lists.sourceforge.net/mailman/listinfo/winmerge-svn">Subscribe</a></li>
  <li><a href="http://lists.sourceforge.net/mailman/listinfo/winmerge-svn">Unsubscribe</a></li>
  <li><a href="http://sourceforge.net/mailarchive/forum.php?forum_name=winmerge-svn">Archive</a></li>
</ul>
<p>Subscribers to this list get notices of each and every code change commited to Subversion.</p>
<?php
  $page->printFoot();
?>