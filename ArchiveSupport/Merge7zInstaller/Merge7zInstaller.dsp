# Microsoft Developer Studio Project File - Name="Merge7zInstaller" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Merge7zInstaller - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "Merge7zInstaller.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "Merge7zInstaller.mak" CFG="Merge7zInstaller - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "Merge7zInstaller - Win32 Release" (basierend auf  "Win32 (x86) Application")
!MESSAGE "Merge7zInstaller - Win32 Debug" (basierend auf  "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Merge7zInstaller - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\Build\Merge7zInstaller\Release"
# PROP Intermediate_Dir "..\..\BuildTmp\Merge7zInstaller\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /pdb:none /machine:I386 /nodefaultlib
# Begin Custom Build
OutDir=.\..\..\Build\Merge7zInstaller\Release
InputPath=\svnroot2\trunk\ArchiveSupport\Build\Merge7zInstaller\Release\Merge7zInstaller.exe
SOURCE="$(InputPath)"

BuildCmds= \
	upx $(OUTDIR)\Merge7zInstaller.exe \
	cd $(OUTDIR) \
	del Merge7z.7z \
	del Merge7z.zip \
	mkdir Zip \
	FOR %%A IN (Zip\*.*) DO del %%A \
	FOR %%A IN (Zip\Codecs\*.*) DO del %%A \
	FOR %%A IN (Zip\Formats\*.*) DO del %%A \
	FOR %%A IN (Zip\Lang\*.*) DO del %%A \
	Merge7zInstaller /commit /standalone /select M M Zip \
	cd Zip \
	7za a ..\Merge7z.7z -bd -r * -mx5 \
	7za a -tzip ..\Merge7z.zip -bd -r * \
	

"$(OUTDIR)\Merge7z.7z" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(OUTDIR)\Merge7z.zip" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "Merge7zInstaller - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\Build\Merge7zInstaller\Debug"
# PROP Intermediate_Dir "..\..\BuildTmp\Merge7zInstaller\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /incremental:no /debug /machine:I386 /nodefaultlib /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Merge7zInstaller - Win32 Release"
# Name "Merge7zInstaller - Win32 Debug"
# Begin Group "Quellcodedateien"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Installer.cpp
# End Source File
# Begin Source File

SOURCE=.\Installer.rc
# End Source File
# End Group
# Begin Group "Header-Dateien"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# Begin Group "Ressourcendateien"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Files.rc2
# End Source File
# End Group
# End Target
# End Project
