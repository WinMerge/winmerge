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

  include 'tabnav.inc';
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
      $this->_title = '';
      $this->_charset = 'UTF-8';
      $this->_description = '';
      $this->_keywords = '';
      $this->_rssfeeds = array();
      $this->_tab = TAB_HOME;
      /* _Stable Release */
      $this->_stablerelease = new Release;
      $this->_stablerelease->setVersionNumber('2.12.4');
      $this->_stablerelease->setDate('2009-06-09');
      $this->_stablerelease->addDownload('setup.exe', 'http://downloads.sourceforge.net/winmerge/WinMerge-2.12.4-Setup.exe', 3212078, '345e957fc483d5eba8d0e99010a9c24e588aeaa1');
      $this->_stablerelease->addDownload('exe.zip', 'http://downloads.sourceforge.net/winmerge/WinMerge-2.12.4-exe.zip', 3504710, '4c721b1b1c2c36e1e9b013d5c8909783d3ab26f0');
      $this->_stablerelease->addDownload('exe.7z', 'http://downloads.sourceforge.net/winmerge/WinMerge-2.12.4-exe.7z', 1967484, '03e568fe8baa2dabe950729464fddf7291a3e74b');
      $this->_stablerelease->addDownload('rt.zip', 'http://downloads.sourceforge.net/winmerge/Runtimes-2.12.4.zip', 1287520, '27a9fd85b3bd92d69fea31aec6129c9bd08ea426');
      $this->_stablerelease->addDownload('rt.7z', 'http://downloads.sourceforge.net/winmerge/Runtimes-2.12.4.7z', 753831, '09e029d03dd34fb0ec1a734d5f32d844da728aac');
      $this->_stablerelease->addDownload('src.zip', 'http://downloads.sourceforge.net/winmerge/WinMerge-2.12.4-src.zip', 9378049, 'd51d6d6d78df761fbe07463b07bbf23eeabedb60');
      $this->_stablerelease->addDownload('src.7z', 'http://downloads.sourceforge.net/winmerge/WinMerge-2.12.4-src.7z', 4887115, '6287dfc44c82ec7c675c023fe436362a9469407e');
      $this->_stablerelease->setBranchName('R2_12');
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
    function printHead($title = '', $tab = '', $onload = '') {
      global $translations;
      
      $this->_title = $title;
      if ($tab != '') {
        $this->_tab = $tab;
      }
      $temp = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\"\n";
      $temp .= "  \"http://www.w3.org/TR/html4/loose.dtd\">\n";
      $temp .= "<html lang=\"" . $translations->getActiveLanguageId() . "\">\n\n";
      $temp .= "<head>\n";
      if ($this->_title != '') {
        $temp .= "  <title>" . $this->_title . " - WinMerge</title>\n";
      }
      else {
        $temp .= "  <title>WinMerge</title>\n";
      }
      $temp .= "  <meta http-equiv=\"content-type\" content=\"text/html; charset=" . $this->_charset . "\">\n";
      if ($this->_description != '') {
        $temp .= "  <meta name=\"description\" content=\"" . $this->_description . "\">\n";
      }
      if ($this->_keywords != '') {
        $temp .= "  <meta name=\"keywords\" content=\"" . $this->_keywords . "\">\n";
      }
      $temp .= "  <style type=\"text/css\" media=\"screen\">@import \"/engine/css/screen.css\";</style>\n";
      $temp .= "  <link rel=\"stylesheet\" type=\"text/css\" media=\"print\" href=\"/engine/css/print.css\">\n";
      $temp .= "  <script src=\"/engine/javascript/winmerge.js\" type=\"text/javascript\"></script>\n";
      $temp .= "  <link rel=\"shortcut icon\" type=\"image/x-icon\" href=\"/favicon.ico\">\n";
      foreach ($this->_rssfeeds as $rssurl => $rsstitle) { //for all RSS feeds...
        $temp .= "  <link rel=\"alternate\" type=\"application/rss+xml\" title=\"" . $rsstitle . "\" href=\"" . $rssurl . "\">\n";
      }
      if ($translations->isBaseLanguage()) { //if base language...
        $temp .= "  <link rel=\"canonical\" href=\"" . $translations->getLink() . "\"/>\n";
      }
      $temp .= "</head>\n\n";
      if ($onload != '')
        $temp .= "<body onload=\"".$onload."\">\n";
      else
        $temp .= "<body>\n";
      $temp .= "<div id=\"wrap\">\n";
      $temp .= "  <div id=\"head\">\n";
      $temp .= "<h1>WinMerge</h1>\n";
      $temp .= "  </div> <!-- #head -->\n";
      $temp .= $this->getTabs();
      $temp .= $this->getAds();
      $temp .= "  <div id=\"content\">\n";
      print($temp);
    }

    /**
     * print the foot from the page
     */
    function printFoot() {
      $temp = $this->getSocialBookmarks();
      $temp .= "  </div> <!-- #content -->\n";
      $temp .= $this->getLanguages();
      $temp .= "<p id=\"poweredby\">\n";
      $temp .= "  <a href=\"http://sourceforge.net/projects/winmerge\"><img src=\"http://sflogo.sourceforge.net/sflogo.php?group_id=13216&amp;type=11\" width=\"120\" height=\"30\" border=\"0\" alt=\"SourceForge.net\" title=\"Get WinMerge at SourceForge.net. Fast, secure and Free Open Source software downloads\" /></a>\n";
      $temp .= "  <a href=\"http://sourceforge.net/donate/index.php?group_id=13216\"><img src=\"/engine/images/project-support.jpg\" width=\"88\" height=\"32\" border=\"0\" alt=\"Donate\" title=\"Donate money to WinMerge\"></a>\n";
      $temp .= "</p>\n";
      $temp .= "</div> <!-- #wrap -->\n";
      $temp .= $this->getPiwikCode();
      $temp .= "</body>\n\n";
      $temp .= "</html>\n";
      print($temp);
    }

    /**
     * print a <h2> heading
     */
    function printHeading($title) {
      print("<h2>" . $title . "</h2>\n");
    }

    /**
     * print a <h2> heading with a RSS feed link
     */
    function printRssHeading($title, $rssfeedurl) {
      print("<h2>" . $title . " " . $this->getRssFeedLink($rssfeedurl) . "</h2>\n");
    }

    /**
     * print a <h3> heading
     */
    function printSubHeading($title) {
      print("<h3>" . $title . "</h3>\n");
    }

    /**
     * print a linked <h3> heading
     */
    function printLinkedSubHeading($title, $link) {
      print("<h3><a href=\"" . $link . "\">" . $title . "</a></h3>\n");
    }

    /**
     * print a <h3> heading with a RSS feed link
     */
    function printRssSubHeading($title, $rssfeedurl) {
      print("<h3>" . $title . " " . $this->getRssFeedLink($rssfeedurl) . "</h3>\n");
    }

    /**
     * print a <h4> heading
     */
    function printSubSubHeading($title) {
      print("<h4>" . $title . "</h4>\n");
    }

    /**
     * print a paragraph
     */
    function printPara() {
      print("<p>");
      $numargs = func_num_args();
      $args = func_get_args();
      for ($i = 0; $i < $numargs; $i++) { //for all arguments...
        if ($i > 0) //if NOT first argument...
          print(' ' . $args[$i]);
        else //if first argument...
          print($args[$i]);
      }
      print("</p>\n");
    }

    /**
     * print the "Download Now!" link
     */
    function printDownloadNow() {
      $temp = "<p><a class=\"downloadnow\" href=\"" . $this->_stablerelease->getDownload('setup.exe') . "\">";
      $temp .= "<strong>" . __('Download Now!') . "</strong> ";
      $temp .= "<em>" . __('Version %1$s; %2$s MB', $this->_stablerelease->getVersionNumber(), $this->_stablerelease->getDownloadSizeMb('setup.exe')) . "</em>";
      $temp .= "</a></p>\n";
      print($temp);
    }

    /**
     * get a RSS feed link
     */
    function getRssFeedLink($url, $text = '') {
      $temp = "<a href=\"" . $url . "\"><img src=\"/engine/images/rss.png\" alt=\"RSS\" border=\"0\"></a>";
      if ($text != '') { //if WITH text...
        $temp .= " <a href=\"" . $url . "\"> " . $text . "</a>";
      }
      return $temp;
    }

    /**
     * print a RSS feed link
     */
    function printRssFeedLink($url, $text = '') {
      print($this->getRssFeedLink($url, $text));
    }

    /**
     * get the tabs menu from the page
     */
    function getTabs() {
      global $translations;
      
      /* Main Tabs */
      $tabs[0] = new Tab(TAB_HOME, $translations->prepareLink('/'), __('Home'), __('WinMerge Home'));
      $tabs[1] = new Tab(TAB_ABOUT, $translations->prepareLink('/about/'), __('About'), __('About WinMerge'));
      $tabs[2] = new Tab(TAB_DOCS, $translations->prepareLink('/docs/'), __('Documentation'), __('WinMerge Documentation'));
      $tabs[3] = new Tab(TAB_DOWNLOADS, $translations->prepareLink('/downloads/'), __('Downloads'), __('WinMerge Downloads'));
      $tabs[4] = new Tab(TAB_SUPPORT, $translations->prepareLink('/support/'), __('Support'), __('WinMerge Support'));
      $tabs[5] = new Tab(TAB_TRANSLATIONS, $translations->prepareLink('/translations/'), __('Translations'), __('WinMerge Translations'));
      
      $temp = "<ul id=\"tabs\">\n";
      foreach ($tabs as $tab) { //for all tabs...
        if ($this->_tab == $tab->getId()) { //if current tab...
          $temp .= "  <li><strong><a href=\"" . $tab->getLink() . "\" id=\"current\" title=\"" . $tab->getToolTipText() . "\">" . $tab->getTitle() . "</a></strong></li>\n";
        }
        else { //if NOT current tab...
          $temp .= "  <li><a href=\"" . $tab->getLink() . "\" title=\"" . $tab->getToolTipText() . "\">" . $tab->getTitle() . "</a></li>\n";
        }
      }
      $temp .= "</ul>\n";
      
      /* Sub Tabs */
      $subtabs = array();
      switch ($this->_tab) {
        case TAB_HOME: //if "Home" ...
          $subtabs[0] = new Subtab($translations->prepareLink('/'), __('Index'), __('WinMerge Home'));
          $subtabs[1] = new Subtab($translations->prepareLink('/links.php'), __('Links'), __('WinMerge Links'));
          $subtabs[2] = new Subtab('http://sourceforge.net/projects/winmerge', __('Project Page'), __('WinMerge Project Page'));
          break;
        case TAB_ABOUT: //if "About" ...
          $subtabs[0] = new Subtab($translations->prepareLink('/about/'), __('Index'), __('About WinMerge'));
          $subtabs[1] = new Subtab($translations->prepareLink('/about/screenshots/'), __('Screenshots'), __('WinMerge Screenshots'));
          $subtabs[2] = new Subtab($translations->prepareLink('/about/history.php'), __('History'), __('WinMerge History'));
          $subtabs[3] = new Subtab($translations->prepareLink('/about/license.php'), __('License'), __('WinMerge License'));
          break;
        case TAB_DOCS: //if "Documentation" ...
          $subtabs[0] = new Subtab($translations->prepareLink('/docs/'), __('Index'), __('WinMerge Documentation'));
          $subtabs[1] = new Subtab($translations->prepareLink('/docs/manual/'), __('Manual'), __('WinMerge Manual'));
          $subtabs[2] = new Subtab($translations->prepareLink('/docs/releasenotes.php'), __('Release Notes'), __('WinMerge Release Notes'));
          $subtabs[3] = new Subtab($translations->prepareLink('/docs/releasenotes.php#issues'), __('Known Issues'), __('WinMerge Known Issues'));
          $subtabs[4] = new Subtab($translations->prepareLink('/docs/changelog.php'), __('Change Log'), __('WinMerge Change Log'));
          $subtabs[5] = new Subtab('/Wiki/', __('Development Wiki'), __('WinMerge Development Wiki'));
          break;
        case TAB_DOWNLOADS: //if "Downloads" ...
          $subtabs[0] = new Subtab($translations->prepareLink('/downloads/'), __('WinMerge'), __('Download WinMerge'));
          $subtabs[1] = new Subtab($translations->prepareLink('/downloads/plugins.php'), __('Plugins'), __('Download WinMerge Plugins'));
          $subtabs[2] = new Subtab($translations->prepareLink('/downloads/source-code.php'), __('Source Code'), __('Download WinMerge Source Code'));
          $subtabs[3] = new Subtab($translations->prepareLink('/downloads/pad-file.php'), __('PAD File'), __('WinMerge PAD File'));
          break;
        case TAB_SUPPORT: //if "Support" ...
          $subtabs[0] = new Subtab($translations->prepareLink('/support/'), __('Index'), __('WinMerge Support'));
          $subtabs[1] = new Subtab('http://forums.winmerge.org/', __('Discussion Forums'), __('WinMerge Discussion Forums'));
          $subtabs[2] = new Subtab($translations->prepareLink('/support/mailing-lists.php'), __('Mailing Lists'), __('WinMerge Mailing Lists'));
          $subtabs[3] = new Subtab('http://sourceforge.net/tracker/?group_id=13216', __('Tracker'), __('WinMerge Tracker'));
          $subtabs[4] = new Subtab($translations->prepareLink('/support/tracker-shortcuts.php'), __('Tracker Shortcuts'), __('WinMerge Tracker Shortcuts'));
          $subtabs[5] = new Subtab('http://sourceforge.net/donate/index.php?group_id=13216', __('Donate'), __('Donate money to WinMerge'));
          break;
        case TAB_TRANSLATIONS: //if "Translations" ...
          $subtabs[0] = new Subtab($translations->prepareLink('/translations/'), __('Index'), __('WinMerge Translations'));
          $subtabs[1] = new Subtab($translations->prepareLink('/translations/instructions.php'), __('Instructions'), __('WinMerge Translations Instructions'));
          $subtabs[2] = new Subtab($translations->prepareLink('/translations/status_branch.php'), __('Status (Stable Branch)'), __('WinMerge Translations Status (Stable Branch)'));
          $subtabs[3] = new Subtab($translations->prepareLink('/translations/status_trunk.php'), __('Status (Unstable Trunk)'), __('WinMerge Translations Status (Unstable Trunk)'));
          break;
      }
      
      if (count($subtabs) > 0) { //if subtabs exists...
        $temp .= "<ul id=\"subtabs\">\n";
        foreach ($subtabs as $subtab) { //for all subtabs...
          if ($subtab->isCurrent() == true) { //if current subtab...
            $temp .= "  <li><strong><a href=\"" . $subtab->getLink() . "\" title=\"" . $subtab->getToolTipText() . "\">" . $subtab->getTitle() . "</a></strong></li>\n";
          }
          else { //if NOT current subtab...
            $temp .= "  <li><a href=\"" . $subtab->getLink() . "\" title=\"" . $subtab->getToolTipText() . "\">" . $subtab->getTitle() . "</a></li>\n";
          }
        }
        $temp .= "</ul>\n";
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
     * get the social bookmarks code
     */
    function getSocialBookmarks() {
      $temp = "<div id=\"bookmarks\">\n";
      $temp .= "<img src=\"http://static.delicious.com/img/delicious.small.gif\" height=\"10\" width=\"10\" alt=\"Delicious\">\n";
      $temp .= "<a href=\"http://delicious.com/save\" onclick=\"window.open('http://delicious.com/save?v=5&amp;noui&amp;jump=close&amp;url='+encodeURIComponent(location.href)+'&amp;title='+encodeURIComponent(document.title), 'delicious','toolbar=no,width=550,height=550'); return false;\"> " . __('Bookmark this on Delicious') . "</a>\n";
      $temp .= "</div> <!-- #bookmarks -->\n";
      return $temp;
    }

    /**
     * get the languages from the page
     */
    function getLanguages() {
      global $translations;
      
      $temp = "";
      if ($translations->getLanguagesCount() > 1) { //if at least 2 languages exists...
        $temp .= "<div id=\"languages\">\n";
        $temp .= "<ul>\n";
        $languages = $translations->getLanguages();
        foreach ($languages as $language) { //for all languages...
          if ($translations->isActiveLanguage($language)) { //if current language...
            $temp .= "  <li><strong><a href=\"" . $translations->getLink($language) . "\">" . __($language->getName()) . "</a></strong></li>\n";
          }
          else { //if NOT current language...
            $temp .= "  <li><a href=\"" . $translations->getLink($language) . "\">" . __($language->getName()) . "</a></li>\n";
          }
        }
        $temp .= "</ul>\n";
        $temp .= "</div> <!-- #languages -->\n";
      }
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
