<?php
  include('page.inc');

  $page = new Page;
  $page->printHead('WinMerge', TAB_HOME);
  $stablerelease = $page->getStableRelease();
?>
<div class="whatisbox">
  <h2>What is WinMerge?</h2>
  <p class="whatisimage">
    <img class="thumbnail" src="about/screenshots/screenshot_small.png" alt="File Comparison" height="150" border="0"><br />
    <a href="about/screenshots/">More Screenshots...</a>
  </p>
  <div class="whatistext">
    <p>WinMerge is an Open Source (<a href="http://www.gnu.org/licenses/gpl-2.0.html">GPL</a>) visual text file differencing and merging tool for Windows. It is highly useful for determing what has changed between project versions, and then merging changes between versions.</p>
    <p><strong><a href="about/">Learn More</a> or <a href="downloads/">Download Now!</a></strong></p>
  </div>
  <br class="whatisclear" />
</div>

<h3>WinMerge <?php echo $stablerelease->getVersionNumber();?> - latest stable version</h3>
<p><a href="2.8/index.php">WinMerge <?php echo $stablerelease->getVersionNumber();?></a> is the latest stable version, and is recommended for most users.</p>
<p><a href="<?php echo $stablerelease->getDownload('setup.exe');?>">Download WinMerge <?php echo $stablerelease->getVersionNumber();?> Installer</a></p>

<h3>Support</h3>
<p>If you need support, look at our <a href="support/index.php">support page</a> for more information how you can get it.</p>

<h3>Developers</h3>
<p>WinMerge is an open source project, which means that the program is maintained and developed by
volunteers. If you''re interested in contributing work to the project, see our <a href="http://winmerge.org/Wiki/index.php?title=Help_Needed">developer page</a>
for further information on how you may contribute.</p>
<p>In addition, WinMerge is translated into a number of different languages. See our <a href="/translations">information on translating WinMerge</a> into your own language.</p>
<p><a href="http://winmerge.org/Wiki/index.php?title=Developers">The WinMerge Development Team</a></p>
<?php
  $page->printFoot();
?>