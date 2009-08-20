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
      $temp .= "<img src=\"images/translated.png\" alt=\"" . __('Translated') . "\" title=\"" . __('Translated: %d%%', round($pt, 1)) . "\" width=\"$wt\" height=\"16\">";
    if ($wf > 0)
      $temp .= "<img src=\"images/fuzzy.png\" alt=\"" . __('Fuzzy') . "\" title=\"" . __('Fuzzy: %d%%', round($pf, 1)) . "\" width=\"$wf\" height=\"16\">";
    if ($wu > 0)
      $temp .= "<img src=\"images/untranslated.png\" alt=\"" . __('Untranslated') . "\" title=\"" . __('Untranslated: %d%%', round($pu, 1)) . "\" width=\"$wu\" height=\"16\">";
    
    return $temp;
  }
  
  /**
   * ...
   */
  function printTranslationsStatus($xmlarray, $svnurl) {
    $update = $xmlarray['TRANSLATIONS'][0]['UPDATE'][0]['VALUE'];
    $temp = "<p>Status from <strong>" . $update . "</strong>:</p>\n";
    $temp .= "<table class=\"status\">\n";
    $temp .= "  <tr>\n";
    $temp .= "    <th class=\"left\">" . __('Language') . "</th>\n";
    $temp .= "    <th class=\"center\">" . __('Graph') . "</th>\n";
    $temp .= "    <th class=\"right translated\">" . __('Translated') . "</th>\n";
    $temp .= "    <th class=\"right fuzzy\">" . __('Fuzzy') . "</th>\n";
    $temp .= "    <th class=\"right untranslated\">" . __('Untranslated') . "</th>\n";
    $temp .= "    <th class=\"center\">" . __('Last Update') . "</th>\n";
    $temp .= "  </tr>\n";
    $translations = $xmlarray['TRANSLATIONS'][0]['TRANSLATION'];
    $translations = multisortArray($translations, 'LANGUAGE', SORT_ASC);
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
        $temp .= "    <td class=\"left\"><a href=\"" . $svnurl . $file_name . "\" rel=\"nofollow\">" . $language_name . "</a></td>\n";
        $temp .= "    <td class=\"center\">" . get_status_graph($strings_count, $strings_translated, $strings_fuzzy, $strings_untranslated) . "</td>\n";
        $temp .= "    <td class=\"right translated\">" . $strings_translated . "</td>\n";
        $temp .= "    <td class=\"right fuzzy\">" . $strings_fuzzy . "</td>\n";
        $temp .= "    <td class=\"right untranslated\">" . $strings_untranslated . "</td>\n";
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
    $temp .= "    <td class=\"left\"><a href=\"" . $svnurl . $english_file_name . "\" rel=\"nofollow\">" . __('English (Template)') . "</a></td>\n";
    $temp .= "    <td class=\"center\"></td>\n";
    $temp .= "    <td colspan=\"3\" class=\"center\">" . __('Total: %s', $english_strings_count) . "</td>\n";
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
    $temp .= "  <li><img src=\"images/translated.png\" alt=\"*\" width=\"16\" height=\"16\"> " . __('Translated') . "</li>\n";
    $temp .= "  <li><img src=\"images/fuzzy.png\" alt=\"*\" width=\"16\" height=\"16\"> " . __('Fuzzy') . "</li>\n";
    $temp .= "  <li><img src=\"images/untranslated.png\" alt=\"*\" width=\"16\" height=\"16\"> " . __('Untranslated') . "</li>\n";
    $temp .= "</ul>\n";
    print($temp);
  }
  
  /**
   * ...
   */
  function printTranslationsStatusRSS($xmlarray, $version, $svnurl) {
    switch ($version) {
      case 'Trunk': $version_title = 'Unstable Trunk'; break;
      case 'Branch': $version_title = 'Stable Branch'; break;
      default: $version_title = $version;
    }
    
    $update = $xmlarray['TRANSLATIONS'][0]['UPDATE'][0]['VALUE'];
    $temp = "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n";
    $temp .= "<rss version=\"2.0\">\n";
    $temp .= "  <channel>\n";
    $temp .= "    <title>WinMerge Translations Status (" . $version_title . ")</title>\n";
    $temp .= "    <link>http://winmerge.org/</link>\n";
    $temp .= "    <description>...</description>\n";
    $temp .= "    <item>\n";
    $temp .= "      <title>Status from " . $update . "</title>\n";
    $temp .= "      <description>\n";
    $temp .= "        <![CDATA[\n";
    
    $temp .= "<ul>\n";
    $translations = $xmlarray['TRANSLATIONS'][0]['TRANSLATION'];
    $translations = multisortArray($translations, 'LANGUAGE', SORT_ASC);
    for ($i = 0; $i < count($translations); $i++) { //For all translations...
      $translation = $translations[$i];
      $language_name = $translation['LANGUAGE'][0]['VALUE'];
      if ($language_name != 'English') { //If NOT English...
        $file_name = $translation['FILE'][0]['VALUE'];
        $translation_update = $translation['UPDATE'][0]['VALUE'];
        $strings = $translation['STRINGS'][0];
        $strings_untranslated = $strings['UNTRANSLATED'][0]['VALUE'];
        
        $temp .= "  <li>\n";
        $temp .= "    <a href=\"" . $svnurl . $file_name . "\">" . $language_name . "</a> (" . $strings_untranslated . " Untranslated)\n";
        $temp .= "  </li>\n";
      }
    }
    $temp .= "</ul>\n";
    
    $temp .= "        ]]>\n";
    $temp .= "      </description>\n";
    $temp .= "      <guid>" . $update . "</guid>\n";
    $temp .= "      <link>http://" . $_SERVER['HTTP_HOST'] . $_SERVER['PHP_SELF'] . "</link>\n";
    $temp .= "    </item>\n";
    $temp .= "  </channel>\n";
    $temp .= "</rss>\n";
    print($temp);
  }
  
  /**
   * ...
   */
  function printTranslationsTranslators($xmlarray) {
    $temp = "<table class=\"status\">\n";
    $temp .= "  <tr>\n";
    $temp .= "    <th class=\"left\">" . __('Language') . "</th>\n";
    $temp .= "    <th class=\"left\">" . __('Translator(s)') . "</th>\n";
    $temp .= "  </tr>\n";
    $translations = $xmlarray['TRANSLATIONS'][0]['TRANSLATION'];
    $translations = multisortArray($translations, 'LANGUAGE', SORT_ASC);
    for ($i = 0; $i < count($translations); $i++) { //For all translations...
      $translation = $translations[$i];
      $language_name = $translation['LANGUAGE'][0]['VALUE'];
      if ($language_name != 'English') { //If NOT English...
        $translators = @$translation['TRANSLATORS'][0]['TRANSLATOR'];
        
        $temp .= "  <tr>\n";
        $temp .= "    <td class=\"left\">" . $language_name . "</td>\n";
        $temp .= "    <td class=\"left\">";
        for ($j = 0; $j < count($translators); $j++) { //For all translators...
          $translator = $translators[$j];
          $translator_name = utf8_encode(@$translator['NAME'][0]['VALUE']);
          $translator_mail = utf8_encode(@$translator['MAIL'][0]['VALUE']);
          $maintainer = @$translator['ATTRIBUTES']['MAINTAINER'];
          
          if ($maintainer == '1')
            $temp .= "<strong title=\"Maintainer\">";
          if ($translator_mail != '')
            $temp .= "<a href=\"mailto:" . $translator_mail . "\" title=\"" . htmlentities($translator_name, ENT_COMPAT, 'UTF-8') . " &lt;" . $translator_mail . "&gt;\">" . htmlentities($translator_name, ENT_COMPAT, 'UTF-8') . "</a>";
          else
            $temp .= $translator_name;
          if ($maintainer == '1')
            $temp .= "</strong>";
          $temp .= "<br>";
        }
        $temp .= "</td>\n";
        $temp .= "  </tr>\n";
      }
    }
    $temp .= "</table>\n";
    print($temp);
  }
  
  /**
   * convert a xml file into an array
   *
   * @link http://www.devdump.com/phpxml.php
   * @link http://www.php.net/manual/en/function.xml-parse-into-struct.php
   */
  function convertXml2Array($filename) {
    if (file_exists($filename) == true) {
      $data = implode('', file($filename));
      $data = utf8_encode($data);
      
      $parser = xml_parser_create('UTF-8');
      xml_parser_set_option($parser, XML_OPTION_SKIP_WHITE, 1);
      xml_parser_set_option($parser, XML_OPTION_TARGET_ENCODING, 'ISO-8859-1');
      xml_parse_into_struct($parser, $data, $vals, $index);
      xml_parser_free($parser);
      
      $i = 0;
      $tree = array();
      if (isset($vals[$i]['attributes'])) {
        $tree[$vals[$i]['tag']][]['ATTRIBUTES'] = $vals[$i]['attributes'];
        $index = count($tree[$vals[$i]['tag']]) - 1;
        $tree[$vals[$i]['tag']][$index] =  array_merge($tree[$vals[$i]['tag']][$index], _getXmlChildren($vals, $i));
      }
      else {
        $tree[$vals[$i]['tag']][] = _getXmlChildren($vals, $i);
      }
      return $tree;
    }
  }
  
  /**
   * helper function from "convertXml2Array"
   *
   * @link http://www.devdump.com/phpxml.php
   * @link http://www.php.net/manual/en/function.xml-parse-into-struct.php
   */
  function _getXmlChildren($vals, &$i) {
    $children = array(); // Contains node data
    
    /* Node has CDATA before it's children */
    if (isset($vals[$i]['value']))
      $children['VALUE'] = $vals[$i]['value'];
    
    /* Loop through children */
    while (++$i < count($vals)) {
      switch ($vals[$i]['type']) {
        /* Node has CDATA after one of it's children
          (Add to cdata found before if this is the case) */
        case 'cdata':
          if (isset($children['VALUE']))
            $children['VALUE'] .= $vals[$i]['value'];
          else
            $children['VALUE'] = $vals[$i]['value'];
          break;
        /* At end of current branch */
        case 'complete':
          if (isset($vals[$i]['attributes'])) {
            $children[$vals[$i]['tag']][]['ATTRIBUTES'] = $vals[$i]['attributes'];
            $index = count($children[$vals[$i]['tag']]) - 1;
            
            if (isset($vals[$i]['value']))
              $children[$vals[$i]['tag']][$index]['VALUE'] = $vals[$i]['value'];
            else
              $children[$vals[$i]['tag']][$index]['VALUE'] = '';
          }
          else {
            if (isset($vals[$i]['value']))
              $children[$vals[$i]['tag']][]['VALUE'] = $vals[$i]['value'];
            else
              $children[$vals[$i]['tag']][]['VALUE'] = '';
          }
          break;
        /* Node has more children */
        case 'open':
          if (isset($vals[$i]['attributes'])) {
            $children[$vals[$i]['tag']][]['ATTRIBUTES'] = $vals[$i]['attributes'];
            $index = count($children[$vals[$i]['tag']]) - 1;
            $children[$vals[$i]['tag']][$index] = array_merge($children[$vals[$i]['tag']][$index],_getXmlChildren($vals, $i));
          }
          else {
            $children[$vals[$i]['tag']][] = _getXmlChildren($vals, $i);
          }
          break;
        /* End of node, return collected data */
        case 'close':
          return $children;
      }
    }
  }
  
  /**
   * sort a multi-dimensional array
   *
   * @link http://www.php.net/manual/en/function.usort.php
   */
  function multisortArray() {
    $arguments = func_get_args();
    $array = $arguments[0];
    $code = '';
    for ($c = 1; $c < count($arguments); $c += 2) {
      if (in_array($arguments[$c + 1], array(SORT_ASC, SORT_DESC))) {
        $code .= 'if ($a["'.$arguments[$c].'"] != $b["'.$arguments[$c].'"]) {';
        if ($arguments[$c + 1] == SORT_ASC) {
          $code .= 'return ($a["'.$arguments[$c].'"] < $b["'.$arguments[$c].'"] ? -1 : 1); }';
        }
        else {
          $code .= 'return ($a["'.$arguments[$c].'"] < $b["'.$arguments[$c].'"] ? 1 : -1); }';
        }
      }
    }
    $code .= 'return 0;';
    $compare = create_function('$a,$b', $code);
    usort($array, $compare);
    return $array;
  }
?>