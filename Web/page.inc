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
   */
  class Page {
    var $_title, $_charset;
    var $_description, $_keywords;

    /**
     * Constructor
     */
    function Page() {
      $this->_title = 'Welcome to WinMerge on SourceForge!';
      $this->_charset = 'iso-8859-1';
      $this->_description = 'WinMerge HomePage: Visual text file comparison and merging (Open Source software).';
      $this->_keywords = 'WinMerge, free, download, Beyond Compare, Scooter Software, DLSuperC, DLSubCF, Component Software Diff, Viff, TreeDiff, FileSync, WMatch, Quick Spot, NTDMatch, windiff, WDIFF, FolderMatch, Merge 99, Merge99, PMdiff, PMDiff, PMDIFF, pmdiff, visual, visually, file, folder, directory, synchronization, comparison, graphical, document, text, compare, diff, difference, revision, Araxis, compare folders, compare files, synchronize folders, merge files, merge folders, Y2K, Year 2000, Year 2K, side-by-side, VFC, GFC, gdiff, CSDiff, OS2GFC, vdiff, ediff, fc';
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
     * print the head from the page
     */
    function printHead($title = '') {
      if ($title != '') {
        $this->_title = $title;
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
     * ...
     */
    function getMenu() {
      $temp = "<p class=\"lefthead\"><i>WinMerge on </i><br>\n";
      $temp .= "  <a href=\"http://sourceforge.net/projects/winmerge\" target=\"_top\">\n";
      $temp .= "    <IMG src=\"http://sourceforge.net/sflogo.php?group_id=13216\" border=\"0\" alt=\"SourceForge Logo\">\n";
      $temp .= "  </a>\n";
      $temp .= "</p>\n";
      $temp .= "<p class=\"lefthead\"><a href=\"http://winmerge.org/index.php\">Users</a></p>\n";
      $temp .= "<p class=\"lefttext\"><A href=\"http://winmerge.org/docs20/index.html\" target=\"_top\">WinMerge User's Guide</a><br>\n";
      $temp .= "  <a href=\"http://sourceforge.net/mail/?group_id=13216\" target=\"_top\">Mailing Lists</a><br>\n";
      $temp .= "  <a href=\"http://sourceforge.net/forum/?group_id=13216\" target=\"_top\">Discussion Forums</a>\n";
      $temp .= "</p>\n";
      $temp .= "<p class=\"lefttext\">\n";
      $temp .= "  <a href=\"http://sourceforge.net/tracker/?group_id=13216&amp;atid=113216\" target=\"_top\">Submit a Bug Report</a><br>\n";
      $temp .= "  <a href=\"http://sourceforge.net/tracker/?group_id=13216&amp;atid=363216\" target=\"_top\">Submit a Feature Request</a><br>\n";
      $temp .= "  <a href=\"http://winmerge.org/WinMergeChanges.html\" target=\"_top\">Change History</a><br><br>\n";
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
      $temp .= "<p class=\"lefttext\">\n";
      $temp .= "  <a href=\"http://sourceforge.net/project/showfiles.php?group_id=13216&amp;package_id=11248\" target=\"_top\">WinMerge</a><br>\n";
      $temp .= "  <a href=\"http://sourceforge.net/project/showfiles.php?group_id=13216&amp;package_id=11403\" target=\"_top\">Language DLL's</a><br>\n";
      $temp .= "  <a href=\"http://sourceforge.net/project/showfiles.php?group_id=13216&amp;package_id=11254\" target=\"_top\">Documentation</a>\n";
      $temp .= "</p>\n";
      $temp .= "<p class=\"lefttext\">\n";
      $temp .= "  <a href=\"http://sourceforge.net/project/showfiles.php?group_id=13216&amp;package_id=29158\" target=\"_top\">WinMerge Beta builds</a><br>\n";
      $temp .= "  <a href=\"http://sourceforge.net/project/showfiles.php?group_id=13216&amp;package_id=92246\" target=\"_top\">WinMerge Experimental builds</a>\n";
      $temp .= "</p>\n";
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
  }
?>