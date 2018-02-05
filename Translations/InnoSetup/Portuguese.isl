; *** Inno Setup version 5.1.11+ Portuguese (Portugal) messages ***
;
; Maintained by NARS (nars AT gmx.net)
;
; $jrsoftware: issrc/Files/Languages/Portuguese.isl,v 1.5 2008/02/21 22:56:57 nars Exp $

[LangOptions]
LanguageName=Portugu<00EA>s (Portugal)
LanguageID=$0816
LanguageCodePage=1252

[Messages]

; *** Application titles
SetupAppTitle=Instalação
SetupWindowTitle=%1 - Instalação
UninstallAppTitle=Desinstalação
UninstallAppFullTitle=%1 - Desinstalação

; *** Misc. common
InformationTitle=Informação
ConfirmTitle=Confirmação
ErrorTitle=Erro

; *** SetupLdr messages
SetupLdrStartupMessage=Irá ser instalado o %1. Deseja continuar?
LdrCannotCreateTemp=Não foi possível criar um ficheiro temporário. Instalação cancelada
LdrCannotExecTemp=Não foi possível executar um ficheiro na diretoria temporária. Instalação cancelada

; *** Startup error messages
LastErrorMessage=%1.%n%nErro %2: %3
SetupFileMissing=O ficheiro %1 não foi encontrado na pasta de instalação. Corrija o problema ou obtenha uma nova cópia do programa.
SetupFileCorrupt=Os ficheiros de instalação estão corrompidos. Obtenha uma nova cópia do programa.
SetupFileCorruptOrWrongVer=Os ficheiros de instalação estão corrompidos, ou são incompatíveis com esta versão do Assistente de Instalação. Corrija o problema ou obtenha uma nova cópia do programa.
NotOnThisPlatform=Este programa não pode ser executado no %1.
OnlyOnThisPlatform=Este programa deve ser executado no %1.
OnlyOnTheseArchitectures=Este programa só pode ser instalado em versões do Windows preparadas para as seguintes arquiteturas:%n%n%1
MissingWOW64APIs=A versão do Windows que está a utilizar não dispõe das funcionalidades necessárias para o Assistente de Instalação poder realizar uma instalação de 64-bit's. Para corrigir este problema, por favor instale o Service Pack %1.
WinVersionTooLowError=Este programa necessita do %1 versão %2 ou mais recente.
WinVersionTooHighError=Este programa não pode ser instalado no %1 versão %2 ou mais recente.
AdminPrivilegesRequired=Deve iniciar sessão como administrador para instalar este programa.
PowerUserPrivilegesRequired=Deve iniciar sessão como administrador ou membro do grupo de Super Utilizadores para instalar este programa.
SetupAppRunningError=O Assistente de Instalação detetou que o %1 está em execução. Feche-o e de seguida clique em OK para continuar, ou clique em Cancelar para cancelar a instalação.
UninstallAppRunningError=O Assistente de Desinstalação detetou que o %1 está em execução. Feche-o e de seguida clique em OK para continuar, ou clique em Cancelar para cancelar a desinstalação.

; *** Misc. errors
ErrorCreatingDir=O Assistente de Instalação não consegue criar a diretoria "%1"
ErrorTooManyFilesInDir=Não é possível criar um ficheiro na diretoria "%1" porque esta contém demasiados ficheiros

; *** Setup common messages
ExitSetupTitle=Terminar a instalação
ExitSetupMessage=A instalação não está completa. Se terminar agora, o programa não será instalado.%n%nMais tarde poderá executar novamente este Assistente de Instalação e concluir a instalação.%n%nDeseja terminar a instalação?
AboutSetupMenuItem=&Acerca de...
AboutSetupTitle=Acerca do Assistente de Instalação
AboutSetupMessage=%1 versão %2%n%3%n%n%1 home page:%n%4
AboutSetupNote=
TranslatorNote=Portuguese translation maintained by NARS (nars@gmx.net)

; *** Buttons
ButtonBack=< &Anterior
ButtonNext=&Seguinte >
ButtonInstall=&Instalar
ButtonOK=OK
ButtonCancel=Cancelar
ButtonYes=&Sim
ButtonYesToAll=Sim para &todos
ButtonNo=&Não
ButtonNoToAll=Nã&o para todos
ButtonFinish=&Concluir
ButtonBrowse=&Procurar...
ButtonWizardBrowse=P&rocurar...
ButtonNewFolder=&Criar Nova Pasta

; *** "Select Language" dialog messages
SelectLanguageTitle=Selecione o Idioma do Assistente de Instalação
SelectLanguageLabel=Selecione o idioma para usar durante a Instalação:

; *** Common wizard text
ClickNext=Clique em Seguinte para continuar ou em Cancelar para cancelar a instalação.
BeveledLabel=
BrowseDialogTitle=Procurar Pasta
BrowseDialogLabel=Selecione uma pasta na lista abaixo e clique em OK.
NewFolderName=Nova Pasta

; *** "Welcome" wizard page
WelcomeLabel1=Bem-vindo ao Assistente de Instalação do [name]
WelcomeLabel2=O Assistente de Instalação irá instalar o [name/ver] no seu computador.%n%nÉ recomendado que feche todas as outras aplicações antes de continuar.

; *** "Password" wizard page
WizardPassword=Palavra-passe
PasswordLabel1=Esta instalação está protegida por palavra-passe.
PasswordLabel3=Insira a palavra-passe e de seguida clique em Seguinte para continuar. Na palavra-passe existe diferença entre maiúsculas e minúsculas.
PasswordEditLabel=&Palavra-passe:
IncorrectPassword=A palavra-passe que introduziu não está correta. Tente novamente.

; *** "License Agreement" wizard page
WizardLicense=Contrato de licença
LicenseLabel=É importante que leia as seguintes informações antes de continuar.
LicenseLabel3=Leia atentamente o seguinte contrato de licença. Deve aceitar os termos do contrato antes de continuar a instalação.
LicenseAccepted=A&ceito o contrato
LicenseNotAccepted=&Não aceito o contrato

; *** "Information" wizard pages
WizardInfoBefore=Informação
InfoBeforeLabel=É importante que leia as seguintes informações antes de continuar.
InfoBeforeClickLabel=Quando estiver pronto para continuar clique em Seguinte.
WizardInfoAfter=Informação
InfoAfterLabel=É importante que leia as seguintes informações antes de continuar.
InfoAfterClickLabel=Quando estiver pronto para continuar clique em Seguinte.

; *** "User Information" wizard page
WizardUserInfo=Informações do utilizador
UserInfoDesc=Introduza as suas informações.
UserInfoName=Nome do &utilizador:
UserInfoOrg=&Organização:
UserInfoSerial=&Número de série:
UserInfoNameRequired=Deve introduzir um nome.

; *** "Select Destination Location" wizard page
WizardSelectDir=Selecione a localização de destino
SelectDirDesc=Onde deverá ser instalado o [name]?
SelectDirLabel3=O [name] será instalado na seguinte pasta.
SelectDirBrowseLabel=Para continuar, clique em Seguinte. Se desejar selecionar uma pasta diferente, clique em Procurar.
DiskSpaceMBLabel=É necessário pelo menos [mb] MB de espaço livre em disco.
ToUNCPathname=O Assistente de Instalação não pode instalar num caminho do tipo UNC. Se está a tentar fazer a instalação numa rede, precisa de mapear a unidade de rede.
InvalidPath=É necessário indicar o caminho completo com a letra de unidade; por exemplo:%n%nC:\APP%n%nou um caminho UNC no formato:%n%n\\servidor\partilha
InvalidDrive=A unidade ou partilha UNC selecionada não existe ou não está acessível. Selecione outra.
DiskSpaceWarningTitle=Não há espaço suficiente no disco
DiskSpaceWarning=O Assistente de Instalação necessita de pelo menos %1 KB de espaço livre, mas a unidade selecionada tem apenas %2 KB disponíveis.%n%nDeseja continuar de qualquer forma?
DirNameTooLong=O nome ou caminho para a pasta é demasiado longo.
InvalidDirName=O nome da pasta não é válido.
BadDirName32=O nome da pasta não pode conter nenhum dos seguintes caracteres:%n%n%1
DirExistsTitle=A pasta já existe
DirExists=A pasta:%n%n%1%n%njá existe. Pretende instalar nesta pasta?
DirDoesntExistTitle=A pasta não existe
DirDoesntExist=A pasta:%n%n%1%n%nnão existe. Pretende que esta pasta seja criada?

; *** "Select Components" wizard page
WizardSelectComponents=Selecione os componentes
SelectComponentsDesc=Que componentes deverão ser instalados?
SelectComponentsLabel2=Selecione os componentes que quer instalar e desmarque os componentes que não quer instalar. Clique em Seguinte quando estiver pronto para continuar.
FullInstallation=Instalação Completa
; if possible don't translate 'Compact' as 'Minimal' (I mean 'Minimal' in your language)
CompactInstallation=Instalação Compacta
CustomInstallation=Instalação Personalizada
NoUninstallWarningTitle=Componentes Encontrados
NoUninstallWarning=O Assistente de Instalação detetou que os seguintes componentes estão instalados no seu computador:%n%n%1%n%nSe desmarcar estes componentes estes não serão desinstalados.%n%nDeseja continuar?
ComponentSize1=%1 KB
ComponentSize2=%1 MB
ComponentsDiskSpaceMBLabel=A seleção atual necessita de pelo menos [mb] MB de espaço em disco.

; *** "Select Additional Tasks" wizard page
WizardSelectTasks=Selecione tarefas adicionais
SelectTasksDesc=Que tarefas adicionais deverão ser executadas?
SelectTasksLabel2=Selecione as tarefas adicionais que deseja que o Assistente de Instalação execute na instalação do [name] e em seguida clique em Seguinte.

; *** "Select Start Menu Folder" wizard page
WizardSelectProgramGroup=Selecione a pasta do Menu Iniciar
SelectStartMenuFolderDesc=Onde deverão ser colocados os ícones de atalho do programa?
SelectStartMenuFolderLabel3=Os ícones de atalho do programa serão criados na seguinte pasta do Menu Iniciar.
SelectStartMenuFolderBrowseLabel=Para continuar, clique em Seguinte. Se desejar selecionar uma pasta diferente, clique em Procurar.
MustEnterGroupName=É necessário introduzir um nome para a pasta.
GroupNameTooLong=O nome ou caminho para a pasta é demasiado longo.
InvalidGroupName=O nome da pasta não é válido.
BadGroupName=O nome da pasta não pode conter nenhum dos seguintes caracteres:%n%n%1
NoProgramGroupCheck2=&Não criar nenhuma pasta no Menu Iniciar

; *** "Ready to Install" wizard page
WizardReady=Pronto para Instalar
ReadyLabel1=O Assistente de Instalação está pronto para instalar o [name] no seu computador.
ReadyLabel2a=Clique em Instalar para continuar a instalação ou clique em Anterior se desejar rever ou alterar alguma das opções.
ReadyLabel2b=Clique em Instalar para continuar a instalação.
ReadyMemoUserInfo=Informações do utilizador:
ReadyMemoDir=Localização de destino:
ReadyMemoType=Tipo de instalação:
ReadyMemoComponents=Componentes selecionados:
ReadyMemoGroup=Pasta do Menu Iniciar:
ReadyMemoTasks=Tarefas adicionais:

; *** "Preparing to Install" wizard page
WizardPreparing=Preparando-se para instalar
PreparingDesc=Preparando-se para instalar o [name] no seu computador.
PreviousInstallNotCompleted=A instalação/desinstalação de uma versão anterior não ficou completa. Necessitará de reiniciar o computador para completar esta instalação.%n%nDepois de reiniciar o computador, execute novamente este Assistente de Instalação para completar a instalação do [name].
CannotContinue=A Instalação não pode continuar. Clique em Cancelar para sair.

; *** "Installing" wizard page
WizardInstalling=A instalar
InstallingLabel=Aguarde enquanto o Assistente de Instalação instala o [name] no seu computador.

; *** "Setup Completed" wizard page
FinishedHeadingLabel=Instalação do [name] concluída
FinishedLabelNoIcons=O Assistente de Instalação concluiu a instalação do [name] no seu computador.
FinishedLabel=O Assistente de Instalação concluiu a instalação do [name] no seu computador. A aplicação pode ser iniciada através dos ícones instalados.
ClickFinish=Clique em Concluir para finalizar o Assistente de Instalação.
FinishedRestartLabel=Para completar a instalação do [name], o Assistente de Instalação deverá reiniciar o seu computador. Deseja reiniciar agora?
FinishedRestartMessage=Para completar a instalação do [name], o Assistente de Instalação deverá reiniciar o seu computador.%n%nDeseja reiniciar agora?
ShowReadmeCheck=Sim, desejo ver o ficheiro LEIAME
YesRadio=&Sim, desejo reiniciar o computador agora
NoRadio=&Não, desejo reiniciar o computador mais tarde
; used for example as 'Run MyProg.exe'
RunEntryExec=Executar %1
; used for example as 'View Readme.txt'
RunEntryShellExec=Visualizar %1

; *** "Setup Needs the Next Disk" stuff
ChangeDiskTitle=O Assistente de Instalação precisa do disco seguinte
SelectDiskLabel2=Introduza o disco %1 e clique em OK.%n%nSe os ficheiros deste disco estiverem num local diferente do mostrado abaixo, indique o caminho correto ou clique em Procurar.
PathLabel=&Caminho:
FileNotInDir2=O ficheiro "%1" não foi encontrado em "%2". Introduza o disco correto ou selecione outra pasta.
SelectDirectoryLabel=Indique a localização do disco seguinte.

; *** Installation phase messages
SetupAborted=A instalação não está completa.%n%nCorrija o problema e execute o Assistente de Instalação novamente.
EntryAbortRetryIgnore=Clique em Repetir para tentar novamente, Ignorar para continuar de qualquer forma, ou Abortar para cancelar a instalação.

; *** Installation status messages
StatusCreateDirs=A criar directorias...
StatusExtractFiles=A extrair ficheiros...
StatusCreateIcons=A criar atalhos...
StatusCreateIniEntries=A criar entradas em INI...
StatusCreateRegistryEntries=A criar entradas no registo...
StatusRegisterFiles=A registar ficheiros...
StatusSavingUninstall=A guardar informações para desinstalação...
StatusRunProgram=A concluir a instalação...
StatusRollback=A anular as alterações...

; *** Misc. errors
ErrorInternal2=Erro interno: %1
ErrorFunctionFailedNoCode=%1 falhou
ErrorFunctionFailed=%1 falhou; código %2
ErrorFunctionFailedWithMessage=%1 falhou; código %2.%n%3
ErrorExecutingProgram=Não é possível executar o ficheiro:%n%1

; *** Registry errors
ErrorRegOpenKey=Erro ao abrir a chave de registo:%n%1\%2
ErrorRegCreateKey=Erro ao criar a chave de registo:%n%1\%2
ErrorRegWriteKey=Erro ao escrever na chave de registo:%n%1\%2

; *** INI errors
ErrorIniEntry=Erro ao criar entradas em INI no ficheiro "%1".

; *** File copying errors
FileAbortRetryIgnore=Clique em Repetir para tentar novamente, Ignorar para ignorar este ficheiro (não recomendado), ou Abortar para cancelar a instalação.
FileAbortRetryIgnore2=Clique em Repetir para tentar novamente, Ignorar para continuar de qualquer forma (não recomendado), ou Abortar para cancelar a instalação.
SourceIsCorrupted=O ficheiro de origem está corrompido
SourceDoesntExist=O ficheiro de origem "%1" não existe
ExistingFileReadOnly=O ficheiro existente tem o atributo "só de leitura".%n%nClique em Repetir para remover o atributo "só de leitura" e tentar novamente, Ignorar para ignorar este ficheiro, ou Abortar para cancelar a instalação.
ErrorReadingExistingDest=Ocorreu um erro ao tentar ler o ficheiro existente:
FileExists=O ficheiro já existe.%n%nDeseja substituí-lo?
ExistingFileNewer=O ficheiro existente é mais recente que o que está a ser instalado. É recomendado que mantenha o ficheiro existente.%n%nDeseja manter o ficheiro existente?
ErrorChangingAttr=Ocorreu um erro ao tentar alterar os atributos do ficheiro existente:
ErrorCreatingTemp=Ocorreu um erro ao tentar criar um ficheiro na directoria de destino:
ErrorReadingSource=Ocorreu um erro ao tentar ler o ficheiro de origem:
ErrorCopying=Ocorreu um erro ao tentar copiar um ficheiro:
ErrorReplacingExistingFile=Ocorreu um erro ao tentar substituir o ficheiro existente:
ErrorRestartReplace=RestartReplace falhou:
ErrorRenamingTemp=Ocorreu um erro ao tentar mudar o nome de um ficheiro na directoria de destino:
ErrorRegisterServer=Não é possível registar o DLL/OCX: %1
ErrorRegSvr32Failed=O RegSvr32 falhou com o código de saída %1
ErrorRegisterTypeLib=Não foi possível registar a livraria de tipos: %1

; *** Post-installation errors
ErrorOpeningReadme=Ocorreu um erro ao tentar abrir o ficheiro LEIAME.
ErrorRestartingComputer=O Assistente de Instalação não consegue reiniciar o computador. Por favor reinicie manualmente.

; *** Uninstaller messages
UninstallNotFound=O ficheiro "%1" não existe. Não é possível desinstalar.
UninstallOpenError=Não foi possível abrir o ficheiro "%1". Não é possível desinstalar.
UninstallUnsupportedVer=O ficheiro log de desinstalação "%1" está num formato que não é reconhecido por esta versão do desinstalador. Não é possível desinstalar
UninstallUnknownEntry=Foi encontrada uma entrada desconhecida (%1) no ficheiro log de desinstalação
ConfirmUninstall=Tem a certeza que deseja remover completamente o %1 e todos os seus componentes?
UninstallOnlyOnWin64=Esta desinstalação só pode ser realizada na versão de 64-bit's do Windows.
OnlyAdminCanUninstall=Esta desinstalação só pode ser realizada por um utilizador com privilégios administrativos.
UninstallStatusLabel=Por favor aguarde enquanto o %1 está a ser removido do seu computador.
UninstalledAll=O %1 foi removido do seu computador com sucesso.
UninstalledMost=A desinstalação do %1 está concluída.%n%nAlguns elementos não puderam ser removidos. Estes elementos podem ser removidos manualmente.
UninstalledAndNeedsRestart=Para completar a desinstalação do %1, o computador deve ser reiniciado.%n%nDeseja reiniciar agora?
UninstallDataCorrupted=O ficheiro "%1" está corrompido. Não é possível desinstalar

; *** Uninstallation phase messages
ConfirmDeleteSharedFileTitle=Remover ficheiro partilhado?
ConfirmDeleteSharedFile2=O sistema indica que o seguinte ficheiro partilhado já não está a ser utilizado por nenhum programa. Deseja removê-lo?%n%nSe algum programa ainda necessitar deste ficheiro, poderá não funcionar corretamente depois de o remover. Se não tiver a certeza, selecione Não. Manter o ficheiro não causará nenhum problema.
SharedFileNameLabel=Nome do ficheiro:
SharedFileLocationLabel=Localização:
WizardUninstalling=Estado da desinstalação
StatusUninstalling=A desinstalar o %1...

; The custom messages below aren't used by Setup itself, but if you make
; use of them in your scripts, you'll want to translate them.

[CustomMessages]

NameAndVersion=%1 versão %2
AdditionalIcons=Ícones adicionais:
OptionalFeatures=Opções adicionais:
CreateDesktopIcon=Criar ícone no Ambiente de &Trabalho
CreateQuickLaunchIcon=&Criar ícone na barra de Iniciação Rápida
ProgramOnTheWeb=%1 na Web
UninstallProgram=Desinstalar o %1
LaunchProgram=Executar o %1
AssocFileExtension=Associa&r o %1 aos ficheiros com a extensão %2
AssocingFileExtension=A associar o %1 aos ficheiros com a extensão %2...

;Things we can also localize
CompanyName=Thingamahoochie Software

;Types
TypicalInstallation=Instalação padrão
FullInstallation=Instalação completa
CompactInstallation=Instalação compacta
CustomInstallation=Instalação personalizada

;Components
AppCoreFiles=Ficheiros principais do WinMerge
Filters=Filtros
Plugins=Plugins

;Localization Components
Languages=Idiomas
BasqueLanguage=Basco menus e diálogos
BulgarianLanguage=Búlgaro menus e diálogos
CatalanLanguage=Catalão menus e diálogos
ChineseSimplifiedLanguage=Chinês (Simplificado) menus e diálogos
ChineseTraditionalLanguage=Chinês (Tradicional) menus e diálogos
CroatianLanguage=Croata menus e diálogos
CzechLanguage=Checo menus e diálogos
DanishLanguage=Dinamarquês menus e diálogos
DutchLanguage=Holandês menus e diálogos
FrenchLanguage=Francês menus e diálogos
GalicianLanguage=Galego menus e diálogos
GermanLanguage=Alemão menus e diálogos
GreekLanguage=Grego menus e diálogos
HungarianLanguage=Húngaro menus e diálogos
ItalianLanguage=Italiano menus e diálogos
JapaneseLanguage=Japonês menus e diálogos
KoreanLanguage=Coreano menus e diálogos
NorwegianLanguage=Norueguês menus e diálogos
PersianLanguage=Persa menus e diálogos
PolishLanguage=Polonês menus e diálogos
PortugueseBrazilLanguage=Português (Brasil) menus e diálogos
PortugueseLanguage=Português menus e diálogos
RomanianLanguage=Romeno menus e diálogos
RussianLanguage=Russo menus e diálogos
SerbianLanguage=Sérvio menus e diálogos
SlovakLanguage=Eslovaco menus e diálogos
SlovenianLanguage=Esloveno menus e diálogos
SpanishLanguage=Espanhol menus e diálogos
SwedishLanguage=Sueco menus e diálogos
TurkishLanguage=Turco menus e diálogos
UkrainianLanguage=Ucraniano menus e diálogos

;Tasks
ExplorerContextMenu=&Permitir o menu de contexto no Explorador
IntegrateTortoiseCVS=Integrar com &TortoiseCVS
IntegrateTortoiseGIT=Integrar com To&rtoiseGIT
IntegrateTortoiseSVN=Integrar com T&ortoiseSVN
IntegrateClearCase=Integrar com Rational &ClearCase
AddToPath=&Criar pasta do WinMerge no caminho do sistema

; 3-way merge wizard page
ThreeWayMergeWizardPageCaption=3-Combinação de modos
ThreeWayMergeWizardPageDescription=Utilizar o WinMerge como ferramenta de três vias para TortoiseSVN/GIT?
RegisterWinMergeAs3WayMergeTool=Registar o WinMerge como ferramenta de combinação de três vias
MergeAtRightPane=Combinar na janela direita
MergeAtCenterPane=Combinar na janela do meio
MergeAtLeftPane=Combinar na janela esquerda
AutoMergeAtStartup=Combinar automaticamente no arranque

;Icon Labels
ReadMe=Ler
UsersGuide=Guia do utilizador
ViewStartMenuFolder=&Ver a pasta do WinMerge do Menu Iniciar

;Code Dialogs
DeletePreviousStartMenu=O Assistente de Instalação detetou que alterou a localização de "%s" para "%s". Deseja eliminar a pasta anterior do Menu Iniciar?

; Project file description
ProjectFileDesc=Ficheiro do projeto WinMerge
