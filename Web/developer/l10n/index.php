<?php
  include('../../page.inc');

  $page = new Page;
  $page->printHead('WinMerge: Translations');
?>
<h2>WinMerge translations</h2>
<p>We currently have WinMerge translated into the languages listed below:</p>
<ul>
  <li>Brazilian Portuguese</li>
  <li>Bulgarian</li>
  <li>Catalan</li>
  <li>Simplified Chinese</li>
  <li>Traditional Chinese</li>
  <li>Czech</li>
  <li>Danish</li>
  <li>Dutch</li>
  <li>Esperanto</li>
  <li>French</li>
  <li>German</li>
  <li>Italian</li>
  <li>Korean</li>
  <li>Norwegian</li>
  <li>Polish</li>
  <li>Russian</li>
  <li>Slovak</li>
  <li>Spanish</li>
  <li>Swedish</li>
</ul>
<p>To change languages, all you have to do is unzip the DLL in the same directory as <code>WinMerge.exe</code>, then select View / Language and choose your language.</p>
<p>If you would like to update any of these translations, then please follow the <a href="instructions.php">instructions</a> above.</p>
<p>You can also take a look at the <a href="status.php">status of the translations</a>.</p>
<?php
  $page->printFoot();
?>