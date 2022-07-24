# WinMerge 2.16.22 Release Notes

- [About This Release](#about-this-release)
- [What Is New in 2.16.22?](#what-is-new-in-21622)
- [What Is New in 2.16.21 beta?](#what-is-new-in-21621-beta)
- [Known issues](#known-issues)

July 2022

## About This Release

This is a WinMerge 2.16.22 stable release.
This release replaces earlier WinMerge stable releases as a recommended release.

Please submit bug reports to our bug-tracker.

## What Is New in 2.16.22

### General

- Allow renaming of untitled pages (#1395)

### File compare

- BugFix: "Replace All" doesn't work when Replace in "Selection" and the new
    string contains the old string. (#1376)
- BugFix: “Match case” in Search always enabled (#1380)
- BugFix: vertical editing for .h file is quite slow (#1386)
- BugFix: replace text using regular expressions behaves incorrectly if
    multiple matches on the same line (#1387, PR #1388)
- Optimize snake function (PR #1411)

### Folder compare

- BugFix: Fix an issue where paths with different case are not displayed
    correctly in the folder column of the folder compare window when comparing
    three directories. (PR #1372)
- BugFix: Fix renaming process in folder compare window. (PR #1392)
- BugFix: Elapsed time was no longer displayed in the status bar after folder
    comparison.
- BugFix: Fix an issue where the folder column is not updated for child items
    after renaming a directory in the folder compare window. (PR #1408)

### Plugins

- Modify textbox behavior (CompareMSExcelFiles options dialog) (PR #1374)
- Make wsc files available as plug-in (PR #1390)

### Archive support

- Update 7-Zip to 22.00

### Project file

- Add a feature to save/restore hidden items to/from a project file.(PR #1377)

### Options dialog

- New setting to decide when to save/restore hidden items when project is saved/loaded (PR #1377)

### Translations

- Translation updates:
  - Bulgarian (PR #1375)
  - French (PR #1418)
  - Galician (PR #1400)
  - German (PR #1396,#1399)
  - Hungarian (PR #1393,#1398)
  - Japanese
  - Lithuanian (PR #1394)
  - Portuguese (PR #1416)

### Internals

  - Fix typo in SuperComboBox.cpp (PR #1379)

## What Is New in 2.16.21 Beta

### General

- In windows 11 i have error 78 sidebyside with the winmerge manifest (#1312)

### File compare

- BugFix: Copy left/right different behavior (#1334)
- BugFix: Line difference coloring in “Word-level” mode does not work
    correctly for Cyrillic-based languages (#1362)
- BugFix: Syntax highlighting: SQL comments (#1354)
- Request for updating code: a new language(ABAP) in "syntax highlighting"
   (PR #1340)
- Added "none" diff algorithm
- Enable mouse wheel scrolling on the location pane.
- Backup files: Misleading error message when file cannot be written (#1326)

### Binary compare

- Allow the "Split Vertically" menu item to be unchecked.

### Image compare

- BugFix: Fix an issue where the pane was split vertically the next time the
    window was displayed, even though the "Split Vertically" menu item was
    unchecked.
- Make patience and histogram diff algorithm selectable.

### Webpage compare

- BugFix: Fix text disappearing when pressing the 'K' key in the address bar
- BugFix: Fix an issue where the pane was split vertically the next time the
    window was displayed, even though the "Split Vertically" menu item was
    unchecked.
- ResourceTree compare: Set the last-modified date and time in resource files
- Added Ctrl+L keyboard shortcut

### Folder compare

- BugFix: Alt/Shift key highlighting issue not resetting start point from move.
    (#1335)
- BugFix: Refresh Selected Marks Unscanned Folders as Identical (#1349)
- BugFix: Make the file name refreshed to detect case changes when running
    "Refresh Selected". (PR #1358)
- BugFix: Fix an issue where paths with different case are not displayed
    correctly in the folder column of the folder compare window when comparing
    three directories. (PR #1372)
- Tab behaviour on comparisons of files vs folders (#1367)
- Make the sort order of file names including numbers the same as Explorer.
   (osdn.net #44557)

### Reports

- BugFix: Fix report generation process. (PR #1324)
- BugFix: Fix report generation process about replacement with HTML entity
    (PR #1344)
- Modify "Tools - Generate Report" (Add column width definition to html output) 
    (PR #1333)
- BugFix: Fix an issue where WinMerge crashes depending on the filename when
    generating a file compare report. (PR #1319)

### Configuration log

- BugFix: [Bug Report] WinMerge does not recognize Win 11 (#1192)

### Plugins

- BugFix: CompareMSExcelFiles Plugins did not compare folders when opening
    .xlsx files from Plugins-> Edit with Unpacker menu item even though the
    "Extract workbook data to multiple files" option is enabled in the plugin
    settings (osdn.net #44522)
- BugFix: Fix a problem where the "Open files in the same window type after
    unpacking" checkbox was checked, but the checkbox was not checked the next
    time the dialog was opened.
- Sugg: Increase the dialogue for Plugins (#1308)

### Command line

- BugFix: Crash on command line compare (#1363)

### Shell extension

- BugFix: Fix an issue where the WinMerge menu displayed in the"Show more
    options" menu of the Windows 11 Explorer context menu is not an advanced
    menu, even though the advanced menu is enabled.
- BugFix: Fix the problem that the WinMerge icon is not correctly displayed on
    the taskbar when WinMerge is started from the Windows 11 context menu.

### Translations

- Translation updates:
  - Chinese Simplified (PR #1330)
  - Corsican (PR #1331,#1347)
  - German (PR #1311,#1329)
  - Hungarian (PR #1325)
  - Italian (PR #1355)
  - Japanese (PR #1338)
  - Lithuanian (PR #1318,#1327)
  - Polish (#1323)
  - Portuguese (PR #1317,#1345)
  - Slovenian
  - Turkish (#1332)
  - Russian (PR #1310)

### Internals

- Fix typo in BCMenu.cpp (PR #1313)
- Fix type: GPL (PR #1342)
- Use isupper+tolower instead of islower+toupper (diffutils ver2.7) (PR #1351)
- Initialize variables defined at "diff.h" (PR #1360)

## Known issues

 - Suggestion to make the result of image comparison more reliable (#1391)
 - Crashes when comparing large files (#325)
 - Very slow to compare significantly different directories (#322)
 - Vertical scrollbar breaks after pasting text (#296)