; *** Inno Setup version 5.1.0+ Turkish messages ***
; Language     " Tükçe"               ::::::    Turkish
; Translate by " Çeviren "            ::::::    Adil YILDIZ
; E-Mail       " Elektronik Posta "   ::::::    adil@kde.org.tr
; Home Page    " Web Adresi "         ::::::    http://www.yildizyazilim.gen.tr
;
; $jrsoftware: issrc/Files/Default.isl,v 1.66 2005/02/25 20:23:48 mlaan Exp $
[LangOptions]
LanguageName=T<00FC>rk<00E7>e
LanguageID=$041f
LanguageCodePage=0
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
SetupAppTitle=Kur
SetupWindowTitle=%1 - Kur
UninstallAppTitle=Kaldýr
UninstallAppFullTitle=%1 Kaldýr

; *** Misc. common
InformationTitle=Bilgi
ConfirmTitle=Sorgu
ErrorTitle=Hata

; *** SetupLdr messages
SetupLdrStartupMessage=Bu %1'i kuracak . Devam etmek istiyor musunuz?
LdrCannotCreateTemp=Bellek dosyasý oluþturulamadý. Kurulum iptal edildi
LdrCannotExecTemp=Bellek dosyasý açýlamadý. Kurulum iptal edildi

; *** Startup error messages
LastErrorMessage=%1.%n%n Hata %2: %3
SetupFileMissing=%1 adlý dosya kurulum dizininde bulunamadý. Lütfen problemi düzeltin veya programýn yeni bir kopyasýný alýn.
SetupFileCorrupt=Kurulum dosyalarý bozulmuþ. Lütfen programýn yeni bir kopyasýný alýn.
SetupFileCorruptOrWrongVer=Kurulum dosyalarý bozulmuþ veya kurulumun bu sürümü ile uyuþmuyor olabilir. Lütfen problemi düzeltin veya Programýn yeni bir kopyasýný alýn.
NotOnThisPlatform=Bu program %1'de çalýþmaz.
OnlyOnThisPlatform=Bu program sadece %1'de çalýþtýrýlmalýdýr.
OnlyOnTheseArchitectures=Bu program sadace belirli mimarideki iþlemcilere için olan Windows'larda çalýþýr:%n%n%1
MissingWOW64APIs=Kullanmýþ olduðunuz Windows sürümü 64-bit kurulum için gerekli olan gereksinimlere sahip deðil. Bu problemi ortadan kaldýrmak için lütfen Service 1 yüleyin %1.
WinVersionTooLowError=Bu programý çalýþtýrmak için %1 sürüm %2 ve sonrasý gereklidir.
WinVersionTooHighError=Bu program %1 sürüm %2 ve sonrasýnda çalýþmaz.
AdminPrivilegesRequired=Bu programý kurmak için yönetici olarak oturum açmanýz gerekir.
PowerUserPrivilegesRequired=Bu programý kurabilmek için Administrator veya Yönetici Grubu üyesi olarak giriþ yapmanýz gerekli.
SetupAppRunningError=Kur %1 in çalýþtýðýný tesbit etti.%n%nLütfen bütün programlarý kapatýn sonra devam etmek için Tamam'a veya çýkmak için Ýptal'e basýn.
UninstallAppRunningError=Kaldýr %1'in çalýþtýðýný tespit etti.%n%nLütfen bütün programlarý kapatýn sonra devam etmek için Tamam'a veya çýkmak için Ýptal'e basýn.

; *** Misc. errors
ErrorCreatingDir=Kur " %1 " dizinini oluþturamadý.
ErrorTooManyFilesInDir=" %1 " dizininde dosya oluþturulamadý. Çünkü dizin çok fazla dosya içeriyor.

; *** Setup common messages
ExitSetupTitle=Kurdan çýk
ExitSetupMessage=Kur tamamlanmadý. Þimdi çýkarsanýz program kurulmayacak.%n%nBaþka bir zaman Kur'u tekrar çalýþtýrarak kuruluma devam edebilirsiniz.%n%nKurdan çýkmak istediðinizden eminmisiniz?
AboutSetupMenuItem=Kur H&akkýnda...
AboutSetupTitle=Kur Hakkýnda
AboutSetupMessage=%1 sürüm %2%n%3%n%n%1 internet:%n%4
AboutSetupNote=
TranslatorNote=Ýyi bir kurulum programý arýyorsanýz buldunuz...%nhttp://www.yildizyazilim.gen.tr

; *** Buttons
ButtonBack=< G&eri
ButtonNext=Ý&leri >
ButtonInstall=&Kur
ButtonOK=Tamam
ButtonCancel=Ýptal
ButtonYes=E&vet
ButtonYesToAll=Tümüne E&vet
ButtonNo=&Hayýr
ButtonNoToAll=Tümüne Ha&yýr
ButtonFinish=&Son
ButtonBrowse=&Gözat...
ButtonWizardBrowse=Göza&t...
ButtonNewFolder=Ye&ni Klasör Oluþtur

; *** "Select Language" dialog messages
SelectLanguageTitle=Kurulum Dilini Seçin
SelectLanguageLabel=Kurulum sýrasýnda kullanacaðýnýz dili seçin:

; *** Common wizard text
ClickNext=Devam etmek için Ýleri'ye , Çýkmak için Ýptal 'e basýn.
BeveledLabel=Inno Setup 5.1+ Türkçe
BrowseDialogTitle=Klasöre Gözat
BrowseDialogLabel=Aþaðýdaki listeden bir klasör seçip, Tamam tuþuna basýn.
NewFolderName=Yeni Klasör

; *** "Welcome" wizard page
WelcomeLabel1=[name] kur programýna hoþgeldiniz.
WelcomeLabel2=Kur þimdi [name/ver] 'ý bilgisayarýnýza kuracak.%n%nDevam etmeden önce çalýþan diðer bütün programlarý  kapatmanýz þiddetle tavsiye edilir. Bu, kurulum esnasýnda oluþabilecek hatalarý önlemeye yardýmcý olur.

; *** "Password" wizard page
WizardPassword=Þifre
PasswordLabel1=Bu kurulum þifre korumalýdýr.
PasswordLabel3=Lütfen þifreyi girin. Sonra Ýleri'ye basarak devam edin. Lütfen þifreyi girerken Büyük -Küçük harflere dikkat edin.
PasswordEditLabel=&Þifre:
IncorrectPassword=Girdiðiniz þifre hatalý . Lütfen tekrar deneyiniz.

; *** "License Agreement" wizard page
WizardLicense=Lisans Anlaþmasý
LicenseLabel=Lütfen devam etmeden önce aþaðýdaki önemli bilgileri okuyunuz.
LicenseLabel3=Lisans Anlaþmasýný lütfen okuyun. Kuruluma devam edebilmek için bu anlaþmanýn koþullarýný kabul etmelisiniz
LicenseAccepted=Anlaþmayý Kabul Ediyorum.
LicenseNotAccepted=Anlaþmayý Kabul Etmiyorum.

; *** "Information" wizard pages
WizardInfoBefore=Bilgi
InfoBeforeLabel=Lütfen devam etmeden önce aþaðýdaki önemli bilgileri okuyunuz.
InfoBeforeClickLabel=Kur ile devam etmeye hazýr olduðunuz zaman Ýleri'yi týklayýn.
WizardInfoAfter=Bilgi
InfoAfterLabel=Lütfen devam etmeden önce aþaðýdaki önemli bilgileri okuyunuz.
InfoAfterClickLabel=Kur ile devam etmeye hazýr olduðunuz zaman Ýleri'yi týklayýn.

; *** "User Information" wizard page
WizardUserInfo=Kullanýcý Bilgileri
UserInfoDesc=Lütfen bilgilerinizi giriniz.
UserInfoName=K&ullanýcý Adý:
UserInfoOrg=Þi&rket:
UserInfoSerial=Seri Numarasý:
UserInfoNameRequired=Bir isim girmelisiniz.

; *** "Select Destination Directory" wizard page
WizardSelectDir=Kurulacak dizini seçin.
SelectDirDesc=[name] hangi dizine kurulsun?
SelectDirLabel3=Kurulum [name]'i aþaðýdaki dizine kuracak.
SelectDirBrowseLabel=Devam etmek için, Ýleri'ye basýn. Baþka bir dizin seçmek istiyorsanýz, Gözat'a basýn.
DiskSpaceMBLabel=Bu program için en az [mb] MB disk alaný gereklidir.
ToUNCPathname=Kur UNC  yollarýna kurulum yapamaz. Eðer Að üzerinde kurulum yapmaya çalýþýyorsanýz. Bir að sürücüsü tanýtmanýz gerekir.
InvalidPath=Sürücü ismi ile birlikte tam yolu girmelisiniz; Örneðin :%nC:\APP
InvalidDrive=Seçtiðiniz sürücü bulunamadý. Lütfen baþka bir sürücü seçin.
DiskSpaceWarningTitle=Yetersiz disk alaný
DiskSpaceWarning=Kur için en az %1 KB disk alaný gerekmektedir. Fakat seçili diskte %2 KB boþ alan mevcuttur.%n%nDevam etmek istiyormusunuz?
DirNameTooLong=Dizin adý veya you çok uzun.
InvalidDirName=Dizin adý geçersiz.
BadDirName32=Dizin adý takib eden karakterlerden her hangi birini içeremez:%n%n%1
DirExistsTitle=Dizin Mevcuttur
DirExists=Dizin:%n%n%1%n%nmevcut. Bu dizine kurmak istediðinizden eminmisiniz?
DirDoesntExistTitle=Dizin Mevcut deðil
DirDoesntExist=Dizin:%n%n%1%n%nmevcut deðil. Bu dizini oluþturmak istediðinizden eminmisiniz?

; *** "Select Components" wizard page
WizardSelectComponents=Bileþen Seç
SelectComponentsDesc=Hangi bileþenler kurulsun?
SelectComponentsLabel2=Kurmak istediðiniz bileþenleri seçin; istemediklerinizi temizleyin.Devam etmeye hazýr olduðunuz zaman Ýleri'ye týklayýn.
FullInstallation=Tam Kurulum
; if possible don't translate 'Compact' as 'Minimal' (I mean 'Minimal' in your language)
CompactInstallation=Küçük Kurulum
CustomInstallation=Özel Kurulum
NoUninstallWarningTitle=Mevcut Bileþenler
NoUninstallWarning=Kur aþaðýdaki bileþenlerin kurulu olduðunu tespit etti:%n%n%1%n%nBu bileþenleri seçmeyerek kaldýramazsýnýz.%n%nYinede devam etmek istiyormusunuz?
ComponentSize1=%1 KB
ComponentSize2=%1 MB
ComponentsDiskSpaceMBLabel=Seçili bileþenler için en az [mb] MB disk alaný gerekmektedir.

; *** "Select Additional Tasks" wizard page
WizardSelectTasks=Ýlave Görev Seç
SelectTasksDesc=Hangi görevler yerine getirilsin?
SelectTasksLabel2=[name] kurulurken eklemek istediðiniz görevleri seçip Ýleri'ye týklayýn.

; *** "Baþlat Menüsü Klasörü Seç" sihirbaz sayfasý
WizardSelectProgramGroup=Baþlat Menüsünde klasör seç
SelectStartMenuFolderDesc=Kur program kýsayollarýný nereye yerleþtirsin?
SelectStartMenuFolderLabel3=Kurulum programýn kýsayollarýný aþaðýdaki Baþlat Menüsü dizinine kuracak.
SelectStartMenuFolderBrowseLabel=Devam etmek için, Ýleri'ye basýn. Baþka bir dizin seçmek istiyorsanýz, Gözat'a basýn.
MustEnterGroupName=Klasör ismi girmelisiniz.
GroupNameTooLong=Dizin adý veya you çok uzun.
InvalidGroupName=Dizin adý geçersiz.
BadGroupName=Klasör adý takib eden karakterlerden her hangi birini içeremez:%n%n%1
NoProgramGroupCheck2=Baþlat menüsünde kýsayol oluþturma

; *** "Ready to Install" wizard page
WizardReady=Kur kurmaya hazýr
ReadyLabel1=Kur [name]'i bilgisayarýnýza kurmak için hazýr.
ReadyLabel2a=Kuruluma devam etmek için Kur'u , kontrol etmek veya deðiþtirmek için Geri'yi týklayýn.
ReadyLabel2b=Kuruluma devam etmek için Kur'u týklayýn.
ReadyMemoUserInfo=Kullanýcý Bilgisi:
ReadyMemoDir=Hedef Dizin:
ReadyMemoType=Kurulum Tipi:
ReadyMemoComponents=Seçili Bileþenler:
ReadyMemoGroup=Baþlat Menüsü :
ReadyMemoTasks=Ýlave Görevler:

; *** "Kur Hazýlanýyor" sihirbaz sayfasý
WizardPreparing=Kur Hazýrlanýyor
PreparingDesc=Kur [name] Bilgisayarýnýza kurmak için hazýrlanýyor.
PreviousInstallNotCompleted=Bir önceki kurma/kaldýrma programýna ait iþlem %ntamamlanmamýþ.Önceki kurulum iþleminin tamamlanmasý için bilgisayarýnýzý %nyeniden baþlatmalýsýnýz.%n%nBilgisayarýnýz katýlýp-açýldýktan sonra, %n[name] adlý uygulamayý kurma iþlemine devam edecek.
CannotContinue=Kur devam edemiyor. Lütfen Ýptal'e týklayýp Çýkýn.

; *** "Kuruluyor" sihirbaz
WizardInstalling=Kuruluyor
InstallingLabel=Kur [name]'i bilgisayarýnýza kurarken bekleyin.

; *** "Setup Completed" wizard page
FinishedHeadingLabel=[name] kurulum sihirbazý tamamlanýyor
FinishedLabelNoIcons=Kur [name]'i bilgisayarýnýza kurma iþlemini tamamladý.
FinishedLabel=Kur [name]'i bilgisayarýnýza kurma iþlemini tamamladý. Programý baþlatmak için kurulu kýsayollarý týklayýn.
ClickFinish=Kur'dan çýkmak için sona týklayýn.
FinishedRestartLabel=[name]'in kurulumunu bitirmek için, Kur bilgisayarýnýzý yeniden baþlatacak. Biligisayarýnýz yeniden baþlatýlsýn mý?
FinishedRestartMessage=[name]'in kurulumunu bitirmek için, Kur bilgisayarýnýzý yeniden baþlatacak. %n%nBiligisayarýnýz yeniden baþlatýlsýn mý?
ShowReadmeCheck=Beni Oku dosyasýný görmek istiyorum.
YesRadio=&Evet , Bilgisayar yeniden baþlatýllsýn.
NoRadio=&Hayýr, Ben sonra yeniden baþlatýrým.
; used for example as 'Run MyProg.exe'
RunEntryExec=%1'ý Çalýþtýr
; used for example as 'View Readme.txt'
RunEntryShellExec=%1'ý gör

; *** "Setup Needs the Next Disk" stuff
ChangeDiskTitle=Bir sonraki disketi takýn.
SelectDiskLabel2=%1 numaralý diski takýp, Tamam'ý týklayýn.%n%nEðer dosyalar Sabit Disk üzerinde ise doðru yolu yazýn veya Gözat'ý týklayýn.
PathLabel=&Yol:
FileNotInDir2=" %1 " adlý dosya " %2 " dizininde bulunamadý. Lütfen doðru diski veya dosyayý seçin.
SelectDirectoryLabel=Lütfen sonraki diskin yerini belirleyin.

; *** Installation phase messages
SetupAborted=Kurulum tamamlanamadý.%n%nLütfen ya problemi düzeltin yada Kur'u tekrar çalýþtýrýn.
EntryAbortRetryIgnore=Tekrar denemek için Tekrar Dene'ye , yinede devam etmek için Yoksay'a , kurulumu iptal etmek için Ýptal'e týklayýn.

; *** Installation status messages
StatusCreateDirs=Dizinler oluþturuluyor...
StatusExtractFiles=Paketler açýlýyor...
StatusCreateIcons=Program kýsayollarý oluþturuluyor...
StatusCreateIniEntries=INI dosyalarý oluþturuluyor...
StatusCreateRegistryEntries=Registry güncelleniyor...
StatusRegisterFiles=Dosyalar sisteme tanýtýlýyor.
StatusSavingUninstall=Kaldýr bilgileri kaydediliyor.
StatusRunProgram=Kurulum sonlandýrýlýyor...
StatusRollback=Kayýt yeniden düzenleniyor...

; *** Misc. errors
ErrorInternal2=Ýç hata: %1
ErrorFunctionFailedNoCode=%1 baþarýsýz.
ErrorFunctionFailed=%1 baþarýsýz oldu; satýr  %2
ErrorFunctionFailedWithMessage=%1 baþarýsýz oldu ; satýr  %2.%n%3
ErrorExecutingProgram=%1 adlý dosya çalýþtýrýlamadý.

; *** Registry errors
ErrorRegOpenKey=Kayýt Defteri anahtarýný açarken hata oluþtu:%n%1\%2
ErrorRegCreateKey=Kayýt Defteri anahtarýný oluþtururken hata oluþtu:%n%1\%2
ErrorRegWriteKey=Kayýt Defteri anahtarýna yazarken hata oluþtu:%n%1\%2

; *** INI errors
ErrorIniEntry=" %1 " adlý dosyada INI yazma hatasý.

; *** File copying errors
FileAbortRetryIgnore=Yeniden denemek için Yeniden Dene'ye, dosyayý atlamak için Yoksay'a (önerilmez), Kurulumu iptal etmek için Ýptal'e týklayýn.
FileAbortRetryIgnore2=Yeniden denemek için Yeniden Dene'ye , yine de devam etmek için Yoksay'a (önerilmez), Kurulumu Ýptal etmek için Ýptal'e týklayýn.
SourceIsCorrupted=Kaynak Dosya Bozulmuþ
SourceDoesntExist=%1 adlý Kaynak Dosya Mevcut Deðil.
ExistingFileReadOnly=Dosya Salt Okunur.%n%nSalt Okunur özelliðini kaldýrýp yeniden denemek için Yeniden Dene'yi , dosyasý atlamak için Yoksay'ý , Kurulumu iptal etmek için Ýptal'i týklayýn.
ErrorReadingExistingDest=Dosyayý okurken bir hata oluþtu :
FileExists=Dosya mevcut.%n%nKur üzerine yazsýn mý?
ExistingFileNewer=Mevcut dosya Kur'un yüklemek istediði dosyadan daha yeni. Mevcut dosyayý saklamanýz önerilir.%n%nMevcut dosya saklansýn mý?
ErrorChangingAttr=Dosyanýn özelliði deðiþtirilirken hata oluþtu:
ErrorCreatingTemp=Hedef dizinde dosya oluþturulurken hata oluþtu:
ErrorReadingSource=Kaynak okunurken hata oluþtu:
ErrorCopying=Dosya kopyalanýrken hata oluþtu:
ErrorReplacingExistingFile=Mevcut dosya deðiþtirilirken hata oluþtu:
ErrorRestartReplace=Tekrar deðiþtirme iþlemi baþarýsýz oldu:
ErrorRenamingTemp=Hedef dizinde oluþturulacak dosyanýn adý deðiþtirilirken hata oldu:
ErrorRegisterServer=%1 adlý DLL/OCX sisteme tanýtýlamadý.
ErrorRegisterServerMissingExport=DLL'i Sisteme tanýtma aracý bulunamadý
ErrorRegisterTypeLib=%1 adlý kütüphane sisteme tanýtýlamadý

; *** Post-installation errors
ErrorOpeningReadme=Beni Oku dosyasý açýlýrken hata oluþtu.
ErrorRestartingComputer=Kur bilgisayarý yeniden baþlatamadý. Lütfen kendiniz kapatýn.

; *** Uninstaller messages
UninstallNotFound=%1 adlý dosya bulunamadý. Kaldýrýlamaz.
UninstallOpenError="%1" dosyasý açýlamýyor. Kaldýrma iþlemi gerçekleþtirilemedi.
UninstallUnsupportedVer=%1 adlý Kaldýr bilgi dosyasý kaldýrýn bu sürümü ile uyuþmuyor. Kaldýrýlamaz
UninstallUnknownEntry=Kaldýr Bilgi dosyasýndaki %1 adlý satýr anlaþýlamadý
ConfirmUninstall=%1 ve bileþenlerini kaldýrmak istediðinizden emin misiniz?
UninstallOnlyOnWin64=Bu kurulum sadece 64-bit Windows'larda kaldýrýlabilir.
OnlyAdminCanUninstall=Kurlumu sadece yönetici yetkisine sahip kiþiler kaldýrabilir.
UninstallStatusLabel=%1 adlý program bilgisayarýnýzdan kaldýrýlýrken bekleyin...
UninstalledAll=%1 bilgisayarýnýzdan tamamen kaldýrýldý.
UninstalledMost=%1'ý kaldýrma iþlemi sona erdi.%n%nBazý bileþenler kaldýrýlamamýþ olabilir. Bu dosyalarý kendiniz silin.
UninstalledAndNeedsRestart=%1 kaldýrýlýlmasý tamamlandý, Bilgisayarýnýzý yeniden baþlatmalýsýnýz.%n%nYeniden Baþlatýlmasýný istiyor musunuz?
UninstallDataCorrupted="%1" adlý dosya bozuk. Kaldýrýlamýyor.

; *** Uninstallation phase messages
ConfirmDeleteSharedFileTitle=Paylaþýlým dosyasýný sil?
ConfirmDeleteSharedFile2=Sistemde belirtilen paylaþýlmýþ dosya bazý programlar tarfýndan kullanýlýyor. Kaldýr bu paylaþýlan dosyayý silsin mi?%n%n Bu dosya bazý programlar tafarýndan kullanýlýyorsa ve silinirse bu programalar düzgün bir þekilde çalýþmayabilir. Emin deðilseniz. Hayýr Butonuna týklayýnýz.
SharedFileNameLabel=Dosya Adý:
SharedFileLocationLabel=Dizin:
WizardUninstalling=Kaldýrma Durumu
StatusUninstalling=%1 Kaldýrýlýyor...

; The custom messages below aren't used by Setup itself, but if you make
; use of them in your scripts, you'll want to translate them.

[CustomMessages]
;Inno Setup Built-in Custom Messages
NameAndVersion=%1 sürüm %2
AdditionalIcons=Ek simgeler:
OptionalFeatures=Optional Features:
CreateDesktopIcon=Masaüstü simg&esi oluþtur
CreateQuickLaunchIcon=Hýzlý Baþlat simgesi &oluþtur
ProgramOnTheWeb=%1 Web Sitesi
UninstallProgram=%1 Programýný Kaldýr
LaunchProgram=%1 Programýný Çalýþtýr
AssocFileExtension=%2 dosya uzantýlarýný %1'e ata
AssocingFileExtension=%2 dosya uzantýlarý %1'e atanýyor...

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
IntegrateDelphi4=Borland® Delphi &4 Binary File Support
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
