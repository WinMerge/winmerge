# WinMerge 2.16.13 Beta Release Notes

- [About This Release](#about-this-release)
- [What Is New in 2.16.13 beta?](#what-is-new-in-21613-beta)
- [Known issues](#known-issues)

June 2021

## About This Release

This is a WinMerge beta release which is meant for preview the current state of
WinMerge development. This release is not recommended for the production.

Please submit bug reports to our <a href="http://github.com/winmerge/winmerge/issues">bug-tracker</a>.

## What Is New in 2.16.13 Beta

### General

- BugFix: Register.bat did not work properly on the Chinese version of
    Windows XP (#780)
- Possibility to store settings in INI file (#248) (PR #750)
- FeatureRequest - Ignoring options - lack in "button menu" (#804)

### File compare

- BugFix: Fix PHP syntax highlighting. (PR #782, PR #802)
- BugFix: BS key did not work at the beginning of the line after splitting
    the pane or clicking the OK button in the Options dialog.
- BugFix: The edited contents were discarded when the "Recompare As" menu item
    was selected after editing the file
- BugFix: Incorrect comparison (#834)
    (This bug occured when enabling both the 'Ignore blank lines' option and
     the 'Completely unhighlight the ignored differences' option.)
- Add Smarty syntax highlighting. (PR #821)
- Thicken the caret in overwrite mode (osdn.net #42179)

### Folder compare

- BugFix: Different Files are Identical? (#768)
    (When comparing files with only BOM and no contents, the comparison result
     became unstable because it referred to the uninitialized memory.)
- BugFix: Fix a crash when re-comparing folders (osdn.net #42219)

### Binary compare

- BugFix: The file could not be saved after creating a new one
- Made Unpacker plugins available for image compare and binary compare

### Image compare

- BugFix: Error on try to show differences between two different gif (#784)
- Made Unpacker plugins available for image compare and binary compare

### Plugins

- Improve plugin system (PR #797)
  - Added categorized Unpacker plugin menu items to various menus 
  - Made it possible to specify multiple Unpacker/Prediffer plugins by
      concatenating them with | and to specify arguments.
  - Allowed Unpacker/Prediffer plugins to be specified for multiple files at
      once in Folder Compare window. The specified plugin will be visible in
      the Unpacker and Prediffer columns
  - Add "(U)" or "(P)" to the tab title of the window opened by the
      Unpacker/Prediffer plugin.
  - Allowed the plugin settings dialog to specify default arguments that will
      be used when no plugin arguments are specified
  - Allowed plugins used by Automatic Unpacking/Prediffing to be excluded in
      the plugin settings dialog
  - New unpacker plugins:
    - ApacheTika
    - PrettifyJSON
    - PrettifyXML
    - QueryCSV
    - QueryTSV
    - QueryJSON
    - MakeUpper
    - MakeLower
    - RemoveDuplicates
    - CountDuplicates
    - SortAscending
    - SortDescending
    - ExecFilterCommand
    - Tokenize
    - Trim
    - VisualizeGraphviz
  - New editor script plugins:
    - RemoveDuplicates
    - CountDuplicates
    - Tokenize
    - Trim
    - ApacheTika
    - PrettifyJSON
    - PrettifyXML

### Translations

- BugFix: Fix an issue where a message is not translated.(PR #763)
- Translation updates:
  - French (PR #762)
  - Japanese
  - Polish (PR #769)
  - Turkish (PR #803)
  - Russian (PR #761)

### Internals

- Update CWindowsManagerDialog (PR #811)
- Update CWindowsManagerDialog - check some pointers for null and made safe
    casts (PR #824)


## Known issues

 - Crashes when comparing large files (GitHub #325)
 - Very slow to compare significantly different directories (GitHub #322)
 - Vertical scrollbar breaks after pasting text (GitHub #296)
