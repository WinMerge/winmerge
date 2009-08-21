<?php
  include('../engine/engine.inc');

  $page = new Page;
  $page->setDescription(__('Subscribe or unsubscribe to one of the following mailing lists: Announce, Support, User, Translate, Development and SVN'));
  $page->setKeywords(__('WinMerge, mailing lists, announce, support, user, translate, development, SVN'));
  $page->printHead(__('Mailing Lists'), TAB_SUPPORT);
  
  $page->printHeading(__('Mailing Lists'));
?>
<ul>
  <li><a href="#announce"><?php __e('Announce List');?></a></li>
  <li><a href="#support"><?php __e('Support List');?></a></li>
  <li><a href="#user"><?php __e('User List');?></a></li>
  <li><a href="#translate"><?php __e('Translate List');?></a></li>
  <li><a href="#development"><?php __e('Development List');?></a></li>
  <li><a href="#svn"><?php __e('SVN List');?></a></li>
</ul>
<div class="infocard">
  <h3><a name="announce"><?php __e('Announce List');?></a></h3>
  <ul>
    <li><strong><?php __e('Low Traffic');?></strong></li>
    <li><a href="http://lists.sourceforge.net/mailman/listinfo/winmerge-announce"><?php __e('Subscribe');?></a></li>
    <li><a href="http://lists.sourceforge.net/mailman/listinfo/winmerge-announce"><?php __e('Unsubscribe');?></a></li>
    <li><a href="http://sourceforge.net/mailarchive/forum.php?forum_name=winmerge-announce"><?php __e('Archive');?></a></li>
  </ul>
  <p><?php __e('This list is only for announcing new releases of WinMerge.');?></p>
</div>
<div class="infocard">
  <h3><a name="support"><?php __e('Support List');?></a></h3>
  <ul>
    <li><strong><?php __e('Normal Traffic');?></strong></li>
    <li><a href="http://lists.sourceforge.net/mailman/listinfo/winmerge-support"><?php __e('Subscribe');?></a></li>
    <li><a href="http://lists.sourceforge.net/mailman/listinfo/winmerge-support"><?php __e('Unsubscribe');?></a></li>
    <li><a href="http://sourceforge.net/mailarchive/forum.php?forum_name=winmerge-support"><?php __e('Archive');?></a></li>
  </ul>
  <p><?php __e('You should ask all your support questions on this list.');?></p>
</div>
<div class="infocard">
  <h3><a name="user"><?php __e('User List');?></a></h3>
  <ul>
    <li><strong><?php __e('Normal Traffic');?></strong></li>
    <li><a href="http://lists.sourceforge.net/mailman/listinfo/winmerge-user"><?php __e('Subscribe');?></a></li>
    <li><a href="http://lists.sourceforge.net/mailman/listinfo/winmerge-user"><?php __e('Unsubscribe');?></a></li>
    <li><a href="http://sourceforge.net/mailarchive/forum.php?forum_name=winmerge-user"><?php __e('Archive');?></a></li>
  </ul>
  <p><?php __e('This list is for user related questions. Having questions posted to this list may allow others having the same problem to solve it on their own.');?></p>
</div>
<div class="infocard">
  <h3><a name="translate"><?php __e('Translate List');?></a></h3>
  <ul>
    <li><strong><?php __e('Low Traffic');?></strong></li>
    <li><a href="http://lists.sourceforge.net/mailman/listinfo/winmerge-translate"><?php __e('Subscribe');?></a></li>
    <li><a href="http://lists.sourceforge.net/mailman/listinfo/winmerge-translate"><?php __e('Unsubscribe');?></a></li>
    <li><a href="http://sourceforge.net/mailarchive/forum.php?forum_name=winmerge-translate"><?php __e('Archive');?></a></li>
  </ul>
  <p><?php __e('This list is used for coordinating the translations.');?></p>
</div>
<div class="infocard">
  <h3><a name="development"><?php __e('Development List');?></a></h3>
  <ul>
    <li><strong><?php __e('Normal Traffic');?></strong></li>
    <li><a href="http://lists.sourceforge.net/mailman/listinfo/winmerge-development"><?php __e('Subscribe');?></a></li>
    <li><a href="http://lists.sourceforge.net/mailman/listinfo/winmerge-development"><?php __e('Unsubscribe');?></a></li>
    <li><a href="http://sourceforge.net/mailarchive/forum.php?forum_name=winmerge-development"><?php __e('Archive');?></a></li>
  </ul>
  <p><?php __e('This is the list where participating developers of the WinMerge meet and discuss issues, code changes/additions, etc.');?></p>
</div>
<div class="infocard">
  <h3><a name="svn"><?php __e('SVN List');?></a></h3>
  <ul>
    <li><strong><?php __e('High Traffic');?></strong></li>
    <li><a href="http://lists.sourceforge.net/mailman/listinfo/winmerge-svn"><?php __e('Subscribe');?></a></li>
    <li><a href="http://lists.sourceforge.net/mailman/listinfo/winmerge-svn"><?php __e('Unsubscribe');?></a></li>
    <li><a href="http://sourceforge.net/mailarchive/forum.php?forum_name=winmerge-svn"><?php __e('Archive');?></a></li>
  </ul>
  <p><?php __e('Subscribers to this list get notices of each and every code change commited to Subversion.');?></p>
</div>
<?php
  $page->printFoot();
?>