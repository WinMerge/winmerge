; *** Inno Setup version 5.1.0+ Spanish messages ***
;
; To download user-contributed translations of this file, go to:
;   http://www.jrsoftware.org/is3rdparty.php
;
; Note: When translating this text, do not add periods (.) to the end of
; messages that didn't have them already, because on those messages Inno
; Setup adds the periods automatically (appending a period would result in
; two periods being displayed).
;
; $jrsoftware: issrc/Files/Default.isl,v 1.66 2005/02/25 20:23:48 mlaan Exp $
; Translated by Emiliano Llano Díaz
; emiliano_d@hotmail.com

[LangOptions]
LanguageName=Espa<00f1>ol
LanguageID=$0c0a
LanguageCodePage=0
; If the language you are translating to requires special font faces or
; sizes, uncomment any of the following entries and change them accordingly.
;DialogFontName=verdana
;DialogFontSize=8
;WelcomeFontName=Verdana
;WelcomeFontSize=12
;TitleFontName=Arial
;TitleFontSize=29
;CopyrightFontName=Arial
;CopyrightFontSize=8

[Messages]

; *** Application titles
SetupAppTitle=Instalar
SetupWindowTitle=Instalar - %1
UninstallAppTitle=Eliminar
UninstallAppFullTitle=Eliminar - %1

; *** Misc. common
InformationTitle=Información
ConfirmTitle=Confirmar
ErrorTitle=Error

; *** SetupLdr messages
SetupLdrStartupMessage=Este programa instalará %1 en su sistema. ¿Desea continuar?
LdrCannotCreateTemp=Imposible crear un archivo temporal. Instalación cancelada
LdrCannotExecTemp=Imposible ejecutar archivo en el directorio temporal. Instalación interrumpida

; *** Startup error messages
LastErrorMessage=%1.%n%nError %2: %3
SetupFileMissing=El archivo %1 no se encuentra en la carpeta de instalación. Por favor corrija el problema u obtenga una nueva copia del programa.
SetupFileCorrupt=Los archivos de instalación están dañados. Por favor obtenga una copia nueva del programa.
SetupFileCorruptOrWrongVer=Los archivos de instalación están dañados, o son incompatibles con esta versión del Instalador. Por favor corrija el problema u obtenga una nueva copia del programa.
NotOnThisPlatform=Este programa no funcionará en un sistema %1.
OnlyOnThisPlatform=Este programa debe ejecutarse en un sistema %1.
OnlyOnTheseArchitectures=Este programa sólo puede instalarse en versiones de Windows diseñadas para las siguientes arquitecturas de procesadores:%n%n%1
MissingWOW64APIs=La versión de Windows que usa no tiene la funcionalidad requerida para realizar instalaciones de 64 bits. Para corregir este problema, por favor instale el Service Pack %1.
WinVersionTooLowError=Este programa requiere %1 versión %2 o posterior.
WinVersionTooHighError=Este programa no puede instalarse en %1 versión %2 o posterior.
AdminPrivilegesRequired=Debe iniciar la sesión como un administrador cuando instale este programa.
PowerUserPrivilegesRequired=Debe ser el administrador o equivalente para poder instalar este programa.
SetupAppRunningError=El Instalador ha detectado que %1 está en ejecución.%n%nPor favor cierre todas sus instancias, luego seleccione Aceptar para continuar, o Cancelar para salir.
UninstallAppRunningError=El Instalador ha detectado que %1 está en ejecución.%n%nPor favor cierre todas sus instancias, luego seleccione Aceptar para continuar, o Cancelar para salir.

; *** Misc. errors
ErrorCreatingDir=Imposible crear la carpeta "%1"
ErrorTooManyFilesInDir=Imposible crear un archivo en la carpeta "%1" porque contiene demasiados archivos

; *** Setup common messages
ExitSetupTitle=Salir de la Instalación
ExitSetupMessage=La instalación no se completó. Si abandona ahora, el programa no se instalará.%n%nPodrá ejecutar de nuevo este programa para completar el proceso.%n%n¿Salir de la Instalación?
AboutSetupMenuItem=&Acerca del Instalador...
AboutSetupTitle=Acerca del Instalador
AboutSetupMessage=%1 versión %2%n%3%n%n%1 página Web:%n%4
AboutSetupNote=
TranslatorNote=


; *** Buttons
ButtonBack=< &Atrás
ButtonNext=&Siguiente >
ButtonInstall=&Instalar
ButtonOK=Aceptar
ButtonCancel=Cancelar
ButtonYes=&Sí
ButtonYesToAll=Sí a &Todo
ButtonNo=&No
ButtonNoToAll=N&o a Todo
ButtonFinish=&Terminar
ButtonBrowse=&Examinar...
ButtonWizardBrowse=E&xaminar...
ButtonNewFolder=&Crear nueva carpeta


; *** "Select Language" dialog messages
SelectLanguageTitle=Seleccione el idioma de la instalación
SelectLanguageLabel=Seleccione el idioma que desea usar durante la instalación:

; *** Common wizard text
ClickNext=Haga clic sobre Siguiente para continuar o sobre Cancelar para salir.
BeveledLabel=
BrowseDialogTitle=Buscar carpeta
BrowseDialogLabel=Seleccione una carpeta de la lista y luego haga clic en Aceptar.
NewFolderName=Nueva carpeta

; *** "Welcome" wizard page
WelcomeLabel1=Bienvenido a la instalación de [name].
WelcomeLabel2=Este programa instalará [name/ver] en su computadora.%n%nSe recomienda cerrar todos los programas en ejecución antes de continuar.  Esto ayudará a prevenir conflictos durante el proceso.

; *** "Password" wizard page
WizardPassword=Contraseña
PasswordLabel1=Esta instalación está protegida por contraseña.
PasswordLabel3=Por favor suministre su contraseña.%n%nUse mayúsculas y minúsculas.
PasswordEditLabel=&Contraseña:
IncorrectPassword=La contraseña suministrada no es correcta. Por favor intente de nuevo.

; *** "License Agreement" wizard page
WizardLicense=Acuerdo de Licencia
LicenseLabel=Por favor lea la siguiente información importante antes de continuar.
LicenseLabel3=Por favor lea el siguiente Acuerdo de Licencia.  Use la barra de desplazamiento o presione la tecla Av Pág para ver el resto de la licencia.
LicenseAccepted=&Acepto los términos del Acuerdo
LicenseNotAccepted=&No acepto los términos del Acuerdo

; *** "Information" wizard pages
WizardInfoBefore=Información
InfoBeforeLabel=Por favor lea la siguiente información importante antes de continuar.
InfoBeforeClickLabel=Cuando esté listo para continuar con la instalación, haga clic sobre el botón Siguiente.
WizardInfoAfter=Información
InfoAfterLabel=Por favor lea la siguiente información importante antes de continuar.
InfoAfterClickLabel=Cuando esté listo para continuar, haga clic sobre el botón Siguiente.

; *** "User Information" wizard page
WizardUserInfo=Información de usuario
UserInfoDesc=Por favor proporcione su información.
UserInfoName=Nombre de &usuario:
UserInfoOrg=&Organización:
UserInfoSerial=Número de &Serie:
UserInfoNameRequired=Debe proporcionar su nombre.

; *** "Select Destination Directory" wizard page
WizardSelectDir=Seleccione la Carpeta Destino
SelectDirDesc=¿En dónde se instalará [name]?
SelectDirLabel3=Se instalará [name] en la siguiente carpeta.
SelectDirBrowseLabel=Para continuar haga clic en Siguiente. Si quiere seleccionar otra carpeta, haga clic en Examinar.
DiskSpaceMBLabel=Se requiere un mínimo de [mb] MB de espacio en el disco.
ToUNCPathname=No se puede instalar en un directorio UNC. Si está tratando de instalar en una red, necesitará asignarlo a una unidad de red.
InvalidPath=Debe proporcionar una ruta completa con la letra de la unidad; por ejemplo:%nC:\APP
InvalidDrive=La unidad que seleccionó no existe. Por favor seleccione otra.
DiskSpaceWarningTitle=No hay espacio suficiente en el disco
DiskSpaceWarning=Se requiere al menos %1 KB de espacio libre para la instalación, pero la unidad seleccionada solamente tiene %2 KB disponibles.%n%n¿Desea continuar de todas formas?
DirNameTooLong=El nombre de la carpeta o ruta es demasiado largo.
InvalidDirName=El nombre de la carpeta no es válido.
BadDirName32=El nombre de una carpeta no puede incluir los siguientes caracteres:%n%n%1
DirExistsTitle=La carpeta ya existe
DirExists=La carpeta:%n%n%1%n%nya existe. ¿Desea instalar en esta carpeta de todas formas?
DirDoesntExistTitle=La carpeta no existe
DirDoesntExist=La carpeta:%n%n%1%n%n no existe. ¿Desea crear la carpeta?

; *** "Select Components" wizard page
WizardSelectComponents=Selección de Componentes
SelectComponentsDesc=¿Qué componentes desea instalar?
SelectComponentsLabel2=Seleccione los componentes que desea instalar; desactive aquellos que no desea.  Haga clic sobre Siguiente cuando esté listo para continuar.
FullInstallation=Instalación Completa
; if possible don't translate 'Compact' as 'Minimal' (I mean 'Minimal' in your language)
CompactInstallation=Instalación Compacta
CustomInstallation=Instalación Personalizada
NoUninstallWarningTitle=Componentes Existentes
NoUninstallWarning=La Instalación ha detectado que los siguientes componentes ya están instalados en su computadora:%n%n%1%n%nAl desactivarlos, no se instalarán.%n%n¿Desea continuar de todos modos?
ComponentSize1=%1 KB
ComponentSize2=%1 MB
ComponentsDiskSpaceMBLabel=La selección actual requiere al menos [mb] MB de espacio en disco.

; *** "Select Additional Tasks" wizard page
WizardSelectTasks=Seleccione las Tareas Adicionales
SelectTasksDesc=¿Qué tareas adicionales deberán ejecutarse?
SelectTasksLabel2=Seleccione las tareas adicionales que desea ejecutar mientras se instala [name], luego haga clic sobre el botón Siguiente.

; *** "Select Start Menu Folder" wizard page
WizardSelectProgramGroup=Seleccione la carpeta del Menú de Inicio
SelectStartMenuFolderDesc=¿En dónde deberán colocarse los iconos de acceso directo al programa?
SelectStartMenuFolderLabel3=La instalación creará los atajos del programa en el siguiente menú de Inicio.
SelectStartMenuFolderBrowseLabel=Para continuar haga clic en Siguiente. Si quiere cambiar el menú haga clic en Examinar.
MustEnterGroupName=Debe proporcionar un nombre de carpeta.
GroupNameTooLong=El nombre de la carpeta o ruta es demasiado largo.
InvalidGroupName=Nombre de carpeta inválido.
BadGroupName=El nombre de la carpeta no puede incluir ninguno de los siguientes caracteres:%n%n%1
NoProgramGroupCheck2=&No crear carpeta en el menú Inicio

; *** "Ready to Install" wizard page
WizardReady=Listo para Instalar
ReadyLabel1=El programa está listo para iniciar la instalación de [name] en su computadora.
ReadyLabel2a=Haga clic sobre Instalar para continuar con el proceso o sobre Atrás si desea revisar o cambiar la configuración.
ReadyLabel2b=Haga clic sobre Instalar para continuar con el proceso.
ReadyMemoUserInfo=Información del usuario:
ReadyMemoDir=Carpeta Destino:
ReadyMemoType=Tipo de Instalación:
ReadyMemoComponents=Componentes Seleccionados:
ReadyMemoGroup=Carpeta del Menú de Inicio:
ReadyMemoTasks=Tareas adicionales:

; *** "Preparing to Install" wizard page
WizardPreparing=Preparándose a Instalar 
PreparingDesc=El programa de instalación está preparándose para copiar [name] en su computadora.
PreviousInstallNotCompleted=La instalación/ eliminación previa del programa no se completó. Deberá reiniciar la computadora para completar el proceso.%n%nUna vez reiniciada la computadora ejecute de nuevo este programa para completar la instalación de [name].
CannotContinue=No se pudo continuar con la instalación. Haga clic sobre el botón Cancelar para salir.

; *** "Installing" wizard page
WizardInstalling=Instalando
InstallingLabel=Por favor espere mientras se instala [name] en su computadora.

; *** "Setup Completed" wizard page
FinishedHeadingLabel=Finalizando el asistente de instalación de [name]
FinishedLabelNoIcons=El programa terminó la instalación de [name] en su computadora.
FinishedLabel=El programa terminó la instalación de [name] en su computadora.  El programa puede ejecutarse seleccionando el icono creado.
ClickFinish=Haga clic sobre Terminar para concluir la Instalación.
FinishedRestartLabel=Para completar la instalación de [name], debe reiniciar su computadora.  ¿Desea reiniciar ahora?
FinishedRestartMessage=Para completar la instalación de [name], debe reiniciar su computadora.%n%n¿Desea reiniciar ahora?
ShowReadmeCheck=Sí, deseo ver el archivo LEAME
YesRadio=&Sí, deseo reiniciar la computadora ahora
NoRadio=&No, reiniciaré la computadora más tarde
; used for example as 'Run MyProg.exe'
RunEntryExec=Ejecutar %1
; used for example as 'View Readme.txt'
RunEntryShellExec=Ver %1

; *** "Setup Needs the Next Disk" stuff
ChangeDiskTitle=La Instalación necesita el siguiente disco
SelectDiskLabel2=Por favor inserte el Disco %1 y haga clic sobre Aceptar.%n%nSi los archivos se localizan en una carpeta diferente a la mostrada abajo, proporcione la ruta correcta o haga clic sobre Examinar.
PathLabel=&Ruta:
FileNotInDir2=El archivo "%1" no se encuentra en "%2".  Por favor inserte el disco correcto o seleccione otra carpeta.
SelectDirectoryLabel=Por favor especifique la ubicación del siguiente disco.

; *** Installation phase messages
SetupAborted=La instalación no fue terminada.%n%nPor favor corrija el problema y ejecute Instalar de nuevo.
EntryAbortRetryIgnore=Haga clic sobre Reintentar para intentar de nuevo, Ignorar para proceder de cualquier forma o sobre Cancelar para interrumpir la instalación.

; *** Installation status messages
StatusCreateDirs=Creando carpetas...
StatusExtractFiles=Copiando archivos...
StatusCreateIcons=Creando iconos del programa...
StatusCreateIniEntries=Creando entradas INI...
StatusCreateRegistryEntries=Creando entradas en el registro...
StatusRegisterFiles=Registrando archivos...
StatusSavingUninstall=Guardando información para eliminar el programa...
StatusRunProgram=Terminando la instalación...
StatusRollback=Deshaciendo los cambios...

; *** Misc. errors
ErrorInternal2=Error Interno %1
ErrorFunctionFailedNoCode=%1 falló
ErrorFunctionFailed=%1 falló; código %2
ErrorFunctionFailedWithMessage=%1 falló; código %2.%n%3
ErrorExecutingProgram=Imposible ejecutar el archivo:%n%1

; *** Registry errors
ErrorRegOpenKey=Error abriendo la clave de registro:%n%1\%2
ErrorRegCreateKey=Error creando la clave de registro:%n%1\%2
ErrorRegWriteKey=Error escribiendo en la clave de registro:%n%1\%2

; *** INI errors
ErrorIniEntry=Error creando entrada en archivo INI "%1".

; *** File copying errors
FileAbortRetryIgnore=Haga clic sobre Reintentar para probar de nuevo, Ignorar para omitir este archivo (no recomendado) o Cancelar para interrumpir la instalación.
FileAbortRetryIgnore2=Haga clic sobre Reintentar para probar de nuevo, Ignorar para proceder de cualquier forma (no se recomienda) o Cancelar para interrumpir la instalación.
SourceIsCorrupted=El archivo de origen está dañado
SourceDoesntExist=El archivo de origen "%1" no existe
ExistingFileReadOnly=El archivo existente está marcado como de sólo lectura.%n%nHaga clic sobre Reintentar para quitar el atributo de sólo lectura y probar de nuevo, Ignorar para saltar este archivo o Cancelar para interrumpir la instalación.
ErrorReadingExistingDest=Ocurrió un error al tratar de leer el archivo:
FileExists=El archivo ya existe.%n%n¿Desea sobrescribirlo?
ExistingFileNewer=El archivo existente es más reciente que el que está tratando de instalar.  Se recomienda conservarlo.%n%n¿Desea mantener el archivo existente?
ErrorChangingAttr=Ocurrió un error al tratar de cambiar los atributos del archivo:
ErrorCreatingTemp=Ocurrió un error al tratar de crear un archivo en la carpeta destino:
ErrorReadingSource=Ocurrió un error al tratar de leer el archivo origen:
ErrorCopying=Ocurrió un error al tratar de copiar el archivo:
ErrorReplacingExistingFile=Ocurrió un error al tratar de reemplazar el archivo existente:
ErrorRestartReplace=Falló el reintento de reemplazar:
ErrorRenamingTemp=Ocurrió un error al tratar de renombrar un archivo en la carpeta destino:
ErrorRegisterServer=Imposible registrar la librería DLL/OCX: %1
ErrorRegisterServerMissingExport=No se encontró DllRegisterServer export
ErrorRegisterTypeLib=Imposible registrar la biblioteca de tipo: %1

; *** Post-installation errors
ErrorOpeningReadme=Ocurrió un error al tratar de abrir el archivo LEAME.
ErrorRestartingComputer=El programa de Instalación no puede reiniciar la computadora. Por favor hágalo manualmente.

; *** Uninstaller messages
UninstallNotFound=El archivo "%1" no existe. No se puede eliminar.
UninstallOpenError=No se pudo abrir el archivo "%1". No es posible eliminar el programa
UninstallUnsupportedVer=El archivo de bitácora para eliminar "%1" está en un formato no reconocido por esta versión del programa. No se puede continuar
UninstallUnknownEntry=Se encontró un registro desconocido (%1 en la bitácora de eliminación
ConfirmUninstall=¿Está seguro que desea eliminar completamente %1 y todos sus componentes?
UninstallOnlyOnWin64=Esta instalación sólo puede eliminarse en un sistema Windows de 64 bits.
OnlyAdminCanUninstall=Este programa sólo puede eliminarlo un usuario con privilegios administrativos.
UninstallStatusLabel=Por favor espere mientras se elimina %1 de su computadora.
UninstalledAll=%1 se eliminó con éxito de su computadora.
UninstalledMost=Finalizó la eliminación de %1.%n%nAlgunos elementos no pudieron quitarse. Estos pueden borrarse manualmente.
UninstalledAndNeedsRestart=Para completar la eliminación de %1 se requiere reiniciar la computadora.%n%n¿Desea reiniciarla en este momento?
UninstallDataCorrupted=El archivo "%1" está dañado. No se puede eliminar

; *** Uninstallation phase messages
ConfirmDeleteSharedFileTitle=¿Eliminar Archivos Compartidos?
ConfirmDeleteSharedFile2=El sistema informa que el siguiente archivo compartido no es utilizado por otros programas.  ¿Desea eliminar este archivo?%n%nSi otros programas están usándolo y es eliminado, éstos podrían no funcionar correctamente. Si no está seguro, elija No.  Dejar el archivo en su sistema no causa ningún daño.
SharedFileNameLabel=Nombre del archivo:
SharedFileLocationLabel=Ubicación:
WizardUninstalling=Estado de la Eliminación
StatusUninstalling=Eliminando %1...

; The custom messages below aren't used by Setup itself, but if you make
; use of them in your scripts, you'll want to translate them.

[CustomMessages]
;Inno Setup Built-in Custom Messages
NameAndVersion=%1 versión %2
AdditionalIcons=Iconos adicionales:
OptionalFeatures=Optional Features:
CreateDesktopIcon=Crear un atajo en el &Escritorio
CreateQuickLaunchIcon=Crear un icono de acceso &rápido
ProgramOnTheWeb=%1 en la Web
UninstallProgram=Eliminar %1
LaunchProgram=Ejecutar %1
AssocFileExtension=&Asociar %1 con la extensión de archivo %2
AssocingFileExtension=Asociar %1 con la extensión de archivo %2...

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
IntegrateDelphi4=Borland® Delphi &4 Binary File Support
UpdatingCommonControls=Updating the System's Common Controls

;Icon Labels
ReadMe=Read Me
UsersGuide=User's Guide
UpdatingCommonControls=Updating the System's Common Controls
ViewStartMenuFolder=&View the WinMerge Start Menu Folder

;Code Dialogs
DeletePreviousStartMenu=The installer has detected that you changed the location of your start menu from "%s" to "%s". Would you like to delete the previous start menu folder?

; Project file description
ProjectFileDesc=WinMerge Project file
