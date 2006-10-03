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
<p>To change a language, select the desired language from the View-&gt;Languages
menu choice; language names are shown in both languages (native language and
current WinMerge language).</p>
<p>If the desired language is not present, then you are missing a language file.
Language files are named as <code>Merge</code> + <code><var>language name</var></code>
+ <code>.lang</code>, <code><em>.lang</em></code> as filename extension. For example, for Catalan, you need
to have the file <code>MergeCatalan.lang</code> in the <code>Languages</code>
subfolder of WinMerge folder. These would normally be installed for you by
installer, if you did a <em>Full Installation</em>. Installer also allows to
select individual translations to install. If you did not install translation(s)
with installer you can copy the desired language file into the correct folder,
and restart WinMerge.</p>
<p>If you would like to update any of these translations or add another translation,
please follow <a href="instructions.php">these instructions</a>.</p>
<p>You can also take a look at the <a href="status.php">status of the translations</a>.</p>
<?php
  $page->printFoot();
?>