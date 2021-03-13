;           Programmed by:  Christian Blackburn, Christian List, Kimmo Varis,
;                 Purpose:  The is the Inno Setup installation script for distributing our WinMerge application.
; Tools Needed To Compile:  Inno Setup 5.1.7+ (http://www.jrsoftware.org/isdl.php), Inno Setup QuickStart Pack 5.1.7+(http://www.jrsoftware.org/isdl.php)
;                           note: the versions of Inno Setup and the QuickStart Pack should be identical to ensure proper function
;Directly Dependant Files:  Because this is an installer. It would be difficult to list and maintain each of the files referenced
;                           throughout the script in the header.  If you search this plain text script for a particular file in our Subversio repository and it
;                           doesn't appear then this script is not directly dependant on that file.
;Compilation Instructions:  1.  Open this file in Inno Setup or ISTool
;                           2.  Make sure Compression=LZMA/Ultra, InternalCompressLevel=Ultra, and SolidCompression=True these values are lowered during
;                               development to speed up compilation, however at release we want the intaller to be as strong as possible.
;                           3. Check all files are present:
;                                   -From ISTool Click
;                           4.  Compile the script: "Project" --> "Verify Files..."
;                                   -From Inno Setup "Click "Build" --> "Compile"
;                                   -From ISTool Click "Project" --> "Compile Setup"
;                           5.  The compiled installer will appear in the \InnoSetup\Output\ directory at currently should be around 1.5MBs in size.
;
; Installer To Do List:
; #  Make the Install7ZipDll() Function automatically work with future versions of Merge7zDLL (Use GetCurentFileName)
; #  Provide the option to or not to assign the Ctrl+Alt+M accelerator to WinMerge., make sure it's turned on for at least one icon
; #  Add WinMerge to the user's path so they can execute comparison's from a Dos Prompt (Cmd.exe/Command.exe)
; #  We need to unregister, and delete the ShellExtension Dll if the user doesn't want it, during installation

; #  Display integration options in gray rather than hiding them if the user doesn't have the application in question installed
; #  We need to ask those that have the RCLLocalization.dll in their plugins folder if they actually want it, their answer will need to be stored in the registry
; #  Write code to detect "\Programs\WinMerge\WinMerge" type start menu installs
;
; Custom Installer Pages:
; #  Bundle 7-Zip with WinMerge or provide on the fly download capability.
; #  Allow users to set their working directory via a custom installer page
;
; Things that make the user's life easier:
; #  Create instructions and a sample language file using the Inno Setup Translator Tool (http://www2.arnes.si/~sopjsimo/translator.html)
; #  Add "WinMerge is running would you like to close it now?" support with programmatic termination
;     -Note: We'll need to add a declares statement to our ISX code so that we can use FindWindowEx directly or a mutex search or two
; #  Rather than requiring users to restart we could just kill all intances of Explorer.exe, but we'll need to prompt the user first and restart it
;    once the ShellExtension.dll file has been added or removed.
;
; Non-Essential Features:
; #  See about getting a higher resolution copy of the Users's Guide.ico source art from somebody (A 32x32, and or 48x48 would be nice)
; #  Using the registry set the order our icons appear within their group in the start menu.:
;      1.  WinMerge
;      2.  Read Me
;      3.  Users's Guide
; #  Create the ability to install to two start menu groups simultaneously
;
; Not yet possible (Limited by Inno Setup):
; #  While uninstalling prompt the user as to whether or not they'd like to remove their WinMerge preferences too?

#define AppVersion GetFileVersion(SourcePath + "\..\..\Build\Release\WinMergeU.exe")
#define ShellExtensionVersion GetFileVersion(SourcePath + "..\..\Build\ShellExtension\ShellExtensionU.dll")

[Setup]
AppName=WinMerge
AppVersion={#AppVersion}
AppVerName=WinMerge {#AppVersion}
AppPublisher=Thingamahoochie Software
AppPublisherURL=https://WinMerge.org/
AppSupportURL=https://WinMerge.org/
AppUpdatesURL=https://WinMerge.org/

; Installer executable's version resource info
VersionInfoCompany=https://winmerge.org
VersionInfoDescription=WinMerge Installer
VersionInfoVersion={#AppVersion}

;This is in case an older version of the installer happened to be
DirExistsWarning=no

;Tells the installer to only display a select language dialog if the an exact match wasn't found
ShowLanguageDialog=auto

DefaultDirName={pf}\WinMerge
DefaultGroupName=WinMerge
DisableStartupPrompt=true
AllowNoIcons=true
InfoBeforeFile=..\..\Docs\users\GPL.rtf
InfoAfterFile=..\..\Docs\users\ReadMe.txt

OutputBaseFilename=WinMerge-{#AppVersion}-Setup

;This must be admin to install C++ Runtimes and shell extension
PrivilegesRequired=admin

;Windows 2000 or later required
MinVersion=0,5.0

UninstallDisplayIcon={app}\WinMergeU.exe

;Artwork References
WizardImageFile=Art\Large Logo.bmp
WizardSmallImageFile=Art\Small Logo.bmp
WizardImageStretch=true

;It is confusing, if Setup/Uninstall use the same icon like WinMerge!
;SetupIconFile=..\..\src\res\Merge.ico

;Compression Parameters
;Please note while Compression=lzma/ultra and InternalCompressLevel=Ultra are better than max
;they also require 320 MB of memory for compression. If you're system has at least 256MB RAM then by all
;means set it to ultra before compilation
Compression=lzma/ultra
InternalCompressLevel=ultra
SolidCompression=true

; Update file associations for shell (project files)
ChangesAssociations=true
; Updates PATH
ChangesEnvironment=true
OutputDir=..\..\Build
AlwaysShowComponentsList=true

;SignTool=signbat $f
;SignedUninstaller=yes

[Languages]
;Inno Setup's Native Language
Name: English; MessagesFile: compiler:Default.isl,..\..\Translations\InnoSetup\English.isl

;Localizations:
Name: Arabic; MessagesFile: ..\..\Translations\InnoSetup\Unbundled.is5\Arabic.isl,..\..\Translations\InnoSetup\Arabic.isl
Name: Basque; MessagesFile: ..\..\Translations\InnoSetup\Unbundled.is5\Basque.isl,..\..\Translations\InnoSetup\Basque.isl; InfoAfterFile: ..\..\Translations\Docs\Readme\ReadMe-Basque.txt
Name: Bulgarian; MessagesFile: ..\..\Translations\InnoSetup\Unbundled.is5\Bulgarian.isl,..\..\Translations\InnoSetup\Bulgarian.isl; InfoAfterFile: ..\..\Translations\Docs\Readme\ReadMe-Bulgarian.txt
Name: Catalan; MessagesFile: compiler:Languages\Catalan.isl,..\..\Translations\InnoSetup\Catalan.isl; InfoAfterFile: ..\..\Translations\Docs\Readme\ReadMe-Catalan.txt
Name: Chinese_Simplified; MessagesFile: ..\..\Translations\InnoSetup\Unbundled.is5\ChineseSimplified.isl,..\..\Translations\InnoSetup\Chinese_Simplified.isl; InfoAfterFile: ..\..\Translations\Docs\Readme\ReadMe-ChineseSimplified.txt
Name: Chinese_Traditional; MessagesFile: ..\..\Translations\InnoSetup\Unbundled.is5\ChineseTraditional.isl,..\..\Translations\InnoSetup\Chinese_Traditional.isl; InfoAfterFile: ..\..\Translations\Docs\Readme\ReadMe-ChineseTraditional.txt
Name: Croatian; MessagesFile: ..\..\Translations\InnoSetup\Unbundled.is5\Croatian.isl,..\..\Translations\InnoSetup\Croatian.isl; InfoAfterFile: ..\..\Translations\Docs\Readme\ReadMe-Croatian.txt
Name: Czech; MessagesFile: compiler:Languages\Czech.isl,..\..\Translations\InnoSetup\Czech.isl
Name: Danish; MessagesFile: compiler:Languages\Danish.isl,..\..\Translations\InnoSetup\Danish.isl
Name: Dutch; MessagesFile: compiler:Languages\Dutch.isl,..\..\Translations\InnoSetup\Dutch.isl; InfoAfterFile: ..\..\Translations\Docs\Readme\ReadMe-Dutch.txt
Name: Finnish; MessagesFile: compiler:Languages\Finnish.isl,..\..\Translations\InnoSetup\Finnish.isl; InfoAfterFile: ..\..\Translations\Docs\Readme\ReadMe-Finnish.txt
Name: French; MessagesFile: compiler:Languages\French.isl,..\..\Translations\InnoSetup\French.isl; InfoAfterFile: ..\..\Translations\Docs\Readme\ReadMe-French.txt
Name: Galician; MessagesFile: ..\..\Translations\InnoSetup\Unbundled.is5\Galician.isl,..\..\Translations\InnoSetup\Galician.isl; InfoAfterFile: ..\..\Translations\Docs\Readme\ReadMe-Galician.txt
Name: German; MessagesFile: compiler:Languages\German.isl,..\..\Translations\InnoSetup\German.isl; InfoAfterFile: ..\..\Translations\Docs\Readme\ReadMe-German.txt
Name: Greek; MessagesFile: compiler:Languages\Greek.isl,..\..\Translations\InnoSetup\Greek.isl; InfoAfterFile: ..\..\Translations\Docs\Readme\ReadMe-Greek.txt
Name: Hungarian; MessagesFile: compiler:Languages\Hungarian.isl,..\..\Translations\InnoSetup\Hungarian.isl
Name: Italian; MessagesFile: compiler:Languages\Italian.isl,..\..\Translations\InnoSetup\Italian.isl; InfoAfterFile: ..\..\Translations\Docs\Readme\ReadMe-Italian.txt
Name: Japanese; MessagesFile: compiler:Languages\Japanese.isl,..\..\Translations\InnoSetup\Japanese.isl; InfoAfterFile: ..\..\Translations\Docs\Readme\ReadMe-Japanese.txt
Name: Korean; MessagesFile: ..\..\Translations\InnoSetup\Unbundled.is5\Korean.isl,..\..\Translations\InnoSetup\Korean.isl
Name: Lithuanian; MessagesFile: ..\..\Translations\InnoSetup\Unbundled.is5\Lithuanian.isl,..\..\Translations\InnoSetup\Lithuanian.isl; InfoAfterFile: ..\..\Translations\Docs\Readme\ReadMe-Lithuanian.txt
Name: Norwegian; MessagesFile: compiler:Languages\Norwegian.isl,..\..\Translations\InnoSetup\Norwegian.isl
Name: Persian; MessagesFile: ..\..\Translations\InnoSetup\Unbundled.is5\Farsi.isl,..\..\Translations\InnoSetup\Persian.isl
Name: Polish; MessagesFile: compiler:Languages\Polish.isl,..\..\Translations\InnoSetup\Polish.isl
Name: Portuguese; MessagesFile: compiler:Languages\Portuguese.isl,..\..\Translations\InnoSetup\Portuguese.isl; InfoAfterFile: ..\..\Translations\Docs\Readme\ReadMe-Portuguese.txt
Name: PortugueseBrazilian; MessagesFile: compiler:Languages\BrazilianPortuguese.isl,..\..\Translations\InnoSetup\Brazilian.isl; InfoAfterFile: ..\..\Translations\Docs\Readme\ReadMe-Brazilian.txt
Name: Romanian; MessagesFile: ..\..\Translations\InnoSetup\Unbundled.is5\Romanian.isl,..\..\Translations\InnoSetup\Romanian.isl; InfoAfterFile: ..\..\Translations\Docs\Readme\ReadMe-Romanian.txt
Name: Russian; MessagesFile: compiler:Languages\Russian.isl,..\..\Translations\InnoSetup\Russian.isl
Name: Serbian; MessagesFile: compiler:Languages\SerbianCyrillic.isl,..\..\Translations\InnoSetup\Serbian.isl; InfoAfterFile: ..\..\Translations\Docs\Readme\ReadMe-Serbian.txt
Name: Sinhala; MessagesFile: ..\..\Translations\InnoSetup\Unbundled.is5\Sinhala.islu,..\..\Translations\InnoSetup\Sinhala.islu
Name: Slovak; MessagesFile: ..\..\Translations\InnoSetup\Unbundled.is5\Slovak.isl,..\..\Translations\InnoSetup\Slovak.isl; InfoAfterFile: ..\..\Translations\Docs\Readme\ReadMe-Slovak.txt
Name: Slovenian; MessagesFile: compiler:Languages\Slovenian.isl,..\..\Translations\InnoSetup\Slovenian.isl; InfoAfterFile: ..\..\Translations\Docs\Readme\ReadMe-Slovenian.txt
Name: Spanish; MessagesFile: compiler:Languages\Spanish.isl,..\..\Translations\InnoSetup\Spanish.isl; InfoAfterFile: ..\..\Translations\Docs\Readme\ReadMe-Spanish.txt
Name: Swedish; MessagesFile: ..\..\Translations\InnoSetup\Unbundled.is5\Swedish.isl,..\..\Translations\InnoSetup\Swedish.isl; InfoAfterFile: ..\..\Translations\Docs\Readme\ReadMe-Swedish.txt
Name: Turkish; MessagesFile: compiler:Languages\Turkish.isl,..\..\Translations\InnoSetup\Turkish.isl; InfoAfterFile: ..\..\Translations\Docs\Readme\ReadMe-Turkish.txt
Name: Ukrainian; MessagesFile: compiler:Languages\Ukrainian.isl,..\..\Translations\InnoSetup\Ukrainian.isl; InfoAfterFile: ..\..\Translations\Docs\Readme\ReadMe-Ukrainian.txt


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
; Executable, libraries, documentation
Name: Core; Description: {cm:AppCoreFiles}; Types: full custom typical compact; Flags: fixed

Name: filters; Description: {cm:Filters}; Flags: disablenouninstallwarning; Types: full typical
Name: Plugins; Description: {cm:Plugins}; Flags: disablenouninstallwarning; Types: full typical
Name: Frhed; Description: {cm:Frhed}; Flags: disablenouninstallwarning; Types: full typical
Name: WinIMerge; Description: {cm:WinIMerge}; Flags: disablenouninstallwarning; Types: full typical
Name: ArchiveSupport; Description: {cm:ArchiveSupport}; Flags: disablenouninstallwarning; Types: full typical
Name: Patch; Description: {cm:Patch}; Flags: disablenouninstallwarning; Types: full typical

;Language components
Name: Languages; Description: {cm:Languages}; Flags: disablenouninstallwarning
Name: Languages\Arabic; Description: {cm:ArabicLanguage}; Flags: disablenouninstallwarning; Types: full; Languages: not Arabic
Name: Languages\Arabic; Description: {cm:ArabicLanguage}; Flags: disablenouninstallwarning; Types: full typical compact; Languages: Arabic

Name: Languages\Basque; Description: {cm:BasqueLanguage}; Flags: disablenouninstallwarning; Types: full; Languages: not Basque
Name: Languages\Basque; Description: {cm:BasqueLanguage}; Flags: disablenouninstallwarning; Types: full typical compact; Languages: Basque

Name: Languages\Bulgarian; Description: {cm:BulgarianLanguage}; Flags: disablenouninstallwarning; Types: full; Languages: not Bulgarian
Name: Languages\Bulgarian; Description: {cm:BulgarianLanguage}; Flags: disablenouninstallwarning; Types: full typical compact; Languages: Bulgarian

Name: Languages\Catalan; Description: {cm:CatalanLanguage}; Flags: disablenouninstallwarning; Types: full; Languages: not Catalan
Name: Languages\Catalan; Description: {cm:CatalanLanguage}; Flags: disablenouninstallwarning; Types: full typical compact; Languages: Catalan

Name: Languages\Chinese_Simplified; Description: {cm:ChineseSimplifiedLanguage}; Flags: disablenouninstallwarning; Types: full; Languages: not Chinese_Simplified
Name: Languages\Chinese_Simplified; Description: {cm:ChineseSimplifiedLanguage}; Flags: disablenouninstallwarning; Types: full typical compact; Languages: Chinese_Simplified

Name: Languages\Chinese_Traditional; Description: {cm:ChineseTraditionalLanguage}; Flags: disablenouninstallwarning; Types: full; Languages: not Chinese_Traditional
Name: Languages\Chinese_Traditional; Description: {cm:ChineseTraditionalLanguage}; Flags: disablenouninstallwarning; Types: full typical compact; Languages: Chinese_Traditional

Name: Languages\Croatian; Description: {cm:CroatianLanguage}; Flags: disablenouninstallwarning; Types: full; Languages: not Croatian
Name: Languages\Croatian; Description: {cm:CroatianLanguage}; Flags: disablenouninstallwarning; Types: full typical compact; Languages: Croatian

Name: Languages\Czech; Description: {cm:CzechLanguage}; Flags: disablenouninstallwarning; Types: full; Languages: not Czech
Name: Languages\Czech; Description: {cm:CzechLanguage}; Flags: disablenouninstallwarning; Types: full typical compact; Languages: Czech

Name: Languages\Danish; Description: {cm:DanishLanguage}; Flags: disablenouninstallwarning; Types: full; Languages: not Danish
Name: Languages\Danish; Description: {cm:DanishLanguage}; Flags: disablenouninstallwarning; Types: full typical compact; Languages: Danish

Name: Languages\Dutch; Description: {cm:DutchLanguage}; Flags: disablenouninstallwarning; Types: full; Languages: not Dutch
Name: Languages\Dutch; Description: {cm:DutchLanguage}; Flags: disablenouninstallwarning; Types: full typical compact; Languages: Dutch

Name: Languages\Finnish; Description: {cm:FinnishLanguage}; Flags: disablenouninstallwarning; Types: full; Languages: not Finnish
Name: Languages\Finnish; Description: {cm:FinnishLanguage}; Flags: disablenouninstallwarning; Types: full typical compact; Languages: Finnish

Name: Languages\French; Description: {cm:FrenchLanguage}; Flags: disablenouninstallwarning; Types: full; Languages: not French
Name: Languages\French; Description: {cm:FrenchLanguage}; Flags: disablenouninstallwarning; Types: full typical compact; Languages: French

Name: Languages\Galician; Description: {cm:GalicianLanguage}; Flags: disablenouninstallwarning; Types: full; Languages: not Galician
Name: Languages\Galician; Description: {cm:GalicianLanguage}; Flags: disablenouninstallwarning; Types: full typical compact; Languages: Galician

Name: Languages\German; Description: {cm:GermanLanguage}; Flags: disablenouninstallwarning; Types: full; Languages: not German
Name: Languages\German; Description: {cm:GermanLanguage}; Flags: disablenouninstallwarning; Types: full typical compact; Languages: German

Name: Languages\Greek; Description: {cm:GreekLanguage}; Flags: disablenouninstallwarning; Types: full; Languages: not Greek
Name: Languages\Greek; Description: {cm:GreekLanguage}; Flags: disablenouninstallwarning; Types: full typical compact; Languages: Greek

Name: Languages\Hungarian; Description: {cm:HungarianLanguage}; Flags: disablenouninstallwarning; Types: full; Languages: not Hungarian
Name: Languages\Hungarian; Description: {cm:HungarianLanguage}; Flags: disablenouninstallwarning; Types: full typical compact; Languages: Hungarian

Name: Languages\Italian; Description: {cm:ItalianLanguage}; Flags: disablenouninstallwarning; Types: full; Languages: not Italian
Name: Languages\Italian; Description: {cm:ItalianLanguage}; Flags: disablenouninstallwarning; Types: full typical compact; Languages: Italian

Name: Languages\Japanese; Description: {cm:JapaneseLanguage}; Flags: disablenouninstallwarning; Types: full; Languages: not Japanese
Name: Languages\Japanese; Description: {cm:JapaneseLanguage}; Flags: disablenouninstallwarning; Types: full typical compact; Languages: Japanese

Name: Languages\Korean; Description: {cm:KoreanLanguage}; Flags: disablenouninstallwarning; Types: full; Languages: not Korean
Name: Languages\Korean; Description: {cm:KoreanLanguage}; Flags: disablenouninstallwarning; Types: full typical compact; Languages: Korean

Name: Languages\Lithuanian; Description: {cm:LithuanianLanguage}; Flags: disablenouninstallwarning; Types: full; Languages: not Lithuanian
Name: Languages\Lithuanian; Description: {cm:LithuanianLanguage}; Flags: disablenouninstallwarning; Types: full typical compact; Languages: Lithuanian

Name: Languages\Norwegian; Description: {cm:NorwegianLanguage}; Flags: disablenouninstallwarning; Types: full; Languages: not Norwegian
Name: Languages\Norwegian; Description: {cm:NorwegianLanguage}; Flags: disablenouninstallwarning; Types: full typical compact; Languages: Norwegian

Name: Languages\Persian; Description: {cm:PersianLanguage}; Flags: disablenouninstallwarning; Types: full; Languages: not Persian
Name: Languages\Persian; Description: {cm:PersianLanguage}; Flags: disablenouninstallwarning; Types: full typical compact; Languages: Persian

Name: Languages\Polish; Description: {cm:PolishLanguage}; Flags: disablenouninstallwarning; Types: full; Languages: not Polish
Name: Languages\Polish; Description: {cm:PolishLanguage}; Flags: disablenouninstallwarning; Types: full typical compact; Languages: Polish

Name: Languages\Portuguese; Description: {cm:PortugueseLanguage}; Flags: disablenouninstallwarning; Types: full; Languages: not Portuguese
Name: Languages\Portuguese; Description: {cm:PortugueseLanguage}; Flags: disablenouninstallwarning; Types: full typical compact; Languages: Portuguese

Name: Languages\PortugueseBrazilian; Description: {cm:PortugueseBrazilLanguage}; Flags: disablenouninstallwarning; Types: full; Languages: not PortugueseBrazilian
Name: Languages\PortugueseBrazilian; Description: {cm:PortugueseBrazilLanguage}; Flags: disablenouninstallwarning; Types: full typical compact; Languages: PortugueseBrazilian

Name: Languages\Romanian; Description: {cm:RomanianLanguage}; Flags: disablenouninstallwarning; Types: full; Languages: not Romanian
Name: Languages\Romanian; Description: {cm:RomanianLanguage}; Flags: disablenouninstallwarning; Types: full typical compact; Languages: Romanian

Name: Languages\Russian; Description: {cm:RussianLanguage}; Flags: disablenouninstallwarning; Types: full; Languages: not Russian
Name: Languages\Russian; Description: {cm:RussianLanguage}; Flags: disablenouninstallwarning; Types: full typical compact; Languages: Russian

Name: Languages\Serbian; Description: {cm:SerbianLanguage}; Flags: disablenouninstallwarning; Types: full; Languages: not Serbian
Name: Languages\Serbian; Description: {cm:SerbianLanguage}; Flags: disablenouninstallwarning; Types: full typical compact; Languages: Serbian

Name: Languages\Sinhala; Description: {cm:SinhalaLanguage}; Flags: disablenouninstallwarning; Types: full; Languages: not Sinhala
Name: Languages\Sinhala; Description: {cm:SinhalaLanguage}; Flags: disablenouninstallwarning; Types: full typical compact; Languages: Sinhala

Name: Languages\Slovak; Description: {cm:SlovakLanguage}; Flags: disablenouninstallwarning; Types: full; Languages: not Slovak
Name: Languages\Slovak; Description: {cm:SlovakLanguage}; Flags: disablenouninstallwarning; Types: full typical compact; Languages: Slovak

Name: Languages\Slovenian; Description: {cm:SlovenianLanguage}; Flags: disablenouninstallwarning; Types: full; Languages: not Slovenian
Name: Languages\Slovenian; Description: {cm:SlovenianLanguage}; Flags: disablenouninstallwarning; Types: full typical compact; Languages: Slovenian

Name: Languages\Spanish; Description: {cm:SpanishLanguage}; Flags: disablenouninstallwarning; Types: full; Languages: not Spanish
Name: Languages\Spanish; Description: {cm:SpanishLanguage}; Flags: disablenouninstallwarning; Types: full typical compact; Languages: Spanish

Name: Languages\Swedish; Description: {cm:SwedishLanguage}; Flags: disablenouninstallwarning; Types: full; Languages: not Swedish
Name: Languages\Swedish; Description: {cm:SwedishLanguage}; Flags: disablenouninstallwarning; Types: full typical compact; Languages: Swedish

Name: Languages\Turkish; Description: {cm:TurkishLanguage}; Flags: disablenouninstallwarning; Types: full; Languages: not Turkish
Name: Languages\Turkish; Description: {cm:TurkishLanguage}; Flags: disablenouninstallwarning; Types: full typical compact; Languages: Turkish

Name: Languages\Ukrainian; Description: {cm:UkrainianLanguage}; Flags: disablenouninstallwarning; Types: full; Languages: not Ukrainian
Name: Languages\Ukrainian; Description: {cm:UkrainianLanguage}; Flags: disablenouninstallwarning; Types: full typical compact; Languages: Ukrainian


[Tasks]
Name: ShellExtension; Description: {cm:ExplorerContextMenu}; GroupDescription: {cm:OptionalFeatures}
Name: modifypath; Description: {cm:AddToPath}; GroupDescription: {cm:OptionalFeatures}; Flags: unchecked
Name: TortoiseCVS; Description: {cm:IntegrateTortoiseCVS}; GroupDescription: {cm:OptionalFeatures}; Check: TortoiseCVSInstalled
Name: TortoiseGit; Description: {cm:IntegrateTortoiseGit}; GroupDescription: {cm:OptionalFeatures}; Check: TortoiseGitInstalled; MinVersion: 0,5.0.2195sp3
Name: TortoiseSVN; Description: {cm:IntegrateTortoiseSVN}; GroupDescription: {cm:OptionalFeatures}; Check: TortoiseSVNInstalled; MinVersion: 0,5.0.2195sp3
Name: desktopicon; Description: {cm:CreateDesktopIcon}; GroupDescription: {cm:AdditionalIcons}; Flags: unchecked

[InstallDelete]
; Diff.txt is a file left over from previous versions of WinMerge (before version 2.0), we just delete it to be nice.
Type: files; Name: {app}\Diff.txt

;All of these files are removed so if the user upgrades their operating system or changes their language selections only the
;necessary files will be left in the installation folder
;Another reason these files might be strays is if a user extracted one of the experimental builds such as:
;WinMerge.{#AppVersion}-exe.7z.
Name: {app}\WinMerge.exe; Type: files
Name: {app}\WinMergeU.exe; Type: files; MinVersion: 0, 4

;Remove manifest files as we don't need them in 2.6.2 / 2.7.1.1 or later
Name: {app}\WinMerge.exe.manifest; Type: files
Name: {app}\WinMergeU.exe.manifest; Type: files

Name: {app}\Merge7z311.dll; Type: files
Name: {app}\Merge7z311U.dll; Type: files; MinVersion: 0, 4

Name: {app}\Merge7z312.dll; Type: files
Name: {app}\Merge7z312U.dll; Type: files; MinVersion: 0, 4

Name: {app}\Merge7z313.dll; Type: files
Name: {app}\Merge7z313U.dll; Type: files; MinVersion: 0, 4

Name: {app}\Merge7z420.dll; Type: files
Name: {app}\Merge7z420U.dll; Type: files; MinVersion: 0, 4

Name: {app}\Merge7z423.dll; Type: files
Name: {app}\Merge7z423U.dll; Type: files; MinVersion: 0, 4

Name: {app}\Merge7z431.dll; Type: files
Name: {app}\Merge7z431U.dll; Type: files; MinVersion: 0, 4

Name: {app}\Merge7z432.dll; Type: files
Name: {app}\Merge7z432U.dll; Type: files; MinVersion: 0, 4

Name: {app}\Merge7z442.dll; Type: files
Name: {app}\Merge7z442U.dll; Type: files; MinVersion: 0, 4

Name: {app}\Merge7z457.dll; Type: files
Name: {app}\Merge7z457U.dll; Type: files; MinVersion: 0, 4

Name: {app}\Merge7z465.dll; Type: files
Name: {app}\Merge7z465U.dll; Type: files; MinVersion: 0, 4

Name: {app}\Merge7z920.dll; Type: files
Name: {app}\Merge7z920U.dll; Type: files; MinVersion: 0, 4

;This won't work, because the file has to be unregistered, and explorer closed, first.
;Name: {app}\ShellExtension.dll; Type: files; Check: TaskDisabled('ShellExtension')

; Remove existing .lang files - we don't need them anymore as we are
; using PO files now.
Name: {app}\Languages\MergeBrazilian.lang; Type: files
Name: {app}\Languages\MergeBulgarian.lang; Type: files
Name: {app}\Languages\MergeCatalan.lang; Type: files
Name: {app}\Languages\MergeChineseSimplified.lang; Type: files
Name: {app}\Languages\MergeChineseTraditional.lang; Type: files
Name: {app}\Languages\MergeCzech.lang; Type: files
Name: {app}\Languages\MergeDanish.lang; Type: files
Name: {app}\Languages\MergeDutch.lang; Type: files
Name: {app}\Languages\MergeFrench.lang; Type: files
Name: {app}\Languages\MergeGerman.lang; Type: files
Name: {app}\Languages\MergeHungarian.lang; Type: files
Name: {app}\Languages\MergeItalian.lang; Type: files
Name: {app}\Languages\MergeJapanese.lang; Type: files
Name: {app}\Languages\MergeKorean.lang; Type: files
Name: {app}\Languages\MergeNorwegian.lang; Type: files
Name: {app}\Languages\MergePersian.lang; Type: files
Name: {app}\Languages\MergePolish.lang; Type: files
Name: {app}\Languages\MergePortuguese.lang; Type: files
Name: {app}\Languages\MergeSlovak.lang; Type: files
Name: {app}\Languages\MergeSpanish.lang; Type: files
Name: {app}\Languages\MergeRussian.lang; Type: files
Name: {app}\Languages\MergeSerbian.lang; Type: files
Name: {app}\Languages\MergeSwedish.lang; Type: files
Name: {app}\Languages\MergeTurkish.lang; Type: files
Name: {app}\MergePlugins\list.txt; Type: files; Check: not IsComponentSelected('Plugins')

;Removes the user's guide icon if the user deselects the user's guide component.
Name: {group}\{cm:UsersGuide}.lnk; Type: files; Check: not IsComponentSelected('Docs')
Name: {group}\{cm:ReadMe}.lnk; Type: files

;This removes the desktop icon in case the user chooses not to install it after previously having it installed
Name: {commondesktop}\WinMerge.lnk; Type: files; Check: not IsTaskSelected('DesktopIcon')

;Removes the Uninstall icon from the start menu...
Name: {group}\{cm:UninstallProgram,WinMerge}.lnk; Type: files;
Name: {group}\{cm:UninstallProgram,WinMerge}; Type: files;

;Remove ANSI executable link from start menu for NT-based Windows versions
;This was installed earlier, but not anymore.
Name: {group}\WinMerge (ANSI).lnk; Type: files; MinVersion: 0,4

Name: {app}\Docs; Type: filesandordirs

Name: {app}\MergePlugins\editor addin.sct; Type: Files; Check: not IsComponentSelected('Plugins')
Name: {app}\MergePlugins\insert datetime.sct; Type: Files; Check: not IsComponentSelected('Plugins')
Name: {app}\MergePlugins\CompareMSExcelFiles.dll; Type: Files; Check: IsComponentSelected('Plugins')
Name: {app}\MergePlugins\CompareMSWordFiles.dll; Type: Files; Check: IsComponentSelected('Plugins')
Name: {app}\MergePlugins; Type: DirIfEmpty; Check: not IsComponentSelected('Plugins')

Name: {app}\Filters\ADAMulti.flt; Type: Files; Check: not IsComponentSelected('Filters')
Name: {app}\Filters\ASPNET.flt; Type: Files; Check: not IsComponentSelected('Filters')
Name: {app}\Filters\CSharp_loose.flt; Type: Files; Check: not IsComponentSelected('Filters')
Name: {app}\Filters\Delphi.flt; Type: Files; Check: not IsComponentSelected('Filters')
Name: {app}\Filters\MASM.flt; Type: Files; Check: not IsComponentSelected('Filters')
Name: {app}\Filters\Merge_GnuC_loose.flt; Type: Files; Check: not IsComponentSelected('Filters')
Name: {app}\Filters\Merge_VC_loose.flt; Type: Files; Check: not IsComponentSelected('Filters')
Name: {app}\Filters\Merge_VB_loose.flt; Type: Files; Check: not IsComponentSelected('Filters')
Name: {app}\Filters\SourceControl.flt; Type: Files; Check: not IsComponentSelected('Filters')
Name: {app}\Filters\Symbian.flt; Type: Files; Check: not IsComponentSelected('Filters')
Name: {app}\Filters\XML_html.flt; Type: Files; Check: not IsComponentSelected('Filters')
Name: {app}\Filters\FileFilter.tmpl; Type: Files; Check: not IsComponentSelected('Filters')
Name: {app}\Filters; Type: DirIfEmpty; Check: not IsComponentSelected('Filters')

;Remove old "List of installed files"...
Name: {app}\Files.txt; Type: files


[Dirs]
;The always uninstall flag tells the uninstaller to remove the folder if it's empty regardless of whether or not it existed prior to the installation
Name: {app}; Flags: uninsalwaysuninstall


[Files]
; WinMerge itself
Source: ..\..\Build\Release\WinMergeU.exe; DestDir: {app}; Flags: promptifolder; Components: Core
; Visual Elements
Source: ..\..\Build\Release\WinMergeU.VisualElementsManifest.xml; DestDir: {app}; Flags: promptifolder; Components: Core
Source: ..\..\Build\Release\LogoImages\*.png; DestDir: {app}\LogoImages; Flags: promptifolder; Components: Core

; Shell extension
Source: ..\..\Build\ShellExtension\ShellExtensionU.dll; DestDir: {app}; Flags: regserver uninsrestartdelete restartreplace promptifolder; MinVersion: 0, 4; Check: not IsWin64 and not AreSourceAndDestinationOfShellExtensionSame(ExpandConstant('{app}\ShellExtensionU.dll'))
; 64-bit version of ShellExtension
Source: ..\..\Build\ShellExtension\ShellExtensionX64.dll; DestDir: {app}; Flags: regserver uninsrestartdelete restartreplace promptifolder 64bit; MinVersion: 0,5.01.2600; Check: IsWin64 and not AreSourceAndDestinationOfShellExtensionSame(ExpandConstant('{app}\ShellExtensionX64.dll'))

; ArchiveSupport
;Please do not reorder the 7z Dlls by version they compress better ordered by platform and then by version
Source: ..\..\Build\Merge7z\Merge7z.dll; DestDir: {app}\Merge7z; Flags: promptifolder replacesameversion; MinVersion: 0, 4; Components: ArchiveSupport
Source: ..\..\Build\Merge7z\7z.dll; DestDir: {app}\Merge7z; Flags: promptifolder; MinVersion: 0, 4; Components: ArchiveSupport
Source: ..\..\Build\Merge7z\*.txt; DestDir: {app}\Merge7z; Flags: promptifolder; MinVersion: 0, 4; Components: ArchiveSupport
Source: ..\..\Build\Merge7z\Lang\*.txt; DestDir: {app}\Merge7z\Lang; Flags: promptifolder; MinVersion: 0, 4; Components: ArchiveSupport

; Language files
Source: ..\..\Translations\WinMerge\Arabic.po; DestDir: {app}\Languages; Components: Languages\Arabic; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\WinMerge\Basque.po; DestDir: {app}\Languages; Components: Languages\Basque; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\Docs\Readme\ReadMe-Basque.txt; DestDir: {app}\Docs; Components: Languages\Basque
Source: ..\..\Translations\WinMerge\Brazilian.po; DestDir: {app}\Languages; Components: Languages\PortugueseBrazilian; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\Docs\Readme\ReadMe-Brazilian.txt; DestDir: {app}\Docs; Components: Languages\PortugueseBrazilian
Source: ..\..\Translations\WinMerge\Bulgarian.po; DestDir: {app}\Languages; Components: Languages\Bulgarian; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\Docs\Readme\ReadMe-Bulgarian.txt; DestDir: {app}\Docs; Components: Languages\Bulgarian
Source: ..\..\Translations\WinMerge\Catalan.po; DestDir: {app}\Languages; Components: Languages\Catalan; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\Docs\Readme\ReadMe-Catalan.txt; DestDir: {app}\Docs; Components: Languages\Catalan
Source: ..\..\Translations\WinMerge\ChineseSimplified.po; DestDir: {app}\Languages; Components: Languages\Chinese_Simplified; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\Docs\Readme\ReadMe-ChineseSimplified.txt; DestDir: {app}\Docs; Components: Languages\Chinese_Simplified
Source: ..\..\Translations\WinMerge\ChineseTraditional.po; DestDir: {app}\Languages; Components: Languages\Chinese_Traditional; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\Docs\Readme\ReadMe-ChineseTraditional.txt; DestDir: {app}\Docs; Components: Languages\Chinese_Traditional
Source: ..\..\Translations\WinMerge\Croatian.po; DestDir: {app}\Languages; Components: Languages\Croatian; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\Docs\Readme\ReadMe-Croatian.txt; DestDir: {app}\Docs; Components: Languages\Croatian
Source: ..\..\Translations\WinMerge\Czech.po; DestDir: {app}\Languages; Components: Languages\Czech; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\WinMerge\Danish.po; DestDir: {app}\Languages; Components: Languages\Danish; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\WinMerge\Dutch.po; DestDir: {app}\Languages; Components: Languages\Dutch; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\Docs\Readme\ReadMe-Dutch.txt; DestDir: {app}\Docs; Components: Languages\Dutch
Source: ..\..\Translations\WinMerge\Finnish.po; DestDir: {app}\Languages; Components: Languages\Finnish; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\Docs\Readme\ReadMe-Finnish.txt; DestDir: {app}\Docs; Components: Languages\Finnish
Source: ..\..\Translations\WinMerge\French.po; DestDir: {app}\Languages; Components: Languages\French; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\Docs\Readme\ReadMe-French.txt; DestDir: {app}\Docs; Components: Languages\French
Source: ..\..\Translations\WinMerge\Galician.po; DestDir: {app}\Languages; Components: Languages\Galician; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\Docs\Readme\ReadMe-Galician.txt; DestDir: {app}\Docs; Components: Languages\Galician
Source: ..\..\Translations\WinMerge\German.po; DestDir: {app}\Languages; Components: Languages\German; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\Docs\Readme\ReadMe-German.txt; DestDir: {app}\Docs; Components: Languages\German
Source: ..\..\Translations\WinMerge\Greek.po; DestDir: {app}\Languages; Components: Languages\Greek; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\Docs\Readme\ReadMe-Greek.txt; DestDir: {app}\Docs; Components: Languages\Greek
Source: ..\..\Translations\WinMerge\Hungarian.po; DestDir: {app}\Languages; Components: Languages\Hungarian; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\WinMerge\Italian.po; DestDir: {app}\Languages; Components: Languages\Italian; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\Docs\Readme\ReadMe-Italian.txt; DestDir: {app}\Docs; Components: Languages\Italian
Source: ..\..\Translations\WinMerge\Japanese.po; DestDir: {app}\Languages; Components: Languages\Japanese; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\Docs\Readme\ReadMe-Japanese.txt; DestDir: {app}\Docs; Components: Languages\Japanese
Source: ..\..\Build\Manual\htmlhelp\WinMergeJapanese.chm; DestDir: {app}\Docs; Components: Languages\Japanese
Source: ..\..\Translations\WinMerge\Korean.po; DestDir: {app}\Languages; Components: Languages\Korean; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\WinMerge\Lithuanian.po; DestDir: {app}\Languages; Components: Languages\Lithuanian; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\Docs\Readme\ReadMe-Lithuanian.txt; DestDir: {app}\Docs; Components: Languages\Lithuanian
Source: ..\..\Translations\WinMerge\Norwegian.po; DestDir: {app}\Languages; Components: Languages\Norwegian; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\WinMerge\Persian.po; DestDir: {app}\Languages; Components: Languages\Persian; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\WinMerge\Polish.po; DestDir: {app}\Languages; Components: Languages\Polish; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\WinMerge\Portuguese.po; DestDir: {app}\Languages; Components: Languages\Portuguese; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\Docs\Readme\ReadMe-Portuguese.txt; DestDir: {app}\Docs; Components: Languages\Portuguese
Source: ..\..\Translations\WinMerge\Romanian.po; DestDir: {app}\Languages; Components: Languages\Romanian; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\Docs\Readme\ReadMe-Romanian.txt; DestDir: {app}\Docs; Components: Languages\Romanian
Source: ..\..\Translations\WinMerge\Russian.po; DestDir: {app}\Languages; Components: Languages\Russian; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\WinMerge\Serbian.po; DestDir: {app}\Languages; Components: Languages\Serbian; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\Docs\Readme\ReadMe-Serbian.txt; DestDir: {app}\Docs; Components: Languages\Serbian
Source: ..\..\Translations\WinMerge\Sinhala.po; DestDir: {app}\Languages; Components: Languages\Sinhala; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\WinMerge\Slovak.po; DestDir: {app}\Languages; Components: Languages\Slovak; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\Docs\Readme\ReadMe-Slovak.txt; DestDir: {app}\Docs; Components: Languages\Slovak
Source: ..\..\Translations\WinMerge\Slovenian.po; DestDir: {app}\Languages; Components: Languages\Slovenian; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\Docs\Readme\ReadMe-Slovenian.txt; DestDir: {app}\Docs; Components: Languages\Slovenian
Source: ..\..\Translations\WinMerge\Spanish.po; DestDir: {app}\Languages; Components: Languages\Spanish; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\Docs\Readme\ReadMe-Spanish.txt; DestDir: {app}\Docs; Components: Languages\Spanish
Source: ..\..\Translations\WinMerge\Swedish.po; DestDir: {app}\Languages; Components: Languages\Swedish; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\Docs\Readme\ReadMe-Swedish.txt; DestDir: {app}\Docs; Components: Languages\Swedish
Source: ..\..\Translations\WinMerge\Turkish.po; DestDir: {app}\Languages; Components: Languages\Turkish; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\Docs\Readme\ReadMe-Turkish.txt; DestDir: {app}\Docs; Components: Languages\Turkish
Source: ..\..\Translations\WinMerge\Ukrainian.po; DestDir: {app}\Languages; Components: Languages\Ukrainian; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\Docs\Readme\ReadMe-Ukrainian.txt; DestDir: {app}\Docs; Components: Languages\Ukrainian

;Filters
Source: ..\..\Filters\*.flt; DestDir: {app}\Filters; Flags: sortfilesbyextension comparetimestamp ignoreversion; Components: filters
Source: ..\..\Filters\FileFilter.tmpl; DestDir: {app}\Filters; Flags: sortfilesbyextension comparetimestamp ignoreversion; Components: filters

;ColorSchemes
Source: ..\..\ColorSchemes\*.ini; DestDir: {app}\ColorSchemes; Flags: sortfilesbyextension comparetimestamp ignoreversion

;Documentation
Source: ..\..\Docs\Users\ReadMe.txt; DestDir: {app}\Docs; Flags: comparetimestamp ignoreversion promptifolder; Components: Core
Source: ..\..\Docs\Users\Contributors.txt; DestDir: {app}; Flags: comparetimestamp ignoreversion promptifolder; Components: Core
Source: ..\..\Docs\Users\ReleaseNotes.html; DestDir: {app}\Docs; Flags: comparetimestamp ignoreversion promptifolder; Components: Core
Source: ..\..\Docs\Users\ChangeLog.html; DestDir: {app}\Docs; Flags: comparetimestamp ignoreversion promptifolder; Components: Core
Source: ..\..\Build\Manual\htmlhelp\WinMerge.chm; DestDir: {app}\Docs\; Flags: overwritereadonly uninsremovereadonly; Components: Core
Source: ..\..\Docs\users\GPL.rtf; DestDir: {app}\Docs\; Flags: comparetimestamp ignoreversion promptifolder; Components: Core

;Plugins
;Please note IgnoreVersion and CompareTimeStamp are to instruct the installer to not not check for version info and go straight to comparing modification dates
Source: ..\..\Plugins\dlls\editor addin.sct; DestDir: {app}\MergePlugins; Flags: IgnoreVersion CompareTimeStamp; Components: Plugins
Source: ..\..\Plugins\dlls\insert datetime.sct; DestDir: {app}\MergePlugins; Flags: IgnoreVersion CompareTimeStamp; Components: Plugins
Source: ..\..\Plugins\dlls\CompareMSExcelFiles.sct; DestDir: {app}\MergePlugins; Flags: IgnoreVersion CompareTimeStamp; Components: Plugins
Source: ..\..\Plugins\dlls\CompareMSWordFiles.sct; DestDir: {app}\MergePlugins; Flags: IgnoreVersion CompareTimeStamp; Components: Plugins
Source: ..\..\Plugins\dlls\CompareMSPowerPointFiles.sct; DestDir: {app}\MergePlugins; Flags: IgnoreVersion CompareTimeStamp; Components: Plugins
Source: ..\..\Plugins\dlls\CompareMSVisioFiles.sct; DestDir: {app}\MergePlugins; Flags: IgnoreVersion CompareTimeStamp; Components: Plugins
Source: ..\..\Plugins\dlls\ApplyPatch.sct; DestDir: {app}\MergePlugins; Flags: IgnoreVersion CompareTimeStamp; Components: Plugins
Source: ..\..\Plugins\dlls\PrediffLineFilter.sct; DestDir: {app}\MergePlugins; Flags: IgnoreVersion CompareTimeStamp; Components: Plugins
Source: ..\..\Plugins\dlls\IgnoreColumns.dll; DestDir: {app}\MergePlugins; Flags: ignoreversion replacesameversion; Components: Plugins
Source: ..\..\Plugins\dlls\IgnoreCommentsC.dll; DestDir: {app}\MergePlugins; Flags: ignoreversion replacesameversion; Components: Plugins
Source: ..\..\Plugins\dlls\IgnoreFieldsComma.dll; DestDir: {app}\MergePlugins; Flags: ignoreversion replacesameversion; Components: Plugins
Source: ..\..\Plugins\dlls\IgnoreFieldsTab.dll; DestDir: {app}\MergePlugins; Flags: ignoreversion replacesameversion; Components: Plugins
Source: ..\..\Plugins\dlls\IgnoreLeadingLineNumbers.dll; DestDir: {app}\MergePlugins; Flags: ignoreversion replacesameversion; Components: Plugins

;Frhed
Source: ..\..\Build\Frhed\GPL.txt; DestDir: {app}\Frhed; Components: Frhed
;Source: ..\..\Build\Frhed\frhed.exe; DestDir: {app}\Frhed; Components: Frhed
Source: ..\..\Build\Frhed\hekseditU.dll; DestDir: {app}\Frhed; Flags: ignoreversion replacesameversion; Components: Frhed
Source: ..\..\Build\Frhed\Docs\ChangeLog.txt; DestDir: {app}\Frhed\Docs; Components: Frhed
Source: ..\..\Build\Frhed\Docs\Contributors.txt; DestDir: {app}\Frhed\Docs; Components: Frhed
Source: ..\..\Build\Frhed\Docs\History.txt; DestDir: {app}\Frhed\Docs; Components: Frhed
Source: ..\..\Build\Frhed\Docs\Links.txt; DestDir: {app}\Frhed\Docs; Components: Frhed
Source: ..\..\Build\Frhed\Docs\Sample.tpl  ; DestDir: {app}\Frhed\Docs; Components: Frhed
Source: ..\..\Build\Frhed\Languages\de.po; DestDir: {app}\Frhed\Languages; Components: Frhed
Source: ..\..\Build\Frhed\Languages\fr.po; DestDir: {app}\Frhed\Languages; Components: Frhed
Source: ..\..\Build\Frhed\Languages\ja.po; DestDir: {app}\Frhed\Languages; Components: Frhed
Source: ..\..\Build\Frhed\Languages\nl.po; DestDir: {app}\Frhed\Languages; Components: Frhed
Source: ..\..\Build\Frhed\Languages\sl.po; DestDir: {app}\Frhed\Languages; Components: Frhed
Source: ..\..\Build\Frhed\Languages\heksedit.lng; DestDir: {app}\Frhed\Languages; Components: Frhed

;WinIMerge
Source: ..\..\Build\WinIMerge\GPL.txt; DestDir: {app}\WinIMerge; Components: WinIMerge
Source: ..\..\Build\WinIMerge\freeimage-license-gplv2.txt; DestDir: {app}\WinIMerge; Components: WinIMerge
;Source: ..\..\Build\WinIMerge\bin\WinIMerge.exe; DestDir: {app}\WinIMerge; Components: WinIMerge
Source: ..\..\Build\WinIMerge\bin\WinIMergeLib.dll; DestDir: {app}\WinIMerge; Flags: ignoreversion replacesameversion; Components: WinIMerge
Source: ..\..\Build\WinIMerge\bin\vcomp140.dll; DestDir: {app}; Components: WinIMerge

;GnuWin32 Patch for Windows
Source: ..\..\Build\GnuWin32\*.*; DestDir: {app}\GnuWin32; Flags: recursesubdirs; Components: Patch

[Dirs]
Name: "{app}\MergePlugins"

[Icons]
;Start Menu Icons
Name: {group}\WinMerge; Filename: {app}\WinMergeU.exe; AppUserModelID: "Thingamahoochie.WinMerge"
Name: {group}\{cm:UsersGuide}; Filename: {app}\Docs\WinMerge.chm

;Desktop Icon
Name: {commondesktop}\WinMerge; Filename: {app}\WinMergeU.exe; Tasks: desktopicon

[Registry]
Root: HKCU; Subkey: Software\Thingamahoochie; Flags: uninsdeletekeyifempty
Root: HKCU; Subkey: Software\Thingamahoochie\WinMerge; Flags: uninsdeletekey
Root: HKLM; Subkey: Software\Thingamahoochie; Flags: uninsdeletekeyifempty
Root: HKLM; Subkey: Software\Thingamahoochie\WinMerge; Flags: uninsdeletekey

;Add Project file extension (.WinMerge) to known extensions and
;set WinMerge to open those files
;set Notepad to edit project files
Root: HKCR; Subkey: .WinMerge; ValueType: String; ValueData: WinMerge.Project.File; Flags: uninsdeletekey
Root: HKCR; Subkey: WinMerge.Project.File; ValueType: String; ValueData: {cm:ProjectFileDesc}; Flags: uninsdeletekey
Root: HKCR; Subkey: WinMerge.Project.File\shell\open\command; ValueType: String; ValueData: """{app}\WinMergeU.exe"" ""%1"""; Flags: uninsdeletekey
Root: HKCR; Subkey: WinMerge.Project.File\shell\edit\command; ValueType: String; ValueData: """NOTEPAD.EXE"" ""%1"""; Flags: uninsdeletekey
Root: HKCR; Subkey: WinMerge.Project.File\DefaultIcon; ValueType: String; ValueData: """{app}\WinMergeU.exe"",1"; Flags: uninsdeletekey

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
Root: HKLM; SubKey: SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\WinMerge.exe; ValueType: string; ValueName: ; ValueData: {app}\WinMergeU.exe
Root: HKLM; SubKey: SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\WinMergeU.exe; ValueType: string; ValueName: ; ValueData: {app}\WinMergeU.exe

;Registry Keys for use by ShellExtension.dll
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge; ValueType: string; ValueName: Executable; ValueData: {app}\WinMergeU.exe
Root: HKCU; SubKey: Software\Thingamahoochie\WinMerge; ValueType: string; ValueName: Executable; ValueData: {app}\WinMergeU.exe

;Enables or disables the Context Menu preference based on what the user selects during install
;Initially the Context menu is explicitly disabled:
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge; ValueType: dword; ValueName: ContextMenuEnabled; ValueData: 0;

;If the user chose to use the context menu then we re-enable it.  This is necessary so it'll turn on and off not just on.
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge; ValueType: dword; ValueName: ContextMenuEnabled; ValueData: 1; Tasks: ShellExtension
Root: HKCU; SubKey: Software\Thingamahoochie\WinMerge; ValueType: dword; ValueName: ContextMenuEnabled; ValueData: {code:ShellMenuEnabled}

;If WinMerge.exe is installed then we'll automatically configure WinMerge as the differencing application
Root: HKCU; SubKey: Software\TortoiseCVS; ValueType: string; ValueName: External Diff Application; ValueData: {app}\WinMergeU.exe; Flags: uninsdeletevalue; Tasks: TortoiseCVS
Root: HKCU; SubKey: Software\TortoiseCVS; ValueType: dword; ValueName: DiffAsUnicode; ValueData: $00000001; Flags: uninsdeletevalue; Tasks: TortoiseCVS
Root: HKCU; SubKey: Software\TortoiseCVS\Prefs\External Diff Application; ValueType: string; ValueName: _; ValueData: {app}\WinMergeU.exe; Flags: uninsdeletevalue dontcreatekey; Tasks: TortoiseCVS
Root: HKCU; SubKey: Software\TortoiseCVS\Prefs\External Diff2 Params; ValueType: string; ValueName: _; ValueData: """%1"" ""%2"""; Flags: uninsdeletevalue dontcreatekey; Tasks: TortoiseCVS

;Tells TortoiseCVS to use WinMerge as its differencing application (this happens whether or not Tortoise is current installed, that way
;if it is installed at a later date this will automatically support it)
Root: HKCU; SubKey: Software\TortoiseCVS; ValueType: string; ValueName: External Merge Application; ValueData: {app}\WinMergeU.exe; Flags: uninsdeletevalue; Tasks: TortoiseCVS
Root: HKCU; SubKey: Software\TortoiseCVS; ValueType: dword; ValueName: MergeAsUnicode; ValueData: $00000001; Flags: uninsdeletevalue; Tasks: TortoiseCVS
Root: HKCU; SubKey: Software\TortoiseCVS\Prefs\External Merge Application; ValueType: string; ValueName: _; ValueData: {app}\WinMergeU.exe; Flags: uninsdeletevalue dontcreatekey; Tasks: TortoiseCVS
Root: HKCU; SubKey: Software\TortoiseCVS\Prefs\External Merge2 Params; ValueType: string; ValueName: _; ValueData: """%mine"" ""%yours"""; Flags: uninsdeletevalue dontcreatekey; Tasks: TortoiseCVS

;Set WinMerge as TortoiseGit diff tool
Root: HKCU; SubKey: Software\TortoiseGit; ValueType: string; ValueName: Diff; ValueData: {app}\WinMergeU.exe -e -ub -dl %bname -dr %yname %base %mine; Flags: uninsdeletevalue; Tasks: TortoiseGit
Root: HKCU; SubKey: Software\TortoiseGit; ValueType: string; ValueName: Merge; ValueData: {code:TortoiseSVNGitMergeToolCommandLine}; Flags: uninsdeletevalue; Check: UseAs3WayMergeTool

;Set WinMerge as TortoiseSVN diff tool
Root: HKCU; SubKey: Software\TortoiseSVN; ValueType: string; ValueName: Diff; ValueData: {app}\WinMergeU.exe -e -ub -dl %bname -dr %yname %base %mine; Flags: uninsdeletevalue; Tasks: TortoiseSVN
Root: HKCU; SubKey: Software\TortoiseSVN; ValueType: string; ValueName: Merge; ValueData: {code:TortoiseSVNGitMergeToolCommandLine}; Flags: uninsdeletevalue; Check: UseAs3WayMergeTool

;Whatever the user chooses at the [Select Setup Language] dialog should also determine what language WinMerge will start up in
;(unless the user already has a startup language specified)
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00001401; Flags: deletevalue; Languages: Arabic
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $0000042d; Flags: deletevalue; Languages: Basque
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000402; Flags: deletevalue; Languages: Bulgarian
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000403; Flags: deletevalue; Languages: Catalan
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000804; Flags: deletevalue; Languages: Chinese_Simplified
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000404; Flags: deletevalue; Languages: Chinese_Traditional
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $0000041a; Flags: deletevalue; Languages: Croatian
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000405; Flags: deletevalue; Languages: Czech
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000406; Flags: deletevalue; Languages: Danish
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000413; Flags: deletevalue; Languages: Dutch
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000409; Flags: deletevalue; Languages: English
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $0000040b; Flags: deletevalue; Languages: Finnish
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $0000040c; Flags: deletevalue; Languages: French
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000456; Flags: deletevalue; Languages: Galician
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000407; Flags: deletevalue; Languages: German
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000408; Flags: deletevalue; Languages: Greek
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $0000040e; Flags: deletevalue; Languages: Hungarian
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000410; Flags: deletevalue; Languages: Italian
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000411; Flags: deletevalue; Languages: Japanese
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000412; Flags: deletevalue; Languages: Korean
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000427; Flags: deletevalue; Languages: Lithuanian 
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000414; Flags: deletevalue; Languages: Norwegian
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000429; Flags: deletevalue; Languages: Persian
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000415; Flags: deletevalue; Languages: Polish
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000816; Flags: deletevalue; Languages: Portuguese
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000416; Flags: deletevalue; Languages: PortugueseBrazilian
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000418; Flags: deletevalue; Languages: Romanian
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000419; Flags: deletevalue; Languages: Russian
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000c1a; Flags: deletevalue; Languages: Serbian
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $0000045b; Flags: deletevalue; Languages: Sinhala
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $0000041b; Flags: deletevalue; Languages: Slovak
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000424; Flags: deletevalue; Languages: Slovenian
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000c0a; Flags: deletevalue; Languages: Spanish
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $0000041D; Flags: deletevalue; Languages: Swedish
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $0000041f; Flags: deletevalue; Languages: Turkish
Root: HKLM; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000422; Flags: deletevalue; Languages: Ukrainian


[Run]
;This will no longer appear unless the user chose to make a start menu group in the first place
Filename: {win}\Explorer.exe; Description: {cm:ViewStartMenuFolder}; Parameters: """{group}"""; Flags: waituntilidle postinstall skipifsilent unchecked; Check: GroupCreated

Filename: {app}\WinMergeU.exe; Description: {cm:LaunchProgram,WinMerge}; Flags: nowait postinstall skipifsilent runmaximized


[UninstallDelete]
;Remove 7-zip integration dlls possibly installed (by hand or using separate installer)
Name: {app}\Merge7z*.dll; Type: files
Name: {app}\7zip_pad.xml; Type: files
Name: {app}\Codecs; Type: filesandordirs
Name: {app}\Formats; Type: filesandordirs
Name: {app}\Lang; Type: filesandordirs

Name: {group}; Type: dirifempty
Name: {app}; Type: dirifempty


[Code]
Var
    g_CheckListBox: TNewCheckListBox;

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


{Determines whether or not TortoiseCVS is installed}
Function TortoiseCVSInstalled(): boolean;
Begin
	{This absolutely must remain as \CustomIcons, because our application used to create some TortoiseCVS keys even if the application wasn't installed!}
    Result := RegKeyExists(HKEY_LOCAL_MACHINE, 'SOFTWARE\TortoiseCVS\CustomIcons') or RegKeyExists(HKEY_LOCAL_MACHINE, 'SOFTWARE\TortoiseCVS\Icons');
End;

{Determines whether or not TortoiseGit is installed}
Function TortoiseGitInstalled(): boolean;
Begin
    Result := RegKeyExists(HKEY_LOCAL_MACHINE, 'SOFTWARE\TortoiseGit') or RegKeyExists(HKEY_LOCAL_MACHINE, 'SOFTWARE\Classes\Directory\Background\shellex\ContextMenuHandlers\TortoiseGit');
End;

{Determines whether or not TortoiseSVN is installed}
Function TortoiseSVNInstalled(): boolean;
Begin
    Result := RegKeyExists(HKEY_LOCAL_MACHINE, 'SOFTWARE\TortoiseSVN') or RegKeyExists(HKEY_LOCAL_MACHINE, 'SOFTWARE\Classes\Directory\Background\shellex\ContextMenuHandlers\TortoiseSVN');
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
Begin
    {Detects the previous start menu group's path, if any}
    strOld := OldGroup();

    {Detects the current start menu group's path, if any (not creating a group is a valid option)}
    strNew := ExpandConstant('{group}');

    {removes the start menu portion of the path from the group path making it match the format of strOld}
    StringChange(strNew, ExpandConstant('{commonprograms}\'), '')

    {if the user does have a previous start menu location then..}
    If strOld <> '' Then
        Begin
            {If the current and previous start menu locations are different then...}
            If Uppercase(strOld) <> UpperCase(strNew) Then
                Begin
                    strMessage := ExpandConstant('{cm:DeletePreviousStartMenu}');
                    strMessage := Format(strMessage, [strOld, strNew]);

                    {Display a dialog asking the user if they'd like to delete the previous start menu group}
                    {If they'd like to delete the previous start menu group then...}
                    If Msgbox(strMessage, mbConfirmation, mb_YesNo) = mrYes Then
                        Begin
                            strOld := ExpandConstant('{commonprograms}\') + strOld;
                            {Remove old start menu}
                            DelTree(strOld, True, True, True);
                        End;
                End;
        End;
End;

{This event procedure is queed each time the user changes pages within the installer}
Procedure CurPageChanged(CurPage: integer);
Begin
    {if the installer reaches the file copy page then...}
    If CurPage = wpInstalling Then
            {Delete the previous start menu group if the location has changed since the last install}
            DeletePreviousStartMenu;
End;

// Checks if context menu is already enabled for shell extension
// If so, we won't overwrite its existing value in [Registry] section
Function ShellMenuEnabled(Unused: string): string;
Var
  ContextMenuEnabled: DWORD;
Begin
  ContextMenuEnabled := 0;
  RegQueryDWORDValue(HKCU, 'Software\Thingamahoochie\WinMerge', 'ContextMenuEnabled', ContextMenuEnabled);
  if IsTaskSelected('ShellExtension') then
    Result := inttostr(ContextMenuEnabled or 1)
  else
    Result := '0';
End;

// Add WinMerge to system path.
// This requires certain order of things to work:
// #1 ModPathDir function must be first (it gets called by others)
// #2 include of modpath.iss so modpath code gets included
// #3 CurStepChanged and CurUninstallStepChanged procedures as they call
//    ModPath (in modpath.iss)
function ModPathDir(): TArrayOfString;
var
    Dir:	TArrayOfString;
begin
    setArrayLength(Dir, 1)
	Dir[0] := ExpandConstant('{app}');
	Result := Dir;
end;

function UseAs3WayMergeTool(): Boolean;
begin
    Result := g_CheckListBox.Checked[0];
end;

function TortoiseSVNGitMergeToolCommandLine(Unused: string): string;
var
    lmr: string;
    Args: string;
begin
    if g_CheckListBox.Checked[1] then begin
        lmr := 'r';
        Args := '/e /ub /fr /wl /wm /dl %bname /dm %tname /dr %yname  %base %theirs %mine /o %merged';
    end else if g_CheckListBox.Checked[2] then begin
        lmr := 'm';
        Args := '/e /ub /fm /wl /wr /dl %tname /dm %bname /dr %yname  %theirs %base %mine /o %merged';
    end else begin
        lmr := 'l';
        Args := '/e /ub /fl /wm /wr /dl %yname /dm %tname /dr %bname  %mine %theirs %base /o %merged';
    end;
    if g_CheckListBox.Checked[4] then begin
        Args := Args + ' /a' + lmr;
    end;
    Result := ExpandConstant('{app}\WinMergeU.exe') + ' ' + Args;
end;

function ThreeWayMergePage_ShouldSkipPage(Page: TWizardPage): Boolean;
begin
    Result := not (IsTaskSelected('TortoiseSVN') or IsTaskSelected('TortoiseGit'));
end;

#include "modpath.iss"

procedure CurStepChanged(CurStep: TSetupStep);
Begin
    if CurStep = ssPostInstall then
    begin
		if IsTaskSelected('modifypath') then
			ModPath();
    end;
End;

Procedure CurUninstallStepChanged(CurUninstallStep: TUninstallStep);
var
	appdir:			String;
	selectedTasks:	AnsiString;
Begin
	appdir := ExpandConstant('{app}')
	if CurUninstallStep = usUninstall then begin
		if LoadStringFromFile(appdir + '\uninsTasks.txt', selectedTasks) then
			if Pos('modifypath', selectedTasks) > 0 then
				ModPath();
		DeleteFile(appdir + '\uninsTasks.txt')
	end;
End;

function BooleanToString(Value : Boolean) : String; 
begin
  if Value then
    Result := 'true'
  else
    Result := 'false';
end;

function StringToBoolean(Value : String) : Boolean; 
begin
  if Value = 'true' then
    Result := true
  else
    Result := false;
end;

function AreSourceAndDestinationOfShellExtensionSame(Filename: String) : Boolean;
var
  ver: String;
begin
  ver := ''
  GetVersionNumbersString(Filename, ver);
  if ver = ExpandConstant('{#ShellExtensionVersion}') then
    Result := true
  else
    Result := false;
end;

procedure RegisterPreviousData(PreviousDataKey: Integer);
begin
  SetPreviousData(PreviousDataKey, 'UseAs3WayMergeTool', BooleanToString(g_CheckListBox.Checked[0]));
  SetPreviousData(PreviousDataKey, 'MergeAtRightPane', BooleanToString(g_CheckListBox.Checked[1]));
  SetPreviousData(PreviousDataKey, 'MergeAtCenterPane', BooleanToString(g_CheckListBox.Checked[2]));
  SetPreviousData(PreviousDataKey, 'MergeAtLeftPane', BooleanToString(g_CheckListBox.Checked[3]));
  SetPreviousData(PreviousDataKey, 'AutoMergeAtStartup', BooleanToString(g_CheckListBox.Checked[4]));
end;

function GetSysColor(ColorType: Integer): Integer;
external 'GetSysColor@user32.dll';

procedure InitializeWizard();
var
  Page: TWizardPage;
begin
  Page := CreateCustomPage(wpSelectTasks, ExpandConstant('{cm:ThreeWayMergeWizardPageCaption}'), ExpandConstant('{cm:ThreeWayMergeWizardPageDescription}'));
  Page.OnShouldSkipPage := @ThreeWayMergePage_ShouldSkipPage;

  g_CheckListBox := TNewCheckListBox.Create(Page);
  g_CheckListBox.Width := Page.SurfaceWidth;
  g_CheckListBox.Height := ScaleY(128);
  g_CheckListBox.Flat := True;
  g_CheckListBox.BorderStyle := bsNone;
  g_CheckListBox.Color := GetSysColor(15);
  g_CheckListBox.Parent := Page.Surface;
  g_CheckListBox.AddCheckBox(ExpandConstant('{cm:RegisterWinMergeAs3WayMergeTool}'), '', 0, StringToBoolean(GetPreviousData('UseAs3WayMergeTool', 'true')), True, False, True, nil);
  g_CheckListBox.AddRadioButton(ExpandConstant('{cm:MergeAtRightPane}'), '', 1, StringToBoolean(GetPreviousData('MergeAtRightPane', 'true')), True, nil);
  g_CheckListBox.AddRadioButton(ExpandConstant('{cm:MergeAtCenterPane}'), '', 1, StringToBoolean(GetPreviousData('MergeAtCenterPane', 'false')), True, nil);
  g_CheckListBox.AddRadioButton(ExpandConstant('{cm:MergeAtLeftPane}'), '', 1, StringToBoolean(GetPreviousData('MergeAtLeftPane', 'false')), True, nil);
  g_CheckListBox.AddCheckBox(ExpandConstant('{cm:AutoMergeAtStartup}'), '', 1, StringToBoolean(GetPreviousData('AutoMergeAtStartup', 'true')), True, False, True, nil);
end;
