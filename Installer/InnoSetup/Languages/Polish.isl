; *** Inno Setup version 4.1.8+ Polish messages ***
; S³awomir Adamski <sadam@wa.onet.pl>
;
; To download user-contributed translations of this file, go to:
;   http://www.jrsoftware.org/is3rdparty.php
;
; Note: When translating this text, do not add periods (.) to the end of
; messages that didn't have them already, because on those messages Inno
; Setup adds the periods automatically (appending a period would result in
; two periods being displayed).
;
; $jrsoftware: issrc/Files/Default.isl,v 1.53 2004/02/25 01:55:24 jr Exp $


[LangOptions]
LanguageName=Polski
LanguageID=$0415
; If the language you are translating to requires special font faces or
; sizes, uncomment any of the following entries and change them accordingly.
;DialogFontName=MS Shell Dlg
;DialogFontSize=8
;WelcomeFontName=Verdana
;WelcomeFontSize=12
;TitleFontName=Arial
;TitleFontSize=29
;CopyrightFontName=Arial
;CopyrightFontSize=8

[Messages]

; *** Application titles
SetupAppTitle=Instalator
SetupWindowTitle=Instalacja - %1
UninstallAppTitle=Odinstalowanie
UninstallAppFullTitle=Odinstaluj %1

; *** Misc. common
InformationTitle=Informacja
ConfirmTitle=Potwierdzenie
ErrorTitle=B³¹d

; *** SetupLdr messages
SetupLdrStartupMessage=Ten program zainstaluje aplikacjê %1. Czy chcesz kontynuowaæ?
LdrCannotCreateTemp=Nie mo¿na utworzyæ pliku tymczasowego. Instalacja przerwana
LdrCannotExecTemp=Nie mo¿na uruchomiæ pliku w tymczasowym folderze. Instalacja przerwana

; *** Startup error messages
LastErrorMessage=%1.%n%nB³¹d %2: %3
SetupFileMissing=W folderze Instalatora brakuje pliku %1. Proszê rozwi¹zaæ ten problem lub uzyskaæ now¹ kopiê Instalatora tego programu od producenta.
SetupFileCorrupt=Pliki sk³adowe Instalatora s¹ uszkodzone. Proszê uzyskaæ now¹ kopiê Instalatora od producenta.
SetupFileCorruptOrWrongVer=Pliki sk³adowe instalatora s¹ uszkodzone lub niezgodne z t¹ wersj¹ Instalatora. Proszê rozwi¹zaæ ten problem lub uzyskaæ now¹ kopiê Instalatora od producenta.
NotOnThisPlatform=Ten program nie dzia³a pod %1.
OnlyOnThisPlatform=Ten program musi byæ uruchomiony z %1.
WinVersionTooLowError=Ten program wymaga %1 w wersji %2 lub póŸniejszej.
WinVersionTooHighError=Ten program nie mo¿e byæ zainstalowany w wersji %2 lub póŸniejszej systemu %1.
AdminPrivilegesRequired=Musisz mieæ uprawnienia administratora aby instalowaæ ten program.
PowerUserPrivilegesRequired=Musisz mieæ uprawnienia administratora lub uzytkownika zaawansowanego aby instalowaæ ten program.
SetupAppRunningError=Instalator wykry³, ¿e jest uruchomiony %1.%n%nZamknij teraz wszystkie okienka tej aplikacji, a potem wybierz przycisk OK, aby kontynuowaæ, lub Anuluj, aby przerwaæ instalacjê.
UninstallAppRunningError=Program odinstalowuj¹cy wykry³, ¿e jest uruchomiony %1.%n%nZamknij teraz wszystkie okna tej aplikacji, a potem wybierz przycisk OK, aby kontynuowaæ, lub Anuluj, aby przerwaæ odinstalowywanie.

; *** Misc. errors
ErrorCreatingDir=Instalator nie móg³ utworzyæ folderu "%1"
ErrorTooManyFilesInDir=Nie mo¿na utworzyæ pliku w folderze %1, poniewa¿ zawiera on za du¿o plików

; *** Setup common messages
ExitSetupTitle=Zakoñcz instalacjê
ExitSetupMessage=Instalacja nie zosta³a zakoñczona. Jeœli zakoñczysz j¹ teraz, aplikacja nie bêdzie zainstalowana.%n%nJeœli chcesz dokoñczyæ instalacjê innym razem, uruchom ponownie Instalatora.%n%nZakoñczyc instalacjê?
AboutSetupMenuItem=&O Instalatorze...
AboutSetupTitle=O Instalatorze
AboutSetupMessage=%1 wersja %2%n%3%n%n Strona domowa %1:%n%4
AboutSetupNote=Polska wersja: Rafa³ P³atek, S³awomir Adamski

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
ButtonNewFolder=&Stwórz nowy folder

; *** "Select Language" dialog messages
SelectLanguageTitle=Wybierz jêzyk instalacji
SelectLanguageLabel=Wybierz jêzyk u¿ywany w czasie instalacji:

; *** Common wizard text
ClickNext=Wybierz przycisk Dalej, aby kontynuowaæ, lub Anuluj, aby zakoñczyæ instalacjê.
BeveledLabel=
BrowseDialogTitle=Wska¿ folder
BrowseDialogLabel=Wybierz folder z poni¿szej listy, a potem wybierz przycisk OK.
NewFolderName=Nowy folder

; *** "Welcome" wizard page
WelcomeLabel1=Witamy w Kreatorze instalacji programu [name].
WelcomeLabel2=Instalator zainstaluje teraz program [name/ver] na Twoim komputerze.%n%nZalecamy zakoñczenie pracy z wszystkimi innymi przez Ciebie uruchomionymi aplikacjami przed rozpoczêciem instalacji.

; *** "Password" wizard page
WizardPassword=Has³o
PasswordLabel1=Ta instalacja jest zabezpieczona has³em.
PasswordLabel3=Podaj has³o, potem wybierz przycisk Dalej, aby kontynuowaæ. W has³ach s¹ rozró¿niane du¿e i ma³e litery.
PasswordEditLabel=&Has³o:
IncorrectPassword=Wprowadzone has³o nie jest poprawne. Spróbuj ponownie.

; *** "License Agreement" wizard page
WizardLicense=Umowa Licencyjna
LicenseLabel=Przed kontynuacj¹ proszê uwa¿nie przeczytaæ tê informacjê.
LicenseLabel3=Proszê przeczytaæ tekst Umowy Licencyjnej. Musisz zgodziæ siê na warunki tej umowy przed kontynuacj¹ instalacji.
LicenseAccepted=&Akceptujê warunki umowy
LicenseNotAccepted=&Nie akceptujê warunków umowy

; *** "Information" wizard pages
WizardInfoBefore=Informacja
InfoBeforeLabel=Przed przejœciem do dalszego etapu instalacji, proszê przeczytaæ t¹ wa¿n¹ informacjê.
InfoBeforeClickLabel=Kiedy bêdziesz gotowy do instalacji, wybierz przycisk Dalej.
WizardInfoAfter=Informacja
InfoAfterLabel=Przed przejœciem do dalszego etapu instalacji, proszê przeczytaæ t¹ wa¿n¹ informacjê.
InfoAfterClickLabel=Gdy bêdziesz gotowy do zakoñczenia instalacji, wybierz przycisk Dalej.

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
SelectDirLabel3=Instalator zainstaluje  [name] do poni¿szego folderu.
SelectDirBrowseLabel=Wybierz przycisk Nastêpny, aby kontynuowaæ. Jeœli chcesz okreœliæ iny folder, wybierz przycisk Przegl¹daj.
DiskSpaceMBLabel=Potrzeba przynajmniej [mb] MB wolnego miejsca na dysku.
ToUNCPathname=Instalator nie mo¿e instalowaæ do œcie¿ki UNC. Jeœli próbujesz instalowaæ program na dysku sieciowym, najpierw zmapuj ten dysk.
InvalidPath=Musisz wprowadziæ pe³n¹ œcie¿kê wraz z liter¹ dysku, np.:%nC:\PROGRAM
InvalidDrive=Wybrany dysk nie istnieje. Proszê wybraæ inny.
DiskSpaceWarningTitle=Niewystarczaj¹ca iloœæ wolnego miejsca na dysku
DiskSpaceWarning=Instalator wymaga co najmniej %1 KB wolnego miejsca na dysku. Wybrany dysk posiada tylko %2 KB dostêpnego miejsca.%n%nCzy mimo to chcesz kontynuowaæ?
DirNameTooLong=Nazwa folderu lub œcie¿ki jest za d³uga.
InvalidDirName=Niepoprawna nazwa folderu.
BadDirName32=Nazwa folderu nie mo¿e zawieraæ ¿adnego z nastêpuj¹cych znaków po dwukropku:%n%n%1
DirExistsTitle=Ten folder ju¿ istnieje
DirExists=Folder%n%n%1%n%nju¿ istnieje. Czy chcesz zainstalowaæ program w tym folderze?
DirDoesntExistTitle=Nie ma takiego folderu
DirDoesntExist=Folder:%n%n%1%n%nnie istnieje. Czy chcesz, aby zosta³ utworzony?

; *** "Select Components" wizard page
WizardSelectComponents=Zaznacz sk³adniki
SelectComponentsDesc=Które komponenty maj¹ byæ zainstalowane?
SelectComponentsLabel2=Zaznacz sk³adniki, które chcesz zainstalowaæ, odznacz te, których nie chcesz zainstalowaæ. Wybierz przycisk Dalej, aby kontynuowaæ.
FullInstallation=Instalacja pe³na
; if possible don't translate 'Compact' as 'Minimal' (I mean 'Minimal' in your language)
CompactInstallation=Instalacja podstawowa
CustomInstallation=Dopasowanie instalacji
NoUninstallWarningTitle=Zainstalowane sk³adniki
NoUninstallWarning=Instalator wykry³, ¿e w twoim komputerze s¹ ju¿ zainstalowane nastêpuj¹ce sk³adniki:%n%n%1%n%nOdznaczenie ich nie spowoduje odinstalowania.%n%nCzy mimo tego chcesz kontynuowaæ?
ComponentSize1=%1 KB
ComponentSize2=%1 MB
ComponentsDiskSpaceMBLabel=Wybrane sk³adniki wymagaj¹ co najmniej [mb] MB na dysku.

; *** "Select Additional Tasks" wizard page
WizardSelectTasks=Zaznacz dodatkowe zadania
SelectTasksDesc=Które dodatkowe zadania maj¹ byæ wykonane?
SelectTasksLabel2=Zaznacz dodatkowe zadania, które Instalator ma wykonaæ podczas instalacji programu [name], potem wybierz przycisk Dalej, aby kontynuowaæ.

; *** "Select Start Menu Folder" wizard page
WizardSelectProgramGroup=Wybierz folder Menu Start
SelectStartMenuFolderDesc=Gdzie maj¹ byæ umieszczone skróty do programu?
SelectStartMenuFolderLabel3=Instalator stworzy skróty do programu w poni¿szym folderze Menu Start.
SelectStartMenuFolderBrowseLabel=Wybierz przycisk Nastêpny, aby kontynuowaæ. Jeœli chcesz okreœliæ iny folder, wybierz przycisk Przegl¹daj.
NoIconsCheck=&Nie twórz ¿adnych skrótów
MustEnterGroupName=Musisz wprowadziæ nazwê folderu.
GroupNameTooLong=Nazwa folderu lub œcie¿ki jest za d³uga.
InvalidGroupName=Niepoprawna nazwa folderu.
BadGroupName=Nazwa folderu nie mo¿e zawieraæ ¿adnego z nastêpuj¹cych znaków:%n%n%1
NoProgramGroupCheck2=&Nie twórz folderu w Menu Start

; *** "Ready to Install" wizard page
WizardReady=Gotowy do rozpoczêcia instalacji
ReadyLabel1=Instalator jest ju¿ gotowy do rozpoczêcia instalacji programu [name] na twoim komputerze.
ReadyLabel2a=Wybierz przycisk Instaluj, aby rozpocz¹æ instalacjê lub Wstecz, jeœli chcesz przejrzeæ lub zmieniæ ustawienia.
ReadyLabel2b=Wybierz przycisk Instaluj, aby kontynuowaæ instalacjê.
ReadyMemoUserInfo=Informacje u¿ytkownika:
ReadyMemoDir=Lokalizacja docelowa:
ReadyMemoType=Rodzaj instalacji:
ReadyMemoComponents=Wybrane sk³adniki:
ReadyMemoGroup=Folder w Menu Start:
ReadyMemoTasks=Dodatkowe zadania:

; *** "Preparing to Install" wizard page
WizardPreparing=Przygotowanie do instalacji
PreparingDesc=Instalator przygotowuje siê do instalacji [name] na Twoim komputerze.
PreviousInstallNotCompleted=Instalacja (usuniêcie) poprzedniej wersji programu nie zosta³a zakoñczona. Bêdziesz musia³ ponownie uruchomiæ komputer, aby zakoñczyæ instalacjê. %n%nPo ponownym uruchomieniu komputera uruchom ponownie instalatora, aby zakoñczyæ instalacjê [name].
CannotContinue=Instalator nie mo¿e kontynuowaæ. Wybierz przycisk Anuluj, aby przerwaæ instalacjê.


; *** "Installing" wizard page
WizardInstalling=Instalowanie
InstallingLabel=Poczekaj, a¿ instalator zainstaluje [name] na Twoim komputerze.

; *** "Setup Completed" wizard page
FinishedHeadingLabel=Zakoñczono instalacjê [name]
FinishedLabelNoIcons=Instalator zakoñczy³ instalacjê programu [name] na Twoim komputerze.
FinishedLabel=Instalator zakoñczy³ instalacjê programu [name] na Twoim komputerze. Aplikacja mo¿e byæ wywo³ana poprzez u¿ycie zainstalowanych skrótów.
ClickFinish=Wybierz przycisk Zakoñcz, aby zakoñczyæ instalacjê.
FinishedRestartLabel=Aby zakoñczyæ instalacjê programu [name], Instalator musi ponownie uruchomiæ Twój komputer. Czy chcesz teraz ponownie uruchomiæ swój komputer?
FinishedRestartMessage=Aby zakoñczyæ instalacjê programu [name], Instalator musi ponownie uruchomiæ Twój komputer.%n%nCzy chcesz teraz ponownie uruchomiæ swój komputer?
ShowReadmeCheck=Tak, chcê przeczytaæ dodatkowe informacje
YesRadio=&Tak, teraz uruchom ponownie
NoRadio=&Nie, sam zrestartujê póŸniej
; used for example as 'Run MyProg.exe'
RunEntryExec=Uruchom %1
; used for example as 'View Readme.txt'
RunEntryShellExec=Poka¿ %1

; *** "Setup Needs the Next Disk" stuff
ChangeDiskTitle=Instalator potrzebuje nastêpnej dyskietki
SelectDiskLabel2=Proszê w³o¿yæ dyskietkê %1 i wybraæ przycisk OK.%n%nJeœli pokazany poni¿ej folder nie okreœla po³o¿enia plików z tej dyskietki, wprowadŸ poprawn¹ œcie¿kê lub wybierz przycisk Przegl¹daj.
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
StatusRegisterFiles=Rejestrowanie plików...
StatusSavingUninstall=Zachowanie informacji o odinstalowywaniu...
StatusRunProgram=Koñczenie instalacji...
StatusRollback=Wycofywanie zmian...

; *** Misc. errors
ErrorInternal2=Wewnêtrzny b³¹d: %1
ErrorFunctionFailedNoCode=B³¹d podczas wykonywania %1
ErrorFunctionFailed=B³¹d podczas wykonywania %1; kod %2
ErrorFunctionFailedWithMessage=B³¹d podczas wykonywania %1; code %2.%n%3
ErrorExecutingProgram=Nie mogê uruchomiæ:%n%1

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
ErrorChangingAttr=Podczas próby zmiany atrybutów istniej¹cego pliku wyst¹pi³ b³¹d:
ErrorCreatingTemp=Podczas próby utworzenia pliku w folderze docelowym wyst¹pi³ b³¹d:
ErrorReadingSource=Podczas próby odczytu pliku Ÿród³owego wyst¹pi³ b³¹d:
ErrorCopying=Podczas próby kopiowania pliku wyst¹pi³ b³¹d:
ErrorReplacingExistingFile=Podczas próby zamiany istniej¹cego pliku wyst¹pi³ b³¹d:
ErrorRestartReplace=B³¹d RestartReplace:
ErrorRenamingTemp=Podczas próby zmiany nazwy pliku w folderze docelowym wyst¹pi³ b³¹d:
ErrorRegisterServer=Nie mo¿na zarejestrowaæ DLL/OCX: %1
ErrorRegisterServerMissingExport=Eksportowana funkcja DllRegisterServer nie zosta³a znaleziony
ErrorRegisterTypeLib=Nie mogê zarejestrowaæ biblioteki typów: %1

; *** Post-installation errors
ErrorOpeningReadme=Wyst¹pi³ b³¹d podczas próby otwarcia pliku README.
ErrorRestartingComputer=Instalator nie móg³ zrestartowaæ tego komputera. Proszê zrobiæ to samodzielnie.

; *** Uninstaller messages
UninstallNotFound=Plik "%1" nie istnieje. Nie mo¿na go odinstalowaæ.
UninstallOpenError=Plik "%1" nie móg³ byæ otwarty. Nie mo¿na odinstalowaæ
UninstallUnsupportedVer=Ta wersja programu odinstalowuj¹cego nie rozpoznaje formatu logu deinstalacji. Nie mo¿na odinstalowaæ
UninstallUnknownEntry=W logu deinstalacji wyst¹pi³a nie znana pozycja (%1)
ConfirmUninstall=Czy na pewno chcesz usun¹æ program %1 i wszystkie jego sk³adniki?
OnlyAdminCanUninstall=Ta instalacja mo¿e byæ odinstalowana tylko przez u¿ytkownika z prawami administratora.
UninstallStatusLabel=Poczekaj a¿ %1 zostanie usuniêty z Twojego komputera.
UninstalledAll=%1 zosta³ usuniêty z Twojego komputera.
UninstalledMost=Odinstalowywanie programu %1 zakoñczone.%n%nNiektóre elementy nie mog³y byæ usuniête. Mo¿esz je usun¹æ rêcznie.
UninstalledAndNeedsRestart=Twój komputer musi byæ ponownie uruchomiony, aby zakoñczyæ odinstalowywanie %1.%n%nCzy chcesz teraz ponownie uruchomiæ komputer?
UninstallDataCorrupted=Plik "%1" jest uszkodzony. Nie mo¿na odinstalowaæ

; *** Uninstallation phase messages
ConfirmDeleteSharedFileTitle=Usun¹æ plik wspó³dzielony?
ConfirmDeleteSharedFile2=System wykry³, ¿e nastêpuj¹cy plik nie jest ju¿ u¿ywany przez ¿aden program. Czy chcesz odinstalowaæ ten wspó³dzielony plik?%n%nJeœli inne programy nadal u¿ywaj¹ tego pliku, a zostanie on usuniêty, mog¹ one przestaæ dzia³aæ prawid³owo. Jeœli nie jesteœ pewny, wybierz przycisk Nie. Pozostawienie tego pliku w Twoim systemie nie spowoduje ¿adnych szkód.
SharedFileNameLabel=Nazwa pliku:
SharedFileLocationLabel=Po³o¿enie:
WizardUninstalling=Stan odinstalowywania
StatusUninstalling=Odinstalowywanie %1...

; The custom messages below aren't used by Setup itself, but if you make
; use of them in your scripts, you'll want to translate them.

[CustomMessages]

AdditionalIcons=Additional icons:
CreateDesktopIcon=Create a &desktop icon
CreateQuickLaunchIcon=Create a &Quick Launch icon
ProgramOnTheWeb=%1 on the Web
UninstallProgram=Uninstall %1
LaunchProgram=Launch %1
AssocFileExtension=&Associate %1 with the %2 file extension
AssocingFileExtension=Associating %1 with the %2 file extension...

;Things we can also localize
English.CompanyName=Thingamahoochie Software

;Types
English.TypicalInstallation=Typical Installation
English.FullInstallation=Full Installation
English.CompactInstallation=Compact Installation
English.CustomInstallation=Custom Installation

;Components
English.AppCoreFile=%1 Core Files
English.ApplicationRuntimes=Application Runtimes
English.UsersGuide=User's Guide
English.Filters=Filters
English.Plugins=Plugins (enhance core behavior)

;Localization Components
English.BulgarianLanguage=Bulgarian menus and dialogs
English.CatalanLanguage=Catalan menus and dialogs
English.ChineseSimplifiedLanguage=Chinese(Simplified) menus and dialogs
English.ChineseTraditionalLanguage=Chinese (Traditional) menus and dialogs
English.CzechLanguage=Czech menus and dialogs
English.DanishLanguage=Danish menus and dialogs
English.DutchLanguage=Dutch menus and dialogs
English.FrenchLanguage=French menus and dialogs
English.GermanLanguage=German menus and dialogs
English.ItalianLanguage=Italian menus and dialogs
English.KoreanLanguage=Korean menus and dialogs
English.NorwegianLanguage=Norwegian menus and dialogs
English.PolishLanguage=Polish menus and dialogs
English.PortugueseLanguage=Portuguese (Brazillian) menus and dialogs
English.RussianLanguage=Russian menus and dialogs
English.SlovakLanguage=Slovak menus and dialogs
English.SpanishLanguage=Spanish menus and dialogs
English.SwedishLanguage=Swedish menus and dialogs

;Tasks
English.ExplorerContextMenu=&Enable Explorer context menu integration
English.IntegrateTortoiseCVS=Integrate with &TortoiseCVS
English.IntegrateDelphi4=Borland® Delphi &4 Binary File Support
English.UpdatingCommonControls=Updating the System's Common Controls

;Icon Labels
English.ReadMe=Read Me
English.UsersGuide=User's Guide
English.UpdatingCommonControls=Updating the System's Common Controls
English.ViewStartMenuFolder=&View the %1 Start Menu Folder