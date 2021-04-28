# WinMerge 2.16.12 Release Notes

- [About This Release](#about-this-release)
- [What Is New in 2.16.12?](#what-is-new-in-21612)
- [What Is New in 2.16.11 beta?](#what-is-new-in-21611-beta)
- [Known issues](#known-issues)

April 2021

## About This Release

This is a WinMerge 2.16.12 stable release.
This release replaces earlier WinMerge stable releases as a recommended release.

Please submit bug reports to our <a href="http://github.com/winmerge/winmerge/issues">bug-tracker</a>.

## What Is New in 2.16.12

### General

- GUI textstrings: grammatical corrections (PR #722)
- Added ARM64 support

### File compare

- BugFix: Fix PHP syntax highlighting. (PR #751)
- BugFix: Strings in a multi-line diff block were not be replaced correctly
    when the substitution filters' regular expression contained ^
- BugFix: Font size restoration with Ctrl+0 key did not work properly in
    Hi-DPI environment
- Make the width of the 'Find what' label in Find dialog a little wider
    (osdn.net #42063)
- Fix conflicting accelerators (osdn.net #42064) 
- [Feature Request] Selection Count on Status Bar (#135)
- Add C# 6.0 reserved words to style list (PR #719)
- FR: Add a shortcut key go to next file (#721)
- Update PHP syntax highlighting keyword list. (PR #724)
- autoit.cpp - Macros >> User 1 ..... Variable >> User 2 (PR #749)
- autoit.cpp - #CS #CE support (PR #753)
- Interrupt a inline diff process when it takes too long
- Make the color of characters that represent spaces and tabs lighter than
    other characters when the "View Whitespace" menu item is enabled
- Added "Open Parent Folder" menu item to the context menu

### Folder compare

- BugFix: Copy confirmation dialog has overlapped Yes/No Button (#739)
- BugFix: Comparison result was not updated by Swap operation in 3-way folder
    comparison (osdn.net #41901)

### Binary compare

- BugFix: Next File or Previous File button on the toolbar did not work if the
    active window was a binary compare window or an image compare window.
- BugFix: Only one pane was zoomed in/out in Ctrl+Mouse Wheel

### Image compare

- BugFix: Next File or Previous File button on the toolbar did not work if the
    active window was a binary compare window or an image compare window.
- BugFix: The header bar widths were not calculated properly when the
    'Split Vertically' menu item is unchecked.

### Open dialog

- Make archive files openable as binary files from the pull-down menu.

### Archive support

- BugFix: Fixed a rare crash when decompressing an archive file

### Plugins

- BugFix: ATL: QIThunk - 2 LEAK in plugins e.g. DisplayBinaryFiles and
    DisplayXMLFiles (#755)

### Translations

- Translation updates:
  - Brazilian (PR #711,#728,#729)
  - Dutch (PR #706)
  - Galician (PR #737)
  - German (PR #718,#752)
  - Hungarian (PR #712)
  - Japanese
  - Korean (PR #709)
  - Lithuanian (PR #708,#713,#738,#754)
  - Portuguese (PR #725)
  - Slovenian
  - Swedish (PR #720,#723)
  - Russian (PR #707)

## What Is New in 2.16.11 Beta

### General

- Make all OK strings same case (PR #593)
- Tab bar: Added shadows to help distinguish between active and inactive tabs
- Added drop-down menu to Open and Save icon on toolbar 

### File compare

- BugFix: Basic syntax highlighter is broken (osdn.net #41440)
- BugFix: File is corrupted while saving differences in changed file (Ctrl+S)
    (#607)
- BugFix: Fix an issue where the syntax highlighting scheme is not applied to
    the split second File pane and the Diff pane when changing it.
    (PR #624)
- BugFix: The position selected by clicking while holding down the shift key is
    incorrect. (sf.net #2237)
- BugFix: Fix an issue where the comment filter might not work properly if the
    file is non-UTF-8 and the comment contains non-ascii characters.
- Add a feature to set up default highlighting by a file extension
    (PR #627)
- Add "Copy Selected Line(s) to/from Left/Right" feature (#463)
    (PR #642)
- If -b or -w is also specified, -B now considers lines to be empty if they
    contain only white space (osdn.net #41355)
- Added BOM checkbox to the Codepage dialog.

### Folder compare

- BugFix: Program crash if you close a tab with the folder from where you
    opened current file (#645)
- BugFix: The title bar path was not updated when swapping files in a Zip file.
- BugFix: The codepage specified in the Codepage dialog did not affect the
    files to be opened.

### Image compare

- BugFix: The image on the right does not open if the file exists only on the
    right (osdn.net #41721)

### Open dialog

- Added Text, Table, Binary and Image menu items to the Compare button in the
    "Select Files or Folders" window

### Archive support

- Rar5 support (#644)

### Options dialog

- BugFix: Fix an issue where custom colors are not saved.
    (PR #648)

### Plugins

- RCLocalizationHelper: Fix memory leaks (PR #596)

### Installer

- Installer issue with Polish diacritics characters (#589)

### Translations

- Translation updates:
  - Bulgarian (PR #599,#625)
  - Brazilian (PR #633)
  - French (PR #637,#649,#658,#659)
  - Galician (PR #587,#650,#677)
  - German (PR #632,#651)
  - Hungarian (PR #682,#683,#685)
  - Japanese
  - Lithuanian (PR #634,#653)
  - Polish (PR #597,#673)
  - Portuguese (PR #674)
  - Slovenian
  - Russian (PR #588)

### Command line

- Single instance mode does not work when launched from Visual Studio 2019
    (#622)
    (Added /sw command line option)

### Internals

- BugFix: Plugins\src_VCPP\VCPPPlugins.vs2017.sln can't open projects any
    more because in revision 69455da the projects were renamed. (#598)
- BugFix: OutputFile of plugin project DisplayXMLFiles is different that the
    other projects (#600)
- BugFix: WinMergeScript.rgs files in Plugins\src_VCPP projects are not found.
    (#605)
- BugFix: typeinfoex.h(189): warning C4701: potentially uninitialized local
    variable 'pTypeLib' used (#605)
- BugFix: Plugins project DisplayXMLFiles.vcxproj contains
    "Unicode Release MinSize" configuration but was removed (#611)
- Expression is always true (#612,PR #613)
- Plugins.cpp function SearchScriptForMethodName can be improved (#690)

## Known issues

 - Crashes when comparing large files (GitHub #325)
 - Very slow to compare significantly different directories (GitHub #322)
 - Vertical scrollbar breaks after pasting text (GitHub #296)
