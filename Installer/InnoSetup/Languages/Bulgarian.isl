; Translation made with Translator 1.32 (http://www2.arnes.si/~sopjsimo/translator.html)
; $Translator:NL=%n:TB=%t
;
; *** Inno Setup version 4.1.8 Bulgarian messages ***
; Mikhail Balabanov <mishob-at-abv.bg>
;
; Note: When translating this text, do not add periods (.) to the end of
; messages that didn't have them already, because on those messages Inno
; Setup adds the periods automatically (appending a period would result in
; two periods being displayed).
;

[LangOptions]
LanguageName=<0411><044A><043B><0433><0430><0440><0441><043A><0438>
LanguageID=$0402
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

; *** Заглавия на приложенията
SetupAppTitle=Инсталиране
SetupWindowTitle=Инсталиране - %1
UninstallAppTitle=Деинсталиране
UninstallAppFullTitle=Деинсталиране - %1


; *** Разни
InformationTitle=Информация
ConfirmTitle=Потвърждение
ErrorTitle=Грешка

; *** Съобщения на зареждащия модул
SetupLdrStartupMessage=Тази програма ще инсталира %1. Желаете ли да продължите?
LdrCannotCreateTemp=Невъзможно е създаването на временен файл. Инсталацията е прекратена
LdrCannotExecTemp=Невъзможно е стартирането на файл от временната директория. Инсталацията е прекратена

; *** Грешки при стартиране
LastErrorMessage=%1.%n%nГрешка %2: %3
SetupFileMissing=Файлът %1 липсва от инсталационната директория. Моля, отстранете проблема или се снабдете с ново копие на програмата.
SetupFileCorrupt=Инсталационните файлове са повредени. Моля, снабдете се с ново копие на програмата.
SetupFileCorruptOrWrongVer=Инсталационните файлове са повредени или несъвместими с тази версия на инсталатора. Моля, отстранете проблема или се снабдете с ново копие на програмата.
NotOnThisPlatform=Тази програма не може да работи под %1.
OnlyOnThisPlatform=Тази програма може да работи само под %1.
WinVersionTooLowError=Тази програма изисква %1 версия %2 или по-нова.
WinVersionTooHighError=Тази програма не може да бъде инсталирана на %1 версия %2 или по-нова.
AdminPrivilegesRequired=За инсталирането на тази програма са необходими администраторски права.
PowerUserPrivilegesRequired=За инсталирането на тази програма трябва да се легитимирате като администратор или потребител с разширени права (power user).
SetupAppRunningError=Инсталаторът е открил, че %1 в момента работи.%n%nМоля, затворете всички стартирани нейни копия и натиснете OK за продължение или Cancel за изход.
UninstallAppRunningError=Деинсталаторът е открил, че %1 в момента работи.%n%nМоля, затворете всички нейни копия и натиснете OK за продължение или Cancel за изход.

; *** Разни грешки
ErrorCreatingDir=Невъзможно е създаването на директория "%1"
ErrorTooManyFilesInDir=Невъзможно е създаването на файл в директория "%1", тъй като тя съдържа прекалено много файлове

; *** Разни съобщения
ExitSetupTitle=Прекъсване на инсталацията
ExitSetupMessage=Инсталацията не е завършена. Ако я прекратите сега, програмата няма да бъде инсталирана.%n%nМожете да стартирате инсталатора по-късно, за да завършите инсталацията.%n%nЖелаете ли прекъсване на инсталацията?
AboutSetupMenuItem=&За инсталатора...
AboutSetupTitle=За инсталатора
AboutSetupMessage=%1 версия %2%n%3%n%nСтраница на %1 в WWW:%n%4
AboutSetupNote=

; *** Бутони
ButtonBack=< &Назад
ButtonNext=На&пред >
ButtonInstall=&Инсталиране
ButtonOK=OK
ButtonCancel=Отказ
ButtonYes=Д&а
ButtonYesToAll=Да (&всички)
ButtonNo=Н&е
ButtonNoToAll=Не (в&сички)
ButtonFinish=&Край
ButtonBrowse=&Избор...

; *** Стандартен текст на помощника
ButtonWizardBrowse=&Избор...
ButtonNewFolder=&Нова папка
SelectLanguageTitle=Избор на език за инсталатора
SelectLanguageLabel=Изберете език за използване при инсталирането:
ClickNext="Напред" - продължение, "Отказ" - изход.
BeveledLabel=

; *** Поздравителна страница на помощника
BrowseDialogTitle=Избор на папка
BrowseDialogLabel=Изберете папка от долния списък и натиснете OK.
NewFolderName=Нова папка
WelcomeLabel1=Инсталираща програма на [name]
WelcomeLabel2=Тази програма ще инсталира [name/ver] на компютъра Ви.%n%nПрепоръчително е да затворите всички останали работещи приложения, преди да продължите.

; *** Страница за парола/сериен номер:
WizardPassword=Парола
PasswordLabel1=Тази инсталация е защитена с парола.
PasswordLabel3=Моля, въведете паролата и натиснете "Напред". Спазвайте малките и главните букви.
PasswordEditLabel=&Парола:
IncorrectPassword=Въведената парола е неправилна. Моля, опитайте пак.

; *** Страница за лицензното съглашение
WizardLicense=Лицензно съглашение
LicenseLabel=Моля, прочетете следващата важна информация, преди да продължите.
LicenseLabel3=Моля, прочетете следващото лицензно съглашение. Трябва да потвърдите съгласието си, преди инсталацията да продължи.
LicenseAccepted=&Приемам съглашението
LicenseNotAccepted=&Отхвърлям съглашението

; *** Страници за допълнителна информация
WizardInfoBefore=Предварителна информация
InfoBeforeLabel=Моля, прочетете следващата важна информация, преди да продължите.
InfoBeforeClickLabel=Когато сте готови да продължите с инсталирането, натиснете "Напред".
WizardInfoAfter=Допълнителна информация
InfoAfterLabel=Моля, прочетете следващата важна информация, преди да продължите.
InfoAfterClickLabel=Когато сте готови да продължите с инсталирането, натиснете "Напред".

; *** Страница за информация за потребителя
WizardUserInfo=Информация за потребителя
UserInfoDesc=Моля, въведете личните си данни.
UserInfoName=&Име:
UserInfoOrg=&Организация:
UserInfoSerial=&Сериен номер:
UserInfoNameRequired=Трябва да въведете име.

; *** Страница за избор на директория - назначение
WizardSelectDir=Избор на директория - назначение
SelectDirDesc=Къде да бъде инсталирана [name]?
SelectDirLabel3=[name] ще бъде инсталирана в следната папка.
SelectDirBrowseLabel=За продължение натиснете Напред. За избор на друга папка натиснете Избор.
DiskSpaceMBLabel=Програмата изисква най-малко [mb] Мб на диска.
ToUNCPathname=Не може да се инсталира на мрежов път. Ако се опитвате да инсталирате в мрежа, ще трябва да зададете псевдоним за мрежовото устройство.
InvalidPath=Трябва да въведете пълен път с име на устройство, например:%n%nC:\APP%n%nили мрежов път UNC във вида:%n%n\\сървър\споделена_папка
InvalidDrive=Избраното от вас устройство не съществува. Моля, изберете друго.
DiskSpaceWarningTitle=Недостиг на дисково пространство
DiskSpaceWarning=Инсталирането изисква %1 Кб свободно място, но на избраното устройство има само %2 Кб.%n%nЖелаете ли да продължите въпреки това?
DirNameTooLong=Името на папката или пътят са твърде дълги.
InvalidDirName=Името на папка е невалидно.
BadDirName32=Име на директория не може да съдържа следните знаци:%n%n%1
;BadDirName16=Име на директория не може да съдържа шпации или следните знаци:%n%n%1
DirExistsTitle=Директорията съществува
DirExists=Директорията%n%n%1%n%nвече съществува. Искате ли да инсталирате в нея?
DirDoesntExistTitle=Директорията не съществува
DirDoesntExist=Директорията%n%n%1%n%nне съществува. Желаете ли да бъде създадена?

; *** Страница за избор на компоненти
WizardSelectComponents=Избор на компоненти
SelectComponentsDesc=Кои компоненти да бъдат инсталирани?
SelectComponentsLabel2=Отметнете компонентите, които желаете да се инсталират, и махнете отметките пред нежеланите. Натиснете "Напред" за продължение.
FullInstallation=Пълна инсталация
; if possible don't translate 'Compact' as 'Minimal' (I mean 'Minimal' in your language)
CompactInstallation=Компактна инсталация
CustomInstallation=Потребителска инсталация
NoUninstallWarningTitle=Съществуващи компоненти
NoUninstallWarning=Инсталаторът е открил, че следните компоненти са вече инсталирани на Вашия компютър:%n%n%1%n%nМахането на отметките няма да ги деинсталира.%n%nЖелаете ли все пак да продължите?
ComponentSize1=%1 Кб
ComponentSize2=%1 Мб
ComponentsDiskSpaceMBLabel=Текущият избор изисква най-малко [mb] Мб дисково пространство.

; *** Страница за допълнителни операции
WizardSelectTasks=Избор на допълнителни операции
SelectTasksDesc=Какви допълнителни операции да бъдат изпълнени?
SelectTasksLabel2=Изберете допълнителните операции, които искате да се извършат с инсталирането на [name], и натиснете "Напред":
WizardSelectProgramGroup=Избор на програмна група
SelectStartMenuFolderDesc=Къде да бъдат поставени иконите на програмата?
SelectStartMenuFolderLabel3=Иконите на програмата ще бъдат добавени в следната папка от менюто Start.
SelectStartMenuFolderBrowseLabel=За продължение натиснете Напред. За избор на друга папка натиснете Избор.
NoIconsCheck=&Инсталиране без икони
MustEnterGroupName=Трябва да въведете име на група.
GroupNameTooLong=Името на папката или пътят са твърде дълги.
InvalidGroupName=Името на папка е невалидно.
BadGroupName=Името на група не може да включва никой от следващите знаци:%n%n%1
NoProgramGroupCheck2=&Инсталиране без група в менюто Start

; *** Страница за готовност
WizardReady=Готовност за инсталиране
ReadyLabel1=Инсталаторът е готов да инсталира [name] на Вашия компютър.
ReadyLabel2a=Натиснете "Инсталиране", за да продължите, или "Назад", ако искате да прегледате или промените някоя настройка.
ReadyLabel2b=Натиснете "Инсталиране", за да продължите.
ReadyMemoUserInfo=Информация за потребителя:
ReadyMemoDir=Директория - назначение:
ReadyMemoType=Конфигурация:
ReadyMemoComponents=Избрани компоненти:
ReadyMemoGroup=Папка в менюто Start:

; *** Страница по време на инсталацията
ReadyMemoTasks=Допълнителни операции:

; *** Страница за избор на програмна група
WizardPreparing=Подготовка за инсталиране
PreparingDesc=Инсталаторът се подготвя да инсталира [name] на компютъра Ви.
PreviousInstallNotCompleted=Инсталирането/премахването на друга програма не е завършило. Трябва да рестартирате компютъра, за да завършите процедурата.%n%nСлед като рестартирате компютъра, стартирайте инсталатора отново, за да инсталирате [name].
CannotContinue=Инсталирането не може да продължи. Моля, натиснете Cancel за изход.
WizardInstalling=Инсталиране
InstallingLabel=Моля, изчакайте инсталирането на [name] на Вашия компютър.

; *** Страница след края на инсталирането
FinishedHeadingLabel=Приключване на инсталацията на [name]
FinishedLabelNoIcons=Инсталирането на [name] на Вашия компютър е завършено.
FinishedLabel=Инсталирането на [name] на Вашия компютър е завършено. Приложението може да бъде стартирано чрез избиране на добавените икони.
ClickFinish=Натиснете "Край" за изход от инсталатора.
FinishedRestartLabel=За да завърши инсталирането на [name], компютърът трябва да се рестартира. Желаете ли да рестартирате веднага?
FinishedRestartMessage=За да завърши инсталирането на [name], компютърът трябва да се рестартира.%n%nЖелаете ли да рестартирате веднага?
ShowReadmeCheck=Да, искам да прочета файла README
YesRadio=&Да, нека компютърът бъде рестартиран веднага
NoRadio=&Не, ще рестартирам компютъра по-късно
; used for example as 'Run MyProg.exe'
RunEntryExec=Стартиране на %1
; used for example as 'View Readme.txt'
RunEntryShellExec=Преглеждане на %1

; *** Текстове за смяна на дискетите
ChangeDiskTitle=Следваща дискета
SelectDiskLabel2=Моля, поставете дискета %1 и натиснете ОК.%n%nАко файловете от дискетата се намират в папка, различна от показаната по-долу, въведете пътя или натиснете "Избор".
PathLabel=&Път:
FileNotInDir2=Файлът "%1" липсва в "%2". Моля, поставете правилната дискета или изберете друга папка.
SelectDirectoryLabel=Моля, посочете местонахождението на следващия диск.

; *** Съобщения по време на инсталацията
SetupAborted=Инсталацията не е довършена.%n%nМоля, отстранете проблема и стартирайте инсталатора отново.
EntryAbortRetryIgnore=Изберете Retry за повторен опит, Ignore за продължение въпреки грешката, или Abort за прекъсване на инсталацията.

; *** Съобщения за състояние на инсталацията
StatusCreateDirs=Създаване на директории...
StatusExtractFiles=Разкомпресиране на файлове...
StatusCreateIcons=Създаване на икони...
StatusCreateIniEntries=Създаване на записи в инициализационен файл...
StatusCreateRegistryEntries=Създаване на записи в регистратурата...
StatusRegisterFiles=Регистриране на файлове...
StatusSavingUninstall=Записване на информация за деинсталиране...
StatusRunProgram=Завършване на инсталацията...

; *** Разни грешки
StatusRollback=Заличаване на промените...
ErrorInternal2=Вътрешна грешка: %1
ErrorFunctionFailedNoCode=%1 - неуспешно изпълнение
ErrorFunctionFailed=%1 - неуспешно изпълнение; код на грешката: %2
ErrorFunctionFailedWithMessage=%1 - неуспешно изпълнение; код на грешката: %2.%n%3
ErrorExecutingProgram=Невъзможно е стартирането на файл:%n%1

; *** Грешки, свързани с DDE

; *** Грешки, свързани с регистратурата
ErrorRegOpenKey=Грешка при отваряне на ключ в регистратурата:%n%1\%2
ErrorRegCreateKey=Грешка при създаване на ключ в регистратурата:%n%1\%2
ErrorRegWriteKey=Грешка при запис в ключ от регистратурата:%n%1\%2

; *** Грешки, свързани с .INI файлове
ErrorIniEntry=Грешка при създаване на инициализационен запис във файл "%1".

; *** Грешки при копиране
FileAbortRetryIgnore=Изберете Retry за повторен опит, Ignore за прескачане на файла (не се препоръчва) или Abort за прекратяване на инсталацията.
FileAbortRetryIgnore2=Изберете Retry за повторен опит, Ignore за продължение (не се препоръчва) или Abort за прекратяване на инсталацията.
SourceIsCorrupted=Файлът - източник е повреден
SourceDoesntExist=Файлът - източник "%1" не съществува
ExistingFileReadOnly=Съществуващият файл е с атрибут "само за четене".%n%nНатиснете Retry за отстраняване на атрибута и нов опит за запис, Ignore за прескачане на файла или Abort за да прекратяване на инсталацията.
ErrorReadingExistingDest=Грешка при четене на съществуващ файл:
FileExists=Файлът вече съществува.%n%nЖелаете ли инсталаторът да го замени?
ExistingFileNewer=Съществуващият файл е по-нов от този, който инсталаторът се опитва да запише. Препоръчително е да го запазите.%n%nЖелаете ли да запазите съществуващия файл?
ErrorChangingAttr=Грешка при опит за смяна на атрибутите на съществуващ файл:
ErrorCreatingTemp=Грешка при опит за създаване на файл в директорията - назначение:
ErrorReadingSource=Грешка при опит за четене на файл - източник:
ErrorCopying=Грешка при опит за копиране на файл:
ErrorReplacingExistingFile=Грешка при опит за заместване на съществуващ файл:
ErrorRestartReplace=Неуспешно отложено заместване на файл:
ErrorRenamingTemp=Грешка при опит за преименуване на файл в директорията - назначение:
ErrorRegisterServer=Не е възможно регистрирането на библиотека от тип DLL/OCX: %1
ErrorRegisterServerMissingExport=Не е намерен експорт DllRegisterServer
ErrorRegisterTypeLib=Не е възможно регистрирането на библиотека от типове: %1

; *** Грешки след края на инсталацията
ErrorOpeningReadme=Възникнала е грешка при опита за отваряне на файла README.
ErrorRestartingComputer=Инсталаторът не можа да рестартира компютъра. Моля, направете това ръчно.

; *** Грешки при деинсталиране
UninstallNotFound=Файлът "%1" не съществува. Деинсталирането е невъзможно.
UninstallOpenError=Файлът "%1" не може да се отвори. Деинсталирането е невъзможно
UninstallUnsupportedVer=Файлът с данни за деинсталиране "%1" е с формат, непознат за тази версия на деинсталатора. Деинсталирането е невъзможно
UninstallUnknownEntry=Неразпознат запис (%1) в инструкциите за деинсталиране
ConfirmUninstall=Сигурни ли сте, че искате напълно да премахнете %1 и всички прилежащи компоненти?
OnlyAdminCanUninstall=За да деинсталирате тази програма, трябва да имате администраторски права.
UninstallStatusLabel=Моля, изчакайте премахването на %1 от компютъра Ви.
UninstalledAll=%1 бе успешно премахната от компютъра.
UninstalledMost=Деинсталацията на %1 е приключена успешно.%n%nНякои елементи са останали. Можете да ги отстраните ръчно.
UninstalledAndNeedsRestart=За да завършите премахването на %1, компютърът трябва да бъде рестартиран.%n%nЩе желаете ли да рестартирате веднага?
UninstallDataCorrupted=Файлът "%1" е повреден. Деинсталирането е невъзможно

; *** Съобщения при деинсталиране
ConfirmDeleteSharedFileTitle=Премахване на споделен файл?
ConfirmDeleteSharedFile2=Системата отчита, че долният споделен файл вече не се използва от никоя програма. Желаете ли деинсталаторът да го отстрани?%n%nАко някоя програма все пак ползва файла и той бъде изтрит, тя може да спре да работи правилно. Ако се колебаете, изберете "Не". Оставянето на файла в системата е безвредно.
SharedFileNameLabel=Име на файла:
SharedFileLocationLabel=Местонахождение:
WizardUninstalling=Ход на деинсталирането
StatusUninstalling=Деинсталиране на %1...

; The custom messages below aren't used by Setup itself, but if you make
; use of them in your scripts, you'll want to translate them.

[CustomMessages]
;Inno Setup Built-in Custom Messages
NameAndVersion=%1 version %2
AdditionalIcons=Additional icons:
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