2003-11-05 Laoran
 change directory name 'Plugins' to 'MergePlugins' (see BUG [ 832090 ])
  WinMerge: Plugins.cpp

2003-11-05 Perry
 deleted unused function files_loadLines per Laurent's suggestion 
  (in PATCH: [ 826250 ] New UniMemFile class for memory-mapping)
 WinMerge: files.cpp files.h

2003-11-04 Perry
 PATCH: [ 836186 ] Unicode fix for lwdisp.co
  common: lwdisp.c

2003-11-05 Laoran
 PATCH: [ 831384 ] dir comparison : state bar & stop button
  WinMerge: DiffContext.cpp DiffContext.h DiffThread.cpp DirDoc.cpp DirFrame.cpp DirFrame.h
   MainFrm.cpp MainFrm.h Merge.dsp Merge.rc resource.h
  WinMerge(addition): DirCompStateBar.cpp DirCompStateBar.h
  Languages : *.rc and resource.h
 
2003-11-04 Laoran
 adapt plugins filters to the syntax of regular expressions
  Plugins\dlls: RCLocalizationHelper.dll
  Plugins\src\HideFirstLetter: WinMergeScript.cpp
  Plugins\src\HideLastLetter: WinMergeScript.cls
  Plugins\src\IgnoreTwoFirstCharsOrder: WinMergeScript.cls
  Plugins\src\RCLocalizationHelper: WinMergeScript.cpp
 fix to test plugin filter with left & right filenames (instead of just right one)
  WinMerge: Plugins.cpp
 PATCH: [ 833961 ] Swedish translation 2.1
 Submitted by Göran Håkansson
  Languages: BuildAll.bat
  Languages\Swedish: MergeSwedish.rc MergeSwedish_LastTranslation.rc resource.h
  Languages\Swedish\res : all files

2003-11-04 Kimmo
 PATCH: [ 831052 ] Write version and configuration info
  WinMerge: MainFrm.h MainFrm.cpp Merge.dsp Merge.rc resource.h
  WinMerge new files: ConfigLog.h ConfigLog.cpp
  Languages: *.rc resource.h
 PATCH: [ 835220 ] Update default colors
  WinMerge: Merge.cpp

2003-11-03 Laoran
 PATCH: [ 834122 ] Add Error Handling around calls to plug-ins
  WinMerge: Merge.rc MergeDoc.cpp Plugins.cpp Plugins.h files.h resource.h
  common: lwdisp.c lwdisp.h
 PATCH: [ 833188 ] clever and explicit error report in TypeLib
  common: lwdisp.c

2003-11-03 Kimmo
 PATCH: [ 834526 ] Check that file exist before executing in ShellExtension
  ShellExtension: ShellExtension.dsp WinMergeShell.h WinMergeShell.cpp

2003-11-02 Kimmo
 BUG: (part of) [ 829435 ] Fix character classification routines for UNICODE
  WinMerge: MainFrm.cpp MergeDoc.cpp Paths.h Paths.cpp
 PATCH: [ 834230 ] Use PostMessage() to update dir compare status
  WinMerge: DiffContext.cpp

2003-11-02 Laoran
 Creation of directory Docs
  WinMerge(moved to Docs): readme-developers.html readme-developers-MakePatchDirs.html
 BUG: [ 834490 ] Ignore blank lines disabled when ignore EOL
  WinMerge: UTIL.C

2003-11-01 Kimmo
 BUG: [ 833096 ] Left/right-only status overwrites skipped-status
  WinMerge: DirViewColItems.cpp
 Fix signed/unsigned clashes
  WinMerge: MergeDiffDetailView.cpp MergeDoc.cpp MergeEditView.cpp
  editlib: ccrystaltextview.cpp

2003-10-31 Kimmo
 Remove whitespaces from begin and end of ShellExtension.dll path
  WinMerge: PropRegistry.cpp
 PATCH: [ 833681 ] Fix default editor for external editor
  WinMerge: MainFrm.h MainFrm.cpp
 Change action.code type to match DIFFITEM.code
  WinMerge: DirView.h
 Removing unused variables
  WinMerge: DiffWrapper.cpp DirActions.cpp DirDoc.cpp DirScan.cpp
   DirViewColHandler.cpp MainFrm.cpp Merge.cpp MergeDoc.cpp
 BUG: [ 830258 ] Problem compiling with visual .net C++
  Submitted with fixes by Steve Beaudoin
  common: UniFile.h
  WinMerge: ChildFrm.cpp files.cpp

2003-10-31 Laoran
 fix Plugins PACK_UNPACK when nothing changes (+ update functions name)
  WinMerge: FileTransform.cpp Plugins.cpp Plugins.h

2003-10-30 Laoran
 BUG: [ 830911 ] trailing whitespaces not ignored
  WinMerge: UTIL.C
 PATCH: [ 830132 ] open empty file
  common: UniFile.cpp
  WinMerge: MergeDoc.cpp files.cpp

2003-10-30 Kimmo
 Remove SillyTestCrap() from Merge.cpp
  WinMerge: Merge.cpp
 Compare filename extensions as lowercase (before calling external editor)
  WinMerge: MainFrm.cpp
 Comment out GetIP() to fix compiling by including coretools.h
  common: coretools.h coretools.cpp

2003-10-29 Laoran
 change events PHYSICAL_PREPROCESS and SYNTAX_PREPROCESS to PREDIFFING
  WinMerge: FileTransform.cpp FileTransform.h Plugins.cpp Plugins.h

2003-10-28 Kimmo
 PATCH: [ 830553 ] Finetune scroll to diff
  WinMerge: MergeEditView.h MergeEditView.cpp

2003-10-27 Laoran
 BUG: [ 830917 ] Rescan effects when validate changes in options
  WinMerge: MainFrm.cpp

2003-10-26 Laoran
 PATCH: [ 827332 ] detach buffer during load and rescan
  WinMerge: GhostTextView.cpp GhostTextView.h MergeDiffDetailView.cpp MergeDiffDetailView.h
   MergeDoc.cpp MergeEditView.cpp MergeEditView.h
  editlib: ccrystaltextview.cpp ccrystaltextview.h
 PATCH: [ 829895 ] Fixed German translation
 Submitted by Tim Gerundt
  Languages: MergeGerman.rc

2003-10-26 Kimmo
 PATCH: [ 829570 ] Open patches after creation
  WinMerge: DirActions.cpp MainFrm.h MainFrm.cpp Merge.rc PatchDlg.c
   PatchDlg.cpp PatchTool.h PatchTool.cpp resource.h
  Languages: *.rc resource.h

2003-10-25 Laoran
 BUG: [ 830121 ] drag&drop changes ghost lines to real lines
  WinMerge: GhostTextView.cpp GhostTextView.h
 fix for context menu when the selection is empty
  WinMerge: MergeEditView.cpp
 fix in PopCursors for ptLastChange
  WinMerge: GhostTextView.cpp
 BUG: [ 830076 ] Context menu plugins change ghost lines to real lines
  WinMerge: GhostTextView.cpp GhostTextView.h MergeEditView.cpp
 BUG: [ 830072 ] "Unload script" does not work
  WinMerge: FileTransform.cpp MergeEditView.cpp

2003-10-25 Perry
 Fix spelling of "delimiter" in comments.
  WinMerge: MainFrm.cpp

2003-10-25 WinMerge experimental release 2.1.3.5 (cvs)

2003-10-25 Perry
 PATCH: [ 828633 ] Abort running scan
  WinMerge: DiffThread.cpp DiffThread.h DirDoc.cpp DirDoc.h
   DirScan.cpp DirScan.h
 PATCH: [ 823924 ] Update about box and copyright year
  WinMerge: Merge.rc

2003-10-24 Perry
 PATCH: [ 814832 ] Timing traces for loading file
  WinMerge: MergeDoc.cpp
 Add byte counts, and remove doubled line returns for load timing traces.
  WinMerge: MergeDoc.cpp
  common: UniMemFile.h
 PATCH: [ 826584 ] User-specified codepage conversions
  WinMerge: DiffContext.h DirView.cpp MainFrm.cpp MainFrm.h
   MergeDoc.cpp MergeDoc.h

2003-10-24 Kimmo
 PATCH: [ 828283 ] Open Read-only files as such
  WinMerge: DirView.cpp files.h files.cpp MainFrm.cpp

2003-10-24 Perry
 BUG: [ 829437 ] Can't compare two files with unicode filenames
  WinMerge: Diff.cpp
 Fix typo & mistake, both in comments.
  common: SuperComboBox.cpp
  WinMerge: DirScan.cpp

2003-10-24 Laoran
 PATCH: [ 829266 ] commandline help
  WinMerge: Merge.cpp Merge.rc resource.h
  Languages: *.rc resource.h
 enabling filters in unicode build
  WinMerge: Dirdoc.cpp
 BUG: [ 829453 ] dropping file in a SuperComboBox, Unicode build
  common: SuperComboBox.cpp
 Fix unifile code to initialize txtstats.nlosses (see comments in BUG [ 828924 ])
  common: UniFile.h

2003-10-23 Perry
 PATCH: [ 828619 ] Prevent closing dir doc whilst scanning
  WinMerge: DirDoc.cpp DirDoc.h
 PATCH: [ 829306 ] Fix unifile code to append last line
  WinMerge: MergeDoc.cpp
 Handle lib & includes for MakeResDll (& rearrange globals). 
 Bump version from 1.0.1.0 to 1.0.2.0.
  MakeResDll: MakeResDll.cpp MakeResDll.rc readme.txt
 Handle lib & includes for .NET and .NET 2003 in MakeResDll.
 Fix location of RegKey.h in MakeResDll.
  MakeResDll: MakeResDll.cpp MakeResDll.dsp readme.txt
 BUG: [ 827996 ] isspace uses (-> _istspace)
  WinMerge: MergeDoc.cpp
  WinMerge\editlib:
   asp.cpp basic.cpp batch.cpp ccrystaleditview.cpp ccrystaltextview2.cpp
   cplusplus.cpp dcl.cpp fortran.cpp html.cpp is.cpp java.cpp lisp.cpp
   pascal.cpp perl.cpp php.cpp python.cpp rexx.cpp rsrc.cpp sgml.cpp
   sh.cpp siod.cpp sql.cpp tcl.cpp tex.cpp

2003-10-23 Kimmo
 BUG: [ 815147 ] Scrollwheel scrolls too far at end of file
  editlib: ccrystaltextview.cpp
 BUG: [ 826834 ] Text selection startpoint not updated when navigating diffs
  WinMerge: MergeEditView.cpp

2003-10-23 Laoran
 PATCH: [ 827615 ] Speed up display of large differences - attachment
  Submitted by Hern Chen
  WinMerge: GhostTextBuffer.cpp MergeDoc.cpp MergeDoc.h
  editlib: ccrystaltextbuffer.cpp ccrystaltextbuffer.h
 BUG: [ 828802 ] plugins interface problem
  WinMerge: Plugins.cpp
 recovering Czech and Slovak custom characters (they were changed to '?')
  Languages: Czech.rc Slovak.rc

2003-10-23 Perry
 PATCH: [ 826250 ] New UniMemFile class for memory-mapping
  WinMerge: Merge.dsp MergeDoc.cpp
  common: unicoder.cpp unicoder.h UniFile.cpp UniFile.h

2003-10-22 Perry
 Fix compile for MakeResDll
  common: coretools.cpp
 Ran "cvs admin -kb *.bmp *.ico" to fix incorrectly typed resources
  Languages: Czech/res Dutch/res French/res German/res 
   Italian/res Spanish/res
 Fix MakeResDll for MSVC.NET & MSVC.NET 2003.
 Bump MakeResDll version from 1.0.0.1 to 1.0.1.0.
  MakeResDll: MakeResDll.cpp MakeResDll.rc
 Add readme.txt to MakeResDll.
  MakeResDll: MakeResDll.dsp readme.txt
 Fix MakeResDll to not create keys not found.
  MakeResDll: MakeResDll.cpp

2003-10-22 WinMerge experimental release 2.1.3.4 (cvs)

2003-10-22 Kimmo
 BUG: [ 826969 ] Wrong Open-dialog icon
  Load WinMerge icon to Open-dialog too
  WinMerge: OpenDlg.cpp

2003-10-21 Perry
 Make icons and bitmaps in WinMerge\res directory cvs file type binary (cvs admin -kb)
 WinMerge: res\*.bmp res\*.ico

2003-10-21 Laoran
 BUG: [ 827064 ] Move "Window" menu after "Plugins" menu
  WinMerge : Merge.rc
  Languages: *.rc 
 Add a backslash in front of relative path used to test for filter (as in 2.0)
 Undo the previous change (it disturbed the display) and make a new one
  WinMerge: DirScan.cpp DirDoc.cpp
 PATCH: [ 826048 ] headerbar improvements
  WinMerge: ChildFrm.cpp ChildFrm.h DirDoc.cpp DirDoc.h DirFrame.cpp DirFrame.h EditorFilepathBar.cpp
   EditorFilepathBar.h FilepathEdit.cpp FilepathEdit.h MergeDoc.cpp MergeDoc.h MergeEditView.cpp

2003-10-21 Kimmo
 PATCH: [ 826149 ] Improved "Merge.ico" and "MergeDoc.ico"
  From Tim Gerundt
  WinMerge/res: Merge.ico MergeDoc.ico
  All Languages/res: Merge.ico MergeDoc.ico
 PATCH: [ 824987 ] Change binary-file detection (look only for zeros)
  WinMerge: files.cpp

2003-10-20 Kimmo
 BUG: [ 826645 ] Scrolling to diff does not move cursor to diff
  WinMerge: MergeEditView.cpp
 Fix Patch Generate dialog's comboboxes to really change files
  WinMerge: PatchDlg.h PatchDlg.cpp
 PATCH: [ 809320 ] Open files with external editor
  WinMerge: DirActions.cpp DirView.h DirView.cpp MainFrm.h MainFrm.cpp
   Merge.rc PropRegistry.h PropRegistry.cpp resource.h
  Languages: *.rc resource.h

2003-10-20 Laoran
 Changing unidiff.cpp for DiffThread.cpp as doxygen example
  WinMerge: readme-developers.html 
 'Comparison Summary' not updated after saving (lost in plugin patch)
  WinMerge: MergeDoc.cpp
 Format for (eol type) must set the asterisk (sign for modified file)
  WinMerge: MergeEditView.cpp

2003-10-20 WinMerge experimental release 2.1.3.3 (patch demo)

2003-10-20 Laoran
 BUG: [ 826233 ] plugins or unicode bug
  WinMerge: UnicodeUtf8.cpp

2003-10-19 Perry
 Add writeup for MakePatchDirs to readme-developers info. Add section on 
  minor patches and bugfixes. Also make minor grammar fixes. Also update 
 language count (to 14).
  WinMerge: readme-developers.html readme-developers-MakePatchDirs.html

2003-10-19 Laoran
 Rearranging strings in .rc (all languages) by growing identifier values
  Languages : *.rc
  WinMerge: Merge.rc
 Rearranging strings in English .rc
  WinMerge: Merge.rc

2003-10-19 Perry
 Added Catalan translation (pre-plugin version)
  Languages: BuildAll.bat Catalan/*
 Update Catalan.rc for plugins (without stringtable rearrangement)
  Languages: Catalan/MergeCatalan.rc resource.h
 {Catalan} PATCH: [ 819188 ] Show column number in statusbar
  Languages: Catalan/MergeCatalan.rc
 PATCH: [ 818427 ] Fix hashing for mac lines and "ignore EOLs"
  WinMerge: IO.C MergeDoc.cpp UTIL.C
 Reapply PATCH: [ 818865 ] Exponentially grow text line array
  I lost it applying PATCH: [ 820364 ] Fix handling of non-ASCII 8-bit files
  from files.cpp cvs version 1.13 to 1.14.
  WinMerge: files.cpp
 PATCH: [ 822451 ] unicoder.cpp update
  common: unicoder.cpp unicoder.h

2003-10-18 Laoran
 Add a backslash in front of relative path used to test for filter (as in 2.0)
  WinMerge: DirScan.cpp

2003-10-17 Christian
 New readme-developers.html added
 PATCH: [ 825035 ] Updated German translation
  Languages: MergeGerman.rc

2003-10-17 WinMerge experimental release 2.1.3.1 (cvs)

2003-10-17 Kimmo
 RFE: [ 784695 ] Smarter scroll to diff
  WinMerge: MergeEditView.cpp

2003-10-17 Laoran
 fix for unicode conversion in unicode build
  WinMerge: UnicodeUtf8.cpp
 fix to delete useless temporary files
  WinMerge: FileTransform.cpp
 PATCH: [ 807263 ] Preprocessing patch
  common: dllproxy.c dllproxy.h lwdisp.c lwdisp.h
  Languages: resource.h *.rc
  WinMerge: DiffThread.cpp DiffWrapper.cpp DiffWrapper.h DirScan.cpp DirView.cpp DirView.h FileFilterMgr.cpp
   FileFilterMgr.h MainFrm.cpp MainFrm.h Merge.cpp Merge.dsp Merge.h Merge.rc MergeDoc.cpp MergeDoc.h
   MergeEditView.cpp OpenDlg.cpp OpenDlg.h resource.h
  WinMerge Add: FileTransform.cpp FileTransform.h Plugins.cpp Plugins.h SelectUnpackerDlg.cpp 
   SelectUnpackerDlg.h UnicodeUtf8.cpp UnicodeUtf8.h
  WinMerge Remove: unidiff.cpp unidiff.h
 PATCH: [ 823556 ] CF_UNICODETEXT format for OLE in unicode build
  WinMerge: FilepathEdit.cpp StdAfx.cpp StdAfx.h 
  editlib: ccrystaleditview.cpp ccrystaltextview2.cpp
 PATCH: [ 823074 ] DiffContext not updated
  WinMerge: DirDoc.cpp

2003-10-16 Kimmo
 Get first selected file from directory compare to
  Patch Generate dialog
  WinMerge: MainFrm.cpp
 BUG: [ 813557 ] Selection not visible after find
  editlib: cfindtextdlg.cpp
 Fix bug: Commandline params for diff were missing from
  generated patch-file in unicode build. (Missing conversion)
  WinMerge: DiffWrapper.cpp

2003-10-15 Kimmo
 PATCH: [ 823382 ] Save As should update filename
  WinMerge: MergeDoc.h MergeDoc.cpp
 BUG: [ 823099 ] RO status not fully implemented when lossy conversions
  WinMerge: ChildFrm.cpp
 Removed unneeded unlocalisable string from SelectFile() ("Directory selection")
  WinMerge: Merge.cpp
 Use SelectFile() from Merge.cpp in Patch Generate dialog
  also fixes bug Result-file asked with "Open" dialog
  WinMerge: PatchDlg.h PatchDlg.cpp
 Use ComboBox with history in Patch Generate -dialog
  WinMerge: Merge.rc PatchDlg.h PatchDlg.cpp
  Languages: *.rc
 Fix bug: File timestamps wrong in patch files
  WinMerge: DiffWrapper.cpp

2003-10-14 Laoran
 PATCH: [ 819123 ] Half-wait cursor during rescan (new try)
  WinMerge: DirDoc.cpp DirDoc.h MainFrm.cpp MainFrm.h WaitStatusCursor.cpp WaitStatusCursor.h
 BUG: correcting memory allocation for Unicode build in FilepathEdit.cpp
  editlib: FilepathEdit.cpp
 BUG: [ 822844 ] Drag & Drop crashes on Unicode build (correcting memory allocation)
  editlib: ccrystaltextview.cpp

2003-10-13 Laoran
 PATCH: [ 821821 ] Merge multiple diffs 
  WinMerge: Merge.rc MergeDoc.cpp MergeDoc.h MergeEditView.cpp MergeEditView.h resource.h
  editlib: ccrystaltextview.cpp ccrystaltextview.h

2003-10-13 Kimmo
 PATCH: [ 797502 ] Bugfix and new options for SourceSafe
  Submitted by Marvin Gouw
  WinMerge: DirActions.cpp MainFrm.h MainFrm.cpp Merge.rc resource.h
   VssPrompt.h VssPrompt.cpp
  Languages: resource.h *.rc

2003-10-12 Kimmo
 PATCH: [ 817079 ] New Copy to Left/Right icons
  WinMerge/res: Toolbar.bmp
  Languages/res: Toolbar.bmp

2003-10-11 WinMerge experimental release 2.1.3.0 (cvs)

2003-10-11 Perry
 PATCH: [ 821532 ] Fix CmpResults After Delete
  WinMerge: DiffContext.h DirActions.cpp
 PATCH: [ 817875 ] Resizable open dialog
  common: CSubclass.h CMoveConstraint.h CSubclass.cpp CMoveConstraint.cpp
  Languages: UpdateAll_resource_h.bat *.rc
  WinMerge: Merge.rc OpenDlg.cpp OpenDlg.h resource.h

2003-10-11 Laoran
 PATCH: [ 820742 ] use extension filter during Rescan in 2.1
  DiffContext.cpp DiffContext.h DirDoc.cpp DirDoc.h DirScan.cpp MainFrm.cpp

2003-10-10 Perry
 PATCH: [ 791487 ] Norwegian (bokmål) translation
 (Enlarge label in color dialog to avoid clipping text.)
  Languages: MergeNorwegian.rc
 PATCH: [ 820364 ] Fix handling of non-ASCII 8-bit files
  common: unicoder.cpp unicoder.h
  WinMerge: files.cpp
 PATCH: [ 821431 ] Update SideFlag When Copy file
  WinMerge: DirActions.cpp

2003-10-10 Laoran
 cosmetic change because I want to use BeSweet with source (delete one space !)
  editlib: ccrystaleditview.cpp
 PATCH: [ 818198 ] Colors for "ignore blank lines"
  Languages: *.rc (except not Brazilian or ChineseSimplified)

2003-10-09 Perry
 PATCH: [ 818865 ] Exponentially grow text line array
  WinMerge: files.cpp

2003-10-09 Laoran
 PATCH: [ 819219 ] Show only simple EOL char when no mixing EOL modes allowed
  WinMerge: MainFrm.cpp MainFrm.h MergeDoc.cpp ccrystaltextview.cpp ccrystaltextview.h

2003-10-09 Perry
 Fix duplicate copy of IDS_LINE_STATUS_INFO (to IDS_LINE_STATUS_INFO_EOL) in most
  language files. Fix missing id ID_POPUP_COPYFROMOTHERSIDE in German file
  Languages: *.rc (except not Brazilian or ChineseSimplified)

2003-10-08 Laoran
 BUG: [ 819793 ] Assert when saving after a Dirdoc session
  WinMerge: DirDoc.cpp
 PATCH: [ 818198 ] Colors for "ignore blank lines"
  WinMerge: MainFrm.cpp Merge.cpp Merge.h Merge.rc MergeDoc.cpp MergeEditView.cpp PropColors.cpp 
   PropColors.h resource.h

2003-10-08 Kimmo
 PATCH: [ 819188 ] Show column number in statusbar
  WinMerge: ChildFrm.h ChildFrm.cpp Merge.rc MergeEditStatus.h MergeEditView.cpp
  Languages: *.rc

2003-10-07 Perry
 PATCH: [ 817502 ] Fix "ignore blank lines"
  WinMerge: analyze.c Diff.cpp DIFF.H DiffWrapper.cpp DiffWrapper.h
   MergeDiffDetailView.cpp MergeDoc.cpp MergeEditView.cpp MergeEditView.h
   UTIL.C
 PATCH: [ 806277 ] Patch output fix (803299) alternate approach
  WinMerge: UTIL.C
 PATCH: 819633	Fix bug in my "fix ignore blank lines" patch
  WinMerge: analyze.c

2003-10-07 Kimmo
 BUG: [ 803330 ] DirView updated too many times after compare
  WinMerge: DiffThread.cpp DirDoc.h DirDoc.cpp
 RFE: [ 817859 ] Generate patch from modified files
  WinMerge: MainFrm.cpp
 Fix Refresh-crash I introduced when fixing BUG [ 803330 ]
  WinMerge: DirDoc.cpp
 Properly clear display before new dir compare (don't use Redisplay())
  WinMerge: DirDoc.cpp

2003-10-05 Kimmo
 Show text "Identical" on diffnumber panel when files are identical

2003-10-05 Laoran
 BUG: [ 817766 ] Dir compare speed drop after 2.0
  WinMerge: DiffContext.cpp

2003-10-05 Kimmo
 PATCH: [ 817269 ] Fix navigation in directory compare
  WinMerge: DirView.cpp
 PATCH: [ 817394 ] Implement Copy diff and advance to next
  WinMerge: Merge.rc MergeEditView.h MergeEditView.cpp resource.h
  WinMerge/res: Toolbar.bmp
  Languages: resource.h *.rc
  Languages/res: Toolbar.bmp

2003-10-04 Perry
 Update dll copy batch file for 4 new build targets
 Languages/CopyAll.bat

2003-10-04 Laoran
 Undo last changes related to PATCH: [ 813794 ] Fix "ignore blank lines"
 Back as of 2003-10-02 23:59

2003-10-04 Perry
 Restore trivial colors originally included in PATCH: [ 813794 ] Fix "ignore blank lines"
  WinMerge: MainFrm.cpp Merge.h MergeEditView.cpp PropColors.h

2003-10-03 Perry
 Bugfix for PATCH: [ 813794 ] Fix "ignore blank lines"
 (Prevent crashes on identical files dereferencing null script).
  WinMerge: analyze.c

2003-10-03 Laoran
 two minor changes for PATCH: [ 813794 ] Fix "ignore blank lines"
  WinMerge: MergeDiffDetailView.cpp MergeDoc.cpp

2003-10-03 Perry
 PATCH: [ 813794 ] Fix "ignore blank lines"
  WinMerge: analyze.c ColorButton.h Diff.cpp DIFF.H DiffWrapper.cpp 
   DiffWrapper.h MainFrm.cpp Merge.cpp Merge.h MergeDiffDetailView.cpp
   MergeDoc.cpp MergeEditView.cpp MergeEditView.h PropColors.cpp
   PropColors.h

2003-10-02 Kimmo
 PATCH: [ 815138 ] Commandline filedescriptors
  Usage: -dl "description" for left and -dr "description" for right
  WinMerge: MainFrm.h Merge.h Merge.cpp MergeDoc.cpp

2003-10-02 Perry
 PATCH: [ 815483 ] Fix file stat structure (lost in dirscan patch)
  WinMerge: Diff.cpp DirScan.cpp
 PATCH: [ 815523 ] Fix utf8len_of_string
  common: unicoder.cpp

2003-09-30 Perry
 PATCH: [ 814829 ] Fix handle bad utf8
  WinMerge: files.cpp
 PATCH: [ 813847 ] Fix Unicode double copy
  WinMerge: MergeDoc.cpp

2003-09-30 Kimmo
 PATCH: [ 814830 ] Prevent rescan of scanning dirdoc
  Submitted by Perry
  WinMerge: DirDoc.cpp
 PATCH: [ 812581 ] Prevent second scan during first scan
  Submitted by Perry
  WinMerge: MainFrm.cpp

2003-09-29 Kimmo
 PATCH: [ 814098 ] More edit-events for delayed rescan
  WinMerge: MergeEditView.cpp

2003-09-29 Laoran
 BUG In DrawSingleLine, Draw EOL only if there is an EOL in the line
  WinMerge: ccrystaltextview.cpp ccrystaltextview.h
 BUG [ 814214 ] ASSERT failure with diff at eof
  WinMerge: files.cpp

2003-09-27 Laoran
 delete NDEBUG from ReleaseMinsize preprocessor definitions
  WinMerge: Merge.dsp

2003-09-27 Perry
 PATCH: [ 809149 ] Handle Unicode files
  common: unicoder.cpp unicoder.h
  WinMerge: DiffContext.cpp DiffContext.h DirScan.cpp FileFilterMgr.cpp files.cpp
   files.h GhostTextBuffer.cpp GhostTextBuffer.cpp.bak GhostTextBuffer.h Merge.dsp
   MergeDoc.cpp MergeDoc.h unidiff.cpp unidiff.h
  WinMerge/editlib: ccrystaltextbuffer.cpp ccrystaltextbuffer.h ccrystaltextview2.cpp

2003-09-26 Perry
 PATCH: [ 811039 ] Identify filters by path to solve problems with reused names
  common: coretools.cpp coretools.h
  WinMerge: FileFilterMgr.cpp FileFilterMgr.h MainFrm.cpp Merge.cpp Merge.h
   PropFilter.cpp PropFilter.h

2003-09-26 WinMerge beta 2.1.2.0 released

2003-09-26 WinMerge experimental release 2.1.1.9 (Unicode handling patch)

2003-09-26 Perry
 PATCH: [ 812820 ] Fix regexp for Unicode
  common: RegExp.cpp RegExp.h
 PATCH: [ 812851 ] Another Unicode fix for PutToClipboard
  WinMerge/editlib/ccrystaltextview2.cpp
 Fix GetFromClipboard for Unicode (variation on PATCH: [ 812853 ])
  WinMerge/editlib/ccrystaltextview2.cpp

2003-09-26 Laoran
 PATCH: [ 811371 ] Replace lines when pasting text to ghost lines
  WinMerge: GhostTextBuffer.cpp GhostTextBuffer.h
  editlib: ccrystaltextbuffer.cpp

2003-09-25 Perry
 fix BUG: [ 812547 ] copy to clipboard crash in WinMergeU.exe
  WinMerge/editlib: ccrystaltextview2.cpp

2003-09-25 Kimmo
 PATCH: [ 811307 ] Open dirview column dialog from menu
  WinMerge: DirView.h DirView.cpp Merge.rc resource.h
  Languages: resource.h *.rc

2003-09-24 Perry
 fix BUG: [ 811750 ] typo in DirScan.cpp
  WinMerge: DirScan.cpp
 fix BUG: [ 811747 ] VersionInfo clipping in Unicode ?
  common: version.cpp

2003-09-24 Kimmo
 BUG: [ 811748 ] Brazilian RC compile failure
  Languages/Brasilian: MergeBrasilian.rc
 Patch Generate improvements and fixes
  WinMerge: DiffWrapper.cpp PatchDlg.h PatchDlg.cpp PatchTool.h
   PatchTool.cpp

2003-09-23 Kimmo
 Don't show [0 files selected] in Patch Generate -dialog
  WinMerge: PatchDlg.cpp

2003-09-23 Perry
 PATCH: [ 810998 ] Unicode fix for ghosttextbuffer
  WinMerge: GhostTextBuffer.cpp

2003-09-22 Kimmo
 PATCH: [ 809902 ] Show if file is changed
  WinMerge: MergeDoc.h MergeDoc.cpp MergeEditView.cpp
 PATCH: [ 810066 ] Add Save Left (as...) and Save Right (as...)
  WinMerge: Merge.rc MergeDoc.h MergeDoc.cpp MergeEditView.h
   MergeEditView.cpp resource.h
  Languages: resource.h *.rc

2003-09-21 Kimmo
 BUG: [ 807766 ] File filter rules not applied to unique items (2.1)
  WinMerge: DirScan.cpp
 Optimise CFilePathEdit to not change text when same text given again
  By Laoran (PATCH "#809902 Show if file is changed" comments)
  WinMerge: FilePathEdit.cpp
 Remove unused function GetPathOnly() from DirDoc.cpp -
  its really defined in coretools.cpp and
  WinMerge: DirDoc.cpp

2003-09-20 WinMerge experimental release 2.1.1.7 (Unicode handling patch)

2003-09-20 Kimmo
 Make directory compare Window to remember maximized state
  WinMerge: DirFrame.h DirFrame.cpp

2003-09-20 Laoran
 Actualizing the GnuC filter *.o -> \.o
  filter: Merge_GnuC_loose.flt

2003-09-19 Laoran
 PATCH: [ 809253 ] New release minsize mode
  WinMerge: Merge.dsp
 Modifications in patch-dialog to compile in Unicode
  WinMerge: PatchDlg.cpp 

2003-09-19 Kimmo
 Select compared files when opening Generate patch-dialog
  WinMerge: MainFrm.cpp PatchDlg.cpp 

2003-09-18 Kimmo
 PATCH: [ 803904 ] Create patches
  WinMerge: DiffWrapper.h DiffWrapper.cpp MainFrm.h MainFrm.cpp Merge.dsp
   Merge.rc resource.h util.cpp
  WinMerge new files: PatchDlg.h PatchDlg.cpp PatchTool.h PatchTool.cpp
  Languages: *.rc resource.h
 BUG: [ 800256 ] Hilight line diff does not scroll to difference
  WinMerge: MergeDoc.cpp

2003-09-17 WinMerge experimental release 2.1.1.6 (Preprocessing patch)

2003-09-16 Perry
 fix BUG: [ 807420 ] assert when saving a modified doc
  WinMerge: DiffContext.cpp

2003-09-16 Laoran
 BUG: [ 807402 ] ListCopy problem for Unix Files
  WinMerge: MergeDoc.cpp

2003-09-16 Kimmo
 PATCH: [ 781013 ] 256 Color icon
  Applying to languages
  Languages/res: Merge.ico
 PATCH: [ 806785 ] Add menuitem to show/hide skipped files
  WinMerge: MainFrm.h MainFrm.cpp Merge.rc resource.h
  Languages: *.rc resource.h

2003-09-15 Kimmo
 BUG: [ 806552 ] DirView icons missing?
  Adding icons for skipped files and folders
  WinMerge: DirView.cpp Merge.rc resource.h
  WinMerge/res: new files fileskip.bmp folderskip.bmp
  Languages: resource.h *.rc new files /res fileskip.bmp folderskip.bmp
 Don't color skipped items filenames in directory compare
  WinMerge: DirView.cpp
 PATCH: [ 804493 ] Read filter files from users profile directory
  NOTE: Filters are read from %profiledir%/Application Data/WinMerge/Filters
  common: coretools.h coretools.cpp
  WinMerge: FileFilterMgr.cpp Merge.cpp

2003-09-14 Perry
 PATCH: [ 805875 ] Switch to diffcode flags
  WinMerge: DiffContext.cpp DiffContext.h DirActions.cpp DirDoc.cpp DirDoc.h
   DirScan.cpp DirView.cpp DirView.h DirViewColHandler.cpp DirViewColItems.cpp
   MainFrm.cpp MainFrm.h
 PATCH: [ 805427 ] Delete unused CStdioFile variable in PrimeTextBuffers
  WinMerge: MergeDoc.cpp
 PATCH: [ 805888 ] Unicode compile of WinMerge
  common: coretools.cpp dllproxy.c LanguageSelect.cpp LanguageSelect.h
   LogFile.cpp lwdisp.c lwdisp.h RegExp.cpp RegExp.h StatLink.cpp
   SuperComboBox.cpp SuperComboBox.h
  WinMerge: ChildFrm.cpp Diff.cpp DiffWrapper.cpp DirDoc.cpp DIRENT.C DirFrame.cpp
   DirView.cpp DirViewColHandler.cpp EditorFilepathBar.cpp FilepathEdit.cpp
   MainFrm.cpp MainFrm.h Merge.cpp Merge.dsp MergeDoc.cpp MergeEditView.cpp
   OpenDlg.cpp paths.cpp PropColors.cpp

2003-09-13 Perry
 Widen label IDC_PRIVATEBUILD on about box.
  WinMerge: Merge.rc
  Languages: *.rc
 PATCH: [ 802120 ] Fix for VC.NET
  WinMerge: DiffContext.cpp DiffContext.h DirScan.cpp DirView.cpp
   DirView.h DirViewColItems.cpp EditorFilepathBar.cpp MainFrm.cpp 
   MainFrm.h MergeEditView.cpp
  editlib: ccrystaleditview.cpp ccrystaleditview.h

2003-09-13 WinMerge experimental release 2.1.1.5 (patch demos)
2003-09-13 WinMerge experimental release 2.1.1.4 (cvs)

2003-09-13 Laoran
 effects of patch [ 797821 ]
  WinMerge: Merge.rc resource.h 
  Languages: resource.h *.rc

2003-09-11 Laoran
 PATCH: [ 804408 ] Fixes to help compile release version
  WinMerge: Merge.dsp 
 PATCH: [ 797821 ] GUI for the file EOL option
  WinMerge: ChildFrm.cpp GhostTextBuffer.cpp MainFrm.cpp MainFrm.h Merge.rc MergeDoc.cpp 
   MergeDoc.h MergeEditView.cpp MergeEditView.h PropGeneral.cpp PropGeneral.h resource.h
  editlib: ccrystaltextbuffer.cpp ccrystaltextbuffer.h

2003-09-10 Kimmo
 PATCH: [ 802762 ] Optimise printc() in diffutils
  WinMerge: cmp.c

2003-09-09 Laoran
 FIX ASSERT in GetAllViews during a merge session
  WinMerge: MainFrm.cpp MainFrm.h
 
2003-09-08 Laoran
 PATCH: [ 799633 ] context menu with scriptlet, from Jochen Tucht
  WinMerge: Merge.dsp MergeEditView.cpp MergeEditView.h
  common new files: dllproxy.c dllproxy.h lwdisp.c lwdisp.h
 PATCH: [ 802373 ] undo/redo/onEditOperation when RO (2.1)
  WinMerge: MergeEditView.cpp
 PATCH: [ 802359 ] display in diff pane when no diff (2.1)
  WinMerge: MergeDiffDetailView.cpp

2003-09-08 Kimmo
 PATCH: [ 802096 ] Fix diff navigation when first/last diff selected
  WinMerge: MergeEditView.cpp

2003-09-08 Laoran
 PATCH: [ 802066 ] diff pane when OnDisplayDiff(-1) (2.1)
  WinMerge: MergeDiffDetailView.cpp

2003-09-07 Perry
 PATCH: [ 802132 ] Resort dir columns after redisplay
  WinMerge: DirView.h DirView.cpp DirDoc.cpp
 Fix bug [ 798404 ] Reordering dirview columns does not preserve sorting column
  WinMerge: DirView.cpp

2003-09-07 Laoran
 PATCH: [ 799464 ] preserve positions during Rescan (2.1)
  WinMerge: GhostTextBuffer.cpp GhostTextView.cpp GhostTextView.h MergeDoc.cpp
  editlib: ccrystaleditview.cpp ccrystaleditview.h ccrystaltextbuffer.cpp ccrystaltextbuffer.h
   ccrystaltextview.cpp ccrystaltextview.h

2003-09-07 Kimmo
 BUG: [ 802063 ] files_analyzeFile bug for UNIX file (2.1)
  WinMerge: files.cpp

2003-09-07 WinMerge experimental release 2.1.1.3 (cvs)

2003-09-07 Kimmo
 PATCH: [ 800420 ] Fix recursive compare and cleanup recursive usage
  WinMerge: DiffThread.h DiffThread.cpp DirDoc.h DirDoc.cpp MainFrm.cpp
 Use constant values for whitespace compare setting
  WinMerge: DiffWrapper.h DiffWrapper.cpp
 Change default diff colors to more eye friendly ones (not so bright ones)
  WinMerge: Merge.cpp
 Tweak more yellow diff color
  WinMerge: Merge.cpp

2003-09-06 Perry
 PATCH: [ 795082 ] Display Private Build version in about box
 WinMerge: Merge.cpp Merge.rc resource.h
 Languages: resource.h *.rc

2003-09-06 Kimmo
 PATCH: [ 793900 ] Editable file filters
 Submitted by Perry
 WinMerge: FileFilterMgr.h FileFilterMgr.cpp Merge.h Merge.cpp Merge.rc
  PropFilter.h PropFilter.cpp Resource.h
 Languages: resource.h *.rc

2003-09-06 Perry
 PATCH: [ 800175 ] skip custom draw if comctl32.dll < 4.71
  WinMerge: DirView.cpp

2003-09-07 WinMerge experimental release 2.1.1.2 (cvs)

2003-09-02 Laoran
 PATCH: [ 797957 ] correct line number for last real line (in statusbar)
  WinMerge: MergeEditView.cpp

2003-09-01 Laoran
 PATCH: [ 798272 ] save settings for the dockable diff bar
  WinMerge: ChildFrm.cpp ChildFrm.h MainFrm.cpp

2003-09-01 Kimmo
 Improve 256-color icon
  WinMerge/res: Merge.ico

2003-08-31 Laoran
 correction for the patch [797741] in CMergeDoc::CDiffTextBuffer::LoadFromFile
  WinMerge: MergeDoc.cpp

2003-08-31 Kimmo
 PATCH: [ 797741 ] Faster file loading
  WinMerge: files.h files.cpp MergeDoc.h MergeDoc.cpp
 PATCH: [ 784302 ] Implement Read-only for files and dirs
  UI changes
  WinMerge: ChildFrm.cpp DirFrame.cpp DirView.h DirView.cpp Merge.rc
   MergeEditView.h MergeEditView.cpp Resource.h
  Languages: resource.h *.rc
 Fix ASSERT when opening files for merging.
  Submitted by laoran (PATCH [ 784302 ] comments)
  WinMerge: ChildFrm.cpp

2003-08-30 Kimmo
 PATCH: [ 797797 ] OnOptions must exchange diffOptions with the registry
  Submitted by Laoran.
  WinMerge: MainFrm.cpp

2003-08-30 Laoran
 PATCH: [ 784109 ] layout with two new horizontal panels
  common new files: sizecbar.cpp sizecbar.h sizecbarg.cpp sizecbarg.h 
  common: SplitterWndEx.cpp SplitterWndEx.h
  WinMerge: ChildFrm.cpp ChildFrm.h MainFrm.cpp Merge.dsp Merge.rc MergeDoc.cpp
   MergeDoc.h MergeEditView.cpp resource.h
  Winmerge new files: DiffViewBar.cpp DiffViewBar.h MergeDiffDetailView.cpp MergeDiffDetailView.h
  editlib: ccrystaltextview.h

2003-08-29 Perry
 PATCH: [ 779818 ] Fix number display to be locale & user-pref aware
  (extended to handle larger numbers than 32 bit, for size)
 WinMerge: DirView.cpp DirViewColItems.cpp locality.cpp locality.h Merge.dsp

2003-08-29 Kimmo
 PATCH: [ 796756 ] Diff API
  WinMerge: DiffWrapper.h DiffWrapper.cpp DirDoc.h DirDoc.cpp DirView.cpp
   MainFrm.h MainFrm.cpp MergeDoc.h MergeDoc.cpp 

2003-08-28 Laoran
 PATCH: [ 794345 ] make visible the changed text when undo/redo
  WinMerge: MergeEditView.cpp

2003-08-27 Perry
 i18n effects of patch#781529: new popup menu, new dialog, alter existing col header strings
  Languages: Czech, Danish, Dutch
 i18n effects of patch#781529: new popup menu, new dialog, alter existing col header strings
  Languages: French, German, Italian, Korean, Norwegian, Slovak, Spanish

2003-08-27 Kimmo
 PATCH: [ 784302 ] Implement Read-only for files and dirs
  Only background changes, no UI changes
  WinMerge: DirDoc.h DirDoc.cpp DirView.cpp MainFrm.h MainFrm.cpp
   MergeDoc.h MergeDoc.cpp MergeEditView.h MergeEditView.cpp

2003-08-27 Perry
 Add new colhdrs from patch#781529 to Brazilian.
 Reorder ChineseSimplified string tables to match English, and add missing entries.
 Reorder ChineseTraditional string tables to match English, and add missing entries.
  Languages: Brazilian, ChineseSimplified, ChineseTraditional

2003-08-26 Perry
 Implement fix for BUG#794514: 'Comparison Summary' not updated properly
  WinMerge: DirDoc.cpp
 i18n effects of patch#781529: new popup menu, new dialog, alter existing col header strings
  Languages: Brazilian, ChineseSimplified, ChineseTraditional
  
2003-08-25 Laoran
 [ 783507 ] normalize replaceSelection and ListCopy
  WinMerge: GhostTextBuffer.cpp GhostTextBuffer.h MergeDoc.cpp MergeDoc.h
  editlib : ccrystaleditview.cpp ccrystaltextbuffer.cpp ccrystaltextbuffer.h

2003-08-25 Laoran
 [ 794081 ] Error when dropping a column out of WinMerge
  WinMerge: DirView.cpp
  
2003-08-24 Perry
 Fix paths_GetLongPath to handle trailing . or ..
  (BUGFIX: [ 792074 ] Directory mnemonics . and .. are not honored properly)
  WinMerge: paths.cpp
 Fix DIFFITEM::getLeftFilepath & DIFFITEM::getRightFilepath
  (BUGFIX: [ 794319 ] Assert when save after merging)
  WinMerge: DiffContext.cpp

2003-08-24 Kimmo
 PATCH: [ 792668 ] Move file load code to CMergeDoc
  WinMerge: MainFrm.cpp MergeDoc.h MergeDoc.cpp

2003-08-23 Perry
 PATCH: [ 791487 ] Norwegian (bokmål) translation
  Corrections#2 Avbrytt->Avbryt and #3 Opdater->Oppdater
  Languages\Norwegian: MergeNorwegian.rc
  Correction#0 Bokmal->Bokmål
  WinMerge\Merge.rc
  Correction#1 værktøylinje(n)->verktøylinje(n)
  Languages\Norwegian: MergeNorwegian.rc
 PATCH: [ 781529 ] Make column info array, and add new columns
  common: version.cpp version.h
  WinMerge: DiffContext.cpp DiffContext.h DirActions.cpp DirDoc.cpp DirDoc.h
   DirScan.cpp DirView.cpp DirView.h MainFrm.cpp Merge.dsp Merge.rc
   MergeDoc.cpp resource.h
  WinMerge new files: DirColsDlg.cpp DirColsDlg.h DirViewColHandler.cpp
   DirViewColItems.cpp DirViewColItems.h
 Bugfix: CDiffContext::AddDiff was not setting diff context subdir from argument.
  WinMerge:: DiffContext.cpp
 Bugfix: Don't prefix subdirectories with dot in ColPathGet.
  WinMerge: DirViewColItems.cpp
 Bugfix: FILE_SKIP & DIR_SKIP are being labelled as errors
  WinMerge: DirViewColItems.cpp Merge.rc resource.h
 Bugfix: Fix CDirView::GetItemFileNames to get path properly (as before 2003-08-07).
  WinMerge: DirActions.cpp
 Bugfix: Remove several memsets of structures whose size varies with common control versions
  WinMerge: DirView.cpp DirViewColHandler.cpp

2003-08-22 Kimmo
 PATCH: [ 787495 ] Create diffutils class (CDiffWrapper)
  WinMerge: Merge.dsp MergeDoc.h MergeDoc.cpp
   Added: DiffWrapper.h DiffWrapper.cpp
 Adding licence text and comments
  WinMerge: files.h files.cpp
 Adding licence text and comments
  WinMerge: DiffThread.h DiffThread.cpp DiffWrapper.h

2003-08-20 Perry
 PATCH: [ 791924 ] Fix sorting when columns are reordered
  WinMerge: DirView.cpp
 Fix color dialog in Norwegian (Earlier today I forgot to move down the previous labels).
  Languages\Norwegian: Norwegian/MergeNorwegian.rc
 Updated color property page in Korean (patch#768740).
  Languages\Korean: MergeKorean.rc

2003-08-20 Kimmo
 PATCH: [ 791263 ] Remove CStringEx from OpenDlg (and from WinMerge?)
  WinMerge: Merge.dsp OpenDlg.cpp
  Common: removed StringEx.h StringEx.cpp

2003-08-20 Perry
 Added Norwegian to batch build file
  Languages: BuildAll.bat
 Updated two property pages in Norwegian
  IDD_PROPPAGE_REGISTRY (patch#789504) and IDD_PROPPAGE_COLORS (patch#768740).
  Languages\Norwegian: resource.h Norwegian/MergeNorwegian.rc
 Updated color property page in Brazilian (patch#768740).
  Languages\Brazilian: MergeBrazilian.rc
 Updated color property page in Simplified Chinese (patch#768740 not quite complete).
  Languages\ChineseSimplified: MergeChineseSimplified.rc
 Updated color property page in ChineseTraditional (patch#768740).
  Languages\ChineseTraditional: MergeChineseTraditional.rc

2003-08-20 Christian
 PATCH: [ 791487 ] Norwegian (bokmål) translation
  Languages: Norwegian added

2003-08-19 WinMerge beta 2.1.0.1 released

2003-08-19 Perry
 Change versions in Merge.rc to "000.000.000.000" as recommended by stampver
  (http://www.elphin.com/products/stampver.html).
  WinMerge: Merge.rc

2003-08-17 Kimmo
 Updated comments for ShellExtension: added comment about used registry values etc
  ShellExtension:
   ShellExtension.cpp WinMergeShell.h WinMergeShell.cpp
 Removed two generated files from CVS (Christian spotted those)
  ShellExtension: 
   ShellExtension.h DllData.cp

2003-08-16 Kimmo
 PATCH: [ 789204 ] Color dir compare based on left/right newer
  WinMerge: DirView.h DirView.cpp
 PATCH: [ 789504 ] Remove unneeded old dir shell assoc
  WinMerge: Merge.rc PropRegistry.h PropRegistry.cpp resource.h
  Languages: resource.h *.rc

2003-08-12 Christian
 PATCH: [ 786618 ] Brazilian Portuguese translation
  Languages: BuilAll.bat and resurrected directory Brazilian
 PATCH: [ 782820 ] Updated Traditional Chinese for 2.0
  Languages\ChineseTraditional: MergeChineseTraditional.rc
 PATCH: [ 782824 ] Add languages to installer
  InnoSetup: WinMerge.iss Info.txt

2003-08-12 Kimmo
 PATCH: [ 785288 ] Overwrite Read-Only file
  WinMerge: MainFrm.h MainFrm.cpp Merge.rc MergeDoc.cpp
  Languages: *.rc

2003-08-07 Kimmo
 PATCH: [ 781013 ] 256 Color icon
  editlib/res: Merge.ico
 PATCH: [ 783593 ] Add Merge-menu
  WinMerge: Merge.rc
  Languages: *.rc

2003-08-07 Perry
 Fix GetItemFileNames to not pull its data from listview (b/c columns may be reordered).
 (Various const fixes needed.)
 WinMerge: DiffContext.cpp DiffContext.h DirActions.cpp DirView.cpp DirView.h

2003-08-05 Laoran
 PATCH: [ 781582 ] Timer for FlushAndRescan
  WinMerge: MergeDoc.cpp MergeDoc.h MergeEditView.cpp MergeEditView.h

2003-08-04 Laoran
 PATCH: [ 782866 ] round off in CMergeDoc::RescanIfNeeded
  WinMerge: MergeDoc.cpp

2003-08-04 Kimmo
 PATCH: [ 780359 ] Open identical files
  WinMerge: DirView.cpp MainFrm.cpp

2003-08-03 WinMerge release 2.0

2003-07-31 Laoran
 PATCH: [ 780827 ] Tooltips for DirView
  Submitted by Kimmo
  WinMerge: DirView.cpp DirView.h

2003-07-31 Laoran
 PATCH: [ 780508 ] Convert Line Length array to CArray
  WinMerge: ccrystaltextview.cpp ccrystaltextview.h 

2003-07-31 Laoran
 PATCH: [ 780067 ] SUndoRecord::SetText must free previous text
  WinMerge: ccrystaltextbuffer.cpp ccrystaltextbuffer.h 

2003-07-30 Laoran
 PATCH: [ 779470 ] disable unwanted options in PropGeneral
  WinMerge: PropGeneral.cpp PropGeneral.h 

2003-07-30 Laoran
 PATCH: [ 777871 ] New headerbar for the file paths 
  WinMerge: ChildFrm.cpp ChildFrm.h dllver.cpp dllver.h Merge.dsp Merge.rc resource.h
  WinMerge new files : EditorFilepathBar.cpp EditorFilepathBar.h FilepathEdit.cpp FilepathEdit.h

2003-07-30 Perry
 PATCH: [ 778379 ] Drag&drop reordering columns
  WinMerge: DirDoc.cpp DirView.cpp DirView.h Merge.dsp dllver.cpp dllver.h
 Annotate UpdateResources methods
  WinMerge: DirDoc.cpp MainFrm.cpp MergeEditView.cpp

2003-07-29 Perry
 PATCH: [ 779224 ] Fix sort bug in right date
  WinMerge: DirView.cpp
 bugfix for PATCH: [ 765225]
  editlib: ccrystaltextview.cpp ccrystaltextview.h

2003-07-29 Kimmo
 PATCH: [ 779172 ] Saving fix for 2.0 and 2.1
  WinMerge: MergeDoc.cpp
 PATCH: [ 779005 ] No temp files when saving local temp files
  WinMerge: MergeDoc.cpp
 Fix Register.bat comments. Batch files use ":" for comments...
  ShellExtension: Register.bat
  
2003-07-29 Perry
 Some fixes to MergeChineseTraditional.rc from Koko
  Languages\ChineseTraditional\MergeChineseTraditional.rc

2003-07-28 Perry
 PATCH: [ 778234 ] Fix DirScan ASSERT when run out of right list
  WinMerge: DirScan.cpp
 PATCH: [ 765225 ] Convert parse cookie array to be CArray
  editlib: ccrystaltextview.cpp ccrystaltextview.h

2003-07-27 Kimmo
 PATCH: [ 778144 ] Use CopyFile instead of DeleteFile() and MoveFile()
  WinMerge: MainFrm.cpp
 PATCH: [ 778384 ] Sync OnDropFiles() from MainFrame to SuperComboBox
  common: SuperComboBox.cpp
 PATCH: [ 778403 ] Fix accel keys in lang files
  Languages: ChineseSimplified, ChineseTraditional 

2003-07-27 Perry
 PATCH: [ 778290 ] Typo in comment
  WinMerge: MergeDoc.cpp

2003-07-26 Kimmo
 BUG: [ 778166 ] Shell integration missing quotes (2.1)
  ShellExtension: WinMergeShell.cpp

2003-07-25 Laoran
 [ 776114 ] french keyboard shortcut
  Languages: french.rc

2003-07-25 Kimmo
 PATCH: [ 773296 ] Add shell integration
  Added new module ShellExtension
  WinMerge: Merge.rc PropRegistry.h PropRegistry.cpp resource.h
  Languages: all *.rc and resource.h files

2003-07-24 Kimmo
 PATCH: [ 776106 ] Fix for TEMP dir error
  Submitted by Laoran
  WinMerge: MergeDoc.cpp

2003-07-24 Perry
 PATCH: [ 766287 ] Use our own dir traversal (instead of diff's)
  WinMerge: Diff.cpp DiffContext.h DiffThread.cpp DirDoc.cpp Merge.dsp 
   DirScan.h DirScan.cpp
 PATCH: [ 773981 ] Modify language build process to eliminate msgboxes
  (also added a version resource to MakeResDll, as it lacked one)
  Languages: BuildAll.bat BuildDll.bat
  MakeResDll: MakeResDll.cpp MakeResDll.rc
 Added F4 accelerator to all languages besides English
  Languages: all *.rc files

2003-07-24 Kimmo
 PATCH: [ 776525 ] Close doc after failed load
  WinMerge: MainFrm.cpp

2003-07-23 Perry
 PATCH: [ 776046 ] Fixes for <Highlight Line Diff> menu item
  WinMerge: Merge.rc MergeEditView.cpp
  Languages: All Merge*.rc files and resource.h files
 PATCH: [ 771666 ] Fix for English descriptions of Chinese
  common: LanguageSelect.rc
  WinMerge: Merge.rc

2003-07-21 Perry
 Added ChineseTraditional, ChineseSimplified, Korean, translations.
 PATCH: [ 775000 ] Improve const correctness in crystaltextbuffer
  editlib: crystaltextbuffer.cpp crystaltextbuffer.h

2003-07-18 Perry
 PATCH: [ 767147 ] Don't clear existing dirdoc until user saves
  WinMerge: DirDoc.cpp
 PATCH: [ 763616 ] New menu item: Highlight line diff
  WinMerge: Merge.rc MergeDoc.cpp MergeDoc.h MergeEditView.cpp MergeEditView.h resource.h
  editlib: ccrystaltextbuffer.cpp ccrystaltextbuffer.h ccrystaltextview.cpp ccrystaltextview.h

2003-07-18 Kimmo
 PATCH: [ 772843 ] Remember custom colors
  WinMerge: PropColors.h PropColors.cpp

2003-07-18 Laoran
 PATCH : [ 771587 ] Undo/Redo functions 
  WinMerge: MainFrm.cpp MergeDoc.cpp MergeDoc.h MergeEditView.cpp
  editlib: ccrystaleditview.cpp ccrystaleditview.h ccrystaltextbuffer.cpp ccrystaltextbuffer.h 

2003-07-17 Laoran
 PATCH : [ 772908 ] ResetView must reset m_nTopSubLine
  editlib: ccrystaltextview.cpp

2003-07-17 Laoran
 PATCH : [ 769535 ] Preserve cursor during Rescan
  WinMerge: MergeDoc.cpp 
  editlib: ccrystaltextbuffer.cpp ccrystaltextbuffer.h ccrystaltextview.cpp ccrystaltextview.h

2003-07-17 Laoran
 PATCH: [ 767878 ] Better handling of ghost flag and EOL
  WinMerge: MergeDoc.cpp MergeEditView.cpp
  editlib: ccrystaleditview.cpp ccrystaltextbuffer.cpp ccrystaltextbuffer.h

2003-07-17 Kimmo
 PATCH: [ 772228 ] Quick bin check for 2.0
  WinMerge: files.h files.cpp MainFrm.cpp MergeDoc.cpp

2003-07-16 Laoran
 PATCH: [ 765926 ] Merge empty last line to the last diffrange (change)
 new version
  WinMerge: MergeDoc.cpp

2003-07-16 Laoran
 PATCH: [ 771525 ] Updating cursor in ListCopy
  WinMerge: MergeDoc.cpp MergeDoc.h
  editlib: ccrystaltextbuffer.cpp ccrystaltextbuffer.h

2003-07-16 Laoran
 PATCH: [ 771503 ] correct UpdateView context in internalInsertText
  editlib: ccrystaltextbuffer.cpp

2003-07-16 Kimmo
 PATCH: [ 768740 ] Additional color options
  Submitted by Gilbert Wellisch
  WinMerge: MainFrm.cpp Merge.cpp Merge.h Merge.rc MergeEditView.cpp
   PropColors.cpp PropColors.h resource.h
  Updated other languages

2003-07-15 Laoran
 PATCH: [ 771625 ] regression for patch [765399]
  WinMerge: MergeEditView.cpp 

2003-07-15 Perry
 PATCH: [ 770361 ] Slovak translation
  Languages: New Slovak subdirectory

2003-07-12 Laoran
 PATCH: [ 765399 ] Careful view initialization in PrimeListWithFile
  WinMerge: MergeDoc.cpp MergeEditView.cpp 
  editlib: ccrystaltextview.cpp

2003-07-12 Laoran
 PATCH: [ 769122 ] Keep top screen line within the shorter panel
  WinMerge: MergeEditView.cpp 

2003-07-11 Perry
 PATCH: [ 765150 ] Fix char count in edit view bottom status line
  WinMerge: ChildFrm.cpp ChildFrm.h Merge.rc MergeEditStatus.h 
   MergeEditView.cpp resource.h
  editlib: ccrystaltextview.cpp ccrystaltextview.h
  Languages: Updated all resource.h and rc files to include two new strings

2003-07-11 Kimmo
 PATCH: [ 769295 ] Fix for threaded dir compare crash
  WinMerge: DiffThread.h DiffThread.cpp DirDoc.h DirDoc.cpp DirView.h DirView.cpp

2003-07-10 Kimmo
 PATCH: [ 766727 ] Threaded dir compare
  WinMerge: DiffContext.h DiffContext.cpp DirDoc.h DirDoc.cpp
   DirView.h DirView.cpp MainFrm.h MainFrm.cpp Merge.dsp
   New files: DiffThread.h DiffThread.cpp

2003-07-10 Laoran
 PATCH: [ 768378 ] use Rescan Timer for Replace All 
  WinMerge: MergeEditView.cpp

2003-07-09 Kimmo
 PATCH: [ 765768 ] Only one Rescan on AllToLeft
  Submitted by Perry
  WinMerge: MergeDoc.h MergeDoc.cpp MergeEditView.cpp

2003-07-08 Kimmo
 PATCH: [ 766473 ] Update identical files
  WinMerge: MergeDoc.cpp

2003-07-08 Laoran
 PATCH: [ 767159 ] merge LF_RIGHT_ONLY and LF_LEFT_ONLY -> LF_GHOST 
  WinMerge: MergeDoc.cpp MergeEditView.cpp
  editlib: ccrystaltextbuffer.h ccrystaltextview.cpp

2003-07-08 Laoran
 PATCH: [ 767051 ] code cleanup for GetTextWithoutEmptys 
  WinMerge: MergeDoc.cpp  MergeDoc.h MergeEditView.cpp
  editlib: ccrystaltextbuffer.cpp ccrystaltextbuffer.h

2003-07-08 Perry
 PATCH: [ 765165 ] Log error string if CreateFile fails (to debug window)
  WinMerge: files.cpp MainFrm.cpp MainFrm.h MergeDoc.cpp StdAfx.cpp StdAfx.h

2003-07-07 Kimmo
 PATCH: [ 767204 ] Fix for assert when rescanning with selection
  Submitted by Gilbert Wellisch
  editlib: ccrystaltextview.cpp

2003-07-05 Laoran
 PATCH: [ 765399 ] resize m_pdwParseCookies before GoToLine
  WinMerge: MergeDoc.cpp  MergeEditView.cpp

2003-07-05 Laoran
 PATCH: [ 765926 ] Merge empty last line to the last diffrange (change)
  WinMerge: MergeDoc.cpp

2003-07-04 Laoran
 PATCH: [ 766038 ] stay in the viewing area during OnFirstdiff
  WinMerge: MergeEditView.cpp

2003-07-04 Kimmo
 PATCH: [ 765463 ] Simplify temp path handling a little
  WinMerge: MergeDoc.h MergeDoc.cpp

2003-07-03 Kimmo
 PATCH: [ 764571 ] Ask about saving when closing with esc
  WinMerge: MainFrm.cpp MergeDoc.cpp MergeEditView.cpp
 PATCH: [ 763499 ] update french language
  Submitted by Laoran
  Lang/MergeFrench.rc

2003-07-03 Perry
 PATCH: [ 763265 ] Fix handling of .. and . components
  WinMerge: paths.cpp

2003-07-02 Perry
 PATCH: [ 764789 ] Fix Private Build string in rc file
  WinMerge: Merge.rc

2003-07-02 Kimmo
 PATCH: [ 763817 ] Hilight diffs when syntax hilight is off
  WinMerge: MergeEditView.cpp
 PATCH: [ 762531 ] Inproper globals in editlib/cs2cs.cpp
  Submitted by jtuc
  editlib: cs2cs.cpp
 PATCH: [ 762529 ] Dirframe trying to clear non-existing pane
  Submitted by jtuc
  WinMerge: DirFrame.cpp
  
2003-06-30 WinMerge beta 2.1.0.0 released

2003-06-30 Laoran
 [ 762394 ] update german language
  Languages: German\MergeGerman.rc

2003-06-30 Perry
 PATCH: [ 762835 ] Display line number etc beneath editor
  WinMerge: ChildFrm.cpp ChildFrm.h DirFrame.cpp Merge.cpp Merge.dsp 
    MergeEditStatus.h MergeEditView.cpp MergeEditView.h
  editlib: ccrystaltextbuffer.h ccrystaltextview.cpp ccrystaltextview.h

2003-06-29 Perry
 PATCH: [ 758912 ] Combine duplicated code in CMergeEditView::OnEditCopy
  WinMerge: MergeEditView.cpp
 PATCH: [ 762533 ] Stdafx.h has dublicated and unneeded includes
  WinMerge: StdAfx.h
 PATCH: [ 762271 ] Alter ListCopy to copy eol chars
  WinMerge: MergeDoc.cpp MergeDoc.h
  editlib: ccrystaltextbuffer.h ccrystaltextbuffer.cpp

2003-06-29 Kimmo
 PATCH: [ 762749 ] Forced rescan after options changed
  WinMerge: MainFrm.cpp

2003-06-28 Kimmo
 PATCH: [ 761874 ] Fix unlocalisable acceskeys
  WinMerge: Merge.rc
  editlib: editres.rc
  Updated language files too
 PATCH: [ 761827 ] Current diff item to select inactive diff
  WinMerge: MergeDoc.h MergeDoc.cpp MergeEditView.cpp
 PATCH: [ 762098 ] Code cleanups from Jochen
  Submitted by jtuc
  WinMerge: MainFrm.h MainFrm.cpp MergeDoc.cpp MergeEditView.cpp

2003-06-28 Perry
 PATCH: [ 758930 ] Display EOL chars with View Whitespace
  WinMerge: MainFrm.cpp
  editlib: ccrystaltextview.h ccrystaltextview.cpp
 PATCH: [ 762268 ] Fix typo in comment (wenn -> when)
  editlib: ccrystaltextview.cpp

2003-06-27 Kimmo
 PATCH: [ 761951 ] Fix scrolling corruption
  Submitted by Jochen
  editlib: ccrystaltextview.cpp

2003-06-27 Laoran
 PATCH: [ 760745 ] Fix empty search field detection and better history in FindDlg and EditReplaceDlg
  editlib : ccrystaleditview.cpp ccrystaltextview.cpp ceditreplacedlg.cpp ceditreplacedlg.h
    cfindtextdlg.cpp cfindtextdlg.h memcombo.cpp memcombo.h
  
2003-06-26 Christian
 PATCH: [ 754001 ] 2.0 doc link points to 1.7 docs
  WinMerge: MainFrm.cpp

2003-06-26 Kimmo
 PATCH: [ 755918 ] Fix typo in CCrystalTextView extension list
  Added more filename extensions
  editlib: ccrystaltextview.cpp
 PATCH: [ 757180 ] Update Open OK on dropped file
  Submitted by Perry
  common: SuperComboBox.cpp

2003-06-25 Kimmo
 PATCH: [ 759755 ] Better rescan error reporting
  WinMerge: MainFrm.cpp MergeDoc.h MergeDoc.cpp
 PATCH: [ 760441 ] Fix recursive d&d compare
  WinMerge: MainFrm.cpp

2003-06-24 Perry
 PATCH: [ 759778 ] Minor cleanup of crystaltextbuffer.cpp
  editlib: ccrystaltextbuffer.cpp
 PATCH: [ 760077 ] Omit final ghostline if last line unterminated
  WinMerge: MergeDoc.cpp
 PATCH: [ 760129 ] Wipe Undo stack if Undo fails, and fix Undo check
  editlib: ccrystaltextbuffer.cpp ccrystaltextbuffer.h

2003-06-23 Perry
 PATCH: [ 759533 ] Fix InternalInsertText & InternalDeleteText
  editlib: ccrystaltextbuffer.cpp
 PATCH: [ 759569 ] Fix Undo/Redo & line mappings
  editlib: ccrystaltextbuffer.cpp ccrystaltextbuffer.h ccrystaleditview.cpp

2003-06-23 Kimmo
 PATCH: [ 759344 ] Add Copy, Cut & Paste to Edit menu
  WinMerge: Merge.rc
  Updated other languages too - translations from Perry and Christian

2003-06-22 Perry
 PATCH: [ 758870 ] Return filesize+1 rather than 0 for new lines at end
  editlib: crystaltextbuffer.cpp crystaltextbuffer.h
 PATCH: [ 758900 ] Fix bug in pasting contiguous empty lines.
  editlib: crystaltextbuffer.cpp
 PATCH: [ 756218 ] Fix copy & paste typo in comment in Merge.cpp
  WinMerge: Merge.cpp

2003-06-21 Perry
 PATCH: [ 716456 ] Update Undo/Redo info with rescan line mapping
  WinMerge: MergeDoc.cpp MergeEditView.h
  editlib: crystaltextbuffer.cpp crystaltextbuffer.h

2003-06-19 Perry
 PATCH: [ 728153 ] If opening first file fails, don't open second
  WinMerge: MainFrm.cpp

2003-06-17 Kimmo
 PATCH: [ 755918 ] Fix typo in CCrystalTextView extension list
  editlib: ccrystaltextview.cpp

2003-06-16 Christian
 PATCH [ 754015 ] WinMerge installer imrovements
  InnoSetup: Info.txt WinMerge.iss WinMergeIcon.bmp WinMergeLarge.bmp

2003-06-12 Kimmo
 PATCH: [ 753284 ] Fix cursor slowdown after rescan
  editlib: ccrystaltextview.cpp

2003-06-08 Kimmo
 PATCH: [ 750160 ] Char width fix
   editlib: ccrystaltextview.cpp

2003-06-07 Kimmo
 PATCH: [ 750302 ] Allow *.flt filter files
  WinMerge: Merge.cpp

2003-06-06 Kimmo
 Updated filter file from patch [ 742041 ] Improvements to file/directory filters
  Filters: Merge_VC_loose.flt
 BUG: [ 749007 ] "Highlighting" is misspelled as "Hilighting"
  WinMerge: Merge.rc

2003-06-05 WinMerge 2.0.0 Release Candidate 1 released

2003-06-05 Christian
 PATCH: [ 749566 ] Updated resource files
  Languages: all resource.h files and all Merge*.rc files

2003-06-04 Christian
 Updated Czech and Danish resources
  Languages: Czech\resource.h Czech\MergeCzech.rc Danish\resource.h Danish\MergeDanish.rc

2003-06-02 Kimmo
 PATCH: [ 747090 ] Fix scroll handling
  editlib: ccrystaltextview.cpp

2003-06-01 Kimmo
 PATCH: [ 746319 ] Adjust horiz scrolling width
  editlib: ccrystaltextview.cpp

2003-05-31 Kimmo
 PATCH: [ 745654 ] Draw long lines
  WinMerge: MergeEditView.cpp
  editlib: ccrystaltextview.cpp
 PATCH: [ 742041 ] Improvements to file/directory filters
  WinMerge: FileFilterMgr.h FileFilterMgr.cpp Merge.cpp
 PATCH: [ 738557 ] Allow "Open Left" for directories
  WinMerge: DirActions.cpp DirView.h DirView.cpp

2003-05-29 Christian
 PATCH: Applied [ 745223 ] Fix pasting different EOL types
  edilib: ccrystaltextbuffer.cpp
 BUG: [ 744990 ] Pasting to the right

2003-05-29 Kimmo
 Made new folder \Filters for filter files.  Added Perry's original filter files
 working with beta 1.9.2.2

2003-05-28 Kimmo
 PATCH: [ 744872 ] Use CopyFile()
  WinMerge: files.h files.cpp MergeDoc.cpp

2003-05-26 Kimmo
 PATCH: [ 743284 ] String fixes
  WinMerge: DirDoc.cpp DirView.cpp MainFrm.cpp MergeDoc.h paths.cpp
  editlib: ccrystaleditview.cpp ccrystaltextview.cpp registry.cpp

2003-05-25 Kimmo
 PATCH: [ 742735 ] Fix CCrystalTextBuffer::InsertLine()
  edilib: ccrystaltextbuffer.cpp

2003-05-23 Christian
 Made new folder \InnoSetup\ for files related to making WinMerge installer using InnoSetup

2003-05-22 Perry
 PATCH: [ 741265 ] Fix for bug#741263: 1.9.2.2 crash on refresh
  WinMerge: Diff.cpp DiffContext.cpp DiffContext.h DirDoc.cpp MainFrm.cpp

2003-05-19 WinMerge beta 1.9.2.2 released

2003-05-18 Perry
 PATCH: [ 739529 ] project file & directory filters
  WinMerge: Diff.cpp DiffContext.cpp DiffContext.h FileFilterMgr.cpp 
    FileFilterMgr.h MainFrm.cpp MainFrm.h Merge.cpp Merge.dsp Merge.h
	Merge.rc PropFilter.cpp PropFilter.h resource.h

2003-05-16 Kimmo
 PATCH: [ 737709 ] Remove unneeded UpdateAllViews()
  WinMerge: MergeDoc.cpp
 PATCH: [ 738467 ] Enable/disable save items
  WinMerge: MergeEditView.h MergeEditView.cpp

2003-05-14 Kimmo
 PATCH: [ 737692 ] Scrolling optimisation - significant speedup
  editlib: ccrystaltextview.cpp

2003-05-13 Kimmo
 PATCH: [ 736443 ] Prpoper Esc fix
  WinMerge: DirView.h DirView.cpp Merge.cpp MergeEditView.h MergeEditView.cpp

2003-05-09 Kimmo
 PATCH: [ 735127 ] German Translation
  Updated German language file submitted by Jörg Schneider
 PATCH: [ 734787 ] Disable ESC closing window
  NOTE: This is to fix BUG [ 734554 ] ESC closes dialog + document
  WinMerge: Merge.cpp
 BUG: [ 711961 ] shift-tab in editor moved block then deselects
  WinMerge: MergeEditView.cpp

2003-05-07 Kimmo
 PATCH: [ 733383 ] Enable Find- and Replace- dialogs
  WinMerge: Merge.rc resource.h
  editlib: ceditreplacedlg.h ceditreplacedlg.cpp cfindtextdlg.h
   cfindtextdlg.cpp editcmd.h editres.rc
  Removed: editlib/resource.h
  Updated other languages
 PATCH: [ 733926 ] Fix find dialog text selection
  editlib: ccrystaltextview.cpp

2003-05-06 Kimmo
 PATCH: [ 732775 ] Add support for MAC files
  WinMerge: MergeDoc.cpp MergeEditView.h

2003-05-03 Kimmo
 BUG: [ 731486 ] Crash after save
  WinMerge: MergeDoc.cpp
 PATCH: [ 731533 ] Temp path fix
  WinMerge: MergeDoc.h MergeDoc.cpp
  editlib: ccrystaltextbuffer.h ccrystaltextbuffer.cpp

2003-05-02 Kimmo
 BUG: [ 724726 ] Status bar says "Rescanning" the first time
  WinMerge: Merge.rc

2003-05-01 Kimmo
 PATCH: [ 730738 ] Update BCMenu (to v3.033)
  WinMerge: BCMenu.h BCMenu.cpp
   Updates BCMenu to v3.033, http://www.codeproject.com/menu/bcmenu.asp

2003-04-29 Kimmo
 PATCH: [ 728542 ] Automatic rescan enable/disable
  WinMerge: MainFrm.h MainFrm.cpp Merge.rc MergeEditView.h MergeEditView.cpp
   PropGeneral.h PropGeneral.cpp resource.h
   Updated other languages too

2003-04-28 Kimmo
 PATCH: [ 726330 ] Saving before refresh - backout saving - see bug [ 723412 ]
  WinMerge: MergeDoc.cpp MergeEditView.cpp
 PATCH: [ 728228 ] Rename properties to options and rearrange
  Renaming "Properties" to "Options"
  WinMerge: MainFrm.h MainFrm.cpp Merge.rc resource.h
  Updated .rc and resource.h for other languages
 BUG: [ 718133 ] Inconsistent user keyword list for C/C++ syntax hiligth
  editlib: cplusplus.cpp

2003-04-27 Kimmo
 BUG: [ 728173 ] Autoscroll first diff breaks syntax hilight
  WinMerge: MergeDoc.cpp

2003-04-26 Kimmo
 PATCH: [ 726330 ] Saving before refresh
  WinMerge: MergeDoc.h MergeDoc.cpp MergeEditView.cpp
 PATCH: [ 727455 ] Autoscroll first diff for dir compare
  WinMerge: DirView.h MainFrm.cpp

2003-04-25 Kimmo
 PATCH: [ 727104 ] Disable margins for unknown filetypes
  editlib: ccrystaltextview.h ccrystaltextview.cpp

2003-04-23 Kimmo
 PATCH: [ 721939 ] Wider horiz scroll + more space after last char
  editlib: ccrystaltextview.cpp
 PATCH: [ 726390 ] Last diff shown incorrectly sometimes
  WinMerge: MergeDoc.cpp

2003-04-20 Perry
 Changed dynamic array to be CArray based
  editlib: ccrystaltextview.cpp
 PATCH: [ 724309 ] Add WaitStatusCursors
  WinMerge: DirDoc.cpp DirView.cpp MainFrm.cpp Merge.dsp MergeDoc.cpp MergeEditView.cpp 
   WaitStatusCursor.cpp WaitStatusCursor.h
 PATCH: [ 724473 ] Move WaitStatusCursor strings into RC
  WinMerge: DirDoc.cpp DirView.cpp MainFrm.cpp MergeDoc.cpp MergeEditView.cpp resource.h StdAfx.cpp StdAfx.h
 PATCH: [ 724479 ] Fix Refresh crash in empty dir view
  WinMerge: DirDoc.cpp
 bugfix: Add new files that were part of patch#724309
  WinMerge: WaitStatusCursor.cpp WaitStatusCursor.h
 PATCH: [ 724493 ] WaitStatusCursor for MergeDoc rescans
  WinMerge: MergeDoc.cpp

2003-04-19 Perry
 [ 724280 ] Could use wait cursors at OpenSelection calls
  WinMerge: DirView.cpp

2003-04-15 Perry
 PATCH: [ 722083 ] refresh button (menu item, accelerator)
  WinMerge: DirView.cpp DirView.h Merge.rc MergeEditView.cpp MergeEditView.h resource.h res/Toolbar.bmp
 Also updated resource files (resource.h, *.rc, Toolbar.bmp) to all languages

2003-04-14 Kimmo
 PATCH: [ 720613 ] CDirView leaks imagelist
  WinMerge: DirView.cpp
 PATCH: [ 720701 ] Use memory-mapped files when saving files
  WinMerge: files.h files.cpp MergeDoc.h MergeDoc.cpp

2003-04-14 Perry
 PATCH: [ 716475 ] Reduce edtlib.h coupling
 WinMerge: MergeEditView.h StdAfx.h editlib/ccrystaleditview.inl

2003-04-13 Kimmo
 PATCH: [ 719872 ] Use memory-mapped files when loading files
  WinMerge: files.h files.cpp Merge.dsp MergeDoc.h MergeDoc.cpp

2003-04-10 Kimmo
 PATCH: [ 717688 ] CrystalEditor optimisations and memleaks
  fix tab bug.
  editlib: ccrystaltextview.cpp

2003-04-09 Kimmo
 PATCH: [ 717688 ] CrystalEditor optimisations and memleaks
  editlib: ccrystaltextview.cpp

2003-04-08 Kimmo
 PATCH: [ 716744 ] Fix CrystalEditor memory leaks
  editlib: ccrystaltextbuffer.cpp ccrystaltextview.cpp

2003-04-07 Kimmo
 PATCH: [ 716308 ] Disable italic text
  editlib: ccrystaltextview.cpp
 PATCH: [ 715553 ] Add commandline options to not add paths to MRU
  Usage: -ur to ignore rigth, -ul to ignore left and -ub to ignore both
  WinMerge: MainFrm.h MainFrm.cpp Merge.cpp 

2003-04-06 Kimmo
 PATCH: [ 715457 ] LoadFromFile() rewrote
  WinMerge: MergeDoc.h MergeDoc.cpp

2003-04-06 Perry
 PATCH: [ 716088 ] Fix bin same/diff bug in Diff.cpp & analyze.c
  WinMerge: analyze.c Diff.cpp

2003-04-04 Christian
 PATCH: [ 711843 ] Allow backspace at beginning of line
  WinMerge: MainFrm.cpp

2003-04-03 Kimmo
 BUG: [ 714570 ] Sensitive to EOL -setting not saved
  WinMerge: MainFrm.cpp

2003-04-02 Kimmo
 PATCH: [ 713322 ] Save whole buffer not line by line
  WinMerge: MergeDoc.cpp
 PATCH: [ 713416 ] MergeEditView optimisations
  WinMerge: MergeEditView.cpp

2003-03-31 Kimmo
 PATCH: [ 711437 ] Don't copy/cut removed lines
 WinMerge: MergeDoc.h MergeDoc.cpp MergeEditView.h MergeEditView.cpp

2003-03-31 Perry
 PATCH: [ 712429 ] Fix UpdateItemStatus bug 712093
 WinMerge: DirActions.cpp DirView.cpp paths.cpp paths.h

2003-03-30 Perry
 PATCH: [ 712417 ] Fix save of open doc before next merge
 WinMerge: ChildFrm.cpp ChildFrm.h DirDoc.cpp DirDoc.h DirFrame.cpp DirFrame.h
  DirView.cpp DirView.h MainFrm.cpp MainFrm.h MergeDoc.cpp MergeDoc.h

2003-03-29 Perry
 PATCH: [ 710347 ] Fix crash of winmerge c:\ d:\
 WinMerge: DiffContext.h DirDoc.cpp MainFrm.cpp Merge.cpp OpenDlg.cpp OpenDlg.h paths.cpp paths.h
 PATCH: [ 687177 ] Handle more errors during copy/creation
 WinMerge: MainFrm.cpp
 PATCH: [ 689884 ] Revise doc/view code (allow multiple docs)
 WinMerge: ChildFrm.cpp DirActions.cpp DirDoc.cpp DirDoc.h DirView.cpp MainFrm.cpp MainFrm.h
  Merge.cpp MergeDoc.cpp MergeDoc.h MergeEditView.cpp
 NB: This broke two things in MainFrm--search for TODO: 2003-03-29
 BUGFIX: [ 711975 ] CMainFrame::GetAllViews crashes
 WinMerge: MainFrm.cpp
 BUGFIX: [ 711983 ] ClearMergeDoc causing ASSERT
 WinMerge: DirDoc.cpp MergeDoc.cpp

2003-03-26 WinMerge beta 1.9.1.4 released

2003-03-25 Perry
 PATCH: [ 709575 ] Fix sanity check in CChildFrame::UpdateHeaderSizes()
 WinMerge: ChildFrm.cpp
 PATCH: [ 708542 ] Make splash & about show language-independent version
 common: version.cpp version.h
 WinMerge: Merge.cpp Splash.cpp

2003-03-25 Christian
 Replace COPYING with corrected version from http://www.gnu.org/licenses/gpl.txt

2003-03-25 Kimmo
 PATCH: [ 708893 ] Don't ask to save file compare
  WinMerge: MergeDoc.h MergeDoc.cpp
 PATCH: [ 703069 ] Fix undo crash
  editlib: crystaltextbuffer.cpp
 PATCH: [ 709502 ] Fix missing/existing EOL at end of file
  WinMerge: MergeDoc.h MergeDoc.cpp

2003-03-23 Perry
 PATCH: [ 706006 ] Suppress Rescan during Merge
  Merge.cpp Merge.h MergeDoc.cpp MergeDoc.h MergeEditView.cpp

2003-03-23 Kimmo
 PATCH: [ 707753 ] Delayed rescan - fix
  WinMerge: MergeEditView.cpp

2003-03-22 Kimmo
 PATCH: [ 707753 ] Delayed rescan
  WinMerge: MergeEditView.h MergeEditView.cpp

2003-03-21 Kimmo
 PATCH: [ 707152 ] Delay path check in opendialog
  WinMerge: OpenDlg.h OpenDlg.cpp

2003-03-14 Kimmo
 PATCH: [ 703234 ] Use stricmp
  WinMerge: DIFF.C diffmain.c SDIFF.C

2003-03-11 Perry
  Deleted unused res files: bitmap1.bmp, bmp00001.bmp, ScreenShot.gif, WinMerge.gif

2003-03-11 Christian
 PATCH: [ 700872 ] Esc to close crashes WinMerge
  WinMerge: Merge.cpp

2003-03-09 Kimmo
 PATCH: [ 699839 ] Add same binaries to dir compare status
  WinMerge: MainFrm.h MainFrm.cpp
 PATCH: [ 699893 ] Add drag&drop of files/paths to main window
  Note: Press down Ctrl when dropping dirs for recursive compare
  WinMerge: MainFrm.h MainFrm.cpp Merge.cpp paths.h paths.cpp

2003-03-07 WinMerge beta 1.9.1.3 released

2003-03-07 Perry
 PATCH: [ 699580 ] Fix paths.cpp
  WinMerge: paths.cpp

2003-03-07 Kimmo
 PATCH: [ 698754 ] Update filetimes after save
  WinMerge: DiffContext.h DiffContext.cpp DirDoc.h DirDoc.cpp MergeDoc.cpp

2003-03-06 Christian
 Update Danish language file with new strings in MergeDanish.rc
 Removed duplicate dialog in French language file in MergeFrench.rc
 Added Czech to BuildAll.bat
 PATCH: [ 698346 ] Use full path for directory context menu option
  WinMerge: PropRegistry.cpp

2003-03-06 Kimmo
 PATCH: [ 696695 ] DirDoc Redisplay optimisation
  WinMerge: DirDoc.h DirDoc.cpp

2003-03-05 Perry
 Added new subdir Languages/Danish/res
  PATCH: [ 695857 ] Fix directory handling, & UNC paths, & shortnames
 Fix Danish language files: Update resource.h and update language dialogbox to use IDC_LANGUAGE_LIST.

2003-03-04
 BUG: [ 684622 ] Escape key closes whole program
  Nikki Locke's patch slightly modified
  WinMerge: Merge.cpp
 PATCH: [ 696588 ] Multimonitor + virtual screens support
  WinMerge: MainFrm.cpp Merge.dsp multimon.h
 BUG: [ 697320 ] Saving enabled when comparing directories
  WinMerge: DirView.h DirView.cpp

2003-02-28 Kimmo
 BUG: [ 693932 ] Trailing \ on path crashes WinMerge
  WinMerge: OpenDlg.h OpenDlg.cpp

2003-02-27 Kimmo
 BUG: [ 693820 ] Typed path failed to compare
  WinMerge: OpenDlg.cpp
 BUG: [ 693004 ] Keyboard does not work for accessing menus
  Add missing WM_MENUCHAR handler from www.codeguru.com
  WinMerge: MainFrm.h MainFrm.cpp

2003-02-26 Kimmo
 BUG: [ 683753 ] Rescan is not prompting to save dirty
      current file
  WinMerge:
   MainFrm.cpp

2003-02-26 Christian
 [ 70970 ] Translate Merge.rc into danish
    BuildAll.bat and added directory Languages/Danish

2003-02-26 Kimmo
 [ 692922 ] Don't try to compare file & directory
  WinMerge:
   OpenDlg.cpp
 [ 686699 ] Check & return file saving success
  Fix2: Do not return save success of "Save As"
  WinMerge:
   MergeDoc.cpp

2003-02-24 Kimmo
 [ 687574 ] Navigate in directory view (fix bug in patch)
  WinMerge:
   DirView.cpp
 [ 686699 ] Check & return file saving success - fix
  WinMerge:  
   MergeDoc.h Mergedoc.cpp

2003-02-22 Perry
 [ 650356 ] suggest commenting out several functions
 [ 684416 ] is_all_whitespace is bad
  Removed bad unused functions in common/coretools.cpp (& .h)
  (Did not remove all unused functions, only ones with bugs)

2003-02-21 Perry
 Sync'ed all language resources to English version.
 [ 684300 ] Stringified Diffitems
   + combined & modularized code in DirDoc.cpp
  WinMerge:
   DiffContext.cpp DiffContext.h DirDoc.cpp DirView.cpp DirView.h

2003-02-20 Perry
 [ 689876 ] Patch to fix bug # 689869 (ASSERT on Rescan)
  WinMerge:
   MainFrm.cpp & new resource IDS_ERROR_FILE_NOT_FOUND
 [ 687574 ] Navigate in directory view
  WinMerge:
   DirView.cpp DirView.h
 [ 690176 ] Cmdline param for ESC closing
   (and updated all obsolete French & German accelerators as well)
  WinMerge:
   Merge.cpp Merge.h Merge.rc
  Languages:
   Czech\MergeCzech.rc Dutch\MergeDutch.rc French\MergeFrench.rc
   German\MergeGerman.rc Italian\MergeItalian.rc Spanish\MergeSpanish.rc
  [ 689896 ] Update to sync Czech resource
    Czech\MergeCzech.rc

2003-02-18 Christian
  Removed annoying SourceSafe path from Merge.dsp

2003-02-18 Kimmo
  [ 684737 ] Fix Language Box to handle dblclick
  Languages:
   LanguageSelect.h LanguageSelect.cpp
  WinMerge:
   Merge.rc resource.h
  [ 687039 ] View whitespaces on editor
  WinMerge:
   MainFrm.h MainFrm.cpp Merge.rc resource.h
  WinMerge/editlib:
   ccrystaltextview.cpp

2003-02-15 Kimmo
  [ 686699 ] Check & return file saving success
  Merge.rc MergeDoc.h Mergedoc.cpp resource.h

2003-02-14 Kimmo
 [ 685491 ] Saving fixes: SaveToFile() rewrite part
  + Missing NULL check for mf->m_pDirDoc in CMergeDoc
  MergeDoc.h MergeDoc.cpp

2003-02-13  Perry
 [ 685816 ] Recommend deleting unlocalized translations
 [ 684290 ] improved bin test:
  Diff.cpp readme.txt
 [ 686090 ] Show/hide binary files
 WinMerge:
  DirDoc.cpp MainFrm.cpp MainFrm.h Merge.rc readme.txt
  resource.h
 Languages:
  Spanish/MergeSpanish.rc Spanish/resource.h

2003-02-12  Perry
 Languages:
  Dutch/MergeDutch.rc
 Some updates to align Dutch rc towards English rc.
 [ 685637 ] CMainFrame::SyncFiles GPF bug

2003-02-12  Christian
 [ 630286 ] WinMerge crashes/fails in WinCVS
 [ 683752 ] Add option to be listed on Directory context menu

2003-02-11  Perry
 Fixing recent SplitFilename & directory concat problems.
 [ 683924 ] Same binaries for directory compare.

2003-02-10  Perry
 Kimmo's fix for crash due to NULL doc pointer in MergeDoc::SaveHelper.
 [ 470013 ] Spanish Orthography and Semantics

2003-02-09  Perry
 [ 683120 ] splitfilename changed to CStrings
 Move new SplitFilename from Chris Mumford into coretools, remove old
 versions, add test chassis, and fix some minor cases.

2003-02-08  Perry
 DirDoc.*: Changed from strftime to COleDateTime::Format (respects user prefs).
 First half of [ 679692 ] Update after editing (with code combination).
 [ 682505 ] API changes for CLog, CDirView and CDirDoc
 2nd half of [ 679692 ] Update after editing
 
2003-02-07  WinMerge beta 1.9.1.1 released

2003-02-07  Christian
 Added 24 missing languages
 Fixed duplicated strings in Italian rc.

2003-02-05  Perry
 Updated Brazilian, Italian, Spanish resources to sync with English/main resources.

2003-01-28  Christian
 Committed [ 424579 ] Escape to exit

2003-01-27  Christian
 [ 668654 ] Tabtype selection feature
 [ 672554 ] Update DirView after individual merge
 Fixed [ 225571 ] After individual merge, dirView not updated
 [ 672879 ] Propose clarifying change in ccrystaleditview.cpp
 [ 672885 ] Fix for ASSERT from CChildFrame::UpdateHeaderSizes
 [ 673063 ] Modification check before close
 [ 675526 ] Waitcursors and merging all

2003-01-21  Christian
 [ 671037 ] Changes for MSVC.NET
 [ 671810 ] Tempfiles to temp dir

2003-01-20  Christian
 [ 671034 ] operator precedence bug in SplitterWndEx.cpp

2003-01-15  Christian
 [ 650384 ] Fix for false equals for binary files
 [ 651689 ] Add OpenLeft, OpenLeftWith, ... context
 [ 650293 ] Implement context menus with multiselect

2003-01-14  Christian
 [ 665993 ] Little optimisation

2002-12-17  WinMerge beta 1.7.2.11 released

2002-11-26  Perry
 Implemented recursive directory delete
 Fix bug in EOL handling in crystaltextbuffer, and add optional EOL sensitivity.
 Added optional EOL insensitivity to gnu diff IO.c.
 Added status bar progress (file count) indicator during scanning
 Fixed tooltips of toolbar buttons to list correct hotkeys
 Split "Show Unique" option into "Show Left Unique" and "Show Right Unique"

2002-12-02  Perry
 Added left files, left dirs, right files, right dirs to status bar feedback

2002-11-18  WinMerge beta 1.7.2.1 released
