# WinMerge 2.16.53 Release Notes

* [About This Release](#about-this-release)
* [What Is New in 2.16.53 beta?](#what-is-new-in-21653-beta)
* [Known issues](#known-issues)

January 2026

## About This Release

This is a WinMerge beta release which is meant for preview the current state of
WinMerge development. This release is not recommended for the production.

Please submit bug reports to our bug-tracker.

## What Is New in 2.16.53

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
