# WinMerge 2.16.54 Release Notes

* [About This Release](#about-this-release)
* [What Is New in 2.16.54](#what-is-new-in-21654)
* [What Is New in 2.16.53 beta?](#what-is-new-in-21653-beta)
* [Known issues](#known-issues)

January 2026

## About This Release

This is a WinMerge 2.16.54 stable release.
This release replaces earlier WinMerge stable releases as a recommended release.

Please submit bug reports to our bug-tracker.

## What Is New in 2.16.54

### General

* BugFix: Cyrilic shortcuts do not work for main menukeyboard shortcuts
* BugFix: 2 recent-entries per project (#2990)
* Updated expat to version 2.7.3

### Folder compare

* Add logInfo, logWarn, logError functions to filter expressions (PR #3131)
* Add new filter expression functions
    (if, ifEach, choose, chooseEach, andEach, orEach, notEach) (PR #3132)
* Add BaseName/IsFolder and new string transformation functions to filters
    (PR #3146)
* Add experimental customizable rename/move detection for folder comparison
    (PR #3126)

### Options window

* Adjusted the height of the Options dialog and the position of the Defaults
    button

### Shell extension

* BugFix: Add-AppxPackage fails if path ends with backslash (PR #3145)

### Installer

* Removed build-time tools (lemon.exe, re2c.exe) from the installer package.

### Translations

* New translation: Vietnamese (PR #3142)
* Translation updates:
  * Brazilian (PR #3150)
  * French (PR #3137)
  * German (PR #3148)
  * Hungarian (PR #3149)
  * Italian (PR #3147)
  * Japanese
  * Korean (PR #3125,#3127)
  * Lithuanian (PR #3144,#3153)
  * Polish (PR #3154)
  * Turkish (PR #3152)
  * Ukrainian (PR #3130)

## What Is New in 2.16.53 Beta

### General

* Allow comparing files by editing header path (Ctrl+L / Alt+D) (PR #3093)

### File compare

* Async binary comparison for “Selected files are identical” dialog
    (PR #3060)

### Binary compare

* BugFix: Fixed crash when an error occurs while opening files in the binary
    comparison window.

### Image compare

* [ImageCompare] Problems with unsupported formats (#3044)

* Update freeimage to 3.19.10

### Webpage compare

* BugFix: Fixed garbled filenames displayed in Web comparison tabs and header
    bar

### Folder compare

* BugFix: This line cannot be displayed correctly after translation.
    msgid "Failed to extract archive.\nCompare as text file?" (#3073)

* BugFix: Unexpected file closure when moving files from a comparison (#3088)

* Add Display Filter to Folder Compare (PR #3106)

* Add folder statistics filter attributes and helper menu (PR #3111)

* Introduce isWithin() and inRange() for explicit inclusive/exclusive range
    checks (PR #3124)

### Options dialog

* Add option to select user data location (AppData or Documents) (PR #3078)

### Plugins

* CompareMSExcelFiles: Replace line breaks with space in MSExcel formulas
    (PR #3028)

* Updated jq to 1.8.1

### Translations

* Translation updates:

  * Brazilian (PR #3034,#3064,#3085,#3101,#3107,#3121)
  * Chinese Simplified (PR #3035,#3077,#3086,#3097,#3123)
  * Chinese Traditional (PR #3052,#3073,WinMerge/frhed#26)
  * Corsican (PR #3119)
  * French (PR #3072,#3105,#3106)
  * German (PR #3037,#3068,#3081,#3098,#3108,#3115)
  * Hungarian (PR #3036,#3061,#3080,#3100,#3116)
  * Italian (PR #3033,#3062,#3099,#3113,#3118)
  * Japanese
  * Korean (PR #3062,#3082)
  * Lithuanian (PR #3032,#3065,#3087,#3104,#3114)
  * Polish (PR #3050,#3089,#3103,#3112,#3122)
  * Turkish (PR #3043,#3063,#3083,#3102,#3111,#3117)

### Manual

* Added FAQ about file selection order in Windows Explorer. (#3090,#3094)

## Known issues

* Pressing OK in the Options window while the INI file specified by /inifile is open for comparison in WinMerge may corrupt the file. (#2685)
* Suggestion to make the result of image comparison more reliable (#1391)
* Crashes when comparing large files (#325)
* Very slow to compare significantly different directories (#322)
* Vertical scrollbar breaks after pasting text (#296)
