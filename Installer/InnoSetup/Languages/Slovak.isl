; *** Inno Setup - version 4.0.5+ SLOVAK messages ***
;
;  TRANSLATION TO SLOVAK:
;  (c) Arpad Toth, ekosoft@signalsoft.sk - 3.0.2
;  (c) Juraj Petrik, jpetrik@i-servis.net - 3.0.6 - 2003-03-14
;  (c) Branislav Kopun, kopun@centrum.sk - 4.0.5 - 2003-07-29
;
;  Pokia¾ by niekto upravoval tuto verziu, pošlite prosím, 
;  emailom upravenú verziu na vyssie uvedene adresy, dakujeme.
;  
;

[LangOptions]
LanguageName=Slovenèina
LanguageID=$041B
; If the language you are translating to requires special font faces or
; sizes, uncomment any of the following entries and change them accordingly.
;DialogFontName=MS Shell Dlg
;DialogFontSize=8
;DialogFontStandardHeight=13
;TitleFontName=Arial
;TitleFontSize=29
;WelcomeFontName=Verdana
;WelcomeFontSize=12
;CopyrightFontName=Arial
;CopyrightFontSize=8

[Messages]

; *** Application titles
SetupAppTitle=Inštalácia
SetupWindowTitle=Inštalácia aplikácie "%1"
UninstallAppTitle=Odinštalovanie
UninstallAppFullTitle=Odinštalovanie aplikácie "%1"

; *** Misc. common
InformationTitle=Informácie
ConfirmTitle=Potvrdenie
ErrorTitle=Chyba

; *** SetupLdr messages
SetupLdrStartupMessage=Inštalácia aplikácie "%1". Prajete si pokraèova?
LdrCannotCreateTemp=Nemono vytvori doèasnı súbor. Inštalácia bude ukonèená.
LdrCannotExecTemp=Nemono spusti súbor v doèasnom adresári. Inštalácia bude ukonèená.

; *** Startup error messages
LastErrorMessage=%1.%n%nChyba %2: %3
SetupFileMissing=V adresári inštalácie chıba súbor %1. Prosím, odstráòte problém alebo si zaobstarajte novú kópiu aplikácie.
SetupFileCorrupt=Inštalaèné súbory sú porušené. Prosím, zaobstarajte si novú kópiu aplikácie.
SetupFileCorruptOrWrongVer=Instalaèné súbory sú porušené alebo sa nezluèujú s touto verziou inštalácie. Prosím, odstráòte problém, alebo si zaobstarajte novú kópiu aplikácie.
NotOnThisPlatform=Túto aplikáciu nemono spusti na %1.
OnlyOnThisPlatform=Táto aplikácia vyaduje pre spustenie systém %1.
WinVersionTooLowError=Táto aplickácia vyaduje %1 verziu %2 alebo vyššiu.
WinVersionTooHighError=Táto aplikácia nemôe by nainštalovaná na %1 verzii %2 alebo vyššej.
AdminPrivilegesRequired=Pre inštaláciu tejto aplikácie musíte by prihlásenı ako administrátor.
PowerUserPrivilegesRequired=Pre inštaláciu tejto aplikácie musíš by prihlásenı ako administrátor alebo ako uívate¾ skupiny "Power Users".
SetupAppRunningError=Inštalátor rozpoznal, e aplikácia %1 je u spustená.%n%nProsím ukonèite všetky jej súèasti a potom pokraèujte "OK" inak kliknite na "Storno" pre ukonèenie.
UninstallAppRunningError=Odinštalátor rozpoznal, e aplikácia %1 je spustená.%n%nProsím ukonèite všetky jej súèasti a potom pokraèujte "OK" inak kliknite "Storno" pre ukonèenie.

; *** Misc. errors
ErrorCreatingDir=Inštalátor nemohol vytvori adresár "%1"
ErrorTooManyFilesInDir=Nemono vytvori súbor v adresári "%1" pretoe obsahuje príliš ve¾a súborov

; *** Setup common messages
ExitSetupTitle=Ukonèenie inštalácie
ExitSetupMessage=Inštalácia nebola dokonèená. Pokia¾ teraz skonèíte, aplikácia nebude nainštalovaná.%n%nK dokonèeniu inštalácie môete inštalaènı program spusti inokedy.%n%nUkonèi inštaláciu?
AboutSetupMenuItem=O progr&ame...
AboutSetupTitle=O inštalácii
AboutSetupMessage=%1 verzia %2%n%3%n%n%1 domovská stránka:%n%4
AboutSetupNote=

; *** Buttons
ButtonBack=< Návra&t
ButtonNext=Pokr&aèuj >
ButtonInstall=&Inštalova
ButtonOK=&OK
ButtonCancel=&Storno
ButtonYes=&Ano
ButtonYesToAll=Áno p&re Všetky
ButtonNo=&Nie
ButtonNoToAll=Nie pr&e Všetky
ButtonFinish=&Dokonèi
ButtonBrowse=&Nalistova...
ButtonWizardBrowse=&Prechádza..
ButtonNewFolder=&Vytvoti novú zloku

; *** "Select Language" dialog messages
SelectLanguageTitle=Vıber jazyka inštalácie
SelectLanguageLabel=Vıber sprievodného jazyka poèas inštalácie:

; *** Common wizard text
ClickNext="Pokraèuj" pre pokraèovanie, "Storno" ukonèí inštaláciu.
BeveledLabel=
BrowseDialogTitle=Vyh¾ada zloku	
BrowseDialogLabel=Z nišie uvedeného zoznamu vyberte zloku a klepnite na OK.	
NewFolderName=Nová zloka

; *** "Welcome" wizard page
WelcomeLabel1=Vitajte v inštalaènom programe aplikácie "[name]".
WelcomeLabel2=Tento program nainštaluje aplikáciu "[name/ver]" na Váš poèítaè.%n%nPredtım, ako budete pokraèova, doporuèujeme uzavrie všetky spustené aplikácie. Predídete tım monım konfliktom behom inštalácie.

; *** "Password" wizard page
WizardPassword=Heslo
PasswordLabel1=Táto inštalácia je chránená heslom.
PasswordLabel3=Prosím zadajte heslo a pokraèujte.%n%nHeslo rozlišuje ve¾ké a malé znaky.
PasswordEditLabel=&Heslo:
IncorrectPassword=Zadané heslo nie je správne. Prosím skúste to znovu.

; *** "License Agreement" wizard page
WizardLicense=Licenèné podmienky
LicenseLabel=Prosím èítajte pozorne nasledujúce licenèné podmienky a potom pokraèujte.
LicenseLabel3=Prosím èítajte pozorne nasledujúce licenèné podmienky. Pre pokraèovanie inštalácie musíte súhlasi s licenènımi podmienkami.
LicenseAccepted=Súhl&asím s licenènımi podmienkami
LicenseNotAccepted=&Nesúhlasím s licenènımi podmienkami

; *** "Information" wizard pages
WizardInfoBefore=Informácia
InfoBeforeLabel=Predtım ako budete pokraèova, preèítajte si prosím najprv nasledujúcu dôleitú informáciu.
InfoBeforeClickLabel=Kliknutím na "Pokraèuj" pokraèujte v inštaláci.
WizardInfoAfter=Informácia
InfoAfterLabel=Predtım ako budete pokraèova, preèítajte si prosím najprv nasledujúcu dôleitú informáciu.
InfoAfterClickLabel=Kliknutím na "Pokraèuj" pokraèujte v inštaláci.

; *** "User Information" wizard page
WizardUserInfo=Informácie o uívate¾ovy
UserInfoDesc=Vyplòte prosím informácie o Vás.
UserInfoName=Meno &uívate¾a:
UserInfoOrg=&Organizácia:
UserInfoSerial=&Sériové èíslo:
UserInfoNameRequired=Musíte zada meno uívate¾a.

; *** "Select Destination Directory" wizard page
WizardSelectDir=Zvo¾te cie¾ovı adresár
SelectDirDesc=Kde má by aplikácia "[name]" nainštalovaná?
SelectDirLabel3=[name] bude nainštalovanı do následujúcej zloky.
SelectDirBrowseLabel=Klepnite na Ïalší pre pokraèovanie. Pokia¾ chcete zvoli inú zloku, klepnite na Prechádza.
DiskSpaceMBLabel=Aplikácia vyaduje najmenej [mb] MB miesta na disku.
ToUNCPathname=Nie je moné inštalova do cesty UNC. Pokia¾ sa pokúšate inštalova do siete, musíte si najkôr namapova sieovı disk.
InvalidPath=Musíte zada celú cestu aj s písmenom disku, napríklad:%nC:\APP
InvalidDrive=Vybranı disk neexistuje. Prosím, vyberte inı.
DiskSpaceWarningTitle=Na disku nie je dostatok miesta.
DiskSpaceWarning=Inštalácia vyaduje najmenej %1 KB vo¾ného miesta, ale na vybranom disku je dostupné len %2 KB.%n%nChcete napriek tomu pokraèova?
DirNameTooLong=Názov zloky alebo jej cesta je príliš dlhá.
InvalidDirName=Toto nie je platnı názov zloky.
BadDirName32=Názov adresára nemôe obsahova iaden z nasledujícich znakov:%n%n%1
DirExistsTitle=Adresár u existuje
DirExists=Adresár menom:%n%n%1%n%nu existuje. Chcete napriek tomu inštalova do tohoto adresára?
DirDoesntExistTitle=Adresár neexistuje
DirDoesntExist=Adresár menom:%n%n%1%n%nneexistuje. Chcete tento adresár vytvori?

; *** "Select Components" wizard page
WizardSelectComponents=Vyberte komponenty ktoré sa budú inštalova
SelectComponentsDesc=Ktoré komponenty majú by nainštalované?
SelectComponentsLabel2=Oznaète si komponenty, ktoré chcete inštalova resp. odznaète tie, ktoré inštalova nechcete.
FullInstallation=Úplná inštalácia
; if possible don't translate 'Compact' as 'Minimal' (I mean 'Minimal' in your language)
CompactInstallation=Kompaktná inštalácia
CustomInstallation=Volite¾ná inštalácia
NoUninstallWarningTitle=Existujúce komponenty
NoUninstallWarning=Instalaènı program zistil tieto nainštalované komponenty:%n%n%1%n%nZrušením vıberu nebudú odinštalované.%n%nNaozaj chcete pokraèova?
ComponentSize1=%1 KB
ComponentSize2=%1 MB
ComponentsDiskSpaceMBLabel=Aktuálny vıber si vyaduje [mb] prázdneho miesta na disku.

; *** "Select Additional Tasks" wizard page
WizardSelectTasks=Vybra prídavné úlohy
SelectTasksDesc=Ktoré prídavné úlohy sa majú vykona?
SelectTasksLabel2=Zvo¾te, ktoré prídavné úlohy sa majú uskutoèni pri inštalácii "[name]", predtım ako budete pokraèova.

; *** "Select Start Menu Folder" wizard page
WizardSelectProgramGroup=Zvo¾te si programovú skupinu
SelectStartMenuFolderDesc=Kde má inštalátor umiestni ú skupinu?
SelectStartMenuFolderLabel3=Zástupci aplikácie budú vytvorené v následujúcej zloke ponuky Štart.
SelectStartMenuFolderBrowseLabel=Klepnite na Ïalší pre pokraèovanie. Pokia¾ chcete zvoli inú zloku, klepnite na Prechádza.

NoIconsCheck=&Nevytvára iadne ikony
MustEnterGroupName=Musíte zada názov programovej skupiny.
GroupNameTooLong=Názov zloky alebo jej cesta je príliš dlhá.
InvalidGroupName=Toto nie je platnı názov zloky.
BadGroupName=Názov skupiny nemôe obsahova iaden z nasledujúcich znakov:%n%n%1
NoProgramGroupCheck2=N&evytvára programovovú skupinu

; *** "Ready to Install" wizard page
WizardReady=Inštalácia je pripravená
ReadyLabel1=Teraz sa bude inštalova aplikácia "[name]" do Vášho poèítaèa.
ReadyLabel2a=Pokraèujte kliknutím na "Inštalova" alebo kliknite na "Návrat" pre opätovnú zmenu nastavenia.
ReadyLabel2b=V inštalácii pokraèujte kliknutím na "Inštalova".
ReadyMemoUserInfo=Informácie o uívate¾ovy:
ReadyMemoDir=Cie¾ovı adresár:
ReadyMemoType=Typ inštalácie:
ReadyMemoComponents=Vybrané komponenty:
ReadyMemoGroup=Programová skupina:
ReadyMemoTasks=Ïalšie úlohy:

; *** "Preparing to Install" wizard page
WizardPreparing=Príprava na in¾taláciu
PreparingDesc=Inštalaènı program sa pripravuje na inštaláciu aplikácie "[name]" na Váš poèítaè.
PreviousInstallNotCompleted=Inštalácia/odinštalácia predchádzajúcej verzie nebola kompletne dokonèená. Je potrebnı reštart poèítaa.%n%nPo reštarte poèítaèa spustite znovu inštalaènı program pre dokonèenie inštalácie aplikácie "[name]".
CannotContinue=Inštalaènı program nemôe pokraèova. Prosím, kliknite na "Storno" pre ukonèenie inštalácie.

; *** "Installing" wizard page
WizardInstalling=Stav inštalácie
InstallingLabel=Prosím poèkajte, pokia¾ sa nedokonèí inštalácia aplikácie "[name]" na Váš poèítaè.

; *** "Setup Completed" wizard page
FinishedHeadingLabel=Dokonèujem inštaláciu [name]
FinishedLabelNoIcons=Inštalácia aplikácie "[name]" do Vášho poèítaèa bola dokonèená.
FinishedLabel=Inštalácia aplikácie "[name]" do Vášho poèítaèa bola dokonèená. Aplikácia môe by spustená pomocou pripravenıch ikon.
ClickFinish=Kliknite na "Dokonèi" pre ukonèenie inštalátora.
FinishedRestartLabel=K dokonèeniu instalácie aplikácie "[name]" je nutné reštartova Váš poèítaè. Chcete reštartova poèítaè teraz?
FinishedRestartMessage=K dokonèeniu instalácie aplikácie "[name]" sa musí reštartova Váš poèítaè.%n%nChcete reštartova poèítaè teraz?
ShowReadmeCheck=Áno, chcem vidie súbor README.
YesRadio=Áno, &reštartova poèítaè hneï
NoRadio=&Nie, budem poèítaè reštartova neskôr
; used for example as 'Run MyProg.exe'
RunEntryExec=Spusti %1
; used for example as 'View Readme.txt'
RunEntryShellExec=Zobrazi %1

; *** "Setup Needs the Next Disk" stuff
ChangeDiskTitle=Inštalácia vyaduje ïalšiu disketu
SelectDiskLabel2=Prosím, vlote Disk %1 a kliknite OK.%n%nPokia¾ môu by súbory na tomto disku ale v inom %2 ako niššie uvedenom, zadajte správnu cestu alebo kliknite na Nalistova.
PathLabel=&Cesta:
FileNotInDir2=Súbor "%1" sa nenachádza v "%2". Prosím vlote správny disk alebo si vyberte inı %3.
SelectDirectoryLabel=Prosím, zadajte umiestnenie ïalšieho disku.

; *** Installation phase messages
SetupAborted=Inštalácia nebola dokonèená.%n%nProsím, odstráòte problém a spustite inštaláciu znovu.
EntryAbortRetryIgnore=Kliknite "Znovu" pre opakovanie, "Ignoruj" pre pokraèovanie alebo "Storno" k ukonèeniu inštalácie.

; *** Installation status messages
StatusCreateDirs=Vytváram adresáre...
StatusExtractFiles=Extrahujem súbory...
StatusCreateIcons=Vytváram ikony programov...
StatusCreateIniEntries=Vytváram záznamy v INI...
StatusCreateRegistryEntries=Vytváram záznamy v registroch...
StatusRegisterFiles=Registrujem súbory...
StatusSavingUninstall=Ukladám informácie k odinštalovaniu...
StatusRunProgram=Ukonèujem inštaláciu...
StatusRollback=Vraciam zmeny do pôvodného stavu...

; *** Misc. errors
ErrorInternal2=Vnútorná chyba %1
ErrorFunctionFailedNoCode=%1 zlyhal
ErrorFunctionFailed=%1 zlyhal; kód %2
ErrorFunctionFailedWithMessage=%1 zlyhal; kód %2.%n%3
ErrorExecutingProgram=Nie je moné spusti súbor:%n%1

; *** Registry errors
ErrorRegOpenKey=Chyba pri otváraní registrov:%n%1\%2
ErrorRegCreateKey=Chyba pri vytváraní registrov:%n%1\%2
ErrorRegWriteKey=Chyba pri zápise do registrov:%n%1\%2

; *** INI errors
ErrorIniEntry=Chyba pri vytváraní INI záznamu v súbore %1.

; *** File copying errors
FileAbortRetryIgnore=Kliknite Znovu pre opakovanie, Ignoruj pre vynechanie tohoto súboru (nedoporuèuje sa) alebo Storno k ukonèeniu inštalácie.
FileAbortRetryIgnore2=Kliknite Znovu pre opakovanie, Ignoruj k pokraèovaniu (nedoporuèuje sa) alebo Storno k ukonèeniu inštalácie.
SourceIsCorrupted=Zdrojovı súbor je porušenı
SourceDoesntExist=Zdrojovı súbor "%1" neexistuje
ExistingFileReadOnly=Existujúci súbor je oznaèenı ako read-only.%n%nKliknite Znovu pre odstránenie atribútu read-only a novému opakovaniu, Ignoruj pre vynechanie tohoto súboru, alebo Storno k ukonèeniu inštalácie.
ErrorReadingExistingDest=Došlo k chybe pri pokuse preèíta u existujúci súbor:
FileExists=Súbor u existuje.%n%nChcete aby ho inštalácia prepísala?
ExistingFileNewer=Pôvodnı súbor je novší ako ten, ktorı sa bude inštalova. Doporuèuje sa zachova pôvodnı súbor.%n%nChcete zachova pôvodnı súbor?
ErrorChangingAttr=Došlo ku chybe pri pokuse zmeni atribúty existujúceho súboru:
ErrorCreatingTemp=Došlo ku chybe pri pokuse vytvori súbor v cie¾ovom adresári:
ErrorReadingSource=Došlo ku chybe pri pokuse preèíta zdrojovı súbor:
ErrorCopying=Došlo ku chybe pri pokuse kopírova súbor:
ErrorReplacingExistingFile=Došlo k chybe pri pokuse nahradi existujúci súbor:
ErrorRestartReplace=RestartReplace zlyhal:
ErrorRenamingTemp=Došlo ku chybe pri pokuse premenova súbor v cie¾ovom adresári:
ErrorRegisterServer=Nemôem zaregistrova kninicu DLL/OCX: %1
ErrorRegisterServerMissingExport=DllRegisterServer export nebol nájdenı
ErrorRegisterTypeLib=Nemôem zaregistrova typ kniznice: %1

; *** Post-installation errors
ErrorOpeningReadme=Vyskytla sa chyba pri pokuse otvori README súbor.
ErrorRestartingComputer=Inštalátor nemohol reštartova poèítaè. Prosím, reštartujte ho manuálne.

; *** Uninstaller messages
UninstallNotFound=Súbor "%1" neexistuje. Nemôem ho odinštalova.
UninstallOpenError=Súbor "%1" sa nedá otvori. Produkt sa nedá odinštalova.
UninstallUnsupportedVer=Táto verzia odinštalátora nevie rozpozna odinštalaènı log súbor "%1". Nemôem odinštalova.
UninstallUnknownEntry=Neznámy vstup (%1) odinštalaèného log súboru - je neoèíslovanı alebo chybnı.
ConfirmUninstall=Ste si istı, e chcete odstráni aplikáciu "%1" vrátane všetkıch nainštalovanıch súèastí?
OnlyAdminCanUninstall=Táto inštalácia aplikácie môe by odinštalovaná len správcom - administrátorom.
UninstallStatusLabel=Èakajte prosím kım sa "%1" odstráni z Vášho poèítaèa.
UninstalledAll=Aplikácia "%1" bola úspešne odstránená z Vášho poèítaèa.
UninstalledMost=Odinštalovanie aplikácie "%1" je dokonèené.%n%nNiektoré èasti nebolo moné odstráni. Môete ich odstráni manuálne.
UninstalledAndNeedsRestart=Pre dokonèenie odinštalácie aplikácie "%1" je potrebnı reštart poèítaèa.%n%nPrajete si vykona re¾štart teraz?
UninstallDataCorrupted=Súbor "%1" je porušenı. Odinštalovanie nie je moné uskutoèni.

; *** Uninstallation phase messages
ConfirmDeleteSharedFileTitle=Odstráni zdie¾anı súbor?
ConfirmDeleteSharedFile2=Systém ukazuje, e nasledujúci zdie¾anı súbor u nie je ïalej pouívanı iadnım programom. Chcete odinštalova tento zdie¾anı súbor?%n%n%1%n%nPokia¾ niektoré aplikácie tento súbor pouívajú, po jeho odstránení nemusia pracova správne. Pokia¾ nie ste si istı, vyberte "Nie". Ponechanie súboru v systéme nevyvolá iadnu škodu.
SharedFileNameLabel=Meno súboru:
SharedFileLocationLabel=Umiestnenie:
WizardUninstalling=Stav odinštalovania
StatusUninstalling=Odinštalovanie %1...


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
PortugueseLanguage=Portuguese (Brazillian) menus and dialogs
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