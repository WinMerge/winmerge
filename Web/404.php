<?php
  //Send 404 headers...
  header('HTTP/1.1 404 Not Found');
  header('Status: 404 Not Found');

  include('page.inc');

  $page = new Page;
  $page->printHead('Error 404 (Page Not Found)', TAB_HOME);
?>
<h2>Page Not Found...</h2>
<p>For some reason (mis-typed URL, faulty referral from another site, out-of-date search engine listing or we simply deleted a file) the page you were looking for could not be found.
This site has recently undergone a major re-working, so that might explain why you got this page instead.</p>

<h3>Were you looking for...</h3>
<ul>
  <li><a href="/about/screenshots/">Screenshots</a>?</li>
  <li><a href="/docs/">Documentation</a>?
    <ul>
      <li><a href="/docs/manual/">Manual</a>?</li>
      <li><a href="/docs/releasenotes.php">Release Notes</a>?</li>
      <li><a href="/docs/changelog.php">Change Log</a>?</li>
    </ul>
  </li>
  <li><a href="/downloads/">Downloads</a>?</li>
  <li><a href="/support/">Support</a>?</li>
  <li><a href="/translations/">Translations</a>?</li>
</ul>
<?php $page->printDownloadNow(); ?>
<?php
  $page->printFoot();
?>