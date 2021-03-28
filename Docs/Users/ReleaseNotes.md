# WinMerge 2.16.11 Beta Release Notes

- [About This Release](#about-this-release)
- [What Is New in 2.16.11 beta?](#what-is-new-in-21611-beta)
- [Known issues](#known-issues)

March 2021

## About This Release

This is a WinMerge beta release which is meant for preview the current state of
WinMerge development. This release is not recommended for the production.

Please submit bug reports to our <a href="http://github.com/winmerge/winmerge/issues">bug-tracker</a>.

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
