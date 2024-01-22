# WinMerge 2.16.38 Release Notes

- [About This Release](#about-this-release)
- [What Is New in 2.16.38](#what-is-new-in-21638)
- [What Is New in 2.16.37 beta?](#what-is-new-in-21637-beta)
- [Known issues](#known-issues)

January 2024

## About This Release

This is a WinMerge 2.16.38 stable release.
This release replaces earlier WinMerge stable releases as a recommended release.

Please submit bug reports to our bug-tracker.

## What Is New in 2.16.38

### File compare

- BugFix: Fixed an issue where changing the BOM in the right pane in the
    Codepage dialog was not reflected.
- Update C++ syntax highlighting keyword list. (PR #2166)

### Webpage compare

- During comparison, the status bar now displays "Comparing...".
- Add Location Pane (PR#2160)

### Folder compare

- Modify the "Display Columns" dialog. (PR#2154)

### Plugins

- BugFix: Fixed the issue where the following string containing double
    quotes is not interpreted correctly when specified to the `/unpacker`
    command line argument.
    
    `/unpacker "Replace ""a"" ""b"""`

### Translations

- Translation updates:
  - Brazilian (PR#2151,#2178)
  - Chinese Simplified (PR#2153,#2183)
  - Corsican (PR#2180)
  - Hungarian (PR#2156,#2157,#2158)
  - Japanese
  - Korean (PR#2152)
  - Lithuanian (PR#2155,#2177)
  - Portuguese (PR#2185)

## What Is New in 2.16.37 Beta

### File compare

- BugFix: Fixed an issue where the scroll position when clicking on Location 
    View may not be as expected when Wrap Lines is enabled.
- BugFix: Can't click and change file line endings with the version 2.16.36.
    (#2129)
- BugFix: Unable to locate CR using \r in regular expression search
- Added new C# keywords (PR#2136)

### Folder compare

- BugFix: Empty files are not copied (#2146)

### Binary compare

- BugFix: Could not replace data at the end of the file

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
  - French (PR #2135,#2140,#2141,#2142,winmerge/frhed#15)
  - Italian (PR #2130)
  - Korean (PR #2126,#2127,#2143)
  - Polish (PR #2128)


## Known issues

 - Suggestion to make the result of image comparison more reliable (#1391)
 - Crashes when comparing large files (#325)
 - Very slow to compare significantly different directories (#322)
 - Vertical scrollbar breaks after pasting text (#296)
