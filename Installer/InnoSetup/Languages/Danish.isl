; Translation made with Translator 1.32 (http://www2.arnes.si/~sopjsimo/translator.html)
; $Translator:NL=%n:TB=%t
;
; *** Inno Setup version 4.1.8+ Danish messages ***
;
; To download user-contributed translations of this file, go to:
;   http://www.jrsoftware.org/is3rdparty.htm
;
; Note: When translating this text, do not add periods (.) to the end of
; messages that didn't have them already, because on those messages Inno
; Setup adds the periods automatically (appending a period would result in
; two periods being displayed).
;
; $Id$

[LangOptions]
LanguageName=Danish
LanguageID=$0406
; If the language you are translating to requires special font faces or
; sizes, uncomment any of the following entries and change them accordingly.
;DialogFontName=MS Shell Dlg
;DialogFontSize=8
;DialogFontStandardHeight=13
;TitleFontName=Arial
;TitleFontSize=29
;WelcomeFontName=Arial
;WelcomeFontSize=12
;CopyrightFontName=Arial
;CopyrightFontSize=8


[Messages]
; *** Application titles
SetupAppTitle=Setup
SetupWindowTitle=Setup - %1
UninstallAppTitle=Afinstaller
UninstallAppFullTitle=%1 Afinstaller

; *** Misc. common
InformationTitle=Information
ConfirmTitle=Bekræft
ErrorTitle=Fejl

; *** SetupLdr messages
SetupLdrStartupMessage=Dette program installerer %1. Fortsæt?
LdrCannotCreateTemp=Kan ikke danne midlertidig fil. Installation er afbrudt.
LdrCannotExecTemp=Kan ikke udføre et program i den midlertidige mappe. Installationen er afbrudt.

; *** Startup error messages
LastErrorMessage=%1.%n%nFejl %2: %3
SetupFileMissing=Filen %1 mangler i installations-mappen. Ret fejlen eller skaf en ny kopi af programmet.
SetupFileCorrupt=Installationsprogrammet er ødelagt. Skaf en ny kopi af programmet.
SetupFileCorruptOrWrongVer=Installationsfilerne er ødelagt, eller også er de ikke kompatible med denne version af installationen. Ret fejlen eller skaf en ny kopi af programmet.
NotOnThisPlatform=Programmet kan ikke anvendes på %1
OnlyOnThisPlatform=Programmet kan kun anvendes på %1
WinVersionTooLowError=Programmet kræver %1 version %2 eller nyere.
WinVersionTooHighError=Programmet kan ikke installeres på %1 version %2 eller nyere.
AdminPrivilegesRequired=Du skal være logget på som administrator for at kunne installere dette program.
PowerUserPrivilegesRequired=Du skal være logget på som administrator eller være medlem af superbruger-gruppen for at kunne installere dette program.
SetupAppRunningError=Programmet %1 er aktivt.%n%nAfslut venligst programmet, og klik OK for at fortsætte, eller Annuller for at afbryde.
UninstallAppRunningError=Programmet %1 er aktivt.%n%nAfslut venligst programmet, og klik OK for at fortsætte, eller Annuller for at afbryde.

; *** Misc. errors
ErrorCreatingDir=Installationen kunne ikke oprette mappen "%1"
ErrorTooManyFilesInDir=Det kan ikke lade sig gøre at oprette en fil i mappen "%1" da mappen indeholder for mange filer

; *** Setup common messages
ExitSetupTitle=Afslut installationen
ExitSetupMessage=Installationen er ikke færdiggjort. Hvis der afbrydes nu, vil programmet ikke blive installeret.%n%nInstallations programmet skal køres igen for at færdiggøre installationen.%n%nAfslut installationen?
AboutSetupMenuItem=&Om Installation...
AboutSetupTitle=Om Installation
AboutSetupMessage=%1 version %2%n%3%n%n%1 hjemmeside:%n%4
AboutSetupNote=

; *** Buttons
ButtonBack=< &Tilbage
ButtonNext=Næ&ste >
ButtonInstall=&Installer
ButtonOK=&OK
ButtonCancel=&Afbryd
ButtonYes=&Ja
ButtonYesToAll=Ja til A&lle
ButtonNo=&Nej
ButtonNoToAll=Nej t&il Alle
ButtonFinish=&Færdig
ButtonBrowse=&Gennemse...

; *** "Select Language" dialog messages
ButtonWizardBrowse=G&ennemse...
ButtonNewFolder=&Opret Ny Mappe
SelectLanguageTitle=Vælg installationssprog
SelectLanguageLabel=Vælg hvilket sprog der skal anvendes under installationen:

; *** Common wizard text
ClickNext=Klik Næste for at fortsætte, eller Afbryd for at afslutte.
BeveledLabel=

; *** "Welcome" wizard page
BrowseDialogTitle=Gennemse Efter Mappe
BrowseDialogLabel=Vælg en mappe fra nedenstående liste. Klik dernæst OK.
NewFolderName=Ny Mappe
WelcomeLabel1=Velkommen til [name] installation.
WelcomeLabel2=Dette program installerer [name/ver] på computeren.%n%nDet anbefales at alle andre programmer afsluttes før der fortsættes.

; *** "Password" wizard page
WizardPassword=Adgangskode
PasswordLabel1=Installationen er beskyttet med adgangskode.
PasswordLabel3=Indtast adgangskoden og klik Næste for at fortsætte. Adgangskoder skelner imellem store og små bogstaver.
PasswordEditLabel=&Adgangskode:
IncorrectPassword=Adgangskoden er ikke korrekt. Prøv igen, og husk at der skelnes melllem store og små bogstaver.

; *** "License Agreement" wizard page
WizardLicense=Licensaftale
LicenseLabel=Læs venligst den følgende information, som er vigtig, inden du fortsætter.
LicenseLabel3=Læs venligst licensaftalen. Du skal acceptere betingelserne i aftalen for at fortsætte installationen.
LicenseAccepted=Jeg &accepterer aftalen
LicenseNotAccepted=Jeg accepterer &ikke aftalen

; *** "Information" wizard pages
WizardInfoBefore=Information
InfoBeforeLabel=Læs følgende information inden du fortsætter.
InfoBeforeClickLabel=Tryk på Næste, når du er klar til at fortsætte installationen.
WizardInfoAfter=Information
InfoAfterLabel=Læs følgende information inden du fortsætter.
InfoAfterClickLabel=Tryk på Næste, når du er klar til at fortsætte installationen.

; *** "User Information" wizard page
WizardUserInfo=Brugerinformation
UserInfoDesc=Indtast dine oplysninger.
UserInfoName=&Brugernavn:
UserInfoOrg=&Organisation:
UserInfoSerial=&Serienummer:
UserInfoNameRequired=Du skal indtaste et navn.

; *** "Select Destination Directory" wizard page
WizardSelectDir=Vælg installationsmappe
SelectDirDesc=Hvor skal [name] installeres?
SelectDirLabel3=Guilden installerer [name] i følgende mappe.
SelectDirBrowseLabel=Klik Næste for at fortsætte. Hvis du vil vælge en anden mappe skal du klikke Søg.
DiskSpaceMBLabel=Der skal være mindst [mb] MB diskplads.
ToUNCPathname=Kan ikke installere på et UNC-stinavn. Hvis du prøver på at installere på et netværk, er du nødt til at oprette et netværksdrev.
InvalidPath=Du skal indtaste den fulde sti med drevangivelse; for eksempel:%nC:\APP%n%neller et UNC-stinavn på formen:%n%n\\server\share
InvalidDrive=Drevet eller UNC-stien du valgte eksisterer ikke. Vælg venligst noget andet.
DiskSpaceWarningTitle=Ikke nok diskplads.
DiskSpaceWarning=Install kræver mindst %1 KB fri diskplads for at kunne installere, men det valgte drev har kun %2 KB diskplads tilgængeligt.%n%nVil du installere alligevel?
DirNameTooLong=Mappens eller stiens navn er for langt.
InvalidDirName=Mappenavnet er ikke gyldigt.
BadDirName32=Navne på mapper må ikke indeholde nogen af følgende karakterer:%n%n%1
DirExistsTitle=Mappen eksisterer
DirExists=Mappen:%n%n%1%n%neksisterer allerede. Ønsker du at installere i denne mappe alligevel?
DirDoesntExistTitle=Mappen eksisterer ikke.
DirDoesntExist=Mappen:%n%n%1%n%neksisterer ikke. Ønsker du at oprette denne mappe?

; *** "Select Components" wizard page
WizardSelectComponents=Vælg Komponenter
SelectComponentsDesc=Hvilke komponenter skal installeres?
SelectComponentsLabel2=Vælg de komponenter der skal installeres, og fjern markering fra dem der ikke skal installeres. Klik Næste for at fortsætte.
FullInstallation=Komplet installation
; if possible don't translate 'Compact' as 'Minimal' (I mean 'Minimal' in your language)
CompactInstallation=Kompakt installation
CustomInstallation=Valgfri installation
NoUninstallWarningTitle=Komponent er installeret
NoUninstallWarning=Installationen har konstateret at følgende komponenter allerede er installeret på computeren:%n%n%1%n%nAt fravælge komponenterne vil ikke fjerne dem.%n%nFortsæt alligevel?
ComponentSize1=%1 KB
ComponentSize2=%1 MB
ComponentsDiskSpaceMBLabel=Det valgte kræver mindst [mb] MB fri plads på harddisken.

; *** "Select Additional Tasks" wizard page
WizardSelectTasks=Vælg ekstra opgaver
SelectTasksDesc=Hvilke andre opgaver skal udføres?
SelectTasksLabel2=Vælg hvilke ekstraopgaver der skal udføres under installationen af [name] og klik på Næste.

; *** "Select Start Menu Folder" wizard page
WizardSelectProgramGroup=Vælg Start Menu mappe
SelectStartMenuFolderDesc=Hvor skal installationen oprette genveje til programmet?
SelectStartMenuFolderLabel3=Installationsguiden opretter genveje (ikoner) til programmet i følgende mappe i Start-menuen.
SelectStartMenuFolderBrowseLabel=Klik Næste for at fortsætte. Hvis du vil vælge en anden mappe skal du klikke Søg.
NoIconsCheck=&Opret ikke ikoner
MustEnterGroupName=Der skal angives et mappe navn.
GroupNameTooLong=Mappens eller stiens navn er for langt.
InvalidGroupName=Mappenavnet er ikke gyldigt.
BadGroupName=Tegnene %1 må ikke anvendes i navnet på en programgruppe. Ret navnet.
NoProgramGroupCheck2=Opret ikke en programgruppe i Start Menuen

; *** "Ready to Install" wizard page
WizardReady=Klar til at installere
ReadyLabel1=Installationen er nu klar til at installere [name] på din computer.
ReadyLabel2a=Tryk på Installer for at fortsætte med installationen, eller tryk på Tilbage hvis du ønsker at se eller ændre dine indstillinger.
ReadyLabel2b=Tryk på Installer for at fortsætte med installationen.
ReadyMemoUserInfo=Bruger oplysninger:
ReadyMemoDir=Destinations mappe :
ReadyMemoType=Installationstype:
ReadyMemoComponents=Valgte komponenter:
ReadyMemoGroup=Start Menu mappe:
ReadyMemoTasks=Valgte ekstraopgaver:

; *** "Preparing to Install" wizard page
WizardPreparing=Forbereder installationen
PreparingDesc=Setup forbereder installationen af [name] på din computer.
PreviousInstallNotCompleted=Den foregående installation eller fjernelse af et program er ikke afsluttet. Du skal genstarte computeren for at afslutte den foregående installation.%n%nEfter genstarten skal du køre Setup igen for at fuldføre installationen af [name].
CannotContinue=Setup kan ikke fortsætte. Klip på Fortryd for at afslutte.

; *** "Installing" wizard page
WizardInstalling=Installerer
InstallingLabel=Vent mens Setup installerer [name] på din computer.

; *** "Setup Completed" wizard page
FinishedHeadingLabel=Afslutter installation af [name]
FinishedLabelNoIcons=[name] er blevet installeret på din computer.
FinishedLabel=[name] er blevet installeret på din computer. [name] kan startes ved at vælge dets genveje i Start-Menu'en.
ClickFinish=Klik på Færdig for at afslutte installationsprogrammet.
FinishedRestartLabel=For at fuldføre installationen af [name], skal din computer genstartes. Vil du genstarte computeren nu?
FinishedRestartMessage=For at fuldføre installationen af [name], skal din computer genstartes.%n%nVil du genstarte computeren nu?
ShowReadmeCheck=Ja, jeg vil gerne læse README filen
YesRadio=&Ja, genstart computeren nu
NoRadio=&Nej, jeg genstarter selv computeren senere
; used for example as 'Run MyProg.exe'
RunEntryExec=Kør %1
; used for example as 'View Readme.txt'
RunEntryShellExec=Læs %1

; *** "Setup Needs the Next Disk" stuff
ChangeDiskTitle=Installationsprogrammet skal bruge den næste disk(ette)
SelectDiskLabel2=Indsæt Disk %1 og klik OK.%n%nHvis filerne findes i en anden mappe så indtast stien eller klik Gennemse.
PathLabel=&Path:
FileNotInDir2=Filen "%1" findes ikke i "%2". Indsæt den rigtige disk eller vælg en anden mappe.
SelectDirectoryLabel=Specificer placeringen af den næste disk.

; *** Installation phase messages
SetupAborted=Installationen blev ikke gennemført. Installer igen, hent programmet på ny, eller kontakt producenten for hjælp.
EntryAbortRetryIgnore=Klik Gentag for at forsøge igen, Ignorer for at fortsætte alligevel, eller Afbryd for at annullere installationen.

; *** Installation status messages
StatusCreateDirs=Opretter mapper...
StatusExtractFiles=Udpakker filer...
StatusCreateIcons=Opretter program genveje...
StatusCreateIniEntries=Opretter INI filer...
StatusCreateRegistryEntries=Opdaterer registrerings databasen...
StatusRegisterFiles=Registerer filer...
StatusSavingUninstall=Gemmer afinstallations information...
StatusRunProgram=Færdiggører installationen...
StatusRollback=Ruller ændringerne tilbage...

; *** Misc. errors
ErrorInternal2=Intern fejl: %1
ErrorFunctionFailedNoCode=%1 fejlede
ErrorFunctionFailed=%1 fejlede; kode %2
ErrorFunctionFailedWithMessage=%1 fejlede; kode %2.%n%3
ErrorExecutingProgram=Kan ikke udføre filen:%n%1

; *** Registry errors
ErrorRegOpenKey=Fejl ved åbning af  registrerings nøgle:%n%1\%2
ErrorRegCreateKey=Fejl ved oprettelse af registrerings nøgle:%n%1\%2
ErrorRegWriteKey=Fejl ved skrivning til registrerings nøgle:%n%1\%2

; *** INI errors
ErrorIniEntry=Fejl ved oprettelse af variabel i INI-filen "%1".

; *** File copying errors
FileAbortRetryIgnore=Klik Gentag for at prøve igen, Ignorer for at springe filen over (kan ikke anbefales) eller Afbryd for at afslutte installationen.
FileAbortRetryIgnore2=Klik Gentag for at prøve igen, Ignorer for at fortsætte alligevel (kan ikke anbefales) eller Afbryd for at afslutte installationen.
SourceIsCorrupted=Kildefilen er beskadiget
SourceDoesntExist=Kildefilen "%1" eksistere ikke
ExistingFileReadOnly=Den eksisterende fil er markeret som skrivebeskyttet.%n%nKlik Gentag for at prøve igen (efter at du har fjernet skrivebeskyttelsen), Ignorer for at springe filen over eller afbryd for at afslutte installationen.
ErrorReadingExistingDest=Der opsted en fejl ved forsøg på at læse den eksisterende fil:
FileExists=Filen eksisterer allerede.%n%nSkal Setup overskrive den?
ExistingFileNewer=Den eksisterende fil er nyere end den installation forsøger at skrive. Det anbefales at beholde den eksisterende fil.%n%n Skal den eksisterende fil beholdes?
ErrorChangingAttr=En fejl opstod ved forsøget på at ændre attributer for den eksisterende fil:
ErrorCreatingTemp=En fejl opstod ved forsøg på at oprette en fil i mappen:
ErrorReadingSource=En fejl opstod ved forsøg på at læse kildefilen:
ErrorCopying=En fejl opstod ved forsøg på at kopiere en fil:
ErrorReplacingExistingFile=En fejl opstod ved forsøg på at overskrive den eksisterende fil:
ErrorRestartReplace=Genstart/Erstat fejlede:
ErrorRenamingTemp=En fejl opstod ved forsøg på at omdøbe en fil i modtage mappen:
ErrorRegisterServer=Kan ikke registrere DLL/OCX: %1
ErrorRegisterServerMissingExport=Funktionen DllRegisterServer, der er nødvendig, for at filen kan registreres, findes ikke i filen
ErrorRegisterTypeLib=Kan ikke registrere type biblioteket: %1

; *** Post-installation errors
ErrorOpeningReadme=Der opstod en fejl ved forsøg på at åbne README filen.
ErrorRestartingComputer=Installationen kunne ikke genstarte computeren. Genstart venligst computeren manuelt.

; *** Uninstaller messages
UninstallNotFound=Filen "%1" eksisterer ikke, afinstalleringen kan ikke fortsætte.
UninstallOpenError=Filen "%1" kunne ikke åbnes. Kan ikke afinstallere
UninstallUnsupportedVer=Afinstallerings log filen "%1" er i et format der ikke kan genkendes denne version af afinstallations-programmet. Afinstalleringen afbrydes
UninstallUnknownEntry=Der er en ukent kommando (%1) i afinstallings logfilen.
ConfirmUninstall=Er du sikker på at %1 og alle dens komponenter skal fjernes fra computeren?
OnlyAdminCanUninstall=Programmet kan kun afinstalleres af en bruger med administrator-rettigheder.
UninstallStatusLabel=Vent venligst imens %1 afinstalleres.
UninstalledAll=%1 er afinstalleret med succes.
UninstalledMost=%1 Afinstalleringer er afsluttet.%n%nNogle filer kunne ikke fjernes. Disse må fjernes manuelt, hvis du ikke ønsker de skal blive liggende.
UninstalledAndNeedsRestart=For at afslutte afinstalleringen af %1 skal computeren genstartes.%n%nVil du genstarte nu?
UninstallDataCorrupted="%1" er beskadiget. Afinstallering kan ikke foretages

; *** Uninstallation phase messages
ConfirmDeleteSharedFileTitle=Fjern Delt Fil?
ConfirmDeleteSharedFile2=Systemet mener ikke længere at følgende delte fil(er) benyttes. Skal afinstallering fjerne de(n) delte fil(er)?%n%nHvis du er usikker så vælg Nej. Beholdes filen på maskinen, vil den ikke gøre nogen skade, men hvis filen fjernes, selv om den stadig anvendes, bliver de programmer, der anvender filen, ustabile
SharedFileNameLabel=Filnavn:
SharedFileLocationLabel=Placering:
WizardUninstalling=Afinstallations Status
StatusUninstalling=Afinstallerer %1...

; The custom messages below aren't used by Setup itself, but if you make
; use of them in your scripts, you'll want to translate them.

[CustomMessages]
;Inno Setup Built-in Custom Messages
NameAndVersion=%1 version %2
AdditionalIcons=Additional icons:
CreateDesktopIcon=Create a &desktop icon
CreateQuickLaunchIcon=Create a &Quick Launch icon
ProgramOnTheWeb=%1 on the Web
UninstallProgram=Uninstall %1
LaunchProgram=Launch %1
AssocFileExtension=&Associate %1 with the %2 file extension
AssocingFileExtension=Associating %1 with the %2 file extension...

;Things we can also localize
CompanyName=Thingamahoochie Software

;Types
TypicalInstallation=Typical Installation
FullInstallation=Full Installation
CompactInstallation=Compact Installation
CustomInstallation=Custom Installation

;Components
AppCoreFiles=WinMerge Core Files
ApplicationRuntimes=Application Runtimes
UsersGuide=User's Guide
Filters=Filters
Plugins=Plugins (See Plugins.txt)

;Localization Components
Languages=Languages
BulgarianLanguage=Bulgarian menus and dialogs
CatalanLanguage=Catalan menus and dialogs
ChineseSimplifiedLanguage=Chinese (Simplified) menus and dialogs
ChineseTraditionalLanguage=Chinese (Traditional) menus and dialogs
CzechLanguage=Czech menus and dialogs
DanishLanguage=Danish menus and dialogs
DutchLanguage=Dutch menus and dialogs
FrenchLanguage=French menus and dialogs
GermanLanguage=German menus and dialogs
ItalianLanguage=Italian menus and dialogs
JapaneseLanguage=Japanese menus and dialogs
KoreanLanguage=Korean menus and dialogs
NorwegianLanguage=Norwegian menus and dialogs
PolishLanguage=Polish menus and dialogs
PortugueseLanguage=Portuguese (Brazilian) menus and dialogs
RussianLanguage=Russian menus and dialogs
SlovakLanguage=Slovak menus and dialogs
SpanishLanguage=Spanish menus and dialogs
SwedishLanguage=Swedish menus and dialogs

;Tasks
ExplorerContextMenu=&Enable Explorer context menu integration
IntegrateTortoiseCVS=Integrate with &TortoiseCVS
IntegrateDelphi4=Borland® Delphi &4 Binary File Support
UpdatingCommonControls=Updating the System's Common Controls

;Icon Labels
ReadMe=Read Me
UsersGuide=User's Guide
UpdatingCommonControls=Updating the System's Common Controls
ViewStartMenuFolder=&View the WinMerge Start Menu Folder

;Code Dialogs
DeletePreviousStartMenu=The installer has detected that you changed the location of your start menu from "%s" to "%s". Would you like to delete the previous start menu folder?