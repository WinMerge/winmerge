# WinMerge 2.16.9 Beta Release Notes

- [About This Release](#about-this-release)
- [What Is New in 2.16.9 beta?](#what-is-new-in-2169-beta)
- [Known issues](#known-issues)

November 2020

## About This Release

This is a WinMerge beta release which is meant for preview the current state of
WinMerge development. This release is not recommended for the production.

Please submit bug reports to our <a href="http://github.com/winmerge/winmerge/issues">bug-tracker</a>.

## What Is New in 2.16.9 Beta?

### General

  - BugFix: MainFrm.cpp: Toolbar was leaking image lists (GitHub PR #432)
  - BugFix: The icons on a 43" 4K screen are too small (GitHub #276)
  - Update English.pot (GitHub #440)
  - Update Merge.rc (GitHub #487)
  - Improved startup time

### File compare

  - BugFix: [UI] Pane enlargement was reset after changing tab (GitHub #403)
  - BugFix: Non-comment differences were sometimes ignored when the comment
      filter was enabled, (osdn.net #40488)
  - BugFix: Line Filters aren't applied when using a 3rd file (GitHub #395)
  - BugFix: "Ignore blank lines" does not work in 3-way compare (GitHub #450)
  - BugFix: Fix the problem that the comparison result is displayed one line
      off in the file compare window if a sync point is added with the first
      line selected. (GitHub PR #435)
  - BugFix: Fix the problem accessing the text buffer out of range if a sync
      point is added by selecting a ghost line that is after the last block.
      (GitHub PR #457)
  - BugFix: Fix the problem that "Goto...", "Goto Line <line>" and moving by
      left click on the location pane do not work appropriately when the Diff
      pane is active in the file compare window. (GitHub PR #476)
  - BugFix: Reloading file does not refresh its encoding (GitHub #466)
  - BugFix: Editor text display is abnormal - character spacing problem
      (GitHub #468)
  - always copy "full line" instead of "selected text only" - option
      CopyFullLine (GitHub PR #459)
  - Add the feature "Go to Moved Line" requested by #278 (GitHub PR #484)
  - how to show white space with linebreak hidden? (GitHub #265)
    (Added View->View EOL menu item)

### Folder compare

  - BugFix: Pausing comparing doesn't pause immediately (GitHub #342)
  - BugFix: Sorting on Comparison Result being done incorectly (GitHub #483)
  - Commandline to display versions in the output report (GitHub #418)
  - "Don't ask again" checkbox for the Confirm Copy dialog (GitHub PR #445)

### Binary compare

  - BugFix: Scrolling binary files (GitHub #456)

### Options dialog
  
  - Added "Automatically scroll to first inline difference" option to Options
    dialog

### Patch Generator dialog

  - BugFix: Fixed the problem that the input to File1 or File2 comboboxes of
      the Patch Generator dialog is not applied when the Patch Generator dialog
      is opened by selecting multiple files. (GitHub PR #421)

### Translations

  - Translation updates:
    - Chinese Simplified (GitHub PR #465)
    - Dutch (GitHub PR #482)
    - Galician (GitHub PR #419,458,493)
    - German (GitHub PR #438,448,480,490)
    - Lithuanian (GitHub PR #417,439,443,449,481)
    - Polish (GitHub PR #447)
    - Portuguese (GitHub PR #429,467)
    - Slovak (GitHub PR #470)
    - Slovenian (GitHub PR #486,488)
    - Spanish (GitHub PR #454)
    - Turkish (GitHub PR #425)

### Manual

  - Update Shortcut_keys.xml (GitHub PR #430)
  - Update manual (GitHub PR #485,492)

### Internals

  - Tweak output from BuildInstaller.cmd and BuildArc.cmd (GitHub PR #424)
  - Fix typo in GhostTextBuffer.cpp (GitHub PR #472)
  - Fix typo in memdc.h (GitHub PR #474)

## Known issues

 - Crashes when comparing large files (GitHub #325)
 - Very slow to compare significantly different directories (GitHub #322)
 - Vertical scrollbar breaks after pasting text (GitHub #296)
