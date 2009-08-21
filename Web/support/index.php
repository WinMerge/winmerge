<?php
  include('../engine/engine.inc');
  include('../engine/simplepie/simplepie.inc');

  $page = new Page;
  $page->setDescription(__('The Open Discussion forum is the fastest way to get help. A other way is a subscription to the support mailing list.'));
  $page->setKeywords(__('WinMerge, support, discussion forums, mailing lists, tracker, donate, bugs, support requests, patches, feature requests, todo, open source, sourceforge.net'));
  $page->printHead(__('Support'), TAB_SUPPORT);

  $page->printHeading(__('Support'));
  $page->printPara(__('The <a href="%s">Open Discussion forum</a> is the fastest way to get help. Please be patient, it may take some time for somebody to answer.', 'http://apps.sourceforge.net/phpbb/winmerge/viewforum.php?f=4'),
                   __('A other way is a subscription to the <a href="%s">support mailing list</a>.', 'mailing-lists.php#support'));
  $page->printPara(__('If you find a bug, please submit it as a <a href="%s">bug report</a>.', 'http://bugs.winmerge.org/'),
                   __('Please attach as much information as you can: at a minimum, the version number of WinMerge that you are using. If you can, also attach a configuration log which, you can display by clicking <span class="guimenu">Help</span> &#8594; <span class="guimenuitem">Configuration</span> in the WinMerge window.'),
                   __('Good information in a bug report makes it more likely that your bug will be fixed quickly.'));
  $page->printPara(__('You must <a href="%s">register with SourceForge.net</a> before posting a bug report (registration is free).', 'https://sourceforge.net/account/registration/'),
                   __('We require registering because anonymous submissions caused a lot of spam and also because there were no possibility to contact people for asking more information.'),
                   __('We rarely sent direct emails but you will get notifications when we ask questions in the bug item.'));
  $page->printPara(__('Wish list items on the <a href="%s">feature request list</a> will also be considered, but we make absolutely no promises.', 'http://feature-requests.winmerge.org/'));

  $page->printSubHeading(__('Tracker Statistics'));
  $feed = new SimplePie();
  $feed->set_feed_url('http://sourceforge.net/export/rss2_projsummary.php?group_id=13216');
  $feed->set_cache_location('../engine/simplepie/cache');
  $feed->enable_order_by_date(false);
  $feed->init();
  print("<ul class=\"rssfeeditems\">\n");
  foreach ($feed->get_items() as $item) { //for all project summary items...
    $title = $item->get_title();
    if (stristr($title, 'Tracker:')) {
      $title = str_replace('Tracker: ', '', $title);
      print("  <li><a href=\"".$item->get_link()."\">".$title."</a></li>\n");
    }
  }
  print("</ul>\n");

  $page->printSubHeading(__('Donate'));
  $page->printPara(__('Since WinMerge is an <a href="%s">Open Source</a> project, you may use it free of charge.', '/about/license.php'),
                   __('But please consider making a <a href="%s">donation</a> to support the continued development of WinMerge.', 'http://sourceforge.net/project/project_donations.php?group_id=13216'));

  $page->printFoot();
?>