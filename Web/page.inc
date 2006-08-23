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

  define ('WINMERGE_USR', 'usr');
  define ('WINMERGE_1_7', '1.7');
  define ('WINMERGE_2_0', '2.0');
  define ('WINMERGE_2_2', '2.2');
  define ('WINMERGE_2_4', '2.4');
  define ('WINMERGE_DEV', 'dev');

  /**
   * ...
   */
  class Page {
    var $_title, $_charset;
    var $_description, $_keywords;
    var $_version;

    /**
     * Constructor
     */
    function Page() {
      $this->_title = 'WinMerge 2.4';
      $this->_charset = 'ISO-8859-1';
      $this->_description = 'WinMerge HomePage: Visual text file comparison and merging (Open Source software).';
      $this->_keywords = 'WinMerge, free, download, Beyond Compare, Scooter Software, DLSuperC, DLSubCF, Component Software Diff, Viff, TreeDiff, FileSync, WMatch, Quick Spot, NTDMatch, windiff, WDIFF, FolderMatch, Merge 99, Merge99, PMdiff, PMDiff, PMDIFF, pmdiff, visual, visually, file, folder, directory, synchronization, comparison, graphical, document, text, compare, diff, difference, revision, Araxis, compare folders, compare files, synchronize folders, merge files, merge folders, Y2K, Year 2000, Year 2K, side-by-side, VFC, GFC, gdiff, CSDiff, OS2GFC, vdiff, ediff, fc';
      $this->_version = WINMERGE_2_4;
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
     * set the WinMerge version from the page
     */
    function setVersion($version) {
      $this->_version = $version;
    }

    /**
     * print the head from the page
     */
    function printHead($title = '', $version = '') {
      if ($title != '') {
        $this->_title = $title;
      }
      if ($version != '') {
        $this->_version = $version;
      }
      $temp = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\"\n";
      $temp .= "  \"http://www.w3.org/TR/html4/loose.dtd\">\n";
      $temp .= "<html>\n\n";
      $temp .= "<head>\n";
      $temp .= "  <title>" . $this->_title . "</title>\n";
      $temp .= "  <meta http-equiv=\"content-type\" content=\"text/html; charset=" . $this->_charset . "\">\n";
      $temp .= "  <meta name=\"description\" content=\"" . $this->_description . "\">\n";
      $temp .= "  <meta name=\"keywords\" content=\"" . $this->_keywords . "\">\n";
      $temp .= "  <style type=\"text/css\" media=\"screen\">@import \"/css/all.css\";</style>\n";
      $temp .= "  <link rel=\"stylesheet\" type=\"text/css\" media=\"print\" href=\"/css/print.css\">\n";
      $temp .= "  <link rel=\"shortcut icon\" type=\"image/x-icon\" href=\"/favicon.ico\">\n";
      $temp .= "</head>\n\n";
      $temp .= "<body>\n";
      $temp .= "<div id=\"head\">\n";
      $temp .= "<h1><a href=\"/index.php\"><img src=\"/images/icon.gif\" height=\"32\" width=\"32\" align=\"top\" alt=\"WinMerge logo\" border=\"0\"></a> <img src=\"/images/logo.gif\" height=\"50\" width=\"218\" align=\"top\" alt=\"WinMerge\" border=\"0\"></h1>\n";
      $temp .= "  <div id=\"tabs\">\n";
      $temp .= "<ul>\n";
      if ($this->_version == WINMERGE_USR) { //if "Users" ...
        $temp .= "  <li class=\"selected\"><a href=\"/index.php\">Users</a></li>\n";
      }
      else { //if NOT "Users" ...
        $temp .= "  <li><a href=\"/index.php\">Users</a></li>\n";
      }
      if ($this->_version == WINMERGE_2_4) { //if WinMerge 2.4 ...
        $temp .= "  <li class=\"selected\"><a href=\"/2.4/index.php\">2.4</a></li>\n";
      }
      else { //if WinMerge 2.4 ...
        $temp .= "  <li><a href=\"/2.4/index.php\">2.4</a></li>\n";
      }
      if ($this->_version == WINMERGE_2_2) { //if WinMerge 2.2 ...
        $temp .= "  <li class=\"selected\"><a href=\"/2.2/index.php\">2.2</a></li>\n";
      }
      else { //if WinMerge 2.2 ...
        $temp .= "  <li><a href=\"/2.2/index.php\">2.2</a></li>\n";
      }
      if ($this->_version == WINMERGE_2_0) { //if WinMerge 2.0 ...
        $temp .= "  <li class=\"selected\"><a href=\"/2.0/index.php\">2.0</a></li>\n";
      }
      else { //if NOT WinMerge 2.0 ...
        $temp .= "  <li><a href=\"/2.0/index.php\">2.0</a></li>\n";
      }
      if ($this->_version == WINMERGE_1_7) { //if WinMerge 1.7 ...
        $temp .= "  <li class=\"selected\"><a href=\"/1.7/index.php\">1.7</a></li>\n";
      }
      if ($this->_version == WINMERGE_DEV) { //if "Developers" ...
        $temp .= "  <li class=\"selected\"><a href=\"/developer/index.php\">Developers</a></li>\n";
      }
      else { //if NOT "Developers" ...
        $temp .= "  <li><a href=\"/developer/index.php\">Developers</a></li>\n";
      }
      $temp .= "</ul>\n";
      $temp .= "  </div>\n";
      $temp .= "</div>\n";
      $temp .= "<div id=\"main\">\n";
      $temp .= "  <div id=\"nav\">\n";
      $temp .= $this->getMenu();
      $temp .= "  </div>\n";
      $temp .= "  <div id=\"rightsideads\">\n";
      $temp .= $this->getRightSideAds();
      $temp .= "  </div>\n";
      $temp .= "  <div id=\"content\">\n";
      print($temp);
    }

    /**
     * print the foot from the page
     */
    function printFoot() {
      $temp = "  <div id=\"bottomads\">\n";
      $temp .= $this->getBottomAds();
      $temp .= "  </div>\n";
      $temp .= "  </div>\n";
      $temp .= "</div>\n";
      $temp .= "</body>\n\n";
      $temp .= "</html>\n";
      print($temp);
    }

    /**
     * get the right side ads to display on the page
     *
     */
    function getRightSideAds() {
      $temp = "<table><tr><td>\n";

      $temp .= "<!-- SiteSearch Google -->\n";
      $temp .= "<form method=\"get\" action=\"http://www.google.com/custom\" target=\"_top\">\n";
      $temp .= "<table border=\"0\" bgcolor=\"#FFFFCC\">\n";
      $temp .= "<tr>\n";
      $temp .= "<td nowrap=\"nowrap\">\n";
      $temp .= "<input type=\"hidden\" name=\"domains\" value=\"winmerge.org\"></input>\n";
      $temp .= "<input type=\"text\" name=\"q\" size=\"15\" maxlength=\"255\" value=\"\"></input>\n";
      $temp .= "</td></tr>\n";
      $temp .= "<tr>\n";
      $temp .= "<td nowrap=\"nowrap\">\n";
      $temp .= "<input type=\"submit\" name=\"sa\" value=\"Google Search\"></input>\n";
      $temp .= "<table>\n";
      $temp .= "<tr>\n";
      $temp .= "<td>\n";
      $temp .= "<input type=\"radio\" name=\"sitesearch\" value=\"winmerge.org\" checked=\"checked\"></input>\n";
      $temp .= "<font size=\"-1\" color=\"#000000\">winmerge.org</font>\n";
      $temp .= "</td>\n";
      $temp .= "</tr>\n";
      $temp .= "<tr>\n";
      $temp .= "<td>\n";
      $temp .= "<input type=\"radio\" name=\"sitesearch\" value=\"\"></input>\n";
      $temp .= "<font size=\"-1\" color=\"#000000\">Web</font>\n";
      $temp .= "</td>\n";
      $temp .= "</tr>\n";
      $temp .= "</table>\n";
      $temp .= "<input type=\"hidden\" name=\"client\" value=\"pub-3779523531306529\"></input>\n";
      $temp .= "<input type=\"hidden\" name=\"forid\" value=\"1\"></input>\n";
      $temp .= "<input type=\"hidden\" name=\"channel\" value=\"9885282933\"></input>\n";
      $temp .= "<input type=\"hidden\" name=\"ie\" value=\"ISO-8859-1\"></input>\n";
      $temp .= "<input type=\"hidden\" name=\"oe\" value=\"ISO-8859-1\"></input>\n";
      $temp .= "<input type=\"hidden\" name=\"cof\" value=\"GALT:#008000;GL:1;DIV:#336699;VLC:663399;AH:center;BGC:FFFFCC;LBGC:FFCC00;ALC:0000FF;LC:0000FF;T:000000;GFNT:0000FF;GIMP:0000FF;LH:50;LW:218;L:http://winmerge.org/images/logo.gif;S:http://winmerge.org;FORID:1\"></input>\n";
      $temp .= "<input type=\"hidden\" name=\"hl\" value=\"en\"></input>\n";
      $temp .= "</td></tr></table>\n";
      $temp .= "</form>\n";
      $temp .= "<!-- SiteSearch Google -->\n";

      $temp .= "</td></tr><tr><td>\n";

      $temp .= "<script type=\"text/javascript\"><!--\n";
      $temp .= "google_ad_client = \"pub-3779523531306529\";\n";
      $temp .= "google_ad_width = 120;\n";
      $temp .= "google_ad_height = 600;\n";
      $temp .= "google_ad_format = \"120x600_as\";\n";
      $temp .= "google_ad_type = \"text\";\n";
      $temp .= "google_ad_channel =\"1940228029\";\n";
      $temp .= "google_color_border = \"FFCC00\";\n";
      $temp .= "google_color_bg = \"FFFFCC\";\n";
      $temp .= "google_color_link = \"0000FF\";\n";
      $temp .= "google_color_text = \"000000\";\n";
      $temp .= "google_color_url = \"008000\";\n";
      $temp .= "//--></script>\n";
      $temp .= "<script type=\"text/javascript\"\n";
      $temp .= "  src=\"http://pagead2.googlesyndication.com/pagead/show_ads.js\">\n";
      $temp .= "</script>\n";
      $temp .= "</td></tr><tr><td>\n";
      $temp .= "</td></tr></table>\n";
      return $temp;
    }

    /**
     * get the bottom ads to display on the page
     *
     */
    function getBottomAds() {
      $temp = "<script type=\"text/javascript\"><!--\n";
      $temp .= "google_ad_client = \"pub-3779523531306529\";\n";
      $temp .= "google_ad_width = 728;\n";
      $temp .= "google_ad_height = 90;\n";
      $temp .= "google_ad_format = \"728x90_as\";\n";
      $temp .= "google_ad_type = \"text\";\n";
      $temp .= "google_ad_channel =\"0025044588\";\n";
      $temp .= "google_color_border = \"FFCC00\";\n";
      $temp .= "google_color_bg = \"FFFFCC\";\n";
      $temp .= "google_color_link = \"0000FF\";\n";
      $temp .= "google_color_text = \"000000\";\n";
      $temp .= "google_color_url = \"008000\";\n";
      $temp .= "//--></script>\n";
      $temp .= "<script type=\"text/javascript\"\n";
      $temp .= "  src=\"http://pagead2.googlesyndication.com/pagead/show_ads.js\">\n";
      $temp .= "</script>\n";
      return $temp;
    }

    /**
     * get the HTML menu from the page
     *
     * @todo the menu items should swap out to a own file (*.xml for example)
     */
    function getMenu() {
      switch ($this->_version) {
        case WINMERGE_1_7: //if WinMerge 1.7 ...
          $temp .= "<h2><a href=\"/1.7/index.php\">WinMerge 1.7</a></h2>\n";
          $temp .= "<ul>\n";
          $temp .= "  <li><a href=\"/1.7/docs/index.html\">User's Guide</a></li>\n";
          $temp .= "  <li><a href=\"/1.7/changes.php\">Change History</a></li>\n";
          $temp .= "</ul>\n";
          $temp .= "<ul>\n";
          $temp .= "  <li>Downloads\n";
          $temp .= "    <ul>\n";
          $temp .= "      <li><a href=\"http://download.sourceforge.net/winmerge/WinMergeSetup.exe\">WinMerge</a></li>\n";
          $temp .= "      <li><a href=\"http://sourceforge.net/project/showfiles.php?group_id=13216&amp;package_id=11403&amp;release_id=14951\">Languages</a></li>\n";
          $temp .= "      <li><a href=\"http://download.sourceforge.net/winmerge/WinMergeDocs1_7.zip\">Documentation</a></li>\n";
          $temp .= "    </ul>\n";
          $temp .= "  </li>\n";
          $temp .= "</ul>\n";
          break;
        case WINMERGE_2_0: //if WinMerge 2.0 ...
          $temp .= "<h2><a href=\"/2.0/index.php\">WinMerge 2.0</a></h2>\n";
          $temp .= "<ul>\n";
          $temp .= "  <li><a href=\"/2.0/docs/index.html\">User's Guide</a></li>\n";
          $temp .= "  <li><a href=\"/2.0/changes.php\">Change History</a></li>\n";
          $temp .= "</ul>\n";
          $temp .= "<ul>\n";
          $temp .= "  <li>Downloads\n";
          $temp .= "    <ul>\n";
          $temp .= "      <li><a href=\"http://sourceforge.net/project/showfiles.php?group_id=13216&amp;package_id=11248&amp;release_id=202894\">WinMerge</a> (<a href=\"http://sourceforge.net/project/shownotes.php?release_id=202894\" title=\"Release Notes\">Notes</a>)</li>\n";
          $temp .= "      <li><a href=\"http://sourceforge.net/project/showfiles.php?group_id=13216&amp;package_id=11403&amp;release_id=177570\">Languages</a></li>\n";
          $temp .= "      <li><a href=\"http://prdownloads.sourceforge.net/winmerge/WinMergeDocs2_0.zip\">Documentation</a></li>\n";
          $temp .= "    </ul>\n";
          $temp .= "  </li>\n";
          $temp .= "</ul>\n";
          break;
        case WINMERGE_2_2: //if WinMerge 2.2 ...
          $temp .= "<h2><a href=\"/2.2/index.php\">WinMerge 2.2</a></h2>\n";
          $temp .= "<ul>\n";
          $temp .= "  <li><a href=\"/2.2/manual/index.html\">Manual</a></li>\n";
          $temp .= "  <li><a href=\"/2.2/screenshots.php\">Screenshots</a></li>\n";
          $temp .= "  <li><a href=\"/2.2/changes.php\">Change History</a></li>\n";
          $temp .= "</ul>\n";
          $temp .= "<ul>\n";
          $temp .= "  <li><a href=\"/2.2/downloads.php\">Downloads</a>\n";
          $temp .= "    <ul>\n";
          $temp .= "      <li><a href=\"http://sourceforge.net/project/showfiles.php?group_id=13216&amp;package_id=11248&amp;release_id=331334\">WinMerge</a> (<a href=\"http://sourceforge.net/project/shownotes.php?release_id=331334\" title=\"Release Notes\">Notes</a>)</li>\n";
          $temp .= "      <li><a href=\"http://sourceforge.net/project/showfiles.php?group_id=13216&amp;package_id=143957\">7-Zip Plugin</a></li>\n";
          $temp .= "      <li><a href=\"http://sourceforge.net/project/showfiles.php?group_id=13216&amp;package_id=11403&amp;release_id=283717\">Languages</a></li>\n";
          $temp .= "      <li><a href=\"http://sourceforge.net/project/showfiles.php?group_id=13216&amp;package_id=11254&amp;release_id=331358\">Documentation</a></li>\n";
          $temp .= "    </ul>\n";
          $temp .= "  </li>\n";
          $temp .= "</ul>\n";
          break;
        case WINMERGE_2_4: //if WinMerge 2.4 ...
        case WINMERGE_USR: //OR if "Users" ...
          $temp .= "<h2><a href=\"/2.4/index.php\">WinMerge 2.4</a></h2>\n";
          $temp .= "<ul>\n";
          $temp .= "  <li><a href=\"/2.4/manual/index.html\">Manual</a></li>\n";
          $temp .= "  <li><a href=\"/2.4/screenshots.php\">Screenshots</a></li>\n";
          $temp .= "  <li><a href=\"/2.4/changes.php\">Change History</a></li>\n";
          $temp .= "  <li><a href=\"/2.4/limitations.php\">Known Limitations</a></li>\n";
          $temp .= "</ul>\n";
          $temp .= "<ul>\n";
          $temp .= "  <li><a href=\"/downloads.php\">Downloads</a>\n";
          $temp .= "    <ul>\n";
          $temp .= "      <li><a href=\"http://sourceforge.net/project/showfiles.php?group_id=13216&amp;package_id=11248&amp;release_id=438554\">WinMerge</a> (<a href=\"http://sourceforge.net/project/shownotes.php?release_id=438554\" title=\"Release Notes\">Notes</a>)</li>\n";
          $temp .= "      <li><a href=\"http://sourceforge.net/project/showfiles.php?group_id=13216&amp;package_id=143957\">7-Zip Plugin</a></li>\n";
          $temp .= "      <li><a href=\"http://sourceforge.net/project/showfiles.php?group_id=13216&amp;package_id=11254&amp;release_id=438564\">Documentation</a></li>\n";
          $temp .= "    </ul>\n";
          $temp .= "  </li>\n";
          $temp .= "</ul>\n";
          break;
        case WINMERGE_DEV: //if "Developers" ...
          $temp .= "<h2><a href=\"/developer/index.php\">Developers</a></h2>\n";
          $temp .= "<ul>\n";
          $temp .= "  <li><a href=\"http://sourceforge.net/projects/winmerge/\">Project Home</a></li>\n";
          $temp .= "  <li>CVS\n";
          $temp .= "    <ul>\n";
          $temp .= "      <li><a href=\"http://winmerge.cvs.sourceforge.net/winmerge/\">Browse CVS Tree</a></li>\n";
          $temp .= "      <li><a href=\"http://sourceforge.net/cvs/?group_id=13216\">CVS Client Access</a></li>\n";
          $temp .= "    </ul>\n";
          $temp .= "  </li>\n";
          $temp .= "  <li><a href=\"/developer/l10n/index.php\">Translations</a>\n";
          $temp .= "    <ul>\n";
          $temp .= "      <li><a href=\"/developer/l10n/instructions.php\">Instructions</a></li>\n";
          $temp .= "      <li><a href=\"/developer/l10n/status.php\">Status</a></li>\n";
          $temp .= "    </ul>\n";
          $temp .= "  </li>\n";
          $temp .= "  <li>Downloads\n";
          $temp .= "    <ul>\n";
          $temp .= "      <li><a href=\"http://sourceforge.net/project/showfiles.php?group_id=13216&amp;package_id=11248\">Stable versions</a></li>\n";
          $temp .= "      <li><a href=\"http://sourceforge.net/project/showfiles.php?group_id=13216&amp;package_id=29158\">Beta versions</a></li>\n";
          $temp .= "      <li><a href=\"http://sourceforge.net/project/showfiles.php?group_id=13216&amp;package_id=92246\">Experimental builds</a></li>\n";
          $temp .= "    </ul>\n";
          $temp .= "  </li>\n";
          $temp .= "</ul>\n";
          break;
      }
      $temp .= "<ul>\n";
      $temp .= "  <li><a href=\"/links.php\">Links</a></li>\n";
      $temp .= "</ul>\n";
      $temp .= "<p align=\"center\">\n";
      $temp .= "  <a href=\"http://sourceforge.net/donate/index.php?group_id=13216\">\n";
      $temp .= "    <img src=\"/images/project-support.jpg\" width=\"88\" height=\"32\" border=\"0\" alt=\"Donate money to WinMerge\">\n";
      $temp .= "  </a>\n";
      $temp .= "</p>\n";
      $temp .= "<h2>Support</h2>\n";
      $temp .= "<ul>\n";
      $temp .= "  <li><a href=\"http://sourceforge.net/mail/?group_id=13216\">Mailing Lists</a></li>\n";
      $temp .= "  <li><a href=\"http://sourceforge.net/forum/?group_id=13216\">Discussion Forums</a></li>\n";
      switch ($this->_version) {
        case WINMERGE_2_0: //if WinMerge 2.0 ...
          $temp .= "  <li><a href=\"/2.0/docs/faq.htm\" title=\"Frequently asked questions\">FAQ</a></li>\n";
          break;
        case WINMERGE_2_2: //if WinMerge 2.2 ...
          $temp .= "  <li><a href=\"/2.2/manual/faq.html\" title=\"Frequently asked questions\">FAQ</a></li>\n";
          break;
        case WINMERGE_2_4: //if WinMerge 2.4 ...
        case WINMERGE_USR: //OR if "Users" ...
        case WINMERGE_DEV: //OR if "Developers" ...
          $temp .= "  <li><a href=\"/2.4/manual/faq.html\" title=\"Frequently asked questions\">FAQ</a></li>\n";
          break;
      }
      $temp .= "</ul>\n";
      $temp .= "<ul>\n";
      $temp .= "  <li><a href=\"http://sourceforge.net/tracker/?group_id=13216&amp;atid=113216\">Submit a Bug Report</a></li>\n";
      $temp .= "  <li><a href=\"http://sourceforge.net/tracker/?group_id=13216&amp;atid=363216\">Submit a Feature Request</a></li>\n";
      $temp .= "  <li><a href=\"http://sourceforge.net/tracker/?group_id=13216&amp;atid=213216\">Submit a Support Request</a></li>\n";
      $temp .= "  <li><a href=\"http://sourceforge.net/tracker/?group_id=13216&amp;atid=313216\">Submit a Patch</a></li>\n";
      $temp .= "</ul>\n";
      $temp .= "<p align=\"center\">\n";
      $temp .= "  <a href=\"/WinMergePAD.php\">\n";
      $temp .= "    <img src=\"/images/padbutton.gif\" alt=\"PAD Enabled\" border=\"0\">\n";
      $temp .= "  </a>\n";
      $temp .= "</p>\n";
      $temp .= "<p align=\"center\">\n";
      $temp .= "  <a href=\"http://sourceforge.net/projects/winmerge\">\n";
      $temp .= "    <img src=\"/images/sourceforge.png\" alt=\"SourceForge Logo\" border=\"0\">\n";
      $temp .= "  </a>\n";
      $temp .= "</p>\n";
      $temp .= "<p align=\"center\">\n";
      $temp .= "<script type=\"text/javascript\">\n";
      $temp .= "<!--\n";
      $temp .= "google_ad_client = \"pub-3779523531306529\";\n";
      $temp .= "google_ad_width = 125;\n";
      $temp .= "google_ad_height = 125;\n";
      $temp .= "google_ad_format = \"125x125_as_rimg\";\n";
      $temp .= "google_cpa_choice = \"CAAQo-aZzgEaCM3CU97Siy5UKK2293M\";\n";
      $temp .= "google_ad_channel = \"9515761622\";\n";
      $temp .= "//-->\n";
      $temp .= "</script>\n";
      $temp .= "<script type=\"text/javascript\" src=\"http://pagead2.googlesyndication.com/pagead/show_ads.js\">\n";
      $temp .= "</script>\n";
      $temp .= "</p>\n";
      return $temp;
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
        
        $parser = xml_parser_create();
        xml_parser_set_option($parser, XML_OPTION_SKIP_WHITE, 1);
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
     * convert a changelog file into html code
     */
    function convertChanges2Html($filename) {
      $html = "<pre>\n";
      if (file_exists($filename) == true) {
        $lines = file($filename);
        $linescount = sizeof($lines);
        
        for ($i = 0; $i < $linescount; $i++) {
          $line = $lines[$i];
          
          $line = preg_replace('#^([0-9][0-9][0-9][0-9]-[0-9][0-9]-[0-9][0-9])#si', '<b>' . '\\1' . '</b>', $line);
          $line = preg_replace('#PATCH: \[ ([0-9]*) \]#si', 'PATCH: [ <a href="http://sourceforge.net/tracker/index.php?func=detail&aid=\\1&group_id=13216&atid=313216" target="_blank">\\1</a> ]', $line);
          $line = preg_replace('#BUG: \[ ([0-9]*) \]#si', 'BUG: [ <a href="http://sourceforge.net/tracker/index.php?func=detail&aid=\\1&group_id=13216&atid=113216" target="_blank">\\1</a> ]', $line);
          $line = preg_replace('#RFE: \[ ([0-9]*) \]#si', 'RFE: [ <a href="http://sourceforge.net/tracker/index.php?func=detail&aid=\\1&group_id=13216&atid=363216" target="_blank">\\1</a> ]', $line);
        
          $html .= $line;
        }
      }
      $html .= "</pre>\n";
      
      return $html;
    }
  }
?>