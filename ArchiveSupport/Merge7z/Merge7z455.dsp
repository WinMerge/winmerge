# Microsoft Developer Studio Project File - Name="Merge7z455" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Merge7z455 - Win32 UnicodeDebug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "Merge7z455.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "Merge7z455.mak" CFG="Merge7z455 - Win32 UnicodeDebug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "Merge7z455 - Win32 Release" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Merge7z455 - Win32 Debug" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Merge7z455 - Win32 UnicodeDebug" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Merge7z455 - Win32 UnicodeRelease" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Merge7z455 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\BuildTmp\Merge7z455___Win32_Release"
# PROP Intermediate_Dir "..\BuildTmp\Merge7z455___Win32_Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MERGE7Z310_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O1 /I "..\..\..\7z455" /I "..\..\..\7z455\CPP" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "Merge7z455_EXPORTS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib shlwapi.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /pdb:none /machine:I386 /out:"../Build/MergeRelease/Merge7z455.dll"

!ELSEIF  "$(CFG)" == "Merge7z455 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\BuildTmp\Merge7z455___Win32_Debug"
# PROP Intermediate_Dir "..\BuildTmp\Merge7z455___Win32_Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MERGE7Z310_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /vd0 /GX /Z7 /Od /I "..\..\..\7z455" /I "..\..\..\7z455\CPP" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "Merge7z455_EXPORTS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib shlwapi.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:no /pdb:"../Build/MergeDebug/Merge7z455.pdb" /debug /machine:I386 /out:"../Build/MergeDebug/Merge7z455.dll"
# SUBTRACT LINK32 /pdb:none /map

!ELSEIF  "$(CFG)" == "Merge7z455 - Win32 UnicodeDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\BuildTmp\Merge7z455___Win32_UnicodeDebug"
# PROP BASE Intermediate_Dir "..\BuildTmp\Merge7z455___Win32_UnicodeDebug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\BuildTmp\Merge7z455___Win32_UnicodeDebug"
# PROP Intermediate_Dir "..\BuildTmp\Merge7z455___Win32_UnicodeDebug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /vd0 /GX /Z7 /Od /I "..\..\..\7z455" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MERGE7Z310_EXPORTS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /vd0 /GX /Z7 /Od /I "..\..\..\7z455" /I "..\..\..\7z455\CPP" /D "Merge7z455_EXPORTS" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "_UNICODE" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shlwapi.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /pdb:none /map /debug /machine:I386 /out:"../Build/MergeDebug/Merge7z455.dll"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib shlwapi.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:no /pdb:"../Build/MergeUnicodeDebug/Merge7z455U.pdb" /debug /machine:I386 /out:"../Build/MergeUnicodeDebug/Merge7z455U.dll"
# SUBTRACT LINK32 /pdb:none /map

!ELSEIF  "$(CFG)" == "Merge7z455 - Win32 UnicodeRelease"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\BuildTmp\Merge7z455___Win32_UnicodeRelease"
# PROP BASE Intermediate_Dir "..\BuildTmp\Merge7z455___Win32_UnicodeRelease"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\BuildTmp\Merge7z455___Win32_UnicodeRelease"
# PROP Intermediate_Dir "..\BuildTmp\Merge7z455___Win32_UnicodeRelease"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "..\..\..\7z455" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MERGE7Z310_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O1 /I "..\..\..\7z455\CPP" /D "Merge7z455_EXPORTS" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "_UNICODE" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shlwapi.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /pdb:none /machine:I386 /out:"../Build/MergeRelease/Merge7z455.dll"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib shlwapi.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /pdb:none /machine:I386 /out:"../Build/MergeUnicodeRelease/Merge7z455U.dll"

!ENDIF 

# Begin Target

# Name "Merge7z455 - Win32 Release"
# Name "Merge7z455 - Win32 Debug"
# Name "Merge7z455 - Win32 UnicodeDebug"
# Name "Merge7z455 - Win32 UnicodeRelease"
# Begin Group "Quellcodedateien"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Merge7z.def
# End Source File
# Begin Source File

SOURCE=.\Merge7z.h
# End Source File
# Begin Source File

SOURCE=.\Merge7z453.cpp
# End Source File
# Begin Source File

SOURCE=.\Merge7zCommon.cpp
# End Source File
# Begin Source File

SOURCE=.\Merge7zCommon.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\tools.cpp
# End Source File
# Begin Source File

SOURCE=.\tools.h
# End Source File
# End Group
# Begin Group "Header-Dateien"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# Begin Group "Ressourcendateien"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\..\..\7z455\CPP\7zip\Ui\Gui\FM.ico
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\7zip\UI\GUI\resource.h
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\7zip\Ui\Gui\resource.rc
# End Source File
# End Group
# Begin Group "Windows"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\7z455\CPP\Windows\DLL.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\Windows\DLL.h
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\Windows\Error.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\Windows\Error.h
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\Windows\FileIO.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\Windows\FileIO.h
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\Windows\PropVariant.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\Windows\PropVariant.h
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\Windows\PropVariantConversions.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\Windows\PropVariantConversions.h
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\Windows\ResourceString.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\Windows\ResourceString.h
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\Windows\Synchronization.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\Windows\Synchronization.h
# End Source File
# End Group
# Begin Group "Common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\7z455\CPP\Common\IntToString.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\Common\IntToString.h
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\Common\Lang.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\Common\Lang.h
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\Common\MyString.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\Common\MyString.h
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\Common\StdInStream.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\Common\StdInStream.h
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\Common\StringConvert.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\Common\StringConvert.h
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\Common\StringToInt.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\Common\StringToInt.h
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\Common\TextConfig.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\Common\TextConfig.h
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\Common\UTFConvert.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\Common\UTFConvert.h
# End Source File
# End Group
# Begin Group "7zip Common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\7z455\CPP\7zip\Ui\Common\ArchiveExtractCallback.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\7zip\UI\Common\ArchiveExtractCallback.h
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\7zip\UI\Common\ArchiveOpenCallback.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\7zip\UI\Common\ArchiveOpenCallback.h
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\7zip\UI\Common\DefaultName.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\7zip\UI\Common\DefaultName.h
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\Windows\Control\Dialog.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\Windows\Control\Dialog.h
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\7zip\UI\FileManager\ExtractCallback.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\7zip\UI\FileManager\ExtractCallback.h
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\7zip\Common\FileStreams.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\7zip\Common\FileStreams.h
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\7zip\UI\FileManager\FormatUtils.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\7zip\UI\FileManager\FormatUtils.h
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\7zip\UI\FileManager\LangUtils.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\7zip\UI\FileManager\LangUtils.h
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\7zip\UI\FileManager\MessagesDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\7zip\UI\FileManager\MessagesDialog.h
# End Source File
# Begin Source File

SOURCE=..\..\..\7z450\CPP\7zip\UI\Common\OpenArchive.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z450\CPP\7zip\UI\Common\OpenArchive.h
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\7zip\UI\FileManager\OpenCallback.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\7zip\UI\FileManager\OpenCallback.h
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\7zip\UI\FileManager\OverwriteDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\7zip\UI\FileManager\OverwriteDialog.h
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\7zip\UI\FileManager\PasswordDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\7zip\UI\FileManager\PasswordDialog.h
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\7zip\UI\FileManager\ProgramLocation.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\7zip\UI\FileManager\ProgramLocation.h
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\7zip\UI\FileManager\ProgressDialog2.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\7zip\Common\ProgressUtils.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\7zip\Common\ProgressUtils.h
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\7zip\Common\StreamUtils.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\7zip\Common\StreamUtils.h
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\7zip\UI\Common\UpdateCallback.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\7zip\UI\Common\UpdateCallback.h
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\7zip\UI\GUI\UpdateCallbackGUI.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\7zip\UI\GUI\UpdateCallbackGUI.h
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\Windows\Window.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\Windows\Window.h
# End Source File
# End Group
# Begin Group "Extract"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\7z455\CPP\7zip\Ui\Common\ExtractingFilePath.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\7zip\UI\Common\ExtractingFilePath.h
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\Windows\FileDir.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\Windows\FileDir.h
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\Windows\FileFind.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\Windows\FileFind.h
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\Windows\FileName.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\Windows\FileName.h
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\7zip\Common\FilePathAutoRename.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\7zip\Common\FilePathAutoRename.h
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\Common\MyVector.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\Common\MyVector.h
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\Common\Wildcard.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\7z455\CPP\Common\Wildcard.h
# End Source File
# End Group
# Begin Group "C"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\7z455\C\Threads.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\7z455\CPP\7zip\Ui\Gui\7zG.exe.manifest
# End Source File
# Begin Source File

SOURCE=.\revision.txt
# End Source File
# End Target
# End Project
