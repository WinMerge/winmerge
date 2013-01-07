; *** Inno Setup version 5.1.11+ Turkish messages ***
; Language     " Tükçe"               ::::::    Turkish
; Translate by " Çeviren "            ::::::    Adil YILDIZ - Mehmet F. YUCE
; E-Mail       " Elektronik Posta "   ::::::    adil@kde.org.tr - mfyuce@yelkenbilgisayar.com
; Home Page    " Web Adresi "         ::::::    http://www.yildizyazilim.gen.tr - http://www.yelkenbilgisayar.com
;
; $jrsoftware: issrc/Files/Default.isl,v 1.66 2005/02/25 20:23:48 mlaan Exp $
[LangOptions]
LanguageName=T<00FC>rk<00E7>e
LanguageID=$041f
LanguageCodePage=1254
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
DialogFontName=
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
SetupLdrStartupMessage=Bu kurulumda %1 programý yüklenecektir. Devam etmek istiyor musunuz?
LdrCannotCreateTemp=Geçici bir dosya oluþturulamadý. Kurulum iptal edildi
LdrCannotExecTemp=Geçici klasördeki dosya çalýþtýrýlamadý. Kurulum iptal edildi

; *** Startup error messages
LastErrorMessage=%1.%n%nHata %2: %3
SetupFileMissing=%1 adlý dosya kurulum klasöründe bulunamadý. Lütfen sorunu düzeltin veya programýn yeni bir kopyasýný edinin.
SetupFileCorrupt=Kurulum dosyalarý bozulmuþ. Lütfen programýn yeni bir kopyasýný edinin.
SetupFileCorruptOrWrongVer=Kurulum dosyalarý bozulmuþ veya kurulumun bu sürümü ile uyuþmuyor olabilir. Lütfen sorunu düzeltin veya programýn yeni bir kopyasýný edinin.
NotOnThisPlatform=Bu program %1 üzerinde çalýþtýrýlamaz.
OnlyOnThisPlatform=Bu program sadece %1 üzerinde çalýþtýrýlmalýdýr.
OnlyOnTheseArchitectures=Bu program sadece aþaðýdaki mimarilere sahip Windows sürümlerinde çalýþýr:%n%n%1
MissingWOW64APIs=Kullandýðýnýz Windows sürümü Kur'un 64-bit yükleme yapabilmesi için gerekli olan özelliklere sahip deðil. Bu sorunu ortadan kaldýrmak için lütfen Service Pack %1 yükleyin.
WinVersionTooLowError=Bu programý çalýþtýrabilmek için %1 %2 sürümü veya daha sonrasý yüklü olmalýdýr.
WinVersionTooHighError=Bu program %1 %2 sürümü veya sonrasýnda çalýþmaz.
AdminPrivilegesRequired=Bu program kurulurken yönetici olarak oturum açmýþ olmanýz gereklidir.
PowerUserPrivilegesRequired=Bu program kurulurken Yönetici veya Güç Yöneticisi Grubu üyesi olarak giriþ yapmýþ olmanýz gereklidir.
SetupAppRunningError=Kur %1 programýnýn çalýþtýðýný algýladý.%n%nLütfen bu programýn çalýþan bütün parçalarýný þimdi kapatarak, devam etmek için Tamam'a veya çýkmak için Ýptal'e týklayýn.
UninstallAppRunningError=Kaldýr %1 programýnýn çalýþtýðýný tespit etti.%n%nLütfen bu programýn çalýþan bütün parçalarýný þimdi kapatarak, devam etmek için Tamam'a veya çýkmak için Ýptal'e týklayýn.

; *** Misc. errors
ErrorCreatingDir=Kur " %1 " klasörü oluþturamadý.
ErrorTooManyFilesInDir=" %1 " klasörü çok fazla dosya içerdiðinden burada bir dosya oluþturulamadý.

; *** Setup common messages
ExitSetupTitle=Kur'dan Çýk
ExitSetupMessage=Kurulum tamamlanmadý. Þimdi çýkarsanýz program kurulmuþ olmayacak.%n%nDaha sonra Kur'u tekrar çalýþtýrarak kurulumu tamamlayabilirsiniz.%n%nKur'dan çýkmak istediðinizden emin misiniz?
AboutSetupMenuItem=Kur H&akkýnda...
AboutSetupTitle=Kur Hakkýnda
AboutSetupMessage=%1 %2 sürümü%n%3%n%n%1 internet:%n%4
AboutSetupNote=
TranslatorNote=Çeviri: Kaya Zeren%n

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
SelectLanguageTitle=Kur Dilini Seçin
SelectLanguageLabel=Lütfen kurulum sýrasýnda kullanacaðýnýz dili seçin:

; *** Common wizard text
ClickNext=Devam etmek için Ýleri'ye , çýkmak için Ýptal 'e týklayýn.
BeveledLabel=Inno Setup 5.1+ Türkçe
BrowseDialogTitle=Klasöre Gözat
BrowseDialogLabel=Aþaðýdaki listeden bir dizin seçip, daha sonra Tamam'a týklayýn.
NewFolderName=Yeni Klasör

; *** "Welcome" wizard page
WelcomeLabel1=[name] Kurulum Sihirbazýna Hoþgeldiniz.
WelcomeLabel2=Kur þimdi [name/ver] programýný bilgisayarýnýza yükleyecek.%n%nDevam etmeden önce çalýþan diðer bütün programlarý kapatmanýz önerilir.

; *** "Password" wizard page
WizardPassword=Parola
PasswordLabel1=Bu kurulum parola korumalýdýr.
PasswordLabel3=Lütfen parolayý yazýn. Daha sonra devam etmek için Ýleri'ye týklayýn. Lütfen parolayý yazarken Büyük-Küçük harflere dikkat edin.
PasswordEditLabel=&Parola:
IncorrectPassword=Yazdýðýnýz parola hatalý. Lütfen tekrar deneyin.

; *** "License Agreement" wizard page
WizardLicense=Lisans Anlaþmasý
LicenseLabel=Devam etmeden önce aþaðýdaki önemli bilgileri okuyun.
LicenseLabel3=Lütfen aþaðýdaki lisans anlaþmasýný okuyun. Kuruluma devam edebilmek için bu anlaþmanýn koþullarýný kabul etmelisiniz.
LicenseAccepted=Anlaþmayý Kabul &Ediyorum.
LicenseNotAccepted=Anlaþmayý Kabul Et&miyorum.

; *** "Information" wizard pages
WizardInfoBefore=Bilgi
InfoBeforeLabel=Devam etmeden önce aþaðýdaki önemli bilgileri okuyun.
InfoBeforeClickLabel=Kur ile devam etmeye hazýr olduðunuzda Ýleri'ye týklayýn.
WizardInfoAfter=Bilgi
InfoAfterLabel=Lütfen devam etmeden önce aþaðýdaki önemli bilgileri okuyun.
InfoAfterClickLabel=Kurma iþlemine devam etmeye hazýr olduðunuzda Ýleri'ye týklayýn.

; *** "User Information" wizard page
WizardUserInfo=Kullanýcý Bilgileri
UserInfoDesc=Lütfen bilgilerinizi yazýn.
UserInfoName=K&ullanýcý Adý:
UserInfoOrg=Ku&rum:
UserInfoSerial=Seri Numarasý:
UserInfoNameRequired=Bir ad yazmalýsýnýz.

; *** "Select Destination Directory" wizard page
WizardSelectDir=Kurulacak klasörü seçiniz
SelectDirDesc=[name] hangi klasöre kurulsun?
SelectDirLabel3=Kur [name] programýný aþaðýdaki klasöre kuracaktýr.
SelectDirBrowseLabel=Devam etmek için Ýleri'ye týklayýn. Baþka bir klasör seçmek istiyorsanýz, Gözat'a týklayýn.
DiskSpaceMBLabel=Bu program en az [mb] MB disk alaný gerektirmektedir.
ToUNCPathname=Kur, UNC tipindeki klasör yollarýna (Örn:\\yol vb.) kurulum yapamaz. Eðer að üzerine kurulum yapmaya çalýþýyorsanýz. Bir að sürücüsü tanýtmanýz gerekir.
InvalidPath=Sürücü adý ile birlikte tam yolu yazmalýsýnýz; Örneðin %nC:\APP%n%n veya bir UNC yolunu %n%n\\sunucu\paylaþým%n%n þeklinde yazmalýsýnýz.
InvalidDrive=Seçtiðiniz sürücü bulunamadý veya ulaþýlamýyor. Lütfen baþka bir sürücü seçin.
DiskSpaceWarningTitle=Yetersiz Disk Alaný
DiskSpaceWarning=Kur için en az %1 KB kullanýlabilir disk alaný gereklidir. Ancak seçili diskte %2 KB boþ alan var.%n%nYine de devam etmek istiyor musunuz?
DirNameTooLong=Klasör adý veya yolu çok uzun.
InvalidDirName=Klasör adý geçersiz.
BadDirName32=Klasör adý þu karakterlerden her hangi birini içeremez:%n%n%1
DirExistsTitle=Klasör Bulundu
DirExists=%n%n%1%n%n klasörü zaten var. Yine de bu klasöre kurmak istediðinizden emin misiniz?
DirDoesntExistTitle=Klasör Bulunamadý
DirDoesntExist=%n%n%1%n%n klasörü bulunamadý. Bu klasörü oluþturmak ister misiniz?

; *** "Select Components" wizard page
WizardSelectComponents=Bileþenleri Seçin
SelectComponentsDesc=Hangi bileþenler kurulsun?
SelectComponentsLabel2=Kurmak istediðiniz bileþenleri seçin; istemediklerinizi býrakýn.Devam etmeye hazýr olduðunuz zaman Ýleri'ye týklayýn.
FullInstallation=Tam Kurulum
; if possible don't translate 'Compact' as 'Minimal' (I mean 'Minimal' in your language)
CompactInstallation=Normal Kurulum
CustomInstallation=Özel Kurulum
NoUninstallWarningTitle=Kurulu Bileþenler
NoUninstallWarning=Kur aþaðýdaki bileþenlerin kurulu olduðunu algýladý:%n%n%1%n%nBu bileþenleri seçmeyerek kaldýrmýþ olmayacaksýnýz.%n%nYine de devam etmek istiyor musunuz?
ComponentSize1=%1 KB
ComponentSize2=%1 MB
ComponentsDiskSpaceMBLabel=Seçili bileþenler için en az [mb] MB disk alaný gereklidir.

; *** "Select Additional Tasks" wizard page
WizardSelectTasks=Ek Görevleri Seçin
SelectTasksDesc=Hangi görevler yerine getirilsin?
SelectTasksLabel2=[name] kurulurken istediðiniz ek görevleri seçip Ýleri'ye týklayýn.

; *** "Baþlat Menüsü Dizini Seç" sihirbaz sayfasý
WizardSelectProgramGroup=Baþlat Menüsü Klasörünü Seçin
SelectStartMenuFolderDesc=Kur program kýsayollarýný nereye yerleþtirsin?
SelectStartMenuFolderLabel3=Kur programýn kýsayollarýný aþaðýdaki Baþlat Menüsü klasörüne kuracak.
SelectStartMenuFolderBrowseLabel=Devam etmek için, Ýleri'ye týklayýn. Baþka bir klasör seçmek istiyorsanýz, Gözat'a týklayýn.
MustEnterGroupName=Bir klasör adý yazmalýsýnýz.
GroupNameTooLong=Klasör adý veya yolu çok uzun.
InvalidGroupName=Klasör adý geçersiz.
BadGroupName=Klasör adý, þu karakterlerden her hangi birini içeremez:%n%n%1
NoProgramGroupCheck2=&Baþlat menüsünde kýsayol oluþturma

; *** "Ready to Install" wizard page
WizardReady=Yükleme için hazýr
ReadyLabel1=Kur [name] programýný bilgisayarýnýza kurmak için hazýr.
ReadyLabel2a=Kuruluma devam etmek için Kur'a, ayarlarýnýzý kontrol etmek veya deðiþtirmek için Geri'ye týklayýn.
ReadyLabel2b=Kuruluma devam etmek için Kur'a týklayýn.
ReadyMemoUserInfo=Kullanýcý Bilgisi:
ReadyMemoDir=Hedef Klasör:
ReadyMemoType=Kurulum Tipi:
ReadyMemoComponents=Seçili Bileþenler:
ReadyMemoGroup=Baþlat Menüsü :
ReadyMemoTasks=Ek Görevler:

; *** "Kur Hazýrlanýyor" sihirbaz sayfasý
WizardPreparing=Kurulum Hazýrlanýyor
PreparingDesc=Kur [name] programýný bilgisayarýnýza kurmak için hazýrlanýyor.
PreviousInstallNotCompleted=Bir önceki Kur/Kaldýr programýna ait iþlem tamamlanmamýþ.Önceki kurulum iþleminin tamamlanmasý için bilgisayarýnýzý yeniden baþlatmalýsýnýz.%n%nBilgisayarýnýz tekrar baþladýktan sonra, Kur'u tekrar çalýþtýrarak [name] programýný kurma iþlemine devam edebilirsiniz.
CannotContinue=Kur devam edemiyor. Lütfen Ýptal'e týklayýp Çýkýn.

; *** "Kuruluyor" sihirbaz
WizardInstalling=Kuruluyor
InstallingLabel=Lütfen [name] bilgisayarýnýza kurulurken bekleyin.

; *** "Setup Completed" wizard page
FinishedHeadingLabel=[name] Kur Sihirbazý tamamlanýyor
FinishedLabelNoIcons=Kur [name] programýný bilgisayarýnýza kurma iþlemini tamamladý.
FinishedLabel=Kur [name] programýný bilgisayarýnýza kurma iþlemini tamamladý. Program yüklenen kýsayol simgelerine týklanarak çalýþtýrýlabilir.
ClickFinish=Kur'dan çýkmak için Son'a týklayýn.
FinishedRestartLabel=[name] programýnýn kurulumunu bitirmek için, Kur bilgisayarýnýzý yeniden baþlatacak. Bilgisayarýnýz þimdi yeniden baþlatýlsýn mý?
FinishedRestartMessage=[name] kurulumunu bitirmek için, bilgisayarýnýzýn yeniden baþlatýlmasý gerekiyor. %n%nBilgisayarýnýz yeniden baþlatýlsýn mý?
ShowReadmeCheck=Beni Oku dosyasýný okumak istiyorum.
YesRadio=&Evet, bilgisayar yeniden baþlatýlsýn.
NoRadio=&Hayýr, daha sonra yeniden baþlatacaðým.
; used for example as 'Run MyProg.exe'
RunEntryExec=%1 uygulamasýný çalýþtýr
; used for example as 'View Readme.txt'
RunEntryShellExec=%1 dosyasýný görüntüle

; *** "Setup Needs the Next Disk" stuff
ChangeDiskTitle=Bir sonraki disketi takýn.
SelectDiskLabel2=%1 numaralý disketi takýp, Tamam'ý týklayýn.%n%nEðer dosyalar baþka bir yerde bulunuyor ise doðru yolu yazýn veya Gözat'ý týklayýn.
PathLabel=&Yol:
FileNotInDir2=" %1 " adlý dosya " %2 " dizininde bulunamadý. Lütfen doðru diski veya dosyayý seçin.
SelectDirectoryLabel=Lütfen sonraki disketin yerini belirtin.

; *** Installation phase messages
SetupAborted=Kurulum tamamlanamadý.%n%nLütfen sorunu düzeltin veya Kur'u tekrar çalýþtýrýn.
EntryAbortRetryIgnore=Tekrar denemek için "Tekrar Dene" ye, devam etmek için Yoksay'a, kurulumu iptal etmek için Ýptal'e týklayýn.

; *** Installation status messages
StatusCreateDirs=Klasörler oluþturuluyor...
StatusExtractFiles=Dosyalar çýkartýlýyor...
StatusCreateIcons=Program kýsayollarý oluþturuluyor...
StatusCreateIniEntries=INI kayýtlarý oluþturuluyor...
StatusCreateRegistryEntries=Kayýt Defteri kayýtlarý oluþturuluyor...
StatusRegisterFiles=Dosyalar sisteme kaydediliyor...
StatusSavingUninstall=Kaldýrma bilgileri kaydediliyor...
StatusRunProgram=Kurulum sonlandýrýlýyor...
StatusRollback=Deðiþiklikler geri alýnýyor...

; *** Misc. errors
ErrorInternal2=Ýç hata: %1
ErrorFunctionFailedNoCode=%1 baþarýsýz oldu.
ErrorFunctionFailed=%1 baþarýsýz oldu; kod  %2
ErrorFunctionFailedWithMessage=%1 baþarýsýz oldu ; kod  %2.%n%3
ErrorExecutingProgram=%1 adlý dosya çalýþtýrýlamadý.

; *** Registry errors
ErrorRegOpenKey=Aþaðýdaki Kayýt Defteri anahtarý açýlýrken hata oluþtu:%n%1\%2
ErrorRegCreateKey=Aþaðýdaki Kayýt Defteri anahtarý oluþturulurken hata oluþtu:%n%1\%2
ErrorRegWriteKey=Aþaðýdaki Kayýt Defteri anahtarýna yazýlýrken hata oluþtu:%n%1\%2

; *** INI errors
ErrorIniEntry=" %1 " adlý dosyada INI kaydý yazma hatasý.

; *** File copying errors
FileAbortRetryIgnore=Yeniden denemek için "Yeniden Dene" ye, dosyayý atlamak için Yoksay'a (önerilmez), Kurulumu iptal etmek için Ýptal'e týklayýn.
FileAbortRetryIgnore2=Yeniden denemek için "Yeniden Dene" ye, devam etmek için Yoksay'a (önerilmez), Kurulumu Ýptal etmek için Ýptal'e týklayýn.
SourceIsCorrupted=Kaynak Dosya Bozulmuþ
SourceDoesntExist=%1 adlý kaynak dosya bulunamadý.
ExistingFileReadOnly=Dosya salt okunur.%n%nSalt okunur özelliðini kaldýrýp yeniden denemek için Yeniden Dene'yi, dosyayý atlamak için Yoksay'ý, kurulumu iptal etmek için Ýptal'i týklayýn.
ErrorReadingExistingDest=Dosyayý okurken bir hata oluþtu :
FileExists=Dosya zaten var.%n%nKurulum'un üzerine yazmasýný ister misiniz?
ExistingFileNewer=Kurulum'un yüklemek istediði dosyadan daha yeni bir dosya var. Bu dosyayý korumanýz önerilir.%n%nVar olan dosya korunsun mu?
ErrorChangingAttr=Var olan dosyanýn özelliði deðiþtirilirken bir hata oluþtu:
ErrorCreatingTemp=Hedef klasörde dosya oluþturulurken bir hata oluþtu:
ErrorReadingSource=Kaynak dosya okunurken bir hata oluþtu:
ErrorCopying=Bir dosya kopyalanýrken bir hata oluþtu:
ErrorReplacingExistingFile=Var olan dosya deðiþtirilirken bir hata oluþtu:
ErrorRestartReplace=Yeniden baþlat deðiþtir baþarýsýz oldu:
ErrorRenamingTemp=Hedef klasörde bulunan dosyanýn adý deðiþtirilirken hata oldu:
ErrorRegisterServer=%1 adlý DLL/OCX sisteme tanýtýlamadý.
ErrorRegSvr32Failed=RegSvr32 çýkýþ hatasý %1 ile baþarýsýz oldu
ErrorRegisterTypeLib=%1 adlý tip kütüphanesi sisteme tanýtýlamadý

; *** Post-installation errors
ErrorOpeningReadme=Beni Oku dosyasý açýlýrken hata oluþtu.
ErrorRestartingComputer=Kurulum bilgisayarý yeniden baþlatamadý. Lütfen kendiniz yeniden baþlatýn.

; *** Uninstaller messages
UninstallNotFound=%1 adlý dosya bulunamadý. Kaldýrma programý çalýþtýrýlamadý.
UninstallOpenError="%1" dosyasý açýlamýyor. Kaldýrma programý çalýþtýrýlamadý.
UninstallUnsupportedVer=%1 adlý Kaldýr bilgi dosyasý kaldýrma programýnýn bu sürümü ile uyuþmuyor. Kaldýrma programý çalýþtýrýlamadý.
UninstallUnknownEntry=Kaldýr Bilgi dosyasýndaki %1 adlý satýr anlaþýlamadý
ConfirmUninstall=%1 ve bileþenlerini kaldýrmak istediðinizden emin misiniz?
UninstallOnlyOnWin64=Bu kurulum sadece 64-bit Windows üzerinden kaldýrýlabilir.
OnlyAdminCanUninstall=Bu kurulum sadece yönetici yetkisine sahip kullanýcýlar tarafýndan kaldýrabilir.
UninstallStatusLabel=Lütfen %1 programý bilgisayarýnýzdan kaldýrýlýrken bekleyin...
UninstalledAll=%1 programý bilgisayarýnýzdan tamamen kaldýrýldý.
UninstalledMost=%1 programýnýn kaldýrýlma iþlemi sona erdi.%n%nBazý bileþenler kaldýrýlamadý. Bu dosyalarý kendiniz silebilirsiniz.
UninstalledAndNeedsRestart=%1 programýnýn kaldýrýlmasý tamamlandý, Bilgisayarýnýzý yeniden baþlatmalýsýnýz.%n%nÞimdi yeniden baþlatýlsýn mý?
UninstallDataCorrupted="%1" adlý dosya bozuk. . Kaldýrma programý çalýþtýrýlamadý.

; *** Uninstallation phase messages
ConfirmDeleteSharedFileTitle=Paylaþýlan dosya kaldýrýlsýn mý?
ConfirmDeleteSharedFile2=Sistemde paylaþýlan bazý dosyalarýn artýk hiçbir program tarafýndan kullanýlmadýðýný belirtiyor. Kaldýr bu paylaþýlan dosyalarý silsin mi?%n%n Bu dosya bazý programlar tafarýndan kullanýlýyorsa ve silinmesini isterseniz, söz konusu programalar düzgün çalýþmayabilir. Emin deðilseniz, Hayýr'a týklayýn. Bu dosyalarýn sisteminizde durmasý hiçbir zarar vermez.
SharedFileNameLabel=Dosya Adý:
SharedFileLocationLabel=Yol:
WizardUninstalling=Kaldýrma Durumu
StatusUninstalling=%1 Kaldýrýlýyor...

; The custom messages below aren't used by Setup itself, but if you make
; use of them in your scripts, you'll want to translate them.

[CustomMessages]
;Inno Setup Built-in Custom Messages
NameAndVersion=%1 %2 sürümü
AdditionalIcons=Ek simgeler:
OptionalFeatures=Ýsteðe Baðlý Özellikler:
CreateDesktopIcon=Masaüstü simg&esi oluþtur
CreateQuickLaunchIcon=Hýzlý Baþlat simgesi &oluþtur
ProgramOnTheWeb=%1 Web Sitesi
UninstallProgram=%1 programýný kaldýr
LaunchProgram=%1 programýný çalýþtýr
AssocFileExtension=%2 dosya uzantýlarýný %1 ile iliþkilendir
AssocingFileExtension=%2 dosya uzantýlarý %1 ile iliþkilendiriliyor...

;Things we can also localize
CompanyName=Thingamahoochie Yazýlým

;Types
TypicalInstallation=Tipik Kurulum
FullInstallation=Tam Kurulum
CompactInstallation=Küçük Kurulum
CustomInstallation=Özel Kurulum

;Components
AppCoreFiles=WinMerge Çekirdek Dosyalarý
Filters=Süzgeçler
Plugins=Eklentiler

;Localization Components
Languages=Diller
BulgarianLanguage=Bulgarca menü ve pencereler
CatalanLanguage=Katalanca menü ve pencereler
ChineseSimplifiedLanguage=Çince (Basitleþtirilmiþ) menü ve pencereler
ChineseTraditionalLanguage=Çince (Geleneksel) menü ve pencereler
CroatianLanguage=Hýrvatça menü ve pencereler
CzechLanguage=Çekce menü ve pencereler
DanishLanguage=Danimarkaca menü ve pencereler
DutchLanguage=Felemenk menü ve pencereler
FrenchLanguage=Fransýzca menü ve pencereler
GalicianLanguage=Galiçce menü ve pencereler
GermanLanguage=Almanca menü ve pencereler
GreekLanguage=Yunanca menü ve pencereler
HungarianLanguage=Macarca menü ve pencereler
ItalianLanguage=Italyanca menü ve pencereler
JapaneseLanguage=Japonca menü ve pencereler
KoreanLanguage=Korece menü ve pencereler
NorwegianLanguage=Norveçce menü ve pencereler
PolishLanguage=Lehçe menü ve pencereler
PortugueseBrazilLanguage=Portekizce (Brezilya) menü ve pencereler
PortugueseLanguage=Portekizce menü ve pencereler
RomanianLanguage=Romence menü ve pencereler
RussianLanguage=Rusça menü ve pencereler
SlovakLanguage=Slovakça menü ve pencereler
SlovenianLanguage=Slovence menü ve pencereler
SpanishLanguage=Ýspanyolca menü ve pencereler
SwedishLanguage=Ýsveçce menü ve pencereler
TurkishLanguage=Türkçe menü ve pencereler
UkrainianLanguage=Ukraynaca menü ve pencereler

;Tasks
ExplorerContextMenu=&Windows gezgini sað týk menü bütünleþmesini etkinleþtir
IntegrateTortoiseCVS=&TortoiseCVS ile bütünleþ
IntegrateTortoiseSVN=T&ortoiseSVN ile bütünleþ
IntegrateClearCase=Rational &ClearCase ile bütünleþ
AddToPath=Win&Merge klasörünü sistem yoluna ekle

;Icon Labels
ReadMe=Beni Oku
UsersGuide=Kullanýcý Kitabý
ViewStartMenuFolder=&WinMerge'i Baþlangýç Menüsü Klasöründe Göster

;Code Dialogs
DeletePreviousStartMenu=The installer has detected that you changed the location of your start menu from "%s" to "%s". Would you like to delete the previous start menu folder?

; Project file description
ProjectFileDesc=WinMerge Proje dosyasý
