<?php
  include('../../page.inc');

  $page = new Page;
  $page->printHead('WinMerge: Translations');
?>
<h2>WinMerge translations</h2>
<p>We currently have WinMerge translated into the languages listed below:</p>
<?php
  $status = $page->convertXml2Array('status.xml');
  
  print("<ul>\n");
  $languages = $status['L10N'][0]['LANGUAGE'];
  for ($i = 0; $i < count($languages); $i++) { //For all languages...
    $language = $languages[$i];
    $language_name = $language['NAME'][0]['VALUE'];
    
    print("  <li>" . $language_name . "</li>\n");
  }
  print("</ul>\n");
?>
<p>To change languages, all you have to do is unzip the DLL in the same directory as <code>WinMerge.exe</code>, then select View / Language and choose your language.</p>
<p>If you would like to update any of these translations or add another translation, then please follow <a href="instructions.php">these instructions</a>.</p>
<p>You can also take a look at the <a href="status.php">status of the translations</a>.</p>
<?php
  $page->printFoot();
?>