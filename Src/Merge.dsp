# Microsoft Developer Studio Project File - Name="Merge" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Merge - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Merge.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Merge.mak" CFG="Merge - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Merge - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Merge - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Merge - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Build\MergeRelease"
# PROP Intermediate_Dir "..\Build\MergeRelease"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W4 /GR /GX /Zi /Od /I "." /I "..\common" /I ".\editlib" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "HAVE_STDLIB_H" /D "STDC_HEADERS" /D HAVE_STRING_H=1 /D PR_FILE_NAME=\"pr\" /D DIFF_PROGRAM=\"diff\" /D "REGEX_MALLOC" /D "__MSC__" /D "__NT__" /D USG=1 /D EDITPADC_CLASS= /D "COMPILE_MULTIMON_STUBS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o /win32 "NUL"
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o /win32 "NUL"
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 version.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"msvcrtd.lib" /out:"..\Build\MergeRelease/WinMerge.exe" /verbose:lib
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
TargetPath=\dev\sf\wm\cvsver\Build\MergeRelease\WinMerge.exe
SOURCE="$(InputPath)"
PostBuild_Cmds=StampVer -vstampver.inf -i4 -j4 -o2 $(TargetPath)
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Build\MergeDebug"
# PROP Intermediate_Dir "..\Build\MergeDebug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "." /I "..\common" /I ".\editlib" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "HAVE_STDLIB_H" /D "STDC_HEADERS" /D HAVE_STRING_H=1 /D PR_FILE_NAME=\"pr\" /D DIFF_PROGRAM=\"diff\" /D "REGEX_MALLOC" /D "__MSC__" /D "__NT__" /D USG=1 /D EDITPADC_CLASS= /D "COMPILE_MULTIMON_STUBS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o /win32 "NUL"
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o /win32 "NUL"
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 version.lib /nologo /subsystem:windows /debug /machine:I386 /out:"..\Build\MergeDebug/WinMerge.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Merge - Win32 Release"
# Name "Merge - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ANALYZE.C

!IF  "$(CFG)" == "Merge - Win32 Release"

# ADD CPP /O2
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\BCMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\CCPrompt.cpp
# End Source File
# Begin Source File

SOURCE=.\ChildFrm.cpp

!IF  "$(CFG)" == "Merge - Win32 Release"

# SUBTRACT CPP /D "HAVE_STDLIB_H" /D "STDC_HEADERS" /D HAVE_STRING_H=1 /D PR_FILE_NAME=\"pr\" /D DIFF_PROGRAM=\"diff\" /D "REGEX_MALLOC" /D "__MSC__" /D "__NT__" /D USG=1

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CMPBUF.C

!IF  "$(CFG)" == "Merge - Win32 Release"

# ADD CPP /O2
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ColorButton.cpp
# End Source File
# Begin Source File

SOURCE=.\CONTEXT.C

!IF  "$(CFG)" == "Merge - Win32 Release"

# ADD CPP /O2
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\coretools.cpp
# End Source File
# Begin Source File

SOURCE=.\Diff.cpp

!IF  "$(CFG)" == "Merge - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DiffContext.cpp
# End Source File
# Begin Source File

SOURCE=.\DiffThread.cpp
# End Source File
# Begin Source File

SOURCE=.\DirActions.cpp
# End Source File
# Begin Source File

SOURCE=.\DirDoc.cpp

!IF  "$(CFG)" == "Merge - Win32 Release"

# SUBTRACT CPP /D "HAVE_STDLIB_H" /D "STDC_HEADERS" /D HAVE_STRING_H=1 /D PR_FILE_NAME=\"pr\" /D DIFF_PROGRAM=\"diff\" /D "REGEX_MALLOC" /D "__MSC__" /D "__NT__" /D USG=1

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DIRENT.C

!IF  "$(CFG)" == "Merge - Win32 Release"

# ADD CPP /O2
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DirFrame.cpp

!IF  "$(CFG)" == "Merge - Win32 Release"

# SUBTRACT CPP /D "HAVE_STDLIB_H" /D "STDC_HEADERS" /D HAVE_STRING_H=1 /D PR_FILE_NAME=\"pr\" /D DIFF_PROGRAM=\"diff\" /D "REGEX_MALLOC" /D "__MSC__" /D "__NT__" /D USG=1

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DirScan.cpp
# End Source File
# Begin Source File

SOURCE=.\DirView.cpp

!IF  "$(CFG)" == "Merge - Win32 Release"

# SUBTRACT CPP /D "HAVE_STDLIB_H" /D "STDC_HEADERS" /D HAVE_STRING_H=1 /D PR_FILE_NAME=\"pr\" /D DIFF_PROGRAM=\"diff\" /D "REGEX_MALLOC" /D "__MSC__" /D "__NT__" /D USG=1

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dllver.cpp
# End Source File
# Begin Source File

SOURCE=.\ED.C

!IF  "$(CFG)" == "Merge - Win32 Release"

# ADD CPP /O2
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\EditorFilepathBar.cpp
# End Source File
# Begin Source File

SOURCE=.\FileFilterMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\FilepathEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\files.cpp
# End Source File
# Begin Source File

SOURCE=.\FNMATCH.C

!IF  "$(CFG)" == "Merge - Win32 Release"

# ADD CPP /O2
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GnuVersion.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\IFDEF.C

!IF  "$(CFG)" == "Merge - Win32 Release"

# ADD CPP /O2
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\IO.C

!IF  "$(CFG)" == "Merge - Win32 Release"

# ADD CPP /O2
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\LanguageSelect.cpp
# End Source File
# Begin Source File

SOURCE=..\common\listvwex.cpp
# End Source File
# Begin Source File

SOURCE=..\common\LogFile.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp

!IF  "$(CFG)" == "Merge - Win32 Release"

# SUBTRACT CPP /D "HAVE_STDLIB_H" /D "STDC_HEADERS" /D HAVE_STRING_H=1 /D PR_FILE_NAME=\"pr\" /D DIFF_PROGRAM=\"diff\" /D "REGEX_MALLOC" /D "__MSC__" /D "__NT__" /D USG=1

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Merge.cpp

!IF  "$(CFG)" == "Merge - Win32 Release"

# SUBTRACT CPP /D "HAVE_STDLIB_H" /D "STDC_HEADERS" /D HAVE_STRING_H=1 /D PR_FILE_NAME=\"pr\" /D DIFF_PROGRAM=\"diff\" /D "REGEX_MALLOC" /D "__MSC__" /D "__NT__" /D USG=1

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Merge.rc
# End Source File
# Begin Source File

SOURCE=.\MergeDoc.cpp

!IF  "$(CFG)" == "Merge - Win32 Release"

# SUBTRACT CPP /D "HAVE_STDLIB_H" /D "STDC_HEADERS" /D HAVE_STRING_H=1 /D PR_FILE_NAME=\"pr\" /D DIFF_PROGRAM=\"diff\" /D "REGEX_MALLOC" /D "__MSC__" /D "__NT__" /D USG=1

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MergeEditView.cpp
# End Source File
# Begin Source File

SOURCE=.\NORMAL.C

!IF  "$(CFG)" == "Merge - Win32 Release"

# ADD CPP /O2
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\OpenDlg.cpp

!IF  "$(CFG)" == "Merge - Win32 Release"

# SUBTRACT CPP /D "HAVE_STDLIB_H" /D "STDC_HEADERS" /D HAVE_STRING_H=1 /D PR_FILE_NAME=\"pr\" /D DIFF_PROGRAM=\"diff\" /D "REGEX_MALLOC" /D "__MSC__" /D "__NT__" /D USG=1

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\OutputDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\paths.cpp
# End Source File
# Begin Source File

SOURCE=.\PropColors.cpp
# End Source File
# Begin Source File

SOURCE=.\PropFilter.cpp
# End Source File
# Begin Source File

SOURCE=.\PropGeneral.cpp

!IF  "$(CFG)" == "Merge - Win32 Release"

# SUBTRACT CPP /D "HAVE_STDLIB_H" /D "STDC_HEADERS" /D HAVE_STRING_H=1 /D PR_FILE_NAME=\"pr\" /D DIFF_PROGRAM=\"diff\" /D "REGEX_MALLOC" /D "__MSC__" /D "__NT__" /D USG=1

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PropRegistry.cpp
# End Source File
# Begin Source File

SOURCE=.\PropSyntax.cpp
# End Source File
# Begin Source File

SOURCE=.\PropVss.cpp

!IF  "$(CFG)" == "Merge - Win32 Release"

# SUBTRACT CPP /D "HAVE_STDLIB_H" /D "STDC_HEADERS" /D HAVE_STRING_H=1 /D PR_FILE_NAME=\"pr\" /D DIFF_PROGRAM=\"diff\" /D "REGEX_MALLOC" /D "__MSC__" /D "__NT__" /D USG=1

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\REGEX.C

!IF  "$(CFG)" == "Merge - Win32 Release"

# ADD CPP /O2
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\RegExp.cpp
# End Source File
# Begin Source File

SOURCE=..\common\RegKey.cpp
# End Source File
# Begin Source File

SOURCE=.\SIDE.C

!IF  "$(CFG)" == "Merge - Win32 Release"

# ADD CPP /O2
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\SortHeaderCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\Splash.cpp

!IF  "$(CFG)" == "Merge - Win32 Release"

# SUBTRACT CPP /D "HAVE_STDLIB_H" /D "STDC_HEADERS" /D HAVE_STRING_H=1 /D PR_FILE_NAME=\"pr\" /D DIFF_PROGRAM=\"diff\" /D "REGEX_MALLOC" /D "__MSC__" /D "__NT__" /D USG=1

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\SplitterWndEx.cpp
# End Source File
# Begin Source File

SOURCE=.\ssapi.cpp
# End Source File
# Begin Source File

SOURCE=..\common\StatLink.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "Merge - Win32 Release"

# ADD CPP /Yc"stdafx.h"
# SUBTRACT CPP /D "HAVE_STDLIB_H" /D "STDC_HEADERS" /D HAVE_STRING_H=1 /D PR_FILE_NAME=\"pr\" /D DIFF_PROGRAM=\"diff\" /D "REGEX_MALLOC" /D "__MSC__" /D "__NT__" /D USG=1

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\StringEx.cpp
# End Source File
# Begin Source File

SOURCE=..\common\SuperComboBox.cpp
# End Source File
# Begin Source File

SOURCE=..\common\textfile.cpp
# End Source File
# Begin Source File

SOURCE=.\UTIL.C

!IF  "$(CFG)" == "Merge - Win32 Release"

# ADD CPP /O2
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\version.cpp
# End Source File
# Begin Source File

SOURCE=.\VssPrompt.cpp

!IF  "$(CFG)" == "Merge - Win32 Release"

# SUBTRACT CPP /D "HAVE_STDLIB_H" /D "STDC_HEADERS" /D HAVE_STRING_H=1 /D PR_FILE_NAME=\"pr\" /D DIFF_PROGRAM=\"diff\" /D "REGEX_MALLOC" /D "__MSC__" /D "__NT__" /D USG=1

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WaitStatusCursor.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\BCMenu.h
# End Source File
# Begin Source File

SOURCE=.\CCPrompt.h
# End Source File
# Begin Source File

SOURCE=.\ChildFrm.h
# End Source File
# Begin Source File

SOURCE=.\CMPBUF.H
# End Source File
# Begin Source File

SOURCE=.\ColorButton.h
# End Source File
# Begin Source File

SOURCE=.\CONFIG.H
# End Source File
# Begin Source File

SOURCE=..\common\coretools.h
# End Source File
# Begin Source File

SOURCE=".\DIFF-decl.H"
# End Source File
# Begin Source File

SOURCE=.\DIFF.H
# End Source File
# Begin Source File

SOURCE=.\DiffContext.h
# End Source File
# Begin Source File

SOURCE=.\DiffThread.h
# End Source File
# Begin Source File

SOURCE=.\DirDoc.h
# End Source File
# Begin Source File

SOURCE=.\DIRENT.H
# End Source File
# Begin Source File

SOURCE=.\DirFrame.h
# End Source File
# Begin Source File

SOURCE=.\DirScan.h
# End Source File
# Begin Source File

SOURCE=.\DirView.h
# End Source File
# Begin Source File

SOURCE=.\dllver.h
# End Source File
# Begin Source File

SOURCE=.\EditFile.h
# End Source File
# Begin Source File

SOURCE=.\EditorFilepathBar.h
# End Source File
# Begin Source File

SOURCE=.\FileFilterMgr.h
# End Source File
# Begin Source File

SOURCE=.\FilepathEdit.h
# End Source File
# Begin Source File

SOURCE=.\files.h
# End Source File
# Begin Source File

SOURCE=.\FNMATCH.H
# End Source File
# Begin Source File

SOURCE=..\common\LanguageSelect.h
# End Source File
# Begin Source File

SOURCE=..\common\listvwex.h
# End Source File
# Begin Source File

SOURCE=..\common\LogFile.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\Merge.h
# End Source File
# Begin Source File

SOURCE=.\MergeDoc.h
# End Source File
# Begin Source File

SOURCE=.\MergeEditStatus.h
# End Source File
# Begin Source File

SOURCE=.\MergeEditView.h
# End Source File
# Begin Source File

SOURCE=.\multimon.h
# End Source File
# Begin Source File

SOURCE=.\OpenDlg.h
# End Source File
# Begin Source File

SOURCE=.\OutputDlg.h
# End Source File
# Begin Source File

SOURCE=.\paths.h
# End Source File
# Begin Source File

SOURCE=.\PropColors.h
# End Source File
# Begin Source File

SOURCE=.\PropFilter.h
# End Source File
# Begin Source File

SOURCE=.\PropGeneral.h
# End Source File
# Begin Source File

SOURCE=.\PropRegistry.h
# End Source File
# Begin Source File

SOURCE=.\PropSyntax.h
# End Source File
# Begin Source File

SOURCE=.\PropVss.h
# End Source File
# Begin Source File

SOURCE=.\REGEX.H
# End Source File
# Begin Source File

SOURCE=..\common\RegExp.h
# End Source File
# Begin Source File

SOURCE=..\common\RegKey.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=..\common\SortHeaderCtrl.h
# End Source File
# Begin Source File

SOURCE=.\Splash.h
# End Source File
# Begin Source File

SOURCE=..\common\SplitterWndEx.h
# End Source File
# Begin Source File

SOURCE=.\ssapi.h
# End Source File
# Begin Source File

SOURCE=..\common\StatLink.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=..\common\StringEx.h
# End Source File
# Begin Source File

SOURCE=..\common\SuperComboBox.h
# End Source File
# Begin Source File

SOURCE=.\SYSTEM.H
# End Source File
# Begin Source File

SOURCE=..\Common\textfile.h
# End Source File
# Begin Source File

SOURCE=..\common\version.h
# End Source File
# Begin Source File

SOURCE=.\VssPrompt.h
# End Source File
# Begin Source File

SOURCE=.\WaitStatusCursor.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\binary.bmp
# End Source File
# Begin Source File

SOURCE=.\res\binarydiff.bmp
# End Source File
# Begin Source File

SOURCE=.\res\equal.bmp
# End Source File
# Begin Source File

SOURCE=.\res\folder1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\lfile.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Merge.ico
# End Source File
# Begin Source File

SOURCE=.\res\Merge.rc2
# End Source File
# Begin Source File

SOURCE=.\res\MergeDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\mg_cur.cur
# End Source File
# Begin Source File

SOURCE=.\res\notequal.bmp
# End Source File
# Begin Source File

SOURCE=.\res\rfile.bmp
# End Source File
# Begin Source File

SOURCE=.\res\rfolder.bmp
# End Source File
# Begin Source File

SOURCE=.\res\splash1.bmp
# End Source File
# Begin Source File

SOURCE=.\Splsh16.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\unknown.bmp
# End Source File
# Begin Source File

SOURCE=.\res\winmerge.bmp
# End Source File
# End Group
# Begin Group "EditLib"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\editlib\asp.cpp

!IF  "$(CFG)" == "Merge - Win32 Release"

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# PROP Intermediate_Dir "..\Build\MergeDebug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\editlib\basic.cpp

!IF  "$(CFG)" == "Merge - Win32 Release"

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# PROP Intermediate_Dir "..\Build\MergeDebug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\editlib\batch.cpp

!IF  "$(CFG)" == "Merge - Win32 Release"

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# PROP Intermediate_Dir "..\Build\MergeDebug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\editlib\ccrystaleditview.cpp

!IF  "$(CFG)" == "Merge - Win32 Release"

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# PROP Intermediate_Dir "..\Build\MergeDebug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\editlib\ccrystaleditview.h

!IF  "$(CFG)" == "Merge - Win32 Release"

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# PROP Intermediate_Dir "..\Build\MergeDebug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\editlib\ccrystaleditview.inl

!IF  "$(CFG)" == "Merge - Win32 Release"

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# PROP Intermediate_Dir "..\Build\MergeDebug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\editlib\ccrystaltextbuffer.cpp

!IF  "$(CFG)" == "Merge - Win32 Release"

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# PROP Intermediate_Dir "..\Build\MergeDebug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\editlib\ccrystaltextbuffer.h

!IF  "$(CFG)" == "Merge - Win32 Release"

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# PROP Intermediate_Dir "..\Build\MergeDebug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\editlib\ccrystaltextbuffer.inl

!IF  "$(CFG)" == "Merge - Win32 Release"

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# PROP Intermediate_Dir "..\Build\MergeDebug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\editlib\ccrystaltextview.cpp

!IF  "$(CFG)" == "Merge - Win32 Release"

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# PROP Intermediate_Dir "..\Build\MergeDebug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\editlib\ccrystaltextview.h

!IF  "$(CFG)" == "Merge - Win32 Release"

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# PROP Intermediate_Dir "..\Build\MergeDebug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\editlib\ccrystaltextview.inl

!IF  "$(CFG)" == "Merge - Win32 Release"

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# PROP Intermediate_Dir "..\Build\MergeDebug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\editlib\ccrystaltextview2.cpp

!IF  "$(CFG)" == "Merge - Win32 Release"

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# PROP Intermediate_Dir "..\Build\MergeDebug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\editlib\ceditreplacedlg.cpp

!IF  "$(CFG)" == "Merge - Win32 Release"

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# PROP Intermediate_Dir "..\Build\MergeDebug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\editlib\ceditreplacedlg.h

!IF  "$(CFG)" == "Merge - Win32 Release"

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# PROP Intermediate_Dir "..\Build\MergeDebug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\editlib\cfindtextdlg.cpp

!IF  "$(CFG)" == "Merge - Win32 Release"

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# PROP Intermediate_Dir "..\Build\MergeDebug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\editlib\cfindtextdlg.h

!IF  "$(CFG)" == "Merge - Win32 Release"

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# PROP Intermediate_Dir "..\Build\MergeDebug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\editlib\chcondlg.cpp

!IF  "$(CFG)" == "Merge - Win32 Release"

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# PROP Intermediate_Dir "..\Build\MergeDebug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\editlib\chcondlg.h

!IF  "$(CFG)" == "Merge - Win32 Release"

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# PROP Intermediate_Dir "..\Build\MergeDebug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\editlib\cplusplus.cpp

!IF  "$(CFG)" == "Merge - Win32 Release"

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# PROP Intermediate_Dir "..\Build\MergeDebug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\editlib\cregexp.cpp
# End Source File
# Begin Source File

SOURCE=.\editlib\cregexp.h
# End Source File
# Begin Source File

SOURCE=.\editlib\crystaleditviewex.cpp

!IF  "$(CFG)" == "Merge - Win32 Release"

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# PROP Intermediate_Dir "..\Build\MergeDebug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\editlib\crystaleditviewex.h

!IF  "$(CFG)" == "Merge - Win32 Release"

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# PROP Intermediate_Dir "..\Build\MergeDebug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\editlib\crystalparser.cpp

!IF  "$(CFG)" == "Merge - Win32 Release"

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# PROP Intermediate_Dir "..\Build\MergeDebug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\editlib\crystalparser.h

!IF  "$(CFG)" == "Merge - Win32 Release"

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# PROP Intermediate_Dir "..\Build\MergeDebug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\editlib\crystaltextblock.cpp

!IF  "$(CFG)" == "Merge - Win32 Release"

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# PROP Intermediate_Dir "..\Build\MergeDebug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\editlib\crystaltextblock.h

!IF  "$(CFG)" == "Merge - Win32 Release"

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# PROP Intermediate_Dir "..\Build\MergeDebug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\editlib\cs2cs.cpp

!IF  "$(CFG)" == "Merge - Win32 Release"

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# PROP Intermediate_Dir "..\Build\MergeDebug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\editlib\cs2cs.h

!IF  "$(CFG)" == "Merge - Win32 Release"

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# PROP Intermediate_Dir "..\Build\MergeDebug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\editlib\dcl.cpp

!IF  "$(CFG)" == "Merge - Win32 Release"

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# PROP Intermediate_Dir "..\Build\MergeDebug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\editlib\editcmd.h

!IF  "$(CFG)" == "Merge - Win32 Release"

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# PROP Intermediate_Dir "..\Build\MergeDebug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\editlib\editreg.h

!IF  "$(CFG)" == "Merge - Win32 Release"

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# PROP Intermediate_Dir "..\Build\MergeDebug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\editlib\edtlib.h

!IF  "$(CFG)" == "Merge - Win32 Release"

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# PROP Intermediate_Dir "..\Build\MergeDebug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\editlib\filesup.cpp

!IF  "$(CFG)" == "Merge - Win32 Release"

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# PROP Intermediate_Dir "..\Build\MergeDebug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\editlib\filesup.h

!IF  "$(CFG)" == "Merge - Win32 Release"

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# PROP Intermediate_Dir "..\Build\MergeDebug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\editlib\filesup.inl

!IF  "$(CFG)" == "Merge - Win32 Release"

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# PROP Intermediate_Dir "..\Build\MergeDebug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\editlib\fortran.cpp

!IF  "$(CFG)" == "Merge - Win32 Release"

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# PROP Intermediate_Dir "..\Build\MergeDebug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\editlib\fpattern.cpp

!IF  "$(CFG)" == "Merge - Win32 Release"

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# PROP Intermediate_Dir "..\Build\MergeDebug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\editlib\fpattern.h

!IF  "$(CFG)" == "Merge - Win32 Release"

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# PROP Intermediate_Dir "..\Build\MergeDebug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\editlib\gotodlg.cpp

!IF  "$(CFG)" == "Merge - Win32 Release"

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# PROP Intermediate_Dir "..\Build\MergeDebug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\editlib\gotodlg.h

!IF  "$(CFG)" == "Merge - Win32 Release"

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# PROP Intermediate_Dir "..\Build\MergeDebug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\editlib\html.cpp

!IF  "$(CFG)" == "Merge - Win32 Release"

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# PROP Intermediate_Dir "..\Build\MergeDebug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\editlib\is.cpp

!IF  "$(CFG)" == "Merge - Win32 Release"

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# PROP Intermediate_Dir "..\Build\MergeDebug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\editlib\java.cpp

!IF  "$(CFG)" == "Merge - Win32 Release"

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# PROP Intermediate_Dir "..\Build\MergeDebug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\editlib\lisp.cpp

!IF  "$(CFG)" == "Merge - Win32 Release"

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# PROP Intermediate_Dir "..\Build\MergeDebug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\editlib\memcombo.cpp

!IF  "$(CFG)" == "Merge - Win32 Release"

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# PROP Intermediate_Dir "..\Build\MergeDebug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\editlib\memcombo.h

!IF  "$(CFG)" == "Merge - Win32 Release"

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# PROP Intermediate_Dir "..\Build\MergeDebug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\editlib\memcombo.inl

!IF  "$(CFG)" == "Merge - Win32 Release"

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# PROP Intermediate_Dir "..\Build\MergeDebug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\editlib\pascal.cpp

!IF  "$(CFG)" == "Merge - Win32 Release"

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# PROP Intermediate_Dir "..\Build\MergeDebug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\editlib\perl.cpp

!IF  "$(CFG)" == "Merge - Win32 Release"

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# PROP Intermediate_Dir "..\Build\MergeDebug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\editlib\php.cpp

!IF  "$(CFG)" == "Merge - Win32 Release"

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# PROP Intermediate_Dir "..\Build\MergeDebug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\editlib\python.cpp

!IF  "$(CFG)" == "Merge - Win32 Release"

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# PROP Intermediate_Dir "..\Build\MergeDebug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\editlib\registry.cpp

!IF  "$(CFG)" == "Merge - Win32 Release"

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# PROP Intermediate_Dir "..\Build\MergeDebug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\editlib\registry.h

!IF  "$(CFG)" == "Merge - Win32 Release"

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# PROP Intermediate_Dir "..\Build\MergeDebug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\editlib\resource.h

!IF  "$(CFG)" == "Merge - Win32 Release"

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# PROP Intermediate_Dir "..\Build\MergeDebug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\editlib\rexx.cpp

!IF  "$(CFG)" == "Merge - Win32 Release"

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# PROP Intermediate_Dir "..\Build\MergeDebug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\editlib\rsrc.cpp
# End Source File
# Begin Source File

SOURCE=.\editlib\sgml.cpp

!IF  "$(CFG)" == "Merge - Win32 Release"

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# PROP Intermediate_Dir "..\Build\MergeDebug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\editlib\sh.cpp

!IF  "$(CFG)" == "Merge - Win32 Release"

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# PROP Intermediate_Dir "..\Build\MergeDebug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\editlib\siod.cpp

!IF  "$(CFG)" == "Merge - Win32 Release"

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# PROP Intermediate_Dir "..\Build\MergeDebug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\editlib\sql.cpp

!IF  "$(CFG)" == "Merge - Win32 Release"

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# PROP Intermediate_Dir "..\Build\MergeDebug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\editlib\statbar.cpp

!IF  "$(CFG)" == "Merge - Win32 Release"

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# PROP Intermediate_Dir "..\Build\MergeDebug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\editlib\statbar.h

!IF  "$(CFG)" == "Merge - Win32 Release"

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# PROP Intermediate_Dir "..\Build\MergeDebug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\editlib\tcl.cpp

!IF  "$(CFG)" == "Merge - Win32 Release"

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# PROP Intermediate_Dir "..\Build\MergeDebug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\editlib\tex.cpp

!IF  "$(CFG)" == "Merge - Win32 Release"

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# PROP Intermediate_Dir "..\Build\MergeDebug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\editlib\wispelld.h

!IF  "$(CFG)" == "Merge - Win32 Release"

!ELSEIF  "$(CFG)" == "Merge - Win32 Debug"

# PROP Intermediate_Dir "..\Build\MergeDebug"

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# Begin Source File

SOURCE=.\stampver.inf
# End Source File
# End Target
# End Project
# Section Merge : {6F747475-446E-6C62-436C-6B0000003100}
# 	1:10:IDB_SPLASH:103
# 	2:21:SplashScreenInsertKey:4.0
# End Section
# Section Merge : {00312E6C-0754-0055-90BD-550078075500}
# 	1:19:IDR_POPUP_ABOUT_DLG:104
# End Section
