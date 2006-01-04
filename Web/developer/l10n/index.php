<?php
  include('../../page.inc');

  $page = new Page;
  $page->printHead('WinMerge: Translations', WINMERGE_DEV);
?>
<h2>Translations</h2>
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
<p>To change languages, select the desired language from the View-&gt;Languages menu choice; language names are shown in both languages (native language and current WinMerge language).</p>
<p>
If the desired language is not present, then you are missing a translation dll.
For example, for simplified Chinese, you need to have the file <code>MergeSimplifiedChinese.dll</code> in the <code>Languages</code> subdirectory next to your <code>WinMerge.exe</code> and/or <code>WinMergeU.exe</code> program files.
These would normally be installed for you, if you did a <em>Full Installation</em> from the normal WinMerge setup program.
If you did not install in that fashion, you may simply copy the desired DLL into the correct directory, and restart <code>WinMerge(U).exe</code>.
</p>
<p>If you would like to update any of these translations or add another translation, then please follow <a href="instructions.php">these instructions</a>.</p>
<p>You can also take a look at the <a href="status.php">status of the translations</a>.</p>
<?php
  $page->printFoot();
?>