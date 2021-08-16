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
SetupWindowTitle=Installationsprogram f�r %1
UninstallAppTitle=Avinstallation
UninstallAppFullTitle=%1 Avinstallation

; *** Misc. common
InformationTitle=Information
ConfirmTitle=Bekr�fta
ErrorTitle=Fel

; *** SetupLdr messages
SetupLdrStartupMessage=%1 kommer att installeras. Vill Ni forts�tta?
LdrCannotCreateTemp=Kan ej skapa en tillf�llig fil. Installationen avbryts
LdrCannotExecTemp=Kan ej k�ra fil i tillf�llig katalog. Installationen avbryts

; *** Startup error messages
LastErrorMessage=%1.%n%nFel %2: %3
SetupFileMissing=Filen %1 saknas i installationskatalogen. R�tta till problemet eller h�mta en ny kopia av programmet.
SetupFileCorrupt=Installationsfilerna �r felaktiga. H�mta en ny kopia av programmet
SetupFileCorruptOrWrongVer=Installationsfilerna �r felaktiga, eller st�mmer ej �verens med denna version av installationsprogrammet. R�tta till felet eller h�mta en ny programkopia.
InvalidParameter=En ogiltig parameter angavs p� kommandoraden:%n%n%1
SetupAlreadyRunning=Setup k�rs redan.
WindowsVersionNotSupported=Programmet st�djer inte den version av Windows som k�rs p� datorn.
WindowsServicePackRequired=Programmet kr�ver %1 Service Pack %2 eller nyare.
NotOnThisPlatform=Detta program kan ej k�ras p� %1.
OnlyOnThisPlatform=Detta program m�ste ha %1.
OnlyOnTheseArchitectures=Detta program kan bara installeras p� Windows versioner med f�ljande processorarkitekturer:%n%n%1
MissingWOW64APIs=Den versionen av Windows Ni k�r har inte den funktionalitet installationsprogrammet beh�ver f�r att genomf�ra en 64-bitars installation. R�tta till problemet genom att installera Service Pack %1.
WinVersionTooLowError=Detta program kr�ver %1, version %2 eller senare.
WinVersionTooHighError=Programmet kan inte installeras p� %1 version %2 eller senare.
AdminPrivilegesRequired=Ni m�ste vara inloggad som Administrat�r n�r Ni installerar detta program.
PowerUserPrivilegesRequired=Ni m�ste vara inloggad som Administrat�r eller medlem av gruppen Privilegierade anv�ndare (Power Users) n�r Ni installerar detta program.
SetupAppRunningError=Installationsprogrammet har uppt�ckt att %1 �r ig�ng.%n%nAvsluta det angivna programmet nu. Klicka sedan p� OK f�r att g� vidare, eller p� Avbryt f�r att avsluta.
UninstallAppRunningError=Avinstalleraren har uppt�ckt att %1 k�rs f�r tillf�llet.%n%nSt�ng alla �ppna instanser av det nu, klicka sedan p� OK f�r att g� vidare, eller p� Avbryt f�r att avsluta.

; *** Misc. errors
ErrorCreatingDir=Kunde inte skapa katalogen "%1"
ErrorTooManyFilesInDir=Kunde inte skapa en fil i katalogen "%1" d�rf�r att den inneh�ller f�r m�nga filer

; *** Setup common messages
ExitSetupTitle=Avsluta installationen
ExitSetupMessage=Installationen �r inte f�rdig. Om Ni avslutar nu, kommer programmet inte att installeras.%n%nNi kan k�ra installationsprogrammet vid ett senare tillf�lle f�r att slutf�ra installationen.%n%nVill Ni avbryta installationen?
AboutSetupMenuItem=&Om installationsprogrammet...
AboutSetupTitle=Om installationsprogrammet
AboutSetupMessage=%1 version %2%n%3%n%n%1 hemsida:%n%4
AboutSetupNote=Svensk �vers�ttning �r gjord av dickg@go.to 1999, 2002%n%nUppdatering till 3.0.2+ av peter@peterandlinda.com, 4.+ av stefan@bodingh.se, �ndrad av pgert@yahoo.se
TranslatorNote=

; *** Buttons
ButtonBack=< &Tillbaka
ButtonNext=&N�sta >
ButtonInstall=&Installera
ButtonOK=OK
ButtonCancel=Avbryt
ButtonYes=&Ja
ButtonYesToAll=Ja till &Allt
ButtonNo=&Nej
ButtonNoToAll=N&ej till allt
ButtonFinish=&Slutf�r
ButtonBrowse=&Bl�ddra...
ButtonWizardBrowse=&Bl�ddra...
ButtonNewFolder=Skapa ny katalog

; *** "Select Language" dialog messages
SelectLanguageTitle=V�lj spr�k f�r installationen
SelectLanguageLabel=V�lj det spr�k som skall anv�ndas under installationen:

; *** Common wizard text
ClickNext=Klicka p� N�sta f�r att forts�tta eller p� Avbryt f�r att avsluta installationen.
BeveledLabel=
BrowseDialogTitle=V�lj katalog
BrowseDialogLabel=V�lj en katalog i listan nedan, klicka sedan p� OK.
NewFolderName=Ny katalog

; *** "Welcome" wizard page
WelcomeLabel1=V�lkommen till installationsprogrammet f�r [name].
WelcomeLabel2=Detta kommer att installera [name/ver] p� Er dator.%n%nDet anr�des att Ni avslutar alla andra program innan Ni forts�tter. Det f�rebygger konflikter under installationens g�ng.

; *** "Password" wizard page
WizardPassword=L�senord
PasswordLabel1=Denna installation �r skyddad med l�senord.
PasswordLabel3=Var god ange l�senordet, klicka sedan p� N�sta f�r att forts�tta. L�senord skiljer p� versaler/gemener.
PasswordEditLabel=&L�senord:
IncorrectPassword=L�senordet Ni angav �r inkorrekt. F�rs�k igen.

; *** "License Agreement" wizard page
WizardLicense=Licensavtal
LicenseLabel=Var god och l�s f�ljande viktiga information innan Ni forts�tter.
LicenseLabel3=Var god och l�s f�ljande licensavtal. Ni m�ste acceptera villkoren i avtalet innan Ni kan forts�tta med installationen.
LicenseAccepted=Jag &accepterar avtalet
LicenseNotAccepted=Jag accepterar &inte avtalet

; *** "Information" wizard pages
WizardInfoBefore=Information
InfoBeforeLabel=Var god l�s f�ljande viktiga information innan Ni forts�tter.
InfoBeforeClickLabel=N�r Ni �r klar att forts�tta med installationen klickar Ni p� N�sta.
WizardInfoAfter=Information
InfoAfterLabel=Var god l�s f�ljande viktiga information innan Ni forts�tter.
InfoAfterClickLabel=N�r Ni �r klar att forts�tta med installationen klickar Ni p� N�sta.

; *** "User Information" wizard page
WizardUserInfo=Anv�ndarinformation
UserInfoDesc=Var god och fyll i f�ljande uppgifter.
UserInfoName=&Namn:
UserInfoOrg=&Organisation:
UserInfoSerial=&Serienummer:
UserInfoNameRequired=Ni m�ste fylla i ett namn.

; *** "Select Destination Directory" wizard page
WizardSelectDir=V�lj installationsplats
SelectDirDesc=Var skall [name] installeras?
SelectDirLabel3=Installationsprogrammet kommer att installera [name] i f�ljande katalog
SelectDirBrowseLabel=F�r att forts�tta klickar Ni p� N�sta. Om Ni vill v�lja en annan katalog klickar Ni p� Bl�ddra.
DiskSpaceMBLabel=Programmet kr�ver minst [mb] MB h�rddiskutrymme.
CannotInstallToNetworkDrive=Setup kan inte installeras p� n�tverksdisk.
CannotInstallToUNCPath=Setup kan inte installeras p� UNC s�kv�g.
InvalidPath=Ni m�ste skriva en fullst�ndig s�kv�g med enhetsbeteckning; till exempel:%n%nC:\Program%n%neller en UNC-s�kv�g i formatet:%n%n\\server\resurs
InvalidDrive=Enheten Ni har valt finns inte eller �r inte tillg�nglig. V�lj en annan.
DiskSpaceWarningTitle=Ej tillr�ckligt med diskutrymme
DiskSpaceWarning=Installationsprogrammet beh�ver �tminstone %1 KB ledigt diskutrymme f�r installationen, men den valda enheten har bara %2  KB tillg�ngligt.%n%nVill Ni forts�tta �nd�?
DirNameTooLong=Katalogens namn\s�kv�g �r f�r l�ngt.
InvalidDirName=Katalogen Ni har valt �r inte tillg�nglig.
BadDirName32=Katalogens namn f�r ej inneh�lla n�got av f�ljande tecken:%n%n%1
DirExistsTitle=Katalogen finns
DirExists=Katalogen:%n%n%1%n%nfinns redan. Vill Ni �nd� forts�tta installationen till den valda katalogen?
DirDoesntExistTitle=Katalogen finns inte
DirDoesntExist=Katalogen:%n%n%1%n%nfinns inte. Vill Ni skapa den?

; *** "Select Components" wizard page
WizardSelectComponents=V�lj komponenter
SelectComponentsDesc=Vilka komponenter skall installeras?
SelectComponentsLabel2=V�lj de komponenter som Ni vill ska installeras; avmarkera de komponenter som Ni inte vill ha. Klicka sedan p� N�sta n�r Ni �r klar att forts�tta.
FullInstallation=Fullst�ndig installation
; if possible don't translate 'Compact' as 'Minimal' (I mean 'Minimal' in your language)
CompactInstallation=Kompakt installation
CustomInstallation=Anpassad installation
NoUninstallWarningTitle=Komponenter finns
NoUninstallWarning=Installationsprogrammet har uppt�ckt att f�ljande komponenter redan finns installerade p� Er dator:%n%n%1%n%nAtt avmarkera dessa komponenter kommer inte att avinstallera dom.%n%nVill Ni forts�tta �nd�?
ComponentSize1=%1 KB
ComponentSize2=%1 MB
ComponentsDiskSpaceMBLabel=Aktuella val kr�ver minst [mb] MB diskutrymme.

; *** "Select Additional Tasks" wizard page
WizardSelectTasks=V�lj extra uppgifter
SelectTasksDesc=Vilka extra uppgifter skall utf�ras?
SelectTasksLabel2=Markera ytterligare uppgifter att utf�ra vid installation av [name], tryck sedan p� N�sta.

; *** "Select Start Menu Folder" wizard page
WizardSelectProgramGroup=V�lj Startmenykatalogen
SelectStartMenuFolderDesc=Var skall installationsprogrammet placera programmets genv�gar?
SelectStartMenuFolderLabel3=Installationsprogrammet kommer att skapa programmets genv�gar i f�ljande katalog.
SelectStartMenuFolderBrowseLabel=F�r att forts�tta klickar Ni p� N�sta. Om Ni vill v�lja en annan katalog, klickar Ni p� Bl�ddra.
MustEnterGroupName=Ni m�ste ange en katalog.
GroupNameTooLong=Katalogens namn\s�kv�g �r f�r l�ngt.
InvalidGroupName=Katalogen Ni har valt �r inte tillg�nglig.
BadGroupName=Katalognamnet kan inte inneh�lla n�gon av f�ljande tecken:%n%n%1
NoProgramGroupCheck2=&Skapa ingen Startmenykatalog

; *** "Ready to Install" wizard page
WizardReady=Redo att installera
ReadyLabel1=Installationsprogrammet �r nu redo att installera [name] p� Er dator.
ReadyLabel2a=Tryck p� Installera om Ni vill forts�tta, eller p� g� Tillbaka om Ni vill granska eller �ndra p� n�got.
ReadyLabel2b=V�lj Installera f�r att p�b�rja installationen.
ReadyMemoUserInfo=Anv�ndarinformation:
ReadyMemoDir=Installationsplats:
ReadyMemoType=Installationstyp:
ReadyMemoComponents=Valda komponenter:
ReadyMemoGroup=Startmenykatalog:
ReadyMemoTasks=Extra uppgifter:

; *** "Preparing to Install" wizard page
WizardPreparing=F�rbereder installationen
PreparingDesc=Installationsprogrammet f�rbereder installationen av [name] p� Er dator.
PreviousInstallNotCompleted=Installation/Avinstallation av ett tidigare program har inte slutf�rts. Ni m�ste starta om datorn f�r att avsluta den installationen.%n%nEfter att ha startat om datorn k�r Ni installationsprogrammet igen f�r att slutf�ra installationen av [name].
CannotContinue=Installationsprogrammet kan inte forts�tta. Klicka p� Avbryt f�r att avsluta.
ApplicationsFound=F�ljande program anv�nder filer som m�ste uppdateras av Setup. Vi anr�der att Ni l�ter Setup automatiskt st�nga ned dessa program.
ApplicationsFound2=F�ljande program anv�nder filer som m�ste uppdateras av Setup. Vi anr�der att Ni l�ter Setup automatiskt st�nga ned dessa program. Efter installationen kommer Setup att f�rs�ka starta programmen igen.
CloseApplications=&St�ng programmen automatiskt
DontCloseApplications=&St�ng inte programmen
ErrorCloseApplications=Installationsprogrammet kunde inte st�nga alla program. Innan installationen forts�tter anr�der Vi att Ni st�nger alla program som anv�nder filer som Setup beh�ver uppdatera. 

; *** "Installing" wizard page
WizardInstalling=Installerar
InstallingLabel=V�nta medans [name] installeras p� Er dator.

; *** "Setup Completed" wizard page
FinishedHeadingLabel=Avslutar installationen av [name]
FinishedLabelNoIcons=[name] har nu installerats p� Er dator.
FinishedLabel=[name] har nu installerats p� Er dator. Programmet kan startas genom att v�lja n�gon av ikonerna.
ClickFinish=V�lj Slutf�r f�r att avsluta installationen.
FinishedRestartLabel=F�r att slutf�ra installationen av [name], m�ste datorn startas om. Vill Ni starta om nu?
FinishedRestartMessage=F�r att slutf�ra installationen av [name], m�ste datorn startas om.%n%nVill Ni starta om datorn nu?
ShowReadmeCheck=Ja, Jag vill se filen L�S MIG
YesRadio=&Ja, Jag vill starta om datorn nu
NoRadio=&Nej, Jag startar sj�lv om datorn senare
; used for example as 'Run MyProg.exe'
RunEntryExec=K�r %1
; used for example as 'View Readme.txt'
RunEntryShellExec=L�s %1

; *** "Setup Needs the Next Disk" stuff
ChangeDiskTitle=Installationsprogrammet beh�ver n�sta diskett
SelectDiskLabel2=Var god s�tt i diskett %1 och tryck OK.%n%nOm filerna kan hittas i en annan katalog �n den som visas nedan, skriv in r�tt s�kv�g eller v�lj Bl�ddra.
PathLabel=&S�kv�g:
FileNotInDir2=Kunde inte hitta filen "%1" i "%2". Var god s�tt i korrekt diskett eller v�lj en annan katalog.
SelectDirectoryLabel=Var god ange s�kv�gen f�r n�sta diskett.

; *** Installation phase messages
SetupAborted=Installationen slutf�rdes inte.%n%nVar god r�tta till felet och k�r installationen igen.
EntryAbortRetryIgnore=V�lj F�rs�k igen eller Fr�nse f�r att forts�tta �nd�, eller v�lj Avbryt f�r att avbryta installationen.

; *** Installation status messages
StatusClosingApplications=St�nger program...
StatusCreateDirs=Skapar kataloger...
StatusExtractFiles=Packar upp filer...
StatusCreateIcons=Skapar programikoner...
StatusCreateIniEntries=Skriver INI-v�rden...
StatusCreateRegistryEntries=Skriver register-v�rden...
StatusRegisterFiles=Registrerar filer...
StatusSavingUninstall=Sparar information f�r avinstallation...
StatusRunProgram=Slutf�r installationen...
StatusRestartingApplications=Startar om program...
StatusRollback=�terst�ller �ndringar...

; *** Misc. errors
ErrorInternal2=Internt fel: %1
ErrorFunctionFailedNoCode=%1 misslyckades
ErrorFunctionFailed=%1 misslyckades; kod %2
ErrorFunctionFailedWithMessage=%1 misslyckades; kod %2.%n%3
ErrorExecutingProgram=Kan inte k�ra filen:%n%1

; *** Registry errors
ErrorRegOpenKey=Fel vid �ppning av registernyckel:%n%1\%2
ErrorRegCreateKey=Kan ej skapa registernyckel:%n%1\%2
ErrorRegWriteKey=Kan ej skriva till registernyckel:%n%1\%2

; *** INI errors
ErrorIniEntry=Kan inte skriva nytt INI-v�rde i filen "%1".

; *** File copying errors
FileAbortRetryIgnore=V�lj F�rs�k igen eller Fr�nse f�r att hoppa �ver denna fil (avr�des), eller v�lj Avbryt installationen.
FileAbortRetryIgnore2=V�lj F�rs�k igen eller Fr�nse och forts�tt �nd� (avr�des), eller v�lj Avbryt installationen.
SourceIsCorrupted=K�llfilen �r felaktig
SourceDoesntExist=K�llfilen "%1" finns inte
ExistingFileReadOnly=Den nuvarande filen �r skrivskyddad.%n%nV�lj F�rs�k igen f�r att ta bort skrivskyddet, Fr�nse f�r att hoppa �ver denna fil, eller v�lj Avbryt installationen.
ErrorReadingExistingDest=Ett fel uppstod vid f�rs�k att l�sa den befintliga filen:
FileExists=Filen finns redan.%n%nVill Ni skriva �ver den?
ExistingFileNewer=Den befintliga filen �r nyare �n den som ska installeras. Ni anr�des att beh�lla den befintliga filen. %n%nVill Ni beh�lla den befintliga filen?
ErrorChangingAttr=Ett fel uppstod vid f�rs�k att �ndra attribut p� den befintliga filen:
ErrorCreatingTemp=Ett fel uppstod vid ett f�rs�k att skapa installationskatalogen:
ErrorReadingSource=Ett fel uppstod vid ett f�rs�k att l�sa k�llfilen:
ErrorCopying=Ett fel uppstod vid kopiering av filen:
ErrorReplacingExistingFile=Ett fel uppstod vid ett f�rs�k att ers�tta den befintliga filen:
ErrorRestartReplace=�terstartaErs�tt misslyckades:
ErrorRenamingTemp=Ett fel uppstod vid ett f�rs�k att byta namn p� en fil i installationskatalogen:
ErrorRegisterServer=Kunde inte registrera DLL/OCX: %1
ErrorRegSvr32Failed=RegSvr32 misslyckades med felkod %1
ErrorRegisterTypeLib=Kunde inte registrera typbibliotek: %1

; *** Post-installation errors
ErrorOpeningReadme=Ett fel uppstod vid �ppnandet av L�S MIG-filen.
ErrorRestartingComputer=Installationsprogrammet kunde inte starta om datorn. Var god g�r det manuellt.

; *** Uninstaller messages
UninstallNotFound=Filen "%1" finns inte. Kan inte avinstallera.
UninstallOpenError=Filen "%1" kan inte �ppnas. Kan inte avinstallera.
UninstallUnsupportedVer=Avinstallationsloggen "%1" �r i ett format som denna version inte k�nner igen. Kan ej avinstallera
UninstallUnknownEntry=En ok�nd rad (%1) hittades i avinstallationsloggen
ConfirmUninstall=�r Ni s�ker p� att Ni vill ta bort %1 och alla tillh�rande komponenter?
UninstallOnlyOnWin64=Denna installation kan endast avinstalleras p� en 64-bitarsversion av Windows.
OnlyAdminCanUninstall=Denna installation kan endast avinstalleras av en anv�ndare med Administrat�rs-beh�righeter.
UninstallStatusLabel=Var god och v�nta medan %1 tas bort fr�n Er dator.
UninstalledAll=%1 �r nu borttaget fr�n Er dator.
UninstalledMost=Avinstallationen av %1 �r nu klar.%n%nEn del filer/kataloger gick ej att ta bort. Dessa kan tas bort egenh�ndigt.
UninstalledAndNeedsRestart=F�r att slutf�ra avinstallationen av %1 m�ste datorn startas om.%n%nVill Ni starta om nu?
UninstallDataCorrupted=Filen "%1" �r felaktig. Kan inte avinstallera

; *** Uninstallation phase messages
ConfirmDeleteSharedFileTitle=Ta bort delad fil?
ConfirmDeleteSharedFile2=Systemet anger att f�ljande delade fil inte l�ngre anv�nds av n�gra program. Vill Ni ta bort den delade filen?%n%n%1%n%nOm n�got program fortfarande anv�nder denna fil och den raderas, kommer programmet kanske att sluta fungera. Om Ni �r os�ker, v�lj Nej. Att l�ta filen ligga kvar i systemet kommer inte att orsaka n�gon skada.
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
AdditionalIcons=�terst�ende ikoner:
CreateDesktopIcon=Skapa en ikon p� skrivbordet
CreateQuickLaunchIcon=Skapa en ikon i Snabbstartf�ltet
ProgramOnTheWeb=%1 p� Webben
UninstallProgram=Avinstallera %1
LaunchProgram=Starta %1
AssocFileExtension=Associera %1 med %2 filnamnstill�gg
AssocingFileExtension=Associerar %1 med %2 filnamnstill�gg...
AutoStartProgramGroupDescription=Autostart:
AutoStartProgram=Starta automatiskt %1
AddonHostProgramNotFound=%1 kunde inte hittas i katalogen Ni valde.%n%nVill Ni forts�tta �nd�?
