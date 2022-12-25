# WinMerge 2.16.25 Release Notes

- [About This Release](#about-this-release)
- [What Is New in 2.16.25 beta?](#what-is-new-in-21625-beta)
- [Known issues](#known-issues)

December 2022

## About This Release

This is a WinMerge beta release which is meant for preview the current state of
WinMerge development. This release is not recommended for the production.

Please submit bug reports to our bug-tracker.

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
  - Brazilian (PR #1617)
  - Chinese Simplified (PR #1614)
  - Corsican (PR #1628)
  - Galician (#1581)
  - German (PR #1616)
  - Hungarian (PR #1618)
  - Lithuanian (PR #1621)
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
