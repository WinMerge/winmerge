# WinMerge 2.16.45.0 Beta Release Notes

- [About This Release](#about-this-release)
- [What Is New in 2.16.45.1 beta?](#what-is-new-in-216451-beta)
- [What Is New in 2.16.45.0 beta?](#what-is-new-in-216450-beta)
- [Known issues](#known-issues)

November 2024

## About This Release

This is a WinMerge beta release which is meant for preview the current state of
WinMerge development. This release is not recommended for the production.

Please submit bug reports to our bug-tracker.

## What Is New in 2.16.45.1 Beta

### General

- BugFix: IME switch not toggle menu when menu hide (PR #2572)
- Add "Ignore missing trailing EOL" option to Compare settings
    (PR #2573,#203,#989,#2317)

### File compare

- BugFix: The height of the lines connecting moving blocks in the location
    pane may be incorrect (#2543)

### Folder compare

- BugFix: Looks like the View Tab Bar "folder icon" is getting a green
    check-mark before the folder compare has even completed. (#2562)

### Plugins

- BugFix: WinMerge 2.16.44.0 : CompareMSWordFiles.sct Plugin : Forced Enabled
    Confirm Conversions Pop-Up (#2566)

### Archive support

- Update 7-Zip to 24.09

### Translations

- Translation updates:
  - Japanese
  - Korean (PR #2558,#2559)

### Internals

- Update POCO C++ Libraries to 1.14.0

## What Is New in 2.16.45.0 Beta

### General

- BugFix: Second monitor - BIG title bar v 2.16.44 (#2510,#2512,#2540,#2549)
- BugFix: Window close button is not extending to the edge of the screen
    when maximized (#2520)
- BugFix: When WinMerge window is maximized, it prevents auto-hidden Windows
    11 taskbar from appearing (#2518)
- Foreground and background title bars are distinguishable (PR #2497)
- Title bar background color and text color follow windows settings when not
    customized (PR #2505)
- Main Menu toggle Show/Hide (PR #2509)
- Inactive title text chosen by inactive title color (PR #2544)
- Reduce the contrast of inactive title bar text (PR #2545)
- System Icon turn gray when app inactive (PR #2547)

### File compare

- BugFix: Wrong line gets copied over when selecting text (#2499)
- BugFix: Bug Report: Display Issues When Resizing WinMerge Across Dual
    Monitors (#2514)
- BugFix: Diff renders incorrectly with "Cascadia Mono" variant (#2525)
- BugFix: The height of the lines connecting moving blocks in the location
    pane may be incorrect
- BugFix: Crash on opening 2 files via drag & drop (#2548)
- BugFix: A crash could occur if a single invalid file path was specified in
    the "Select files or folders" window.
- BugFix: "Debug Assertion Failed!" Error When Refreshing After Deleting All
    Content in Left or Right Pane (#2556)
- Adjust right-click deselect from down to up to preserve RB + scroll anywhere
    (PR #2511)
- Press 1-3 to copy current diff line to clipbd while popup menu is open
    (PR #2532)
- Caret (text cursor) follow windows settings (PR #2534)
- Feature Request: Fixed-Width Font in "Find what" and "Replace with" Fields
    (#2539)

### Folder compare

- BugFix: Crash on typing while search is in progress (#1814), (PR #2557)

### Options dialog

- Add a "Defaults" button to the "Options (Colors/System)" dialog. (PR #2506)
- The "Clear all recent items" button in the Shell Integration category of the
    Options dialog now clears combobox history as well as jump lists. (#2555)

### Select Files or Folders dialog

- BugFix: Crash on opening 2 files via drag & drop (#2548)

### Plugins

- Apache Tika plugin: Update Apache Tika to 3.0.0

### Installer

- BugFix: When uninstalling WinMerge, the Shell Extension for Windows 11 was
    not uninstalled.
- Installer - Run PowerShell hidden and with -NoProfile when installing
    silently (#2515)

### Translations

- Translation updates:
  - Brazilian (PR #2526)
  - Chinese Simplified (PR #2501,#2524) 
  - French (PR #2536)
  - Hungarian (PR #2495,#2523)
  - Italian (PR #2529)
  - Japanese
  - Korean (PR #2496,#2533)
  - Lithuanian (PR #2503,#2537)
  - Portuguese (PR #2507)
  - Russian (PR #2500,#2502,#2521,#2522,#2528,#2531,#2538,#2542)

## Known issues

 - Suggestion to make the result of image comparison more reliable (#1391)
 - Crashes when comparing large files (#325)
 - Very slow to compare significantly different directories (#322)
 - Vertical scrollbar breaks after pasting text (#296)
