# Microsoft Developer Studio Project File - Name="pcre" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=pcre - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "pcre.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "pcre.mak" CFG="pcre - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "pcre - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "pcre - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "pcre - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\bin\release"
# PROP Intermediate_Dir "o\release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PCRE_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\bin" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PCRE_EXPORTS" /D "DLL_EXPORT" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=mkdir ..\..\..\Build\pcre	Copy ..\bin\release\pcre.lib ..\..\..\Build\pcre	Copy ..\bin\release\pcre.dll ..\..\..\Build\pcre
# End Special Build Tool

!ELSEIF  "$(CFG)" == "pcre - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\bin\debug"
# PROP Intermediate_Dir "o\debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PCRE_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\bin" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PCRE_EXPORTS" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=mkdir ..\..\..\Build\pcre	Copy ..\bin\release\pcre.lib ..\..\..\Build\pcre	Copy ..\bin\debug\pcre.dll ..\..\..\Build\pcre
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "pcre - Win32 Release"
# Name "pcre - Win32 Debug"
# Begin Source File

SOURCE=..\bin\pcre_chartables.c
# End Source File
# Begin Source File

SOURCE="..\pcre-6.7\pcre_compile.c"
# End Source File
# Begin Source File

SOURCE="..\pcre-6.7\pcre_config.c"
# End Source File
# Begin Source File

SOURCE="..\pcre-6.7\pcre_dfa_exec.c"
# End Source File
# Begin Source File

SOURCE="..\pcre-6.7\pcre_exec.c"
# End Source File
# Begin Source File

SOURCE="..\pcre-6.7\pcre_fullinfo.c"
# End Source File
# Begin Source File

SOURCE="..\pcre-6.7\pcre_get.c"
# End Source File
# Begin Source File

SOURCE="..\pcre-6.7\pcre_globals.c"
# End Source File
# Begin Source File

SOURCE="..\pcre-6.7\pcre_info.c"
# End Source File
# Begin Source File

SOURCE="..\pcre-6.7\pcre_maketables.c"
# End Source File
# Begin Source File

SOURCE="..\pcre-6.7\pcre_ord2utf8.c"
# End Source File
# Begin Source File

SOURCE="..\pcre-6.7\pcre_refcount.c"
# End Source File
# Begin Source File

SOURCE="..\pcre-6.7\pcre_study.c"
# End Source File
# Begin Source File

SOURCE="..\pcre-6.7\pcre_tables.c"
# End Source File
# Begin Source File

SOURCE="..\pcre-6.7\pcre_try_flipped.c"
# End Source File
# Begin Source File

SOURCE="..\pcre-6.7\pcre_ucp_searchfuncs.c"
# End Source File
# Begin Source File

SOURCE="..\pcre-6.7\pcre_valid_utf8.c"
# End Source File
# Begin Source File

SOURCE="..\pcre-6.7\pcre_version.c"
# End Source File
# Begin Source File

SOURCE="..\pcre-6.7\pcre_xclass.c"
# End Source File
# End Target
# End Project
