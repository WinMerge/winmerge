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
;                           2.  Make sure Compression=LZMA/Ultra, InternalCompressLevel=Ultra, and SolidCompression=True these values are lowered during
;                               development to speed up compilation, however at release we want the intaller to be as strong as possible.
;                           3.  Compile the script:
;                                   -From Inno Setup "Click "Build" --> "Compile"
;                                   -From ISTool Click "Project" --> "Compile Setup"
;                           4.  The compiled installer will appear in the \InnoSetup\Output\ directory at currently should be around 1.5MBs in size.
;
; Installer To Do List:
; Localization
; #  Create instructions and a sample language file using the Inno Setup Translator Tool (http://www2.arnes.si/~sopjsimo/translator.html)
; #  Seier will need to apply this {#AppVersion} directive to all localizations:
;        English.SetupAppTitle=Setup - WinMerge {#AppVersion}
;
; Bugs & Other Priority Items:
; #  Add the Windows Scripting Host if the user chooses the plugins (*.SCT)
; #  UnpackDFM.dll requires ADVAPI32.dll, we'll have to bundle IE or at least display a warning :).  Also figure out if Delphi 4+ /Free Pascal wouldn't
;    install this to begin with
; #  Make the Install7ZipDll() Function automatically work with future versions of Merge7zDLL (Use GetCurentFileName)
; #  Provide the option to or not to assign the Ctrl+Alt+M accelerator to WinMerge., make sure it's turned on for at least one icon
; #  Add WinMerge to the user's path so they can execute comparison's from a Dos Prompt (Cmd.exe/Command.exe)
; #  We need to unregister, and delete the ShellExtension Dll if the user doesn't want it, during installation
; #  When Explorer.exe is restarted we should record what windows were present before hand and restore them afterwards.

; #  Display integration options in gray rather than hiding them if the user doesn't have the application in question installed
; #  Put Plugins.txt in docs and shortcut to it in \Plugins
; #  Distribute AppHelp.dll since it's required by WinMerge.exe, ShellExtension.dll, Merge7zU311.zip
; #  Only display TortoiseCVS option if the user has it installed
; #  We need to ask those that have the RCLLocalization.dll in their plugins folder if they actually want it, their answer will need to be stored in the registry
; #  Write code to detect "\Programs\WinMerge\WinMerge" type start menu installs
;
; #  Application Integration:
; #  We need to backup the previously integrated viewer in TortoiseCVS, at uninstallation ([UninstallRun] Regedit -S Tortoise.reg)
;    or when the user deselects it this will need to be uninstalled
; #  Automatically detect and configure WinMerge to work with Visual Source Safe
; #  We need to add support for Tortoises' SubVersion program mimicking our support settings for TortoiseCVS if possible
; #  If the user happens to have Syn Text Editor installed then we'll make that the default text editor rather than notepad.
; #  If the user has ultra-edit installed then offer to use it as the text editor
; #  If the user has Aedix installed then offer to use it as the text editor
; #  We need to determine if our application can cooperate with WinCVS and if so how
; #  Add Delphi 4 Detection for UnpackDFM.dll, making it both selected and visible
;
; Custom Installer Pages:
; #  Bundle 7-Zip with WinMerge or provide on the fly download capability.
; #  Allow users to set their working directory via a custom installer page
;
; Things that make the user's life easier:
; #  Evaluate current default settings in WinMerge and bug those that don't make sense
; #  Create instructions and a sample language file using the Inno Setup Translator Tool (http://www2.arnes.si/~sopjsimo/translator.html)
; #  Add "WinMerge is running would you like to close it now?" support with programmatic termination
;     -Note: We'll need to add a declares statement to our ISX code so that we can use FindWindowEx directly or a mutex search or two
; #  Evaluate current default settings in WinMerge and bug those that don't make sense
; #  Rather than requiring users to restart we could just kill all intances of Explorer.exe, but we'll need to prompt the user first and restart it
;    once the ShellExtension.dll file has been added or removed.
;
; Non-Essential Features:
; #  Add a Desktop.ini file to our \Program Files\WinMerge folder
;    Warn users about the reprocussions for enhancing their folder icon.  Or figure out how to keep these folders visible at a command-prompt.
; #  Make an uninstall icon and use it: UninstallIconFile=..\src\res\Merge.ico
; #  Make a Floppy Disk /Low Bandwidth Edition of the WinMerge Installer that doesn't include outdated 7-Zip Support (3.11, 3.12) or the Language files
;     If the user requires any of these we'll download it on the fly.  (maybe that should be the default behavior from the get go?)
; #  See about getting a higher resolution copy of the Users's Guide.ico source art from somebody (A 32x32, and or 48x48 would be nice)
; #  Create a switch for the installer to unzip all of the included binaries as if it were a zip file.
; #  Using the registry set the order our icons appear within their group in the start menu.:
;      1.  WinMerge
;      2.  Read Me
;      3.  Users's Guide
;      4.  WinMerge on the Web
;      5.  Uninstall WinMerge
; #  Create the ability to install to two start menu groups simultaneously
;
; Misc
; #  Determine whether NT 3.51 with a 3.0 or higher version of IE can run our application I don't want the system requirements
;     in \Docs\Read Me.RTF to be inaccurate.
;
; Not yet possible (Limited by Inno Setup):
; #  While uninstalling prompt the user as to whether or not they'd like to remove their WinMerge preferences too?

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

;Tells the installer to only display a select language dialog if the an exact match wasn't found
ShowLanguageDialog=Auto

DefaultDirName={pf}\WinMerge
DefaultGroupName=WinMerge
DisableStartupPrompt=yes
AllowNoIcons=yes
InfoBeforeFile=..\Docs\users\GPL.rtf
InfoAfterFile=..\Docs\users\Read Me.rtf

OutputBaseFilename=WinMerge-{#AppVersion}-Setup

;This must be admin to install C++ Runtimes
PrivilegesRequired=admin

UninstallDisplayIcon={app}\{code:ExeName}

;File Version Info
VersionInfoVersion={#AppVersion}

;Artwork References
WizardImageFile=Art\Large Logo.bmp
WizardSmallImageFile=Art\Small Logo.bmp
WizardImageStretch=No

SetupIconFile=..\src\res\Merge.ico

;The uninstall icon shouldn't match the WinMerge icon, because it would look confusing in the start menu.
;  So I've remmed this until someone (probably me [Seier Blackburn]) creates a decent WinMerge specific uninstall icon
;UninstallIconFile=..\src\res\Merge.ico

;Compression Parameters
;Please note while Compression=lzma/ultra and InternalCompressLevel=Ultra are better than max
;they also require 320 MB of memory for compression. If you're system has at least 256MB RAM then by all
;means set it to ultra before compilation
Compression=LZMA/Ultra
InternalCompressLevel=Ultra
SolidCompression=True


[Languages]
;Inno Setup's Native Language
Name: English; MessagesFile: Languages\English.isl

;Localizations:
Name: Bulgarian; MessagesFile: Languages\Bulgarian.isl
Name: Catalan; MessagesFile: Languages\Catalan.isl
Name: Chinese_Simplified; MessagesFile: Languages\Chinese_Simplified.isl
Name: Chinese_Traditional; MessagesFile: Languages\Chinese_Traditional.isl
Name: Czech; MessagesFile: Languages\Czech.isl
Name: Danish; MessagesFile: Languages\Danish.isl
Name: Dutch; MessagesFile: Languages\Dutch.isl
Name: French; MessagesFile: Languages\French.isl
Name: German; MessagesFile: Languages\German.isl
Name: Italian; MessagesFile: Languages\Italian.isl
Name: Japanese; MessagesFile: Languages\Japanese.isl
Name: Korean; MessagesFile: Languages\Korean.isl
Name: Norwegian; MessagesFile: Languages\Norwegian.isl
Name: Polish; MessagesFile: Languages\Polish.isl
Name: Portuguese; MessagesFile: Languages\Brazilian_Portuguese.isl
Name: Russian; MessagesFile: Languages\Russian.isl
Name: Slovak; MessagesFile: Languages\Slovak.isl
Name: Spanish; MessagesFile: Languages\Spanish.isl
Name: Swedish; MessagesFile: Languages\Swedish.isl; InfoAfterFile: ..\Docs\Users\Languages\Read Me-Swedish.rtf


[Messages]
English.FinishedLabel=Setup has finished installing WinMerge on your computer.
English.SetupAppTitle=Setup - WinMerge {#AppVersion}
English.WizardInfoBefore=License Agreement
English.InfoBeforeLabel=GNU General Public License


[Types]
Name: typical; Description: {cm:TypicalInstallation}
Name: full; Description: {cm:FullInstallation}
Name: compact; Description: {cm:CompactInstallation}
Name: custom; Description: {cm:CustomInstallation}; Flags: iscustom


[Components]
Name: Core; Description: {cm:AppCoreFiles}; Types: full custom typical compact; Flags: fixed
Name: Runtimes; Description: {cm:ApplicationRuntimes}; Types: full custom typical compact; Flags: fixed

Name: docs; Description: {cm:UsersGuide}; Flags: disablenouninstallwarning; Types: full typical
Name: filters; Description: {cm:Filters}; Flags: disablenouninstallwarning; Types: full typical
Name: Plugins; Description: {cm:Plugins}; Flags: disablenouninstallwarning; Types: full typical

;Language components, please note that whatever language you chose to install in will be installed in addition to any of the language components you've
;selected.  This is to make it easier for users that have a working localization of Inno Setup.
Name: Languages; Description: {cm:Languages}; Flags: disablenouninstallwarning
Name: Languages\Bulgarian; Description: {cm:BulgarianLanguage}; Flags: disablenouninstallwarning; Types: full; Check: LanguageDisabled(Bulgarian)
Name: Languages\Bulgarian; Description: {cm:BulgarianLanguage}; Flags: disablenouninstallwarning fixed; Types: full typical compact; Languages: Bulgarian

Name: Languages\Catalan; Description: {cm:CatalanLanguage}; Flags: disablenouninstallwarning; Types: full; Check: LanguageDisabled(Catalan)
Name: Languages\Catalan; Description: {cm:CatalanLanguage}; Flags: disablenouninstallwarning fixed; Types: full typical compact; Languages: Catalan

Name: Languages\Chinese_Simplified; Description: {cm:ChineseSimplifiedLanguage}; Flags: disablenouninstallwarning; Types: full; Check: LanguageDisabled(Chinese_Simplified)
Name: Languages\Chinese_Simplified; Description: {cm:ChineseSimplifiedLanguage}; Flags: disablenouninstallwarning fixed; Types: full typical compact; Languages: Chinese_Simplified

Name: Languages\Chinese_Traditional; Description: {cm:ChineseTraditionalLanguage}; Flags: disablenouninstallwarning; Types: full; Check: LanguageDisabled(Chinese_Traditional)
Name: Languages\Chinese_Traditional; Description: {cm:ChineseTraditionalLanguage}; Flags: disablenouninstallwarning fixed; Types: full typical compact; Languages: Chinese_Traditional

Name: Languages\Czech; Description: {cm:CzechLanguage}; Flags: disablenouninstallwarning; Types: full; Check: LanguageDisabled(Czech)
Name: Languages\Czech; Description: {cm:CzechLanguage}; Flags: disablenouninstallwarning fixed; Types: full typical compact; Languages: Czech

Name: Languages\Danish; Description: {cm:DanishLanguage}; Flags: disablenouninstallwarning; Types: full; Check: LanguageDisabled(Danish)
Name: Languages\Danish; Description: {cm:DanishLanguage}; Flags: disablenouninstallwarning fixed; Types: full typical compact; Languages: Danish

Name: Languages\Dutch; Description: {cm:DutchLanguage}; Flags: disablenouninstallwarning; Types: full; Check: LanguageDisabled(Dutch)
Name: Languages\Dutch; Description: {cm:DutchLanguage}; Flags: disablenouninstallwarning fixed; Types: full typical compact; Languages: Dutch

Name: Languages\French; Description: {cm:FrenchLanguage}; Flags: disablenouninstallwarning; Types: full; Check: LanguageDisabled(French)
Name: Languages\French; Description: {cm:FrenchLanguage}; Flags: disablenouninstallwarning fixed; Types: full typical compact; Languages: French

Name: Languages\German; Description: {cm:GermanLanguage}; Flags: disablenouninstallwarning; Types: full; Check: LanguageDisabled(German)
Name: Languages\German; Description: {cm:GermanLanguage}; Flags: disablenouninstallwarning fixed; Types: full typical compact; Languages: German

Name: Languages\Italian; Description: {cm:ItalianLanguage}; Flags: disablenouninstallwarning; Types: full; Check: LanguageDisabled(Italian)
Name: Languages\Italian; Description: {cm:ItalianLanguage}; Flags: disablenouninstallwarning fixed; Types: full typical compact; Languages: Italian

Name: Languages\Japanese; Description: {cm:JapaneseLanguage}; Flags: disablenouninstallwarning; Types: full; Check: LanguageDisabled(Japanese)
Name: Languages\Japanese; Description: {cm:JapaneseLanguage}; Flags: disablenouninstallwarning fixed; Types: full typical compact; Languages: Japanese

Name: Languages\Korean; Description: {cm:KoreanLanguage}; Flags: disablenouninstallwarning; Types: full; Check: LanguageDisabled(Korean)
Name: Languages\Korean; Description: {cm:KoreanLanguage}; Flags: disablenouninstallwarning fixed; Types: full typical compact; Languages: Korean

Name: Languages\Norwegian; Description: {cm:NorwegianLanguage}; Flags: disablenouninstallwarning; Types: full; Check: LanguageDisabled(Norwegian)
Name: Languages\Norwegian; Description: {cm:NorwegianLanguage}; Flags: disablenouninstallwarning fixed; Types: full typical compact; Languages: Norwegian

Name: Languages\Polish; Description: {cm:PolishLanguage}; Flags: disablenouninstallwarning; Types: full; Check: LanguageDisabled(Polish)
Name: Languages\Polish; Description: {cm:PolishLanguage}; Flags: disablenouninstallwarning fixed; Types: full typical compact; Languages: Polish

Name: Languages\Portuguese; Description: {cm:PortugueseLanguage}; Flags: disablenouninstallwarning; Types: full; Check: LanguageDisabled(Portuguese)
Name: Languages\Portuguese; Description: {cm:PortugueseLanguage}; Flags: disablenouninstallwarning fixed; Types: full typical compact; Languages: Portuguese

Name: Languages\Russian; Description: {cm:RussianLanguage}; Flags: disablenouninstallwarning; Types: full; Check: LanguageDisabled(Russian)
Name: Languages\Russian; Description: {cm:RussianLanguage}; Flags: disablenouninstallwarning fixed; Types: full typical compact; Languages: Russian

Name: Languages\Slovak; Description: {cm:SlovakLanguage}; Flags: disablenouninstallwarning; Types: full; Check: LanguageDisabled(Slovak)
Name: Languages\Slovak; Description: {cm:SlovakLanguage}; Flags: disablenouninstallwarning fixed; Types: full typical compact; Languages: Slovak

Name: Languages\Spanish; Description: {cm:SpanishLanguage}; Flags: disablenouninstallwarning; Types: full; Check: LanguageDisabled(Spanish)
Name: Languages\Spanish; Description: {cm:SpanishLanguage}; Flags: disablenouninstallwarning fixed; Types: full typical compact; Languages: Spanish

Name: Languages\Swedish; Description: {cm:SwedishLanguage}; Flags: disablenouninstallwarning; Types: full; Check: LanguageDisabled(Swedish)
Name: Languages\Swedish; Description: {cm:SwedishLanguage}; Flags: disablenouninstallwarning fixed; Types: full typical compact; Languages: Swedish

[Tasks]
Name: ShellExtension; Description: {cm:ExplorerContextMenu}; GroupDescription: Optional Features:
Name: TortoiseCVS; Description: {cm:IntegrateTortoiseCVS}; GroupDescription: Optional Features:; Check: TortoiseCVSInstalled
Name: desktopicon; Description: {cm:CreateDesktopIcon}; GroupDescription: {cm:AdditionalIcons}; Flags: unchecked
Name: quicklaunchicon; Description: {cm:CreateQuickLaunchIcon}; GroupDescription: {cm:AdditionalIcons}

[InstallDelete]
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
Name: {app}\WinMerge.exe; Type: files; MinVersion: 4, 0
Name: {app}\WinMergeU.exe; Type: files; MinVersion: 0, 4

;The other manifest file is legitimate, but is only valid on windows XP or higher, so if the operating system isn't at least windows XP the
;Manifest file is indeed removed.
Name: {app}\WinMergeU.exe.manifest; Type: files; OnlyBelowVersion: 0, 5.01

Name: {app}\Merge7z311.dll; Type: files; MinVersion: 4, 0
Name: {app}\Merge7z311U.dll; Type: files; MinVersion: 0, 4

Name: {app}\Merge7z312.dll; Type: files; MinVersion: 4, 0
Name: {app}\Merge7z312U.dll; Type: files; MinVersion: 0, 4

Name: {app}\Merge7z313.dll; Type: files; MinVersion: 4, 0
Name: {app}\Merge7z313U.dll; Type: files; MinVersion: 0, 4

Name: {app}\Merge7z407.dll; Type: files; MinVersion: 4, 0
Name: {app}\Merge7z407U.dll; Type: files; MinVersion: 0, 4

Name: {app}\Merge7z408.dll; Type: files; MinVersion: 4, 0
Name: {app}\Merge7z408U.dll; Type: files; MinVersion: 0, 4

Name: {app}\Merge7z409.dll; Type: files; MinVersion: 4, 0
Name: {app}\Merge7z409U.dll; Type: files; MinVersion: 0, 4

Name: {app}\Merge7z410.dll; Type: files; MinVersion: 4, 0
Name: {app}\Merge7z410U.dll; Type: files; MinVersion: 0, 4


;This won't work, because the file has to be unregistered, and explorer closed, first.
;Name: {app}\ShellExtension.dll; Type: files; Check: TaskDisabled('ShellExtension')

Name: {app}\MergeBulgarian.lang; Type: files; Check: ComponentDisabled('Bulgarian')
Name: {app}\MergeBrazilian.lang; Type: files; Check: ComponentDisabled('Portuguese')
Name: {app}\MergeCatalan.lang; Type: files; Check: ComponentDisabled('Catalan')
Name: {app}\MergeChineseSimplified.lang; Type: files; Check: ComponentDisabled('Chinese_Simplified')
Name: {app}\MergeChineseTraditional.lang; Type: files; Check: ComponentDisabled('Chinese_Traditional')
Name: {app}\MergeCzech.lang; Type: files; Check: ComponentDisabled('Czech')
Name: {app}\MergeDanish.lang; Type: files; Check: ComponentDisabled('Danish')
Name: {app}\MergeDutch.lang; Type: files; Check: ComponentDisabled('Dutch')
Name: {app}\MergeFrench.lang; Type: files; Check: ComponentDisabled('French')
Name: {app}\MergeGerman.lang; Type: files; Check: ComponentDisabled('German')
Name: {app}\MergeItalian.lang; Type: files; Check: ComponentDisabled('Italian')
Name: {app}\MergeJapanese.lang; Type: files; Check: ComponentDisabled('Japanese')
Name: {app}\MergeKorean.lang; Type: files; Check: ComponentDisabled('Korean')
Name: {app}\MergeNorwegian.lang; Type: files; Check: ComponentDisabled('Norwegian')
Name: {app}\MergePolish.lang; Type: files; Check: ComponentDisabled('Polish')
Name: {app}\MergeSlovak.lang; Type: files; Check: ComponentDisabled('Slovak')
Name: {app}\MergeSpanish.lang; Type: files; Check: ComponentDisabled('Spanish')
Name: {app}\MergeRussian.lang; Type: files; Check: ComponentDisabled('Russian')
Name: {app}\MergeSwedish.lang; Type: files; Check: ComponentDisabled('Swedish')
Name: {app}\MergePlugins\list.txt; Type: files; Check: ComponentDisabled('Plugins')

;Removes the user's guide icon if the user deselects the user's guide component.
Name: {group}\{cm:UsersGuide}.lnk; Type: files; Check: componentDisabled('Docs')


;Removes misplaced Files
Name: {app}\WinMerge.url; Type: files
Name: {app}\Read Me.rtf; Type: files
Name: {app}\Contributors.rtf; Type: files

;Remove deprecated Contributors file
Name: {app}\Docs\Contributors.rtf; Type: files

;This removes the quick launch icon in case the user chooses not to install it after previously having it installed
Name: {userappdata}\Microsoft\Internet Explorer\Quick Launch\WinMerge.lnk; Type: files; Check: TaskDisabled('QuickLauchIcon')

;This removes the desktop icon in case the user chooses not to install it after previously having it installed
Name: {userdesktop}\WinMerge.lnk; Type: files; Check: TaskDisabled('DesktopIcon')

Name: {app}\Docs; Type: filesandordirs

Name: {app}\MergePlugins\editor addin.sct; Type: Files; Check: ComponentDisabled('Plugins')
Name: {app}\MergePlugins\insert datetime.sct; Type: Files; Check: ComponentDisabled('Plugins')
Name: {app}\Docs\Plugins.txt; Type: Files; Check: DontInstallPluginsText
Name: {app}\MergePlugins; Type: DirIfEmpty; Check: ComponentDisabled('Plugins')

;Removes a CVS ignore record created by TortoiseCVS on Seier's System, I'm hoping this was never let
;out into the wild, but it could have been
Name: {app}\Filters\.cvsignore; TYpe: Files
Name: {app}\Filters\Merge_GnuC_loose.flt; Type: Files; Check: ComponentDisabled('Filters')
Name: {app}\Filters\Merge_VC_loose.flt; Type: Files; Check: ComponentDisabled('Filters')
Name: {app}\Filters\Merge_VB_loose.flt; Type: Files; Check: ComponentDisabled('Filters')
Name: {app}\Filters\XML_html.flt; Type: Files; Check: ComponentDisabled('Filters')
Name: {app}\Filters; Type: DirIfEmpty; Check: ComponentDisabled('Filters')



[Dirs]
;The always uninstall flag tells the uninstaller to remove the folder if it's empty regardless of whether or not it existed prior to the installation
Name: {app}; Flags: uninsalwaysuninstall


[Files]
;The MinVersion forces Inno Setup to only copy the following file if the user is running a WinNT platform system
Source: ..\Build\MergeUnicodeRelease\WinMergeU.exe; DestDir: {app}; Flags: promptifolder; MinVersion: 0, 4; Components: Core
Source: ..\Build\MergeRelease\WinMerge.exe; DestDir: {app}; Flags: promptifolder; Components: Core

;The MinVersion forces Inno Setup to only copy the following file if the user is running a WinNT platform system
Source: WinMergeU.exe.manifest; DestDir: {app}; Flags: promptifolder; MinVersion: 0, 5.01; Components: Core
Source: WinMerge.exe.manifest; DestDir: {app}; Flags: promptifolder; Components: Core; MinVersion: 0,5.01

;Installs the ComCtl32.dll update on any system where its DLLs are more recent
Source: Runtimes\50comupd.exe; DestDir: {tmp}; Flags: DeleteAfterInstall; Check: InstallComCtlUpdate

; We still need APPHelp.dll!

; begin VC system files

;Atl.dll
Source: Runtimes\Atla.dll; DestDir: {sys}; Destname: Atl.dll; Flags: restartreplace uninsneveruninstall regserver sharedfile; MinVersion: 4, 0; Components: Runtimes
Source: Runtimes\Atlu.dll; DestDir: {sys}; Destname: Atl.dll; Flags: restartreplace uninsneveruninstall regserver sharedfile; MinVersion: 0, 4; Components: Runtimes

Source: Runtimes\mfc42.dll; DestDir: {sys}; Flags: restartreplace uninsneveruninstall regserver sharedfile; Components: Runtimes
Source: Runtimes\mfc42u.dll; DestDir: {sys}; Flags: restartreplace uninsneveruninstall regserver sharedfile; MinVersion: 0, 4; Components: Runtimes

Source: Runtimes\msvcrt.dll; DestDir: {sys}; Flags: restartreplace uninsneveruninstall sharedfile; Components: Runtimes

Source: Runtimes\OleAut32.dll; DestDir: {sys}; Flags: restartreplace uninsneveruninstall regserver sharedfile; Components: Runtimes
; end VC system files

Source: ..\Build\MergeRelease\ShellExtension.dll; DestDir: {app}; Flags: regserver uninsrestartdelete restartreplace promptifolder

;Please do not reorder the 7z Dlls by version they compress better ordered by platform and then by version
Source: ..\Build\MergeUnicodeRelease\Merge7z410U.dll; DestDir: {app}; Flags: promptifolder; MinVersion: 0, 4; Check: Install7ZipDll(410)
Source: ..\Build\MergeUnicodeRelease\Merge7z409U.dll; DestDir: {app}; Flags: promptifolder; MinVersion: 0, 4; Check: Install7ZipDll(409)
Source: ..\Build\MergeUnicodeRelease\Merge7z408U.dll; DestDir: {app}; Flags: promptifolder; MinVersion: 0, 4; Check: Install7ZipDll(408)
Source: ..\Build\MergeUnicodeRelease\Merge7z407U.dll; DestDir: {app}; Flags: promptifolder; MinVersion: 0, 4; Check: Install7ZipDll(407)
Source: ..\Build\MergeUnicodeRelease\Merge7z313U.dll; DestDir: {app}; Flags: promptifolder; MinVersion: 0, 4; Check: Install7ZipDll(313)
Source: ..\Build\MergeUnicodeRelease\Merge7z312U.dll; DestDir: {app}; Flags: promptifolder; MinVersion: 0, 4; Check: Install7ZipDll(312)
Source: ..\Build\MergeUnicodeRelease\Merge7z311U.dll; DestDir: {app}; Flags: promptifolder; MinVersion: 0, 4; Check: Install7ZipDll(311)

Source: ..\Build\MergeRelease\Merge7z410.dll; DestDir: {app}; Flags: promptifolder; MinVersion: 4, 0; Check: Install7ZipDll(410)
Source: ..\Build\MergeRelease\Merge7z409.dll; DestDir: {app}; Flags: promptifolder; MinVersion: 4, 0; Check: Install7ZipDll(409)
Source: ..\Build\MergeRelease\Merge7z408.dll; DestDir: {app}; Flags: promptifolder; MinVersion: 4, 0; Check: Install7ZipDll(408)
Source: ..\Build\MergeRelease\Merge7z407.dll; DestDir: {app}; Flags: promptifolder; MinVersion: 4, 0; Check: Install7ZipDll(407)
Source: ..\Build\MergeRelease\Merge7z313.dll; DestDir: {app}; Flags: promptifolder; MinVersion: 4, 0; Check: Install7ZipDll(313)
Source: ..\Build\MergeRelease\Merge7z312.dll; DestDir: {app}; Flags: promptifolder; MinVersion: 4, 0; Check: Install7ZipDll(312)
Source: ..\Build\MergeRelease\Merge7z311.dll; DestDir: {app}; Flags: promptifolder; MinVersion: 4, 0; Check: Install7ZipDll(311)

Source: ..\Plugins\dlls\UnpackDFM.dll; DestDir: {app}\MergePlugins; Flags: promptifolder; Components: Plugins

Source: ..\Src\Languages\DLL\MergeBulgarian.lang; DestDir: {app}; Components: Languages\Bulgarian; Flags: ignoreversion comparetimestamp
Source: ..\Src\Languages\DLL\MergeCatalan.lang; DestDir: {app}; Components: Languages\Catalan; Flags: ignoreversion comparetimestamp
Source: ..\Src\Languages\DLL\MergeChineseSimplified.lang; DestDir: {app}; Components: Languages\Chinese_Simplified; Flags: ignoreversion comparetimestamp
Source: ..\Src\Languages\DLL\MergeChineseTraditional.lang; DestDir: {app}; Components: Languages\Chinese_Traditional; Flags: ignoreversion comparetimestamp
Source: ..\Src\Languages\DLL\MergeCzech.lang; DestDir: {app}; Components: Languages\Czech; Flags: ignoreversion comparetimestamp
Source: ..\Src\Languages\DLL\MergeDanish.lang; DestDir: {app}; Components: Languages\Danish; Flags: ignoreversion comparetimestamp
Source: ..\Src\Languages\DLL\MergeDutch.lang; DestDir: {app}; Components: Languages\Dutch; Flags: ignoreversion comparetimestamp
Source: ..\Src\Languages\DLL\MergeFrench.lang; DestDir: {app}; Components: Languages\French; Flags: ignoreversion comparetimestamp
Source: ..\Src\Languages\DLL\MergeGerman.lang; DestDir: {app}; Components: Languages\German; Flags: ignoreversion comparetimestamp
Source: ..\Src\Languages\DLL\MergeItalian.lang; DestDir: {app}; Components: Languages\Italian; Flags: ignoreversion comparetimestamp
Source: ..\Src\Languages\DLL\MergeJapanese.lang; DestDir: {app}; Components: Languages\Japanese; Flags: ignoreversion comparetimestamp
Source: ..\Src\Languages\DLL\MergeKorean.lang; DestDir: {app}; Components: Languages\Korean; Flags: ignoreversion comparetimestamp
Source: ..\Src\Languages\DLL\MergeNorwegian.lang; DestDir: {app}; Components: Languages\Norwegian; Flags: ignoreversion comparetimestamp
Source: ..\Src\Languages\DLL\MergePolish.lang; DestDir: {app}; Components: Languages\Polish; Flags: ignoreversion comparetimestamp
Source: ..\Src\Languages\DLL\MergeBrazilian.lang; DestDir: {app}; Components: Languages\Portuguese; Flags: ignoreversion comparetimestamp
Source: ..\Src\Languages\DLL\MergeRussian.lang; DestDir: {app}; Components: Languages\Russian; Flags: ignoreversion comparetimestamp
Source: ..\Src\Languages\DLL\MergeSlovak.lang; DestDir: {app}; Components: Languages\Slovak; Flags: ignoreversion comparetimestamp
Source: ..\Src\Languages\DLL\MergeSpanish.lang; DestDir: {app}; Components: Languages\Spanish; Flags: ignoreversion comparetimestamp
Source: ..\Src\Languages\DLL\MergeSwedish.lang; DestDir: {app}; Components: Languages\Swedish; Flags: ignoreversion comparetimestamp

Source: ..\Filters\*.flt; DestDir: {app}\Filters; Flags: sortfilesbyextension comparetimestamp ignoreversion; Components: filters

;Documentation
Source: ..\Docs\Users\Read Me.rtf; DestDir: {app}\Docs; Flags: comparetimestamp ignoreversion promptifolder; Components: Core
Source: ..\Docs\Users\Contributors.txt; DestDir: {app}; Flags: comparetimestamp ignoreversion promptifolder; Components: Core
Source: ..\Build\Manual\*; DestDir: {app}\Docs\Manual\; Flags: overwritereadonly recursesubdirs uninsremovereadonly sortfilesbyextension; Components: docs

;Plugins
;Please note IgnoreVersion and CompareTimeStamp are to instruct the installer to not not check for version info and go straight to comparing modification dates
Source: ..\Plugins\dlls\editor addin.sct; DestDir: {app}\MergePlugins; Flags: IgnoreVersion CompareTimeStamp; Components: Plugins
Source: ..\Plugins\dlls\insert datetime.sct; DestDir: {app}\MergePlugins; Flags: IgnoreVersion CompareTimeStamp; Components: Plugins
Source: ..\Plugins\dlls\list.txt; DestDir: {app}\Docs; DestName: Plugins.txt; Flags: IgnoreVersion CompareTimeStamp; Check: InstallPluginsText


[INI]
Filename: {group}\{cm:ProgramOnTheWeb,WinMerge}.url; Section: InternetShortcut; Key: URL; String: http://WinMerge.org/; Check: GroupCreated


[Icons]
;Start Menu Icons
Name: {group}\WinMerge; Filename: {app}\{code:ExeName}
Name: {group}\{cm:ReadMe}; Filename: {app}\Docs\Read Me.rtf; IconFileName: {win}\NOTEPAD.EXE
Name: {group}\Plugins.txt; Filename: {app}\Docs\Plugins.txt; Check: InstallPluginsText
Name: {group}\{cm:UsersGuide}; Filename: {app}\Docs\Manual\index.html; Components: docs
Name: {group}\{cm:UninstallProgram,WinMerge}; Filename: {uninstallexe}

;Desktop Icon
Name: {userdesktop}\WinMerge; Filename: {app}\{code:ExeName}; Tasks: desktopicon

;Quick Launch Icon
Name: {userappdata}\Microsoft\Internet Explorer\Quick Launch\WinMerge; Filename: {app}\{code:ExeName}; Tasks: quicklaunchicon

;Documentation Shortcuts
Name: {app}\MergePlugins\Plugins.txt; Filename: {app}\Docs\Plugins.txt; Check: InstallPluginsText


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

;This removes the key that remembers which messageboxes to hide from the user, this is because the text of that message
;can change and make it more clear as to the user why they might want to pay attention to a particular dialog and also
;because a particular message might be added or removed and a new message might occupy a previous message's ID number
Root: HKLM; Subkey: Software\Thingamahoochie\WinMerge\MessageBoxes; ValueType: none; Flags: deletekey

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

;Whatever the user chooses at the [Select Setup Language] dialog should also determine what language WinMerge will start up in
;(unless the user already has a startup language specified)
Root: HKCU; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000402; Flags: deletevalue; Languages: Bulgarian
Root: HKCU; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000403; Flags: deletevalue; Languages: Catalan
Root: HKCU; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000804; Flags: deletevalue; Languages: Chinese_Simplified
Root: HKCU; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000404; Flags: deletevalue; Languages: Chinese_Traditional
Root: HKCU; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000405; Flags: deletevalue; Languages: Czech
Root: HKCU; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000406; Flags: deletevalue; Languages: Danish
Root: HKCU; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000413; Flags: deletevalue; Languages: Dutch
Root: HKCU; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000409; Flags: deletevalue; Languages: English
Root: HKCU; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $0000040c; Flags: deletevalue; Languages: French
Root: HKCU; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000407; Flags: deletevalue; Languages: German
Root: HKCU; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000410; Flags: deletevalue; Languages: Italian
Root: HKCU; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000411; Flags: deletevalue; Languages: Japanese
Root: HKCU; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000412; Flags: deletevalue; Languages: Korean
Root: HKCU; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000414; Flags: deletevalue; Languages: Norwegian
Root: HKCU; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000415; Flags: deletevalue; Languages: Polish
Root: HKCU; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000419; Flags: deletevalue; Languages: Russian
Root: HKCU; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $0000041b; Flags: deletevalue; Languages: Slovak
Root: HKCU; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000c0a; Flags: deletevalue; Languages: Spanish
Root: HKCU; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $0000041D; Flags: deletevalue; Languages: Swedish
Root: HKCU; SubKey: Software\Microsoft\Windows\CurrentVersion\Explorer\MenuOrder\Start Menu\Programs\{code:RelativeGroupPath}; ValueType: binary; ValueName: Order; ValueData: 08 00 00 00 02 00 00 00 a2 01 00 00 01 00 00 00 05 00 00 00 4c 00 00 00 01 00 00 00 3d 00 00 00 41 75 67 4d 01 00 00 00 01 00 00 00 00 00 00 00 27 00 32 00 2b 05 00 00 95 30 17 80 20 00 52 65 61 64 20 4d 65 2e 6c 6e 6b 00 52 45 41 44 4d 45 7e 31 2e 4c 4e 4b 00 00 00 00 00 00 05 00 00 00 56 00 00 00 04 00 00 00 48 00 00 00 41 75 67 4d 01 00 00 00 01 00 00 00 00 00 00 00 32 00 32 00 be 01 00 00 95 30 17 80 20 00 55 6e 69 6e 73 74 61 6c 6c 20 57 69 6e 4d 65 72 67 65 2e 6c 6e 6b 00 55 4e 49 4e 53 54 7e 31 2e 4c 4e 4b 00 00 00 00 00 05 00 00 00 50 00 00 00 02 00 00 00 42 00 00 00 41 75 67 4d 01 00 00 00 01 00 00 00 00 00 00 00 2c 00 32 00 c9 05 00 00 95 30 17 80 20 00 55 73 65 72 27 73 20 47 75 69 64 65 2e 6c 6e 6b 00 55 53 45 52 27 53 7e 31 2e 4c 4e 4b 00 00 00 00 00 05 00 00 00 58 00 00 00 03 00 00 00 49 00 00 00 41 75 67 4d 01 00 00 00 01 00 00 00 00 00 00 00 33 00 32 00 2e 00 00 00 95 30 17 80 20 00 57 69 6e 4d 65 72 67 65 20 6f 6e 20 74 68 65 20 57 65 62 2e 75 72 6c 00 57 49 4e 4d 45 52 7e 31 2e 55 52 4c 00 00 00 00 00 00 05 00 00 00 4c 00 00 00 00 00 00 00 3e 00 00 00 41 75 67 4d 01 00 00 00 01 00 00 00 00 00 00 00 28 00 32 00 ce 01 00 00 95 30 17 80 20 00 57 69 6e 4d 65 72 67 65 2e 6c 6e 6b 00 57 49 4e 4d 45 52 47 45 2e 4c 4e 4b 00 00 00 00 00 05 00 00 00


[Run]
;Installs the Microsoft Common Controls Update
Filename: {tmp}\50comupd.exe; Parameters: /r:n /q:1; StatusMsg: {cm:UpdatingCommonControls}; Check: InstallComCtlUpdate

;This will no longer appear unless the user chose to make a start menu group in the first place
Filename: {win}\Explorer.exe; Description: {cm:ViewStartMenuFolder}; Parameters: """{group}"""; Flags: waituntilidle postinstall skipifsilent unchecked; Check: GroupCreated

Filename: {app}\{code:ExeName}; Description: {cm:LaunchProgram, WinMerge}; Flags: nowait postinstall skipifsilent runmaximized


[UninstallDelete]
Name: {group}\{cm:ProgramOnTheWeb,WinMerge}.url; Type: Files
Name: {group}; Type: dirifempty
Name: {app}; Type: dirifempty



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
    Case ShouldProcessEntry('core', strTask) of
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

    {Debug
    If Result = True then
        Msgbox('TaskDisabled(' + strTask + ')=True', mbInformation, mb_OK)
    else
        Msgbox('TaskDisabled(' + strTask + ')=False', mbInformation, mb_OK); }


End;

{Returns true or false based on whether the specified task is disabled}
Function ComponentDisabled(strComponent: string): boolean;
Begin
    Case ShouldProcessEntry(strComponent, '') of
        srNo:
            Result := True;
        srYes:
            Result := False;
        srUnknown:
            Begin
                msgbox('ComponentDisabled(' + strComponent + ')=Unknown', mbInformation, mb_Ok)
                Result := False
            end;
    End;

    {Debug
    If Result = True then
        Msgbox('ComponentDisabled(' + strComponent + ')=True', mbInformation, mb_OK)
    else
        Msgbox('ComponentDisabled(' + strComponent + ')=False', mbInformation, mb_OK); }


End;

{Install Plugins.txt if plugins are installed}
Function InstallPluginsText(): Boolean;
Begin
    {If Plugins are to be installed then...}
    If (ComponentDisabled('Plugins') = False) Then
        {We should install the Plugins.txt File}
        Result := True
    Else
        {The Plugins.txt file should not be installed}
        Result := False;
End;

{This function is needed as a check parameter in the [InstallDelete] section unfortunately there's not yet a way to get the Check: parameter to check
for a false value and respond accordingly}
Function DontInstallPluginsText(): Boolean;
Begin
    {If the Plugins.txt file shouldn't be installed then...}
    If InstallPluginsText() = False Then

        {This function returns True}
        Result := True
    Else
        {This function returns False}
        Result := False;
End;

{Determines whether or not TortoiseCVS is installed}
Function TortoiseCVSInstalled(): boolean;
Begin
	{This absolutely must remain as \CustomIcons, because our application used to create some TortoiseCVS keys even if the application wasn't installed!}
    Result := RegKeyExists(HKEY_LOCAL_MACHINE, 'SOFTWARE\TortoiseCVS\CustomIcons');
End;

Function RelativeGroupPath(Unused: string): String;
Var
    strGroup: String;
Begin
    strGroup := ExpandConstant('{group}');
    StringChange(strGroup, ExpandConstant('{commonprograms}\'), '');
    Result := strGroup
End;

Function OldGroup(): string;
Begin
    {Stores where in \All Users\Programs\ our start menu used to be located}
     RegQueryStringValue(HKEY_LOCAL_MACHINE, 'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\WinMerge_is1', 'Inno Setup: Icon Group', Result)
End;

Procedure DeletePreviousStartMenu();
Var
        strOld: string;
        strNew: string;
        strMessage: string;
        strShortcut: string;
Begin
    {Detects the previous start menu group's path, if any}
    strOld := OldGroup();

    {Detects the current start menu group's path, if any (not creating a group is a valid option)}
    strNew := ExpandConstant('{group}');

    {removes the start menu portion of the path from the group path making it match the format of strOld}
    StringChange(strNew, ExpandConstant('{commonprograms}\'), '')

    {if the user does have a previous start menu location then..}
    If strOld <> '' THen
        Begin
            {If the current and previous start menu locations are different then...}
		    If Uppercase(strOld) <> UpperCase(strNew) Then
		        Begin
		            strMessage := ExpandConstant('{cm:DeletePreviousStartMenu}');
                    strMessage := Format2(strMessage, strOld, strNew);

		              {Display a dialog asking the user if they'd like to delete the previous start menu group}
		            {If they'd like to delete the previous start menu group then...}
					If Msgbox(strMessage, mbConfirmation, mb_YesNo) = mrYes Then
						Begin
						    strOld := ExpandConstant('{commonprograms}\') + strOld;

                            {Removes each of the start menu icons to make the folder empty}
                            strShortcut := strOld + '\' + ExpandConstant('{cm:ReadMe}') + '.lnk';
			                DeleteFile(strShortcut)

			                strShortcut := strOld + '\Read Me.lnk';
			                DeleteFile(strShortcut)

			                strShortcut := strOld + '\' + ExpandConstant('{cm:UninstallProgram,WinMerge}') + '.lnk';
			                DeleteFile(strShortcut)

			                strShortcut := strOld + '\Uninstall WinMerge.lnk';
			                DeleteFile(strShortcut)

			                strShortcut := strOld + '\' + ExpandConstant('{cm:UsersGuide}') + '.lnk';
                            DeleteFile(strShortcut)

                            strShortcut := strOld + '\User''s Guide.lnk';
			                DeleteFile(strShortcut)

                            strShortcut := strOld + '\' + ExpandConstant('{cm:ProgramOnTheWeb,WinMerge}') + '.url';
			                DeleteFile(strShortcut)

			                strShortcut := strOld + '\Winmerge on the Web.lnk';
			                DeleteFile(strShortcut)

			                strShortcut := strOld + '\Winmerge on the Web.url';
			                DeleteFile(strShortcut)

			                strShortcut := strOld + '\WinMerge.lnk';
			                DeleteFile(strShortcut)

			                {Deletes the empty start menu directory (This has to already be empty in order for this to work, hence the
			                file deletions above)}
			                RemoveDir(strOld)
			            end;
		              {end; }
		        end;
        end;
End;

{Returns True if the installer isn't running in a particular language.
Inputs: The Internal Name of the Language (These are all defined in the [Languages] section}
Function LanguageDisabled(strCheck_Language: string): boolean;
Begin
    if strCheck_Language <> ExpandConstant('{language}') Then
        Result := True
    Else
        Result := False;

End;

{This event procedure is queed each time the user changes pages within the installer}
Procedure CurPageChanged(CurPage: integer);
Begin
    {if the installer reaches the file copy page then...}
    If CurPage = wpInstalling Then
            {Delete the previous start menu group if the location has changed since the last install}
            DeletePreviousStartMenu;
End;
