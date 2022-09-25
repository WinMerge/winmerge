# WinMerge 2.16.23 Beta Release Notes

- [About This Release](#about-this-release)
- [What Is New in 2.16.23 beta?](#what-is-new-in-21623-beta)
- [Known issues](#known-issues)

September 2022

## About This Release

This is a WinMerge beta release which is meant for preview the current state of
WinMerge development. This release is not recommended for the production.

Please submit bug reports to our bug-tracker.

## What Is New in 2.16.23 Beta
### General

- BugFix: Fix an issue where filenames containing "&" are not displayed
    properly in the MDI tab bar and its tooltips. (PR #1466)

### Color schemes

- Create Midnight.ini (PR #1430)

### File compare

- BugFix: Non existing backup directory should be automatically created (#1438)
- Remember zoom level for all files (#1433)
- The feature will allow the user to right-click the selected lines and… add
    them to Line Filter so that those lines added to the Line Filter will be
    ignored if found in any file. (PR #1481)
- CrystalEdit/parsers/SQL: Added more keywords (PR #1493)

### Table compare

- Bugfix: Inline differences ware not displayed even if the caret is moved to
    the position of an inline difference that is hidden due to the narrow
    column width.

### Webpage compare

- [EXPERIMENTAL] Webpage Compare: Highlight differences (PR #1357) 

### Folder compare

- BugFix: Disable rename operations when in read-only mode in the folder
    compare window. (PR #1434)
- BugFix: Fix an issue where renaming to a file name or directory name
    containing "\" or "/" is not done properly. (PR #1451)
- BugFix: Fix "Left to Right" and "Right to Left" copying in 2-way folder
    comparison. (PR #1495)
- BugFix: Folder compare with jpg images crashes  (#1176)
    (Previous versions were linked with unfixed freeimage.)

### Archive support

- Update 7-Zip to 22.01 (#1425)

### Translations

- Translation updates:
  - Brazilian (PR #1436,#1437,#1441,#1459,#1463)
  - Corsican (PR #1443,#1480,#1486)
  - Dutch (PR #1474)
  - Finnish (PR #1460)
  - French (PR #1491)
  - German (PR #1455,#1484)
  - Hungarian (PR #1431,#1454)
  - Japanese
  - Lithuanian (PR #1457,#1485)
  - Polish (PR #1427,#1456)
  - Portuguese (PR #1453,#1490)
  - Russian (PR #1426)
  - Slovenian (#1424,PR #1461)
  - Spanish (PR #1406)

## Known issues

 - Suggestion to make the result of image comparison more reliable (#1391)
 - Crashes when comparing large files (#325)
 - Very slow to compare significantly different directories (#322)
 - Vertical scrollbar breaks after pasting text (#296)
