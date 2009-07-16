<?php
  include('../engine/engine.inc');

  $page = new Page;
  $page->setDescription('Download the source code of WinMerge, which is released under the GNU General Public License.');
  $page->setKeywords('WinMerge, free, download, source code, GPL, Subversion, SVN, TortoiseSVN');
  $page->printHead('Download Source Code', TAB_DOWNLOADS, 'toggle(\'checksumslist\');');
  $stablerelease = $page->getStableRelease();
?>
<h2>Download Source Code</h2>
<p>WinMerge is released under the <a href="http://www.gnu.org/licenses/gpl-2.0.html">GNU General Public License</a>. That means you can get the whole source code and can build the program yourself.</p>
<div class="downloadmatrix">
<ul>
  <li><strong>WinMerge <?php echo $stablerelease->getVersionNumber();?> - Source Code</strong>
    <ul>
      <li><a href="<?php echo $stablerelease->getDownload('src.zip');?>">Zip-Format (<?php echo $stablerelease->getDownloadSizeMb('src.zip');?> MB)</a></li>
      <li><a href="<?php echo $stablerelease->getDownload('src.7z');?>">7z-Format (<?php echo $stablerelease->getDownloadSizeMb('src.7z');?> MB)</a></li>
    </ul>
  </li>
</ul>
</div> <!-- .downloadmatrix -->
<div id="checksums">
  <h3><a href="javascript:toggle('checksumslist')">SHA-1 Checksums</a></h3>
<pre id="checksumslist">
<?php
  echo $stablerelease->getDownloadSha1Sum('src.zip') . ' ' . $stablerelease->getDownloadFileName('src.zip') . "\n";
  echo $stablerelease->getDownloadSha1Sum('src.7z') . ' ' . $stablerelease->getDownloadFileName('src.7z') . "\n";
?>
</pre>
</div> <!-- #checksums -->
<p>The source code is hosted on <a href="http://sourceforge.net/">SourceForge.net</a> in a <a href="http://sourceforge.net/svn/?group_id=13216">Subversion</a> repository.</p>
<p>You can <a href="http://winmerge.svn.sourceforge.net/viewvc/winmerge/">browse the source code</a> with a web browser or you can check out the whole code by clicking on one of the following links (if you have <a href="http://tortoisesvn.net/">TortoiseSVN</a> installed):</p>
<dl class="headinglist">
  <dt>Developer Version</dt>
  <dd><a href="tsvn:https://winmerge.svn.sourceforge.net/svnroot/winmerge/trunk">https://winmerge.svn.sourceforge.net/svnroot/winmerge/trunk</a></dd>
  <dt>WinMerge <?php echo $stablerelease->getVersionNumberMajor();?></dt>
  <dd><a href="tsvn:https://winmerge.svn.sourceforge.net/svnroot/winmerge/branches/<?php echo $stablerelease->getBranchName();?>">https://winmerge.svn.sourceforge.net/svnroot/winmerge/branches/<?php echo $stablerelease->getBranchName();?></a></dd>
</dl>
<?php
  $page->printFoot();
?>