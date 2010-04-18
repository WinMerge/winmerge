# Microsoft Developer Studio Project File - Name="Merge7zxxx" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** 編集しないでください **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Merge7zxxx - Win32 UnicodeDebug
!MESSAGE これは有効なﾒｲｸﾌｧｲﾙではありません。 このﾌﾟﾛｼﾞｪｸﾄをﾋﾞﾙﾄﾞするためには NMAKE を使用してください。
!MESSAGE [ﾒｲｸﾌｧｲﾙのｴｸｽﾎﾟｰﾄ] ｺﾏﾝﾄﾞを使用して実行してください
!MESSAGE 
!MESSAGE NMAKE /f "Merge7zxxx.mak".
!MESSAGE 
!MESSAGE NMAKE の実行時に構成を指定できます
!MESSAGE ｺﾏﾝﾄﾞ ﾗｲﾝ上でﾏｸﾛの設定を定義します。例:
!MESSAGE 
!MESSAGE NMAKE /f "Merge7zxxx.mak" CFG="Merge7zxxx - Win32 UnicodeDebug"
!MESSAGE 
!MESSAGE 選択可能なﾋﾞﾙﾄﾞ ﾓｰﾄﾞ:
!MESSAGE 
!MESSAGE "Merge7zxxx - Win32 Release" ("Win32 (x86) Dynamic-Link Library" 用)
!MESSAGE "Merge7zxxx - Win32 Debug" ("Win32 (x86) Dynamic-Link Library" 用)
!MESSAGE "Merge7zxxx - Win32 UnicodeDebug" ("Win32 (x86) Dynamic-Link Library" 用)
!MESSAGE "Merge7zxxx - Win32 UnicodeRelease" ("Win32 (x86) Dynamic-Link Library" 用)
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Merge7zxxx - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\BuildTmp\Merge7zxxx___Win32_Release"
# PROP Intermediate_Dir "..\BuildTmp\Merge7zxxx___Win32_Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MERGE7Z310_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O1 /I "../../Src/" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "Merge7zxxx_EXPORTS" /FR /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib shlwapi.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib advapi32.lib version.lib /nologo /dll /pdb:none /machine:I386 /out:"../../Build/MergeRelease/Merge7zxxx.dll"

!ELSEIF  "$(CFG)" == "Merge7zxxx - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\BuildTmp\Merge7zxxx___Win32_Debug"
# PROP Intermediate_Dir "..\BuildTmp\Merge7zxxx___Win32_Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MERGE7Z310_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /vd0 /GX /Z7 /Od /I "../../Src/" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "Merge7zxxx_EXPORTS" /FR /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib shlwapi.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib advapi32.lib version.lib /nologo /dll /incremental:no /pdb:"../../Build/MergeDebug/Merge7zxxx.pdb" /debug /machine:I386 /out:"../../Build/MergeDebug/Merge7zxxx.dll"
# SUBTRACT LINK32 /pdb:none /map

!ELSEIF  "$(CFG)" == "Merge7zxxx - Win32 UnicodeDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\BuildTmp\Merge7zxxx___Win32_UnicodeDebug"
# PROP BASE Intermediate_Dir "..\BuildTmp\Merge7zxxx___Win32_UnicodeDebug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\BuildTmp\Merge7zxxx___Win32_UnicodeDebug"
# PROP Intermediate_Dir "..\BuildTmp\Merge7zxxx___Win32_UnicodeDebug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /vd0 /GX /Z7 /Od /I "$(SRC7Z)\7zxxx" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MERGE7Z310_EXPORTS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /vd0 /GX /Z7 /Od /I "../../Src/" /D "Merge7zxxx_EXPORTS" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "_UNICODE" /FR /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shlwapi.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /pdb:none /map /debug /machine:I386 /out:"../../Build/MergeDebug/Merge7zxxx.dll"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib shlwapi.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib advapi32.lib version.lib /nologo /dll /incremental:no /pdb:"../../Build/MergeUnicodeDebug/Merge7zxxxU.pdb" /debug /machine:I386 /out:"../../Build/MergeUnicodeDebug/Merge7zxxxU.dll"
# SUBTRACT LINK32 /pdb:none /map

!ELSEIF  "$(CFG)" == "Merge7zxxx - Win32 UnicodeRelease"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\BuildTmp\Merge7zxxx___Win32_UnicodeRelease"
# PROP BASE Intermediate_Dir "..\BuildTmp\Merge7zxxx___Win32_UnicodeRelease"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\BuildTmp\Merge7zxxx___Win32_UnicodeRelease"
# PROP Intermediate_Dir "..\BuildTmp\Merge7zxxx___Win32_UnicodeRelease"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "$(SRC7Z)\7zxxx" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MERGE7Z310_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O1 /I "../../Src/" /D "Merge7zxxx_EXPORTS" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "_UNICODE" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shlwapi.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /pdb:none /machine:I386 /out:"../../Build/MergeRelease/Merge7zxxx.dll"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib shlwapi.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib advapi32.lib version.lib /nologo /dll /pdb:none /machine:I386 /out:"../../Build/MergeUnicodeRelease/Merge7zxxxU.dll"

!ENDIF 

# Begin Target

# Name "Merge7zxxx - Win32 Release"
# Name "Merge7zxxx - Win32 Debug"
# Name "Merge7zxxx - Win32 UnicodeDebug"
# Name "Merge7zxxx - Win32 UnicodeRelease"
# Begin Source File

SOURCE=.\Merge7z.def
# End Source File
# Begin Source File

SOURCE=.\Merge7z.h
# End Source File
# Begin Source File

SOURCE=.\Merge7zXXX.cpp
# End Source File
# Begin Source File

SOURCE=.\Merge7zxxx.rc
# End Source File
# Begin Source File

SOURCE=.\PipeIPC.cpp
# End Source File
# Begin Source File

SOURCE=.\PipeIPC.h
# End Source File
# Begin Source File

SOURCE=.\revision.txt
# End Source File
# Begin Source File

SOURCE=.\tools.cpp
# End Source File
# Begin Source File

SOURCE=.\tools.h
# End Source File
# End Target
# End Project
