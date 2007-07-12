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
    var $_setupdownload, $_setupsize;

    /**
     * Constructor
     */
    function Release() {
      $this->_versionnumber = '';
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
     * set the setup download from the release
     */
    function setSetupDownload($setupdownload) {
      $this->_setupdownload = $setupdownload;
    }

    /**
     * get the setup download from the release
     */
    function getSetupDownload() {
      return $this->_setupdownload;
    }

    /**
     * set the setup size (in bytes) from the release
     */
    function setSetupSize($setupsize) {
      $this->_setupsize = $setupsize;
    }

    /**
     * get the setup size (in bytes) from the release
     */
    function getSetupSize() {
      return $this->_setupsize;
    }

    /**
     * get the formatted setup size from the release
     */
    function getSetupSizeFormatted($unit = '') {
      return $this->formatSize($this->_setupsize, $unit);
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