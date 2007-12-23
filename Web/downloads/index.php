<?php
  include('../page.inc');

  $page = new Page;
  $page->printHead('WinMerge: Downloads', TAB_DOWNLOADS);
  $stablerelease = $page->getStableRelease();
?>
<h2>Downloads</h2>
<ul>
  <li><a href="#current">Current Version</a></li>
  <li><a href="#other">Other Versions</a></li>
  <li><a href="#plugins">Plugins</a></li>
  <li><a href="#sourcecode">Source Code</a></li>
</ul>
<h3><a name="current">Current Version</a></h3>
<?php $page->printDownloadNow(); ?>
<p>The current version is <strong>2.6.12</strong> and was released at <strong>2007-09-26</strong>. For detailed info on what's new, read the <a href="/2.6/changes.php">changelog</a> and the <a href="http://sourceforge.net/project/shownotes.php?release_id=542344">release notes</a>.</p>
<div class="downloadmatrix">
<ul>
  <li><strong>Installer</strong>
    <ul>
      <li><a href="<?php echo $stablerelease->getSetupDownload();?>">Exe-Format (<?php echo $stablerelease->getSetupSizeFormatted('mb');?> MB)</a></li>
    </ul>
  </li>
  <li><strong>Binaries</strong>
    <ul>
      <li><a href="http://prdownloads.sourceforge.net/winmerge/WinMerge-2.6.12-exe.zip">Zip-Format (5.18 MB)</a></li>
      <li><a href="http://prdownloads.sourceforge.net/winmerge/WinMerge-2.6.12-exe.7z">7z-Format (1.68 MB)</a></li>
    </ul>
  </li>
  <li><strong>Runtimes</strong>
    <ul>
      <li><a href="http://prdownloads.sourceforge.net/winmerge/Runtimes-2.6.12.zip">Zip-Format (1.23 MB)</a></li>
      <li><a href="http://prdownloads.sourceforge.net/winmerge/Runtimes-2.6.12.7z">7z-Format (0.72 MB)</a></li>
    </ul>
  </li>
  <li><strong>Source Code</strong>
    <ul>
      <li><a href="http://prdownloads.sourceforge.net/winmerge/WinMerge-2.6.12-src.zip">Zip-Format (5.06 MB)</a></li>
      <li><a href="http://prdownloads.sourceforge.net/winmerge/WinMerge-2.6.12-src.7z">7z-Format (2.79 MB)</a></li>
    </ul>
  </li>
</ul>
</div>
<p>The easiest way to install WinMerge is to download and run the Installer. Read the <a href="2.6/manual/Installing.html">online manual</a> for help using it.</p>
<h3><a name="other">Other Versions</a></h3>
<ul>
  <li><a href="http://sourceforge.net/project/showfiles.php?group_id=13216&amp;package_id=11248">Stable Versions</a></li>
  <li><a href="http://sourceforge.net/project/showfiles.php?group_id=13216&amp;package_id=29158">Beta Versions</a></li>
  <li><a href="http://sourceforge.net/project/showfiles.php?group_id=13216&amp;package_id=92246">Experimental Builds</a></li>
</ul>
<h3><a name="plugins">Plugins</a></h3>
<h4>7-Zip Plugin</h4>
<p>This is the 7-Zip plugin for WinMerge and the current version <strong>DllBuild 0026</strong> was released at <strong>2007-12-23</strong>.</p>
<div class="downloadmatrix">
<ul>
  <li><strong>Installer</strong>
    <ul>
      <li><a href="http://prdownloads.sourceforge.net/winmerge/Merge7zInstaller0026-432-457.exe">Exe-Format (1.05 MB)</a></li>
    </ul>
  </li>
  <li><strong>Binaries</strong>
    <ul>
      <li><a href="http://prdownloads.sourceforge.net/winmerge/Merge7z0026-432-457.7z">7z-Format (0.91 MB)</a></li>
    </ul>
  </li>
</ul>
</div>
<p>You can also read the <a href="http://sourceforge.net/project/shownotes.php?release_id=563695&amp;group_id=13216">Release Notes</a> or search for <a href="http://sourceforge.net/project/showfiles.php?group_id=13216&amp;package_id=143957">other versions</a>.</p>
<h3><a name="sourcecode">Source Code</a></h3>
<p>WinMerge is released under the <a href="http://www.gnu.org/licenses/gpl-2.0.html">GNU General Public License</a>. That means you can get the whole source code and can build the program yourself.<br />
The source code is hosted on <a href="http://sourceforge.net/">SourceForge.net</a> in a <a href="http://sourceforge.net/svn/?group_id=13216">Subversion</a> repository.<br />
You can <a href="http://winmerge.svn.sourceforge.net/viewvc/winmerge/">browse the source code</a> with a web browser or you can check out the whole code by clicking on one of the following links (if you have <a href="http://tortoisesvn.net/">TortoiseSVN</a> installed):</p>
<dl class="headinglist">
  <dt>Developer Version</dt>
  <dd><a href="tsvn:https://winmerge.svn.sourceforge.net/svnroot/winmerge/trunk">https://winmerge.svn.sourceforge.net/svnroot/winmerge/trunk</a></dd>
  <dt>WinMerge 2.6</dt>
  <dd><a href="tsvn:https://winmerge.svn.sourceforge.net/svnroot/winmerge/branches/R2_6">https://winmerge.svn.sourceforge.net/svnroot/winmerge/branches/R2_6</a></dd>
</dl>
<?php
  $page->printFoot();
?>