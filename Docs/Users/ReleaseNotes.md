# WinMerge 2.16.57 Beta Release Notes

* [About This Release](#about-this-release)
* [What Is New in 2.16.57 Beta?](#what-is-new-in-21657-beta)
* [Known issues](#known-issues)

July 2026

## About This Release

This is a WinMerge beta release which is meant for preview the current state of
WinMerge development. This release is not recommended for the production.

Please submit bug reports to our bug-tracker.

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

* Improve syntax highlighting with Tree-sitter integration for better support of multiple languages (PR #3306, PR #3337, PR #3338, PR #3350, PR #3415, PR #3427)

* Refactor syntax parsing behind ISyntaxParser interface (PR #3415)

### Folder compare

* BugFix: Crash when trying to delete a "left-only" or "right-only" file during renaming (#3411)

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
  * Italian (PR #3399, PR #3434)
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

