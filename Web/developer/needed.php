<?php
  include('../page.inc');

  $page = new Page;
  $page->printHead('WinMerge: Developers Needed!', WINMERGE_DEV);
?>
<h2>Developers Needed!</h2>
<p>One of the reasons WinMerge is on <a href="http://sourceforge.net/projects/winmerge">SourceForge</a> is so other
developers can get involved. If you have some free time, are
proficient with DeveloperStudio 6 and MFC, and would like to contribute,
then check out the list of items to be done from the
<a href="http://sourceforge.net/tracker/?group_id=13216&amp;atid=113216">Bugs</a> and
<a href="http://sourceforge.net/tracker/?group_id=13216&amp;atid=363216">RFE</a> lists.
If you see something in there that you think
you can handle, <a href="http://sourceforge.net/forum/forum.php?forum_id=41639">let us know</a>
or leave a comment at the item.</p>

<p>Then grab a copy of the WinMerge source from
<a href="http://sourceforge.net/cvs/?group_id=13216">anonymous CVS</a>
or you can download a <a href="http://sourceforge.net/project/showfiles.php?group_id=13216">zipped version</a> of the source,
write your code, and then upload the changed code <a href="http://sourceforge.net/tracker/?group_id=13216&amp;atid=313216">as a patch</a>
(we prefer the original and changed codes in one zip file).
Then we will merge them back into the main codebase.</p>

<p>If you do decide to join in, please follow these simple guidelines,
which will ultimately make all our lives easier:</p>

<ul>
  <li>Please consider that WinMerge is translated into
    multiple languages, so it is always better to keep UI changes to a
    minimum.</li>
  <li><b>Do not</b> make trivial
    changes to existing code, such as deleting blanks, consolidating
    multi-line comments into single line comments, etc. This just adds
    a bunch of needless differences to the code. Besides, if every
    developer massaged the code to fit his/her style, it would be a
    never-ending struggle.</li>
  <li>Please try to match existing coding style as
    much as possible. Note that WinMerge 'core' uses tabs, but <code>editor</code>
    and <code>diffutils</code> code uses spaces for indents.</li>
  <li>Please make sure to comment your work so we can
    all understand what you've done.</li>
  <li>Please don't make changes that don't relate to
    the task at hand. If you come across other issues while handling a
    task, please submit them as bugs or patches.</li>
  <li>Please don't make changes that aren't on the bug or RFE list;
    if everyone just threw in everything they wanted, the product would
    become a hodge-podge.</li>
</ul>

<p>You find more informations about programming WinMerge in the CVS directory
<a href="http://cvs.sourceforge.net/viewcvs.py/winmerge/WinMerge/Docs/Developers/">Docs/Developers/</a>.</p>

<p>The general plan is to start people with anonymous CVS access and let the regular developers commit
their changes to the CVS repository. Those developers that
continue to contribute will be considered for read-write CVS access.</p>
<?php
  $page->printFoot();
?>