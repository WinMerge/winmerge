; *** Inno Setup version 5.1.0+ Turkish messages ***
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
WelcomeFontName=Verdana
WelcomeFontSize=12
TitleFontName=Arial
TitleFontSize=29
CopyrightFontName=Arial
CopyrightFontSize=8
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
SetupLdrStartupMessage=Bu kurulum %1 programýný yükleyecektir. Devam etmek istiyor musunuz?
LdrCannotCreateTemp=Geçici bir dosya oluþturulamadý. Kurulum iptal edildi.
LdrCannotExecTemp=Geçici dizindeki dosya çalýþtýrýlamadý. Kurulum iptal edildi

; *** Startup error messages
LastErrorMessage=%1.%n%nHata %2: %3
SetupFileMissing=%1 adlý dosya kurulum dizininde bulunamadý. Lütfen problemi düzeltiniz veya programýn yeni bir kopyasýný edininiz.
SetupFileCorrupt=Kurulum dosyalarý bozulmuþ. Lütfen programýn yeni bir kopyasýný edininiz.
SetupFileCorruptOrWrongVer=Kurulum dosyalarý bozulmuþ veya kurulumun bu sürümü ile uyuþmuyor olabilir. Lütfen problemi düzeltiniz veya Programýn yeni bir kopyasýný edininiz.
NotOnThisPlatform=Bu program %1 üzerinde çalýþtýrýlamaz.
OnlyOnThisPlatform=Bu program sadece %1 üzerinde çalýþtýrýlmalýdýr.
OnlyOnTheseArchitectures=Bu program sadece %n%n%1 mimarideki iþlemciler için düzenlenmiþ Windows'larda çalýþýr
MissingWOW64APIs=Kullandýðýnýz Windows sürümü Kur'un 64-bit yükleme yapabilmesi için gerekli olan özelliklere sahip deðildir. Bu problemi ortadan kaldýrmak için lütfen "Service Pack %1" yükleyiniz.
WinVersionTooLowError=Bu programý çalýþtýrabilmek içim %1 %2 sürümü veya daha sonrasý yüklü olmalýdýr.
WinVersionTooHighError=Bu program %1 %2 sürümü veya sonrasýnda çalýþmaz.
AdminPrivilegesRequired=Bu program kurulurken yönetici olarak oturum açýlmýþ olmak gerekmektedir.
PowerUserPrivilegesRequired=Bu program kurulurken Yönetici veya Güç Yöneticisi Grubu üyesi olarak giriþ yapýlmýþ olmasý gerekmektedir.
SetupAppRunningError=Kur %1 programýnýn çalýþtýðýný tesbit etti.%n%nLütfen bu programýn çalýþan bütün parçalarýný þimdi kapatýnýz, daha sonra devam etmek için Tamam'a veya çýkmak için Ýptal'e basýnýz.
UninstallAppRunningError=Kaldýr %1 programýnýn çalýþtýðýný tespit etti.%n%nLütfen bu programýn çalýþan bütün parçalarýný þimdi kapatýnýz, daha sonra devam etmek için Tamam'a veya çýkmak için Ýptal'e basýnýz.

; *** Misc. errors
ErrorCreatingDir=Kur " %1 " dizinini oluþturamadý.
ErrorTooManyFilesInDir=" %1 " dizininde bir dosya oluþturulamadý. Çünkü dizin çok fazla dosya içeriyor

; *** Setup common messages
ExitSetupTitle=Kur'dan Çýk
ExitSetupMessage=Kurulum tamamlanmadý. Þimdi çýkarsanýz program kurulmuþ olmayacak.%n%nDaha sonra Kur'u tekrar çalýþtýrarak kurulumu tamamlayabilirsiniz.%n%nKur'dan çýkmak istediðinizden emin misiniz?
AboutSetupMenuItem=Kur H&akkýnda...
AboutSetupTitle=Kur Hakkýnda
AboutSetupMessage=%1 %2 sürümü%n%3%n%n%1 internet:%n%4
AboutSetupNote=
TranslatorNote=Ýyi bir kurulum programý arýyorsanýz buldunuz...%nhttp://www.yildizyazilim.gen.tr,http://www.yelkenbilgisayar.com

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
ButtonNewFolder=Ye&ni Dizin Oluþtur

; *** "Select Language" dialog messages
SelectLanguageTitle=Kur Dilini Seçiniz
SelectLanguageLabel=Lütfen kurulum sýrasýnda kullanacaðýnýz dili seçiniz:

; *** Common wizard text
ClickNext=Devam etmek için Ýleri'ye , çýkmak için Ýptal 'e basýnýz.
BeveledLabel=Inno Setup 5.1+ Türkçe
BrowseDialogTitle=Dizine Gözat
BrowseDialogLabel=Aþaðýdaki listeden bir dizin seçip, daha sonra Tamam tuþuna basýnýz.
NewFolderName=Yeni Dizin

; *** "Welcome" wizard page
WelcomeLabel1=[name] Kurulum Sihirbazýna Hoþgeldiniz.
WelcomeLabel2=Kur þimdi [name/ver] programýný bilgisayarýnýza yükleyecektir.%n%nDevam etmeden önce çalýþan diðer bütün programlarý kapatmanýz tavsiye edilir.

; *** "Password" wizard page
WizardPassword=Þifre
PasswordLabel1=Bu kurulum þifre korumalýdýr.
PasswordLabel3=Lütfen þifreyi giriniz. Daha sonra devam etmek için Ýleri'ye basýnýz. Lütfen þifreyi girerken Büyük-Küçük harflere dikkat ediniz.
PasswordEditLabel=&Þifre:
IncorrectPassword=Girdiðiniz þifre hatalý. Lütfen tekrar deneyiniz.

; *** "License Agreement" wizard page
WizardLicense=Lisans Anlaþmasý
LicenseLabel=Lütfen devam etmeden önce aþaðýdaki önemli bilgileri okuyunuz.
LicenseLabel3=Lütfen Aþaðýdaki Lisans Anlaþmasýný okuyunuz. Kuruluma devam edebilmek için bu anlaþmanýn koþullarýný kabul etmiþ olmalýsýnýz.
LicenseAccepted=Anlaþmayý Kabul &Ediyorum.
LicenseNotAccepted=Anlaþmayý Kabul Et&miyorum.

; *** "Information" wizard pages
WizardInfoBefore=Bilgi
InfoBeforeLabel=Lütfen devam etmeden önce aþaðýdaki önemli bilgileri okuyunuz.
InfoBeforeClickLabel=Kur ile devam etmeye hazýr olduðunuzda Ýleri'yi týklayýnýz.
WizardInfoAfter=Bilgi
InfoAfterLabel=Lütfen devam etmeden önce aþaðýdaki önemli bilgileri okuyunuz.
InfoAfterClickLabel=Kur ile devam etmeye hazýr olduðunuzda Ýleri'yi týklayýnýz.

; *** "User Information" wizard page
WizardUserInfo=Kullanýcý Bilgileri
UserInfoDesc=Lütfen bilgilerinizi giriniz.
UserInfoName=K&ullanýcý Adý:
UserInfoOrg=Þi&rket:
UserInfoSerial=Seri Numarasý:
UserInfoNameRequired=Bir isim girmelisiniz.

; *** "Select Destination Directory" wizard page
WizardSelectDir=Kurulacak dizini seçiniz
SelectDirDesc=[name] hangi dizine kurulsun?
SelectDirLabel3=Kur [name] programýný aþaðýdaki dizine kuracaktýr.
SelectDirBrowseLabel=Devam etmek için Ýleri'ye basýnýz. Baþka bir dizin seçmek istiyorsanýz, Gözat'a basýnýz.
DiskSpaceMBLabel=Bu program en az [mb] MB disk alaný gerektirmektedir.
ToUNCPathname=Kur UNC tipindeki dizin yollarýna (Örn:\\yol vb.) kurulum yapamaz. Eðer Að üzerinde kurulum yapmaya çalýþýyorsanýz. Bir að sürücüsü tanýtmanýz gerekir.
InvalidPath=Sürücü ismi ile birlikte tam yolu girmelisiniz; Örneðin %nC:\APP%n%n veya bir UNC yolunu %n%n\\sunucu\paylaþým%n%n þeklinde girmelisiniz.
InvalidDrive=Seçtiðiniz sürücü bulunamadý veya ulaþýlamýyor. Lütfen baþka bir sürücü seçiniz.
DiskSpaceWarningTitle=Yetersiz disk alaný
DiskSpaceWarning=Kur en az %1 KB kullanýlabilir disk alaný gerektirmektedir. Ancak seçili diskte %2 KB boþ alan bulunmaktadýr.%n%nYine de devam etmek istiyor musunuz?
DirNameTooLong=Dizin adý veya yolu çok uzun.
InvalidDirName=Dizin adý geçersiz.
BadDirName32=Dizin adý takib eden karakterlerden her hangi birini içeremez:%n%n%1
DirExistsTitle=Dizin Bulundu
DirExists=Dizin:%n%n%1%n%n zaten var. Yine de bu dizine kurmak istediðinizden emin misiniz?
DirDoesntExistTitle=Dizin Bulunamadý
DirDoesntExist=Dizin:%n%n%1%n%nbulunmamaktadýr. Bu dizini oluþturmak ister misiniz?

; *** "Select Components" wizard page
WizardSelectComponents=Bileþen Seç
SelectComponentsDesc=Hangi bileþenler kurulsun?
SelectComponentsLabel2=Kurmak istediðiniz bileþenleri seçiniz; istemediklerinizi temizleyiniz.Devam etmeye hazýr olduðunuz zaman Ýleri'ye týklayýnýz.
FullInstallation=Tam Kurulum
; if possible don't translate 'Compact' as 'Minimal' (I mean 'Minimal' in your language)
CompactInstallation=Normal Kurulum
CustomInstallation=Özel Kurulum
NoUninstallWarningTitle=Var olan Bileþenler
NoUninstallWarning=Kur aþaðýdaki bileþenlerin kurulu olduðunu tespit etti:%n%n%1%n%nBu bileþenleri seçmeyerek kaldýrmýþ olmayacaksýnýz.%n%nYine de devam etmek istiyor musunuz?
ComponentSize1=%1 KB
ComponentSize2=%1 MB
ComponentsDiskSpaceMBLabel=Seçili bileþenler için en az [mb] MB disk alaný gerekmektedir.

; *** "Select Additional Tasks" wizard page
WizardSelectTasks=Ek Görevleri Seçiniz
SelectTasksDesc=Hangi görevler yerine getirilsin?
SelectTasksLabel2=[name] kurulurken istediðiniz ek görevleri seçip Ýleri'ye týklayýnýz.

; *** "Baþlat Menüsü Dizini Seç" sihirbaz sayfasý
WizardSelectProgramGroup=Baþlat Menüsü Dizinini seçiniz
SelectStartMenuFolderDesc=Kur program kýsayollarýný nereye yerleþtirsin?
SelectStartMenuFolderLabel3=Kur programýn kýsayollarýný aþaðýdaki Baþlat Menüsü dizinine kuracak.
SelectStartMenuFolderBrowseLabel=Devam etmek için, Ýleri'ye basýnýz. Baþka bir dizin seçmek istiyorsanýz, Gözat'a basýnýz.
MustEnterGroupName=Bir dizin ismi girmelisiniz.
GroupNameTooLong=Dizin adý veya yolu çok uzun.
InvalidGroupName=Dizin adý geçersiz.
BadGroupName=Dizin adý, takip eden karakterlerden her hangi birini içeremez:%n%n%1
NoProgramGroupCheck2=&Baþlat menüsünde kýsayol oluþturma

; *** "Ready to Install" wizard page
WizardReady=Yükleme için hazýr
ReadyLabel1=Kur [name] programýný bilgisayarýnýza kurmak için hazýr.
ReadyLabel2a=Kuruluma devam etmek için Kur'a , ayarlarýnýzý kontrol etmek veya deðiþtirmek için Geri'ye týklayýnýz.
ReadyLabel2b=Kuruluma devam etmek için Kur'a týklayýnýz.
ReadyMemoUserInfo=Kullanýcý Bilgisi:
ReadyMemoDir=Hedef Dizin:
ReadyMemoType=Kurulum Tipi:
ReadyMemoComponents=Seçili Bileþenler:
ReadyMemoGroup=Baþlat Menüsü :
ReadyMemoTasks=Ek Görevler:

; *** "Kur Hazýlanýyor" sihirbaz sayfasý
WizardPreparing=Kurulum Hazýrlanýyor
PreparingDesc=Kur [name] programýný bilgisayarýnýza kurmak için hazýrlanýyor.
PreviousInstallNotCompleted=Bir önceki Kurulum/Kaldýr programýna ait iþlem tamamlanmamýþ.Önceki kurulum iþleminin tamamlanmasý için bilgisayarýnýzý yeniden baþlatmalýsýnýz.%n%nBilgisayarýnýz tekrar baþladýktan sonra,Kurulum'u tekrar çalýþtýrarak [name] programýný kurma iþlemine devam edebilirsiniz.
CannotContinue=Kur devam edemiyor. Lütfen Ýptal'e týklayýp Çýkýn.

; *** "Kuruluyor" sihirbaz
WizardInstalling=Kuruluyor
InstallingLabel=Lütfen [name] bilgisayarýnýza kurulurken bekleyiniz.

; *** "Setup Completed" wizard page
FinishedHeadingLabel=[name] Kur Sihirbazý tamamlanýyor
FinishedLabelNoIcons=Kur [name] programýný bilgisayarýnýza kurma iþlemini tamamladý.
FinishedLabel=Kur [name] programýný bilgisayarýnýza kurma iþlemini tamamladý. Program yüklenen kýsayol ikonlarýna týklanarak çalýþtýrýlabilir.
ClickFinish=Kur'dan çýkmak için "Son"'a týklayýnýz.
FinishedRestartLabel=[name] programýnýn kurulumunu bitirmek için, Kur bilgisayarýnýzý yeniden baþlatacak. Bilgisayarýnýz yeniden baþlatýlsýn mý?
FinishedRestartMessage=[name] kurulumunu bitirmek için, bilgisayarýnýzýn yeniden baþlatýlmasý gerekmektedir. %n%nBiligisayarýnýz yeniden baþlatýlsýn mý?
ShowReadmeCheck=Beni Oku dosyasýný okumak istiyorum.
YesRadio=&Evet , bilgisayar yeniden baþlatýlsýn.
NoRadio=&Hayýr, daha sonra yeniden baþlatýrým.
; used for example as 'Run MyProg.exe'
RunEntryExec=%1 uygulamasýný Çalýþtýr
; used for example as 'View Readme.txt'
RunEntryShellExec=%1 dosyasýný görüntüle

; *** "Setup Needs the Next Disk" stuff
ChangeDiskTitle=Bir sonraki disketi takýnýz
SelectDiskLabel2=%1 numaralý diski takýp, Tamam'ý týklayýnýz.%n%nEðer dosyalar baþka bir yerde bulunuyor ise doðru yolu yazýnýz veya Gözat'ý týklayýnýz.
PathLabel=&Yol:
FileNotInDir2=" %1 " adlý dosya " %2 " dizininde bulunamadý. Lütfen doðru diski veya dosyayý seçiniz.
SelectDirectoryLabel=Lütfen sonraki diskin yerini belirleyiniz.

; *** Installation phase messages
SetupAborted=Kurulum tamamlanamadý.%n%nLütfen problemi düzeltiniz veya Kurulum'u tekrar çalýþtýrýnýz.
EntryAbortRetryIgnore=Tekrar denemek için "Tekrar Dene" ye , yine de devam etmek için Yoksay'a , kurulumu iptal etmek için ise Ýptal'e týklayýnýz.

; *** Installation status messages
StatusCreateDirs=Dizinler oluþturuluyor...
StatusExtractFiles=Dosyalar çýkartýlýyor...
StatusCreateIcons=Program kýsayollarý oluþturuluyor...
StatusCreateIniEntries=INI girdileri oluþturuluyor...
StatusCreateRegistryEntries=Kayýt Defteri girdileri oluþturuluyor...
StatusRegisterFiles=Dosyalar sisteme kaydediliyor...
StatusSavingUninstall=Kaldýr bilgileri kaydediliyor...
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
ErrorIniEntry=" %1 " adlý dosyada INI girdisi yazma hatasý.

; *** File copying errors
FileAbortRetryIgnore=Yeniden denemek için "Yeniden Dene" ye, dosyayý atlamak için Yoksay'a (önerilmez), Kurulumu iptal etmek için Ýptal'e týklayýnýz.
FileAbortRetryIgnore2=Yeniden denemek için "Yeniden Dene" ye , yine de devam etmek için Yoksay'a (önerilmez), Kurulumu Ýptal etmek için Ýptal'e týklayýnýz.
SourceIsCorrupted=Kaynak Dosya Bozulmuþ
SourceDoesntExist=%1 adlý Kaynak Dosya bulunamadý.
ExistingFileReadOnly=Dosya Salt Okunur.%n%nSalt Okunur özelliðini kaldýrýp yeniden denemek için Yeniden Dene'yi , dosyasý atlamak için Yoksay'ý , Kurulumu iptal etmek için Ýptal'i týklayýnýz.
ErrorReadingExistingDest=Dosyayý okurken bir hata oluþtu :
FileExists=Dosya zaten var.%n%nKurulum'un üzerine yazmasýný ister misiniz?
ExistingFileNewer=Zaten var olan dosya Kurulum'un yüklemek istediði dosyadan daha yeni. Var olan dosyayý saklamanýz önerilir.%n%nVar olan dosya saklansýn mý?
ErrorChangingAttr=Zaten var olan dosyanýn özelliði deðiþtirilirken bir hata oluþtu:
ErrorCreatingTemp=Hedef dizinde dosya oluþturulurken bir hata oluþtu:
ErrorReadingSource=Kaynak dosya okunurken bir hata oluþtu:
ErrorCopying=Bir dosya kopyalanýrken bir hata oluþtu:
ErrorReplacingExistingFile=Zaten var olan dosya deðiþtirilirken bir hata oluþtu:
ErrorRestartReplace=RestartReplace baþarýsýz oldu:
ErrorRenamingTemp=Hedef dizinde bulunan dosyanýn adý deðiþtirilirken hata oldu:
ErrorRegisterServer=%1 adlý DLL/OCX sisteme tanýtýlamadý.
ErrorRegisterServerMissingExport=DllRegisterServer çýkýþý bulunamadý
ErrorRegisterTypeLib=%1 adlý tip kütüphanesi (Type Library) sisteme tanýtýlamadý

; *** Post-installation errors
ErrorOpeningReadme=Beni Oku dosyasý açýlýrken hata oluþtu.
ErrorRestartingComputer=Kurulum bilgisayarý yeniden baþlatamadý. Lütfen kendiniz kapatýnýz.

; *** Uninstaller messages
UninstallNotFound=%1 adlý dosya bulunamadý. Kaldýrma programý çalýþtýrýlamadý.
UninstallOpenError="%1" dosyasý açýlamýyor. Kaldýrma programý çalýþtýrýlamadý.
UninstallUnsupportedVer=%1 adlý Kaldýr bilgi dosyasý kaldýrma programýnýn bu sürümü ile uyuþmuyor. Kaldýrma programý çalýþtýrýlamadý.
UninstallUnknownEntry=Kaldýr Bilgi dosyasýndaki %1 adlý satýr anlaþýlamadý
ConfirmUninstall=%1 ve bileþenlerini kaldýrmak istediðinizden emin misiniz?
UninstallOnlyOnWin64=Bu kurulum sadece 64-bit Windows'lardan kaldýrýlabilir.
OnlyAdminCanUninstall=Bu kurulum sadece yönetici yetkisine sahip kullanýcýlar tarafýndan kaldýrabilir.
UninstallStatusLabel=Lütfen %1 programý bilgisayarýnýzdan kaldýrýlýrken bekleyin...
UninstalledAll=%1 programý bilgisayarýnýzdan tamamen kaldýrýldý.
UninstalledMost=%1 programýnýn kaldýrýlma iþlemi sona erdi.%n%nBazý bileþenler kaldýrýlamadý. Bu dosyalarý kendiniz silebilirsiniz.
UninstalledAndNeedsRestart=%1 programýnýn kaldýrýlmasý tamamlandý, Bilgisayarýnýzý yeniden baþlatmalýsýnýz.%n%nÞimdi yeniden baþlatýlsýn mý?
UninstallDataCorrupted="%1" adlý dosya bozuk. . Kaldýrma programý çalýþtýrýlamadý.

; *** Uninstallation phase messages
ConfirmDeleteSharedFileTitle=Paylaþýlan dosya kaldýrýlsýn mý?
ConfirmDeleteSharedFile2=Sistemde paylaþýlan bazý dosyalarýn artýk hiçbir program tarafýndan kullanýlmadýðýný belirtiyor. Kaldýr bu paylaþýlan dosyalarý silsin mi?%n%n Bu dosya bazý programlar tafarýndan kullanýlýyorsa ve silinmesini isterseniz, bu programalar düzgün çalýþmayabilir. Emin deðilseniz, Hayýr'a týklayýnýz. Dosyanýn sisteminizde durmasý hiçbir zarar vermez.
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
OptionalFeatures=Optional Features:
CreateDesktopIcon=Masaüstü simg&esi oluþtur
CreateQuickLaunchIcon=Hýzlý Baþlat simgesi &oluþtur
ProgramOnTheWeb=%1 Web Sitesi
UninstallProgram=%1 Programýný Kaldýr
LaunchProgram=%1 Programýný Çalýþtýr
AssocFileExtension=%2 dosya uzantýlarýný %1 ile iliþkilendir
AssocingFileExtension=%2 dosya uzantýlarý %1 ile iliþkilendiriliyor...

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
CroatianLanguage=Croatian menus and dialogs
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
IntegrateDelphi4=Borland® Delphi &4 Binary File Support
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
