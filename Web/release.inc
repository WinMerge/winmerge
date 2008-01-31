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
  class Release {
    var $_versionnumber;
    var $_date;
    var $_downloads, $_downloadsizes;

    /**
     * Constructor
     */
    function Release() {
      $this->_versionnumber = '';
      $this->_date = '';
      $this->_downloads = array();
      $this->_downloadsizes = array();
    }

    /**
     * set the version number from the release
     */
    function setVersionNumber($versionnumber) {
      $this->_versionnumber = $versionnumber;
    }

    /**
     * get the version number from the release
     */
    function getVersionNumber() {
      return $this->_versionnumber;
    }

    /**
     * set the date from the release
     */
    function setDate($date) {
      $this->_date = $date;
    }

    /**
     * get the date from the release
     */
    function getDate() {
      return $this->_date;
    }

    /**
     * add a download from the release
     */
    function addDownload($id, $url, $size) {
      $this->_downloads[$id] = $url;
      $this->_downloadsizes[$id] = $size;
    }

    /**
     * get a download from the release
     */
    function getDownload($id) {
      return $this->_downloads[$id];
    }

    /**
     * get a download size (in bytes) from the release
     */
    function getDownloadSize($id) {
      return $this->_downloadsizes[$id];
    }

    /**
     * get a download size (in kilobytes) from the release
     */
    function getDownloadSizeKb($id) {
      return $this->formatSize($this->_downloadsizes[$id], 'kb');
    }

    /**
     * get a download size (in megabytes) from the release
     */
    function getDownloadSizeMb($id) {
      return $this->formatSize($this->_downloadsizes[$id], 'mb');
    }

    /**
     * get a formatted download size from the release
     */
    function getDownloadSizeFormatted($id, $unit = '') {
      return $this->formatSize($this->_downloadsizes[$id], $unit);
    }

    /**
     * format the size
     */
    function formatSize($bytes, $unit = '') {
      switch (strtolower($unit)) {
        case 'kb':
          return number_format($bytes / 1024);
          break;
        case 'mb':
          return number_format($bytes / 1048576, 2);
          break;
        default:
          return number_format($bytes);
      }
    }
  }
?>