<?php
  //Send 404 headers...
  header('HTTP/1.1 404 Not Found');
  header('Status: 404 Not Found');

  include('page.inc');

  $page = new Page;
  $page->printHead('WinMerge: Error 404 (Page Not Found)', TAB_HOME);
?>
<h2>Page Not Found...</h2>
<p>For some reason (mis-typed URL, faulty referral from another site, out-of-date search engine listing or we simply deleted a file) the page you were looking for could not be found.
<!--This site has recently undergone a major re-working, so that might explain why you got this page instead.--></p>

<h3>Were you looking for...</h3>
<ul>
  <li><a href="/2.6/manual/index.html">Manual</a>?</li>
  <li><a href="/2.6/screenshots.php">Screenshots</a>?</li>
  <!--<li><a href="/docs">Documentation</a>?</li>-->
  <li><a href="/downloads">Downloads</a>?</li>
  <li><a href="/support">Support</a>?</li>
  <li><a href="/translations">Translations</a>?</li>
</ul>
<?php
  $page->printFoot();
?>