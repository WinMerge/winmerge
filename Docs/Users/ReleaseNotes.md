# WinMerge 2.16.47 Beta Release Notes

- [About This Release](#about-this-release)
- [What Is New in 2.16.47 beta?](#what-is-new-in-21647-beta)
- [Known issues](#known-issues)

March 2025

## About This Release

This is a WinMerge beta release which is meant for preview the current state of
WinMerge development. This release is not recommended for the production.

Please submit bug reports to our bug-tracker.

## What Is New in 2.16.47 Beta

### General

- BugFix: /inifile won't work with root directory (#2635)
- BugFix: "Another application has updated" message when window is deactivated
    (#2668)
- Reduced flickering of title bar buttons and the status bar.
- Add Visual Studio Dark color scheme (PR #2634)
- Rewrite English.pot, add JXL image extension (PR #2682)

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

### Select Files or Folders dialog

### Installer

- BugFix: Context menu not working if installed from non-elevated (non admin)
    user on win 10 x64 (#2638) (PR #2639)

### Manual

- Help pages -> Appendix A. Shortcut keys (#2659)

### Translations

- Translation updates:
  - Brazilian (PR #2675,#2688)
  - Corsican (PR #2632,#2676)
  - Hebrew (PR #2673)
  - Italian (PR #2689)
  - Japanese
  - Polish (PR #2671)
  - Portuguese (PR #2650)
  - Swedish (PR #2679)
  - Turkish (PR #2636)

### Internals

- BugFix: Fix build in VS2022 17.13.0 (PR #2645)
- BugFix: Use zopflipng to reduce size of splash.png (PR #2646)

## Known issues

 - Suggestion to make the result of image comparison more reliable (#1391)
 - Crashes when comparing large files (#325)
 - Very slow to compare significantly different directories (#322)
 - Vertical scrollbar breaks after pasting text (#296)
