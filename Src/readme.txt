2005-03-19 Perry
 PATCH: [ 1157769 ] Move GuessEncoding functions into one file
  Src: DiffContext.cpp DiffContext.cpp MainFrm.cpp Merge.dsp
  Src/Common: UniFile.cpp UniFile.h
  Src new: codepage_detect.cpp codepage_detect.h

2005-03-18 Perry
 PATCH: [ 1155977 ] Log scriptlet plugin errors
  Src: Plugins.cpp
 PATCH: [ 1162571 ] Simplify SplitFilename
  Src/Common: coretools.cpp
 Cosmetic (new comments)
  Src/Common: LanguageSelect.cpp

2005-03-16 Kimmo
 PATCH: [ 1161709 ] Virtual InitOption() and SaveOption() for COptionsMgr
  Src/Common: OptionsMgr.h

2005-03-16 Jochen
 Widen controls to display Merge7z plugin names (unicode names got chopped off)
  Src: Merge.rc
 BUG: [ 1154551 ] Merge7zInstaller download link focus issue
  Src: 7zCommon.cpp

2005-03-15 Jochen
 Add Merge7z415(U).dll to Merge7zInstaller
  Merge7zInstaller: Files.rc2
 Merge7zInstaller - Initialize filename in Open dialog to "*.exe"
  Merge7zInstaller: Installer.cpp
 Fix typo in IDD_MERGE7ZMISMATCH dialog
  Src: Merge.rc

IDD_MERGE7ZMISMATCH
2005-03-15 WinMerge beta 2.3.2.0 released

2005-03-15 Kimmo
 Add include I missed from fix to bug #1154153
  Src: MainFrm.cpp
 Remove version numbers accidentally applied earlier
  Src: Merge.rc

2005-03-14 Kimmo
 BUG: [ 1154153 ] Poor font choice for default font
  Fix submitted by Takashi Sawanaka
  Src: MainFrm.cpp

2005-03-13 Kimmo
 BUG: [ 1162374 ] Recent WinMerge doesn't support NT4
  Src/Common: UniFile.cpp

2005-03-13 Jochen
 BUG: [ 1162369 ] short path name problem
  Fix paths_GetLongPath(), which erroneously added trailing backslash when passing in short path.
  Src: paths.cpp

2005-03-13 Kimmo
 PATCH: [ 1161671 ] Convert couple of VCS options to COptionsMgr
  Src: MainFrm.cpp MainFrm.h OptionsDef.h
 BUG: [ 1160919 ] Bad layout in the OpenDialog
  Fix submitted by Tim
  Languages/German: MergeGerman.rc

2005-03-11 Kimmo
 PATCH: [ 1160923 ] Remove CMainFrame::m_nCompMethod (ugly hack)
  Src: DirDoc.cpp MainFrm.cpp MainFrm.h

2005-03-10 Kimmo
 PATCH: [ 1160064 ] Undo for Copy All
  Src: MergeDoc.cpp MergeDoc.h
 Fix invalid compare for VSS dialog returnvalue
  Src: MainFrm.cpp

2005-03-09 Perry
 PATCH: [ 1157359 ] Minor change for FILEFILTER_INFOLIST
  Src: FileFilterHelper.h
 PATCH: [ 1159465 ] Add Jesus (Mack) to contributors/Spanish
  Add [Jesús M. Delgado 'MacK'] to Spanish contributors. 
  Alphabetize Swedish contributors.
  Users/Docs: Contributors.txt

2005-03-08 Perry
 Cosmetic: Added & doxygen'd comments, and prefixed members with m_.
  Src: FilepathEdit.cpp FilepathEdit.h
 BUG: [ 1143619 ] Unicode chars in filename display as '_'
  Remove ES_OEMCONVERT from edit boxes in IDD_EDITOR_HEADERBAR.
  Src: Merge.rc
  Src/Languages: Merge*.rc

2005-03-08 WinMerge experimental release 2.3.1.6 (cvs)

2005-03-08 Kimmo
 PATCH: [ 1158456 ] Fix setting font for headerbar editcontrols
  Src: EditorFilepathBar.cpp EditorFilepathBar.h
 PATCH: [ 1158484 ] Fix assert from headerbar-setactive() when closing views
  Src: EditorFilepathBar.cpp EditorFilepathBar.h
 PATCH: [ 1159112 ] Swedish translation revision
  Submitted by Hans Eriksson
  Languages/Swedish: MergeSwedish.rc
 PATCH: [ 1158136 ] Spanish translation revised
  Submitted by Jesus M. Delgado 'Mack'
  Languages/Spanish: MergeSpanish.rc

2005-03-07 Perry
 PATCH: [ 1157859 ] Add LVS_SHOWSELALWAYS style to dirview
 Postlude: Alter WindowStyle_Add to call CWnd::ModifyStyle.
  Src/Common: WindowStyle.cpp

2005-03-06 Tim
 PATCH: [ 1156198 ] Non-standard html in developer docs
  Docs/Developers: readme-developers-*.html

2005-03-06 Perry
 PATCH: [ 1157524 ] Fix view line length
  editlib: ccrystaltextview.cpp ccrystaltextview.h
 PATCH: [ 964676 ] rc file lacks bytes after DIALOGEX defs
  Add 0,0,0x1 to any FONT line lacking it, for all DIALOGEX dialogs
  Src: Merge.rc
  Languages: Merge*.rc
 PATCH: [ 968285 ] DBCS fixes from Sawanaka
  (continuing editlib portions)
  Src/editlib: batch.cpp crystaleditview.cpp crystaltextview2.cpp cplusplus.cpp
   cregexp.cpp csharp.cpp dcl.cpp filesup.cpp fortran.cpp html.cpp js.cpp
   java.cpp list.cpp pascal.cpp perl.cpp php.cpp python.cpp rexx.cpp rsrc.cpp
   sgml.cpp sh.cpp siod.cpp sql.cpp tcl.cpp tex.cpp xml.cpp
 PATCH: [ 1157859 ] Add LVS_SHOWSELALWAYS style to dirview
  Src: DirView.cpp Merge.dsp
  Common new files: WindowStyle.cpp WindowStyle.h


2005-03-05 Perry
 Rename CDirFrame::SetFilter to CDirFrame::SetFilterStatusDisplay for easier greps.
  Src: DirDoc.cpp DirFrame.cpp DirFrame.h
 More filter clarifying renames
  Src: DirDoc.cpp FileFilterHelper.cpp FileFilterHelper.h MainFrm.cpp OpenDlg.cpp
 Rename FilterHelper's GetUseMask to IsUsingMask.
  Src: FileFilterHelper.cpp FileFilterHelper.h MainFrm.cpp OpenDlg.cpp
 BUG: [ 1153184 ] DiffWrapper.cpp missing NEW redefinition
  Added boilerplate new redefintion for DEBUG builds to DiffWrapper.cpp
  Src: DiffWrapper.cpp
 PATCH: [ 1157015 ] Spanish translation update
  Lots of updates to Spanish translation by Jesus M. Delgado 'Mack'
  Src/Languages/Spanish: MergeSpanish.rc
 PATCH: [ 1157145 ] FileFilterHelper doesn't have boilerplate new redefinition
  Src: FileFilterHelper.cpp
 PATCH: [ 968285 ] DBCS fixes from Sawanaka
  Applied asp & basic portions (there are many more)
  editlib: asp.cpp basic.cpp

2005-03-04 Perry
 PATCH: [ 1147039 ] experimental patch for colored word diff
  Merge all the easy new Japanese string translations
   (Omit major changes to IDD_PROPPAGE_EDITOR & IDD_MERGE7ZMISMATCH.
  Languages: Japanese/MergeJapanese.rc
 PATCH: [ 1112871 ] new byte_compare function to bypass diffutils
  Src: DiffContext.h DiffWrapper.cpp DiffWrapper.h DirDoc.cpp DirScan.cpp Merge.dsp
  Src new files: ByteComparator.cpp ByteComparator.h

2005-03-04 Kimmo
 PATCH: [ 1155319 ] Select font for dir compare view
  We must allow selecting file compare font even if no open views
  Src: Mainfrm.cpp
 PATCH: [ 1155436 ] Remove non-const GetDiffAt() from CDiffContext
  Src: DiffContext.cpp DiffContext.h DirDoc.cpp DirView.h DirViewColHandler.cpp
 PATCH: [ 1156188 ] Updated readme-innosetup
  Docs/Developers: readme-developers-InnoSetup.html
 PATCH: [ 1147039 ] experimental patch for colored word diff
  Submitted by Takashi Sawanaka
  Src: MainFrm.cpp Merge.rc MergeDiffDetailView.cpp MergeDiffDetailView.h MergeDoc.h
   MergeDocLineDiffs.cpp MergeEditView.cpp MergeEditView.h OptionsDef.h
   PropColors.cpp PropColors.h resource.h
  Src/editlib: asp.cpp basic.cpp batch.cpp ccrystaltextview.cpp ccrystaltextview.h
   cplusplus.cpp csharp.cpp dcl.cpp fortran.cpp html.cpp ini.cpp is.cpp java.cpp
   lisp.cpp pascal.cpp perl.cpp php.cpp python.cpp rexx.cpp rsrc.cpp sgml.cpp sh.cpp
   siod.cpp sql.cpp tcl.cpp tex.cpp xml.cpp
  Src/Languages: *.rc resource.h

2005-03-04 Perry
 PATCH: [ 1156253 ] Fix filter helper handling of new filters
 BUG: [ 1153018 ] Create new filter, now cannot find it
  Src: FileFilterHelper.cpp FileFilterHelper.h FileFiltersDlg.cpp Merge.cpp
   Merge.h paths.cpp paths.h

2005-03-03 Kimmo
 PATCH: [ 1155319 ] Select font for dir compare view
  Src: DirView.cpp DirView.h MainFrm.cpp MainFrm.h Merge.rc OptionsDef.h
  Languages: *.rc
 PATCH: [ 1154667 ] Make CDiffContext paths private members
  Src: DiffContext.cpp DiffContext.h DirActions.cpp DirDoc.cpp DirScan.cpp DirView.cpp

2005-03-02 Perry
 PATCH: [ 1154259 ] Non blurred 'error.bmp'
  New clearer error bitmap from Tim Gerundt
 Languages: res/error.bmp

2005-03-03 Perry
 PATCH: [ 1151951 ] Alter getRightFilepath for root paths
 re: BUG: [ 1105933 ] Problem accessing files in network shares and removables
  Src: DiffContext.cpp

2005-03-02 Kimmo
 BUG: [ 975226 ] Editor: Always refresh with Del-Key
  Fix submitted by Takashi Sawanaka
  Src: MergeEditView.cpp

2005-03-02 Perry
 PATCH: [ 1154259 ] Non blurred 'error.bmp'
  New clearer error bitmap from Tim Gerundt
  Src/Res: error.bmp
 PATCH: [ 1153127 ] Rename Filepath to FilepathWithEncoding
  Src: DiffWrapper.cpp DiffWrapper.h
 PATCH: [ 967230 ] Initialize default language from locale
  Src/Common: LanguageSelect.cpp LanguageSelect.h


2005-03-01 WinMerge experimental release 2.3.1.5 (cvs)

2005-03-01 Kimmo
 PATCH: [ 1153870 ] Disable redrawing dir compare view while adding items
  Src: DirDoc.cpp

2005-03-01 Jochen
 PATCH: [ 1152341 ] Merge7z update (DllBuild_Merge7z_10)
  Merge7z: Merge7z.h Merge7z311.cpp Merge7z311.dsp Merge7z312.dsp Merge7z313.dsp Merge7z407.dsp Merge7z408.dsp
   Merge7z409.cpp Merge7z409.dsp Merge7z410.dsp Merge7z411.dsp Merge7z412.dsp Merge7z413.dsp Merge7z414.dsp
   Merge7z415.dsp Merge7zCommon.cpp Merge7zCommon.h revision.txt tools.cpp tools.h
 PATCH: [ 1152469 ] Add Merge7zInstaller download link
  Src: 7zCommon.cpp 7zCommon.h dllpstub.cpp dllpstub.h markdown.cpp markdown.h Merge.dsp Merge.rc

2005-02-28 Perry
 BUG: [ 895732 ] paths_GetLongPath usually ignores 2nd parameter
  Remove 2nd parameter entirely from paths_GetLongPath, and improve documentation.
  Src: Merge.cpp OpenDlg.cpp paths.cpp paths.h readme.txt

2005-02-28 Kimmo
 PATCH: [ 1153091 ] Fix selecting default font from View menu
  Src: MainFrm.cpp MainFrm.h OptionsDef.h
 PATCH: [ 1144545 ] Project Handling Patch3 (remember last valid project path)
  Submitted by Andreas Morsing
  Src: MainFrm.cpp OpenDlg.cpp OptionsDef.h

2005-02-28 Perry
 BUG: [ 1039158 ] Confusing EOL differences messagebox
  Src: Merge.rc
  Languages: Merge*.rc

2005-02-27 Perry
 PATCH: [ 1112636 ] Suggest cosmetic change in DiffFileData::OpenFiles
  (new method DiffFileData::Filepath::AssignPath)
  Src: DiffWrapper.cpp DiffWrapper.h
 BUG [ 1144425 ] German: Options/codepage dlg needs adjust
  (Fix by Tim Gerundt)
  Languages: MergeGerman.rc
 PATCH: [ 925085 ] Propose failing files 2gigs+ in size
  Src/Common: UniFile.cpp

2005-02-27 Kimmo
 BUG: [ 1153017 ] FileFilterHelper::SetFilter ASSERTS on empty string
  Src: FileFilterHelper.cpp

2005-02-27 Perry
 BUG: [ 1152913 ] LocationView type bugs?
  (Fixed 17 type conversion warnings in MSVC.NET 2003, mostly by explicit casts)
  Src: LocationView.cpp
 PATCH: [ 1152967 ] Fix re_match_2 declaration warnings
  Src/diffutils/lib: REGEX.C
 BUG: [ 1038897 ] Weird comment in DiffWrapper.cpp
  cosmetic: removing obsolete comment, improving existing comments
  Src: DiffWrapper.cpp
 Cosmetic: Add more explanation to function header of 
  FileFilterHelper::LoadFileFilterDirPattern.
  Src: FileFilterHelper.cpp
 BUG: [ 1152911 ] CListCtrlEx::Dump is an infinite loop.
  Src/Common: listvwex.h

2005-02-26 Jochen
 PATCH: [ 1152256 ] UniFile may fail to read last line
  Src/Common: UniFile.cpp
 Attempt to fix bug [1152375] 'Help -> Archive Support disabled' broken
  Src: 7zCommon.cpp

2005-02-26 Perry
 PATCH: [ 1151949 ] Fix CDirDoc::FindItemFromPaths for root
  Src: DirDoc.cpp

2005-02-23 Perry
 PATCH: [ 1145016 ] i18n the version control strings
  Src: Merge.rc PropVss.cpp PropVss.h resource.h
  Languages: resource.h Merge*.rc

2005-02-23 WinMerge experimental release 2.3.1.4 (cvs)

2005-02-23 Kimmo
 PATCH: [ 1149081 ] Invalid string compare in CChildFrm.cpp
  Src: ChildFrm.cpp
 PATCH: [ 1149365 ] Check linenumber to goto for upper limit
  Src: MergeEditView.cpp
 PATCH: [ 1149388 ] Check for previous compare text before find again
  Src/editlib: ccrystaltextview.cpp
 Add compiler-selecting ifdef to Unifile.cpp to make it compile with MSVC6 and MSVC7
  Src/Common: UniFile.cpp

2005-02-22 Kimmo
 PATCH: [ 1145522 ] fix crash on search
  Submitted by Takashi Sawanaka
  Src/editlib: ccrystaltextview.cpp
 BUG: [ 1144580 ] markdown.cpp compile error with VS 2003
  Fix from Jochen
  Src: markdown.h

2005-02-21 Kimmo
 Update Docbook instructions in devel doc (path changed in sf.net)
  Docs/Developers: readme-developers.html

2005-02-20 Perry
 BUG [ 1144978 ] No languages available in development
  Fix CopyAll.bat, and cosmetic changes to LanguageSelect.*
  Src/Common: LanguageSelect.cpp LanguageSelect.h
  Languages: CopyAll.bat

2005-02-20 Tim
 Fix 'cliboard' typo in rc files.
  Languages: *.rc

2005-02-20 Perry
 BUG: [ 1144917 ] cvs source does not compile: '_stati64' : undeclared identif
  Fix ANSI compilation of stati64
  Src/Common: UniFile.cpp

2005-02-20 Kimmo
 PATCH: [ 1144588 ] Remove show bak files setting from configlog
  Src: ConfigLog.cpp ConfigLog.h

2005-02-19 Kimmo
 PATCH: [ 1144059 ] Allow getting filestatus from open file in UniXXXFile
  Common: UniFile.cpp UniFile.h
 Fix compile error in MainFrm.cpp
  Src: MainFrm.cpp
 Add typecast for VC6
  Common UniFile.cpp

2005-02-18 Kimmo
 PATCH: [ 1124475 ] Update of the swedish translation
  Submitted by Hans Eriksson
  Src/Languages/Swedish: MergeSwedish.rc

2005-02-17 Kimmo
 PATCH: [ 1124380 ] Use _tstati64() in UniLocalFile::DoGetFileStatus()
  Common: UniFile.cpp UniFile.h

2005-02-16 Kimmo
 PATCH: [ 1123539 ] Small improvement for code creating backup copy
  Src: MainFrm.cpp

2005-02-13 Kimmo
 PATCH: [ 1118261 ] Allow creating and deleting filefilters from UI
  Src: FileFilterHelper.cpp FileFilterHelper.h FileFilterMgr.cpp FileFilterMgr.cpp
   FileFiltersDlg.cpp FileFiltersDlg.h Merge.cpp Merge.rc Plugins.cpp Plugins.h resource.h
  Filters new file: FileFilter.tmpl

2005-02-10 Kimmo
 PATCH: [ 1116778 ] Project Handling patch
  Submitted by Andreas Morsing
  Src: MainFrm.cpp MainFrm.h Merge.rc OpenDlg.cpp OpenDlg.h ProjectFile.cpp ProjectFile.h resource.h
  Languages: *.rc resource.h
  Docs/Users: Contributors.txt

2005-02-07 Kimmo
 PATCH: [ 1117375 ] Rememer status/toolbar visible status
  Src: MainFrm.cpp MainFrm.h OptionsDef.h

2005-01-22 Jochen
 PATCH: [ 1107467 ] Detect 7-Zip version from 7zip_pad.xml (2)
  Src: 7zCommon.cpp 7zCommon.h ConfigLog.cpp Merge.dsp Merge.rc paths.cpp paths.h 
  Src new files: charsets.c charsets.h language.c language.h markdown.cpp markdown.h
  Src/Common: version.cpp version.h

2005-02-03 Kimmo
 PATCH: [ 1114360 ] Non-blocking file filter editing
  Src: DiffFileInfo.cpp DiffFileInfo.h DirDoc.cpp DirViewColItems.cpp FileFilterHelper.cpp
   FileFilterHelper.h FileFilterMgr.cpp FileFilterMgr.h MainFrm.cpp Merge.dsp
  Src new files: FileInfo.cpp FileInfo.h

2005-02-02 Kimmo
 BUG: [ 1039592 ] No differences shown after changing EOL options when loading
  Src: MergeDoc.cpp MergeEditView.cpp MergeEditView.h

2005-02-01 Kimmo
 Backout fix for bug [ 1110826 ] Regex $ breaks linefilter
  Src: MainFrm.cpp

2005-01-30 Kimmo
 BUG: [ 1110826 ] Regex $ breaks linefilter
  Src: MainFrm.cpp
 PATCH: [ 1112111 ] Confusing message for identical scratchpads
  Src: MergeDoc.cpp

2005-01-29 Kimmo
 PATCH: [ 1108716 ] Load lang files from 'Languages' subdirectory
  Src/Common: LanguageSelect.cpp

2005-01-28 Perry
  Src/Common: unicoder.cpp
   cosmetic: add comments to get_unicode_char

2005-01-27 Kimmo
 PATCH: [ 946600 ] Copy lines with linenumbers
  Src: Merge.rc MergeEditView.cpp MergeEditView.h resource.h
  Languages: *.rc resource.h

2005-01-24 WinMerge experimental release 2.3.1.3 (cvs)

2005-01-23 Perry
 PATCH: [ 1062082 ] Fix bug [1057997] (long lines issue)
  (Applied UniFile.cpp portion of this patch)
  Src/Common: UniFile.cpp UniFile.h
  Src: MergeDoc.cpp
 BUG: [ 1029643 ] (Catalan) Options/codepage too crowded
  Moved codepage edit box over 20 units, widened label 10 units
  Languages/Catalan: MergeCatalan.rc
 PATCH: [ 1062082 ] Fix bug [1057997] (long lines issue)
  (Applied UniFile.cpp portion of this patch)
  Minor adjustment (remove "sline(1024)" which broken VS.NET compilation).
  Src: MergeDoc.cpp

2005-01-23 Kimmo
 PATCH: [ 1107508 ] Add dirview statusbar panel showing current filter name
  Src: DirDoc.cpp DirFrame.cpp DirFrame.h resource.h
  Languages: resource.h
 Update MASM filter file
  Submitted by Matt Bossard (to forum)
  Filters file: MASM.flt
 Fix bug that left path was shown twice in caption
  Src: DirDoc.cpp

2005-01-23 Perry
 PATCH: [ 1107516 ] Suggest code simplification for DiffFileInfo ctr
  Src: DiffFileInfo.h
 PATCH: [ 1103530 ] Include codepages in "force unification" question
  Src: MainFrm.cpp Merge.rc
  Languages: *.rc
 Minor grammar fixes & wording changes
  Docs/Developers/readme-developers.html
 Add section on "Forcing compiler version with MakeResDll"
  Docs/Developers/readme-developers.html
 PATCH: [ 1107686 ] Fix LANGAGE to LANGUAGE in Merge.rc
  Src: Merge.rc
  Languages: *.rc
 PATCH: [ 1103530 ] Include codepages in "force unification" question
  Placeholder in Japanese text.
  Languages/Japanese: MergeJapanese.rc
 PATCH: [ 1039228 ] add codepage info strings
  Src: codepage.cpp codepage.h Merge.rc resource.h
  Src/Languages: Merge*.rc, resource.h

2005-01-22 Perry
 PATCH: [ 1107484 ] Fix broken version columns
  Src: DiffContext.cpp DiffContext.h DiffFileInfo.cpp DiffFileInfo.h 
   DirViewColItems.cpp

2005-01-22 Jochen
 PATCH: [ 1103266 ] Merge7z update (DllBuild_Merge7z_9)
  Merge7z: Merge7z.h Merge7z311.cpp Merge7z311.dsp Merge7z312.dsp Merge7z313.dsp Merge7z407.dsp Merge7z408.dsp
   Merge7z409.cpp Merge7z409.dsp Merge7z410.dsp Merge7z411.dsp Merge7z412.dsp Merge7z413.dsp Merge7z414.dsp
   Merge7zCommon.cpp Merge7zCommon.h revision.txt
 PATCH: [ 1103274 ] Merge7z interface plugin installer
  Merge7zInstaller: Files.rc2 Installer.cpp Installer.rc Merge7zInstaller.dsp resource.h

2005-01-21 Perry
 PATCH: [ 1103542 ] Declare _MBCS in ANSI builds
  Src: Merge.dsp

2005-01-21 Kimmo
 BUG: [ 1002327 ] "Untitled left" not translated when language switches
  Src: MergeDoc.cpp
 PATCH: [ 1106330 ] Change position in locationpane with single mouseclick
  Src: LocationView.cpp LocationView.h
 PATCH: [ 1106383 ] Show few chars of 'context' when horizontal scrolling lines
  Src/editlib: ccrystaltextview.cpp

2005-01-20 Kimmo
 PATCH: [ 1105640 ] Properly update locationpane visible area indicator
  Src: LocationView.cpp LocationView.h MergeEditView.cpp MergeEditView.h
 Add new MASM filter file
  Submitted by Matt Bossard (to forum)
  Filters new file: MASM.flt

2005-01-18 Kimmo
 PATCH: [ 1062082 ] Fix bug [1057997] (long lines issue)
  View scrolling optimisation from Takashi Sawanaka
  Src/editlib: ccrystaltextview.cpp

2005-01-13 Kimmo
 PATCH: [ 1001341 ] Update edit view status bar on language change
  Submitted by Perry
  Src: ChildFrm.cpp ChildFrm.h DirDoc.cpp DirFrame.cpp DirView.cpp
   MainFrm.cpp MergeDoc.cpp MergeDoc.h MergeEditView.cpp 

2005-01-12 Kimmo
 PATCH: [ 1100462 ] Center find result in file compare
  Src/editlib: ccrystaltextview.cpp

2005-01-11 WinMerge experimental release 2.3.1.2 (cvs)

2005-01-11 Kimmo
 PATCH: [ 1100176 ] small fix in DirDoc RefreshOptions
  Submitted by Jim Fougeron
  Src: DirDoc.cpp
 Update contributors.txt (Jim Fougeron)
  Doc/Users: Contributors.txt

2005-01-10 Kimmo
 Add Hungarian to resource.h updates in UpdateAll_resource_h.bat
  Src/Languages: UpdateAll_resource_h.bat
 PATCH: [ 1099011 ] Added "Hide files" to right click menu (onthefly filter)
  Submitted by Jim Fougeron
  Src: DirView.cpp DirView.h Merge.rc resource.h
  Src/Languages: *.rc resource.h

2005-01-09 Kimmo
 PATCH: [ 1019445 ] IDS_WINMERGE_THIS_DIRECTORY is unused
  Src: Merge.rc resource.h
  Src/Languages: *.rc resource.h

2005-01-08 Kimmo
 PATCH: [ 1098340 ] crash rarely when starting many winmerge continuously
  Anonymous submission
  Src: FileFilterMgr.cpp

2005-01-06 Kimmo
 PATCH: [ 1096609 ] Show tooltips for filefilter dialog list
  Src: FileFiltersDlg.cpp FileFiltersDlg.h

2005-01-05 Kimmo
 PATCH: [ 1094534 ] Add description field for file filters
  Src: FileFilterHelper.cpp FileFilterHelper.h FileFilterMgr.cpp FileFilterMgr.h
   FileFiltersDlg.cpp FileFiltersDlg.h MainFrm.cpp Merge.rc resource.h
  Src/Languages: *.rc resource.h
  Filters: ADAMulti.flt Merge_GnuC_loose.flt Merge_VB_loose.flt Merge_VC_loose.flt
   XML_html.flt
 PATCH: [ 1094467 ] Update contributors: Tim is now developer
  Docs/Users: Contributors.txt

2005-01-03 Kimmo
 PATCH: [ 1094474 ] Visible area indicator for location pane
  Src: LocationView.cpp LocationView.h MergeEditView.h

2005-01-02 Kimmo
 PATCH: [ 1088049 ] Ask about saving when Window/Close is selected
  Src: MergeEditView.cpp MergeEditView.h

2004-12-29 Perry
 BUG: [ 1064942 ] Copyright notice????
  Bumped copyright year to continue through 2005
   Src: Merge.rc
 BUG: [ 859995 ] convertToBuffer assumes ISO-8895-1 codepage
  Add comment about flawed code in actual code
   Src/Common: unicoder.cpp
 BUG: [ 859995 ] convertToBuffer assumes ISO-8895-1 codepage
  Removed flawed and unused method convertToBuffer.
   Src/Common: unicoder.cpp unicoder.h

2004-12-17 Kimmo
 PATCH: [ 1086601 ] Dircompare statepane miscounting skipped items
  Src: DirCompStateBar.cpp
 PATCH: [ 1086745 ] Find- and Replace-dialogs to remember strings too
  Src/editlib: ccrystaltextview.cpp ceditreplacedlg.cpp ceditreplacedlg.h
   cfindtextdlg.cpp cfindtextdlg.h

2004-12-15 Tim
 RFE: [ 1085099 ] 'fileskip.bmp' looks to similar to 'equal.bmp'
  Src/res: fileskip.bmp
  Languages: fileskip.bmp

2004-12-05 WinMerge experimental release 2.3.1.1 (cvs)

2004-12-04 Kimmo
 PATCH: [ 1078505 ] Use DOS eol as filename/pathname clipboard separator
  Src: DirView.cpp
 PATCH: [ 981218 ] Show filenames/paths in caption
  Src: DirDoc.cpp DirDoc.h DirView.cpp DirView.h MainFrm.cpp MergeDoc.cpp MergeDoc.h 

2004-12-03 Kimmo
 PATCH: [ 1068011 ] Partial French translation
  Submitted by Matthieu Pupat
  Src/Languages/French: MergeFrench.rc
  Docs/Users: Contributors.txt

2004-12-01 Kimmo
 PATCH: [ 1075591 ] Disable file filter edit button for no filter selection
  Src: FileFiltersDlg.cpp FileFiltersDlg.h
 PATCH: [ 1062082 ] Fix bug [1057997] (long lines issue)
  Additional optimisation from Takashi Sawanaka
  Src/editlib: ccrystaltextview.cpp
 
2004-11-27 Jochen
 Remove unneeded system.cpp (no longer present in 7z411+) from Merge7z DSPs
  Merge7z: Merge7z311.dsp Merge7z312.dsp Merge7z313.dsp
  Merge7z407.dsp Merge7z408.dsp Merge7z409.dsp Merge7z410.dsp Merge7z411.dsp Merge7z412.dsp

2004-11-20 Kimmo
 PATCH: [ 1058142 ] update coloring of batch file
  Submitted by Andre Arpin
  Src/editlib: batch.cpp

2004-11-17 Kimmo
 PATCH: [ 1067374 ] Norwegian translation for 2.2.0
  Submitted by Hans Fr. Nordhaug
  Src/Languages/Norwegian: MergeNorwegian.rc

2004-11-14 Kimmo
 PATCH: [ 1065462 ] Show all skipped items when selected so
  Src: DirDoc.cpp

2004-11-12 Kimmo
 PATCH: [ 1057062 ] Separate Skipped flags from compare result flags
  Src: DiffContext.h DirCompStateBar.cpp DirDoc.cpp DirScan.cpp
   DirView.cpp DirViewColItems.cpp

2004-11-11 Kimmo
 PATCH: [ 1063342 ] Fix problem in XML syntax highlight and backslash
  Src/editlib: xml.cpp
 BUG: [ 1063100 ] Files with a space not handled (Clearcase integration)
  Src: MainFrm.cpp
 PATCH: [ 1059785 ] Simplify dir compare item update after copy etc
  Src: DiffContext.cpp DiffContext.h DiffFileInfo.cpp DiffFileInfo.h
   DirActions.cpp DirDoc.cpp DirDoc.h

2004-11-09 Jochen
 PATCH: [ 1062082 ] Fix bug [1057997] (long lines issue)
  Src/editlib: ccrystaltextview.cpp ccrystaltextview.h
 PATCH: [ 1059604 ] Fix bug [1048997] archive popup interferes with Help menu
  Src: 7zCommon.cpp

2004-11-09 Perry
 PATCH: [ 1050329 ] readme-developers-InnoSetup.html
  Docs/Developers: readme-developers-InnoSetup.html

2004-11-07 Kimmo
 BUG: [ 828065 ] More overlapping letters in Dansk/Norsk
  Fix from Christian
  Src/Languages/Danish: MergeDanish.rc 

2004-11-06 Perry
 PATCH: [ 1053475 ] Exponentially grow long lines when reading
  Src/Common: UniFile.cpp UniFile.h
  Src: Merge.dsp MergeDoc.cpp
 Fix UniFile.cpp for UNICODE build (PATCH: [ 1053475 ]).

2004-11-05 Kimmo
 PATCH: [ 1061155 ] Updated danish translation
  Submitted by Christian
  Src/Languages/Danish: MergeDanish.rc

2004-11-03 Kimmo
 PATCH: [ 1058992 ] More flexible whitespaces for file filter files
  Src: FileFilterMgr.cpp
 BUG: [ 1058947 ] Filter files sensitive to spaces
  Add new file filter for ADAMulti (from jpizzi)
  Filters new file: ADAMulti.flt
 PATCH: [ 1053282 ] Korean translation
  Submitted by Sukjoon, Hwang
  Src/Languages/Korean: MergeKorean.rc

2004-11-02 Kimmo
 PATCH: [ 1058293 ] The finished german language file
  Submitted by Tim Gerundt
  Src/Languages/German: MergeGerman.rc
 BUG: [ 1057699 ] GetModTime() in DirDoc.cpp is unused
  Src: DirDoc.cpp

2004-11-01 Kimmo
 PATCH: [ 1057719 ] Add Hungarian translator to Contributors list
  Docs/Users: Contributors.txt

2004-10-31 Perry
 PATCH: [ 1036683 ] unicoder.cpp always use GetACP codepage (remove dead variable)
  Src/Common: unicoder.cpp

2004-10-30 Perry
 PATCH: [ 1055624 ] The finished hungarian language file
  (translation credit in versioninfo)
 Src/Languages: Hungarian/MergeHungarian.rc

2004-10-29 Jochen
 Add Merge7z410.dsp
  Merge7z: Merge7z410.dsp

2004-10-29 Kimmo
 PATCH: [ 1055681 ] Add Bool type for options and convert existing options
  Src: DirActions.cpp DirDoc.cpp DirView.cpp MainFrm.cpp MergeDiffDetailView.cpp
   MergeDoc.cpp MergeEditView.cpp OpenDlg.cpp
  Src/Common: OptionsMgr.cpp OptionsMgr.h varprop.h

2004-10-28 Kimmo
 PATCH: [ 1055551 ] Move cursor to begin of previous line when deleting diff
  Src: MergeDoc.cpp MergeDoc.h MergeEditView.cpp
 PATCH: [ 1053234 ] Suggested expansions for Organization.txt
  Submitted by Perry
  Docs/Developers: Organisation.txt

2004-10-28 Perry
 PATCH: [ 1055067 ] Add Hungarian support
  Src/Common: LanguageSelect.cpp
 PATCH: [ 1055624 ] The finished hungarian language file
  Src/Languages: Hungarian/*
 PATCH: [ 1053421 ] Changing warning level to 3 in release targets
  Src: Merge.dsp

2004-10-27 Perry
 PATCH: [ 1003457 ] Move difflist to new class DiffList
  Fix warnings for signed/unsigned comparison
  Src: DiffList.cpp

2004-10-26 Kimmo
 PATCH: [ 1003457 ] Move difflist to new class DiffList
  Last minute updates to files I forgot to add (thanks Perry!)
  Src: DiffList.cpp DiffList.h

2004-10-25 Perry
 PATCH: [ 1003457 ] Move difflist to new class DiffList
  Src: DiffList.cpp DiffList.h

2004-10-25 Kimmo
 PATCH: [ 1003457 ] Move difflist to new class DiffList
  Src: DiffWrapper.cpp DiffWrapper.h LocationView.h Merge.dsp
   MergeDiffDetailView.cpp MergeDoc.cpp MergeDoc.h MergeEditView.cpp
 BUG: [ 1052762 ] WinMerge VERY slow to startup
  Src: BCMenu.cpp

2004-10-24 WinMerge experimental release 2.1.7.16 (cvs) (Perry)

2004-10-24 Perry
 PATCH: [ 1053282 ] Korean translation
 Src/Languages/Korean: MergeKorean.rc

2004-10-24 Kimmo
 PATCH: [ 1048288 ] Fixes to Organization.txt
  Submitted by Perry
  Docs/Developers/Organisation.txt

2004-10-24 Perry
 BUG: [ 1048439 ] UNICODE build calling IsDBCSLeadByte (shouldn't)
  Src: stringdiffs.cpp
  Src/editlib: ccrystaltextview.cpp

2004-10-23 Kimmo
 BUG: [ 1052283 ] after edit, asks TWICE for rewriting the original file (-e switch)
  Src: MergeDoc.cpp
 Clarify comment about closing app in CMainFrame::PreTranslateMessage()
  Src: MainFrm.cpp

2004-10-22 Perry
 PATCH: [ 966908 ] find byte difference in line
 Src: Merge.rc MergeDiffDetailView.cpp MergeDiffDetailView.h
  MergeDoc.h MergeDocLineDiffs.cpp MergeEditView.cpp
  MergeEditView.h resource.h stringdiffs.cpp stringdiffs.h

2004-10-19 Perry
 PATCH: [ 1049688 ] Fix caret handling of displayable whitespace
 Src/editlib: ccrystaltextview.cpp

2004-10-18 Kimmo
 PATCH: [ 1048838 ] German translation updated for RC
  Submitted by Tim Gerundt
  Src/Languages/German: MergeGerman.rc

2004-10-18 Perry
 PATCH: [ 1048412 ] Log font info in configuration
  Src: ConfigLog.cpp ConfigLog.h MainFrm.cpp
 PATCH: [ 1036683 ] unicoder.cpp always use GetACP codepage
  Src/Common: unicoder.cpp

2004-10-17 Perry
 PATCH: [ 1048405 ] Delete old, commented out line in ConfigLog.cpp
  Src: ConfigLog.cpp

2004-10-17 Jochen
 PATCH: [ 1043909 ] Add support for 7-Zip 4.09, plus DSPs for 4.07 and 4.08
 Leave decision whether to recurse into folders to enumerator
 Fix leading zero in 7-Zip minor version numbers shown in "archive support disabled" message
  Merge7z: Merge7z.def Merge7z.h Merge7z311.cpp Merge7z407.dsp Merge7z408.dsp Merge7z409.cpp
   Merge7z409.dsp Merge7zCommon.cpp Merge7zCommon.h revision.txt StdAfx.h
  Src: 7zCommon.cpp 7zCommon.h

2004-10-16 Kimmo
 PATCH: [ 1046546 ] Open new manual locally
  Src: MainFrm.cpp

2004-10-15 Kimmo
 PATCH: [ 1046583 ] Improved 48x48 icon
  Submitted by Tim Gerundt
  Src/res: Merge.ico
  Src/Languages/*/res: Merge.ico
 PATCH: [ 1047670 ] Catalan translation update
  Submitted by Jordi Vilar
  Src/Languages/Catalan: MergeCatalan.rc
 BUG: [ 1047812 ] number of differences not recalculated (merging whole files)
  Src: DirActions.cpp

2004-10-15 WinMerge experimental release 2.1.7.15 (cvs) (Perry)

2004-10-14 Kimmo
 PATCH: [ 942447 ] CVS organisation doc
  Docs/Developers new file: Organisation.txt

2004-10-13 Kimmo
 PATCH: [ 1045473 ] Disable Ctrl+Arrow merging in Mergemode
  Src: MergeEditView.cpp
 Additional change for patch [ 1045473 ]
  Src: MergeEditView.cpp

2004-10-12 Perry
 PATCH: [ 1043819 ] Fix recent compiler warnings in src/util.c
  diffutils: src/util.c

2004-10-11 Perry
 PATCH: [ 1040016 ] patch for BUG[1039615] caret splitting wide characters
  Src/editlib: ccrystaltextview.cpp

2004-10-09 Kimmo
 BUG: [ 1035227 ] Selected font change won't be applied... when not needed
  Src: MainFrm.cpp
 PATCH: [ 1043246 ] Fix problem selecting <none> as filefilter
  Src: FileFilterHelper.cpp FileFiltersDlg.cpp
 PATCH: [ 1043367 ] Change brace indentation automation for C/C++
  Src/editlib: ccrystaltextview.cpp

2004-10-06 Kimmo
 PATCH: [ 1040041 ] [ 1039640 ] reproducible tooltip ASSERT in debug
  Submitted by Takashi Sawanaka
  Src: MergeEditView.cpp

2004-10-05 Kimmo
 PATCH: [ 1039552 ] Italian translation updated for RC
  Submitted by Andrea Decorte
  Src/Languages/Italian: MergeItalian.rc
 PATCH: [ 1034854 ] Use "Terminal" font instead of "Courier" as default font.
  Submitted by Takashi Sawanaka
  Src: MainFrm.cpp

2004-10-04 Kimmo
 BUG: [ 1037587 ] XML Syntax highlighting: Problems with numbers
  Src/editLib: xml.cpp
 PATCH: [ 1039552 ] Italian translation updated for RC
  Submitted by Andrea Decorte
  Src/Languages/Italian: MergeItalian.rc
 PATCH: [ 1039495 ] Trap diffutils errors with SEH
  Src: DiffWrapper.cpp DiffWrapper.h
  Src/diffutils/src: UTIL.C
 PATCH: [ 1003107 ] Remove unused CMergeDoc::CountPrevBlanks()
  Src: MergeDoc.cpp MergeDoc.h

2004-10-04 Perry
 PATCH: [ 1039300 ] Factor listviewex code so it can be used with a CListCtrlEx.
  Src/Common: listvwex.cpp listvwex.h

2004-10-03 Perry
 PATCH:[ 1037449 ] Fix char width computation for displayable whitespace
  Src/editlib: ccrystaltextview.cpp ccrystaltextview.h

2004-10-03 Kimmo
 PATCH: [ 1039096 ] Save ignore EOL option when changed during file loading
  Src: MergeDoc.cpp
 PATCH: [ 1039372 ] Save mixed EOL option when changed during file loading
  Src: MergeDoc.cpp
 PATCH: [ 1039552 ] Italian translation updated for RC
  Submitted by Andrea Decorte
  Src/Languages/Italian: MergeItalian.rc

2004-10-02 Kimmo
 PATCH: [ 1039001 ] Swedish translation update
  Submitted by Hans Eriksson
  Src/Languages/Swedish: MergeSwedish.rc

2004-10-01 Perry
 PATCH: [ 1038749 ] Fix whitespace display
  Src/editlib: ccrystaltextview.cpp
 PATCH: [ 1036558 ] Cosmetic cleanup in CMergeApp::DoMessageBox
  Src: Merge.cpp

2004-10-01 Kimmo
 PATCH: [ 1038091 ] Various Updates in German Translation
  Submitted by Tim Gerundt
  Src/Languages/German: MergeGerman.rc
 PATCH: [ 1038361 ] Left/Right read-only cmdline arguments wrong way around
  Submitted by markanthony
  Src: MainFrm.cpp

2004-09-30 2.2RC (2.1.8.0) released

2004-09-30 Perry
 Fix bracketing mistake in PATCH: [ 1034810 ]
  Src/editlib: ccrystaltextview.cpp
 PATCH: [ 1034754 ] Fix demoGuessEncoding's default
  Src: codepage.cpp codepage.h DiffWrapper.cpp
 PATCH: [ 1037418 ] Fix whitespace for Unicode non-1252
 PATCH: [ 1037424 ] fix non-unicode initialization loop in ViewableWhitespace
  Src: ViewableWhitespace.cpp

2004-09-29 Perry
 PATCH: [ 1034810 ] fix whitespace for other encodings
  Src: Merge.dsp ViewableWhitespace.cpp ViewableWhitespace.h
  Src/editlib: ccrystaltextview.cpp

2004-09-29 Kimmo
 PATCH: [ 1036134 ] Show error about invalid linefilter regexps
  Src: MainFrm.cpp MainFrm.h Merge.rc resource.h
  Src/diffutils/lib: REGEX.C REGEX.H
  Src/Languages: *.rc resource.h

2004-09-28 Kimmo
 PATCH: [ 1034719 ] IDS_SUGGEST_IGNORECODEPAGE change?
  Submitted by Perry
  Src: Merge.rc
  Src/Languages: *.rc (except MergeCatalan.rc, MergeItalian.rc & MergeSwedish.rc)
 PATCH: [ 1002247 ] Remove unneeded CDirView::OnCustomDraw()
  Src: DirView.cpp DirView.h

2004-09-27 Kimmo
 Add links to DocBook DSSL customisation
  Docs/Developers: readme-developers.html

2004-09-27 Perry
 PATCH: [ 968285 ] DBCS fixes from Sawanaka
  The crystaltextview.cpp portion of patch, with a new method (GetCharWidthFromChar) to reduce ifdefs.
  Src/EditLib: ccrystaltextview.cpp

2004-09-25 Kimmo
 PATCH: [ 1034550 ] Catalan translation update
  Submitted by Jordi Vilar
  Src/Languages/Catalan: MergeCatalan.rc
 PATCH: [ 1034557 ] Swedish translation update
  Submitted by Hans Eriksson
  Src/Languages/Swedish: MergeSwedish.rc
 PATCH: [ 1034642 ] Updated Japanese Translation
  Submitted by Takashi Sawanaka
  Src/Languages/Japanese: MergeJapanese.rc

2004-09-24 Kimmo
 PATCH: [ 1033678 ] Better description for 'Reset messageboxes'
  Src: Merge.rc
  Src/Languages: *.rc
 Open WinMerge manual from http://winmerge.org/2.2/manual/index.html
  Src: MainFrm.cpp
 BUG: [ 1034019 ] Italian: language list unsorted
  Src/Languages/Italian: MergeItalian.rc

2004-09-23 Kimmo
 PATCH: [ 1032841 ] Add 'DiffEngine' filegroup to project file
  Src: Merge.dsp

2004-09-22 Kimmo
 BUG: [ 1029646 ] (Norwegian) Options/Editor text chopped off
  Fix all languages
  Src: Merge.rc
  Src/Languages: *.rc
 BUG: [ 1029637 ] (Italian) Options/Codepage detect codepage label cut-off
  Fix all languages
  Src: Merge.rc
  Src/Languages: *.rc

2004-09-21 Kimmo
 Widen space for text in language change dialog
  Src: Merge.rc
  Src/Languages: *.rc

2004-09-20 Kimmo
 PATCH: [ 1030752 ] Set focus to file view when goto line in location pane
  Src: LocationView.cpp
 BUG: [ 1026833 ] WinMergeU.exe build to dir MergeUnicodeReleaseMinsize
  Build WinMergeU.exe to Build\MergeUnicodeRelease\
  Src: Merge.dsp

2004-09-19 Kimmo
 PATCH: [ 1030490 ] Updated Swedish Translation
  Submitted by Hans Eriksson
  Src/Languages/Swedish: MergeSwedish.rc
 BUG: [ 1029644 ] (English) Options/System wording fix
  Src: Merge.rc
 More space for texts in Options/System
  Src: Merge.rc
  Src/Languages: *.rc

2004-09-17 Kimmo
 PATCH: [ 1029538 ] Reverse 'Ignore carriage return differences' when resetting
  Src: PropCompare.cpp
 BUG: [ 1029647 ] (English) Options/General text cut-off
  Fixed other languages also
  Src: Merge.rc
  Src/Languages: *.rc (except MergeCatalan.rc)

2004-09-16 Perry
 Fix Italian editor property page (tabs section) & widen corresponding English controls
  Src: Merge.rc
  Src/Languages: Italian/MergeItalian.rc

2004-09-15 WinMerge experimental release 2.1.7.14 (cvs)

2004-09-15 Kimmo
 PATCH: [ 1027936 ] Fix statusbar display for ghost lines
  Src: MergeEditView.cpp
 PATCH: [ 1027994 ] Use current filename and path as defaults for SaveAs dialog
  Src: MergeDoc.cpp
 BUG: [ 1027605 ] Esc (as indicated by File Exit) doesn't quit app
  Remove ESC from menuitem. Also fix merge-mode shortcut to F9.
  Src: Merge.rc
  Src/Languages: *.rc
 PATCH: [ 1026619 ] Fix bug [ 1017264 ] Location pane contextmenu items...
  Submitted by Jochen (jtuc)
  Src: LocationView.cpp

2004-09-14 Kimmo
 PATCH: [ 1027118 ] Separate items for Copy Path in dircompare context menu
  Src: DirView.cpp DirView.h Merge.rc resource.h
  Src/Languages: *.rc resource.h
 PATCH: [ 1027382 ] Customised Save As dialog captions
  Src: Merge.rc MergeDoc.cpp resource.h
  Src/Languages: *.rc resource.h
 Add Lee Jae-Hong as Korean translator contributor
  Docs/Users: Contributors.txt
 Remove declaration of non-existent function DeleteFileOrError()
  Src: MainFrm.h
 Remove declaration of non-existent function CleanupFileBufs()
  Src: MainFrm.h
 Don't hide rescan result for binary file compare
  Src: MergeDoc.cpp

2004-09-13 Kimmo
 PATCH: [ 1026894 ] More space for translations in Goto-dialog
  Src: Merge.rc
  Src/Languages: *.rc
 PATCH: [ 1027086 ] Catalan translation Update
  Submitted by Jordi Vilar
  Src/Languages/Catalan: MergeCatalan.rc
 Add note about changed commandline when using docbook2html in SF.net
  Docs/Developers: readme-developers.html
 PATCH: [ 1027183 ] Swedish Translation Update
  Submitted by Hans Eriksson
  Src/Languages/Swedish: MergeSwedish.rc
 Fix couple of lang names in Swedish translation
  Submitted by Hans Eriksson
  Src/Languages/Swedish: MergeSwedish.rc

2004-09-12 Kimmo
 Fix Italian lang file build error and remove unneeded \n
  Src/Languages/Italian: MergeItalian.rc

2004-09-10 Kimmo
 PATCH: [ 1025411 ] Cleanup PropVSS.cpp
  Src: PropVss.cpp
 PATCH: [ 1025443 ] Add '1 Difference Found' text
  Src: Merge.rc MergeDoc.cpp resource.h
  Src/Languages: *.rc resource.h
 Add instructions for compiling lang files (using MakeResDll.exe)
  Docs/Developers: readme-developers.html

2004-09-09 Kimmo
 PATCH: [ 1025052 ] Italian translation updated for latest experimental version
  Submitted by Andrea Decorte
  Src/Languages/Italian: MergeItalian.rc
 PATCH: [ 1023074 ] Fix and cleanup version control options dialog
  Src: Merge.rc
  Src/Languages: *.rc

2004-09-08 Kimmo
 PATCH: [ 1023917 ] Don't close dirview when mergedocs closed by reusing code
  Fixes bug #1023412 Crash in WinMerge.exe 2.1.7.13
  Src: DirDoc.cpp DirDoc.h

2004-09-03 WinMerge experimental release 2.1.7.13 (cvs)

2004-09-03 Kimmo
 PATCH: [ 1021153 ] Fix ansi files EOL detection in unicode builds
  Src/Common: UniFile.cpp
 BUG: [ 986708 ] Wrong EOL character
  DOS EOL was inserted when editing unix or mac files
  Src/editlib: ccrystaleditview.cpp
 PATCH: [ 1021478 ] Korean Resource file update
  Submitted by Lee Jae-Hong
  Src/Languages/Korean: MergeKorean.rc

2004-09-02 Kimmo
 PATCH: [ 1020403 ] Allow WinMerge to open files write-locked by another app
  Src/Common: UniFile.cpp
 Fix for patch #1019108: update rescan info also after saving
  Src: MergeDoc.cpp
 BUG: [ 1021003 ] Unicode builds do not read ansi files properly (UniFile.cpp)
  Fix what seems to be invalid #ifdef'ing for unicode build
  Src/Common: UniFile.cpp
 PATCH: [ 1014455 ] more explicit error message if archive support not available
  Submitted by Jochen
  Src: 7zCommon.cpp 7zCommon.h MainFrm.cpp MainFrm.h Merge.rc resource.h
  Src/Languages: *.rc resource.h

2004-09-01 Kimmo
 PATCH: [ 1019759 ] Allow D&D projectfile to main window
  Src: MainFrm.cpp
 PATCH: [ 1019945 ] Don't fail configlog writing silently
  Src: ConfigLog.cpp ConfigLog.h MainFrm.cpp
 PATCH: [ 1019393 ] Inform user about missing archive support (Zip* menuitems)
  Src: 7zCommon.cpp 7zCommon.h DirView.cpp Merge.rc resource.h
  Src/Languages: *.rc resource.h

2004-08-31 Kimmo
 PATCH: [ 1019108 ] Warn if file changed on disk between rescans
  Src: Merge.rc MergeDoc.cpp MergeDoc.h resource.h
  Src/Languages: *.rc resource.h
 Add note about overwriting WinMerge executable path for ShellExtension
  Docs/Developers: readme-developers.html

2004-08-27 WinMerge experimental release 2.1.7.12 (cvs)

2004-08-27 Kimmo
 PATCH: [ 1017294 ] When closing file compare close also empty directory compare
  Src: DirDoc.cpp

2004-08-26 Kimmo
 BUG: [ 1016056 ] Panes don't remember settings when new file is opened
  Src: ChildFrm.cpp
 PATCH: [ 1016282 ] Add GetDefault() to COptionsMgr
  Src: PropColors.cpp
  Src/Common: OptionsMgr.cpp OptionsMgr.h
 Convert Compare options dialog for GetDefault()
  Src: PropCompare.cpp

2004-08-25 Kimmo
 PATCH: [ 1008755 ] avoid flickering when diff view opens
  Don't update desktop icon (from Takashi Sawanaka)
  Src: ChildFrm.cpp
 PATCH: [ 1015330 ] Better file saving error message
  Src: Merge.rc MergeDoc.cpp MergeDoc.h
  Src/Common: UniFile.cpp
  Src/Languages: *.rc
 PATCH: [ 1015393 ] Strip whitespaces from paths in Open-dialog
  Src: OpenDlg.cpp OpenDlg.h

2004-08-23 Kimmo
 PATCH: [ 1013715 ] Catalan translation update
  Submitted by Jordi Vilar
  Src/Languages/Catalan: MergeCatalan.rc
 Check path length in VSS integration
  Src: VSSHelper.cpp
 PATCH: [ 1012328 ] Simpler Zip* menuitems
  Src: DirView.cpp Merge.rc
  Src/Languages: *.rc

2004-08-20 Kimmo
 PATCH: [ 1012291 ] Open Find-dialog for F3 if no previous find
  Src/editlib: ccrystaltextview.cpp
 PATCH: [ 1012192 ] Show selected items count in dir compare statusbar
  Src: DirView.cpp DirView.h Merge.rc resource.h
  Src/Languages: resource.h *.rc

2004-08-19 Kimmo
 PATCH: [ 982140 ] Merge7z : compress folders
  Submitted by Laoran
  Merge7z: Merge7z311.cpp Merge7z311.dsp Merge7z312.dsp Merge7z313.dsp
 PATCH: [ 981788 ] modify UI to compress folders
  Submitted by Laoran
  Src: DirView.cpp
 RFE: [ 953630 ] Ctrl+Shift+Z for Redo in file comparison
  Src: Merge.rc
  Src/Languages: *.rc

2004-08-18 Kimmo
 RFE: [ 952959 ] split Are you sure you want to copy into more lines
  Src: Merge.rc
  Src/Languages: *.rc
 PATCH: [ 1011066 ] Better error message for failed file loading
  Src: MergeDoc.cpp MergeDoc.h
  Src/Common: UniFile.cpp
 PATCH: [ 1002432 ] codepage UI version 2
  Patch by Perry and Laoran
  Src: ConfigLog.cpp ConfigLog.h DirDoc.cpp DirScan.cpp MainFrm.cpp Merge.dsp
   Merge.rc OptionsDef.h PropGeneral.cpp PropGeneral.h resource.h
  Src new files: codepage.cpp codepage.h PropCodepage.cpp PropCodepage.h
  Src/Common: LanguageSelect.cpp multiformatText.cpp unicoder.cpp unicoder.h unifile.cpp
  Src/Languages: resource.h *.rc

2004-08-17 WinMerge experimental release 2.1.7.11 (cvs)

2004-08-17 Kimmo
 PATCH: [ 1010387 ] Updated MergeSwedish.rc file
  Submitted by Hans Eriksson
  Src/Languages/Swedish: MergeSwedish.rc
 PATCH: [ 975840 ] Cleanup VCS/VSS code part 2
  Src: MainFrm.cpp MainFrm.h Merge.dsp 
  Src new files: VSSHelper.cpp VSSHelper.h

2004-08-16 Kimmo
 BUG: [ 1002035 ] Crash with locked files (WM v2.1.7.10)
  Src: MainFrm.cpp
 BUG: [ 1002035 ] Crash with locked files (WM v2.1.7.10)
  Src: MergeDoc.cpp
 BUG: [ 1009918 ] Binary files not detected (unicode builds)
  Src/Common: UniFile.cpp

2004-08-14 Kimmo
 PATCH: [ 1008755 ] avoid flickering when diff view opens
  Submitted by Takashi Sawanaka
  Src: ChildFrm.cpp DiffViewBar.cpp
 Don't post ID_FORCE_REDRAW in CCrystalTextView::OnSize()
  Suggested by Takashi Sawanaka in patch item #1008755
  Src/editlib: ccrystaltextview.cpp

2004-08-13 Kimmo
 BUG: [ 1007320 ] VB Syntax highlighting: Wrong masking from "
  Src/editlib: basic.cpp

2004-08-12 Kimmo
 PATCH: [ 1007212 ] Make extension matching in Open-dialog case insensitive
  Src: FileFilterHelper.cpp
 PATCH: [ 1007216 ] Allow '*.' to match files without extension in Open-dialog
  Src: FileFilterHelper.cpp
 PATCH: [ 1007627 ] Filter selection fixes
  Src: FileFilterHelper.cpp FileFilterHelper.h MainFrm.cpp OpenDlg.cpp
   OpenDlg.h OptionsDef.h

2004-08-11 Kimmo
 PATCH: [ 1006475 ] Don't save editor settings to different key in registry
  Src/editlib: editreg.h
 PATCH: [ 1006870 ] Don't add '\' to filenames for regexp filtering
  Src: FileFilterHelper.cpp
 Backout PATCH: [ 999738 ] Gray out moved block menuitems when moved blocks disabled
  Those menuitems didn't work like I expected
  Src: LocationView.cpp

2004-08-10 Kimmo
 BUG: [ 1006164 ] accelerator conflict
  Src: Merge.rc
 PATCH: [ 1003181 ] Move filter options to OptionsManager
  Src: MainFrm.cpp MainFrm.h OptionsDef.h

2004-08-04 Kimmo
 PATCH: [ 1001981 ] Italian translation updated
  Submitted by Andrea Decorte
  Src/Languages/Italian: MergeItalian.rc
 PATCH: [ 1002585 ] Update dir compare statepane counts when reopening
  Src: DirView.cpp
 PATCH: [ 1002887 ] Enable Change Pane menuitem for Diff Pane
  Src: MergeDiffDetailView.cpp MergeDoffDetailView.h
 PATCH: [ 1000020 ] Simple project files (no GUI)
  Src: MainFrm.cpp MainFrm.h Merge.cpp Merge.dsp Merge.h OpenDlg.cpp OpenDlg.h
  Src new files: ProjectFile.cpp ProjectFile.h
 Option OPT_HIDE_BACKUP didn't get removed when UI was removed, remove it now
  Src: ConfigLog.cpp ConfigLog.h DirDoc.cpp MainFrm.cpp OptionsDef.h
 Backout previous 'Option OPT_HIDE_BACKUP didn't get removed..' checkin.
  It caused build with VC6 to crash in dir compare -- really weird why
  Src: ConfigLog.cpp ConfigLog.h DirDoc.cpp MainFrm.cpp OptionsDef.h

  
2004-08-03 Kimmo
 PATCH: [ 1002333 ] Set default filter to *.* when starting WinMerge
  Src: Merge.cpp
 PATCH: [ 1002280 ] Don't show dircompare statepane if no dircompare results
  Src: DirView.cpp
 Remove GPL licence from coretools.cpp/h - that code is mostly freeware, not GPL
  Src/Common: coretools.cpp coretools.h
 PATCH: [ 1001974 ] Allow ESC to stop dir compare
  Src: DirCompStateBar.cpp
 PATCH: [ 1000423 ] Try to fix invisible find result
  Updated patch
  Src/editlib: ccrystaltextview.cpp

2004-08-02 Kimmo
 PATCH: [ 1001111 ] Warn if user selected binary files for patch creating
  Src: MainFrm.cpp Merge.rc PatchTool.cpp resource.h
  Src/Languages: *.rc resource.h
 PATCH: [ 999804 ] Fix setting recursive for Open-dialog
  New patch fixing bug #1001301 recursive not getting remembered
  Src: MainFrm.cpp MainFrm.h Merge.cpp

2004-08-01 Perry
 PATCH: [ 967405 ] Add Japanese satellite dll to MakePatchDirs
  MakePatchDirs: MakePatchDirs.dsp MakePatchDirs.rc
   MakePatchDirsApp.cpp MakePatchDirsDlg.cpp MakePatchDirsDlg.h
   resource.h StdAfx.h
 PATCH: [ 967405 ] Add Japanese satellite dll to MakePatchDirs
 (Adding new files forgotten at first check-in)
  MakePatchDirs: MakePatchDirs_ja.dsp MakePatchDirs_ja.rc 
  MakePatchDirs_sat.cpp Satellites.cpp Satellites.h

2004-07-31 Perry
 PATCH: [ 989288 ] Inno Setup Localization: Italian
  InnoSetup\Languages: Italian.isl
 PATCH: [ 963429 ] Fix language in floating status bar
  Src: DirCompStateBar.cpp DirCompStateBar.h DirDoc.cpp DirFrame.cpp DirFrame.h 
   DirView.cpp

2004-07-31 Kimmo
 PATCH: [ 1000714 ] Don't create difflist or patch file for binary files
  Src: DiffWrapper.cpp
 PATCH: [ 1000772 ] Fix focus in SaveClosing dialog
  Src: SaveClosingDlg.cpp

2004-07-30 Kimmo
 PATCH: [ 999738 ] Gray out moved block menuitems when moved blocks disabled
  Src: LocationView.cpp
 PATCH: [ 1000117 ] Hide only skipped subfolders in non-recursive compare
  Fixes bug #922718 'show different files' off : subdirs not displayed
  Src: DirDoc.cpp DirDoc.h
 PATCH: [ 1000308 ] Add GotoLine() to CMergeEditView and use it
  Src: LocationView.cpp LocationView.h MergeEditView.cpp MergeEditView.h
 Add icons to Find and Replace result messageboxes.
  Src/editlib: ceditreplacedlg.cpp cfindtextdlg.cpp
 PATCH: [ 1000423 ] Try to fix invisible find result
  Bug: #951584 Find: word found not visible or badly placed
  Src: MergeDocLineDiffs.cpp
  Src/editlib: ccrystaltextview.cpp ccrystaltextview.h

2004-07-29 Kimmo
 PATCH: [ 999681 ] Accept filter name from commandline
  Src: FilterHelper.cpp FilterHelper.h Merge.cpp Merge.rc
  Src/Languages: *.rc
 PATCH: [ 999804 ] Fix setting recursive for Open-dialog
  Src: MainFrm.cpp OpenDlg.cpp OpenDlg.h

2004-07-28 Kimmo
 Remove suppressing checkbox from commandline help messagebox
  Src: Merge.cpp

2004-07-23 Perry
 PATCH: [ 988505 ] updated email address for catalan translator
  Update email address for Jordi Vilar (Catalan translator)
   Web\developer\l10n: status.xml
 PATCH: [ 988503 ] Inno Setup Localization: Catalan
  InnoSetup\Languages: Catalan.isl

2004-07-23 WinMerge experimental release 2.7.1.10 (cvs)

2004-07-19 Perry
 BUG: [ 993402 ] Need review of native language names
  Removed asterisks from Nederlands & Svenska (thanks to A. Decorte).
   Src/Common: LanguageSelect.cpp
   
2004-07-18 Perry
 BUG: [ 969933 ] 'discriminated' language names
  Capitalized native language names to make consistent in language selection box
   Src/Common: LanguageSelect.cpp

2004-07-17 Kimmo
 PATCH: [ 992218 ] Add -f commandline switch for filemask
  Usage: WinMerge -f "*.ext1 *.ext2" (like in Open-dialog)
  Src: FileFilterHelper.cpp FileFilterHelper.h MainFrm.cpp Merge.cpp
   Merge.rc OpenDlg.cpp OpenDlg.h
  Src/Languages: *.rc
 
2004-07-16 Kimmo
 PATCH: [ 990976 ] FileFilterHelper (filter code cleanup)
  Src: DiffContext.cpp DiffContext.h DirDoc.cpp DirDoc.h DirScan.cpp
   FileFiltersDlg.cpp FileFilterDlg.h MainFrm.cpp Merge.cpp Merge.dsp
   Merge.h OpenDlg.cpp
  Src new files: FileFilterHelper.cpp FileFilterHelper.h
 PATCH: [ 986393 ] Lose '(debug)' from 'Reload plugins'
  Src: Merge.rc
  Src/Languages: *.rc
 PATCH: [ 941621 ] widen find/replace dialog
  Find-dialog
  Src: Merge.rc
  Src/Languages: *.rc
 PATCH: [ 941621 ] widen find/replace dialog
  Replace-dialog
  Src: Merge.rc
  Src/Languages: *.rc
 PATCH: [ 990901 ] Add Move to DirView Context menu commands.
  Submitted by Vladimir Lukianov
  Src: DirActions.cpp DirView.cpp DirView.h Merge.rc resource.h
  Src/Languages: *.rc resource.h

2004-07-15 Kimmo
 PATCH: [ 991181 ] Disable 'Goto Line' outside reduced locationpane bars
  Src: LocationView.cpp
 PATCH: [ 990365 ] Dialog to ask about saving modified files
  Src: Merge.dsp Merge.rc MergeDoc.cpp resource.h
  Src new files: SaveClosingDlg.cpp SaveClosingDlg.h
  Src/Languages: *.rc resource.h

2004-07-14 Kimmo
 PATCH: [ 988494 ] Catalan translation update
  Submitted by Jordi Vilar
  Src/Languages/Catalan: MergeCatalan.rc 
  Src/Languages/Catalan/res: splash1.bmp

2004-07-08 Kimmo
 PATCH: [ 987122 ] Horizontal scroll for dirview columns dialog listboxes
  Src: DirColsDlg.cpp DirColsDlg.h Merge.dsp Merge.rc
  Src/Common new files: HScrollListBox.cpp HScrollListBox.h
  Src/Languages: *.rc

2004-07-08 Kimmo
 PATCH: [ 986794 ] Hide 'Apply' button from filters dialog
  Src: MainFrm.cpp
 PATCH: [ 986785 ] Improve location pane with short files
  Src: LocationView.cpp LocationView.h
 PATCH: [ 986873 ] Allow recursive compares for archive files
  Src: OpenDlg.cpp

2004-07-07 Kimmo
 PATCH: [ 983265 ] Italian translation updated for latest experimental version
  Src/Languages/Italian: MergeItalian.rc

2004-07-04 WinMerge experimental release 2.1.7.9 (cvs)

2004-07-04 Perry
 Add lost piece of PATCH: [ 982960 ] Rescan when saving to update dir compare status
  Src: DirDoc.cpp

2004-07-03 Kimmo
 PATCH: [ 984520 ] Don't add '...' items as fileaction targets
  Src: DirActions.cpp

2004-07-02 Kimmo
 PATCH: [ 982960 ] Rescan when saving to update dir compare status
  Src: DiffContext.cpp DiffContext.h DirDoc.cpp DirDoc.h MergeDoc.cpp
   MergeDoc.h MergeEditView.cpp
 PATCH: [ 983410 ] Warn that New clears dircompare results
  Src: MainFrm.cpp Merge.rc resource.h
  Src/Languages: *.rc resource.h
 PATCH: [ 983721 ] Remove 'don't show again' checkbox from saving messages
  Src: MergeDoc.cpp
 PATCH: [ 984059 ] Disable merge actions for '...' item in dir compare
  Src: DirView.cpp

2004-07-01 Kimmo
 PATCH: [ 982683 ] Clear results only after user chooses new files or folders
  Src: MainFrm.cpp
 PATCH: [ 982703 ] Open manual when user presses F1
  Src: MainFrm.cpp MainFrm.h Merge.cpp Merge.h Merge.rc
  Src/Languages: *.rc

2004-06-29 Laoran
 BUG: [ 981796 ] Dir compare : rescan clears "auto-prediffer"
  Src: DirView.cpp
 PATCH: [ 981529 ] remove hide .BAK from menu
  Filters: Merge_GnuC_loose.flt Merge_VB_loose.flt Merge_VC_loose.flt
  Src: MainFrm.cpp MainFrm.h Merge.rc resource.h
  Src/Languages: *.rc resource.h

2004-06-29 Kimmo
 PATCH: [ 981468 ] Properly handle different commentstyles for Pascal
  Src/editlib: pascal.cpp
 PATCH: [ 981526 ] Don't ignore one-char lines in syntax highlight
  Src/editlib: asp.cpp basic.cpp batch.cpp cplusplus.cpp csharp.cpp dcl.cpp
   fortran.cpp html.cpp ini.cpp is.cpp java.cpp lisp.cpp pascal.cpp perl.cpp
   php.cpp python.cpp rexx.cpp rsrc.cpp siod.cpp sql.cpp tex.cpp xml.cpp

2004-06-28 Laoran
 PATCH: [ 981290 ] Updated Swedish MergeSwedish.rc file
 Submitted by Hans Eriksson 
  Src/Languages/Swedish: MergeSwedish.rc

2004-06-28 Kimmo
 PATCH: [ 980851 ] Don't save VSS password
  Src: MainFrm.cpp
 PATCH: [ 978987 ] [RFE 976364] Expand Environment Strings in paths
  Submitted by Jordi Vilar - minor tweak by Kimmo
  Src: Paths.cpp

2004-06-23 WinMerge experimental release 2.1.7.8 (cvs)

2004-06-23 Kimmo
 PATCH: [ 977587 ] Fix fileattrib handling and show all attribs in dir compare
  Src: DiffFileInfo.cpp DiffFileInfo.h DirScan.cpp DirView.cpp
 PATCH: [ 977796 ] Make dir compare column cust. dialog usable from keyboard
  Src: DirColsDlg.cpp DirColsDlg.h
 BUG: [ 936525 ] RO not removed in refresh
  Don't set files/dirs with RO fileflag RO in UI - this was bad idea!
  Src: DirView.cpp MainFrm.cpp
 BUG: [ 978062 ] VSS support broken
  Src: MainFrm.cpp
 PATCH: [ 978393 ] Better VSS errormessages
  Src: MainFrm.cpp MainFrm.h Merge.rc resource.h
  Src/Languages: *.rc resource.h

2004-06-22 Kimmo
 PATCH: [ 977475 ] Log DeleteFile() failure in FileTransform.cpp
  Src: FileTransform.cpp
 PATCH: [ 977479 ] Delete non-functional line in CPatchDlg::SaveSettings()
  Src: PatchDlg.cpp

2004-06-21 Kimmo
 BUG: [ 974662 ] Inconsistent menutexts for dircompare column dialog
  Scr: DirView.cpp Merge.rc resource.h
  Src/Languages: *.rc resource.h

2004-06-21 Laoran
 PATCH: [ 975187 ] getDefaultCodepage, use UI language only when UI not english
  Src/common: unicoder.cpp

2004-06-20 Kimmo
 PATCH: [ 972500 ] Italian translation updated
  Submitted by Andrea Decorte
  Src/Languages/Italian: MergeItalian.rc
 PATCH: [ 975025 ] Fix column number in file compare statusbar
  Src: MergeEditView.cpp
 RFE: [ 891123 ] Short format for line info in file compare
  Src: Merge.rc
  Src/Languages: *.rc
 PATCH: [ 976255 ] Add total columns and current char to lineinfo
  Src: ChildFrm.cpp ChildFrm.h Merge.rc MergeEditStatus.h MergeEditView.cpp
  Src/Languages: *.rc

2004-06-19 Kimmo
 PATCH: [ 975432 ] Add codepage detection option to configlog
  Src: ConfigLog.cpp ConfigLog.h MainFrm.cpp

2004-06-19 Laoran
 PATCH: [ 974714 ] detect codepage when open documents directly, small update
  Src: MainFrm.cpp
 PATCH: [ 974714 ] detect codepage when open documents directly
  Src: DiffContext.cpp MainFrm.cpp MainFrm.h
  Src/Common: UniFile.cpp UniFile.h
 PATCH: [ 975213 ] warn when codepages differ
  Src: MainFrm.cpp Merge.rc resource.h
  Src/Languages: *.rc resource.h

2004-06-18 Kimmo
 BUG: [ 930574 ] Remove old and unused undo-code
  Src: MergeDoc.h MergeDoc.cpp

2004-06-17 Kimmo
 Remove commented out functions ExpandTabs() and Tabify()
  Src: MergeDoc.h MergeDoc.cpp

2004-06-17 Laoran
 PATCH: [ 972108 ] use getDefaultCodepage for document with no codepage
  Src: MainFrm.cpp
  Src/Common: UniFile.cpp unicoder.cpp unicoder.h
 PATCH: [ 973029 ] Scroll when more than 65536 characters
  Src/Common: SplitterWndEx.cpp
  Src/editlib: ccrystaltextview.cpp
 widen labels in colors dialog
  Src: Merge.rc
  Src/Languages: *.rc

2004-06-15 Kimmo
 PATCH: [ 972575 ] Goto real line from location pane
  Src: LocationView.cpp LocationView.h
 Add instructions for generating HTML documentation using SF.net shell server
  Docs/Developers: readme-developers.html

2004-06-14 Kimmo
 PATCH: [ 972188 ] Add More Pascal comments styles to syntax highlight
  Src/editlib: pascal.cpp

2004-06-14 Laoran
 PATCH: [ 972290 ] In LoadFromFile, free the handle before deleting temp files
  Src: MergeDoc.cpp
 PATCH: [ 972296 ] GetLineCount in MergeDoc must test validity of buffers
  Src: LocationView.cpp MergeDoc.cpp MergeDoc.h
 declare GetLeftView()/GetRightView()/GetLeftDetailView()/GetRightDetailView() as const
  Src: MergeDoc.h

2004-06-13 Laoran
 PATCH: [ 972007 ] Japanese translation update(2) ; submitted by Takashi Sawanaka
  Src/Languages: UpdateAll_resource_h.bat
  Src/Languages/Japanese: MergeJapanese.rc
 PATCH: [ 971862 ] Various Updates in German Translation ; submitter : Tim Gerundt
  Src/Languages/German: MergeGerman.rc

2004-06-12 WinMerge experimental release 2.1.7.7 (cvs)

2004-06-11 Laoran
 BUG: [ 970936 ] Cannot merge last diff
  Src: MergeDiffDetailView.cpp
 PATCH: [ 970991 ] Force redraw after changing moved block option
  Src: LocationView.cpp
 PATCH: [ 970221 ] Keep overlay mode after Rescan
  Src/editlib: ccrystaltextview.cpp

2004-06-11 Kimmo
 BUG: [ 969922 ] inconsistent 'options' dialog behavior
  Fix enabling/disabling 'Apply to unrecognized side' checkbox in Editor options
  Src: PropEditor.cpp
 BUG: [ 970940 ] Location pane bars not shown if syntax highlight disabled
  Src: LocationView.cpp
 BUG: [ 969922 ] inconsistent 'options' dialog behavior
  Fix enabling/disabling 'Enable advanced menu' checkbox in System options
  Src: PropRegistry.cpp PropRegistry.h

2004-06-10 Kimmo
 BUG: [ 969922 ] inconsistent 'options' dialog behavior
  Remove 'Apply' button from options-dialog
  Src: MainFrm.cpp
 BUG: [ 826560 ] CVssPrompt::OnInitDialog neglects to notice failure in open
  Also inform user if VSS databases not found
  Src: Merge.rc resource.h VssPrompt.cpp
  Src/Languages: *.rc resource.h
 PATCH: [ 970478 ] Cleanup VCS/VSS code part 1
  Src: MainFrm.cpp MainFrm.h Merge.rc resource.h
  Src/Languages: *.rc resource.h

2004-06-10 Laoran
 PATCH: [ 968887 ] UI for codepage detection
  Src: DiffContext.cpp DiffContext.h DiffWrapper.cpp DiffWrapper.h DirDoc.cpp
   MainFrm.cpp Merge.rc OptionsDef.h PropGeneral.cpp PropGeneral.h resource.h
  Src/Languages: *.rc resource.h

2004-06-09 Kimmo
 PATCH: [ 969277 ] Color options for moved blocks
  Src: MainFrm.cpp Merge.rc MergeEditView.cpp MergeEditView.h OptionsDef.h
   PropColors.cpp PropColors.h resource.h
  Src/Languages: *.rc resource.h

2004-06-08 Kimmo
 PATCH: [ 968504 ] Add Goto Line to location pane context menu
  Src: LocationView.cpp LocationView.h Merge.rc resource.h
  Src/Languages: *.rc resource.h
 PATCH: [ 968960 ] Add moved blocks setting to configlog
  Src: ConfigLog.cpp ConfigLog.h MainFrm.cpp

2004-06-08 Perry
 [ 967406 ] Add .clw to filtered out files in MakePatchDirs
 Filter out also clw files in MSVC filter. Bump version to 1.2.2.0.
  MakePatchDirs: MakePatchDirs.rc MakePatchDirsDlg.cpp
 [ 967315 ] Remove needless cast in unicoder.cpp:CrossConvert
  Src/Common: unicoder.cpp

2004-06-07 Kimmo
 BUG: [ 967899 ] Ignore carriage return differences logic backwards
  Src: MainFrm.cpp
 PATCH: [ 968255 ] Rename EOL sensitive in configlog
  Src: ConfigLog.cpp
 PATCH: [ 967700 ] Add Goto-items to location pane context menu
  Src: LocationView.cpp Merge.rc MergeEditView.h resource.h
  Src/Languages: *.rc resource.h

2004-06-07 Laoran
 PATCH: [ 966693 ] set scroll width in diffpane for widest view
  Src: MergeDiffDetailView.cpp MergeDiffDetailView.h
  Src\editlib: ccrystaltextview.h
 PATCH: [ 963124 ] better name for variable for Undo/deleteText
  Src: GhostTextBuffer.cpp GhostTextBuffer.h

2004-06-07 Perry
 Updated Catalan translation from Jordi Vilar
  Src/Languages: Catalan/MergeCatalan.rc

2004-06-06 Kimmo
 BUG: [ 959729 ] View menu show-items disabled in mergeview
  Second try, renumber items in editlib/editcmd.h
  Src/editlib/editcmd.h

2004-06-06 Laoran
 update for MergeSwedish.rc ; contribution from Hans Eriksson
  Language/Swedish: MergeSwedish.rc

2004-06-04 WinMerge experimental release 2.1.7.6 (cvs)

2004-06-04 Kimmo
 PATCH: [ 965950 ] Simplify and fix Open-dialog extension parser
  Src: OpenDlg.cpp

2004-06-03 Laoran
 BUG: [ 964242 ] WinMerge getting slower loading and showing files
  Src/Common: unicoder.cpp

2004-06-03 Kimmo
 PATCH: [ 963814 ] Cosmetic: fix call in OpenDlg to LoadPosition
  Submitted by Perry
  Src: OpenDlg.cpp

2004-06-02 Perry
 PATCH: [ 964665 ] And another unneeded include in DirDoc.h
  Src: DirDoc.h

2004-06-02 Laoran
 PATCH: [ 964497 ] fix display of moved block in detail pane
  Src: MergeDiffDetailView.cpp

2004-06-02 Kimmo
 BUG: [ 964885 ] ColExtGet() returns extension converted to lowercase
  Src: DirViewColItems.cpp

2004-06-02 Perry
 PATCH: [ 964678 ] New MergeJapanese.rc file
  Src/Languages: Japanese/MergeJapanese.rc etc
 Update "18 languages" to "19 languages"
  Docs\Developers: readme-developers.html
 PATCH: [ 964661 ] Remove unneeded include from DirDoc.h
  Src: 7Common.cpp DirDoc.h
 PATCH: [ 964664 ] Remove another unneeded include from DirDoc.h
  Src: 7Common.h DirDoc.h

2004-06-01 Kimmo
 BUG: [ 963820 ] Cutting off trailing chars in opendlg
  Submitted by Perry
  Src/Common: CoreTools.cpp

2004-06-01 Laoran
 PATCH: [ 948368 ] display moved blocks in location bar
  Src: DiffWrapper.cpp DiffWrapper.h LocationView.cpp LocationView.h MainFrm.cpp
   Merge.rc MergeDoc.cpp MergeDoc.h OptionsDef.h resource.h
  Src/Languages: *.rc resource.h
 PATCH: [ 960455 ] Clip text in DrawLineHelperImpl
  Src/editlib: crystaltextview.cpp

2004-05-31 Perry
 PATCH: [ 963804 ] Report also thread codepage in config
  Src: ConfigLog.cpp

2004-05-31 Kimmo
 Backout patch for BUG: [ 959729 ] View menu show-items disabled in mergeview
  It caused bug #963299 Can't execute 2.1.7.5 under Windows 98
  Src/editlib/editcmd.h

2004-05-31 Perry
 PATCH: [ 963092 ] Cannot compile LanguageSelect.cpp using MSVC Japanese Ver.
  Src/Common: LanguageSelect.cpp
  Src/editlib: crystaltextview.cpp cs2cs.cpp
 PATCH: [ 962460 ] Add codepage & locale info to config log
  Src: ConfigLog.cpp
 PATCH: [ 962538 ] In Rescan, set status to binary if caller said so
  Src: MergeDoc.cpp
 PATCH: [ 963438 ] Fix selection of language in dialog
  Src/Common: LanguageSelect.cpp

2004-05-30 Kimmo
 PATCH: [ 963019 ] keyboard shortcuts in replace dialog
  Submitted by Takashi Sawanaka
  Src: Merge.rc
  Src/Languages: *.rc
 PATCH: [ 913743 ] Icons for menuitems (RFE 873697) - 16-color icons, 3 new icons
  Submitted by Tim Gerundt
  Src: MainFrm.cpp Merge.rc
  Src/res: cascade.bmp change_pane.bmp close.bmp error.bmp exit.bmp
   go_to.bmp horizontally.bmp language.bmp replace.bmp select_font.bmp
   unknown.bmp vertically.bmp
  Src/res new files: customize_columns.bmp filters.bmp select_all.bmp
  Src/languages: *.rc resource.h
  Src/languages/res: same changes than in Src/res
 PATCH: [ 962509 ] Copy pathnames and filenames from directory compare
  Src: DirView.cpp DirView.h Merge.rc resource.h
  Src/languages: *.rc resource.h
 BUG: [ 962449 ] Zip* items in dircompare context menu don't have help text
  Src: Merge.rc
  Src/languages: *.rc

2004-05-30 Laoran
 BUG: [ 909779 ] Uninitialised variable in CGhostTextBuffer::DeleteText() ; fix the fix !
  Src: GhostTextBuffer.cpp
 PATCH: [ 963118 ] variable name fix in CGhostTextBuffer::Undo
  Src: GhostTextBuffer.cpp

2004-05-30 Perry
 PATCH: [ 962558 ] Some MBCS fixes from sawanaka
  Src: paths.cpp paths.h StdAfx.cpp StdAfx.h
  Src/Common: coretools.cpp
 PATCH: [ 962539 ] Add ResetInit which handles partly full buffers
  Src: MergeDoc.cpp
  Src/editlib: ccrystaltextbuffer.h

2004-05-29 Kimmo
 PATCH: [ 961810 ] Store path for ShellExtension
  ShellExtension: resource.h ShellExtension.cpp ShellExtension.rc
   WinMergeShell.cpp WinMergeShell.h
 PATCH: [ 961780 ] ShellExtension advanced menu - UI
  Src: Merge.rc PropRegistry.cpp PropRegistry.h resource.h
  Src/Languages: *.rc resource.h
 PATCH: [ 962348 ] Center OpenDialog to MainFrame
  Src: OpenDlg.cpp OpenDlg.h

2004-05-29 Perry
 Updated WinMerge Localized.iss from seier (manifest & plugins.txt changes)
  InnoSetup: WinMerge Localized.iss
 PATCH: [ 945759 ] Move to location when double-click location view
  Src/Common: LocationView.cpp LocationView.h
 BUG: [ 961901 ] Possible overflow bug in SplitFilename
  Src/Common: coretools.cpp
 New InnoSetup/Languages/Japanese.isl from sawanaka
  From url posted in discussion in BUG [ 958805 ] 2 bytes display error
  (I removed the entry for English before checking it in, as there is no English satellite DLL)
  InnoSetup: Languages/Japanese.isl

2004-05-29 WinMerge experimental release 2.1.7.5 (cvs)

2004-05-28 Kimmo
 BUG: [ 959729 ] View menu show-items disabled in mergeview
  Src/editlib: editcmd.h

2004-05-28 Laoran
 PATCH: [ 960771 ] disable .. in dirview if upward move is not reversible
  Src: DirView.cpp DirView.h Merge.rc resource.h
  Src/res Add: folderup.bmp folderup_disable.bmp
  Src/Languages: *.rc resource.h
  Src/Languages/res Add: folderup.bmp folderup_disable.bmp
 PATCH: [ 961304 ] WinDiff-compatible (F7/F8) keybindings for next/prev diff
 submitted by Krzysztof Kowalczyk
  Src: Merge.rc
  Src/Languages: *.rc

2004-05-27 Laoran
 PATCH: [ 960996 ] 'backspace' key in dirview for parent directory
  Src: DirView.cpp DirView.h

2004-05-27 Perry
 PATCH: [ 959843 ] Fix codepage initialization to thread default
  Src: DiffWrapper.cpp DiffWrapper.h DirScan.cpp DirScan.h Merge.cpp
  Src/Common: unicoder.cpp
 Localized some language names into Catalan
  Src/Languages: MergeCatalan.rc

2004-05-26 Perry
 Added a few more native or ASCII language names (Albanian, Arabic, Armenian
  Belarusian, Croatian, Estonian, Farsi, Georgian, Greek, Urdu)
  Src/Common: LanguageSelect.cpp

2004-05-27 Kimmo
 PATCH: [ 960265 ] Don't show size for folder or non-existing file
  Src: DiffFileInfo.cpp DiffFileInfo.h DirScan.cpp DirViewColItems.cpp

2004-05-26 Perry
 PATCH: [ 959468 ] A patch for "Bug [ 958805 ] 2 bytes display error".
  Src: StdAfx.cpp
  Src/editlib: ccrystaltextview.cpp ccrystaltextview.h

2004-05-25 Perry
 (cosmetic) Update edit dates in file headers
  Src/Common: unicoder.cpp unicoder.h
 Localized language names into Russian
  Src/Languages: MergeRussian.rc
 Localized language names into Bulgarian
  Src/Languages: MergeBulgarian.rc

2004-05-25 Kimmo
 PATCH: [ 959627 ] Fix crash when opening context menu for '..' item in dirview
  Src: DirActions.cpp DirView.cpp
 BUG: [ 880104 ] CCCPrompt--does it work at all?
  Disable "Save as..." button since its not used
  Src: Merge.rc
  Src/Languages: *.rc
 RFE [ 950290 ] Use MS Shell Dlg font in dialogs
  Convert Chinese and Korean translations too
  Src/Languages: MergeChineseSimplified.rc MergeChineseTraditional.rc MergeKorean.rc
 PATCH: [ 959503 ] Enable word deletion function(Ctrl+Del)
  Submitted by Takashi Sawanaka
  Src/editlib: editcmd.h editres.rc

2004-05-25 Perry
 PATCH: [ 950243 ] 2 minor updates to developers docs
  Docs/Developers: readme-developers.html
 Localized language names into Italian
  Src/Languages: MergeItalian.rc

2004-05-24 Perry
 PATCH: [ 958973 ] CDiffTextBuffer::LoadFromFile wrong return
  Src: MergeDoc.cpp

2004-05-24 Kimmo
 PATCH: [ 959111 ] Optionally don't disable Open-dialog OK-button
  Src: MainFrm.cpp Merge.rc OpenDlg.cpp OptionsDef.h PropGeneral.cpp
   PropGeneral.h resource.h
  Src/Languages: *.rc resource.h

2004-05-24 Perry
 Fix compilation (trailing comma in CCrystalTextView::m_SourceDefs)
  Src/editlib: ccrystaltextview.cpp

2004-05-24 Kimmo
 PATCH: [ 958938 ] Better way to close WinMerge with ESC (-e switch)
  Src: MainFrm.cpp MainFrm.h Merge.cpp Merge.h
 PATCH: [ 959035 ] Registry value for enabling logging
  Src: MainFrm.cpp OptionsDef.h
 Add note about logging (patch #959035]
  Docs/Developers: readme-developers.html
 RFE: [ 934419 ] Syntax highlighting *.cls with Visual Basic (basic.cpp)
  Submitted by Tim Gerundt
  Src/editlib: ccrystaltextview.cpp

2004-05-24 Perry
 PATCH: [ 893202 ] Bug fixed for DBCS text edit (Attach again)
 Src\editlib: ccrystaleditview.cpp ccrystaltextview.cpp

2004-05-23 WinMerge experimental release 2.1.7.4 (cvs)

2004-05-23 Laoran
 PATCH: [ 957317 ] Broken menu in Winmerge 2.1.7.3
  Src: Merge.cpp
  Src/Common: LanguageSelect.cpp
 typo in russian language : APTION -> CAPTION
  Src/Languages/Russian: MergeRussian.rc

2004-05-23 Kimmo
 PATCH: [ 958478 ] Allow ESC close WinMerge if no open docs
  Src: MainFrm.cpp MainFrm.h

2004-05-23 Perry
 Localized language names into Portuguese
  Src/Languages: MergeBrazilian.rc

2004-05-22 Kimmo
 PATCH: [ 958122 ] Add C# syntax highlight
  Src: Merge.dsp
  Src/editlib: crystaltextview.cpp crystaltextview.h
  Src/editlib new file: csharp.cpp
 RFE: [ 950290 ] Use MS Shell Dlg font in dialogs
  Src: Merge.rc
  Src/Languages: *.rc, except MergeChineseSimplified.rc
   Merge ChineseTraditional.rc, MergeKorean.rc

2004-05-22 Perry
 Localized language names into French
  Src/Languages: MergeFrench.rc
 Updates to Spanish localized language names
  Src/Languages: MergeSpanish.rc
 BUG: [ 898924 ] Lang order in Language selection dialog?
 RFE: [ 953015 ] sort language names
  Mark listbox in language dialog as sorted
  Src: Merge.rc
  Src/Languages: Merge*.rc

2004-05-21 Perry
 Fix PATCH [ 957740] - declare CMergeDoc::CompareBinaries
  Src: MergeDoc.h
 Updated WinMerge Localized.iss from seier (add Swedish Readme after install)
  InnoSetup: WinMerge Localized.iss
 Updated Swedish ReadMe from seier
  Docs/Users/Languages: Read Me-Swedish.rtf
 Updated all non-localized language versions of Read Me*.rtf to current Read Me.rtf
  Docs\Users\Languages: Read Me*.rtf (except Swedish)
 Remove all localization Contributors*.rtf
  Docs\Users\Languages: removed Contributors*.rtf

2004-05-21 Kimmo
 PATCH: [ 957740 ] Tell if two binary files are different/identical
  Src: DiffWrapper.cpp DiffWrapper.h Merge.rc MergeDoc.cpp resource.h
  Src/Languages: *.rc resource.h

2004-05-21 Laoran
 PATCH: [ 947680 ] Organize resource strings, part 2 (IDD and IDB)
  Src: resource.h
  Src/Languages: resource.h

2004-05-21 Kimmo
 PrivateBuild in version resource must be empty string in CVS.
  Src: Merge.rc
  Src/Languages: *.rc

2004-05-21 Perry
 Localized language names into Korean
  Src/Languages: MergeKorean.rc

2004-05-20 Kimmo
 PATCH: [ 956893 ] Fix toolbar problem with XP themes and VC6
  Src: MainFrm.cpp Merge.cpp
 PATCH: [ 954991 ] Add directory compare column descriptions
  Src: DirColsDlg.cpp DirColsDlg.h DirView.h DirViewColHandler.cpp
   DirViewColItems.cpp Merge.rc resource.h
  Src/Languages: *.rc resource.h

2004-05-18 Kimmo
 PATCH: [ 955408 ] Select first item in dir compare
  Src: DirView.cpp
 PATCH: [ 956044 ] Don't select '..' and don't skip unique folders in dirview
  Src: DirView.cpp

2004-05-18 Perry
 PATCH: [ 955573 ] Swedish localization of Read Me.rtf
 Docs/User/Languages: Read Me-Swedish.rtf

2004-05-18 WinMerge experimental release 2.1.7.3 (cvs)

2004-05-18 Perry
 PATCH: (continued) [ 942021 ] Propose translation of language names
  Src/Languages: MergeBrazilian.rc MergeBulgarian.rc MergeCatalan.rc
   MergeChineseSimplified.rc MergeChineseTraditional.rc
   MergeKorean.rc MergeRussian.rc
 BUG: [ 955550 ] Slovak rc file broken (typo)
  Src/Languages: MergeSlovak.rc
 BUG: [ 955553 ] Russian rc file broken
  Src/Languages: MergeRussian.rc
 BUG: [ 955557 ] Chinese traditional rc file broken (cvs)
  Src/Languages: MergeChineseTraditional.rc
  
2004-05-17 Kimmo
 PATCH: [ 955411 ] Remove non-functional firstdiff selection
  Src: DirView.h MainFrm.cpp

2004-05-15 Kimmo
 Fix build error, VC6 doesn't like CList::GetSize()
  Src: DirActions.cpp
 PATCH: [ 952877 ] Return better results from Rescan()
  Src: MergeDoc.cpp MergeDoc.h 

2004-05-14 Kimmo
 PATCH: [ 952825 ] Cleanup VSS dialog layout
  Src: Merge.rc
  Src/Languages: *.rc
 PATCH: [ 952586 ] Improve readonly file and folder handling
  Src: DirActions.cpp MainFrm.cpp MainFrm.h Merge.rc MergeDoc.cpp resource.h
  Src/Languages: *.rc resource.h
 PATCH: [ 948592 ] Open and select filefilter from Open-dialog
  Src: FileFiltersDlg.cpp MainFrm.cpp MainFrm.h Merge.cpp Merge.h
   Merge.rc OpenDlg.cpp OpenDlg.h resource.h
  Src/Languages: *.rc resource.h

2004-05-13 Kimmo
 Backout last change to Merge.rc - restore version 1.187
  Src: Merge.rc
 BUG: [ 953232 ] Some lang-files broken
  Backout last changes to lang files (see bug item for details)
   Src/Languages: MergeBulgarian.rc MergeChineseSimplified.rc
    MergeChineseTraditional.rc MergeKorean.rc MergeRussian.rc
 PATCH: [ 953056 ] Rename 'Sensitive to EOL' option
  Src: Merge.rc
  Src/Languages: *.rc
 PATCH: [ 953515 ] Improve LocationPane drawing code
  Src: LocationView.cpp

2004-05-13 Perry
 PATCH: [ 950530 ] Remove unneeded cast in DirView.cpp
  Src: DirView.cpp
 PATCH: [ 945722 ] Two functions are never called
  Src: DiffContext.cpp DiffContext.h
 PATCH: [ 945152 ] Installer: My last revisions
  InnoSetup: WinMerge Localized.iss
  InnoSetup/Languages: *.isl (except Swedish.isl)
 PATCH: [ 950236 ] Set DIFFCODE::BOTH correctly
  Src: DirScan.cpp
 PATCH: [ 942021 ] Propose translation of language names
  Src: Merge.rc
  Src/Common: LanguageSelect.cpp LanguageSelect.h
  Src/Languages: *.rc
 PATCH: [ 950212 ] wrong language shown in LanguageSelect
  Src/Common: LanguageSelect.cpp
 BUG: [ 876539 ] Wrong textinclude language def in ChineseTraditional
  Src/Languages: ChineseTraditional.rc
 Translate language names into Dutch (follow-up to PATCH#942021)
  Src/Languages: MergeDutch.rc
 Add a few more native language names
  Src/Common: LanguageSelect.cpp

2004-05-12 Kimmo
 PATCH: [ 948448 ] Remove WinMerge.exe path from system options
  Src: Merge.rc PropRegistry.cpp PropRegistry.h resource.h

2004-05-11 Laoran
 PATCH: [ 948528 ] Build BCMenu after select language
  Src: MainFrm.cpp MainFrm.h Merge.cpp
  Src\Common: LanguageSelect.cpp

2004-05-11 Perry
 Fix MakePatchDirs to ignore BuildTmp.
 MakePatchDirs: ChangeLog.txt MakePatchDirs.rc MakePatchDirsDlg.cpp
 
2004-05-11 Kimmo
 PATCH: [ 951404 ] Add statusmessage for dircompare copy/delete ops
  Src: DirActions.cpp Merge.rc resource.h
  Src/Languages: *.rc resource.h

2004-05-10 Christian
 PATCH: [ 945467 ] Remove ReleaseMinsize and UnicodeReleaseMinsize
  Src: Merge.dsp
  Src/Languages: CopyAll.bat

2004-05-10 Kimmo
 PATCH: [ 951234 ] Use resourceIDs in dircompare statebar bitmpaps
  Src: Merge.rc
  Src/Languages: *.rc

2004-05-10 Laoran
 apply again modifications to IDS_QUICKHELP, lost during PATCH: [ 947680 ]
  Src: Merge.rc 
  Src\Languages\Swedish: MergeSwedish.rc

2004-05-07 Laoran
 PATCH: [ 947680 ] Organize resource strings
  Src: CCPrompt.cpp DiffWrapper.cpp DirFrame.cpp MainFrm.cpp Merge.rc MergeDoc.cpp
   PropGeneral.cpp VssPrompt.cpp resource.h  
  Src\editlib: ccrystaltextview.cpp ccrystaltextview.h editcmd.h
  Src\Common: LanguageSelect.h
  Src\Languages: *.rc resource.h

2004-05-08 WinMerge experimental release 2.1.7.2 (cvs)

2004-05-07 Laoran
 update for MergeSwedish.rc ; contribution from Hans Eriksson
  Language/Swedish: MergeSwedish.rc

2004-05-07 Kimmo
 PATCH: [ 949428 ] Show VSS dialogs only once
  Src: DirActions.cpp MainFrm.cpp MainFrm.h Merge.rc resource.h
  Languages: *.rc resource.h
 BUG: [ 947121 ] ShellExtension does not compile with Visual Studio 2003
  ShellExtension: ShellExtension.dsp
 BUG: [ 941335 ] RC file missing strings and errors
  Src: Merge.rc
  Languages: *.rc

2004-05-07 Laoran
 PATCH: [ 948366 ] clear moved blocks before Rescan
  Src: DiffWrapper.cpp DiffWrapper.h MergeDoc.cpp

2004-05-06 Kimmo
 PATCH: [ 948634 ] Commandline switch to open left/right side as readonly
  Src: Merge.cpp Merge.rc
  Languages: *.rc

2004-05-05 Kimmo
 PATCH: [ 947232 ] Add primary path to WinMerge.exe for ShellExtension
  ShellExtension: ShellExtension.rc WinMergeShell.cpp

2004-05-04 Laoran
 PATCH: [ 947599 ] Remove EditFile.cpp EditFile.h
  Src remove: EditFile.cpp EditFile.h
  Src: Merge.dsp Merge.rc resource.h
  Languages: *.rc resource.h

2004-05-03 Kimmo
 PATCH: [ 942565 ] Option for closing windows with ESC
  Src: DirDoc.cpp DirView.cpp DirView.h MainFrm.cpp Merge.rc MergeEditView.cpp
   MergeEditView.h OptionsDef.h PropGeneral.cpp PropGeneral.h resource.h
  Languages: *.rc resource.h
 PATCH: [ 946550 ] [MergeMode] Don't try to merge if diff not selected
  Src: MergeEditView.cpp

2004-05-03 Laoran
 PATCH: [ 946091 ] Fix for GetLineActualLength
  Src/editlib: ccrystaltextview.cpp

2004-05-03 Laoran
 PATCH: [ 936842 ] Custom message when same file in both panels
  Src: Merge.rc MergeDoc.cpp resource.h
  Languages: *.rc resource.h
  
2004-05-02 Kimmo
 PATCH: [ 833180 ] Filters dialog
  Src new files: FileFiltersDlg.cpp FileFiltersDlg.h
  Src: MainFrm.cpp MainFrm.h Merge.dsp Merge.rc
   PropFilter.cpp PropFilter.h resource.h
  Languages: *.rc resource.h

2004-05-01 Laoran
 PATCH: addon to [ 941623 ] Propose moving IDC_COMPARISON_STOP to block with ...CLOSE
  Src: Merge.rc
  Languages: *.rc

2004-05-01 Perry
 PATCH: [ 941979 ] Add cast to suppress VS.NET 2002 warning
  Src: MergeDoc.cpp
 PATCH: [ 927929 ] Reload language menus after resources hooked
  Src: Merge.cpp
 PATCH: [ 941623 ] Propose moving IDC_COMPARISON_STOP to block with ...CLOSE
  Src:
   Merge.rc
  Languages: MergeBrazilian.rc MergeBulgarian.rc MergeCatalan.rc
   MergeChineseSimplified.rc MergeChineseTraditional.rc MergeCzech.rc
   MergeDanish.rc MergeDutch.rc MergeFrench.rc MergeGerman.rc MergeItalian.rc
   MergeKorean.rc MergeNorwegian.rc MergePolish.rc MergeRussian.rc
   MergeSlovak.rc MergeSpanish.rc MergeSwedish.rc
 PATCH: [ 909761 ] Misplaced return in CDirView::ValidateColumnOrdering()?
  Src: DirViewColHandler.cpp

2004-04-30 Christian
 PATCH: [ 941982 ] Fix VS6 compilation
  Src/Common: MessageBoxDialog.cpp MessageBoxDialog.h

2004-04-29 Kimmo
 PATCH: [ 943149 ] Contributors fix
  Src: Merge.cpp Merge.rc
  Languages: *.rc
  Docs/Users/Contributors.txt
  Removed: Docs/Users/Contributors.rtf
 BUG: [ 944290 ] Crash when deleting files
  Src: DirActions.cpp

2004-04-28 Kimmo
 PATCH: [ 937947 ] Open-dialog improvements (no codepage controls)
  Src: Merge.rc OpenDlg.cpp OpenDlg.h resource.h
  Languages: *.rc resource.h

2004-04-27 Kimmo
 RFE: [ 942591 ] Loose filter for Visual Basic
  Submitted by Tim Gerundt
  Filters new file: Merge_VB_loose.flt
 PATCH: [ 930482 ] Add comments to filter: Merge_GnuC_loose.flt
  Submitted by Tim Gerundt
  Filters: Merge_GnuC_loose.flt

2004-04-26 Kimmo
 BUG: [ 940709 ] Dircompare column widths saved only when WinMerge closed
  Fix from Perry
  Src: DirViewColHandler.cpp

2004-04-26 Perry
 PATCH: [ 941986 ] Remove blank lines in popup dirview menu in Merge.rc
  Src: Merge.rc

2004-04-25 WinMerge experimental release 2.1.7.1 (cvs)

2004-04-25 Perry
 More removing obsolete keywords to sync languages to English
  Languages: MergeDutch.rc MergeFrench.rc MergeGerman.rc
   MergeItalian.rc MergeKorean.rc MergeNorwegian.rc

2004-04-25 Kimmo
 PATCH: [ 941524 ] Reset dircompare column widths
  Src: DirColsDlg.cpp DirColsDlg.h DirView.cpp DirView.h DirViewColHandler.cpp
 PATCH: [ 941545 ] Fix Shift-Arrow to select text in merge mode
  Src: MergeEditView.cpp

2004-04-25 Perry
 More removing obsolete keywords to sync languages to English
  Languages: MergeCatalan.rc MergeChineseSimplified.rc
   MergeChineseTraditional.rc MergeCzech.rc MergeDanish.rc
   MergeFrench.rc MergePolish.rc

2004-04-24 Perry
 More removing obsolete keywords to sync languages to English
  Src: Merge.rc
  Languages: MergeBrazilian.rc MergeSpanish.rc MergeSwedish.rc
 More removing obsolete keywords to sync languages to English
  Src: Merge.rc
  Languages: MergeRussian.rc MergeSlovak.rc MergeSpanish.rc
   MergeSwedish.rc
 More removing obsolete keywords to sync languages to English
  Languages: MergeBrazilian.rc MergeSlovak.rc MergeSpanish.rc
   MergeSwedish.rc

2004-04-24 Kimmo
 BUG: [ 939245 ] Location Bar don't show diff in first line
  Src: LocationView.cpp
 PATCH: [ 938822 ] Enable merge items in file compare context menu
  Src: Merge.rc
  Languages: *.rc

2004-04-24 Perry
 PATCH: [ 941014 ] Various Updates in German Translation (Tim Gerundt)
  (Except I didn't remove the synchro comments)
  Languages: MergeGerman.rc
 Added synchro comments to Merge.rc. Synchronized various languages to English.
  Src: Merge.rc
  Languages: MergeBrazilian.rc MergeBulgarian.rc MergeCatalan.rc 
   MergeChineseSimplified.rc MergeChineseTraditional.rc
   MergeCzech.rc MergeDanish.rc MergeFrench.rc MergeGerman.rc
   MergeSpanish.rc

2004-04-23 Kimmo
 PATCH: [ 940713 ] Fix for resetting dirview columns
  Src: DirViewColHandler.cpp
 Remove deprecated keywords from RC file I added in when applying #936682.
  Src: Merge.rc
 PATCH: [ 940089 ] Add Binary column and shorten short result
  Src: DirViewColHandler.cpp DirViewColItems.cpp Merge.rc resource.h
  Languages: *.rc resource.h
 PATCH: [ 940036 ] Add missing dirview column table indexes
  Src DirViewColHandler.cpp

2004-04-22 Kimmo
 PATCH: [ 936682 ] Highlight syntax in unrecognized side
  Src: MainFrm.cpp Merge.rc MergeDoc.cpp OptionsDef.h PropEditor.cpp
   PropEditor.h resource.h
  Languages: *.rc resource.h

2004-04-21 Kimmo
 PATCH: [ 938609 ] Merge mode (simplified)
  Src: MainFrm.cpp Merge.rc MergeDoc.cpp MergeDoc.h MergeEdiView.cpp
   MergeEditView.h OptionsDef.h resource.h
  Languages: *.rc resource.h
 BUG: [ 939548 ] Location bar crash if diff at end missing last line EOL
  Src: LocationView.cpp

2004-04-20 WinMerge experimental release 2.1.7.0 (cvs)

2004-04-20 Laoran
 Syntax correction for Unicode
  Src: DirView.cpp

2004-04-20 Laoran
 BUG:[ 938373 ] Filtering broken - infinite loop when file filtered
  Src: DirScan.cpp

2004-04-20 Laoran
 updated swedish translation
  Submitted by Hans Eriksson <hans.ericson@bredband.net>
  Languages/Swedish: MergeSwedish.rc

2004-04-20 Laoran
 PATCH: [ 929210 ] GUI for prediffer (compare two files)
  Src: DiffWrapper.cpp DirView.cpp MainFrm.cpp MainFrm.h Merge.rc MergeDoc.cpp MergeEditView.cpp 
   MergeEditView.h resource.h
  Languages: *.rc resource.h

2004-04-19 Perry
 Fix English typos in Localization Instructions.txt
 InnoSetup/Languages/Localization Instructions.txt

2004-04-19 Kimmo
 PATCH [ 937535 ] Updated syntax highlighting for resources files (*.rc)
  Submitted by Tim Gerundt
  Src/editlib: rsrc.cpp

2004-04-18 Perry
 BUG: [ 937435 ] cvs doesn't compile -- typo in CMergeDoc::ShowRescanError(
  Src: MergeDoc.cpp
 PATCH: [ 937441 ] CMergeEditView:Avoid calling SelectDiff when no current diff
   Src: MergeEditView.cpp
 BUG: [ 936079 ] castellano options
   Src/Languages/Spanish: MergeSpanish.rc

2004-04-18 Kimmo
 PATCH: [ 934738 ] CopyLeftTo/CopyRightTo Including Common Subdirectories
  Submitted by Ben (?) - some modifications by Kimmo
  Src: DirActions.cpp
 PATCH: [ 930456 ] Allow Copy Left/Right to copy folders too
  Src: DirActions.cpp
 PATCH: [ 934370 ] Log temp file errors and own errormessage
  Src: Merge.rc MergeDoc.cpp MergeDoc.h resource.h
  Languages: *.rc resource.h
  
2004-04-17 Kimmo
 PATCH: [ 910557 ] Location Bar
  Submitted by Vincent Osele
  Src: ChildFrm.cpp ChildFrm.h MainFrm.cpp Merge.dsp Merge.rc
   MergeDoc.cpp MergeDoc.h MergeEditView.h resource.h
  Src new files: LocationBar.cpp LocationBar.h LocationView.h LocationView.cpp
  Languages: *.rc resource.h

2004-04-17 Laoran
 PATCH: [ 929001 ] Delete commented out "diff_dirs2"
  Src: DirDoc.cpp

2004-04-17 Laoran
 BUG: [ 936824 ] Prediffer not initialized when generate patch
  Src: PatchTool.cpp

2004-04-17 Laoran
 BUG: [ 936524 ] Cannot compare/Cancel,Retry,Continue
  Src: DirView.cpp

2004-04-17 Kimmo
 Fix for unicode build after patch #935048 -- add _T() around string
  Src: DirView.cpp

2004-04-16 Laoran
 PATCH: [ 935048 ] Plugin manager and per-file plugin settings V2
  Src: DiffContext.cpp DiffContext.h DiffWrapper.cpp DiffWrapper.h DirActions.cpp DirDoc.cpp DirDoc.h 
   DirScan.cpp DirView.cpp DirView.h MainFrm.cpp Merge.dsp Merge.rc MergeDoc.cpp MergeDoc.h resource.h
  Src Add: PluginManager.cpp PluginManager.h

2004-04-16 Laoran
 updated swedish translation
  Submitted by Hans Eriksson <hans.ericson@bredband.net>
  Languages/Swedish: MergeSwedish.rc

2004-04-16 Kimmo
 RFE: [ 934446 ] Syntax highlighting for XML files
  Submitted by Tim Gerundt
  Src: Merge.dsp
  Src/editlib: ccrystaltextview.cpp ccrystaltextview.h
  Src/editlib new file: xml.cpp

2004-04-14 Kimmo
 BUG: [ 926578 ] Division by zero exception
  Fix tabsize limits in options dialog
  Src: PropEditor.cpp
 PATCH: [ 934344 ] Reset dircompare columns
  Src: DirColsDlg.cpp DirColsDlg.h DirViewColHandler.cpp Merge.rc resource.h
  Languages: *.rc resource.h

2004-04-13 Jochen
 PATCH: [ 889357 ] Attempt to improve DirScan speed -- updated patch from 2004-03-18
  Src: 7zCommon.cpp 7zCommon.h DiffContext.cpp DiffContext.h DiffFileInfo.h
   DiffWrapper.cpp DiffWrapper.h DirDoc.cpp DirScan.cpp DirViewColHandler.cpp DirViewColItems.cpp DirViewColItems.h
   MainFrm.cpp
  Merge7z: Merge7z.def Merge7z.h Merge7z311.cpp Merge7zCommon.cpp Merge7z311.dsp Merge7z312.dsp Merge7z313.dsp

2004-04-13 Kimmo
 PATCH: [ 932086 ] Syntax highlighting of HTML not complete (Bug 930930)
  Submitted by Tim Gerundt
  Src/editlib: ccrystaltextview.cpp html.cpp php.cpp
 Fix typo causing wrong icon for error items in dir compare
  Src: DirView.cpp

2004-04-07 Laoran
 BUG: [ 930995 ] Cursor help text ... ; part two : inversion in menu
  Src: Merge.rc
  Src/Languages: *.rc

2004-04-07 Kimmo
 PATCH: [ 930495 ] Log delete file errors from FileTransform.cpp
  Src: FileTransform.cpp
 PATCH: [ 930512 ] Log delete file errors from multiformatText.cpp
  Src/Common: multiformatText.cpp

2004-04-07 Laoran
 BUG: [ 930995 ] Cursor help text ...
  Src/Languages/German: MergeGerman.rc

2004-04-05 Kimmo
 PATCH: [ 929155 ] Add identical folders to dircompare statepane
  Src: DirCompStateBar.cpp DirCompStateBar.h DirView.cpp Merge.dsp
   Merge.rc resource.h
  Src/res new file: error.bmp
  Languages: Merge.rc resource.h res/error.bmp
 PATCH: [ 929584 ] Reset scratchpad names
  Don't show "Untitled Left/Right" for docs opened after scratchpads
  Src: MainFrm.cpp

2004-04-04 Perry
 PATCH: [ 928976 ] Handle when selected language not in list
  Src/Common: LanguageSelect.cpp

2004-04-04 Kimmo
 PATCH: [ 914513 ] Move message ids to stdafx.h
  DirDoc.h StdAfx.h

2004-04-03 Perry
 Move enum ACT_TYPE, struct action, and typedef DeletedItemList inside of 
  struct ActionList, of which they are part. Add header comment for ActionList.
 Src: DirActions.cpp DirView.h

2004-04-03 Laoran
 PATCH: [ 923044 ] Partial compare large files
  Plugins: list of sources.txt
  Plugins\src_VCPP_WatchBeginningOfLog add: all files
  Plugins\src_VCPP_WatchEndOfLog add: all files

2004-04-02 Laoran
 PATCH [ 925172 ] Check for presence of Windows Script
 Src: ConfigLog.cpp ConfigLog.h Merge.rc MergeEditView.cpp Plugins.cpp Plugins.h resource.h

2004-04-02 Laoran
 Change plugins event title 'CONTEXT_MENU' -> 'EDITOR_SCRIPT'
  Src: FileTransform.cpp FileTransform.h MergeEditView.cpp Plugins.cpp Plugins.h
  Plugins/dlls: editor addin.sct insert datetime.sct list.txt
  Plugins: list of sources.txt readme.txt syntax.txt
  Plugins/src_VB/ToUpper: WinMergeScript.cls

2004-04-01 Perry
 Remove most of obsolete keywords in rc file (a la Merge.rc 1.149, which didn't have them)
  Src: Merge.rc
 PATCH: [ 927000 ] Russian translation update (based on 1.32)
  (Not applied exactly, but updated to match current English Merge.rc (mostly removing obsolete keywords).
  Languages: Russian/MergeRussian.rc
 Merge Portuguese Brazilian to current English Merge.rc (mostly removing obsolete keywords).
  Languages: Brazilian/MergeBrazilian.rc

2004-03-31 Kimmo
 PATCH: [ 924521 ] Add context menu for file compare
  Src: DirView.cpp FilePathEdit.cpp Merge.rc MergeEditView.cpp resource.h
  Languages: resource.h *.rc

2004-03-30 seier
  Fixed Swedish Language had a description of Spanish in the installer.  
  Fixed Swedish was installing along with Spanish rather than the Swedish component description in the installer.  
     Src: InnoSetup/WinMerge.iss
  Release Notes Table template created in response to the table located at http://sourceforge.net/project/shownotes.php?release_id=226701.  
     Src: Docs/Users/Release Notes Template.htm
  Migration of /UsersGuide --> /Docs/Users/UsersGuide --> /Docs/Users/Guide Documented
     Src: Docs/Developers/File Migration Log.htm

2004-03-30 Laoran
 PATCH: [ 922724 ] "Don't display again" option for messageboxes
  Src: DirActions.cpp DirView.cpp MainFrm.cpp Merge.cpp Merge.dsp Merge.rc
       MergeDoc.cpp PatchDlg.cpp PatchTool.cpp PropGeneral.cpp PropGeneral.h StdAfx.h resource.h
  Src/editlib: ceditreplacedlg.cpp
  Src/common add: MessageBoxDialog.cpp MessageBoxDialog.h
  Languages: *.rc resource.h

2004-03-30 Kimmo
 PATCH: [ 925179 ] Remove is_*file() functions from coretools.h/cpp
  Common: coretools.cpp coretools.h

2004-03-30 Laoran
 BUG [ 925202 ] broken feature: after save error, save with another filename
  Src: MergeDoc.cpp

2004-03-28 Kimmo
 PATCH: [ 913743 ] Icons for menuitems (RFE 873697)
  File/Exit and Help/Contents icons from Tim Gerundt
  Src: MainFrm.cpp Merge.rc resource.h
  Src/res new files: exit.bmp help_contents.bmp
  Languages: *.rc resource.h
  Languages/res: exit.bmp help_contents.bmp

2004-03-27 Laoran
 PATCH [ 921788 ] scripts submenu in the main menu
 Src: FileTransform.cpp FileTransform.h MainFrm.cpp MainFrm.h Merge.rc
  MergeEditView.cpp MergeEditView.h Plugins.cpp Plugins.h resource.h
 Src/Languages: *.rc resource.h

2003-03-27 WinMerge beta 2.1.6.0 released

2004-03-26 Perry
 Src/Languages: Bulgarian/MergeBulgarian.rc
  Many cosmetic changes to make MergeBulgarian.rc match Merge.rc (mostly dropping obsolete keywords).

2004-03-26 Laoran
 [ 847155 ] Utf8len_of_string is wrong for 8-bit
  Src/Common: unicoder.cpp unicoder.h

2004-03-26 Perry
 Docs/Users: Contributors.rtf
  Add Bulgarian credit to contributors.rtf. Move Portuguese to alphabetically Portuguese.

2004-03-26 seier
  Fixed Swedish Language had a description of Spanish in the installer.  
  Fixed Swedish was installing along with Spanish rather than the Swedish component description in the installer.  
     Src: InnoSetup/WinMerge.iss
     
2004-03-26 Perry
 Add new Bulgarian translation
  Src/Languages/Bulgarian

2004-03-25 Kimmo
 BUG: [ 918760 ] doxygen.cfg moved
  Update paths for new location of doxygen.cfg in docs/developers
  docs/developers: doxygen.cfg

2004-03-24 Laoran
 HideFirstLetter plugin : fix : open input and output file as binary 
  Plugins/HideFirstLetter: WinMergeScript.cpp

2004-03-24 WinMerge experimental release 2.1.5.16 (cvs)

2004-03-24 Kimmo
 BUG: [ 922716 ] "delete both" broken
  Src: DirActions.cpp

2004-03-23 Perry
 Add simple comments in action list code
 Src: DirActions.cpp

2004-03-23 Kimmo
 PATCH: [ 921440 ] UI option for detecting moved blocks
  Src: DirDoc.cpp MainFrm.cpp Merge.rc MergeDoc.cpp OptionsDef.h
   PropCompare.cpp PropCompare.h resource.h
  Languages: *.rc resource.h

2004-03-22 Seier
 I added /InnoSetup/desktop.ini file to be used as a template for making C:\Program Files\WinMerge, or wherever that might be, have a customized (WinMerge)   folder icon. 
  InnoSetup: desktop.ini
 The DocBook version of the manual was revised to reference GIF versions of all previously referenced BMP images.  Thiw provides better compression and       browser compatiblity.  
  Docs: Users/Manual/screenshots/*.bmp --> *.gif
  Docs: Users/Manual/Compare_dirs.xml
  Docs: Users/Manual/Compare_files.xml
   
2004-03-22 Kimmo
 PATCH: [ 920530 ] Remember Patch-dialog settings
  Src: Merge.rc PatchDlg.cpp PatchDlg.h resource.h
  Languages: *.rc resource.h
 BUG: [ 920965 ] Merging moved block messes up linecolors
  Src: MergeDoc.cpp
 BUG: [ 902915 ] Directory compare statepane button text blinks
  Fix submitted by Jochen in Patch #889357 Attempt to improve DirScan speed.
  Src: DirCompStateBar.cpp
 Change directory compare statepane button to defaultbutton
  Allows stopping directory compare with Enter-key
  Src: Merge.rc
  Languages: *.rc

2004-03-22 Laoran
 PATCH: [ 909672 ] Undo makes marker messy
  Src: MergeEditView.cpp
  Src/editlib: ccrystaltextbuffer.cpp ccrystaltextbuffer.h

2004-03-21 Seier
  Installer now also installs WinMerge as the merging application in addition being the differencing application.  
  Installer updated to use \ShellExtension\ShellExtension.dll as the new location for that file 
  Installer will now remove the ComCtlupd.exe at the end of the installation.  
  Patch: [ 919134 ] I added the OleAut32.dll necessary for others to compile the WinMerge.iss script.
    Src: /InnoSetup/WinMerge.iss
  I compressed Splash1.bmp using RLE (Run Length Encoding [Lossless compression]) and named it as Splash2.bmp.  I only modified the English version.  Please test it out and send me an e-mail if it works and I'll create the RLE versions of all the other images.  PS: the file size went from 163 --> 111KB.  
     Src: /Src/Res/Splash2.bmp
 
2004-03-21 Kimmo
 PATCH: [ 918545 ] New Splash Screen
  Submitted and graphics by Dean Grimm
  Src: Splash.cpp Splash.h
  Src/res: splash1.bmp

2004-03-19 Christian
 PATCH: [ 919134 ] Paths fixed in installer
  InnoSetup: WinMerge.iss

2004-03-19 Kimmo
 BUG: [ 919090 ] Memory leak in DiffFileData
  Src: DiffWrapper.cpp

2004-03-18 Perry
 BUG: [ 919160 ] Cannot execute notepad.exe
  Fix OpenFileToExternalEditor to prepend space before quoted filename.
   Src/MainFrm.cpp
 PATCH: [ 891498 ] moved block detection
   Src: Diff.cpp DiffWrapper.cpp DiffWrapper.h MergeDoc.cpp MergeDoc.h
    MergeEditView.cpp MergeEditView.h
   Src\diffutils\src: analyze.c DIFF.H

2004-03-17 Seier
 Modified  \ShellExtension\Register.bat to also UnRegister via a /U (case-insensitive) parameter.  
  I also added an UnRegister.bat which does nothing more than call Register.bat with the /u switch.  
 If you're wondering where a particular file or folder went and why please see \Docs\Developers\File Migration Log.htm.  
  Everyone can use this file from now on.

2004-03-17 Kimmo
 PATCH: [ 913743 ] Icons for menuitems (RFE 873697)
  More nice icons from Tim Gerundt (Edit/Goto, Edit/Replace, View/Language, View/Font, Window/Change Pane)
  Src: MainFrm.cpp Merge.rc resource.h
  Src/res new files: change_pane.bmp go_to.bmp language.bmp replace.bmp select_font.bmp
  Languages: *.rc resource.h
  Languages/res new files: change_pane.bmp go_to.bmp language.bmp replace.bmp select_font.bmp
 PATCH: [ 915790 ] Open Contributors.rtf from About dialog
  Handle errors if wordpad or associated app couldn't be run.
  Src: Merge.cpp

2004-03-16 Seier
  [Patch 917142] for Installer:
    -Now detects whether or not the user requires an updated ComCtl32.dll and if needed updates it.  
    -Corrupt Shell Extension folder association removed from the registry (no more errors in Windows Explorer)
    -Explorer "WinMerge" context menu support added
    -The View Start Menu folder option at the end of the install no longer appears if the user chose not to create a start menu group
    -Portugese is now listed amongst the other selectable languages in alpahbetic order
    -Catalan, Norwegian, Polish, Russian, and Swedish translations have been added to the installer for a total of 17 languages! Way to go 
      Translators!  Eventually we'll all just get lazy and speak Mergish :).  
    -WinMerge's "User's Guide" and "Read Me" files installed and added to the user's start menu
      Program Files\Docs\ created containing:
        * Read Me.rtf
        * Contributors.rtf
        * \User's Guide\	
    -The Read Me.rtf was divided into two files: Read Me.rtf and Contributors.rtf 
      (since this information is non-technical ands considerable bloat to the main Read Me file)
    -Obsolete registry settings LeftMax and DirViewMax are now deleted rather than reset 
      (note: all registry deletions in Inno Setup must have Type set to "None")  
    -Whether or not TortoiseCVS is installed support for it is added to the registry.  That way if a user ever installs it our application with start working       with it automatically :) (don't worry this only causes two possibly unecessary registry values and one key to be created)	
    -Since all three of the core WinMerge application files WinMerge*.exe, Merge7z*.dll, and ShellExtension.dll all use Ole Automation, I've included the 	      necessary runtime: OleAut32.dll (VS6 Service Pack 5)

2004-03-16 Kimmo
 RFE [ 917196 ] Directory Comparison Results baloon annoyance
  Don't show infotip for filename when whole filename is already shown
  Src: DirView.cpp
 PATCH: [ 915790 ] Open Contributors.rtf from About dialog
  Src: Merge.cpp Merge.rc resource.h

2004-03-15 Perry
 Fix Bulgarian (Cyrillic) spelling of Bulgarian.
 Src/Common: LanguageSelect.cpp

2004-03-15 Kimmo
 RFE: [ 874366 ] Add goto line-number
  Scroll line searched to center of view, as suggested by harry mckame
  Src: MergeEditView.cpp
 Change URLs to point http://winmerge.org 
  (As discussed in BUG #916674 Hardcoded url to help and website from help-menu)
  MainFrm.cpp Merge.cpp

2004-03-14 Kimmo
 RFE [ 897975 ] Syntax highlighting for INI files
  Color "[...]"
  Src/editlib: ini.cpp
 Apply icon change (2004-03-13 Seier) to languages
  Languages/res: Merge.ico
 PATCH: [ 913743 ] Icons for menuitems (RFE 873697)
  Submitted by Tim Gerundt
  Src: MainFrm.cpp Merge.rc resource.h
  Src/res: Toolbar.bmp
  Src/res new files: close.bmp
  Languages: *.rc resource.h
   res/bmp files
 BUG: [ 916028 ] Deleting unique file does not remove it from view
  Src: DirActions.cpp

2004-03-13 Seier
 Add 48*48 icon version (256 & 16 color)
  Src: /res/Merge.ico

2004-03-13 Kimmo
 Add User Manual draft to CVS
  Docs/Users/Manual: *.*
 PATCH: [ 915145 ] Delay automatic close of compare pane
  Src: DirView.cpp

2004-03-13 WinMerge experimental release 2.1.5.15 (cvs)

2004-03-11 Kimmo
 PATCH: [ 902191 ] Uninline CDiffTextBuffer functions
  Src: GhostTextBuffer.cpp MergeDoc.cpp MergeDoc.h
 BUG: [ 906467 ] Link error with Visual Studio 2003
  Fix submitted by hberni
  Merge7z: Merge7z.h
  Src: DirActions.cpp
 PATCH: [ 911442 ] Sort folders first when sorting by name
  Also fixing grouping doxygen comments for functions
  Src: DirViewColItems.cpp

2004-03-11 WinMerge experimental release 2.1.5.14 (installer test)

2004-03-11 Kimmo
 PATCH: [ 913792 ] Cache color options for MergeView
  Src: MergeDoc.cpp MergeEditView.cpp MergeEditView.h
 BUG: [ 872882 ] Some selected menu items looks to small
  Adds missing handler functions BCMenu needs
  Also added code to implement document menus (commented out for now)
  Src: MainFrm.cpp MainFrm.h Merge.rc
 PATCH: [ 913743 ] Icons for menuitems (RFE 873697)
  Submitted by Tim Gerundt
  Src: MainFrm.cpp Merge.rc resource.h
  Src/res new files: cascade.bmp cut.bmp cut.bmp horizontally.bmp
   paste.bmp search.bmp vertically.bmp
  Languages: *.rc resource.h
   res/bmp files

2004-03-10 Kimmo
 Fix for OptionsMgr bugs when loading string options from registry
  UNICODE strings were not handled correctly, incorrect buffer lengths
  Src/Common: OptionsMgr.cpp OptionsMgr.h
 PATCH: [ 912754 ] Close open MergeDoc before opening new from DirDoc
  Src: DirDoc.cpp DirDoc.h DirView.cpp
 RFE: [ 874366 ] Add goto line-number
  Show few lines of context above line scrolled to
  Src: MergeEditView.cpp
 PATCH: [ 913073 ] Option to automatically close compare pane
  Src: DirView.cpp DirView.h MainFrm.cpp Merge.rc OptionsDef.h
   PropGeneral.cpp PropGeneral.cpp resource.h
  Languages: *.rc resource.h

2004-03-09 Kimmo
 PATCH: [ 912297 ] Splashscreen and syntax highlight options
  Src: MainFrm.cpp Merge.cpp Merge.h MergeDiffDetailView.cpp
   MergeEditView.cpp OptionsDef.h
 Fix compile of CShellFileOp with Visual Studio 2003
  Fix from original author Michael Dunn
  Common: CShellFileOp.cpp

2004-03-08 Kimmo
 PATCH: [ 911350 ] Restore default compare options
  Src: MainFrm.cpp Merge.rc PropCompare.cpp PropCompare.h resource.h
  Languages: *.rc resource.h
 PATCH: [ 892397 ] Distinguish prediffer from unpacker
  Submitted by Laoran
  Src: DiffWrapper.cpp DirScan.cpp FileTransform.cpp FileTransform.h
   MainFrm.cpp MergeDoc.cpp SelectUnpackerDlg.cpp
 BUG: [ 909779 ] Uninitialised variable in CGhostTextBuffer::DeleteText()
  Fix submitted by Laoran
  Src: GhostTextBuffer.cpp
 BUG: [ 909796 ] Uninitialised variable on OnSelchangeUnpackerName()
  Fix submitted by Laoran
  Src: SelectUnpackerDlg.cpp
 BUG: [ 909090 ] MainFrm::m_CheckOutMulti not initialized
  Also m_bVCProjSync was unitilialised.
  Src: MainFrm.cpp

2004-03-07 Kimmo
 PATCH: [ 911114 ] DiffOptions to new system
  Src: ConfigLog.cpp ConfigLog.h DiffWrapper.cpp DiffWrapper.h DirDoc.cpp
   DirDoc.h MainFrm.cpp MainFrm.h MergeDoc.cpp MergeDoc.h OptionsDef.h
 PATCH: [ 906872 ] Change ShellExtension to open directories non-recursive
  ShellExtension: ShellExtension.rc WinMergeShell.cpp WinMergeShell.h
 RFE: [ 874366 ] Add goto line-number
  Improvement: Show file's current line in Goto-dialog as selected
  Src: MergeEditView.cpp
 Add contributors.txt
  Docs: contributors.txt

2004-03-06 WinMerge experimental release 2.1.5.13 (cvs)

2004-03-05 Kimmo
 PATCH: [ 909980 ] Fix buffer types and descriptions
  Src: MergeDoc.cpp MergeDoc.h
 Two unrelated lines got removed by checkin of #909980
  Src: MergeDoc.cpp

2004-03-04 Kimmo
 Fix invalid return in paths_IsPathAbsolute()
  paths.cpp
 PATCH: [ 907898 ] Detect filechange by another app before saving
  Src: DiffContext.cpp DiffContext.h Merge.dsp Merge.rc MergeDoc.cpp
   MergeDoc.h resource.h
  Src new files: DiffFileInfo.cpp DiffFileInfo.h
  Languages: *.rc resource.h

2004-03-03 Perry
 fix BUG: [ 902430 ] copy&paste comment bug
  Src: DirView.cpp

2004-03-03 Kimmo
 PATCH: [ 908606 ] Restore default colors
  Add "Defaults" button to Color options page
  Src/Common: OptionsMgr.cpp OptionsMgr.cpp
  Src: Merge.rc PropColor.cpp PropColors.h resource.h
  Languages: recource.h *.rc
 PATCH: [ 908211 ] DiffContext.h should not include RegExp.h
  Submitted by Perry
  Src: DiffContext.h DirDoc.h
 BUG: [ 908217 ] MergeDoc has incorrect include
  Submitted by Perry
  Src: MergeDoc.h
  
2004-03-02 Kimmo
 PATCH: [ 907087 ] Allow commandline switches for external editor
  Src/Common: coretools.cpp coretools.h
  Src: MainFrm.cpp
 PATCH: [ 907641 ] Convert color options to new system
  Src: ColorButton.cpp ColorButton.h MainFrm.cpp Merge.cpp Merge.h
   MergeDiffDetailView.cpp MergeEditView.cpp OptionsDef.h
   PropColors.cpp PropColors.h

2004-03-01 Kimmo
 PATCH: [ 906180 ] Use SHBrowseForFolder() to select folder
  Src: Merge.cpp Merge.h
 FIX wrong checkin comments and wrong patch number and description
  in previous checkin.
 PATCH: [ 906934 ] Fix filenames in patch-files
  Src: DiffWrapper.cpp

2004-02-28 WinMerge experimental release 2.1.5.12 (cvs)

2004-02-28 Kimmo
 PATCH: [ 906120 ] Improve error handling for dircompare fileoperations
  Src: DirActions.cpp
 PATCH: [ 906224 ] Convert more options to new system
  Src: MainFrm.cpp MainFrm.h MergeDiffDetailView.cpp MergeDoc.cpp
   MergeDoc.h MergeEditView.cpp OptionsDef.h

2004-02-27 Kimmo
 PATCH: [ 905115 ] Put deleted files to Recycle Bin
  Also added option to disable it in "System" options
  DirActions.cpp MainFrm.cpp Merge.rc OptionsDef.h PropRegistry.cpp
  PropRegistry.h resource.h
 Fix copy/paste error
  Src: DirActions.cpp
 Better absolute path check (I thought I already applied this)
  Src: paths.cpp

2004-02-26 Kimmo
 PATCH: [ 903670 ] Copy Folder (and convert to CShellFileOp)
  Src: DirActions.cpp DirView.h MainFrm.cpp MainFrm.h

2004-02-23 Kimmo
 PATCH: [ 902738 ] Two errors in resource (Whitespaces and Hilight)
  Also RFE [ 837773 ] fix all about boxes
  Src: Merge.rc
  Languages: *.rc

2004-02-21 Kimmo
 PATCH: [ 896153 ] OptionsManager
  Src/Common: new files OptionsMgr.cpp OptionsMgr.h
  Src: DirDoc.cpp DirView.cpp MainFrm.cpp MainFrm.h Merge.dsp MergeDoc.cpp
   MergeEditView.cpp 
  Src: new file OptionsDef.h
 PATCH: [ 900718 ] Updated German translation
  Submitted by Tim Gerundt
  Languages/German: MergeGerman.rc

2004-02-20 Perry
 Fix CopyAll.bat for new directory structure.
  Src: Languages\CopyAll.bat
 Add GetClipTcharTextFormat (needed by coretools now) to StdAfx.*
  MakeResDll: StdAfx.cpp StdAfx.h
 Fix MakeResDll.dsp to compile (new directory structure).
  MakeResDll\MakeResDll.dsp

2004-02-20 WinMerge experimental release 2.1.5.11 (cvs)

2004-02-20 Kimmo
 PATCH: [ 899926 ] Write LogErrorString()s to logfile
  Src: DiffWrapper.cpp MainFrm.cpp MergeDoc.cpp StdAfx.cpp

2004-02-19 Jochen
 BUG: [ 888992 ] 7zip integration: Error deleting file when closing WinMerge
  Src: 7zCommon.cpp

2004-02-19 Kimmo
 PATCH: [ 899145 ] Check if patch path is absolute
  Src: Merge.rc PatchDlg.cpp paths.cpp paths.h resource.h
  Languages: *.rc resource.h (incl. changes from #889357)
 BUG: [ 893410 ] Typo in IDS_DIFF_FILEOVERWRITE
  Src: Merge.rc
  Languages: *.rc
 RFE: [ 897975 ] Syntax highlighting for INI files
   Patch submitted by Tim Gerundt
   Src: Merge.dsp
   editlib: ccrystaltextview.cpp ccrystaltextview.h
   editlib new file: ini.cpp

2004-02-18 Jochen
 PATCH: [ 889357 ] Attempt to improve DirScan speed
  The VERIFY(::DeleteFile... ) in prepAndCompareTwoFiles() won't fire as long as
  the gLog.m_nMaskLevel's LOGLEVEL::LSILENTVERIFY bit is set, which is the case
  by default.
  Common: logfile.cpp logfile.h
  Src: stdafx.cpp stdafx.h Merge.rc resource.h Diff.cpp DiffContext.cpp
   DiffContext.h DiffWrapper.cpp DiffWrapper.h DirCompStateBar.cpp
   DirCompStateBar.h DirScan.cpp DirView.h DirViewColHandler.cpp
   DirViewColItems.cpp DirViewColItems.h files.cpp FileTransform.h
   stringdiffs.cpp

2004-02-18 Kimmo
 BUG: [ 898745 ] Wrong result for folders (non-recursive compare)
  Src: DirViewColHandler.cpp

2004-02-17 Kimmo
 BUG: [ 887280 ] Swap toolbar icon (usability)
  Swaps 'Last Diff' and 'First diff' buttons + adds separator before
  Src: Merge.rc
  Src/res: Toolbar.bmp
  Languages: *.rc
  Languages/res: Toolbar.bmp 

2004-02-16 Kimmo
 PATCH: [ 873151 ] Attempt to fix Help/Configuration
  Submitted by Perry
  Src: paths.cpp
 PATCH: [ 891364 ] Enumerated doctype (scratchpads)
  Src: MergeDoc.cpp MergeDoc.h

2004-02-09 Kimmo
 BUG: [ 893287 ] Assign wrong file property when storing changes.
  Src: MainFrm.cpp

2004-02-08 Perry
 Add header comments to functions lacking such in StdAfx.cpp
  Src: StdAfx.cpp

2004-02-08 Kimmo
 PATCH: [ 892509 ] Reset dircompare statuspane before new compare
  Src: DirCompStateBar.cpp DirCompStateBar.h DirFrame.cpp
 PATCH: [ 890758 ] Fix and tune doxy config file
  Docs: doxygen.cfg
 BUG: [ 887271 ] Weird context menu position if opened from keyboard
  Src: MergeEditView.cpp

2004-02-07 Laoran
 BUG: [ 892158 ] Empty lines added to end of file
  Src: MergeDoc.cpp

2004-02-07 WinMerge experimental release 2.1.5.9 (cvs)

2004-02-07 Kimmo
 BUG: [ 892115 ] Directory compare always recursive
  Src: DirScan.cpp

2004-02-05 Kimmo
 Remove doubled entrys from MessageMap
  Src: MergeEditView.cpp

2004-02-05 Perry
 BUG: [ 876500 ] OlecharToUTF8 doesn't distinguish UCS-2BE?
 Add function header comment to OlecharToUTF8 in multiformatText.cpp
  Common: multiformatText.cpp

2004-02-04 Kimmo
 PATCH: [ 888662 ] Add Editor options page
  Src: MainFrm.cpp Merge.dsp Merge.rc PropGeneral.cpp PropGeneral.h resource.g
  Src new files: PropEditor.cpp PropEditor.h
  Src removed files: PropSyntax.cpp PropSyntax.h
  Lanuages: *.rc resource.h

2004-02-04 Laoran
 Correcting the path for the original resource.h
  Languages: UpdateAll_resource_h.bat

2004-02-03 Perry
 PATCH: [ 882683 ] Combine transform code in DirScan.cpp
  Src: DirScan.cpp FileTransform.cpp FileTransform.h MergeDoc.cpp
 PATCH: [ 890019 ] Fix diff_flag to bin_flag in CDiffWrapper::RunFileDiff
  Src: DiffWrapper.cpp

2004-02-03 Kimmo
 Fix "Copy Right to.." (copy/paste error)
  Src: DirActions.cpp
 PATCH: [ 889115 ] Delete transformed temp files in file compare
  Src: DiffWrapper.cpp
 PATCH: [ 889334 ] Improve and enable logging
  Logging is now enabled for debug builds, written to $temp/WinMerge.log
  Common: LogFile.cpp LogFile.h
  Src: DiffWrapper.cpp DirActions.cpp DirDoc.cpp DirScan.cpp
   MainFrm.cpp MergeDoc.cpp StdAfx.cpp
 PATCH: [ 887800 ] Add frames around items in dircompare statusbar
  Src: DirCompStateBar.cpp DirCompStateBar.h Merge.rc
  Languages: *.rc

2004-02-01 Perry
 Cosmetic: fix copy&paste error in UniFile.cpp comment.
 Src/Common: UniFile.cpp

2004-02-01 Kimmo
 PATCH: [ 885496 ] Move compare options to Compare-page
  src: MainFrm.cpp Merge.rc PropCompare.cpp PropCompare.h
   PropGeneral.cpp PropGeneral.h
  Languages: *.rc
 PATCH: [ 887085 ] CMergeDoc::TrySaveAs() cleanup
  src: MergeDoc.cpp MergeDoc.h
 Remove some unused variables
  src: DiffWrapper.cpp MergeDoc.cpp PatchDlg.cpp StringDiffs.cpp
  src/common: multiformatText.cpp
 Initialise all PackingInfo members
  src: FileTransform.h

2004-01-31 Laoran
 BUG: [ 885018 ] Crash when trying to save scratchpad
  src: MergeDoc.cpp

2004-01-31 Kimmo
 PATCH: [ 887283 ] Menu cleanup
  scr: Merge.rc
  Languages: *.rc

2004-01-31 Perry
 BUG: [ 887957 ] Some builds of ShellExtension need include dir fixed
  ShellExtension: ShellExtension.dsp
 PATCH: [ 882650 ] Move temp build files out of Build into BuildTmp (more)
  ShellExtension: ShellExtension.dsp
 [ 879536 ] Add MakePatchDirs source to repository
  MakePatchDirs: AppVersion.cpp AppVersion.h CDirDialog.cpp CDirDialog.h
   CMoveConstraint.cpp CMoveConstraint.h CSubclass.cpp CSubclass.h
   ChangeLog.txt DropEdit.cpp DropEdit.h MakeDirs.cpp MakeDirs.h
   MakePatchDirs.dsp MakePatchDirs.ico MakePatchDirs.rc MakePatchDirs.rc2
   MakePatchDirsApp.cpp MakePatchDirsApp.h MakePatchDirsDlg.cpp 
   MakePatchDirsDlg.h ReadMe.txt StdAfx.cpp StdAfx.h dist.txt exc.cpp
   exc.h resource.h

2004-01-30 WinMerge experimental release 2.1.5.8 (cvs)

2004-01-30 Kimmo
 BUG: [ 887543 ] Word-diff (No difference) messagebox has Cancel-button
  src: MergeDocLineDiffs.cpp

2004-01-29 Kimmo
 PATCH: [ 885092 ] CMergeDoc::DoSave() cleanup
  src: MergeDoc.h MergeDoc.cpp

2004-01-27 Kimmo
 PATCH: [ 884301 ] Modified Visual Basic syntax highlighting
  Submitted by Tim Gerundt
  src/editlib: basic.cpp

2004-01-26 Christian
 PATCH: [ 883303 ] Upgrade WinMerge Home Page
  Web: index.php winmerge.css WinMergePAD.xml images/redswirl.gif

2004-01-26 Kimmo
 BUG: [ 846034 ] drag and drop files does not set extension
  src: OpenDlg.cpp

2004-01-26 Jochen
 Avoid copying DIFFITEMs
  This badly slowed down DirView sorting and navigation.
  src: DirViewColHandler.cpp DirView.cpp DirView.h 7zCommon.cpp 7zCommon.h
 Fix ASSERT in vc98/mfc/src/olevar.cpp when sorting DirView by date
  src: DirViewColItems.cpp

2004-01-25 Jochen
 Fix bad default for OPENFILENAME::nFilterIndex
  src: 7zCommon.cpp

2004-01-24 Perry
 [ 882650 ] Move temp build files out of Build into BuildTmp
  Src: Merge.dsp
  MakeResDll: MakeResDll.dsp

2004-01-23 Kimmo
 PATCH: [ 881602 ] Don't allow canceling saving when its pointless
  src: MainFrm.cpp MergeDiffDetailView.cpp MergeDoc.cpp MergeDoc.h MergeEditView.cpp
 PATCH: [ 880769 ] Select all in Dir compare
  src: DirView.h DirView.cpp Merge.rc
  Languages: *.rc
 Minor update to 2.2 plan - couple of items completed, two new items (25, 26)

2004-01-23 Perry
 PATCH: [ 880006 ] Code cleanup--old commented out lines in DirView.h
  src: DirView.h
 Cleanup stdafx.h; comment includes & declarations, remove unneeded include of afxsock.h
  src: StdAfx.cpp StdAfx.h
 PATCH: [ 802113 ] strip out spurious tests: _MSC_VER >= 1000 { part #2/2 }
  src: CCPrompt.h ColorButton.h DiffViewBar.h DirCompStateBar.h MergeEditView.h
   OutputDlg.h PatchDlg.h PropColors.h PropCompare.h PropFilter.h PropRegistry.h
   PropSyntax.h SelectUnpackerDlg.h WMGotoDlg.h
  src/Common: SplitterWndEx.h

2004-01-23 Kimmo
 PATCH: [ 848159 ] RFE 826652 : Compare by modified date
  Submitted by Tim Musschoot - some modifications by Kimmo
  src: DirScan.cpp MainFrm.h MainFrm.cpp Merge.dsp Merge.rc resource.h
  src new files: PropCompare.h PropCompare.cpp
  Languages: *.rc resource.h

2004-01-22 Perry
 Add some comments before initialization of g_cols
  src: DirViewColItems.cpp

2004-01-21 Perry
 PATCH: [ 878631 ] Add generic named properties to DiffContext
  src: Diff.cpp DiffContext.cpp DiffContext.h DirDoc.cpp DirScan.cpp
   DirView.h DirViewColHandler.cpp DirViewColItems.cpp Merge.dsp
   Merge.rc resource.h
  src/Common: varprop.cpp varprop.h

2004-01-21 Kimmo
 PATCH: [ 880898 ] BCMenu update to v3.034
  src: BCMenu.h BCMenu.cpp

2004-01-20 WinMerge experimental release 2.1.5.7 (cvs)

2004-01-20 Jochen
 Fix bug 880320
  New class CSilentException to be thrown on subsequent failures in proxy linking to merge7z*.dll.
  src: 7zCommon.cpp 7zCommon.h MainFrm.cpp
  
2004-01-19 Jochen
 Finish Merge7z integration
  src: Merge.rc resource.h DirView.cpp DirView.h DirDoc.cpp MainFrm.cpp Merge.dsp
  Merge7z: Merge7z311.dsp Merge7z312.dsp Merge7z313.dsp

2004-01-19 Perry
 PATCH: [ 880046 ] Save every line including last one in SaveToFile
  src: MergeDoc.cpp
 UniStdioFile::OpenCreateUtf8 (shortcut for creating UTF-8 file)
  src/Common: UniFile.cpp UnFile.h

2004-01-19 Kimmo
 PATCH: 876875 Go To -Dialog
  Fix crash in GotoDlg. This was in latest patch, but I merged part of older patch :(
  MergeEditView.h MergeEditView.cpp

2004-01-19 Perry
 PATCH: [ 878625 ] Simplify and unify file operations calling diff_2_files.
  src: Diff.cpp DiffWrapper.cpp DiffWrapper.h
 PATCH: [ 847121 ] Code cleanup (remove unneeded AssertValid & Dump implementations)
  src: ChildFrm.cpp ChildFrm.h DirDoc.cpp DirDoc.h DirFrame.cpp DirFrame.h
   DirView.cpp DirView.h MainFrm.cpp MainFrm.h MergeDiffDetailView.cpp 
   MergeDiffDetailView.h MergeEditView.cpp MergeEditView.h
 PATCH: [ 802113 ] strip out spurious tests: _MSC_VER >= 1000 { part#1/2 }
  src: ChildFrm.h DirFrame.h DirView.h EditFile.h FilepathEdit.h
   GhostTextBuffer.h GhostTextView.h MainFrm.h Merge.h MergeDoc.h
   PropGeneral.h PropVss.h StdAfx.h VssPrompt.h
 PATCH: [ 880046 ] Save every line including last one in SaveToFile
  src: MergeDoc.cpp

2004-01-18 Kimmo
 PATCH: 876875 Go To -Dialog
  src new files: WMGotoDlg.h WMGotoDlg.cpp
  src: Merge.dsp Merge.rc MergeEditView.h MergeEditView.cpp resource.h
  src/Languages: *.rc resource.h
 PATCH: [ 872413 ] UnpackDFM TestStreamFormat
  Submitted by Michael Richter
  Plugins/Delphi/UnpackFDM: UnpackFDM.dpr UnpackFDMLib_TLB.pas
   WinMergeUnit.pas
  Plugins/dlls: UnpackFDM.dll

2004-01-17 Kimmo
 PATCH: [ 871789 ] Add "Copy to..." items to dirview context menu
  src/Common: new files CShellOp.h CShellOp.cpp
  src: DirActions.cpp DirView.h DirView.cpp Merge.cpp Merge.dsp
   Merge.h Merge.rc resource.h
  src/Languages: *.rc resource.h
 PATCH: [ 876556 ] (Minor) code cleanup for SaveToFile
  Submitted by Perry
  src: MergeDoc.cpp

2004-01-16 Perry
 Fix bug in UniFile::WriteString counting of written bytes.
  Src/Common/UniFile.cpp

2004-01-16 WinMerge experimental release 2.1.5.6 (cvs)

2004-01-15 Kimmo
 PATCH: [ 873940 ] Diff navigation code cleanups
  src: MergeEditView.cpp
 Remove _T()'s from _RPTF() macros (forgot to do before last checkin)
  src: MergeEditView.cpp

2004-01-15 Perry
 Remove rcs/cvs ID line from unicoder.* (because I share these elsewhere)
  src/WinMerge/Common: unicoder.cpp unicoder.h
 Fix bug in unicoder.cpp: WideCharToMultiByte: lpDefaultChar & lpUsedDefaultChar must be NULL when using UTF-8.
  src/WinMerge/Common: unicoder.cpp

2004-01-13 Perry
 PATCH: [ 876546 ] Fix CMergeDoc::CDiffTextBuffer::SaveToFile ghost line detect
  src/WinMerge: MergeDoc.cpp

2004-01-13 Jochen
 Start Merge7z integration (to be continued)
  src: 7zCommon.cpp 7zCommon.h DirView.h
 
2004-01-13 Kimmo
 PATCH: [ 875567 ] Compile after reorg
  Submitted by Christian to make WinMerge compile again!
  Src: Merge.dsp

2004-01-12 Kimmo
 PATCH: [ 874451 ] Update lineposition offpane when navigating diffs
 Fixes wrong linenumber and cursor position in inactive file
  WinMerge: MergeEditView.cpp

2004-01-11 Perry
 PATCH: [ 875237 ] Fix fwrite call in UniStdioFile::WriteString
 Fix byte count and argument order when UniStdioFile::WriteString calls fwrite.
  common: UniFile.cpp
 PATCH: [ 875215 ] Fix UnicodeFileToOlechar obtaining BOM
  common: multiformatText.cpp

2004-01-11 WinMerge experimental release 2.1.5.5 (cvs)

2004-01-11 Perry
 PATCH: [ 860054 ] Reimplement SaveToFile using stream output
  common: multiformatText.cpp unicoder.cpp unicoder.h UniFile.cpp UniFile.h
  WinMerge: DiffContext.cpp MergeDoc.cpp

2004-01-11 Kimmo
 Minor update to 2.2 plan
  Docs: Planning_2_2.txt

2004-01-11 WinMerge experimental release 2.1.5.4 (cvs)

2004-01-11 Perry
 Update Merge7z project files to build to ../Build directories
 in same fashion as WinMerge project
  Merge7z: Merge7z311.dsp Merge7z312.dsp Merge7z313.dsp

2004-01-09 Perry
 PATCH: [ 872762 ] Stricter error handling for Plugins
  WinMerge: Plugins.cpp

2004-01-09 Kimmo
 Fix parameter type, CMergeDoc::TrySaveAs() second param is int.
  WinMerge: CMergeDoc.cpp

2004-01-08 Perry
 PATCH: [ 872727 ] Fix function comment on lwdisp.c: ReportError
  common: lwdisp.c
 PATCH: [ 862040 ] CAssureScriptsForThread like WaitCursor
  WinMerge: DiffThread.cpp FileTransform.cpp Merge.cpp Merge.h
   MergeEditView.cpp Plugins.cpp Plugins.h SelectUnpackerDlg.cpp
 PATCH: [ 860072 ] Allow singlethreaded diffs
  WinMerge: DiffThread.cpp


2004-01-08 Kimmo
 Updated Russian translation from Dmitriy Aleshkowskiy
  Languages/Russian: MergeRussian.rc

2004-01-08 Perry
 PATCH: [ 872032 ] Add SEH around calls to load plugins
  WinMerge: Plugins.cpp

2004-01-07 jtuc
 Added new module Merge7z
  Merge7z: Merge7z.h Merge7z311.cpp Merge7z311.dsp Merge7z312.dsp
   Merge7z313.dsp Merge7zCommon.cpp Merge7zCommon.h StdAfx.cpp
   StdAfx.h tools.cpp tools.h

2004-01-07 Kimmo
 PATCH: [ 856396 ] Browse folders in non-recursive directory compare
  WinMerge: DirDoc.h DirDoc.cpp DirScan.cpp DirView.h DirView.cpp
   DirViewColHandler.cpp Merge.dsp Merge.rc Paths.h Paths.cpp resource.h
  WinMerge/res: new file folder.bmp
  Languages: *.rc resource.h
  Languages/res: new file folder.bmp

2004-01-06 Kimmo
 Add Polish translation contributed by Pawel Wawrzyszko
 Languages/Polish/*
 Languages: BuildAll.bat UpdateAll_resource_h.bat

2004-01-06 WinMerge experimental release 2.1.5.3 (cvs)

2004-01-06 Perry
 PATCH: [ 869575 ] UniMemFile needs to init charsize to 1
  common: UniFile.cpp
 PATCH: [ 869518 ] Fix IO.C to count non-UNIX prefix lines correctly
  WinMerge: IO.C

2004-01-04 Kimmo
 BUG: [ 869289 ] Typo in XML_html.flt excludes gif files
  Filters: XML_html.flt

2004-01-02 Kimmo
 PATCH: [ 868584 ] Move DIRSEL_TAG to string resource
  WinMerge: Merge.rc OpenDlg.cpp resource.h
  Languages: *.rc resource.h
 Adding missing line in patch [ 868584 ] and fixing one tab
  WinMerge: OpenDlg.cpp

2003-12-28 Perry
 PATCH: [ 863738 ] Various Updates in German Translation
  Updates to German translation by Tim Gerundt (gerundt), based on 1.73 MergeGerman.rc
  Languages: German/MergeGerman.rc
 PATCH: [ 860472 ] Fix SuperComboBox to handle its own OLE requirements
  common: SuperComboBox.cpp SuperComboBox.h
 New Russian translation by Dmitriy Aleshkowskiy
  Languages\Russian\*

2003-12-22 Perry
 Put ID_POPUP_COPYALLDIFFSFROMOTHERSIDE back into rc file so it parses
  Winmerge: Merge.rc

2003-12-22 Laoran
 BUG: [ 860043 ] binary file detection broken
  WinMerge: DiffWrapper.cpp DirScan.cpp

2003-12-19 Kimmo
 PATCH: [ 861099 ] Dirview context menu cleanup
  WinMerge: DirView.cpp Merge.rc resource.h
  Languages: *.rc resource.h

2003-12-18 Kimmo
 PATCH: [ 860988 ] Change File from menu (Adds Window/Change Pane)
  WinMerge: Merge.rc MergeEditView.h MergeEditView.cpp resource.h
  Languages: *.rc resource.h
 PATCH: [ 861813 ] Optimise dircompare statuspane updating
  Fixes bug [ 860204 ] Dir compare statuspane flickers on slower machine
  WinMerge: DirCompStateBar.h DirCompStateBar.cpp
  
2003-12-18 Laoran
 adapt plugins syntax check, consequence of patch #853147 
  WinMerge: Plugins.cpp

2003-12-17 Kimmo
 PATCH: [ 858283 ] Add isResultDiff() to DiffItem
  Fix bug in patch (DirDoc.cpp)
  WinMerge: DirDoc.cpp

2003-12-17 Laoran
 update plugin docs and examples, consequence of patch #853147 
  plugins: list of sources.txt readme.txt syntax.txt
  plugins\dlls: RCLocalizationHelper.dll list.txt
  plugins\src_VB\IgnoreTwoFirstCharsOrder: WinMergeScript.cls
  plugins\src_VCPP\RCLocalizationHelper: RCLocalizationHelper.idl WinMergeScript.cpp WinMergeScript.h

2003-12-16 Perry
 PATCH: [ 860072 ] Allow singlethreaded diffs
  Add unused code to trigger diffs in main thread (for developer to trigger via breakpoint).
  WinMerge: DiffThread.cpp
 unpatch: Reverse (back out) patch [ 860072 ] Allow singlethreaded diffs
  WinMerge: DiffThread.cpp

2003-12-16 Kimmo
 PATCH: [ 858283 ] Add isResultDiff() to DiffItem
  Fixes BUG [ 852998 ] Left Only being filtered out if not view different
  WinMerge: DiffContext.h DirDoc.cpp

2003-12-14 Kimmo
 BUG: [ 833096 ] Left/right-only status overwrites skipped-status
  Apply fix for long result too
  WinMerge: DirViewColItems.cpp
 PATCH: [ 851586 ] Improve Patch generate dialog
  Also implements RFE #849890 Disable first line in patch-file
  WinMerge: DiffWrapper.h DiffWrapper.cpp Merge.rc PatchDlg.h PatchDlg.cpp
   PatchTool.cpp resource.h
  Languages: *.rc resource.h

2003-12-11 Kimmo
 Adding 2.2 plan
  Docs: Planning_2_2.txt

2003-12-11 Laoran
 remove files UnicodeUtf8, consequence of patch #853147 
  WinMerge: FileTransform.cpp Merge.dsp
  WinMerge delete: UnicodeUtf8.cpp UnicodeUtf8.h
 PATCH: [ 853147 ] New class for text conversion in plugins
  common: unicoder.cpp unicoder.h
  common add: multiformatText.cpp multiformatText.h
  WinMerge: DiffWrapper.cpp DirScan.cpp FileTransform.cpp FileTransform.h Merge.dsp MergeDoc.cpp 
   Plugins.cpp Plugins.h SelectUnpackerDlg.cpp

2003-12-10 Perry
 Add comment inside CMergeDoc::CDiffTextBuffer::SaveToFile about existing buggy code.
  WinMerge: MergeDoc.cpp

2003-12-10 Kimmo
 PATCH: [ 857003 ] Move linediff messagebox text into resource
  WinMerge: Merge.rc MergeDocLineDiffs.cpp resource.h
  Languages: *.rc resource.h
 PATCH: [ 856974 ] More space for numbers in dir compare statuswindow
  WinMerge: Merge.rc
  Languages: *.rc

2003-12-09 Laoran
 BUG: [ 842812 ] Some extension filtering happening by default (fix for *.*)
  WinMerge: DirDoc.cpp 
 Don't preprend a backslash to filename if there is already one (test for filter)
  WinMerge: DirDoc.cpp

2003-12-04 Laoran
 BUG: [ 836868 ] Search and replace field size limited (apply to languages)
  Languages : *.rc
 PATCH: [ 848476 ] Wrap end of file in replace dialog
  WinMerge: Merge.rc resource.h
  editlib: ccrystaleditview.cpp ccrystaltextview.h ceditreplacedlg.cpp ceditreplacedlg.h cfindtextdlg.h
  Languages : *.rc resource.h

2003-12-03 WinMerge experimental release 2.1.5.1 (cvs)

2003-12-01 Laoran
 reclassing strings in resource.h in same order as in Merge.rc
  WinMerge: resource.h
  Languages: resource.h

2003-12-01 Kimmo
 PATCH: [ 852062 ] XML/HTML file filter
 Filters: new file XML_html.flt

2003-11-30 Perry
  Added comment giving Unicode for hanguko (Korean in Hangul alphabet)
  common: LanguageSelect.cpp: Korean/MergeKorean.rc

2003-11-29 Laoran
 PATCH: [ 847884 ] Remove obsolete registry keys
  WinMerge: PropRegistry.cpp PropRegistry.h
  InnoSetup: WinMerge.iss

2003-11-29 Kimmo
 Rename folder1.bmp to lfolder.bmp (suggested in PATCH [ 841039 ] Green "binary.bmp")
  WinMerge: Merge.dsp Merge.rc
  Added WinMerge/res/lfolder.bmp
  Removed WinMerge/res/folder1.bmp
  Updated languages

2003-11-28 Laoran
 Add header and RCS ID
  common: lwdisp.h unicoder.cpp unicoder.h FileTransform.h Plugins.h SelectUnpackerDlg.h

2003-11-28 Kimmo
 BUG: [ 848725 ] Crash trying to generate report
  Fix submitted by Perry
  WinMerge: DirView.cpp

2003-11-27 Perry
 PATCH: [ 849424 ] Fix worddiffs for whitespace
 Handle Sensitive to whitespace in stringdiffs.
  WinMerge: MergeDocLineDiffs.cpp stringdiffs.cpp
 PATCH: [ 842914 ] Add codepage guessing, and codepage column
 Optional codepage column in dirview. Code to check RC & HTML files for codepage
  WinMerge: DiffContext.cpp DiffContext.h DirViewColItems.cpp Merge.rc resource.h
  Languages: All resource.h and *.rc files.

2003-11-26 Perry
 PATCH: [ 842534 ] Move copy & delete ops to submenus in dirview popup
 First part of patch; moved menu items into popup submenus.
  WinMerge: DirActions.cpp DirView.cpp DirView.h Merge.rc resource.h
  Languages: All resource.h and *.rc files.

2003-11-26 Kimmo
 PATCH: [ 848955 ] Don't ask about saving after user selects 'Cancel'
  WinMerge: MergeDoc.cpp

2003-11-25 Perry
 PATCH: [ 849138 ] Fix stringdiffs for diff in first word
  (Code was trying to access word#-1 when diff hit in first word)
  WinMerge: stringdiffs.cpp stringdiffs.h

2003-11-25 Kimmo
 Missed one diff in PATCH: [ 847697 ] Move desc texts into doc classes
 WinMerge: MergeDoc.cpp

2003-11-25 WinMerge experimental release 2.1.5.0 (cvs)

2003-11-24 Perry
 PATCH: [ 847134 ] More commenting for DirScan (doxygenize function comments)
  WinMerge: DirScan.cpp DirScan.h
 [ 846405 ] Highlight individual word diffs in line
  (Enhance F4 to compute individual differences, and toggle through them in a cycle)
  WinMerge: Merge.dsp MergeDoc.cpp MergeDoc.h MergeDocLineDiffs.cpp
   stringdiffs.cpp stringdiffs.h stringdiffsi.h

2003-11-24 Laoran
 BUG: [ 848461 ] Toggle 'show different files' is broken (thanks to Kimmo)
  WinMerge: DirDoc.cpp

2003-11-24 Perry
 Improvements to language names in LanguageSelect.cpp (and reference URLs), from Laurent.
 common: LanguageSelect.cpp

2003-11-24 Kimmo
 PATCH: [ 847137 ] Report from dir compare results
  Submitted by Vikrant
  common: coretools.h coretools.cpp
  WinMerge: DirView.h DirView.cpp Merge.rc resource.h
  Languages: *.rc resource.h
 Within PATCH: [ 847137 ] - add space to New File item tooltip
 PATCH: [ 847137 ] Report from dir compare results
  Missed one diff from DirView.cpp
  WinMerge: DirView.cpp
 PATCH: [ 847697 ] Move desc texts into doc classes
  Fixes bug [ 847635 ] filenames after scratchpad
  WinMerge: DirDoc.h DirDoc.cpp MainFrm.h MainFrm.cpp  MergeDoc.h MergeDoc.cpp

2003-11-23 Perry
 Fix Exceptions.h for Unicode build
  WinMerge: Exceptions.h

2003-11-23 Laoran
 PATCH: [ 846237 ] Fix for the maximized/restored state (dirframe, docframe)
  WinMerge: ChildFrm.cpp DirFrame.cpp
 PATCH: [ 843124 ] Initialize COM in secondary threads
  WinMerge: Plugins.cpp Plugins.h
 BUG: [ 846622 ] rc files wrongly displayed as identical (fix : no automatic prediffing)
  WinMerge: FileTransform.cpp FileTransform.h MainFrm.cpp
 cosmetic: replace bUnpackerMode with m_bUnpackerMode
  WinMerge: FileTransform.cpp FileTransform.h MainFrm.cpp
 PATCH: [ 846523 ] error description for plugins error handler
  WinMerge: Merge.dsp Merge.rc Plugins.cpp
  WinMerge add: Exceptions.h

2003-11-23 Kimmo
 PATCH: [ 847638 ] Unused member variables in MainFrm.h
  WinMerge: MainFrm.h

2003-11-23 Laoran
 typo in TrySaveAs and merge error in DoSave
  WinMerge: MergeDoc.cpp

2003-11-23 Perry
 PATCH: [ 842906 ] Hardcode native language names into LanguageSelection.cpp
  Now Merge.rc holds all ASCII language names, and non-ASCII versions (if different)
   are in LanguageSelection.cpp, as Unicode strings.
  WinMerge: Merge.rc
  common: LanguageSelection.cpp LanguageSelection.h

2003-11-22 Kimmo
 PATCH: [ 820676 ] Scratchpad
  WinMerge: MainFrm.h MainFrm.cpp Merge.rc MergeDoc.h MergeDoc.cpp resource.h
  WinMerge/res: Toolbar.bmp
  Languages:  *.rc resource.h
  Languages/res: Toolbar.bmp

2003-11-22 Perry
 PATCH: [ 844780 ] MergeDoc code cleanup
 (removed AssertValid, Dump, and unused Serialize body)
  WinMerge: MergeDoc.cpp MergeDoc.h
 PATCH: [ 847126 ] Split large function in DirScan into 2 pieces
  WinMerge: DirScan.cpp
 PATCH: [ 847138 ] Initialize some variables in Plugins functions
  WinMerge: Plugins.cpp
 PATCH: [ 847149 ] Fix typo in DirScan (i->j)
  WinMerge: DirScan.cpp

2003-11-21 Laoran
 BUG: [ 846450 ] "Winmerge executable:" label missing from registry options
  WinMerge: Merge.rc
  Languages: *.rc
 cosmetic in Dirscan : FilterAdd -> StoreDiffResult
  WinMerge: DirScan.cpp
 SaveToFile : returned value (BOOL -> int)
 cosmetic changes in TrySaveAs/DoSave/OnFileSaveAsLeft/OnFileSaveAsRight
  WinMerge: MergeDoc.cpp MergeDoc.h

2003-11-19 WinMerge beta 2.1.4.0 released

2003-11-19 Laoran
 BUG: [ 845196 ] UTF-8 files not correctly compared
  WinMerge: MergeDoc.cpp MergeDoc.h 
 BUG: [ 845012 ] SaveToFile does not write BOM
  WinMerge: MergeDoc.cpp

2003-11-18 Laoran
 PATCH: [ 842817 ] Initialize OpenDlg extension filter to blank
 fix proposed by Perry
  WinMerge: OpenDlg.cpp
 BUG: [ 840759 ] Failed to load library UnpackDFM.dll
 fix proposed by Jochen
  common: lwdisp.c

2003-11-17 Perry
 [ 843526 ] cosmetic changes from the archives patch
  WinMerge: DirDoc.cpp DirDoc.h MainFrm.cpp

2003-11-16 Perry
 PATCH: [ 841897 ] Bail out of LoadFromFile if see embedded zeros
  WinMerge: MergeDoc.cpp

2003-11-16 Kimmo
 BUG: [ 818416 ] is stat population missing from RunFileDiff ?
  WinMerge: DiffWrapper.cpp

2003-11-16 Laoran 
 BUG: [ 842746 ] Highlight diff tool button lacks tooltip
  WinMerge: Merge.rc
  Languages: *.rc
  
2003-11-15 Perry
 PATCH: [ 838290 ] Move inf creation down in RunFileDiff (cosmetic)
  WinMerge: DiffWrapper.cpp
 PATCH: [ 841878 ] Fix wchar to char conversions at save
  WinMerge: MergeDoc.cpp MergeDoc.h
  common: unicoder.cpp unicoder.h UniFile.cpp UniFile.h

2003-11-15 WinMerge experimental release 2.1.3.9 (cvs)

2003-11-14 Laoran 
 PATCH: [ 840296 ] Informative messageboxes when loading files
  WinMerge: files.h MainFrm.cpp MainFrm.h Merge.rc MergeDoc.cpp resource.h
  Languages: *.rc resource.h
 PATCH: [ 841062 ] Alternative icons for PATCH [ 840273 ] (for hilight line diff)
 Submitted by Tim Gerundt
  WinMerge: Merge.rc
  res: Toolbar.bmp
  Languages: *.rc
  Languages/res: Toolbar.bmp

2003-11-14 Kimmo
 PATCH: [ 841767 ] Store unique item path
  Fixes bug: #840547 Copying unique file does not update date and size
  WinMerge: DirScan.cpp
 PATCH: [ 841889 ] Make long path in CConfigLog::WriteLogFile
  Submitted by Perry
  WinMerge: ConfigLog.cpp
 PATCH: [ 841039 ] Green "binary.bmp"?New binary item icons for dir compare)
  Submitted by Tim Gerundt
  WinMerge/res: binary.bmp binarydiff.bmp
  Languages/res: binary.bmp binarydiff.bmp

2003-11-14 Laoran 
 BUG: [ 838272 ] corrupt string operations in lwdisp.c
  common: lwdisp.c lwdisp.h

2003-11-13 Laoran 
 BUG: [ 841428 ] WinMerge executable path is deleted
  WinMerge: MainFrm.cpp PropRegistry.h

2003-11-13 Kimmo
 PATCH: [ 838084 ] Add * to newer file's date field in dir compare
  WinMerge: DirView.cpp DirViewColHandler.cpp
 BUG: [ 840785 ] Menu: Plugins | Automatic/Manual inconsistent
  WinMerge: MainFrm.cpp
 Code cleanup: use SetDiffCompare() and SetDiffSide()
  (I think those functions were designed to be used instead of SetDiffStatus())
  WinMerge: DirActions.cpp
 Don't update filedata multiple times + comment changes/updates
  (In journey to solve BUG: 840547 Copying unique file does not update date and size
  WinMerge: DirActions.cpp DirDoc.cpp DirView.cpp
 Removing one unneeded function + code cleanups + comment changes/updates
  (Hunting reason for BUG: 840547 Copying unique file does not update date and size)
  WinMerge: DiffContext.cpp DirDoc.h DirDoc.h 

2003-11-13 Laoran 
 modify the assert in GetText/GetTextWithoutEmptys (see patch #837480)
 submitted by Jochen
  WinMerge: GhostTextBuffer.cpp
  editlib: ccrystaltextbuffer.cpp
 Merging lwdisp.c with Jochen's changes (see patch #837480)
  common: lwdisp.c lwdisp.h Plugins.cpp
 PATCH: [ 838354 ] Revisit Speedup Speed up display of large differences
 submitted by Hern Chen (hern_chen)
  WinMerge: GhostTextBuffer.cpp MergeDoc.cpp
  editlib: ccrystaltextbuffer.cpp ccrystaltextbuffer.h

2003-11-11 Laoran
 avoid double message "no line difference"
  WinMerge: MergeDoc.cpp
 PATCH: [ 838530 ] Simplify GhostTextBuffer for Undo
  WinMerge: GhostTextBuffer.cpp GhostTextBuffer.h MergeDoc.cpp MergeDoc.h
 PATCH: [ 839970 ] Moving from LoadFromFile to OpenDocs all view functions
  WinMerge: MergeDoc.cpp

2003-11-11 Kimmo
 PATCH: [ 838545 ] Fix osversion stuff in ConfigLog.cpp
  Submitted by Perry
  WinMerge: ConfigLog.cpp
  WinMerge new file: winnt_supp.h
 PATCH: [ 838898 ] Cleanup Registry -optionpage
  WinMerge: MainFrm.cpp Merge.rc PropRegistry.h PropRegistry.cpp
  Languages: *.rc

2003-11-11 Laoran
 PATCH: [ 839243 ] F4 should hilight diff in both files
  WinMerge: MergeDoc.cpp MergeDoc.h
 fix for syntax highlight in left pane view
  WinMerge: MergeDoc.cpp
 update global options dialog : 'Allow mixing EOL' -> 'preserve original EOL chars'
 update global options dialog : 'ignore EOL' like in 2.0 
  WinMerge: Merge.rc PropGeneral.cpp PropGeneral.h
  Languages : *.rc
 PATCH: [ 831308 ] File filter selection text "<None>" hardcoded
  WinMerge: FileTransform.h Merge.cpp Merge.rc PropFilter.cpp SelectUnpackerDlg.cpp
   resource.h
  Languages : resource.h

2003-11-09 Laoran
 PATCH: [ 838574 ] SplitterWndEx OnHScroll
  common: SplitterWndEx.cpp SplitterWndEx.h
 fixing text alignment in dialogs (right aligned on screen->RTEXT, centered->CTEXT)
  WinMerge: Merge.rc
  Languages : *.rc

2003-11-09 Kimmo
 PATCH: [ 835198 ] Command line path arguments handling
  Submitted by Hern Chen
  WinMerge: Merge.cpp Paths.h Paths.cpp

2003-11-09 Laoran
 comments for invoke helpers (see BUG[ 838275 ])
  WinMerge: Plugins.cpp
  common: lwdisp.h

2003-11-08 Perry
 PATCH: [ 838283 ] Simplify OnFileSave
  WinMerge: MergeDoc.cpp

2003-11-08 WinMerge experimental release 2.1.3.8 (patch demo)

2003-11-08 WinMerge experimental release 2.1.3.7 (cvs)

2003-11-08 Laoran
 BUG: [ 838248 ] possible typo in plugin code (cosmetic)
  WinMerge: Plugins.cpp

2003-11-07 Perry
 PATCH: [ 836931 ] Move QueryRegMachine into RegKey
  common: RegKey.cpp RegKey.h
  WinMerge: MainFrm.cpp VssPrompt.cpp

2003-11-07 Laoran
 BUG: [ 837980 ] bug in save right as
  WinMerge: MergeDoc.cpp

2003-11-07 Kimmo
 BUG: [ 837760 ] regression since 2.1.3.6 (diff array)
  WinMerge: MergeDoc.cpp
 PATCH: [ 836927 ] Improved platform detection in ConfigLog
  Submitted by Perry
  WinMerge: ConfigLog.cpp

2003-11-07 Perry
 Updated Catalan translation (from Jordi Vilar, the translation author)
  Languages: Catalan/MergeCatalan.rc

2003-11-06 Laoran
 [ 836870 ] Scrollbar "bounces back" in diff pane.
  editlib: ccrystaltextview.cpp

2003-11-06 Kimmo
 BUG: [ 836781 ] File not found: notepad.exe
  WinMerge: MainFrm.cpp
 PATCH: [ 836714 ] Separate counts of normal and trivial diffs
  WinMerge: MergeDoc.h MergeDoc.cpp
 Backout PATCH: [ 834230 ] Use PostMessage() to update dir compare status
  It caused bug [ 837366 ] Dir compare status panel not drawn correctly
  WinMerge: DiffContext.cpp

2003-11-06 Laoran
 BUG: [ 836868 ] Search and replace field size limited
  WinMerge : Merge.rc
 BUG: [ 836384 ] File extension wildcards does not work. 
  WinMerge: OpenDlg.cpp

2003-11-05 Perry
 Typofix: "Wiew Whitespaces" -> "View Whitespace"
  WinMerge: ConfigLog.cpp
 Update language count mentioned in readme-developers.html
  Docs: readme-developers.html

2003-11-05 Kimmo
 BUG: [ 829435 ] Fix character classification routines for UNICODE
  common: RegExp.cpp

2003-11-05 Perry
 Added doxygen config file. (Subsequently fixed cvs log messages.)
  Docs: doxygen.cfg

2003-11-05 Kimmo
 BUG: [ 829435 ] Fix character classification routines for UNICODE
  WinMerge: paths.cpp stdafx.cpp
  editlib: cregexp.cpp

2003-11-05 WinMerge experimental release 2.1.3.6 (cvs)

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
