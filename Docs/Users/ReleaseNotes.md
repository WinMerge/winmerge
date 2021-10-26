# WinMerge 2.16.16 Release Notes

- [About This Release](#about-this-release)
- [What Is New in 2.16.16?](#what-is-new-in-21616)
- [What Is New in 2.16.15 beta?](#what-is-new-in-21615-beta)
- [Known issues](#known-issues)

October 2021

## About This Release

This is a WinMerge 2.16.16 stable release.
This release replaces earlier WinMerge stable releases as a recommended release.

Please submit bug reports to our <a href="http://github.com/winmerge/winmerge/issues">bug-tracker</a>.

## What Is New in 2.16.16

### General

- Fix a problem where the string in the Windows common dialog would not 
    change to the language when switching languages.

### File compare

- BugFix: Fix not getting the proper error message when saving failed 

### Table compare

- BugFix: Cannot resize last column with UI (#998)
- Reloading a file that was changed by another application does not preserve
    column widths (#951)

### Image compare

- BugFix: Fix an issue where drag-and-drop of file would only work once.

### Folder compare

- BugFix: Sync (Super Slow) (#771)
- BugFix: Fix an issue where filters are not applied correctly when opening
    a project file containing multiple items with different filters. (PR #995)
- [Feature Request] New Display Columns: Dimensions + Size Difference (#131)
- FolderCompare: Additional Properties (Windows Property System+Hash
    (MD5, SHA-1, SHA-256))  (PR #996)

### Options dialog

- BugFix: Fix the problem that the "Register Shell Extension for Windows 11 or
    later" button is not enabled when another user has registered
    ShellExtension for Windows 11.

### Plugins

- BugFix: Plugin unpacked file extension problem 
    (get_PluginUnpackedFileExtension) (#983)
- BugFix: Comparing broken lnk-files (windows shortcuts) freezes WinMerge
    (#1007)
- Apache Tika plugin: Update Apache tika to 2.1.0 and change the
    download URL
- CompareMSExcelFiles.sct: Make the number before the sheet name zero-padded

### Shell extension

- BugFix: ShellExtension for Windows 11 did not work on machines that did not
    have MSVCP140.dll VCRUNTIME140*.dll installed.
- BugFix: Loop counter should be the same type as the count type. (PR #987)
- ShellExtension for Windows11: Disable Registry Write Virtualization

### Manual

- Where to report documentation/help errors? (#1004)

### Translations

- Translation updates:
  - Galician (PR #1005)
  - German (PR #986)
  - Hungarian (PR #991,#1023)
  - Japanese
  - Lithuanian (PR #979)
  - Portuguese (PR #1017)
  - Slovenian (#1026)
  - Turkish (PR #980)

### Internals

- BugFix: Missing packages.config (also outdated) and wrong NuGet packages
    path in the WinMergeContextMenu.vcxproj and .filters file (#985)
- Fix typo in OpenView.cpp (PR #1000)

## What Is New in 2.16.15 Beta
### General

- BugFix: WinMerge would crash when launched if the registry or INI file
    contained invalid values.
- BugFix: Winmerge Crash when comparing 2 files from Windows Explorer context
    menu (#808, #908, #913)
- BugFix: Incorrect text color for selected menu item on Windows 11
- BugFix: 50% cpu use by winmergeu.exe after program closed (#903)
- Digitally sign packages (#152)

### File compare

- BugFix: The mouse cursor did not change to an hourglass when the files or 
    plugins were taking a long time to load.
- BugFix: Save Middle and Save Middle As menu items were not enabled when
    comparing three files.
- BugFix: A two-pane window was displayed even though 
    New (3panes) → Table menu item was selected.
- BugFix: The height of each pane in the Diff pane was calculated incorrectly
    when comparing three files.
- BugFix: Unicode SMP chars misrendered after performing a find (#914)
- BugFix: Crash when pressing Shift+F4 key
- BugFix: Replace slow (#940)
- BugFix: When moving in the scroll pane, the selected position is incorrect
    (#970)
- BugFix: When the Diff pane was redisplayed, the scroll position of the Diff
   pane was not appropriate. (osdn.net #42862)
- Make "Do not close this box" checkbox in search window On by default (#941)

### Image compare

- BugFix: Duplicate menu shortcut in translations (#905)
- BugFix: Image comparison (winimerge #24)

### Project file

- Add a feature to save/restore compare options to/from a project file.(#498)
    (PR #915)

### Options dialog

- Add a feature to set items saved to or restored from the project file.
    (PR #953)

### Plugins

- New unpacker plugins:
  - DecompileJVM
  - DecompileIL
  - DisassembleNative

### Command line

- Added /c `column number` command line option
- Added /EnableExitCode command line option

### Shell extension

- BugFix: WinMerge's extended menu items were doubly inserted into the context
    menu of Explorer's navigation pane. (osdn.net #42702)
- BugFix: Right click - compare - is unclear (#249)
- Added a new DLL (WinMergeContextMenu.dll) for the Windows 11 Explorer context
    menu (currently unstable and not registered by default) (PR #954)

### Translations

- Translation updates:
  - Brazilian (PR #910)
  - Dutch (PR #921,#969)
  - German (PR #965,#977)
  - Hungarian (PR #937,#955)
  - Italian (PR #911)
  - Japanese
  - Korean (PR #932)
  - Portuguese (PR #956,#964,#976)
  - Russian (PR #901,#927,#963)
  - Slovenian
  - Swedish (PR #974)
  - Turkish (PR #899)

### Internals

- README.md: Make it clear that requirements are to build, not use the
    application (PR #942)
- compiler-calculated maximum value for `m_SourceDefs` (PR #966)

## Known issues

 - Crashes when comparing large files (GitHub #325)
 - Very slow to compare significantly different directories (GitHub #322)
 - Vertical scrollbar breaks after pasting text (GitHub #296)
