<?php
  include('../page.inc');
  include('../engine/simplepie/simplepie.inc');

  $page = new Page;
  $page->setDescription('The Open Discussion forum is the fastest way to get help. A other way is a subscription to the support mailing list.');
  $page->setKeywords('WinMerge, support, discussion forums, mailing lists, tracker, donate, bugs, support requests, patches, feature requests, todo, open source, sourceforge.net');
  $page->printHead('WinMerge: Support', TAB_SUPPORT);
?>
<h2>Support</h2>
<p>The <a href="http://apps.sourceforge.net/phpbb/winmerge/viewforum.php?f=4">Open
Discussion forum</a> is the fastest way to get help. Please be patient, it may take
some time for somebody to answer. A other way is a subscription to the
<a href="mailing-lists.php#support">support mailing list</a>.</p>

<p>If you find a bug, please submit it as a <a href="http://sourceforge.net/tracker/?group_id=13216&amp;atid=113216">bug
report</a>. Please attach as much information as you can: at a minimum,
the version number of WinMerge that you are using. If you can, also attach a
configuration log which, you can display by clicking <span class="guimenu">Help</span>
&#8594; <span class="guimenuitem">Configuration</span> in the WinMerge window.
Good information in a bug report makes it more likely that your bug will be fixed quickly.</p>

<p>You must <a href="https://sourceforge.net/account/registration/">register
with SourceForge.net</a> before posting a bug report (registration is free).
We require registering because anonymous submissions caused a lot of spam and
also because there were no possibility to contact people for asking more
information. We rarely sent direct emails but you'll get notifications when we
ask questions in the bug item.</p>

<p>Wish list items on the <a href="http://sourceforge.net/tracker/?group_id=13216&amp;atid=363216">feature
request list</a> will also be considered, but we make absolutely no promises.</p>

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
<p>Since WinMerge is an <a href="/about/license.php">Open Source</a> project, you may use it free of charge.
But please consider making a <a href="http://sourceforge.net/project/project_donations.php?group_id=13216">donation</a>
to support the continued development of WinMerge.</p>
<?php
  $page->printFoot();
?>