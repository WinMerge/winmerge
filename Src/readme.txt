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
 
2003-02-07
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

2002-11-26  Perry
 Implemented recursive directory delete
 Fix bug in EOL handling in crystaltextbuffer, and add optional EOL sensitivity.
 Added optional EOL insensitivity to gnu diff IO.c.
 Added status bar progress (file count) indicator during scanning
 Fixed tooltips of toolbar buttons to list correct hotkeys
 Split "Show Unique" option into "Show Left Unique" and "Show Right Unique"

2002-12-02  Perry
 Added left files, left dirs, right files, right dirs to status bar feedback

