# WinMerge 2.16.43 Beta Release Notes

- [About This Release](#about-this-release)
- [What Is New in 2.16.43 beta?](#what-is-new-in-21643-beta)
- [Known issues](#known-issues)

October 2024

## About This Release

This is a WinMerge beta release which is meant for preview the current state of
WinMerge development. This release is not recommended for the production.

Please submit bug reports to our bug-tracker.

## What Is New in 2.16.43 Beta

### General

- BugFix: WinMerge incorrectly states "The selected files are identical" (#2408)
- Implement Menu Bar as a Toolbar (PR #2400)
- Add support for placing the tab bar on the title bar (PR #2428)
- Add One-Hand mice wheel scroll diff and merge (PR #2435)
- Add Support for Custom System Color Overrides (PR #2376)
### File compare

- BugFix: Wrong syntax highlighting for TeX files (#2415)
- BugFix: Text replacement result is wrong. (#2422)
- Copy Selected Difference to Clipboard (PR #2429)
- Added Ada parser and default file associations (#2452)
- Added C++20 modules default file associations cppm and ixx (PR #2464)
- Anti-aliasing has been applied to the lines that connect moved blocks in the
    location pane.

### Folder compare

- BugFix: ExitCode always 2 with /enableexitcode with no apparent issues (#2450)

### Options dialog

- Add a "Defaults" button to the "Options (Project)" dialog. (PR #2401)
- Add a "Defaults" button to the "Options (Backup Files)" dialog. (PR #2406)
- Add a "Defaults" button to the "Options (Codepage)" dialog. (PR #2448)
- Add a "Defaults" button to the "Options (General)" dialog. (PR #2453)

### Plugins

- Create CompareEscapedJavaPropertiesFiles.sct (PR #2455)

### Manual

- BugFix: 2 mistakes about default values in manual (#2456)
- Update documentation for "Include unique subfolders contents" setting (PR #2396)

### Translations

- Translation updates:
  - Brazilian (PR #2397,#2431,#2439,#2454,#2460,#2468,#2477,#2486)
  - Chinese Simplified (PR #2394,#2461,#2469,#2485)
  - Dutch (PR #2474)
  - French (PR #2399,#2444)
  - Hungarian (PR #2433,#2440,#2458,#2472,#2482)
  - Italian (PR #2393,#2438,#2446,#2447,#2457,#2467,#2470)
  - Japanese
  - Korean (PR #2426,#2459,#2473)
  - Lithuanian (PR #2436,#2443,#2462,#2471,#2483)
  - Portuguese (PR #2410,#2441,#2480)
  - Russian (PR #2462)

### Internals

- Make InsertLineNumberInPOFiles.bat and RenewPOFiles.bat work again

## Known issues

 - Suggestion to make the result of image comparison more reliable (#1391)
 - Crashes when comparing large files (#325)
 - Very slow to compare significantly different directories (#322)
 - Vertical scrollbar breaks after pasting text (#296)
