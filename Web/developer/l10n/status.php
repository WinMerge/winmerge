<?php
  include('../../page.inc');

  $page = new Page;
  $page->printHead('WinMerge: Status of translations', WINMERGE_DEV);
?>
<h2>Status of translations</h2>

<p>Our goal is to have stable releases fully localized. But as development is
pretty fast, there is no way to keep translations up-to-date in beta
releases.</p>

<p class="note">Stable bugfix-releases do not have translation changes after
stable release (for example 2.2.x releases do not have changes after 2.2.0).</p>

<p>The table below shows the releases where a particular language was updated.</p>

<h3>Maintained or unmaintained?</h3>
<p>Translations are divided to four categories:</p>
<ul>
  <li><strong class="actively"><a id="actively" name="actively">Actively maintained</a></strong> - translator(s) send frequent
  updates</li>
  <li><strong class="maintained"><a id="maintained" name="maintained">Maintained</a></strong> - translator(s) (we hope) send updates
  when we ask</li>
  <li><strong class="unmaintained"><a id="unmaintained" name="unmaintained">Unmaintained</a></strong> - we haven't heard anything from
  translator(s)</li>
  <li><strong class="unknown"><a id="unknown" name="unknown">Unknown</a></strong> - we got update to latest stable but don't
  know if translator is still interested</li>
</ul>

<?php
  $status = $page->convertXml2Array('status.xml');

  print("<table id=\"status\">\n");
  print("  <tr>\n");
  print("    <th class=\"left\">Language</th>\n");
  print("    <th class=\"left\">Status</th>\n");
  print("    <th class=\"left\">Author</th>\n");
  print("    <th class=\"center\" colspan=\"6\">Versions</th>\n");
  print("  </tr>\n");
  $languages = $status['L10N'][0]['LANGUAGE'];
  $languages = $page->multisortArray($languages, 'NAME', SORT_ASC);
  for ($i = 0; $i < count($languages); $i++) { //For all languages...
    $language = $languages[$i];
    $language_name = $language['NAME'][0]['VALUE'];
    $language_status = $language['STATUS'][0]['VALUE'];
    $authors = $language['AUTHORS'][0]['AUTHOR'];
    $versions = $language['VERSIONS'][0]['VERSION'];

    switch ($language_status) {
      case 1: //Actively maintained
        print("  <tr class=\"actively\">\n");
        print("    <td class=\"left\">" . $language_name . "</td>\n");
        print("    <td class=\"left\"><a href=\"#actively\" title=\"translator(s) send frequent updates\">Actively maintained</a></td>\n");
        break;
      case 2: //Maintained
        print("  <tr class=\"maintained\">\n");
        print("    <td class=\"left\">" . $language_name . "</td>\n");
        print("    <td class=\"left\"><a href=\"#maintained\" title=\"translator(s) (we hope) send updates when we ask\">Maintained</a></td>\n");
        break;
      case 3: //Unmaintained
        print("  <tr class=\"unmaintained\">\n");
        print("    <td class=\"left\">" . $language_name . "</td>\n");
        print("    <td class=\"left\"><a href=\"#unmaintained\" title=\"we haven't heard anything from translator(s)\">Unmaintained</a></td>\n");
        break;
      case 4: //Unknown
        print("  <tr class=\"unknown\">\n");
        print("    <td class=\"left\">" . $language_name . "</td>\n");
        print("    <td class=\"left\"><a href=\"#unknown\" title=\"we got update to latest stable but don't know if translator is still interested\">Unknown</a></td>\n");
        break;
    }
    print("    <td class=\"left\">");
    for ($j = 0; $j < count($authors); $j++) { //For all authors...
      $author = $authors[$j];
      $author_name = $author['NAME'][0]['VALUE'];
      $author_email = $author['EMAIL'][0]['VALUE'];
      $author_comment = $author['COMMENT'][0]['VALUE'];

      if ($author_email != '')
        print("<a href=\"mailto:" . $author_email . "\" title=\"" . htmlentities($author_name) . " &lt;" . $author_email . "&gt;\">" . htmlentities($author_name) . "</a>");
      else
        print($author_name);
      if ($author_comment != '') print(" (" . htmlentities($author_comment) . ")");
      print("<br />");
    }
    print("</td>\n");

    $version_26 = '&nbsp;';
    $version_25 = '&nbsp;';
    $version_24 = '&nbsp;';
    $version_22 = '&nbsp;';
    $version_20 = '&nbsp;';
    $version_17 = '&nbsp;';
    for ($j = 0; $j < count($versions); $j++) { //For all versions...
      $version = $versions[$j]['VALUE'];
      
      switch ($version) {
        case '2.6':
          $version_26 = '<span title="upcoming stable version">2.6</span>';
          break;
        case '2.5':
          $version_25 = '<span title="latest development version">2.5</span>';
          break;
        case '2.4':
          $version_24 = '<strong title="latest stable version">2.4</strong>';
          break;
        case '2.2':
          $version_22 = '<span title="older stable version">2.2</span>';
          break;
        case '2.0':
          $version_20 = '<span title="older stable version">2.0</span>';
          break;
        case '1.7':
          $version_17 = '<span title="much older stable version">1.7</span>';
          break;
      }
    }
    print("    <td class=\"center\">" . $version_26 . "</td>\n");
    print("    <td class=\"center\">" . $version_25 . "</td>\n");
    print("    <td class=\"center\">" . $version_24 . "</td>\n");
    print("    <td class=\"center\">" . $version_22 . "</td>\n");
    print("    <td class=\"center\">" . $version_20 . "</td>\n");
    print("    <td class=\"center\">" . $version_17 . "</td>\n");
    print("  </tr>\n");
  }
  print("</table>\n");
?>

<ul>
  <li><strong>2.2.2</strong>: Please use version 2.2.2 language files since there are bugfixes in them.</li>
  <li><strong>2.0.2</strong>: Release 2.0 language versions also works with release 2.0.2.</li>
  <li><strong>1.7</strong>: Release 1.4 language versions also works with release 1.7.</li>
</ul>

<p>If you would like to update any of these translations or add another translation, then please follow <a href="instructions.php">these instructions</a>.</p>
<?php
  $page->printFoot();
?>