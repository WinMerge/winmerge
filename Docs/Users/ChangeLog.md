# Change log

## WinMerge 2.16.9 - 2020-11-29

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

## WinMerge 2.16.8 - 2020-08-28

### General
  - BugFix: More space for some internationalized strings (GitHub #402)
  - BugFix: Some improvements (GitHub #405,411)

### File compare
  - BugFix: Ignore case option did not work when Diff algorithm was
      other than default
  - BugFix: A white vertical rectangle was sometimes drawn in the selected area
  - BugFix: The title of the file comparison window after changing the language
      was accidentally changed to "Untitled left/right"
  - BugFix: Wrong merging (GitHub #420)
      (minimal/patience/histogram diff algorithm did not work on Windows XP)
  - Extended F4 key movement range to the whole file
  - Don't treat UTF-8 and UTF-8 with BOM the same when the 
      "Ignore codepage differences" option is disabled

### Folder compare
  - BugFix: Appropriate error messages were not displayed when the file to be
      deleted no longer existed
  - BugFix: 'Show Middle/Right Unique items' menu item does not work properly
      in 3-way folder compare (osdn.net #40672)
  - CTRL+Drag folders now has the opposite behavior of the current
      Include Subfolders option

### Image compare
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
    - Ignore codepage differences -&gt; disabled
    - Include unique subfolders contents -&gt; enabled

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

## WinMerge 2.16.7 - 2020-07-26
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
  - BugFix: HTML5 encoding (&lt;meta charset="..." &gt;) is ignored. (osdn.net #40476)
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
      displays when executing "Compare As" &gt; "Binary" or "Compare As" &gt; "Image"
      in the context menu in the folder compare window with selecting 3 items.
      (GitHub PR #381)
  - BugFix: Fix a problem where a text file could be considered a binary file.
      (osdn.net #40296)
  - BugFix: crash when clicking [Merge]-&gt; [Delete] menu item while renaming a
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

## WinMerge 2.16.6 - 2020-02-23

### General
  - Added CWindowsManagerDialog class for handling open tabs with Ctrl+Tab,
      now the application is behave just like professional editors
      (Visual Studio, Notepad++, etc.) to switch and activate the open tabs.
      (GitHub #247)

### File compare
  - BugFix: GhostTextBuffer: Don't unexpectedly bring back empty lines user
      wants to delete (GitHub #244)
  - BugFix: Prevent silent abort with File Comparison of files whose size
      is an exact multiple of 2^32 bytes (GitHub #257)
  - BugFix: WinMerge 2.16.4.0 crashes after de-selecting Split Vertically
      (GitHub #259)
  - BugFix: "Replace All" may not replace all text
  - BugFix: An error message is shown when pressing Enter key in Diff pane
      (osdn.net #39924)
  - BugFix: Non-moved lines are treated as moved lines (osdn.net #39851)
  - Add support for touchpad horizontal scroll (GitHub #254)

### Image compare
  - BugFix: Support 16-bit images (GitHub WinMerge/winimerge issue #6)
  - Add support for SVG, PDF, WMF and EMF file types
    (Only available in WinMerge 64bit version on Windows 10)  
      Demo1: https://gyazo.com/b605edb820bc52d0f4f6232eb8ad78aa  
      Demo2: https://gyazo.com/f5f267546db27f2dc801c00df8cb4251

### Archive support
  - BugFix: Third tgz doesn't get fully extracted (first two do) (GitHub #263)

### Patch generator dialog
  - BugFix: Crash when generating patch for multiple files and a file only on
      one side (GitHub #252)

### Installer
  - BugFix: Fix spelling of Git (GitHub #246)

### Translations
  - New translation: Arabic (sf.net #3038)
  - Translation updates:
    - Bulgarian (GitHub #236, #268)
    - French (GitHub #251)
    - German (GitHub #239, #245, #258)
    - Lithuanian (GitHub #233, #267)
    - Portuguese (GitHub #237)
    - Spanish (GitHub #266)

### Manual
  - BugFix: Fix spelling of Git (GitHub #246)
  - Update Configuration.xml (GitHub #262)

### Internals
  - Consolidate FolderCmp (GitHub #240, #242)
  - Avoid some back and forth file path transcoding between UTF16 and UTF8
      (GitHub #243)

## WinMerge 2.16.5 - 2019-12-09

### File compare
  - BugFix: Suspicious lack of Release() calls in lwdisp.c (GitHub #171)
  - BugFix: Performance using Unpacker (GitHub #180)
  - BugFix: WinMerge toolbar appears too small on UHD and should default
      to the conforming Windows 10 scaling/DPI accessibility setting
      (GitHub #182)
  - BugFix: "Recompare as" menu option doesn't follow current choice
      (GitHub #191)
  - BugFix: Switching between "Find Next" and "Find Prev" need to click twice
      (GitHub #202)
  - BugFix: Program is crashed! (GitHub #229)
  - Switch "Find" buttons in Find dialog (GitHub #201)
  - Improve UNICODE character support using icu.dll usable
      from Windows 10 Creators Update 
  - Add support for color emoji (Only available in 64bit version)
      (To enable color emoji support, select DirectWrite* in Rendering
       mode combobox on Editor page in the Options dialog)  
      Demo: https://gyazo.com/7cbbbd2c1de195fcd214d588b21b21d4

### Folder compare
  - BugFix: Crash when clicking Next Difference button after unchecking
      Show Different Items menu item
  - Changed symbols in Newer/Older column for better visual clarity
      (GitHub #169)
  - Add "Ignore codepage differences" to quick options (GitHub #197)
  - Limit the window flashing count. (GitHub #206)

### Image compare
  - BugFix: Image compare breaks when zoomed too much with large pictures
      (GitHub #188)
  - BugFix: [Image compare] Zoom-in focuses on top-left corner instead of
      cursor position (GitHub #211)

### Shell extension
  - BugFix: "Run as Administrator" incompatible with "Shell integration"
      (ver 2.16.4) (sf.net #2234)

### Installer
  - BugFix: Installer russian translation (#168)
  - BugFix: Windows 10 shell integration not working (GitHub #176)
  - Installer - Proposal - Separate Inno Setup strings from WinMerge
      installer strings (GitHub #167)

### Translations
  - Update Italian translation (GitHub #164, #165)
  - Update Russian translation (GitHub #166)
  - Update Bulgarian translation (GitHub #170, #175, #179, #212)
  - Update Portuguese translation (GitHub #185, #199)
  - Update French translation (GitHub #187)
  - Update Lithuanian translation (GitHub #189, #208, #217, #225)
  - Update Swedish translation (GitHub #194, #196)
  - Update German translation (GitHub #204, #205, #215)
  - Update Polish translation (GitHub #218)
  - Update Simplified Chinese translation (GitHub #228)
  - Update Finnish translation (sf.net #3037)
  - Update English.pot (#216)
  - Add Japanese manual (GitHub #183)

### Manual
  - Small Manual tweaks (GitHub #190)

## WinMerge 2.16.4 - 2019-07-28

### File compare
  - BugFix: coretools.cpp: linelen() should not truncate lines with embedded
      NULs (GitHub #156)
  - BugFix: file compare : right-click doesn't select the diff under
      the mouse (GitHub #159)
  - BugFix: Avoid an exception in GuessCodepageEncoding() when filepath equals
      "NUL" (GitHub #162)
  - BugFix: Auto-indent did not work if the EOL-style was not CRLF

### Folder compare
  - BugFix: Generating HTML Folder Compare report including File Compare
      report did not complete (Bitbucket #15)
  - BugFix: Compare Statistics dialog: The number of diff folders was
      counted in the number of diff files

### Plugins
  - BugFix: PrediffLineFilter.sct: Wrong encoding for settings
      dialog (Bitbucket #16)

### Translations
  - Update Russian translation (Bitbucket PR #51)
  - Update Italian translation (Bitbucket PR #52)

### Internals
  - Favor PathContext::GetSize() over PathContext::size() (GitHub #157)
  - Consolidate FolderCmp (GitHub #158, #160, #161)
  - Avoid some InnoSetup compiler warnings (Bitbucket PR #53)

## WinMerge 2.16.3 - 2019-06-29

### General
  - BugFix: Slow startup with documents folder redirected to high-latency
      network drive (Bitbucket #155)
  - Add VisualElementsManifest for Windows 10 start menu (Bitbucket PR #47)
  - Reduce the size of the executable file

### File compare
  - BugFix: Location and Diff pane visibility broken in 2.16.1 
      (GitHub #138, Bitbucket #163, sf.net #2228)
  - BugFix: Temporary files could not be created (GitHub #143, sf.net #2220)
  - BugFix: Trivial bugfix (GitHub #155)
  - BugFix: Regression: Incorrectly shows 'no changes' for large portions of
      certain binary files! (Bitbucket #162)
  - Small tweaks for HTML file reports (Bitbucket PR #44, #45)
  - Made diff algorithm selectable by using libXDiff
      algorithms: default(Myers), minimal, patience, histogram
  - triple click to select whole line (Bitbucket #144)
  - Add support for merging word level diffs in selection  
      Demo: https://i.gyazo.com/af18960bd1f121213a2cd9287cae9cf4.gif

### Folder compare
  - BugFix: WinMerge 3-Way Compare Bugs: Always Shows Unique Items.
      (GitHub #154)
  - BugFix: WinMerge could not compare files that are opened in other
      applications
  - Show progress bar while generating Folder Compare Report
  - Use own colors settings for folder compare (Bitbucket PR #49)

### Project file
  - BugFix: Possible Bugs in 2.16.2 (sf.net #2221)
  - Supporting relative path in project file (Bitbucket #158)
  - Allow multiple &lt;paths&gt; in project file

### Image compare
  - BugFix: no message was displayed when file saving failed
  - Implement Insertion/Deletion Detection  
      Demo: https://gyazo.com/17d8773354d23b5ae51262f28b0f1f80

### Options dialog
  - Tweak size of combobox "codepage" in options (GitHub #144)
  - Sort combobox codepage and add manual codepage (GitHub #145)

### Installer
  - Allow per-user installation (only x64 installer)
  - Don't install Files.txt and don't add "Uninstall WinMerge" to the
      start menu (Bitbuket #38)

### Translations
  - Update French translation (GitHub #149, #150)
  - Update Lithuanian translation (Bitbucket PR #36, #40, #43, #46, #48, #50)
  - Update Russian translation (Bitbucket PR #41, #42)

## WinMerge 2.16.2 - 2019-04-04

### File compare
  - BugFix: Edit &gt; Replace &gt; All, Undo: should undo all (sf.net #2113)
  - BugFix: Save As: default directory should be file's original
      directory (sf.net #2163)
  - BugFix: Strings with embedded 0 bytes are handled incorrectly (sf.net
      #2187)
  - BugFix: Removed word level merge support due to unstable behavior
  - BugFix: Menu item "File-&gt;File Encoding" moves main window (Bitbucket #150)
  - BugFix: Crash when maximizing MDI child window
  - BugFix: Crash when failed to load file
  - Add syntax highlight for Lua (Bitbucket #114)

### Folder compare
  - BugFix: Compare results refresh incorrectly after deletions (sf.net #2217)

### Archive Support
  - Upgraded to 7-zip 19.00

### Tab bar
  - Improve icon display at 120DPI

### Image compare
  - BugFix: Zoom In/Out shortcuts don't update zoom level in location
      pane (Bitbucket #149)
  - Zoom In/Out menu items for image compare (Bitbucket #148)

### Open dialog
  - Allow quoted filenames in "Select Files or Folders" (sf.net #1240,
    GitHub #137)

### Manual
  - Some tweaks for the manual (Bitbucket PR #35)

### Installer
  - BugFix: Gibberish language during installation (Bitbucket #147)

### Translations
  - Update Catalan translation (Bitbucket PR #29)
  - Update Italian translation (Bitbucket PR #32)
  - Update Lithuanian translation (Bitbucket PR #33)
  - Update Simplified Chinese translation (Bitbucket PR #34)

## WinMerge 2.16.1 - 2019-02-24

### General
  - Updated the copyright year to 2019, actually (GitHub #110)
  - Prevent splitter view from claiming input focus (GitHub #127)
  - Contributors.txt: Changed EOL from Unix (LF) to Windows (CR LF),
      because not shows correctly in Windows Notepad. (Bitbucket PR #17)

### Open dialog
  - Alternatively to Swap buttons, allow drag & drop between filetype
      icons (GitHub #118)
  - Expose cleaner moving/resizing behavior esp. in non-maximized
      state (GitHub #119)
  - Avoid retaining bogus filetype icons from previous selections (GitHub #122)

### File compare
  - BugFix: Extra blank displayed after left/right single/double
      quotes (Bitbucket #134)
  - BugFix: Click doesn't trigger document to scroll in Location pane under
      circumstance (Bitbucket #140)
  - BugFix: Frustrating text selection bug (GitHub #109)
  - BugFix: Left and Right files swap after losing focus
      v2.16.0.0 (sf.net #2213)
  - BugFix: WinMerge 2.16.0.0 Unicoe X64 version displaying
      Microsoft Visual C++ Runtime Library Asset in DiffWraper.CPP
      line 984 (sf.net #2214)
  - BugFix: Cancelation of selection range with reverse indent (sf.net #2215)
  - BugFix: upward and downward search not work correctly
  - Bugifx: Fix menu accelerator conflict
  - Add File/Recompare As/Image menu item
  - Add support for merging word level diffs in selection  
    Demo: https://i.gyazo.com/af18960bd1f121213a2cd9287cae9cf4.gif

### Folder compare
  - BugFix: Unique files are not shown (Bitbucket #138)
  - BugFix: Improve Hard-IO error handling, other bug fixes, cleanup,
      tweaks (GitHub #120)

### Archive Support
  - Upgraded to 7-zip 18.06 (Bitbucket #119)

### Options dialog
  - BugFix: Widen the width of labels in Options dialog (GitHub#108)
  - BugFix: view settings make winmerge crash (Bitbucket #109)
  - BugFix: 64bit version can't browse between option pages (Bitbucket #128)
  - BugFix: Shell Integration "Enable Advanced Menu" disabled on new
    install (Bitbucket #137)
  - BugFix: Binary File options don't change from default (Bitbucket #135)

### Shell extension
  - BugFix: CreateTranslatedRcFiles needs an update (GitHub #113)

### Translations
  - Update Brazilian Portuguese translation (GitHub #108)
  - Update Swedish translation (sf.net #3035, GitHub #112, #114)
  - Update Lithuanian translation (Bitbucket PR #7, #8, #27)
  - Update Russian translation (Bitbucket PR #9-#26, #139)
  - Update Bulgarian translation (Bitbucket PR #28)
  - Update Turkish translation (sf.net #3036)
  - Update Japanese translation
  - Remove now unneeded MergeLang.dll

### Manual
  - BugFix: Some fixes in the manual (GitHub #116)
  - Bugifx: end of line documentation (sf.net #2211)
  - Clarification as per
      https://github.com/WinMerge/winmerge-v2/issues/41 (GitHub #126)

### Internals
  - Remove `nFinalNullLines` checking, disabled by an earlier
      commit (GitHub #111)
  - Various minor updates, plus preparation for VS2019 (GitHub #115)
  - DiffTextBuffer: Create Unicode temp files (makes EscapeControlChars()
      obsolete) (GitHub #123)
  - Don't pass `DIFFITEM *` by casting to `uintptr_t` (GitHub #124, #125)

## WinMerge 2.16.0 - 2018-11-23

### General
  - BugFix: Disabled icon on toolbar not gray out when running on XP

### File compare
  - BugFix: Copy to X and Advance" not work in 3way-compare (Bitbucket #123)
  - BugFix: File compare always showing different ending (GitHub #106)
  - BugFix: Scrolling issue (GitHub #105)
  - BugFix: A crash (GitHub #104)
  - Add separators to "Zoom" and "Diff Context" menus (Bitbucket PR #5)

### Open dialog
  - BugFix: Drag&Drop in Open Dialog won't work correct (Bitbucket #125)

### Installer
  - Drop ClearCase integration (Bitbucket PR #6)

### Translations
  - Add Lithuanian translation (Bitbucket #124)
  - Update Portuguese translation (GitHub #102,103)

## WinMerge 2.15.5 - 2018-10-28

### General
  - Add support for very long path and file names (GitHub #87,95,99)
  - Enable Ctrl+C shortcut key on the file path bar
  - Remove support for old version control system (GitHub #96)
  - Improve Options &gt; Color &gt; Differences dialog layout (GitHub #89)
  - Improve information and formatting in Help &gt; Configuration reporting (GitHub #92)
  - Improve visual layout of Open dialog (GitHub #97)
  - Add icon for "New (3 panes)" menu item (GitHub #97)

### File compare
  - Add Window/Split menu item
  - Improve handling of last lines in files (GitHub #89,90,93)
  - Improve "Visual C++ Loose" filter (GitHub #95)
  - BugFix: Crash when saving a file if the active pane is diff detail pane
  - BugFix: Crash when pressing Alt+Down key while loading large files
  - BugFix: Crash when "mousing over" Plugin Setting context menu if Plugins are disabled
  - BugFix: Various problems reading of files &gt; 2GB (GitHub #81)
  - BugFix: Selecting files "From MRU list" again works correctly

### Translations
  - Update Italian translation (GitHub #86)
  - Update Japanese translation (Bitbucket #3)
  - Update Dutch translation
  - Update German translation
  - BugFix: Spanish translation error (Bitbucket Issue #108)

### Internals
  - Various diffengine and compilation/build fixes
      (GitHub #89,90,91,92,94)

## WinMerge 2.15.4 - 2018-04-28

### Appearance
  - BugFix: Dot icon in a drop-down menu is not drawn correctly

### File compare
  - BugFix: Crash when reading a file in Google Drive File Stream (sf.net#2206)
  - BugFix: Crash when removing ghost lines (GitHub #80)
  - BugFix: Lines without EOL appears even though it is not the last line
  - Add Ctrl++/Ctrl+-(not numpad key) shortcut key to zoom in/out (sf.net#1727)
  - Remove 2GB file size limit for 64-bit build (GitHub #81, #82)

### Folder compare
  - Add Pause button to Folder Compare Progress dialog (sf.net#828,#1222,#1237)
  - Re-detect the file encoding when opening files (sf.net#2131)

### Open dialog
  - Add Options button to Open dialog
  - Add Save Project button to Open dialog

### Patch generator dialog
  - BugFix: File1/File2 fields don't remember manual input (sf.net#1949)

### Reports
  - BugFix: Garbled text was copied to the clipboard when generating a report
      on the clipboard (sf.net#2200)

### Plugins
  - Add PrediffLineFilter.sct plugin
  - Make ignored lines by Prediffer plugin colored

### Translations
  - Update Slovak translation (sf.net#2902)
  - Update Portuguese translation (GitHub #84,#85)

### Internals
  - Adapt to VS2017 version 15.6.1 (GitHub #79)

## WinMerge 2.15.3 - 2018-03-04

### File compare
  - Fix assertion error when started on Windows XP
  - Fix a problem that moved blocks are sometimes shown as normal diff blocks
  - Fix a problem that EOL is removed unexpectedly when removing lines
      included ghost lines
  - Fix reverse search to search last line correctly
  - Fix infinite loop when replacing ^ (sf.net#2094)
  - Fix to show text that contains & on message box, correctly (sf.net#2067)

### Folder compare
  - Fix a problem that unique files are invisible when comparing files using
      'Date' compare method
  - Remove trailing garbage from a dragged text (Github#75)

### Patch generator dialog
  - Fix a problem that typed-in Context in Generate Patch is not applied
      (sf.net#2179)

### Reports
  - Entitify file paths in HTML & XML directory comparison reports (sf.net#2199)

### Location pane
  - Add support for HiDPI screens (Github #54)

### Plugins
  - CompareMSExcelFiles.sct: Add "Compare headers and footers" checkbox to
      settings dialog (sf.net#2102)

### Installer
  - Remove extra space in installer: " Launch WinMerge" (sf.net#2144)
  - Add "How to Apply These Terms to Your New Programs" section into GPL.rtf
      (sf.net#2098)

### Translations
  - Add Finnish translation (sf.net#3031)
  - Add Sinhala translation (sf.net#3032)
  - Update Korean translation (Github #61)
  - Update Portuguese translation (GitHub #56-78)
  - Update Polish translation (sf.net#2177)
  - Update Turkish translation (Bitbucket#85, #86)
  - Change the file encodings of all .po files of WinMerge GUI to UTF-8 

## WinMerge 2.15.2 - 2018-01-28

### Appearance
  - Flatten GUI pane edges
  - Partial support for HiDPI (not Per-Monitor DPI Aware)
  - Remove splash screen. The splash screen image is now displayed in About
      dialog

### Tool bar
  - Add "View-&gt;Toolbar-&gt;Huge" menu item
  - Add drop down menu to Options icon
  - Add icons for 3-way merge functions

### Open dialog
  - Change window type of Open dialog from modal window to MDI child window
  - Add Read-only checkbox to Open dialog
  - Add buttons to swap items
  - Add the third path combobox for 3-way comparison
  - Allow file name patterns like not only *.ext but file*.ext

### Tab bar
  - Make Tabbar reorderable by using drag-and-drop
  - Add "Close Other Tabs", "Close Right Tabs" and "Close Left Tabs" menu items
      to system menu
  - Add "Enable Auto Max Width" menu item to Tab Bar context menu
  - Show close button when hovering over tabs
  - Add icons to each tab

### Options dialog
  - Add "Close Select Files or Folders Dialog on clicking OK button" checkbox
      to "General" page
  - Add "Language" combobox to "General" page and remove Language dialog
  - Add "Ignore codepage differences" checkbox to "Compare/General" page
  - Remove checkboxes in "Enable multiple compare windows for" group from
      "General" page (Now always enabled multi compare windows)
  - Add "Include Subfolders" checkbox to "Compare/Folder" page
  - Add "Automatically expand all subfolders" checkbox to "Compare/Folder" page
  - Add "Ignore Reparse Points" checkbox to "Compare/Folder" page
  - Add "Number of compare threads" textbox to "Compare/Folder" page
  - Add "Compare/Binary" page
  - Add "Compare/Image" page
  - Add checkboxes whether to use custom colors for text to 
      "Colors/Differences" page
  - Add "GitHub/Bitbucket" button to "Colors/Differences" page
  - Add "Margin" color button to "Colors/Text" page
  - Add "Colors/Markers" page
  - Remove "Use stand-alone 7-Zip if available" and 
      "Use local 7-Zip from WinMerge folder" radio button from 
      "Archive Support" page
      (Now always use local 7-Zip from WinMerge folder)
  - Make $file and $linenum parameters available in External editor textbox
      on "System" page
  - Make Custom codepage on "Codepage" page selectable from dropdown list 
  - Add "Detect codepage for text files using mlang.dll" checkbox to 
      "Codepage" page
  - Remove "Include subfolders by default" checkbox from 
      "Shell Integration" page
      (ShellExtension now refers to "Include subfolders" checkbox 
        on "Compare/Folder" page or on "Select Files or Folders" dialog)
  - Remove "Add Shell menu to context menu" from "Shell Integration" page
      (WinMerge now always adds shell menu to context menu in Folder window)
  - Add "Register shell extension" button to "Shell Integration" page

### Folder Compare
  - Add Multi-thread compare support for Full Contents or Quick Contents
      method
  - Color the background of items in list view
  - Restore collapse/expand state of folder tree after re-comparison 
  - Make comparing window closeable
  - Make sub-folder re-comparable
  - Display folder items in list view during comparison
  - Display the file name currently being compared in progress window
  - Add "Parent Folder" menu item in 'Open Left/Middle/Right' menu
  - Add "Compare Non-horizontally" menu item to context menu
  - Add "Copy items To Clipboard" menu item to context menu
  - Add "Swap panes" menu item to View menu
  - Add "Auto/Manual Prediffer" menu item to Plugin menu
  - Open selected folders in new tab when clicking "Compare" menu item with
      pressing Ctrl key
  - Cleanup Date column alignment (GitHub #25)
  - BugFix: Version column not sorted correctly
  - BugFix: A unique item that is a binary file was not detected
      as a binary file
  - BugFix: Properly handle 3-way comparison of Binary files (Github #23)
  - BugFix: Use proper font for View&gt;Use Default Font menu item (GitHub #24)

### File Compare
  - Add support for 3-way compare/merge
  - Add "Diff Context" menu to View menu to adjust the number of displayed
      diff context lines
  - Add "Split Vertically" menu item to View menu
  - Add "Add Synchronization Point" to Merge menu
  - Add "Auto/Manual Prediffer" menu item to Plugin menu
  - Show the name of the applied plugin in the status bar
  - Add shortcut(Ctrl+Alt+E) to open with external editor
  - Add Go and Rust highlighter
  - Add VHDL syntax highlighter (Bitbucket #2)
  - Add C++11 keywords
  - Make merge editor window D&Dable
  - Highlight the search text and implement Marker
  - Highlight the EOL pane in status bar if there is a difference 
      in the EOL of files
  - Reload files when another application changes files opened by WinMerge
  - Improve in-line diff algorithm
  - BugFix: More on Issue #73 Fixed the -dl -dm and -dr description parameters
      for conflict files
      (for both 2 way and 3 way conflict files).(Bitbucket #1)
  - BugFix: Fix Memory Leak with Drop Targets (GitHub #26)

### Binary Compare
  - Install binary file editor (frhed) component again by default
  - Make binary file editor undoable

### Image Compare
  - Add image file compare/merge support

### Reports
  - Add "Include File Compare Report" checkbox to "Folder Compare Report"
      dialog
  - Change the encoding of HTML folder compare report from ANSI to UTF-8
  - Add icons to the HTML folder compare report
  - Change visual style of HTML report

### Archive Support
  - Install 7-zip plugin by default

### Drag and Drop
  - Accept drag&droped files or folders from various places like:
      Zip folder, FTP folder, Recycle bin, images in Web browser

### Plugins
  - Introduce new plugin type FILE_FOLDER_PACK_UNPACKER
    which allows unpacking data in a file to multiple files in a folder
  - Use FILE_FOLDER_PACK_UNPACKER plugin for decompressing archives
  - Add support for unpacker plugin written in VBScript/Javascript.
  - Add WinMerge32BitPluginProxy.exe to make 32-bit plugins usable in 64-bit
      WinMerge 
  - Rewrite some plugins in VBScript
  - Add CompareMSPowerPointFiles.sct plugin
  - Add ApplyPatch.sct plugin
  - Add SortAscending, SortDescending and ExecFilterCommand menu item to 
      Plugin-&gt;Script menu
  - Make plugins configurable with settings dialog of each plugin

### Filters
  - Update *.flt files for Visual Studio 2015 and 2017

### Shell extension
  - Display another WinMerge icon in Explorer's context menu when a folder is
      selected

### Installer
  - 64-bit version of WinMerge is available
  - Install plugins by default

### Translations
  - Update Portuguese translation (GitHub #2-17)
  - Update Korean translation (GitHub #45)
  - Update Traditional Chinese translation (GitHub #53)
  - Remove "Language" menu item from View menu
      (You can specify UI language at Options dialog)

### Command line
  - Incompatible change: The third file path is now treated as a right path
      for 3-way comparison, not output path
      Use /o option for ouput path instead
  - Add /wm, /dm, /fl, /fm, /fr, /al, /am and /ar option for 3-way comparison

### Other changes
  - Add support for jump list introduced in Windows 7

### Internals
  - Use POCO C++ libraries instead of pcre, expat and scew.
      They use pcre, expat internally
  - Link statically with MFC and VC runtime libraries
  - Various compilation/build fixes
      (GitHub #1,19,21,29,31-33,36-38,42-44,49-52)

## WinMerge 2.14.0 - 2013-02-02 (r7596)
  - BugFix: Shell extension uses unquoted program path (patches#3023)
  - Translation updates
    - Dutch (patches:#3020)
    - Slovenian (patches:#3021)

## WinMerge 2.13.22 - 2013-01-13 (r7585)
  - Translation updates
    - Turkish (patches:#2967)
    - Russian (patches:#3017)
    - Norwegian (patches:#3018)
    - Danish (patches:#3019)

## WinMerge 2.13.21 - 2012-12-30 (r7575)
  - Update PCRE to version 8.10
  - Update SCEW to version 1.1.2
  - Improve startup time (#2788142)
  - Add menuitems for selecting automatic or manual prediffing (#2638608)
  - Add accelerator keys for Shell context menu (#2823536)
  - Improve editing of linefilter regular expressions (#3015416)
  - Allow editing context line count in patch creator (#2092180)
  - Improve color options organization (#2818451)
  - Add /xq command line switch for closing WinMerge after identical files
      and not showing message (#2827836)
  - Allow setting codepage from command line (#2725549)
  - Allow giving encoding name as custom codepage (#2813825, #3010934)
  - Add new options dialog panel for folder compare options (#2819626)
  - Add options GUI for quick compare limit (#2825628)
  - Write config log as UTF-8 file (r7057)
  - BugFix: Untranslated string ("Merge.rc:nnnn") was displayed 
      in status bar (#3025855)
  - BugFix: Pane headers not updated after language change (#2923684)
  - BugFix: Quick contents compare didn't ignore EOL byte differences (#2929005)
  - BugFix: Compare by size always checked file times too (#2919510)
  - BugFix: Crash when pasting from clipboard (#3109525)
  - BugFix: Keeps verifing path even turned off in options (#3111581)
  - BugFix: Crash after deleting text (#3109521)
  - BugFix: Added EOL chars between copied file/path names (#2817546)
  - BugFix: Created new matching folder to wrong folder (#2890961)
  - BugFix: Strange scrolling effect in location pane (#2942869)
  - BugFix: Plugin error after interrupting folder compare (#2919475)
  - BugFix: "+" and "-" from the number block don't work in the editor (#3306182)
  - BugFix: Date format did not respect Regional Settings (#3175189)
  - BugFix: When selecting multiple files in Patch Generator dialog,
      "Swap" button led to an error.  (#3043635, #3066200)
  - BugFix: WinMerge contained a vulnerability in handling project files (#3185386)
      (http://www.zeroscience.mk/mk/vulnerabilities/ZSL-2011-4997.php)
  - Installer: Remove OpenCandy from the InnoSetup installer (r7572, r7539)
  - New translation: Basque (#3387142)
  - Translation updates
    - French (#3412030)
    - Hungarian (#3164982)
    - Spanish (#3412937)

## WinMerge 2.13.20 - 2010-10-20 (r7319)
  - Add missing keywords to Pascal highlighter (#2834192)
  - Recognize .ascx files as ASP files (#3042393)
  - Fix help locations (#2988974)
  - Show only "Copy to other side" item in file compare
      context menu (#2600787)
  - Expand/collapse folders from keyboard (#2203904)
  - Improve detecting XML files in file compare (#2726531)
  - Initialize folder selection dialog to currently selected folder in
      options dialog (r6570)
  - New translation: Persian (#2877121, #3065119)
  - New translation: Serbian (#3017674, #3065119)
  - Installer: Drop Windows 9x/ME/NT4 support and use Microsoft runtime
      libraries installer (#3070254)
  - Installer: Remove Uninstall shortcut from start menu folder (#3076909)
  - Installer: Don't install quick launch icon for Windows 7 (#3079966)
  - Installer: Add OpenCandy to the InnoSetup installer (#3088720)
  - BugFix: WinMerge was vulnerable to DLL hijacking as described in
      Microsoft Security Advisory (2269637)  (#33056008)
  - BugFix: Location pane focus enabled "Save" (#3022292)
  - BugFix: "Copy and advance" toolbar icons not automatically enabled (#3033325)
  - Translation updates
    - Bulgarian (#3082392)
    - Chinese (#3033324)
    - Dutch (#2804979)
    - French (#2850842, #2968200)
    - Slovenian (#2917796, #2932094, #2934354, #3070136)
    - Spanish (#2930734)
    - Turkish (#2825132, #2827817)
    - Ukrainian (#2817835)

## WinMerge 2.12.4 - 2009-06-09 (r6833)
  - Add more C/C++ types to syntax highlight (#2776705)
  - Create config log file to My Documents-folder (#2789839)
  - BugFix: Reports had same left/right descriptions (#2772646)
  - BugFix: When conflict file saved trailing line-ending was lost (#2550412)
  - BugFix: File compare HTML reports use invald DOCTYPE (#2783477)
  - BugFix: "##" in file filter pattern caused an infinite loop (#2789345)
  - BugFix: Could select prediffer when plugins were disabled (#2787131)
  - BugFix: Opening mixed-EOL file one side forced both files to be handled as
      mixed-eol files (#2022312)
  - BugFix: Didn't show correct EOL chars if whitespace chars were turned off
      and again on (#2791402)
  - BugFix: Generated patch had wrong file time for UTF-16 files (#2791506)
  - BugFix: Did not export changed options values (#2799149)
  - BugFix: Reset the compare method to original if it was changed (#2802427)

## WinMerge 2.12.2 - 2009-04-01 (r6625)
  - Disable folder compare tree-view by default (#2714968)
  - BugFix: Filename extension column was empty for files (#2670046)
  - BugFix: Crash when selecting backup folder (#2686382)
  - BugFix: Swapping panes did not update statusbar (#2661838)
  - BugFix: Says files are identical after making files different in another
      application and re-loading them (#2672737)
  - BugFix: Files with missing last EOL weren't merged correctly (#2712120)
  - Translation updates
    - Polish (#2717382, #2720875)

## WinMerge 2.12.0 - 2009-03-05 (r6542)
  - Remember last target folder for Copy/Move to in folder compare (#2659228)
  - Detect Windows 7 and Windows 2008 in config log (#2599256)
  - BugFix: Help window opened from WinMerge was modal (#2590079)
  - BugFix: Crash in editor when undoing (#2590212)
  - BugFix: Browse button in Option dialog reverted to previous settings
    (#2606153)
  - BugFix: Files without extension get backed up to name..bak (#2627752)
  - BugFix: Must Show Different to Show L/R Unique (#2129561)
  - Translation updates
    - Chinese Traditional (#2608190)
    - Croatian (#2596949)
    - Czech (r6538)
    - Danish (#2648183)
    - Dutch (#2586422, #2590430)
    - French (#2603591)
    - Galician (#2632173)
    - German (#2586497)
    - Greek (#2615285)
    - Japanese (r6456)
    - Swedish (#2586274)
    - Ukrainian (#2609242)

## WinMerge 2.11.2 - 2009-02-05 (r6428)
  - Update Python syntax highlight keywords for Python 2.6 (#2473375)
  - More accurate visible area rect in location pane (#2479804)
  - New options page for Shell Integration options (#2433648)
  - Remove Frhed hex editor from GUI (context menu) (r6376)
  - Register.bat should use Find with full path (#2536582)
  - Better Unicode file detection for full contents compare (2477657)
  - BugFix: Could not open project file that contains an ampersand 
    character in folder names (#2372000)
  - BugFix: File compare method "Modified Date and Size" ignored file size
      (#2455595)
  - BugFix: Quick compare didn't ignore EOL differences (#2433677)
  - BugFix: Remove EOL characters from command line (#2458728)
  - BugFix: Merging one line diff to empty other side didn't work (#2456399)
  - BugFix: Location pane was empty for some files with diffs (#2459234)
  - BugFix: Line filter editing didn't update edit box when editing was
      canceled by ESC key (#2493935)
  - BugFix: Wrong number shown for difference when using line filters (#2493980)
  - BugFix: Crashed if compared file disappeared during file compare (#2493945)
  - BugFix: Creating new files caused message about missing files (#2515875)
  - BugFix: Could not hide folders in recursive compare (#2528749)
  - BugFix: Folder Compare:Copy Right/Left don't create folder
  - BugFix: Must Show Different to Show L/R Unique (#2129561)
  - BugFix: Could not copy files from right to left (#2556904)
  - BugFix: Don't show file name extension for folders (#2563314)
  - Translation updates
    - Brazilian (#2461443, #2524796)
    - Chinese Traditional (#2540115)
    - Czech (r6201)
    - Dutch (#2481022, #2494378, #2499994)
    - French (#2551043, #2551066)
    - Greek (#2550911)
    - Swedish (#2452467, #2465385, #2511043)

## WinMerge 2.12.x (R2_12) branch created (r6202)

## WinMerge 2.11.1.8 - 2008-12-19 (r6200)
  - Allow reloading the file if it has been changed on the disk
    behind WinMerge (#2354125)
  - Add option to show Open-dialog on WinMerge startup (#2327424)
  - Add Git to Source Control filter and fix ignore file rules (#2329381)
  - Add build identifier for 64-bit builds to About-dialog (r6142)
  - Installer: Option to add WinMerge to system path (#2435175)
  - Add Unicode version of heksedit (#2036603)
  - Allow multiple file compare documents by default (#2445749)
  - BugFix: Move confirmation dialog sometimes had wrong paths (#2327565, #2309190)
  - BugFix: Use translated version of readme.txt for Chinese Simplified (#2391000)
  - BugFix: Installer didn't install readme.txt for all translations (#2396437)
  - BugFix: Could not give paths to commandline when started WinMerge
      from Cygwin prompt (#2427946)
  - BugFix: Infinite loop in heksedit's Replace-dialog (r6176)
  - Translation updates
    - Croatian (#2390558)
    - German (r6130)
    - Greek (#2379356)
    - Japanese (r6154)
    - Swedish (#2318778, #2374278)

## WinMerge 2.11.1.7 - 2008-11-20 (r6112)
  - Add shell context menu to folder compare context menu (#2189315)
  - Update PCRE to version 7.8 (#2209911)
  - Show missing lines in location pane for diffs having both different
      and missing lines (#2119235)
  - Syntax highlighting for Verilog files (#2231145)
  - Tweak new "equal file" icon a little bit (#2312381)
  - Readme update (#2234689)
  - Translation updates
    - Brazilian (#2219568)
    - Czech (r6034)
    - Swedish (#2215167, #2236651)

## WinMerge 2.11.1.6 - 2008-10-23 (r6033)
  - General compare status for files (not known if text/binary) (#2175736)
  - Change icons for the new general compare status (#2185302)
  - Use 24bit icons for "Bookmarks" and "Delete" menu items (#2141184)
  - Installer: create backup of ClearCase config file (#2129095)
  - BugFix: Translatable strings having space before colon (#1971868)
  - BugFix: Wrong cursor position when disabling selection margin (#2138555)
  - BugFix: Showed a black line when disabling selection margin (#2068790)
  - BugFix: Fix manual URL in Readme.txt (#2181628)
  - BugFix: Long unique paths were mixed up in folder compare view (#2129634)
  - BugFix: Some calls made to plugins when plugins were disabled (#2182788)
  - BugFix: If messagebox was shown during file loading location pane
      didn't show any differences (#2101780)

## WinMerge 2.11.1.5 - 2008-10-02 (r5995)
  - Use external editor to edit file filters (#2082774)
  - Improved command line handling (#2081779)
  - Change Manual url to http://winmerge.org/docs/manual/ (r5956)
  - Manual updates
  - Plugins: Show processing instructions in XML plugin (#1979321)
  - Add icon for "Compare" menu item (#2119843)
  - Change download URL to current URL (#2136765)
  - Installer: create backup of ClearCase config file (#2129095)
  - BugFix: ClearCase checkout/commit dialog translation error (#2051069)
  - BugFix: Table of Contents in manual (#2071625)
  - BugFix: Highlight ShowUnInstDetails keyword for NSIS files (#2084879)
  - BugFix: Displayed a blank time when file last modified time is 00:00:00
      (#2098626)
  - BugFix: Customized colors weren't used in some areas (#2110218)
  - BugFix: Zip files were extracted into wrong folder (#2110350)
  - BugFix: Case-sensitive regexp search didn't work (#1991259)
  - BugFix: There were some installer regressions
         (eg. Installer didn't create Quick Launch icon) (#1460517)
  - BugFix: Whitespace ignore options didn't work with quick compare (#2120174)
  - BugFix: Giving path to file and folder to command line
      did not work (#2138560)
  - New translation: Galician (#2120685)
  - Translation updates
    - German (r5934)
    - Russian (#2067785)
    - Swedish (#2075513)
    - Ukrainian (#2131753)

## WinMerge 2.11.1.4 - 2008-08-21 (r5846)
  - Binary file edit/compare (#2036603)
  - Add wait cursor for undo and redo (#2039228)
  - Add icon for "Open Conflict File" menu item (#2047054)
  - Include unique subfolder contents in recursive folder compare (#2034259)
  - Installer: Use -u switch in TortoiseSVN command line (#2060782)
  - Add icon for "Plugins-&gt;List" menu item (#2060771)
  - BugFix: External editor command line switches did not work (#2037862)
  - BugFix: Deleting/Moving file crashed WinMerge in debugger (#2025833)
  - BugFix: Lost focus after folder compare operations (#2046002)
  - BugFix: Some files had folder icons when aborting folder compare (#2018836)
  - BugFix: Didn't select word-diff cyclically by pressing F4 key (#1989858)
  - BugFix: Could not reset EOL ignore after selecting it from query
      dialog (#2056741)
  - Translation updates
    - Brazilian (#2059836)
    - German (#2045666)
    - Greek (#2062442)
    - Chinese Traditional (#2039929)
    - Japanese (r5805)
    - Swedish (#2062107)

## WinMerge 2.11.1.3 - 2008-08-04 (r5733)
  - Rename color scheme menu item "None" and add separator (#2027300)
  - Add -u commandline parameter as alternative to -ub (#2029275, #2030359)
  - Remove RCLocalizationHelper plugin (#2029457)
  - Clarify Supercopier caused hang in release notes (#2030040)
  - Improve tree mode in folder compare (#2018836)
  - Unlimited undo (#2033754)
  - BugFix: Could change syntax highlight mode when highlighting was
      disabled from options (#2027325)
  - BugFix: Editing line filters updated wrong filter (#2025130)
  - BugFix: Boldfacing did work only for Courier font (#2000996)
  - BugFix: Shift+Backspace didn't work in editor (#1153696)

## WinMerge 2.11.1.2 - 2008-07-24 (r5673)
  - Refresh compare after changing filter (#2017426)
  - Add dialog listing available plugins (#2021722)
  - Hierarchical directory comparison result view (#2018836)
  - Add "Open Conflict" for file/folder compare menus (#2025472)
  - Enable hierarchial folder compare view by default (#2025478)
  - Confirmation dialog for move operations (#2026508)
  - Manual: New build system (#2015434)
  - Manual: Improve Opening Files and Folders (#2017901)
  - Manual: Improve folder compare documentation (#2024587)
  - BugFix: Failed to compare UTF-8 files plugins disabled (#2018287)
  - BugFix: Don't warn about closing multiple windows when only one
      window is open (#2010463)
  - BugFix: Moving was allowed for protected items (#2023734)
  - BugFix: Could change EOL style for protected file (#2025781)

## WinMerge 2.11.1.1 - 2008-07-11 (r5609)
  - Manual: Introduction update (#2011365)
  - Manual: Reorganize structure (#2011924)
  - Add ShellExtension menu for drive letters (root folders) (#2011602)
  - Remove buildall.bat (#2013576)
  - New option to enable plugins (disabled by default) (#2013845)
  - Plugins: Show processing instructions in XML plugin (#1979321)
  - BugFix: Use system codepage for paths (#1979429, #2008581)
  - BugFix: ANSI build crash in folder compare (#2010053)
  - BugFix: Add insert/overtype mode indicator to statusbar (#2015174)
  - BugFix: Detected UTF-8 encoding shown as 65001 (#2012377)

## WinMerge 2.10.x (R2_10) branch created (r5561)

## WinMerge 2.9.1.5 - 2008-07-04 (r5560)
  - Manual: Copy editing (#2000322, #2005418)
  - Manual: Clarify 64-bit support (#2002431)
  - Reduce executable sizes (#2003203)
  - Release Notes update (#2007673, r5557)
  - BugFix: Possible crash when enabling line filter (#2004160)
  - BugFix: Location Pane flickers when scrolling (#1938177, #2005031)
  - BugFix: Use system codepage for paths (#1979429, #2008581)
  - BugFix: Random crashes in folder compare (#2005308)
  - Translation updates
    - Chinese Traditional (#2007426)

## WinMerge 2.9.1.4 - 2008-06-24 (r5518)
  - Use PCRE regular expressions in the editor (#1991259)
  - Installer: install to all users (#1460517)
  - Update release notes (#1990886, #1995265, #1999880)
  - Manual: Use PNG files (#1988845, #1989744)
  - BugFix: Temp paths in Reports created from archive file (#1984346)
  - BugFix: ISL file syntax highlighting starts comment with { (#1981326)
  - BugFix: Crash when starting a second instance (#1993835)
  - BugFix: Crash when starting WinMerge with only one path (#1994402)
  - BugFix: Messages missing from installer translations (r5506)
  - BugFix: Always right side as origin side in copy confirmation dialog
      when Copy To... (#2001237)
  - New translation: Slovenian (#1997236)
  - Translation updates
    - Brazilian (#1994578)
    - Swedish (#2000307)

## WinMerge 2.9.1.3 - 2008-06-05 (r5438)
  - Update PCRE (regular expression library) to version 7.7 (#1941779)
  - Add VB.net keywords to Basic highlighter (#1970912)
  - Open conflict files by drag & dropping to main window (#1984884)
  - Possible improvement for WinMerge exit bug (#1602313)
  - BugFix: Location Pane flickers when resizing (#1740106)
  - BugFix: Command line description text starting with "/" confused
      WinMerge (#1973225)
  - BugFix: Item count in statusbar not get cleared (#1976480)
  - New translation: Ukrainian (#1973149, #1974021)
  - Translation updates
    - Brazilian (#1978374)
    - Korean (#1978362)
    - Swedish (#1980970)

## WinMerge 2.9.1.2 - 2008-05-23 (r5362)
  - Optimize location pane (r5341, r5342, r5346, r5351, r5355, r5356)
  - BugFix: Version Control filter did not match all VSS files (#1881186)
  - BugFix: Fix showing differences in the end of the line (#1883409, #1962816)
  - BugFix: Fix command line paths in quotes ending with "\" (#1963523)
  - New translation: Greek (#1968429)
  - Translation updates
    - Bulgarian (#1957434)
    - Croatian (#1958283)
    - Swedish (#1950188, #1970074)

## WinMerge 2.9.1.1 - 2008-04-23 (r5324)
  - Release Notes update (#1933950)
  - Installer: Use correct way to create start menu web link (#1913919)
  - Installer: Simplify DeletePreviousStartMenu() function (#1913949)
  - Installer: Updated the side art of the installer application (#1915012)
  - Update URLs and switch to 2.8 manual (#1919965)
  - New splash screen (#1914869)
  - Tab icons at Filter dialog (#1926531)
  - Add VS2003/VS2005 project files (#1926346)
  - Create separate document for compiling WinMerge (#1927502)
  - Update InnoSetup translations (#1931492)
  - Make 'Yes' the default button in copy confirmation dialog (#1943647)
  - Allow drag & drop of paths to Open dialog (#1945254)
  - Enlarge 'Discard All' button in Save confirmation dialog (#1947216)
  - BugFix: Installer could create a registry access error (#1909967)
  - BugFix: The filter text box in Open-dialog sometimes displayed some garbage
      data (#1915424)
  - BugFix: Print area was not clipped correctly. (#1915536)
  - BugFix: Undo after pane swapping (#1915536, #1923625)
  - BugFix: Quick compare for same folder didn't show differences (#1918599)
  - BugFix: Installer didn't have ChangeLog.txt included (r5255)
  - BugFix: Shortcuts given in command line were not expanded (#1942217)
  - New translation: Romanian (#1939729)
  - Translation updates
    - Brazilian (#1913723)
    - Chinese Traditional (#1927605)
    - French (#1920380)
    - German (r5130, r5139, r5171, r5203, r5236, #1949068)
    - Italian (#1939235)
    - Japanese (r5152)
    - Polish (#1934655)
    - Russian (#1914466)
    - Spanish (#1907152, r5250)
    - Swedish (#1908289)

## WinMerge 2.8 RC - 2008-03-04 (r5125)
  - Update developers list in splash screen (#1891548)
  - Better error handling for Excel plugin (#1510293)
  - Add macros, bookmarks and document properties to Word plugin (#1510298)
  - Add *.dot (document template) extension to Word plugin (r5120)
  - BugFix: Copy&Paste from VB6 added binary chars (#1904355)
  - Translation updates
    - Chinese Traditional (#1905323)
    - Swedish (#1905520)

## WinMerge 2.7.7.6 - 2008-02-28 (r5099)
  - Remove sintance.h/sintance.cpp as not compatible with GPL (#1886580)
  - Automatically switch to mixed-EOL mode when loading files with
      two or more EOL styles (#1901547)
  - Indent "Explorer Context Menu" sub options (#1901763)
  - Replace 'DOS' in EOL style strings with 'Windows' (#1902583)
  - Show 'Mixed' as file compare EOL style instead of empty style (r5091)
  - Typo fixes and grammar improvements to several strings (#1898401)
  - BugFix: Ignore codepage specified on File Encoding dialog if file was 
      detected as UTF-8 (#1900728)
  - BugFix: Recognize invalid UTF-8 sequence as UTF-8 (#1900733)
  - BugFix: Didn't highlight any difference between untitled documents (#1900257)
  - Translation updates
    - Czech (r5073)
    - Japanese (r5076)
    - Swedish (#1901784)

## WinMerge 2.7.7.5 - 2008-02-22 (r5069)
  - Cleaning up Help-menu (#1875111)
  - Right-align tab size number in editor options (r5037)
  - Move Time difference ignore -option to compare options (#1892283)
  - Add option to select temporary folder (#1893167)
  - Show file encoding in file compare statusbar (#1895629)
  - Unify EOL style strings in folder compare and file compare (#1896462)
  - Remove "DisplayBinaryFiles" and "EditBinaryFiles" plugin (#1899161)
  - BugFix: Potential lockup in folder compare (#1865131, #1889907)
  - BugFix: Line filter didn't filter non-ascii lines correctly (#1880628)
  - BugFix: GNU General Public License name in menu (#1868989)
  - BugFix: Didn't switch back to content compare once switched to quick 
      compare in folder compare (#1770373)
  - BugFix: Quick compare didn't ignore EOL differences (#1884717)
  - Translation updates
    - Croatian (r5032)
    - Czech (r5017)
    - German (r5038)
    - Slovak (#1895583)
    - Swedish (#1891326)

## WinMerge 2.7.7.4 - 2008-02-07 (r5011)
  - Detect (and read/write) UTF-8 files without BOM (#1879271)
  - Tell user that file is not a conflict file (#1880423)
  - Allow opening conflict file from command line (#1880857)
  - Use radio symbol for current EOL menu item (#1869755)
  - Ask and create pair for unique folder and open them (#1881454)
  - Add publisher info to version resource (#1884920)
  - Move "Zoom In/Out" to own sub menu (#1879340)
  - Installer: Add more version resource info (#1885793)
  - Reset text zoom to default size with Ctrl + * (#1886664)
  - BugFix: Always return 0 to command line (#1854419)
  - BugFix: Merge/Delete deleted items from read-only sides (#1882019)
  - BugFix: Shellextension had no publisher info and wrong
      description (#1878828)
  - BugFix: Lockup when showing binary files (#1880711)
  - Translation updates
    - Croatian (r4952)
    - Japanese (r4962)
    - Swedish (#1884918)

## WinMerge 2.7.7.3 - 2008-01-23 (r4951)
  - Resolve conflict files (cvs, svn...) (#1875129)
  - Save line filter when edit box loses focus (#1875853)
  - Add link to translations website at help menu (#1699883)
  - Manual: Add instructions for conflict file resolving (#1876770)
  - Manual: Document third path given to command line (#1877735)
  - BugFix: Fix Quick Compare regression in 2.7.7.2 (#1872165)
  - BugFix: Clipped text in file saving error message (#1874854)

## WinMerge 2.7.7.2 - 2008-01-14 (r4915)
  - Syntax highlighting for PowerShell files (#1859657)
  - Syntax highlighting for PO(T) files (#1866221)
  - Change font size with Ctrl + MouseWheel (#1865795, #1869686)
  - Horizontal scrolling with Shift + MouseWheel (#1865795)
  - Installer: Add support for integration with latest version of TortoiseCVS and
      TortoiseSVN x64 (#1865168)
  - Close compare tab with middle mouse button (#1865220)
  - Add index.html for documentation folders (#1866183)
  - Manual: Clarify file filters folder name matching (#1867212)
  - Improve file compare margin icons location (#1718699)
  - Enable file compare to open binary files (#1869647)
  - File encoding dialog for file compare (#1871079)
  - BugFix: Crash when using TrackPoint middle-button scrolling (#1864040)
  - BugFix: WinMerge didn't show the contents of the symbolic link target (#1864978)
  - BugFix: Unused registry key HKCU\Thingamahoochie\WinMerge was created 
      (#1865202)
  - BugFix: Register.bat didn't work on Windows 98 and Vista (#1869821)
  - BugFix: Open-dialog was not displayed on Windows 98 (#1866442, #1869858)
  - BugFix: Right mouse button didn't move cursor to new place (#1814184)
  - Translation updates
    - Spanish (#1867234)
    - Swedish (#1867765, #1867844)

## WinMerge 2.7.7.1 - 2007-12-20 (r4842)
  - New option to keep file's time when saving in file compare (#1851921)
  - Installer: Link to translated ReadMe in Start menu, if file exists (#1805818)
  - Add HTML-formatted release notes (#1851308, #1852534)
  - Installer: Install release notes file (#1852599)
  - Installer: Move manual to core files component and remove
  - User Guide component (#1853409)
  - BugFix: VS2008 and VS2005 compile problems (#1847265)
  - BugFix: More difference highlight code reverts (#1748940)
  - Translation updates
    - Spanish (#1655577)

## WinMerge 2.7.6 - 2007-11-29 (r4807)
  - Touchpad scrolling improvements (#1837457)
  - BugFix: Values with "," inside could break the CSV report (#1831512)
  - BugFix: Failed to create backup file in folder compare (#1835283)
  - BugFix: Revert difference highlight code to 2.6.12 version (#1811695)
  - BugFix: Backspace key didn't work when renaming items (#1738790)
  - Translation updates
    - Croatian (r4786)

## WinMerge 2.7.5.7 - 2007-11-19 (r4772)
  - Remove MFC dependency from ShellExtension (#1833521)
  - Update ShellExtension icon to new WinMerge icon (#1833616)
  - BugFix: Build script didn't update PO files correctly before copying (r4756)
  - BugFix: ShellExtension's icon was clipped if system font size was small
      (#1833616)
  - BugFix: Merge controls were not enabled for missing lines (#1833714)

## WinMerge 2.7.5.6 - 2007-11-15 (r4752)
  - BugFix: Many translation-system related fixes and improvements (#1804762)
  - BugFix: When copying files to VCS system, destination file was
      not checked out (#1828930)
  - BugFix: Visible area indicator on location pane shook when clicking
      above or below the vertical scroll bar to scroll one screen (#1829572)
  - Translation updates
    - Croatian (#1829301)
    - Japanese (r4730)

## WinMerge 2.7.5.5 - 2007-11-08 (r4722)
  - BugFix: Don't enable merge GUI for ignored differences (#1826375)
  - BugFix: PgUp key caused the error message "An invalid argument was
      encountered" when word-wrap mode was on (#1820631)
  - BugFix: Keep selected items visible in Folder Compare columns dialog (r4715)
  - BugFix: Disable Folder Compare column selection dialog's up/down buttons
      when first/last items are selected (r4716)
  - BugFix: Many translation-system related fixes and improvements (#1804762)
  - Add Croatian translation (#1820308)
  - Translation updates
    - Chinese Traditional (#1824691)

## WinMerge 2.7.5.4 - 2007-11-01 (r4687)
  - New PO files based translation system (#1804762)
  - New folder compare column selection dialog (#1804555)
  - Install WinMerge.exe (ANSI version) only for Windows 95/98/ME (#1809140)
  - Better multi-monitor support, with option to lock panes (#1788168)
  - Installer: Install PO files and MergeLang.dll (#1820689)
  - BugFix: Ignored differences were merged when merging multiple
      differences (#1811745)
  - BugFix: Don't clear selection when right-clicking outside it (#1814184)
  - BugFix: Configuration log labeled Vista as Longhorn (#1815859)
  - BugFix: Customized text color for ignored differences didn't shown (#1807895)
  - Translation updates
    - Chinese Traditional (#1810192, #1810541)
    - Italian (#1805044)

## WinMerge 2.7.5.3 - 2007-09-27 (r4572)
  - Use PO files for translations (generate RC-files from PO-files)
  - Add Portuguese translation (#1756364)
  - Change Up/Down button names in folder compare columns dialog(#1800064)
  - BugFix: Fix MakeResDll crash (#1795421)
  - BugFix: Full screen mode didn't work with multiple monitors (#1788168)
  - BugFix: Revert multiple monitor patches (#1788168)
  - BugFix: PgUp key didn't do anything in beginning of the file (#1795680)
  - BugFix: The option 'Automatically scroll to first difference' didn't work
      properly when word-wrap mode was on (#1795814)
  - Translation updates
    - French (#1800064, #1802363)
    - Japanese (r4546)

## WinMerge 2.7.5.2 - 2007-09-11 (r4528)
  - Graphical improvements to location pane (#1770175)
  - Remove MFC dependency from resource compiler (#1783581)
  - Better multi-monitor support (split file view on monitor boundary) (#1788168)
  - BugFix: Renaming unique item (only other side) renamed parent folder (#1787816)
  - BugFix: Goto didn't change the active pane when selecting another pane (#1791351)
  - Translation updates
  - Traditional Chinese (r4473)

## WinMerge 2.7.5.1 - 2007-08-16 (r4440)
  - HTML reports for file compare (#1739209)
  - Ask confirmation when exiting with multiple windows open (#1757800)
  - Translation updates
    - Catalan (r4370)
    - Simplified Chinese (r4369)
    - Danish (r4360)
    - French (r4383)
    - Russian (r4421)
    - Slovak (r4413)
    - Swedish (r4412)

## WinMerge 2.7.4 - 2007-06-27 (r4352)
  - Batch-file syntax highlighting keywords cleanup (#1743504)
  - BugFix: Archivesupport Zip--&gt;both didn't work correctly (#1734439)
  - BugFix: Linediff didn't highlight whitespace differences (#1698781)
  - BugFix: Copy confirmation dialog did show wrong To-path (#1740024)
  - Translation updates
    - Italian (#1737491)

## WinMerge 2.7.3.7 - 2007-06-14 (r4330)
  - New big toolbar (actual image instead of placeholder) (#1736520)
  - BugFix: Fixes to folder compare copy/rename operations (#1730534)
  - Translation updates
    - Swedish (#1735635)

## WinMerge 2.7.3.6 - 2007-06-07 (r4321)
  - Recent project files in File-menu (#1731551)
  - Automatically import existing linefilters to new dialog (#1718040)
  - BugFix: copy/delete of unique items failed in folder compare
      in 2.7.3.5 (#1730534)
  - BugFix: crashed when ESC was pressed while renaming item in
      folder compare (#1705874)

## WinMerge 2.7.3.5 - 2007-05-31 (r4304)
  - Relax selecting parent folder restrictions (up arrow in folder compare) (#1727829)
  - Show a warning if item can't be found anymore in folder compare
      and it is tried to access (copy/move/delete) (#1723778)
  - New Borland Delphi file filter (#1699783)
  - Gradient toolbar (#1717774)
  - BugFix: could not create new file filters in 2.7.3.4 (#1719892)

## WinMerge 2.7.3.4 - 2007-05-15 (r4287)
  - Big (32x32) toolbar (image still placeholder) (#1698641)
  - Improve folder compare copy confirmation dialog (#1699585, #1700241)
  - New file filter for Symbian developers (#1694048)
  - BugFix: several fixes to in-line difference highlight (#1694102, #1714088)
  - BugFix: word wrap lose horizontal scrollbar (#1706476)
  - BugFix: fixes to tabbed window control (#1718148)
  - Translation updates
    - German (#1701151)
    - Japanese
    - Korean (#1704904)
    - Swedish (#1703350)

## WinMerge 2.7.3.3 - 2007-04-04 (r4234)
  - Tabbed interface (#1603292)
  - Enable shell integration for folder backgrounds (#1693113)
  - Allow selecting syntax highlighting (#1690346)
  - New C# development file filter (#1689854)
  - Icon updates (#1686279, #1692097)
  - BugFix: compare by date and by size broken in 2.7.3.2 (#1693441)
  - BugFix: Open-dialog file filter failed if modified (#1693275)
  - Translation updates
    - Swedish (#1689515)

## WinMerge 2.7.3.2 - 2007-03-27 (r4201)
  - Several fixes to line difference highlighting (#1491334, #1683061, #1639453)
  - Icon updates (#1684865)
  - Printing improvement (#1687430)
  - Language files size reduced (#1687661, #1687983, #1688012)
  - BugFix: 2.7.3.1 binary file compare crash (#1687966)
  - BugFix: long linefilters now restored correctly (#1680920)
  - Translation updates
    - Japanese

## WinMerge 2.7.3.1 - 2007-03-20 (r4179)
  - New copy confirmation dialog in folder compare (#1675087, #1683842)
  - Improved line filter dialog (#1682475)
  - Installer/uninstaller icons restored to default icons
  - Some icon updates (#1680209)
  - BugFix: folder compare lost focus after confirmation dialog (#1670991)
  - BugFix: crash comparing empty folders (#1675211)
  - Translation updates
    - Czech
    - French (#1664689)
    - Polish (#1673082)
    - Swedish (#1673908, #1678676, #1683289)

## WinMerge 2.7.2 - 2007-02-27 (r4137)
  - Fix ShellExtension installation bug 
  - Translation updates
    - Czech
    - German (#1667564)
    - French (#1664689)

## WinMerge 2.7.1.7 - 2007-02-20 (r4120)
  - Folder compare threading improvement - should be faster in many
      situations (#1662002)
  - New options for backup file location and naming (#1652696)
  - Syntax highlighting for CSS files (#1582537)
  - Allow to hide folder compare margins (View/View Margins) (#1663798)
  - Translation updates
    - Czech
    - Polish (#1650831)
    - Swedish (#1657664)

## WinMerge 2.7.1.6 - 2007-02-01 (r4094)
  - New post-compare line filters for folder compare (#1644820)
  - Optimization for word wrap code (#1640741)
  - BugFix: 2.7.1.5 forgets selected filter (#1637433)
  - BugFix: limiting to single file compare window now works more
      logically (#1636314)
  - BugFix: ANSI regular expressions didn't work (#1644668)
  - Translation updates
    - Bulgarian (#1639493)
    - Catalan (#1646638)
    - Swedish (#1634968)

## WinMerge 2.7.1.5 - 2007-01-10 (r4030)
  - New line filter implementation (please test and report bugs!) (#1593810)
  - Minimum height for location pane's visible area (#1489875)
  - Short label "Error" for error items in folder compare (#1567749)
  - BugFix: rename edits wrong field in folder compare (#1597939)
  - BugFix: too much scrolling in difference navigation
  - BugFix: wrong text selection after pane switch in file compare (#1630630)
  - BugFix: crash when editing space-char indented files (#1631613)
  - Manual updates

## WinMerge 2.7.1.4 - 2006-12-21 (r3991)
  - Match lines inside differences (#1447744)
  - Workaround-patch (locally included to the build) for
      shutdown problems (#1602313)
  - New ASP.Net filefilter (#1619689)
  - BugFix: folder compare speed regression in 2.7.1.3 (#1610442)
  - BugFix: new fix for folder compare Copy Left/Right To.. (#1603061)
  - BugFix: copyright info missing from about dialog when
      translation selected (#1604115)
  - BugFix: location pane problems with word-wrap (#1584068, #1611542)
  - BugFix: invalid path added to project file -dialog (#1602219)
  - Swedish translation update (#1614442)
  - Manual updates

## WinMerge 2.7.1.3 - 2006-12-05 (r3919)
  - Now hides skipped items by default (#1604078)
  - Improved INI file syntax highlighting (#1607193)
  - Easier folder selection in Project File -dialog (#1603196)
  - New "Exclude Source Control files and directories filter" filefilter (#1557295)
  - BugFix: project files weren't loaded correctly from command line
      (using shell file association) (#1602214)
  - BugFix: crash in syntax parser (#1556688)
  - BugFix: wrong difference navigation when word-wrap enabled (#1597814)
  - BugFix: ClearCase integration didn't install if "Program Files" -folder
      was named differently (non-English Windows)
  - Manual updates
    - Slovak translation update (#1606496)

## WinMerge 2.7.1.2 - 2006-11-24 (r3844)
  - close WinMerge if Open-dialog is canceled when started
      from command line (#1600714)
  - BugFix: project files didn't work (loaded right-side path
      for both sides) (#1600000)
  - BugFix: location pane misdrawn when word-wrap enabled (#1584068)

## WinMerge 2.7.1.1 - 2006-11-16 (r3810)
  - Use expat + scew for XML handling (partially converted) (1587574)
  - Uses PCRE for regular expressions (partially converted) (#1591605)
  - New folder compare icons (#1586705)
  - New bookmarks icons (#1586689)
  - Manifest files in resource, no need for separate files (#1588212)
  - BugFix: WinMerge/Filters folder was always created (#1578419)
  - BugFix: modality problems in file selection dialogs (#1586869)
  - BugFix: crash if file modification time was missing (#1589563)
  - BugFix: /x parameter didn't work with some other parameters (#1564283)
  - BugFix: asked sometimes twice about saving modified files (#1596692)


For older changes look at the VCS (Version Control System).
