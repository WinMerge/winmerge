# Microsoft Developer Studio Project File - Name="SampleStatic" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=SampleStatic - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SampleStatic.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SampleStatic.mak" CFG="SampleStatic - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SampleStatic - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "SampleStatic - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "SampleStatic - Win32 Unicode Release" (based on "Win32 (x86) Application")
!MESSAGE "SampleStatic - Win32 Unicode Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SampleStatic - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I ".\Src" /I "." /I "..\Src\editlib" /I "Common" /I "..\Src\Common" /I "..\Src" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D EDITPADC_CLASS= /D "CRYSTALEDIT_ENABLELOADER" /D "CRYSTALEDIT_ENABLESAVER" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /i "..\crystaledit" /i ".." /i "." /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 imm32.lib shlwapi.lib /nologo /subsystem:windows /machine:I386 /libpath:"..\output"

!ELSEIF  "$(CFG)" == "SampleStatic - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\Src" /I "." /I "..\Src\editlib" /I "Common" /I "..\Src\Common" /I "..\Src" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D EDITPADC_CLASS= /D "CRYSTALEDIT_ENABLELOADER" /D "CRYSTALEDIT_ENABLESAVER" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i "..\crystaledit" /i ".." /i "." /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 imm32.lib shlwapi.lib /nologo /subsystem:windows /profile /debug /machine:I386 /libpath:"..\output"

!ELSEIF  "$(CFG)" == "SampleStatic - Win32 Unicode Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\output"
# PROP BASE Intermediate_Dir "URelease"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "URelease"
# PROP Intermediate_Dir "URelease"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I ".\Src" /I "." /I "..\Src\editlib" /I "Common" /I "..\Src\Common" /I "..\Src" /D "NDEBUG" /D "_UNICODE" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D EDITPADC_CLASS= /D "CRYSTALEDIT_ENABLELOADER" /D "CRYSTALEDIT_ENABLESAVER" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /i "..\crystaledit" /i ".." /i "." /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386 /out:"Release/crysedit_demo.exe"
# ADD LINK32 imm32.lib shlwapi.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386 /libpath:"..\output"

!ELSEIF  "$(CFG)" == "SampleStatic - Win32 Unicode Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\output"
# PROP BASE Intermediate_Dir "UDebug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "UDebug"
# PROP Intermediate_Dir "UDebug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "." /I "..\Src\editlib" /I "Common" /I "..\Src\Common" /I "..\Src" /D "_DEBUG" /D "_UNICODE" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D EDITPADC_CLASS= /D "CRYSTALEDIT_ENABLELOADER" /D "CRYSTALEDIT_ENABLESAVER" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i "..\crystaledit" /i ".." /i "." /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 imm32.lib shlwapi.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386 /pdbtype:sept /libpath:"..\output"

!ENDIF 

# Begin Target

# Name "SampleStatic - Win32 Release"
# Name "SampleStatic - Win32 Debug"
# Name "SampleStatic - Win32 Unicode Release"
# Name "SampleStatic - Win32 Unicode Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ChildFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\Sample.cpp
# End Source File
# Begin Source File

SOURCE=.\Sample.rc
# End Source File
# Begin Source File

SOURCE=.\SampleDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\SampleView.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ChildFrm.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\Sample.h
# End Source File
# Begin Source File

SOURCE=.\SampleDoc.h
# End Source File
# Begin Source File

SOURCE=.\SampleView.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\mg_icons.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Sample.ico
# End Source File
# Begin Source File

SOURCE=.\res\Sample.rc2
# End Source File
# Begin Source File

SOURCE=.\res\SampleDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# End Group
# Begin Group "editlib"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Src\editlib\asp.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\basic.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\batch.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\ccrystaleditview.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\ccrystaleditview.h
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\ccrystaleditview.inl
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\ccrystaltextbuffer.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\ccrystaltextbuffer.h
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\ccrystaltextbuffer.inl
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\ccrystaltextview.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\ccrystaltextview.h
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\ccrystaltextview.inl
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\ccrystaltextview2.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\ceditreplacedlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\ceditreplacedlg.h
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\cfindtextdlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\cfindtextdlg.h
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\chcondlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\chcondlg.h
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\cplusplus.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\cregexp.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\cregexp.h
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\crystaleditviewex.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\crystaleditviewex.h
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\crystalparser.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\crystalparser.h
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\crystaltextblock.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\crystaltextblock.h
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\cs2cs.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\cs2cs.h
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\csharp.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\dcl.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\editcmd.h
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\editreg.h
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\edtlib.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\edtlib.h
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\filesup.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\filesup.h
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\filesup.inl
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\fortran.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\fpattern.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\fpattern.h
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\gotodlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\gotodlg.h
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\html.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\ini.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\innosetup.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\is.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\java.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\lisp.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\memcombo.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\memcombo.h
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\memcombo.inl
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\nsis.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\pascal.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\perl.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\php.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\python.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\registry.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\registry.h
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\rexx.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\rsrc.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\ruby.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\sgml.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\sh.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\siod.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\splash.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\splash.h
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\sql.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\statbar.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\statbar.h
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\tcl.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\tex.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\wispelld.h
# End Source File
# Begin Source File

SOURCE=..\Src\editlib\xml.cpp
# End Source File
# End Group
# Begin Group "common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\isx.cpp
# End Source File
# Begin Source File

SOURCE=.\isx.h
# End Source File
# Begin Source File

SOURCE=..\Src\Common\OptionsMgr.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\Common\OptionsMgr.h
# End Source File
# Begin Source File

SOURCE=..\Src\Common\SuperComboBox.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\Common\SuperComboBox.h
# End Source File
# Begin Source File

SOURCE=..\Src\SyntaxColors.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\SyntaxColors.h
# End Source File
# Begin Source File

SOURCE=..\Src\Common\varprop.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\Common\varprop.h
# End Source File
# Begin Source File

SOURCE=..\Src\ViewableWhitespace.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\ViewableWhitespace.h
# End Source File
# End Group
# End Target
# End Project
