; RCS ID line follows -- this is updated by CVS
; $Id$
;
;           Programmed by:  Christian Blackburn and Christian List
;                 Purpose:  The is the Inno Setup installation script for distributing our WinmMerge application.
; Tools Needed To Compile:  Inno Setup 4.18+ (http://www.jrsoftware.org/isdl.php), Inno Setup QuickStart Pack 4.18+(http://www.jrsoftware.org/isdl.php)
;                           note: the versions of Inno Setup and the QuickStart Pack should be identical to ensure proper function
;Directly Dependant Files:  Because this is an installer. It would be difficult to list and maintain each of the files referenced
;                           throughout the script in the header.  If you search this plain text script for a particular file in our CVS and it
;                           doesn't appear then this script is not directly dependant on that file.
;Compilation Instructions:  1.  Open this file in Inno Setup or ISTool
;                           2.  Make sure Compression=7Zip/Ultra, InternalCompressLevel=Ultra, and SolidCompression=True these values are lowered during
;                               development to speed up compilation, however at release we want the intaller to be as strong as possible.
;                           3.  Compile the script:
;                                   -From Inno Setup "Click "Build" --> "Compile"
;                                   -From ISTool Click "Project" --> "Compile Setup"
;                           4.  The compiled installer will appear in the \InnoSetup\Output\ directory at currently should be around 1.5MBs in size.
;
;Installer To Do List:
; #  Create a task for modifying TortoiseCVS, also we need to uninstall the modification too.
; #  Automatically detect and configure WinMerge to work with Visual Source Safe
; #  We need to add support for Tortoises' SubVersion program mimicing our support settings for TortoiseCVS if possible
; #  Make the Install7ZipDll() Function automatically work with future versions of Merge7zDLL
; #  Test and distribute the files contained within "MergePlugins (From 2140).7z" or in the folder MergePlugins of "WinMerge2140-exe.7z"
;    Also take a look at this folder and see if there are any more we should be including WinMerge\Plugins
; #  Bundle 7-Zip with WinMerge or provide on the fly download capability.
; #  Automatically detect the user's locale, select, and install that language and the registry settings that would make that language be
;     the language on startup.
; #  Evaluate current default settings in WinMerge and bug those that don't make sense
; #  Create instructions and a sample language file using the Inno Setup Translator Tool (http://www2.arnes.si/~sopjsimo/translator.html)
; #  Add WinMerge is running would you like to close it now support with programmatic termination
;     -Note: I tried this with ISX and the extensions are innadequate I will have to use AutoIt 3.X instead
; #  Add WinMerge to the user's path so they can execute comparison's from a Dos Prompt (Cmd.exe/Command.exe)
; #   Make an uninstall icon and use it: UninstallIconFile=..\src\res\Merge.ico
; #  Make a Floppy Disk /Low Bandwidth Edition of the WinMerge Installer that doesn't include outdated (3.11, 3.12) 7-Zip Support or the Language files
;     If the user requires any of these we'll download it on the fly.  (maybe that should be the default behavior from the get go?)
; #  We need to look in the registry and (HKLM\...\Uninstall\Winmerge_is1) determine what the previous start
;     menu location was so that we can remove it if the user chooses to install to a different location.
; #  Determine whether NT 3.51 with a 3.0 or higher version of IE can run our application I don't want the system requirements
;     in \Docs\Read Me.RTF to be inaccurate.
; #  Create two info pages during installation one for our Contributors and a second one for our Read Me file.
;     If this isn't possible then we'll need to use ISPP and somehow programatically combine the two RTF files prior to compilation.
; #  While uninstalling prompt the user as to whether or not they'd like to remove their WinMerge preferences too?
; #  Be dilegent about getting a higher resolution copy of the Users's Guide.ico source art from somebody
; #  If the user happens to have Syn Text Editor installed then we'll make that the default text editor rather than notepad.
; #  Set the order of icons within the start menu (we'll have to use a registry hack since Inno Setup doesn't yet have this level of granularity).
; #  Create a switch for the installer to unzip all of the included binaries as if it were a zip file.
; #  Perry wanted a Select All button to appear on the components page.  Let's just ask Jordan Russell or Martijn Laan if they'll do it :).
; #  Provide the option to or not to assign the Ctrl+Alt+M accelerator to WinMerge.
; #  Using the registry set the order our icons appear within their group in the start menu.:
;      1.  WinMerge
;      2.  Read Me
;      3.  Users's Guide
;      4.  WinMerge on the Web
;      5.  Uninstall WinMerge
; #  We need to determine if our application can cooperate with WinCVS and if so how
; #  We need to unregister the ShellExtension Dll if the user doesn't want it, during installation
; #  When Explorer.exe is restarted we should record what windows were present before hand and restore them afterwards.
; #  Use Syn Text Editor as our external editor if the user has it installed
; #  Use Aedix as our external editor if the user has it installed
; #  WinMerge requires an SHWLAPI.dll (which is IE 5 or better right?)
; #  RCLLocalizationHelper.dll and UnpackDFM.dll both require ADVAPI32.dll, we'll have to bundle IE or at least display a warning :).
; #  Add the Windows Scripting Host if the user chooses the plugins (*.SCT)
; #  Add Delphi 4 Detection for UnpackDFM.dll, making it both selected and visible
; #  Make the Integrate with TortoiseCVS grey rather than invisible if the user doesn't have it installed
; #  Make RCLLocalizationHelper.dll a non-default separate option from the rest of the plugins
; #  Figure out advapi.dll and shlwapi.dll dependencies
; #  Only display TortoiseCVS option if the user has it installed
; #  Seier will need to apply this {#AppVersion} directive to all localizations:
;        English.SetupAppTitle=Setup - WinMerge {#AppVersion}
; #  We need to ask those that have the RCLLocalization.dll in their plugins folder if they actually want it, their answer will need to be stored in the registry

#define AppVersion GetFileVersion(SourcePath + "\..\Build\MergeRelease\WinMerge.exe")
#define FriendlyAppVersion Copy(GetFileVersion(SourcePath + "\..\Build\MergeRelease\WinMerge.exe"), 1, 5)


[Setup]
AppName=WinMerge
AppVerName=WinMerge {#AppVersion}
AppPublisher=Thingamahoochie Software
AppPublisherURL=http://WinMerge.org/
AppSupportURL=http://WinMerge.org/
AppUpdatesURL=http://WinMerge.org/

;This is in case an older version of the installer happened to be
DirExistsWarning=No

;This requires IS Pack 4.18(full install).
AppVersion={#AppVersion}

DefaultDirName={pf}\WinMerge
DefaultGroupName=WinMerge
DisableStartupPrompt=yes
AllowNoIcons=yes
LicenseFile=..\src\COPYING
InfoBeforeFile=..\Docs\Users\Read Me.rtf
OutputBaseFilename=WinMerge-{#AppVersion}-Setup

;This must be admin to install C++ Runtimes
PrivilegesRequired=admin

UninstallDisplayIcon={app}\{code:ExeName}

;File Version Info
VersionInfoVersion={#AppVersion}

;Artwork References
WizardImageFile=Art\Large Logo.rle
WizardSmallImageFile=Art\Small Logo.rle
WizardImageStretch=No

SetupIconFile=..\src\res\Merge.ico

;The uninstall icon shouldn't match the WinMerge icon, because it would look confusing in the start menu.
;  So I've remmed this until someone (probably me [Seier Blackburn]) creates a decent WinMerge specific uninstall icon
;UninstallIconFile=..\src\res\Merge.ico

;Compression Parameters
;Please note while Compression=lzma/ultra and InternalCompressLevel=Ultra are better than max
;they also require 320 MB RAM for compression, if you're system has more than than in RAM then by all
;means set it to ultra before compilation
Compression=LZMA/Ultra
InternalCompressLevel=Ultra
SolidCompression=True

[Messages]
English.FinishedLabel=Setup has finished installing [Name] on your computer.
English.SetupAppTitle=Setup - WinMerge {#AppVersion}

Dutch.BeveledLabel=Nederlands
German.BeveledLabel=Deutsch
;Spanish.BeveledLabel=Español
French.BeveledLabel=Français
Italian.BeveledLabel=Italiano
Norwegian.BeveledLabel=Norsk


[Languages]
;Inno Setup's Native Language
Name: English; MessagesFile: compiler:Default.isl

;Localizations:
Name: Bulgarian; MessagesFile: compiler:Languages\Bulgarian-4.1.8.isl
;Name: Catalan; MessagesFile: "compiler:Languages\Catalan-4-4.1.4.isl"
Name: Chinese_Simplified; MessagesFile: compiler:Languages\ChineseSimp-5-4.1.8.isl
Name: Chinese_Traditional; MessagesFile: compiler:Languages\ChineseTrad-3-4.1.8.isl
Name: Czech; MessagesFile: compiler:Languages\Czech-5-4.1.8.isl
Name: Danish; MessagesFile: compiler:Languages\Danish-4-4.1.8.isl
Name: Dutch; MessagesFile: compiler:Languages\Dutch.isl
Name: French; MessagesFile: compiler:Languages\French-15-4.1.8.isl
Name: German; MessagesFile: compiler:Languages\German.isl
Name: Italian; MessagesFile: compiler:Languages\Italian-14-4.1.8.isl
Name: Korean; MessagesFile: compiler:Languages\Korean-5-4.1.8.isl
Name: Norwegian; MessagesFile: compiler:Languages\Norwegian-4.1.8.isl
Name: Polish; MessagesFile: compiler:Languages\Polish-1-4.1.8.isl
;Name: Portuguese; MessagesFile: "compiler:Languages\BrazilianPortuguese-7-4.1.4.isl"
;Name: Russian; MessagesFile: "compiler:Languages\Russian-19-4.1.4.isl"
;Name: Slovak; MessagesFile: "compiler:Languages\Slovak-4-4.0.5.isl"
;Name: Spanish; MessagesFile: "compiler:Languages\SpanishStd-1-4.1.4.isl"
Name: Swedish; MessagesFile: compiler:Languages\Swedish-8-4.1.8.isl


[Tasks]
Name: ShellExtension; Description: &Enable Explorer context menu integration; GroupDescription: Optional Features:
Name: TortoiseCVS; Description: Integrate with &TortoiseCVS; GroupDescription: Optional Features:; Check: TortoiseCVSInstalled
Name: Delphi4Viewer; Description: Delphi &4 Viewer (Binary --> Plain Text); GroupDescription: Optional Features:; Flags: unchecked
Name: desktopicon; Description: Create a &Desktop Icon; GroupDescription: Additional Icons:; Flags: unchecked
Name: quicklaunchicon; Description: Create a &Quick Launch Icon; GroupDescription: Additional Icons:
Name: CustomFolderIcon; Description: "&Customize the folder Icon for ""{app}"""; GroupDescription: Additional Icons:



[Components]
Name: main; Description: WinMerge Core Files; Types: full compact custom; Flags: fixed
Name: docs; Description: User's Guide; Types: full
Name: filters; Description: Filters; Types: full
Name: Plugins; Description: Plug-Ins (enhance core behavior); Types: full

;Non-English Languages are no longer a default part of a normal installation.  If a user selected a language as part of their last installation
;that language will be selected automatically during subsequent installs.
Name: Catalan_Language; Description: Catalan menus and dialogs; Flags: disablenouninstallwarning
Name: Portuguese_Language; Description: Portuguese (Brazilian) menus and dialogs; Flags: disablenouninstallwarning
Name: Russian_Language; Description: Russian menus and dialogs; Flags: disablenouninstallwarning
Name: Slovak_Language; Description: Slovak menus and dialogs; Flags: disablenouninstallwarning
Name: Spanish_Language; Description: Spanish menus and dialogs; Flags: disablenouninstallwarning

[InstallDelete]
;Experimental Versions 2.1.5.10 - WinMerge.2.1.5.13 shipped with the default behavior of creating a folder by the same name
; just in case the user was a bozo and left behind that folder we'll go ahead and check for it and delete them as they occurr

;Deletes the following folder and all its contents during installation
Type: filesandordirs; Name: {app}\WinMerge.2.1.5.0-exe

;Deletes the following folder and all its contents during installation
Type: filesandordirs; Name: {app}\WinMerge.2.1.5.1-exe

;Deletes the following folder and all its contents during installation
Type: filesandordirs; Name: {app}\WinMerge.2.1.5.2-exe

;Deletes the following folder and all its contents during installation
Type: filesandordirs; Name: {app}\WinMerge.2.1.5.3-exe

;Deletes the following folder and all its contents during installation
Type: filesandordirs; Name: {app}\WinMerge.2.1.5.4-exe

;Deletes the following folder and all its contents during installation
Type: filesandordirs; Name: {app}\WinMerge.2.1.5.5-exe

;Deletes the following folder and all its contents during installation
Type: filesandordirs; Name: {app}\WinMerge.2.1.5.6-exe

;Deletes the following folder and all its contents during installation
Type: filesandordirs; Name: {app}\WinMerge.2.1.5.7-exe

;Deletes the following folder and all its contents during installation
Type: filesandordirs; Name: {app}\WinMerge.2.1.5.8-exe

;Deletes the following folder and all its contents during installation
Type: filesandordirs; Name: {app}\WinMerge.2.1.5.9-exe

;Deletes the following folder and all its contents during installation
Type: filesandordirs; Name: {app}\WinMerge.2.1.5.10-exe

;Deletes the following folder and all its contents during installation
Type: filesandordirs; Name: {app}\WinMerge.2.1.5.11-exe

;Deletes the following folder and all its contents during installation
Type: filesandordirs; Name: {app}\WinMerge.2.1.5.12-exe

;Deletes the following folder and all its contents during installation
Type: filesandordirs; Name: {app}\WinMerge.2.1.5.13-exe

;Deletes the following folder and all its contents during installation
Type: filesandordirs; Name: {app}\WinMerge2140-exe

;Deletes the following folder and all its contents during installation
Type: filesandordirs; Name: {app}\WinMerge2154-exe

;Deletes the following folder and all its contents during installation
Type: filesandordirs; Name: {app}\WinMerge2158-exe

; Diff.txt is a file left over from previous versions of WinMerge (before version 2.0), we just delete it to be nice.
Type: files; Name: {app}\Diff.txt

; A few users might have some intermediate Chinese translations on their machines (from version 2.0.0.2),
;we just delete those to be nice.
Type: files; Name: {app}\MergeChineseSimplifiedGB2312.lang
Type: files; Name: {app}\MergeChineseTraditionalBIG5.lang

;All of these files are removed so if the user upgrades their operating system or changes their language selections only the
;necessary files will be left in the installation folder
;Another reason these files might be strays is if a user extracted one of the experimental builds such as:
;WinMerge.{#AppVersion}-exe.7z.
Name: {app}\WinMerge.exe; Type: files
Name: {app}\WinMergeU.exe; Type: files

Name: {app}\Merge7z311.dll; Type: files
Name: {app}\Merge7z311U.dll; Type: files

Name: {app}\Merge7z312.dll; Type: files
Name: {app}\Merge7z312U.dll; Type: files

Name: {app}\Merge7z313.dll; Type: files
Name: {app}\Merge7z313U.dll; Type: files

;This won't work, because the file has to be unregistered first.
;Name: {app}\ShellExtension.dll; Type: files

Name: {app}\MergeBrazilian.lang; Type: files
Name: {app}\MergeCatalan.lang; Type: files
Name: {app}\MergeChineseSimplified.lang; Type: files
Name: {app}\MergeChineseTraditional.lang; Type: files
Name: {app}\MergeCzech.lang; Type: files
Name: {app}\MergeDanish.lang; Type: files
Name: {app}\MergeDutch.lang; Type: files
Name: {app}\MergeFrench.lang; Type: files
Name: {app}\MergeGerman.lang; Type: files
Name: {app}\MergeItalian.lang; Type: files
Name: {app}\MergeKorean.lang; Type: files
Name: {app}\MergeNorwegian.lang; Type: files
Name: {app}\MergePolish.lang; Type: files
Name: {app}\MergeSlovak.lang; Type: files
Name: {app}\MergeSpanish.lang; Type: files
Name: {app}\MergeRussian.lang; Type: files
Name: {app}\MergeSwedish.lang; Type: files

Name: {app}\MergePlugins\list.txt; Type: files

Name: {app}\Installer Helper.exe; Type: files








Name: {app}\Read Me.rtf; Type: files
Name: {app}\Contributors.rtf; Type: files

;Removes the previous start menu items and group in case the user chooses to install to a new start menu location next time
Name: {commonstartmenu}\Programs\WinMerge\WinMerge.lnk; Type: files
Name: {commonstartmenu}\Programs\WinMerge\Uninstall WinMerge.lnk; Type: files
Name: {commonstartmenu}\Programs\WinMerge\WinMerge on the Web.lnk; Type: files
Name: {commonstartmenu}\Programs\WinMerge\User's Guide.lnk; Type: files
Name: {commonstartmenu}\Programs\WinMerge\Read Me.lnk; Type: files
Name: {commonstartmenu}\Programs\WinMerge; Type: dirifempty

Name: {group}\WinMerge.lnk; Type: files
Name: {group}\Uninstall WinMerge.lnk; Type: files
Name: {group}\WinMerge on the Web.lnk; Type: files
Name: {group}\Read Me.lnk; Type: files
Name: {group}\User's Guide.lnk; Type: files
Name: {group}; Type: dirifempty

;This removes the quick launch icon in case the user chooses not to install it after previously having it installed
Name: {userappdata}\Microsoft\Internet Explorer\Quick Launch\WinMerge.lnk; Type: files

;This removes the desktop icon in case the user chooses not to install it after previously having it installed
Name: {userdesktop}\WinMerge.lnk; Type: files

Name: {app}\Docs; Type: filesandordirs

;This is removed at installation so that if the user doesn't choose to customize his C:\Program Files\WinMerge icon the current customization will be
;removed
Name: {app}\Desktop.ini; Type: files

[Dirs]
;The always uninstall flag tells the uninstaller to remove the folder if it's empty regardless of whether or not it existed prior to the installation
Name: {app}; Flags: uninsalwaysuninstall

[Files]
;The MinVersion forces Inno Setup to only copy the following file if the user is running a WinNT platform system
Source: ..\Build\MergeUnicodeRelease\WinMergeU.exe; DestDir: {app}; Flags: ignoreversion; MinVersion: 0, 4; Components: main

;The MinVersion forces Inno Setup to only copy the following file if the user is running Win9X platform system
Source: ..\Build\MergeRelease\WinMerge.exe; DestDir: {app}; Flags: ignoreversion; MinVersion: 4, 0; Components: main

;Installs the ComCtl32.dll update on any system where its DLLs are more recent
Source: Runtimes\50comupd.exe; DestDir: {tmp}; Flags: DeleteAfterInstall; Check: InstallComCtlUpdate

;Adds Seier's Explorer Restart and Shell Extension Deletion Utility
Source: Setup Helper.exe; DestDir: {app} 

; begin VC system files
Source: Runtimes\Atla.dll; DestDir: {sys}; Flags: restartreplace uninsneveruninstall regserver sharedfile; MinVersion: 4, 0
Source: Runtimes\Atlu.dll; DestDir: {sys}; Flags: restartreplace uninsneveruninstall regserver sharedfile; MinVersion: 0, 4
Source: Runtimes\mfc42.dll; DestDir: {sys}; Flags: restartreplace uninsneveruninstall regserver sharedfile
Source: Runtimes\mfc42u.dll; DestDir: {sys}; Flags: restartreplace uninsneveruninstall regserver sharedfile; MinVersion: 0, 4
Source: Runtimes\msvcrt.dll; DestDir: {sys}; Flags: restartreplace uninsneveruninstall sharedfile
Source: Runtimes\OleAut32.dll; DestDir: {sys}; Flags: restartreplace uninsneveruninstall regserver sharedfile
; end VC system files

;Installs PSAPI.dll on Windows NT 4.0 only!
Source: Runtimes\PSAPi.dll; DestDir: {sys}; Flags: restartreplace sharedfile; MinVersion: 0, 4; OnlyBelowVersion: 0, 4

Source: ..\ShellExtension\ShellExtension.dll; DestDir: {app}; Flags: regserver; BeforeInstall: ShellExtension(/I); Tasks: ShellExtension
Source: ..\ShellExtension\ShellExtension.dll; DestDir: {app}; BeforeInstall: ShellExtension(/U); Check: TaskDisabled(ShellExtension)

;Please do not reorder the 7z Dlls by version they compress better ordered by platform and then by version
Source: ..\Build\MergeUnicodeRelease\Merge7z313U.dll; DestDir: {app}; Flags: ignoreversion; MinVersion: 0, 4; Check: Install7ZipDll(313)
Source: ..\Build\MergeUnicodeRelease\Merge7z312U.dll; DestDir: {app}; Flags: ignoreversion; MinVersion: 0, 4; Check: Install7ZipDll(312)
Source: ..\Build\MergeUnicodeRelease\Merge7z311U.dll; DestDir: {app}; Flags: ignoreversion; MinVersion: 0, 4; Check: Install7ZipDll(311)

Source: ..\Build\MergeRelease\Merge7z313.dll; DestDir: {app}; Flags: ignoreversion; MinVersion: 4, 0; Check: Install7ZipDll(313)
Source: ..\Build\MergeRelease\Merge7z312.dll; DestDir: {app}; Flags: ignoreversion; MinVersion: 4, 0; Check: Install7ZipDll(312)
Source: ..\Build\MergeRelease\Merge7z311.dll; DestDir: {app}; Flags: ignoreversion; MinVersion: 4, 0; Check: Install7ZipDll(311)

Source: ..\Plugins\dlls\UnpackDFM.dll; DestDir: {app}\MergePlugins; Tasks: Delphi4Viewer

Source: ..\Src\Languages\DLL\MergeBrazilian.lang; DestDir: {app}; Components: Portuguese_Language
Source: ..\Src\Languages\DLL\MergeCatalan.lang; DestDir: {app}; Components: Catalan_Language
Source: ..\Src\Languages\DLL\MergeChineseSimplified.lang; DestDir: {app}; Languages: Chinese_Simplified
Source: ..\Src\Languages\DLL\MergeChineseTraditional.lang; DestDir: {app}; Languages: Chinese_Traditional
Source: ..\Src\Languages\DLL\MergeCzech.lang; DestDir: {app}; Languages: Czech
Source: ..\Src\Languages\DLL\MergeDanish.lang; DestDir: {app}; Languages: Danish
Source: ..\Src\Languages\DLL\MergeDutch.lang; DestDir: {app}; Languages: Dutch
Source: ..\Src\Languages\DLL\MergeFrench.lang; DestDir: {app}; Languages: French
Source: ..\Src\Languages\DLL\MergeGerman.lang; DestDir: {app}; Languages: German
Source: ..\Src\Languages\DLL\MergeItalian.lang; DestDir: {app}; Languages: Italian
Source: ..\Src\Languages\DLL\MergeKorean.lang; DestDir: {app}; Languages: Korean
Source: ..\Src\Languages\DLL\MergeNorwegian.lang; DestDir: {app}; Languages: Norwegian
Source: ..\Src\Languages\DLL\MergePolish.lang; DestDir: {app}; Languages: Polish
Source: ..\Src\Languages\DLL\MergeRussian.lang; DestDir: {app}; Components: Russian_Language
Source: ..\Src\Languages\DLL\MergeSlovak.lang; DestDir: {app}; Components: Slovak_Language
Source: ..\Src\Languages\DLL\MergeSpanish.lang; DestDir: {app}; Components: Spanish_Language
Source: ..\Src\Languages\DLL\MergeSwedish.lang; DestDir: {app}; Languages: Swedish


Source: ..\Docs\Users\Guide\*.*; DestDir: {app}\Docs\User's Guide\; Flags: sortfilesbyextension; Components: docs
Source: ..\Docs\Users\Guide\Art\*.*; DestDir: {app}\Docs\User's Guide\Art; Flags: sortfilesbyextension; Components: docs
Source: ..\Filters\*.*; DestDir: {app}\Filters; Flags: sortfilesbyextension; Components: filters

;Documentation
Source: ..\Docs\Users\Read Me.rtf; DestDir: {app}\Docs; Components: main

Source: ..\Docs\Users\Contributors.rtf; DestDir: {app}\Docs; Components: main

;Please note I removed CompareTimeStamp since neither old nor new versions of these files contain version info
Source: ..\Plugins\dlls\editor addin.sct; DestDir: {app}\MergePlugins; Components: Plugins
Source: ..\Plugins\dlls\insert datetime.sct; DestDir: {app}\MergePlugins; Components: Plugins
Source: ..\Plugins\dlls\list.txt; DestDir: {app}\Docs; DestName: Plugins.txt; Components: Plugins


[INI]
Filename: {app}\WinMerge.url; Section: InternetShortcut; Key: URL; String: http://WinMerge.org/
Filename: {app}\Desktop.ini; Section: .ShellClassInfo; Key: IconFile; String: {app}\{code:ExeName}; Tasks: CustomFolderIcon
Filename: {app}\Desktop.ini; Section: .ShellClassInfo; Key: IconIndex; String: 1; Tasks: CustomFolderIcon


[Icons]
;Start Menu Icons
Name: {group}\WinMerge; Filename: {app}\{code:ExeName}; HotKey: Ctrl+Alt+M
Name: {group}\Read Me; Filename: {app}\Docs\Read Me.rtf; IconFileName: {win}\NOTEPAD.EXE
Name: {group}\User's Guide; Filename: {app}\Docs\User's Guide\Index.html; IconFileName: {app}\Docs\User's Guide\Art\User's Guide.ico; Components: docs
Name: {group}\WinMerge on the Web; Filename: {app}\WinMerge.url
Name: {group}\Uninstall WinMerge; Filename: {uninstallexe}

;Desktop Icon
Name: {userdesktop}\WinMerge; Filename: {app}\{code:ExeName}; Tasks: desktopicon

;Quick Launch Icon
Name: {userappdata}\Microsoft\Internet Explorer\Quick Launch\WinMerge; Filename: {app}\{code:ExeName}; Tasks: quicklaunchicon


[Registry]
Root: HKCU; Subkey: Software\Thingamahoochie; Flags: uninsdeletekeyifempty
Root: HKCU; Subkey: Software\Thingamahoochie\WinMerge; Flags: uninsdeletekey
Root: HKCU; Subkey: Software\Thingamahoochie\WinMerge\Font

; Set the default font to Courier New size 12
Root: HKCU; Subkey: Software\Thingamahoochie\WinMerge\Font; ValueType: string; ValueName: FaceName; ValueData: Courier New
Root: HKCU; Subkey: Software\Thingamahoochie\WinMerge\Font; ValueType: dword; ValueName: Specified; ValueData: 1
Root: HKCU; Subkey: Software\Thingamahoochie\WinMerge\Font; ValueType: dword; ValueName: Height; ValueData: $fffffff0
Root: HKCU; Subkey: Software\Thingamahoochie\WinMerge\Font; ValueType: dword; ValueName: Width; ValueData: 0
Root: HKCU; Subkey: Software\Thingamahoochie\WinMerge\Font; ValueType: dword; ValueName: Escapement; ValueData: 0
Root: HKCU; Subkey: Software\Thingamahoochie\WinMerge\Font; ValueType: dword; ValueName: Orientation; ValueData: 0
Root: HKCU; Subkey: Software\Thingamahoochie\WinMerge\Font; ValueType: dword; ValueName: Weight; ValueData: $190
Root: HKCU; Subkey: Software\Thingamahoochie\WinMerge\Font; ValueType: dword; ValueName: Italic; ValueData: 0
Root: HKCU; Subkey: Software\Thingamahoochie\WinMerge\Font; ValueType: dword; ValueName: Underline; ValueData: 0
Root: HKCU; Subkey: Software\Thingamahoochie\WinMerge\Font; ValueType: dword; ValueName: StrikeOut; ValueData: 0
Root: HKCU; Subkey: Software\Thingamahoochie\WinMerge\Font; ValueType: dword; ValueName: CharSet; ValueData: 0
Root: HKCU; Subkey: Software\Thingamahoochie\WinMerge\Font; ValueType: dword; ValueName: OutPrecision; ValueData: 3
Root: HKCU; Subkey: Software\Thingamahoochie\WinMerge\Font; ValueType: dword; ValueName: ClipPrecision; ValueData: 2
Root: HKCU; Subkey: Software\Thingamahoochie\WinMerge\Font; ValueType: dword; ValueName: Quality; ValueData: 1
Root: HKCU; Subkey: Software\Thingamahoochie\WinMerge\Font; ValueType: dword; ValueName: PitchAndFamily; ValueData: $31

; delete obsolete values
;In Inno Setup Version 4.18 ValueData couldn't be null and compile,
;if this is fixed in a later version feel free to remove the parameter
Root: HKCU; Subkey: Software\Thingamahoochie\WinMerge\Settings; ValueType: none; ValueName: LeftMax; Flags: deletevalue
Root: HKCU; Subkey: Software\Thingamahoochie\WinMerge\Settings; ValueType: none; ValueName: DirViewMax; Flags: deletevalue

Root: HKCR; SubKey: Directory\Shell\WinMerge\command; ValueType: none; Flags: deletekey noerror
Root: HKCR; SubKey: Directory\Shell\WinMerge; ValueType: none; Flags: deletekey noerror

;Adds "Start Menu" --> "Run" Support for WinMerge
Root: HKLM; Subkey: Software\Microsoft\Windows\CurrentVersion\App Paths\WinMerge.exe; ValueType: none; Flags: uninsdeletekey
Root: HKLM; Subkey: Software\Microsoft\Windows\CurrentVersion\App Paths\WinMergeU.exe; ValueType: none; Flags: uninsdeletekey
Root: HKLM; SubKey: SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\WinMerge.exe; ValueType: string; ValueName: ; ValueData: {app}\{code:ExeName}
Root: HKLM; SubKey: SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\WinMergeU.exe; ValueType: string; ValueName: ; ValueData: {app}\{code:ExeName}

;Registry Keys for use by ShellExtension.dll
Root: HKCU; SubKey: Software\Thingamahoochie\WinMerge; ValueType: string; ValueName: Executable; ValueData: {app}\{code:ExeName}

;Enables or disables the Context Menu preference based on what the user selects during install
;Initially the Context menu is explicitly disabled:
Root: HKCU; SubKey: Software\Thingamahoochie\WinMerge; ValueType: dword; ValueName: ContextMenuEnabled; ValueData: 0

;If the user chose to use the context menu then we re-enable it.  This is necessary so it'll turn on and off not just on.
Root: HKCU; SubKey: Software\Thingamahoochie\WinMerge; ValueType: dword; ValueName: ContextMenuEnabled; ValueData: 1; Tasks: ShellExtension

;If WinMerge.exe is installed then we'll automatically configure WinMerge as the differencing application
Root: HKCU; SubKey: Software\TortoiseCVS; ValueType: string; ValueName: External Diff Application; ValueData: {app}\{code:ExeName}; Flags: uninsdeletevalue; Tasks: TortoiseCVS
Root: HKCU; SubKey: Software\TortoiseCVS; ValueType: dword; ValueName: DiffAsUnicode; ValueData: $00000001; Flags: uninsdeletevalue; Tasks: TortoiseCVS

;Tells TortoiseCVS to use WinMerge as its differencing application (this happens whether or not Tortoise is current installed, that way
;if it is installed at a later date this will automatically support it)
Root: HKCU; SubKey: Software\TortoiseCVS; ValueType: string; ValueName: External Merge Application; ValueData: {app}\{code:ExeName}; Flags: uninsdeletevalue; Tasks: TortoiseCVS
Root: HKCU; SubKey: Software\TortoiseCVS; ValueType: dword; ValueName: MergeAsUnicode; ValueData: $00000001; Flags: uninsdeletevalue; Tasks: TortoiseCVS


[Run]
Filename: {sys}\Attrib.exe; Parameters: """{app}"" +S"; Flags: nowait runhidden; Tasks: CustomFolderIcon
Filename: {sys}\Attrib.exe; Parameters: """{app}\desktop.ini"" +H"; Flags: nowait runhidden; Tasks: CustomFolderIcon

;Installs the Microsoft Common Controls Update
Filename: {tmp}\50comupd.exe; Parameters: /r:n /q:1; StatusMsg: Updating the System's Common Controls; Check: InstallComCtlUpdate

;This will no longer appear unless the user chose to make a start menu group in the first place
Filename: {win}\Explorer.exe; Description: &View the WinMerge Start Menu Folder; Parameters: """{group}"""; Flags: waituntilidle postinstall skipifsilent; Check: GroupCreated

Filename: {app}\{code:ExeName}; Description: &Launch WinMerge; Flags: nowait postinstall skipifsilent runmaximized


[UninstallDelete]
Type: files; Name: {app}\WinMerge.url
Type: files; Name: {app}\Desktop.ini; Tasks: CustomFolderIcon
Type: dirifempty; Name: {app}


[UninstallRun]
;Restarts and deletes the shellextension file so we don't have to restart
Filename: {app}\Setup Helper.exe; Parameters: /U; StatusMsg: Removing ShellExtension.dll

Filename: {sys}\Attrib.exe; Parameters: """{app}"" -S"; Flags: runhidden; Tasks: CustomFolderIcon


[Code]
Var
    {Stores the version of 7-Zip Installed}
    int7Zip_Version: Integer;

    {Determines two things whether or not ComCtrl is needed and whether or not we've already checked}
    intComCtlNeeded: Integer;

{Determines whether or not the user chose to create a start menu}
Function GroupCreated(): boolean;
Var
    {Stores the path of the start menu group Inno Setup may have created}
    strGroup_Path: string;
Begin
    {Saves the path that Inno Setup intended to create the start menu group at}
    strGroup_Path := ExpandConstant('{group}');

    {If the start menu path isn't blank then..}
    if strGroup_Path <> '' Then
        Begin
            {If the user choose to create the start menu then this folder will exist.
            If the folder exists then GroupCreated = True otherwise it does not.}
            Result := DirExists(strGroup_Path)
        end
    else
        {Since the start menu path is null, we know that the user chose not to create a
        start menu group (note in Inno Setup 4.18 this didn't yet work, but I'm sure it will in the future}
        Result := False;

     {Debug

    If DirExists(strGroup_Path) = True Then
        Msgbox('The group "' + ExpandConstant('group') + '" was found', mbInformation, mb_ok)
    Else
        Msgbox('The group "' + ExpandConstant('group') + '" doesn''t exist.', mbInformation, mb_ok); }
End;


{Returns the appropriate name of the .EXE being installed}
Function ExeName(Unused: string): string;
Begin

  If UsingWinNT() = True Then
	 Result := 'WinMergeU.exe'
  Else
    Result := 'WinMerge.exe';
End;

Function FixVersion(strInput: string): string;
{Returns a version with four segments A.B.C.D}
Var
  {Stores the number of periods found within the version string}
  intPeriods: integer;

  {Creates a counter}
  i: integer;

  {Generates the string to be returned to the user}
  strVersion: string;
Begin

  {Creates a copy of the input string before we tear it apart}
  strVersion := strInput;

  {Until strInput is empty do..}
  While Length(strInput) > 0 do
    Begin
      {if the first character of the input string is a period then}
      If Copy(strInput, 1, 1) = '.' Then

        {Incriment the number of periods found}
        intPeriods := intPeriods + 1;

      {Remove the first character from the Input string}
      strInput := Copy(strINput, 2, length(strINput));
    End;

  {For every period shy of 3 do..}
  For i := 1 to 3 - intperiods do

    {Add a '.0' to the version string}
    strVersion := strVersion + '.0';

  {Returns the Version string with the correct number of segments}
  Result := strVersion;

End;

Function RemoveLeadingZeros(strInput: string): string;
{Removes the leading zeros if any from a numeric string}
Begin

  {While the first character of the string is a zero}
  While Copy(strInput, 1, 1) = '0' Do
    begin

    {Removes one leading zero from the input string}
      strInput := Copy(strInput, 2, Length(strINput));
    end;

  {Returns the formatted string}
  Result := strInput;

End;


{Returns whether or not the version string detected is meets the version number requirement}
Function VersionAtLeast(strVersion_Installed: string; intMajor: integer; intMinor: integer; intRevision: integer; intBuild: integer): boolean;
Var

  {Stores the Major of the Version installed (X.0.0.0)}
  intMajor_Installed: Integer;

  {Stores the Minor of the Version installed (1.X.0.0)}
  intMinor_Installed: Integer;

  {Stores the Revision of the Version installed (1.0.X.0)}
  intRevision_Installed: Integer;

  {Stores the Revision of the Version installed (1.0.0.X)}
  intBuild_Installed: Integer;

  {Stores the last valid character of a particular segment (Major, Minor, Revision) of the Version string}
  intEnd_Pos: Integer;

begin
  {Debug
  Msgbox('The version installed is ' + strVersion_Installed + ' and the required version is ' + IntToStr(intMajor) + '.' + IntToStr(intMinor) + '.' + IntToStr(intRevision) + '.' + IntToStr(intBuild), mbINformation, mb_OK)
        }

  {Makes sure the version string contains four numberic segments 5.2 ---> 5.2.0.0}
  strVersion_Installed := FixVersion(strVersion_Installed);

  {If the version number is empty then quit the function}
  if strVersion_Installed = '' Then
    begin
      Result := False;

      {Stops analyzing the version installed and returns that the version installed was inadequate}
      exit;
    end;

  {Starts detecting the Major value of the Version Installed}

  {Sets the end position equal to one character before the first period in the version number}
  intEnd_Pos := Pos('.', strVersion_Installed) -1

  {Sets the major version equal to all character before the first period }
  intMajor_installed := StrToIntDef(RemoveLeadingZeros(Copy(strVersion_Installed, 1, intEnd_Pos)), 0);

  {Debug
  msgbox('The Major version installed is ' + IntToStr(intMajor_installed) + ' and the required Major is ' + IntToStr(intMajor) + '.', mbInformation, MB_OK)
        }

  {If the Major Version Installed is greater than the required value then...}
  if intMajor_Installed > intMajor Then
    begin
      {Returns that the version number was adequate, since it actually exceeded the requirement}
      Result := True;

        {Debug
        msgbox(IntToStr(intMajor_installed) + ' > ' +  IntToStr(intMajor), mbInformation, MB_OK)
        }

      {Stops analyzing the version number since we already know it met the requirement}
      exit;
    end;

  {If the Major version installed is less than the requirement then...}
  If intMajor_Installed < intMajor Then
    begin
      {Debug
       msgbox(IntToStr(intMajor_installed) + ' < ' +  IntToStr(intMajor), mbInformation, MB_OK)
       }

      Result := False;

      {Stops analyzing the version number since we already know it's inadequate and returns False (inadequate)}
      exit;
    end


  {Starts detecting the Minor version of the Version Installed}

  {Modifies strVersion_Installed removing the first period and everything prior to it (Removes the Major Version)}
  strVersion_Installed := Copy(strVersion_Installed, intEnd_Pos + 2, Length(strVersion_Installed));

  {Sets the end position equal to one character before the first period in the version number}
  intEnd_Pos := Pos('.', strVersion_Installed) -1

  {Sets the Minor version equal to all character before the first period }
	intMinor_installed := StrToIntDef(RemoveLeadingZeros(Copy(strVersion_Installed, 1, intEnd_Pos)), 0)

	{Debug
  msgbox('The Minor version installed is ' + IntToStr(intMinor_installed) + ' and the required Minor is ' + IntToStr(intMinor) + '.', mbInformation, MB_OK)
        }

	{If the Minor Version Installed is greater than the required value then...}
	If intMinor_Installed > intMinor Then
    begin
      {Returns that the version number was adequate}
      Result := True;

      {Stops further analyzation of the version number}
      exit
    end;

  {If the minor installed is less than what was required}
  If intMinor_Installed < intMinor Then
    Begin
      Result := False;

      {Returns that the version installed did not meet the required value and stops analyzing the version number}
      exit;
    end;


  {Starts Detecting the Revision of the Version Installed}

	{Modifies strVersion_Installed removing the first period and everything prior to it (Removes the Minor Version)}
	strVersion_Installed := Copy(strVersion_Installed, intEnd_Pos + 2, Length(strVersion_Installed));

	{Sets the last character of the Revision to last character before the first period}
	intEnd_Pos := Pos('.', strVersion_Installed) -1

	{Sets the Revision Installed equal to everything before the first period}
	intRevision_Installed := strToIntDef(RemoveLeadingZeros(Copy(strVersion_Installed, 1, intEnd_Pos)), 0);

	{Debug
  msgbox('The Revision version installed is ' + IntToStr(intRevision_installed) + ' and the required Revision is ' + IntToStr(intRevision) + '.', mbInformation, MB_OK)
        }

	{If the Revision Installed is greater than the required value then...}
  If intRevision_Installed > intRevision Then
    begin
      {Return that the version installed was adequate}
      Result := True;

      {Stops further analyzation of the version number}
      exit
    end;

  {If the revision installed did not meet the requirement then...}
  If intRevision_Installed < intRevision Then
    begin
      Result := False;

    {Return that the version number failed to meet the requirement and stops further analyzation of the version number}
      exit;
    end;

  {Start Detection the Build Installed}

	{Modifies strVersion_Installed removing the first period and everything prior to it (Removes the Revision) leaving behind only the build number}
	strVersion_Installed := Copy(strVersion_Installed, intEnd_Pos + 2, Length(strVersion_Installed));

	{Set the build installed = to what's left of the strVersion_Installed text}
	intBuild_installed := strToIntDef(RemoveLeadingZeros(strVersion_Installed), 0);

	{Debug
  msgbox('The Build version installed is ' + IntToStr(intBuild_installed) + ' and the required Build is ' + IntToStr(intBuild) + '.', mbInformation, MB_OK)
        }

	{If the build installed is greater than or equal to the requirement then...}
	if intBuild_installed >= intBuild Then

	 {Report that the version installed was adequate}
    Result := True
  else

    {Reports the inadequacy of the version installed and seases processing }
    Result := False;
end;

{Returns whether or not the version of particular file is at least equal to requested value}
Function FileVersionAtLeast(strFile_Path: string; intMajor: integer; intMinor: integer; intRevision: integer; intBuild: integer): boolean;
  Var
  {Stores the version of the file to be compared}
  strVersion: string;
Begin
  {Gets the version number of the specified file}
  GetVersionNumbersString(strFile_Path, strVersion)

  {File Version at least is the result of the VersionAtLeast Determination}
  Result := VersionAtLeast(strVersion, intMajor, intMinor, intRevision, intBuild);


  {Debug: If you'd like to debug with a messagebox then un rem this
	If Result = True then

	 Msgbox('The version of ' + strFile_path + ' required is "' + IntToStr(intMajor) + '.' + IntToStr(intMinor) + '.' + IntToStr(intRevision) + '.' + IntToStr(intBuild) + '". The version found was "'  + strVersion + '.  The version detected met the required value.', mbInformation, MB_OK)
	else
	 Msgbox('The version of ' + strFile_path + ' required is "' + IntToStr(intMajor) + '.' + IntToStr(intMinor) + '.' + IntToStr(intRevision) + '.' + IntToStr(intBuild) + '". The version found was "' + strVersion + '.  The version detected did not meet the required value.', mbInformation, MB_OK);
    }
end;

function Install7ZipDll(strDLL_Version: string): Boolean;
Var
    {Stores the file path of the 7-Zip File Manager Program}
    str7Zip_Path: String;

    {Stores the version of 7-Zip Installed}
    str7Zip_Version: String;

    {Stores the DLL's Version Function Input Parameter in integer format}
    intDLL_Version: Integer;
Begin

    {If the actual version of 7-Zip Installed hasn't been determined yet then...}
    If int7Zip_Version = 0 Then
        Begin
	       {Detects the install location of 7-Zip from the registry, if it's installed}
	       RegQueryStringValue(HKLM, 'SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\7zFM.exe', '', str7Zip_Path)

	        {If there is 7-Zip information in the registry then...}
			If length(str7Zip_Path) > 0 Then
				begin
				    {If the 7zFM.exe file exists then...}
				    If FileExists(str7Zip_Path) = True Then
				        Begin
							{Detects the version of the 7-Zip Installed}
							GetVersionNumbersString(str7Zip_Path, str7Zip_Version)



				            {If the version of 7-Zip Installed is at least 3.11 Then...}
				            If VersionAtLeast(str7Zip_Version, 3, 11, 0, 0) = True Then
				                begin
				                    {If the user has 3.12 or higher installed then...}
				                    If VersionAtLeast(str7Zip_Version, 3, 12, 0, 0) = True Then
                                        Begin
				                            {If the user has 3.13 or higher installed then...}
				                            If VersionAtLeast(str7Zip_Version, 3, 13, 0, 0) = True Then

				                                {We record the version of 7-Zip installed as 3.13 regardless of whether or not it's actually 3.14, 3.15, etc..}
				                                int7Zip_Version := 313

				                            Else

				                                {Since it was at least 3.12, but not 3.13 then it must be 3.12}
				                                int7Zip_Version := 312;

				                        end
				                    Else
				                            {Since it was at least 3.11, but not 3.12 then it must be 3.11}
				                            int7Zip_Version := 311;
				                end;
                        End
				    Else
				       {Records that the 7-Zip program didn't exist for the rest of the installation}
				        int7Zip_Version := -1;
				end
			Else
				{Records that the 7-Zip program wasn't installed for rest of the installation}
				int7Zip_Version := -1;
	    end;

    {Converts the DLL Version String to an Integer for numeric evaluation}
    intDLL_Version := StrToInt(strDLL_Version);

	{If 7-Zip either wasn't installed or was of inadequate version then...}
	If int7Zip_Version = -1 Then
		Begin
			{If the program is trying to determine if the 313 DLL should be installed then the answer is yes
			we install this, because it's the most recent version and if they were to install 7-zip this
			would be the version they'd want (since people generally install the latest and greatest)}
			if intDLL_Version = 313 Then
				Result := True

			{If the program is trying to install anything, but 313 on a system without 7-Zip
			then we disallow the installation of that DLL}
			else
				Result := False;
		End

	{If the version of 7-Zip was sufficient then...}
	Else
		Begin
			{if the version the program is trying to install matches the version installed on the clients system then...}
			If int7Zip_Version = intDLL_Version Then
				Result := True

			{If the program is trying to install the 31X DLL on a 31Y system then we won't allow the file to be copied...}
            else
				Result := False;
		End;

    {Debug:
    If UsingWinNT = True Then
        begin
            If Result = True Then
                Msgbox('We''re are installing Merge7z' + strDLL_Version + 'U.DLL because the system has 7-Zip ' + IntToStr(int7Zip_Version) + ' installed.', mbInformation, mb_Ok)
            Else
                Msgbox('We''re aren''t installing Merge7z' + strDLL_Version + 'U.DLL because the system has 7-Zip ' + IntToStr(int7Zip_Version) + ' installed.', mbInformation, mb_Ok);
        end


    Else
         begin
            If Result = True Then
                Msgbox('We''re are installing Merge7z' + strDLL_Version + '.DLL because the system has 7-Zip ' + IntToStr(int7Zip_Version) + ' installed.', mbInformation, mb_Ok)
            Else
                Msgbox('We''re aren''t installing Merge7z' + strDLL_Version + '.DLL because the system has 7-Zip ' + IntToStr(int7Zip_Version) + ' installed.', mbInformation, mb_Ok);
        end }

End;

{Determines whether or not to Install the Comctl Update
Note: this confusing code isn't Seier's it's from jrsoftware.org :)
}
function InstallComCtlUpdate: Boolean;
var
  MS, LS: Cardinal;
begin
    Case intComCtlNeeded of
        -1: Result := False;
        0:
            begin

                // Only install if the existing comctl32.dll is < 5.80
                Result := False;
                intComCtlNeeded := -1;
                if GetVersionNumbers(ExpandConstant('{sys}\comctl32.dll'), MS, LS) then
                    begin
                        if MS < $00050050 then
                            begin
                                Result := True;
                                intComCtlNeeded := 1
                            end;
                    end
            end;
        1: Result := True;

    end;
end;

{Returns true or false based on whether the specified task is disabled}
Function TaskDisabled(strTask: string): boolean;
Begin
    Case ShouldProcessEntry('main', strTask) of
        srNo:
            Result := True;
        srYes:
            Result := False;
        srUnknown:
            Begin
                msgbox('TaskDisabled(' + strTask + ')=Unknown', mbInformation, mb_Ok)
                Result := False
            end;
    End;

    {Debug}
    If Result = True then
        Msgbox('TaskDisabled(' + strTask + ')=True', mbInformation, mb_OK)
    else
        Msgbox('TaskDisabled(' + strTask + ')=False', mbInformation, mb_OK);


End;

Procedure ShellExtension(strCommand: string);
var
    intReturn_Code: integer;
    strShellExt_Path: string;
Begin

    If FileExists(ExpandConstant('{sys}\Atl.dll')) = True Then
        Begin

            {Debug}
            msgbox('ShellExtension(' + strCommand + ')', mbInformation, mb_OK)

            strShellExt_Path := ExpandConstant('{app}\ShellExtension.dll');



            If FileExists(strShellExt_Path) = True Then
                UnRegisterServer(strShellExt_Path, True);

            If strCommand = '/I' Then
                InstExec(ExpandConstant('{app}\Setup Helper.exe'), '/I', '', True, False, 0, intReturn_Code)
            Else
                InstExec(ExpandConstant('{app}\Setup Helper.exe'), '/U', '', True, False, 0, intReturn_Code);
            BringToFrontandRestore
       end
    else
        msgbox('Atl.dll doesn''t yet exist on the clients desktop and therefore the system must be restarted anyhow so we''ll skip launching Setup Helper.exe', mbINformation, mb_OK);
end;


Function TortoiseCVSInstalled(): boolean;
Begin
    Result := RegKeyExists(HKEY_LOCAL_MACHINE, 'SOFTWARE\TortoiseCVS\CustomIcons');
End;

Function OldGroup(): string;
Begin
    {Stores where in \All Users\Programs\ our start menu used to be located}
     RegQueryStringValue(HKEY_LOCAL_MACHINE, 'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\WinMerge_is1', 'Inno Setup: Icon Group', Result)

     {If the there actually was a previous start menu location then...}
     If Result <> '' Then
        {Sets the resulting path equal to the location of the \All Users\Programs\
        and specifically where WinMerge was as a subdirectory of that location}
        Result := ExpandConstant('{commonprograms}')  + '\' + Result;
End;

Procedure DeletePreviousStartMenu();
Var
        strOld: string;
        strNew: string;
Begin
    strOld := UpperCase(OldGroup());
    strNew := UpperCase(ExpandConstant('{group}'));

    If strOld = strNew Then
        {Debug}
        msgbox('The old and new start menu intall locations haven''t changed: ' + strOld + '.', mbInformation, mb_OK)
    else
        Begin
            If strOld = '' Then
                {Debug}
                msgbox('There is no previous known start menu for WinMerge.', mbInformation, mb_OK)
            Else
                Begin
                    {Debug}
                    msgbox('The old and new start menu locations are different' + #13 + #13 + '[strOld]=' + strOld + #13 + '[strNew]=' + strNew, mbInformation, mb_OK)
                    RemoveDir(strOld)
                end;
        end;
End;

Procedure CurPageChanged(CurPage: integer);
Begin
    If CurPage = wpInstalling Then
        DeletePreviousStartMenu;
End;





// Function generated by ISTool.
function NextButtonClick(CurPage: Integer): Boolean;
begin
    Result := True;
	{Result := istool_download(CurPage);}
end;

;[_ISToolDownload]
;Name: 7-Zip; Description: Compression Tool; GroupDescription: Viewers; Flags: checkedonce; MessagesFile: compiler:Default.isl; Source: http://prdownloads.sourceforge.net/sevenzip/7z313.exe?use_mirror=aleron; DestDir: {tmp}; DestName: 7-Zip Install.exe; Components: Plugins
