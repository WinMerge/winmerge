; *** Inno Setup version 5.1.0+ Polish messages ***
; Krzysztof Cynarski <krzysztof at cynarski.net>
;
; To download user-contributed translations of this file, go to:
;   http://www.jrsoftware.org/is3rdparty.php
;
; Note: When translating this text, do not add periods (.) to the end of
; messages that didn't have them already, because on those messages Inno
; Setup adds the periods automatically (appending a period would result in
; two periods being displayed).
;
; $jrsoftware: issrc/Files/Languages/Polish.isl,v 1.15 2005/02/27 19:09:39 jr Exp $

[LangOptions]
LanguageName=Polski
LanguageID=$0415
LanguageCodePage=1250

[Messages]

; *** Application titles
SetupAppTitle=Instalator
SetupWindowTitle=Instalacja - %1
UninstallAppTitle=Deinstalacja
UninstallAppFullTitle=Odinstaluj %1

; *** Misc. common
InformationTitle=Informacja
ConfirmTitle=PotwierdŸ
ErrorTitle=B³¹d

; *** SetupLdr messages
SetupLdrStartupMessage=Ten program zainstaluje aplikacjê %1. Czy chcesz kontynuowaæ?
LdrCannotCreateTemp=Nie mo¿na utworzyæ pliku tymczasowego. Instalacja przerwana
LdrCannotExecTemp=Nie mo¿na uruchomiæ pliku w folderze tymczasowym. Instalacja przerwana

; *** Startup error messages
LastErrorMessage=%1.%n%nB³¹d %2: %3
SetupFileMissing=W folderze instalacyjnym brak pliku %1.%nProszê usun¹æ problem lub uzyskaæ now¹ kopiê programu instalacyjnego.
SetupFileCorrupt=Pliki sk³adowe Instalatora s¹ uszkodzone. Proszê uzyskaæ now¹ kopiê Instalatora od producenta.
SetupFileCorruptOrWrongVer=Pliki sk³adowe instalatora s¹ uszkodzone lub niezgodne z t¹ wersj¹ Instalatora. Proszê rozwi¹zaæ ten problem lub uzyskaæ now¹ kopiê Instalatora od producenta.
NotOnThisPlatform=Tego programu nie mo¿na uruchomiæ w systemie %1.
OnlyOnThisPlatform=Ten program wymaga systemu %1.
OnlyOnTheseArchitectures=Ten program mo¿e byæ uruchomiony tylko w systemie Windows zaprojektowanym na procesory o architekturach:%n%n%1
MissingWOW64APIs=Ta wersja systemu Windows nie zawiera komponentów niezbêdnych do przeprowadzenia 64 bitowej instalacji. Aby usun¹æ ten problem, proszê zainstalowaæ Service Pack %1.
WinVersionTooLowError=Ten program wymaga %1 w wersji %2 lub póŸniejszej.
WinVersionTooHighError=Ten program nie mo¿e byæ zainstalowany w wersji %2 lub póŸniejszej systemu %1.
AdminPrivilegesRequired=Aby przeprowadziæ instalacjê tego programu, U¿ytkownik musi byæ zalogowany z uprawnieniami administratora.
PowerUserPrivilegesRequired=Aby przeprowadziæ instalacjê tego programu, U¿ytkownik musi byæ zalogowany z uprawnieniami administratora lub u¿ytkownika zaawansowanego.
SetupAppRunningError=Instalator wykry³, ¿e %1 jest aktualnie uruchomiony.%n%nZamknij wszystkie okienka tej aplikacji, a potem wybierz przycisk OK, aby kontynuowaæ, lub Anuluj, aby przerwaæ instalacjê.
UninstallAppRunningError=Deinstalator wykry³, ¿e %1 jest aktualnie uruchomiony.%n%nZamknij teraz wszystkie okna tej aplikacji, a nastêpnie wybierz przycisk OK, aby kontynuowaæ, lub Anuluj, aby przerwaæ deinstalacje.

; *** Misc. errors
ErrorCreatingDir=Instalator nie móg³ utworzyæ foldera "%1"
ErrorTooManyFilesInDir=Nie mo¿na utworzyæ pliku w folderze %1, poniewa¿ zawiera on za du¿o plików

; *** Setup common messages
ExitSetupTitle=Zakoñcz instalacjê
ExitSetupMessage=Instalacja nie jest zakoñczona. Je¿eli przerwiesz j¹ teraz, program nie zostanie zainstalowany. Mo¿na ponowiæ instalacjê póŸniej, uruchamiaj¹c pakiet Instalatora.%n%nCzy chcesz przerwaæ instalacjê ?
AboutSetupMenuItem=&O Instalatorze...
AboutSetupTitle=O Instalatorze
AboutSetupMessage=%1 wersja %2%n%3%n%n Strona domowa %1:%n%4
AboutSetupNote=
TranslatorNote=Wersja Polska: Krzysztof Cynarski%n<krzysztof at cynarski.net>

; *** Buttons
ButtonBack=< &Wstecz
ButtonNext=&Dalej >
ButtonInstall=&Instaluj
ButtonOK=OK
ButtonCancel=Anuluj
ButtonYes=&Tak
ButtonYesToAll=Tak na &wszystkie
ButtonNo=&Nie
ButtonNoToAll=N&ie na wszystkie
ButtonFinish=&Zakoñcz
ButtonBrowse=&Przegl¹daj...
ButtonWizardBrowse=P&rzegl¹daj...
ButtonNewFolder=&Utwórz nowy folder

; *** "Select Language" dialog messages
SelectLanguageTitle=Wybierz jêzyk instalacji
SelectLanguageLabel=Wybierz jêzyk u¿ywany podczas instalacji:

; *** Common wizard text
ClickNext=Wybierz przycisk Dalej, aby kontynuowaæ, lub Anuluj, aby zakoñczyæ instalacjê.
BeveledLabel=
BrowseDialogTitle=Wska¿ folder
BrowseDialogLabel=Wybierz folder z poni¿szej listy, a nastêpnie wybierz przycisk OK.
NewFolderName=Nowy folder

; *** "Welcome" wizard page
WelcomeLabel1=Witamy w Kreatorze instalacji programu [name].
WelcomeLabel2=Instalator zainstaluje teraz program [name/ver] na Twoim komputerze.%n%nZalecane jest zamkniêcie wszystkich innych uruchomionych programów przed rozpoczêciem procesu instalacji.

; *** "Password" wizard page
WizardPassword=Has³o
PasswordLabel1=Ta instalacja jest zabezpieczona has³em.
PasswordLabel3=Podaj has³o, potem wybierz przycisk Dalej, aby kontynuowaæ. W has³ach rozró¿niane s¹ du¿e i ma³e litery.
PasswordEditLabel=&Has³o:
IncorrectPassword=Wprowadzone has³o nie jest poprawne. Spróbuj ponownie.

; *** "License Agreement" wizard page
WizardLicense=Umowa Licencyjna
LicenseLabel=Przed kontynuacj¹ proszê przeczytaæ poni¿sze wa¿ne informacje.
LicenseLabel3=Proszê przeczytaæ tekst Umowy Licencyjnej. Musisz zgodziæ siê na warunki tej umowy przed kontynuacj¹ instalacji.
LicenseAccepted=&Akceptujê warunki umowy
LicenseNotAccepted=&Nie akceptujê warunków umowy

; *** "Information" wizard pages
WizardInfoBefore=Informacja
InfoBeforeLabel=Przed przejœciem do dalszego etapu instalacji, proszê przeczytaæ poni¿sz¹ informacjê.
InfoBeforeClickLabel=Kiedy bêdziesz gotowy do instalacji, kliknij przycisk Dalej.
WizardInfoAfter=Informacja
InfoAfterLabel=Przed przejœciem do dalszego etapu instalacji, proszê przeczytaæ poni¿sz¹ informacjê.
InfoAfterClickLabel=Gdy bêdziesz gotowy do zakoñczenia instalacji, kliknij przycisk Dalej.

; *** "User Information" wizard page
WizardUserInfo=Dane U¿ytkownika
UserInfoDesc=Proszê podaæ swoje dane.
UserInfoName=&Nazwisko:
UserInfoOrg=&Organizacja:
UserInfoSerial=Numer &seryjny:
UserInfoNameRequired=Musisz podaæ nazwisko.

; *** "Select Destination Location" wizard page
WizardSelectDir=Wybierz docelow¹ lokalizacjê
SelectDirDesc=Gdzie ma byæ zainstalowany program [name]?
SelectDirLabel3=Instalator zainstaluje program [name] do poni¿szego folderu.
SelectDirBrowseLabel=Kliknij przycisk Dalej, aby kontynuowaæ. Jeœli chcesz okreœliæ inny folder, kliknij przycisk Przegl¹daj.
DiskSpaceMBLabel=Potrzeba przynajmniej [mb] MB wolnego miejsca na dysku.
ToUNCPathname=Instalator nie mo¿e instalowaæ do œcie¿ki UNC. Jeœli próbujesz zainstalowaæ program na dysku sieciowym, najpierw zmapuj ten dysk.
InvalidPath=Musisz wprowadziæ pe³n¹ œcie¿kê wraz z liter¹ dysku, np.:%n%nC:\PROGRAM%n%nlub scie¿kê sieciow¹ (UNC) w formacie:%n%n\\serwer\udzia³
InvalidDrive=Wybrany dysk lub udostêpniony folder sieciowy nie istnieje. Proszê wybraæ inny.
DiskSpaceWarningTitle=Niewystarczaj¹ca iloœæ wolnego miejsca na dysku
DiskSpaceWarning=Instalator wymaga co najmniej %1 KB wolnego miejsca na dysku. Wybrany dysk posiada tylko %2 KB dostêpnego miejsca.%n%nCzy pomimo to chcesz kontynuowaæ?
DirNameTooLong=Nazwa folderu lub œcie¿ki jest za d³uga.
InvalidDirName=Niepoprawna nazwa folderu.
BadDirName32=Nazwa folderu nie mo¿e zawieraæ ¿adnego z nastêpuj¹cych znaków:%n%n%1
DirExistsTitle=Ten folder ju¿ istnieje
DirExists=Folder%n%n%1%n%nju¿ istnieje. Czy pomimo to chcesz zainstalowaæ program w tym folderze?
DirDoesntExistTitle=Nie ma takiego folderu
DirDoesntExist=Folder:%n%n%1%n%nnie istnieje. Czy chcesz, aby zosta³ utworzony?

; *** "Select Components" wizard page
WizardSelectComponents=Zaznacz komponenty
SelectComponentsDesc=Które komponenty maj¹ byæ zainstalowane?
SelectComponentsLabel2=Zaznacz komponenty, które chcesz zainstalowaæ, odznacz te, których nie chcesz zainstalowaæ. Kliknij przycisk Dalej, aby kontynuowaæ.
FullInstallation=Instalacja pe³na
; if possible don't translate 'Compact' as 'Minimal' (I mean 'Minimal' in your language)
CompactInstallation=Instalacja podstawowa
CustomInstallation=Instalacja u¿ytkownika
NoUninstallWarningTitle=Zainstalowane komponenty
NoUninstallWarning=Instalator wykry³, ¿e w twoim komputerze s¹ ju¿ zainstalowane nastêpuj¹ce komponenty:%n%n%1%n%nOdznaczenie któregokolwiek z nich nie spowoduje ich deinstalacji.%n%nCzy pomimo tego chcesz kontynuowaæ?
ComponentSize1=%1 KB
ComponentSize2=%1 MB
ComponentsDiskSpaceMBLabel=Wybrane komponenty wymagaj¹ co najmniej [mb] MB na dysku.

; *** "Select Additional Tasks" wizard page
WizardSelectTasks=Zaznacz dodatkowe zadania
SelectTasksDesc=Które dodatkowe zadania maj¹ byæ wykonane?
SelectTasksLabel2=Zaznacz dodatkowe zadania, które Instalator ma wykonaæ podczas instalacji programu [name], a nastêpnie kliknij przycisk Dalej, aby kontynuowaæ.

; *** "Select Start Menu Folder" wizard page
WizardSelectProgramGroup=Wybierz folder Menu Start
SelectStartMenuFolderDesc=Gdzie maj¹ byæ umieszczone skróty do programu?
SelectStartMenuFolderLabel3=Instalator stworzy skróty do programu w poni¿szym folderze Menu Start.
SelectStartMenuFolderBrowseLabel=Kliknij przycisk Dalej, aby kontynuowaæ. Jeœli chcesz okreœliæ inny folder, kliknij przycisk Przegl¹daj.
MustEnterGroupName=Musisz wprowadziæ nazwê folderu.
GroupNameTooLong=Nazwa folderu lub œcie¿ki jest za d³uga.
InvalidGroupName=Niepoprawna nazwa folderu.
BadGroupName=Nazwa folderu nie mo¿e zawieraæ ¿adnego z nastêpuj¹cych znaków:%n%n%1
NoProgramGroupCheck2=Nie twórz folderu w &Menu Start

; *** "Ready to Install" wizard page
WizardReady=Gotowy do rozpoczêcia instalacji
ReadyLabel1=Instalator jest ju¿ gotowy do rozpoczêcia instalacji programu [name] na twoim komputerze.
ReadyLabel2a=Kliknij przycisk Instaluj, aby rozpocz¹æ instalacjê lub Wstecz, jeœli chcesz przejrzeæ lub zmieniæ ustawienia.
ReadyLabel2b=Kliknij przycisk Instaluj, aby kontynuowaæ instalacjê.
ReadyMemoUserInfo=Informacje u¿ytkownika:
ReadyMemoDir=Lokalizacja docelowa:
ReadyMemoType=Rodzaj instalacji:
ReadyMemoComponents=Wybrane komponenty:
ReadyMemoGroup=Folder w Menu Start:
ReadyMemoTasks=Dodatkowe zadania:

; *** "Preparing to Install" wizard page
WizardPreparing=Przygotowanie do instalacji
PreparingDesc=Instalator przygotowuje instalacjê programu [name] na Twoim komputerze.
PreviousInstallNotCompleted=Instalacja (usuniêcie) poprzedniej wersji programu nie zosta³a zakoñczona. Bêdziesz musia³ ponownie uruchomiæ komputer, aby zakoñczyæ instalacjê. %n%nPo ponownym uruchomieniu komputera uruchom ponownie instalatora, aby zakoñczyæ instalacjê aplikacji [name].
CannotContinue=Instalator nie mo¿e kontynuowaæ. Kliknij przycisk Anuluj, aby przerwaæ instalacjê.


; *** "Installing" wizard page
WizardInstalling=Instalacja
InstallingLabel=Poczekaj, a¿ instalator zainstaluje aplikacjê [name] na Twoim komputerze.

; *** "Setup Completed" wizard page
FinishedHeadingLabel=Zakoñczono instalacjê programu [name]
FinishedLabelNoIcons=Instalator zakoñczy³ instalacjê programu [name] na Twoim komputerze.
FinishedLabel=Instalator zakoñczy³ instalacjê programu [name] na Twoim komputerze. Aplikacja mo¿e byæ uruchomiona poprzez u¿ycie zainstalowanych skrótów.
ClickFinish=Kliknij przycisk Zakoñcz, aby zakoñczyæ instalacjê.
FinishedRestartLabel=Aby zakoñczyæ instalacjê programu [name], Instalator musi ponownie uruchomiæ Twój komputer. Czy chcesz teraz wykonaæ restart komputera?
FinishedRestartMessage=Aby zakoñczyæ instalacjê programu [name], Instalator musi ponownie uruchomiæ Twój komputer.%n%nCzy chcesz teraz wykonaæ restart komputera?
ShowReadmeCheck=Tak, chcê przeczytaæ dodatkowe informacje
YesRadio=&Tak, teraz uruchom ponownie
NoRadio=&Nie, sam zrestartujê póŸniej
; used for example as 'Run MyProg.exe'
RunEntryExec=Uruchom %1
; used for example as 'View Readme.txt'
RunEntryShellExec=Poka¿ %1

; *** "Setup Needs the Next Disk" stuff
ChangeDiskTitle=Instalator potrzebuje nastêpnej dyskietki
SelectDiskLabel2=Proszê w³o¿yæ dyskietkê %1 i klikn¹æ przycisk OK.%n%nJeœli pokazany poni¿ej folder nie okreœla po³o¿enia plików z tej dyskietki, wprowadŸ poprawn¹ œcie¿kê lub kliknij przycisk Przegl¹daj.
PathLabel=Œ&cie¿ka:
FileNotInDir2=Plik "%1" nie zosta³ znaleziony na dyskietce "%2". Proszê w³o¿yæ w³aœciw¹ dyskietkê lub wybraæ inny folder.
SelectDirectoryLabel=Proszê okreœliæ lokalizacjê nastêpnej dyskietki.

; *** Installation phase messages
SetupAborted=Instalacja nie zosta³a zakoñczona.%n%nProszê rozwi¹zaæ problem i ponownie rozpocz¹æ instalacjê.
EntryAbortRetryIgnore=Mo¿esz ponowiæ nieudan¹ czynnoœæ, zignorowaæ j¹ (nie zalecane) lub przerwaæ instalacjê.

; *** Installation status messages
StatusCreateDirs=Tworzenie folderów...
StatusExtractFiles=Dekompresja plików...
StatusCreateIcons=Tworzenie ikon aplikacji...
StatusCreateIniEntries=Tworzenie zapisów w plikach INI...
StatusCreateRegistryEntries=Tworzenie zapisów w rejestrze...
StatusRegisterFiles=Rejestracja plików...
StatusSavingUninstall=Zachowanie informacji deinstalatora...
StatusRunProgram=Koñczenie instalacji...
StatusRollback=Cofanie zmian...

; *** Misc. errors
ErrorInternal2=Wewnêtrzny b³¹d: %1
ErrorFunctionFailedNoCode=B³¹d podczas wykonywania %1
ErrorFunctionFailed=B³¹d podczas wykonywania %1; kod %2
ErrorFunctionFailedWithMessage=B³¹d podczas wykonywania %1; code %2.%n%3
ErrorExecutingProgram=Nie mo¿na uruchomiæ:%n%1

; *** Registry errors
ErrorRegOpenKey=B³¹d podczas otwierania klucza rejestru:%n%1\%2
ErrorRegCreateKey=B³¹d podczas tworzenia klucza rejestru:%n%1\%2
ErrorRegWriteKey=B³¹d podczas zapisu do klucza rejestru:%n%1\%2

; *** INI errors
ErrorIniEntry=B³¹d podczas tworzenia pozycji w pliku INI: "%1".

; *** File copying errors
FileAbortRetryIgnore=Mo¿esz ponowiæ nieudan¹ czynnoœæ, zignorowaæ j¹, aby omin¹æ ten plik (nie zalecane), lub przerwaæ instalacjê.
FileAbortRetryIgnore2=Mo¿esz ponowiæ nieudan¹ czynnoœæ, zignorowaæ j¹ (nie zalecane) lub przerwaæ instalacjê.
SourceIsCorrupted=Plik Ÿród³owy jest uszkodzony
SourceDoesntExist=Plik Ÿród³owy "%1" nie istnieje
ExistingFileReadOnly=Istniej¹cy plik jest oznaczony jako tylko-do-odczytu.%n%nMo¿esz ponowiæ (aby usun¹æ oznaczenie) zignorowaæ (aby omin¹æ ten plik) lub przerwaæ instalacjê.
ErrorReadingExistingDest=Wyst¹pi³ b³¹d podczas próby odczytu istniej¹cego pliku:
FileExists=Plik ju¿ istnieje.%n%nCzy chcesz, aby Instalator zamieni³ go na nowy?
ExistingFileNewer=Istniej¹cy plik jest nowszy ni¿ ten, który Instalator próbuje skopiowaæ. Zalecanym jest zachowanie istniej¹cego pliku.%n%nCzy chcesz zachowaæ istniej¹cy plik?
ErrorChangingAttr=Wyst¹pi³ b³¹d podczas próby zmiany atrybutów docelowego pliku:
ErrorCreatingTemp=Wyst¹pi³ b³¹d podczas próby utworzenia pliku w folderze docelowym:
ErrorReadingSource=Wyst¹pi³ b³¹d podczas próby odczytu pliku Ÿród³owego:
ErrorCopying=Wyst¹pi³ b³¹d podczas próby kopiowania pliku:
ErrorReplacingExistingFile=Wyst¹pi³ b³¹d podczas próby zamiany istniej¹cego pliku:
ErrorRestartReplace=Próba zast¹pienia plików podczas restartu komputera nie powiod³a siê.
ErrorRenamingTemp=Wyst¹pi³ b³¹d podczas próby zmiany nazwy pliku w folderze docelowym:
ErrorRegisterServer=Nie mo¿na zarejestrowaæ DLL/OCX: %1
ErrorRegisterServerMissingExport=Eksportowana funkcja DllRegisterServer nie zosta³a znaleziona
ErrorRegisterTypeLib=Nie mogê zarejestrowaæ biblioteki typów: %1

; *** Post-installation errors
ErrorOpeningReadme=Wyst¹pi³ b³¹d podczas próby otwarcia pliku README.
ErrorRestartingComputer=Instalator nie móg³ zrestartowaæ tego komputera. Proszê zrobiæ to samodzielnie.

; *** Uninstaller messages
UninstallNotFound=Plik "%1" nie istnieje. Nie mo¿na go odinstalowaæ.
UninstallOpenError=Plik "%1" nie móg³ byæ otwarty. Nie mo¿na odinstalowaæ
UninstallUnsupportedVer=Ta wersja programu deinstalacyjnego nie rozpoznaje formatu logu deinstalacji. Nie mo¿na odinstalowaæ
UninstallUnknownEntry=W logu deinstalacji wyst¹pi³a nieznana pozycja (%1)
ConfirmUninstall=Czy na pewno chcesz usun¹æ program %1 i wszystkie jego sk³adniki?
UninstallOnlyOnWin64=Ten program moze byæ odinstalowany tylo w 64 bitowej wersji systemu Windows.
OnlyAdminCanUninstall=Ta instalacja mo¿e byæ odinstalowana tylko przez u¿ytkownika z prawami administratora.
UninstallStatusLabel=Poczekaj a¿ program %1 zostanie usuniêty z Twojego komputera.
UninstalledAll=%1 zosta³ usuniêty z Twojego komputera.
UninstalledMost=Odinstalowywanie programu %1 zakoñczone.%n%nNiektóre elementy nie mog³y byæ usuniête. Mo¿esz je usun¹æ rêcznie.
UninstalledAndNeedsRestart=Twój komputer musi byæ ponownie uruchomiony, aby zakoñczyæ odinstalowywanie %1.%n%nCzy chcesz teraz ponownie uruchomiæ komputer?
UninstallDataCorrupted=Plik "%1" jest uszkodzony. Nie mo¿na odinstalowaæ

; *** Uninstallation phase messages
ConfirmDeleteSharedFileTitle=Usun¹æ plik wspó³dzielony?
ConfirmDeleteSharedFile2=System wykry³, ¿e nastêpuj¹cy plik nie jest ju¿ u¿ywany przez ¿aden program. Czy chcesz odinstalowaæ ten plik wspó³dzielony?%n%nJeœli inne programy nadal u¿ywaj¹ tego pliku, a zostanie on usuniêty, mog¹ one przestaæ dzia³aæ prawid³owo. Jeœli nie jesteœ pewny, wybierz przycisk Nie. Pozostawienie tego pliku w Twoim systemie nie spowoduje ¿adnych szkód.
SharedFileNameLabel=Nazwa pliku:
SharedFileLocationLabel=Po³o¿enie:
WizardUninstalling=Stan deinstalacji
StatusUninstalling=Deinstalacja %1...

; The custom messages below aren't used by Setup itself, but if you make
; use of them in your scripts, you'll want to translate them.

[CustomMessages]
;Inno Setup Built-in Custom Messages
NameAndVersion=%1 wersja %2
AdditionalIcons=Dodatkowe ikony:
OptionalFeatures=Optional Features:
CreateDesktopIcon=Utwórz ikonê na &pulpicie
CreateQuickLaunchIcon=Utwórz ikonê na pasku &szybkiego uruchamiania
ProgramOnTheWeb=Strona WWW programu %1
UninstallProgram=Deinstalacja programu %1
LaunchProgram=Uruchom program %1
AssocFileExtension=&Przypisz program %1 do rozszerzenia pliku %2
AssocingFileExtension=Przypisywanie programu %1 do rozszerzenia pliku %2...

;Things we can also localize
CompanyName=Thingamahoochie Software

;Types
TypicalInstallation=Typowa instalacja
FullInstallation=Pelna instalacja
CompactInstallation=Ograniczona instalacja
CustomInstallation=Instalacja uzytkownika

;Components
AppCoreFiles=WinMerge ? pliki zasadnicze
ApplicationRuntimes=Moduly wykonywalne
UsersGuide=Przewodnik uzytkownika
Filters=Filtry
Plugins=Wtyczki

;Localization Components
Languages=Jezyki
BulgarianLanguage=Bulgarski interfejs
CatalanLanguage=Katalonski interfejs
ChineseSimplifiedLanguage=Chinski (uproszczony) interfejs
ChineseTraditionalLanguage=Chinski (tradycyjny) interfejs
CzechLanguage=Czeski interfejs
DanishLanguage=Dunski interfejs
DutchLanguage=Holenderski interfejs
FrenchLanguage=Francuski interfejs
GermanLanguage=Niemiecki interfejs
HungarianLanguage=Wegierski interfejs
ItalianLanguage=Wloski interfejs
JapaneseLanguage=Japonski interfejs
KoreanLanguage=Koreanski interfejs
NorwegianLanguage=Norweski interfejs
PolishLanguage=Polski interfejs
PortugueseLanguage=Portugalski (Brazylijski) interfejs
RussianLanguage=Rosyjski interfejs
SlovakLanguage=Slowacki interfejs
SpanishLanguage=Hiszpanski interfejs
SwedishLanguage=Szwedzki interfejs
TurkishLanguage=turecki interfejs

;Tasks
ExplorerContextMenu=&Wlacz integracje z menu Eksploratora
IntegrateTortoiseCVS=Integracja z &TortoiseCVS
IntegrateTortoiseSVN=Integrate with T&ortoiseSVN
IntegrateDelphi4=Obsluga plików binarnych Borland® Delphi &4
UpdatingCommonControls=Aktualizacja wspólnych kontrolek systemowych SCC

;Icon Labels
ReadMe=Czytaj to
UsersGuide=Przewodnik uzytkownika
UpdatingCommonControls=Aktualizacja wspólnych kontrolek systemowych
ViewStartMenuFolder=&Podglad katalogu menu Start programu WinMerge
PluginsText=Plugins

;Code Dialogs
DeletePreviousStartMenu=Instalator wykryl, ze zmieniles lokalizacje menu startowego z "%s" na "%s". Czy usunac poprzedni katalog menu startowego?

; Project file description
ProjectFileDesc=WinMerge Project file
