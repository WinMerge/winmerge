# WinMerge 2.16.40 Release Notes

- [About This Release](#about-this-release)
- [What Is New in 2.16.40](#what-is-new-in-21640)
- [What Is New in 2.16.39 beta?](#what-is-new-in-21639-beta)
- [Known issues](#known-issues)

April 2024

## About This Release

This is a WinMerge 2.16.40 stable release.
This release replaces earlier WinMerge stable releases as a recommended release.

Please submit bug reports to our bug-tracker.

## What Is New in 2.16.40

### General

- BugFix: Fixed several Wine-related issues

### File compare

- BugFix: Replace text issue (#2279)

### Options dialog

- Add a "Defaults" button to the "Options (Editor > General)" dialog.
    (PR #2283)
- Adjust position of "Defaults" button in "Options > Compare > Table"
    (PR #2295)

### Plugins

- BugFix: CompareMSExcelFiles.sct: Fixed the issue where, when the 'Extract
    workbook data to multiple files' option was enabled, all sheets of .xlsx
    files containing more than 10 sheets were not compared.
    (This bug was introduced in version 2.16.39 beta.)
- BugFix: insert datetime.sct: Fixed an issue where it didn't work properly
    depending on the locale.
    (This bug was introduced in version 2.16.39 beta.)
- BugFix: Fixed an issue where a "Catastrophic failure" message box might have
    appeared when reloading a plugin while the Select Files or Folders window
    was displayed.
- Allow plugin pipeline aliases or simple plugins to be registered in the GUI
    (PR #2257)

### Installer

- BugFix: French encoding problem on the end of installation (last dialog box)
   (#2272)

### Translations

- Translation updates:
  - Brazilian (PR #2289)
  - Catalan (PR #2270,#2274)
  - Chinese Simplified (PR #2290)
  - Corsican (PR #2288)
  - French (PR #2271,#2275,#2297)
  - Hungarian (PR #2291)
  - Japanese
  - Polish (PR#2277)

## What Is New in 2.16.39 Beta

### General

- BugFix: Fixed an issue where typing only a single character in the header
    bar could cause a crash.

### File compare

- BugFix: Fixed possible crash
- BugFix: Creation of .bak files fails when the original filename starts with
    a dot (#2217)
- Update Java syntax highlighting keyword list. (PR #2215)
- Replace "Match similar lines" with "Align similar lines" (PR #2230)
- Make it possible to select the behavior when copying to another pane with
    "Copy to Right" etc. while text is selected.  (PR #2224)
- Make it possible to transform the text copied with "Copy Right/Left" using a
    plugin. (PR #2238)
- Update PHP syntax highlighting keyword list. (PR #2265)

### Webpage compare

- BugFix: Make event sync settings persistent (#2248)

### Select Files or Folders dialog

- BugFix: Fixed the issue when the compare button could not be clicked when
    specifying an archive file and a regular file.

### Reports

- BugFix: Fixed an issue where the widths of the left and right panes in HTML
    reports are not equal.
- BugFix: Fixed an issue where the caption set in the header bar was not being
    applied to the HTML report.

### Plugins

- BugFix: Fixed crash when error occurs in Unpacker plugin
- Replace the source code written in VBScript with JScript (PR #2098)
- Add the ability to replace using patterns from Substitution Filters to the
    Replace plugin. (PR #2252)
- Added sanity check for regular expressions in PrediffLineFilter plugin.

### Installer

- BugFix: If the /noicons option was specified in the previous installation,
    a message box asking you to delete the previous start menu will be
    displayed in the next installation. (#2206)
- Move custom messages in a separate iss file (#2247)

### Archive support

- BugFix: 7zip encrypted archives with encrypted file names (#2225)

### Internals

- Update codeql-analysis.yml - Version v2 to v3 (PR #2196)

### Translations

- Translation updates:
  - Brazilian (PR #2234,#2243)
  - Chinese Simplified (PR #2241,#2244,#2251,#2256,#2259)
  - Corsican (PR #2266)
  - French (PR #2237,#2264)
  - Hungarian (PR #2232,#2250)
  - Italian (PR #2245,#2249)
  - Japanese
  - Korean (PR #2239)
  - Lithuanian (PR #2235,#2246)
  - Portuguese (PR #2263)
  - Russian (PR #2194,#2195,#2198,#2210,#2211,#2212)
  - Slovenian (#2096)

## Known issues

 - Suggestion to make the result of image comparison more reliable (#1391)
 - Crashes when comparing large files (#325)
 - Very slow to compare significantly different directories (#322)
 - Vertical scrollbar breaks after pasting text (#296)
