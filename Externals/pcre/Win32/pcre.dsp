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
!MESSAGE "pcre - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "pcre - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "pcre - Win32 MinSizeRel" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "pcre - Win32 RelWithDebInfo" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "pcre - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\..\Build\pcre\Debug"
# PROP BASE Intermediate_Dir "..\..\..\BuildTmp\pcre\pcre.dir\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\Build\pcre\Debug"
# PROP Intermediate_Dir "..\..\..\BuildTmp\pcre\pcre.dir\Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Zi /Od /I "." /I ".." /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "_CRT_SECURE_NO_DEPRECATE" /D "pcre_EXPORTS" /D "_MBCS" /Fo"$(IntDir)\\" /Fd"$(IntDir)/pcred.pdb" /D /TC /GZ /Zm1000/GX "CMAKE_INTDIR=\" Debug\"" /c
# ADD CPP /nologo /MDd /W3 /Zi /Od /I "." /I ".." /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "_CRT_SECURE_NO_DEPRECATE" /D "pcre_EXPORTS" /D "_MBCS" /Fo"$(IntDir)\\" /Fd"$(IntDir)/pcred.pdb" /D /TC /GZ /Zm1000/GX "CMAKE_INTDIR=\" Debug\"" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "_CRT_SECURE_NO_DEPRECATE" /D "pcre_EXPORTS" /out "..\..\..\BuildTmp\pcre\pcre.dir\Debug" /Oicf /D"CMAKE_INTDIR=\"Debug\"" /proxy"$(InputName)_p.c" /win32
# ADD MTL /nologo /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "_CRT_SECURE_NO_DEPRECATE" /D "pcre_EXPORTS" /out "..\..\..\BuildTmp\pcre\pcre.dir\Debug" /Oicf /D"CMAKE_INTDIR=\"Debug\"" /proxy"$(InputName)_p.c" /win32
# ADD BASE RSC /l 0x409 /i "." /i ".." /d "WIN32" /d "_WINDOWS" /d "_DEBUG" /d "HAVE_CONFIG_H" /d "_CRT_SECURE_NO_DEPRECATE" /d "pcre_EXPORTS" /d "CMAKE_INTDIR=\" Debug\""
# ADD RSC /l 0x409 /i "." /i ".." /d "WIN32" /d "_WINDOWS" /d "_DEBUG" /d "HAVE_CONFIG_H" /d "_CRT_SECURE_NO_DEPRECATE" /d "pcre_EXPORTS" /d "CMAKE_INTDIR=\" Debug\""
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib $(NOINHERIT) kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /version:0.0 /stack:0x989680 /subsystem:windows /dll /pdb:"..\..\..\BuildTmp\pcre\pcre.dir\Debug\pcred.pdb" /debug /machine:I386 /implib:"$(OutDir)/pcre.lib" /pdbtype:sept
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib $(NOINHERIT) kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /version:0.0 /stack:0x989680 /subsystem:windows /dll /pdb:"..\..\..\BuildTmp\pcre\pcre.dir\Debug\pcred.pdb" /debug /machine:I386 /implib:"$(OutDir)/pcre.lib" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
OutDir=.\..\..\..\Build\pcre\Debug
SOURCE="$(InputPath)"
PostBuild_Cmds=copy $(OutDir)\pcre.dll ..\..\..\Build\pcre	copy $(OutDir)\pcre.lib ..\..\..\Build\pcre
# End Special Build Tool

!ELSEIF  "$(CFG)" == "pcre - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\..\Build\pcre\Release"
# PROP BASE Intermediate_Dir "..\..\..\BuildTmp\pcre\pcre.dir\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\Build\pcre\Release"
# PROP Intermediate_Dir "..\..\..\BuildTmp\pcre\pcre.dir\Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /O2 /Ob2 /I "." /I ".." /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "_CRT_SECURE_NO_DEPRECATE" /D "pcre_EXPORTS" /D "_MBCS" /D /TC /Zm1000/GX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /Ob2 /I "." /I ".." /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "_CRT_SECURE_NO_DEPRECATE" /D "pcre_EXPORTS" /D "_MBCS" /D /TC /Zm1000 /c
# ADD BASE MTL /nologo /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "_CRT_SECURE_NO_DEPRECATE" /D "pcre_EXPORTS" /out "..\..\..\BuildTmp\pcre\pcre.dir\Release" /Oicf /D"CMAKE_INTDIR=\"Release\"" /proxy"$(InputName)_p.c" /win32
# ADD MTL /nologo /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "_CRT_SECURE_NO_DEPRECATE" /D "pcre_EXPORTS" /out "..\..\..\BuildTmp\pcre\pcre.dir\Release" /Oicf /win32
# ADD BASE RSC /l 0x409 /i "." /i ".." /d "WIN32" /d "_WINDOWS" /d "NDEBUG" /d "HAVE_CONFIG_H" /d "_CRT_SECURE_NO_DEPRECATE" /d "pcre_EXPORTS" /d "CMAKE_INTDIR=\" Release\""
# ADD RSC /l 0x409 /i "." /i ".." /d "WIN32" /d "_WINDOWS" /d "NDEBUG" /d "HAVE_CONFIG_H" /d "_CRT_SECURE_NO_DEPRECATE" /d "pcre_EXPORTS"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib $(NOINHERIT) kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /version:0.0 /stack:0x989680 /subsystem:windows /dll /pdb:"$(Intdir)\pcre.pdb" /machine:I386 /out:"$(Outdir)\pcre.dll" /implib:"$(OutDir)/pcre.lib" /pdbtype:sept
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /version:0.0 /stack:0x989680 /subsystem:windows /dll /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
OutDir=.\..\..\..\Build\pcre\Release
SOURCE="$(InputPath)"
PostBuild_Cmds=copy $(OutDir)\pcre.dll ..\..\..\Build\pcre	copy $(OutDir)\pcre.lib ..\..\..\Build\pcre
# End Special Build Tool

!ELSEIF  "$(CFG)" == "pcre - Win32 MinSizeRel"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\..\Build\pcre\MinSizeRel"
# PROP BASE Intermediate_Dir "..\..\..\BuildTmp\pcre\pcre.dir\MinSizeRel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\Build\pcre\MinSizeRel"
# PROP Intermediate_Dir "..\..\..\BuildTmp\pcre\pcre.dir\MinSizeRel"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /O1 /I "." /I ".." /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "_CRT_SECURE_NO_DEPRECATE" /D "pcre_EXPORTS" /D "_MBCS" /Fo"$(IntDir)\\" /D /TC /Zm1000/GX "CMAKE_INTDIR=\" MinSizeRel\"" /c
# ADD CPP /nologo /MD /W3 /O1 /I "." /I ".." /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "_CRT_SECURE_NO_DEPRECATE" /D "pcre_EXPORTS" /D "_MBCS" /Fo"$(IntDir)\\" /D /TC /Zm1000/GX "CMAKE_INTDIR=\" MinSizeRel\"" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "_CRT_SECURE_NO_DEPRECATE" /D "pcre_EXPORTS" /out "..\..\..\BuildTmp\pcre\pcre.dir\MinSizeRel" /Oicf /D"CMAKE_INTDIR=\"MinSizeRel\"" /proxy"$(InputName)_p.c" /win32
# ADD MTL /nologo /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "_CRT_SECURE_NO_DEPRECATE" /D "pcre_EXPORTS" /out "..\..\..\BuildTmp\pcre\pcre.dir\MinSizeRel" /Oicf /D"CMAKE_INTDIR=\"MinSizeRel\"" /proxy"$(InputName)_p.c" /win32
# ADD BASE RSC /l 0x409 /i "." /i ".." /d "WIN32" /d "_WINDOWS" /d "NDEBUG" /d "HAVE_CONFIG_H" /d "_CRT_SECURE_NO_DEPRECATE" /d "pcre_EXPORTS" /d "CMAKE_INTDIR=\" MinSizeRel\""
# ADD RSC /l 0x409 /i "." /i ".." /d "WIN32" /d "_WINDOWS" /d "NDEBUG" /d "HAVE_CONFIG_H" /d "_CRT_SECURE_NO_DEPRECATE" /d "pcre_EXPORTS" /d "CMAKE_INTDIR=\" MinSizeRel\""
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib $(NOINHERIT) kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /version:0.0 /stack:0x989680 /subsystem:windows /dll /pdb:"..\..\..\BuildTmp\pcre\pcre.dir\MinSizeRel\pcre.pdb" /machine:I386 /implib:"$(OutDir)/pcre.lib" /pdbtype:sept
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib $(NOINHERIT) kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /version:0.0 /stack:0x989680 /subsystem:windows /dll /pdb:"..\..\..\BuildTmp\pcre\pcre.dir\MinSizeRel\pcre.pdb" /machine:I386 /implib:"$(OutDir)/pcre.lib" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
OutDir=.\..\..\..\Build\pcre\MinSizeRel
SOURCE="$(InputPath)"
PostBuild_Cmds=copy $(OutDir)\pcre.dll ..\..\..\Build\pcre	copy $(OutDir)\pcre.lib ..\..\..\Build\pcre
# End Special Build Tool

!ELSEIF  "$(CFG)" == "pcre - Win32 RelWithDebInfo"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\..\Build\pcre\RelWithDebInfo"
# PROP BASE Intermediate_Dir "..\..\..\BuildTmp\pcre\pcre.dir\RelWithDebInfo"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\Build\pcre\RelWithDebInfo"
# PROP Intermediate_Dir "..\..\..\BuildTmp\pcre\pcre.dir\RelWithDebInfo"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /Zi /O2 /I "." /I ".." /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "_CRT_SECURE_NO_DEPRECATE" /D "pcre_EXPORTS" /D "_MBCS" /Fo"$(IntDir)\\" /Fd"$(IntDir)/pcre.pdb" /D /TC /Zm1000/GX "CMAKE_INTDIR=\" RelWithDebInfo\"" /c
# ADD CPP /nologo /MD /W3 /Zi /O2 /I "." /I ".." /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "_CRT_SECURE_NO_DEPRECATE" /D "pcre_EXPORTS" /D "_MBCS" /Fo"$(IntDir)\\" /Fd"$(IntDir)/pcre.pdb" /D /TC /Zm1000/GX "CMAKE_INTDIR=\" RelWithDebInfo\"" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "_CRT_SECURE_NO_DEPRECATE" /D "pcre_EXPORTS" /out "..\..\..\BuildTmp\pcre\pcre.dir\RelWithDebInfo" /Oicf /D"CMAKE_INTDIR=\"RelWithDebInfo\"" /proxy"$(InputName)_p.c" /win32
# ADD MTL /nologo /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "_CRT_SECURE_NO_DEPRECATE" /D "pcre_EXPORTS" /out "..\..\..\BuildTmp\pcre\pcre.dir\RelWithDebInfo" /Oicf /D"CMAKE_INTDIR=\"RelWithDebInfo\"" /proxy"$(InputName)_p.c" /win32
# ADD BASE RSC /l 0x409 /i "." /i ".." /d "WIN32" /d "_WINDOWS" /d "NDEBUG" /d "HAVE_CONFIG_H" /d "_CRT_SECURE_NO_DEPRECATE" /d "pcre_EXPORTS" /d "CMAKE_INTDIR=\" RelWithDebInfo\""
# ADD RSC /l 0x409 /i "." /i ".." /d "WIN32" /d "_WINDOWS" /d "NDEBUG" /d "HAVE_CONFIG_H" /d "_CRT_SECURE_NO_DEPRECATE" /d "pcre_EXPORTS" /d "CMAKE_INTDIR=\" RelWithDebInfo\""
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib $(NOINHERIT) kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /version:0.0 /stack:0x989680 /subsystem:windows /dll /incremental:yes /pdb:"..\..\..\BuildTmp\pcre\pcre.dir\RelWithDebInfo\pcre.pdb" /debug /machine:I386 /implib:"$(OutDir)/pcre.lib" /pdbtype:sept
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib $(NOINHERIT) kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /version:0.0 /stack:0x989680 /subsystem:windows /dll /incremental:yes /pdb:"..\..\..\BuildTmp\pcre\pcre.dir\RelWithDebInfo\pcre.pdb" /debug /machine:I386 /implib:"$(OutDir)/pcre.lib" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
OutDir=.\..\..\..\Build\pcre\RelWithDebInfo
SOURCE="$(InputPath)"
PostBuild_Cmds=copy $(OutDir)\pcre.dll ..\..\..\Build\pcre	copy $(OutDir)\pcre.lib ..\..\..\Build\pcre
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "pcre - Win32 Debug"
# Name "pcre - Win32 Release"
# Name "pcre - Win32 MinSizeRel"
# Name "pcre - Win32 RelWithDebInfo"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=pcre_chartables.c
DEP_CPP_PCRE_=\
	"..\pcre_internal.h"\
	"..\ucp.h"\
	".\config.h"\
	".\pcre.h"\
	
# End Source File
# Begin Source File

SOURCE=..\pcre_compile.c
DEP_CPP_PCRE_C=\
	"..\pcre_internal.h"\
	"..\pcre_printint.src"\
	"..\ucp.h"\
	".\config.h"\
	".\pcre.h"\
	
# End Source File
# Begin Source File

SOURCE=..\pcre_config.c
DEP_CPP_PCRE_CO=\
	"..\pcre_internal.h"\
	"..\ucp.h"\
	".\config.h"\
	".\pcre.h"\
	
# End Source File
# Begin Source File

SOURCE=..\pcre_dfa_exec.c
DEP_CPP_PCRE_D=\
	"..\pcre_internal.h"\
	"..\ucp.h"\
	".\config.h"\
	".\pcre.h"\
	
# End Source File
# Begin Source File

SOURCE=..\pcre_exec.c
DEP_CPP_PCRE_E=\
	"..\pcre_internal.h"\
	"..\ucp.h"\
	".\config.h"\
	".\pcre.h"\
	
# End Source File
# Begin Source File

SOURCE=..\pcre_fullinfo.c
DEP_CPP_PCRE_F=\
	"..\pcre_internal.h"\
	"..\ucp.h"\
	".\config.h"\
	".\pcre.h"\
	
# End Source File
# Begin Source File

SOURCE=..\pcre_get.c
DEP_CPP_PCRE_G=\
	"..\pcre_internal.h"\
	"..\ucp.h"\
	".\config.h"\
	".\pcre.h"\
	
# End Source File
# Begin Source File

SOURCE=..\pcre_globals.c
DEP_CPP_PCRE_GL=\
	"..\pcre_internal.h"\
	"..\ucp.h"\
	".\config.h"\
	".\pcre.h"\
	
# End Source File
# Begin Source File

SOURCE=..\pcre_info.c
DEP_CPP_PCRE_I=\
	"..\pcre_internal.h"\
	"..\ucp.h"\
	".\config.h"\
	".\pcre.h"\
	
# End Source File
# Begin Source File

SOURCE=..\pcre_maketables.c
DEP_CPP_PCRE_M=\
	"..\pcre_internal.h"\
	"..\ucp.h"\
	".\config.h"\
	".\pcre.h"\
	
# End Source File
# Begin Source File

SOURCE=..\pcre_newline.c
DEP_CPP_PCRE_N=\
	"..\pcre_internal.h"\
	"..\ucp.h"\
	".\config.h"\
	".\pcre.h"\
	
# End Source File
# Begin Source File

SOURCE=..\pcre_ord2utf8.c
DEP_CPP_PCRE_O=\
	"..\pcre_internal.h"\
	"..\ucp.h"\
	".\config.h"\
	".\pcre.h"\
	
# End Source File
# Begin Source File

SOURCE=..\pcre_refcount.c
DEP_CPP_PCRE_R=\
	"..\pcre_internal.h"\
	"..\ucp.h"\
	".\config.h"\
	".\pcre.h"\
	
# End Source File
# Begin Source File

SOURCE=..\pcre_study.c
DEP_CPP_PCRE_S=\
	"..\pcre_internal.h"\
	"..\ucp.h"\
	".\config.h"\
	".\pcre.h"\
	
# End Source File
# Begin Source File

SOURCE=..\pcre_tables.c
DEP_CPP_PCRE_T=\
	"..\pcre_internal.h"\
	"..\ucp.h"\
	".\config.h"\
	".\pcre.h"\
	
# End Source File
# Begin Source File

SOURCE=..\pcre_try_flipped.c
DEP_CPP_PCRE_TR=\
	"..\pcre_internal.h"\
	"..\ucp.h"\
	".\config.h"\
	".\pcre.h"\
	
# End Source File
# Begin Source File

SOURCE=..\pcre_ucp_searchfuncs.c
DEP_CPP_PCRE_U=\
	"..\pcre_internal.h"\
	"..\ucp.h"\
	"..\ucpinternal.h"\
	"..\ucptable.h"\
	".\config.h"\
	".\pcre.h"\
	
# End Source File
# Begin Source File

SOURCE=..\pcre_valid_utf8.c
DEP_CPP_PCRE_V=\
	"..\pcre_internal.h"\
	"..\ucp.h"\
	".\config.h"\
	".\pcre.h"\
	
# End Source File
# Begin Source File

SOURCE=..\pcre_version.c
DEP_CPP_PCRE_VE=\
	"..\pcre_internal.h"\
	"..\ucp.h"\
	".\config.h"\
	".\pcre.h"\
	
# End Source File
# Begin Source File

SOURCE=..\pcre_xclass.c
DEP_CPP_PCRE_X=\
	"..\pcre_internal.h"\
	"..\ucp.h"\
	".\config.h"\
	".\pcre.h"\
	
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=config.h
# End Source File
# Begin Source File

SOURCE=pcre.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\CMakeLists.txt
# PROP Ignore_Default_Tool 1
# End Source File
# End Target
# End Project
