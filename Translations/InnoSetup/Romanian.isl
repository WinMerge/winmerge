; *** Inno Setup version 5.1.11+ Romanian messages ***
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
LanguageName=Romana
LanguageID=$0418
LanguageCodePage=1252

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
SetupAppTitle=Setup
SetupWindowTitle=Setup - %1
UninstallAppTitle=Dezinstalare
UninstallAppFullTitle=%1 Uninstall

; *** Misc. common
InformationTitle=Informare
ConfirmTitle=Confirmati
ErrorTitle=Eroare

; *** SetupLdr messages
SetupLdrStartupMessage=Acesta va instala %1. Doriti sa continuati?
LdrCannotCreateTemp=Nu pot crea fisiere temporare. Abandon instalare.
LdrCannotExecTemp=Eroare executie fisier in directorul temporar. Instalare abandonata

; *** Startup error messages
LastErrorMessage=%1.%n%nEroare %2: %3
SetupFileMissing=Fisierul %1 lipseste din directorul de instalare. Va rog corectati problema sau utilizati o alta copie a programului.
SetupFileCorrupt=Integritatea fisierului de instalare este compromisa. Va rog utilizati o alta copie.
SetupFileCorruptOrWrongVer=Integritatea fisierelor de instalare este compromisa sau acestea nu sunt compatibile cu aceasta versiune de Setup. Corectati problema sau utilizati o noua copie de program.
NotOnThisPlatform=Acest program nu ruleaza pe %1.
OnlyOnThisPlatform=Acest program trebuie rulat pe %1.
OnlyOnTheseArchitectures=Acest program poate fi instalat pe versiuni de Windows ce permit urmatoarele arhitecturi de procesor:%n%n%1
MissingWOW64APIs=Aceasta versiune de Windows nu include functionalitatea ceruta de Setup pentru a proceda la o instalare pe 64 de biti. Pentru a corecta problema, instalati Service Pack %1.
WinVersionTooLowError=Acest program necesita %1 versiunea %2 or mai noua.
WinVersionTooHighError=Acest program nu poate fi instalat pe %1 versiunea %2 sau mai noua.
AdminPrivilegesRequired=Trebuie sa aveti drepturi de Administrator cand faceti instalarea
PowerUserPrivilegesRequired=Trebuie sa fiti conectat la sistem ca Administrator ori ca membru al grupului "Power Users" atunci cand instalati acest program
SetupAppRunningError=Setup a detectat ca aplicatia %1 este activa.%n%nInchideti TOATE instantele acum, apoi apasati OK pentru a continua sau <Termina> pentru a abandona.
UninstallAppRunningError=Dezinstalatorul a detectat ca aplicatia %1 este activa.%n%nInchideti TOATE instantele acum, apoi apasati OK pentru a continua sau <Termina> pentru a abandona.

; *** Misc. errors
ErrorCreatingDir=Setup nu poate crea directorul  "%1"
ErrorTooManyFilesInDir=Nu poate fi creat un fisier in directorul "%1" deoarece acesta contine prea multe fisiere

; *** Setup common messages
ExitSetupTitle=Iesire din Setup
ExitSetupMessage=Setup nu este complet. Daca iesiti acum, programul nu va fi instalat.%n%nPuteti rula acest Setup din nou oricand pentru a finaliza instalarea.%n%nTerminati Setup?
AboutSetupMenuItem=&Despre Setup...
AboutSetupTitle=Despre Setup
AboutSetupMessage=%1 versiunea %2%n%3%n%n%1 home page:%n%4
AboutSetupNote=
TranslatorNote=

; *** Buttons
ButtonBack=< &Inapoi
ButtonNext=&Inainte >
ButtonInstall=&Instaleaza
ButtonOK=OK
ButtonCancel=Abandon
ButtonYes=&Da
ButtonYesToAll=Da la &Toate
ButtonNo=&Nu
ButtonNoToAll=N&u la Toate
ButtonFinish=&Terminare
ButtonBrowse=&Cautare...
ButtonWizardBrowse=C&autare...
ButtonNewFolder=&Creare Folder nou

; *** "Select Language" dialog messages
SelectLanguageTitle=Selectare limba Setup
SelectLanguageLabel=Selectati limba folosita pe timpul instalarii:

; *** Common wizard text
ClickNext=Apasati <Inainte> pentru a continua sau <Abandon> pentru a iesi din Setup.
BeveledLabel=
BrowseDialogTitle=Cautare director
BrowseDialogLabel=Selectati un folder din lista de mai jos, apoi apasati OK.
NewFolderName=Folder nou

; *** "Welcome" wizard page
WelcomeLabel1=Bine ati venit in cadrul ghidului de instalare a aplicatiei [name]
WelcomeLabel2=Acest Setup va instala [name/ver] pe calculatorul dvs.%n%nSe recomanda sa inchideti TOATE celelalte aplicatii inainte de a continua instalarea.

; *** "Password" wizard page
WizardPassword=Parola
PasswordLabel1=Aceasta instalare este protejata prin parola.
PasswordLabel3=Va rog introduceti parola, apoi apasati <Inainte> pentru a continua. ATENTIE!: Parola este diferita daca se utilizeaza aldine sau nu.
PasswordEditLabel=&Parola:
IncorrectPassword=Parola introdusa este eronata. Va rog incercati din nou.

; *** "License Agreement" wizard page
WizardLicense=Conventie de Licentiere
LicenseLabel=Va rog cititi cu atentie informatia care urmeaza.Este foarte important inainte de a continua.
LicenseLabel3=Va rog cititi cu atentie urmatoarea Conventie de Licentiere. Inainte de a continua procesul de instalare, trebuie sa acceptati termenii acestei Conventii.
LicenseAccepted=A&ccept conditiile
LicenseNotAccepted=N&u accept conditiile

; *** "Information" wizard pages
WizardInfoBefore=Informare
InfoBeforeLabel=Va rog cititi cu atentie informatia care urmeaza.Este foarte important inainte de a continua.
InfoBeforeClickLabel=Cand sunteti pregatit pentru a continua Setup, apasati <Inainte>.
WizardInfoAfter=Informare
InfoAfterLabel=Va rog cititi cu atentie informatia care urmeaza.Este foarte important inainte de a continua.
InfoAfterClickLabel=Cand sunteti pregatit pentru a continua apasati <Inainte>.

; *** "User Information" wizard page
WizardUserInfo=Informatii DESPRE utilizator
UserInfoDesc=Introduceti informatiile dvs.
UserInfoName=&Nume Utilizator:
UserInfoOrg=&Organizatia:
UserInfoSerial=&Numar Serial:
UserInfoNameRequired=Trebuie sa introduceti un Nume.

; *** "Select Destination Location" wizard page
WizardSelectDir=Selectati Folder-ul destinatie
SelectDirDesc=Unde ar trebui instalata aplicatia [name] ?
SelectDirLabel3=Setup va instala aplicatia [name] in folder-ul de mai jos.
SelectDirBrowseLabel=Pentru a continua, apasati <Inainte>. Daca doriti sa selectati un alt folder, apasati <Cautare>.
DiskSpaceMBLabel=Cel putin [mb] MB spatiu liber este necesar pentru o instalare sigura.
ToUNCPathname=Setup nu poate instala pe o cale descrisa UNC. Daca incercati sa instalati pe retea, trebuie mai intai sa asignati o litera de disc caii dorite utilizand c-da "map".
InvalidPath=Trebuie sa introduceti o cale completa, cu litera de disc cu tot; de exemplu:%n%nC:\APP%n%nsau o definitie UNC in forma:%n%n\\server\share
InvalidDrive=Discul sau definitia UNC partajata nu sunt accesibile sau nu exista. Faceti alta selectie.
DiskSpaceWarningTitle=Nu exista suficient spatiu disc.
DiskSpaceWarning=Setup necesita cel putin %1 KB de spatiu liber pentru o buna instalare, dar discul selectat are doar %2 KB disponibili.%n%nDoriti sa continuati totusi?
DirNameTooLong=Calea sau numele folder-ului sunt prea lung definite.
InvalidDirName=Numele folder-ului nu este valid.
BadDirName32=Un nume valid de folder nu poate contine urmatoarele caractere:%n%n%1
DirExistsTitle=Folder-ul EXISTA
DirExists=Folder-ul:%n%n%1%n%nDEJA exista. Doriti sa instalati in acest folder ?
DirDoesntExistTitle=Folder-ul NU EXISTA
DirDoesntExist=Folder-ul:%n%n%1%n%nNU EXISTA. Doriti crearea lui?

; *** "Select Components" wizard page
WizardSelectComponents=Selectati componentele
SelectComponentsDesc=Care componente trebuiesc instalate?
SelectComponentsLabel2=Selectati componentele pe care le doriti instalate; deselectati pe cele care trebuiesc ignorate. Apasati <Inainte> cand sunteti pregatit sa continuati.
FullInstallation=Instalare Completa
; if possible don't translate 'Compact' as 'Minimal' (I mean 'Minimal' in your language)
CompactInstallation=Instalare Compacta
CustomInstallation=Instalare Variabila
NoUninstallWarningTitle=Componetele EXISTA
NoUninstallWarning=Setup a detectat ca urmatoarele componente sunt deja instalate pe calculatorul dvs.:%n%n%1%n%nChiar deselectandu-le, acestea nu vor face parte din procesul de dezinstalare.%n%nDoriti sa continuati?
ComponentSize1=%1 KB
ComponentSize2=%1 MB
ComponentsDiskSpaceMBLabel=Selectia curenta necesita cel putin [mb] MB de spatiu liber pe disc.

; *** "Select Additional Tasks" wizard page
WizardSelectTasks=Selectati operatiuni suplimentare
SelectTasksDesc=Ce operatiune suplimentara trebuie efectuata?
SelectTasksLabel2=Selectati operatiunea suplimentara pe care doriti ca Setup sa o execute in timpul instalarii aplicatiei [name], apoi apasati <Inainte>

; *** "Select Start Menu Folder" wizard page
WizardSelectProgramGroup=S-a selectat folder-ul alocat meniului "Start"
SelectStartMenuFolderDesc=Unde ar trebui sa plaseze Setup shortcut-urile aplicatiei?
SelectStartMenuFolderLabel3=Setup va crea shortcut-urile aplicatiei in sub-folder-ul aratat mai jos ce apartine meniului "Start".
SelectStartMenuFolderBrowseLabel=Pentru a continua, apasati <Inainte>. Daca doriti sa selectati un alt folder, apasati <Cautare>.
MustEnterGroupName=Trebuie sa introduceti un nume de folder.
GroupNameTooLong=Calea sau numele folder-ului sunt prea lung definite.
InvalidGroupName=Numele folder-ului nu este corect.
BadGroupName=Numele de folder nu poate contine niciunul din caracterele urmatoare:%n%n%1
NoProgramGroupCheck2=&Nu creea un folder in meniul "Start"

; *** "Ready to Install" wizard page
WizardReady=Gata de instalare
ReadyLabel1=Setup este acum pregatit sa inceapa instalarea aplicatiei [name] pe calculatorul dvs.
ReadyLabel2a=Apasati <Instaleaza> pentru a continua instalarea, sau <Inapoi> pentru a revedea sau a schimba setarile facute.
ReadyLabel2b=Apasati <Instaleaza> pentru a continua procesul de instalare.
ReadyMemoUserInfo=Informatii utilizator:
ReadyMemoDir=Destinatia:
ReadyMemoType=Tipul de Setup:
ReadyMemoComponents=Componente selectate:
ReadyMemoGroup="Start Menu" folder:
ReadyMemoTasks=Aplicatii aditionale:

; *** "Preparing to Install" wizard page
WizardPreparing=Pregatire pentru instalare
PreparingDesc=Setup se pregateste sa instaleze aplicatia [name] pe calculatorul dvs.
PreviousInstallNotCompleted=Instalarea/Dezinstalarea anterioara nu a fost COMPLETA. Este necesar sa reporniti calculatorul pentru a completa acel proces anterior.%n%nDupa repornire, rulati din nou Setup pentru a face o instalare completa a aplicatiei [name].
CannotContinue=Setup nu poate continua. Va rog, apasati <Abandon> pentru a termina executia.

; *** "Installing" wizard page
WizardInstalling=Instalez
InstallingLabel=Va rog asteptati cat timp Setup va instala aplicatia [name] pe calculatorul dvs.

; *** "Setup Completed" wizard page
FinishedHeadingLabel=Ghidul de instalare a aplicatiei [name] este complet
FinishedLabelNoIcons=Setup a terminat de instalat aplicatia [name] pe calculatorul dvs.
FinishedLabel=Setup a terminat de instalat aplicatia [name] pe calculatorul dvs. Aplicatia poate fi lansata selectand icon-urile instalate.
ClickFinish=Apasati <Terminare> pentru a iesi din Setup.
FinishedRestartLabel=Pentru a completa instalarea aplicatiei [name], Setup trebuie sa reporneasca calculatorul dvs. Doriti asta ACUM?
FinishedRestartMessage=Pentru a completa instalarea aplicatiei [name], Setup trebuie sa reporneasca calculatorul dvs.%n%nDoriti asta ACUM?
ShowReadmeCheck=Da, vreau sa citesc informatiile din fisierul README
YesRadio=&Da, reporneste calculatorul ACUM
NoRadio=&NU, am sa-l repornesc eu mai tarziu
; used for example as 'Run MyProg.exe'
RunEntryExec=Ruleaza %1
; used for example as 'View Readme.txt'
RunEntryShellExec=Vizualizare %1

; *** "Setup Needs the Next Disk" stuff
ChangeDiskTitle=Setup are nevoie de urmatorul disc
SelectDiskLabel2=Va rog introduceti Discul %1 si apasati OK%n%nDaca fisierele de pe acest disc pot fi gasite intr-un alt folder decat cel afisat mai jos introduceti calea corecta sau apasati <Cautare>.
PathLabel=&Cale:
FileNotInDir2=Fisierul "%1" nu a putut fi localizat in "%2". Va rog, introduceti discul corect sau selectati un alt folder.
SelectDirectoryLabel=Va rog specificati localizarea discului urmator.

; *** Installation phase messages
SetupAborted=Setup este INCOMPLET.%n%nVa rog, corectati problema si reluati rularea instalarii.
EntryAbortRetryIgnore=Apasati <Retry> pentru a reincerca din nou, <Ignore> pentru a forta continuarea, sau <Abort> pentru a termina procesul de instalare.

; *** Installation status messages
StatusCreateDirs=Creare directoare...
StatusExtractFiles=Extrag fisierele...
StatusCreateIcons=Creez shortcut-urile...
StatusCreateIniEntries=Creez intrarile INI...
StatusCreateRegistryEntries=Creez intrarile in registry sistemului...
StatusRegisterFiles=Inregistrez fisierele...
StatusSavingUninstall=Salvez informatiile pentru dezinstalarea ulterioara...
StatusRunProgram=Inchei instalarea...
StatusRollback=Refac modificarile la starea anterioara instalarii...

; *** Misc. errors
ErrorInternal2=Eroare interna: %1
ErrorFunctionFailedNoCode=%1 a esuat
ErrorFunctionFailed=%1 a esuat; cod %2
ErrorFunctionFailedWithMessage=%1 a esuat; cod %2.%n%3
ErrorExecutingProgram=Imposibil sa execut fisierul:%n%1

; *** Registry errors
ErrorRegOpenKey=Eroare deschidere registry key:%n%1\%2
ErrorRegCreateKey=Eroare creare registry key:%n%1\%2
ErrorRegWriteKey=Eroare scriere registry key:%n%1\%2

; *** INI errors
ErrorIniEntry=Eroare creare intrare INI in fisierul "%1".

; *** File copying errors
FileAbortRetryIgnore=Apasati <Retry> pentru a reincerca din nou, <Ignore> pentru a sari acest fisier (nerecomandat!), sau <Abort> pentru a termina procesul de instalare.
FileAbortRetryIgnore2=Apasati <Retry> pentru a reincerca din nou, <Ignore> pentru a forta continuarea (nerecomandat!), sau <Abort> pentru a termina procesul de instalare.
SourceIsCorrupted=Fisierul sursa este distrus
SourceDoesntExist=Fisierul sursa "%1" NU EXISTA
ExistingFileReadOnly=Fisierul existent este marcat cu atributul "doar citire" (read-only).%n%nApasati <Retry> pentru a elimina marcarea si a incerca din nou, <Ignore> pentru a sari peste acest fisier, sau <Abort> pentru abandonarea instalarii.
ErrorReadingExistingDest=A aparut o eroare in timp ce s-a incercat sa se citesca din fisierul existent:
FileExists=Fisierul deja exista.%n%nDoriti ca Setup sa-l suprascrie?
ExistingFileNewer=Fisierul existent este mai nou decat cel pe care Setup incearca sa-l instaleze. Se recomanda pstrarea celui existent deja.%n%nDoriti sa pastrati fisierul existent?
ErrorChangingAttr=A aparut o eroare in timp ce s-a incercat o schimbare de atribute la fisierul existent:
ErrorCreatingTemp=A aparut o eroare in timp ce s-a incercat sa se creeze un fisier temporar in directorul-destinatie:
ErrorReadingSource=A aparut o eroare in timp ce s-a incercat citirea unui fisier-sursa:
ErrorCopying=A aparut o eroare in timp ce s-a incercat copierea unui fisier:
ErrorReplacingExistingFile=A aparut o eroare in timp ce s-a incercat inlocuirea unui fisier existent:
ErrorRestartReplace=Esec RestartReplace:
ErrorRenamingTemp=A aparut o eroare in timp ce s-a incercat redenumirea unui fisier in directorul-destinatie:
ErrorRegisterServer=Nu pot inregistra DLL-ul / OCX-ul: %1
ErrorRegSvr32Failed=Eroare RegSvr32; exit code %1
ErrorRegisterTypeLib=Nu pot face inregistrarea tipului librariei: %1

; *** Post-installation errors
ErrorOpeningReadme=A aparut o eroare in timp ce s-a incercat deschiderea fisierului README.
ErrorRestartingComputer=Setup nu poate provoca repornirea calculatorului.  Va rog s-o faceti dvs. manual.

; *** Uninstaller messages
UninstallNotFound=Fisierul "%1" nu exista. Nu pot continua dezinstalarea
UninstallOpenError=Fisierul "%1" nu poate fi deschis. Nu pot continua dezinstalarea
UninstallUnsupportedVer=Fisierul-martor al procesului de instalare (log file) "%1" este intr-un format nerecunoscut de aceasta versiune de program de dezinstalare. Nu pot continua dezinstalarea
UninstallUnknownEntry=O intrare de tip necunoscut (%1) a fost intalnita in fisierul de dezinstalare
ConfirmUninstall=Sunteti SIGUR ca doriti sa eliminati complet aplicatia %1 cu toate componentele sale?
UninstallOnlyOnWin64=Aceasta instalare nu poate fi dezinstalata decat pe o masina Windows pe 64 de biti.
OnlyAdminCanUninstall=Aceasta instalare nu poate fi dezinstalata decat de un utilizator cu drepturi de Administrator.
UninstallStatusLabel=Va rog asteptati in timp ce aplicatia %1 este eliminata de pe calculatorul dvs.
UninstalledAll=Aplicatia %1 a fost dezinstalata cu succes de pe calculatorul dvs.
UninstalledMost=Aplicatia %1 a fost eliminata.%n%nCateva elemente n-au putut fi eliminate. Acestea pot fi eliminate manual...
UninstalledAndNeedsRestart=Pentru a completa dezinstalarea aplicatiei %1, calculatorul dvs. trebuie repornit.%n%nDoriti asta ACUM?
UninstallDataCorrupted=Structura fisierului "%1" este compromisa. Nu pot efectua dezinstalarea.

; *** Uninstallation phase messages
ConfirmDeleteSharedFileTitle=Eliminati fisierele partajate?
ConfirmDeleteSharedFile2=Sistemul indica faptul ca urmatoarele fisiere partajate nu mai sunt in utilizare de nici-o alta aplicatie. Doriti ca dezinstalatorul sa elimine aceste fisiere partajate?%n%nDaca vreun program inca mai foloseste vreunul din aceste fisiere si acesta este eliminat, acel program nu va mai functiona corect. Daca sunteti nesigur, selectati <No>. Lasand fisierul in sistem nu cauzeaza pagube.
SharedFileNameLabel=Nume fisier:
SharedFileLocationLabel=Localizare:
WizardUninstalling=Status dezinstalare
StatusUninstalling=Dezinstalez %1...

; The custom messages below aren't used by Setup itself, but if you make
; use of them in your scripts, you'll want to translate them.

[CustomMessages]
;Inno Setup Built-in Custom Messages
NameAndVersion=%1 versiune %2
AdditionalIcons=Icoane adi'ionale:
OptionalFeatures=Caracteristici optionale:
CreateDesktopIcon=Creaza o icoana pe &desktop
CreateQuickLaunchIcon=Creaza o icoana în bara de lansare rapida
ProgramOnTheWeb=%1 pe Web
UninstallProgram=Dezinstaleaza %1
LaunchProgram=Lanseaza %1
AssocFileExtension=&Asociaza %1 cu extensia %2
AssocingFileExtension=Se asociaza %1 cu extensia %2 ...

;Things we can also localize
CompanyName=Thingamahoochie Software

;Types
TypicalInstallation=Instalare tipica
FullInstallation=Instalare completa
CompactInstallation=Instalare compacta
CustomInstallation=Instalare personalizata

;Components
AppCoreFiles=Fisierele WinMerge de baza
Filters=Filtre
Plugins=Plug-in-uri

;Localization Components
Languages=Languages
BulgarianLanguage=Meniuri si dialoguri în Bulgara
CatalanLanguage=Meniuri si dialoguri în  Catalana
ChineseSimplifiedLanguage=Meniuri si dialoguri în  Chineza (Simplificata)
ChineseTraditionalLanguage=Meniuri si dialoguri în Chineza (Traditionala)
CroatianLanguage=Meniuri si dialoguri în Croata
CzechLanguage=Meniuri si dialoguri în  Ceha
DanishLanguage=Meniuri si dialoguri în Daneza
DutchLanguage=Meniuri si dialoguri în Olandeza
FrenchLanguage=Meniuri si dialoguri în Franceza
GalicianLanguage=Galician menus and dialogs
GermanLanguage=Meniuri si dialoguri în Germana
GreekLanguage=Greek menus and dialogs
HungarianLanguage=Meniuri si dialoguri în Maghiara
ItalianLanguage=Meniuri si dialoguri în Italiana
JapaneseLanguage=Meniuri si dialoguri în Japoneza
KoreanLanguage=Meniuri si dialoguri în Coreana
NorwegianLanguage=Meniuri si dialoguri în Norvegiana
PersianLanguage=Persian menus and dialogs
PolishLanguage=Meniuri si dialoguri în  Poloneza
PortugueseBrazilLanguage=Meniuri si dialoguri în Portugheza (Braziliana)
PortugueseLanguage=Meniuri si dialoguri în Portugheza
RomanianLanguage=Meniuri si dialoguri în Româna
RussianLanguage=Meniuri si dialoguri în Rusa
SerbianLanguage=Serbian menus and dialogs
SlovakLanguage=Meniuri si dialoguri în Slovaca
SlovenianLanguage=Slovenian menus and dialogs
SpanishLanguage=Meniuri si dialoguri în Spaniola
SwedishLanguage=Meniuri si dialoguri în Suedeza
TurkishLanguage=Meniuri si dialoguri în Turca
UkrainianLanguage=Ukrainian menus and dialogs

;Tasks
ExplorerContextMenu=&Activeaza integrarea in meniurile contextuale Explorer
IntegrateTortoiseCVS=Integrare cu &TortoiseCVS
IntegrateTortoiseSVN=Integrare cu T&ortoiseSVN
IntegrateClearCase=Integrare cu Rational &ClearCase
AddToPath=&Add WinMerge folder to your system path

;Icon Labels
ReadMe=Citeste
UsersGuide=Ghidul utilizatorului
ViewStartMenuFolder=&Vizualizeaza directorul WinMerge din Start Menu

;Code Dialogs
DeletePreviousStartMenu=Instalatorul a detectat ca ati schimbat locatia meniului dumneavoastra de start de la "%s" la "%s". Doriti sa stergeti directorul din meniul de start anterior?

; Project file description
ProjectFileDesc=Fisier de proiect WinMerge
