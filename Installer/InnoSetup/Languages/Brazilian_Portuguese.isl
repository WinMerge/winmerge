; *** Inno Setup version 4.1.4+ Portuguese (Brazil) messages ***
;
; To download user-contributed translations of this file, go to:
;   http://www.jrsoftware.org/is3rdparty.php
;
; Note: When translating this text, do not add periods (.) to the end of
; messages that didn't have them already, because on those messages Inno
; Setup adds the periods automatically (appending a period would result in
; two periods being displayed).
;
; $id: BrazilianPortuguese-7-4.1.4.isl,v 1.51 2004/02/12 01:22:03 $

[LangOptions]
LanguageName=Portuguese (Brazil)
LanguageID=$0416
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
SetupAppTitle=Programa de Instalação
SetupWindowTitle=Programa de Instalação - %1
UninstallAppTitle=Desinstalação
UninstallAppFullTitle=Desinstalação - %1

; *** Misc. common
InformationTitle=Informação
ConfirmTitle=Confirmação
ErrorTitle=Erro

; *** SetupLdr messages
SetupLdrStartupMessage=Este programa irá instalar o %1. Você gostaria de continuar?
LdrCannotCreateTemp=Não foi possível criar um arquivo temporário. Instalação abortada
LdrCannotExecTemp=Não foi possível executar um arquivo na pasta de arquivos temporários. Instalação abortada

; *** Startup error messages
LastErrorMessage=%1.%n%nErro %2: %3
SetupFileMissing=O arquivo %1 está faltando na pasta de instalação. Corrija o problema ou obtenha uma nova cópia do programa.
SetupFileCorrupt=Os arquivos de instalação estão corrompidos. Obtenha uma nova cópia do programa.
SetupFileCorruptOrWrongVer=Os arquivos de instalação estão corrompidos ou são incompatíveis com esta versão do Programa de Instalação. Corrija o problema ou obtenha uma nova cópia.
NotOnThisPlatform=Este programa não irá executar em %1.
OnlyOnThisPlatform=Este programa deverá executar em %1.
WinVersionTooLowError=Este programa requer %1 versão %2 ou superior.
WinVersionTooHighError=Este programa não pode ser instalado no %1 versão %2 ou superior.
AdminPrivilegesRequired=Você deverá estar logado como administrador para instalar este programa.
PowerUserPrivilegesRequired=Você deve ser logado como um administrador ou como um membro do grupo "Power Users" para instalar este programa.
SetupAppRunningError=O programa de instalação detectou que %1 está em execução.%n%nPor favor, feche todas as suas instâncias agora, então clique em OK para continuar, ou Cancelar para sair.
UninstallAppRunningError=O programa de desinstalação detectou que %1 está em execução.%n%nPor favor, feche todas as suas instâncias agora, então clique em OK para continuar, ou Cancelar para sair.

; *** Misc. errors
ErrorCreatingDir=O programa de instalação não pode criar a pasta "%1"
ErrorTooManyFilesInDir=Não foi possível criar um arquivo no pasta "%1" - muitos arquivos

; *** Setup common messages
ExitSetupTitle=Terminar a Instalação
ExitSetupMessage=A instalação não está completa. Se você terminar agora, o programa não será instalado.%n%nVocê poderá tentar novamente mais tarde para completar a instalação.%n%nTerminar a instalação?
AboutSetupMenuItem=&Sobre o Programa de Instalação...
AboutSetupTitle=Sobre o Programa de Instalação
AboutSetupMessage=%1 versão %2%n%3%n%n%1 home page:%n%4
AboutSetupNote=

; *** Buttons
ButtonBack=< &Voltar
ButtonNext=&Avançar >
ButtonInstall=&Instalar
ButtonOK=OK
ButtonCancel=Cancelar
ButtonYes=&Sim
ButtonYesToAll=Sim para &Todos
ButtonNo=&Não
ButtonNoToAll=Nã&o para Todos
ButtonFinish=&Concluir
ButtonBrowse=&Procurar...
ButtonWizardBrowse=P&rocurar...
ButtonNewFolder=Criar &nova pasta

; *** "Select Language" dialog messages
SelectLanguageTitle=Selecione o idioma da instalação
SelectLanguageLabel=Selecione o idioma a ser utilizado durante a instalação:

; *** Common wizard text
ClickNext=Clique Avançar para continuar, Cancelar para encerrar.
BeveledLabel=
BrowseDialogTitle=Procurar pasta
BrowseDialogLabel=Selecione uma pasta na lista a seguir, e clique OK.
NewFolderName=Nova pasta

; *** "Welcome" wizard page
WelcomeLabel1=Bem-vindo ao Assistente de Instalação do [name]
WelcomeLabel2=Este programa irá instalar o [name/ver] no seu computador.%n%nÉ recomendado que você feche as aplicações abertas antes de continuar. Isto evitará conflitos durante a instalação.

; *** "Password" wizard page
WizardPassword=Senha
PasswordLabel1=Esta instalação está protegida por senha.
PasswordLabel3=Por favor digite a senha e então clique em Avançar para continuar. Maiúsculas e minúsculas são diferentes.
PasswordEditLabel=&Senha:
IncorrectPassword=A senha que você digitou não está correta. Tente novamente.

; *** "License Agreement" wizard page
WizardLicense=Contrato de Licença de Uso
LicenseLabel=Leia as seguintes informações importantes antes de continuar.
LicenseLabel3=Leia a licença seguinte. Você precisa aceitar os termos desta licença antes de continuar com a instalação.
LicenseAccepted=Eu &aceito a licença
LicenseNotAccepted=Eu &não aceito a licença

; *** "Information" wizard pages
WizardInfoBefore=Informação
InfoBeforeLabel=Leia as seguintes informações importantes antes de continuar.
InfoBeforeClickLabel=Quando você estiver pronto para continuar clique em Avançar.
WizardInfoAfter=Informação
InfoAfterLabel=Leia as seguintes informações importantes antes de continuar.
InfoAfterClickLabel=Quando você estiver pronto para continuar clique em Avançar.

; *** "User Information" wizard page
WizardUserInfo=Informações do Usuário
UserInfoDesc=Por favor, informe seus dados.
UserInfoName=&Nome de Usuário:
UserInfoOrg=&Organização:
UserInfoSerial=&Número Serial:
UserInfoNameRequired=Você precisa informar um nome.

; *** "Select Destination Directory" wizard page
WizardSelectDir=Escolha o local de destino
SelectDirDesc=Onde [name] será instalado?
SelectDirLabel3=Setup will install [name] into the following folder.
SelectDirBrowseLabel=To continue, click Next. If you would like to select a different folder, click Browse.
DiskSpaceMBLabel=Este programa exige [mb] MB de espaço.
ToUNCPathname=O Programa de Instalação não pode instalar em um caminho UNC. Se você está tentando instalar em uma rede, você precisa mapear uma unidade da rede.
InvalidPath=Você deve entrar um caminho completo com a letra da unidade; por exemplo:%nC:\APP
InvalidDrive=A unidade não existe. Escolha outra.
DiskSpaceWarningTitle=Não há espaço suficiente
DiskSpaceWarning=O Programa de Instalação exige %1 KB de espaço livre para instalar, mas a unidade selecionada tem somente %2 KB disponíveis.%n%você quer continuar?
DirNameTooLong=O nome da pasta ou caminho é muito longo.
InvalidDirName=O nome da pasta não é válido.
BadDirName32=O nome da pasta não pode conter os seguintes caracteres:%n%n%1
DirExistsTitle=A pasta já existe
DirExists=A pasta%n%n%1%n%njá existe. Você gostaria de instalar nesta pasta?
DirDoesntExistTitle=A pasta não existe
DirDoesntExist=A pasta :%n%n%1%n%nnão existe. Você gostaria de criar a pasta?

; *** "Select Components" wizard page
WizardSelectComponents=Selecione Componentes
SelectComponentsDesc=Que componentes serão instalados?
SelectComponentsLabel2=Selecione os componentes que você quer instalar, desmarque os componentes que você não quer instalar. Clique em Avançar quando você estiver pronto para continuar.
FullInstallation=Instalação Completa
; if possible don't translate 'Compact' as 'Minimal' (I mean 'Minimal' in your language)
CompactInstallation=Instalação Compacta
CustomInstallation=Instalação Personalizada
NoUninstallWarningTitle=Componentes Encontrados
NoUninstallWarning=O Programa de Instalação detectou que os seguintes componentes estão instalados em seu computador:%n%n%1%n%nDesmarcando estes componentes eles não serão desinstalados.%n%nVocê gostaria de continuar assim mesmo?
ComponentSize1=%1 KB
ComponentSize2=%1 MB
ComponentsDiskSpaceMBLabel=Seleção atual requer [mb] MB de espaço.

; *** "Select Additional Tasks" wizard page
WizardSelectTasks=Selecione Tarefas Adicionais
SelectTasksDesc=Que tarefas adicionais serão executadas?
SelectTasksLabel2=Selecione as tarefas adicionais que você gostaria que o Programa de Instalação execute enquanto instala o [name] e então clique em Avançar.

; *** "Select Start Menu Folder" wizard page
WizardSelectProgramGroup=Escolha a pasta do Menu Iniciar
SelectStartMenuFolderDesc=Onde o Programa de Instalação irá criar os ícones do programa?
SelectStartMenuFolderLabel3=Setup will create the program's shortcuts in the following Start Menu folder.
SelectStartMenuFolderBrowseLabel=To continue, click Next. If you would like to select a different folder, click Browse.
NoIconsCheck=Não criar ícones
MustEnterGroupName=Você deve digitar um nome de uma pasta do Menu Iniciar.
GroupNameTooLong=O nome da pasta ou caminho é muito longo.
InvalidGroupName=O nome da pasta não é válido.
BadGroupName=O nome do pasta não pode incluir os seguintes caracteres:%n%n%1
NoProgramGroupCheck2=&Não criar a pasta do Menu Iniciar

; *** "Ready to Install" wizard page
WizardReady=Pronto para Instalar
ReadyLabel1=O Programa de Instalação está pronto para iniciar a instalar [name] no seu computador.
ReadyLabel2a=Clique em Instalar para iniciar a instalação, ou clique Voltar se você quer rever ou verificar suas opções.
ReadyLabel2b=Clique em Instalar para iniciar a instalação.
ReadyMemoUserInfo=Informações do Usuário:
ReadyMemoDir=Local de destino:
ReadyMemoType=Tipo de Instalação:
ReadyMemoComponents=Componentes Selecionados:
ReadyMemoGroup=Grupo de Programas:
ReadyMemoTasks=Tarefas Adicionais:

; *** "Preparing to Install" wizard page
WizardPreparing=Preparando para Instalar
PreparingDesc=Instalação está preparando para instalar [name] em seu computador.
PreviousInstallNotCompleted=A instalação/remoção do programa anterior não foi completada. Você precisa reiniciar o computadorpara completar esta instalação. %n%nApós reiniciar seu computador, rode a Instalação novamente para completar a instalação do [name].
CannotContinue=A Instalação não pode continuar. Clique em Cancelar para sair.

; *** "Installing" wizard page
WizardInstalling=Instalando
InstallingLabel=Aguarde enquanto o Programa de Instalação instala [name] em seu computador.

; *** "Setup Completed" wizard page
FinishedHeadingLabel=Instalação Terminada
FinishedLabelNoIcons=O Programa de Instalação terminou de instalar [name] no seu computador.
FinishedLabel=O Programa de Instalação terminou de instalar [name] no seu computador. A aplicação pode ser iniciada escolhendo os ícones instalados.
ClickFinish=Clique em Concluir para finalizar o Programa de Instalação.
FinishedRestartLabel=Para completar a instalação do [name], o Programa de Instalação deverá reiniciar o seu computador. Você gostaria de reiniciar agora?
FinishedRestartMessage=Para completar a instalação do [name], o Programa de Instalação deverá reiniciar o seu computador. Você gostaria de reiniciar agora?
ShowReadmeCheck=Sim, eu quero ver o arquivo LEIAME
YesRadio=&Sim, reiniciar o computador agora
NoRadio=&Não, eu reiniciarei o computador mais tarde
; used for example as 'Run MyProg.exe'
RunEntryExec=Executar %1
; used for example as 'View Readme.txt'
RunEntryShellExec=Visualizar %1

; *** "Setup Needs the Next Disk" stuff
ChangeDiskTitle=O Programa de Instalação precisa do próximo disco
SelectDiskLabel2=Insira o disco %1 e clique OK.%n%nSe os arquivos deste disco estiverem em uma outra pasta, digite o caminho correto ou clique em Procurar.
PathLabel=&Caminho:
FileNotInDir2=O arquivo "%1" não pôde ser encontrado em "%2". Insira o disco correto ou escolha outra pasta.
SelectDirectoryLabel=Indique a localização do próximo disco.

; *** Installation phase messages
SetupAborted=A instalação não foi completada.%n%nCorrija o problema e execute o Programa de Instalação novamente.
EntryAbortRetryIgnore=Clique Repetir para tentar novamente, Ignorar para continuar, Anular para cancelar.

; *** Installation status messages
StatusCreateDirs=Criando pastas...
StatusExtractFiles=Extraindo arquivos...
StatusCreateIcons=Criando ícones...
StatusCreateIniEntries=Criando entradas INI...
StatusCreateRegistryEntries=Criando entradas no registro...
StatusRegisterFiles=Registrando arquivos...
StatusSavingUninstall=Salvando informação para desinstalação...
StatusRunProgram=Terminando a instalação...
StatusRollback=Rolando de volta as mudanças...

; *** Misc. errors
ErrorInternal2=Erro interno: %1
ErrorFunctionFailedNoCode=%1 falhou
ErrorFunctionFailed=%1 falhou; código %2
ErrorFunctionFailedWithMessage=%1 falhou; código %2.%n%3
ErrorExecutingProgram=Não foi possível executar:%n%1

; *** Registry errors
ErrorRegOpenKey=Erro ao abrir a chave de registro:%n%1\%2
ErrorRegCreateKey=Erro ao criar a chave de registro:%n%1\%2
ErrorRegWriteKey=Erro ao escrever na chave de registro:%n%1\%2

; *** INI errors
ErrorIniEntry=Erro ao criar entrada INI no arquivo %1.

; *** File copying errors
FileAbortRetryIgnore=Clique Repetir para tentar novamente, Ignorar para ignorar este arquivo (não recomendado), ou Anular para cancelar a instalação.
FileAbortRetryIgnore2=Clique Repetir para tentar novamente, Ignorar para continuar assim mesmo (não recomendado), ou Anular para cancelar a instalação.
SourceIsCorrupted=O arquivo de origem está corrompido
SourceDoesntExist=O arquivo de origem "%1" não existe
ExistingFileReadOnly=O arquivo existente no seu computador está marcado como somente para leitura.%n%nClique em Repetir para remover o atributo de somente leitura e tentar novamente, Ignorar para continuar, ou Anular para cancelar a instalação.
ErrorReadingExistingDest=Um erro ocorreu ao tentar ler o arquivo existente no seu computador
FileExists=O arquivo já existe.%n%nVocê gostaria de sobrescrevê-lo?
ExistingFileNewer=O arquivo existente no seu computador é mais novo que aquele que o Programa de Instalação está tentando instalar. É recomendado que você mantenha o arquivo existente.%n%nVocê quer manter o arquivo existente?
ErrorChangingAttr=Um erro ocorreu ao tentar mudar os atributos do arquivo existente no seu computador
ErrorCreatingTemp=Um erro ocorreu ao tentar criar um arquivo na pasta destino
ErrorReadingSource=Um erro ocorreu ao tentar ler o arquivo fonte:
ErrorCopying=Um erro ocorreu ao tentar compiar um arquivo
ErrorReplacingExistingFile=Um erro ocorreu ao tentar substituir um arquivo existente:
ErrorRestartReplace=RestartReplace falhou:
ErrorRenamingTemp=Um erro ocorreu ao tentar renomear um arquivo na pasta destino:
ErrorRegisterServer=Não foi possível registrar DLL/OCX: %1
ErrorRegisterServerMissingExport=DllRegisterServer não encontrado
ErrorRegisterTypeLib=Não foi possível registrar a biblioteca de tipos: %1

; *** Post-installation errors
ErrorOpeningReadme=Um erro ocorreu ao tentar abrir o arquivo LEIAME.
ErrorRestartingComputer=O Programa de Instalação não conseguiu reiniciar o computador. Por favor faça isso manualmente.

; *** Uninstaller messages
UninstallNotFound=O arquivo "%1" não existe. Não é possível desinstalar.
UninstallOpenError=O arquivo "%1" não pode ser aberto. Não é possível desinstalar
UninstallUnsupportedVer=O arquivo de log de desinstação "%1" está em um formato que não é reconhecido por esta versão do desinstalador. Não é possível desinstalar
UninstallUnknownEntry=Uma entrada desconhecida (%1) foi encontrada no log de desinstalação
ConfirmUninstall=Você tem certeza que quer remover completamente %1 e todos os seus componentes?
OnlyAdminCanUninstall=Está instalação só pode ser desinstalada por um usuário com privilégios administrativos.
UninstallStatusLabel=Por favor, aguarde enquanto o %1 é removido do seu computador.
UninstalledAll=O %1 foi removido com sucesso do seu computador.
UninstalledMost=A desinstalação do %1 terminou.%n%nAlguns elementos não podem ser removidos. Estes elementos podem ser removidos manualmente.
UninstalledAndNeedsRestart=Para completar a desintalação do %1, você precisa reiniciar seu computador. %n%nVocê gostaria de reiniciar seu computador agora?
UninstallDataCorrupted=O arquivo "%1" está corrompido. Não pode desinstalar

; *** Uninstallation phase messages
ConfirmDeleteSharedFileTitle=Remover arquivo compartilhado?
ConfirmDeleteSharedFile2=O sistema indicou que o seguinte arquivo compartilhado não está mais sendo usando por nenhum outro programa. Você gostaria de remover este arquivo compartilhado?%n%n%Se qualquer programa ainda estiver usando este arquivo e ele for removido, este programa pode não funcionar corretamente. Se você não tiver certeza, escolha Não. Manter o arquivo no computador não causará nenhum problema.
SharedFileNameLabel=Nome do arquivo:
SharedFileLocationLabel=Local:
WizardUninstalling=Progresso da Desinstalação
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