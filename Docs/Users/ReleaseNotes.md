# WinMerge 2.16.17 Beta Release Notes

- [About This Release](#about-this-release)
- [What Is New in 2.16.17 beta?](#what-is-new-in-21617-beta)
- [Known issues](#known-issues)

December 2021

## About This Release

This is a WinMerge beta release which is meant for preview the current state of
WinMerge development. This release is not recommended for the production.

Please submit bug reports to our <a href="http://github.com/winmerge/winmerge/issues">bug-tracker</a>.

## What Is New in 2.16.17 Beta


### General

- New Option to ignore numbers. (PR #1025,#923)
- Add the feature to display tooltips on the MDI tab. (PR #1038)
- Issue with closing WinMerge with Esc keyboard key (#1052)
- Add an "Defaults" section to the ini file (PR #1071)

### File compare

- BugFix: Release 2.16.16 crashes when comparing large files - likely a
    regression (#1036)
- BugFix: Fixed C#(Java, JavaScript) keyword highlighting. (#1040)
- BugFix: The current pane switches without me asking it to. (#1050)
- BugFix: Fix PHP syntax highlighting. (PR #1055)
- BugFix:  Source Files Comparison doesn't seem to Work properly (#1057)
- Add D syntax highlighting. (PR #1042)
- Improved 'Match similar lines' option (#1013)
- Make the 'Match similar lines' option work for 3-way comparisons
    (PR #1051,#510)
- Please add a huge icon for "Filter is active" (#1056)

### Image compare

- Added support for creating multi-page image compare report
    (osdn.net #43374)

### Folder compare

- BugFix: Fixed a problem where Duplicate Group Numbers were not assigned to
    files with the same content but different file names.
- BugFix: Fix crash when comparing 3 folders if additional properties were added
- FolderCompare: Improve performance when tree mode is disabled (#PR #1069)

### Project file

- Add a feature to save/restore the "Ignore numbers" setting to/from a project
    file.  (PR #1068)

### Patch Generator dialog

- Put the diff patch to the clipboard rather than to files (#923)

### Plugins

- BugFix: Fixed the problem that Plugins-&gt;Reload Plugins menu item does not
    work.

### Archive support

- Update 7-Zip to 21.06

### Shell extension

- ShellExtension for Windows 11: Implemented advanced menu 

### Translations

- New translation:
  - Corsican (PR #1072,#1085)
- Translation updates:
  - Chinese Traditional (PR #1079)
  - Galician (PR #1089)
  - German (PR #1062,#1086,#1088) 
  - Hungarian (PR #1032)
  - Japanese
  - Korean (PR #1078)
  - Lithuanian (PR #1043,#1061,#1082,#1087)
  - Polish (PR #1049)
  - Portuguese (PR #1034,#1039,#1060,#1065)
  - Russian (PR #1031)
  - Slovenian

### Internals

- BugFix: Fix typo in BCMenu.cpp (PR #1054)
- BugFix: Return better HRESULTs (PR #1077)
- Make it buildable for ARM32 architecture 

## Known issues

 - Crashes when comparing large files (GitHub #325)
 - Very slow to compare significantly different directories (GitHub #322)
 - Vertical scrollbar breaks after pasting text (GitHub #296)
