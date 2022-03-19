# WinMerge 2.16.19 Beta Release Notes

- [About This Release](#about-this-release)
- [What Is New in 2.16.19 beta?](#what-is-new-in-21619-beta)
- [Known issues](#known-issues)

March 2022

## About This Release

This is a WinMerge beta release which is meant for preview the current state of
WinMerge development. This release is not recommended for the production.

## What Is New in 2.16.19 Beta

### General

- Update Merge.rc (PR #1219,#1227,#1231,#1232)

### File compare

- BugFix: Match similar lines breaks with Ignore whitespace change (#1209)
- BugFix: Copy & Advance skips differences when moved block detection is on
    (#1235)
- BugFix: Fix inline difference selection by F4 key not working well in 
    various cases
- Different exit procedure required for small vs. large files (#1218)
- Added View → View Top Margins menu item. (A ruler appears in the margin)

### Table compare

- Pinning first row of file (#999)
  - Added Use First Line as Headers menu item to the column header context menu.

### Folder compare

- BugFix: Fix the problem that the status bar displays "0 items selected" even
    though multiple items are selected.
- BugFix: Change the file naming method of the file compare report to avoid
    duplication of the file compare report file name linked from the folder
    compare report. (PR #1171)
- BugFix: Fix an issue where comparison results are not displayed correctly
    when "Refresh Selected" is performed by selecting an item that has a
    directory and file with the same name. (PR #1189)
- BugFix: Folder compare with jpg images crashes  (#1176)
- BugFix: Fix renaming process in folder compare window. (PR #1246)

### Filters

- Filters enchancement (PR #1179,#1174)
  - File masks
    - Putting `!` before file masks will exclude files that match that masks.
    - If you put `\` after the file masks, the masks will match folders instead
        of files.
  - File filters
    - Added `f!:` and `d!:` to exclude files or folders that match the pattern
        specified in `f:` and `d:`.

### Options dialog

- BugFix: Help text is truncated (#1210)
- Improve vertical alignment string (#1200)
- Some improvements (#1212)

### Plugins

- BugFix: Select Plugin Dialog: Fix the problem that the plugin arguments are
    deleted by clicking the "Add pipe" button after entering them.

### Archive support

### Translations

- Translation updates:
  - Catalan (PR #1237)
  - Chinese Simplified (PR #1257)
  - Chinese Traditional (PR #1204)
  - Corsican (PR #1188,#1205,#1221,#1251,#1260)
  - Dutch (PR #1187)
  - French (PR #1211)
  - German (PR #1208,#1228,#1254,#1262)
  - Hungarian (PR #1203,#1220,#1252,#1259)
  - Japanese (PR #1165)
  - Korean (PR #1181)
  - Lithuanian (PR #1197,#1202,#1224,#1255)
  - Norwegian (PR #1170)
  - Portuguese (PR #1178,#1222)
  - Russian (PR #1164)
  - Slovak (PR #1196)
  - Slovenian (PR #1163,#1261)
  - Ukrainian (PR #1172)

### Internals

- Fix typo in ShellFileOperations.cpp (PR #1256)
- [Big PR - big changes] A lot of refactor and optimization commits (PR #1258)
- Wrong links for ShellExtension on Translations page (#1185)
- Tweak translations status (PR #1201)

## Known issues

 - Crashes when comparing large files (GitHub #325)
 - Very slow to compare significantly different directories (GitHub #322)
 - Vertical scrollbar breaks after pasting text (GitHub #296)
