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

  define ('TAB_HOME', 'home');
  define ('TAB_ABOUT', 'about');
  define ('TAB_DOCS', 'docs');
  define ('TAB_DOWNLOADS', 'downloads');
  define ('TAB_SUPPORT', 'support');
  define ('TAB_TRANSLATIONS', 'translations');

  include 'release.inc';

  /**
   * ...
   */
  class Page {
    var $_title, $_charset;
    var $_description, $_keywords, $_rssfeeds;
    var $_tab;
    var $_stablerelease;

    /**
     * Constructor
     */
    function Page() {
      $this->_title = 'WinMerge';
      $this->_charset = 'ISO-8859-1';
      $this->_description = 'WinMerge HomePage: Visual text file comparison and merging (Open Source software).';
      $this->_keywords = 'WinMerge, free, download, Beyond Compare, Scooter Software, DLSuperC, DLSubCF, Component Software Diff, Viff, TreeDiff, FileSync, WMatch, Quick Spot, NTDMatch, windiff, WDIFF, FolderMatch, Merge 99, Merge99, PMdiff, PMDiff, PMDIFF, pmdiff, visual, visually, file, folder, directory, synchronization, comparison, graphical, document, text, compare, diff, difference, revision, Araxis, compare folders, compare files, synchronize folders, merge files, merge folders, Y2K, Year 2000, Year 2K, side-by-side, VFC, GFC, gdiff, CSDiff, OS2GFC, vdiff, ediff, fc';
      $this->_rssfeeds = array();
      $this->_tab = TAB_HOME;
      /* _Stable Release */
      $this->_stablerelease = new Release;
      $this->_stablerelease->setVersionNumber('2.10.2');
      $this->_stablerelease->setDate('2008-11-06');
      $this->_stablerelease->addDownload('setup.exe', 'http://downloads.sourceforge.net/winmerge/WinMerge-2.10.2-Setup.exe', 3243855);
      $this->_stablerelease->addDownload('exe.zip', 'http://downloads.sourceforge.net/winmerge/WinMerge-2.10.2-exe.zip', 3441767);
      $this->_stablerelease->addDownload('exe.7z', 'http://downloads.sourceforge.net/winmerge/WinMerge-2.10.2-exe.7z', 2015200);
      $this->_stablerelease->addDownload('rt.zip', 'http://downloads.sourceforge.net/winmerge/Runtimes-2.10.2.zip', 1287520);
      $this->_stablerelease->addDownload('rt.7z', 'http://downloads.sourceforge.net/winmerge/Runtimes-2.10.2.7z', 753833);
      $this->_stablerelease->addDownload('src.zip', 'http://downloads.sourceforge.net/winmerge/WinMerge-2.10.2-src.zip', 9767957);
      $this->_stablerelease->addDownload('src.7z', 'http://downloads.sourceforge.net/winmerge/WinMerge-2.10.2-src.7z', 5106442);
    }

    /**
     * set the title from the page
     */
    function setTitle($title) {
      $this->_title = $title;
    }

    /**
     * set the charset from the page
     */
    function setCharset($charset) {
      $this->_charset = $charset;
    }

    /**
     * set the description from the page
     */
    function setDescription($description) {
      $this->_description = $description;
    }

    /**
     * set the keywords from the page
     */
    function setKeywords($keywords) {
      $this->_keywords = $keywords;
    }

    /**
     * add RSS feed to the page head
     */
    function addRssFeed($url, $title = 'RSS') {
      $this->_rssfeeds[$url] = $title;
    }

    /**
     * set the tab from the page
     */
    function setTab($tab) {
      $this->_tab = $tab;
    }

    /**
     * print the head from the page
     */
    function printHead($title = '', $tab = '') {
      if ($title != '') {
        $this->_title = $title;
      }
      if ($tab != '') {
        $this->_tab = $tab;
      }
      $temp = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\"\n";
      $temp .= "  \"http://www.w3.org/TR/html4/loose.dtd\">\n";
      $temp .= "<html>\n\n";
      $temp .= "<head>\n";
      $temp .= "  <title>" . $this->_title . "</title>\n";
      $temp .= "  <meta http-equiv=\"content-type\" content=\"text/html; charset=" . $this->_charset . "\">\n";
      $temp .= "  <meta name=\"description\" content=\"" . $this->_description . "\">\n";
      $temp .= "  <meta name=\"keywords\" content=\"" . $this->_keywords . "\">\n";
      $temp .= "  <style type=\"text/css\" media=\"screen\">@import \"/css/screen.css\";</style>\n";
      $temp .= "  <link rel=\"stylesheet\" type=\"text/css\" media=\"print\" href=\"/css/print.css\">\n";
      $temp .= "  <link rel=\"shortcut icon\" type=\"image/x-icon\" href=\"/favicon.ico\">\n";
      foreach ($this->_rssfeeds as $rssurl => $rsstitle) { //for all RSS feeds...
        $temp .= "  <link rel=\"alternate\" type=\"application/rss+xml\" title=\"" . $rsstitle . "\" href=\"" . $rssurl . "\">\n";
      }
      $temp .= "</head>\n\n";
      $temp .= "<body>\n";
      $temp .= "<div id=\"head\">\n";
      $temp .= "<h1>WinMerge</h1>\n";
      $temp .= "<p id=\"rightcorner\">\n";
      $temp .= "  <a href=\"http://sourceforge.net/\"><img src=\"http://sourceforge.net/sflogo.php?group_id=13216\" width=\"88\" height=\"31\" border=\"0\" alt=\"SourceForge\" title=\"SourceForge.net\"></a>\n";
      $temp .= "  <a href=\"/WinMergePAD.php\"><img src=\"/images/padbutton.gif\" width=\"80\" height=\"31\" border=\"0\" alt=\"PAD Enabled\" title=\"PAD Enabled\"></a>\n";
      $temp .= "  <a href=\"http://sourceforge.net/donate/index.php?group_id=13216\"><img src=\"/images/project-support.jpg\" width=\"88\" height=\"32\" border=\"0\" alt=\"Donate\" title=\"Donate money to WinMerge\"></a>\n";
      $temp .= "</p>\n";
      $temp .= "</div> <!-- #head -->\n";
      $temp .= $this->getTabs();
      $temp .= $this->getAds();
      $temp .= "<div id=\"content\">\n";
      print($temp);
    }

    /**
     * print the foot from the page
     */
    function printFoot() {
      $temp = "</div> <!-- #content -->\n";
      $temp .= $this->getPiwikCode();
      $temp .= "</body>\n\n";
      $temp .= "</html>\n";
      print($temp);
    }

    /**
     * print the "Download Now!" link
     */
    function printDownloadNow() {
      $temp = "<p><a class=\"downloadnow\" href=\"" . $this->_stablerelease->getDownload('setup.exe') . "\">";
      $temp .= "<strong>Download Now!</strong> ";
      $temp .= "<em>Version " . $this->_stablerelease->getVersionNumber() . "; " . $this->_stablerelease->getDownloadSizeMb('setup.exe') . " MB</em>";
      $temp .= "</a></p>\n";
      print($temp);
    }

    /**
     * print a RSS feed link
     */
    function printRssFeedLink($url, $text = '') {
      $temp = "<a href=\"" . $url . "\"><img src=\"/images/rss.png\" alt=\"RSS\" border=\"0\"></a>";
      if ($text != '') { //if WITH text...
        $temp .= " <a href=\"" . $url . "\"> " . $text . "</a>";
      }
      print($temp);
    }

    /**
     * get the tabs menu from the page
     */
    function getTabs() {
      /* Main Tabs */
      $temp = "<ul id=\"tabs\">\n";
      if ($this->_tab == TAB_HOME) { //if "Home" ...
        $temp .= "  <li><strong><a href=\"/\" id=\"current\">Home</a></strong></li>\n";
      }
      else { //if NOT "Home" ...
        $temp .= "  <li><a href=\"/\">Home</a></li>\n";
      }
      if ($this->_tab == TAB_ABOUT) { //if "About" ...
        $temp .= "  <li><strong><a href=\"/about/\" id=\"current\">About</a></strong></li>\n";
      }
      else { //if NOT "About" ...
        $temp .= "  <li><a href=\"/about/\">About</a></li>\n";
      }
      if ($this->_tab == TAB_DOCS) { //if "Documentation" ...
        $temp .= "  <li><strong><a href=\"/docs/\" id=\"current\">Documentation</a></strong></li>\n";
      }
      else { //if NOT "Documentation" ...
        $temp .= "  <li><a href=\"/docs/\">Documentation</a></li>\n";
      }
      if ($this->_tab == TAB_DOWNLOADS) { //if "Downloads" ...
        $temp .= "  <li><strong><a href=\"/downloads/\" id=\"current\">Downloads</a></strong></li>\n";
      }
      else { //if NOT "Downloads" ...
        $temp .= "  <li><a href=\"/downloads/\">Downloads</a></li>\n";
      }
      if ($this->_tab == TAB_SUPPORT) { //if "Support" ...
        $temp .= "  <li><strong><a href=\"/support/\" id=\"current\">Support</a></strong></li>\n";
      }
      else { //if NOT "Support" ...
        $temp .= "  <li><a href=\"/support/\">Support</a></li>\n";
      }
      if ($this->_tab == TAB_TRANSLATIONS) { //if "Translations" ...
        $temp .= "  <li><strong><a href=\"/translations/\" id=\"current\">Translations</a></strong></li>\n";
      }
      else { //if NOT "Translations" ...
        $temp .= "  <li><a href=\"/translations/\">Translations</a></li>\n";
      }
      $temp .= "</ul>\n";
      /* Sub Tabs */
      switch ($this->_tab) {
        case TAB_HOME: //if "Home" ...
          $temp .= "<ul id=\"subtabs\">\n";
          $temp .= "  <li><a href=\"http://sourceforge.net/projects/winmerge\">Project&nbsp;Page</a></li>\n";
          $temp .= "  <li><a href=\"/links.php\">Links</a></li>\n";
          $temp .= "  <li><a href=\"/WinMergePAD.php\">PAD&nbsp;file</a></li>\n";
          $temp .= "</ul>\n";
          break;
        case TAB_ABOUT: //if "About" ...
          $temp .= "<ul id=\"subtabs\">\n";
          $temp .= "  <li><a href=\"/about/\" title=\"Index\">Index</a></li>\n";
          $temp .= "  <li><a href=\"/about/screenshots/\" title=\"Screenshots\">Screenshots</a></li>\n";
          $temp .= "  <li><a href=\"/about/history.php\">History</a></li>\n";
          $temp .= "</ul>\n";
          break;
        case TAB_DOCS: //if "Documentation" ...
          $temp .= "<ul id=\"subtabs\">\n";
          $temp .= "  <li><a href=\"/docs/\" title=\"Index\">Index</a></li>\n";
          $temp .= "  <li><a href=\"/docs/manual/\" title=\"Manual\">Manual</a></li>\n";
          $temp .= "  <li><a href=\"/docs/releasenotes.php\" title=\"Release Notes\">Release Notes</a> (<a href=\"/docs/releasenotes.php#issues\" title=\"Known Issues\">Known Issues</a>)</li>\n";
          $temp .= "  <li><a href=\"/docs/changelog.php\" title=\"Change Log\">Change Log</a></li>\n";
          $temp .= "  <li><a href=\"/Wiki/\" title=\"WinMerge Development Wiki\">Development Wiki</a></li>\n";
          $temp .= "</ul>\n";
          break;
        case TAB_SUPPORT: //if "Support" ...
          $temp .= "<ul id=\"subtabs\">\n";
          $temp .= "  <li><a href=\"/support/mailing-lists.php\" title=\"Mailing Lists\">Mailing&nbsp;Lists</a></li>\n";
          $temp .= "  <li><a href=\"http://forums.winmerge.org\" title=\"Discussion Forums\">Discussion&nbsp;Forums</a></li>\n";
          $temp .= "  <li><a href=\"http://sourceforge.net/tracker/?group_id=13216&amp;atid=113216\" title=\"Submit a Bug Report\">Bug&nbsp;Reports</a></li>\n";
          $temp .= "  <li><a href=\"http://sourceforge.net/tracker/?group_id=13216&amp;atid=363216\" title=\"Submit a Feature Request\">Feature&nbsp;Requests</a></li>\n";
          $temp .= "  <li><a href=\"http://sourceforge.net/tracker/?group_id=13216&amp;atid=213216\" title=\"Submit a Support Request\">Support&nbsp;Requests</a></li>\n";
          $temp .= "  <li><a href=\"http://sourceforge.net/tracker/?group_id=13216&amp;atid=313216\" title=\"Submit a Patch\">Patches</a></li>\n";
          $temp .= "</ul>\n";
          break;
        case TAB_TRANSLATIONS: //if "Translations" ...
          $temp .= "<ul id=\"subtabs\">\n";
          $temp .= "  <li><a href=\"/translations/\" title=\"Index\">Index</a></li>\n";
          $temp .= "  <li><a href=\"/translations/instructions.php\" title=\"Translations instructions\">Instructions</a></li>\n";
          $temp .= "  <li><a href=\"/translations/status_branch.php\" title=\"Translations Status (Branch)\">Status (Branch)</a></li>\n";
          $temp .= "  <li><a href=\"/translations/status_trunk.php\" title=\"Translations Status (Trunk)\">Status (Trunk)</a></li>\n";
          $temp .= "</ul>\n";
          break;
      }
      return $temp;
    }

    /**
     * get the ads to display on the page
     */
    function getAds() {
      $temp = "<div id=\"ads\">\n";
      $temp .= "<script type=\"text/javascript\"><!--\n";
      $temp .= "google_ad_client = \"pub-1411904495533584\";\n";
      $temp .= "google_ad_width = 728;\n";
      $temp .= "google_ad_height = 90;\n";
      $temp .= "google_ad_format = \"728x90_as\";\n";
      $temp .= "google_ad_type = \"text\";\n";
      $temp .= "google_ad_channel =\"\";\n";
      $temp .= "google_color_border = \"FFCC00\";\n";
      $temp .= "google_color_bg = \"FFFFCC\";\n";
      $temp .= "google_color_link = \"0000FF\";\n";
      $temp .= "google_color_text = \"000000\";\n";
      $temp .= "google_color_url = \"008000\";\n";
      $temp .= "//--></script>\n";
      $temp .= "<script type=\"text/javascript\"\n";
      $temp .= "  src=\"http://pagead2.googlesyndication.com/pagead/show_ads.js\">\n";
      $temp .= "</script>\n";
      $temp .= "</div> <!-- #ads -->\n";
      return $temp;
    }

    /**
     * get the Piwik java script code
     */
    function getPiwikCode() {
      $temp = "<!-- Piwik -->\n";
      $temp .= "<script type=\"text/javascript\">\n";
      $temp .= "var pkBaseURL = ((\"https:\" == document.location.protocol) ? \"https://apps.sourceforge.net/piwik/winmerge/\" : \"http://apps.sourceforge.net/piwik/winmerge/\");\n";
      $temp .= "document.write(unescape(\"%3Cscript src='\" + pkBaseURL + \"piwik.js' type='text/javascript'%3E%3C/script%3E\"));\n";
      $temp .= "</script><script type=\"text/javascript\">\n";
      $temp .= "piwik_action_name = '';\n";
      $temp .= "piwik_idsite = 1;\n";
      $temp .= "piwik_url = pkBaseURL + \"piwik.php\";\n";
      $temp .= "piwik_log(piwik_action_name, piwik_idsite, piwik_url);\n";
      $temp .= "</script>\n";
      $temp .= "<object><noscript><p><img src=\"http://apps.sourceforge.net/piwik/winmerge/piwik.php?idsite=1\" alt=\"piwik\"/></p></noscript></object>\n";
      $temp .= "<!-- End Piwik Tag -->\n";
      return $temp;
    }

    /**
     * get the stable release
     */
    function getStableRelease() {
      return $this->_stablerelease;
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
          $tree[$vals[$i]['tag']][$index] =  array_merge($tree[$vals[$i]['tag']][$index], $this->_getXmlChildren($vals, $i));
        }
        else {
          $tree[$vals[$i]['tag']][] = $this->_getXmlChildren($vals, $i);
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
              $children[$vals[$i]['tag']][$index] = array_merge($children[$vals[$i]['tag']][$index],$this->_getXmlChildren($vals, $i));
            }
            else {
              $children[$vals[$i]['tag']][] = $this->_getXmlChildren($vals, $i);
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
    
    /**
     * get the content (between <body> and </body>) from a HTML file
     */
    function getContentFromHtmlFile($filename, $patterns = '', $replacements = '') {
      $html = "";
      if (file_exists($filename) == true) { //if the file exists...
        $lines = file($filename);
        $linescount = sizeof($lines);
        
        $insidebody = false;
        for ($i = 0; $i < $linescount; $i++) { //for all lines...
          $line = $lines[$i];
          
          if ($insidebody == false) { //if OUTSIDE <body>...
            if (preg_match('#\<body#si', $line)) {
              $insidebody = true;
            }
          }
          else { //if INSIDE <body>...
            if (preg_match('#\</body>#si', $line)) {
              break;
            }
            if (($patterns != '') && ($replacements != '')) { //if text should be replaced...
              $line = preg_replace($patterns, $replacements, $line);
            }
            $html .= $line;
          }
        }
      }
      return $html;
    }
    
    /**
     * convert a (old) changelog file into html code
     */
    function convertChanges2Html($filename) {
      $html = "<pre>\n";
      if (file_exists($filename) == true) {
        $lines = file($filename);
        $linescount = sizeof($lines);
        
        for ($i = 0; $i < $linescount; $i++) {
          $line = $lines[$i];
          
          $line = preg_replace('#^([0-9][0-9][0-9][0-9]-[0-9][0-9]-[0-9][0-9])#si', '<b>' . '\\1' . '</b>', $line);
          $line = preg_replace('#PATCH: \[ ([0-9]*) \]#si', 'PATCH: [ <a href="http://winmerge.org/patch/\\1" target="_blank">\\1</a> ]', $line);
          $line = preg_replace('#BUG: \[ ([0-9]*) \]#si', 'BUG: [ <a href="http://winmerge.org/bug/\\1" target="_blank">\\1</a> ]', $line);
          $line = preg_replace('#RFE: \[ ([0-9]*) \]#si', 'RFE: [ <a href="http://winmerge.org/rfe/\\1" target="_blank">\\1</a> ]', $line);
          $line = preg_replace('#TODO: \[ ([0-9]*) \]#si', 'TODO: [ <a href="http://winmerge.org/todo/\\1" target="_blank">\\1</a> ]', $line);
        
          $html .= $line;
        }
      }
      $html .= "</pre>\n";
      
      return $html;
    }
    
    /**
     * convert a (new) changelog file into html code
     */
    function convertChangeLog2Html($filename) {
      $html = "";
      if (file_exists($filename) == true) { //if the file exists...
        $html .= "<pre>\n";
        
        $lines = file($filename);
        $linescount = sizeof($lines);
        
        for ($i = 0; $i < $linescount; $i++) { //for all lines...
          $line = $lines[$i];
          
          $line = preg_replace('#(WinMerge [0-9]\.[0-9].*-.*\))#si', '<h3>\\1</h3>', $line);
          $line = preg_replace('#r([0-9]+)#si', '<a href="http://winmerge.svn.sourceforge.net/viewvc/winmerge?view=rev&amp;revision=\\1" target="_blank">r\\1</a>', $line);
          $line = preg_replace('#\#([0-9]+)#si', '<a href="http://sourceforge.net/support/tracker.php?aid=\\1" target="_blank">#\\1</a>', $line);
          
          $html .= $line;
        }
        $html .= "</pre>\n";
      }
      return $html;
    }
  }
?>
