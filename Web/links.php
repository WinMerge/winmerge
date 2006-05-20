<?php
  include('page.inc');

  $page = new Page;
  $page->printHead('WinMerge: Links', WINMERGE_USR);
?>
<h2>Links</h2>
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
</dl>
<h3>CVS</h3>
<dl>
  <dt><a href="http://cvsgui.sourceforge.net/">WinCVS</a></dt>
  <dd><b>WinCVS</b> is a GUI front-end for <acronym title="Concurrent Versions System">CVS</acronym>.</dd>
  <dt><a href="http://www.tortoisecvs.org/">TortoiseCVS</a></dt>
  <dd><b>TortoiseCVS</b> lets you work with files under <acronym title="Concurrent Versions System">CVS</acronym> version control directly from Windows Explorer.</dd>
</dl>
<h3>Regular expressions</h3>
<dl>
  <dt><a href="http://www.regular-expressions.info/">regular-expressions.info</a></dt>
  <dd>Good site devoted for regular expressions</dd>
  <dt><a href="http://www.amk.ca/python/howto/regex/">regex HOWTO</a></dt>
  <dd>Good HOWTO document for regular expressions</dd>
  <dt><a href="http://www.zvon.org/other/PerlTutorial/Output/index.html">Tutorial</a></dt>
  <dd>Regular expressions tutorial</dd>
  <dt><a href="http://gmckinney.info/resources/regex.pdf">regex.pdf</a></dt>
  <dd>PDF quick reference</dd>
</dl>
<?php
  $page->printFoot();
?>