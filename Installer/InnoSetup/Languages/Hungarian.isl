; *** Inno Setup version 4.2.2+ Hungarian messages ***
;
; To download user-contributed translations of this file, go to:
;   http://www.jrsoftware.org/is3rdparty.php
;
; Note: When translating this text, do not add periods (.) to the end of
; messages that didn't have them already, because on those messages Inno
; Setup adds the periods automatically (appending a period would result in
; two periods being displayed).
;
; $jrsoftware: issrc/Files/Default.isl,v 1.58 2004/04/07 20:17:13 jr Exp $

[LangOptions]
LanguageName=Magyar
LanguageID=$0409
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
SetupAppTitle=Telepítés
SetupWindowTitle=Telepítés - %1
UninstallAppTitle=Eltávolítás
UninstallAppFullTitle=%1 Eltávolítás

; *** Misc. common
InformationTitle=Információ
ConfirmTitle=Megerõsítés
ErrorTitle=Hiba

; *** SetupLdr messages
SetupLdrStartupMessage=Ezzel megkezded a %1 telepítését. Kívánod folytatni?
LdrCannotCreateTemp=Nem lehet létrehozni az ideiglenes fájlokat. A telepítés megszakadt
LdrCannotExecTemp=Nem lehet futtatni az ideiglenes fájlokat. A telepítés megszakadt

; *** Startup error messages
LastErrorMessage=%1.%n%nHiba %2: %3
SetupFileMissing=Ez a fájl: '%1' hiányzik a telepítési könyvtárból. Kérlek, ellenõrizd a hibát, és ha szükséges, szerezz be egy új példányt a programból.
SetupFileCorrupt=A telepítõ fájlok hibásak. Kérlek, szerezz be egy új példányt a programból.
SetupFileCorruptOrWrongVer=A telepitõ fájlok hibásak, vagy nem kompatibilisak a Telepítõvel. Kérlek, ellenõrizd a hibát, és ha szükséges, szerezz be egy új példányt a programból.
NotOnThisPlatform=Ez a program nem fog futni %1 rendszeren.
OnlyOnThisPlatform=Ezt a programot csak %1 rendszeren lehet futtatni.
WinVersionTooLowError=A programhoz a %1 rendszer szükséges legalább %2 verziószámmal.
WinVersionTooHighError=A program nem telepíthetõ %1 rendszer %2, vagy újabb verzióján.
AdminPrivilegesRequired=A program telepítéshez rendszergazdaként kell bejelentkezni.
PowerUserPrivilegesRequired=A program telepítéshez rendszergazdaként, vagy a telepítésre jogosult felhasználóként szükséges bejelentkezni.
SetupAppRunningError=A Telepítõ a %1 program futását érzékelte.%n%nKérlek, zárj be minden éppen futó példányt majd nyomd meg a Rendben gombot a folytatáshoz, vagy a Mégsem gombot a kilépéshez.
UninstallAppRunningError=Az Eltávolítás a %1 program futását érzékelte.%n%nKérlek, zárj be minden éppen futó példányt majd nyomd meg a Rendben gombot a folytatáshoz, vagy a Mégsem gombot a kilépéshez.

; *** Misc. errors
ErrorCreatingDir=A Telepítõ nem tudta létrehozni a következõ könyvtárat: "%1"
ErrorTooManyFilesInDir=Nem lehetett létrehozni a(z) "%1" fájlt, mivel a célkönyvtár túl sok fájlt tartalmaz

; *** Setup common messages
ExitSetupTitle=Kilépés a Telepítõbõl
ExitSetupMessage=A Telepítés nem fejezõdött be. Amennyiben most kilépsz, a program nem fog mûködni.%n%nTermészetesen késõbb bármikor újrafuttathatod a Telepítõt.%n%nKilépsz a Telepítõbõl?
AboutSetupMenuItem=A &Telepítõrõl...
AboutSetupTitle=A Telepítõrõl
AboutSetupMessage=%1 verzió: %2%n%3%n%n%1 Honlap:%n%4
AboutSetupNote=Magyar fordítás: Márton Balázs

; *** Buttons
ButtonBack=< &Vissza
ButtonNext=&Tovább >
ButtonInstall=&Telepítés
ButtonOK=Rendben
ButtonCancel=Mégsem
ButtonYes=&Igen
ButtonYesToAll=Igen &Mindenre
ButtonNo=&Nem
ButtonNoToAll=N&em Mindenre
ButtonFinish=&Befejezés
ButtonBrowse=&Tallózás...
ButtonWizardBrowse=T&allózás...
ButtonNewFolder=&Új könyvtár létrehozása

; *** "Select Language" dialog messages
SelectLanguageTitle=Válaszd ki a Telepítõ nyelvét
SelectLanguageLabel=Válaszd ki a telepítés alatt használni kívánt nyelvet:

; *** Common wizard text
ClickNext=Kattints a Tovább gombra a folytatáshoz, a Mégsem gombra a kilépéshez.
BeveledLabel=
BrowseDialogTitle=Könyvtárak tallózása
BrowseDialogLabel=Válassz egy könyvtárat a listából, majd nyomd meg a Rendben gombot.
NewFolderName=Új könyvtár

; *** "Welcome" wizard page
WelcomeLabel1=Üdvözöl a [name] Telepítõ Varázsló
WelcomeLabel2=Ezzel megkezded a [name/ver] telepítését a számítógépre.%n%nTanácsos bezárni minden más futó programot a folytatás elõtt.

; *** "Password" wizard page
WizardPassword=Jelszó
PasswordLabel1=A telepítõ program jelszóval védett.
PasswordLabel3=Kérlek, add meg a jelszót, majd nyomd meg a Tovább gombot. A program érzékeny a kis-nagybetûkre.
PasswordEditLabel=&Jelszó:
IncorrectPassword=A megadott jelszó nem megfelelõ. Kérlek, add meg újra.

; *** "License Agreement" wizard page
WizardLicense=Licensz Szerzõdés
LicenseLabel=Kérlek, olvasd el a következõ fontos információt folytatás elõtt.
LicenseLabel3=Kérlek, olvasd el a következõ Licensz Szerzõdést. A telepítés folytatásához el kell fogadnod a szerzõdés feltételeit.
LicenseAccepted=&Elfogadom a Szerzõdést
LicenseNotAccepted=&Nem fogadom el a szerzõdést

; *** "Information" wizard pages
WizardInfoBefore=Információ
InfoBeforeLabel=Kérlek, olvasd el a következõ fontos információt folytatás elõtt.
InfoBeforeClickLabel=Amennyiben készen állsz a Telepítés folytatására, kattints a Tovább gombra.
WizardInfoAfter=Információ
InfoAfterLabel=Kérlek, olvasd el a következõ fontos információt folytatás elõtt.
InfoAfterClickLabel=Amennyiben készen állsz a Telepítés folytatására, kattints a Tovább gombra.

; *** "User Information" wizard page
WizardUserInfo=Felhasználói adatok
UserInfoDesc=Kérlek, ad meg az adataidat.
UserInfoName=&Felhasználói név:
UserInfoOrg=&Vállalat:
UserInfoSerial=&Széria szám:
UserInfoNameRequired=Meg kell adnod egy nevet.

; *** "Select Destination Location" wizard page
WizardSelectDir=Add meg a telepítési célt
SelectDirDesc=Hol legyen a(z) [name] telepítve?
SelectDirLabel3=A Telepítõ a(z) [name] programot ide fogja telepíteni.
SelectDirBrowseLabel=A folytatáshoz nyomd meg a Tovább gombot. Amennyiben más könyvtárat szeretnél választani, kattints a Tallózás gombra.
DiskSpaceMBLabel=Legalább [mb] MB szabad hely szükséges.
ToUNCPathname=A Telepítõ nem tud UNC elérési úton telepíteni. Amennyiben hálózatra szeretnéd telepíteni a programot, hozz létre egy hálózati meghajtót.
InvalidPath=Teljes elérési utat kell megadnod, pl.:%n%nC:\APP%n%nvagy egy UNC elérési utat, pl.:%n%n\\server\share
InvalidDrive=A meghajtó vagy UNC elérési út nem létezik. Kérlek, adj meg egy másikat.
DiskSpaceWarningTitle=Nincs elegendõ szabad hely
DiskSpaceWarning=A Telepítõ legalább %1 KB szabad helyet igényel a telepítéshez, de a megadott meghajtón csak %2 KB szabad hely van.%n%nEnnek ellenére folytatni akarod?
DirNameTooLong=A könyvtár neve vagy az elérési út túl hosszú.
InvalidDirName=A könyvtár neve érvénytelen.
BadDirName32=A könyvtár neve nem tartalmazhatja a következõ karaktereket:%n%n%1
DirExistsTitle=A könyvtár már létezik
DirExists=Ez a könyvtár %n%n(%1)%n%nmár létezik. Ennek ellenére szeretnéd ide telepíteni a programot?
DirDoesntExistTitle=A könyvtár nem létezik
DirDoesntExist=Ez a könyvtár:%n%n(%1)%n%n még nem létezik. Szeretnéd, ha most létrehoznám ezt a könyvtárat?

; *** "Select Components" wizard page
WizardSelectComponents=Válaszd ki az összetevõket
SelectComponentsDesc=Mely összetevõk legyenek telepítve?
SelectComponentsLabel2=Válaszd ki a telepíteni kívánt összetevõket; Hagyd üresen azokat az összetevõket, amelyeket nem szeretnél telepíteni. Amennyiben kész vagy, kattints a Tovább gombra.
FullInstallation=Teljes telepítés
; if possible don't translate 'Compact' as 'Minimal' (I mean 'Minimal' in your language)
CompactInstallation=Kompakt telepítés
CustomInstallation=Egyedi telepítés
NoUninstallWarningTitle=Az összetevõ már létezik
NoUninstallWarning=A Telepítés észlelte a következõ összetevõket a számítógépen:%n%n%1%n%nAmennyiben üresen hagyod õket, nem lesznek telepítve.%n%nEnnek ellenére folytatni akarod?
ComponentSize1=%1 KB
ComponentSize2=%1 MB
ComponentsDiskSpaceMBLabel=A jelenlegi összeállítás [mb] MB szabad helyet igényel.

; *** "Select Additional Tasks" wizard page
WizardSelectTasks=Válaszd ki a további feladatokat
SelectTasksDesc=Milyen további feladatok legyenek végrehajtva?
SelectTasksLabel2=Válaszd ki azokat a további feladatokat, melyek a(z) [name] telepítésekor lesznek végrehajtva, majd nyomd meg a Tovább gombot.

; *** "Select Start Menu Folder" wizard page
WizardSelectProgramGroup=Válaszd ki a Start Menü mappát
SelectStartMenuFolderDesc=Hol helyezze el a Telepítõ a parancsikonokat?
SelectStartMenuFolderLabel3=A Telepítõ a következõ Start Menü mappában helyezi majd el a parancsikonokat.
SelectStartMenuFolderBrowseLabel=A folytatáshoz kattints a Tovább gombra. Amennyiben más mappát szeretnél megadni, kattints a Tallózás gombra.
NoIconsCheck=&Ne hozzon létre parancsikonokat
MustEnterGroupName=Meg kell adnod egy mappát.
GroupNameTooLong=A mappa neve, vagy elérési útja túl hosszú.
InvalidGroupName=A mappa neve érvénytelen.
BadGroupName=A mappa neve nem tartalmazhatja a következõ karaktereket:%n%n%1
NoProgramGroupCheck2=N&e hozzon létre mappát a Start Menüben

; *** "Ready to Install" wizard page
WizardReady=Készen állsz a Telepítésre
ReadyLabel1=A Telepítõ készen áll a(z) [name] telepítésére a számítógépre.
ReadyLabel2a=Kattints a Telepítés gombra a telepítés megkezdéséhez, vagy a Vissza gombra a beállításaid módosításához.
ReadyLabel2b=Kattints a Telepítés gombra a telepítés megkezdéséhez.
ReadyMemoUserInfo=Felhasználói adatok:
ReadyMemoDir=Telepítés helye:
ReadyMemoType=Telepítés típusa:
ReadyMemoComponents=Kiválasztott komponensek:
ReadyMemoGroup=Start Menü mappa:
ReadyMemoTasks=További feladatok:

; *** "Preparing to Install" wizard page
WizardPreparing=Felkészülés a telepítésre
PreparingDesc=A Telepítõ felkészül a(z) [name] telepítésére a számítógépen.
PreviousInstallNotCompleted=A Telepítés/Eltávolítás még nem fejezõdött be. A befejezéshez a számítógép újraindítása szükséges.%n%nA számítógép újraindítása után futtasd újra a Telepítõt a(z) [name] telepítéséhez.
CannotContinue=A Telepítés nem folytatódhat. Nyomd meg a Mégsem gombot a kilépéshez.

; *** "Installing" wizard page
WizardInstalling=Telepítés
InstallingLabel=Kérlek várj, amíg a Telepítõ elvégzi a(z) [name] telepítését a számítógépre.

; *** "Setup Completed" wizard page
FinishedHeadingLabel=A(z) [name] Telepítõ Varázsló elõkészítése
FinishedLabelNoIcons=A Telepítõ befejezte a(z) [name] telepítését a számítógépre.
FinishedLabel=A Telepítõ befejezte a(z) [name] telepítését a számítógépre. A program a telepített ikonokra kattintva futtatható.
ClickFinish=A kilépéshez nyomd meg a Befejezés gombot.
FinishedRestartLabel=A(z) [name] telepítésének befejezéséhez, a Telepítõnek újra kell indítania a számítógépet. Szeretnéd ezt most megtenni?
FinishedRestartMessage=A(z) [name] telepítésének befejezéséhez, a Telepítõnek újra kell indítania a számítógépet.%n%nSzeretnéd ezt most megtenni?
ShowReadmeCheck=Igen, szeretném elolvasni a README fájlt
YesRadio=&Igen, indítsa újra most a számítógépet
NoRadio=&Nem, majd késõbb indítom újra a számítógépet
; used for example as 'Run MyProg.exe'
RunEntryExec=A(z) %1 futtatása
; used for example as 'View Readme.txt'
RunEntryShellExec=A(z) %1 megtekintése

; *** "Setup Needs the Next Disk" stuff
ChangeDiskTitle=A Telepítõnek szüksége van a következõ lemezre
SelectDiskLabel2=Kérlek tedd be a %1. számú lemezt és nyomd meg a Rendben gombot.%n%nAmennyiben ezen a lemezen a fájlok más elérési úton találhatóak meg, mint a lenti, kérlek add meg a helyes útvonalat, vagy kattints a Tallózás gombra.
PathLabel=&Útvonal:
FileNotInDir2=A(z) "%1" nevû fájl nem található meg a(z) "%2" útvonalon. Kérlek, tedd be a megfelelõ lemezt vagy add meg a helyes elérési útvonalat.
SelectDirectoryLabel=Kérlek, add meg a következõ lemez elérési útvonalát.

; *** Installation phase messages
SetupAborted=A Telepítés nem fejezõdött be.%n%nKérlek, javítsd ki a hibát és futtasd újra a Telepítõt.
EntryAbortRetryIgnore=Nyomd meg az Újra gombot az újbóli próbálkozáshoz, a Kihagy gombot a folytatáshoz, vagy a Mégsem gombot a telepítés befejezéséhez.

; *** Installation status messages
StatusCreateDirs=Könyvtárak létrehozása...
StatusExtractFiles=Fájlok kibontása...
StatusCreateIcons=Parancsikonok létrehozása...
StatusCreateIniEntries=INI bejegyzések létrehozása...
StatusCreateRegistryEntries=Regisztrációs bejegyzések létrehozása...
StatusRegisterFiles=Fájlok regisztrálása...
StatusSavingUninstall=Eltávolítási információk mentése...
StatusRunProgram=Telepítés befejezése...
StatusRollback=Változtatások visszafejtése...

; *** Misc. errors
ErrorInternal2=Belsõ hiba: %1
ErrorFunctionFailedNoCode=A(z) %1 sikertelen volt
ErrorFunctionFailed=A(z) %1 sikertelen volt; kód: %2
ErrorFunctionFailedWithMessage=A(z) %1 sikertelen volt; kód: %2.%n%3
ErrorExecutingProgram=Nem lehet futtatni a következõ fájlt:%n%1

; *** Registry errors
ErrorRegOpenKey=Nem lehetett megnyitni a következõ regisztrációs bejegyzést:%n%1\%2
ErrorRegCreateKey=Nem lehetett létrehozni a következõ regisztrációs bejegyzést:%n%1\%2
ErrorRegWriteKey=Nem lehetett írni a következõ regisztrációs bejegyzést:%n%1\%2

; *** INI errors
ErrorIniEntry=Nem lehetett létrehozni az INI bejegyzést a következõ fájlban: "%1".

; *** File copying errors
FileAbortRetryIgnore=Nyomd meg az Ismét gombot az újrapróbálkozáshoz, a Kihagy gombot a fájl kihagyásához (nem javasolt), vagy a Befejez gombot a telepítés megszakításához.
FileAbortRetryIgnore2=Nyomd meg az Ismét gombot az újrapróbálkozáshoz, a Kihagy gombot a fájl kihagyásához (nem javasolt), vagy a Befejez gombot a telepítés megszakításához.
SourceIsCorrupted=A forrás fájl hibás
SourceDoesntExist=Egy forrás fájl (%1) hiányzik
ExistingFileReadOnly=A létezõ fájl csak olvashatóként van megjelölve.%n%nKattints az Újra gombra ennek megszûntetéséhez, a Kihagy gombra a fájl kihagyásához, vagy a Befejez gombot a telepítés megszakításához.
ErrorReadingExistingDest=Hiba történt a következõ, már létezõ fájl írásakor:
FileExists=A fájl már létezik.%n%nSzeretnéd, hogy a Telepítõ felülírja azt?
ExistingFileNewer=A telepített fájl újabb annál, amire a Telepítõ lecserélné. Javasolt, hogy tartsd meg az eredeti fájlt.%n%nSzeretnéd ezt tenni?
ErrorChangingAttr=Hiba történt a következõ fájl attribútumainak megváltoztatásakor:
ErrorCreatingTemp=Hiba történt a következõ fájl létrehozásakor:
ErrorReadingSource=Hiba történt a következõ forrásfájl olvasásakor:
ErrorCopying=Hiba történt a következõ fájl olvasásakor:
ErrorReplacingExistingFile=Hiba történt a következõ, már létezõ fájl írásakor:
ErrorRestartReplace=RestartReplace failed:
ErrorRenamingTemp=Hiba történt a következõ fájl átnevezésekor:
ErrorRegisterServer=Nem lehetett regisztráni a következõ DLL/OCX modult: %1
ErrorRegisterServerMissingExport=A DllRegisterServer exportálás nem támogatott
ErrorRegisterTypeLib=Nem lehetett regisztrálni a következõ típus könyvtárat: %1

; *** Post-installation errors
ErrorOpeningReadme=Hiba történt a README fájl megnyitása közben.
ErrorRestartingComputer=A Telepítõ nem tudta újraindítani a számítógépet. Kérlek, tedd ezt meg a hagyományos úton.

; *** Uninstaller messages
UninstallNotFound="%1" nevû fájl nem létezik. Nem lehetett eltávolítani.
UninstallOpenError=A "%1" nevû fájlt nme lehetett megnyitni. Nem lehetett eltávolítani.
UninstallUnsupportedVer=Az eltávolítási napló fájl (%1) formátuma nem értelmezhetõ a Telepítõnek eme verziójával. Nem lehetett eltávolítani.
UninstallUnknownEntry=Ismeretlen bejegyzés (%1) található az eltávolítási naplóban
ConfirmUninstall=Biztosan szeretnéd eltávolítani a(z) %1 programot és annak minden összetevõjét?
OnlyAdminCanUninstall=Ezt programot csak adminisztrátori jogokkal rendelkezõ felhasználó távolíthatja el.
UninstallStatusLabel=Kérlek várj, amíg a(z) %1 programot eltávolítom a számítógéprõl.
UninstalledAll=A(z) %1 sikeresen el lett távolítva a számítógéprõl.
UninstalledMost=A(z) %1 eltávolítása sikeres volt.%n%nNéhány elemet nem lehetett eltávolítani. Ezeket a hagyományos módon lehet törölni.
UninstalledAndNeedsRestart=A(z) %1 teljes mértékû eltávolításához a számítógépet újra kell indítani.%n%nSzeretnéd ezt most megtenni?
UninstallDataCorrupted=A(z) "%1" nevû fájl hibás. Nem lehet eltávolítani.

; *** Uninstallation phase messages
ConfirmDeleteSharedFileTitle=Eltávolítsam a megosztott fájlt?
ConfirmDeleteSharedFile2=A rendszer jelzése szerint a következõ megosztott fájl már egyáltalán nincs használatban. Szeretnéd, ha az Eltávolító eltávolítaná a számítógéprõl?%n%nAmennyiben eltávolításra kerül, esetleg más programok nem fognak mûködni. Amennyiben bizonytalan vagy, válaszd a Nem gombot. Semmi baj nem történhet, ha a fájl a számítógépen marad.
SharedFileNameLabel=Fájlnév:
SharedFileLocationLabel=Hely:
WizardUninstalling=Eltávolítási állapot
StatusUninstalling=A(z) %1 eltávolítása...

; The custom messages below aren't used by Setup itself, but if you make
; use of them in your scripts, you'll want to translate them.

[CustomMessages]
;Inno Setup Built-in Custom Messages
NameAndVersion=%1 - Verziószám: %2
AdditionalIcons=További ikonok:
CreateDesktopIcon=Ikon létrehozása az &Asztalon
CreateQuickLaunchIcon=Ikon létrehozása a &Gyorsindítás pulton
ProgramOnTheWeb=%1 a weben
UninstallProgram=A(z) %1 eltávolítása
LaunchProgram=A(z)%1 &indítása
AssocFileExtension=A(z) %1 &társítása a %2 kiterjesztésû fájlokkal
AssocingFileExtension=A(z) %1 &társítása a %2 kiterjesztésû fájlokkal...

;Things we can also localize
CompanyName=Thingamahoochie Software

;Types
TypicalInstallation=Szokásos Telepítés
FullInstallation=TeljesTelepítés
CompactInstallation=Kompakt Telepítés
CustomInstallation=Egyedi Telepítés

;Components
AppCoreFiles=A WinMerge kéreg fájljai
ApplicationRuntimes=Az alkalmazás futásidejû fájljai
UsersGuide=Felhasználói kézikönyv
Filters=Szûrõk
Plugins=Bõvítmények (Lásd a Plugins.txt fájlt)

;Localization Components
Languages=Nyelvek
BulgarianLanguage=Bolgár menük és párbeszédablakok
CatalanLanguage=Katalán menük és párbeszédablakok
ChineseSimplifiedLanguage=Kínai (Egyszerûsített) menük és párbeszédablakok
ChineseTraditionalLanguage=Kynai (Hagyományos) menük és párbeszédablakok
CzechLanguage=Cseh menük és párbeszédablakok
DanishLanguage=Dán menük és párbeszédablakok
DutchLanguage=Holland menük és párbeszédablakok
FrenchLanguage=Francia menük és párbeszédablakok
GermanLanguage=Német menük és párbeszédablakok
HungarianLanguage=Hungarian menus and dialogs
ItalianLanguage=Olasz menük és párbeszédablakok
JapaneseLanguage=Japán menük és párbeszédablakok
KoreanLanguage=Koreai menük és párbeszédablakok
NorwegianLanguage=Norvég menük és párbeszédablakok
PolishLanguage=Lengyel menük és párbeszédablakok
PortugueseLanguage=Portugál (Brazíliai) menük és párbeszédablakok
RussianLanguage=Orosz menük és párbeszédablakok
SlovakLanguage=Szlovák menük és párbeszédablakok
SpanishLanguage=Spanyol menük és párbeszédablakok
SwedishLanguage=Svéd menük és párbeszédablakok

;Tasks
ExplorerContextMenu=Integrálás az Intézõ &context-menüjéhez
IntegrateTortoiseCVS=Integrálás a &TortoiseCVS programmal
IntegrateDelphi4=Borland® Delphi &4 bináris fájl támogatás
UpdatingCommonControls=Updating the System's Common Controls

;Icon Labels
ReadMe=Olvass el
UsersGuide=Felhasználói kézikönyv
UpdatingCommonControls=Updating the System's Common Controls
ViewStartMenuFolder=A WinMerge Start Menü mappájának &megtekintése

;Code Dialogs
DeletePreviousStartMenu=A Telepítés észlelte, hogy megváltoztattad a Start Menü helyét errõl: "%s" erre: "%s". Szeretnéd eltávolítani a korábbi Start Menü mappát?
