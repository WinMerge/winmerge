; *** Inno Setup version 5.1.0+ Korean messages ***
;
; To download user-contributed translations of this file, go to:
;   http://www.jrsoftware.org/is3rdparty.php
;
; Note: When translating this text, do not add periods (.) to the end of
; messages that didn't have them already, because on those messages Inno
; Setup adds the periods automatically (appending a period would result in
; two periods being displayed).
;
; translated : Woong-Jae An (a183393@hanmail.net)
; 
; 번역은 좀 더 알아보기 쉽도록, 최소한 "직역"이 아닌, "의역"으로 하도록 노렸하였습니다.
; 만약에 제가 번역한 문장들이 원본과 뜻이 상이하거나 좀더 매끈한 표현으로 바꿨으면 하는 부분, 그 밖의 건의사항이나 질문이 있으시다면 주저마시고 메일 주시기 바랍니다.
;
; Fixed: Fixed bug in Select Setup Language box that not correctly showed Language Name, Korean. Sorry to find and fix it too late. (5.1.0)
; 수정: 설치 언어 선택 대화 상자에서 언어 이름인 "한국어"를 제대로 표시하지 못하던 버그를 수정하였습니다. 너무 늦게 발견하여 고친 점 죄송합니다. (5.1.0)
;
; $jrsoftware: issrc/Files/Default.isl,v 1.66 2005/02/25 20:23:48 mlaan Exp $

[LangOptions]
LanguageName=<D55C><AD6D><C5B4>
LanguageID=$0412
LanguageCodePage=949
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
SetupAppTitle=설치
SetupWindowTitle=설치 - %1
UninstallAppTitle=프로그램 제거
UninstallAppFullTitle=%1 제거

; *** Misc. common
InformationTitle=정보
ConfirmTitle=확인
ErrorTitle=오류

; *** SetupLdr messages
SetupLdrStartupMessage=이 프로그램은 %1을(를) 설치할 것입니다. 계속 하시겠습니까?
LdrCannotCreateTemp=임시 파일을 생성할 수 없습니다. 설치가 중단되었습니다
LdrCannotExecTemp=임시 폴더 내의 파일을 실행할 수 없습니다. 설치가 중단되었습니다

; *** Startup error messages
LastErrorMessage=%1.%n%n오류 %2: %3
SetupFileMissing=설치 폴더에 %1 파일이 없습니다. 문제를 해결해 주시거나 새로운 설치 프로그램을 구해보십시오.
SetupFileCorrupt=설치 파일이 손상되었습니다. 새로운 설치 프로그램을 구해보십시오.
SetupFileCorruptOrWrongVer=설치 파일이 손상되었거나 이 버전의 설치 프로그램과 호환이 되지 않습니다. 문제를 해결해 주시거나 새로운 설치 프로그램을 구해보십시오.
NotOnThisPlatform=이 프로그램은 %1 에서 설치되지 않습니다.
OnlyOnThisPlatform=이 프로그램은 %1 에서만 설치됩니다.
OnlyOnTheseArchitectures=이 프로그램은 다음과 같은 프로세서 아키텍처에 맞게 디자인된 Windows 에서만 설치됩니다:%n%n%1
MissingWOW64APIs=실행 중인 Windows 가 설치 프로그램이 64비트 설치를 수행하기 위한 기능을 포함하고 있지 않습니다. 이 문제를 해결하시려면, 서비스 팩 %1 을(를) 설치하십시오.
WinVersionTooLowError=이 프로그램은 %1 %2 (이)나 그 이상의 Windows 에서만 설치됩니다.
WinVersionTooHighError=이 프로그램은 %1 %2 (이)나 그 이상의 Windows 에서는 설치되지 않습니다.
AdminPrivilegesRequired=이 프로그램을 설치하기 위해서는 Administrator 권한이 필요합니다.
PowerUserPrivilegesRequired=이 프로그램을 설치하기 위해서는 Administrator 또는 Power Users 권한이 필요합니다.
SetupAppRunningError=설치 프로그램이 %1 이(가) 실행중인 것을 감지했습니다.%n%n지금 관련 프로그램을 모두 닫으십시오. 설치를 계속하시려면 "확인"을, 취소하시려면 "취소"를 클릭하십시오.
UninstallAppRunningError=설치 제거 프로그램이 %1 이(가) 실행중인 것을 감지했습니다.%n%n지금 관련 프로그램을 모두 닫으십시오. 설치를 계속하시려면 "확인"을, 취소하시려면 "취소"를 클릭하십시오.

; *** Misc. errors
ErrorCreatingDir=설치 프로그램이 "%1" 폴더를 생성할 수 없습니다
ErrorTooManyFilesInDir="%1" 폴더 안에 파일이 너무 많아 파일을 생성할 수 없습니다

; *** Setup common messages
ExitSetupTitle=설치 종료
ExitSetupMessage=설치가 아직 끝나지 않았습니다. 지금 종료하시면, 프로그램은 설치되지 않을 것입니다.%n%n설치를 완료하기 위해선 설치 프로그램을 다시 실행해야 합니다.%n%n설치를 종료하시겠습니까?
AboutSetupMenuItem=설치에 대하여(&A)...
AboutSetupTitle=설치에 대하여
AboutSetupMessage=%1 %2%n%3%n%n%1 홈페이지:%n%4
AboutSetupNote=
TranslatorNote=이 번역문은 안웅재에 의해 번역되었습니다. 이 번역에 대한 문의 사항은 E-Mail: a183393@hanmail.net 으로 해 주시기 바랍니다.

; *** Buttons
ButtonBack=< 뒤로(&B)
ButtonNext=다음(&N) >
ButtonInstall=설치(&I)
ButtonOK=확인
ButtonCancel=취소
ButtonYes=예(&Y)
ButtonYesToAll=모두 예(&A)
ButtonNo=아니요(&N)
ButtonNoToAll=모두 아니요(&O)
ButtonFinish=완료(&F)
ButtonBrowse=찾아보기(&B)...
ButtonWizardBrowse=찾아보기(&R)...
ButtonNewFolder=폴더 만들기(&M)

; *** "Select Language" dialog messages
SelectLanguageTitle=설치 언어 선택
SelectLanguageLabel=설치 과정 중에 사용할 언어를 선택해 주십시오:

; *** Common wizard text
ClickNext=설치를 계속 하시려면 "다음"을, 종료하시려면 "취소"를 클릭하십시오.
BeveledLabel=
BrowseDialogTitle=폴더 찾아보기
BrowseDialogLabel=아래의 목록에서 폴더를 선택하고 "확인"을 클릭하십시오.
NewFolderName=새 폴더

; *** "Welcome" wizard page
WelcomeLabel1=[name] 의 설치에 오신 것을 환영합니다
WelcomeLabel2=이 프로그램은 [name/ver] 을(를) 설치할 것입니다.%n%n설치를 계속하시기 전에 실행 중인 모든 프로그램의 종료를 권합니다.

; *** "Password" wizard page
WizardPassword=암호
PasswordLabel1=이 프로그램의 설치는 암호로 보호되고 있습니다.
PasswordLabel3=암호를 입력하신 후 "다음"을 클릭해 주십시오. 암호는 대소문자를 구분합니다.
PasswordEditLabel=암호(&P):
IncorrectPassword=입력하신 암호는 올바르지 않습니다. 다시 입력해 주십시오.

; *** "License Agreement" wizard page
WizardLicense=사용자 계약
LicenseLabel=설치를 계속하시기 전에 아래의 중요한 정보를 꼭 읽어보십시오.
LicenseLabel3=다음 사용자 계약을 자세히 읽어주십시오. 설치를 계속하시려면 이 계약에 동의해야 합니다.
LicenseAccepted=사용자 계약에 동의합니다(&A)
LicenseNotAccepted=사용자 계약에 동의하지 않습니다(&D)

; *** "Information" wizard pages
WizardInfoBefore=정보
InfoBeforeLabel=설치를 계속하시기 전에 아래의 중요한 정보를 꼭 읽어보십시오.
InfoBeforeClickLabel=설치를 계속할 준비가 되셨으면, "다음"을 클릭해 주십시오.
WizardInfoAfter=정보
InfoAfterLabel=설치를 끝마치기 전에 아래의 중요한 정보를 꼭 읽어보십시오.
InfoAfterClickLabel=다 읽으셨다면, "다음"을 클릭해 주십시오.

; *** "User Information" wizard page
WizardUserInfo=사용자 정보
UserInfoDesc=사용자 정보를 입력해 주십시오.
UserInfoName=사용자 이름(&U):
UserInfoOrg=단체명(&O):
UserInfoSerial=시리얼 넘버(&S):
UserInfoNameRequired=이름을 입력해야 합니다.

; *** "Select Destination Location" wizard page
WizardSelectDir=설치할 위치 선택
SelectDirDesc=어디에 [name] 을(를) 설치하시겠습니까?
SelectDirLabel3=설치 프로그램은 [name] 을(를) 다음 폴더에 설치할 것입니다.
SelectDirBrowseLabel=계속하시려면 "다음"을 클릭하십시오. 다른 폴더를 선택하시려면, "찾아보기"를 클릭하십시오.
DiskSpaceMBLabel=최소 [mb] MB 의 디스크 여유 공간이 설치에 필요합니다.
ToUNCPathname=선택하신 네트워크 경로로 설치할 수 없습니다. 네트워크 드라이브에 설치하시려면, 네트워크 드라이브에 접근할 수 있어야 합니다.
InvalidPath=드라이브 문자를 포함한 전체 경로를 입력하셔야 합니다. 예:%n%nC:\APP%n%n 네트워크 드라이브의 예:%n%n\\server\share
InvalidDrive=설치할 드라이브나 네트워크 경로가 존재하지 않거나 접근할 수 없습니다. 다른 경로를 선택하십시오.
DiskSpaceWarningTitle=디스크 공간 부족
DiskSpaceWarning=이 프로그램을 설치하는 데에 최소 %1 KB 의 여유공간이 필요하나, 선택하신 드라이브는 %2 KB 만 사용 가능합니다.%n%n그래도 계속하시겠습니까?
DirNameTooLong=폴더 이름이나 경로가 너무 깁니다.
InvalidDirName=폴더 이름이 정확하지 않습니다.
BadDirName32=폴더 이름에는 다음 문자들을 포함할 수 없습니다.%n%n%1
DirExistsTitle=존재하는 폴더
DirExists=폴더:%n%n%1%n%n이(가) 이미 존재합니다. 그래도 이 폴더에 설치하시겠습니까?
DirDoesntExistTitle=존재하지 않는 폴더
DirDoesntExist=폴더:%n%n%1%n%n이(가) 존재하지 않습니다. 폴더를 새로 만드시겠습니까?

; *** "Select Components" wizard page
WizardSelectComponents=구성 요소 설치
SelectComponentsDesc=어떤 구성 요소를 설치하시겠습니까?
SelectComponentsLabel2=설치하고 싶은 구성 요소는 선택하시고, 설치하고 싶지 않은 구성 요소는 선택을 해제하십시오. 설치를 계속할 준비가 되셨으면 "다음"을 클릭하십시오.
FullInstallation=전체 설치
; if possible don't translate 'Compact' as 'Minimal' (I mean 'Minimal' in your language)
CompactInstallation=최소 설치
CustomInstallation=사용자 설치
NoUninstallWarningTitle=기존 구성 요소 존재
NoUninstallWarning=설치 프로그램이 다음 구성 요소가 이미 설치되어 있음을 발견했습니다.%n%n%1%n%n프로그램 제거 시 이 구성 요소 들은 제거되지 않을 것입니다.%n%n그래도 계속 하시겠습니까?
ComponentSize1=%1 KB
ComponentSize2=%1 MB
ComponentsDiskSpaceMBLabel=선택한 구성 요소 설치에 필요한 최소 용량: [mb] MB

; *** "Select Additional Tasks" wizard page
WizardSelectTasks=추가 사항 적용
SelectTasksDesc=어떤 사항을 추가로 적용하시겠습니까?
SelectTasksLabel2=[name] 의 설치 과정에서 추가로 적용하고자 하는 사항을 선택하시고, "다음"을 클릭하십시오.

; *** "Select Start Menu Folder" wizard page
WizardSelectProgramGroup=시작 메뉴 폴더 선택
SelectStartMenuFolderDesc=어느 곳에 프로그램의 바로 가기를 만드시겠습니까?
SelectStartMenuFolderLabel3=설치 프로그램은 프로그램의 바로 가기를 다음 시작 메뉴 폴더에 만들 것입니다.
SelectStartMenuFolderBrowseLabel=계속하시려면 "다음"을 클릭하십시오. 다른 폴더를 선택하시려면 "찾아보기"를 클릭하십시오.
MustEnterGroupName=폴더 이름을 입력해야 합니다.
GroupNameTooLong=폴더 이름 또는 경로가 너무 깁니다.
InvalidGroupName=폴더 이름이 정확하지 않습니다.
BadGroupName=폴더 이름에는 다음의 문자들을 포함할 수 없습니다.%n%n%1
NoProgramGroupCheck2=시작 메뉴 폴더를 만들지 않음(&D)

; *** "Ready to Install" wizard page
WizardReady=설치 준비 완료
ReadyLabel1=[name] 을(를) 설치할 준비가 되었습니다.
ReadyLabel2a="설치"를 클릭하여 설치를 시작하시거나, "뒤로"를 클릭하여 설치 내용을 검토하거나 바꾸실 수 있습니다.
ReadyLabel2b="설치"를 클릭하여 설치를 시작하십시오.
ReadyMemoUserInfo=사용자 정보:
ReadyMemoDir=설치 경로:
ReadyMemoType=설치 종류:
ReadyMemoComponents=설치할 구성요소:
ReadyMemoGroup=시작 메뉴 폴더:
ReadyMemoTasks=추가로 적용되는 옵션:

; *** "Preparing to Install" wizard page
WizardPreparing=설치 준비 중...
PreparingDesc=설치 프로그램이 [name] 을(를) 설치할 준비를 하고 있습니다.
PreviousInstallNotCompleted=이전의 설치나 프로그램 제거 작업이 완료되지 않았습니다. 이전의 설치를 완료하기 위하여 컴퓨터를 재시작 할 필요가 있습니다.%n%n컴퓨터를 재시작 한 후, 설치 프로그램을 재시작하여 [name] 의 설치를 완료하십시오.
CannotContinue=설치를 계속할 수 없습니다. "취소"를 클릭하여 설치를 종료하십시오.

; *** "Installing" wizard page
WizardInstalling=설치 중...
InstallingLabel=설치 프로그램이 [name] 을(를) 설치하는 동안 기다려 주십시오.

; *** "Setup Completed" wizard page
FinishedHeadingLabel=[name] 설치 완료
FinishedLabelNoIcons=설치 프로그램이 [name] 의 설치를 완료했습니다.
FinishedLabel=설치 프로그램이 [name] 의 설치를 완료했습니다. 설치된 바로 가기를 실행하시면 프로그램이 실행됩니다.
ClickFinish="완료"를 클릭하여 설치를 완료하십시오.
FinishedRestartLabel=[name] 의 설치를 완료하려면 시스템이 다시 시작되어야만 합니다. 지금 시스템을 다시 시작하시겠습니까?
FinishedRestartMessage=[name] 의 설치를 완료하려면 시스템이 다시 시작되어야만 합니다.%n%n지금 시스템을 다시 시작하시겠습니까?
ShowReadmeCheck=README 파일을 읽어봅니다
YesRadio=예, 지금 시스템을 다시 시작하겠습니다(&Y)
NoRadio=아니요, 나중에 시스템을 다시 시작하겠습니다(&N)
; used for example as 'Run MyProg.exe'
RunEntryExec=%1 실행하기
; used for example as 'View Readme.txt'
RunEntryShellExec=%1 읽어보기

; *** "Setup Needs the Next Disk" stuff
ChangeDiskTitle=다음 디스크가 필요
SelectDiskLabel2=디스크 %1 을(를) 넣고 "확인" 단추를 클릭하십시오.%n%n만약 다음 디스크가 아래에 표시된 경로와 다른 폴더에 있다면 올바른 경로를 입력하시거나 "찾아보기" 단추를 클릭해 올바른 경로를 지정해 주십시오.
PathLabel=경로(&P):
FileNotInDir2="%1" 파일이 "%2" 에 존재하지 않습니다. 올바른 디스크를 넣거나 다른 폴더를 선택하십시오.
SelectDirectoryLabel=다음 디스크가 있는 경로를 지정하십시오.

; *** Installation phase messages
SetupAborted=설치가 완료되지 않았습니다.%n%n문제를 해결하고 설치 프로그램을 다시 시작하십시오.
EntryAbortRetryIgnore=다시 시도하시려면 "다시 시도"를, 무시하고 설치를 계속하시려면 "무시"를, 설치를 종료하시려면 "취소"를 클릭하십시오.

; *** Installation status messages
StatusCreateDirs=폴더 생성 중...
StatusExtractFiles=파일의 압축을 푸는 중...
StatusCreateIcons=바로 가기 생성 중...
StatusCreateIniEntries=INI 엔트리 생성 중...
StatusCreateRegistryEntries=레지스트리 키 생성 중...
StatusRegisterFiles=파일 등록 중...
StatusSavingUninstall=프로그램 제거 정보 저장 중...
StatusRunProgram=설치 마무리 중...
StatusRollback=설치 이전 상태로 되돌리는 중...

; *** Misc. errors
ErrorInternal2=내부 오류: %1
ErrorFunctionFailedNoCode=%1 실패
ErrorFunctionFailed=%1 실패; 코드 %2
ErrorFunctionFailedWithMessage=%1 실패; 코드 %2.%n%3
ErrorExecutingProgram=파일 실행 불가능:%n%1

; *** Registry errors
ErrorRegOpenKey=레지스트리 키 여는 중 오류 발생:%n%1\%2
ErrorRegCreateKey=레지스트리 키 생성 중 오류 발생:%n%1\%2
ErrorRegWriteKey=레지스트리 키 쓰는 중 오류 발생:%n%1\%2

; *** INI errors
ErrorIniEntry="%1" 파일에 INI 엔트리 생성 중 오류 발생.

; *** File copying errors
FileAbortRetryIgnore=다시 시도하시려면 "다시 시도"를, 무시하시려면 "무시"를 (권장하지 않음), 설치를 종료하시려면 "취소"를 클릭하십시오.
FileAbortRetryIgnore2=다시 시도하시려면 "다시 시도"를, 무시하시려면 "무시"를 (권장하지 않음), 설치를 종료하시려면 "취소"를 클릭하십시오.
SourceIsCorrupted=원본 파일이 손상되었습니다
SourceDoesntExist=원본 파일 "%1" 이(가) 존재하지 않습니다
ExistingFileReadOnly=다음 파일은 읽기 전용입니다.%n%n읽기 전용 속성을 제거하고 다시 시도하시려면 "다시 시도"를, 이 파일을 무시하시려면 "무시"를, 설치를 종료하시려면 "취소"를 클릭하십시오.
ErrorReadingExistingDest=파일 읽는 도중 오류 발생:
FileExists=이미 존재하는 파일입니다.%n%n파일을 덮어쓰시겠습니까?
ExistingFileNewer=이미 존재하는 파일이 설치하려는 파일보다 신버전입니다. 기존 파일을 보존할 것을 권장합니다.%n%n기존 파일을 보존하시겠습니까?
ErrorChangingAttr=파일 속성 변경 도중 오류 발생:
ErrorCreatingTemp=대상 폴더에 파일 생성 중 오류 발생:
ErrorReadingSource=원본 파일을 읽는 중 오류 발생:
ErrorCopying=파일 복사 중 오류 발생:
ErrorReplacingExistingFile=파일 덮어쓰는 중 오류 발생:
ErrorRestartReplace=RestartReplace 실패:
ErrorRenamingTemp=대상 폴더 내의 파일 이름 변경 도중 오류 발생:
ErrorRegisterServer=다음 DLL/OCX 을(를) 등록할 수 없음: %1
ErrorRegisterServerMissingExport=DllRegisterServer export 발견되지 않음
ErrorRegisterTypeLib=다음 파일 형식을 등록할 수 없음: %1

; *** Post-installation errors
ErrorOpeningReadme=README 파일을 여는 도중 오류가 발생했습니다.
ErrorRestartingComputer=설치 프로그램이 시스템을 재시작 할 수 없습니다. 수동으로 재시작 해 주십시오.

; *** Uninstaller messages
UninstallNotFound="%1" 파일이 존재하지 않습니다. 프로그램을 제거할 수 없습니다.
UninstallOpenError="%1" 파일을 열 수 없습니다. 프로그램을 제거할 수 없습니다
UninstallUnsupportedVer=프로그램 제거 정보 파일인 "%1" 이(가) 이 버전의 제거 프로그램이 인식할 수 없는 형식으로 되어 있습니다. 프로그램을 제거할 수 없습니다
UninstallUnknownEntry=알 수 없는 엔트리 (%1) 가 프로그램 제거 정보 파일에 기록되어 있습니다
ConfirmUninstall=정말로 %1 와(과) 그 구성 요소들을 완전히 제거하시겠습니까?
UninstallOnlyOnWin64=이 프로그램은 64비트 Windows 에서만 제거됩니다.
OnlyAdminCanUninstall=이 프로그램은 Administrator 권한이 있는 사용자만 제거하실 수 있습니다.
UninstallStatusLabel=%1 이(가) 제거되는 동안 기다려 주십시오.
UninstalledAll=%1 이(가) 완전히 제거되었습니다.
UninstalledMost=%1 의 제거가 끝났습니다.%n%n일부 항목은 제거할 수 없었습니다. 관련 항목들을 직접 제거하시기 바랍니다.
UninstalledAndNeedsRestart=%1 의 제거를 완료하려면, 컴퓨터가 다시 시작되어야 합니다.%n%n지금 컴퓨터를 다시 시작하시겠습니까?
UninstallDataCorrupted="%1" 파일이 손상되었습니다. 프로그램을 제거할 수 없습니다

; *** Uninstallation phase messages
ConfirmDeleteSharedFileTitle=공유 파일을 삭제하시겠습니까?
ConfirmDeleteSharedFile2=시스템은 다음 공유 파일이 어떤 프로그램에서도 사용되지 않음을 발견했습니다. 다음 공유 파일을 삭제하시겠습니까?%n%n만약 이 공유 파일들이 다른 프로그램들에 의해서 사용된다면, 이 공유 파일 삭제 후 다른 프로그램들이 제대로 작동하지 않을 수 있습니다. 확실하지 않다면 "아니요" 를 클릭하십시오. 파일을 남겨두어도 시스템에 영향을 끼치지 않습니다.
SharedFileNameLabel=파일 이름:
SharedFileLocationLabel=경로:
WizardUninstalling=설치 제거 상태
StatusUninstalling=%1 제거 중...

; The custom messages below aren't used by Setup itself, but if you make
; use of them in your scripts, you'll want to translate them.

[CustomMessages]
;Inno Setup Built-in Custom Messages
NameAndVersion=%1 버전 %2
AdditionalIcons=아이콘 생성:
OptionalFeatures=Optional Features:
CreateDesktopIcon=바탕 화면에 아이콘 생성(&D)
CreateQuickLaunchIcon=빠른 실행에 아이콘 생성(&Q)
ProgramOnTheWeb=웹 상의 %1
UninstallProgram=%1 제거
LaunchProgram=%1 실행
AssocFileExtension=%2 확장자를 %1 에 연결(&A)
AssocingFileExtension=%2 확장자를 %1 에 연결 중...

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
PortugueseBrazilLanguage=Portuguese (Brazilian) menus and dialogs
PortugueseLanguage=Portuguese menus and dialogs
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
