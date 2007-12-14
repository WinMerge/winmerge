<?php
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

  /**
   * ...
   *
   * Based partially on the TortoiseSVN Drupal module:
   * @link http://tortoisesvn.tigris.org/svn/tortoisesvn/trunk/contrib/drupal-modules/module/tortoisesvn.module
   */
  function get_status_graph($count, $translated, $fuzzy, $untranslated) {
    // Calculate width of bars
    $wc = 150;
    $wt = round($wc * $translated / $count);
    $wf = round($wc * $fuzzy / $count);
    $wu = round($wc * $untranslated / $count);
    
    // Make sure that each bar is at least 2px wide if its value is > 0
    if (($wt<2) && ($translated>0)) $wt=2;
    if (($wf<2) && ($fuzzy>0)) $wf=2;
    if (($wu<2) && ($untranslated>0)) $wu=2;
    
    // Make sure total width stays constant by determining the widest bar
    // and subtracting the width of the shorter bars from it.
    if ($wt+$wf+$wu!=$wc) {
      if ($wt>=$wf && $wt>=$wu) {
        $wt = $wc-$wf-$wu;
      } else if ($wf>=$wt && $wf>=$wu) {
        $wf = $wc-$wt-$wu;
      } else {
        $wu = $wc-$wt-$wf;
      }
    }
    
    // Calculate percentages
    $pt = ($translated * 100) / $count;
    $pf = ($fuzzy * 100) / $count;
    $pu = ($untranslated * 100) / $count;
    
    $temp = "";
    if ($wt > 0)
      $temp .= "<img src=\"images/translated.png\" alt=\"Translated\" title=\"Translated: " . round($pt, 1) . "%\" width=\"$wt\" height=\"16\">";
    if ($wf > 0)
      $temp .= "<img src=\"images/fuzzy.png\" alt=\"Fuzzy\" title=\"Fuzzy: " . round($pf, 1) . "%\" width=\"$wf\" height=\"16\">";
    if ($wu > 0)
      $temp .= "<img src=\"images/untranslated.png\" alt=\"Untranslated\" title=\"Untranslated: " . round($pu, 1) . "%\" width=\"$wu\" height=\"16\">";
    
    return $temp;
  }
  
  /**
   * ...
   */
  function printTranslationsStatus($xmlarray, $svnurl) {
    global $page;
    
    $update = $xmlarray['TRANSLATIONS'][0]['UPDATE'][0]['VALUE'];
    $temp = "<p>Status from <strong>" . $update . "</strong>:</p>\n";
    $temp .= "<table class=\"status\">\n";
    $temp .= "  <tr>\n";
    $temp .= "    <th class=\"left\">Language</th>\n";
    $temp .= "    <th class=\"center\">Graph</th>\n";
    $temp .= "    <th class=\"right\">Translated</th>\n";
    $temp .= "    <th class=\"right\">Fuzzy</th>\n";
    $temp .= "    <th class=\"right\">Untranslated</th>\n";
    $temp .= "    <th class=\"center\">Last Update</th>\n";
    $temp .= "  </tr>\n";
    $translations = $xmlarray['TRANSLATIONS'][0]['TRANSLATION'];
    $translations = $page->multisortArray($translations, 'LANGUAGE', SORT_ASC);
    for ($i = 0; $i < count($translations); $i++) { //For all translations...
      $translation = $translations[$i];
      $language_name = $translation['LANGUAGE'][0]['VALUE'];
      if ($language_name != 'English') { //If NOT English...
        $file_name = $translation['FILE'][0]['VALUE'];
        $translation_update = $translation['UPDATE'][0]['VALUE'];
        $strings = $translation['STRINGS'][0];
        $strings_count = $strings['COUNT'][0]['VALUE'];
        $strings_translated = $strings['TRANSLATED'][0]['VALUE'];
        $strings_fuzzy = $strings['FUZZY'][0]['VALUE'];
        $strings_untranslated = $strings['UNTRANSLATED'][0]['VALUE'];
        
        $temp .= "  <tr>\n";
        $temp .= "    <td class=\"left\"><a href=\"" . $svnurl . $file_name . "\">" . $language_name . "</a></td>\n";
        $temp .= "    <td class=\"center\">" . get_status_graph($strings_count, $strings_translated, $strings_fuzzy, $strings_untranslated) . "</td>\n";
        $temp .= "    <td class=\"right\">" . $strings_translated . "</td>\n";
        $temp .= "    <td class=\"right\">" . $strings_fuzzy . "</td>\n";
        $temp .= "    <td class=\"right\">" . $strings_untranslated . "</td>\n";
        $temp .= "    <td class=\"center\">" . $translation_update . "</td>\n";
        $temp .= "  </tr>\n";
      }
      else { //If English...
        $english = $translations[$i];
        $english_file_name = $english['FILE'][0]['VALUE'];
        $english_translation_update = $english['UPDATE'][0]['VALUE'];
        $english_strings_count = $english['STRINGS'][0]['COUNT'][0]['VALUE'];
      }
    }
    $temp .= "  <tr>\n";
    $temp .= "    <td class=\"left\"><a href=\"" . $svnurl . $english_file_name . "\">English (Template)</a></td>\n";
    $temp .= "    <td class=\"center\"></td>\n";
    $temp .= "    <td colspan=\"3\" class=\"center\">Total: " . $english_strings_count . "</td>\n";
    $temp .= "    <td class=\"center\">" . $english_translation_update . "</td>\n";
    $temp .= "  </tr>\n";
    $temp .= "</table>\n";
    print($temp);
  }
  
  /**
   * ...
   */
  function printTranslationsStatusGraphLegend() {
    $temp = "<ul class=\"inline\">\n";
    $temp .= "  <li><img src=\"images/translated.png\" alt=\"*\" width=\"16\" height=\"16\"> Translated</li>\n";
    $temp .= "  <li><img src=\"images/fuzzy.png\" alt=\"*\" width=\"16\" height=\"16\"> Fuzzy</li>\n";
    $temp .= "  <li><img src=\"images/untranslated.png\" alt=\"*\" width=\"16\" height=\"16\"> Untranslated</li>\n";
    $temp .= "</ul>\n";
    print($temp);
  }
  
  /**
   * ...
   */
  function printTranslationsTranslators($xmlarray) {
    global $page;
    
    $temp = "<table class=\"status\">\n";
    $temp .= "  <tr>\n";
    $temp .= "    <th class=\"left\">Language</th>\n";
    $temp .= "    <th class=\"left\">Translator(s)</th>\n";
    $temp .= "  </tr>\n";
    $translations = $xmlarray['TRANSLATIONS'][0]['TRANSLATION'];
    $translations = $page->multisortArray($translations, 'LANGUAGE', SORT_ASC);
    for ($i = 0; $i < count($translations); $i++) { //For all translations...
      $translation = $translations[$i];
      $language_name = $translation['LANGUAGE'][0]['VALUE'];
      if ($language_name != 'English') { //If NOT English...
        $translators = $translation['TRANSLATORS'][0]['TRANSLATOR'];
        
        $temp .= "  <tr>\n";
        $temp .= "    <td class=\"left\">" . $language_name . "</td>\n";
        $temp .= "    <td class=\"left\">";
        for ($j = 0; $j < count($translators); $j++) { //For all translators...
          $translator = $translators[$j];
          $translator_name = $translator['NAME'][0]['VALUE'];
          $translator_mail = $translator['MAIL'][0]['VALUE'];
          
          if ($translator_mail != '')
            $temp .= "<a href=\"mailto:" . $translator_mail . "\" title=\"" . htmlentities($translator_name) . " &lt;" . $translator_mail . "&gt;\">" . htmlentities($translator_name) . "</a>";
          else
            $temp .= $translator_name;
          $temp .= "<br>";
        }
        $temp .= "</td>\n";
        $temp .= "  </tr>\n";
      }
    }
    $temp .= "</table>\n";
    print($temp);
  }
?>