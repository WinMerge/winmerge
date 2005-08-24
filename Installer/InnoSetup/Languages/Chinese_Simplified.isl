; Translation made with Translator 1.32 (http://www2.arnes.si/~sopjsimo/translator.html)
; $Translator:NL=%n:TB=%t
;
; *** Inno Setup version 4.1.8+ Chinese Simplified messages ***
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
LanguageName=Chinese (PRC)
LanguageID=$0804
; If the language you are translating to requires special font faces or
; sizes, uncomment any of the following entries and change them accordingly.
;WelcomeFontName=Verdana
;WelcomeFontSize=12
;TitleFontSize=29

DialogFontName=
[Messages]

; *** Application titles
SetupAppTitle=安装
SetupWindowTitle=安装 - %1
UninstallAppTitle=卸载
UninstallAppFullTitle=%1 卸载

; *** Misc. common
InformationTitle=信息
ConfirmTitle=确认
ErrorTitle=错误

; *** SetupLdr messages
SetupLdrStartupMessage=即将安装 %1。是否继续？
LdrCannotCreateTemp=无法创建临时文件。安装中断
LdrCannotExecTemp=无法运行临时目录中的文件。安装中断

; *** Startup error messages
LastErrorMessage=%1。%n%n错误 %2: %3
SetupFileMissing=安装目录中文件 %1 丢失。请设法纠正此问题或者重新索取安装程序的副本。
SetupFileCorrupt=安装文件已经损坏。请重新索取安装程序的副本。
SetupFileCorruptOrWrongVer=安装文件已经损坏，或者版本不兼容。请设法纠正此问题或者重新索取安装程序的副本。
NotOnThisPlatform=本程序不能运行在 %1。
OnlyOnThisPlatform=本程序必须运行在 %1。
WinVersionTooLowError=本程序需要 %1 版本 %2 或更高。
WinVersionTooHighError=本程序不能安装在 %1 版本 %2 或更高。
AdminPrivilegesRequired=您必须以管理员身份登录才能安装本程序。
PowerUserPrivilegesRequired=要安装此程序，您必须以管理员或超级用户组的成员身份登录。
SetupAppRunningError=安装程序检测到 %1 正在运行。%n%n请您关闭其所有进程及副本，然后单击“确定”按钮继续，或按下“取消”退出安装。
UninstallAppRunningError=卸载程序检测到 %1 正在运行。%n%n请您关闭其所有进程及副本，然后单击“确定”按钮继续，或按下“取消”退出卸载。

; *** Misc. errors
ErrorCreatingDir=安装程序无法创建目录 "%1"
ErrorTooManyFilesInDir=无法在目录 "%1" 中创建文件，其中包含文件太多

; *** Setup common messages
ExitSetupTitle=退出安装
ExitSetupMessage=安装尚未完成。如果现在退出，此软件将不被安装。%n%n您可以稍后再运行安装程序来完成安装过程。%n%n是否退出安装？
AboutSetupMenuItem=关于安装(&A)...
AboutSetupTitle=关于安装
AboutSetupMessage=%1 版本 %2%n%3%n%n%1 主页：%n%4
AboutSetupNote=

; *** Buttons
ButtonBack=< 上一步(&B)
ButtonNext=下一步(&N) >
ButtonInstall=安装(&I)
ButtonOK=确定
ButtonCancel=取消
ButtonYes=是(&Y)
ButtonYesToAll=全部是(&A)
ButtonNo=否(&N)
ButtonNoToAll=全部否(&O)
ButtonFinish=完成(&F)
ButtonBrowse=浏览(&B)...
ButtonWizardBrowse=浏览(&R)...
ButtonNewFolder=新建文件夹(&M)

; *** "Select Language" dialog messages
SelectLanguageTitle=选择安装语言
SelectLanguageLabel=选择在安装过程中所使用的语言：

; *** Common wizard text
ClickNext=请单击“下一步”继续，或“取消”退出安装。
BeveledLabel=
BrowseDialogTitle=浏览文件夹
BrowseDialogLabel=在下面列表中选择一个文件夹，并单击“确定”。
NewFolderName=新建文件夹

; *** "Welcome" wizard page
WelcomeLabel1=欢迎使用 [name] 安装向导
WelcomeLabel2=此程序将安装 [name/ver] 到您的计算机中。%n%n强烈建议您在继续安装之前关闭其他所有正在运行的程序，以避免安装过程中可能产生的相互冲突。

; *** "Password" wizard page
WizardPassword=密码
PasswordLabel1=本安装程序受密码保护。
PasswordLabel3=请输入密码，然后单击“下一步”继续。密码对大小写敏感，请勿混淆。
PasswordEditLabel=密码(&P):
IncorrectPassword=输入的密码无效。请重试。

; *** "License Agreement" wizard page
WizardLicense=使用许可协议
LicenseLabel=在继续安装之前，请阅读下面的重要信息。
LicenseLabel3=请仔细阅读下面的使用许可协议。您必须在继续安装之前接受本协议。
LicenseAccepted=我接受该协议(&A)
LicenseNotAccepted=我不接受该协议(&A)

; *** "Information" wizard pages
WizardInfoBefore=信息
InfoBeforeLabel=在继续安装之前，请先阅读下面的重要信息。
InfoBeforeClickLabel=准备好后，请单击“下一步”。
WizardInfoAfter=信息
InfoAfterLabel=在安装结束之前，请先阅读下面的重要信息。
InfoAfterClickLabel=准备好后，请单击“下一步”。

; *** "User Information" wizard page
WizardUserInfo=用户信息
UserInfoDesc=请输入您的信息。
UserInfoName=用户名称(&U)：
UserInfoOrg=公司(&O)：
UserInfoSerial=序列号(&S)
UserInfoNameRequired=您必须输入一个名字。

; *** "Select Destination Location" wizard page
WizardSelectDir=选择目标位置
SelectDirDesc=您准备将 [name] 安装到哪里？
SelectDirLabel3=安装程序将安装 [name] 至下列文件夹。
SelectDirBrowseLabel=继续安装，请单击“下一步”。如果您希望选择其他文件夹，请单击“浏览”。
DiskSpaceMBLabel=安装本软件至少需要 [mb] MB 磁盘空间。
ToUNCPathname=安装程序无法将程序安装到一个 UNC 路径名。如果你确实需要将程序安装到网络上，必须先映射一个网络驱动器。
InvalidPath=您必须输入一个包含驱动器号的完整路径；例如：%n%nC:\APP%n%n或一个 UNC 路径形式:%n%n\\server\share
InvalidDrive=您所选择的驱动器或 UNC 共享不存在，或是不可存取的。请另外选择。
DiskSpaceWarningTitle=磁盘可用空间不足
DiskSpaceWarning=安装程序至少需要 %1 KB 的剩余磁盘空间，但是您选择的驱动器只有 %2 KB 可用。%n%n无论如何您都要继续安装吗？
DirNameTooLong=文件夹名或路径过长。
InvalidDirName=此文件夹名无效。
BadDirName32=文件夹名不能包含下列任何一个字符：%n%n%1
DirExistsTitle=文件夹已经存在
DirExists=文件夹：%n%n%1%n%n已经存在。 您无论如何都要安装到此文件夹吗？
DirDoesntExistTitle=文件夹不存在
DirDoesntExist=文件夹：%n%n%1%n%n不存在。是否创建新文件夹？

; *** "Select Components" wizard page
WizardSelectComponents=选择组件
SelectComponentsDesc=将准备安装哪些组件？
SelectComponentsLabel2=选择您准备安装的组件；清除您不想安装的组件。准备好后单击“下一步”继续。
FullInstallation=完全安装
; if possible don't translate 'Compact' as 'Minimal' (I mean 'Minimal' in your language)
CompactInstallation=典型安装
CustomInstallation=自定义安装
NoUninstallWarningTitle=组件已存在
NoUninstallWarning=安装程序检测到下列组件已经存在于您的计算机中：%n%n%1%n%n清除这些组件会导致其不能被卸载。%n%n您无论如何都要继续安装吗？
ComponentSize1=%1 KB
ComponentSize2=%1 MB
ComponentsDiskSpaceMBLabel=当前选项至少需要 [mb] MB 剩余磁盘空间。

; *** "Select Additional Tasks" wizard page
WizardSelectTasks=选择附加任务
SelectTasksDesc=您准备运行哪些附加任务？
SelectTasksLabel2=选择您准备在安装 [name] 期间执行的附加任务，然后单击“下一步”。

; *** "Select Start Menu Folder" wizard page
WizardSelectProgramGroup=选择开始菜单文件夹
SelectStartMenuFolderDesc=准备将程序的快捷方式放置在哪里？
SelectStartMenuFolderLabel3=安装程序将在下列开始菜单中创建程序的快捷方式。
SelectStartMenuFolderBrowseLabel=继续安装，请单击“下一步”。如果您希望选择其他文件夹，请单击“浏览”。
NoIconsCheck=不创建任何图标(&D)
MustEnterGroupName=您必须输入一个文件夹名。
GroupNameTooLong=文件夹名或路径过长。
InvalidGroupName=此文件夹名无效。
BadGroupName=文件夹名不能包含下列的任何一个字符：%n%n%1
NoProgramGroupCheck2=不创建开始菜单文件夹(&D)

; *** "Ready to Install" wizard page
WizardReady=准备安装
ReadyLabel1=安装程序已准备好安装 [name] 到您的计算机中。
ReadyLabel2a=单击“安装”按钮开始安装，或单击“上一步”返回更改刚才的设置。
ReadyLabel2b=单击“安装”按钮开始安装。
ReadyMemoUserInfo=用户信息：
ReadyMemoDir=目标位置：
ReadyMemoType=安装类型：
ReadyMemoComponents=选择组件：
ReadyMemoGroup=开始菜单文件夹：
ReadyMemoTasks=附加任务：

; *** "Preparing to Install" wizard page
WizardPreparing=准备安装
PreparingDesc=安装程序已准备好安装 [name] 到您的计算机中。
PreviousInstallNotCompleted=先前的程序安装或删除没有完成。你将需要重新启动你的计算机来完成程序安装或删除。在重新启动你的计算机之后，再一次运行 [name] 的安装程序完成安装。
CannotContinue=安装不能继续。 请单击“取消”取消安装。

; *** "Installing" wizard page
WizardInstalling=正在安装
InstallingLabel=正在安装 [name] 到您的计算机中，请等待。

; *** "Setup Completed" wizard page
FinishedHeadingLabel=安装完成
FinishedLabelNoIcons=[name] 在你计算机中安装完毕。
FinishedLabel=[name] 在你计算机中安装完毕。您可以选择已安装好的程序图标来启动程序。
ClickFinish=单击“完成”结束安装。
FinishedRestartLabel=要最后完成 [name] 的安装，必须重新启动计算机。是否立即重新启动？
FinishedRestartMessage=要最后完成 [name] 的安装，必须重新启动计算机。%n%n是否立即重新启动？
ShowReadmeCheck=是，我要查看自述文件
YesRadio=是，立即重新启动(&Y)
NoRadio=不，我将稍后重新启动(&N)
; used for example as 'Run MyProg.exe'
RunEntryExec=运行 %1
; used for example as 'View Readme.txt'
RunEntryShellExec=查看 %1

; *** "Setup Needs the Next Disk" stuff
ChangeDiskTitle=需要下一张磁盘
SelectDiskLabel2=请插入磁盘 %1 并单击“确定”。%n%n除了下面所示，如果安装文件也能在其他文件夹中找到，请输入其正确路径或单击“浏览”进行选择。
PathLabel=路径(&P)：
FileNotInDir2=文件“%1” 不能在“%2” 中找到。请插入正确的磁盘或选择另外的文件夹。
SelectDirectoryLabel=请指定下一张磁盘的位置。

; *** Installation phase messages
SetupAborted=安装不能完成。%n%n请设法纠正此问题，然后重新运行安装程序。
EntryAbortRetryIgnore=单击“重试”按钮重试，“忽略”无条件继续，“中止”取消安装。

; *** Installation status messages
StatusCreateDirs=正在创建目录...
StatusExtractFiles=正在展开文件...
StatusCreateIcons=正在创建程序图标...
StatusCreateIniEntries=正在创建 INI 条目...
StatusCreateRegistryEntries=正在创建注册表条目...
StatusRegisterFiles=正在进行文件注册...
StatusSavingUninstall=正在保存卸载信息...
StatusRunProgram=正在结束安装...
StatusRollback=正在收回改变...

; *** Misc. errors
ErrorInternal2=内部错误： %1
ErrorFunctionFailedNoCode=%1 失败
ErrorFunctionFailed=%1 失败；代码 %2
ErrorFunctionFailedWithMessage=%1 失败；代码 %2.%n%3
ErrorExecutingProgram=不能运行文件：%n%1

; *** Registry errors
ErrorRegOpenKey=打开注册表主键出错：%n%1\%2
ErrorRegCreateKey=创建注册表主键出错：%n%1\%2
ErrorRegWriteKey=写入注册表主键出错：%n%1\%2

; *** INI errors
ErrorIniEntry=创建文件 “%1” INI 条目时出错。

; *** File copying errors
FileAbortRetryIgnore=单击“重试”按钮重试，“忽略”跳过此文件(不建议)，或“中止”取消安装。
FileAbortRetryIgnore2=单击“重试”按钮重试，“忽略”无条件继续(不建议)，或“中止”取消安装。
SourceIsCorrupted=源文件受损
SourceDoesntExist=源文件 “%1” 不存在
ExistingFileReadOnly=已存在的文件被标记为只读。%n%n单击“重试”解除其只读属性并重试，“忽略”跳过此文件，“中止”取消安装。
ErrorReadingExistingDest=正尝试读取已有文件时出错：
FileExists=文件已经存在。%n%n你是否希望安装程序覆盖它？
ExistingFileNewer=正要安装的文件比现有文件更旧。建议保留已有文件。%n%n你是否希望保留它？
ErrorChangingAttr=正尝试改变已有文件属性时出错：
ErrorCreatingTemp=正尝试在目的目录中创建文件时出错：
ErrorReadingSource=正尝试读取源文件时出错：
ErrorCopying=正尝试复制文件时出错：
ErrorReplacingExistingFile=正尝试替换已有文件时出错：
ErrorRestartReplace=重新启动替换失败：
ErrorRenamingTemp=正尝试在目的目录中文件重命名时出错：
ErrorRegisterServer=不能注册 DLL/OCX： %1
ErrorRegisterServerMissingExport=DllRegisterServer 出口没有发现
ErrorRegisterTypeLib=不能注册类库： %1

; *** Post-installation errors
ErrorOpeningReadme=正尝试打开自述文件时出错。
ErrorRestartingComputer=安装程序不能重启动计算机。请手动进行。

; *** Uninstaller messages
UninstallNotFound=文件“%1” 不存在。不能卸载。
UninstallOpenError=不能打开 "%1" 文件。不能卸载。
UninstallUnsupportedVer=卸载记录文件 “%1” 的格式不能被此版本卸载程序识别。不能卸载
UninstallUnknownEntry=卸载记录文件中遇到不能识别的条目 (%1)
ConfirmUninstall=确认要完全删除 %1 和它所有的部件吗？
OnlyAdminCanUninstall=只能由有管理员权限的用户进行卸载。
UninstallStatusLabel=正在从您的计算机中卸载 %1，请等待。
UninstalledAll=%1 已经从您的计算机中成功卸载。
UninstalledMost=%1 卸载完成。%n%n某些项目不能移除。您可以手动删除。
UninstalledAndNeedsRestart=要完全卸载 %1, 您必须重新启动电脑。%n%n您希望现在重新启动电脑吗？
UninstallDataCorrupted=“%1” 文件受损。不能卸载

; *** Uninstallation phase messages
ConfirmDeleteSharedFileTitle=删除共享文件吗？
ConfirmDeleteSharedFile2=系统显示下列共享文件可能已不再被任何程序使用。是否移除这些共享文件？%n%n如果某个程序仍然用到这些文件而他们却已被删除，该程序可能不能正常运行。如果你不能确定，最好选择“否”。将这些文件保留在系统中不会产生什么危害。
SharedFileNameLabel=文件名：
SharedFileLocationLabel=位置：
WizardUninstalling=卸载状态
StatusUninstalling=正在卸载 %1...
[CustomMessages]
;Inno Setup Built-in Custom Messages
NameAndVersion=%1 version %2
AdditionalIcons=Additional icons:
OptionalFeatures=Optional Features:
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
IntegrateDelphi4=Borland Delphi &4 Binary File Support
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
