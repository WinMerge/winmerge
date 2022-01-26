# WinMerge 2.16.18 Release Notes

- [About This Release](#about-this-release)
- [What Is New in 2.16.18?](#what-is-new-in-21618)
- [What Is New in 2.16.17 beta?](#what-is-new-in-21617-beta)
- [Known issues](#known-issues)

January 2022

## About This Release

This is a WinMerge 2.16.18 stable release.
This release replaces earlier WinMerge stable releases as a recommended release.

Please submit bug reports to our <a href="http://github.com/WinMerge/winmerge/issues">bug-tracker</a>.

## What Is New in 2.16.18

### General

- BugFix: Crash when comparing files in Google Drive
- [Feature Request] Lengthen title of File Compare window or add tip (#960)
- added me to contributor list (PR #1094)
- Made it so that the parent window cannot be operated while the font selection
    dialog displayed from View→Select Font menu item is displayed.

### File compare

- BugFix: wm 2.16.16.0 crashes with file attached (#1101)
- BugFix: Fix a problem that 'Encountered an improper argument' error occurs
    when a pane split by Window→Split menu item is unsplit by drag operation.
- BugFix: Colors -> Differences -> Word Difference : Text color ignored (#1116)
- BugFix: WinMerge crashes with specific regex search (#1160)
- 3-Way File Merge: No Keyboard / hot keys available for Merging from Left Pane
    to right pane and vice versa (#957)
- Winmerge hangs when i try to compare files. (#1111)
- Fast encoding switching. (#793)

### Clipboard Compare 

- New Feature: Clipboard Compare (PR #1147)
  - Click File → Open Clipboard menu item to compare the two most recent
      contents of the clipboard history.
  - You can also compare by pressing Ctrl+V when the child MDI window is not
      visible.
  - This feature is available on Windows 10 version 1809 or higher and
      WinMerge 64-bit version.

### Folder compare

- BugFix: Sorting on Comparison Result being done incorectly (#483)
- BugFix: Fix an issue where WinMerge sometimes crashes when executing
    "Refresh Selected" in the folder compare window. (PR #1120)
- BugFix: Fixed a bug that the parent folder icon was not displayed in non-recursive mode.
- BugFix: Fixed the problem that the sort order is different from version 2.16.16 or earlier

### Plugins

- Fix for https://github.com/WinMerge/winmerge/discussions/1139 (#1139,PR #1140)
- Make plugin descriptions translatable
- Upgrade Apache Tika to 2.2.1

### Command line

- Added `/clipboard-compare` command line option

### Archive support

- Update 7-Zip to 21.07

### Installer

- BugFix: Incorrect link to "Quick Start" guide at WM installation end (#1127)
- BugFix: Add replacesameversion flag to 7z.dll
- Re-enabled the process of installing ShellExtension for Windows 11.

### Manual

- BugFix: "Quick compare limit" and "Binary compare limit" settings don't have
    the expected (and documented) purpose (#1100)

### Translations

- Translation updates:
  - Chinese Simplified (PR #1109,#1112,#1134)
  - Corsican (PR #1103,#1119,#1137,#1142,#1154)
  - Dutch (PR #1123)
  - French (PR #1121,#1122,#1157)
  - German (PR #1110,#1117,#1143,#1155)
  - Hungarian (PR #1102,#1115,#1136,#1141,#1150)
  - Japanese
  - Lithuanian (PR #1124,#1144)
  - Portuguese (PR #1097,#1098,#1106,#1133,#1149)
  - Slovenian (PR #1148,#1153)
  - Turkish (PR #1099)

### Internals

- Fix typo in DirScan.cpp (PR #1118)

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
