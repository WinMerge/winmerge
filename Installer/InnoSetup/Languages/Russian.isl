; *** Inno Setup version 4.2.2+ Russian messages ***
;
; Translation is made by Dmitry Kann, http://devtools.narod.ru/
; The highest accuracy was the first priority.
;
; Note: When translating this text, do not add periods (.) to the end of
; messages that didn't have them already, because on those messages Inno
; Setup adds the periods automatically (appending a period would result in
; two periods being displayed).
;
; $jrsoftware: issrc/Files/Languages/Russian.isl,v 1.7 2004/04/08 05:29:15 yktoo Exp $

[LangOptions]
LanguageName=<0420><0443><0441><0441><043A><0438><0439>
LanguageID=$0419
LanguageCodePage=1251

[Messages]

; *** Application titles
SetupAppTitle=Установка
SetupWindowTitle=Установка — %1
UninstallAppTitle=Деинсталляция
UninstallAppFullTitle=Деинсталляция — %1

; *** Misc. common
InformationTitle=Информация
ConfirmTitle=Подтверждение
ErrorTitle=Ошибка

; *** SetupLdr messages
SetupLdrStartupMessage=Данная программа установит %1 на Ваш компьютер, продолжить?
LdrCannotCreateTemp=Невозможно создать временный файл. Установка прервана
LdrCannotExecTemp=Невозможно выполнить файл во временном каталоге. Установка прервана

; *** Startup error messages
LastErrorMessage=%1.%n%nОшибка %2: %3
SetupFileMissing=Файл %1 отсутствует в папке установки. Пожалуйста, устраните проблему или получите новую версию программы.
SetupFileCorrupt=Установочные файлы повреждены. Пожалуйста, получите новую копию программы.
SetupFileCorruptOrWrongVer=Эти установочные файлы повреждены или несовместимы с данной версией программы установки. Пожалуйста, устраните проблему или получите новую копию программы.
NotOnThisPlatform=Эта программа не будет работать в %1.
OnlyOnThisPlatform=Эту программу можно запускать только в %1.
WinVersionTooLowError=Эта программа требует %1 версии %2 или выше.
WinVersionTooHighError=Программа не может быть установлена в %1 версии %2 или выше.
AdminPrivilegesRequired=Чтобы установить данную программу, Вы должны выполнить вход в систему как Администратор.
PowerUserPrivilegesRequired=Чтобы установить эту программу, Вы должны выполнить вход в систему как Администратор или член группы «Опытные пользователи» (Power Users).
SetupAppRunningError=Обнаружен запущенный экземпляр %1.%n%nПожалуйста, закройте все экземпляры приложения, затем нажмите «OK», чтобы продолжить, или «Отмена», чтобы выйти.
UninstallAppRunningError=Деинсталлятор обнаружил запущенный экземпляр %1.%n%nПожалуйста, закройте все экземпляры приложения, затем нажмите «OK», чтобы продолжить, или «Отмена», чтобы выйти.

; *** Misc. errors
ErrorCreatingDir=Невозможно создать папку "%1"
ErrorTooManyFilesInDir=Невозможно создать файл в каталоге "%1", так как в нём слишком много файлов

; *** Setup common messages
ExitSetupTitle=Выход из программы установки
ExitSetupMessage=Установка не завершена. Если Вы выйдете, программа не будет установлена.%n%nВы можете установить программу в другой раз.%n%nВыйти из программы установки?
AboutSetupMenuItem=&О программе...
AboutSetupTitle=О программе
AboutSetupMessage=%1, версия %2%n%3%n%nСайт %1:%n%4
AboutSetupNote=

; *** Buttons
ButtonBack=< &Назад
ButtonNext=&Далее >
ButtonInstall=&Установить
ButtonOK=OK
ButtonCancel=Отмена
ButtonYes=&Да
ButtonYesToAll=Да для &Всех
ButtonNo=&Нет
ButtonNoToAll=Н&ет для Всех
ButtonFinish=&Завершить
ButtonBrowse=&Обзор...

; *** "Select Language" dialog messages
ButtonWizardBrowse=&Обзор...
ButtonNewFolder=&Создать папку
SelectLanguageTitle=Выберите язык установки
SelectLanguageLabel=Выберите язык, который будет использован в  процессе установки:

; *** Common wizard text
ClickNext=Нажмите «Далее», чтобы продолжить, или «Отмена», чтобы выйти из программы установки.
BeveledLabel=

; *** "Welcome" wizard page
BrowseDialogTitle=Обзор папок
BrowseDialogLabel=Выберите папку из списка и нажмите «ОК».
NewFolderName=Новая папка
WelcomeLabel1=Вас приветствует Мастер установки [name]
WelcomeLabel2=Программа установит [name/ver] на Ваш компьютер.%n%nРекомендуется закрыть все прочие приложения перед тем, как продолжить.

; *** "Password" wizard page
WizardPassword=Пароль
PasswordLabel1=Эта программа защищена паролем.
PasswordLabel3=Пожалуйста, наберите пароль, потом нажмите «Далее». Пароли необходимо вводить с учётом регистра.
PasswordEditLabel=&Пароль:
IncorrectPassword=Введенный Вами пароль неверен. Пожалуйста, попробуйте снова.

; *** "License Agreement" wizard page
WizardLicense=Лицензионное Соглашение
LicenseLabel=Пожалуйста, прочтите следующую важную информацию перед тем, как продолжить.
LicenseLabel3=Пожалуйста, прочтите следующее Лицензионное Соглашение. Вы должны принять условия этого соглашения перед тем, как продолжить.
LicenseAccepted=Я &принимаю условия соглашения
LicenseNotAccepted=Я &не принимаю условия соглашения

; *** "Information" wizard pages
WizardInfoBefore=Информация
InfoBeforeLabel=Пожалуйста, прочитайте следующую важную информацию перед тем, как продолжить.
InfoBeforeClickLabel=Когда Вы будете готовы продолжить установку, нажмите «Далее».
WizardInfoAfter=Информация
InfoAfterLabel=Пожалуйста прочитайте следующую важную информацию перед тем, как продолжить.
InfoAfterClickLabel=Когда Вы будете готовы продолжить установку, нажмите «Далее».

; *** "User Information" wizard page
WizardUserInfo=Информация о пользователе
UserInfoDesc=Пожалуйста, введите данные о себе.
UserInfoName=&Имя и фамилия пользователя:
UserInfoOrg=&Организация:
UserInfoSerial=&Серийный номер:
UserInfoNameRequired=Вы должны ввести имя.

; *** "Select Destination Directory" wizard page
WizardSelectDir=Выбор папки установки
SelectDirDesc=В какую папку Вы хотите установить [name]?
SelectDirLabel3=Программа установит [name] в следующую папку.
SelectDirBrowseLabel=Нажмите «Далее», чтобы продолжить. Если Вы хотите выбрать другую папку, нажмите «Обзор».
DiskSpaceMBLabel=Требуется как минимум [mb] Мб свободного дискового пространства.
ToUNCPathname=Установка не может выполняться в папку по её сетевому имени. Если Вы устанавливаете в сетевую папку, Вы должны подключить её в виде сетевого диска.
InvalidPath=Вы должны указать полный путь с буквой диска; например:%n%nC:\APP%n%nили в форме UNC:%n%n\\имя_сервера\имя_ресурса
InvalidDrive=Выбранный Вами диск или сетевой путь не существуют или недоступны. Пожалуйста, выберите другой.
DiskSpaceWarningTitle=Недостаточно места на диске
DiskSpaceWarning=Установка требует не менее %1 Кб свободного места, а на выбранном Вами диске доступно только %2 Кб.%n%nВы желаете тем не менее продолжить установку?
DirNameTooLong=Имя папки или путь к ней превышают допустимую длину.
InvalidDirName=Указанное имя папки недопустимо.
BadDirName32=Имя папки не может содержать символов: %n%n%1
DirExistsTitle=Папка существует
DirExists=Папка%n%n%1%n%nуже существует. Всё равно установить в эту папку?
DirDoesntExistTitle=Папка не существует
DirDoesntExist=Папка%n%n%1%n%nне существует. Вы хотите создать её?

; *** "Select Components" wizard page
WizardSelectComponents=Выбор компонентов
SelectComponentsDesc=Какие компоненты должны быть установлены?
SelectComponentsLabel2=Выберите компоненты, которые Вы хотите установить; снимите флажки с компонентов, устанавливать которые не требуется. Нажмите «Далее», когда Вы будете готовы продолжить.
FullInstallation=Полная установка
; if possible don't translate 'Compact' as 'Minimal' (I mean 'Minimal' in your language)
CompactInstallation=Компактная установка
CustomInstallation=Выборочная установка
NoUninstallWarningTitle=Установленные компоненты
NoUninstallWarning=Программа установки обнаружила, что следующие компоненты уже установлены на Вашем компьютере:%n%n%1%n%nОтмена выбора этих компонент не удалит их.%n%nПродолжить?
ComponentSize1=%1 Кб
ComponentSize2=%1 Мб
ComponentsDiskSpaceMBLabel=Текущий выбор требует не менее [mb] Мб на диске.

; *** "Select Additional Tasks" wizard page
WizardSelectTasks=Выберите дополнительные задачи
SelectTasksDesc=Какие дополнительные задачи необходимо выполнить?
SelectTasksLabel2=Выберите дополнительные задачи, которые должны выполниться при установке [name], после этого нажмите «Далее»:

; *** "Select Start Menu Folder" wizard page
WizardSelectProgramGroup=Выберите папку в меню «Пуск»
SelectStartMenuFolderDesc=Где программа установки должна создать ярлыки?
SelectStartMenuFolderLabel3=Программа создаст ярлыки в следующей папке меню «Пуск».
SelectStartMenuFolderBrowseLabel=Нажмите «Далее», чтобы продолжить. Если Вы хотите выбрать другую папку, нажмите «Обзор».
NoIconsCheck=&Не создавать значков
MustEnterGroupName=Вы должны ввести имя папки.
GroupNameTooLong=Имя папки группы или путь к ней превышают допустимую длину.
InvalidGroupName=Указанное имя папки недопустимо.
BadGroupName=Имя папки не может содержать символов:%n%n%1
NoProgramGroupCheck2=&Не создавать папку в меню «Пуск»

; *** "Ready to Install" wizard page
WizardReady=Всё готово к установке
ReadyLabel1=Программа установки готова начать установку [name] на Ваш компьютер.
ReadyLabel2a=Нажмите «Установить», чтобы продолжить, или «Назад», если Вы хотите просмотреть или изменить опции установки.
ReadyLabel2b=Нажмите «Установить», чтобы продолжить.
ReadyMemoUserInfo=Информация о пользователе:
ReadyMemoDir=Папка установки:
ReadyMemoType=Тип установки:
ReadyMemoComponents=Выбранные компоненты:
ReadyMemoGroup=Папка в меню «Пуск»:
ReadyMemoTasks=Дополнительные задачи:

; *** "Preparing to Install" wizard page
WizardPreparing=Подготовка к установке
PreparingDesc=Программа установки подготавливается к установке [name] на Ваш компьютер.
PreviousInstallNotCompleted=Установка или удаление предыдущей программы не были завершены. Вам потребуется перезагрузить Ваш компьютер, чтобы завершить ту установку.%n%nПосле перезагрузки запустите вновь Программу установки, чтобы завершить установку [name].
CannotContinue=Невозможно продолжить установку. Нажмите «Отмена» для выхода из программы.

; *** "Installing" wizard page
WizardInstalling=Установка...
InstallingLabel=Пожалуйста, подождите, пока [name] установится на Ваш компьютер.

; *** "Setup Completed" wizard page
FinishedHeadingLabel=Завершение Мастера установки [name]
FinishedLabelNoIcons=Программа [name] установлена на Ваш компьютер.
FinishedLabel=Программа [name] установлена на Ваш компьютер. Приложение можно запустить с помощью соответствующего значка.
ClickFinish=Нажмите «Завершить», чтобы выйти из программы установки.
FinishedRestartLabel=Для завершения установки [name] требуется перезагрузить компьютер. Произвести перезагрузку сейчас?
FinishedRestartMessage=Для завершения установки [name] требуется перезагрузить компьютер.%n%nПроизвести перезагрузку сейчас?
ShowReadmeCheck=Я хочу просмотреть файл README
YesRadio=&Да, перезагрузить компьютер сейчас
NoRadio=&Нет, я произведу перезагрузку позже
; used for example as 'Run MyProg.exe'
RunEntryExec=Запустить %1
; used for example as 'View Readme.txt'
RunEntryShellExec=Просмотреть %1

; *** "Setup Needs the Next Disk" stuff
ChangeDiskTitle=Необходимо вставить следующий диск
SelectDiskLabel2=Пожалуйста, вставьте диск %1 и нажмите «OK».%n%nЕсли файлы этого диска могут быть найдены в папке, отличающейся от показанной ниже, введите правильный путь или нажмите «Обзор».
PathLabel=&Путь:
FileNotInDir2=Файл "%1" не найден в "%2". Пожалуйста, вставьте правильный диск или выберите другую папку.
SelectDirectoryLabel=Пожалуйста, укажите путь к следующему диску.

; *** Installation phase messages
SetupAborted=Установка не была завершена.%n%nПожалуйста, устраните проблему и запустите установку снова.
EntryAbortRetryIgnore=Нажмите «Повтор», чтобы повторить попытку, «Пропустить», чтобы пропустить файл, или «Отказ» для отмены установки.

; *** Installation status messages
StatusCreateDirs=Создание папок...
StatusExtractFiles=Распаковка файлов...
StatusCreateIcons=Создание ярлыков программы...
StatusCreateIniEntries=Создание INI-файлов...
StatusCreateRegistryEntries=Создание записей реестра...
StatusRegisterFiles=Регистрация файлов...
StatusSavingUninstall=Сохранение информации для деинсталляции...
StatusRunProgram=Завершение установки...
StatusRollback=Откат изменений...

; *** Misc. errors
ErrorInternal2=Внутренняя ошибка: %1
ErrorFunctionFailedNoCode=%1: сбой
ErrorFunctionFailed=%1: сбой; код %2
ErrorFunctionFailedWithMessage=%1: сбой; код %2.%n%3
ErrorExecutingProgram=Невозможно выполнить файл:%n%1

; *** Registry errors
ErrorRegOpenKey=Ошибка открытия ключа реестра:%n%1\%2
ErrorRegCreateKey=Ошибка создания ключа реестра:%n%1\%2
ErrorRegWriteKey=Ошибка записи в ключ реестра:%n%1\%2

; *** INI errors
ErrorIniEntry=Ошибка создания записи в INI-файле "%1".

; *** File copying errors
FileAbortRetryIgnore=Нажмите «Повтор», чтобы повторить, «Пропустить», чтобы пропустить файл (не рекомендуется) или «Отказ» для выхода.
FileAbortRetryIgnore2=Нажмите «Повтор», чтобы повторить, «Пропустить», чтобы игнорировать ошибку (не рекомендуется) или «Отказ» для выхода.
SourceIsCorrupted=Исходный файл поврежден
SourceDoesntExist=Исходный файл "%1" не существует
ExistingFileReadOnly=Существующий файл помечен как «файл только для чтения».%n%nНажмите «Повтор» для удаления атрибута «только для чтения», «Пропустить», чтобы пропустить файл или «Отказ» для выхода.
ErrorReadingExistingDest=Произошла ошибка при попытке чтения существующего файла:
FileExists=Файл уже существует.%n%nПерезаписать его?
ExistingFileNewer=Существующий файл более новый, чем устанавливаемый. Рекомендуется сохранить существующий файл.%n%nВы хотите сохранить существующий файл?
ErrorChangingAttr=Произошла ошибка при попытке изменения атрибутов существующего файла:
ErrorCreatingTemp=Произошла ошибка при попытке создания файла в папке назначения:
ErrorReadingSource=Произошла ошибка при попытке чтения исходного файла:
ErrorCopying=Произошла ошибка при попытке копирования файла:
ErrorReplacingExistingFile=Произошла ошибка при попытке замены существующего файла:
ErrorRestartReplace=Ошибка RestartReplace:
ErrorRenamingTemp=Произошла ошибка при попытке переименования файла в папке назначения:
ErrorRegisterServer=Невозможно зарегистрировать DLL/OCX: %1
ErrorRegisterServerMissingExport=Точка входа функции DllRegisterServer не найдена
ErrorRegisterTypeLib=Невозможно зарегистрировать библиотеку типов (Type Library): %1

; *** Post-installation errors
ErrorOpeningReadme=Произошла ошибка при попытке открытия файла README.
ErrorRestartingComputer=Программе установки не удалось перезапустить компьютер. Пожалуйста, выполните это самостоятельно.

; *** Uninstaller messages
UninstallNotFound=Файл "%1" не существует, деинсталляция невозможна.
UninstallOpenError=Невозможно открыть файл "%1". Деинсталляция невозможна
UninstallUnsupportedVer=Файл протокола для деинсталляции "%1" не распознан данной версией программы-деинсталлятора. Деинсталляция невозможна
UninstallUnknownEntry=Встретился неизвестный пункт (%1) в файле протокола для деинсталляции
ConfirmUninstall=Вы действительно хотите удалить %1 и все компоненты программы?
OnlyAdminCanUninstall=Эта программа может быть деинсталлирована только пользователем с административными привилегиями.
UninstallStatusLabel=Пожалуйста, подождите, пока %1 будет удалена с Вашего компьютера.
UninstalledAll=Программа %1 была полностью удалена с Вашего компьютера.
UninstalledMost=Деинсталляция %1 завершена.%n%nЧасть элементов не удалось удалить. Вы можете удалить их самостоятельно.
UninstalledAndNeedsRestart=Для завершения деинсталляции %1 необходимо произвести перезагрузку Вашего компьютера.%n%nВыполнить перезагрузку сейчас?
UninstallDataCorrupted=Файл "%1" поврежден. Деинсталляция невозможна

; *** Uninstallation phase messages
ConfirmDeleteSharedFileTitle=Удалить совместно используемый файл?
ConfirmDeleteSharedFile2=Система указывает, что следующий совместно используемый файл больше не используется никакими другими приложениями. Подтверждаете удаление файла?%n%nЕсли какие-либо программы всё еще используют этот файл, и он будет удалён, они не смогут работать правильно. Если Вы не уверены, выберите «Нет». Оставленный файл не навредит Вашей системе.
SharedFileNameLabel=Имя файла:
SharedFileLocationLabel=Расположение:
WizardUninstalling=Состояние деинсталляции
StatusUninstalling=Деинсталляция %1...

; The custom messages below aren't used by Setup itself, but if you make
; use of them in your scripts, you'll want to translate them.

[CustomMessages]
;Inno Setup Built-in Custom Messages
NameAndVersion=%1, версия %2
AdditionalIcons=Дополнительные значки:
CreateDesktopIcon=Создать значок на &Рабочем столе
CreateQuickLaunchIcon=Создать значок в &Панели быстрого запуска
ProgramOnTheWeb=Сайт %1 в Интернете
UninstallProgram=Деинсталлировать %1
LaunchProgram=Запустить %1
AssocFileExtension=Св&язать %1 с файлами, имеющими расширение %2
AssocingFileExtension=Связывание %1 с файлами %2...

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
ItalianLanguage=Italian menus and dialogs
JapaneseLanguage=Japanese menus and dialogs
KoreanLanguage=Korean menus and dialogs
NorwegianLanguage=Norwegian menus and dialogs
PolishLanguage=Polish menus and dialogs
PortugueseLanguage=Portuguese (Brazillian) menus and dialogs
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