<?php
  include('page.inc');

  $page = new Page;
  $page->printHead('WinMerge: Links', WINMERGE_USR);
?>
<h2>Links</h2>
<p>Here is a collection of links to tools and reference material that we find is generally useful when you work with WinMerge.</p>
<h3>Tools</h3>
<dl>
  <dt><a href="http://www.7-zip.org/">7-Zip</a></dt>
  <dd><b>7-Zip</b> is a file archiver with high compression ratio.</dd>
  <dt><a href="http://winscp.sourceforge.net/">WinSCP</a></dt>
  <dd><b>WinSCP</b> is an open source <acronym title="SSH File Transfer Protocol">SFTP</acronym> and <acronym title="Secure Copy Protocol">SCP</acronym> client for Windows using <acronym title="Secure Shell">SSH</acronym>.</dd>
  <dt><a href="http://gnuwin32.sourceforge.net/">GnuWin32</a></dt>
  <dd><b>GnuWin32</b> provides Win32 ports of tools with a <a href="http://www.gnu.org/">GNU</a> or similar <a href="http://opensource.org/licenses/">open source license</a>.</dd>
  <dt><a href="https://sourceforge.net/projects/externaldiff">External diff Tool</a></dt>
  <dd><b>External diff Tool</b> is an <a href="http://www.eclipse.org/">Eclipse</a> Plugin</a> that allows the launch of WinMerge (or an other external diff tool) for file compares rather than the default built-in Eclipse diff tool.</dd>
  <dt><a href="http://tortoisesvn.tigris.org/">TortoiseSVN</a></dt>
  <dd><b>TortoiseSVN</b> lets you work with files under <em>Subversion</em> version control directly from Windows Explorer.</dd>
</dl>
<h3>Regular expressions</h3>
<dl>
  <dt><a href="http://www.regular-expressions.info/">regular-expressions.info</a></dt>
  <dd>Good site devoted for regular expressions</dd>
  <dt><a href="http://www.amk.ca/python/howto/regex/">regex HOWTO</a></dt>
  <dd>Good HOWTO document for regular expressions</dd>
  <dt><a href="http://www.zvon.org/other/PerlTutorial/Output/index.html">Tutorial</a></dt>
  <dd>Regular expressions tutorial</dd>
  <dt><a href="http://www.night-ray.com/regex.pdf">regex.pdf</a></dt>
  <dd>PDF quick reference</dd>
</dl>
<h3>Miscellaneous</h3>
<dl>
  <dt><a href="http://ohloh.net/">ohloh.net</a></dt>
  <dd>Map the open source world by collecting objective information on open source projects. [<a href="http://www.ohloh.net/projects/225">WinMerge Report</a>]</dd>
  <dt><a href="http://www.diffutils.com/">diffutils.com</a></dt>
  <dd>Reviews of popular comparison software. [<a href="http://www.diffutils.com/project-revision-control/winmerge.html">WinMerge Review</a>]</dd>
  <dt><a href="http://www.betanews.com/">BetaNews.com</a></dt>
  <dd>Inside informations, unreleased products and articles on various soon-to-be-released software titles.</dd>
</dl>
<?php
  $page->printFoot();
?>