; *** Inno Setup version 4.1.4+ Spanish messages ***
;
; Note: When translating this text, do not add periods (.) to the end of
; messages that didn't have them already, because on those messages Inno
; Setup adds the periods automatically (appending a period would result in
; two periods being displayed).
;
; $Id$
;
; Versión 2: Traducida al Español por Germán Giraldo G. - Colombia
; e-mail: tripleg@tutopia.com
;
; Versión 3+: Adaptado al Español (España) por Jordi Latorre - España
;               e-mail: jlatorref@eic.ictnet.es
;             y Ximo Tamarit - España
;               e-mail: tamarit@mail.ono.es
;

[LangOptions]
LanguageName=Español (Traditional Sort)
LanguageID=$040a
; Si el lenguaje al cual está traduciendo requiere un tipo de letra o
; tamaño, quite el comentario de alguna de las siguientes entradas y cámbielas según el caso.
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
SetupAppTitle=Instalar
SetupWindowTitle=Instalar - %1
UninstallAppTitle=Desinstalar
UninstallAppFullTitle=Desinstalar - %1

; *** Misc. common
InformationTitle=Información
ConfirmTitle=Confirmar
ErrorTitle=Error

; *** SetupLdr messages
SetupLdrStartupMessage=Se instalará %1. ¿Desea continuar?
LdrCannotCreateTemp=No se ha podido crear el archivo temporal. Se cancela la instalación
LdrCannotExecTemp=No se ha podido ejecutar el archivo en el directorio temporal. Se cancela la instalación

; *** Startup error messages
LastErrorMessage=%1.%n%nError %2: %3
SetupFileMissing=No se encuentra el archivo %1 de la carpeta de instalación. Por favor, corrija el problema u obtenga una copia nueva del programa.
SetupFileCorrupt=Los archivos de instalación están dañados. Por favor, obtenga una copia nueva del programa.
SetupFileCorruptOrWrongVer=Los archivos de instalación están dañados, o son incompatibles con su versión de la instalación. Por favor, corrija el problema u obtenga una copia nueva del programa.
NotOnThisPlatform=Este programa no se ejecutará en %1.
OnlyOnThisPlatform=Este programa debe ejecutarse en %1.
WinVersionTooLowError=Este programa requiere %1 versión %2 o posterior.
WinVersionTooHighError=Este programa no puede instalarse en %1 versión %2 o posterior.
AdminPrivilegesRequired=Debe iniciar la sesión como administrador para instalar este programa.
PowerUserPrivilegesRequired=Debe iniciar la sesión como administrador o miembro del grupo Usuarios Avanzados para instalar este programa.
SetupAppRunningError=La instalación ha detectado que %1 se está ejecutando actualmente.%n%nPor favor, ciérrelo ahora, luego haga clic en Aceptar para continuar, o Cancelar para salir.
UninstallAppRunningError=La desinstalación ha detectado que %1 se está ejecutando actualmente.%n%nPor favor, ciérrelo ahora, luego haga clic en Aceptar para continuar, o Cancelar para salir.

; *** Misc. errors
ErrorCreatingDir=Imposible crear la carpeta "%1"
ErrorTooManyFilesInDir=Imposible crear un archivo en la carpeta "%1" porque contiene demasiados archivos.

; *** Setup common messages
ExitSetupTitle=Salir de la Instalación
ExitSetupMessage=La instalación no se ha completado. Si abandona ahora, el programa no quedará instalado.%n%nPodrá ejecutar de nuevo el programa de instalación para completarla.%n%n¿Salir de la Instalación?
AboutSetupMenuItem=&Acerca de Instalar...
AboutSetupTitle=Acerca de Instalar
AboutSetupMessage=%1 versión %2%n%3%n%n%1 página web:%n%4
AboutSetupNote=

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
ButtonWizardBrowse=&Examinar...
ButtonNewFolder=C&rear Carpeta...

; *** "Select Language" dialog messages
SelectLanguageTitle=Elija el idioma de instalación
SelectLanguageLabel=Elija el idioma que se usará durante la instalación:

; *** Common wizard text
ClickNext=Haga clic en Siguiente para continuar, Cancelar para salir.
BeveledLabel=
BrowseDialogTitle=Buscar Carpeta
BrowseDialogLabel=Seleccione una Carpeta de la siguiente lista, haga clic en Aceptar.
NewFolderName=Nueva Carpeta

; *** "Welcome" wizard page
WelcomeLabel1=Bienvenido a la instalación de [name].
WelcomeLabel2=Este programa instalará [name/ver] en su sistema.%n%nSe recomienda que cierre todas las demás aplicaciones antes de continuar.

; *** "Password" wizard page
WizardPassword=Contraseña
PasswordLabel1=Esta instalación está protegida.
PasswordLabel3=Por favor, suministre su contraseña, haga clic en Siguiente para continuar. Las contraseñas diferencian entre mayúsculas y minúsculas.
PasswordEditLabel=&Contraseña:
IncorrectPassword=La contraseña suministrada no es correcta. Por favor, inténtelo de nuevo.

; *** "License Agreement" wizard page
WizardLicense=Contrato de Licencia
LicenseLabel=Por favor, lea la siguiente información importante antes de continuar.
LicenseLabel3=Por favor, lea detenidamente el siguiente contrato de licencia. Debe de aceptar los términos de este contrato antes de continuar con la instalación.
LicenseAccepted=A&cepto el contrato
LicenseNotAccepted=&No acepto el contrato

; *** "Information" wizard pages
WizardInfoBefore=Información
InfoBeforeLabel=Por favor, lea la siguiente información importante antes de continuar.
InfoBeforeClickLabel=Cuando esté listo para continuar con la instalación, haga clic en Siguiente.
WizardInfoAfter=Información
InfoAfterLabel=Por favor, lea la siguiente información importante antes de continuar.
InfoAfterClickLabel=Cuando esté listo para continuar, haga clic en Siguiente.

; *** "User Information" wizard page
WizardUserInfo=Información de usuario
UserInfoDesc=Por favor, introduzca su información.
UserInfoName=Nombre de &Usuario:
UserInfoOrg=&Empresa:
UserInfoSerial=Número de &Serie:
UserInfoNameRequired=Debe de introducir un nombre.

; *** "Select Destination Location" wizard page
WizardSelectDir=Seleccione la Carpeta Destino
SelectDirDesc=¿Dónde debe instalarse [name]?
SelectDirLabel3=Setup will install [name] into the following folder.
SelectDirBrowseLabel=To continue, click Next. If you would like to select a different folder, click Browse.
DiskSpaceMBLabel=Se requieren al menos [mb] MB de espacio libre en el disco.
ToUNCPathname=No se puede instalar en un directorio UNC. Si está tratando de instalar en una red, necesitará mapear una unidad de la red.
InvalidPath=Debe introducir una ruta completa con la letra de unidad; por ejemplo:%n%nC:\APP%n%no una ruta UNC de la siguiente forma:%n%n\\servidor\compartido
InvalidDrive=La unidad o ruta UNC que seleccionó no existe o no es accesible. Por favor, seleccione otra.
DiskSpaceWarningTitle=No hay suficiente espacio en el disco
DiskSpaceWarning=Se requiere al menos %1 KB de espacio libre para la instalación, pero la unidad seleccionada solamente tiene %2 KB disponibles.%n%n¿Desea continuar?
DirNameTooLong=El nombre de la carpeta o su ruta es demasiado largo.
InvalidDirName=El nombre de la carpeta no es válido.
BadDirName32=Los nombres de carpeta no pueden incluir ninguno de los siguientes caracteres:%n%n%1
DirExistsTitle=La Carpeta Existe
DirExists=La carpeta:%n%n%1%n%nya existe. ¿Desea instalar en dicha carpeta de todas formas?
DirDoesntExistTitle=La Carpeta No Existe
DirDoesntExist=La carpeta:%n%n%1%n%nno existe. ¿Desea que se cree dicha carpeta?

; *** "Select Components" wizard page
WizardSelectComponents=Selección de Componentes
SelectComponentsDesc=¿Qué componentes deben de instalarse?
SelectComponentsLabel2=Seleccione los componentes a instalar; desmarque los componentes que no desea instalar. Haga clic en Siguiente cuando desee continuar.
FullInstallation=Instalación Completa
; Si es posible no traduzca 'Compacta' a 'Minima' (Me refiero a 'Minima' en su lenguaje)
CompactInstallation=Instalación Compacta
CustomInstallation=Instalación Personalizada
NoUninstallWarningTitle=Componentes Existentes
NoUninstallWarning=La instalación ha detectado que los siguientes componentes están instalados en su sistema:%n%n%1%n%nQuitando la selección de estos componentes, no serán desinstalados.%n%n¿Desea continuar de todos modos?
ComponentSize1=%1 KB
ComponentSize2=%1 MB
ComponentsDiskSpaceMBLabel=La selección actual requiere al menos [mb] MB de espacio en disco.

; *** "Select Additional Tasks" wizard page
WizardSelectTasks=Selección de Tareas Adicionales
SelectTasksDesc=¿Qué tareas adicionales deben realizarse?
SelectTasksLabel2=Seleccione las tareas adicionales que usted desea que se realicen durante la instalación de [name] y haga clic en Siguiente.

; *** "Select Start Menu Folder" wizard page
WizardSelectProgramGroup=Selección de la carpeta del Menú de Inicio
SelectStartMenuFolderDesc=¿Dónde deben ubicarse los iconos de programa?
SelectStartMenuFolderLabel3=Setup will create the program's shortcuts in the following Start Menu folder.
SelectStartMenuFolderBrowseLabel=To continue, click Next. If you would like to select a different folder, click Browse.
NoIconsCheck=&No crear ningún icono
MustEnterGroupName=Debe introducir un nombre de carpeta.
GroupNameTooLong=El nombre de la carpeta o su ruta es demasiado largo.
InvalidGroupName=El nombre de la carpeta no es válido.
BadGroupName=El nombre de la carpeta no puede incluir alguno de los siguientes caracteres:%n%n%1
NoProgramGroupCheck2=&No crear un grupo en el Menú Inicio

; *** "Ready to Install" wizard page
WizardReady=Listo para Instalar
ReadyLabel1=Ahora el programa está listo para empezar la instalación de [name] en su sistema.
ReadyLabel2a=Haga clic Instalar para continuar con la instalación, o haga clic en Atrás si desea revisar o cambiar alguna configuración.
ReadyLabel2b=Haga clic Instalar para continuar con la instalación.
ReadyMemoUserInfo=Información de usuario:
ReadyMemoDir=Carpeta de Destino:
ReadyMemoType=Tipo de Instalación:
ReadyMemoComponents=Componentes Seleccionados:
ReadyMemoGroup=Carpeta del Menú de Inicio:
ReadyMemoTasks=Tareas Adicionales:

; *** "Preparing to Install" wizard page
WizardPreparing=Preparándose para Instalar
PreparingDesc=El programa se está preparando para instalar [name] en su sistema.
PreviousInstallNotCompleted=La instalación/desinstalación previa de otro programa no se completó. Necesitará reiniciar el sistema para completar la instalación.%n%nUna vez reiniciado el sistema, ejecute el programa de nuevo para completar la instalación de [name].
CannotContinue=El programa no puede continuar. Por favor, presione Cancelar para salir.

; *** "Installing" wizard page
WizardInstalling=Instalando
InstallingLabel=Por favor, espere mientras se instala [name] en su sistema.

; *** "Setup Completed" wizard page
FinishedHeadingLabel=Completando la instalación de [name]
FinishedLabelNoIcons=El programa terminó la instalación de [name] en su sistema.
FinishedLabel=El programa terminó la instalación de [name] en su sistema. Puede ejecutar la aplicación haciendo clic sobre el icono instalado.
ClickFinish=Haga clic en Terminar para salir de la Instalación.
FinishedRestartLabel=Para completar la instalación de [name], debe reiniciar su sistema. ¿Desea reiniciar ahora?
FinishedRestartMessage=Para completar la instalación de [name], debe reiniciar su sistema.%n%n¿Desea reiniciar ahora?
ShowReadmeCheck=Sí, deseo ver el archivo LÉAME.
YesRadio=&Sí, deseo reiniciar el sistema ahora
NoRadio=&No, yo reiniciaré el sistema más tarde
; used for example as 'Run MyProg.exe'
RunEntryExec=Ejecutar %1
; used for example as 'View Readme.txt'
RunEntryShellExec=Ver %1

; *** "Setup Needs the Next Disk" stuff
ChangeDiskTitle=La Instalación Necesita el Siguiente Disco
SelectDiskLabel2=Por favor, inserte el Disco %1 y haga clic en Aceptar.%n%nSi los archivos pueden hallarse en una carpeta diferente a la mostrada abajo, introduzca la ruta correcta o haga clic en Examinar.
PathLabel=&Ruta:
FileNotInDir2=El archivo "%1" no puede localizarse en "%2". Por favor, inserte el disco correcto o seleccione otra carpeta.
SelectDirectoryLabel=Por favor, especifique la localización del siguiente disco.

; *** Installation phase messages
SetupAborted=La instalación no puede completarse.%n%nPor favor, corrija el problema y ejecute Instalar de nuevo.
EntryAbortRetryIgnore=Haga clic en Reintentar para intentarlo de nuevo, Ignorar para continuar como sea, o Anular para cancelar la instalación.

; *** Installation status messages
StatusCreateDirs=Creando carpetas...
StatusExtractFiles=Copiando archivos...
StatusCreateIcons=Creando accesos directos...
StatusCreateIniEntries=Creando entradas en INI...
StatusCreateRegistryEntries=Creando entradas de registro...
StatusRegisterFiles=Registrando archivos...
StatusSavingUninstall=Guardando información para desinstalar...
StatusRunProgram=Terminando la instalación...
StatusRollback=Deshaciendo cambios...

; *** Misc. errors
ErrorInternal2=Error Interno: %1
ErrorFunctionFailedNoCode=%1 falló
ErrorFunctionFailed=%1 falló; código %2
ErrorFunctionFailedWithMessage=%1 falló; código %2.%n%3
ErrorExecutingProgram=Imposible ejecutar el archivo:%n%1

; *** Registry errors
ErrorRegOpenKey=Error abriendo clave de registro:%n%1\%2
ErrorRegCreateKey=Error creando clave de registro:%n%1\%2
ErrorRegWriteKey=Error escribiendo en clave de registro:%n%1\%2

; *** INI errors
ErrorIniEntry=Error creando entrada en archivo INI "%1".

; *** File copying errors
FileAbortRetryIgnore=Haga clic en Reintentar para intentarlo de nuevo, Ignorar para omitir este archivo (no recomendado), o Anular para cancelar la instalación.
FileAbortRetryIgnore2=Hag clic en Reintentar para intentarlo de nuevo, Ignorar para proceder de cualquier forma (no recomendado), o Anular para cancelar la instalación.
SourceIsCorrupted=El archivo de origen está dañado
SourceDoesntExist=El archivo de origen "%1" no existe
ExistingFileReadOnly=El archivo existente está marcado como sólo-lectura.%n%nHaga clic en Reintentar para quitar el atributo sólo-lectura e intentarlo de nuevo, Ignorar para omitir este archivo, o Anular para cancelar la instalación.
ErrorReadingExistingDest=Ocurrió un error tratando de leer el archivo existente:
FileExists=El archivo ya existe.%n%n¿Desea sobreescribirlo?
ExistingFileNewer=El archivo existente es más reciente que el que está tratando de instalar. Se recomienda que mantenga el archivo existente.%n%n¿Desea mantener el archivo existente?
ErrorChangingAttr=Ocurrió un error tratando de cambiar los atributos del archivo:
ErrorCreatingTemp=Ocurrió un error tratando de crear un archivo en la carpeta de destino:
ErrorReadingSource=Ocurrió un error tratando de leer el archivo de origen:
ErrorCopying=Ocurrió un error tratando de copiar el archivo:
ErrorReplacingExistingFile=Ocurrió un error tratando de reemplazar el archivo:
ErrorRestartReplace=Falló reintento de reemplazar:
ErrorRenamingTemp=Ocurrió un error tratando de renombrar un archivo en la carpeta de destino:
ErrorRegisterServer=Imposible registrar el DLL/OCX: %1
ErrorRegisterServerMissingExport=No se encuentra DllRegisterServer export
ErrorRegisterTypeLib=Imposible registrar la librería de tipo: %1

; *** Post-installation errors
ErrorOpeningReadme=Ocurrió un error tratando de abrir el archivo LÉAME.
ErrorRestartingComputer=El programa de Instalación no puede reiniciar el sistema. Por favor, hágalo manualmente.

; *** Uninstaller messages
UninstallNotFound=El archivo "%1" no existe. No se puede desinstalar.
UninstallOpenError=El archivo "%1" no pudo abrirse. No se puede desinstalar.
UninstallUnsupportedVer=El archivo de bitácora para desinstalar "%1" está en un formato no reconocido por esta versión de desinstalación. No se puede desinstalar
UninstallUnknownEntry=Una entrada desconocida (%1) se encontró en el bitácora para desinstalar
ConfirmUninstall=¿Está seguro que desea eliminar completamente %n%1 y todos sus componentes?
OnlyAdminCanUninstall=Este programa sólo puede desinstalarlo un usuario con privilegios de administrador.
UninstallStatusLabel=Por favor, espere mientras se elimina %1 de su sistema.
UninstalledAll=%1 se eliminó con éxito de su sistema.
UninstalledMost=La desinstalación de %1 terminó.%n%nAlgunos elementos no pudieron eliminarse. Puede usted eliminarlos manualmente.
UninstalledAndNeedsRestart=Para completar la desinstalación de %1, el sistema debe de reiniciarse.%n%nQuiere reiniciarlo ahora?
UninstallDataCorrupted=El archivo "%1" está dañado. No puede desinstalarse

; *** Uninstallation phase messages
ConfirmDeleteSharedFileTitle=¿Eliminar Archivos Compartidos?
ConfirmDeleteSharedFile2=El sistema indica que el siguiente archivo compartido no es usado por ningún otro programa. ¿Desea eliminar este archivo compartido?%n%nSi otros programas usan este archivo y es eliminado, pueden dejar de funcionar correctamente. Si no está seguro, elija <No>. Dejar el archivo en su sistema no producirá ningún daño.
SharedFileNameLabel=Nombre de archivo:
SharedFileLocationLabel=Localización:
WizardUninstalling=Estado de la Desinstalación
StatusUninstalling=Desinstalando %1...

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