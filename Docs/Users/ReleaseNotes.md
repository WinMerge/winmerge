# WinMerge 2.16.37 Beta Release Notes

- [About This Release](#about-this-release)
- [What Is New in 2.16.37 beta?](#what-is-new-in-21637-beta)
- [Known issues](#known-issues)

December 2023

## About This Release

This is a WinMerge beta release which is meant for preview the current state of
WinMerge development. This release is not recommended for the production.

Please submit bug reports to our bug-tracker.

## What Is New in 2.16.37 Beta

### File compare

- BugFix: Fixed an issue where the scroll position when clicking on Location 
    View may not be as expected when Wrap Lines is enabled.
- BugFix: Can't click and change file line endings with the version 2.16.36.
    (#2129)
- Added new C# keywords (PR#2136)

### Image compare

- BugFix: Fixed an issue where the file name was not displayed in the header
    even after saving a newly created pane with a name.

### Webpage compare

- BugFix: Fixed an issue where the message box "Another application has updated
    file ... since WinMerge scanned it last time" is displayed when comparing
    URLs that are file://.
- Improved synchronize events (winmerge/winwebdiff#4) 

### Translations

- Translation updates:
  - Bulgarian (PR #2124)
  - French (PR #2135,#2140)
  - Italian (PR #2130)
  - Korean (PR #2126,2127)
  - Polish (PR #2128)


## Known issues

 - Suggestion to make the result of image comparison more reliable (#1391)
 - Crashes when comparing large files (#325)
 - Very slow to compare significantly different directories (#322)
 - Vertical scrollbar breaks after pasting text (#296)
