; *** Inno Setup version 5.1.11+ Croatian messages ***

; To download user-contributed translations of this file, go to:
;   http://www.jrsoftware.org/is3rdparty.php

; Note: When translating this text, do not add periods (.) to the end of
; messages that didn't have them already, because on those messages Inno
; Setup adds the periods automatically (appending a period would result in
; two periods being displayed).
;
; Maintained by HasanOsmanagiæ(hasan.osmanagic@public.carnet.hr)
; Based on translation v. 5.1.1 by: Krunoslav Kanjuh (krunoslav.kanjuh@zg.t-com.hr)

[LangOptions]
; The following three entries are very important. Be sure to read and 
; understand the '[LangOptions] section' topic in the help file.
LanguageName=Hrvatski
LanguageID=$041a
LanguageCodePage=1250

[Messages]

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
SetupAppTitle=Instalacija
SetupWindowTitle=Instalacija - %1
UninstallAppTitle=Deinstalacija
UninstallAppFullTitle=Deinstalacija %1

; *** Misc. common
InformationTitle=Informacija
ConfirmTitle=Potvrda
ErrorTitle=Greška

; *** SetupLdr messages
SetupLdrStartupMessage=Poèela je instalacija programa %1. Nastaviti?
LdrCannotCreateTemp=Ne mogu stvoriti privremenu datoteku. Instalacija je prekinuta
LdrCannotExecTemp=Ne mogu uèitati datoteku u privremenoj mapi. Instalacija je prekinuta

; *** Startup error messages
LastErrorMessage=%1.%n%nGreška %2: %3
SetupFileMissing=Nema datoteke %1 u instalacijskoj mapi. Molimo vas riješite problem ili nabavite novu kopiju programa
SetupFileCorrupt=Instalacijske datoteke sadrže grešku. Nabavite novu kopiju programa
SetupFileCorruptOrWrongVer=Instalacijske datoteke sadrže grešku, ili nisu kompatibilne s ovom verzijom instalera. Molimo vas riješite problem ili nabavite novu kopiju programa
NotOnThisPlatform=Ovaj program ne radi na %1
OnlyOnThisPlatform=Ovaj program se mora pokrenuti na %1
OnlyOnTheseArchitectures=Ovaj program može biti instaliran na verzijama Windowsa predviðenim za sljedeæu procesorsku arhitekturu:%n%n%1
MissingWOW64APIs=Verzija Windowsa ne udovoljava zahtjevima za 64-bitnu instalaciju. Pokušajte riješiti problem instalacijom: Service Pack %1
WinVersionTooLowError=Ovaj program zahtjeva %1 verziju %2 ili noviju
WinVersionTooHighError=Ovaj program ne može biti instaliran na %1 verziji %2 ili starijoj
AdminPrivilegesRequired=Za instaliranje morate imati administratorske ovlasti
PowerUserPrivilegesRequired=Za instaliranje morate imati administratorske ovlasti ili biti ovlašteni korisnik
SetupAppRunningError=Instaler nalazi neugašen %1.%n%nMolimo zatvorite program i sve instance i potom pritisnite tipku 'Nastavi>' za nastavak ili 'Odustajem' za prekid
UninstallAppRunningError=Deinstaler nalazi neugašen %1.%n%nMolimo zatvorite program i sve njegove instance i potom pritisnite tipku 'Nastavi>' za nastavak ili 'Odustajem' za prekid

; *** Misc. errors
ErrorCreatingDir=Ne mogu stvoriti mapu "%1"
ErrorTooManyFilesInDir=Ne može se stvoriti nova datoteka u mapi "%1" jer veæ sadrži previše datoteka

; *** Setup common messages
ExitSetupTitle=Prekid instalacije
ExitSetupMessage=Instalacija nije završena. Ako sad prekinete, program neæe biti instaliran.%n%nInstalaciju uvijek možete pokrenuti kasnije.%n%nPrekid instalacije?
AboutSetupMenuItem=&O Instaleru...
AboutSetupTitle=O Instaleru
AboutSetupMessage=%1 verzija %2%n%3%n%n%1 Wev strana:%n%4
AboutSetupNote=
TranslatorNote=

; *** Buttons
ButtonBack=< Na&zad
ButtonNext=&Nastavi >
ButtonInstall=&Instaliraj
ButtonOK=U redu
ButtonCancel=Odustajem
ButtonYes=&Da
ButtonYesToAll=Da za &sve
ButtonNo=&Ne
ButtonNoToAll=N&e za sve
ButtonFinish=&Izlaz
ButtonBrowse=&Odabir...
ButtonWizardBrowse=Oda&bir...
ButtonNewFolder=Stvori novu &mapu

; *** "Select Language" dialog messages
SelectLanguageTitle=Odabir jezika instalacije
SelectLanguageLabel=Odberite jezik koji želite koristiti pri instalaciji

; *** Common wizard text
ClickNext=Pritisnite tipku 'Nastavi >' za nastavak ili 'Odustajem' za prekid instalacije
BeveledLabel=
BrowseDialogTitle=Odabir mape
BrowseDialogLabel=Odaberite mapu iz popisa koja slijedi te pritisnite tipku 'U redu'
NewFolderName=Nova mapa

; *** "Welcome" wizard page
WelcomeLabel1=Dobro došli u instalaciju programa [name]
WelcomeLabel2=Instaler æe instalirati program [name/ver] na vaše raèunalo.%n%nPreporuèamo da zatvorite sve programe prije nastavka

; *** "Password" wizard page
WizardPassword=Lozinka
PasswordLabel1=Instalacija je zaštiæena lozinkom.
PasswordLabel3=Upišite lozinku. Lozinka razlikuje mala i velika slova
PasswordEditLabel=&Lozinka:
IncorrectPassword=Upisana je pogrešna lozinka. Pokušajte ponovo

; *** "License Agreement" wizard page
WizardLicense=Ugovor o korištenju
LicenseLabel=Molimo vas, prije nastavka, pažljivo proèitajte slijedeæe:
LicenseLabel3=Molimo vas, pažljivo proèitajte Ugovor o korištenju. Za nastavak instalacije morate prihvatiti Ugovor o korištenju.
LicenseAccepted=&Prihvaæam Ugovor o korištenju
LicenseNotAccepted=&Ne prihvaæam Ugovor o korištenju

; *** "Information" wizard pages
WizardInfoBefore=Napomena
InfoBeforeLabel=Molimo vas, prije nastavka, proèitajte sljedeæe:
InfoBeforeClickLabel=Kada budete spremni nastaviti instalaciju, pritisnite tipku 'Nastavi >'
WizardInfoAfter=Napomena
InfoAfterLabel=Molimo vas, prije nastavka, proèitajte sljedeæe:
InfoAfterClickLabel=Kada budete spremni nastaviti instalaciju pritisnite tipku 'Nastavi >'

; *** "User Information" wizard page
WizardUserInfo=Korisnik
UserInfoDesc=Upišite vaše podatke
UserInfoName=&Ime korisnika:
UserInfoOrg=&Organizacija:
UserInfoSerial=&Serijski broj:
UserInfoNameRequired=Morate upisati ime

; *** "Select Destination Directory" wizard page
WizardSelectDir=Odabir odredišne mape
SelectDirDesc=Mapa instalacije programa.
SelectDirLabel3=[name] æe biti instaliran u mapu
SelectDirBrowseLabel=Za nastavak pritisnite tipku 'Nastavi >'. Za odabir druge mape, pritisnite tipku 'Odabir'
DiskSpaceMBLabel=Ovaj program zahtjeva minimalno [mb] MB slobodnog prostora na disku.
ToUNCPathname=Instaler ne može instalirati na UNC datoteku. Ako pokušavate instalirati u mrežu, mrežni disk mora biti mapiran
InvalidPath=Morate unijeti punu stazu zajedno sa slovnom oznakom diska; npr:%nC:\APP
InvalidDrive=Disk koji ste odabrali ne postoji. Odaberite neki drugi
DiskSpaceWarningTitle=Nedovoljno prostora na disku
DiskSpaceWarning=Instalacija zahtjeva bar %1 KB slobodnog prostora, a odabrani disk ima %2 KB na raspolaganju.%n%nNastaviti?
DirNameTooLong=Predugi naziv mape ili staze
InvalidDirName=Krivi naziv mape.
BadDirName32=Naziv mape ne smije, nakon toèke, sadržavati niti jedan od sljedeæih znakova:%n%n%1
DirExistsTitle=Mapa veæ postoji
DirExists=mapa:%n%n%1%n%nveæ postoji. Instalirati u nju?
DirDoesntExistTitle=Mapa ne postoji
DirDoesntExist=mapa:%n%n%1%n%nne postoji. Stvoriti?

; *** "Select Components" wizard page
WizardSelectComponents=Odabir komponenata
SelectComponentsDesc=Koje komponente želite instalirati?
SelectComponentsLabel2=Odaberite komponente koje želite instalirati ili uklonite kvaèicu uz komponente koje ne želite
FullInstallation=Puna instalacija

; if possible don't translate 'Compact' as 'Minimal' (I mean 'Minimal' in your language)
CompactInstallation=Najmanja instalacija
CustomInstallation=Instalacija prema želji
NoUninstallWarningTitle=Postojeæe komponente
NoUninstallWarning=Na vašem raèunalu veæ postoje komponente:%n%n%1%n%nNeodabir tih komponenata ne dovodi do njihove deinstalacije.%n%nNastaviti?
ComponentSize1=%1 KB
ComponentSize2=%1 MB
ComponentsDiskSpaceMBLabel=Vaš izbor zahtijeva najmanje [mb] MB prostora na disku

; *** "Select Additional Tasks" wizard page
WizardSelectTasks=Odaberite radnje
SelectTasksDesc=Koje radnje želite da se izvrše?
SelectTasksLabel2=Odaberite radnje koje æe se izvršiti tokom instalacije programa [name]

; *** "Select Start Menu Folder" wizard page
WizardSelectProgramGroup=Odaberite programsku grupu
SelectStartMenuFolderDesc=Lokacija preèice programa
SelectStartMenuFolderLabel3=Instaler æe kreirati preèicu programa u Poèetnom izborniku
SelectStartMenuFolderBrowseLabel=Za nastavak pritisnite tipku 'Nastavi >'. Ako želite odabrati drugu mapu pritisnite tipku 'Odabir'
MustEnterGroupName=Morate unijeti naziv programske grupe
GroupNameTooLong=Predugi naziv mape ili staze
InvalidGroupName=Naziv mape je pogrešan
BadGroupName=Naziv programske grupe ne smije sadržavati sljedeæe znakove:%n%n%1
NoProgramGroupCheck2=&Ne stvaraj %1 programsku grupu

; *** "Ready to Install" wizard page
WizardReady=Spreman za instalaciju
ReadyLabel1=Instaler je spreman instalirati program [name] na vaše raèunalo
ReadyLabel2a=Pritisnite tipku 'Instaliraj' za instalaciju programa ili 'Nazad' ako želite provjeriti ili promjeniti postavke
ReadyLabel2b=Pritisnite tipku 'Instaliraj' za instalaciju programa
ReadyMemoUserInfo=Podaci o korisniku:
ReadyMemoDir=Odredišni mapa:
ReadyMemoType=Tip instalacije:
ReadyMemoComponents=Odabrane komponente:
ReadyMemoGroup=Programska grupa:
ReadyMemoTasks=Dodatni zadaci:

; *** "Preparing to Install" wizard page
WizardPreparing=Pripremam instalaciju
PreparingDesc=Instaler se priprema da program [name] instalira na vaše raèunalo
PreviousInstallNotCompleted=Instalacija/deinstalacija prethodnog programa nije završena. Morate restartati raèunalo kako bi završili instalaciju.%n%nNakon restartanja raèunala, ponovno pokrenite instalaciju [name]
CannotContinue=Instaler ne može nastaviti. Molimo pritisnite tipku 'Odustajem' za izlaz

; *** "Installing" wizard page
WizardInstalling=Instaliram
InstallingLabel=Prièekajte završetak instalacije programa [name] na vaše raèunalo

; *** "Setup Completed" wizard page
FinishedHeadingLabel=Program [name] je instaliran
FinishedLabelNoIcons=Instalacija programa [name] je završena
FinishedLabel=Instalacija programa [name] je završena. Program možete pokrenuti preko instaliranih preèica
ClickFinish=Pritisnite tipku 'Izlaz' za izlaz iz Instalera
FinishedRestartLabel=Za završetak instalacije programa [name], potrebno je ponovno pokrenuti raèunalo. Pokrenuti odmah?
FinishedRestartMessage=Završitak instalacije programa [name], zahtijeva ponovno pokretanje raèunala.%n%nPokrenuti odmah?
ShowReadmeCheck=Da, želim proèitati datoteku Proèitaj
YesRadio=&Da, želim odmah ponovno pokrenuti raèunalo
NoRadio=&Ne, kasnije æu ga pokrenuti

; used for example as 'Run MyProg.exe'
RunEntryExec=Pokreni %1
; used for example as 'View Readme.txt'
RunEntryShellExec=Pregledati %1

; *** "Setup Needs the Next Disk" stuff
ChangeDiskTitle=Potreban je sljedeæi disk:
SelectDiskLabel2=Umetnite disketu %1 i pritisnite tipku 'U redu'.%n%nAko se datoteke s ove diskete nalaze na nekom drugom mediju %2 , ispravno upišite stazu do njega ili pritisnite tipku 'Odaberi'
PathLabel=&Staza:
FileNotInDir2=Datoteka "%1" ne postoji u "%2". Molimo vas umetnite odgovarajuæi disk ili odaberete drugi %3.
SelectDirectoryLabel=Molimo vas odaberite lokaciju sljedeæeg diska

; *** Installation phase messages
SetupAborted=Instalacija nije završena.%n%nMolimo vas, riješite problem i opet pokrenite instalaciju
EntryAbortRetryIgnore=Pritisnite tipku 'Retry' za novi pokušaj, 'Ignore' za nastavak, ili 'Abort' za prekid instalacije

; *** Installation status messages
StatusCreateDirs=Stvaram mape...
StatusExtractFiles=Izdvajam datoteke...
StatusCreateIcons=Stvaram ikone...
StatusCreateIniEntries=Stvaram INI datoteke...
StatusCreateRegistryEntries=Stvaram podatke za registar...
StatusRegisterFiles=Registriram datoteke...
StatusSavingUninstall=Pohranjujem deinstalacijske podatke...
StatusRunProgram=Završavam instalaciju...
StatusRollback=Poništavam promjene...

; *** Misc. errors
ErrorInternal2=Interna greška: %1
ErrorFunctionFailedNoCode=%1 nije uspjelo
ErrorFunctionFailed=%1 nije uspjelo; code %2
ErrorFunctionFailedWithMessage=%1 nije uspjelo; code %2.%n%3
ErrorExecutingProgram=Ne mogu uèitati datoteku:%n%1

; *** Registry errors
ErrorRegOpenKey=Greška pri otvaranju kljuèa registra:%n%1\%2
ErrorRegCreateKey=Greška pri stvaranju kljuèa registra:%n%1\%2
ErrorRegWriteKey=Greške pri pohrani u kljuè registra:%n%1\%2

; *** INI errors
ErrorIniEntry=Greška pri stvaranju INI podataka u datoteci "%1"

; *** File copying errors
FileAbortRetryIgnore=Pritisnite tipku 'Retry' za novi pokušaj, 'Ignore' za zanemarivanje ove datoteke (ne preporuèa se) ili 'Abort' za prekid instalacije
FileAbortRetryIgnore2=Pritisnite tipku 'Retry' za novi pokušaj, 'Ignore' za nastavak zanemarujuæi poruku (ne preporuèa se) ili 'Abort' za prekid instalacije
SourceIsCorrupted=Izvorišna datoteka je ošteæena
SourceDoesntExist=Izvorišna datoteka "%1" ne postoji
ExistingFileReadOnly=Datoteka je oznaèena "samo-za-èitanje".%n%nPritisnite tipku 'Retry' da uklonite oznaku "samo-za-èitanje" i pokušajte ponovno, 'Ignore' za zanemarivanje ove datoteke, ili 'Abort' za prekid instalacije
ErrorReadingExistingDest=Pojavila se greška pri pokušaju èitanja postojeæe datoteke:
FileExists=Datoteka veæ postoji.%n%nNatpisati?
ExistingFileNewer=Postojeæa datoteka je novija od instalacijske. Preporuèa se zadržati postojeæu datoteku.%n%nZadržati postojeæu datoteku?
ErrorChangingAttr=Pojavila se greška pri pokušaju promjene atributa postojeæe datoteke
ErrorCreatingTemp=Pojavila se greška pri pokušaju stvaranja datoteke u odredišnoj mapi
ErrorReadingSource=Pojavila se greška pri pokušaju èitanja izvorišne datoteke
ErrorCopying=Pojavila se greška pri pokušaju kopiranja datoteke
ErrorReplacingExistingFile=Pojavila se greška pri pokušaju zamjene datoteke
ErrorRestartReplace=Zamjena nakon ponovnog pokretanja nije uspjela
ErrorRenamingTemp=Pojavila se greška pri pokušaju preimenovanja datoteke u odredišnoj mapi
ErrorRegisterServer=Ne mogu registrirati DLL/OCX: %1
ErrorRegSvr32Failed=Greška u RegSvr32: greška %1
ErrorRegisterTypeLib=Ne mogu registrirati datoteku library: %1

; *** Post-installation errors
ErrorOpeningReadme=Pojavila se greška pri pokušaju otvaranja datoteke Proèitaj
ErrorRestartingComputer=Instaler ne može ponovno pokrenuti raèunalo. Molimo vas, uèinite to sami

; *** Uninstaller messages
UninstallNotFound=Datoteka "%1" ne postoji. Deinstalacija prekinuta
UninstallOpenError=Datoteku "%1" ne mogu otvoriti. Deinstalacija nije moguæa
UninstallUnsupportedVer=Deinstalacijska datoteka "%1" je u obliku koju ove verzija deinstalera ne prihvaæa. Nije moguæa deinstalacija
UninstallUnknownEntry=U deinstalacijskoj datoteci je pronaðen nepoznat zapis (%1)
ConfirmUninstall=Ukloniti %1 i sve pripadne komponente?
UninstallOnlyOnWin64=Ova instalacija može biti uklonjena samo na 64-bitnim Windows-ima
OnlyAdminCanUninstall=Ova instalacija može biti uklonjena samo s administratorskim ovlastima
UninstallStatusLabel=Prièekajte dok %1 ne bude uklonjen s vašeg raèunala
UninstalledAll=Program %1 je uspješno uklonjen sa vašeg raèunala
UninstalledMost=Deinstalacija programa %1 je završena.%n%nNeke elemente nije bilo moguæe ukloniti. Molimo vas da to uèinite sami
UninstalledAndNeedsRestart=Za završetak deinstalacije %1, vaše raèunalo morate ponovno pokrenuti%n%nPokrenuti odmah?
UninstallDataCorrupted="%1" datoteka je ošteæena. Deinstalacija nije moguæa

; *** Uninstallation phase messages
ConfirmDeleteSharedFileTitle=Brisanje dijeljene datoteke?
ConfirmDeleteSharedFile2=Sistem ukazuje da sljedeæe dijeljene datoteke ne koristi niti jedan program. Želite li da Deintaler ukloni te dijeljene datoteke?%n%nAko neki programi ipak koriste te datoteke, a one su obrisane, ti programi neæe ispravno raditi. Ako niste sigurni, odaberite Ne. Ostavljanje datoteka neæe uzrokovati štetu vašem sistemu
SharedFileNameLabel=Datoteka:
SharedFileLocationLabel=Staza:
WizardUninstalling=Deinstalacija
StatusUninstalling=Deinstaliram %1...

; The custom messages below aren't used by Setup itself, but if you make
; use of them in your scripts, you'll want to translate them.

[CustomMessages]
;Inno Setup Built-in Custom Messages
NameAndVersion=%1 verzija %2
AdditionalIcons=Dodatne ikone:
OptionalFeatures=Po volji:
CreateDesktopIcon=Stvori ikonu na &desktopu
CreateQuickLaunchIcon=Stvori ikonu u brzom izborniku
ProgramOnTheWeb=%1 je na Web-u
UninstallProgram=Deinstaliraj %1
LaunchProgram=Pokreni %1
AssocFileExtension=Pridru&ži %1 s %2 ekstenzijom datoteke
AssocingFileExtension=Pridružujem %1 s %2 ekstenzijom datoteke

;Things we can also localize
CompanyName=Thingamahoochie Software

;Types
TypicalInstallation=Uobièajena instalacija
FullInstallation=Puna instalacija
CompactInstallation=Najmanja instalacija
CustomInstallation=Izborna instalacija

;Components
AppCoreFiles=WinMerge temeljne datoteke
ApplicationRuntimes=Datoteke izvoðenja (Runtimes)
Filters=Filtri
Plugins=Dodaci

;Localization Components
Languages=Jezici
BulgarianLanguage=Bulgarian menus and dialogs
CatalanLanguage=Catalan menus and dialogs
ChineseSimplifiedLanguage=Chinese (Simplified) menus and dialogs
ChineseTraditionalLanguage=Chinese (Traditional) menus and dialogs
CroatianLanguage=Hrvatski izbornik i dijalozi
CzechLanguage=Czech menus and dialogs
DanishLanguage=Danish menus and dialogs
DutchLanguage=Dutch menus and dialogs
FrenchLanguage=French menus and dialogs
GermanLanguage=German menus and dialogs
HungarianLanguage=Hungarian menus and dialogs
ItalianLanguage=Italian menus and dialogs
JapaneseLanguage=Japanese menus and dialogs
KoreanLanguage=Korean menus and dialogs
NorwegianLanguage=Norwegian menus and dialogs
PolishLanguage=Polish menus and dialogs
PortugueseBrazilLanguage=Portuguese (Brazilian) menus and dialogs
PortugueseLanguage=Portuguese menus and dialogs
RussianLanguage=Russian menus and dialogs
SlovakLanguage=Slovak menus and dialogs
SpanishLanguage=Spanish menus and dialogs
SwedishLanguage=Swedish menus and dialogs
TurkishLanguage=Turkish menus and dialogs

;Tasks
ExplorerContextMenu=&Integracija u Explorer kontekst izbornik
IntegrateTortoiseCVS=Integracija s &TortoiseCVS
IntegrateTortoiseSVN=Integracija s T&ortoiseSVN
IntegrateClearCase=Integracija s Rational &ClearCase
IntegrateDelphi4=Rad s Borland® Delphi &4 Binarnim datotekama
UpdatingCommonControls=Obnova System's Common kontrola

;Icon Labels
ReadMe=Proèitaj
UsersGuide=Vodiæ za korisnika
UpdatingCommonControls=Obnova System's Common kontrola
ViewStartMenuFolder=&Pregled WinMerge poèetne mape
PluginsText=Dodaci

;Code Dialogs
DeletePreviousStartMenu=Instaler je otkrio promjenu lokacije Poèetnog izbornika od "%s" u "%s". Obrisati stari Poèetni izbornik?

; Project file description
ProjectFileDesc=WinMerge Usporedbe
