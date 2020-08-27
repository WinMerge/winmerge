# WinMerge 2.16.8 Release Notes

- [About This Release](#about-this-release)
- [What Is New in 2.16.8?](#what-is-new-in-2168)
- [What Is New in 2.16.7 Beta?](#what-is-new-in-2167-beta)
- [Known issues](#known-issues)

August 2020

## About This Release

This is a WinMerge 2.16.8 stable release. This release replaces earlier WinMerge 
stable releases as a recommended release.

Please submit bug reports to our <a href="http://github.com/winmerge/winmerge/issues">bug-tracker</a>.

## What Is New in 2.16.8?

### General
  - BugFix: More space for some internationalized strings (GitHub #402)
  - BugFix: Some improvements (GitHub #405,411)

### File compare
  - BugFix: Ignore case option did not work when Diff algorithm was
      other than default
  - BugFix: A white vertical rectangle was sometimes drawn in the selected area
  - BugFix: The title of the file comparison window after changing the language
      was accidentally changed to "Untitled left/right"
  - Extended F4 key movement range to the whole file
  - Don't treat UTF-8 and UTF-8 with BOM the same when the 
      "Ignore codepage differences" option is disabled

### Folder compare
  - BugFix: Appropriate error messages were not displayed when the file to be
      deleted no longer existed
  - CTRL+Drag folders now has the opposite behavior of the current
      Include Subfolders option

### Folder compare
  - BugFix: Fix scrolling glitches (GitHub WinMerge/winimerge PR #8)
  - Reduce flicker on resize (GitHub WinMerge/winimerge PR #9)

### Options dialog
  - Allow choosing image filename patterns from a multi-selection dropdown list
      (GitHub PR #391)
  - WildcardDropList: Avoid the String instance as it could throw std::bad_alloc
      (GitHub PR #397)
  - Remove duplicate filename patterns without relying on WildcardDropList
      (GitHub PR #400)
  - Made Options dialog resizable
  - Changed the default values for the following options:
    - Ignore codepage differences -> disabled
    - Include unique subfolders contents -> enabled

### About dialog
  - Rework the fix for Github issue #316: GUI glitches/bugs #2 (GitHub PR #392)
  - Replace outdated list of developers in AboutBox with ascii-art gnu from FSF
      (GitHub PR #394)

### Installer
  - BugFix: Installation - Internal error: Failed to expand shell folder
      constant "userdesktop" (GitHub #354)
  - BugFix: Lithuanian.po is missing (GitHub PR #415)
  - New installer for per-user installation
      (WinMerge-2.16.8-x64-PerUser-Setup.exe)

### Translations
  - Translation updates:
    - Brazilian (GitHub PR #383)
    - Galician (GitHub PR #393)
    - German (GitHub PR #388,389,398,399,401,406,412,414, WinMerge/frhed PR #2)
    - Lithuanian (GitHub PR #385,390,407,408,413,415)
    - Turkish (GitHub PR #386)
    - Russian (GitHub PR #387)

### Manual
  - Update manual for IgnoreCommentsC change (GitHub PR #384)
  - Update Shortcut_keys.xml (GitHub PR #410)

## What Is New in 2.16.7 Beta?

### General
  - BugFix: The icons on a 43" 4K screen are too small (GitHub #276)
  - BugFix: GUI glitches/bugs (GitHub #316)
  - BugFix: Several issues regarding i18n (GitHub #314)
  - Add Solarized Dark/Light color scheme (GitHub #287)
  - Compile WinMerge with ASLR and CFG enabled (GitHub #315)

### File compare
  - BugFix: Diff Pane issues (GitHub #307)
  - BugFix: Codepage not updated on refresh (GitHub #320)
  - BugFix: Crashes when comparing large files (GitHub #325)
  - BugFix: WinMerge does not recognize the same files (GitHub #332)
  - BugFix: Moved ghost lines are shown in the wrong color (GitHub #358)
  - BugFix: opening same file (GitHub #362)
  - BugFix: Find dialog sometimes remembers old text to start a new find
      instead of the new text. (GitHub #368)
  - BugFix: Fix a problem where markers that are out of the range of the 
      current difference appear in the difference pane. (osdn.net 40407)
  - BugFix: HTML5 encoding (<meta charset="..." >) is ignored. (osdn.net #40476)
  - '=' should be included in word separator (osdn.net #40224)
  - Allow specifying default for EOL warning checkbox (GitHub #297)
  - Only indent existing lines (GitHub #356)

### Table compare
  - Made it possible to display the contents of CSV and TSV files like
      spreadsheet software.

### Folder compare
  - BugFix: DST causes incorrect dates shown in Date column (GitHub #299)
  - BugFix: Long filename issue (GitHub #339)
  - BugFix: Winmerge Freeze if large number of files are listed in Window View
      (GitHub #348)
  - BugFix: Change to disable some menu items that should not be executed for
      directories, when directories are selected in the folder compare window. 
      (GitHub PR #366)
  - BugFix: Update DirViewColItems.cpp (GitHub #376)
  - BugFix: Fix the problem that not 3 panes but 2 panes file compare window
      displays when executing "Compare As" > "Binary" or "Compare As" > "Image"
      in the context menu in the folder compare window with selecting 3 items.
      (GitHub PR #381)
  - BugFix: Fix a problem where a text file could be considered a binary file.
      (osdn.net #40296)
  - BugFix: crash when clicking [Merge]-> [Delete] menu item while renaming a
      file
  - Add a feature generating a patch from directories (#283) (GitHub PR #331)
  - added bin and obj directories to C# filter (GitHub PR #365)

### Image compare
  - BugFix: Scrolling behavior when dragging images is wrong (osdn.net #40205)

### Open dialog
  - BugFix: Open dialogs are sometimes left as garbage (osdn.net #40487)
  - Browse button in the file/dir selection show wrong path (GitHub #346)

### Options dialog
  - BugFix: Pressing the [Compare/Binary] category button in the Options dialog
      twice will cause a crash. (osdn.net #40308)

### Plugins
  - BugFix: Fix handling of line breaks in SortAscending, SortDescending
      (osdn.net PR #40266)
  - BugFix: Error when comparing images in the CompareMSExcelFiles.sct plugin
      (osdn.net #40472)
  - CompareMSExcelFiles.sct: Include Excel Addin files (*.xlam) (GitHub PR #269)
  - Add support for C# and TypeScript (GitHub PR #382)
  - Add a plugin for Visio (osdn.net PR #40473)
  - Plugin fixes for the new Table Compare
      (CompareMSExcelFiles.sct, IgnoreFieldsComma.dll, IgnoreFieldsTab.dll)

### Archive support
  - BugFix: Crash when comparing encrypted 7z files (GitHub #367)

### Installer
  - Create the installer with Inno Setup 5.x since installers created with Inno
      Setup 6.0.x are identified as malware by multiple virus scanning engines

### Translations
  - Translation updates:
    - Brazilian (GitHub #360)
    - Chinese Simplified (GitHub PR #303,377)
    - Galician (GitHub PR #292,293,308,313,326)
    - German (GitHub PR #312,357)
    - Lithuanian (GitHub PR #291,298,333)
    - Polish (GitHub PR #280)
    - Portuguese (GitHub PR #305)
    - Russian (GitHub PR #270,271,272,302,327,328)
    - Slovak (GitHub PR #300)
    - Slovenian (GitHub #285)
    - Spanish (GitHub PR #292,293,304,308)
    - Turkish (GitHub PR #335,336,337,338)

### Manual
  - Minor changes to translations README.md file (GitHub #289)
  - Update winmerge.org URL to HTTPS, many small improvements (GitHub PR #306)

### Internals
  - BugFix: Src\Common\MyCom.h unused? (GitHub #284)
  - BugFix: Error on git repository cloning (GitHub #288)

## Known issues

 - Bug: Line Filters aren't applied when using a 3rd file (GitHub #395)
 - Pausing comparing doesn't pause immediately (GitHub #342)
 - Crashes when comparing large files (GitHub #325)
 - Very slow to compare significantly different directories (GitHub #322)
 - Vertical scrollbar breaks after pasting text (GitHub #296)
 - When the "Filter comments" option is enabled, the difference between the
     parts that do not comment blocks may be ignored. (osdn.net #40488)