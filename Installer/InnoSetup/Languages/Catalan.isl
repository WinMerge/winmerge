; *** Inno Setup version 4 Catalan messages ***
;
; To download user-contributed translations of this file, go to:
;   http://www.jrsoftware.org/is3rdparty.php
;
; Note: When translating this text, do not add periods (.) to the end of
; messages that didn't have them already, because on those messages Inno
; Setup adds the periods automatically (appending a period would result in
; two periods being displayed).
;
; Traduït per Carles Millan, Barcelona, http://carlesmillan.com (30-01-04)

[LangOptions]

LanguageName=Català
LanguageID=$0403

[Messages]

; *** Application titles
SetupAppTitle=Instal·lació
SetupWindowTitle=Instal·lació - %1
UninstallAppTitle=Desinstal·lació
UninstallAppFullTitle=Desinstal·lar %1

; *** Misc. common
InformationTitle=Informació
ConfirmTitle=Confirmació
ErrorTitle=Error

; *** SetupLdr messages
SetupLdrStartupMessage=Aquest programa instal·larà %1. Voleu continuar?
LdrCannotCreateTemp=No s'ha pogut crear un fitxer temporal. Instal·lació cancel·lada
LdrCannotExecTemp=No s'ha pogut executar el fitxer a la carpeta temporal. Instal·lació cancel·lada

; *** Startup error messages
LastErrorMessage=%1.%n%nError %2: %3
SetupFileMissing=El fitxer %1 no es troba a la carpeta d'instal·lació. Si us plau, resoleu el problema o obteniu una nova còpia del programa.
SetupFileCorrupt=Els fitxers d'instal·lació estan corromputs. Si us plau, obteniu una nova còpia del programa.
SetupFileCorruptOrWrongVer=Els fitxers d'instal·lació estan espatllats, o són incompatibles amb aquesta versió del programa. Si us plau, resoleu el problema o obteniu una nova còpia del programa.
NotOnThisPlatform=Aquest programa no funcionarà sota %1.
OnlyOnThisPlatform=Aquest programa només pot ser executat sota %1.
WinVersionTooLowError=Aquest programa requereix %1 versió %2 o posterior.
WinVersionTooHighError=Aquest programa no pot ser instal·lat sota %1 versió %2 o posterior.
AdminPrivilegesRequired=Cal que tingueu privilegis d'administrador per poder instal·lar aquest programa.
PowerUserPrivilegesRequired=Cal que accediu com a administrador o com a membre del grup Power Users en instal·lar aquest programa.
SetupAppRunningError=El programa d'instal·lació ha detectat que %1 s'està executant actualment.%n%nSi us plau, tanqueu el programa i premeu Següent per a continuar o Cancel·lar per a sortir.
UninstallAppRunningError=El programa de desinstal·lació ha detectat que %1 s'està executant en aquest moment.%n%nSi us plau, tanqueu el programa i premeu Següent per a continuar o Cancel·lar per a sortir.

; *** Misc. errors
ErrorCreatingDir=El programa d'instal·lació no ha pogut crear la carpeta "%1"
ErrorTooManyFilesInDir=No s'ha pogut crear un fitxer a la carpeta "%1" perquè conté massa fitxers

; *** Setup common messages
ExitSetupTitle=Sortir
ExitSetupMessage=La instal·lació no s'ha completat. Si sortiu ara, el programa no serà instal·lat.%n%nPer a completar-la podreu tornar a executar el programa d'instal·lació quan vulgueu.%n%nVoleu sortir-ne?
AboutSetupMenuItem=&Sobre la instal·lació...
AboutSetupTitle=Sobre la instal·lació
AboutSetupMessage=%1 versió %2%n%3%n%nPàgina web de %1:%n%4
AboutSetupNote=

; *** Buttons
ButtonBack=< &Tornar
ButtonNext=&Següent >
ButtonInstall=&Instal·lar
ButtonOK=Següent
ButtonCancel=Cancel·lar
ButtonYes=&Sí
ButtonYesToAll=Sí a &Tot
ButtonNo=&No
ButtonNoToAll=N&o a tot
ButtonFinish=&Finalitzar
ButtonBrowse=&Explorar...

; *** "Select Language" dialog messages
ButtonWizardBrowse=&Cercar...
ButtonNewFolder=Crear &nova carpeta
SelectLanguageTitle=Seleccioneu idioma
SelectLanguageLabel=Seleccioneu idioma a emprar durant la instal·lació:

; *** Common wizard text
ClickNext=Premeu Següent per a continuar o Cancel·lar per a abandonar la instal·lació.
BeveledLabel=

; *** "Welcome" wizard page
BrowseDialogTitle=Trieu una carpeta
BrowseDialogLabel=Trieu la carpeta de destinació i premeu Següent.
NewFolderName=Nova carpeta
WelcomeLabel1=Benvingut a l'assistent d'instal·lació de [name]
WelcomeLabel2=Aquest programa instal·larà [name/ver] al vostre ordinador.%n%nÉs molt recomanable que abans de continuar tanqueu tots els altres programes oberts, per tal d'evitar conflictes durant el procés d'instal·lació.

; *** "Password" wizard page
WizardPassword=Codi d'accés
PasswordLabel1=Aquesta instal·lació està protegida amb un codi d'accés.
PasswordLabel3=Indiqueu el codi d'accés i premeu Següent per a continuar. Aquest codi distingeix entre majúscules i minúscules.
PasswordEditLabel=&Codi:
IncorrectPassword=El codi introduït no és correcte. Torneu-ho a intentar.

; *** "License Agreement" wizard page
WizardLicense=Acceptació de la llicencia d'ús.
LicenseLabel=Cal que llegiu aquesta informació abans de continuar.
LicenseLabel3=Si us plau, llegiu l'Acord de Llicència següent. Cal que n'accepteu els termes abans de continuar amb la instal·lació.
LicenseAccepted=&Accepto l'acord
LicenseNotAccepted=&No accepto l'acord

; *** "Information" wizard pages
WizardInfoBefore=Informació
InfoBeforeLabel=Si us plau, llegiu la informació següent abans de continuar.
InfoBeforeClickLabel=Quan estigueu preparat per a continuar, premeu Següent
WizardInfoAfter=Informació
InfoAfterLabel=Si us plau, llegiu la informació següent abans de continuar.
InfoAfterClickLabel=Quan estigueu preparat per a continuar, premeu Següent

; *** "User Information" wizard page
WizardUserInfo=Informació sobre l'usuari
UserInfoDesc=Si us plau, entreu la vostra informació.
UserInfoName=&Nom de l'usuari:
UserInfoOrg=&Organització
UserInfoSerial=&Número de sèrie:
UserInfoNameRequired=Cal que hi entreu un nom

; *** "Select Destination Directory" wizard page
WizardSelectDir=Trieu Carpeta de Destinació
SelectDirDesc=On s'ha d'instal·lar [name]?
SelectDirLabel3=Setup will install [name] into the following folder.
DiskSpaceMBLabel=Aquest programa necessita un mínim de [mb] MB d'espai a disc.
ToUNCPathname=La instal·lació no pot instal·lar el programa en una carpeta UNC. Si esteu  provant d'instal·lar-lo en xarxa, haureu d'assignar una lletra (D:, E:, etc...) al disc de destinació.
InvalidPath=Cal donar una ruta completa amb lletra d'unitat, per exemple:%n%nC:\Aplicació%n%no bé una ruta UNC en la forma:%n%n\\servidor\compartit
InvalidDrive=El disc o ruta de xarxa seleccionat no existeix, si us plau trieu-ne un altre.
DiskSpaceWarningTitle=No hi ha prou espai al disc
DiskSpaceWarning=El programa d'instal·lació necessita com a mínim %1 KB d'espai lliure, però el disc seleccionat només té %2 KB disponibles.%n%nTot i amb això, desitgeu continuar?
DirNameTooLong=El nom de la carpeta o de la ruta és massa llarg.
InvalidDirName=El nom de la carpeta no és vàlid.
BadDirName32=Un nom de carpeta no pot contenir cap dels caràcters següents:%n%n%1
DirExistsTitle=La carpeta existeix
DirExists=La carpeta:%n%n%1%n%nja existeix. Voleu instal·lar igualment el programa en aquesta carpeta?
DirDoesntExistTitle=La Carpeta No Existeix
DirDoesntExist=La carpeta:%n%n%1%n%nno existeix. Voleu que sigui creada?

; *** "Select Program Group" wizard page
WizardSelectComponents=Trieu Components
SelectComponentsDesc=Quins components cal instal·lar?
SelectComponentsLabel2=Seleccioneu els components que voleu instal·lar; elimineu els components que no voleu instal·lar. Premeu Següent per a continuar.
FullInstallation=Instal·lació completa
; if possible don't translate 'Compact' as 'Minimal' (I mean 'Minimal' in your language)
CompactInstallation=Instal·lació compacta
CustomInstallation=Instal·lació personalitzada
NoUninstallWarningTitle=Els components Existeixen
NoUninstallWarning=El programa d'instal·lació ha detectat que els components següents ja es troben al vostre ordinador:%n%n%1%n%nSi no estan seleccionats no seran desinstal·lats.%n%nVoleu continuar igualment?
ComponentSize1=%1 Kb
ComponentSize2=%1 Mb
ComponentsDiskSpaceMBLabel=Aquesta selecció requereix un mínim de [mb] Mb d'espai al disc.

; *** "Select Additional Tasks" wizard page
WizardSelectTasks=Trieu tasques addicionals
SelectTasksDesc=Quines tasques addicionals cal executar?
SelectTasksLabel2=Trieu les tasques addicionals que voleu que siguin executades mentre s'instal·la [name], i després premeu Següent.

; *** "Select Start Menu Folder" wizard page
WizardSelectProgramGroup=Trieu la carpeta del Menú Inici
SelectStartMenuFolderDesc=On cal situar els enllaços del programa?
SelectStartMenuFolderLabel3=Setup will create the program's shortcuts in the following Start Menu folder.
SelectStartMenuFolderBrowseLabel=To continue, click Next. If you would like to select a different folder, click Browse.
SelectDirBrowseLabel=To continue, click Next. If you would like to select a different folder, click Browse.
NoIconsCheck=&No crear cap icona
MustEnterGroupName=Cal que hi entreu un nom de carpeta.
GroupNameTooLong=El nom de la carpeta o de la ruta és massa llarg.
InvalidGroupName=El nom de la carpeta no és vàlid.
BadGroupName=El nom del grup no pot contenir cap dels caràcters següents:%n%n%1
NoProgramGroupCheck2=&No crear una carpeta al Menú Inici

; *** "Ready to Install" wizard page
WizardReady=Preparat per a instal·lar
ReadyLabel1=El programa d'instal·lació està preparat per a iniciar la instal·lació de [name] al vostre ordinador.
ReadyLabel2a=Premeu Instal·lar per a continuar amb la instal·lació, o Tornar si voleu revisar o modificar les opcions d'instal·lació.
ReadyLabel2b=Premeu Instal·lar per a continuar amb la instal·lació.
ReadyMemoUserInfo=Informació de l'usuari:
ReadyMemoDir=Carpeta de destinació:
ReadyMemoType=Tipus d'instal·lació:
ReadyMemoComponents=Components seleccionats:
ReadyMemoGroup=Carpeta del Menú Inici:
ReadyMemoTasks=Tasques addicionals:

; *** "Preparing to Install" wizard page
WizardPreparing=Preparant la instal·lació
PreparingDesc=Preparant la instal·lació de [name] al vostre ordinador.
PreviousInstallNotCompleted=La instal·lació o desinstal·lació anterior no s'ha dut a terme. Caldrà que reinicieu l'ordinador per a finalitzar aquesta instal·lació.%n%nDesprés de reiniciar l'ordinador, executeu aquest programa de nou per completar la instal·lació de [name].
CannotContinue=La instal·lació no pot continuar. Si us plau, premeu Cancel·lar per a sortir.

; *** "Installing" wizard page
WizardInstalling=Instal·lant
InstallingLabel=Si us plau, espereu mentre s'instal·la [name] al vostre ordinador.

; *** "Setup Completed" wizard page
FinishedHeadingLabel=Completant l'assistent d'instal·lació de [name]
FinishedLabelNoIcons=El programa ha finalitzat la instal·lació de [name] al vostre ordinador.
FinishedLabel=El programa ha finalitzat la instal·lació de [name] al vostre ordinador. L'aplicació pot ser iniciada seleccionant les icones instal·lades.
ClickFinish=Premeu Finalitzar per a sortir de la instal·lació.
FinishedRestartLabel=Per a completar la instal·lació de [name] cal reiniciar l'ordinador. Voleu fer-ho ara?
FinishedRestartMessage=Per a completar la instal·lació de [name] cal reiniciar l'ordinador. Voleu fer-ho ara?
ShowReadmeCheck=Sí, vull visualitzar el fitxer LLEGIUME.TXT
YesRadio=&Sí, reiniciar l'ordinador ara
NoRadio=&No, reiniciaré l'ordinador més tard
; used for example as 'Run MyProg.exe'
RunEntryExec=Executar %1
; used for example as 'View Readme.txt'
RunEntryShellExec=Visualitzar %1

; *** "Setup Needs the Next Disk" stuff
ChangeDiskTitle=El programa d'instal·lació necessita el disc següent
SelectDiskLabel2=Si us plau, introduiu el disc %1 i premeu Continuar.%n%nSi els fitxers d'aquest disc es poden trobar en una carpeta diferent de la indicada tot seguit, entreu-ne la ruta correcta o bé premeu Explorar.
PathLabel=&Ruta:
FileNotInDir2=El fitxer "%1" no s'ha pogut trobar a "%2". Si us plau, introduïu el disc correcte o trieu una altra carpeta.
SelectDirectoryLabel=Si us plau, indiqueu on es troba el disc següent.

; *** Installation phase messages
SetupAborted=La instal·lació no s'ha completat.%n%n%Si us plau, resoleu el problema i executeu de nou el programa d'instal·lació.
EntryAbortRetryIgnore=Premeu Reintentar per a intentar-ho de nou, Ignorar per a continuar igualment, o Cancel·lar per a abandonar la instal·lació.

; *** Installation status messages
StatusCreateDirs=Creant carpetes...
StatusExtractFiles=Extraient fitxers...
StatusCreateIcons=Creant enllaços del programa...
StatusCreateIniEntries=Creant entrades al fitxer INI...
StatusCreateRegistryEntries=Creant entrades de registre...
StatusRegisterFiles=Registrant fitxers...
StatusSavingUninstall=Desant informació de desinstal·lació...
StatusRunProgram=Finalitzant la instal·lació...
StatusRollback=Desfent els canvis...

; *** Misc. errors
ErrorInternal2=Error intern: %1
ErrorFunctionFailedNoCode=%1 ha fallat
ErrorFunctionFailed=%1 ha fallat; codi %2
ErrorFunctionFailedWithMessage=%1 ha fallat; codi %2.%n%3
ErrorExecutingProgram=No es pot executar el fitxer:%n%1

; *** Registry errors
ErrorRegOpenKey=Error en obrir la clau de registre:%n%1\%2
ErrorRegCreateKey=Error en crear la clau de registre:%n%1\%2
ErrorRegWriteKey=Error en escriure a la clau de registre:%n%1\%2

; *** INI errors
ErrorIniEntry=Error en crear l'entrada INI al fitxer "%1".

; *** File copying errors
FileAbortRetryIgnore=Premeu Reintentar per a intentar-ho de nou, Ignorar per a saltar-se aquest fitxer (no recomanat), o Cancel·lar per a abandonar la instal·lació.
FileAbortRetryIgnore2=Premeu Reintentar per a intentar-ho de nou, Ignorar per a continuar igualment (no recomanat), o Cancel·lar per a abandonar la instal·lació.
SourceIsCorrupted=El fitxer d'origen està corromput
SourceDoesntExist=El fitxer d'origen "%1" no existeix
ExistingFileReadOnly=El fitxer és de només lectura.%n%nPremeu Reintentar per a treure-li l'atribut de només lectura i tornar-ho a intentar, Ometre per a saltar-se'l (no recomanat), o Anul·lar per a abandonar la instal·lació.
ErrorReadingExistingDest=S'ha produït un error en llegir el fitxer:
FileExists=El fitxer ja existeix.%n%nVoleu que sigui sobre-escrit?
ExistingFileNewer=El fitxer existent és més nou que el que s'intenta instal·lar. Es recomana mantenir el fitxer existent.%n%nVoleu mantenir-lo?
ErrorChangingAttr=Hi ha hagut un error en canviar els atributs del fitxer:
ErrorCreatingTemp=Hi ha hagut un error en crear un fitxer a la carpeta de destinació:
ErrorReadingSource=Hi ha hagut un error en llegir el fitxer d'origen:
ErrorCopying=Hi ha hagut un error en copiar un fitxer:
ErrorReplacingExistingFile=Hi ha hagut un error en reemplaçar el fitxer existent:
ErrorRestartReplace=Ha fallar reemplaçar:
ErrorRenamingTemp=Hi ha hagut un error en reanomenar un fitxer a la carpeta de destinació:
ErrorRegisterServer=No s'ha pogut registrar el DLL/OCX: %1
ErrorRegisterServerMissingExport=No s'ha trobat l'exportador DllRegisterServer
ErrorRegisterTypeLib=No s'ha pogut registrar la biblioteca de tipus: %1

; *** Post-installation errors
ErrorOpeningReadme=Hi ha hagut un error en obrir el fitxer LLEGIUME.TXT.
ErrorRestartingComputer=El programa d'instal·lació no ha pogut reiniciar l'ordinador. Si us plau, feu-ho manualment.

; *** Uninstaller messages
UninstallNotFound=El fitxer "%1" no existeix. No es pot desinstal·lar.
UninstallOpenError=El fitxer "%1" no pot ser obert. No es pot desinstal·lar.
UninstallUnsupportedVer=El fitxer de desinstal·lació "%1" està en un format no reconegut per aquesta versió del desinstal·lador. No es pot desinstal·lar
UninstallUnknownEntry=S'ha trobat una entrada desconeguda (%1) al fitxer de desinstal·lació.
ConfirmUninstall=Esteu segur de voler eliminar completament %1 i tots els seus components?
OnlyAdminCanUninstall=Aquest programa només pot ser desinstal·lat per un usuari amb privilegis d'administrador.
UninstallStatusLabel=Si us plau, espereu mentre s'elimina %1 del vostre ordinador.
UninstalledAll=%1 ha estat desinstal·lat correctament del vostre ordinador.
UninstalledMost=Desinstal·lació de %1 completada.%n%nAlguns elements no s'han pogut eliminar. Poden ser eliminats manualment.
UninstalledAndNeedsRestart=Per completar la instal·lació de %1, cal reiniciar el vostre ordinador.%n%nVoleu fer-ho ara?
UninstallDataCorrupted=El fitxer "%1" està corromput. No es pot desinstal·lar.

; *** Uninstallation phase messages
ConfirmDeleteSharedFileTitle=Eliminar fitxer compartit?
ConfirmDeleteSharedFile2=El sistema indica que el fitxer compartit següent ja no és emprat per cap altre programa. Voleu que la desinstal·lació elimini aquest fitxer?%n%nSi algun programa encara el fa servir i és eliminat, podria no funcionar correctament. Si no n'esteu segur, trieu No. Deixar el fitxer al sistema no farà cap mal.
SharedFileNameLabel=Nom del fitxer:
SharedFileLocationLabel=Localització:
WizardUninstalling=Estat de la desinstal·lació
StatusUninstalling=Desinstal·lant %1...

; The custom messages below aren't used by Setup itself, but if you make
; use of them in your scripts, you'll want to translate them.

[CustomMessages]
;Inno Setup Built-in Custom Messages
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