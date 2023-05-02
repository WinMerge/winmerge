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

#define ARCH "x64"
#define ShellExtension32bit "ShellExtensionU.dll"
#define ShellExtension64bit "ShellExtensionX64.dll"
#define AppVersion GetFileVersion(SourcePath + "\..\..\Build\" + ARCH + "\Release\WinMergeU.exe")
#define ShellExtensionVersion GetFileVersion(SourcePath + "..\..\Build\ShellExtension\" + ShellExtension64bit)
#define WinMergeContextMenuVersion GetFileVersion(SourcePath + "..\..\Build\ShellExtension\" + ARCH + "\WinMergeContextMenu.dll")

[Setup]
AppName=WinMerge
AppVersion={#AppVersion}
AppVerName=WinMerge {#AppVersion} {#ARCH} (Current user, 64-bit)
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

DefaultDirName={userpf}\WinMerge
DefaultGroupName=WinMerge
DisableStartupPrompt=true
AllowNoIcons=true
InfoBeforeFile=..\..\Docs\users\GPL.rtf
InfoAfterFile=..\..\Docs\users\ReadMe.txt

OutputBaseFilename=WinMerge-{#AppVersion}-{#ARCH}-PerUser-Setup

PrivilegesRequired=lowest

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

ArchitecturesInstallIn64BitMode={#ARCH}

#if GetEnv("SIGNBAT_PATH") != ""
SignTool=signbat $f
SignedUninstaller=yes
#endif

[Languages]
;Inno Setup's Native Language
Name: English; MessagesFile: compiler:Default.isl,..\..\Translations\InnoSetup\English.isl

;Localizations:
Name: Arabic; MessagesFile: ..\..\Translations\InnoSetup\Unbundled.is5\Arabic.isl,..\..\Translations\InnoSetup\Arabic.isl
Name: Basque; MessagesFile: ..\..\Translations\InnoSetup\Unbundled.is5\Basque.isl,..\..\Translations\InnoSetup\Basque.isl; InfoAfterFile: ..\..\Translations\Docs\Readme\ReadMe-Basque.txt
Name: Bulgarian; MessagesFile: ..\..\Translations\InnoSetup\Unbundled.is5\Bulgarian.isl,..\..\Translations\InnoSetup\Bulgarian.isl; InfoAfterFile: ..\..\Translations\Docs\Readme\ReadMe-Bulgarian.txt
Name: Catalan; MessagesFile: compiler:Languages\Catalan.isl,..\..\Translations\InnoSetup\Catalan.isl; InfoAfterFile: ..\..\Translations\Docs\Readme\ReadMe-Catalan.txt
Name: ChineseSimplified; MessagesFile: ..\..\Translations\InnoSetup\Unbundled.is5\ChineseSimplified.isl,..\..\Translations\InnoSetup\ChineseSimplified.isl; InfoAfterFile: ..\..\Translations\Docs\Readme\ReadMe-ChineseSimplified.txt
Name: ChineseTraditional; MessagesFile: ..\..\Translations\InnoSetup\Unbundled.is5\ChineseTraditional.isl,..\..\Translations\InnoSetup\ChineseTraditional.isl; InfoAfterFile: ..\..\Translations\Docs\Readme\ReadMe-ChineseTraditional.txt
Name: Corsican; MessagesFile: compiler:Languages\Corsican.isl,..\..\Translations\InnoSetup\Corsican.isl; InfoAfterFile: ..\..\Translations\Docs\Readme\ReadMe-Corsican.txt
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
Name: ShellExtension32bit; Description: {cm:ShellExtension32bit}; Types: full typical

Name: filters; Description: {cm:Filters}; Flags: disablenouninstallwarning; Types: full typical
Name: Plugins; Description: {cm:Plugins}; Flags: disablenouninstallwarning; Types: full typical

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

Name: Languages\ChineseSimplified; Description: {cm:ChineseSimplifiedLanguage}; Flags: disablenouninstallwarning; Types: full; Languages: not ChineseSimplified
Name: Languages\ChineseSimplified; Description: {cm:ChineseSimplifiedLanguage}; Flags: disablenouninstallwarning; Types: full typical compact; Languages: ChineseSimplified

Name: Languages\ChineseTraditional; Description: {cm:ChineseTraditionalLanguage}; Flags: disablenouninstallwarning; Types: full; Languages: not ChineseTraditional
Name: Languages\ChineseTraditional; Description: {cm:ChineseTraditionalLanguage}; Flags: disablenouninstallwarning; Types: full typical compact; Languages: ChineseTraditional

Name: Languages\Corsican; Description: {cm:CorsicanLanguage}; Flags: disablenouninstallwarning; Types: full; Languages: not Corsican
Name: Languages\Corsican; Description: {cm:CorsicanLanguage}; Flags: disablenouninstallwarning; Types: full typical compact; Languages: Corsican

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
Name: {group}\{cm:UsersGuide}.lnk; Type: files; Check: not IsComponentSelected('Docs') and not WizardNoIcons
Name: {group}\{cm:ReadMe}.lnk; Type: files; Check: not WizardNoIcons

;This removes the desktop icon in case the user chooses not to install it after previously having it installed
Name: {userdesktop}\WinMerge.lnk; Type: files; Check: not IsTaskSelected('DesktopIcon')

;Removes the Uninstall icon from the start menu...
Name: {group}\{cm:UninstallProgram,WinMerge}.lnk; Type: files; Check: not WizardNoIcons
Name: {group}\{cm:UninstallProgram,WinMerge}; Type: files; Check: not WizardNoIcons

;Remove ANSI executable link from start menu for NT-based Windows versions
;This was installed earlier, but not anymore.
Name: {group}\WinMerge (ANSI).lnk; Type: files; MinVersion: 0,4; Check: not WizardNoIcons

Name: {app}\Docs; Type: filesandordirs

Name: {app}\MergePlugins\editor addin.sct; Type: Files; Check: not IsComponentSelected('Plugins')
Name: {app}\MergePlugins\insert datetime.sct; Type: Files; Check: not IsComponentSelected('Plugins')
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
Source: ..\..\Build\{#ARCH}\Release\WinMergeU.exe; DestDir: {app}; Flags: promptifolder; Components: Core
; Visual Elements
Source: ..\..\Build\{#ARCH}\Release\WinMergeU.VisualElementsManifest.xml; DestDir: {app}; Flags: promptifolder; Components: Core
Source: ..\..\Build\{#ARCH}\Release\LogoImages\*.png; DestDir: {app}\LogoImages; Flags: promptifolder; Components: Core
; 32Bit Plugin Proxy
Source: ..\..\Plugins\WinMerge32BitPluginProxy\Release\WinMerge32BitPluginProxy.exe; DestDir: {app}; Flags: promptifolder; Components: Core

; Shell extension
Source: ..\..\Build\ShellExtension\{#ShellExtension32bit}; DestDir: {app}; Flags: uninsrestartdelete restartreplace promptifolder; \
  MinVersion: 0, 4; Components: ShellExtension32bit; \
  Check: not AreSourceAndDestinationOfShellExtensionSame(ExpandConstant('{app}\{#ShellExtension32bit}')) and \
    RenameShellExtensionDLLIfExists(ExpandConstant('{app}\{#ShellExtension32bit}'))
; 64-bit version of ShellExtension
Source: ..\..\Build\ShellExtension\{#ShellExtension64bit}; DestDir: {app}; Flags: uninsrestartdelete restartreplace promptifolder 64bit; \
  MinVersion: 0,5.01.2600; \
  Check: IsWin64 and not AreSourceAndDestinationOfShellExtensionSame(ExpandConstant('{app}\{#ShellExtension64bit}')) and \
    RenameShellExtensionDLLIfExists(ExpandConstant('{app}\{#ShellExtension64bit}'))
Source: ..\..\Build\ShellExtension\{#ARCH}\WinMergeContextMenu.dll; DestDir: {app}; Flags: uninsrestartdelete restartreplace promptifolder 64bit; \
  Check: IsWin64 and not AreSourceAndDestinationOfWinMergeContextMenuSame(ExpandConstant('{app}\WinMergeContextMenu.dll')) and \
    UnregisterWinMergeContextMenuPackage and RenameShellExtensionDLLIfExists(ExpandConstant('{app}\WinMergeContextMenu.dll'))
Source: ..\..\Build\ShellExtension\WinMergeContextMenuPackage.msix; DestDir: {app}; Flags: uninsrestartdelete restartreplace promptifolder 64bit; \
  Check: IsWin64 and not AreSourceAndDestinationOfWinMergeContextMenuSame(ExpandConstant('{app}\WinMergeContextMenuPackage.msix'))

; ArchiveSupport
;Please do not reorder the 7z Dlls by version they compress better ordered by platform and then by version
Source: ..\..\Build\{#ARCH}\Release\Merge7z\Merge7z.dll; DestDir: {app}\Merge7z; Flags: promptifolder replacesameversion; MinVersion: 0, 4; Components: Core
Source: ..\..\Build\{#ARCH}\Release\Merge7z\7z.dll; DestDir: {app}\Merge7z; Flags: promptifolder replacesameversion; MinVersion: 0, 4; Components: Core
Source: ..\..\Build\{#ARCH}\Release\Merge7z\*.txt; DestDir: {app}\Merge7z; Flags: promptifolder; MinVersion: 0, 4; Components: Core
Source: ..\..\Build\{#ARCH}\Release\Merge7z\Lang\*.txt; DestDir: {app}\Merge7z\Lang; Flags: promptifolder; MinVersion: 0, 4; Components: Core

; Language files
Source: ..\..\Translations\WinMerge\Arabic.po; DestDir: {app}\Languages; Components: Languages\Arabic; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\ShellExtension\Arabic.po; DestDir: {app}\Languages\ShellExtension; Components: Languages\Arabic; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\WinMerge\Basque.po; DestDir: {app}\Languages; Components: Languages\Basque; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\ShellExtension\Basque.po; DestDir: {app}\Languages\ShellExtension; Components: Languages\Basque; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\Docs\Readme\ReadMe-Basque.txt; DestDir: {app}\Docs; Components: Languages\Basque
Source: ..\..\Translations\WinMerge\Brazilian.po; DestDir: {app}\Languages; Components: Languages\PortugueseBrazilian; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\ShellExtension\Brazilian.po; DestDir: {app}\Languages\ShellExtension; Components: Languages\PortugueseBrazilian; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\Docs\Readme\ReadMe-Brazilian.txt; DestDir: {app}\Docs; Components: Languages\PortugueseBrazilian
Source: ..\..\Translations\WinMerge\Bulgarian.po; DestDir: {app}\Languages; Components: Languages\Bulgarian; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\ShellExtension\Bulgarian.po; DestDir: {app}\Languages\ShellExtension; Components: Languages\Bulgarian; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\Docs\Readme\ReadMe-Bulgarian.txt; DestDir: {app}\Docs; Components: Languages\Bulgarian
Source: ..\..\Translations\WinMerge\Catalan.po; DestDir: {app}\Languages; Components: Languages\Catalan; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\ShellExtension\Catalan.po; DestDir: {app}\Languages\ShellExtension; Components: Languages\Catalan; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\Docs\Readme\ReadMe-Catalan.txt; DestDir: {app}\Docs; Components: Languages\Catalan
Source: ..\..\Translations\WinMerge\ChineseSimplified.po; DestDir: {app}\Languages; Components: Languages\ChineseSimplified; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\ShellExtension\ChineseSimplified.po; DestDir: {app}\Languages\ShellExtension; Components: Languages\ChineseSimplified; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\Docs\Readme\ReadMe-ChineseSimplified.txt; DestDir: {app}\Docs; Components: Languages\ChineseSimplified
Source: ..\..\Translations\WinMerge\ChineseTraditional.po; DestDir: {app}\Languages; Components: Languages\ChineseTraditional; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\ShellExtension\ChineseTraditional.po; DestDir: {app}\Languages\ShellExtension; Components: Languages\ChineseTraditional; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\Docs\Readme\ReadMe-ChineseTraditional.txt; DestDir: {app}\Docs; Components: Languages\ChineseTraditional
Source: ..\..\Translations\WinMerge\Corsican.po; DestDir: {app}\Languages; Components: Languages\Corsican; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\ShellExtension\Corsican.po; DestDir: {app}\Languages\ShellExtension; Components: Languages\Corsican; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\Docs\Readme\ReadMe-Corsican.txt; DestDir: {app}\Docs; Components: Languages\Corsican
Source: ..\..\Translations\WinMerge\Croatian.po; DestDir: {app}\Languages; Components: Languages\Croatian; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\ShellExtension\Croatian.po; DestDir: {app}\Languages\ShellExtension; Components: Languages\Croatian; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\Docs\Readme\ReadMe-Croatian.txt; DestDir: {app}\Docs; Components: Languages\Croatian
Source: ..\..\Translations\WinMerge\Czech.po; DestDir: {app}\Languages; Components: Languages\Czech; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\ShellExtension\Czech.po; DestDir: {app}\Languages\ShellExtension; Components: Languages\Czech; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\WinMerge\Danish.po; DestDir: {app}\Languages; Components: Languages\Danish; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\ShellExtension\Danish.po; DestDir: {app}\Languages\ShellExtension; Components: Languages\Danish; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\WinMerge\Dutch.po; DestDir: {app}\Languages; Components: Languages\Dutch; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\ShellExtension\Dutch.po; DestDir: {app}\Languages\ShellExtension; Components: Languages\Dutch; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\Docs\Readme\ReadMe-Dutch.txt; DestDir: {app}\Docs; Components: Languages\Dutch
Source: ..\..\Translations\WinMerge\Finnish.po; DestDir: {app}\Languages; Components: Languages\Finnish; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\ShellExtension\Finnish.po; DestDir: {app}\Languages\ShellExtension; Components: Languages\Finnish; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\Docs\Readme\ReadMe-Finnish.txt; DestDir: {app}\Docs; Components: Languages\Finnish
Source: ..\..\Translations\WinMerge\French.po; DestDir: {app}\Languages; Components: Languages\French; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\ShellExtension\French.po; DestDir: {app}\Languages\ShellExtension; Components: Languages\French; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\Docs\Readme\ReadMe-French.txt; DestDir: {app}\Docs; Components: Languages\French
Source: ..\..\Translations\WinMerge\Galician.po; DestDir: {app}\Languages; Components: Languages\Galician; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\ShellExtension\Galician.po; DestDir: {app}\Languages\ShellExtension; Components: Languages\Galician; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\Docs\Readme\ReadMe-Galician.txt; DestDir: {app}\Docs; Components: Languages\Galician
Source: ..\..\Translations\WinMerge\German.po; DestDir: {app}\Languages; Components: Languages\German; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\ShellExtension\German.po; DestDir: {app}\Languages\ShellExtension; Components: Languages\German; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\Docs\Readme\ReadMe-German.txt; DestDir: {app}\Docs; Components: Languages\German
Source: ..\..\Translations\WinMerge\Greek.po; DestDir: {app}\Languages; Components: Languages\Greek; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\ShellExtension\Greek.po; DestDir: {app}\Languages\ShellExtension; Components: Languages\Greek; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\Docs\Readme\ReadMe-Greek.txt; DestDir: {app}\Docs; Components: Languages\Greek
Source: ..\..\Translations\WinMerge\Hungarian.po; DestDir: {app}\Languages; Components: Languages\Hungarian; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\ShellExtension\Hungarian.po; DestDir: {app}\Languages\ShellExtension; Components: Languages\Hungarian; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\WinMerge\Italian.po; DestDir: {app}\Languages; Components: Languages\Italian; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\ShellExtension\Italian.po; DestDir: {app}\Languages\ShellExtension; Components: Languages\Italian; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\Docs\Readme\ReadMe-Italian.txt; DestDir: {app}\Docs; Components: Languages\Italian
Source: ..\..\Translations\WinMerge\Japanese.po; DestDir: {app}\Languages; Components: Languages\Japanese; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\ShellExtension\Japanese.po; DestDir: {app}\Languages\ShellExtension; Components: Languages\Japanese; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\Docs\Readme\ReadMe-Japanese.txt; DestDir: {app}\Docs; Components: Languages\Japanese
Source: ..\..\Build\Manual\htmlhelp\WinMergeJapanese.chm; DestDir: {app}\Docs; Components: Languages\Japanese
Source: ..\..\Translations\WinMerge\Korean.po; DestDir: {app}\Languages; Components: Languages\Korean; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\ShellExtension\Korean.po; DestDir: {app}\Languages\ShellExtension; Components: Languages\Korean; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\WinMerge\Lithuanian.po; DestDir: {app}\Languages; Components: Languages\Lithuanian; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\ShellExtension\Lithuanian.po; DestDir: {app}\Languages\ShellExtension; Components: Languages\Lithuanian; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\Docs\Readme\ReadMe-Lithuanian.txt; DestDir: {app}\Docs; Components: Languages\Lithuanian
Source: ..\..\Translations\WinMerge\Norwegian.po; DestDir: {app}\Languages; Components: Languages\Norwegian; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\ShellExtension\Norwegian.po; DestDir: {app}\Languages\ShellExtension; Components: Languages\Norwegian; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\WinMerge\Persian.po; DestDir: {app}\Languages; Components: Languages\Persian; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\ShellExtension\Persian.po; DestDir: {app}\Languages\ShellExtension; Components: Languages\Persian; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\WinMerge\Polish.po; DestDir: {app}\Languages; Components: Languages\Polish; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\ShellExtension\Polish.po; DestDir: {app}\Languages\ShellExtension; Components: Languages\Polish; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\WinMerge\Portuguese.po; DestDir: {app}\Languages; Components: Languages\Portuguese; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\ShellExtension\Portuguese.po; DestDir: {app}\Languages\ShellExtension; Components: Languages\Portuguese; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\Docs\Readme\ReadMe-Portuguese.txt; DestDir: {app}\Docs; Components: Languages\Portuguese
Source: ..\..\Translations\WinMerge\Romanian.po; DestDir: {app}\Languages; Components: Languages\Romanian; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\ShellExtension\Romanian.po; DestDir: {app}\Languages\ShellExtension; Components: Languages\Romanian; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\Docs\Readme\ReadMe-Romanian.txt; DestDir: {app}\Docs; Components: Languages\Romanian
Source: ..\..\Translations\WinMerge\Russian.po; DestDir: {app}\Languages; Components: Languages\Russian; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\ShellExtension\Russian.po; DestDir: {app}\Languages\ShellExtension; Components: Languages\Russian; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\WinMerge\Serbian.po; DestDir: {app}\Languages; Components: Languages\Serbian; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\ShellExtension\Serbian.po; DestDir: {app}\Languages\ShellExtension; Components: Languages\Serbian; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\Docs\Readme\ReadMe-Serbian.txt; DestDir: {app}\Docs; Components: Languages\Serbian
Source: ..\..\Translations\WinMerge\Sinhala.po; DestDir: {app}\Languages; Components: Languages\Sinhala; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\ShellExtension\Sinhala.po; DestDir: {app}\Languages\ShellExtension; Components: Languages\Sinhala; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\WinMerge\Slovak.po; DestDir: {app}\Languages; Components: Languages\Slovak; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\ShellExtension\Slovak.po; DestDir: {app}\Languages\ShellExtension; Components: Languages\Slovak; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\Docs\Readme\ReadMe-Slovak.txt; DestDir: {app}\Docs; Components: Languages\Slovak
Source: ..\..\Translations\WinMerge\Slovenian.po; DestDir: {app}\Languages; Components: Languages\Slovenian; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\ShellExtension\Slovenian.po; DestDir: {app}\Languages\ShellExtension; Components: Languages\Slovenian; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\Docs\Readme\ReadMe-Slovenian.txt; DestDir: {app}\Docs; Components: Languages\Slovenian
Source: ..\..\Translations\WinMerge\Spanish.po; DestDir: {app}\Languages; Components: Languages\Spanish; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\ShellExtension\Spanish.po; DestDir: {app}\Languages\ShellExtension; Components: Languages\Spanish; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\Docs\Readme\ReadMe-Spanish.txt; DestDir: {app}\Docs; Components: Languages\Spanish
Source: ..\..\Translations\WinMerge\Swedish.po; DestDir: {app}\Languages; Components: Languages\Swedish; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\ShellExtension\Swedish.po; DestDir: {app}\Languages\ShellExtension; Components: Languages\Swedish; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\Docs\Readme\ReadMe-Swedish.txt; DestDir: {app}\Docs; Components: Languages\Swedish
Source: ..\..\Translations\WinMerge\Turkish.po; DestDir: {app}\Languages; Components: Languages\Turkish; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\ShellExtension\Turkish.po; DestDir: {app}\Languages\ShellExtension; Components: Languages\Turkish; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\Docs\Readme\ReadMe-Turkish.txt; DestDir: {app}\Docs; Components: Languages\Turkish
Source: ..\..\Translations\WinMerge\Ukrainian.po; DestDir: {app}\Languages; Components: Languages\Ukrainian; Flags: ignoreversion comparetimestamp
Source: ..\..\Translations\ShellExtension\Ukrainian.po; DestDir: {app}\Languages\ShellExtension; Components: Languages\Ukrainian; Flags: ignoreversion comparetimestamp
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
Source: ..\..\Plugins\Plugins.xml; DestDir: {app}\MergePlugins; Flags: IgnoreVersion CompareTimeStamp; Components: Plugins
Source: ..\..\Plugins\dlls\editor addin.sct; DestDir: {app}\MergePlugins; Flags: IgnoreVersion CompareTimeStamp; Components: Plugins
Source: ..\..\Plugins\dlls\insert datetime.sct; DestDir: {app}\MergePlugins; Flags: IgnoreVersion CompareTimeStamp; Components: Plugins
Source: ..\..\Plugins\dlls\CompareMSExcelFiles.sct; DestDir: {app}\MergePlugins; Flags: IgnoreVersion CompareTimeStamp; Components: Plugins
Source: ..\..\Plugins\dlls\CompareMSWordFiles.sct; DestDir: {app}\MergePlugins; Flags: IgnoreVersion CompareTimeStamp; Components: Plugins
Source: ..\..\Plugins\dlls\CompareMSPowerPointFiles.sct; DestDir: {app}\MergePlugins; Flags: IgnoreVersion CompareTimeStamp; Components: Plugins
Source: ..\..\Plugins\dlls\CompareMSVisioFiles.sct; DestDir: {app}\MergePlugins; Flags: IgnoreVersion CompareTimeStamp; Components: Plugins
Source: ..\..\Plugins\dlls\ApplyPatch.sct; DestDir: {app}\MergePlugins; Flags: IgnoreVersion CompareTimeStamp; Components: Plugins
Source: ..\..\Plugins\dlls\PrediffLineFilter.sct; DestDir: {app}\MergePlugins; Flags: IgnoreVersion CompareTimeStamp; Components: Plugins
Source: ..\..\Plugins\dlls\{#ARCH}\IgnoreColumns.dll; DestDir: {app}\MergePlugins; Flags: ignoreversion replacesameversion; Components: Plugins
Source: ..\..\Plugins\dlls\{#ARCH}\IgnoreCommentsC.dll; DestDir: {app}\MergePlugins; Flags: ignoreversion replacesameversion; Components: Plugins
Source: ..\..\Plugins\dlls\{#ARCH}\IgnoreFieldsComma.dll; DestDir: {app}\MergePlugins; Flags: ignoreversion replacesameversion; Components: Plugins
Source: ..\..\Plugins\dlls\{#ARCH}\IgnoreFieldsTab.dll; DestDir: {app}\MergePlugins; Flags: ignoreversion replacesameversion; Components: Plugins
Source: ..\..\Plugins\dlls\IgnoreLeadingLineNumbers.sct; DestDir: {app}\MergePlugins; Flags: ignoreversion replacesameversion; Components: Plugins

;Frhed
Source: ..\..\Build\{#ARCH}\Release\Frhed\GPL.txt; DestDir: {app}\Frhed; Components: Core
;Source: ..\..\Build\{#ARCH}\Release\Frhed\frhed.exe; DestDir: {app}\Frhed; Components: Core
Source: ..\..\Build\{#ARCH}\Release\Frhed\hekseditU.dll; DestDir: {app}\Frhed; Flags: ignoreversion replacesameversion; Components: Core
Source: ..\..\Build\{#ARCH}\Release\Frhed\Docs\ChangeLog.txt; DestDir: {app}\Frhed\Docs; Components: Core
Source: ..\..\Build\{#ARCH}\Release\Frhed\Docs\Contributors.txt; DestDir: {app}\Frhed\Docs; Components: Core
Source: ..\..\Build\{#ARCH}\Release\Frhed\Docs\History.txt; DestDir: {app}\Frhed\Docs; Components: Core
Source: ..\..\Build\{#ARCH}\Release\Frhed\Docs\Links.txt; DestDir: {app}\Frhed\Docs; Components: Core
Source: ..\..\Build\{#ARCH}\Release\Frhed\Docs\Sample.tpl  ; DestDir: {app}\Frhed\Docs; Components: Core
Source: ..\..\Build\{#ARCH}\Release\Frhed\Languages\de.po; DestDir: {app}\Frhed\Languages; Components: Core
Source: ..\..\Build\{#ARCH}\Release\Frhed\Languages\fr.po; DestDir: {app}\Frhed\Languages; Components: Core
Source: ..\..\Build\{#ARCH}\Release\Frhed\Languages\ja.po; DestDir: {app}\Frhed\Languages; Components: Core
Source: ..\..\Build\{#ARCH}\Release\Frhed\Languages\nl.po; DestDir: {app}\Frhed\Languages; Components: Core
Source: ..\..\Build\{#ARCH}\Release\Frhed\Languages\sl.po; DestDir: {app}\Frhed\Languages; Components: Core
Source: ..\..\Build\{#ARCH}\Release\Frhed\Languages\heksedit.lng; DestDir: {app}\Frhed\Languages; Components: Core

;WinIMerge
Source: ..\..\Build\{#ARCH}\Release\WinIMerge\GPL.txt; DestDir: {app}\WinIMerge; Components: Core
Source: ..\..\Build\{#ARCH}\Release\WinIMerge\freeimage-license-gplv2.txt; DestDir: {app}\WinIMerge; Components: Core
;Source: ..\..\Build\{#ARCH}\Release\WinIMerge\WinIMerge.exe; DestDir: {app}\WinIMerge; Components: Core
Source: ..\..\Build\{#ARCH}\Release\WinIMerge\WinIMergeLib.dll; DestDir: {app}\WinIMerge; Flags: ignoreversion replacesameversion; Components: Core
Source: ..\..\Build\{#ARCH}\Release\WinIMerge\vcomp140.dll; DestDir: {app}; Components: Core

;WinWebDiff
Source: ..\..\Build\{#ARCH}\Release\WinWebDiff\LICENSE*.txt; DestDir: {app}\WinWebDiff; Components: Core
Source: ..\..\Build\{#ARCH}\Release\WinWebDiff\WinWebDiffLib.dll; DestDir: {app}\WinWebDiff; Flags: ignoreversion replacesameversion; Components: Core

;GnuWin32 Patch for Windows
Source: ..\..\Build\GnuWin32\*.*; DestDir: {app}\Commands\GnuWin32; Flags: recursesubdirs; Components: Plugins
; HTML Tidy
Source: ..\..\Build\tidy-html5\bin\*.*; DestDir: {app}\Commands\tidy-html5; Flags: recursesubdirs; Components: Plugins
Source: ..\..\Build\tidy-html5\tidy-html5-5.4.0\README\LICENSE.md; DestDir: {app}\Commands\tidy-html5; Flags: recursesubdirs; Components: Plugins
; jq
Source: ..\..\Build\jq\jq-win32.exe; DestDir: {app}\Commands\jq; DestName: jq.exe; Flags: recursesubdirs; Components: Plugins
Source: ..\..\Build\jq\jq-jq-1.4\COPYING; DestDir: {app}\Commands\jq; Flags: recursesubdirs; Components: Plugins
; md4c
Source: ..\..\Build\md4c\mingw32\bin\*.*; DestDir: {app}\Commands\md4c; Flags: recursesubdirs; Components: Plugins
Source: ..\..\Build\md4c\mingw32\share\licenses\md4c\LICENSE.md; DestDir: {app}\Commands\md4c; Flags: recursesubdirs; Components: Plugins
Source: ..\..\Plugins\Commands\md4c\*.bat; DestDir: {app}\Commands\md4c; Flags: recursesubdirs; Components: Plugins
; PlantUML
Source: ..\..\Plugins\Commands\PlantUML\*.bat; DestDir: {app}\Commands\PlantUML; Flags: recursesubdirs; Components: Plugins
Source: ..\..\Plugins\Commands\PlantUML\*.txt; DestDir: {app}\Commands\PlantUML; Flags: recursesubdirs; Components: Plugins
; Apache Tika
Source: ..\..\Plugins\Commands\Apache-Tika\*.bat; DestDir: {app}\Commands\Apache-Tika; Flags: recursesubdirs; Components: Plugins
Source: ..\..\Plugins\Commands\Apache-Tika\*.txt; DestDir: {app}\Commands\Apache-Tika; Flags: recursesubdirs; Components: Plugins
; q
Source: ..\..\Plugins\Commands\q\*.bat; DestDir: {app}\Commands\q; Flags: recursesubdirs; Components: Plugins
Source: ..\..\Plugins\Commands\q\*.txt; DestDir: {app}\Commands\q; Flags: recursesubdirs; Components: Plugins
; yq
Source: ..\..\Plugins\Commands\yq\*.bat; DestDir: {app}\Commands\yq; Flags: recursesubdirs; Components: Plugins
Source: ..\..\Plugins\Commands\yq\*.txt; DestDir: {app}\Commands\yq; Flags: recursesubdirs; Components: Plugins
; dumpbin
Source: ..\..\Plugins\Commands\dumpbin\*.bat; DestDir: {app}\Commands\dumpbin; Flags: recursesubdirs; Components: Plugins
; ildasm
Source: ..\..\Plugins\Commands\ildasm\*.bat; DestDir: {app}\Commands\ildasm; Flags: recursesubdirs; Components: Plugins
; Java
Source: ..\..\Plugins\Commands\Java\*.bat; DestDir: {app}\Commands\Java; Flags: recursesubdirs; Components: Plugins
Source: ..\..\Plugins\Commands\Java\*.txt; DestDir: {app}\Commands\Java; Flags: recursesubdirs; Components: Plugins
; DownloadFiles.bat
Source: ..\..\Plugins\Commands\DownloadFiles.bat; DestDir: {app}\Commands\; Flags: recursesubdirs; Components: Plugins

[Dirs]
Name: "{app}\MergePlugins"

[Icons]
;Start Menu Icons
Name: {group}\WinMerge; Filename: {app}\WinMergeU.exe; AppUserModelID: "Thingamahoochie.WinMerge"; Check: not WizardNoIcons
Name: {group}\{cm:UsersGuide}; Filename: {app}\Docs\WinMerge.chm; Check: not WizardNoIcons

;Desktop Icon
Name: {userdesktop}\WinMerge; Filename: {app}\WinMergeU.exe; Tasks: desktopicon

[Registry]
Root: HKCU; Subkey: Software\Thingamahoochie; Flags: uninsdeletekeyifempty
Root: HKCU; Subkey: Software\Thingamahoochie\WinMerge; Flags: uninsdeletekey

;Add Project file extension (.WinMerge) to known extensions and
;set WinMerge to open those files
;set Notepad to edit project files
Root: HKCU; Subkey: .WinMerge; ValueType: String; ValueData: WinMerge.Project.File; Flags: uninsdeletekey
Root: HKCU; Subkey: WinMerge.Project.File; ValueType: String; ValueData: {cm:ProjectFileDesc}; Flags: uninsdeletekey
Root: HKCU; Subkey: WinMerge.Project.File\shell\open\command; ValueType: String; ValueData: """{app}\WinMergeU.exe"" ""%1"""; Flags: uninsdeletekey
Root: HKCU; Subkey: WinMerge.Project.File\shell\edit\command; ValueType: String; ValueData: """NOTEPAD.EXE"" ""%1"""; Flags: uninsdeletekey
Root: HKCU; Subkey: WinMerge.Project.File\DefaultIcon; ValueType: String; ValueData: """{app}\WinMergeU.exe"",1"; Flags: uninsdeletekey

; delete obsolete values
;In Inno Setup Version 4.18 ValueData couldn't be null and compile,
;if this is fixed in a later version feel free to remove the parameter
Root: HKCU; Subkey: Software\Thingamahoochie\WinMerge\Settings; ValueType: none; ValueName: LeftMax; Flags: deletevalue
Root: HKCU; Subkey: Software\Thingamahoochie\WinMerge\Settings; ValueType: none; ValueName: DirViewMax; Flags: deletevalue

;This removes the key that remembers which messageboxes to hide from the user, this is because the text of that message
;can change and make it more clear as to the user why they might want to pay attention to a particular dialog and also
;because a particular message might be added or removed and a new message might occupy a previous message's ID number
Root: HKCU; Subkey: Software\Thingamahoochie\WinMerge\MessageBoxes; ValueType: none; Flags: deletekey

Root: HKCU; SubKey: Directory\Shell\WinMerge\command; ValueType: none; Flags: deletekey noerror
Root: HKCU; SubKey: Directory\Shell\WinMerge; ValueType: none; Flags: deletekey noerror

;Adds "Start Menu" --> "Run" Support for WinMerge
Root: HKCU; Subkey: Software\Microsoft\Windows\CurrentVersion\App Paths\WinMerge.exe; ValueType: none; Flags: uninsdeletekey
Root: HKCU; Subkey: Software\Microsoft\Windows\CurrentVersion\App Paths\WinMergeU.exe; ValueType: none; Flags: uninsdeletekey
Root: HKCU; SubKey: SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\WinMerge.exe; ValueType: string; ValueName: ; ValueData: {app}\WinMergeU.exe
Root: HKCU; SubKey: SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\WinMergeU.exe; ValueType: string; ValueName: ; ValueData: {app}\WinMergeU.exe

;Registry Keys for use by ShellExtension.dll
Root: HKCU; SubKey: Software\Thingamahoochie\WinMerge; ValueType: string; ValueName: Executable; ValueData: {app}\WinMergeU.exe

;Enables or disables the Context Menu preference based on what the user selects during install
;Initially the Context menu is explicitly disabled:
Root: HKCU; SubKey: Software\Thingamahoochie\WinMerge; ValueType: dword; ValueName: ContextMenuEnabled; ValueData: 0;

;If the user chose to use the context menu then we re-enable it.  This is necessary so it'll turn on and off not just on.
Root: HKCU; SubKey: Software\Thingamahoochie\WinMerge; ValueType: dword; ValueName: ContextMenuEnabled; ValueData: 1; Tasks: ShellExtension
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
Root: HKCU; SubKey: Software\TortoiseGit; ValueType: string; ValueName: Merge; ValueData: {code:TortoiseSVNGitMergeToolCommandLine}; Flags: uninsdeletevalue; Check: UseAs3WayMergeTool; Tasks: TortoiseGit

;Set WinMerge as TortoiseSVN diff tool
Root: HKCU; SubKey: Software\TortoiseSVN; ValueType: string; ValueName: Diff; ValueData: {app}\WinMergeU.exe -e -ub -dl %bname -dr %yname %base %mine; Flags: uninsdeletevalue; Tasks: TortoiseSVN
Root: HKCU; SubKey: Software\TortoiseSVN; ValueType: string; ValueName: Merge; ValueData: {code:TortoiseSVNGitMergeToolCommandLine}; Flags: uninsdeletevalue; Check: UseAs3WayMergeTool; Tasks: TortoiseSVN

;Whatever the user chooses at the [Select Setup Language] dialog should also determine what language WinMerge will start up in
;(unless the user already has a startup language specified)
Root: HKCU; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00001401; Flags: deletevalue; Languages: Arabic
Root: HKCU; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $0000042d; Flags: deletevalue; Languages: Basque
Root: HKCU; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000402; Flags: deletevalue; Languages: Bulgarian
Root: HKCU; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000403; Flags: deletevalue; Languages: Catalan
Root: HKCU; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000804; Flags: deletevalue; Languages: ChineseSimplified
Root: HKCU; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000404; Flags: deletevalue; Languages: ChineseTraditional
Root: HKCU; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000483; Flags: deletevalue; Languages: Corsican
Root: HKCU; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $0000041a; Flags: deletevalue; Languages: Croatian
Root: HKCU; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000405; Flags: deletevalue; Languages: Czech
Root: HKCU; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000406; Flags: deletevalue; Languages: Danish
Root: HKCU; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000413; Flags: deletevalue; Languages: Dutch
Root: HKCU; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000409; Flags: deletevalue; Languages: English
Root: HKCU; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $0000040b; Flags: deletevalue; Languages: Finnish
Root: HKCU; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $0000040c; Flags: deletevalue; Languages: French
Root: HKCU; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000456; Flags: deletevalue; Languages: Galician
Root: HKCU; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000407; Flags: deletevalue; Languages: German
Root: HKCU; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000408; Flags: deletevalue; Languages: Greek
Root: HKCU; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $0000040e; Flags: deletevalue; Languages: Hungarian
Root: HKCU; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000410; Flags: deletevalue; Languages: Italian
Root: HKCU; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000411; Flags: deletevalue; Languages: Japanese
Root: HKCU; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000412; Flags: deletevalue; Languages: Korean
Root: HKCU; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000427; Flags: deletevalue; Languages: Lithuanian 
Root: HKCU; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000414; Flags: deletevalue; Languages: Norwegian
Root: HKCU; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000429; Flags: deletevalue; Languages: Persian
Root: HKCU; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000415; Flags: deletevalue; Languages: Polish
Root: HKCU; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000816; Flags: deletevalue; Languages: Portuguese
Root: HKCU; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000416; Flags: deletevalue; Languages: PortugueseBrazilian
Root: HKCU; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000418; Flags: deletevalue; Languages: Romanian
Root: HKCU; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000419; Flags: deletevalue; Languages: Russian
Root: HKCU; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000c1a; Flags: deletevalue; Languages: Serbian
Root: HKCU; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $0000045b; Flags: deletevalue; Languages: Sinhala
Root: HKCU; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $0000041b; Flags: deletevalue; Languages: Slovak
Root: HKCU; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000424; Flags: deletevalue; Languages: Slovenian
Root: HKCU; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000c0a; Flags: deletevalue; Languages: Spanish
Root: HKCU; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $0000041D; Flags: deletevalue; Languages: Swedish
Root: HKCU; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $0000041f; Flags: deletevalue; Languages: Turkish
Root: HKCU; SubKey: Software\Thingamahoochie\WinMerge\Locale; ValueType: dword; ValueName: LanguageId; ValueData: $00000422; Flags: deletevalue; Languages: Ukrainian


[Run]
;This will no longer appear unless the user chose to make a start menu group in the first place
Filename: {win}\Explorer.exe; Description: {cm:ViewStartMenuFolder}; Parameters: """{group}"""; Flags: waituntilidle postinstall skipifsilent unchecked; Check: GroupCreated

Filename: {app}\WinMergeU.exe; Description: {cm:LaunchProgram,WinMerge}; Flags: nowait postinstall skipifsilent runmaximized

Filename: {syswow64}\regsvr32.exe; Parameters: "/s /n /i:user ""{app}\{#ShellExtension32bit}"""; Flags: waituntilterminated; Components: ShellExtension32bit
Filename: {sys}\regsvr32.exe; Parameters: "/s /n /i:user ""{app}\{#ShellExtension64bit}"""; Flags: waituntilterminated; Check: not IsWindows11OrLater
Filename: {app}\WinMerge32BitPluginProxy.exe; Parameters: "/RegServerPerUser"; Flags: waituntilidle
Filename: {win}\sysnative\WindowsPowerShell\v1.0\PowerShell.exe; Parameters: "-c ""$host.ui.RawUI.WindowTitle = 'Registering WinMergeContextMenu package...'; if ((Get-AppxPackage -name WinMerge) -eq $null) {{ Add-AppxPackage '{app}\WinMergeContextMenuPackage.msix' -ExternalLocation '{app}'}"""; Flags: waituntilterminated; Check: IsWindows11OrLater

[UninstallRun]
Filename: {syswow64}\regsvr32.exe; Parameters: "/s /u /n /i:user ""{app}\{#ShellExtension32bit}"""; Flags: waituntilterminated; Components: ShellExtension32bit
Filename: {sys}\regsvr32.exe; Parameters: "/s /u /n /i:user ""{app}\{#ShellExtension64bit}"""; Flags: waituntilterminated
Filename: {app}\WinMerge32BitPluginProxy.exe; Parameters: "/UnregServerPerUser"; Flags: waituntilidle

[UninstallDelete]
;Remove 7-zip integration dlls possibly installed (by hand or using separate installer)
Name: {app}\Merge7z*.dll; Type: files
Name: {app}\7zip_pad.xml; Type: files
Name: {app}\Codecs; Type: filesandordirs
Name: {app}\Formats; Type: filesandordirs
Name: {app}\Lang; Type: filesandordirs

Name: {group}; Type: dirifempty; Check: not WizardNoIcons
Name: {app}; Type: dirifempty


[Code]
Var
    g_CheckListBox: TNewCheckListBox;
    g_IsExplorerRestartRequired: boolean;

{Determines whether or not the user chose to create a start menu}
Function GroupCreated(): boolean;
Var
    {Stores the path of the start menu group Inno Setup may have created}
    strGroup_Path: string;
Begin
    {Saves the path that Inno Setup intended to create the start menu group at}
    if not WizardNoIcons Then strGroup_Path := ExpandConstant('{group}');

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
    If not WizardNoIcons Then strNew := ExpandConstant('{group}');

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

Function UnregisterWinMergeContextMenuPackage: Boolean;
var
  ResultCode: Integer;
Begin;
  if RegKeyExists(HKCU, 'SOFTWARE\Classes\PackagedCom\ClassIndex\{90340779-F37E-468E-9728-A2593498ED32}') then
    Exec(ExpandConstant('{win}\sysnative\WindowsPowerShell\v1.0\PowerShell.exe'), '-c "$host.ui.RawUI.WindowTitle = ""Unregistering WinMergeContextMenu package...""; Get-AppxPackage -name WinMerge | Remove-AppxPackage"', '', SW_SHOW, ewWaitUntilTerminated, ResultCode);  
  Result := true;
End;

function GetCurrentProcessId: DWORD;
  external 'GetCurrentProcessId@kernel32.dll stdcall';

function RenameShellExtensionDLLIfExists(Filename: String) : Boolean;
var
  dest: String;
begin
  if FileExists(Filename) then begin
    dest := Filename + '.' + IntToStr(GetCurrentProcessId()) + '.old';
    RenameFile(Filename, dest);
  end;
  Result := true;
end;

function DeleteRenamedShellExtensionDLL(Filename: String; fRestartReplace: Boolean; fRetry: Boolean) : Boolean;
var
  dest: String;
  i: Integer;
begin
  dest := Filename + '.' + IntToStr(GetCurrentProcessId()) + '.old';
  if FileExists(dest) then begin
    for i := 0 to 40 do begin
      Result := DeleteFile(dest);
      if not fRetry or Result = true then break;
      Sleep(100);
    end;
    if Result = false then begin
      if fRestartReplace then begin
        RestartReplace(dest, '');
      end;
      g_IsExplorerRestartRequired := true;
    end;
  end;
  Result := true;
end;

procedure RestartExplorer;
var
  ResultCode: Integer;
begin
  Exec(ExpandConstant('{sys}\taskkill.exe'), '/f /im explorer.exe', '', SW_HIDE, ewWaitUntilTerminated, ResultCode);
  Exec(ExpandConstant('{win}\explorer.exe'), '', '', SW_SHOW, ewNoWait, ResultCode);
end;

procedure DeleteRenamedFiles();
begin
  DeleteRenamedShellExtensionDLL(ExpandConstant('{app}\{#ShellExtension32bit}'), false, false);
  DeleteRenamedShellExtensionDLL(ExpandConstant('{app}\{#ShellExtension64bit}'), false, false);
  DeleteRenamedShellExtensionDLL(ExpandConstant('{app}\WinMergeContextMenu.dll'), false, false);
  if g_IsExplorerRestartRequired then begin
    if SuppressibleMsgBox(ExpandConstant('{cm:ExplorerNeedsRestart}'), mbConfirmation, MB_YESNO or MB_DEFBUTTON2, IDNO) = IDYES then begin
      RestartExplorer();
      DeleteRenamedShellExtensionDLL(ExpandConstant('{app}\{#ShellExtension32bit}'), true, true);
      DeleteRenamedShellExtensionDLL(ExpandConstant('{app}\{#ShellExtension64bit}'), true, true);
      DeleteRenamedShellExtensionDLL(ExpandConstant('{app}\WinMergeContextMenu.dll'), true, true);
    end;
  end;
end;

procedure RegisterUserTasks();
var
  params: string;
  UserTasksFlags: DWORD;
  ResultCode: Integer;
Begin
  UserTasksFlags := 4097; { 4096(Clipboard Compare)+1(New Text Compare) }
  RegQueryDWORDValue(HKCU, 'Software\Thingamahoochie\WinMerge', 'UserTasksFlags', UserTasksFlags);
  params := '/s- /minimize /noninteractive /set-usertasks-to-jumplist ' + IntToStr(UserTasksFlags);
  Exec(ExpandConstant('{app}\WinMergeU.exe'), params, '', SW_HIDE, ewWaitUntilTerminated, ResultCode);
end;

{This event procedure is queed each time the user changes pages within the installer}
Procedure CurPageChanged(CurPage: integer);
Begin
    {if the installer reaches the file copy page then...}
    If CurPage = wpInstalling Then
            {Delete the previous start menu group if the location has changed since the last install}
            DeletePreviousStartMenu;
    If CurPage = wpFinished Then Begin
      DeleteRenamedFiles;
      RegisterUserTasks;
    End;
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

#include "modpathNonAdmin.iss"

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
		DeleteFile(appdir + '\uninsTasks.txt');
		UnregisterWinMergeContextMenuPackage()
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

function AreSourceAndDestinationOfWinMergeContextMenuSame(Filename: String) : Boolean;
var
  ver: String;
begin
  ver := ''
  GetVersionNumbersString(Filename, ver);
  if ver = ExpandConstant('{#WinMergeContextMenuVersion}') then
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
  g_IsExplorerRestartRequired := false;
end;

procedure DeinitializeSetup();
var
  ResultCode: Integer;
  { Start Explorer just before exiting the installer if the Explorer restart that was run to install the ShellExtension fails. }
begin
  if FindWindowByClassName('progman') = 0 then begin
    Exec(ExpandConstant('{win}\explorer.exe'), '', '', SW_SHOW, ewNoWait, ResultCode)
  end
end;

Function IsWindows11OrLater(): Boolean;
Var
  OSVersion: TWindowsVersion;
Begin
  Result := false;
  GetWindowsVersionEx(OSVersion);
  if OSVersion.Major > 10 then
    Result := true
  else if (OSVersion.Major = 10) and (OSVersion.Minor > 0) then
    Result := true
  else if (OSVersion.Major = 10) and (OSVersion.Build >= 22000) then
    Result := true
  else
    Result := false;
End;
