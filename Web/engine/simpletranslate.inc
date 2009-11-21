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
  class SimpleTranslate {
    /**
     * URL parameter
     * @var string
     * @access private
     */
    var $_urlparameter;
    /**
     * Force URL parameter?
     * @var bool
     * @access private
     */
    var $_forceurlparameter;
    /**
     * Languages
     * @var array
     * @access private
     */
    var $_languages;
    /**
     * ...
     * @var string
     * @access private
     */
    var $_baselanguageid;
    /**
     * ...
     * @var string
     * @access private
     */
    var $_activelanguageid;
    /**
     * Translations
     * @var array
     * @access private
     */
    var $_translations;

    /**
     * Constructor
     */
    function SimpleTranslate() {
      $this->_urlparameter = 'lang';
      $this->_forceurlparameter = false;
      $this->_languages = array();
      $this->_baselanguageid = '';
      $this->_activelanguageid = '';
      $this->_translations = array();
    }

    /**
     * get the URL parameter...
     * @return string URL parameter
     */
    function getUrlParameter() {
      return $this->_urlparameter;
    }

    /**
     * set the URL parameter...
     * @param string $urlparameter URL parameter
     */
    function setUrlParameter($urlparameter) {
      $this->_urlparameter = $urlparameter;
    }

    /**
     * append URL parameter to link...
     * @param string $link Link
     * @param string $urlparameter URL parameter
     * @param string $id Language ID
     * @return string Link with URL parameter
     */
    function appendUrlParameter($link, $urlparameter, $id = '') {
      $urlparts = parse_url($link);
      
      $url = '';
      if (isset($urlparts['scheme']) && $urlparts['scheme'] != '') { //if "scheme" exists...
        $url .= $urlparts['scheme'] . '://';
      }
      if (isset($urlparts['user']) && $urlparts['user'] != '' && isset($urlparts['pass']) && $urlparts['pass'] != '') { //if "username" AND "password" exists...
        $url .= $urlparts['user'] . ':' . $urlparts['pass'] . '@';
      }
      if (isset($urlparts['host']) && $urlparts['host'] != '') { //if "hostname" exists...
        $url .= $urlparts['host'];
      }
      if (isset($urlparts['path']) && $urlparts['path'] != '') { //if "path" exists...
        $url .= $urlparts['path'];
      }
      if (isset($urlparts['query']) && $urlparts['query'] != '') { //if "query" exists...
        $found = false;
        
        $temp = '';
        $queries = explode('&', $urlparts['query']);
        foreach ($queries as $query) { //for all queries...
          @list($key, $value) = explode('=', $query);
          if (strtolower($key) == strtolower($urlparameter)) { //if language URL parameter...
            if ($id != '') { //if a language id exists...
              if ($temp == '')
                $temp .= '?' . $urlparameter . '=' . $id;
              else
                $temp .= '&' . $urlparameter . '=' . $id;
            }
            $found = true;
          }
          else { //if NOT language URL parameter...
            if ($temp == '')
              $temp .= '?' . $key . '=' . $value;
            else
              $temp .= '&' . $key . '=' . $value;
          }
        }
        $url .= $temp;
        
        if ($found == false) {
          if ($id != '') { //if a language id exists...
            $url .= '&' . $urlparameter . '=' . $id;
          }
        }
      }
      else { //if NO "query" exists...
        if ($id != '') { //if a language id exists...
          $url .= '?' . $urlparameter . '=' . $id;
        }
      }
      if (isset($urlparts['fragment']) && $urlparts['fragment'] != '') { //if "fragment" exists...
        $url .= '#' . $urlparts['fragment'];
      }
      
      return $url;
    }

    /**
     * get forcing of the URL parameter...
     * @return bool Force URL parameter?
     */
    function getForceUrlParameter() {
      return $this->_forceurlparameter;
    }

    /**
     * set forcing of the URL parameter...
     * @param bool $force Force URL parameter?
     */
    function setForceUrlParameter($force) {
      $this->_forceurlparameter = $force;
    }

    /**
     * add a language...
     * @param string $id Language ID
     * @param string $name Language name
     * @param string $pofile PO file
     */
    function addLanguage($id, $name, $pofile = '') {
      $id = strtolower($id);
      if ($pofile == '') { //if base language...
        $this->_languages[$id] = new SimpleTranslateBaseLanguage($id, $name);
        $this->_baselanguageid = $id;
      }
      else { //if normal language...
        if (file_exists($pofile) == true) { //if the PO file exists...
          $this->_languages[$id] = new SimpleTranslateLanguage($id, $name, $pofile);
        }
      }
    }

    /**
     * exists the language?
     * @param string $id Language ID (in lower case)
     * @return bool Exists?
     */
    function existsLanguage($id) {
      return array_key_exists($id, $this->_languages);
    }

    /**
     * get the language...
     * @param string $id Language ID
     * @return object Language
     */
    function getLanguage($id) {
      $id = strtolower($id);
      if ($this->existsLanguage($id)) { //if the language exists...
        return $this->_languages[$id];
      }
      return null;
    }

    /**
     * get the languages...
     * @return array Languages
     */
    function getLanguages() {
      return $this->_languages;
    }

    /**
     * get the languages count...
     * @return int Languages count
     */
    function getLanguagesCount() {
      return count($this->_languages);
    }

    /**
     * get the base language ID...
     * @return string Base language ID
     */
    function getBaseLanguageId() {
      return $this->_baselanguageid;
    }

    /**
     * get the base language...
     * @return object Base language
     */
    function getBaseLanguage() {
      return $this->_languages[$this->_baselanguageid];
    }

    /**
     * check if the language is the base language...
     * @param object $language Language
     * @return bool Is base language?
     */
    function isBaseLanguage($language = null) {
      if (isset($language)) { //if language committed...
        if ($language == $this->getBaseLanguage()) { //if base language...
          return true;
        }
      }
      else { //if language NOT committed...
        if ($this->_baselanguageid == $this->_activelanguageid) { //if base language is active...
          return true;
        }
      }
      return false;
    }

    /**
     * get the active language ID...
     * @return string Active language ID
     */
    function getActiveLanguageId() {
      if ($this->_activelanguageid != '') {
        return $this->_activelanguageid;
      }
      return $this->_baselanguageid;
    }

    /**
     * get the active language...
     * @return object Active language
     */
    function getActiveLanguage() {
      if ($this->_activelanguageid != '') {
        return $this->_languages[$this->_activelanguageid];
      }
      return $this->_languages[$this->_baselanguageid];
    }

    /**
     * set the active language...
     * @param string $id Language ID
     * @return bool Successful?
     */
    function setActiveLanguage($id) {
      $id = strtolower($id);
      if ($this->existsLanguage($id)) { //if the language exists...
        $this->_activelanguageid = $id;
        $language = $this->_languages[$id];
        $this->_translations = $language->getTranslations();
        return true;
      }
      return false;
    }

    /**
     * check if the language is the active language...
     * @param object $language Language
     * @return bool Is active language?
     */
    function isActiveLanguage($language) {
      if ($language == $this->getActiveLanguage()) { //if active language...
        return true;
      }
      return false;
    }

    /**
     * check if the language ID is valid...
     * @param string $id Language ID
     * @return bool Is valid language ID?
     */
    function isValidLanguageId($id = null) {
      if (isset($id)) { //if language ID committed...
        if (!empty($id)) { //if language ID NOT empty...
          return $this->existsLanguage(strtolower($id));
        }
        return false;
      }
      else { //if language ID NOT committed...
        if (isset($_GET[$this->_urlparameter])) {
          return $this->existsLanguage(strtolower($_GET[$this->_urlparameter]));
        }
        return true;
      }
    }

    /**
     * detect the language from the url...
     * @return bool Detected?
     */
    function detectLanguageFromUrl() {
      if (isset($_GET[$this->_urlparameter])) {
        return $this->setActiveLanguage($_GET[$this->_urlparameter]);
      }
      return false;
    }

    /**
     * detect the language from the browser...
     * @return bool Detected?
     */
    function detectLanguageFromBrowser() {
      if (isset($_SERVER['HTTP_ACCEPT_LANGUAGE'])) {
        $accepted_languages = split(',', $_SERVER['HTTP_ACCEPT_LANGUAGE']);
        foreach ($accepted_languages as $accepted_language) { //for all accepted languages...
          $tmp = split(';', $accepted_language);
          
          $lang_id = strtolower($tmp[0]);
          if ($this->existsLanguage($lang_id)) { //if the language exists...
            $this->_activelanguageid = $lang_id;
            $language = $this->_languages[$lang_id];
            $this->_translations = $language->getTranslations();
            return true;
          }
        }
      }
      return false;
    }

    /**
     * prepare link...
     * @param string $link Link
     * @return string Prepared link
     */
    function prepareLink($link) {
      $activelanguage = $this->getActiveLanguage();
      if ((strtolower(get_class($activelanguage)) == 'simpletranslatelanguage') || ($this->_forceurlparameter == true)) { //if normal language OR forcing URL parameter...
        return $this->appendUrlParameter($link, $this->_urlparameter, $activelanguage->getId());
      }
      else { //if base language...
        return $this->appendUrlParameter($link, $this->_urlparameter);
      }
    }

    /**
     * get the language link...
     * @param object $language Language
     * @param bool $forceurlparameter Force URL parameter?
     * @return string Language link
     */
    function getLink($language = null, $forceurlparameter = null) {
      $phpself = str_replace('index.php', '', $_SERVER['PHP_SELF']);
      if (isset($language)) { //if language committed...
        $forceing = $this->_forceurlparameter;
        if (isset($forceurlparameter)) { //if force URL parameter committed...
          $forceing = $forceurlparameter;
        }
        if ((strtolower(get_class($language)) == 'simpletranslatelanguage') || ($forceing == true)) { //if normal language OR forcing URL parameter...
          return $phpself . '?' . $this->_urlparameter . '=' . $language->getId();
        }
      }
      return $phpself;
    }

    /**
     * translate a text...
     * @param string $text Untranslated text
     * @return string Translated text
     */
    function translate($text) {
      $text = str_replace("\r", '', $text);
      $text = str_replace("\n", '\n', $text);
      if (array_key_exists($text, $this->_translations)) { //if the translation exists...
        $translation = $this->_translations[$text];
        if ($translation != '') { //if the translation is NOT empty...
          return str_replace('\n', "\n", $translation);
        }
      }
      return str_replace('\n', "\n", $text);
    }
  }

  /**
   * ...
   */
  class SimpleTranslateLanguage {
    /**
     * ID
     * @var string
     * @access private
     */
    var $_id;
    /**
     * Name
     * @var string
     * @access private
     */
    var $_name;
    /**
     * PO file
     * @var string
     * @access private
     */
    var $_pofile;

    /**
     * Constructor
     * @param string $id ID
     * @param string $name Name
     * @param string $pofile PO file
     */
    function SimpleTranslateLanguage($id, $name, $pofile) {
      $this->_id = $id;
      $this->_name = $name;
      $this->_pofile = $pofile;
    }

    /**
     * get the language id...
     * @return string ID
     */
    function getId() {
      return $this->_id;
    }

    /**
     * get the language name...
     * @return string Name
     */
    function getName() {
      return $this->_name;
    }

    /**
     * get the language PO file...
     * @return string PO file
     */
    function getPoFile() {
      return $this->_pofile;
    }

    /**
     * get the translations array...
     * @return array Translations
     */
    function getTranslations() {
      $translations = array();
      if (file_exists($this->_pofile) == true) { //if the file exists...
        $msgstarted = 0;
        $msgid = '';
        $msgstr = '';
        
        $lines = file($this->_pofile);
        $linescount = sizeof($lines);
        
        for ($i = 0; $i < $linescount; $i++) { //for all lines...
          $line = trim($lines[$i]);
          
          if ($line != '') { //if NOT empty line...
            if ($line[0] != '#') { //if NOT comment line...
              if (preg_match('#^msgid "(.*)"$#si', $line, $matches)) { //if "msgid"...
                $msgstarted = 1;
                $msgid = $matches[1];
              }
              elseif (preg_match('#^msgstr "(.*)"$#si', $line, $matches)) { //if "msgstr"...
                $msgstarted = 2;
                $msgstr = $matches[1];
              }
              elseif (preg_match('#^"(.*)"$#si', $line, $matches)) { //if "msgid" or "msgstr" continued...
                if ($msgstarted == 1)
                  $msgid = $msgid . $matches[1];
                elseif ($msgstarted == 2)
                  $msgstr = $msgstr . $matches[1];
              }
            }
            else { //if comment line...
              $msgstarted = -1;
            }
          }
          else { //if empty line
            $msgstarted = 0;
          }
          
          if ($msgstarted == 0) { //if NOT inside a translation...
            if ($msgid != '') {
              $msgid = str_replace('\"', '"', $msgid);
              $msgstr = str_replace('\"', '"', $msgstr);
              $translations[$msgid] = $msgstr;
            }
            $msgid = '';
            $msgstr = '';
          }
        }
      }
      return $translations;
    }
  }

  /**
   * ...
   */
  class SimpleTranslateBaseLanguage extends SimpleTranslateLanguage {

    /**
     * Constructor
     * @param string $id ID
     * @param string $name Name
     */
    function SimpleTranslateBaseLanguage($id, $name) {
      $this->_id = $id;
      $this->_name = $name;
      $this->_pofile = '';
    }

    /**
     * get the translations array...
     * @return array Translations
     */
    function getTranslations() {
      return array();
    }
  }
?>
