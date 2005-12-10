# Microsoft Developer Studio Project File - Name="expat_maps_static" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=expat_maps_static - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "expat_maps_static.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "expat_maps_static.mak" CFG="expat_maps_static - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "expat_maps_static - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "expat_maps_static - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "expat_maps_static - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release_static"
# PROP Intermediate_Dir "Release_static"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\ExpatLib" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Release_static\libexpatMapsMT.lib"

!ELSEIF  "$(CFG)" == "expat_maps_static - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug_static"
# PROP Intermediate_Dir "Debug_static"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\ExpatLib" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Debug_static\libexpatMapsMT.lib"

!ENDIF 

# Begin Target

# Name "expat_maps_static - Win32 Release"
# Name "expat_maps_static - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\expat_maps.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\expat_maps.h
# End Source File
# Begin Source File

SOURCE=.\map_data_8859_1.h
# End Source File
# Begin Source File

SOURCE=.\map_data_8859_10.h
# End Source File
# Begin Source File

SOURCE=.\map_data_8859_11.h
# End Source File
# Begin Source File

SOURCE=.\map_data_8859_13.h
# End Source File
# Begin Source File

SOURCE=.\map_data_8859_14.h
# End Source File
# Begin Source File

SOURCE=.\map_data_8859_15.h
# End Source File
# Begin Source File

SOURCE=.\map_data_8859_16.h
# End Source File
# Begin Source File

SOURCE=.\map_data_8859_2.h
# End Source File
# Begin Source File

SOURCE=.\map_data_8859_3.h
# End Source File
# Begin Source File

SOURCE=.\map_data_8859_4.h
# End Source File
# Begin Source File

SOURCE=.\map_data_8859_5.h
# End Source File
# Begin Source File

SOURCE=.\map_data_8859_6.h
# End Source File
# Begin Source File

SOURCE=.\map_data_8859_7.h
# End Source File
# Begin Source File

SOURCE=.\map_data_8859_8.h
# End Source File
# Begin Source File

SOURCE=.\map_data_8859_9.h
# End Source File
# Begin Source File

SOURCE=.\map_data_CP1250.h
# End Source File
# Begin Source File

SOURCE=.\map_data_CP1251.h
# End Source File
# Begin Source File

SOURCE=.\map_data_CP1252.h
# End Source File
# Begin Source File

SOURCE=.\map_data_CP1253.h
# End Source File
# Begin Source File

SOURCE=.\map_data_CP1254.h
# End Source File
# Begin Source File

SOURCE=.\map_data_CP1255.h
# End Source File
# Begin Source File

SOURCE=.\map_data_CP1256.h
# End Source File
# Begin Source File

SOURCE=.\map_data_CP1257.h
# End Source File
# Begin Source File

SOURCE=.\map_data_CP1258.h
# End Source File
# End Group
# End Target
# End Project
