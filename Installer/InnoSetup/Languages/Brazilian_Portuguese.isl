; *** Inno Setup version 5.1.11+ Brazilian Portuguese messages ***
;
; To download user-contributed translations of this file, go to:
;   http://www.jrsoftware.org/files/istrans/
;
; Note: When translating this text, do not add periods (.) to the end of
; messages that didn't have them already, because on those messages Inno
; Setup adds the periods automatically (appending a period would result in
; two periods being displayed).

[LangOptions]
; The following three entries are very important. Be sure to read and 
; understand the '[LangOptions] section' topic in the help file.
LanguageName=Portugu<00EA>s (Brasil)
LanguageID=$0416
LanguageCodePage=1252

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
SetupAppTitle=Instalador
SetupWindowTitle=%1 - Instalador
UninstallAppTitle=Desinstalar
UninstallAppFullTitle=Desinstalar %1

; *** Misc. common
InformationTitle=Informação
ConfirmTitle=Confirmar
ErrorTitle=Erro

; *** SetupLdr messages
SetupLdrStartupMessage=Isto instalará o %1. Você deseja continuar?
LdrCannotCreateTemp=Incapaz de criar um arquivo temporário. Instalação abortada
LdrCannotExecTemp=Incapaz de executar o arquivo no diretório temporário. Instalação abortada

; *** Startup error messages
LastErrorMessage=%1.%n%nErro %2: %3
SetupFileMissing=Está faltando o arquivo %1 do diretório de instalação. Por favor, corrija o problema ou obtenha uma nova cópia do programa.
SetupFileCorrupt=Os arquivos de instalação estão corrompidos. Por favor, obtenha uma nova cópia do programa.
SetupFileCorruptOrWrongVer=Os arquivos de instalação estão corrompidos ou são incompatíveis com esta versão do Instalador. Por favor, corrija o problema ou obtenha uma nova cópia do programa.
NotOnThisPlatform=Este programa não executará no %1.
OnlyOnThisPlatform=Este programa deve ser executado no %1.
OnlyOnTheseArchitectures=Este programa só pode ser instalado em versões do Windows projetadas para as seguintes arquiteturas de processadores:%n%n% 1
MissingWOW64APIs=A versão do Windows que você está executando não inclui a funcionalidade requerida pelo Instalador para realizar uma instalação de 64 bits. Para corrigir este problema, por favor instale o Service Pack %1.
WinVersionTooLowError=Este programa requer %1 versão %2 ou posterior.
WinVersionTooHighError=Este programa não pode ser instalado na %1 versão %2 ou posterior.
AdminPrivilegesRequired=Você deve estar logado como um administrador quando instalar este programa.
PowerUserPrivilegesRequired=Você deve estar logado como um administrador ou como um membro do grupo de Usuários Power quando instalar este programa.
SetupAppRunningError=O Instalador detectou que o %1 está executando atualmente.%n%nPor favor, feche todas as instâncias dele agora, então clique em OK para continuar, ou em Cancelar para sair.
UninstallAppRunningError=O Desinstalador detectou que o %1 está executando atualmente.%n%nPor favor, feche todas as instâncias dele agora, então clique em OK para continuar, ou em Cancelar para sair.

; *** Misc. errors
ErrorCreatingDir=O Instalador foi incapaz de criar o diretório "%1"
ErrorTooManyFilesInDir=Incapaz de um arquivo no diretório "%1" porque ele contém arquivos demais

; *** Setup common messages
ExitSetupTitle=Sair do Instalador
ExitSetupMessage=A Instalação não está completa. Se você sair agora, o programa não será instalado.%n%nVocê pode executar o instalador de novo outra hora para completar a instalação.%n%nSair do Instalador?
AboutSetupMenuItem=&Sobre o Instalador...
AboutSetupTitle=Sobre o Instalador
AboutSetupMessage=%1 versão %2%n%3%n%n%1 home page:%n%4
AboutSetupNote=
TranslatorNote=

; *** Buttons
ButtonBack=< &Voltar
ButtonNext=&Próximo >
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
ButtonNewFolder=&Criar Nova Pasta

; *** "Select Language" dialog messages
SelectLanguageTitle=Selecionar o Idioma do Instalador
SelectLanguageLabel=Selecione o idioma para usar durante a instalação:

; *** Common wizard text
ClickNext=Clique em Próximo para continuar, ou em Cancelar para sair do Instalador.
BeveledLabel=
BrowseDialogTitle=Procurar Pasta
BrowseDialogLabel=Selecione uma pasta na lista abaixo, então clique em OK.
NewFolderName=Nova Pasta

; *** "Welcome" wizard page
WelcomeLabel1=Bem-vindo ao Assistente do Instalador do [name]
WelcomeLabel2=Isto instalará o [name/ver] no seu computador.%n%nÉ recomendado que você feche todos os outros aplicativos antes de continuar.

; *** "Password" wizard page
WizardPassword=Senha
PasswordLabel1=Esta instalação é protegida por senha.
PasswordLabel3=Por favor, forneça a senha, então clique em Próximo para continuar. As senhas são caso-sensitivo.
PasswordEditLabel=&Senha:
IncorrectPassword=A senha que você inseriu não está correta. Por favor, tente de novo.

; *** "License Agreement" wizard page
WizardLicense=Acordo de Licença
LicenseLabel=Por favor, leia as seguintes informações importantes antes de continuar.
LicenseLabel3=Por favor, leia o seguinte Acordo de Licença. Você deve aceitar os termos deste acordo antes de continuar com a instalação.
LicenseAccepted=Eu &aceito o acordo
LicenseNotAccepted=Eu &não aceito o acordo

; *** "Information" wizard pages
WizardInfoBefore=Informação
InfoBeforeLabel=Por favor, leia as seguintes informações importantes antes de continuar.
InfoBeforeClickLabel=Quando você estiver pronto para continuar com o Instalador, clique em Próximo.
WizardInfoAfter=Informação
InfoAfterLabel=Por favor, leia as seguintes informações importantes antes de continuar.
InfoAfterClickLabel=Quando você estiver pronto para continuar com o Instalador, clique em Próximo.

; *** "User Information" wizard page
WizardUserInfo=Informação do Usuário
UserInfoDesc=Por favor, insira suas informações.
UserInfoName=&Nome do Usuário:
UserInfoOrg=&Organização:
UserInfoSerial=&Número de Série:
UserInfoNameRequired=Você deve inserir um nome.

; *** "Select Destination Location" wizard page
WizardSelectDir=Selecione o Local de Destino
SelectDirDesc=Onde o [name] deve ser instalado?
SelectDirLabel3=O Instalador instalará o [name] na seguinte pasta.
SelectDirBrowseLabel=Para continuar, clique em Próximo. Se você gostaria de selecionar uma pasta diferente, clique em Procurar.
DiskSpaceMBLabel=Pelo menos [mb] MB de espaço livre em disco é requerido.
ToUNCPathname=O Instalador não pode instalar em um nome de caminho UNC. Se você está tentando instalar por uma rede, você precisará mapear um drive de rede.
InvalidPath=Você deve inserir um caminho completo com a letra do drive; por exemplo:%n%nC:\APP%n%não um caminho UNC na forma:%n%n\\servidor\compartilhamento
InvalidDrive=O drive ou compartilhamento UNC que você selecionou não existe ou não está acessível. Por favor, selecione outro.
DiskSpaceWarningTitle=Sem Espaço em Disco o Suficiente
DiskSpaceWarning=O Instalador requer pelo menos %1 KB de espaço livre para instalar, mas o drive selecionado apenas tem %2 KB disponíveis.%n%nVocê quer continuar de qualquer modo?
DirNameTooLong=O nome da pasta ou caminho é muito longo.
InvalidDirName=O nome da pasta não é válido.
BadDirName32=Os nomes das pastas não pode incluir quaisquer dos seguintes caracteres:%n%n%1
DirExistsTitle=A Pasta Existe
DirExists=A pasta:%n%n%1%n%njá existe. Você gostaria de instalar nesta pasta de qualquer maneira?
DirDoesntExistTitle=A Pasta Não Existe
DirDoesntExist=A pasta:%n%n%1%n%nnão existe. Você gostaria quer a pasta fosse criada?

; *** "Select Components" wizard page
WizardSelectComponents=Selecionar Componentes
SelectComponentsDesc=Quais componentes devem ser instalados?
SelectComponentsLabel2=Selecione os componentes que você quer instalar; desmarque os componentes que você não quer instalar. Clique em Próximo quando estiver pronto para continuar.
FullInstallation=Instalação completa
; if possible don't translate 'Compact' as 'Minimal' (I mean 'Minimal' in your language)
CompactInstallation=Instalação compacta
CustomInstallation=Instalação personalizada
NoUninstallWarningTitle=O Componente Existe
NoUninstallWarning=O Instalador detectou que os seguintes componentes já estão instalados no seu computador:%n%n%1%n%nNão selecionar estes componentes não desinstalará eles.%n%nVocê gostaria de continuar de qualquer modo?
ComponentSize1=%1 KB
ComponentSize2=%1 MB
ComponentsDiskSpaceMBLabel=A seleção atual requer pelo menos [mb] MB de espaço em disco.

; *** "Select Additional Tasks" wizard page
WizardSelectTasks=Selecionar Tarefas Adicionais
SelectTasksDesc=Quais tarefas adicionais devem ser executadas?
SelectTasksLabel2=Selecione as tarefas adicionais que você gostaria que o Instalador executasse enquanto instala o [name], então clique em Próximo.

; *** "Select Start Menu Folder" wizard page
WizardSelectProgramGroup=Selecionar a Pasta do Menu Iniciar
SelectStartMenuFolderDesc=Onde o Instalador deve colocar os atalhos do programa?
SelectStartMenuFolderLabel3=O Instalador criará os atalhos do programa na seguinte pasta do Menu Iniciar.
SelectStartMenuFolderBrowseLabel=Para continuar, clique em Próximo. Se você gostaria de selecionar uma pasta diferente, clique em Procurar.
MustEnterGroupName=Você deve inserir um nome de pasta.
GroupNameTooLong=O nome da pasta ou caminho é muito longo.
InvalidGroupName=O nome da pasta não é válido.
BadGroupName=O nome da pasta não pode incluir quaisquer dos seguintes caracteres:%n%n%1
NoProgramGroupCheck2=&Não criar uma pasta no Menu Iniciar

; *** "Ready to Install" wizard page
WizardReady=Pronto para Instalar
ReadyLabel1=O Instalador está agora pronto para começar a instalar o [name] no seu computador.
ReadyLabel2a=Clique em Instalar para continuar com a instalação, ou clique em Voltar se você quer revisar ou mudar quaisquer configurações.
ReadyLabel2b=Clique em Instalar para continuar com a instalação.
ReadyMemoUserInfo=Informação do usuário:
ReadyMemoDir=Local de destino:
ReadyMemoType=Tipo de Instalação:
ReadyMemoComponents=Componentes selecionados:
ReadyMemoGroup=Pasta do Menu Iniciar:
ReadyMemoTasks=Tarefas adicionais:

; *** "Preparing to Install" wizard page
WizardPreparing=Preparando para Instalar
PreparingDesc=O Instalador está se preparando para instalar o [name] no seu computador.
PreviousInstallNotCompleted=A instalação/remoção de um programa anterior não foi completada. Você precisará reiniciar o computador para completar essa instalação.%n%nApós reiniciar seu computador, execute o Instalador de novo para completar a instalação do [name].
CannotContinue=O Instalador não pode continuar. Por favor, clique em Cancelar para sair.

; *** "Installing" wizard page
WizardInstalling=Instalando
InstallingLabel=Por favor, aguarde enquanto o Instalador instala o [name] no seu computador.

; *** "Setup Completed" wizard page
FinishedHeadingLabel=Completando o Assistente do Instalador do [name]
FinishedLabelNoIcons=O Instalador terminou de instalar o [name] no seu computador.
FinishedLabel=O Instalador terminou de instalar o [name] no seu computador. O aplicativo pode ser iniciado selecionando os ícones instalados.
ClickFinish=Clique em Concluir para sair do Instalador.
FinishedRestartLabel=Para completar a instalação do [name], o Instalador deve reiniciar seu computador. Você gostaria de reiniciar agora?
FinishedRestartMessage=Para completar a instalação do [name], o Instalador deve reiniciar seu computador.%n%nVocê gostaria de reiniciar agora?
ShowReadmeCheck=Sim, eu quero visualizar o arquivo LEIA-ME
YesRadio=&Sim, reiniciar o computador agora
NoRadio=&Não, eu reiniciarei o computador depois
; used for example as 'Run MyProg.exe'
RunEntryExec=Executar %1
; used for example as 'View Readme.txt'
RunEntryShellExec=Visualizar %1

; *** "Setup Needs the Next Disk" stuff
ChangeDiskTitle=O Instalador Precisa do Próximo Disco
SelectDiskLabel2=Por favor, insira o Disco %1 e clique em OK.%n%nSe os arquivos neste disco podem ser achados numa pasta diferente do que a exibida abaixo, insira o caminho correto ou clique em Procurar.
PathLabel=&Caminho:
FileNotInDir2=O arquivo "%1" não pôde ser localizado em "%2". Por favor, insira o disco correto ou escolha outra pasta.
SelectDirectoryLabel=Por favor, especifique o local do próximo disco.

; *** Installation phase messages
SetupAborted=A instalação não foi completada.%n%nPor favor, corrija o problema e execute o Instalador de novo.
EntryAbortRetryIgnore=Clique em Repetir para tentar de novo, Ignorar para prosseguir de qualquer maneira, ou em Abortar para cancelar a instalação.

; *** Installation status messages
StatusCreateDirs=Criando diretórios...
StatusExtractFiles=Extraindo arquivos...
StatusCreateIcons=Criando atalhos...
StatusCreateIniEntries=Criando entradas INI...
StatusCreateRegistryEntries=Criando entradas no Registro...
StatusRegisterFiles=Registrando arquivos...
StatusSavingUninstall=Salvando informações de desinstalação...
StatusRunProgram=Concluindo a instalação...
StatusRollback=Desfazendo as mudanças...

; *** Misc. errors
ErrorInternal2=Erro interno: %1
ErrorFunctionFailedNoCode=%1 falhou
ErrorFunctionFailed=%1 falhou; código %2
ErrorFunctionFailedWithMessage=%1 falhou; código %2.%n%3
ErrorExecutingProgram=Incapaz de executar o arquivo:%n%1

; *** Registry errors
ErrorRegOpenKey=Erro ao abrir a chave do registro:%n%1\%2
ErrorRegCreateKey=Erro ao criar a chave do registro:%n%1\%2
ErrorRegWriteKey=Erro ao escrever na chave do registro:%n%1\%2

; *** INI errors
ErrorIniEntry=Erro ao criar entrada INI no arquivo "%1".

; *** File copying errors
FileAbortRetryIgnore=Clique em Repetir para tentar de novo, Ignorar para pular este arquivo (não recomendado) ou em Abortar para cancelar a instalação.
FileAbortRetryIgnore2=Clique em Repetir para tentar de novo, Ignorar para prosseguir de qualquer maneira (não recomendado), ou em Abortar para cancelar a instalação.
SourceIsCorrupted=O arquivo de origem está corrompido
SourceDoesntExist=O arquivo de origem "%1" não existe
ExistingFileReadOnly=O arquivo existente está marcado como somente leitura.%n%nClique em Repetir para remover o atributo somente leitura e tentar de novo, Ignorar para pular este arquivo, ou em Abortar para cancelar a instalação.
ErrorReadingExistingDest=Um erro ocorreu enquanto tentava ler o arquivo existente:
FileExists=O arquivo já existe.%n%nVocê gostaria que o Instalador sobrescrevesse ele?
ExistingFileNewer=O arquivo existente é mais novo do que o arquivo que o Instalador está tentando instalar. É recomendado que você mantenha o arquivo existente.%n%nVocê quer manter o arquivo existente?
ErrorChangingAttr=Um erro ocorreu enquanto tentava mudar os atributos do arquivo existente:
ErrorCreatingTemp=Um erro ocorreu enquanto tentava criar um arquivo no diretório destino:
ErrorReadingSource=Um erro ocorreu enquanto tentava ler o arquivo de origem:
ErrorCopying=Um erro ocorreu enquanto tentava copiar um arquivo:
ErrorReplacingExistingFile=Um erro ocorreu enquanto tentava substituir o arquivo existente:
ErrorRestartReplace=Reiniciar/Substituir falhou:
ErrorRenamingTemp=Um erro ocorreu enquanto tentava renomear um arquivo no diretório destino:
ErrorRegisterServer=Incapaz de registrar a DLL/OCX: %1
ErrorRegSvr32Failed=RegSvr32 falhou com o código de saída %1
ErrorRegisterTypeLib=Incapaz de registrar a biblioteca de tipos: %1

; *** Post-installation errors
ErrorOpeningReadme=Um erro ocorreu enquanto tentava abrir o arquivo README.
ErrorRestartingComputer=O Instalador foi incapaz de reiniciar o computador. Por favor, faça isto manualmente.

; *** Uninstaller messages
UninstallNotFound=O arquivo "%1" não existe. Não pode desinstalar.
UninstallOpenError=O arquivo "%1" não pôde ser aberto. Não pode desinstalar
UninstallUnsupportedVer=O arquivo de log da desinstalação "%1" está num formato não reconhecido por esta versão do desinstalador. Não pode desinstalar
UninstallUnknownEntry=Uma entrada desconhecida (%1) foi encontrada no log da desinstalação
ConfirmUninstall=Você tem certeza que você quer remover completamente o %1 e todos os seus componentes?
UninstallOnlyOnWin64=Esta instalação só pode ser desinstalada em Windows 64 bits.
OnlyAdminCanUninstall=Esta instalação só pode ser desinstalada por um usuário com privilégios administrativos.
UninstallStatusLabel=Por favor, aguarde enquanto o %1 é removido do seu computador.
UninstalledAll=%1 foi removido com sucesso do seu computador.
UninstalledMost=Desinstalação do %1 completa.%n%nAlguns elementos não puderam ser removidos. Estes podem ser removidos manualmente.
UninstalledAndNeedsRestart=Para completar a desinstalação do %1, seu computador deve ser reiniciado.%n%nVocê gostaria de reiniciar agora?
UninstallDataCorrupted=O arquivo "%1" está corrompido. Não pode desinstalar

; *** Uninstallation phase messages
ConfirmDeleteSharedFileTitle=Remover Arquivo Compartilhado?
ConfirmDeleteSharedFile2=O sistema indica que o seguinte arquivo compartilhado não está mais em uso por quaisquer programas. Você gostaria que a Desinstalação removesse este arquivo compartilhado?%n%nSe quaisquer programas ainda estão usando este arquivo e ele é removido, esses programas podem não funcionar apropriadamente. Se você está incerto, escolha Não. Deixar o arquivo no seu sistema não causará qualquer dano.
SharedFileNameLabel=Nome do arquivo:
SharedFileLocationLabel=Local:
WizardUninstalling=Status da Desinstalação
StatusUninstalling=Desinstalando o %1...

; The custom messages below aren't used by Setup itself, but if you make
; use of them in your scripts, you'll want to translate them.

[CustomMessages]
;Inno Setup Built-in Custom Messages
NameAndVersion=%1 versão %2
AdditionalIcons=Ícones adicionais:
OptionalFeatures=Funções Opcionais:
CreateDesktopIcon=Criar um ícone &na Área de Trabalho
CreateQuickLaunchIcon=Criar um ícone na &Barra de Inicialização Rápida
ProgramOnTheWeb=%1 na Web
UninstallProgram=Desinstalar o %1
LaunchProgram=Iniciar o %1
AssocFileExtension=&Associar o %1 com a extensão do arquivo %2
AssocingFileExtension=Associando o %1 com a extensão do arquivo %2...

;Things we can also localize
CompanyName=Software Thingamahoochie

;Types
TypicalInstallation=Instalação Típica
FullInstallation=Instalação Completa
CompactInstallation=Instalação Compacta
CustomInstallation=Instalação Personalizada

;Components
AppCoreFiles=Arquivos do Core do WinMerge
Filters=Filtros
Plugins=Plugins

;Localization Components
Languages=Idiomas
BulgarianLanguage=Menus e diálogos búlgaros
CatalanLanguage=Menus e diálogos catalãos
ChineseSimplifiedLanguage=Menus e diálogos chineses	(simplificados)
ChineseTraditionalLanguage=Menus e diálogos chineses (tradicionais)
CroatianLanguage=Menus e diálogos croatas
CzechLanguage=Menus e diálogos tchecos
DanishLanguage=Menus e diálogos dinamarqueses
DutchLanguage=Menus e diálogos holandeses
FrenchLanguage=Menus e diálogos franceses
GermanLanguage=Menus e diálogos alemães
GreekLanguage=Greek menus and dialogs
HungarianLanguage=Hungarian menus and dialogs
ItalianLanguage=Menus e diálogos italianos
JapaneseLanguage=Menus e diálogos japoneses
KoreanLanguage=Menus e diálogos coreanos
NorwegianLanguage=Menus e diálogos noruegueses
PolishLanguage=Menus e diálogos poloneses
PortugueseBrazilLanguage=Menus e diálogos portugueses (brasileiros)
PortugueseLanguage=Menus e diálogos portugueses
RomanianLanguage=Menus e diálogos romenos
RussianLanguage=Menus e diálogos russos
SlovakLanguage=Menus e diálogos eslovacos
SlovenianLanguage=Slovenian menus and dialogs
SpanishLanguage=Menus e diálogos espanhóis
SwedishLanguage=Menus e diálogos suecos
TurkishLanguage=Menus e diálogos turcos
UkrainianLanguage=Ukrainian menus and dialogs

;Tasks
ExplorerContextMenu=&Ativar a integração com o menu de contexto do explorer
IntegrateTortoiseCVS=Integrar com o &TortoiseCVS
IntegrateTortoiseSVN=Integrar com o T&ortoiseSVN
IntegrateClearCase=Integrar com o Rational &ClearCase

;Icon Labels
ReadMe=Leia Me
UsersGuide=Guia do Usuário
ViewStartMenuFolder=&Visualizar a Pasta do Menu Iniciar do WinMerge

;Code Dialogs
DeletePreviousStartMenu=O instalador detectou que você mudou o local do seu menu iniciar de "s" para "s". Você gostaria de apagar a pasta anterior do menu iniciar?

; Project file description
ProjectFileDesc=Arquivo do Projeto do WinMerge
