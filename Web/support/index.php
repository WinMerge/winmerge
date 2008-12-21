<?php
  include('../page.inc');
  include('../engine/simplepie/simplepie.inc');

  $page = new Page;
  $page->printHead('WinMerge: Support', TAB_SUPPORT);
?>
<h2>Support</h2>
<p>WinMerge is an Open Source project, that is covered
under the <a href="http://www.gnu.org/licenses/gpl-2.0.html">GNU General Public License</a>.
As such, your main form of support for WinMerge will be through
subscription to the <a href="http://lists.sourceforge.net/lists/listinfo/winmerge-support">WinMerge-support mailing list</a>.
In addition, SourceForge has bug tracking capabilities, so please report any
bugs <a href="http://sourceforge.net/bugs/?group_id=13216">here</a>. Wish list items on the
<a href="http://sourceforge.net/tracker/?group_id=13216&amp;atid=363216">feature request list</a> will also be considered, but we make
absolutely no promises.</p>
<p>We prefer that you <a href="https://sourceforge.net/account/registration/">register</a>
with <a href="http://sourceforge.net/">SourceForge</a> before posting a bug report (registration is
public and free), so that you may get email notifications if we post follow-up questions to your bug.
It is not at this time required that you register to post a bug report (although, we do generally
give less priority to unregistered bug reports, because it has been our experience that unregistered
posters often do not remember to return and check to answer follow-on questions).</p>
<h3>Tracker Statistics</h3>
<?php
  $feed = new SimplePie();
  $feed->set_feed_url('http://sourceforge.net/export/rss2_projsummary.php?group_id=13216');
  $feed->set_cache_location('../engine/simplepie/cache');
  $feed->enable_order_by_date(false);
  $feed->init();
  print("<ul class=\"rssfeeditems\">\n");
  foreach ($feed->get_items() as $item) { //for all project summary items...
    $title = $item->get_title();
    if (stristr($title, "Tracker:")) {
      $title = str_replace('Tracker: ', '', $title);
      print("  <li><a href=\"".$item->get_link()."\">".$title."</a></li>\n");
    }
  }
  print("</ul>\n");
?>
<h3>Donate</h3>
<p>Since WinMerge is an Open Source project, you may use it free of charge.
But please consider making a <a href="http://sourceforge.net/project/project_donations.php?group_id=13216">donation</a>
to support the continued development of WinMerge.</p>
<?php
  $page->printFoot();
?>