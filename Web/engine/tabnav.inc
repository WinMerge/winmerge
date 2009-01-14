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
  class Tab {
    var $_id, $_link, $_title, $_tooltiptext;
    var $_subtabs;

    /**
     * Constructor
     */
    function Tab($id, $link, $title, $tooltiptext = '') {
      $this->_id = $id;
      $this->_link = $link;
      $this->_title = $title;
      $this->_tooltiptext = $tooltiptext;
      $this->_subtabs = array();
    }

    /**
     * set the id from the tab
     */
    function setId($id) {
      $this->_id = $id;
    }

    /**
     * get the id from the tab
     */
    function getId() {
      return $this->_id;
    }

    /**
     * set the link from the tab
     */
    function setLink($link) {
      $this->_link = $link;
    }

    /**
     * get the link from the tab
     */
    function getLink() {
      return $this->_link;
    }

    /**
     * set the title from the tab
     */
    function setTitle($title) {
      $this->_title = $title;
    }

    /**
     * get the title from the tab
     */
    function getTitle() {
      return $this->_title;
    }

    /**
     * set the tool tip text from the tab
     */
    function setToolTipText($id) {
      $this->_tooltiptext = $id;
    }

    /**
     * get the tool tip text from the tab
     */
    function getToolTipText() {
      return $this->_tooltiptext;
    }
  }

  /**
   * ...
   */
  class Subtab {
    var $_link, $_title, $_tooltiptext;

    /**
     * Constructor
     */
    function Subtab($link, $title, $tooltiptext = '') {
      $this->_link = $link;
      $this->_title = $title;
      $this->_tooltiptext = $tooltiptext;
    }

    /**
     * set the link from the sub tab
     */
    function setLink($link) {
      $this->_link = $link;
    }

    /**
     * get the link from the sub tab
     */
    function getLink() {
      return $this->_link;
    }

    /**
     * set the title from the sub tab
     */
    function setTitle($title) {
      $this->_title = $title;
    }

    /**
     * get the title from the sub tab
     */
    function getTitle() {
      return $this->_title;
    }

    /**
     * set the tool tip text from the sub tab
     */
    function setToolTipText($id) {
      $this->_tooltiptext = $id;
    }

    /**
     * get the tool tip text from the sub tab
     */
    function getToolTipText() {
      return $this->_tooltiptext;
    }

    /**
     * is the current sub tab?
     */
    function isCurrent() {
      if ($this->_link == $_SERVER['PHP_SELF']) {
        return true;
      }
      elseif ($this->_link == str_replace('index.php', '', $_SERVER['PHP_SELF'])) {
        return true;
      }
      return false;
    }
  }
?>