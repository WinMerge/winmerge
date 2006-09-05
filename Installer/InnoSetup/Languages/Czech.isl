; ******************************************************
; ***                                                ***
; *** Inno Setup version 5.1.0+ Czech messages       ***
; ***                                                ***
; *** Original Author:                               ***
; ***                                                ***
; ***   Ing. Ivo Bauer (bauer@ozm.cz)                ***
; ***                                                ***
; *** Contributors:                                  ***
; ***                                                ***
; ***   Lubos Stanek (lubek@users.sourceforge.net)   ***
; ***                                                ***
; ******************************************************
;
; $jrsoftware: issrc/Files/Languages/Czech.isl,v 1.16 2005/02/28 14:18:39 ibauer Exp $

[LangOptions]
LanguageName=<010C>e<0161>tina
LanguageID=$0405
LanguageCodePage=1250

[Messages]

; *** Application titles
SetupAppTitle=Prùvodce instalací
SetupWindowTitle=Prùvodce instalací - %1
UninstallAppTitle=Prùvodce odinstalací
UninstallAppFullTitle=Prùvodce odinstalací - %1

; *** Misc. common
InformationTitle=Informace
ConfirmTitle=Potvrzení
ErrorTitle=Chyba

; *** SetupLdr messages
SetupLdrStartupMessage=Vítá Vás prùvodce instalací produktu %1. Chcete pokraèovat?
LdrCannotCreateTemp=Nelze vytvoøit doèasnı soubor. Prùvodce instalací bude ukonèen
LdrCannotExecTemp=Nelze spustit soubor v doèasné sloce. Prùvodce instalací bude ukonèen

; *** Startup error messages
LastErrorMessage=%1.%n%nChyba %2: %3
SetupFileMissing=Instalaèní sloka neobsahuje soubor %1. Opravte prosím tuto chybu nebo si poøiïte novou kopii tohoto produktu.
SetupFileCorrupt=Soubory prùvodce instalací jsou poškozeny. Poøiïte si prosím novou kopii tohoto produktu.
SetupFileCorruptOrWrongVer=Soubory prùvodce instalací jsou poškozeny nebo se nesluèují s touto verzí prùvodce instalací. Opravte prosím tuto chybu nebo si poøiïte novou kopii tohoto produktu.
NotOnThisPlatform=Tento produkt nelze spustit ve %1.
OnlyOnThisPlatform=Tento produkt musí bıt spuštìn ve %1.
OnlyOnTheseArchitectures=Tento produkt lze nainstalovat pouze ve verzích MS Windows s podporou architektury procesorù:%n%n%1
MissingWOW64APIs=Aktuální verze MS Windows postrádá funkce, které vyaduje prùvodce instalací pro 64-bitovou instalaci. Opravte prosím tuto chybu nainstalováním aktualizace Service Pack %1.
WinVersionTooLowError=Tento produkt vyaduje %1 verzi %2 nebo vyšší.
WinVersionTooHighError=Tento produkt nelze nainstalovat ve %1 verzi %2 nebo vyšší.
AdminPrivilegesRequired=K instalaci tohoto produktu musíte bıt pøihlášeni s právy administrátora.
PowerUserPrivilegesRequired=K instalaci tohoto produktu musíte bıt pøihlášeni s právy administrátora nebo èlena skupiny Power Users.
SetupAppRunningError=Prùvodce instalací zjistil, e produkt %1 je nyní spuštìn.%n%nUkonèete prosím všechny spuštìné instance tohoto produktu a pokraèujte klepnutím na tlaèítko OK nebo ukonèete instalaci tlaèítkem Storno.
UninstallAppRunningError=Prùvodce odinstalací zjistil, e produkt %1 je nyní spuštìn.%n%nUkonèete prosím všechny spuštìné instance tohoto produktu a pokraèujte klepnutím na tlaèítko OK nebo ukonèete odinstalaci tlaèítkem Storno.

; *** Misc. errors
ErrorCreatingDir=Prùvodce instalací nemohl vytvoøit sloku "%1"
ErrorTooManyFilesInDir=Nelze vytvoøit soubor ve sloce "%1", protoe tato sloka ji obsahuje pøíliš mnoho souborù

; *** Setup common messages
ExitSetupTitle=Ukonèit prùvodce instalací
ExitSetupMessage=Instalace nebyla zcela dokonèena. Jestlie nyní ukonèíte prùvodce instalací, produkt nebude nainstalován.%n%nPrùvodce instalací mùete znovu spustit kdykoliv jindy a dokonèit instalaci.%n%nChcete ukonèit prùvodce instalací?
AboutSetupMenuItem=&O prùvodci instalací...
AboutSetupTitle=O prùvodci instalací
AboutSetupMessage=%1 verze %2%n%3%n%n%1 domovská stránka:%n%4
AboutSetupNote=
TranslatorNote=Czech translation maintained by Ing. Ivo Bauer (bauer@ozm.cz) and Lubos Stanek (lubek@users.sourceforge.net)

; *** Buttons
ButtonBack=< &Zpìt
ButtonNext=&Další >
ButtonInstall=&Instalovat
ButtonOK=OK
ButtonCancel=Storno
ButtonYes=&Ano
ButtonYesToAll=Ano &všem
ButtonNo=&Ne
ButtonNoToAll=N&e všem
ButtonFinish=&Dokonèit
ButtonBrowse=&Procházet...
ButtonWizardBrowse=&Procházet...
ButtonNewFolder=&Vytvoøit novou sloku

; *** "Select Language" dialog messages
SelectLanguageTitle=Vıbìr jazyka prùvodce instalací
SelectLanguageLabel=Zvolte jazyk, kterı se má pouít pøi instalaci:

; *** Common wizard text
ClickNext=Pokraèujte klepnutím na tlaèítko Další nebo ukonèete prùvodce instalací tlaèítkem Storno.
BeveledLabel=
BrowseDialogTitle=Vyhledat sloku
BrowseDialogLabel=Z níe uvedeného seznamu vyberte sloku a klepnìte na OK.
NewFolderName=Nová sloka

; *** "Welcome" wizard page
WelcomeLabel1=Vítá Vás prùvodce instalací produktu [name].
WelcomeLabel2=Produkt [name/ver] bude nainstalován na Váš poèítaè.%n%nDøíve ne budete pokraèovat, doporuèuje se ukonèit veškeré spuštìné aplikace.

; *** "Password" wizard page
WizardPassword=Heslo
PasswordLabel1=Tato instalace je chránìna heslem.
PasswordLabel3=Zadejte prosím heslo a pokraèujte klepnutím na tlaèítko Další. Pøi zadávání hesla rozlišujte malá a velká písmena.
PasswordEditLabel=&Heslo:
IncorrectPassword=Zadané heslo není správné. Zkuste to prosím znovu.

; *** "License Agreement" wizard page
WizardLicense=Licenèní smlouva
LicenseLabel=Døíve ne budete pokraèovat, pøeètìte si prosím pozornì následující dùleité informace.
LicenseLabel3=Pøeètìte si prosím tuto licenèní smlouvu. Musíte souhlasit s podmínkami této smlouvy, aby instalace mohla pokraèovat.
LicenseAccepted=&Souhlasím s podmínkami licenèní smlouvy
LicenseNotAccepted=&Nesouhlasím s podmínkami licenèní smlouvy

; *** "Information" wizard pages
WizardInfoBefore=Informace
InfoBeforeLabel=Døíve ne budete pokraèovat, pøeètìte si prosím pozornì následující dùleité informace.
InfoBeforeClickLabel=Pokraèujte v instalaci klepnutím na tlaèítko Další.
WizardInfoAfter=Informace
InfoAfterLabel=Døíve ne budete pokraèovat, pøeètìte si prosím pozornì následující dùleité informace.
InfoAfterClickLabel=Pokraèujte v instalaci klepnutím na tlaèítko Další.

; *** "User Information" wizard page
WizardUserInfo=Informace o uivateli
UserInfoDesc=Zadejte prosím poadované údaje.
UserInfoName=&Uivatelské jméno:
UserInfoOrg=&Spoleènost:
UserInfoSerial=Sé&riové èíslo:
UserInfoNameRequired=Uivatelské jméno musí bıt zadáno.

; *** "Select Destination Location" wizard page
WizardSelectDir=Zvolte cílové umístìní
SelectDirDesc=Kam má bıt produkt [name] nainstalován?
SelectDirLabel3=Prùvodce nainstaluje produkt [name] do následující sloky.
SelectDirBrowseLabel=Pokraèujte klepnutím na tlaèítko Další. Chcete-li zvolit jinou sloku, klepnìte na tlaèítko Procházet.
DiskSpaceMBLabel=Instalace vyaduje nejménì [mb] MB volného místa na disku.
ToUNCPathname=Prùvodce instalací nemùe instalovat do cesty UNC. Pokud se pokoušíte instalovat v síti, budete muset pouít nìkterou z dostupnıch síovıch jednotek.
InvalidPath=Musíte zadat úplnou cestu vèetnì písmene jednotky; napøíklad:%n%nC:\Aplikace%n%nnebo cestu UNC ve tvaru:%n%n\\server\sdílená sloka
InvalidDrive=Vámi zvolená jednotka nebo cesta UNC neexistuje nebo není dostupná. Zvolte prosím jiné umístìní.
DiskSpaceWarningTitle=Nedostatek místa na disku
DiskSpaceWarning=Prùvodce instalací vyaduje nejménì %1 KB volného místa pro instalaci produktu, ale na zvolené jednotce je dostupnıch pouze %2 KB.%n%nChcete pøesto pokraèovat?
DirNameTooLong=Název sloky nebo cesta jsou pøíliš dlouhé.
InvalidDirName=Název sloky není platnı.
BadDirName32=Název sloky nemùe obsahovat ádnı z následujících znakù:%n%n%1
DirExistsTitle=Sloka existuje
DirExists=Sloka:%n%n%1%n%nji existuje. Má se pøesto instalovat do této sloky?
DirDoesntExistTitle=Sloka neexistuje
DirDoesntExist=Sloka:%n%n%1%n%nneexistuje. Má bıt tato sloka vytvoøena?

; *** "Select Components" wizard page
WizardSelectComponents=Zvolte souèásti
SelectComponentsDesc=Jaké souèásti mají bıt nainstalovány?
SelectComponentsLabel2=Zaškrtnìte souèásti, které mají bıt nainstalovány; souèásti, které se nemají instalovat, ponechte nezaškrtnuté. Pokraèujte klepnutím na tlaèítko Další.
FullInstallation=Úplná instalace
; if possible don't translate 'Compact' as 'Minimal' (I mean 'Minimal' in your language)
CompactInstallation=Kompaktní instalace
CustomInstallation=Volitelná instalace
NoUninstallWarningTitle=Souèásti existují
NoUninstallWarning=Prùvodce instalací zjistil, e následující souèásti jsou ji na Vašem poèítaèi nainstalovány:%n%n%1%n%nNezahrnete-li tyto souèásti do vıbìru, nebudou nyní odinstalovány.%n%nChcete pøesto pokraèovat?
ComponentSize1=%1 KB
ComponentSize2=%1 MB
ComponentsDiskSpaceMBLabel=Vybrané souèásti vyadují nejménì [mb] MB místa na disku.

; *** "Select Additional Tasks" wizard page
WizardSelectTasks=Zvolte další úlohy
SelectTasksDesc=Které další úlohy mají bıt provedeny?
SelectTasksLabel2=Zvolte další úlohy, které mají bıt provedeny v prùbìhu instalace produktu [name] a pokraèujte klepnutím na tlaèítko Další.

; *** "Select Start Menu Folder" wizard page
WizardSelectProgramGroup=Vyberte sloku v nabídce Start
SelectStartMenuFolderDesc=Kam má prùvodce instalací umístit zástupce aplikace?
SelectStartMenuFolderLabel3=Prùvodce instalací vytvoøí zástupce aplikace v následující sloce nabídky Start.
SelectStartMenuFolderBrowseLabel=Pokraèujte klepnutím na tlaèítko Další. Chcete-li zvolit jinou sloku, klepnìte na tlaèítko Procházet.
MustEnterGroupName=Musíte zadat název sloky.
GroupNameTooLong=Název sloky nebo cesta jsou pøíliš dlouhé.
InvalidGroupName=Název sloky není platnı.
BadGroupName=Název sloky nemùe obsahovat ádnı z následujících znakù:%n%n%1
NoProgramGroupCheck2=&Nevytváøet sloku v nabídce Start

; *** "Ready to Install" wizard page
WizardReady=Instalace je pøipravena
ReadyLabel1=Prùvodce instalací je nyní pøipraven nainstalovat produkt [name] na Váš poèítaè.
ReadyLabel2a=Pokraèujte v instalaci klepnutím na tlaèítko Instalovat. Pøejete-li si zmìnit nìkterá nastavení instalace, klepnìte na tlaèítko Zpìt.
ReadyLabel2b=Pokraèujte v instalaci klepnutím na tlaèítko Instalovat.
ReadyMemoUserInfo=Informace o uivateli:
ReadyMemoDir=Cílové umístìní:
ReadyMemoType=Typ instalace:
ReadyMemoComponents=Vybrané souèásti:
ReadyMemoGroup=Sloka v nabídce Start:
ReadyMemoTasks=Další úlohy:

; *** "Preparing to Install" wizard page
WizardPreparing=Pøíprava k instalaci
PreparingDesc=Prùvodce instalací pøipravuje instalaci produktu [name] na Váš poèítaè.
PreviousInstallNotCompleted=Instalace/odinstalace pøedchozího produktu nebyla zcela dokonèena. Dokonèení tohoto procesu vyaduje restart tohoto poèítaèe.%n%nPo restartování poèítaèe spuste znovu tohoto prùvodce instalací, aby bylo moné dokonèit instalaci produktu [name].
CannotContinue=Prùvodce instalací nemùe pokraèovat. Ukonèete prosím prùvodce instalací klepnutím na tlaèítko Storno.

; *** "Installing" wizard page
WizardInstalling=Instalování
InstallingLabel=Èekejte prosím dokud prùvodce instalací nedokonèí instalaci produktu [name] na Váš poèítaè.

; *** "Setup Completed" wizard page
FinishedHeadingLabel=Dokonèuje se instalace produktu [name]
FinishedLabelNoIcons=Prùvodce instalací dokonèil instalaci produktu [name] na Váš poèítaè.
FinishedLabel=Prùvodce instalací dokonèil instalaci produktu [name] na Váš poèítaè. Produkt lze spustit pomocí nainstalovanıch zástupcù.
ClickFinish=Ukonèete prùvodce instalací klepnutím na tlaèítko Dokonèit.
FinishedRestartLabel=Pro dokonèení instalace produktu [name] je nezbytné, aby prùvodce instalací restartoval Váš poèítaè. Chcete jej nyní restartovat?
FinishedRestartMessage=Pro dokonèení instalace produktu [name] je nezbytné, aby prùvodce instalací restartoval Váš poèítaè.%n%nChcete jej nyní restartovat?
ShowReadmeCheck=Ano, chci zobrazit dokument "ÈTIMNE"
YesRadio=&Ano, chci nyní restartovat poèítaè
NoRadio=&Ne, poèítaè restartuji pozdìji
; used for example as 'Run MyProg.exe'
RunEntryExec=Spustit %1
; used for example as 'View Readme.txt'
RunEntryShellExec=Zobrazit %1

; *** "Setup Needs the Next Disk" stuff
ChangeDiskTitle=Prùvodce instalací vyaduje další disk
SelectDiskLabel2=Vlote prosím disk %1 a klepnìte na OK.%n%nPokud se soubory na tomto disku nacházejí v jiné sloce, ne v té, která je zobrazena níe, pak zadejte správnou cestu nebo ji zvolte klepnutím na tlaèítko Procházet.
PathLabel=&Cesta:
FileNotInDir2=Soubor "%1" nelze najít v "%2". Vlote prosím správnı disk nebo zvolte jinou sloku.
SelectDirectoryLabel=Specifikujte prosím umístìní dalšího disku.

; *** Installation phase messages
SetupAborted=Instalace nebyla zcela dokonèena.%n%nOpravte prosím chybu a spuste prùvodce instalací znovu.
EntryAbortRetryIgnore=Akci zopakujete klepnutím na tlaèítko Opakovat. Akci vynecháte klepnutím na tlaèítko Pøeskoèit. Instalaci stornujete klepnutím na tlaèítko Pøerušit.

; *** Installation status messages
StatusCreateDirs=Vytváøejí se sloky...
StatusExtractFiles=Extrahují se soubory...
StatusCreateIcons=Vytváøejí se zástupci...
StatusCreateIniEntries=Vytváøejí se záznamy v inicializaèních souborech...
StatusCreateRegistryEntries=Vytváøejí se záznamy v systémovém registru...
StatusRegisterFiles=Registrují se soubory...
StatusSavingUninstall=Ukládají se informace pro odinstalaci produktu...
StatusRunProgram=Dokonèuje se instalace...
StatusRollback=Provedené zmìny se vracejí zpìt...

; *** Misc. errors
ErrorInternal2=Interní chyba: %1
ErrorFunctionFailedNoCode=%1 selhala
ErrorFunctionFailed=%1 selhala; kód %2
ErrorFunctionFailedWithMessage=%1 selhala; kód %2.%n%3
ErrorExecutingProgram=Nelze spustit soubor:%n%1

; *** Registry errors
ErrorRegOpenKey=Došlo k chybì pøi otevírání klíèe systémového registru:%n%1\%2
ErrorRegCreateKey=Došlo k chybì pøi vytváøení klíèe systémového registru:%n%1\%2
ErrorRegWriteKey=Došlo k chybì pøi zápisu do klíèe systémového registru:%n%1\%2

; *** INI errors
ErrorIniEntry=Došlo k chybì pøi vytváøení záznamu v inicializaèním souboru "%1".

; *** File copying errors
FileAbortRetryIgnore=Akci zopakujete klepnutím na tlaèítko Opakovat. Tento soubor pøeskoèíte klepnutím na tlaèítko Pøeskoèit (nedoporuèuje se). Instalaci stornujete klepnutím na tlaèítko Pøerušit.
FileAbortRetryIgnore2=Akci zopakujete klepnutím na tlaèítko Opakovat. Pokraèujete klepnutím na tlaèítko Pøeskoèit (nedoporuèuje se). Instalaci stornujete klepnutím na tlaèítko Pøerušit.
SourceIsCorrupted=Zdrojovı soubor je poškozen
SourceDoesntExist=Zdrojovı soubor "%1" neexistuje
ExistingFileReadOnly=Existující soubor je urèen pouze pro ètení.%n%nAtribut "pouze pro ètení" odstraníte a akci zopakujete klepnutím na tlaèítko Opakovat. Tento soubor pøeskoèíte klepnutím na tlaèítko Pøeskoèit. Instalaci stornujete klepnutím na tlaèítko Pøerušit.
ErrorReadingExistingDest=Došlo k chybì pøi pokusu o ètení existujícího souboru:
FileExists=Soubor ji existuje.%n%nMá bıt prùvodcem instalace pøepsán?
ExistingFileNewer=Existující soubor je novìjší ne ten, kterı se prùvodce instalací pokouší nainstalovat. Doporuèuje se ponechat existující soubor.%n%nChcete jej ponechat?
ErrorChangingAttr=Došlo k chybì pøi pokusu o zmìnu atributù existujícího souboru:
ErrorCreatingTemp=Došlo k chybì pøi pokusu o vytvoøení souboru v cílové sloce:
ErrorReadingSource=Došlo k chybì pøi pokusu o ètení zdrojového souboru:
ErrorCopying=Došlo k chybì pøi pokusu o zkopírování souboru:
ErrorReplacingExistingFile=Došlo k chybì pøi pokusu o nahrazení existujícího souboru:
ErrorRestartReplace=Funkce "RestartReplace" prùvodce instalací selhala:
ErrorRenamingTemp=Došlo k chybì pøi pokusu o pøejmenování souboru v cílové sloce:
ErrorRegisterServer=Nelze zaregistrovat DLL/OCX: %1
ErrorRegisterServerMissingExport=Nelze nalézt export DllRegisterServer
ErrorRegisterTypeLib=Nelze zaregistrovat typovou knihovnu: %1

; *** Post-installation errors
ErrorOpeningReadme=Došlo k chybì pøi pokusu o otevøení dokumentu "ÈTIMNE".
ErrorRestartingComputer=Prùvodci instalace se nepodaøilo restartovat Váš poèítaè. Restartujte jej prosím ruènì.

; *** Uninstaller messages
UninstallNotFound=Soubor "%1" neexistuje. Produkt nelze odinstalovat.
UninstallOpenError=Soubor "%1" nelze otevøít. Produkt nelze odinstalovat.
UninstallUnsupportedVer=Prùvodci odinstalací se nepodaøilo rozpoznat formát souboru obsahujícího informace k odinstalaci produktu "%1". Produkt nelze odinstalovat
UninstallUnknownEntry=V souboru obsahujícím informace k odinstalaci produktu byla zjištìna neznámá poloka (%1)
ConfirmUninstall=Jste si opravdu jisti, e chcete odinstalovat produkt %1 a všechny jeho souèásti?
UninstallOnlyOnWin64=Tento produkt lze odinstalovat pouze v 64-bitovıch verzích MS Windows.
OnlyAdminCanUninstall=K odinstalaci tohoto produktu musíte bıt pøihlášeni s právy administrátora.
UninstallStatusLabel=Èekejte prosím dokud produkt %1 nebude odinstalován z Vašeho poèítaèe.
UninstalledAll=Produkt %1 byl úspìšnì odinstalován z Vašeho poèítaèe.
UninstalledMost=Produkt %1 byl odinstalován z Vašeho poèítaèe.%n%nNìkteré jeho souèásti se však nepodaøilo odinstalovat. Ty lze odstranit ruènì.
UninstalledAndNeedsRestart=K dokonèení odinstalace produktu %1 je nezbytné, aby prùvodce odinstalací restartoval Váš poèítaè.%n%nChcete jej nyní restartovat?
UninstallDataCorrupted=Soubor "%1" je poškozen. Produkt nelze odinstalovat

; *** Uninstallation phase messages
ConfirmDeleteSharedFileTitle=Odebrat sdílenı soubor?
ConfirmDeleteSharedFile2=Systém indikuje, e následující sdílenı soubor není pouíván ádnımi jinımi aplikacemi. Má bıt tento sdílenı soubor prùvodcem odinstalací odstranìn?%n%nPokud nìkteré aplikace tento soubor pouívají, pak po jeho odstranìní nemusejí pracovat správnì. Pokud si nejste jisti, zvolte Ne. Ponechání tohoto souboru ve Vašem systému nezpùsobí ádnou škodu.
SharedFileNameLabel=Název souboru:
SharedFileLocationLabel=Umístìní:
WizardUninstalling=Stav odinstalace
StatusUninstalling=Probíhá odinstalace produktu %1...

; The custom messages below aren't used by Setup itself, but if you make
; use of them in your scripts, you'll want to translate them.

[CustomMessages]
;Inno Setup Built-in Custom Messages
NameAndVersion=%1 verze %2
AdditionalIcons=Další zástupci:
OptionalFeatures=Optional Features:
CreateDesktopIcon=Vytvoøit zástupce na &ploše
CreateQuickLaunchIcon=Vytvoøit zástupce na panelu &Snadné spuštìní
ProgramOnTheWeb=Aplikace %1 na internetu
UninstallProgram=Odinstalovat aplikaci %1
LaunchProgram=Spustit aplikaci %1
AssocFileExtension=Vytvoøit &asociaci mezi soubory typu %2 a aplikací %1
AssocingFileExtension=Vytváøí se asociace mezi soubory typu %2 a aplikací %1...

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
Plugins=Plugins

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
HungarianLanguage=Hungarian menus and dialogs
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
TurkishLanguage=Turkish menus and dialogs

;Tasks
ExplorerContextMenu=&Enable Explorer context menu integration
IntegrateTortoiseCVS=Integrate with &TortoiseCVS
IntegrateTortoiseSVN=Integrate with T&ortoiseSVN
IntegrateDelphi4=Borland® Delphi &4 Binary File Support
UpdatingCommonControls=Updating the System's Common Controls

;Icon Labels
ReadMe=Read Me
UsersGuide=User's Guide
UpdatingCommonControls=Updating the System's Common Controls
ViewStartMenuFolder=&View the WinMerge Start Menu Folder
PluginsText=Plugins

;Code Dialogs
DeletePreviousStartMenu=The installer has detected that you changed the location of your start menu from "%s" to "%s". Would you like to delete the previous start menu folder?

; Project file description
ProjectFileDesc=WinMerge Project file
