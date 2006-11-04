# Microsoft Developer Studio Project File - Name="scew" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=scew - Win32 XMLStatic Debug Unicode
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "scew.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "scew.mak" CFG="scew - Win32 XMLStatic Debug Unicode"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "scew - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "scew - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "scew - Win32 Debug Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "scew - Win32 Release Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "scew - Win32 XMLStatic Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "scew - Win32 XMLStatic Release" (based on "Win32 (x86) Static Library")
!MESSAGE "scew - Win32 XMLStatic Debug Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE "scew - Win32 XMLStatic Release Unicode" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "scew - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "obj\Release"
# PROP BASE Intermediate_Dir "obj\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "obj\Release"
# PROP Intermediate_Dir "obj\Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W4 /GX /O2 /I "../../expat/lib" /I "../../expat/xmlwf" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"lib\scew.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy  lib\scew.lib  ..\..\..\Build\expat\ 
# End Special Build Tool

!ELSEIF  "$(CFG)" == "scew - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "obj\Debug"
# PROP BASE Intermediate_Dir "obj\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "obj\Debug"
# PROP Intermediate_Dir "obj\Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /GX /ZI /Od /I "../../expat/lib" /I "../../expat/xmlwf" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x411 /d "_DEBUG"
# ADD RSC /l 0x411 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"lib\scew_d.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy  lib\scew_d.lib  ..\..\..\Build\expat\ 
# End Special Build Tool

!ELSEIF  "$(CFG)" == "scew - Win32 Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "obj\Debug Unicode"
# PROP BASE Intermediate_Dir "obj\Debug Unicode"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "obj\DebugU"
# PROP Intermediate_Dir "obj\DebugU"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MDd /W4 /Gm /GX /ZI /Od /I "../../expat/lib" /I "../../expat/xmlwf" /D "_DEBUG" /D "WIN32" /D "_LIB" /D "_UNICODE" /D "XML_UNICODE_WCHAR_T" /FD /GZ /c
# SUBTRACT CPP /WX /YX
# ADD BASE RSC /l 0x411 /d "_DEBUG"
# ADD RSC /l 0x411 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"lib\scew_ud.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy  lib\scew_ud.lib  ..\..\..\Build\expat\ 
# End Special Build Tool

!ELSEIF  "$(CFG)" == "scew - Win32 Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "obj\Release Unicode"
# PROP BASE Intermediate_Dir "obj\Release Unicode"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "obj\ReleaseU"
# PROP Intermediate_Dir "obj\ReleaseU"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MD /W4 /GX /O2 /I "../../expat/lib" /I "../../expat/xmlwf" /D "NDEBUG" /D "WIN32" /D "_LIB" /D "_UNICODE" /D "XML_UNICODE_WCHAR_T" /FD /c
# SUBTRACT CPP /WX /YX
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"lib\scew_u.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy  lib\scew_u.lib  ..\..\..\Build\expat\ 
# End Special Build Tool

!ELSEIF  "$(CFG)" == "scew - Win32 XMLStatic Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "scew___Win32_XMLStatic_Debug"
# PROP BASE Intermediate_Dir "scew___Win32_XMLStatic_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "obj\DebugS"
# PROP Intermediate_Dir "obj\DebugS"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W4 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MDd /W4 /Gm /GX /ZI /Od /I "../../expat/lib" /I "../../expat/xmlwf" /D "_DEBUG" /D "_MBCS" /D "WIN32" /D "_LIB" /D "XML_STATIC" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x411 /d "_DEBUG"
# ADD RSC /l 0x411 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"lib\scew_d.lib"
# ADD LIB32 /nologo /out:"lib\scew_sd.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy  lib\scew_sd.lib  ..\..\..\Build\expat\ 
# End Special Build Tool

!ELSEIF  "$(CFG)" == "scew - Win32 XMLStatic Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "scew___Win32_XMLStatic_Release"
# PROP BASE Intermediate_Dir "scew___Win32_XMLStatic_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "obj\ReleaseS"
# PROP Intermediate_Dir "obj\ReleaseS"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MD /W4 /GX /O2 /I "../../expat/lib" /I "../../expat/xmlwf" /D "NDEBUG" /D "_MBCS" /D "WIN32" /D "_LIB" /D "XML_STATIC" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"lib\scew.lib"
# ADD LIB32 /nologo /out:"lib\scew_s.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy  lib\scew_s.lib  ..\..\..\Build\expat\ 
# End Special Build Tool

!ELSEIF  "$(CFG)" == "scew - Win32 XMLStatic Debug Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "scew___Win32_XMLStatic_Debug_Unicode"
# PROP BASE Intermediate_Dir "scew___Win32_XMLStatic_Debug_Unicode"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "obj\DebugUS"
# PROP Intermediate_Dir "obj\DebugUS"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W4 /Gm /GX /ZI /Od /D "_DEBUG" /D "_UNICODE" /D "WIN32" /D "_LIB" /FD /GZ /c
# SUBTRACT BASE CPP /WX /YX
# ADD CPP /nologo /MDd /W4 /Gm /GX /ZI /Od /I "../../expat/lib" /I "../../expat/xmlwf" /D "_DEBUG" /D "XML_STATIC" /D "WIN32" /D "_LIB" /D "_UNICODE" /D "XML_UNICODE_WCHAR_T" /FD /GZ /c
# SUBTRACT CPP /WX /YX
# ADD BASE RSC /l 0x411 /d "_DEBUG"
# ADD RSC /l 0x411 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"lib\scew_ud.lib"
# ADD LIB32 /nologo /out:"lib\scew_usd.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy  lib\scew_usd.lib  ..\..\..\Build\expat\ 
# End Special Build Tool

!ELSEIF  "$(CFG)" == "scew - Win32 XMLStatic Release Unicode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "scew___Win32_XMLStatic_Release_Unicode"
# PROP BASE Intermediate_Dir "scew___Win32_XMLStatic_Release_Unicode"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "obj\ReleaseUS"
# PROP Intermediate_Dir "obj\ReleaseUS"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W4 /GX /O2 /D "NDEBUG" /D "_UNICODE" /D "WIN32" /D "_LIB" /FD /c
# SUBTRACT BASE CPP /WX /YX
# ADD CPP /nologo /MD /W4 /GX /O2 /I "../../expat/lib" /I "../../expat/xmlwf" /D "NDEBUG" /D "XML_STATIC" /D "WIN32" /D "_LIB" /D "_UNICODE" /D "XML_UNICODE_WCHAR_T" /FD /c
# SUBTRACT CPP /WX /YX
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"lib\scew_u.lib"
# ADD LIB32 /nologo /out:"lib\scew_us.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy  lib\scew_us.lib  ..\..\..\Build\expat\ 
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "scew - Win32 Release"
# Name "scew - Win32 Debug"
# Name "scew - Win32 Debug Unicode"
# Name "scew - Win32 Release Unicode"
# Name "scew - Win32 XMLStatic Debug"
# Name "scew - Win32 XMLStatic Release"
# Name "scew - Win32 XMLStatic Debug Unicode"
# Name "scew - Win32 XMLStatic Release Unicode"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\scew\attribute.c
# End Source File
# Begin Source File

SOURCE=..\scew\element.c
# End Source File
# Begin Source File

SOURCE=..\scew\error.c
# End Source File
# Begin Source File

SOURCE=..\scew\parser.c
# End Source File
# Begin Source File

SOURCE=..\scew\str.c
# End Source File
# Begin Source File

SOURCE=..\scew\tree.c
# End Source File
# Begin Source File

SOURCE=..\scew\writer.c
# End Source File
# Begin Source File

SOURCE=..\scew\xattribute.c
# End Source File
# Begin Source File

SOURCE=..\scew\xerror.c
# End Source File
# Begin Source File

SOURCE=..\scew\xhandler.c
# End Source File
# Begin Source File

SOURCE=..\scew\xparser.c
# End Source File
# Begin Source File

SOURCE=..\scew\xprint.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\scew\attribute.h
# End Source File
# Begin Source File

SOURCE=..\scew\element.h
# End Source File
# Begin Source File

SOURCE=..\scew\error.h
# End Source File
# Begin Source File

SOURCE=..\scew\parser.h
# End Source File
# Begin Source File

SOURCE=..\scew\scew.h
# End Source File
# Begin Source File

SOURCE=..\scew\str.h
# End Source File
# Begin Source File

SOURCE=..\scew\tree.h
# End Source File
# Begin Source File

SOURCE=..\scew\types.h
# End Source File
# Begin Source File

SOURCE=..\scew\writer.h
# End Source File
# Begin Source File

SOURCE=..\scew\xattribute.h
# End Source File
# Begin Source File

SOURCE=..\scew\xelement.h
# End Source File
# Begin Source File

SOURCE=..\scew\xerror.h
# End Source File
# Begin Source File

SOURCE=..\scew\xhandler.h
# End Source File
# Begin Source File

SOURCE=..\scew\xparser.h
# End Source File
# Begin Source File

SOURCE=..\scew\xprint.h
# End Source File
# Begin Source File

SOURCE=..\scew\xtree.h
# End Source File
# End Group
# End Target
# End Project
