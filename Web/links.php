<?php
  include('engine/engine.inc');

  $page = new Page;
  $page->printHead('Links', TAB_HOME);
  $page->printHeading(__('Links'));
  $page->printPara(__('Here is a collection of links to tools and reference material that we find is generally useful when you work with WinMerge.'));
?>
<h3><?php __e('Tools');?></h3>
<dl>
  <dt><a href="http://www.7-zip.org/">7-Zip</a></dt>
  <dd><?php __e('<b>7-Zip</b> is a file archiver with high compression ratio.') ;?></dd>
  <dt><a href="http://winscp.sourceforge.net/">WinSCP</a></dt>
  <dd><?php __e('<b>WinSCP</b> is an open source <acronym title="SSH File Transfer Protocol">SFTP</acronym>, <acronym title="File Transfer Protocol">FTP</acronym> and <acronym title="Secure Copy Protocol">SCP</acronym> client for Windows.');?></dd>
  <dt><a href="http://gnuwin32.sourceforge.net/">GnuWin32</a></dt>
  <dd><?php __e('<b>GnuWin32</b> provides Win32 ports of tools with a <a href="%1$s">GNU</a> or similar <a href="%2$s">open source license</a>.', 'http://www.gnu.org/', 'http://opensource.org/licenses/');?></dd>
  <dt><a href="https://sourceforge.net/projects/externaldiff">External diff Tool</a></dt>
  <dd><?php __e('<b>External diff Tool</b> is an <a href="%s">Eclipse</a> Plugin</a> that allows the launch of WinMerge (or an other external diff tool) for file compares rather than the default built-in Eclipse diff tool.', 'http://www.eclipse.org/');?></dd>
  <dt><a href="http://tortoisesvn.tigris.org/">TortoiseSVN</a></dt>
  <dd><?php __e('<b>TortoiseSVN</b> lets you work with files under <em>Subversion</em> version control directly from Windows Explorer.');?></dd>
  <dt><a href="http://frhed.sourceforge.net/">Frhed</a></dt>
  <dd><?php __e('<b>Frhed</b> is an binary file editor for Windows. It is small but has many advanced features like ability to load big files partially.');?></dd>
</dl>
<h3><?php __e('Regular expressions');?></h3>
<dl>
  <dt><a href="http://www.regular-expressions.info/">regular-expressions.info</a></dt>
  <dd><?php __e('Good site devoted for regular expressions');?></dd>
  <dt><a href="http://www.amk.ca/python/howto/regex/"><?php __e('regex HOWTO');?></a></dt>
  <dd><?php __e('Good HOWTO document for regular expressions');?></dd>
  <dt><a href="http://www.zvon.org/other/PerlTutorial/Output/index.html"><?php __e('Tutorial');?></a></dt>
  <dd><?php __e('Regular expressions tutorial');?></dd>
  <dt><a href="http://www.night-ray.com/regex.pdf">regex.pdf</a></dt>
  <dd><?php __e('PDF quick reference');?></dd>
</dl>
<h3><?php __e('Miscellaneous');?></h3>
<dl>
  <dt><a href="http://ohloh.net/">ohloh.net</a></dt>
  <dd><?php __e('Map the open source world by collecting objective information on open source projects. [<a href="%s">WinMerge Report</a>]', 'http://www.ohloh.net/projects/225');?></dd>
  <dt><a href="http://www.diffutils.com/">diffutils.com</a></dt>
  <dd><?php __e('Reviews of popular comparison software. [<a href="%s">WinMerge Review</a>]', 'http://www.diffutils.com/project-revision-control/winmerge.html');?></dd>
  <dt><a href="http://www.betanews.com/">BetaNews.com</a></dt>
  <dd><?php __e('Inside informations, unreleased products and articles on various soon-to-be-released software titles.');?></dd>
  <dt><a href="http://portableapps.com/apps/utilities/winmerge_portable"><?php __e('WinMerge Portable');?></a></dt>
  <dd><?php __e('This is WinMerge packaged with a PortableApps.com Launcher as a <a href="%s">portable app</a>, so you can compare files on the go.', 'http://portableapps.com/about/what_is_a_portable_app');?></dd>
  <dt><a href="http://www.facebook.com/group.php?gid=13326170958"><?php __e('WinMerge Facebook Group');?></a></dt>
  <dd><?php __e('A <a href="%s">Facebook</a> group for people interested in WinMerge.', 'http://www.facebook.com/');?></dd>
</dl>
<?php
  $page->printFoot();
?>
