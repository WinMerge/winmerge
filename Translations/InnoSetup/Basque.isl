; *** Inno Setup version 5.1.11+ Basque messages ***
;
; Translated by 3ARRANO (3arrano@3arrano.com) + EUS_Xabier Aramendi (azpidatziak@gmail.com)
;
; To download user-contributed translations of this file, go to:
;   http://www.jrsoftware.org/is3rdparty.php
;
; Note: When translating this text, do not add periods (.) to the end of
; messages that didn't have them already, because on those messages Inno
; Setup adds the periods automatically (appending a period would result in
; two periods being displayed).

[LangOptions]
; The following three entries are very important. Be sure to read and 
; understand the '[LangOptions] section' topic in the help file.
LanguageName=Euskara
LanguageID=$042d
LanguageCodePage=0
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
SetupAppTitle=Ezarpena
SetupWindowTitle=Ezarpena - %1
UninstallAppTitle=Kentzea
UninstallAppFullTitle=Kentzea - %1

; *** Misc. common
InformationTitle=Argibideak
ConfirmTitle=Berretsi
ErrorTitle=Akatsa

; *** SetupLdr messages
SetupLdrStartupMessage=Programa honek %1 ezarriko du. Jarraitu nahi duzu?
LdrCannotCreateTemp=Ezin izan da aldibaterako agirik. Ezarpena ezeztatu egin da
LdrCannotExecTemp=Aldibaterako agiritegiko agiria ezin izan da abiarazi. Ezarpena ezeztatu egin da

; *** Startup error messages
LastErrorMessage=%1.%n%n%2 akatsa: %3
SetupFileMissing=Ez da %1 agiria aurkitu ezarpenaren zuzenbidean. Mesedez zuzendu arazoa edo eskuratu programaren kopia berri bat.
SetupFileCorrupt=Ezarpen agiriak hondaturik daude. Eskuratu programaren kopia berri bat.
SetupFileCorruptOrWrongVer=Ezarpen agiriak hondaturik daude, edo ez dira ezartzailearen bertsio honekin bateragarriak. Mesedez zuzendu arazoa edo eskuratu programaren kopia berri bat.
NotOnThisPlatform=Programa hau ez dabil %1 sistemapean.
OnlyOnThisPlatform=Programa hau %1 sistemapean bakarrik dabil.
OnlyOnTheseArchitectures=Programa hau ondorengo prozesagailuen arkitekturetarako diseinatu diren Windowsen bertsioetan bakarrik ezarri daiteke:%n%n%1
MissingWOW64APIs=Darabilzun Windowsen bertsioak ez dakar 64-biteko ezarpen bat burutzeko ezartzaileak behar duen funtzionalitaterik. Arazo hau konpontzeko, ezarri Service Pack %1 zerbitzu paketea.
WinVersionTooLowError=Programa honek %1 %2 edo bertsio berriagoa behar du.
WinVersionTooHighError=Programa hau ezin da ezarri %1 %2 edo bertsio berriagoan.
AdminPrivilegesRequired=Programa hau ezartzeko administratzaile bezala hasi behar duzu saioa.
PowerUserPrivilegesRequired=Programa hau ezartzeko administratzaile bezala edo Agintedun Erabiltzaileen taldeko kide bezala hasi behar duzu saioa.
SetupAppRunningError=Ezartzaileak une honetan %1 irekita dagoela nabaritu du.%n%nItxi bere leiho guztiak, ondoren klikatu Ongi jarraitzeko, edo Ezeztatu irteteko.
UninstallAppRunningError=Ezartzaileak une honetan %1 irekita dagoela nabaritu du.%n%nItxi bere leiho guztiak, ondoren klikatu Ongi jarraitzeko, edo Ezeztatu irteteko.

; *** Misc. errors
ErrorCreatingDir=Ezartzaileak ezin izan du "%1" zuzenbidea sortu
ErrorTooManyFilesInDir=Ezinezkoa izan da "%1" zuzenbidean agiri bat sortzea, agiri gehiegi dituelako barnean

; *** Setup common messages
ExitSetupTitle=Irten Ezartzeiletik
ExitSetupMessage=Ezarpena ez da burutu. Orain irtenez gero, programa ez da ezarriko.%n%nEzarpena burutzeko edonoiz ireki dezakezu berriro ezartzailea.%n%nEzartzailetik Irten?
AboutSetupMenuItem=&Ezartzaileari Buruz...
AboutSetupTitle=Ezartzaileari Buruz
AboutSetupMessage=%1 bertsioa %2%n%3%n%n%1 webgunea:%n%4
AboutSetupNote= 
TranslatorNote=

; *** Buttons
ButtonBack=< A&tzera
ButtonNext=&Hurrengoa >
ButtonInstall=&Ezarri
ButtonOK=Ongi
ButtonCancel=Ezeztatu
ButtonYes=&Bai
ButtonYesToAll=Bai &Guztiari
ButtonNo=&Ez
ButtonNoToAll=E&z Guztiari
ButtonFinish=A&maitu
ButtonBrowse=&Bilatu...
ButtonWizardBrowse=B&ilatu...
ButtonNewFolder=&Agiritegi Berria Sortu

; *** "Select Language" dialog messages
SelectLanguageTitle=Hautatu Ezartzailearen Hizkuntza
SelectLanguageLabel=Hautatu ezarpenerako erabili nahi duzun hizkuntza:

; *** Common wizard text
ClickNext=Klikatu Hurrengoa jarraitzeko, edo Ezeztatu ezartzailetik irteteko.
BeveledLabel=
BrowseDialogTitle=Bilatu Agirtegia
BrowseDialogLabel=Hautatu agiritegi bat azpiko zerrendatik, ondoren klikatu Ongi.
NewFolderName=Agiritegi Berria

; *** "Welcome" wizard page
WelcomeLabel1=Ongi etorri [name] Ezarpen Laguntzailera
WelcomeLabel2=Programa honek [name/ver] zure ordenagailuan ezarriko du.%n%nGomendagarria da jarraitu aurretik gainontzeko aplikazioak istea.

; *** "Password" wizard page
WizardPassword=Sar-hitza
PasswordLabel1=Ezarpen hau sar-hitzez babesturik dago.
PasswordLabel3=Idatzi sar-hitza, ondoren klikatu Hurrengoa jarraitzeko. Sar-hitzetan hizki larriak bereizten dira.
PasswordEditLabel=&Sar-hitza:
IncorrectPassword=Idatzi duzun sar-hitza ez da zuzena. Saiatu berriro.

; *** "License Agreement" wizard page
WizardLicense=Baimenaren Onarpena
LicenseLabel=Irakurri ondorengo argibide garrantzitsu hauek jarraitu aurretik.
LicenseLabel3=Irakurri ondorengo Baimenaren Onarpena. Baimen honen baldintzak onartu behar dituzu ezarpenarekin jarraitu aurretik.
LicenseAccepted=Baimenaren baldintzak &onartzen ditut
LicenseNotAccepted=&Ez ditut baimenaren baldintzak onartzen

; *** "Information" wizard pages
WizardInfoBefore=Argibideak
InfoBeforeLabel=Irakurri ondorengo argibide garrantzitsu hauek jarraitu aurretik.
InfoBeforeClickLabel=Ezarpenarekin jarraitzeko gertu egotean, klikatu Hurrengoa.
WizardInfoAfter=Argibideak
InfoAfterLabel=Irakurri ondorengo argibide garrantzitsu hauek jarraitu aurretik.
InfoAfterClickLabel=Ezarpenarekin jarraitzeko gertu egotean, klikatu Hurrengoa.

; *** "User Information" wizard page
WizardUserInfo=Erabiltzailearen Datuak
UserInfoDesc=Idatzi zure datuak.
UserInfoName=&Erabiltzaile Izena:
UserInfoOrg=E&rakundea:
UserInfoSerial=&Serie Zenbakia:
UserInfoNameRequired=Izen bat idatzi behar duzu.

; *** "Select Destination Location" wizard page
WizardSelectDir=Hautatu Kokaleku Zuzenbidea
SelectDirDesc=Non ezarri beharko litzake [name]?
SelectDirLabel3=Ezartzaileak [name] ondorengo agiritegian ezarri du.
SelectDirBrowseLabel=Jarraitzeko, klikatu Hurrengoa. Beste agiritegi bat hautatu nahi baduzu, klikatu Bilatu.
DiskSpaceMBLabel=Gutxienez [mb] MBko toki hutsa behar da diskan.
ToUNCPathname=Ezartzaileak ezin du UNC helburu-izen batean ezarri. Sarean ezartzen saiatzen ari bazara, sareko diska bat mapeatu beharko duzu.
InvalidPath=Helburu-izen oso bat sartu behar duzu, gidagailu hizki eta guzti; adibidez:%n%nC:\APP%n%nedo UNC helburu-izen bat honela:%n%n\\zerbitzaria\elkarbanatua
InvalidDrive=Hautatu duzun gidagailua edo UNC elkarbanatua ez dago edo ezin da bertara sartu. Hautatu beste bat.
DiskSpaceWarningTitle=Ez Dago Behar Beste Toki Diskan
DiskSpaceWarning=Ezartzaileak gutxienez %1 KBko toki hutsa behar du ezartzeko, baina hautaturiko gidagailuak %2 KB bakarrik ditu hutsik.%n%nHala ere jarraitu nahi duzu?
DirNameTooLong=Agiritegiaren izena edo helburu-izena luzeegia da.
InvalidDirName=Agiritegiaren izena ez da zuzena.
BadDirName32=Agiritegiaren izenak ezin dezake ondorengo hizkietatik bat ere eduki:%n%n%1
DirExistsTitle=Agiritegia Badago
DirExists=Agiritegi hau:%n%n%1%n%nlehendik ere badago. Hala ere bertan ezarri nahi duzu?
DirDoesntExistTitle=Agiritegia Ez Dago
DirDoesntExist=Agiritegi hau:%n%n%1%n%nez dago. Sortu nahi duzu?

; *** "Select Components" wizard page
WizardSelectComponents=Hautatu Osagaiak
SelectComponentsDesc=Zein osagai ezarri behar dira?
SelectComponentsLabel2=Hautatu ezarri nahi dituzun osagaiak; garbitu ezarri nahi ez dituzunak. Klikatu Hurrengoa jarraitzeko gertu egotean.
FullInstallation=Guztia ezarri
; if possible don't translate 'Compact' as 'Minimal' (I mean 'Minimal' in your language)
CompactInstallation=Ezarpen Trinkoa
CustomInstallation=Ezarpen Norbereratua
NoUninstallWarningTitle=Osagai Hauek Badituzu
NoUninstallWarning=Ezartzaileak nabaritu du ondorengo osagaiok jadanik ordenagailuan ezarririk dituzula:%n%n%1%n%nOsagai hauek ez aukeratzeak ez ditu kenduko.%n%nHala ere jarraitu nahi duzu?
ComponentSize1=%1 KB
ComponentSize2=%1 MB
ComponentsDiskSpaceMBLabel=Uneko aukeraketak gutxienez [mb] MBko toki hutsa behar du diskan.

; *** "Select Additional Tasks" wizard page
WizardSelectTasks=Hautatu Eginkizun Gehigarriak
SelectTasksDesc=Zein eginkizun gehigarri burutu behar dira?
SelectTasksLabel2=Hautatu [name] ezarri bitartean ezartzaileak burutu beharreko eginkizun gehigarriak, ondoren klikatu Hurrengoa.

; *** "Select Start Menu Folder" wizard page
WizardSelectProgramGroup=Hautatu Hasi Menuko Agiritegia
SelectStartMenuFolderDesc=Non sortu behar ditu ezartzaileak programaren lasterbideak?
SelectStartMenuFolderLabel3=Ezartzaileak Hasi Menuko ondorengo agiritegina sortuko ditu programaren lasterbideak.
SelectStartMenuFolderBrowseLabel=Jarraitzeko, klikatu Hurrengoa. Beste agiritegi bat hautatu nahi baduzu, klikatu Bilatu.
MustEnterGroupName=Agiritegi izen bat sartu behar duzu.
GroupNameTooLong=Agiritegiaren izena edo helburu-izena luzeegia da.
InvalidGroupName=Agiritegiaren izena ez da zuzena.
BadGroupName=Agiritegiaren izenak ezin dezake ondorengo hizkietatik bat ere eduki:%n%n%1
NoProgramGroupCheck2=&Ez sortu Hasi Menuko agiritegirik

; *** "Ready to Install" wizard page
WizardReady=Ezarpentzeko Gertu
ReadyLabel1=Ezarpentzailea [name] zure ordenagailuan ezartzen hasteko gertu dago.
ReadyLabel2a=Klikatu Ezarri ezarpenarekin jarraitzeko, edo klikatu Atzera ezarpenen bat berrikusi edo aldatu nahi baduzu.
ReadyLabel2b=Klikatu Instalatu instalaketarekin jarraitzeko.
ReadyMemoUserInfo=Erabiltzailearen datuak:
ReadyMemoDir=Kokaleku zuzenbidea:
ReadyMemoType=Ezarpen mota:
ReadyMemoComponents=Hautaturiko osagaiak:
ReadyMemoGroup=Hasi Menuko agiritegia:
ReadyMemoTasks=Eginkizun gehigarriak:

; *** "Preparing to Install" wizard page
WizardPreparing=Ezarpenerako Gertatzen
PreparingDesc=Ezartzailea [name] zure ordenagailuan ezartzeko gertatzen ari da.
PreviousInstallNotCompleted=Aurreko programa baten ezartzea/kentzea ez da burutu. Ezarpen hura burutzeko ordenagailua berrabiarazi beharko duzu.%n%nOrdenagailua berrabiarazi ondoren, ireki ezartzailea berriro [name] ezarpenarekin bukatzeko.
CannotContinue=Ezinezkoa da ezarpenakin jarraitzea. Klikatu Ezeztatu irteteko.

; *** "Installing" wizard page
WizardInstalling=Ezartzen
InstallingLabel=Itxaron ezartzaileak [name] zure ordenagailuan ezarri artean.

; *** "Setup Completed" wizard page
FinishedHeadingLabel=[name] Ezarpen Laguntzailea Burutzen
FinishedLabelNoIcons=Ezartzaileak [name] zure ordenagailuan ezarri du.
FinishedLabel=Ezartzaileak [name] zure ordenagailuan ezarri du. Aplikazioa abiarazteko ezarririko ikonoetan klikatu.
ClickFinish=Klikatu Amaitu ezartzailetik irteteko.
FinishedRestartLabel=[name] programaren ezarpena burutzeko, ezartzaileak ordenagailua berrabiarazi beharra du. Orain berrabiarazi nahi duzu?
FinishedRestartMessage=[name] programaren ezarpena burutzeko, ezartzaileak ordenagailua berrabiarazi beharra du.%n%nOrain berrabiarazi nahi duzu?
ShowReadmeCheck=Bai, IRAKURRI agiria ikusi nahi dut
YesRadio=&Bai, berrabiarazi orain
NoRadio=&Ez, beranduago berrabiaraziko dut
; used for example as 'Run MyProg.exe'
RunEntryExec=Abiarazi %1
; used for example as 'View Readme.txt'
RunEntryShellExec=Ikusi %1

; *** "Setup Needs the Next Disk" stuff
ChangeDiskTitle=Ezartzaileak Hurrengo Diska Behar Du
SelectDiskLabel2=Sartu %1. diska eta klikatu Ongi.%n%nDiska honetako agiriak ez badaude azpian ageri den agirtegian, sartu helburu egokia edo klikatu Bilatu.
PathLabel=&Helburua:
FileNotInDir2="%1" agiria ezin izan da "%2" agiritegian aurkitu. Sartu diska zuzena edo hautatu beste agiritegi bat.
SelectDirectoryLabel=Zehaztu hurrengo diskaren kokapena.

; *** Installation phase messages
SetupAborted=Ezarpena ez da burutu.%n%nKonpondu arazoa eta abiarazi ezartzailea berriro.
EntryAbortRetryIgnore=Klikatu Saiatu Berriz berriro saiatzeko, Ezikusi hala ere jarraitzeko, edo Utzi ezarpena ezeztatzeko.

; *** Installation status messages
StatusCreateDirs=Zuzenbideak sortzen...
StatusExtractFiles=Agiriak ateratzen...
StatusCreateIcons=Lasterbideak sortzen...
StatusCreateIniEntries=INI sarrerak sortzen...
StatusCreateRegistryEntries=Erregistroko sarrerak sortzen...
StatusRegisterFiles=Agiriak erregistratzen...
StatusSavingUninstall=Kentzeko datuak gordetzen...
StatusRunProgram=Ezarpena burutzen...
StatusRollback=Aldaketak desegiten...

; *** Misc. errors
ErrorInternal2=Barneko akatsa: %1
ErrorFunctionFailedNoCode=Hutsegitea: %1
ErrorFunctionFailed=Hutsegitea: %1; Kodea: %2
ErrorFunctionFailedWithMessage=Hutsegitea: %1; Kodea: %2.%n%3
ErrorExecutingProgram=Ezin izan da agiri hau abiarazi:%n%1

; *** Registry errors
ErrorRegOpenKey=Akatsa erregistroko gakoa irekitzean:%n%1\%2
ErrorRegCreateKey=Akatsa erregistroko gakoa sortzean:%n%1\%2
ErrorRegWriteKey=Akatsa erregistroko gakoa idaztean:%n%1\%2

; *** INI errors
ErrorIniEntry=Akatsa "%1" agirian INI sarrera sortzean.

; *** File copying errors
FileAbortRetryIgnore=Klikatu Saiatu Berriz berriro saiatzeko, Ezikusi agiri honi jauzi egiteko (ez da gomendagarria), edo Utzi ezarpena ezeztatzeko
FileAbortRetryIgnore2=Klikatu Saiatu Berriz berriro saiatzeko, Ezikusi hala ere jarraitzeko (ez da gomendagarria), edo Utzi ezarpena ezeztatzeko.
SourceIsCorrupted=Iturburu agiria hondaturik dago
SourceDoesntExist=Ez dago "%1" izeneko iturburu agiririk
ExistingFileReadOnly=Lehendik zegoen agiria irakurtzeko-bakarrik bezala gisa markaturik dago.%n%nKlikatu Saiatu Berriz irakurtzeko-bakarrik ezaugarria ezabatu eta berriro saiatzeko, Ezikusi agiri honi jauzi egiteko, edo Utzi ezarpena ezeztatzeko.
ErrorReadingExistingDest=Akats bat izan da lehendik zegoen agiri hau irakurtzean:
FileExists=Fitxategia lehendik ere bazegoen.%n%nEzartzaileak gainidatzi dezan nahi duzu?
ExistingFileNewer=Lehendik zegoen agirai Ezartzaileak ezarri nahi duena baino berriagoa da. Lehendik zegoenari heustea gomendatzen da.%n%nLehengoari heustea nahi duzu?
ErrorChangingAttr=Akats bat izan da lehendik zegoen agiri honen ezaugarriak aldatzean:
ErrorCreatingTemp=Akats bat izan da ondorengo kokaleku zuzenbidean agiri bat sortzean:
ErrorReadingSource=Akats bat izan da iturburu agiria irakurtzean:
ErrorCopying=Akats bat izan da agiri hau kopiatzean:
ErrorReplacingExistingFile=Akats bat izan da lehendik zegoen agiri hau ordezkatzean:
ErrorRestartReplace=BerrabiaraziOrdeztuk huts egin du:
ErrorRenamingTemp=Akats bat izan da ondorengo kokaleku zuzenbidean agiri bat berrizendatzean:
ErrorRegisterServer=Ezinezkoa izan da DLL/OCX hau erregistratzea: %1
ErrorRegSvr32Failed=RegSvr32k huts egin du %1 itxiera kodea emanez
ErrorRegisterTypeLib=Ezinezkoa izan da moten liburutegi hau erregistratzea: %1

; *** Post-installation errors
ErrorOpeningReadme=Akats bat izan da IRAKURRI agiria irekitzean.
ErrorRestartingComputer=Ezartzaileak ezin izan du ordenaagailua berrabiarazi. Egin ezazu eskuz.

; *** Uninstaller messages
UninstallNotFound=Ez da "%1" agiria aurkitu. Ezin izan da kendu.
UninstallOpenError=Ezin izan da "%1" ireki. Ezin izan da kendu
UninstallUnsupportedVer=Kentzeko "%1" ohar agiria ezartzailearen bertsio honek ezagutzen ez duen formatu batean dago. Ezin izan da kendu
UninstallUnknownEntry=Sarrera ezezagun bat (%1) aurkitu da kentzeko oharrean
ConfirmUninstall=Zihur %1 eta bere osagai guztiak ezabatu nahi dituzula?
UninstallOnlyOnWin64=Ezarpen hau 64-biteko Windowsean bakarrik kendu daiteke.
OnlyAdminCanUninstall=Ezarpen hau administratzaile eskumenak dituen erabiltzaile batek bakarrik kendu dezake.
UninstallStatusLabel=Itxaron %1 zure ordenagailutik ezabatzen den artean.
UninstalledAll=%1 ongi ezabatu da zure ordenagailutik.
UninstalledMost=%1 kendu da.%n%nZenbait agiri ezin izan dira ezabatu. Agiri hauek eskuz ezaba daitezke.
UninstalledAndNeedsRestart=%1 guztiz kentzeko, zure ordenagailua berrabiarazi beharra dago.%n%nOrain berrabiarazi nahi duzu?
UninstallDataCorrupted="%1" agiria hondaturik dago. Ezin izan da kendu

; *** Uninstallation phase messages
ConfirmDeleteSharedFileTitle=Agiri Elkarbanatua Ezabatu?
ConfirmDeleteSharedFile2=Sistemaren arabera ondorengo agiri elkarbanatua ez du inongo programak erabiliko hemendik aurrera. Kentzaileak agiri hau ezabatu dezan nahi duzu?%n%nProgramaren bat agiri hau erabiltzen ari da oraindik eta ezabatzen baduzu, programa hori ez da egoki ibiliko. Zihur ez bazaude, hautatu Ez. Agiria sisteman uzteak ez dizu inongo kalterik eragingo.
SharedFileNameLabel=Agiri izena:
SharedFileLocationLabel=Kokapena:
WizardUninstalling=Kentze Egoera
StatusUninstalling=Kentzen: %1...

; The custom messages below aren't used by Setup itself, but if you make
; use of them in your scripts, you'll want to translate them.

[CustomMessages]
;Inno Setup Built-in Custom Messages
NameAndVersion=%1 %2 bertsioa
AdditionalIcons=Ikono gehigarriak:
OptionalFeatures=Optional Features:
CreateDesktopIcon=&Mahaigainean lasterbidea sortu
CreateQuickLaunchIcon=&Eginkizun Barran lasterbidea sortu
ProgramOnTheWeb=%1 sarean
UninstallProgram=%1 kendu
LaunchProgram=%1 abiarazi
AssocFileExtension=&Lotu %1 programa %2 agiri luzapenarekin
AssocingFileExtension=%1 programa %2 agiri luzapenarekin lotzen...

;Things we can also localize
CompanyName=Thingamahoochie Software

;Types
TypicalInstallation=Ohiko Ezarpena
FullInstallation=Ezarpen Osoa
CompactInstallation=Ezarpen Trinkoa
CustomInstallation=Norbere Ezarpena

;Components
AppCoreFiles=WinMerge Core Agiriak
Filters=Iragazkiak
Plugins=Pluginak

;Localization Components
Languages=Hizkuntzak
BasqueLanguage=Euskara menuak eta elkarrizketak
BulgarianLanguage=Bulgariera menuak eta elkarrizketak
CatalanLanguage=Kataluniera menuak eta elkarrizketak
ChineseSimplifiedLanguage=Txinera (Arrunta) menuak eta elkarrizketak
ChineseTraditionalLanguage=Txinera (Tradizionala) menuak eta elkarrizketak
CroatianLanguage=Kroaziera menuak eta elkarrizketak
CzechLanguage=Txekiera menuak eta elkarrizketak
DanishLanguage=Daniera menuak eta elkarrizketak
DutchLanguage=Herbeherera menuak eta elkarrizketak
FrenchLanguage=Frantziera menuak eta elkarrizketak
GalicianLanguage=Galiziera menuak eta elkarrizketak
GermanLanguage=Alemaniera menuak eta elkarrizketak
GreekLanguage=Greziera menuak eta elkarrizketak
HungarianLanguage=Hungariera menuak eta elkarrizketak
ItalianLanguage=Italiera menuak eta elkarrizketak
JapaneseLanguage=Japoniera menuak eta elkarrizketak
KoreanLanguage=Koreaera menuak eta elkarrizketak
NorwegianLanguage=Norbegiera menuak eta elkarrizketak
PersianLanguage=Persiera menuak eta elkarrizketak
PolishLanguage=Poloniera menuak eta elkarrizketak
PortugueseBrazilLanguage=Portugalera (Brazilgoa) menuak eta elkarrizketak
PortugueseLanguage=Portugalera menuak eta elkarrizketak
RomanianLanguage=Errumaniera menuak eta elkarrizketak
RussianLanguage=Errusiera menuak eta elkarrizketak
SerbianLanguage=Serbiera menuak eta elkarrizketak
SlovakLanguage=Eslovakiera menuak eta elkarrizketak
SlovenianLanguage=Esloveniera menuak eta elkarrizketak
SpanishLanguage=Espainiera menuak eta elkarrizketak
SwedishLanguage=Suediera menuak eta elkarrizketak
TurkishLanguage=Turkieramenuak eta elkarrizketak
UkrainianLanguage=Ukrainiera menuak eta elkarrizketak

;Tasks
ExplorerContextMenu=&Gaitu Explorer hitzinguru menu baterapena
IntegrateTortoiseCVS=Bateratu &TortoiseCVS-rekin
IntegrateTortoiseSVN=Bateratu T&ortoiseSVN-rekin
IntegrateClearCase=Bateratu Rational &ClearCase-rekin
AddToPath=&Gehitu WinMerge agiritegia zure sistema helburura

;Icon Labels
ReadMe=Irakurri nazazu
UsersGuide=Erabiltzaile Gida
ViewStartMenuFolder=&Ikusi WinMerge Hasiera Menua Agiritegia

;Code Dialogs
DeletePreviousStartMenu=Ezartzaileak nabaritu du zure hasiera menuaren kokalekua "%s"-tik "%s"-ra aldatu duzula. Lehengo hasiera menuko agiritegia ezabatzea nahi duzu?

; Project file description
ProjectFileDesc=WinMerge Egitasmo agiria
