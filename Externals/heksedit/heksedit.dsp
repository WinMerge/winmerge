# Microsoft Developer Studio Project File - Name="heksedit" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=heksedit - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "heksedit.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "heksedit.mak" CFG="heksedit - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "heksedit - Win32 Release" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "heksedit - Win32 Debug" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "heksedit - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../Build/heksedit/Release"
# PROP Intermediate_Dir "../../BuildTmp/heksedit/Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Od /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /FR /Yu"precomp.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 version.lib comctl32.lib wininet.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib shlwapi.lib htmlhelp.lib /nologo /subsystem:windows /dll /machine:I386
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
IntDir=.\../../BuildTmp/heksedit/Release
TargetPath=\svnroot2\trunk\Build\heksedit\Release\heksedit.dll
SOURCE="$(InputPath)"
PreLink_Cmds=PreLink.bat $(IntDir) $(TargetPath)
# End Special Build Tool

!ELSEIF  "$(CFG)" == "heksedit - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../Build/heksedit/Debug"
# PROP Intermediate_Dir "../../BuildTmp/heksedit/Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /Z7 /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /FR /Yu"precomp.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 version.lib comctl32.lib wininet.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib shlwapi.lib htmlhelp.lib /nologo /subsystem:windows /dll /incremental:no /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /nodefaultlib
# Begin Special Build Tool
IntDir=.\../../BuildTmp/heksedit/Debug
TargetPath=\svnroot2\trunk\Build\heksedit\Debug\heksedit.dll
SOURCE="$(InputPath)"
PreLink_Cmds=PreLink.bat $(IntDir) $(TargetPath)
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "heksedit - Win32 Release"
# Name "heksedit - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AddBmkDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\AppendDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\BinTrans.cpp
# End Source File
# Begin Source File

SOURCE=.\BitManipDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ChooseDiffDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\clipboard.cpp
# End Source File
# Begin Source File

SOURCE=.\CopyDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CopyHexdumpDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CutDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\dllmain.cpp
# End Source File
# Begin Source File

SOURCE=.\DllProxies.cpp
# End Source File
# Begin Source File

SOURCE=.\EnterDecimalValueDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\FillWithDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\FindDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\gktools.cpp
# End Source File
# Begin Source File

SOURCE=.\GoToDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\gtools.cpp
# End Source File
# Begin Source File

SOURCE=.\hexwdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\hexwnd.cpp
# End Source File
# Begin Source File

SOURCE=.\hgstream.cpp
# End Source File
# Begin Source File

SOURCE=.\ido.cpp
# End Source File
# Begin Source File

SOURCE=.\ids.cpp
# End Source File
# Begin Source File

SOURCE=.\idt.cpp
# End Source File
# Begin Source File

SOURCE=.\InvokeHtmlHelp.cpp
# End Source File
# Begin Source File

SOURCE=.\LangArray.cpp
# End Source File
# Begin Source File

SOURCE=.\LoadHexFile.cpp
# End Source File
# Begin Source File

SOURCE=.\MoveCopyDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\OpenPartiallyDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PasteDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PDrive95.cpp
# End Source File
# Begin Source File

SOURCE=.\PDriveNT.cpp
# End Source File
# Begin Source File

SOURCE=.\PhysicalDrive.cpp
# End Source File
# Begin Source File

SOURCE=.\PMemoryBlock.cpp
# End Source File
# Begin Source File

SOURCE=.\precomp.cpp
# ADD CPP /Yc"precomp.h"
# End Source File
# Begin Source File

SOURCE=.\regtools.cpp
# End Source File
# Begin Source File

SOURCE=.\RemoveBmkDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ReplaceDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ReverseDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SelectBlockDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ShortcutsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\shtools.cpp
# End Source File
# Begin Source File

SOURCE=.\Simparr.cpp
# End Source File
# Begin Source File

SOURCE=.\StringTable.cpp
# End Source File
# Begin Source File

SOURCE=.\UpgradeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ViewSettingsDlg.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\BinTrans.h
# End Source File
# Begin Source File

SOURCE=.\clipboard.h
# End Source File
# Begin Source File

SOURCE=.\DllProxies.h
# End Source File
# Begin Source File

SOURCE=.\gktools.h
# End Source File
# Begin Source File

SOURCE=.\GlobalStream.h
# End Source File
# Begin Source File

SOURCE=.\gtools.h
# End Source File
# Begin Source File

SOURCE=.\heksedit.h
# End Source File
# Begin Source File

SOURCE=.\hexwdlg.h
# End Source File
# Begin Source File

SOURCE=.\hexwnd.h
# End Source File
# Begin Source File

SOURCE=.\ido.h
# End Source File
# Begin Source File

SOURCE=.\ids.h
# End Source File
# Begin Source File

SOURCE=.\idt.h
# End Source File
# Begin Source File

SOURCE=.\InvokeHtmlHelp.h
# End Source File
# Begin Source File

SOURCE=.\LangArray.h
# End Source File
# Begin Source File

SOURCE=.\LoadHexFile.h
# End Source File
# Begin Source File

SOURCE=.\ntdiskspec.h
# End Source File
# Begin Source File

SOURCE=.\PDrive95.h
# End Source File
# Begin Source File

SOURCE=.\PDriveNT.h
# End Source File
# Begin Source File

SOURCE=.\PhysicalDrive.h
# End Source File
# Begin Source File

SOURCE=.\PMemoryBlock.h
# End Source File
# Begin Source File

SOURCE=.\precomp.h
# End Source File
# Begin Source File

SOURCE=..\RAWIO32\RAWIO32.h
# End Source File
# Begin Source File

SOURCE=.\regtools.h
# End Source File
# Begin Source File

SOURCE=.\shtools.h
# End Source File
# Begin Source File

SOURCE=.\Simparr.h
# End Source File
# Begin Source File

SOURCE=.\StringTable.h
# End Source File
# Begin Source File

SOURCE=.\toolbar.h
# End Source File
# Begin Source File

SOURCE=.\UpgradeDlg.h
# End Source File
# Begin Source File

SOURCE=.\version.h
# End Source File
# Begin Source File

SOURCE=.\VersionData.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\heksedit.rc
# End Source File
# Begin Source File

SOURCE=.\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\Toolbar.bmp
# End Source File
# End Group
# Begin Group "Text Files"

# PROP Default_Filter "txt"
# Begin Source File

SOURCE=.\Bugs.txt
# End Source File
# Begin Source File

SOURCE=.\History.txt
# End Source File
# Begin Source File

SOURCE=.\Readme.txt
# End Source File
# Begin Source File

SOURCE=.\Todo.txt
# End Source File
# End Group
# End Target
# End Project
