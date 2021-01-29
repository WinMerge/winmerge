# WinMerge 2.16.10 Release Notes

- [About This Release](#about-this-release)
- [What Is New in 2.16.10?](#what-is-new-in-21610)
- [What Is New in 2.16.9 beta?](#what-is-new-in-2169-beta)
- [Known issues](#known-issues)

January 2021

## About This Release

This is a WinMerge 2.16.10 stable release.
This release replaces earlier WinMerge stable releases as a recommended release.

Please submit bug reports to our <a href="http://github.com/winmerge/winmerge/issues">bug-tracker</a>.

## What Is New in 2.16.10

### General

- BugFix: Fixed processing that uses GetAsyncKeyState(). (GitHub PR #505)
- BugFix: Fixed the problem that the language setting is not applied to the
    context menu of the MDI tab when the language setting is changed. 
    (GitHub PR #523)
- BugFix: command-line option /x did not work for binary and image comparison
- BugFix: Substiturions vs. Substitutions (GitHub #551)
- Swap Panes functionality for 3 way compares (Issue #507) (GitHub PR #533)
- [feature request] Add options to command line to change "compare methods"  
    (GitHub #530)
- Added the command line option "/s-" to ensure that another instance is
    always executed, ignoring the value of the 'Allow only one instance to 
    run' option.

### File compare

- BugFix: WinMerge crashes with mouse double click (GitHub #531)
- BugFix: Fixed an issue where the message box "The report has been created
    successfully." was displayed even if the report creation failed.
- BugFix: BUG: Doesn't respect "Don't ask this question again" checkbox when
    comparing large files (GitHub #574)
- The Location pane doesn't draw a line connecting moved differences by
    default (GitHub #498)
- Update crystallineparser.cpp - AutoIt (\*.au3) support as a "Basic"
    (GitHub PR #543)
- s\_apszAutoItKeywordList[] - actualization for AutoIt (GitHub PR #584)
- Added JavaScript and JSON syntax highligher (osdn.net #41083)
- Added View -> Diff Context -> Invert menu item
- Feature Request: Move To Next File option while comparing files #475
    (GitHub PR #561)
- A new feature "Ignored Substutions" (GitHub PR #544,#549,#560)
  ("Ignored Substitutions" was renamed to "Substitution Filters")

### Folder compare

- BugFix: Winmerge crashes consistently when deleting files
    (GitHub #491)
- BugFix: Copy Folder does not copy subfolders and I don't see any option for it
    (GitHub #537)

### Table compare

- Added File -> New -> Table menu item

### Binary compare

- Added File -> New -> Binary menu item

### Image compare

- Implement copy and paste images (GitHub PR #524)
- Added File -> New -> Image menu item
- Added Image -> Compare Extracted Text From Image menu item

### Filter dialog

- Making CPropertySheet resizable needs some re-work. (Issue #509)
    (GitHub PR #535)
  
### Patch Generator dialog

- BugFix: Crash when generating patch (GitHub #521)

### Translations

- Translation updates:
  - Dutch (GitHub #578,#579,#580)
  - Galician (GitHub PR #493,#538,#570)
  - German (GitHub PR #532,#548,#563,#585)
  - Japanese
  - Lithuanian (GitHub PR #496,#528,#536,#562,#564)
  - Portuguese (GitHub PR #495)
  - Slovenian (GitHub #553,#565)
  - Russian (GitHub PR #494)

### Manual

- Update manual (GitHub PR #497,#513,#546)
- Small tweaks for the Manual (GitHub PR #508)
- Windows XP Pro SP0 vs Requirements (GitHub #515)

### Internals

- Fix Various Warnings (GitHub PR #539)
- Various fixes to testing (GitHub PR #545)
- Some tweaks for translations status (GitHub PR #556)
- Link error when `building Plugins\src_VCPP\DisplayXMLFiles` with Visual
    Studio 2019 16.7.7 (GitHub #554)
- Link error when building VCPP plugin DisplayXMLFiles with Visual Studio
    2019 16.7.7 (GitHub #555)
- Link warnings when building VCPP plugin DisplayXMLFiles with Visual Studio
    2019 16.7.7 (GitHub #558)
- Some more files should be added to the .gitignore file (GitHub #559)

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
    - Galician (GitHub PR #419,#458,#493)
    - German (GitHub PR #438,#448,#480,#490)
    - Lithuanian (GitHub PR #417,#439,#443,#449,#481)
    - Polish (GitHub PR #447)
    - Portuguese (GitHub PR #429,#467)
    - Slovak (GitHub PR #470)
    - Slovenian (GitHub PR #486,#488)
    - Spanish (GitHub PR #454)
    - Turkish (GitHub PR #425)

### Manual

  - Update Shortcut_keys.xml (GitHub PR #430)
  - Update manual (GitHub PR #485,#492)

### Internals

  - Tweak output from BuildInstaller.cmd and BuildArc.cmd (GitHub PR #424)
  - Fix typo in GhostTextBuffer.cpp (GitHub PR #472)
  - Fix typo in memdc.h (GitHub PR #474)

## Known issues

 - Crashes when comparing large files (GitHub #325)
 - Very slow to compare significantly different directories (GitHub #322)
 - Vertical scrollbar breaks after pasting text (GitHub #296)
