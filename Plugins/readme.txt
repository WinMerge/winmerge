2005-05-21 Perry
 PATCH [ 1206215 ] Fix DisplayBinaryFiles dll source to compile
 BUG [ 1206209 ] DisplayBinaryFiles.dll does not compile under MSVC7 (2003)
 BUG [ 1205960 ] DisplayBinaryFiles code does not compile in debug
  Fix compilation for DisplayBinaryFiles, for both MSVC6 (debug) and MSVC7
  DisplayBinaryFiles: DisplayBinaryFiles.dsp typeinfoex.h WinMergeScript.cpp
   new file ChangeLog.txt
 PATCH [ 1206256 ] Fix DisplayBinaryFiles plugin to handle unicode files
  DisplayBinaryFiles: ChangeLog.txt DisplayBinaryFiles.dsp DisplayBinaryFiles.rc
   WinMergeScript.cpp
   new files unicheck.cpp & unicheck.h

2005-01-27 Kimmo
 PATCH: [ 1109405 ] Binary diff is totally broken (buffer overwrite)
  Submitted by Jim Fougeron
  Plugins\src_VCPP\DisplayBinaryFiles: WinMergeScript.cpp
  Plugins\dlls: DisplayBinaryFiles.dll

2004-06-21 Laoran
 PATCH: [ 972299 ] Unpacker for comparing binary files
  Plugins: list of sources.txt
  Plugins\dlls add: DisplayBinaryFiles.dll
  Plugins\dlls: list.txt
  Plugins\src_VCPP\DisplayBinaryFiles add: all files

2004-06-13 Laoran
 plugins doc : new sections, how to write and debug plugins
  Plugins: readme_developpers.txt syntax.txt

2004-06-10 Laoran
 PATCH: [ 969667 ] Ignore positions/tabbed columns (plugins)
  Plugins: list of sources.txt
  Plugins\dlls add: IgnoreColumns.dll IgnoreFieldsComma.dll IgnoreFieldsTab.dll 
  Plugins\dlls: list.txt
  Plugins\src_VCPP\IgnoreColumns add: all files
  Plugins\src_VCPP\IgnoreFieldsComma add: all files
  Plugins\src_VCPP\IgnoreFieldsTab add: all files

2004-06-10 Laoran
 update plugins text doc :
  Plugins: readme_developpers.txt syntax.txt
  Plugins/dlls Add: readme_users.txt

2004-04-08 Laoran
 Creation of 'readme_developpers.txt' and of a 'readme.txt' to log patches
 Rename 'list of sources.txt' to 'list_of_sources.txt'
  Plugins: list_of_sources.txt readme.txt readme_developpers.txt

2004-04-03 Laoran
 PATCH: [ 923044 ] Partial compare large files
  Plugins: list of sources.txt
  Plugins\src_VCPP\WatchBeginningOfLog add: all files
  Plugins\src_VCPP\WatchEndOfLog add: all files

2004-04-02 Laoran
 Change plugins event title 'CONTEXT_MENU' -> 'EDITOR_SCRIPT'
  Src: FileTransform.cpp FileTransform.h MergeEditView.cpp Plugins.cpp Plugins.h
  Plugins/dlls: editor addin.sct insert datetime.sct list.txt
  Plugins: list of sources.txt readme.txt syntax.txt
  Plugins/src_VB/ToUpper: WinMergeScript.cls

2004-03-24 Laoran
 HideFirstLetter plugin : fix : open input and output file as binary 
  Plugins/HideFirstLetter: WinMergeScript.cpp

2004-01-18 Kimmo
 PATCH: [ 872413 ] UnpackDFM TestStreamFormat
  Submitted by Michael Richter
  Plugins/Delphi/UnpackFDM: UnpackFDM.dpr UnpackFDMLib_TLB.pas
   WinMergeUnit.pas
  Plugins/dlls: UnpackFDM.dll

2003-12-17 Laoran
 update plugin docs and examples, consequence of patch #853147 
  plugins: list of sources.txt readme.txt syntax.txt
  plugins\dlls: RCLocalizationHelper.dll list.txt
  plugins\src_VB\IgnoreTwoFirstCharsOrder: WinMergeScript.cls
  plugins\src_VCPP\RCLocalizationHelper: RCLocalizationHelper.idl WinMergeScript.cpp WinMergeScript.h

2003-11-10 Laoran
 PATCH: [ 834113 ] Plugin written in Delphi
 submitted by Bill Binder
  Plugins/Delphi/UnpackFDM add: readme.txt UnpackFDM.dof UnpackFDM.dpr UnpackFDM.tlb UnpackFDMLib_TLB.pas WinMergeUnit.pas
  Plugins/dlls add: UnpackFDM.dll

2003-11-04 Laoran
 adapt plugins filters to the syntax of regular expressions
  Plugins\dlls: RCLocalizationHelper.dll
  Plugins\src\HideFirstLetter: WinMergeScript.cpp
  Plugins\src\HideLastLetter: WinMergeScript.cls
  Plugins\src\IgnoreTwoFirstCharsOrder: WinMergeScript.cls
  Plugins\src\RCLocalizationHelper: WinMergeScript.cpp

