; *** Inno Setup version 5.1.0+ Simplified Chinese messages ***
;
; To download user-contributed translations of this file, go to:
;   http://www.jrsoftware.org/is3rdparty.php
;
;	Translated by Peng Bai
;		baipeng@sina.com
;
; $jrsoftware: issrc/Files/Languages/Chinese.isl,v 1.66 2005/03/05 11:33:56 mlaan Exp $

[LangOptions]
LanguageName=<7B80><4F53><4E2D><6587>
;简体中文
LanguageID=$0804
;LanguageCodePage=0
DialogFontName=宋体
DialogFontSize=9
;WelcomeFontName=Verdana
;WelcomeFontSize=12
TitleFontName=宋体_gb2312
;TitleFontSize=29
CopyrightFontName=宋体
CopyrightFontSize=9

DialogFontName=
[Messages]

; *** Application titles
SetupAppTitle=安装程序
SetupWindowTitle=安装程序 - %1
UninstallAppTitle=卸载程序
UninstallAppFullTitle=%1 卸载

; *** Misc. common
InformationTitle=消息
ConfirmTitle=确认
ErrorTitle=错误

; *** SetupLdr messages
SetupLdrStartupMessage=即将安装 %1。 是否继续?
LdrCannotCreateTemp=无法创建临时文件。 安装中断
LdrCannotExecTemp=无法执行临时文件夹下的文件。 安装中断

; *** Startup error messages
LastErrorMessage=%1.%n%n错误 %2: %3
SetupFileMissing=在安装文件中缺失 %1 。请纠正这个问题，或获取完整的程序。
SetupFileCorrupt=安装文件损坏，请获取完整的程序。
SetupFileCorruptOrWrongVer=安装文件损坏，或与当前版本不兼容。请纠正这个问题，或获取完整的程序。
NotOnThisPlatform=该软件将不能在 %1 上运行。
OnlyOnThisPlatform=该软件只能在 %1 上运行。
OnlyOnTheseArchitectures=该程序只能安装在为以下处理器设计的 Windows 上：%n%n%1
MissingWOW64APIs=您在运行的 Windows 版本不支持安装程序所需的64位功能。 请安装 Service Pack %1 纠正这个问题。
WinVersionTooLowError=本软件需要 %1 版本 %2 或更高的版本。
WinVersionTooHighError=本软件不能在 %1 版本 %2 或更高的版本上运行。
AdminPrivilegesRequired=安装时，你需要具有超级用户的权限。
PowerUserPrivilegesRequired=安装时，你需要具有超级用户的权限，或属于超级用户组。
SetupAppRunningError=安装程序检测到 %1 正在运行。%n%n请关闭这些程序，按 确定 继续，或 取消 退出。
UninstallAppRunningError=卸载程序检测到 %1 正在运行。%n%n请关闭这些程序，按 确定 继续，或 取消 退出。

; *** Misc. errors
ErrorCreatingDir=安装程序无法创建文件夹 "%1"
ErrorTooManyFilesInDir=无法在文件夹 "%1" 中加入新文件，因为其中已有太多文件了。

; *** Setup common messages
ExitSetupTitle=取消安装
ExitSetupMessage=安装尚未完成。如果您现在退出，程序尚未成功安装。%n%n你可以选择其它时间完成此安装过程.%n%n取消吗?
AboutSetupMenuItem=关于(&A)...
AboutSetupTitle=关于
AboutSetupMessage=%1 版本 %2%n%3%n%n%1 主页：%n%4
AboutSetupNote=

; *** Buttons
TranslatorNote=简体中文翻译由白鹏支持（baipeng@sina.com）
ButtonBack=< 上一步(&B)
ButtonNext=下一步(&N) >
ButtonInstall=安装(&I)
ButtonOK=确定
ButtonCancel=取消
ButtonYes=是(&Y)
ButtonYesToAll=均是(&A)
ButtonNo=否(&N)
ButtonNoToAll=均否(&o)
ButtonFinish=完成(&F)
ButtonBrowse=浏览(&B)...
ButtonWizardBrowse=浏览(&r)...
ButtonNewFolder=创建新的文件夹(&M)

; *** "Select Language" dialog messages
SelectLanguageTitle=选择安装语言
SelectLanguageLabel=选择你在安装过程中需要的语言：

; *** Common wizard text
ClickNext=点击 下一步 继续，点击 取消 取消安装。
BeveledLabel=
BrowseDialogTitle=选择安装目录，单击 浏览
BrowseDialogLabel=在列表中选择一个目录，然后点击 确定.
NewFolderName=新建文件夹

; *** "Welcome" wizard page
WelcomeLabel1=欢迎安装 [name]
WelcomeLabel2=即将在您的计算机上安装 [name/ver]。%n%n建议您关闭所有的运行程序后继续。

; *** "Password" wizard page
WizardPassword=密码
PasswordLabel1=安装受密码保护。
PasswordLabel3=请提供密码。点击 下一步 继续，密码大小写敏感。
PasswordEditLabel=密码(&P)：
IncorrectPassword=您的密码不正确，请重试。

; *** "License Agreement" wizard page
WizardLicense=许可证
LicenseLabel=在继续前，请阅读以下重要信息：
LicenseLabel3=请阅读版权许可，继续前必须同意其中的条款。
LicenseAccepted=我同意(&a)
LicenseNotAccepted=我不同意(&d)

; *** "Information" wizard pages
WizardInfoBefore=信息
InfoBeforeLabel=在继续前，请阅读以下重要信息。
InfoBeforeClickLabel=准备好了继续，点击 下一步。
WizardInfoAfter=信息
InfoAfterLabel=在继续前，请阅读以下重要信息。
InfoAfterClickLabel=准备好了继续，点击 下一步。

; *** "User Information" wizard page
WizardUserInfo=用户信息
UserInfoDesc=请输入您的相关信息：
UserInfoName=姓名(&U)：
UserInfoOrg=组织机构(&O)：
UserInfoSerial=序列号(&S)：
UserInfoNameRequired=您必须输入姓名。

; *** "Select Destination Location" wizard page
WizardSelectDir=选择安装文件夹
SelectDirDesc=在哪里安装 [name]？
SelectDirLabel3=安装程序将在以下文件夹安装 [name]。
SelectDirBrowseLabel=点击 下一步 继续。欲改变安装文件夹，点击 浏览。
DiskSpaceMBLabel=至少需要 [mb] MB 磁盘空间。
ToUNCPathname=不能安装到一个UNC路径下。如果您试图安装到网络，您必须将其映射为网络磁盘。
InvalidPath=你必须键入含盘符的全路径，例如：%n%nC:\APP%n%而不是一个这种形式的UNC路径：%n%n\\server\share
InvalidDrive=你选择的磁盘或UNC share不存在或无法访问。请选择其它的。
DiskSpaceWarningTitle=没有足够的磁盘空间。
DiskSpaceWarning=安装程序至少需要 %1 KB 可用空间，但所选磁盘只有 %2 KB 。%n%n你想强行继续吗？
DirNameTooLong=路径名太长。
InvalidDirName=目录名无效。
BadDirName32=目录名不能包含以下符合：%n%n%1
DirExistsTitle=文件夹已存在
DirExists=文件夹：%n%n%1%n%n已存在。你仍要安装到那个文件夹中吗？
DirDoesntExistTitle=文件夹不存在
DirDoesntExist=文件夹：%n%n%1%n%n不存在。你想创建它吗？

; *** "Select Components" wizard page
WizardSelectComponents=选择安装组件
SelectComponentsDesc=您希望安装哪些组件？
SelectComponentsLabel2=选择您希望安装的组件；清除不愿安装的组件。点击 下一步 继续。
FullInstallation=完全安装
CompactInstallation=典型安装
CustomInstallation=定制安装
NoUninstallWarningTitle=组件已存在
NoUninstallWarning=安装程序检测到在您的计算机上已存在以下组件：%n%n%1%n%n不选中它们，今后也将不卸载之。%n%n您想就这样继续吗？
ComponentSize1=%1 KB
ComponentSize2=%1 MB
ComponentsDiskSpaceMBLabel=当前选择至少需要 [mb] MB 磁盘空间。

; *** "Select Additional Tasks" wizard page
WizardSelectTasks=选择额外任务
SelectTasksDesc=您想执行什么额外任务？
SelectTasksLabel2=选择您希望安装程序安装 [name] 时执行的额外任务，再点击 下一步 继续安装。

; *** "Select Start Menu Folder" wizard page
WizardSelectProgramGroup=选择 开始 目录
SelectStartMenuFolderDesc=在哪里创建快捷方式？
SelectStartMenuFolderLabel3=安装程序将在以下位置创建快捷方式。
SelectStartMenuFolderBrowseLabel=点击 下一步 继续。欲选择别的目录，点击 浏览。
MustEnterGroupName=你必须键入一个名称。
GroupNameTooLong=目录名称或路径太长。
InvalidGroupName=目录名无效。
BadGroupName=目录名不能包含以下符号:%n%n%1
NoProgramGroupCheck2=不要在 开始 中创建任何目录。(&D)

; *** "Ready to Install" wizard page
WizardReady=准备安装
ReadyLabel1=安装程序即将安装 [name]。
ReadyLabel2a=点击 安装 继续安装，点击 上一步 检查并改变设置。
ReadyLabel2b=点击 安装 继续。
ReadyMemoUserInfo=用户信息：
ReadyMemoDir=安装文件夹：
ReadyMemoType=安装类型：
ReadyMemoComponents=安装组件：
ReadyMemoGroup=开始菜单：
ReadyMemoTasks=额外任务：

; *** "Preparing to Install" wizard page
WizardPreparing=准备安装
PreparingDesc=准备在您的计算机上安装 [name] 。
PreviousInstallNotCompleted=前次安装/卸载尚未完成。您必须重新启动后完成它。%n%n重启后，运行 安装程序 继续安装 [name]。
CannotContinue=安装无法继续。点击 取消 退出。

; *** "Installing" wizard page
WizardInstalling=正在安装
InstallingLabel=请稍等片刻，正在安装 [name]。

; *** "Setup Completed" wizard page
FinishedHeadingLabel=完成 [name] 安装向导
FinishedLabelNoIcons=正在完成安装 [name]。
FinishedLabel=[name]的安装已完成。点击图标可执行本程序。
ClickFinish=点击 完成 退出安装向导。
FinishedRestartLabel=必须重启计算机，完成安装 [name]。你想现在重新启动吗？
FinishedRestartMessage=必须重启计算机，完成安装 [name]。%n%n你想现在重新启动吗？
ShowReadmeCheck=是的，我想阅读 自述文件。
YesRadio=是，我想现在重启。(&Y)
NoRadio=不，我将稍后重启(&N)
; used for example as 'Run MyProg.exe'
RunEntryExec=运行 %1
; used for example as 'View Readme.txt'
RunEntryShellExec=查看 %1

; *** "Setup Needs the Next Disk" stuff
ChangeDiskTitle=安装需要下一张磁盘。
SelectDiskLabel2=请插入磁盘 %1 并点击 确定。%n%n如果可以在下面显示的文件夹之外找到这些文件，请输入正确的路径或点击 浏览。
PathLabel=路径(&P)：
FileNotInDir2=不能在 "%2" 中定位文件 "%1"。请输入正确的盘符或选择别的文件夹。
SelectDirectoryLabel=请指定下一个磁盘的位置。

; *** Installation phase messages
SetupAborted=安装未完成。%n%n请解决问题后，运行 安装程序 继续安装。
EntryAbortRetryIgnore=点击 重试 再试一遍，忽略 强制继续，中止 取消。

; *** Installation status messages
StatusCreateDirs=创建目录...
StatusExtractFiles=复制文件...
StatusCreateIcons=建立快捷方式...
StatusCreateIniEntries=创建 INI 项...
StatusCreateRegistryEntries=修改注册表...
StatusRegisterFiles=注册组件...
StatusSavingUninstall=储存卸载信息...
StatusRunProgram=完成安装...
StatusRollback=复原...

; *** Misc. errors
ErrorInternal2=内部错误：%1
ErrorFunctionFailedNoCode=%1 失败
ErrorFunctionFailed=%1 失败；错误码 %2
ErrorFunctionFailedWithMessage=%1 失败；错误码 %2.%n%3
ErrorExecutingProgram=无法执行文件：%n%1

; *** Registry errors
ErrorRegOpenKey=打开注册表主键：%n%1\%2 发生错误。
ErrorRegCreateKey=创建注册表主键：%n%1\%2 发生错误。
ErrorRegWriteKey=写入注册表主键：%n%1\%2 发生错误。

; *** INI errors
ErrorIniEntry=在 "%1" 中创建INI入口错误。

; *** File copying errors
FileAbortRetryIgnore=点击 重试 再试一遍，忽略 跳过该文件（不推荐），中止 取消安装。
FileAbortRetryIgnore2=点击 重试 再试一遍，忽略 强制继续（不推荐），中止 取消安装。
SourceIsCorrupted=源文件毁坏
SourceDoesntExist=源文件 "%1" 不存在
ExistingFileReadOnly=存在的文件属性为只读。%n%n点击 重试 去掉 只读 属性并继续，忽略 跳过该文件，中止 取消安装。
ErrorReadingExistingDest=当读取已存在文件时发生错误：
FileExists=文件已存在。%n%n您想覆盖它吗？
ExistingFileNewer=已存在的文件比要安装的文件更新。建议保留已存在文件。%n%n您想保留已存在文件吗？
ErrorChangingAttr=当改变此文件属性时发生错误：
ErrorCreatingTemp=在目的目录中创建文件发生错误：
ErrorReadingSource=当读取源文件时发生错误：
ErrorCopying=当复制文件时发生错误：
ErrorReplacingExistingFile=当覆盖已存在文件时发生错误：
ErrorRestartReplace=覆盖失败：
ErrorRenamingTemp=当试图给目的目录中文件改名时失败：
ErrorRegisterServer=无法注册 DLL/OCX： %1
ErrorRegisterServerMissingExport=DllRegisterServer支持未发现
ErrorRegisterTypeLib=无法注册类型：%1

; *** Post-installation errors
ErrorOpeningReadme=当试图打开 自述文件 时发生错误。
ErrorRestartingComputer=安装程序 无法重新启动。请自行重启。

; *** Uninstaller messages
UninstallNotFound=文件 "%1" 不存在，无法卸载。
UninstallOpenError=文件 "%1" 无法打开，不能卸载
UninstallUnsupportedVer=卸载信息 "%1" 格式不能被当前版本的卸载程序识别，无法卸载
UninstallUnknownEntry=在卸载信息中遭遇一个无法确认的键 (%1)
ConfirmUninstall=您确定完全删除 %1 和它的所有组件？
UninstallOnlyOnWin64=该程序只能在64位的 Windows 下安装。
OnlyAdminCanUninstall=运行这个卸载程序必须具有超级用户的权限。
UninstallStatusLabel=请稍等，正在从您的计算机中移去 %1 。
UninstalledAll=%1 成功从您的电脑上卸载。
UninstalledMost=%1 卸载完成。%n%n一些对象无法删除，但它们可以被手动删除。
UninstalledAndNeedsRestart=完成卸载过程 %1，必须重新启动。%n%n你想现在重启吗？
UninstallDataCorrupted="%1" 毁坏，无法卸载。

; *** Uninstallation phase messages
ConfirmDeleteSharedFileTitle=删除共享文件？
ConfirmDeleteSharedFile2=系统显示以下共享文件将不再被任何程序使用。你想 卸载程序 删除这些共享文件吗？%n%n如果某程序仍在使用这个文件，而它却被删除，这些程序可能无法正常运行。如果您不确定选择 否 保留它们，这不会对您的系统造成伤害。
SharedFileNameLabel=文件名：
SharedFileLocationLabel=位置：
WizardUninstalling=卸载状态
StatusUninstalling=正在卸载 %1...
[CustomMessages]
;Inno Setup Built-in Custom Messages
NameAndVersion=%1 版本 %2
AdditionalIcons=附加图标:
OptionalFeatures=Optional Features:
CreateDesktopIcon=在桌面创建图标(&d)
CreateQuickLaunchIcon=在快捷启动栏创建图标(&Q)
ProgramOnTheWeb=网络上的 %1
UninstallProgram=卸载 %1
LaunchProgram=运行 %1
AssocFileExtension=将 %1 与扩展名 %2 连接(&A)
AssocingFileExtension=正在将 %1 与扩展名 %2 连接...

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
IntegrateTortoiseSVN=Integrate with T&ortoiseSVN
IntegrateClearCase=Integrate with Rational &ClearCase
IntegrateDelphi4=Borland Delphi &4 Binary File Support
UpdatingCommonControls=Updating the System's Common Controls

;Icon Labels
ReadMe=Read Me
UsersGuide=User's Guide
UpdatingCommonControls=Updating the System's Common Controls
ViewStartMenuFolder=&View the WinMerge Start Menu Folder
PluginsText=Plugins

;Code Dialogs
DeletePreviousStartMenu=The installer has detected that you changed the location of your start menu from "%s" to "%s". Would you like to delete the previous start menu folder?

; Project file description
ProjectFileDesc=WinMerge Project file
