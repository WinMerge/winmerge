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

