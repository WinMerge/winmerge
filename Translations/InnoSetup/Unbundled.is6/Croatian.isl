; *** Inno Setup version 6.0.0+ Croatian messages ***
; Translated by: Milo Ivir (mail@milotype.de)
; Based on translation by Elvis Gambiraža (el.gambo@gmail.com)
; Based on translation by Krunoslav Kanjuh (krunoslav.kanjuh@zg.t-com.hr)
;
; To download user-contributed translations of this file, go to:
;   http://www.jrsoftware.org/files/istrans/
;
; Note: When translating this text, do not add periods (.) to the end of
; messages that didn't have them already, because on those messages Inno
; Setup adds the periods automatically (appending a period would result in
; two periods being displayed).

[LangOptions]
; The following three entries are very important. Be sure to read and 
; understand the '[LangOptions] section' topic in the help file.
LanguageName=Hrvatski
LanguageID=$041a
LanguageCodePage=1250
; If the language you are translating to requires special font faces or
; sizes, uncomment any of the following entries and change them accordingly.
;DialogFontName=MS Shell Dlg
;DialogFontSize=8
;WelcomeFontName=Arial
;WelcomeFontSize=12
;TitleFontName=Arial
;TitleFontSize=29
;CopyrightFontName=Arial
;CopyrightFontSize=8

[Messages]

; *** Application titles
SetupAppTitle=Instalacija
SetupWindowTitle=Instalacija – %1
UninstallAppTitle=Deinstalacija
UninstallAppFullTitle=Deinstalacija programa %1

; *** Misc. common
InformationTitle=Informacija
ConfirmTitle=Potvrda
ErrorTitle=Greška

; *** SetupLdr messages
SetupLdrStartupMessage=Ovime ćeš instalirati %1. Želiš li nastaviti?
LdrCannotCreateTemp=Nije moguće stvoriti privremenu datoteku. Instalacija je prekinuta
LdrCannotExecTemp=Nije moguće pokrenuti datoteku u privremenoj mapi. Instalacija je prekinuta
HelpTextNote=

; *** Startup error messages
LastErrorMessage=%1.%n%nnGreška %2: %3
SetupFileMissing=Datoteka %1 se ne nalazi u mapi instalacije. Ispravi problem ili nabavi novu kopiju programa.
SetupFileCorrupt=Datoteke instalacije su oštećene. Nabavi novu kopiju programa.
SetupFileCorruptOrWrongVer=Datoteke instalacije su oštećene ili nisu kompatibilne s ovom verzijom instalacije. Ispravi problem ili nabavi novu kopiju programa.
InvalidParameter=Neispravan parametar je prenijet u naredbenom retku:%n%n%1
SetupAlreadyRunning=Instalacija je već pokrenuta.
WindowsVersionNotSupported=Program ne podržava verziju Windowsa koju koristite.
WindowsServicePackRequired=Program zahtijeva %1 servisni paket %2 ili noviji.
NotOnThisPlatform=Ovaj program neće raditi na %1.
OnlyOnThisPlatform=Ovaj program se mora pokrenuti na %1.
OnlyOnTheseArchitectures=Ovaj program može biti instaliran na verziji Windowsa dizajniranim za sljedeću procesorsku arhitekturu:%n%n%1
WinVersionTooLowError=Ovaj program zahtijeva %1 verziju %2 ili noviju.
WinVersionTooHighError=Ovaj program se ne može instalirati na %1 verziji %2 ili novijoj.
AdminPrivilegesRequired=Morate biti prijavljeni kao administrator prilikom instaliranja ovog programa.
PowerUserPrivilegesRequired=Morate biti prijavljeni kao administrator ili član grupe naprednih korisnika prilikom instaliranja ovog programa.
SetupAppRunningError=Instalacija je otkrila da je %1 pokrenut.%n%nZatvorite program i potom kliknite "Dalje" za nastavak ili "Odustani" za prekid instalacije.
UninstallAppRunningError=Deinstalacija je otkrila da je %1 pokrenut.%n%nZatvorite program i potom kliknite "Dalje" za nastavak ili "Odustani" za prekid instalacije.

; *** Startup questions
PrivilegesRequiredOverrideTitle=Odaberite način instaliranja
PrivilegesRequiredOverrideInstruction=Odaberite način instaliranja
PrivilegesRequiredOverrideText1=%1 se može instalirati za sve korisnike (zahtijeva administrativna prava) ili samo za vas.
PrivilegesRequiredOverrideText2=%1 se može instalirati samo za vas ili za sve korisnike (zahtijeva administrativna prava).
PrivilegesRequiredOverrideAllUsers=Instaliraj z&a sve korisnike
PrivilegesRequiredOverrideAllUsersRecommended=Instaliraj z&a sve korisnike (preporučeno)
PrivilegesRequiredOverrideCurrentUser=Instaliraj samo za &mene
PrivilegesRequiredOverrideCurrentUserRecommended=Instaliraj samo za &mene (preporučeno)

; *** Misc. errors
ErrorCreatingDir=Instalacija nije mogla stvoriti mapu "%1"
ErrorTooManyFilesInDir=Nemoguće stvaranje datoteke u mapi "%1", jer ona sadrži previše datoteka

; *** Setup common messages
ExitSetupTitle=Prekini instalaciju
ExitSetupMessage=Instalacija nije završena. Ako sad izađete, program neće biti instaliran.%n%nInstalaciju možete pokrenuti kasnije, ukoliko ju želite završiti.%n%nPrekinuti instalaciju?
AboutSetupMenuItem=&O instalaciji …
AboutSetupTitle=O instalaciji
AboutSetupMessage=%1 verzija %2%n%3%n%n%1 početna stranica:%n%4
AboutSetupNote=
TranslatorNote=Prevodioci:%n%nKrunoslav Kanjuh%n%nElvis Gambiraža%n%nMilo Ivir

; *** Buttons
ButtonBack=< Na&trag
ButtonNext=&Dalje >
ButtonInstall=&Instaliraj
ButtonOK=U redu
ButtonCancel=Odustani
ButtonYes=&Da
ButtonYesToAll=D&a za sve
ButtonNo=&Ne
ButtonNoToAll=N&e za sve
ButtonFinish=&Završi
ButtonBrowse=&Pretraži …
ButtonWizardBrowse=Odabe&ri …
ButtonNewFolder=&Stvori novu mapu

; *** "Select Language" dialog messages
SelectLanguageTitle=Odaberite jezik za instalaciju
SelectLanguageLabel=Odberite jezik koji želite koristiti tijekom instaliranja.

; *** Common wizard text
ClickNext=Kliknite "Dalje" za nastavak ili "Odustani" za prekid instalacije.
BeveledLabel=
BrowseDialogTitle=Odaberite mapu
BrowseDialogLabel=Odaberite mapu iz popisa te kliknite "U redu".
NewFolderName=Nova mapa

; *** "Welcome" wizard page
WelcomeLabel1=Dobro došli u instalaciju programa [name]
WelcomeLabel2=Ovaj program će instalirati [name/ver] na vaše računalo.%n%nPreporučamo da zatvorite sve programe prije nego što nastavite dalje.

; *** "Password" wizard page
WizardPassword=Lozinka
PasswordLabel1=Instalacija je zaštićena lozinkom.
PasswordLabel3=Upišite lozinku i kliknite "Dalje". Lozinke su osjetljive na mala i velika slova.
PasswordEditLabel=&Lozinka:
IncorrectPassword=Upisana je pogrešna lozinka. Pokušajte ponovo.

; *** "License Agreement" wizard page
WizardLicense=Licencni ugovor
LicenseLabel=Prije nastavka pažljivo pročitajte sljedeće važne informacije.
LicenseLabel3=Pročitajte licencni ugovor. Morate prihvatiti uvjete ugovora kako biste nastavili s instaliranjem.
LicenseAccepted=&Prihvaćam ugovor
LicenseNotAccepted=&Ne prihvaćam ugovor

; *** "Information" wizard pages
WizardInfoBefore=Informacije
InfoBeforeLabel=Pročitajte sljedeće važne informacije prije nego što nastavite dalje.
InfoBeforeClickLabel=Kada ste spremni nastaviti s instaliranjem, kliknite "Dalje".
WizardInfoAfter=Informacije
InfoAfterLabel=Pročitajte sljedeće važne informacije prije nego što nastavite dalje.
InfoAfterClickLabel=Kada ste spremni nastaviti s instaliranjem, kliknite "Dalje".

; *** "User Information" wizard page
WizardUserInfo=Informacije o korisniku
UserInfoDesc=Upišite informacije o vama.
UserInfoName=&Ime korisnika:
UserInfoOrg=&Organizacija:
UserInfoSerial=&Serijski broj:
UserInfoNameRequired=Morate upisati ime.

; *** "Select Destination Location" wizard page
WizardSelectDir=Odaberite odredišno mjesto
SelectDirDesc=Gdje treba instalirati [name]?
SelectDirLabel3=Instalacija će instalirati [name] u sljedeću mapu.
SelectDirBrowseLabel=Za nastavak kliknite na "Dalje". Ako želite odabrati drugu mapu, kliknite na "Odaberi".
DiskSpaceMBLabel=Potrebno je barem [mb] MB slobodnog prostora na disku.
CannotInstallToNetworkDrive=Instalacija ne može instalirati na mrežnu jedinicu.
CannotInstallToUNCPath=Instalacija ne može instalirati na UNC stazu.
InvalidPath=Morate unijeti punu stazu zajedno sa slovom diska, npr.:%n%nC:\APP%n%nili UNC stazu u obliku:%n%n\\server\share
InvalidDrive=Disk koji ste odabrali ne postoji. Odaberite neki drugi.
DiskSpaceWarningTitle=Nedovoljno prostora na disku
DiskSpaceWarning=Instalacija zahtijeva barem %1 KB slobodnog prostora, a odabrani disk ima samo %2 KB na raspolaganju.%n%nŽelite li svejedno nastaviti?
DirNameTooLong=Naziv mape ili staze je predugačak.
InvalidDirName=Naziv mape je neispravan.
BadDirName32=Naziv mape ne smije sadržavati niti jedan od sljedećih znakova:%n%n%1
DirExistsTitle=Mapa već postoji
DirExists=Mapa:%n%n%1%n%nveć postoji. Želite li svejedno u nju instalirati?
DirDoesntExistTitle=Mapa ne postoji
DirDoesntExist=The folder:%n%n%1%n%nne postoji. Želite li ju stvoriti?

; *** "Select Components" wizard page
WizardSelectComponents=Odaberite komponente
SelectComponentsDesc=Koje komponente želite instalirati?
SelectComponentsLabel2=Odaberite komponente koje želite instalirati, isključite komponente koje ne želite instalirati. Za nastavak kliknite na "Dalje".
FullInstallation=Kompletna instalacija
; if possible don't translate 'Compact' as 'Minimal' (I mean 'Minimal' in your language)
CompactInstallation=Kompaktna instalacija
CustomInstallation=Prilagođena instalacija
NoUninstallWarningTitle=Postojeće komponente
NoUninstallWarning=Instalacija je utvrdila da na vašem računalu već postoje sljedeće komponente:%n%n%1%n%nIsključivanjem tih komponenata, one neće biti deinstalirane.%n%nŽelite li ipak nastaviti?
ComponentSize1=%1 KB
ComponentSize2=%1 MB
ComponentsDiskSpaceMBLabel=Trenutačni odabir zahtijeva barem [mb] MB na disku.

; *** "Select Additional Tasks" wizard page
WizardSelectTasks=Odaberite dodatne zadatke
SelectTasksDesc=Koje dodatne zadatke želite izvršiti?
SelectTasksLabel2=Odaberite zadatke koje želite izvršiti tijekom instaliranja programa [name], zatim kliknite "Dalje".

; *** "Select Start Menu Folder" wizard page
WizardSelectProgramGroup=Odaberite mapu iz "Start" izbornika
SelectStartMenuFolderDesc=Gdje želite da instalacija spremi programske prečace?
SelectStartMenuFolderLabel3=Instalacija će stvoriti programske prečace u sljedeću mapu "Start" izbornika.
SelectStartMenuFolderBrowseLabel=Kliknite "Dalje" za nastavak ili "Odaberi" za odabir jedne druge mape.
MustEnterGroupName=Morate upisati naziv mape.
GroupNameTooLong=Naziv mape ili staze je predug.
InvalidGroupName=Naziv mape nije ispravan.
BadGroupName=Naziv mape ne smije sadržavati sljedeće znakove:%n%n%1
NoProgramGroupCheck2=&Ne stvaraj mapu u "Start" izborniku

; *** "Ready to Install" wizard page
WizardReady=Sve je spremno za instaliranje
ReadyLabel1=Instalacija je spremna za instaliranje [name] na vaše računalo.
ReadyLabel2a=Kliknite "Instaliraj" ako želite instalirati program ili "Natrag" ako želite pregledati ili promijeniti postavke
ReadyLabel2b=Kliknite "Instaliraj" ako želite instalirati program.
ReadyMemoUserInfo=Korisnički podaci:
ReadyMemoDir=Odredišno mjesto:
ReadyMemoType=Vrsta instalacije:
ReadyMemoComponents=Odabrane komponente:
ReadyMemoGroup=Mapa u "Start" izborniku:
ReadyMemoTasks=Dodatni zadaci:

; *** "Preparing to Install" wizard page
WizardPreparing=Priprema za instaliranje
PreparingDesc=Instalacija se priprema za instaliranje [name] na vaše računalo.
PreviousInstallNotCompleted=The installation/removal of a previous program was not completed. You will need to restart your computer to complete that installation.%n%nAfter restarting your computer, run Setup again to complete the installation of [name].
CannotContinue=Instalacija ne može nastaviti. Kliknite na "Odustani" za izlaz.
ApplicationsFound=Sljedeći programi koriste datoteke koje instalacija mora aktualiziranti. Preporučamo da dopustite instalaciji da zatvori ove programe.
ApplicationsFound2=Sljedeći programi koriste datoteke koje instalacija mora aktualiziranti. Preporučamo da dopustite instalaciji da zatvori ove programe. Kad instaliranje završi, instalacija će pokušati ponovo pokrenuti programe.
CloseApplications=&Zatvori programe automatski
DontCloseApplications=&Ne zatvaraj programe
ErrorCloseApplications=Instalacija nij uspjela automatski zatvoriti programe. Preporučamo da zatvorite sve programe koji koriste datoteke, koje se moraju aktulaizirati.

; *** "Installing" wizard page
WizardInstalling=Instaliranje
InstallingLabel=Pričekajte dok ne završi instaliranje programa [name] na vaše računalo.

; *** "Setup Completed" wizard page
FinishedHeadingLabel=Završavanje instalacijskog čarobnjaka za [name]
FinishedLabelNoIcons=Instalacija je završila instaliranje programa [name] na vaše računalo.
FinishedLabel=Instalacija je završila instaliranje programa [name] na vaše računalo. Program se može pokrenuti pomoću instaliranih prečaca.
ClickFinish=Kliknite na "Završi" kako biste izašli iz instalacije.
FinishedRestartLabel=Kako biste završili instaliranje programa [name], potrebno je ponovo pokrenuti računalo. Želite li to sada učiniti?
FinishedRestartMessage=Kako biste završili instaliranje programa [name], potrebno je ponovo pokrenuti računalo.%n%nŽelite li to sada učiniti?
ShowReadmeCheck=Da, želim pročitati README datoteku
YesRadio=&Da, želim sad ponovo pokrenuti računalo
NoRadio=&Ne, kasnije ću ponovo pokrenuti računalo 
; used for example as 'Run MyProg.exe'
RunEntryExec=Pokreni %1
; used for example as 'View Readme.txt'
RunEntryShellExec=Prikaži %1

; *** "Setup Needs the Next Disk" stuff
ChangeDiskTitle=Instalacija treba sljedeći disk
SelectDiskLabel2=Umetnite disk %1 i kliknite na "U redu".%n%nAko se datoteke s ovog diska nalaze na nekom drugom mjestu od prikazanog ispod, upišite ispravnu stazu ili kliknite na "Odaberi".
PathLabel=&Staza:
FileNotInDir2=Staza "%1" ne postoji u "%2". Umetnite odgovarajući disk ili odaberite jednu drugu mapu.
SelectDirectoryLabel=Odaberite mjesto sljedećeg diska.

; *** Installation phase messages
SetupAborted=Instalacija nije završena.%n%nIspravite problem i ponovo pokrenite instalaciju.
AbortRetryIgnoreSelectAction=Odaberite radnju
AbortRetryIgnoreRetry=&Pokušaj ponovo
AbortRetryIgnoreIgnore=&Zanemari grešku i nastavi
AbortRetryIgnoreCancel=Prekini s instaliranjem

; *** Installation status messages
StatusClosingApplications=Zatvaranje programa …
StatusCreateDirs=Stvaranje mapa …
StatusExtractFiles=Izdvajanje datoteka …
StatusCreateIcons=Stvaranje prečaca …
StatusCreateIniEntries=Stvaranje INI unosa …
StatusCreateRegistryEntries=Stvaranje unosa u registar …
StatusRegisterFiles=Registriranje datoteka …
StatusSavingUninstall=Spremanje podataka deinstalacije …
StatusRunProgram=Završavanje instaliranja …
StatusRestartingApplications=Ponovno pokretanje programa …
StatusRollback=Poništavanje promjena …

; *** Misc. errors
ErrorInternal2=Interna greška: %1
ErrorFunctionFailedNoCode=%1 nije uspjelo
ErrorFunctionFailed=%1 nije uspjelo; kod %2
ErrorFunctionFailedWithMessage=%1 failed; kod %2.%n%3
ErrorExecutingProgram=Nije moguće pokrenuti datoteku:%n%1

; *** Registry errors
ErrorRegOpenKey=Greška prilikom otvaranja ključa registra:%n%1\%2
ErrorRegCreateKey=Greška prilikom stvaranja ključa registra:%n%1\%2
ErrorRegWriteKey=Greška prilikom pisanja u ključ registra:%n%1\%2

; *** INI errors
ErrorIniEntry=Greška prilikom stvaranja INI unosa u datoteci "%1".

; *** File copying errors
FileAbortRetryIgnoreSkipNotRecommended=&Preskoči ovu datoteku (ne preporuča se)
FileAbortRetryIgnoreIgnoreNotRecommended=&Zanemari grešku i nastavi (ne preporuča se)
SourceIsCorrupted=Izvorišna datoteka je oštećena
SourceDoesntExist=Izvorišna datoteka "%1" ne postoji
ExistingFileReadOnly2=Postojeću datoteku nije bilo moguće zamijeniti, jer je označena sa "samo-za-čitanje".
ExistingFileReadOnlyRetry=&Uklonite atribut "samo-za-čitanje" i pokušajte ponovo
ExistingFileReadOnlyKeepExisting=&Zadrži postojeću datoteku
ErrorReadingExistingDest=Pojavila se greška prilikom pokušaja čitanja postojeće datoteke:
FileExists=The file already exists.%n%nŽelite li da ju instalacija prepiše?
ExistingFileNewer=Postojeća datoteka je novija od one, koju pokušavate instalirati. Preporuča se da zadržite postojeću datoteku.%n%nŽelite li zadržati postojeću datoteku?
ErrorChangingAttr=Pojavila se greška prilikom pokušaja promjene atributa postojeće datoteke:
ErrorCreatingTemp=Pojavila se greška prilikom pokušaja stvaranja datoteke u odredišnoj mapi:
ErrorReadingSource=Pojavila se greška prilikom pokušaja čitanja izvorišne datoteke:
ErrorCopying=Pojavila se greška prilikom pokušaja kopiranja datoteke:
ErrorReplacingExistingFile=Pojavila se greška prilikom pokušaja zamijenjivanja datoteke:
ErrorRestartReplace=Zamijenjivanje nakon ponovnog pokretanja nije uspjelo:
ErrorRenamingTemp=Pojavila se greška prilikom pokušaja preimenovanja datoteke u odredišnoj mapi:
ErrorRegisterServer=Nije moguće registrirati DLL/OCX: %1
ErrorRegSvr32Failed=Greška u RegSvr32. Izlazni kod %1
ErrorRegisterTypeLib=Nije moguće registrirati type library: %1

; *** Uninstall display name markings
; used for example as 'My Program (32-bit)'
UninstallDisplayNameMark=%1 (%2)
; used for example as 'My Program (32-bit, All users)'
UninstallDisplayNameMarks=%1 (%2, %3)
UninstallDisplayNameMark32Bit=32-bitni
UninstallDisplayNameMark64Bit=64-bitni
UninstallDisplayNameMarkAllUsers=Svi korisnici
UninstallDisplayNameMarkCurrentUser=Trenutačni korisnik

; *** Post-installation errors
ErrorOpeningReadme=Pojavila se greška prilikom pokušaja otvaranja README datoteke.
ErrorRestartingComputer=Instalacija nije mogla ponovo pokrenuti računalo. Učinite to ručno.

; *** Uninstaller messages
UninstallNotFound=Datoteka "%1" ne postoji. Deinstaliranje nije moguće.
UninstallOpenError=Datoteku "%1" nije bilo moguće otvoriti. Deinstaliranje nije moguće
UninstallUnsupportedVer=Deinstalacijska datoteka "%1" je u formatu koji ova verzija deinstalacijskog programa ne prepoznaje. Deinstaliranje nije moguće
UninstallUnknownEntry=Nepoznat zapis (%1) je pronađen u deinstalacijskoj datoteci
ConfirmUninstall=Zaista želite ukloniti %1 i sve pripadajuće komponente?
UninstallOnlyOnWin64=Ovu instalaciju je moguće ukloniti samo na 64-bitnom Windows sustavu.
OnlyAdminCanUninstall=Ovu instalaciju je moguće ukloniti samo korisnik s administrativnim pravima.
UninstallStatusLabel=Pričekajte dok se %1 uklanja s vašeg računala.
UninstalledAll=%1 je uspješno uklonjen s vašeg računala.
UninstalledMost=Deinstaliranje programa %1 je završeno.%n%nNeke elemente nije bilo moguće ukloniti. Mogu se ukloniti ručno.
UninstalledAndNeedsRestart=Kako biste završili deinstalirati %1, morate ponovo pokrenuti vaše računalo%n%nŽelite li to sad učiniti?
UninstallDataCorrupted="%1" datoteka je oštećena. Deinstaliranje nije moguće

; *** Uninstallation phase messages
ConfirmDeleteSharedFileTitle=Ukloniti dijeljene datoteke?
ConfirmDeleteSharedFile2=Sustav ukazuje na to, da sljedeće dijeljenu datoteku ne koristi niti jedan program. Želite li ukloniti tu dijeljenu datoteku?%n%nAko neki programi i dalje koriste tu datoteku, a ona se izbriše, ti programi neće ispravno raditi. Ako niste sigurni, odaberite "Ne". Datoteka neće štetiti vašem sustavu.
SharedFileNameLabel=Datoteka:
SharedFileLocationLabel=Mjesto:
WizardUninstalling=Stanje deinstalacije
StatusUninstalling=%1 deinstaliranje …

; *** Shutdown block reasons
ShutdownBlockReasonInstallingApp=%1 instaliranje.
ShutdownBlockReasonUninstallingApp=%1 deinstaliranje.

; The custom messages below aren't used by Setup itself, but if you make
; use of them in your scripts, you'll want to translate them.

[CustomMessages]

NameAndVersion=%1 verzija %2
AdditionalIcons=Dodatni prečaci:
CreateDesktopIcon=Stvori prečac na ra&dnoj površini
CreateQuickLaunchIcon=Stvori prečac u traci za &brzo pokretanje
ProgramOnTheWeb=%1 na internetu
UninstallProgram=Deinstaliraj %1
LaunchProgram=Pokreni %1
AssocFileExtension=&Poveži program %1 s datotečnim nastavkom %2
AssocingFileExtension=Povezivanje programa %1 s datotečnim nastavkom %2 …
AutoStartProgramGroupDescription=Pokretanje:
AutoStartProgram=Automatski pokreni %1
AddonHostProgramNotFound=%1 nije nađen u odabranoj mapi.%n%nŽelite li svejedno nastaviti?
