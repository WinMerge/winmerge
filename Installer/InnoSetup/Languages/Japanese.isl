; *** Inno Setup version 4.1.4+ Japanese messages ***
;
; To download user-contributed translations of this file, go to:
;   http://www.jrsoftware.org/is3rdparty.php
;
; Note: When translating this text, do not add periods (.) to the end of
; messages that didn't have them already, because on those messages Inno
; Setup adds the periods automatically (appending a period would result in
; two periods being displayed).
;
; $jrsoftware: issrc/Files/Default.isl,v 1.51 2004/02/08 18:50:49 jr Exp $

[LangOptions]
LanguageName=Japanese
LanguageID=$0411
; If the language you are translating to requires special font faces or
; sizes, uncomment any of the following entries and change them accordingly.
DialogFontName=ＭＳ Ｐゴシック
DialogFontSize=9
TitleFontName=ＭＳ Ｐゴシック
TitleFontSize=29
WelcomeFontName=ＭＳ Ｐゴシック
WelcomeFontSize=12
CopyrightFontName=ＭＳ Ｐゴシック
CopyrightFontSize=8

[Messages]

; *** Application titles
SetupAppTitle=セットアップ
SetupWindowTitle=%1 セットアップ
UninstallAppTitle=ア?イ?ストー?
UninstallAppFullTitle=%1 ア?イ?ストー?

; *** Misc. common
InformationTitle=情報
ConfirmTitle=確認
ErrorTitle=エ?ー

; *** SetupLdr messages
SetupLdrStartupMessage=%1 をイ?ストー?します。続行しますか？
LdrCannotCreateTemp=一?ファイ?を作成できません。セットアップを?止します。
LdrCannotExecTemp=一?フォ?ダのファイ?を実行できません。セットアップを?止します。

; *** Startup error messages
LastErrorMessage=%1.%n%nエ?ー %2: %3
SetupFileMissing=ファイ? %1 が見つかりません。問題を解?するか新しいセットアッププ?グ??を入手してください。
SetupFileCorrupt=セットアップファイ?が壊れています。新しいセットアッププ?グ??を入手してください。
SetupFileCorruptOrWrongVer=セットアップファイ?が壊れているか、このバージ??のセットアップと互換性が?りません。問題を解?するか新しいセットアッププ?グ??を入手してください。
NotOnThisPlatform=このプ?グ??は %1 では動作しません。
OnlyOnThisPlatform=このプ?グ??の実行には %1 が必要です。
WinVersionTooLowError=このプ?グ??の実行には %1 %2 以降が必要です。
WinVersionTooHighError=このプ?グ??は %1 %2 以降では動作しません。
AdminPrivilegesRequired=このプ?グ??をイ?ストー?するためには管?者として?グイ?する必要が?ります。
PowerUserPrivilegesRequired=このプ?グ??をイ?ストー?するためには管?者またはパ?ー?ーザーとして?グイ?する必要が?ります。
SetupAppRunningError=セットアップは実行?の %1 を?出しました。%n%n開いているアプ?ケーシ??をすべて閉じてから「OK」をク?ックしてください。「キ??セ?」をク?ックすると、セットアップを終了します。
UninstallAppRunningError=ア?イ?ストー?は実行?の %1 を?出しました。%n%n開いているアプ?ケーシ??をすべて閉じてから「OK」をク?ックしてください。「キ??セ?」をク?ックすると、セットアップを終了します。

; *** Misc. errors
ErrorCreatingDir=フォ?ダ %1 を作成?にエ?ーが発生しました。
ErrorTooManyFilesInDir=フォ?ダ %1 にファイ?を作成?にエ?ーが発生しました。ファイ?の?が多すぎます。

; *** Setup common messages
ExitSetupTitle=セットアップ終了
ExitSetupMessage=セットアップ作業は完了していません。ここでセットアップを?止するとプ?グ??はイ?ストー?されません。%n%n改めてイ?ストー?する場?は、もう一度セットアップを実行してください。%n%nセットアップを終了しますか？
AboutSetupMenuItem=セットアップについて(&A)...
AboutSetupTitle=セットアップについて
AboutSetupMessage=%1 %2%n%3%n%n%1 ホー?ページ:%n%4
AboutSetupNote=

; *** Buttons
ButtonBack=< 戻る(&B)
ButtonNext=?へ(&N) >
ButtonInstall=イ?ストー?(&I)
ButtonOK=OK
ButtonCancel=キ??セ?
ButtonYes=はい(&Y)
ButtonYesToAll=すべてはい(&A)
ButtonNo=いいえ(&N)
ButtonNoToAll=すべていいえ(&O)
ButtonFinish=完了(&F)
ButtonBrowse=参照(&B)...
ButtonWizardBrowse=参照(&R)
ButtonNewFolder=新しいフォ?ダ(&M)

; *** "Select Language" dialog messages
SelectLanguageTitle=セットアップの言語
SelectLanguageLabel=イ?スト??に使用する言語を選択します:

; *** Common wizard text
ClickNext=続行するには「?へ」、セットアップを終了するには「キ??セ?」をク?ックしてください。
BeveledLabel=
BrowseDialogTitle=フォ?ダ参照
BrowseDialogLabel=?ストからフォ?ダを選びOKを?してください。
NewFolderName=新しいフォ?ダ

; *** "Welcome" wizard page
WelcomeLabel1=[name] セットアップウィザードの開始
WelcomeLabel2=このプ?グ??はご使用のコ?ピ?ータへ [name/ver] をイ?ストー?します。%n%n続行する前に他のアプ?ケーシ??をすべて終了してください。

; *** "Password" wizard page
WizardPassword=パス?ード
PasswordLabel1=このイ?ストー?プ?グ??はパス?ードによって保護されています。
PasswordLabel3=パス?ードを入力して「?へ」をク?ックしてください。パス?ードは大文?と小文?が区別されます。
PasswordEditLabel=パス?ード(&P):
IncorrectPassword=入力されたパス?ードが正しく?りません。もう一度入力しなおしてください。

; *** "License Agreement" wizard page
WizardLicense=使用?諾契約?の同意
LicenseLabel=続行する前に以下の重要な情報をお読みください。
LicenseLabel3=以下の使用?諾契約?をお読みください。イ?ストー?を続行するにはこの契約?に同意する必要が?ります。
LicenseAccepted=同意する(&A)
LicenseNotAccepted=同意しない(&D)

; *** "Information" wizard pages
WizardInfoBefore=情報
InfoBeforeLabel=続行する前に以下の重要な情報をお読みください。
InfoBeforeClickLabel=セットアップを続行するには「?へ」をク?ックしてください。
WizardInfoAfter=情報
InfoAfterLabel=続行する前に以下の重要な情報をお読みください。
InfoAfterClickLabel=セットアップを続行するには「?へ」をク?ックしてください。

; *** "User Information" wizard page
WizardUserInfo=?ーザー情報
UserInfoDesc=?ーザー情報を入力してください。
UserInfoName=?ーザー名(&U):
UserInfoOrg=組織(&O):
UserInfoSerial=シ?ア?番?(&S):
UserInfoNameRequired=?ーザー名を入力してください。

; *** "Select Destination Directory" wizard page
WizardSelectDir=イ?ストー?先の指定
SelectDirDesc=[name] のイ?ストー?先を指定してください。
;SelectDirLabel2=[name] をイ?ストー?するフォ?ダを指定して、「?へ」をク?ックしてください。
SelectDirLabel3=[name] を以下のフォ?ダにイ?ストー?します。
SelectDirBrowseLabel=続行するには、「?へ」をク?ックしてください。別フォ?ダにイ?ストー?する場?は、「参照」をク?ックしてください。
DiskSpaceMBLabel=このプ?グ??は最低 [mb] MBのディスク空き領域を必要とします。
ToUNCPathname=セットアップはUNCフォ?ダにイ?ストー?することができません。ネット?ークにイ?ストー?する場?はネット?ークド?イブに?り?ててください。
InvalidPath=ド?イブ文?を含む完全なパスを入力してください。%n%n例：C:\APP%n%nまたはUNC形式のパスを入力してください。%n%n例：\\server\share
InvalidDrive=指定したド?イブまたはUNCパスが見つからないかアクセスできません。別のパスを指定してください。
DiskSpaceWarningTitle=ディスク空き領域の不足
DiskSpaceWarning=イ?ストー?には最低 %1 KBのディスク空き領域が必要ですが、指定されたド?イブには %2 KBの空き領域しか?りません。%n%nこのまま続行しますか？
DirNameTooLong=ディ?クト?名、?はパスが長過ぎます。
InvalidDirName=フォ?ダ名が無効です。
BadDirName32=以下の文?を含むフォ?ダ名は指定できません。:%n%n%1
DirExistsTitle=既存のフォ?ダ
DirExists=フォ?ダ %n%n%1%n%nが既に存在します。このままこのフォ?ダへイ?ストー?しますか？
DirDoesntExistTitle=新しいフォ?ダ
DirDoesntExistTitle=フォ?ダが見つかりません。
DirDoesntExist=フォ?ダ %n%n%1%n%nが見つかりません。新しいフォ?ダを作成しますか？

; *** "Select Components" wizard page
WizardSelectComponents=コ?ポーネ?トの選択
SelectComponentsDesc=イ?ストー?コ?ポーネ?トを選択してください。
SelectComponentsLabel2=イ?ストー?するコ?ポーネ?トを選択してください。イ?ストー?する必要のないコ?ポーネ?トはチェックを外してください。続行するには「?へ」をク?ックしてください。
FullInstallation=フ?イ?ストー?
; if possible don't translate 'Compact' as 'Minimal' (I mean 'Minimal' in your language)
CompactInstallation=コ?パクトイ?ストー?
CustomInstallation=カスタ?イ?ストー?
NoUninstallWarningTitle=既存のコ?ポーネ?ト
NoUninstallWarning=セットアップは以下のコ?ポーネ?トが既にイ?ストー?されていることを?出しました。%n%n%1%n%nこれらのコ?ポーネ?トの選択を解?してもア?イ?ストー?はされません。%n%nこのまま続行しますか？
ComponentSize1=%1 KB
ComponentSize2=%1 MB
ComponentsDiskSpaceMBLabel=現在の選択は最低 [mb] MBのディスク空き領域を必要とします。

; *** "Select Additional Tasks" wizard page
WizardSelectTasks=追加タスクの選択
SelectTasksDesc=実行する追加タスクを選択してください。
SelectTasksLabel2=[name] イ?ストー??に実行する追加タスクを選択して、「?へ」をク?ックしてください。

; *** "Select Start Menu Folder" wizard page
WizardSelectProgramGroup=プ?グ??グ?ープの指定
SelectStartMenuFolderDesc=プ?グ??アイコ?を作成する場?を指定してください。
SelectStartMenuFolderLabel3=セットアップは?のスタート?ニ?ーフォ?ダにプ?グ??のシ?ートカットを作成します。
;SelectStartMenuFolderLabel2=プ?グ??のアイコ?を作成するプ?グ??グ?ープを指定して、「?へ」をク?ックしてください。
SelectStartMenuFolderBrowseLabel=続行するには、「?へ」をク?ックしてください。別フォ?ダを選択したい場?は、「参照」をク?ックしてください。
NoIconsCheck=アイコ?を作成しない(&D)
MustEnterGroupName=グ?ープ名を指定してください。
GroupNameTooLong=フォ?ダ名?はパスが長過ぎます。
InvalidGroupName=グ?ープ名が無効です。
BadGroupName=以下の文?を含むグ?ープ名は指定できません。:%n%n%1
NoProgramGroupCheck2=プ?グ??グ?ープを作成しない(&D)

; *** "Ready to Install" wizard page
WizardReady=イ?ストー??備完了
ReadyLabel1=ご使用のコ?ピ?ータへ [name] をイ?ストー?する?備ができました。
ReadyLabel2a=イ?ストー?を続行するには「イ?ストー?」を、設定の確認や変更を行うには「戻る」をク?ックしてください。
ReadyLabel2b=イ?ストー?を続行するには「イ?ストー?」をク?ックしてください。
ReadyMemoUserInfo=?ーザー情報:
ReadyMemoDir=イ?ストー?先:
ReadyMemoType=セットアップの種類:
ReadyMemoComponents=選択コ?ポーネ?ト:
ReadyMemoGroup=プ?グ??グ?ープ:
ReadyMemoTasks=追加タスク一?:

; *** "Preparing to Install" wizard page
WizardPreparing=イ?ストー??備?
PreparingDesc=ご使用のコ?ピ?ータへ [name] をイ?ストー?する?備をしています。
PreviousInstallNotCompleted=前回行ったアプ?ケーシ??のイ?ストー?または削?が完了していません。完了するにはコ?ピ?ータを再起動する必要が?ります。%n%n[name] のイ?ストー?を完了するためには、再起動後にもう一度セットアップを実行してください。
CannotContinue=セットアップを続行できません。「キ??セ?」をク?ックしてセットアップを終了してください。

; *** "Installing" wizard page
WizardInstalling=イ?ストー?状況
InstallingLabel=ご使用のコ?ピ?ータに [name] をイ?ストー?しています。しばらくお待ちください。

; *** "Setup Completed" wizard page
FinishedHeadingLabel=[name] セットアップウィザードの完了
FinishedLabelNoIcons=ご使用のコ?ピ?ータに [name] がセットアップされました。
FinishedLabel=ご使用のコ?ピ?ータに [name] がセットアップされました。アプ?ケーシ??を実行するにはイ?ストー?されたアイコ?を選択してください。
ClickFinish=セットアップを終了するには「完了」をク?ックしてください。
FinishedRestartLabel=[name] のイ?ストー?を完了するためには、コ?ピ?ータを再起動する必要が?ります。すぐに再起動しますか？
FinishedRestartMessage=[name] のイ?ストー?を完了するためには、コ?ピ?ータを再起動する必要が?ります。%n%nすぐに再起動しますか？
ShowReadmeCheck=READMEファイ?を表示する。
YesRadio=すぐ再起動(&Y)
NoRadio=後で手動で再起動(&N)
; used for example as 'Run MyProg.exe'
RunEntryExec=%1 の実行
; used for example as 'View Readme.txt'
RunEntryShellExec=%1 の表示

; *** "Setup Needs the Next Disk" stuff
ChangeDiskTitle=ディスクの挿入
SelectDiskLabel2=ディスク %1 を挿入し、「OK」をク?ックしてください。%n%nこのディスクのファイ?が下に表示されているフォ?ダ以外の場?に?る場?は、正しいパスを入力するか「参照」ボタ?をク?ックしてください。
PathLabel=パス(&P):
FileNotInDir2=ファイ? %1 が %2 に見つかりません。正しいディスクを挿入するか、別のフォ?ダを指定してください。
SelectDirectoryLabel=?のディスクの?る場?を指定してください。

; *** Installation phase messages
SetupAborted=セットアップは完了していません。%n%n問題を解?してから、もう一度セットアップを実行してください。
EntryAbortRetryIgnore=もう一度やりなおすには「再?行」、エ?ーを無?して続行するには「無?」、イ?ストー?を?止するには「?止」をク?ックしてください。

; *** Installation status messages
StatusCreateDirs=フォ?ダを作成しています...
StatusExtractFiles=ファイ?を展開しています...
StatusCreateIcons=シ?－トカットを作成しています...
StatusCreateIniEntries=INIファイ?を設定しています...
StatusCreateRegistryEntries=?ジスト?を設定しています...
StatusRegisterFiles=ファイ?を登録しています...
StatusSavingUninstall=ア?イ?ストー?情報を保存しています...
StatusRunProgram=イ?ストー?を完了しています...
StatusRollback=変更を元に戻しています...

; *** Misc. errors
ErrorInternal2=内?エ?ー: %1
ErrorFunctionFailedNoCode=%1 エ?ー
ErrorFunctionFailed=%1 エ?ー: コード %2
ErrorFunctionFailedWithMessage=%1 エ?ー: コード %2.%n%3
ErrorExecutingProgram=ファイ?実行エ?ー:%n%1

; *** Registry errors
ErrorRegOpenKey=?ジスト?キーオープ?エ?ー:%n%1\%2
ErrorRegCreateKey=?ジスト?キー作成エ?ー:%n%1\%2
ErrorRegWriteKey=?ジスト?キー?き?みエ?ー:%n%1\%2

; *** INI errors
ErrorIniEntry=INIファイ?エ?ト?作成エ?ー: ファイ? %1

; *** File copying errors
FileAbortRetryIgnore=もう一度やりなおすには「再?行」、このファイ?をスキップして続行するには「無?」（?奨されません）、イ?ストー?を?止するには「?止」をク?ックしてください。
FileAbortRetryIgnore2=もう一度やりなおすには「再?行」、このファイ?をスキップして続行するには「無?」（?奨されません）、イ?ストー?を?止するには「?止」をク?ックしてください。
SourceIsCorrupted=コピー元のファイ?が壊れています。
SourceDoesntExist=コピー元のファイ? %1 が見つかりません。
ExistingFileReadOnly=既存のファイ?は読み取り専用です。%n%n読み取り専用属性を解?してもう一度やりなおすには「再?行」、このファイ?をスキップして続行するには「無?」、イ?ストー?を?止するには「?止」をク?ックしてください。
ErrorReadingExistingDest=既存のファイ?を読み?み?にエ?ーが発生しました。:
FileExists=ファイ?は既に存在します。%n%n上?きしますか？
ExistingFileNewer=イ?ストー?しようとしているファイ?よりも新しいファイ?が存在します。既存のファイ?を残すことをお奨めします。%n%n既存のファイ?を残しますか。
ErrorChangingAttr=既存ファイ?の属性を変更?にエ?ーが発生しました。:
ErrorCreatingTemp=コピー先のフォ?ダにファイ?を作成?にエ?ーが発生しました。:
ErrorReadingSource=コピー元のファイ?を読み?み?にエ?ーが発生しました。:
ErrorCopying=ファイ?をコピー?にエ?ーが発生しました。:
ErrorReplacingExistingFile=既存ファイ?を置き換え?にエ?ーが発生しました。:
ErrorRestartReplace=置き換え再開?にエ?ーが発生しました。:
ErrorRenamingTemp=コピー先フォ?ダのファイ?名を変更?にエ?ーが発生しました。:
ErrorRegisterServer=DLL/OCXの登録に失敗しました。: %1
ErrorRegisterServerMissingExport=DllRegisterServerエクスポートが見つかりません。
ErrorRegisterTypeLib=タイプ?イブ??への登録に失敗しました。: %1

; *** Post-installation errors
ErrorOpeningReadme=READMEファイ?のオープ?に失敗しました。
ErrorRestartingComputer=コ?ピ?ータの再起動に失敗しました。手動で再起動してください。

; *** Uninstaller messages
UninstallNotFound=ファイ? %1 が見つかりません。ア?イ?ストー?を実行できません。
UninstallOpenError=ファイ? %1 を開けることができません。ア?イ?ストー?を実行できません。
UninstallUnsupportedVer=ア?イ?ストー??グファイ? %1 は、このバージ??のア?イ?ストー?プ?グ??が認識できない形式です。ア?イ?ストー?を実行できません。
UninstallUnknownEntry=ア?イ?ストー??グに不明のエ?ト? %1 が見つかりました。
ConfirmUninstall=%1 とその関連コ?ポーネ?トをすべて削?します。よろしいですか？
OnlyAdminCanUninstall=ア?イ?ストー?するためには管?者?限が必要です。
UninstallStatusLabel=ご使用のコ?ピ?ータから %1 を削?しています。しばらくお待ちください。
UninstalledAll=%1 はご使用のコ?ピ?ータから正常に削?されました。
UninstalledMost=%1 のア?イ?ストー?が完了しました。%n%nいくつかの?目が削?できませんでした。手動で削?してください。
UninstalledAndNeedsRestart=[name] の削?を完了するためには、コ?ピ?ータを再起動する必要が?ります。すぐに再起動しますか？
UninstallDataCorrupted=ファイ? %1 が壊れています。ア?イ?ストー?を実行できません。

; *** Uninstallation phase messages
ConfirmDeleteSharedFileTitle=共有ファイ?の削?
ConfirmDeleteSharedFile2=システ?上で、?の共有ファイ?はどのプ?グ??でも使用されていません。この共有ファイ?を削?しますか？%n%n他のプ?グ??がまだこのファイ?を使用する場?、削?するとプ?グ??が動作しなくなる恐れが?ります。?まり確実でない場?は「いいえ」を選択してください。システ?にファイ?を残しても問題を引き起こすことは?りません。
SharedFileNameLabel=ファイ?名:
SharedFileLocationLabel=場?:
WizardUninstalling=ア?イ?ストー?状況
StatusUninstalling=%1 をア?イ?ストー?しています...

; The custom messages below aren't used by Setup itself, but if you make
; use of them in your scripts, you'll want to translate them.

[CustomMessages]
;Inno Setup Built-in Custom Messages
NameAndVersion=%1 version %2
AdditionalIcons=追加するアイコ?:
CreateDesktopIcon=デスクトップにアイコ?を作成する(&D)
CreateQuickLaunchIcon=クイック??チにアイコ?を作成する(&Q)
ProgramOnTheWeb=%1 on the Web
UninstallProgram=%1 のア?イ?ストー?
LaunchProgram=%1 を起動
AssocFileExtension=&Associate %1 with the %2 file extension
AssocingFileExtension=Associating %1 with the %2 file extension...

;Things we can also localize
CompanyName=Thingamahoochie Software

;Types
TypicalInstallation=標?イ?ストー?
FullInstallation=フ?イ?ストー?
CompactInstallation=最小イ?ストー?
CustomInstallation=カスタ?イ?ストー?

;Components
AppCoreFiles=WinMerge コアファイ?
ApplicationRuntimes=アプ?ケーシ????タイ?
UsersGuide=?ーザーガイド
Filters=フィ?タ
Plugins=プ?グイ? (Plugins.txtを参照)

;Localization Components
Languages=言語
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
ExplorerContextMenu=エクスプ?ー?のコ?テキスト?ニ?ーに追加(&E)
IntegrateTortoiseCVS=TortoiseCVSとの連携(&T)
IntegrateDelphi4=Borland Delphi 4 バイナ?ファイ?サポート(&4)
UpdatingCommonControls=システ?のコ??コ?ト?ー?をアップデートします

;Icon Labels
ReadMe=Read Me
UsersGuide=?ーザーガイド
UpdatingCommonControls=システ?のコ??コ?ト?ー?をアップデートしています
ViewStartMenuFolder=WinMerge スタート?ニ?ーフォ?ダを表示(&V)

;Code Dialogs
DeletePreviousStartMenu=イ?ストー?は、スタート?ニ?ーフォ?ダの位置が "%s" から "%s" に変更されたことを?出しました. 以前のスタート?ニ?ーフォ?ダを削?しますか?
