# WinMerge 2.16.42.1 Release Notes

- [About This Release](#about-this-release)
- [What Is New in 2.16.42.1](#what-is-new-in-216421)
- [What Is New in 2.16.42](#what-is-new-in-21642)
- [What Is New in 2.16.41 beta?](#what-is-new-in-21641-beta)
- [Known issues](#known-issues)

July 2024

## About This Release

This is a WinMerge 2.16.42.1 stable release.
This release replaces earlier WinMerge stable releases as a recommended release.

Please submit bug reports to our bug-tracker.

## What Is New in 2.16.42.1

### General

- BugFix: Digital signature was not added to version 2.16.42.

### Translations

- Translation updates:
  - Chinese Simplified (PR #2394)
  - Italian (PR #2393)

## What Is New in 2.16.42

### General

- Enable Rounded Corners for WinMerge Menu on Windows 11 (PR #2364)
- Introduce a flat status bar without separator lines. (PR #2377)

### Folder compare

- BugFix: Folder comparison status not accurately reflected in toolbar (#2374)
- BugFix: Status of folder comparison not correctly reflected by tab icon after
    Refresh (F5) (#2383)

### File compare

- Replace - more intelligent choice of replace in (whole file vs selection) (#2368)

### Image compare

- BugFix: Image Comparison : In Alpha Animation, dialogs hang winmerge (#2370) 
- BugFix: Fixed an issue where the right edge of the status bar appeared white.
- Added the ability to change the blink interval and overlay animation interval.

### Plugins

- Make the gpt4-o-mini model selectable in the AIConvertText plugin settings
    window

### Archive support

- Update 7-Zip to 24.07

### Translations

- Translation updates:
  - Chinese Simplified (PR #2392)
  - Brazilian (PR #2391)
  - Hungarian (PR #2388)
  - Lithuanian (PR #2390)

## What Is New in 2.16.41 Beta

### General

- Made the tabs in the tab bar rounded like Firefox.

### File compare

- Added support for F# (FSharp) language syntax highlight (PR #2298)
- Improved F# parsing, support for multi-line comments (PR #2301)
- Multi line comment fix (PR #2303)
- Multi line comment fix part 2 (PR #2309)
- Typescript files to use JavaScript highlighting as default (PR #2312)

### Folder compare

- BugFix: Refresh selected shows wrong file names being compared (#2338)
- BugFix: Selected files changes on expanding a folder that is upper on the
    tree (#2359)

### Options dialog

- BugFix: Options - Syntax color settings get reverted when
    "Customized text colors" settings is touched. (#2355)
- Add a "Defaults" button to the "Options (Editor > Compare/Merge)" dialog. (PR #2348)
- Add a "Defaults" button to the "Options (Archive Support)" dialog. (PR #2362)

### Plugins

- BugFix: Fixed the plugin pipeline not being configured correctly in the Select Plugin dialog.
- Add a text conversion plugin using OpenAI API (PR #2258)
- Update jq to 1.7.1
- Update md4c to 0.5.2

### Archive support

- Update 7-Zip to 24.05

### Installer

- BugFix; WinMerge 2.16.40 - Installer - Missing version and archicteure info (#2300)

### Translations

- Translation updates:
  - Brazilian (PR #2308)
  - Chinese Simplified (PR #2306,#2310,#2314,#2352)
  - Corsican (PR #2361)
  - French (PR #2354)
  - German (PR #2349)
  - Hungarian (PR #2307)
  - Italian (PR #2299)
  - Lithuanian (PR #2318,#2320)
  - Japanese
  - Korean (PR #2333)
  - Polish (PR #2313)
  - Portuguese (PR #2321)
  - Turkish (PR #2315)

### Internals

- Update GitHub Actions CI (#2304)
- Fix some typos (#2305)
- Update POCO C++ Libraries to 1.13.3
- Update boost to 1.85.0
- Update Google C++ Testing Framework to 1.14.0

## Known issues

 - Suggestion to make the result of image comparison more reliable (#1391)
 - Crashes when comparing large files (#325)
 - Very slow to compare significantly different directories (#322)
 - Vertical scrollbar breaks after pasting text (#296)
