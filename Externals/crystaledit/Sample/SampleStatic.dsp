# Microsoft Developer Studio Project File - Name="SampleStatic" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** 編集しないでください **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=SampleStatic - Win32 Debug
!MESSAGE これは有効なﾒｲｸﾌｧｲﾙではありません。 このﾌﾟﾛｼﾞｪｸﾄをﾋﾞﾙﾄﾞするためには NMAKE を使用してください。
!MESSAGE [ﾒｲｸﾌｧｲﾙのｴｸｽﾎﾟｰﾄ] ｺﾏﾝﾄﾞを使用して実行してください
!MESSAGE 
!MESSAGE NMAKE /f "SampleStatic.mak".
!MESSAGE 
!MESSAGE NMAKE の実行時に構成を指定できます
!MESSAGE ｺﾏﾝﾄﾞ ﾗｲﾝ上でﾏｸﾛの設定を定義します。例:
!MESSAGE 
!MESSAGE NMAKE /f "SampleStatic.mak" CFG="SampleStatic - Win32 Debug"
!MESSAGE 
!MESSAGE 選択可能なﾋﾞﾙﾄﾞ ﾓｰﾄﾞ:
!MESSAGE 
!MESSAGE "SampleStatic - Win32 Release" ("Win32 (x86) Application" 用)
!MESSAGE "SampleStatic - Win32 Debug" ("Win32 (x86) Application" 用)
!MESSAGE "SampleStatic - Win32 Unicode Release" ("Win32 (x86) Application" 用)
!MESSAGE "SampleStatic - Win32 Unicode Debug" ("Win32 (x86) Application" 用)
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
# ADD CPP /nologo /MD /W3 /GX /O2 /I ".\Src" /I "." /I "..\editlib" /I "..\Common" /I ".." /I "..\..\Externals\pcre\Win32" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D EDITPADC_CLASS= /D "CRYSTALEDIT_ENABLELOADER" /D "CRYSTALEDIT_ENABLESAVER" /Yu"stdafx.h" /FD /c
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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\Src" /I "." /I "..\editlib" /I "..\Common" /I ".." /I "..\..\Externals\pcre\Win32" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D EDITPADC_CLASS= /D "CRYSTALEDIT_ENABLELOADER" /D "CRYSTALEDIT_ENABLESAVER" /FR /Yu"stdafx.h" /FD /c
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
# ADD CPP /nologo /MD /W3 /GX /O2 /I ".\Src" /I "." /I "..\editlib" /I "..\Common" /I ".." /I "..\..\Externals\pcre\Win32" /D "NDEBUG" /D "_UNICODE" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D EDITPADC_CLASS= /D "CRYSTALEDIT_ENABLELOADER" /D "CRYSTALEDIT_ENABLESAVER" /Yu"stdafx.h" /FD /c
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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "." /I "..\editlib" /I "..\Common" /I ".." /I "..\..\Externals\pcre\Win32" /D "_DEBUG" /D "_UNICODE" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D EDITPADC_CLASS= /D "CRYSTALEDIT_ENABLELOADER" /D "CRYSTALEDIT_ENABLESAVER" /Yu"stdafx.h" /FD /c
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
# Begin Group "parsers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\editlib\asp.cpp
# End Source File
# Begin Source File

SOURCE=..\editlib\basic.cpp
# End Source File
# Begin Source File

SOURCE=..\editlib\batch.cpp
# End Source File
# Begin Source File

SOURCE=..\editlib\cplusplus.cpp
# End Source File
# Begin Source File

SOURCE=..\editlib\csharp.cpp
# End Source File
# Begin Source File

SOURCE=..\editlib\css.cpp
# End Source File
# Begin Source File

SOURCE=..\editlib\dcl.cpp
# End Source File
# Begin Source File

SOURCE=..\editlib\fortran.cpp
# End Source File
# Begin Source File

SOURCE=..\editlib\html.cpp
# End Source File
# Begin Source File

SOURCE=..\editlib\ini.cpp
# End Source File
# Begin Source File

SOURCE=..\editlib\innosetup.cpp
# End Source File
# Begin Source File

SOURCE=..\editlib\is.cpp
# End Source File
# Begin Source File

SOURCE=..\editlib\java.cpp
# End Source File
# Begin Source File

SOURCE=..\editlib\lisp.cpp
# End Source File
# Begin Source File

SOURCE=..\editlib\nsis.cpp
# End Source File
# Begin Source File

SOURCE=..\editlib\pascal.cpp
# End Source File
# Begin Source File

SOURCE=..\editlib\perl.cpp
# End Source File
# Begin Source File

SOURCE=..\editlib\php.cpp
# End Source File
# Begin Source File

SOURCE=..\editlib\po.cpp
# End Source File
# Begin Source File

SOURCE=..\editlib\powershell.cpp
# End Source File
# Begin Source File

SOURCE=..\editlib\python.cpp
# End Source File
# Begin Source File

SOURCE=..\editlib\rexx.cpp
# End Source File
# Begin Source File

SOURCE=..\editlib\rsrc.cpp
# End Source File
# Begin Source File

SOURCE=..\editlib\ruby.cpp
# End Source File
# Begin Source File

SOURCE=..\editlib\sgml.cpp
# End Source File
# Begin Source File

SOURCE=..\editlib\sh.cpp
# End Source File
# Begin Source File

SOURCE=..\editlib\siod.cpp
# End Source File
# Begin Source File

SOURCE=..\editlib\sql.cpp
# End Source File
# Begin Source File

SOURCE=..\editlib\tcl.cpp
# End Source File
# Begin Source File

SOURCE=..\editlib\tex.cpp
# End Source File
# Begin Source File

SOURCE=..\editlib\verilog.cpp
# End Source File
# Begin Source File

SOURCE=..\editlib\xml.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=..\editlib\ccrystaleditview.cpp
# End Source File
# Begin Source File

SOURCE=..\editlib\ccrystaleditview.h
# End Source File
# Begin Source File

SOURCE=..\editlib\ccrystaleditview.inl
# End Source File
# Begin Source File

SOURCE=..\editlib\ccrystaltextbuffer.cpp
# End Source File
# Begin Source File

SOURCE=..\editlib\ccrystaltextbuffer.h
# End Source File
# Begin Source File

SOURCE=..\editlib\ccrystaltextbuffer.inl
# End Source File
# Begin Source File

SOURCE=..\editlib\ccrystaltextview.cpp
# End Source File
# Begin Source File

SOURCE=..\editlib\ccrystaltextview.h
# End Source File
# Begin Source File

SOURCE=..\editlib\ccrystaltextview.inl
# End Source File
# Begin Source File

SOURCE=..\editlib\ccrystaltextview2.cpp
# End Source File
# Begin Source File

SOURCE=..\editlib\ceditreplacedlg.cpp
# End Source File
# Begin Source File

SOURCE=..\editlib\ceditreplacedlg.h
# End Source File
# Begin Source File

SOURCE=..\editlib\cfindtextdlg.cpp
# End Source File
# Begin Source File

SOURCE=..\editlib\cfindtextdlg.h
# End Source File
# Begin Source File

SOURCE=..\editlib\chcondlg.cpp
# End Source File
# Begin Source File

SOURCE=..\editlib\chcondlg.h
# End Source File
# Begin Source File

SOURCE=..\editlib\cregexp.cpp
# End Source File
# Begin Source File

SOURCE=..\editlib\cregexp.h
# End Source File
# Begin Source File

SOURCE=..\editlib\crystaleditviewex.cpp
# End Source File
# Begin Source File

SOURCE=..\editlib\crystaleditviewex.h
# End Source File
# Begin Source File

SOURCE=..\editlib\crystalparser.cpp
# End Source File
# Begin Source File

SOURCE=..\editlib\crystalparser.h
# End Source File
# Begin Source File

SOURCE=..\editlib\crystaltextblock.cpp
# End Source File
# Begin Source File

SOURCE=..\editlib\crystaltextblock.h
# End Source File
# Begin Source File

SOURCE=..\editlib\cs2cs.cpp
# End Source File
# Begin Source File

SOURCE=..\editlib\cs2cs.h
# End Source File
# Begin Source File

SOURCE=..\editlib\editcmd.h
# End Source File
# Begin Source File

SOURCE=..\editlib\editreg.h
# End Source File
# Begin Source File

SOURCE=..\editlib\edtlib.cpp
# End Source File
# Begin Source File

SOURCE=..\editlib\edtlib.h
# End Source File
# Begin Source File

SOURCE=..\editlib\filesup.cpp
# End Source File
# Begin Source File

SOURCE=..\editlib\filesup.h
# End Source File
# Begin Source File

SOURCE=..\editlib\filesup.inl
# End Source File
# Begin Source File

SOURCE=..\editlib\fpattern.cpp
# End Source File
# Begin Source File

SOURCE=..\editlib\fpattern.h
# End Source File
# Begin Source File

SOURCE=..\editlib\gotodlg.cpp
# End Source File
# Begin Source File

SOURCE=..\editlib\gotodlg.h
# End Source File
# Begin Source File

SOURCE=..\editlib\LineInfo.cpp
# End Source File
# Begin Source File

SOURCE=..\editlib\LineInfo.h
# End Source File
# Begin Source File

SOURCE=..\editlib\memcombo.cpp
# End Source File
# Begin Source File

SOURCE=..\editlib\memcombo.h
# End Source File
# Begin Source File

SOURCE=..\editlib\memcombo.inl
# End Source File
# Begin Source File

SOURCE=..\editlib\registry.cpp
# End Source File
# Begin Source File

SOURCE=..\editlib\registry.h
# End Source File
# Begin Source File

SOURCE=..\editlib\splash.cpp
# End Source File
# Begin Source File

SOURCE=..\editlib\splash.h
# End Source File
# Begin Source File

SOURCE=..\editlib\statbar.cpp
# End Source File
# Begin Source File

SOURCE=..\editlib\statbar.h
# End Source File
# Begin Source File

SOURCE=..\editlib\string_util.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\editlib\string_util.h
# End Source File
# Begin Source File

SOURCE=..\editlib\SyntaxColors.cpp
# End Source File
# Begin Source File

SOURCE=..\editlib\SyntaxColors.h
# End Source File
# Begin Source File

SOURCE=..\editlib\UndoRecord.cpp
# End Source File
# Begin Source File

SOURCE=..\editlib\UndoRecord.h
# End Source File
# Begin Source File

SOURCE=..\editlib\ViewableWhitespace.cpp
# End Source File
# Begin Source File

SOURCE=..\editlib\ViewableWhitespace.h
# End Source File
# Begin Source File

SOURCE=..\editlib\wispelld.h
# End Source File
# End Group
# End Target
# End Project
