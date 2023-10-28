# WinMerge 2.16.34 Release Notes

- [About This Release](#about-this-release)
- [What Is New in 2.16.34?](#what-is-new-in-21634)
- [What Is New in 2.16.33 beta?](#what-is-new-in-21633-beta)
- [Known issues](#known-issues)

October 2023

## About This Release

This is a WinMerge 2.16.34 stable release.
This release replaces earlier WinMerge stable releases as a recommended release.

Please submit bug reports to our bug-tracker.

## What Is New in 2.16.34

### File compare

- Allow NUL and \\.\NUL in paths specified as command line arguments (PR #2056)
- Added "(F4)" to the description of the toolbar icon "Difference in the Current Line". (#2050)
- Fix a crash problem when the Diff algorithm is set to something other than
    default and one file does not have an EOL of the last line and the other
    file has an EOL of the last line.

### Image compare

- BugFix:  Fixed the issue where .png is added when saving even if the file
    format does not use a converter such as jpg or png file.

### Folder compare

- BugFix: Folder compare omits unique folders from results if they contain any
    files and/or subfolders folder compare/merge (#2046)

### Line filters

- Modify the "Line Filters" dialog. (PR #2047)

### Substitution filters

- Modify the "Substitution Filters" dialog. (PR #2068)

### Translations

- Translation updates:
  - French (PR #2055,#2058)
  - Korean (PR #2057,#2059,#2060)

### Internals

- Small code changes for CMDIChildWnd handling (#2043)

## What Is New in 2.16.33 Beta

### General

- Reduce startup time and decrease the usage of Win32 user objects.

### File compare

- BugFix: Cannot compare one-line file (#1972)
- BugFix:  "Current Difference" specified by double-clicking cannot merge
    using the "Copy to Right (or Left)" menu. (#1980)
- BugFix: Wimerge saves changes to the wrong file (#1985) (PR #1988)
- BugFix: "Ignore comment differences" still compares inline comments (#2008)
- Update Rust syntax highlighting keyword list. (PR #1998)
- [Feature Request] Both Shell Menu (#1986) (PR #2021)

### Table compare

- When "Use First Line as Header" is enabled, make the header display the
    first line regardless of the scroll position when the first line is hidden.
- Generate reports in tabular format for table comparisons. (PR #1983)

### Folder compare

- BugFix: Fixed an issue where Differences, Left/Right EOL columns, etc. were
    displayed as undefined values when the file comparison method was
    Full Contents or Quick Contents and the file size exceeded 64MB.
- BugFix: Fix the problem that when comparing with the BinaryContents compare
    method, the contents of the files are identical, but if one side is a
    symbolic link, it is judged to be different. (#1976)
- BugFix: Fixed an issue where values in the Left/Right EOL column may not
    display correctly when using the Quick contents compare method.
- Add Expand Different Subfolders menu item (#1382) (PR #1964)
- Allow Diff algorithms (patience, histogram) other than default to be applied
    to folder comparisons (PR #2015) (#2002)
- Show confirmation message when closing a window that took a long time
    to compare folders

### Line filters

- Improve line filters and substitution filters (PR #2032) (#796) (#1620)

### Substitution filters

- Avoid infinite loops in the RegularExpression::subst() function when 
    the length of the string matching the pattern is 0
- Improve line filters and substitution filters (PR #2032) (#796) (#1620)

### Options dialog

-  Execute the "pause" command to prevent the error message from disappearing
     if the registration of the ShellExtension for Windows 11 fails

### Plugins

- BugFix: WinMerge cannot successfully disable some of its Plugins (#2012)
- Update jq to version 1.7

### Manual

- Manual: Use po4a for manual translation (PR #1994) (#499)

### Translations

- Translation updates:
  - Brazilian (PR #1969,#2001,#2025)
  - Chinese Traditional (PR #1953,#1971,#2017,#2026)
  - Corsican (PR #2022)
  - German (PR #1952,#1977,#1989)
  - Hungarian (PR #1968,#1999)
  - Japanese
  - Korean (PR #1979,#2030)
  - Lithuanian (PR #1974,#2018,#2027)
  - Polish (PR #1990)
  - Portuguese (PR #1973,#2014)
  - Slovenian
  - Ukrainian (PR #1955)

### Internals

- Optimize inserts in std containers using reserve (PR #2000)


## Known issues

 - Suggestion to make the result of image comparison more reliable (#1391)
 - Crashes when comparing large files (#325)
 - Very slow to compare significantly different directories (#322)
 - Vertical scrollbar breaks after pasting text (#296)
