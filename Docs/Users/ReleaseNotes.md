# WinMerge 2.16.28 Release Notes

- [About This Release](#about-this-release)
- [What Is New in 2.16.28?](#what-is-new-in-21628)
- [What Is New in 2.16.26?](#what-is-new-in-21626)
- [What Is New in 2.16.25 beta?](#what-is-new-in-21625-beta)
- [Known issues](#known-issues)

February 2023

## About This Release

This is a WinMerge 2.16.28 stable release.
This release replaces earlier WinMerge stable releases as a recommended release.

Please submit bug reports to our bug-tracker.

## What Is New in 2.16.28

### Folder compare

- BugFix: Fixed an issue where files with no extension were not compared if they
    were in a folder with a '.' in the folder name.

## What Is New in 2.16.26

### General

- Fixed issue where the program would crash when certain path names were set
    in the file path bar.

### File compare

- Feature request: Allow pasting when editing caption of pages (PR #1651)

### Folder compare

- BugFix: Filters aren't saved anywhere (#1638)
- BugFix: Fixed issue where the Open menu item in file path bar of folder
    comparison window was disabled.
- Add processing to indicate that two directories are identical in the
    "Comparison result" column when they are identical in a 3-way folder
    comparison. (PR #1649)
- Request: highlight the file after opening its parent folder (#1662)
- Show/hide directories in 3-way comparison (PR #1683)

### Binary compare

- BugFix: Fixed issue where the Open menu item in file path bar of binary
    comparison window was disabled.

### Webpage compare

- BugFix: Deleted color of Word Difference in Options dialog was not used.
- Implemented Ignore numbers comparison option.

### Options dialog

- Modify the "Options (Compare > Folder)" dialog. (PR #1645)

### Plugins

- Add PreviewMarkdown plugin (PR #1641)
- Add PreviewPlantUML plugin (PR #1666)
- CompareMSExcelFiles: Added "Compare worksheets as HTML" in CompareMSExcelFiles
    plugin options window
- ApacheTika: Updated Apache Tika to version 2.6.0
- ApacheTika: If Java is not installed, OpenJDK 19.0.2 will now be downloaded
    and used.

### Translations

- Translation updates:
  - Brazilian (PR #1656,#1670)
  - Chinese Simplified (PR #1635,#1665,#1667,#1677,#1681)
  - Corsican (PR #1674)
  - French (PR #1640,#1679)
  - German (PR #1660,#1671)
  - Hungarian (PR #1664)
  - Japanese
  - Lithuanian (PR #1657,#1673)
  - Polish (PR #1648)
  - Portuguese (PR #1669)
  - Russian (PR #1676)
  - Slovenian
  - Swedish (PR #1655,#1663,#1682)

## What Is New in 2.16.25 Beta

### File compare

- BugFix: Selection in "Replace" by regular expression doesn't work with `\n`
    (#1556)
- BugFix: WinMerge hangs for a certain regex search & replace action for
    clearing all lines not containing '%' (#1575)
- Add html5 keywords to crystaledit (PR #1565)
- Add css keywords to crystaledit (PR #1572)
- Preliminary exit (performance optimization) for Scrollbars calculation
    (PR #1574, #1573)
- Fix issue #1583 Syntax highlighting for SQL leaves many things out.
    (PR #1591, #1583)
- Starting Pane Consistency (#1598)

### Binary compare

- BugFix: Crash 0xc0000409 (#1544)
- Binary compare: Allow 64bit versions to open files larger than 2GB
    (PR #1549)

### Folder compare

- BugFix: Fix the problem that WinMerge crashes when pressing the "OK" button 
    in the "Display Columns" dialog in the debug version. (PR #1568)
- BugFix: Crash when copying files/folders (#1558)
- BugFix: File Duplication Bug In Outputted Zips (#1588)
- BugFix: Fixed problem with scrolling to unexpected position when expanding
    folders (osdn.net #46061)
- BugFix: Fixed incorrect links to files with # in filename in folder
    comparison report (osdn.net #46082)
- Changes the display processing of the "Comparison result" column for a 3-way
    folder comparison. (PR #1545)
- Add "Copy All Displayed Columns" to the context menu of the folder compare
    window. (PR #1615)
    
### Options dialog

- Added Auto-reload modified files option (PR #1611)

### Translations

- Translation updates:
  - Brazilian (PR #1617,#1630)
  - Chinese Simplified (PR #1614)
  - Corsican (PR #1628,#1629)
  - Galician (#1581)
  - German (PR #1616,#1633)
  - Hungarian (PR #1618,#1631)
  - Lithuanian (PR #1621,#1632)
  - Japanese
  - Polish (PR #1566)
  - Russian (PR #1553,#1554,#1555)
  - Slovenian
  - Swedish (PR #1594)
  - Turkish (PR #1563)

### Others

- png optimization(loss less) (PR #1541)
- Fields rearranged for size optimization (PR #1576)
- refactoring Diff3.h (PR #1577)
- Fix: Mismatch between keyword list and comments (PR #1578)
- DiffFileData optimization for same-files (PR #1579)
- Fixed potentially wrong SubEditList ReadOnly attribute storage; refactoring
    (PR #1580)
- CheckForInvalidUtf8 performance improvement; code cleanup (PR #1582)
- Update unicoder.cpp (PR #1584)
- unicoder.cpp light performance improvements (PR #1586)
- Update markdown.cpp (PR #1590)
- Add a feature for debugging. (PR #1595)

## Known issues

 - Shell integration does not open winmerge when selecting two folders (#1619)
 - Suggestion to make the result of image comparison more reliable (#1391)
 - Crashes when comparing large files (#325)
 - Very slow to compare significantly different directories (#322)
 - Vertical scrollbar breaks after pasting text (#296)
