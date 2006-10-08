<?php
  include('../page.inc');

  $page = new Page;
  $page->printHead('WinMerge: Developers Needed!', WINMERGE_DEV);
?>
<h2>Help Needed!</h2>
<p>WinMerge is an <a href="http://en.wikipedia.org/wiki/Open_source">Open Source Software</a>.
This means everybody can download WinMerge source code, including documentation,
web pages etc. And improve and modify them. Only thing we ask is that people
submit their improvements and modifications back to us. This is the way the
WinMerge gets improved.</p>

<p>There is a small group of active developers, as listed in <a href="http://winmerge.org/developer/index.php">
Developers-page</a>. Unfortunately WinMerge has grown over years and requires
more and more work to be done. So we are constantly hoping new people to step
up and start working with us to improve WinMerge.</p>

<p>Coding is not the only thing we need more people for, you can:</p>
<ul>
  <li>write code</li>
  <li>test</li>
  <li>write documentation (user- and developer-)</li>
  <li>translate WinMerge</li>
  <li>improve installer</li>
</ul>

<p>WinMerge is mostly C/C++ code and is compiled with Visual Studio (VC6 and
later work) and GUI code uses MFC heavily. However, C/C++ wizardry or deep
knowledge of MFC is not required to work with WinMerge. There is a lot of low
level code for file handling, unicode etc.</p>

<p>Our tracker items are good starting point for looking things to do:
<a href="http://sourceforge.net/tracker/?group_id=13216&amp;atid=113216">Bugs</a>,
<a href="http://sourceforge.net/tracker/?group_id=13216&amp;atid=363216">Feature Requests</a> and
<a href="http://sourceforge.net/tracker/?group_id=13216&amp;atid=759426">Todo</a>
lists. If you see something in there that you think
you can handle, let us know (see contacting developers below) or leave a
comment at the item. If you want to do something not in those lists, please
submit it as a new item and/or let us know about it other ways (see below).</p>

<p>The best way to contact developers is posting a message to
<a href="http://sourceforge.net/forum/forum.php?forum_id=41639">Developers-forum</a>
or to winmerge-development <a href="http://sourceforge.net/mail/?group_id=13216">
mailing list</a>. We try to answer as soon as possible, but sometimes it may take
even days to anybody to answer. Be patient.</p>

<p>When you have decided to do something, grab a copy of the WinMerge source from
<a href="http://sourceforge.net/svn/?group_id=13216">Subversion</a>
or you can download a <a href="http://sourceforge.net/project/showfiles.php?group_id=13216">zipped version</a>
of the source tree, do your changes, and then upload the changes
<a href="http://sourceforge.net/tracker/?group_id=13216&amp;atid=313216">as a patch</a>
(we prefer the original and changed codes in one zip file). Then we will commit
them into the subversion repository.</p>

<p>You find more informations about programming WinMerge in the Subversion directory
<a href="http://svn.sourceforge.net/viewvc/winmerge/trunk/Docs/Developers/">Docs/Developers/</a>.
We have also started a <a href="http://wiki.winmerge.org">developer-wiki</a> for WinMerge.
Our plan is to use the wiki for planning and documenting WinMerge internals.</p>

<p>The general plan is to start people with anonymous Subversion access and let the regular developers commit
their changes to the Subversion repository. Those developers that
continue to contribute will be considered for write Subversion access.</p>
<?php
  $page->printFoot();
?>