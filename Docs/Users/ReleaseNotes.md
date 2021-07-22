# WinMerge 2.16.14 Release Notes

- [About This Release](#about-this-release)
- [What Is New in 2.16.14?](#what-is-new-in-21614)
- [What Is New in 2.16.13 beta?](#what-is-new-in-21613-beta)
- [Known issues](#known-issues)

July 2021

## About This Release

This is a WinMerge 2.16.14 stable release.
This release replaces earlier WinMerge stable releases as a recommended release.

Please submit bug reports to our <a href="http://github.com/winmerge/winmerge/issues">bug-tracker</a>.

## What Is New in 2.16.14

### General

- Fixed an issue where the WinMerge process might not terminate even though the
    WinMerge window was closed.

### File compare

- BugFix: Fixed an infinite loop when "find what" in the substitution filters
   is empty.

### Folder compare

- BugFix: Fix an issue where a file is deselected when returning to the folder
    compare window after opening the file compare window by double-clicking
    the file in the folder compare window. (PR #857)
- Right click context menu - Compare files or folders in a new tab (#232,#277)

### Binary compare

- BugFix: Fixed an issue where window titles may not be updated

### Image compare

- winmerge shows (differences) rotated image (winmerge/winimerge #20)
- Added following menu items to the context menu
  - Rotate Right 90deg
  - Rotate Left 90deg
  - Flip Vertically
  - Flip Horizontally

### Options dialog

- Add preference option to clear "Don't ask this question again"
    CompareLargeFiles choice (#772, PR #859)

### Select Files or Folders dialog

- BugFix: Fix the Select Files or Folders dialog. (PR #882,#892)

### Plugins

- BugFix: CompareMSExcelFiles.sct: "This picture only contains a bitmap" was
    displayed when comparing Excel files that contain shapes.
- BugFix: CString rangestr = (argc > 0) ? argv[0] : GetColumnRangeString();
    (#853)
- Improve plugin system (editor script) (PR #871)
- New unpacker plugins:
  - PrettifyHTML
  - PrettifyYAML
  - ValidateHTML
  - QueryYAML
  - SelectColumns
  - SelectLines
  - ReverseColumns
  - ReverseLines
  - Replace
- New editor script plugins:
  - PrettifyHTML
  - PrettifyYAML
  - SelectColumns
  - SelectLines
  - ReverseColumns
  - ReverseLines
  - Replace
- Updated Apache Tika to version 2.0.0
- Updated yq to version 4.11.1

### Command line

- Added /l command line option (osdn.net #41528)
- Added /t, /table-delimiter, /new, /fileext and /inifile command line option

### Installer

- Installer integrates with TortoiseGit and TortoiseSVN despite being told not
    to (#878)

### Translations

- Translation updates:
  - Bulgarian (PR #850)
  - Dutch (PR #842,#893)
  - Galician (PR #869)
  - German (PR #860,#870,#883,#890)
  - Hungarian (PR #845,#856)
  - Japanese
  - Lithuanian (PR #840,#849,#866,#875,#879)
  - Portuguese (PR #846,#872)
  - Slovenian (#858)
  - Russian (PR #847)
  - Turkish (PR #848)

### Internals

- BugFix: WinMerge doesn't build under Visual Studio 16.10.2 (#841)
- BugFix: x64: LINK : warning LNK4010: invalid subsystem version number 5.01;
    default subsystem version assumed (#855)
- BugFix: Project: heksedit cl : command line warning D9002: ignoring unknown
    option '/arch:SSE' (#861)
- BugFix:ALL.vs2019.sln cl : command line warning D9035: option 'Gm' has been
    deprecated and will be removed in a future release (#862)
- Resolves: Add continuous code security and quality analysis (PR #844, #843)
- Improvement: Add check and error mesage in DownloadDeps.cmd that path to
    7-zip exists (#864)

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
