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

  define ('WINMERGE_1_7', '1.7');
  define ('WINMERGE_2_0', '2.0');
  define ('WINMERGE_2_2', '2.2');

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
      $this->_title = 'Welcome to WinMerge on SourceForge!';
      $this->_charset = 'ISO-8859-1';
      $this->_description = 'WinMerge HomePage: Visual text file comparison and merging (Open Source software).';
      $this->_keywords = 'WinMerge, free, download, Beyond Compare, Scooter Software, DLSuperC, DLSubCF, Component Software Diff, Viff, TreeDiff, FileSync, WMatch, Quick Spot, NTDMatch, windiff, WDIFF, FolderMatch, Merge 99, Merge99, PMdiff, PMDiff, PMDIFF, pmdiff, visual, visually, file, folder, directory, synchronization, comparison, graphical, document, text, compare, diff, difference, revision, Araxis, compare folders, compare files, synchronize folders, merge files, merge folders, Y2K, Year 2000, Year 2K, side-by-side, VFC, GFC, gdiff, CSDiff, OS2GFC, vdiff, ediff, fc';
      $this->_version = WINMERGE_2_0;
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
      $temp .= "  <link rel=\"stylesheet\" type=\"text/css\" href=\"/winmerge.css\">\n";
      $temp .= "  <link rel=\"shortcut icon\" type=\"image/x-icon\" href=\"/favicon.ico\">\n";
      $temp .= "</head>\n\n";
      $temp .= "<body>\n";
      $temp .= "<table border=\"0\" cellpadding=\"4\">\n";
      $temp .= "  <tr>\n";
      $temp .= "    <td width=\"200\" valign=\"top\" bgcolor=\"#c0c0c0\">\n";
      $temp .= $this->getMenu();
      $temp .= "    </td>\n\n";
      $temp .= "    <td width=\"*\" valign=\"top\">\n";
      print($temp);
    }

    /**
     * print the foot from the page
     */
    function printFoot() {
      $temp = "    </td>\n";
      $temp .= "  </tr>\n";
      $temp .= "</table>\n";
      $temp .= "</body>\n\n";
      $temp .= "</html>\n";
      print($temp);
    }

    /**
     * get the HTML menu from the page
     *
     * @todo the menu items should swap out to a own file (*.xml for example)
     */
    function getMenu() {
      $temp = "<p class=\"lefthead\"><i>WinMerge on </i><br>\n";
      $temp .= "  <a href=\"http://sourceforge.net/projects/winmerge\" target=\"_top\">\n";
      $temp .= "    <IMG src=\"http://sourceforge.net/sflogo.php?group_id=13216\" border=\"0\" alt=\"SourceForge Logo\">\n";
      $temp .= "  </a>\n";
      $temp .= "</p>\n";
      $temp .= "<p class=\"lefthead\"><a href=\"http://winmerge.org/index.php\">Users</a></p>\n";
      switch ($this->_version) {
        case WINMERGE_1_7: //if WinMerge 1.7 ...
          $temp .= "<p class=\"lefttext\"><a href=\"/1.7/docs/index.html\">WinMerge User's Guide</a><br>\n";
          break;
        case WINMERGE_2_0: //if WinMerge 2.0 ...
          $temp .= "<p class=\"lefttext\"><a href=\"/2.0/docs/index.html\">WinMerge User's Guide</a><br>\n";
          break;
        case WINMERGE_2_2: //if WinMerge 2.2 ...
          $temp .= "<p class=\"lefttext\"><a href=\"/2.2/manual/index.html\">WinMerge Manual</a><br>\n";
          break;
      }
      $temp .= "  <a href=\"http://sourceforge.net/mail/?group_id=13216\" target=\"_top\">Mailing Lists</a><br>\n";
      $temp .= "  <a href=\"http://sourceforge.net/forum/?group_id=13216\" target=\"_top\">Discussion Forums</a>\n";
      $temp .= "</p>\n";
      $temp .= "<p class=\"lefttext\">\n";
      $temp .= "  <a href=\"http://sourceforge.net/tracker/?group_id=13216&amp;atid=113216\" target=\"_top\">Submit a Bug Report</a><br>\n";
      $temp .= "  <a href=\"http://sourceforge.net/tracker/?group_id=13216&amp;atid=363216\" target=\"_top\">Submit a Feature Request</a><br>\n";
      switch ($this->_version) {
        case WINMERGE_1_7: //if WinMerge 1.7 ...
          $temp .= "  <a href=\"/1.7/changes.php\">Change History</a><br><br>\n";
          break;
        case WINMERGE_2_0: //if WinMerge 2.0 ...
          $temp .= "  <a href=\"/2.0/changes.php\">Change History</a><br><br>\n";
          break;
        case WINMERGE_2_2: //if WinMerge 2.2 ...
          $temp .= "  <a href=\"/2.2/changes.php\">Change History</a><br><br>\n";
          break;
      }
      $temp .= "  <center><a href=\"http://sourceforge.net/donate/index.php?group_id=13216\" target=\"_top\">\n";
      $temp .= "    <img src=\"http://images.sourceforge.net/images/project-support.jpg\"\n";
      $temp .= "      width=\"88\" height=\"32\" border=\"0\" alt=\"Donate money to WinMerge\" />\n";
      $temp .= "  </a></center>\n";
      $temp .= "</p>\n";
      $temp .= "<p class=\"lefthead\"><a href=\"http://winmerge.org/developer/index.php\">Developers</a></p>\n";
      $temp .= "<p class=\"lefttext\"><a href=\"http://sourceforge.net/projects/winmerge/\" target=\"_top\">Project Home</a><br>\n";
      $temp .= "  <a href=\"http://cvs.sourceforge.net/viewcvs.py/winmerge/\" target=\"_top\">Go to CVSWeb</a><br>\n";
      $temp .= "  <a href=\"http://sourceforge.net/cvs/?group_id=13216\" target=\"_top\">CVS Client Access</a><br>\n";
      $temp .= "  <a href=\"http://winmerge.org/developer/wincvs.php\" target=\"_top\">WinCVS Hints</a><br>\n";
      $temp .= "  <a href=\"http://sourceforge.net/tracker/?group_id=13216&amp;atid=313216\" target=\"_top\">Submit a Patch</a><br>\n";
      $temp .= "  <a href=\"http://winmerge.org/developer/translate.php\" target=\"_top\">Translate WinMerge!</a>\n";
      $temp .= "</p>\n";
      $temp .= "<p class=\"lefthead\">Downloads</p>\n";
      switch ($this->_version) {
        case WINMERGE_1_7: //if WinMerge 1.7 ...
          $temp .= "<p class=\"lefttext\">\n";
          $temp .= "  <a href=\"http://download.sourceforge.net/winmerge/WinMergeSetup.exe\">WinMerge Installer</a><br>\n";
          $temp .= "  <a href=\"http://download.sourceforge.net/winmerge/WinMergeDocs1_7.zip\">Documentation</a>\n";
          $temp .= "</p>\n";
          $temp .= "<p class=\"lefttext\">\n";
          $temp .= "  <a href=\"http://download.sourceforge.net/winmerge/MergeCzech.zip\">Czech DLL</a><br>\n";
          $temp .= "  <a href=\"http://download.sourceforge.net/winmerge/MergeDutch.zip\">Dutch DLL</a><br>\n";
          $temp .= "  <a href=\"http://download.sourceforge.net/winmerge/MergeFrench.zip\">French DLL</a><br>\n";
          $temp .= "  <a href=\"http://download.sourceforge.net/winmerge/MergeGerman.zip\">German DLL</a><br>\n";
          $temp .= "  <a href=\"http://download.sourceforge.net/winmerge/MergeItalian.zip\">Italian DLL</a><br>\n";
          $temp .= "  <a href=\"http://download.sourceforge.net/winmerge/MergeSpanish.zip\">Spanish DLL</a>\n";
          $temp .= "</p>\n";
          break;
        case WINMERGE_2_0: //if WinMerge 2.0 ...
          $temp .= "<p class=\"lefttext\">\n";
          $temp .= "  <a href=\"http://sourceforge.net/project/showfiles.php?group_id=13216&amp;package_id=11248\">WinMerge</a><br>\n";
          $temp .= "  <a href=\"http://sourceforge.net/project/showfiles.php?group_id=13216&amp;package_id=11403\">Language DLL's</a><br>\n";
          $temp .= "  <a href=\"http://sourceforge.net/project/showfiles.php?group_id=13216&amp;package_id=11254\">Documentation</a>\n";
          $temp .= "</p>\n";
          $temp .= "<p class=\"lefttext\">\n";
          $temp .= "  <a href=\"http://sourceforge.net/project/showfiles.php?group_id=13216&amp;package_id=29158\">WinMerge Beta versions</a><br>\n";
          $temp .= "  <a href=\"http://sourceforge.net/project/showfiles.php?group_id=13216&amp;package_id=92246\">WinMerge Experimental builds</a>\n";
          $temp .= "</p>\n";
          break;
        case WINMERGE_2_2: //if WinMerge 2.2 ...
          $temp .= "<p class=\"lefttext\">\n";
          $temp .= "  <a href=\"http://sourceforge.net/project/showfiles.php?group_id=13216&package_id=29158&release_id=273507\">WinMerge Release Candidate</a><br>\n";
          //$temp .= "  <a href=\"http://sourceforge.net/project/showfiles.php?group_id=13216&amp;package_id=11248\">WinMerge</a><br>\n";
          //$temp .= "  <a href=\"http://sourceforge.net/project/showfiles.php?group_id=13216&amp;package_id=11403\">Language DLL's</a><br>\n";
          //$temp .= "  <a href=\"http://sourceforge.net/project/showfiles.php?group_id=13216&amp;package_id=11254\">Documentation</a>\n";
          $temp .= "</p>\n";
          $temp .= "<p class=\"lefttext\">\n";
          $temp .= "  <a href=\"http://sourceforge.net/project/showfiles.php?group_id=13216&amp;package_id=29158\">WinMerge Beta versions</a><br>\n";
          $temp .= "  <a href=\"http://sourceforge.net/project/showfiles.php?group_id=13216&amp;package_id=92246\">WinMerge Experimental builds</a>\n";
          $temp .= "</p>\n";
          break;
      }
      $temp .= "<p align=\"center\">\n";
      $temp .= "  <a href=\"http://i3t.org/sponsorship/showaward.jsp?id=1595259\" target=\"_top\">\n";
      $temp .= "    <img src=\"http://i3t.org/sponsorship/emblem.jsp?id=1595259\"\n";
      $temp .= "      alt=\"The I3T Award for Software Excellence\">\n";
      $temp .= "  </a>\n";
      $temp .= "</p>\n";
      $temp .= "<p align=\"center\">\n";
      $temp .= "  <a href=\"http://validator.w3.org/check/referer\">\n";
      $temp .= "    <img border=\"0\" src=\"http://www.w3.org/Icons/valid-html401.gif\" alt=\"Valid HTML 4.01!\" height=\"31\" width=\"88\">\n";
      $temp .= "  </a>\n";
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