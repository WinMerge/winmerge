; *** Inno Setup version 1.2.0 CZECH messages ***
;
;  TRANSLATION TO CZECH:
;  (c) Josef Planeta, planeta@iach.cz
;
;  Pokud bude nekdo provadet upravy/opravy tohoto prekladu, at mi prosim posle
;  emailem opravenou verzi.
;  20.9.1999: Dve drobne zmeny dle skacel@stereo.cz
;  22.9.1999: Upgrade prekladu na verzi 1.12beta10
;  27.9.1999: Upgrade prekladu na verzi 1.2.0 [a vyssi]
;  15.11.1999: Zmena v ConfirmUninstall dle skacel@stereo.cz
;  17.4.2000: Provedeny (temer vsechny) zmeny, ktere navrhl Vaclav Slavik <v.slavik@volny.cz>


[Messages]

; *** Application titles

SetupAppTitle=Instalace
SetupWindowTitle=Instalace - %1
UninstallAppTitle=Odinstalování
UninstallAppFullTitle=%1 - Odinstalování

; *** Icons
DefaultUninstallIconName=%1 - Odinstalování

; *** Misc. common

InformationTitle=Informace
ConfirmTitle=Potvrzení
ErrorTitle=Chyba
DirectoryOld=adresáø
DirectoryNew=sloku
ProgramManagerOld=Program Manager
ProgramManagerNew=Start Menu

; *** SetupLdr messages

SetupLdrStartupMessage=Instalace programu %1. Pøejete si pokraèovat?
LdrCannotCreateTemp=Nelze vytvoøit doèasnı soubor. Instalace se ukonèí
LdrCannotExecTemp=Nelze spustit soubor v doèasném adresáøi. Instalace se ukonèí


; *** Startup error messages

LastErrorMessage=%1.%n%nChyba %2: %3
SetupFileMissing=Soubor %1 chybí v instalaèním adresáøi. Prosím, odstraòte problém nebo si opatøete novou kopii programu.
SetupFileCorrupt=Instalaèní soubory jsou porušeny. Prosím, opatøete si novou kopii programu.
SetupFileCorruptOrWrongVer=Instalaèní soubory jsou porušeny nebo se nesluèují s touto verzí Instalace. Prosím, odstraòte problém nebo si opatøete novou kopii programu.
NotOnThisPlatform=Tento program nelze spustit pod %1.
OnlyOnThisPlatform=Tento program musí bıt spuštìn pod %1.
WinVersionTooLowError=Tento program vyaduje %1 verze %2 nebo vyšší.
WinVersionTooHighError=Tento program nemùe bıt nainstalován na %1 verze %2 nebo vyšší.
AdminPrivilegesRequired=K instalaci tohoto programu musíte bıt pøihlášen jako administrátor.


; *** Misc. errors

ErrorCreatingDir=Instalaèní program nemohl vytvoøit adresáø "%1"
ErrorTooManyFilesInDir=Nelze vytvoøit soubor v adresáøi "%1" protoe obsahuje pøíliš mnoho souborù
ErrorThunk=Thunk selhal; code %1.


; *** Setup common messages

ExitSetupTitle=Ukonèení instalace
ExitSetupMessage=Instalace nebyla dokonèena. Pokud nyní skonèíte, program nebude nainstalován.%n%nInstalaèní program mùete spustit jindy k dokonèení instalace.%n%nUkonèit instalaci?
AboutSetupMenuItem=O progr&amu...
AboutSetupTitle=O instalaci
AboutSetupMessage=%1 verze %2%n%3%n%n%1 domovská stránka:%n%4
AboutSetupNote=


; *** Buttons

ButtonBack=< &Zpìt
ButtonNext=&Další >
ButtonInstall=&Nainstalovat
ButtonOK=OK
ButtonCancel=Storno
ButtonYes=&Ano
ButtonNo=&Ne
ButtonFinish=&Dokonèit
ButtonBrowse=&Procházet...


; *** Common wizard text

ClickNext=Další pro pokraèování, Storno ukonèí instalaci.


; *** "Welcome" wizard page

WizardWelcome=Vítejte
WelcomeLabel=Vítejte v instalaèním programu pro [name]. Program nainstaluje [name/ver] na váš poèítaè.%n%nNe budete pokraèovat, doporuèujeme uzavøít všechny spuštìné aplikace. Pøedejdete tím monım konfliktùm bìhem instalace.

; *** "Password" wizard page

WizardPassword=Heslo
PasswordLabel=Tato instalace je chránìna heslem. Prosím zadejte heslo.%n%nHesla rozlišují velké a malé znaky.
PasswordEditLabel=&Heslo:
IncorrectPassword=Zadané heslo není správné. Prosím zadejte jej znovu.


; *** "License Agreement" wizard page

WizardLicense=Licenèní ujednání
LicenseLabel1=Prosíme pøeètìte si pozornì následující licenèní ujednání. K zobrazení celého textu pouijte posuvnou lištu nebo klávesu Page Down.
LicenseLabel2=Souhlasíte se všemi èástmi licenèního ujednání? Pokud zvolíte Ne, instalace se ukonèí. Abyste mohli nainstalovat [name] musíte souhlasit s tímto ujednáním.


; *** "Information" wizard pages

WizardInfoBefore=Informace
InfoBeforeLabel=Pøed tím ne budete pokraèovat, ètìte prosím nejprve následující dùleitou informaci.
InfoBeforeClickLabel=Stiskem Další pokraèujte v instalaci.
WizardInfoAfter=Informace
InfoAfterLabel=Pøed tím ne budete pokraèovat, pøeètìte si prosím nejprve následující dùleitou informaci.
InfoAfterClickLabel=Stiskem Další pokraèujte v instalaci.

; *** "Select Destination Directory" wizard page

WizardSelectDir=Vyberte cílovı adresáø

; the %1 below is changed to either DirectoryOld or DirectoryNew
; depending on whether the user is running Windows 3.x, or 95 or NT 4.0

SelectFolderLabel=Vyberte %1 kam chcete nainstalovat [name] :
DiskSpaceMBLabel=Aplikace vyaduje nejménì [mb] MB místa na disku.
ToUNCPathname=Nelze instalovat do cesty UNC. Pokud se pokoušíte instalovat do sítì, musíte namapovat síovı disk.
InvalidPath=Musíte zadat celou cestu s písmenem disku, napøíklad:%nC:\APP
InvalidDrive=Vybranı disk neexistuje. Prosím, vyberte jinı.
PathTooLong=Èást zadané cesty obsahuje pøíliš mnoho znakù. Platnı název adresáøe nesmí pøesáhnout délku 8 znakù, ale mùe té zahrnovat pøíponu tøí znakù.
DiskSpaceWarningTitle=Na disku není dost místa
DiskSpaceWarning=Instalace vyaduje nejménì %1 KB volného místa, ale na vybraném disku je dostupné pouze %2 KB.%n%nChcete pøesto pokraèovat?
BadDirName32=Název adresáøe nemùe obsahovat ádnı z následujících znakù:%n%n%1
BadDirName16=Název adresáøe nemùe obsahovat mezery nebo nìkterı z následujících znakù:%n%n%1
DirExistsTitle=Adresáø ji existuje
DirExists=Adresáø %1 ji existuje.%n%nChcete pøesto do tohoto adresáøe instalovat?
DirDoesntExistTitle=Adresáø neexistuje
DirDoesntExist=Adresáø:%n%n%1%n%nneexistuje. Chcete aby byl adresáø vytvoøen?


; *** "Select Program Group" wizard page

WizardSelectProgramGroup=Vyberte programovou skupinu

; the %1 below is changed to either ProgramManagerOld or ProgramManagerNew
; depending on whether the user is running Windows 3.x, or 95 or NT 4.0

IconsLabel=Instalace pøidá ikony programù do programové skupiny %1.
NoIconsCheck=Nevytváøet ádné ikony
MustEnterGroupName=Musíte zadat název skupiny.
BadGroupName=Název skupiny nemùe obsahovat ádnı z následujících znakù:%n%n%1


; *** "Ready to Install" wizard page

WizardReady=Instalace je pøipravena
ReadyLabel1=Nyní se [name] nainstaluje na váš poèítaè...
ReadyLabel2a=Pokraèujte kliknutím na Instalovat nebo kliknìte Zpìt, pokud chcete zmìnit nastavení.
ReadyLabel2b=V instalaci pokraèujte klikem na Instalovat.


; *** "Setup Completed" wizard page

WizardFinished=Instalace byla dokonèena
FinishedLabelNoIcons=Instalace aplikace [name] na váš poèítaè byla dokonèena.
FinishedLabel=Instalace aplikace [name] na váš poèítaè byla dokonèena. Aplikace mùete spustit pomocí nainstalovanıch ikon.
ClickFinish=Kliknutím na Dokonèit dokonèíte instalaci.
FinishedRestartLabel=K dokonèení instalace [name] se musí restartovat váš poèítaè. Chcete nyní provést restart?
ShowReadmeCheck=Ano, chci vidìt soubor README
YesRadio=&Ano, restartovat poèítaè hned
NoRadio=&Ne, restartuji poèítaè pozdìji


; *** "Setup Needs the Next Disk" stuff

ChangeDiskTitle=Instalace vyaduje další disketu
SelectDirectory=Vyberte adresáø

; the %2 below is changed to either SDirectoryOld or SDirectoryNew
; depending on whether the user is running Windows 3.x, or 95 or NT 4.0

SelectDiskLabel=Prosím, vlote Disk %1 a kliknìte OK.%n%nPokud mohou bıt soubory na tomto disku nalezeny v jiném %2 ne v níe uvedeném, zadejte správnou cestu nebo kliknìte na Nalistovat.
PathLabel=&Cesta:

; the %3 below is changed to either SDirectoryOld or SDirectoryNew
; depending on whether the user is running Windows 3.x, or 95 or NT 4.0

FileNotInDir=Soubor "%1" nelze najít v "%2". Prosím vlote správnı disk nebo vyberte jinı %3.
SelectDirectoryLabel=Prosím, zadejte umístìní dalšího disku.

; *** Installation phase messages

SetupAborted=Instalace nebyla dokonèena.%n%nProsím, odstraòte problém a spuste instalaci znovu.
EntryAbortRetryIgnore=Kliknìte Znovu pro opakování, Ignoruj pro pokraèování nebo Storno k ukonèení instalace.


; *** Installation status messages

StatusCreateDirs=Vytváøím adresáøe...
StatusExtractFiles=Rozbaluji soubory...
StatusCreateIcons=Vytváøím ikony programù...
StatusCreateIniEntries=Vytváøím záznamy v INI...
StatusCreateRegistryEntries=Vytváøím záznamy v registrech...
StatusRegisterFiles=Registruji soubory...
StatusSavingUninstall=Ukládám informace k odinstalování...


; *** Misc. errors

ErrorInternal=Vnitøní chyba %1
ErrorFunctionFailedNoCode=%1 selhal
ErrorFunctionFailed=%1 selhal; code %2
ErrorFunctionFailedWithMessage=%1 selhal; code %2.%n%3
ErrorExecutingProgram=Není moné spustit soubor:%n%1


; *** DDE errors

ErrorDDEExecute=DDE: DDE: Chyba bìhem provádìní transakce (code: %1)
ErrorDDECommandFailed=DDE: Pøíkaz nebyl úspìšnı
ErrorDDERequest=DDE: Chyba bìhem poadavku na transakci (code: %1)


; *** Registry errors

ErrorRegOpenKey=Chyba pøi otevírání registrù:%n%1\%2
ErrorRegCreateKey=Chyba pøi vytváøení registrù:%n%1\%2
ErrorRegWriteKey=Chyba pøi zápisu do registrù:%n%1\%2


; *** INI errors

ErrorIniEntry=Chyba pøi vytváøení INI záznamu v souboru %1.


; *** File copying errors

FileAbortRetryIgnore=Kliknìte na Znovu pro opakování, Ignoruj pro vynechání tohoto souboru (nedoporuèuje se) nebo Storno k ukonèení instalace.
FileAbortRetryIgnore2=Kliknìte na Znovu pro opakování, Ignoruj k pokraèování (nedoporuèuje se) nebo Storno k ukonèení instalace.
SourceIsCorrupted=Zdrojovı soubor je porušen
SourceDoesntExist=Zdrojovı soubor "%1" neexistuje
ExistingFileReadOnly=Existující soubor je oznaèen jako read-only.%n%nKliknìte Znovu pro odstranìní atributu read-only a novému opakování, Ignoruj pro vynechání tohoto souboru, nebo Storno k ukonèení instalace.
ErrorReadingExistingDest=Došlo k chybì pøi pokusu pøeèíst ji existující soubor:
FileExists=Soubor ji existuje.%n%nChcete aby jej instalace pøepsala?
ExistingFileNewer=Pùvodní soubor je novìjší ne ten, kterı se bude instalovat. Doporuèuje se zachovat pùvodní soubor.%n%nChcete zachovat pùvodní soubor?
ErrorChangingAttr=Došlo k chybì pøi pokusu zmìnit atributy existujícího souboru:
ErrorCreatingTemp=Došlo k chybì pøi pokusu vytvoøit soubor v cílovém adresáøi:
ErrorReadingSource=Došlo k chybì pøi pokusu pøeèíst zdrojovı soubor:
ErrorCopying=Došlo k chybì pøi pokusu kopírovat soubor:
ErrorReplacingExistingFile=Došlo k chybì pøi pokusu nahradit existující soubor:
ErrorRestartReplace=RestartReplace selhal:
ErrorRenamingTemp=Došlo k chybì pøi pokusu pøejmenovat soubor v cílovém adresáøi:
ErrorRegisterServer=Nelze zaregistrovat DLL/OCX: %1
ErrorRegisterServerMissingExport=DllRegisterServer export nebyl nalezen
ErrorRegisterTypeLib=Nelze zaregistrovat typ knihovny: %1


; *** Post-installation errors

ErrorOpeningReadme=Vyskytla se chyba pøi pokusu otevøít README soubor.
ErrorRestartingComputer=Setup nemohl restartovat poèítaè. Prosím, restartujte manuálnì.


; *** Uninstaller messages

UninstallNotFound=Soubor "%1" neexistuje. Nelze odinstalovat.
ConfirmUninstall=Jste si jist(a), e chcete odstranit %1 vèetnì všech souèástí?
OnlyAdminCanUninstall=Tato instalace mùe bıt odinstalována pouze uivatelem - administrátorem.
UninstalledAll=Program %1 byl úspìšnì odstranìn z vašeho poèítaèe.
UninstalledMost=Odinstalování %1 je dokonèeno.%n%nNìkteré èásti nemohly bıt odstranìny. Mohou bıt odstranìny manuálnì.
UninstallDataCorrupted=Soubor "%1" je porušen. Nelze odinstalovat


; *** Uninstallation phase messages

ConfirmDeleteSharedFileTitle=Odstranit sdílenı soubor?
ConfirmDeleteSharedFile=Systém ukazuje, e následující sdílenı soubor ji není dále pouíván ádnımi programy. Chcete odinstalovat tento sdílenı soubor?%n%n%1%n%nPokud nìkteré programy tento soubor pouívají, po jeho odstranìní nemusí pracovat správnì. Pokud si nejste jist, vyberte Ne. Ponechání souboru v systému nevyvolá ádnou škodu.


; The custom messages below aren't used by Setup itself, but if you make
; use of them in your scripts, you'll want to translate them.

[CustomMessages]
;Inno Setup Built-in Custom Messages
AdditionalIcons=Additional Icons:
CreateDesktopIcon=Create a &Desktop Icon
CreateQuickLaunchIcon=Create a &Quick Launch Icon
ProgramOnTheWeb=%1 on the Web
UninstallProgram=Uninstall %1
LaunchProgram=&Launch %1
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
AppCoreFile=%1 Core Files
ApplicationRuntimes=Application Runtimes
UsersGuide=User's Guide
Filters=Filters
Plugins=Plugins (enhance core behavior)

;Localization Components
BulgarianLanguage=Bulgarian menus and dialogs
CatalanLanguage=Catalan menus and dialogs
ChineseSimplifiedLanguage=Chinese(Simplified) menus and dialogs
ChineseTraditionalLanguage=Chinese (Traditional) menus and dialogs
CzechLanguage=Czech menus and dialogs
DanishLanguage=Danish menus and dialogs
DutchLanguage=Dutch menus and dialogs
FrenchLanguage=French menus and dialogs
GermanLanguage=German menus and dialogs
ItalianLanguage=Italian menus and dialogs
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
ViewStartMenuFolder=&View the %1 Start Menu Folder

;Things we can also localize
CompanyName=Thingamahoochie Software

;Types
TypicalInstallation=Typical Installation
FullInstallation=Full Installation
CompactInstallation=Compact Installation
CustomInstallation=Custom Installation

;Components
AppCoreFile=%1 Core Files
ApplicationRuntimes=Application Runtimes
UsersGuide=User's Guide
Filters=Filters
Plugins=Plugins (enhance core behavior)

;Localization Components
BulgarianLanguage=Bulgarian menus and dialogs
CatalanLanguage=Catalan menus and dialogs
ChineseSimplifiedLanguage=Chinese(Simplified) menus and dialogs
ChineseTraditionalLanguage=Chinese (Traditional) menus and dialogs
CzechLanguage=Czech menus and dialogs
DanishLanguage=Danish menus and dialogs
DutchLanguage=Dutch menus and dialogs
FrenchLanguage=French menus and dialogs
GermanLanguage=German menus and dialogs
ItalianLanguage=Italian menus and dialogs
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
ViewStartMenuFolder=&View the %1 Start Menu Folder