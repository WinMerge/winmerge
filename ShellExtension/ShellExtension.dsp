# Microsoft Developer Studio Project File - Name="ShellExtension" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=ShellExtension - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ShellExtension.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ShellExtension.mak" CFG="ShellExtension - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ShellExtension - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ShellExtension - Win32 Unicode Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ShellExtension - Win32 Release MinSize" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ShellExtension - Win32 Release MinDependency" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ShellExtension - Win32 Unicode Release MinSize" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ShellExtension - Win32 Unicode Release MinDependency" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ShellExtension - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\BuildTmp\ShellExtension__Debug"
# PROP Intermediate_Dir "..\BuildTmp\ShellExtension__Debug"

# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../src/common" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /D "_ATL_NO_UUIDOF" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /out:"..\Build\MergeDebug/ShellExtension.dll" /pdbtype:sept
# Begin Custom Build - Performing registration
OutDir=.\..\BuildTmp\ShellExtension__Debug
TargetPath=.\..\BuildTmp\ShellExtension__Debug
InputPath=.\..\BuildTmp\ShellExtension__Debug
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "ShellExtension - Win32 Unicode Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DebugU"
# PROP BASE Intermediate_Dir "DebugU"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\BuildTmp\ShellExtension__UnicodeDebug"
# PROP Intermediate_Dir "..\BuildTmp\ShellExtension__UnicodeDebug"

# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../src/common" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_UNICODE" /D "_ATL_NO_UUIDOF" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /out:"..\Build\MergeUnicodeDebug\ShellExtensionU.dll" /pdbtype:sept
# Begin Custom Build - Performing registration
OutDir=.\..\BuildTmp\ShellExtension__UnicodeDebug
TargetPath=.\..\BuildTmp\ShellExtension__UnicodeDebug
InputPath=.\..\BuildTmp\ShellExtension__UnicodeDebug
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if "%OS%"=="" goto NOTNT 
	if not "%OS%"=="Windows_NT" goto NOTNT 
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	goto end 
	:NOTNT 
	echo Warning : Cannot register Unicode DLL on Windows 95 
	:end 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "ShellExtension - Win32 Release MinSize"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseMinSize"
# PROP BASE Intermediate_Dir "ReleaseMinSize"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\BuildTmp\ShellExtension__ReleaseMinSize"
# PROP Intermediate_Dir "..\BuildTmp\ShellExtension__ReleaseMinSize"

# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O1 /I "../src/common" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /D "_ATL_DLL" /D "_ATL_NO_UUIDOF" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386 /out:"..\Build\MergeReleaseMinsize\ShellExtension.dll"
# Begin Custom Build - Performing registration
OutDir=.\..\BuildTmp\ShellExtension__ReleaseMinSize
TargetPath=.\..\BuildTmp\ShellExtension__ReleaseMinSize
InputPath=.\..\BuildTmp\ShellExtension__ReleaseMinSize
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "ShellExtension - Win32 Release MinDependency"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseMinDependency"
# PROP BASE Intermediate_Dir "ReleaseMinDependency"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\BuildTmp\ShellExtension__ReleaseMinDependency"
# PROP Intermediate_Dir "..\BuildTmp\ShellExtension__ReleaseMinDependency"

# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O1 /I "../src/common" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /D "_ATL_NO_UUIDOF" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386 /out:"..\Build\MergeReleaseMinDependency\ShellExtension.dll"
# Begin Custom Build - Performing registration
OutDir=.\..\BuildTmp\ShellExtension__ReleaseMinDependency
TargetPath=.\..\BuildTmp\ShellExtension__ReleaseMinDependency
InputPath=.\..\BuildTmp\ShellExtension__ReleaseMinDependency
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "ShellExtension - Win32 Unicode Release MinSize"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseUMinSize"
# PROP BASE Intermediate_Dir "ReleaseUMinSize"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\BuildTmp\ShellExtension__UnicodeReleaseMinsize"
# PROP Intermediate_Dir "..\BuildTmp\ShellExtension__UnicodeReleaseMinsize"

# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_UNICODE" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O1 /I "../src/common" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_UNICODE" /D "_ATL_DLL" /D "_ATL_NO_UUIDOF" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386 /out:"..\Build\MergeUnicodeReleaseMinsize\ShellExtensionU.dll"
# Begin Custom Build - Performing registration
OutDir=.\..\BuildTmp\ShellExtension__UnicodeReleaseMinsize
TargetPath=.\..\BuildTmp\ShellExtension__UnicodeReleaseMinsize
InputPath=.\..\BuildTmp\ShellExtension__UnicodeReleaseMinsize
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if "%OS%"=="" goto NOTNT 
	if not "%OS%"=="Windows_NT" goto NOTNT 
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	goto end 
	:NOTNT 
	echo Warning : Cannot register Unicode DLL on Windows 95 
	:end 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "ShellExtension - Win32 Unicode Release MinDependency"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseUMinDependency"
# PROP BASE Intermediate_Dir "ReleaseUMinDependency"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\BuildTmp\ShellExtension__UnicodeReleaseMinDependency"
# PROP Intermediate_Dir "..\BuildTmp\ShellExtension__UnicodeReleaseMinDependency"

# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_UNICODE" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O1 /I "../src/common" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_USRDLL" /D "_UNICODE" /D "_ATL_STATIC_REGISTRY" /D "_ATL_NO_UUIDOF" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386 /out:"..\Build\MergeUnicodeReleaseMinDependency\ShellExtensionU.dll"
# Begin Custom Build - Performing registration
OutDir=.\..\BuildTmp\ShellExtension__UnicodeReleaseMinDependency
TargetPath=.\..\BuildTmp\ShellExtension__UnicodeReleaseMinDependency
InputPath=.\..\BuildTmp\ShellExtension__UnicodeReleaseMinDependency
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if "%OS%"=="" goto NOTNT 
	if not "%OS%"=="Windows_NT" goto NOTNT 
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	goto end 
	:NOTNT 
	echo Warning : Cannot register Unicode DLL on Windows 95 
	:end 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "ShellExtension - Win32 Debug"
# Name "ShellExtension - Win32 Unicode Debug"
# Name "ShellExtension - Win32 Release MinSize"
# Name "ShellExtension - Win32 Release MinDependency"
# Name "ShellExtension - Win32 Unicode Release MinSize"
# Name "ShellExtension - Win32 Unicode Release MinDependency"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\src\common\coretools.cpp
# End Source File
# Begin Source File

SOURCE=..\src\common\RegKey.cpp
# End Source File
# Begin Source File

SOURCE=.\ShellExtension.cpp
# End Source File
# Begin Source File

SOURCE=.\ShellExtension.def

!IF  "$(CFG)" == "ShellExtension - Win32 Debug"

!ELSEIF  "$(CFG)" == "ShellExtension - Win32 Unicode Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ShellExtension - Win32 Release MinSize"

!ELSEIF  "$(CFG)" == "ShellExtension - Win32 Release MinDependency"

!ELSEIF  "$(CFG)" == "ShellExtension - Win32 Unicode Release MinSize"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ShellExtension - Win32 Unicode Release MinDependency"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ShellExtension.idl
# ADD MTL /tlb ".\ShellExtension.tlb" /h "ShellExtension.h" /iid "ShellExtension_i.c" /Oicf
# End Source File
# Begin Source File

SOURCE=.\ShellExtensionU.def

!IF  "$(CFG)" == "ShellExtension - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ShellExtension - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "ShellExtension - Win32 Release MinSize"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ShellExtension - Win32 Release MinDependency"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "ShellExtension - Win32 Unicode Release MinSize"

!ELSEIF  "$(CFG)" == "ShellExtension - Win32 Unicode Release MinDependency"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\WinMergeShell.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\src\common\coretools.h
# End Source File
# Begin Source File

SOURCE=..\src\common\RegKey.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\WinMergeShell.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\ShellExtension.rc
# End Source File
# Begin Source File

SOURCE=.\Languages\ShellExtensionBrazilian.rc
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Languages\ShellExtensionBulgarian.rc
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Languages\ShellExtensionCatalan.rc
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Languages\ShellExtensionChineseSimplified.rc
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Languages\ShellExtensionChineseTraditional.rc
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Languages\ShellExtensionCzech.rc
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Languages\ShellExtensionDanish.rc
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Languages\ShellExtensionDutch.rc
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Languages\ShellExtensionFrench.rc
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Languages\ShellExtensionGerman.rc
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Languages\ShellExtensionHungarian.rc
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Languages\ShellExtensionItalian.rc
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Languages\ShellExtensionJapanese.rc
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Languages\ShellExtensionKorean.rc
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Languages\ShellExtensionNorwegian.rc
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Languages\ShellExtensionPolish.rc
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Languages\ShellExtensionRussian.rc
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Languages\ShellExtensionSlovak.rc
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Languages\ShellExtensionSpanish.rc
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Languages\ShellExtensionSwedish.rc
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Languages\ShellExtensionTurkish.rc
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\WinMerge.bmp
# End Source File
# Begin Source File

SOURCE=.\WinMergeShell.rgs
# End Source File
# End Group
# End Target
# End Project
