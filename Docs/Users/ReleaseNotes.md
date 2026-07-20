# WinMerge 2.16.58 Release Notes

* [About This Release](#about-this-release)
* [What Is New in 2.16.58?](#what-is-new-in-21658)
* [What Is New in 2.16.57 Beta?](#what-is-new-in-21657-beta)
* [Known issues](#known-issues)

July 2026

## About This Release

This is a WinMerge 2.16.58 stable release.
This release replaces earlier WinMerge stable releases as a recommended release.

Please submit bug reports to our bug-tracker.

## What Is New in 2.16.58?

### File compare

* BugFix: Crash (Access Violation) when file changes externally over a given
    threshold (#3456, PR #3457)

* BugFix: Parts of the panes become black text on a black background after
    copying changes in 3-way merge mode (#3468, #3469)

### Folder compare

* BugFix: Fix archive vs folder comparison selecting the wrong source
    (#3449, PR #3458)

* BugFix: Fix HICON resource leaks in image list initialization

### Select files or folders dialog

* BugFix: Handle exceptions raised while creating the common file dialog
    (#3462, PR #3464)

* BugFix: Fix MRU handling for three-way comparisons. (#3452)

### Reports

* Add support for generating a single HTML report from multiple file
    comparisons (PR #3450)

### Generate Patch dialog

* Add a file list with checkboxes to the Generate Patch dialog (PR #3470)

### Command line

* Add /g command line option for single-instance groups (PR #3472)

### Translations

* Translation updates:

  * Brazilian (PR #3459, PR #3476)
  * Chinese Simplified (PR #3460)
  * French (PR #3478)
  * Italian (PR #3455, PR #3473, PR #3477)
  * Japanese
  * Korean (PR #3447)
  * Lithuanian (PR #3461, PR #3471)
  * Polish (PR #3463)
  * Turkish (PR #3465, PR #3474)
  * Russian (PR #3448)

## What Is New in 2.16.57 Beta?

### General

* BugFix: Fixed an issue where file mapping failures could incorrectly trigger a crash (#3412)

* Add module-relative offsets to crash log stack traces

### File compare

* BugFix: Avoid crash (issue #3402) by using GetFullLineLength() to avoid accessing line EOL directly

* BugFix: Fix incorrect caret position in status bar

* BugFix: Clear sync points before reloading compare files

* BugFix: Fix assertion failure on empty buffer access when all lines are deleted from a pane

* Add display line filter bar for file compare window (PR #3374)

* Improve syntax highlighting with Tree-sitter integration for better support of multiple languages (PR #3306, PR #3337, PR #3338, PR #3350, PR #3415, PR #3427, PR #3440)

* Refactor syntax parsing behind ISyntaxParser interface (PR #3415)

### Folder compare

* BugFix: Crash when trying to delete a "left-only" or "right-only" file during renaming (#3411)

* BugFix: ZIP export includes unchanged files in subfolders (#3444, PR#3445)

* Refactor filter evaluation parameters with new EvalContext for better code maintainability (PR #3353)

### Archive support

* BugFix: Fix error dialogs when comparing two Rar4 files (#3392)

* Update 7-Zip to 26.02

### Select Files or Folders dialog

* Add support for clipboard URL comparison and clipboard history menu (PR #3352)

### Plugins

* Add HandleSchemeClipboard plugin (PR #3352)

### Project files

* Warn user before opening project files containing plugin arguments for security reasons (PR #3397, #3396)

### Translations

* Translation updates:

  * Brazilian (PR #3431)
  * Chinese Simplified (PR #3429, PR #3438)
  * Chinese Traditional (PR #3426)
  * Corsican (PR #3420)
  * French (PR #3393, PR #3406, PR #3408, PR #3435)
  * German (PR #3432)
  * Italian (PR #3399, PR #3434, PR #3446)
  * Korean (PR #3422, PR #3433)
  * Lithuanian (PR #3405, PR #3439)
  * Polish (PR #3418, PR #3430)
  * Portuguese (PR #3391, PR #3414)
  * Russian (PR #3395, PR #3428)
  * Spanish (PR #3421, PR #3423, PR #3424, PR #3425)
  * Turkish (PR #3403, PR #3436)

## Known issues

* Pressing OK in the Options window while the INI file specified by /inifile is open for comparison in WinMerge may corrupt the file. (#2685)
* Suggestion to make the result of image comparison more reliable (#1391)
* Crashes when comparing large files (#325)
* Very slow to compare significantly different directories (#322)
* Vertical scrollbar breaks after pasting text (#296)

