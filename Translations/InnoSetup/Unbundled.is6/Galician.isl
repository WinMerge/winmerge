; *** Inno Setup version 6.1.0+ Galician messages ***
;
; Maintained by: Julio Mojon Fuentes
; (juliomf AT edu.xunta.gal)
;
; To download user-contributed translations of this file, go to:
;   https://jrsoftware.org/files/istrans/

[LangOptions]
LanguageName=Galego
LanguageID=$0456
LanguageCodePage=1252

[Messages]
; *** Application titles
SetupAppTitle=Instalar
SetupWindowTitle=Instalar %1
UninstallAppTitle=Desinstalar
UninstallAppFullTitle=Desinstalar %1

; *** Misc. common
InformationTitle=Información
ConfirmTitle=Confirmar
ErrorTitle=Erro

; *** SetupLdr messages
SetupLdrStartupMessage=Isto vai instalar %1. Desexa continuar?
LdrCannotCreateTemp=Non foi posible crear un arquivo temporal. Instalación cancelada
LdrCannotExecTemp=Non foi posible executar un arquivo no cartafol temporal. Instalación cancelada
HelpTextNote=

; *** Startup error messages
LastErrorMessage=%1.%n%nErro %2: %3
SetupFileMissing=Non se atopa o arquivo %1 no cartafol de instalación. Por favor, corrixa o problema ou consiga unha nova copia do programa.
SetupFileCorrupt=Os arquivos de instalación están corruptos. Por favor, consiga unha nova copia do programa.
SetupFileCorruptOrWrongVer=Os arquivos de instalación están corruptos, ou son incompatibles con esta versión do asistente de instalación. Por favor, corrixa o problema ou consiga unha nova copia do programa.
InvalidParameter=Un parámetro no válido foi pasado na liña de comando:%n%n%1
SetupAlreadyRunning=O asistente de instalación xa está en execución.
WindowsVersionNotSupported=Este programa non é compatible coa versión de Windows que corre no seu equipo.
WindowsServicePackRequired=Este programa require %1 Service Pack %2 ou posterior.
NotOnThisPlatform=Este programa non se pode executar en %1.
OnlyOnThisPlatform=Este programa debe executarse en %1.
OnlyOnTheseArchitectures=Este programa só pode ser instalado en versións de Windows deseñadas para as seguintes arquitecturas de procesador:%n%n%1
WinVersionTooLowError=Este programa require %1, versión %2 ou posterior.
WinVersionTooHighError=Este programa non pode ser instalado en %1, versión %2 ou posterior.
AdminPrivilegesRequired=Debe iniciar sesión como administrador para instalar este programa.
PowerUserPrivilegesRequired=Debe iniciar sesión como administrador ou como membro do grupo de Usuarios Avanzados para instalar este programa.
SetupAppRunningError=O asistente de instalación detectou que %1 está en execución.%n%nPor favor, peche agora todas as súas copias e prema Aceptar para continuar, ou Cancelar para saír.
UninstallAppRunningError=O asistente de desinstalación detectou que %1 está en execución.%n%nPor favor, peche agora todas as súas copias e prema Aceptar para continuar, ou Cancelar para saír.

; *** Startup questions
PrivilegesRequiredOverrideTitle=Selección do modo de instalación
PrivilegesRequiredOverrideInstruction=Seleccione o modo de instalación
PrivilegesRequiredOverrideText1=%1 pode instalarse para todos os usuarios (require privilexios administrativos), ou só para vostede.
PrivilegesRequiredOverrideText2=%1 pode instalarse só para vostede, ou para todos os usuarios (require privilexios administrativos).
PrivilegesRequiredOverrideAllUsers=Instalar para &todos os usuarios
PrivilegesRequiredOverrideAllUsersRecommended=Instalar para &todos os usuarios (recomendado)
PrivilegesRequiredOverrideCurrentUser=Instalar só para &min
PrivilegesRequiredOverrideCurrentUserRecommended=Instalar só para &min (recomendado)

; *** Misc. errors
ErrorCreatingDir=O asistente de instalación non puido crear o cartafol "%1"
ErrorTooManyFilesInDir=Non se puido crear un arquivo no cartafol "%1" porque contén demasiados arquivos

; *** Setup common messages
ExitSetupTitle=Saír da instalación
ExitSetupMessage=A instalación non está completa. Se sae agora, o programa non se instalará.%n%nPode executar de novo o asistente de instalación noutro momento para completar a instalación.%n%nSaír da instalación?
AboutSetupMenuItem=&Acerca do asistente de instalación...
AboutSetupTitle=Acerca do asistente de instalación
AboutSetupMessage=%1, versión %2%n%3%n%nPáxina de inicio de %1:%n%4
AboutSetupNote=
TranslatorNote=Galician translation mantained by Julio Mojón Fuentes (juliomf AT edu.xunta.gal)

; *** Buttons
ButtonBack=< &Anterior
ButtonNext=&Seguinte >
ButtonInstall=&Instalar
ButtonOK=Aceptar
ButtonCancel=Cancelar
ButtonYes=&Si
ButtonYesToAll=Si a &todo
ButtonNo=&Non
ButtonNoToAll=N&on a todo
ButtonFinish=&Finalizar
ButtonBrowse=&Examinar...
ButtonWizardBrowse=E&xaminar...
ButtonNewFolder=&Crear novo cartafol

; *** "Select Language" dialog messages
SelectLanguageTitle=Seleccione a lingua de instalación
SelectLanguageLabel=Seleccione a lingua para usar durante a instalación.

; *** Common wizard text
ClickNext=Prema Seguinte para continuar, ou Cancelar para saír da instalación.
BeveledLabel=
BrowseDialogTitle=Examinar cartafol
BrowseDialogLabel=Seleccione un cartafol na lista inferior e prema Aceptar.
NewFolderName=Novo cartafol

; *** "Welcome" wizard page
WelcomeLabel1=Benvido ao asistente de instalación de [name]
WelcomeLabel2=Isto vai instalar [name/ver] no seu computador.%n%nRecoméndaselle que peche todas as outras aplicacións antes de continuar.

; *** "Password" wizard page
WizardPassword=Contrasinal
PasswordLabel1=Esta instalación está protexida por contrasinal.
PasswordLabel3=Por favor, introduza o contrasinal e prema Seguinte para continuar. Os contrasinais diferencian entre maiúsculas e minúsculas.
PasswordEditLabel=&Contrasinal:
IncorrectPassword=O contrasinal que introduciu non é correcto. Por favor, ténteo de novo.

; *** "License Agreement" wizard page
WizardLicense=Acordo de licenza
LicenseLabel=Por favor, lea a seguinte información importante antes de continuar.
LicenseLabel3=Por favor, lea o seguinte acordo de licenza. Debe aceptar os termos deste acordo antes de continuar coa instalación.
LicenseAccepted=&Acepto o acordo
LicenseNotAccepted=&Non acepto o acordo

; *** "Information" wizard pages
WizardInfoBefore=Información
InfoBeforeLabel=Por favor, lea a seguinte información importante antes de continuar.
InfoBeforeClickLabel=Cando estea preparado para continuar coa instalación, prema Seguinte.
WizardInfoAfter=Información
InfoAfterLabel=Por favor, lea a seguinte información importante antes de continuar.
InfoAfterClickLabel=Cando estea preparado para continuar coa instalación, prema Seguinte.

; *** "User Information" wizard page
WizardUserInfo=Información de usuario
UserInfoDesc=Por favor, introduza os seus datos.
UserInfoName=Nome de &usuario:
UserInfoOrg=&Organización:
UserInfoSerial=Número de &serie:
UserInfoNameRequired=Debe introducir un nome.

; *** "Select Destination Location" wizard page
WizardSelectDir=Selección da localización de destino
SelectDirDesc=Onde debería instalarse [name]?
SelectDirLabel3=O asistente de instalación vai instalar [name] no seguinte cartafol.
SelectDirBrowseLabel=Para continuar, prema Seguinte. Se desexa seleccionar un cartafol diferente, prema Examinar.
DiskSpaceGBLabel=Cómpren polo menos [gb] GB de espazo libre en disco.
DiskSpaceMBLabel=Cómpren polo menos [mb] MB de espazo libre en disco.
CannotInstallToNetworkDrive=O asistente de instalación non pode instalar nunha unidade de rede.
CannotInstallToUNCPath=O asistente de instalación non pode instalar nunha ruta UNC.
InvalidPath=Debe introducir unha ruta completa con letra de unidade; por exemplo:%n%nC:\APP%n%nou unha ruta UNC na forma:%n%n\\server\share
InvalidDrive=A unidade ou ruta UNC compartida que seleccionou non existe ou non é accesible. Por favor, seleccione outra.
DiskSpaceWarningTitle=Sen espazo en disco suficiente
DiskSpaceWarning=Ao asistente de instalación cómprelle polo menos %1 KB de espazo libre para instalar, pero a unidade seleccionada só ten %2 KB dispoñibles.%n%nDesexa continuar de todas formas?
DirNameTooLong=O nome ou a ruta do cartafol é longa de máis.
InvalidDirName=O nome do cartafol non é válido.
BadDirName32=Os nomes de cartafol non poden incluír ningún dos seguintes caracteres:%n%n%1
DirExistsTitle=O cartafol existe
DirExists=Xa existe o cartafol:%n%n%1%n%nDesexa instalar nese cartafol de todas formas?
DirDoesntExistTitle=O cartafol non existe
DirDoesntExist=Non existe o cartafol:%n%n%1%n%nDesexa crear o cartafol?

; *** "Select Components" wizard page
WizardSelectComponents=Selección de compoñentes
SelectComponentsDesc=Que compoñentes deberían ser instalados?
SelectComponentsLabel2=Seleccione os compoñentes que desexa instalar; quite a selección aos compoñentes que non desexa instalar. Prema Seguinte cando estea preparado para continuar.
FullInstallation=Instalación completa
CompactInstallation=Instalación compacta
CustomInstallation=Instalación personalizada
NoUninstallWarningTitle=Os compoñentes existen
NoUninstallWarning=O asistente de instalación detectou que os seguintes compoñentes xa están instalados no seu computador:%n%n%1%n%nQuitarlles a selección a estes compoñentes non os desinstalará.%n%nDesexa continuar de todas formas?
ComponentSize1=%1 KB
ComponentSize2=%1 MB
ComponentsDiskSpaceGBLabel=A selección actual require polo menos [gb] GB de espazo en disco.
ComponentsDiskSpaceMBLabel=A selección actual require polo menos [mb] MB de espazo en disco.

; *** "Select Additional Tasks" wizard page
WizardSelectTasks=Selección de tarefas adicionais
SelectTasksDesc=Que tarefas adicionais deben realizarse?
SelectTasksLabel2=Seleccione as tarefas adicionais que desexa que realice o asistente de instalación mentres instala [name], e prema Seguinte.

; *** "Select Start Menu Folder" wizard page
WizardSelectProgramGroup=Selección do cartafol do Menú Inicio
SelectStartMenuFolderDesc=Onde debería colocar o asistente de instalación os atallos do programa?
SelectStartMenuFolderLabel3=O asistente de instalación vai crear os atallos do programa no seguinte cartafol do Menú Inicio.
SelectStartMenuFolderBrowseLabel=Para continuar, prema Seguinte. Se desexa seleccionar un cartafol diferente, prema Examinar.
MustEnterGroupName=Debe introducir un nome de cartafol.
GroupNameTooLong=O nome ou a ruta do cartafol é longo de máis.
InvalidGroupName=O nome do cartafol non é válido.
BadGroupName=O nome do cartafol non pode incluír ningún dos seguintes caracteres:%n%n%1
NoProgramGroupCheck2=&Non crear un cartafol do Menú Inicio

; *** "Ready to Install" wizard page
WizardReady=Preparado para instalar
ReadyLabel1=O asistente de instalación está preparado agora para comezar a instalar [name] no seu computador.
ReadyLabel2a=Prema Instalar para continuar coa instalación, ou prema Atrás se desexa revisar ou mudar calquera configuración.
ReadyLabel2b=Prema Instalar para continuar coa instalación.
ReadyMemoUserInfo=Información do usuario:
ReadyMemoDir=Localización do destino:
ReadyMemoType=Tipo de instalación:
ReadyMemoComponents=Compoñentes seleccionados:
ReadyMemoGroup=Cartafol do Menú Inicio:
ReadyMemoTasks=Tarefas adicionais:

; *** TDownloadWizardPage wizard page and DownloadTemporaryFile
DownloadingLabel=A descargar arquivos adicionais...
ButtonStopDownload=&Deter descarga
StopDownload=Está seguro de que desexa deter a descarga?
ErrorDownloadAborted=Descarga cancelada
ErrorDownloadFailed=A descarga fallou: %1 %2
ErrorDownloadSizeFailed=Non se puido determinar o tamaño: %1 %2
ErrorFileHash1=Non se puido calcular o hash do arquivo: %1
ErrorFileHash2=Hash do arquivo inválido: esperado %1, atopado %2
ErrorProgress=Progreso inválido: %1 de %2
ErrorFileSize=Tamaño de arquivo inválido: esperado %1, atopado %2

; *** "Preparing to Install" wizard page
WizardPreparing=A prepararse para instalar
PreparingDesc=O asistente de instalación está a prepararse para instalar [name] no seu computador.
PreviousInstallNotCompleted=A instalación ou desinstalación dun programa anterior non está completa. Cómpre que reinicie o seu computador para completar esa instalación.%n%nDespois de reiniciar o seu computador, execute o asistente de instalación de novo para completar a instalación de [name].
CannotContinue=O asistente de instalación non pode continuar. Por favor, prema Cancelar para saír.
ApplicationsFound=As seguintes aplicacións están a utilizar arquivos que o asistente de instalación necesita actualizar. Recoméndaselle que permita que o asistente de instalación peche automaticamente esas aplicacións.
ApplicationsFound2=As seguintes aplicacións están a utilizar arquivos que o asistente de instalación necesita actualizar. Recoméndaselle que permita que o asistente de instalación peche automaticamente esas aplicacións. Cando a instalación estea completa, o asistente de instalación tentará reiniciar as aplicacións.
CloseApplications=Pechar &automaticamente as aplicacións
DontCloseApplications=Non &pechar as aplicacións
ErrorCloseApplications=O asistente de instalación non puido pechar automaticamente todas as aplicacións. Antes de continuar, recoméndaselle que peche todas as aplicacións que utilizan arquivos que o asistente de instalación necesita actualizar.
PrepareToInstallNeedsRestart=O asistente de instalación debe reiniciar o seu computador. Despois de reinicialo, execute o asistente de instalación de novo para completar a instalación de [name].%n%nDesexa reinicialo agora?

; *** "Installing" wizard page
WizardInstalling=A instalar
InstallingLabel=Por favor, espere mentres o asistente de instalación instala [name] no seu computador.

; *** "Setup Completed" wizard page
FinishedHeadingLabel=A completar o asistente de instalación de [name]
FinishedLabelNoIcons=O asistente de instalación terminou de instalar [name] no seu computador.
FinishedLabel=O asistente de instalación terminou de instalar [name] no seu computador. Pode executar a aplicación seleccionando os atallos instalados.
ClickFinish=Prema Finalizar para saír do asistente de instalación.
FinishedRestartLabel=Para completar a instalación de [name], o asistente de instalación debe reiniciar o seu computador. Desexa reinicialo agora?
FinishedRestartMessage=Para completar a instalación de [name], o asistente de instalación debe reiniciar o seu computador.%n%nWDesexa reinicialo agora?
ShowReadmeCheck=Si, desexo ver o arquivo LÉAME
YesRadio=&Si, reiniciar agora o computador
NoRadio=&Non, reiniciarei o computador máis tarde
RunEntryExec=Executar %1
RunEntryShellExec=Ver %1

; *** "Setup Needs the Next Disk" stuff
ChangeDiskTitle=Ao asistente de instalación cómprelle o seguinte disco
SelectDiskLabel2=Por favor, insira o disco %1 e prema Aceptar.%n%nSe os arquivos deste disco se poden atopar noutro cartafol diferente ao mostrado abaixo, introduza a ruta correcta ou prema Examinar.
PathLabel=&Ruta:
FileNotInDir2=Non se puido atopar o arquivo "%1" en "%2". Por favor, insira o disco correcto ou seleccione outro cartafol.
SelectDirectoryLabel=Por favor, especifique a localización do seguinte disco.

; *** Installation phase messages
SetupAborted=A instalación non se completou.%n%nPor favor, corrixa o problema e execute o asistente de instalación de novo.
AbortRetryIgnoreSelectAction=Seleccione acción
AbortRetryIgnoreRetry=&Tentar de novo
AbortRetryIgnoreIgnore=&Ignorar o erro e continuar
AbortRetryIgnoreCancel=Cancelar a instalación

; *** Installation status messages
StatusClosingApplications=A pechar aplicacións...
StatusCreateDirs=A crear cartafoles...
StatusExtractFiles=A extraer arquivos...
StatusCreateIcons=A crear atallos...
StatusCreateIniEntries=A crear entradas INI...
StatusCreateRegistryEntries=A crear entradas do rexistro...
StatusRegisterFiles=A rexistrar arquivos...
StatusSavingUninstall=Gardando información de desinstalación...
StatusRunProgram=A terminar a instalación...
StatusRestartingApplications=A reiniciar aplicacións...
StatusRollback=A desfacer cambios...

; *** Misc. errors
ErrorInternal2=Erro interno: %1
ErrorFunctionFailedNoCode=%1 fallou
ErrorFunctionFailed=%1 fallou; código %2
ErrorFunctionFailedWithMessage=%1 fallou; código %2.%n%3
ErrorExecutingProgram=Non foi posible executar o arquivo:%n%1

; *** Registry errors
ErrorRegOpenKey=Erro ao abrir a chave do rexistro:%n%1\%2
ErrorRegCreateKey=Erro ao crear a chave do rexistro:%n%1\%2
ErrorRegWriteKey=Erro ao escribir na chave do rexistro:%n%1\%2

; *** INI errors
ErrorIniEntry=Erro ao crear a entrada INI no arquivo "%1".

; *** File copying errors
FileAbortRetryIgnoreSkipNotRecommended=&Saltar este arquivo (non recomendado)
FileAbortRetryIgnoreIgnoreNotRecommended=&Ignorar o erro e continuar (non recomendado)
SourceIsCorrupted=O arquivo orixe está corrupto
SourceDoesntExist=O arquivo orixe "%1" non existe
ExistingFileReadOnly2=Non foi posible substituír o arquivo existente porque está marcado como de só lectura.
ExistingFileReadOnlyRetry=&Eliminar o atributo de só lectura e tentar de novo
ExistingFileReadOnlyKeepExisting=&Manter o arquivo existente
ErrorReadingExistingDest=Ocorreu un erro ao tentar ler o arquivo existente:
FileExistsSelectAction=Seleccionar acción
FileExists2=O arquivo xa existe.
FileExistsOverwriteExisting=&Sobrescribir o arquivo existente
FileExistsKeepExisting=&Manter o arquivo existente
FileExistsOverwriteOrKeepAll=&Facer isto para os seguintes conflitos
ExistingFileNewerSelectAction=Seleccionar acción
ExistingFileNewer2=O arquivo existente é máis novo ca o que o asistente de instalación tenta instalar.
ExistingFileNewerOverwriteExisting=&Sobrescribir o arquivo existente
ExistingFileNewerKeepExisting=&Manter o arquivo existente (recomendado)
ExistingFileNewerOverwriteOrKeepAll=&Facer isto para os seguintes conflitos
ErrorChangingAttr=Ocorreu un erro ao tentar mudar os atributos do arquivo existente:
ErrorCreatingTemp=Ocorreu un erro ao tentar crear un arquivo no cartafol de destino:
ErrorReadingSource=Ocorreu un erro ao tentar ler o arquivo orixe:
ErrorCopying=Ocorreu un erro ao tentar copiar un arquivo:
ErrorReplacingExistingFile=Ocorreu un erro ao tentar substituír o arquivo existente:
ErrorRestartReplace=RestartReplace fallou:
ErrorRenamingTemp=Ocorreu un erro ao tentar renomear un arquivo no cartafol de destino:
ErrorRegisterServer=Non foi posible rexistrar a DLL ou OCX: %1
ErrorRegSvr32Failed=RegSvr32 fallou co código de saída %1
ErrorRegisterTypeLib=Non foi posible rexistrar a biblioteca de tipos: %1

; *** Uninstall display name markings
UninstallDisplayNameMark=%1 (%2)
UninstallDisplayNameMarks=%1 (%2, %3)
UninstallDisplayNameMark32Bit=32 bits
UninstallDisplayNameMark64Bit=64 bits
UninstallDisplayNameMarkAllUsers=Todos os usuarios
UninstallDisplayNameMarkCurrentUser=Usuario actual

; *** Post-installation errors
ErrorOpeningReadme=Ocorreu un erro ao tentar abrir o arquivo LÉAME.
ErrorRestartingComputer=O asistente de instalación non puido reiniciar o computador. Por favor, fágao manualmente.

; *** Uninstaller messages
UninstallNotFound=O arquivo "%1" non existe. Non se pode desinstalar.
UninstallOpenError=Non se puido abrir o arquivo "%1". Non se pode desinstalar
UninstallUnsupportedVer=O arquivo de rexistro "%1" está nun formato non recoñecido por esta versión do asistente de desinstalación. Non se pode desinstalar
UninstallUnknownEntry=Atopouse unha entrada descoñecida (%1) no arquivo de rexistro da desinstalación
ConfirmUninstall=Está seguro de que desexa eliminar por completo %1 e todos os seus compoñentes?
UninstallOnlyOnWin64=Esta instalación só se pode desinstalar nun Windows de 64 bits.
OnlyAdminCanUninstall=Só un usuario con privilexios administrativos pode desinstalar esta instalación.
UninstallStatusLabel=Por favor, espere mentres se elimina %1 do seu computador.
UninstalledAll=Eliminouse %1 con éxito do seu computador.
UninstalledMost=Desinstalación de %1 completa.%n%nNon foi posible eliminar algúns elementos. Pode eliminalos manualmente.
UninstalledAndNeedsRestart=Para completar a desinstalación de %1, debe reiniciar o seu computador.%n%nDesexa reinicialo agora?
UninstallDataCorrupted=O arquivo "%1" está corrupto. Non se pode desinstalar

; *** Uninstallation phase messages
ConfirmDeleteSharedFileTitle=Eliminar arquivo compartido?
ConfirmDeleteSharedFile2=O sistema indica que o seguinte arquivo compartido xa non é usado por ningún programa. Quere que o programa de desinstalación elimine este arquivo compartido?%n%nSe calquera programa aínda usa este arquivo e é eliminado, ese programa podería funcionar incorrectamente. Se non está certo, seleccione Non. Manter o arquivo no seu sistema non lle causará ningún problema.
SharedFileNameLabel=Nome do arquivo:
SharedFileLocationLabel=Localización:
WizardUninstalling=Estado da instalación
StatusUninstalling=A desinstalar %1...

; *** Shutdown block reasons
ShutdownBlockReasonInstallingApp=A instalar %1.
ShutdownBlockReasonUninstallingApp=A desinstalar %1.

[CustomMessages]
NameAndVersion=%1 versión %2
AdditionalIcons=Atallos adicionais:
CreateDesktopIcon=Crear un atallo no &Escritorio
CreateQuickLaunchIcon=Crear un atallo en &Inicio rápido
ProgramOnTheWeb=%1 na web
UninstallProgram=Desinstalar %1
LaunchProgram=Executar %1
AssocFileExtension=&Asociar %1 coa extensión de arquivo %2
AssocingFileExtension=A asociar %1 coa %2 extensión de arquivo...
AutoStartProgramGroupDescription=Inicio:
AutoStartProgram=Iniciar %1 automaticamente
AddonHostProgramNotFound=Non se puido localizar %1 no cartafol que seleccionou.%n%nDesexa continuar de todas formas?
