# Microsoft Developer Studio Project File - Name="MakePatchDirs" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=MakePatchDirs - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MakePatchDirs.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MakePatchDirs.mak" CFG="MakePatchDirs - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MakePatchDirs - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "MakePatchDirs - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MakePatchDirs - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\BuildTmp\MakePatchDirs__ReleaseA"
# PROP Intermediate_Dir "..\BuildTmp\MakePatchDirs__ReleaseA"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 version.lib /nologo /subsystem:windows /machine:I386 /out:"..\Build\MergeRelease\MakePatchDirs.exe"

!ELSEIF  "$(CFG)" == "MakePatchDirs - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\BuildTmp\MakePatchDirs__DebugA"
# PROP Intermediate_Dir "..\BuildTmp\MakePatchDirs__DebugA"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 version.lib /nologo /subsystem:windows /debug /machine:I386 /out:"..\Build\MergeDebug\MakePatchDirs.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "MakePatchDirs - Win32 Release"
# Name "MakePatchDirs - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AppVersion.cpp
# End Source File
# Begin Source File

SOURCE=.\CDirDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\CMoveConstraint.cpp
# End Source File
# Begin Source File

SOURCE=.\CSubclass.cpp
# End Source File
# Begin Source File

SOURCE=.\DropEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\exc.cpp
# End Source File
# Begin Source File

SOURCE=.\MakeDirs.cpp
# End Source File
# Begin Source File

SOURCE=.\MakePatchDirs.rc
# End Source File
# Begin Source File

SOURCE=.\MakePatchDirsApp.cpp
# End Source File
# Begin Source File

SOURCE=.\MakePatchDirsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Satellites.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AppVersion.h
# End Source File
# Begin Source File

SOURCE=.\CDirDialog.h
# End Source File
# Begin Source File

SOURCE=.\CMoveConstraint.h
# End Source File
# Begin Source File

SOURCE=.\CSubclass.h
# End Source File
# Begin Source File

SOURCE=.\DropEdit.h
# End Source File
# Begin Source File

SOURCE=.\exc.h
# End Source File
# Begin Source File

SOURCE=.\MakeDirs.h
# End Source File
# Begin Source File

SOURCE=.\MakePatchDirsApp.h
# End Source File
# Begin Source File

SOURCE=.\MakePatchDirsDlg.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\Satellites.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\MakePatchDirs.ico
# End Source File
# Begin Source File

SOURCE=.\MakePatchDirs.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=.\ChangeLog.txt
# End Source File
# Begin Source File

SOURCE=.\dist.txt
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
