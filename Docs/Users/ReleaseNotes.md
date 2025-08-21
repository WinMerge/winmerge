# WinMerge 2.16.50.2 Release Notes

* [About This Release](#about-this-release)
* [What Is New in 2.16.50.2](#what-is-new-in-216502)
* [What Is New in 2.16.50](#what-is-new-in-21650)
* [What Is New in 2.16.49 beta?](#what-is-new-in-21649-beta)
* [Known issues](#known-issues)

August 2025

## About This Release

This is a WinMerge 2.16.50.2 stable release.
This release replaces earlier WinMerge stable releases as a recommended release.

Please submit bug reports to our bug-tracker.

## What Is New in 2.16.50.2

### File compare

- BugFix: WinMerge 2.16.50 regression: Crashes when doing "Show only comparison
    results (not file contents)" for large binaries. (#2874)

### Folder compare

- BugFix: After Install of v2.16.50 Folder: Filter is now only being applied to
    1st folder path and not the 2nd and 3th folder paths (#2875)

### Filters dialog

- BugFix: Fixed a crash in the Filters dialogfs File Filters tab after installing or creating a new filter.

### Translations

- Translation updates:
  - Brazilian (PR #2883)
  - Chinese Simplified (PR #2865)
  - French (PR #2873)
  - Italian (PR #2869)
  - Korean (PR #2870)
  - Lithuanian (PR #2864)
  - Portuguese (PR #2882)
  - Russian (PR #2867)
  - Swedish
  - Tamil ()
  - Turkish ()
 
## What Is New in 2.16.50

### General

* Treat a mouse click on the very top edge of the title bar while maximized
  as if it's within the tab button (PR #2828)

### Translations

* BugFix: WinMerge - Strings untranslated #2852
* Translation updates:

  * Brazilian (PR #2849)
  * Chinese Simplified (PR #2856)
  * Hungarian (PR #2857,#2859)
  * Italian (PR #2850,#2851,#2858)
  * Japanese
  * Swedish
  * Russian (PR #2847)

## What Is New in 2.16.49 Beta

## General

* \[Enhancement] Tab Switching and Tab Bar Scrolling with Mouse Wheel (PR #2821)

### Appearance

* Use themed size grip, use toolbar's tooltips instead of controls bar's
  (PR #2769)

### File compare

* BugFix: Fixed crash when copying multiple selected diff hunks
* Syntax parser for Pascal should be extended to support compiler directives
  (#2814)
* \[Enhancement] Enable/disable automatic navigation to a line when clicking on
  the Location Panel (PR #2827)

### Folder compare

* BugFix: WinMerge shows the timestamp of the non-existent file (#2764)
* BugFix: Manual Refresh of a single folder undoes the comparison for most
  other folders randomly (#2792)
* Add the feature to compare multiple files in one action in the folder compare
  window requested in #324. (PR #2825)
* Improve filter system: expression support and UI enhancements (PR #2802)

### Options dialog

* BugFix: \[Bug Fix] Enable/disable 'Switch to ...' options in Compare > Folder
  page (PR #2819)

### Command line

* BugFix: Fixed failure to compare files with relative paths when /s option
  is used from a different current directory than the running WinMerge process

### Archive support

* Update 7-Zip to 25.00

### Installer

* Installer - Windows "Programs and Features" name (#2798)

### Translations

* Translation updates:

  * Brazilian (PR #2829)
  * Bulgarian (PR #2763)
  * Chinese Simplified (PR #2831)
  * Chinese Traditional (PR #2838)
  * Corsican (PR #2844)
  * French (PR #2754,#2761,#2762)
  * Hungarian (PR #2836)
  * Italian (PR #2832)
  * Japanese
  * Korean (PR #2766,#2845)
  * Lithuanian (PR #2799,#2805,#2826,#2830)
  * Portuguese (PR #2768)
  * Russian (PR #2760,#2840)
  * Swedish
  * Tamil (PR #2812,#2813)
  * Turkish (PR #2782)

### Internals

* Fix syntax warning in GetTranslationsStatus.py (PR #2751)

## Known issues

* Pressing OK in the Options window while the INI file specified by /inifile is open for comparison in WinMerge may corrupt the file. (#2685)
* Suggestion to make the result of image comparison more reliable (#1391)
* Crashes when comparing large files (#325)
* Very slow to compare significantly different directories (#322)
* Vertical scrollbar breaks after pasting text (#296)
