<?php
  include('../page.inc');

  $page = new Page;
  $page->printHead('WinMerge: Developers Needed!');
?>
<h2>Developers Needed!</h2>
<p>One of the reasons WinMerge is on SourceForge is so other
developers can get involved. If you have some free time, are
proficient with DeveloperStudio 6 and MFC, and would like to contribute,
then check out the list of items to be done from the
<a href="http://sourceforge.net/tracker/?group_id=13216&amp;atid=113216" target="_blank">Bugs</a> and
<a href="http://sourceforge.net/tracker/?group_id=13216&amp;atid=363216" target="_blank">RFE</a> lists.
If you see something in there that you think
you can handle, <a href="mailto:winmerge-support@lists.sourceforge.net">let us know</a>
or leave a comment at the item.</p>

<p>Then grab a copy of the WinMerge source from
<a href="http://sourceforge.net/cvs/?group_id=13216" target="_top">anonymous CVS</a>
or you can download a <a href="http://sourceforge.net/project/showfiles.php?group_id=13216" target="_top">zipped version</a> of the source,
write your code, and then upload the changed code <a href="http://sourceforge.net/tracker/?group_id=13216&amp;atid=313216" target="_top">as a patch</a>.
Then we will merge them back into the main codebase.</p>

<p>If you do decide to join in, please follow these simple guidelines,
which will ultimately make all our lives easier:</p>

<ul>
  <li class="toc">Please consider that WinMerge is translated into
    multiple languages, so it is always better to keep UI changes to a
    minimum. Any addition of text to the UI means we have to go back
    to the translators.</li>
  <li class="toc"><b>Do not</b> make trivial
    changes to existing code, such as deleting blanks, consolidating
    multi-line comments into single line comments, etc. This just adds
    a bunch of needless differences to the code. Besides, if every
    developer massaged the code to fit his/her style, it would be a
    never-ending struggle.</li>
  <li class="toc">Please try to match existing coding style as
    much as possible.</li>
  <li class="toc">Please make sure to comment your work so we can
    all understand what you've done (even though I didn't ;-) ).</li>
  <li class="toc">Please don't make changes that don't relate to
    the task at hand. If you come across other issues while handling a
    task, please submit them as bugs or patches.</li>
  <li class="toc">Please don't make changes that aren't on the bug or RFE list;
    if everyone just threw in everything they wanted, the product would
    become a hodge-podge.</li>
</ul>

<p>The general plan is to start people with anonymous CVS access and let the regular developers commit
their changes to the CVS repository. Those developers that
continue to contribute will be considered for read-write CVS access.</p>
<?php
  $page->printFoot();
?>