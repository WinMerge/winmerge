; *** Inno Setup version 4.2.1+ Swedish messages ***
;
; Translated by stefan@bodingh.se
; -------+---------+---------+---------+---------+---------+---------+---------
; Translators notes (for Swedes only :)
;
; Rev 040510: för WinMerge av hans.ericson@bredband.net
; Rev 040128: 4.1.2 av stefan@bodingh.se
; Rev 030501: 4.0.0 av stefan@bodingh.se
; Rev 020903: 3.0.3 av soren.edzen@frontec.se
; Rev 020602: 3.0.2 av peter@peterandlinda.com
; Rev 010508: 2.0.8 av peter.kind@telia.com
; Rev 001019: 1.3.21 av peter.kind@telia.com
; Rev 000428: 1.3x
; Rev 991028: Smärre ändringar.
; Rev 990927: Nytt
;
; Note: When translating this text, do not add periods (.) to the end of
; messages that didn't have them already, because on those messages Inno
; Setup adds the periods automatically (appending a period would result in
; two periods being displayed).
;

[LangOptions]
LanguageName=Svenska
LanguageID=$041D

[Messages]

; *** Application titles
SetupAppTitle=Installationsprogram
SetupWindowTitle=Installationsprogram för %1
UninstallAppTitle=Avinstallation
UninstallAppFullTitle=%1 Avinstallation

; *** Misc. common
InformationTitle=Information
ConfirmTitle=Bekräfta
ErrorTitle=Fel

; *** SetupLdr messages
SetupLdrStartupMessage=%1 kommer att installeras. Vill du fortsätta?
LdrCannotCreateTemp=Kan ej skapa en temporär fil. Installationen avbryts
LdrCannotExecTemp=Kan inte köra fil i temporär mapp. Installationen avbryts

; *** Startup error messages
LastErrorMessage=%1.%n%nFel %2: %3
SetupFileMissing=Filen %1 saknas i installationsmappen. Rätta till problemet eller hämta en ny kopia av programmet.
SetupFileCorrupt=Installationsfilerna är felaktiga. Hämta en ny kopia av programmet
SetupFileCorruptOrWrongVer=Installationsfilerna är felaktiga, eller stämmer ej överens med denna version av installationsprogrammet. Rätta till felet eller hämta en ny programkopia.
NotOnThisPlatform=Detta program kan ej köras på %1.
OnlyOnThisPlatform=Detta program måste ha %1.
WinVersionTooLowError=Detta program kräver %1, version %2 eller senare.
WinVersionTooHighError=Programmet kan inte installeras på %1 version %2 eller senare.
AdminPrivilegesRequired=Du måste vara inloggad som administratör när du installerar detta program.
PowerUserPrivilegesRequired=Du måste vara inloggad som administratör eller medlem av gruppen Privilegierade användare (Power Users) när du installerar  detta program.
SetupAppRunningError=Installationsprogrammet har upptäckt att %1 är igång.%n%nAvsluta det angivna programmet nu. Klicka sedan på OK för att gå vidare, eller på Avbryt för att avsluta.
UninstallAppRunningError=Avinstalleraren har upptäckt att %1 körs för tillfället.%n%nStäng all öppna instanser av det nu, Klicka sedan på OK för att gå vidare, eller på Avbryt för att avsluta.

; *** Misc. errors
ErrorCreatingDir=Kunde inte skapa mappen "%1"
ErrorTooManyFilesInDir=Kunde inte skapa en fil i mappen "%1" därför att den innehåller för många filer

; *** Setup common messages
ExitSetupTitle=Avsluta installationen
ExitSetupMessage=Installationen är inte färdig. Om du avslutar nu, kommer programmet inte att installeras.%n%nDu kan köra installationsprogrammet vid ett senare tillfälle för att slutföra installationen.%n%nVill du avbryta installationen?
AboutSetupMenuItem=&Om installationsprogrammet...
AboutSetupTitle=Om installationsprogrammet
AboutSetupMessage=%1 version %2%n%3%n%n%1 hemsida:%n%4
AboutSetupNote=Svensk översättning är gjord av dickg@go.to 1999, 2002%n%nUppdatering till 3.0.2+ av peter@peterandlinda.com, 4.x.x av  Stefan Bodingh

; *** Buttons
ButtonBack=< &Tillbaka
ButtonNext=&Nästa >
ButtonInstall=&Installera
ButtonOK=OK
ButtonCancel=Avbryt
ButtonYes=&Ja
ButtonYesToAll=Ja till &Allt
ButtonNo=&Nej
ButtonNoToAll=N&ej till allt
ButtonFinish=&Slutför
ButtonBrowse=&Bläddra...
ButtonWizardBrowse=&Bläddra...
ButtonNewFolder=Skapa ny mapp
SelectLanguageTitle=Välj språk för installationen
SelectLanguageLabel=Välj språk som skall användas under installationen:

; *** Common wizard text
ClickNext=Klicka på Nästa för att fortsätta eller på Avbryt för att avsluta installationen.
BeveledLabel=
BrowseDialogTitle=Bläddra mapp
BrowseDialogLabel=Välj en mapp i listan nedan, klicka sedan på OK.
NewFolderName=Ny mapp

; *** "Welcome" wizard page
;WizardWelcome=Välkommen
;WelcomeLabel1=Välkommen till installationsprogrammet för [name].
;WelcomeLabel2=Detta kommer att installera [name/ver] på din dator.%n%nDet rekommenderas att du avslutar alla andra program innan du fortsätter. Det förebygger konflikter under installationens gång.
; *** "Password" wizard page
WelcomeLabel1=Välkommen till installationen av [name]
WelcomeLabel2=Installationsprogrammet kommer att installera [name/ver] på din dator.
WizardPassword=Lösenord
PasswordLabel1=Denna installation är skyddad med lösenord.
PasswordLabel3=Var god ange lösenordet, klicka sedan på Nästa för att fortsätta. Lösenord skiljer på versaler/gemener.
PasswordEditLabel=&Lösenord:
IncorrectPassword=Lösenordet du angav är inkorrekt. Försök igen.

; *** "License Agreement" wizard page
WizardLicense=Licensavtal
LicenseLabel=Var god och läs följande viktiga information innan du fortsätter.
LicenseLabel3=Var god och läs följande licensavtal. Du måste acceptera villkoren i avtalet innan du kan fortsätta med installationen.
LicenseAccepted=Jag &accepterar avtalet
LicenseNotAccepted=Jag accepterar &inte avtalet

; *** "Information" wizard pages
WizardInfoBefore=Information
InfoBeforeLabel=Var god läs följande viktiga information innan du fortsätter.
InfoBeforeClickLabel=När du är klar att fortsätta installationen klickar du på Nästa.
WizardInfoAfter=Information
InfoAfterLabel=Var god läs följande viktiga information innan du fortsätter.
InfoAfterClickLabel=När du är klar att fortsätta installationen klickar du på Nästa.

; *** "User Information" wizard page
WizardUserInfo=Användarinformation
UserInfoDesc=Var god och fyll i följande uppgifter.
UserInfoName=&Namn:
UserInfoOrg=&Organisation:
UserInfoSerial=&Serienummer:
UserInfoNameRequired=Du måste fylla i ett namn.

; *** "Select Destination Directory" wizard page
WizardSelectDir=Välj installationsplats
SelectDirDesc=Var skall [name] installeras?
SelectDirLabel3=Installationsprogrammet kommer att installera [name] i följande mapp
SelectDirBrowseLabel=För att fortsätta klickar du på Nästa. Om du vill välja en annan mapp klickar du på Bläddra.
DiskSpaceMBLabel=Programmet kräver minst [mb] MB hårddiskutrymme.
ToUNCPathname=Installationsprogrammet kan inte installera till UNC sökväg. Om du försöker installera via ett nätverk, ska du ange  nätverksenheten.
InvalidPath=Du måste skriva en fullständig sökväg med enhetsbeteckning; till exempel:%C:\APP
InvalidDrive=Enheten du har valt finns inte eller är inte tillgänglig. Välj en annan.
DiskSpaceWarningTitle=Ej tillräckligt med diskutrymme
DiskSpaceWarning=Installationsprogrammet behöver åtminstone %1 KB ledigt diskutrymme för installationen, men den valda enheten har bara %2  KB tillgängligt.%n%nVill du fortsätta ändå?
DirNameTooLong=Mappens namn eller sökväg är för lång.
InvalidDirName=Mappen du har valt är inte tillgänglig.
BadDirName32=Mappens namn får ej innehålla något av följande tecken:%n%n%1
DirExistsTitle=Mappen finns
DirExists=Mappen:%n%n%1%n%nfinns redan. Vill du ändå fortsätta installationen till den valda mappen?
DirDoesntExistTitle=Mappen finns inte
DirDoesntExist=Mappen:%n%n%1%n%nfinns inte. Vill du skapa den?

; *** "Select Components" wizard page
WizardSelectComponents=Välj komponenter
SelectComponentsDesc=Vilka komponenter skall installeras?
SelectComponentsLabel2=Välj de komponenter som du vill ska installeras; avmarkera de komponenter som du inte vill ha. Klicka sedan på Nästa när du är klar att fortsätta.
FullInstallation=Fullständig installation

; if possible don't translate 'Compact' as 'Minimal' (I mean 'Minimal' in your language)
CompactInstallation=Kompakt installation
CustomInstallation=Anpassad installation
NoUninstallWarningTitle=Komponenter finns
NoUninstallWarning=Installationsprogrammet har upptäckt att följande komponenter redan finns installerade på din dator:%n%n%1%n%nAtt  avmarkera dessa komponenter kommer inte att avinstallera dom.%n%nVill du fortsätta ändå?
ComponentSize1=%1 KB
ComponentSize2=%1 MB
ComponentsDiskSpaceMBLabel=Aktuella val kräver minst [mb] MB diskutrymme.

; *** "Select Additional Tasks" wizard page
WizardSelectTasks=Välj extra uppgifter
SelectTasksDesc=Vilka extra uppgifter skall utföras?
SelectTasksLabel2=Markera ytterligare uppgifter att utföra vid installation av [name], tryck sedan på Nästa.

; *** "Select Start Menu Folder" wizard page
WizardSelectProgramGroup=Välj Startmeny-mapp
SelectStartMenuFolderDesc=Var skall installationsprogrammet placera programmets genvägar?
SelectStartMenuFolderLabel3=Installationsprogrammet kommer att skapa programmets genvägar i följande mapp.
SelectStartMenuFolderBrowseLabel=För att fortsätta klickar du på Nästa. Om du vill välja en annan mapp klickar du på Bläddra.
NoIconsCheck=&Skapa inte ikoner
MustEnterGroupName=Du måste ange en mapp.
GroupNameTooLong=Mappens namn eller sökväg är för lång.
InvalidGroupName=Mappen du har valt är inte tillgänglig.
BadGroupName=Mappnamnet kan inte innehålla någon av följande tecken:%n%n%1
NoProgramGroupCheck2=&Skapa ingen Startmeny-mapp

; *** "Ready to Install" wizard page
WizardReady=Redo att installera
ReadyLabel1=Installationsprogrammet är nu redo att installera [name] på din dator.
ReadyLabel2a=Tryck på Installera om du vill fortsätta, eller på gå Tillbaka om du vill granska eller ändra på något.
ReadyLabel2b=Välj Installera för att påbörja installationen.
ReadyMemoUserInfo=Användarinformation:
ReadyMemoDir=Installationsplats:
ReadyMemoType=Installationstyp:
ReadyMemoComponents=Valda komponenter:
ReadyMemoGroup=Startmeny-mapp:
ReadyMemoTasks=Extra uppgifter:

; *** "Preparing to Install" wizard page
WizardPreparing=Förbereder installationen
PreparingDesc=Installationsprogrammet förbereder installationen av [name] på din dator.
PreviousInstallNotCompleted=Installationen/avinstallationen av ett tidigare program har inte slutförts. Du måste starta om datorn för att avsluta den installationen.%n%nEfter att ha startat om datorn kör du installationsprogrammet igen för att slutföra installationen av [name].
CannotContinue=Installationsprogrammet kan inte fortsätta. Klicka på Avbryt för att avsluta.

; *** "Installing" wizard page
WizardInstalling=Installerar
InstallingLabel=Vänta medan [name] installeras på din dator.

; *** "Setup Completed" wizard page
FinishedHeadingLabel=Avslutar installationen av [name]
FinishedLabelNoIcons=[name] har nu installerats på din dator.
FinishedLabel=[name] har nu installerats på din dator. Programmet kan startas genom att välja någon av ikonerna.
ClickFinish=Välj Slutför för att avsluta installationen.
FinishedRestartLabel=För att slutföra installationen av [name], måste datorn startas om. Vill du starta om nu?
FinishedRestartMessage=För att slutföra installationen av [name], måste datorn startas om.%n%nVill du starta om datorn nu?
ShowReadmeCheck=Ja, jag vill se filen LÄS MIG
YesRadio=&Ja, jag vill starta om datorn nu
NoRadio=&Nej, jag startar själv om datorn senare

; used for example as 'Run MyProg.exe'
RunEntryExec=Kör %1

; used for example as 'View Readme.txt'
RunEntryShellExec=Läs %1

; *** "Setup Needs the Next Disk" stuff
ChangeDiskTitle=Installationsprogrammet behöver nästa diskett
SelectDiskLabel2=Var god sätt i diskett %1 och tryck OK.%n%nOm filerna kan hittas i en annan mapp än den som visas nedan, skriv in rätt sökväg eller välj Bläddra.
PathLabel=&Sökväg:
FileNotInDir2=Kunde inte hitta filen "%1" i "%2". Var god sätt i korrekt diskett eller välj en annan mapp.
SelectDirectoryLabel=Var god ange sökvägen för nästa diskett.

; *** Installation phase messages
SetupAborted=Installationen slutfördes inte.%n%nVar god rätta till felet och kör installationen igen.
EntryAbortRetryIgnore=Välj Försök igen eller Ignorera för att fortsätta ändå, eller välj Avbryt för att avbryta installationen.

; *** Installation status messages
StatusCreateDirs=Skapar mappar...
StatusExtractFiles=Packar upp filer...
StatusCreateIcons=Skapar programikoner...
StatusCreateIniEntries=Skriver INI-värden...
StatusCreateRegistryEntries=Skriver register-värden...
StatusRegisterFiles=Registrerar filer...
StatusSavingUninstall=Sparar information för avinstallation...
StatusRunProgram=Slutför installationen...
StatusRollback=Återställer ändringar...

; *** Misc. errors
ErrorInternal2=Internt fel: %1
ErrorFunctionFailedNoCode=%1 misslyckades
ErrorFunctionFailed=%1 misslyckades; kod %2
ErrorFunctionFailedWithMessage=%1 misslyckades; kod %2.%n%3
ErrorExecutingProgram=Kan inte köra filen:%n%1

; *** Registry errors
ErrorRegOpenKey=Fel vid öppning av registernyckel:%n%1\%2
ErrorRegCreateKey=Kan ej skapa registernyckel:%n%1\%2
ErrorRegWriteKey=Kan ej skriva till registernyckel:%n%1\%2

; *** INI errors
ErrorIniEntry=Kan inte skriva nytt INI-värde i filen "%1".

; *** File copying errors
FileAbortRetryIgnore=Välj Försök igen eller Ignorera för att hoppa över denna fil (ej rekommenderat), eller välj Avbryt installationen.
FileAbortRetryIgnore2=Välj Försök igen eller Ignorera och fortsätt ändå (ej rekommenderat), eller välj Avbryt installationen.
SourceIsCorrupted=Källfilen är felaktig
SourceDoesntExist=Källfilen "%1" finns inte
ExistingFileReadOnly=Den nuvarande filen är skrivskyddad.%n%nVälj Försök igen för att ta bort skrivskyddet, Ignorera för att hoppa över denna fil, eller välj Avbryt installationen.
ErrorReadingExistingDest=Ett fel uppstod vid försök att läsa den befintliga filen:
FileExists=Filen finns redan.%n%nVill du skriva över den?
ExistingFileNewer=Den befintliga filen är nyare än den som ska installeras. Du rekommenderas att behålla den befintliga filen. %n%nVill Du behålla den befintliga filen?
ErrorChangingAttr=Ett fel uppstod vid försök att ändra attribut på den befintliga filen:
ErrorCreatingTemp=Ett fel uppstod vid ett försök att skapa installationsmappen:
ErrorReadingSource=Ett fel uppstod vid ett försök att läsa källfilen:
ErrorCopying=Ett fel uppstod vid kopiering av filen:
ErrorReplacingExistingFile=Ett fel uppstod vid ett försök att ersätta den befintliga filen:
ErrorRestartReplace=ÅterstartaErsätt misslyckades:
ErrorRenamingTemp=Ett fel uppstod vid ett försök att byta namn på en fil i installationsmappen:
ErrorRegisterServer=Kunde inte registrera DLL/OCX: %1
ErrorRegisterServerMissingExport=DllRegisterServer export kunde inte hittas
ErrorRegisterTypeLib=Kunde inte registrera typbibliotek: %1

; *** Post-installation errors
ErrorOpeningReadme=Ett fel uppstod vid öppnandet av LÄS MIG-filen.
ErrorRestartingComputer=Installationsprogrammet kunde inte starta om datorn. Var god gör det manuellt.

; *** Uninstaller messages
UninstallNotFound=Filen "%1" finns inte. Kan inte avinstallera.
UninstallOpenError=Filen "%1" kan inte öppnas. Kan inte avinstallera.
UninstallUnsupportedVer=Avinstallationsloggen "%1" är i ett format som denna version inte känner igen. Kan ej avinstallera
UninstallUnknownEntry=En okänd rad (%1) hittades i avinstallationsloggen
ConfirmUninstall=Är du säker på att du vill ta bort %1 och alla tillhörande komponenter?
OnlyAdminCanUninstall=Denna installation kan endast avinstalleras av en användare med administrativa rättigheter.
UninstallStatusLabel=Var god och vänta medan %1 tas bort från din dator.
UninstalledAll=%1 är nu borttaget från din dator.
UninstalledMost=Avinstallationen av %1 är nu klar.%n%nEn del filer/mappar gick ej att ta bort. Dessa kan tas bort manuellt.
UninstalledAndNeedsRestart=För att slutföra avinstallationen av %1 måste datorn startas om.%n%nVill du starta om nu?
UninstallDataCorrupted=Filen "%1" är felaktig. Kan inte avinstallera

; *** Uninstallation phase messages
ConfirmDeleteSharedFileTitle=Ta bort delad fil?
ConfirmDeleteSharedFile2=Systemet indikerar att följande delade fil inte längre används av några program. Vill du ta bort den delade filen?%n%n%1%n%nOm något program fortfarande använder denna fil och den raderas, kommer programmet kanske att sluta fungera. Om du är osäker, välj Nej. Att låta filen ligga kvar i systemet kommer inte att orsaka någon skada.
SharedFileNameLabel=Filnamn:
SharedFileLocationLabel=Plats:
WizardUninstalling=Avinstallationsstatus
StatusUninstalling=Avinstallerar %1...
[CustomMessages]
NameAndVersion=%1 version %2
AdditionalIcons=Återstående ikoner:
CreateDesktopIcon=Skapa en ikon på Skrivbordet
CreateQuickLaunchIcon=Skapa en ikon i Snabbstartfältet
ProgramOnTheWeb=%1 på Webben
UninstallProgram=Avinstallera %1
LaunchProgram=Starta %1
AssocFileExtension=&Associera %1 med %2 filnamnstillägg
AssocingFileExtension=Associerar %1 med %2 filnamnstillägg...

;Things we can also localize
CompanyName=Thingamahoochie Software

;Types
TypicalInstallation=Typisk Installation
FullInstallation=Full Installation
CompactInstallation=Kompakt Installation
CustomInstallation=Anpassad Installation

;Components
AppCoreFiles=WinMerge Kärnfiler
ApplicationRuntimes=Applikationsexekuterbara filer
UsersGuide=Användarguide
Filters=Filter
Plugins=Plugins (Se Plugins.txt)

;Localization Components
Languages=Språk
BulgarianLanguage=Bulgariska menyer och dialoger
CatalanLanguage=Katalanska menyer och dialoger
ChineseSimplifiedLanguage=Kinesiska (Förenklad) menyer och dialoger
ChineseTraditionalLanguage=Kinesiska (Traditionell) menyer och dialoger
CzechLanguage=Tjeckiska menyer och dialoger
DanishLanguage=Danska menyer och dialoger
DutchLanguage=Nederländska menyer och dialoger
FrenchLanguage=Franska menyer och dialoger
GermanLanguage=Tyska menyer och dialoger
HungarianLanguage=Ungerska menyer och dialoger
ItalianLanguage=Italienska menyer och dialoger
JapaneseLanguage=Japanska menyer och dialoger
KoreanLanguage=Koreanska menyer och dialoger
NorwegianLanguage=Norska menyer och dialoger
PolishLanguage=Polska menyer och dialoger
PortugueseLanguage=Portugisiska (Brasiliansk) menyer och dialoger
RussianLanguage=Ryska menyer och dialoger
SlovakLanguage=Slovakiska menyer och dialoger
SpanishLanguage=Spanska menyer och dialoger
SwedishLanguage=Svenska menyer och dialoger

;Tasks
ExplorerContextMenu=&Möjliggör integration med Explorers menyer
IntegrateTortoiseCVS=Integrera med &TortoiseCVS
IntegrateDelphi4=Stöd Borland® Delphi4 Binära filer
UpdatingCommonControls=Uppdatera Systemets Vanliga Kontroller

;Icon Labels
ReadMe=Läs Mig
UsersGuide=Användarguide
UpdatingCommonControls=Uppdatera Systemets Vanliga Kontroller
ViewStartMenuFolder=Visa WinMerges startkatalog

;Code Dialogs
DeletePreviousStartMenu=Installationsprogrammet har upptäckt att du har ändrat sökvägen till din startmeny från "%s" till "%s". Vill du ta bort den tidigare startmenymappen?
