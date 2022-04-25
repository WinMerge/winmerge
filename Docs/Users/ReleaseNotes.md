# WinMerge 2.16.20 Release Notes

- [About This Release](#about-this-release)
- [What Is New in 2.16.20?](#what-is-new-in-21620)
- [What Is New in 2.16.19 beta?](#what-is-new-in-21619-beta)
- [Known issues](#known-issues)

April 2022

## About This Release

This is a WinMerge 2.16.20 stable release.
This release replaces earlier WinMerge stable releases as a recommended release.

Please submit bug reports to our bug-tracker.

## What Is New in 2.16.20

### General

- BugFix: New filter (F) display (#1281 a))

### File compare

- BugFix: Fixed a problem where the caret would not display in the correct
    position on lines containing tab characters, depending on the font in use
      (osdn.net #44417)

### Webpage compare

- Webpage Compare \[EXPERIMENTAL\] (PR #1182)
  - Requires WebView2 Runtime.
  - Only supported on Windows 10 and above.
  - Currently, it is not possible to directly highlight differences between
      web pages, but it is possible to display two or three web pages side by
      side. You can also compare the following content of the displayed web
      pages
    - Screenshots
    - HTML contents
    - Extracted texts
    - Resource trees

### Folder compare

- BugFix: Fix an issue where items with different case are not displayed
    correctly in the folder compare window when comparing three directories.
    (PR #1299)

### Options dialog

- Allow resizing Options dialog box in both directions (#1265)

### Plugins

- BugFix: CompareMSExcelFiles.sct: Date formats interpreted inconsistently
    (#279)
- Add URL handler plugins (PR #1270)
- Replace plugin: When regular expression substitution is performed with the
    `-e` option, `\r` and `\n` in the second argument are now treated as
    control characters CR and LF.
- PrettifyHTML plugin: Added "--tidy-mark no" to default plugin arguments

### Translations

- Translation updates:
  - Bulgarian (PR #1269)
  - French (PR #1294)
  - Galician (PR 1292)
  - German (PR #1276)
  - Hungarian (PR #1274)
  - Japanese
  - Lithuanian (PR #1263, #1275)
  - Polish (PR #1272, #1287, #1288)
  - Portuguese (PR #1273, #1277)
  - Slovenian (#1289)
  - Turkish (PR #1264)

### Internals


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
