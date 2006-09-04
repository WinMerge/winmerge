<?php
  include('../page.inc');

  $page = new Page;
  $page->printHead('WinMerge: Developers Needed!', WINMERGE_DEV);
?>
<h2>Developers Needed!</h2>
<p>One of the reasons WinMerge is on <a href="http://sourceforge.net/projects/winmerge">SourceForge</a> is so other
developers can get involved. If you have some free time, are
proficient with MS Visual Studio and MFC, and would like to contribute,
then check out the list of items to be done from the
<a href="http://sourceforge.net/tracker/?group_id=13216&amp;atid=113216">Bugs</a>,
<a href="http://sourceforge.net/tracker/?group_id=13216&amp;atid=363216">Feature Requests</a> and
<a href="http://sourceforge.net/tracker/?group_id=13216&amp;atid=759426">Todo</a>
lists.
If you see something in there that you think
you can handle, <a href="http://sourceforge.net/forum/forum.php?forum_id=41639">let us know</a>
or leave a comment at the item.</p>

<p>Then grab a copy of the WinMerge source from
<a href="http://sourceforge.net/svn/?group_id=13216">Subversion</a>
or you can download a <a href="http://sourceforge.net/project/showfiles.php?group_id=13216">zipped version</a> of the source,
write your code, and then upload the changed code <a href="http://sourceforge.net/tracker/?group_id=13216&amp;atid=313216">as a patch</a>
(we prefer the original and changed codes in one zip file).
Then we will merge them back into the main codebase.</p>

<p>You find more informations about programming WinMerge in the Subversion directory
<a href="http://svn.sourceforge.net/viewvc/winmerge/trunk/Docs/Developers/">Docs/Developers/</a>.</p>

<p>The general plan is to start people with anonymous Subversion access and let the regular developers commit
their changes to the Subversion repository. Those developers that
continue to contribute will be considered for write Subversion access.</p>
<?php
  $page->printFoot();
?>