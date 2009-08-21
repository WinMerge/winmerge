<?php
  //Send 404 headers...
  header('HTTP/1.1 404 Not Found');
  header('Status: 404 Not Found');

  include('engine/engine.inc');

  $page = new Page;
  $page->printHead(__('Error 404 (Page Not Found)'), TAB_HOME);

  $page->printHeading(__('Page Not Found...'));
  $page->printPara(__('For some reason (mis-typed URL, faulty referral from another site, out-of-date search engine listing or we simply deleted a file) the page you were looking for could not be found.'),
                   __('This site has recently undergone a major re-working, so that might explain why you got this page instead.'));

  $page->printSubHeading(__('Were you looking for...'));
?>
<ul>
  <li><?php __e('<a href="/about/screenshots/">Screenshots</a>?');?></li>
  <li><?php __e('<a href="/docs/">Documentation</a>?');?>
    <ul>
      <li><?php __e('<a href="/docs/manual/">Manual</a>?');?></li>
      <li><?php __e('<a href="/docs/releasenotes.php">Release Notes</a>?');?></li>
      <li><?php __e('<a href="/docs/changelog.php">Change Log</a>?');?></li>
    </ul>
  </li>
  <li><?php __e('<a href="/downloads/">Downloads</a>?');?></li>
  <li><?php __e('<a href="/support/">Support</a>?');?></li>
  <li><?php __e('<a href="/translations/">Translations</a>?');?></li>
</ul>
<?php
  $page->printDownloadNow();
  $page->printFoot();
?>