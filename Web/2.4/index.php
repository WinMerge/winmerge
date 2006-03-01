<?php
  include('../page.inc');

  $page = new Page;
  $page->printHead('WinMerge 2.4', WINMERGE_2_4);
?>
<p>WinMerge is an Open Source visual text file differencing and merging tool for Win32 platforms. It is highly useful for determing what has changed between project versions, and then merging changes between versions.</p>
<h2>Features</h2>
<ul>
  <li>Visual differencing and merging of text files</li>
  <li>Flexible editor with syntax highlighting</li>
  <li>Handles DOS, UNIX and MAC text file formats</li>
  <li>Unicode support</li>
  <li>Difference pane shows current difference in two vertical panes</li>
  <li>Location pane shows map of files compared</li>
  <li>Highlights differences inside lines</li>
  <li>File filters in directory diff</li>
  <li>Moved lines detection in file compare</li>
  <li>Shell Integration</li>
  <li>Rudimentary Visual SourceSafe and Rational ClearCase integration</li>
  <li>Archive file support using 7-zip</li>
  <li>Plugins</li>
  <li>Localizable interface via resource DLL</li>
  <li>HTML-based <a href="manual/index.html">Manual</a></li>
</ul>

<h2>Screenshot</h2>
<p><img src="images/screenshot.gif" alt="Screenshot of WinMerge"></p>
<p>See the <a href="screenshots.php">screenshots page</a> for more screenshots.</p>

<h2>Installation</h2>
<p>Just download the <a href="http://prdownloads.sourceforge.net/winmerge/WinMerge-2.4.6-Setup.exe">installer</a> and double-click it.</p>
<p>Or you may simply unzip the <a href="http://prdownloads.sourceforge.net/winmerge/WinMerge-2.4.6-exe.zip">binary zip package</a> into the desired location, and run
<code>WinMerge.exe</code> or <code>WinMergeU.exe</code> (the latter is the Unicode version, only for use on NT/2000/XP).</p>
<p>For more details please look at the <a href="manual/installing.html">Installation section</a> from the <a href="manual/index.html">Manual</a>.</p>

<h2>Using WinMerge</h2>
<p>You can use the <a href="manual/index.html">online Manual</a>, or you can
<a href="http://sourceforge.net/project/showfiles.php?group_id=13216&amp;package_id=11254&amp;release_id=397471">download</a> a copy.</p>

<h2>Support</h2>
<p>WinMerge is an Open Source project, that is covered
under the <a href="http://www.gnu.org/copyleft/gpl.html">GNU Public License</a>.
As such, your main form of support for WinMerge will be through
subscription to the <a href="http://lists.sourceforge.net/lists/listinfo/winmerge-support">WinMerge-support mailing list</a>.
In addition, SourceForge has bug tracking capabilities, so please report any
bugs <a href="http://sourceforge.net/bugs/?group_id=13216">here</a>. Wish list items on the
<a href="http://sourceforge.net/tracker/?group_id=13216&amp;atid=363216">RFE list</a> will also be considered, but we make
absolutely no promises.</p>
<p>We prefer that you <a href="http://sourceforge.net/account/newuser_emailverify.php">register</a>
with <a href="http://sourceforge.net/">SourceForge</a> before posting a bug report (registration is
public and free), so that you may get email notifications if we post follow-up questions to your bug.
It is not at this time required that you register to post a bug report (although, we do generally
give less priority to unregistered bug reports, because it has been our experience that unregistered
posters often do not remember to return and check to answer follow-on questions).</p>

<p>Here is the current <a href="changes.php">change history</a>.</p>

<p>Since WinMerge is an Open Source project, you may use it free of charge.
But please consider making a <a href="http://sourceforge.net/project/project_donations.php?group_id=13216">donation</a>
to support the continued development of WinMerge.</p>
<?php
  $page->printFoot();
?>