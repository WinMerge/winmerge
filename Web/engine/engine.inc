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

  include('simpletranslate.inc');
  include('page.inc');

  $languagesdir = dirname(__FILE__) . '/languages/';

  $translations = new SimpleTranslate;
  $translations->setUrlParameter('lang');
  $translations->addLanguage('en', __('English'));
  $translations->addLanguage('de', __('German'), $languagesdir . 'de.po');
  $translations->addLanguage('fr', __('French'), $languagesdir . 'fr.po');
  $translations->addLanguage('ja', __('Japanese'), $languagesdir . 'ja.po');
  $translations->addLanguage('nl', __('Dutch'), $languagesdir . 'nl.po');
  $translations->addLanguage('se', __('Swedish'), $languagesdir . 'se.po');
  if (!$translations->detectLanguageFromUrl()) { //if NOT detect language from URL...
    $translations->detectLanguageFromBrowser();
  }

  function __($text) {
    global $translations;
    
    $numargs = func_num_args();
    if ($numargs > 1 ) { //if more the one argument...
      $args = func_get_args();
      array_shift($args);
      return vsprintf($translations->translate($text), $args);
    }
    else { //if one argument...
      return $translations->translate($text);
    }
  }

  function __e($text) {
    global $translations;
    
    $numargs = func_num_args();
    if ($numargs > 1 ) { //if more the one argument...
      $args = func_get_args();
      array_shift($args);
      vprintf($translations->translate($text), $args);
    }
    else { //if one argument...
      echo $translations->translate($text);
    }
  }
?>