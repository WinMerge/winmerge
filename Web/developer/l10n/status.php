<?php
  include('../../page.inc');

  $page = new Page;
  $page->printHead('WinMerge: Status of translations');
?>
<h2>Status of translations</h2>

<p>Our goal is to have stable releases fully localized. But as development is
pretty fast, there is no way to keep translations up-to-date in beta
releases.</p>

<p>The table below shows the releases where a particular language was updated.</p>

<h3>Maintained or unmaintained?</h3>
<p>Translations are divided to four categories:</p>
<ul>
  <li class="toc"><b><a name="actively">Actively maintained</a></b> - translator(s) send frequent
  updates</li>
  <li class="toc"><b><a name="maintained">Maintained</a></b> - translator(s) (we hope) send updates
  when we ask</li>
  <li class="toc"><b><a name="unmaintained">Unmaintained</a></b> - we haven't heard anything about
  translator(s)</li>
  <li class="toc"><b><a name="unknown">Unknown</a></b> - we get update to latest stable but don't
  know if translator is still interested</li>
</ul>

<?php
  $status = $page->convertXml2Array('status.xml');

  print("<div align=\"center\">\n");
  print("<table border=\"1\" cellpadding=\"3\" cellspacing=\"0\">\n");
  print("  <tr>\n");
  print("    <th>Language</th>\n");
  print("    <th>Status</th>\n");
  print("    <th>Author</th>\n");
  print("    <th colspan=\"3\">Versions</th>\n");
  print("  </tr>\n");
  $languages = $status['L10N'][0]['LANGUAGE'];
  for ($i = 0; $i < count($languages); $i++) { //For all languages...
    $language = $languages[$i];
    $language_name = $language['NAME'][0]['VALUE'];
    $language_status = $language['STATUS'][0]['VALUE'];
    $authors = $language['AUTHORS'][0]['AUTHOR'];
    $versions = $language['VERSIONS'][0]['VERSION'];

    print("  <tr>\n");
    print("    <td>" . $language_name . "</td>\n");
    switch ($language_status) {
      case 1: //Actively maintained
        print("    <td><a href=\"#actively\">Actively maintained</a></td>\n");
        break;
      case 2: //Maintained
        print("    <td><a href=\"#maintained\">Maintained</a></td>\n");
        break;
      case 3: //Unmaintained
        print("    <td><a href=\"#unmaintained\">Unmaintained</a></td>\n");
        break;
      case 4: //Unknown
        print("    <td><a href=\"#unknown\">Unknown</a></td>\n");
        break;
    }
    print("    <td>");
    for ($j = 0; $j < count($authors); $j++) { //For all authors...
      $author = $authors[$j];
      $author_name = $author['NAME'][0]['VALUE'];
      $author_email = $author['EMAIL'][0]['VALUE'];
      $author_comment = $author['COMMENT'][0]['VALUE'];

      if ($author_email != '')
        print("<a href=\"mailto:" . $author_email . "\">" . $author_name . "</a>");
      else
        print($author_name);
      if ($author_comment != '') print(" (" . $author_comment . ")");
      print("<br>");
    }
    print("</td>\n");

    $version_21 = '&nbsp;';
    $version_20 = '&nbsp;';
    $version_17 = '&nbsp;';
    for ($j = 0; $j < count($versions); $j++) { //For all versions...
      $version = $versions[$j]['VALUE'];
      
      switch ($version) {
        case '2.1':
          $version_21 = '2.1';
          break;
        case '2.0':
          $version_20 = '2.0';
          break;
        case '1.7':
          $version_17 = '1.7';
          break;
      }
    }
    print("    <td>" . $version_21 . "</td>\n");
    print("    <td>" . $version_20 . "</td>\n");
    print("    <td>" . $version_17 . "</td>\n");
    print("  </tr>\n");
  }
  print("</table>\n");
  print("</div>\n");
?>

<p>Release 2.0 language versions also works with release 2.0.2.<br>
Release 1.4 language versions also works with release 1.7.</p>

<p>If you would like to update any of these translations or add another translation, then please follow <a href="instructions.php">these instructions</a>.</p>
<?php
  $page->printFoot();
?>