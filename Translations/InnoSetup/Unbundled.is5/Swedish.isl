; *** Inno Setup version 5.5.3+ Swedish messages ***
;
; To download user-contributed translations of this file, go to:
;   http://www.jrsoftware.org/files/istrans/
;
; Note: When translating this text, do not add periods (.) to the end of
; messages that didn't have them already, because on those messages Inno
; Setup adds the periods automatically (appending a period would result in
; two periods being displayed).
;
; Translated by christer_1@hotmail.com (Christer Toivonen)
;

[LangOptions]
; The following three entries are very important. Be sure to read and 
; understand the '[LangOptions] section' topic in the help file.
LanguageName=Svenska	
LanguageID=$041D
LanguageCodePage=1252
; If the language you are translating to requires special font faces or
; sizes, uncomment any of the following entries and change them accordingly.
;DialogFontName=
;DialogFontSize=8
;WelcomeFontName=Verdana
;WelcomeFontSize=12
;TitleFontName=Arial
;TitleFontSize=29
;CopyrightFontName=Arial
;CopyrightFontSize=8

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
SetupLdrStartupMessage=%1 kommer att installeras. Vill Ni fortsätta?
LdrCannotCreateTemp=Kan ej skapa en tillfällig fil. Installationen avbryts
LdrCannotExecTemp=Kan ej köra fil i tillfällig katalog. Installationen avbryts

; *** Startup error messages
LastErrorMessage=%1.%n%nFel %2: %3
SetupFileMissing=Filen %1 saknas i installationskatalogen. Rätta till problemet eller hämta en ny kopia av programmet.
SetupFileCorrupt=Installationsfilerna är felaktiga. Hämta en ny kopia av programmet
SetupFileCorruptOrWrongVer=Installationsfilerna är felaktiga, eller stämmer ej överens med denna version av installationsprogrammet. Rätta till felet eller hämta en ny programkopia.
InvalidParameter=En ogiltig parameter angavs på kommandoraden:%n%n%1
SetupAlreadyRunning=Setup körs redan.
WindowsVersionNotSupported=Programmet stödjer inte den version av Windows som körs på datorn.
WindowsServicePackRequired=Programmet kräver %1 Service Pack %2 eller nyare.
NotOnThisPlatform=Detta program kan ej köras på %1.
OnlyOnThisPlatform=Detta program måste ha %1.
OnlyOnTheseArchitectures=Detta program kan bara installeras på Windows versioner med följande processorarkitekturer:%n%n%1
MissingWOW64APIs=Den versionen av Windows Ni kör har inte den funktionalitet installationsprogrammet behöver för att genomföra en 64-bitars installation. Rätta till problemet genom att installera Service Pack %1.
WinVersionTooLowError=Detta program kräver %1, version %2 eller senare.
WinVersionTooHighError=Programmet kan inte installeras på %1 version %2 eller senare.
AdminPrivilegesRequired=Ni måste vara inloggad som Administratör när Ni installerar detta program.
PowerUserPrivilegesRequired=Ni måste vara inloggad som Administratör eller medlem av gruppen Privilegierade användare (Power Users) när Ni installerar detta program.
SetupAppRunningError=Installationsprogrammet har upptäckt att %1 är igång.%n%nAvsluta det angivna programmet nu. Klicka sedan på OK för att gå vidare, eller på Avbryt för att avsluta.
UninstallAppRunningError=Avinstalleraren har upptäckt att %1 körs för tillfället.%n%nStäng alla öppna instanser av det nu, klicka sedan på OK för att gå vidare, eller på Avbryt för att avsluta.

; *** Misc. errors
ErrorCreatingDir=Kunde inte skapa katalogen "%1"
ErrorTooManyFilesInDir=Kunde inte skapa en fil i katalogen "%1" därför att den innehåller för många filer

; *** Setup common messages
ExitSetupTitle=Avsluta installationen
ExitSetupMessage=Installationen är inte färdig. Om Ni avslutar nu, kommer programmet inte att installeras.%n%nNi kan köra installationsprogrammet vid ett senare tillfälle för att slutföra installationen.%n%nVill Ni avbryta installationen?
AboutSetupMenuItem=&Om installationsprogrammet...
AboutSetupTitle=Om installationsprogrammet
AboutSetupMessage=%1 version %2%n%3%n%n%1 hemsida:%n%4
AboutSetupNote=Svensk översättning är gjord av dickg@go.to 1999, 2002%n%nUppdatering till 3.0.2+ av peter@peterandlinda.com, 4.+ av stefan@bodingh.se, ändrad av pgert@yahoo.se
TranslatorNote=

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
ButtonNewFolder=Skapa ny katalog

; *** "Select Language" dialog messages
SelectLanguageTitle=Välj språk för installationen
SelectLanguageLabel=Välj det språk som skall användas under installationen:

; *** Common wizard text
ClickNext=Klicka på Nästa för att fortsätta eller på Avbryt för att avsluta installationen.
BeveledLabel=
BrowseDialogTitle=Välj katalog
BrowseDialogLabel=Välj en katalog i listan nedan, klicka sedan på OK.
NewFolderName=Ny katalog

; *** "Welcome" wizard page
WelcomeLabel1=Välkommen till installationsprogrammet för [name].
WelcomeLabel2=Detta kommer att installera [name/ver] på Er dator.%n%nDet anrådes att Ni avslutar alla andra program innan Ni fortsätter. Det förebygger konflikter under installationens gång.

; *** "Password" wizard page
WizardPassword=Lösenord
PasswordLabel1=Denna installation är skyddad med lösenord.
PasswordLabel3=Var god ange lösenordet, klicka sedan på Nästa för att fortsätta. Lösenord skiljer på versaler/gemener.
PasswordEditLabel=&Lösenord:
IncorrectPassword=Lösenordet Ni angav är inkorrekt. Försök igen.

; *** "License Agreement" wizard page
WizardLicense=Licensavtal
LicenseLabel=Var god och läs följande viktiga information innan Ni fortsätter.
LicenseLabel3=Var god och läs följande licensavtal. Ni måste acceptera villkoren i avtalet innan Ni kan fortsätta med installationen.
LicenseAccepted=Jag &accepterar avtalet
LicenseNotAccepted=Jag accepterar &inte avtalet

; *** "Information" wizard pages
WizardInfoBefore=Information
InfoBeforeLabel=Var god läs följande viktiga information innan Ni fortsätter.
InfoBeforeClickLabel=När Ni är klar att fortsätta med installationen klickar Ni på Nästa.
WizardInfoAfter=Information
InfoAfterLabel=Var god läs följande viktiga information innan Ni fortsätter.
InfoAfterClickLabel=När Ni är klar att fortsätta med installationen klickar Ni på Nästa.

; *** "User Information" wizard page
WizardUserInfo=Användarinformation
UserInfoDesc=Var god och fyll i följande uppgifter.
UserInfoName=&Namn:
UserInfoOrg=&Organisation:
UserInfoSerial=&Serienummer:
UserInfoNameRequired=Ni måste fylla i ett namn.

; *** "Select Destination Directory" wizard page
WizardSelectDir=Välj installationsplats
SelectDirDesc=Var skall [name] installeras?
SelectDirLabel3=Installationsprogrammet kommer att installera [name] i följande katalog
SelectDirBrowseLabel=För att fortsätta klickar Ni på Nästa. Om Ni vill välja en annan katalog klickar Ni på Bläddra.
DiskSpaceMBLabel=Programmet kräver minst [mb] MB hårddiskutrymme.
CannotInstallToNetworkDrive=Setup kan inte installeras på nätverksdisk.
CannotInstallToUNCPath=Setup kan inte installeras på UNC sökväg.
InvalidPath=Ni måste skriva en fullständig sökväg med enhetsbeteckning; till exempel:%n%nC:\Program%n%neller en UNC-sökväg i formatet:%n%n\\server\resurs
InvalidDrive=Enheten Ni har valt finns inte eller är inte tillgänglig. Välj en annan.
DiskSpaceWarningTitle=Ej tillräckligt med diskutrymme
DiskSpaceWarning=Installationsprogrammet behöver åtminstone %1 KB ledigt diskutrymme för installationen, men den valda enheten har bara %2  KB tillgängligt.%n%nVill Ni fortsätta ändå?
DirNameTooLong=Katalogens namn\sökväg är för långt.
InvalidDirName=Katalogen Ni har valt är inte tillgänglig.
BadDirName32=Katalogens namn får ej innehålla något av följande tecken:%n%n%1
DirExistsTitle=Katalogen finns
DirExists=Katalogen:%n%n%1%n%nfinns redan. Vill Ni ändå fortsätta installationen till den valda katalogen?
DirDoesntExistTitle=Katalogen finns inte
DirDoesntExist=Katalogen:%n%n%1%n%nfinns inte. Vill Ni skapa den?

; *** "Select Components" wizard page
WizardSelectComponents=Välj komponenter
SelectComponentsDesc=Vilka komponenter skall installeras?
SelectComponentsLabel2=Välj de komponenter som Ni vill ska installeras; avmarkera de komponenter som Ni inte vill ha. Klicka sedan på Nästa när Ni är klar att fortsätta.
FullInstallation=Fullständig installation
; if possible don't translate 'Compact' as 'Minimal' (I mean 'Minimal' in your language)
CompactInstallation=Kompakt installation
CustomInstallation=Anpassad installation
NoUninstallWarningTitle=Komponenter finns
NoUninstallWarning=Installationsprogrammet har upptäckt att följande komponenter redan finns installerade på Er dator:%n%n%1%n%nAtt avmarkera dessa komponenter kommer inte att avinstallera dom.%n%nVill Ni fortsätta ändå?
ComponentSize1=%1 KB
ComponentSize2=%1 MB
ComponentsDiskSpaceMBLabel=Aktuella val kräver minst [mb] MB diskutrymme.

; *** "Select Additional Tasks" wizard page
WizardSelectTasks=Välj extra uppgifter
SelectTasksDesc=Vilka extra uppgifter skall utföras?
SelectTasksLabel2=Markera ytterligare uppgifter att utföra vid installation av [name], tryck sedan på Nästa.

; *** "Select Start Menu Folder" wizard page
WizardSelectProgramGroup=Välj Startmenykatalogen
SelectStartMenuFolderDesc=Var skall installationsprogrammet placera programmets genvägar?
SelectStartMenuFolderLabel3=Installationsprogrammet kommer att skapa programmets genvägar i följande katalog.
SelectStartMenuFolderBrowseLabel=För att fortsätta klickar Ni på Nästa. Om Ni vill välja en annan katalog, klickar Ni på Bläddra.
MustEnterGroupName=Ni måste ange en katalog.
GroupNameTooLong=Katalogens namn\sökväg är för långt.
InvalidGroupName=Katalogen Ni har valt är inte tillgänglig.
BadGroupName=Katalognamnet kan inte innehålla någon av följande tecken:%n%n%1
NoProgramGroupCheck2=&Skapa ingen Startmenykatalog

; *** "Ready to Install" wizard page
WizardReady=Redo att installera
ReadyLabel1=Installationsprogrammet är nu redo att installera [name] på Er dator.
ReadyLabel2a=Tryck på Installera om Ni vill fortsätta, eller på gå Tillbaka om Ni vill granska eller ändra på något.
ReadyLabel2b=Välj Installera för att påbörja installationen.
ReadyMemoUserInfo=Användarinformation:
ReadyMemoDir=Installationsplats:
ReadyMemoType=Installationstyp:
ReadyMemoComponents=Valda komponenter:
ReadyMemoGroup=Startmenykatalog:
ReadyMemoTasks=Extra uppgifter:

; *** "Preparing to Install" wizard page
WizardPreparing=Förbereder installationen
PreparingDesc=Installationsprogrammet förbereder installationen av [name] på Er dator.
PreviousInstallNotCompleted=Installation/Avinstallation av ett tidigare program har inte slutförts. Ni måste starta om datorn för att avsluta den installationen.%n%nEfter att ha startat om datorn kör Ni installationsprogrammet igen för att slutföra installationen av [name].
CannotContinue=Installationsprogrammet kan inte fortsätta. Klicka på Avbryt för att avsluta.
ApplicationsFound=Följande program använder filer som måste uppdateras av Setup. Vi anråder att Ni låter Setup automatiskt stänga ned dessa program.
ApplicationsFound2=Följande program använder filer som måste uppdateras av Setup. Vi anråder att Ni låter Setup automatiskt stänga ned dessa program. Efter installationen kommer Setup att försöka starta programmen igen.
CloseApplications=&Stäng programmen automatiskt
DontCloseApplications=&Stäng inte programmen
ErrorCloseApplications=Installationsprogrammet kunde inte stänga alla program. Innan installationen fortsätter anråder Vi att Ni stänger alla program som använder filer som Setup behöver uppdatera. 

; *** "Installing" wizard page
WizardInstalling=Installerar
InstallingLabel=Vänta medans [name] installeras på Er dator.

; *** "Setup Completed" wizard page
FinishedHeadingLabel=Avslutar installationen av [name]
FinishedLabelNoIcons=[name] har nu installerats på Er dator.
FinishedLabel=[name] har nu installerats på Er dator. Programmet kan startas genom att välja någon av ikonerna.
ClickFinish=Välj Slutför för att avsluta installationen.
FinishedRestartLabel=För att slutföra installationen av [name], måste datorn startas om. Vill Ni starta om nu?
FinishedRestartMessage=För att slutföra installationen av [name], måste datorn startas om.%n%nVill Ni starta om datorn nu?
ShowReadmeCheck=Ja, Jag vill se filen LÄS MIG
YesRadio=&Ja, Jag vill starta om datorn nu
NoRadio=&Nej, Jag startar själv om datorn senare
; used for example as 'Run MyProg.exe'
RunEntryExec=Kör %1
; used for example as 'View Readme.txt'
RunEntryShellExec=Läs %1

; *** "Setup Needs the Next Disk" stuff
ChangeDiskTitle=Installationsprogrammet behöver nästa diskett
SelectDiskLabel2=Var god sätt i diskett %1 och tryck OK.%n%nOm filerna kan hittas i en annan katalog än den som visas nedan, skriv in rätt sökväg eller välj Bläddra.
PathLabel=&Sökväg:
FileNotInDir2=Kunde inte hitta filen "%1" i "%2". Var god sätt i korrekt diskett eller välj en annan katalog.
SelectDirectoryLabel=Var god ange sökvägen för nästa diskett.

; *** Installation phase messages
SetupAborted=Installationen slutfördes inte.%n%nVar god rätta till felet och kör installationen igen.
EntryAbortRetryIgnore=Välj Försök igen eller Frånse för att fortsätta ändå, eller välj Avbryt för att avbryta installationen.

; *** Installation status messages
StatusClosingApplications=Stänger program...
StatusCreateDirs=Skapar kataloger...
StatusExtractFiles=Packar upp filer...
StatusCreateIcons=Skapar programikoner...
StatusCreateIniEntries=Skriver INI-värden...
StatusCreateRegistryEntries=Skriver register-värden...
StatusRegisterFiles=Registrerar filer...
StatusSavingUninstall=Sparar information för avinstallation...
StatusRunProgram=Slutför installationen...
StatusRestartingApplications=Startar om program...
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
FileAbortRetryIgnore=Välj Försök igen eller Frånse för att hoppa över denna fil (avrådes), eller välj Avbryt installationen.
FileAbortRetryIgnore2=Välj Försök igen eller Frånse och fortsätt ändå (avrådes), eller välj Avbryt installationen.
SourceIsCorrupted=Källfilen är felaktig
SourceDoesntExist=Källfilen "%1" finns inte
ExistingFileReadOnly=Den nuvarande filen är skrivskyddad.%n%nVälj Försök igen för att ta bort skrivskyddet, Frånse för att hoppa över denna fil, eller välj Avbryt installationen.
ErrorReadingExistingDest=Ett fel uppstod vid försök att läsa den befintliga filen:
FileExists=Filen finns redan.%n%nVill Ni skriva över den?
ExistingFileNewer=Den befintliga filen är nyare än den som ska installeras. Ni anrådes att behålla den befintliga filen. %n%nVill Ni behålla den befintliga filen?
ErrorChangingAttr=Ett fel uppstod vid försök att ändra attribut på den befintliga filen:
ErrorCreatingTemp=Ett fel uppstod vid ett försök att skapa installationskatalogen:
ErrorReadingSource=Ett fel uppstod vid ett försök att läsa källfilen:
ErrorCopying=Ett fel uppstod vid kopiering av filen:
ErrorReplacingExistingFile=Ett fel uppstod vid ett försök att ersätta den befintliga filen:
ErrorRestartReplace=ÅterstartaErsätt misslyckades:
ErrorRenamingTemp=Ett fel uppstod vid ett försök att byta namn på en fil i installationskatalogen:
ErrorRegisterServer=Kunde inte registrera DLL/OCX: %1
ErrorRegSvr32Failed=RegSvr32 misslyckades med felkod %1
ErrorRegisterTypeLib=Kunde inte registrera typbibliotek: %1

; *** Post-installation errors
ErrorOpeningReadme=Ett fel uppstod vid öppnandet av LÄS MIG-filen.
ErrorRestartingComputer=Installationsprogrammet kunde inte starta om datorn. Var god gör det manuellt.

; *** Uninstaller messages
UninstallNotFound=Filen "%1" finns inte. Kan inte avinstallera.
UninstallOpenError=Filen "%1" kan inte öppnas. Kan inte avinstallera.
UninstallUnsupportedVer=Avinstallationsloggen "%1" är i ett format som denna version inte känner igen. Kan ej avinstallera
UninstallUnknownEntry=En okänd rad (%1) hittades i avinstallationsloggen
ConfirmUninstall=Är Ni säker på att Ni vill ta bort %1 och alla tillhörande komponenter?
UninstallOnlyOnWin64=Denna installation kan endast avinstalleras på en 64-bitarsversion av Windows.
OnlyAdminCanUninstall=Denna installation kan endast avinstalleras av en användare med Administratörs-behörigheter.
UninstallStatusLabel=Var god och vänta medan %1 tas bort från Er dator.
UninstalledAll=%1 är nu borttaget från Er dator.
UninstalledMost=Avinstallationen av %1 är nu klar.%n%nEn del filer/kataloger gick ej att ta bort. Dessa kan tas bort egenhändigt.
UninstalledAndNeedsRestart=För att slutföra avinstallationen av %1 måste datorn startas om.%n%nVill Ni starta om nu?
UninstallDataCorrupted=Filen "%1" är felaktig. Kan inte avinstallera

; *** Uninstallation phase messages
ConfirmDeleteSharedFileTitle=Ta bort delad fil?
ConfirmDeleteSharedFile2=Systemet anger att följande delade fil inte längre används av några program. Vill Ni ta bort den delade filen?%n%n%1%n%nOm något program fortfarande använder denna fil och den raderas, kommer programmet kanske att sluta fungera. Om Ni är osäker, välj Nej. Att låta filen ligga kvar i systemet kommer inte att orsaka någon skada.
SharedFileNameLabel=Filnamn:
SharedFileLocationLabel=Plats:
WizardUninstalling=Avinstallationsstatus
StatusUninstalling=Avinstallerar %1...

; *** Shutdown block reasons
ShutdownBlockReasonInstallingApp=Installerar %1.
ShutdownBlockReasonUninstallingApp=Avinstallerar %1.

; The custom messages below aren't used by Setup itself, but if you make
; use of them in your scripts, you'll want to translate them.

[CustomMessages]

NameAndVersion=%1 version %2
AdditionalIcons=Återstående ikoner:
CreateDesktopIcon=Skapa en ikon på skrivbordet
CreateQuickLaunchIcon=Skapa en ikon i Snabbstartfältet
ProgramOnTheWeb=%1 på Webben
UninstallProgram=Avinstallera %1
LaunchProgram=Starta %1
AssocFileExtension=Associera %1 med %2 filnamnstillägg
AssocingFileExtension=Associerar %1 med %2 filnamnstillägg...
AutoStartProgramGroupDescription=Autostart:
AutoStartProgram=Starta automatiskt %1
AddonHostProgramNotFound=%1 kunde inte hittas i katalogen Ni valde.%n%nVill Ni fortsätta ändå?
