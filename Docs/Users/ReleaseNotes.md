# WinMerge 2.16.36 Release Notes

- [About This Release](#about-this-release)
- [What Is New in 2.16.36?](#what-is-new-in-21636)
- [What Is New in 2.16.35 beta?](#what-is-new-in-21635-beta)
- [Known issues](#known-issues)

November 2023

## About This Release

This is a WinMerge 2.16.36 stable release.
This release replaces earlier WinMerge stable releases as a recommended release.

Please submit bug reports to our bug-tracker.

## What Is New in 2.16.36

### Image compare

- BugFix: Fix an issue where opening read-only and multi-page image files
    would cause them to be treated as only one-page images.
    (winmerge/winimerge#32)

### Webpage compare

- Improved performance when there are many differences

### File filter

- Bugfix: Modify the "File Filters" dialog. (#2118) 

### Translations

- Translation updates:
  - Galician (PR #2120)
  - Portuguese (PR #2119)
  - Spanish (PR #2120)
  - Slovenian
  - Turkish (PR #2116)

## What Is New in 2.16.35 Beta

### File compare

- BugFix: Fixed an issue where "Copy to Left/Right and Advance" would
    sometimes skip differences when comparing 3 files. (#1234)
- BugFix: Print Preview buttons are not translatable (#2083,#2079)
- BugFix: Fixed the issue where the caption set in the header bar is restored
    when the window is resized.

### Binary compare

- BugFix: V2.16.34 "X86" wired action (#2081)

### Image compare

- BugFix: Close main window with 'Esc' if there is only one MDI child window
    (#2084)

### Webpage compare

- BugFix: Close main window with 'Esc' if there is only one MDI child window
    (#2084)
- Webpage Compare: synchronize events (#2111,#2064) 

### Folder compare

- BugFix: Ignoring carriage return differences doesn't work anymore
    (#2080,#2099)

### File filters

- BugFix: Fixed the issue where "[F]" could be set as the file filter if no file
    filter was selected in the Filters dialog.

### Installer

- BugFix: fix bug of Registry path (PR #2086)
- BugFix: "Register Windows 11 Shell Extension" fails because PowerShell script
    doesn't escape special chars correctly (#2109)

### Translations

- Translation updates:
  - Brazilian (PR #2088,#2112)
  - Bulgarian (PR #2105)
  - Chinese Simplified (PR #2091,#2113)
  - French (PR #2106)
  - Galician (PR #2085,#2107)
  - Hungarian (PR #2093)
  - Japanese
  - Korean (PR #2092)
  - Lithuanian (PR #2090,#2114)
  - Polish (PR #2087)
  - Romanian (PR #2089,#2095)
  - Slovenian (#2096)
  - Spanish (PR #2085,#2107)
  - Turkish (PR #2076)

## Known issues

 - Suggestion to make the result of image comparison more reliable (#1391)
 - Crashes when comparing large files (#325)
 - Very slow to compare significantly different directories (#322)
 - Vertical scrollbar breaks after pasting text (#296)
