# WinMerge 2.16.48 Release Notes

- [About This Release](#about-this-release)
- [What Is New in 2.16.48](#what-is-new-in-21648)
- [What Is New in 2.16.47 beta?](#what-is-new-in-21647-beta)
- [Known issues](#known-issues)

April 2025

## About This Release

This is a WinMerge 2.16.46 stable release.
This release replaces earlier WinMerge stable releases as a recommended release.

Please submit bug reports to our bug-tracker.

## What Is New in 2.16.48

### General

- FR: Add `Ctrl+,` keyboard shortcut to open Options dialog box (#2705)
- Update Merge.rc (PR #2711)

### File compare

 - BugFix: [BUG} Phantom line added when identical blocks of text pasted
     (#2702)
 - BugFix: Fixed crash when quitting WinMerge while the search dialog is open
 - Syntax/Pascal: Add initialization and finalization (PR #2719)
 - Syntax/Pascal: Support multiline strings (PR #2720)

### Folder compare

- BugFix: Unable to compare files with zero mtime using
    "Modified Date and Size" (#2714)

### Webpage compare

 - BugFix: Fixed an issue where comparisons could fail when tab characters
     were present.
 - BugFix: Fixed incorrect line wrapping in certain HTML elements.
 - BugFix: Fixed an issue where differences within \<pre\> elements could appear
     concatenated instead of on separate lines.

### Plugins

 - BugFix: ApacheTika and PlantUML plugins: Fixed issue where no error message
     was displayed when execution failed (#715)
 - BugFix: ApacheTika plugin: Fixed failure to run when the filename contained 
     non-alphanumeric characters (now uses the short filename instead) (#715)

### Translations

- Translation updates:
  - Brazilian (PR #2706,#2715,#2726)
  - Chinese Simplified (PR #2710,#2730)
  - Corsican (PR #2722)
  - French (PR #2716)
  - Hungarian (PR #2703,#2713,#2724)
  - Italian (PR #2704,#2723,#2737)
  - Japanese
  - Lithuanian (PR #2708)

## What Is New in 2.16.47 Beta

### General

- BugFix: /inifile won't work with root directory (#2635)
- BugFix: "Another application has updated" message when window is deactivated
    (#2668)
- Reduced flickering of title bar buttons and the status bar.
- Rewrite English.pot, add JXL image extension (PR #2682)
- Add Output Pane for Logs and Messages (PR #2663)

### File compare

- BugFix: Fixed an issue where $ in regular expressions in Substitution filters
    didn't work correctly with "Ignore carriage return differences" enabled.
    (#2640)
- BugFix: Syntax highlighting of Python multiline strings failed on lines > 1
    (#2643)
- BugFix: Fixed an issue where "&" appeared as an underline in the status bar
    when using both Unpacker and Prediffer plugins.
- BugFix: Sync Codepage Settings on Options and Diff Window (#2678)
- Update C syntax highlighting keyword list. (PR #2669)

### Image compare

- Rewrite English.pot, add JXL image extension (PR #2682)

### Options dialog

- BugFix: Fixed an issue where ShellExtensionU.dll was not properly
    unregistered when clicking the Unregister shell extension button in the
    Shell Integration options. (#2419)
- Modify the "Options (Compare > Image)" dialog. (PR #2655)

### Color schemes

- Add Visual Studio Dark color scheme (PR #2634)
- Twilight - new dark theme (PR #2694)

### Installer

- BugFix: Context menu not working if installed from non-elevated (non admin)
    user on win 10 x64 (#2638) (PR #2639)

### Manual

- Help pages -> Appendix A. Shortcut keys (#2659)

### Translations

- Translation updates:
  - Brazilian (PR #2675,#2688,#2697)
  - Chinese Simplified (PR #2700) 
  - Corsican (PR #2632,#2676)
  - Hebrew (PR #2673)
  - Italian (PR #2689,#2689)
  - Japanese
  - Polish (PR #2671)
  - Portuguese (PR #2650,#2695)
  - Swedish (PR #2679)
  - Turkish (PR #2636)

### Internals

- BugFix: Fix build in VS2022 17.13.0 (PR #2645)
- BugFix: Use zopflipng to reduce size of splash.png (PR #2646)
- README major update, Twilight theme minor edit (PR #2696)

## Known issues

 - Pressing OK in the Options window while the INI file specified by /inifile is open for comparison in WinMerge may corrupt the file. (#2685)
 - Suggestion to make the result of image comparison more reliable (#1391)
 - Crashes when comparing large files (#325)
 - Very slow to compare significantly different directories (#322)
 - Vertical scrollbar breaks after pasting text (#296)
