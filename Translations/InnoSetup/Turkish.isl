; *** Inno Setup version 5.1.11+ Turkish messages ***
;
; To download user-contributed translations of this file, go to:
;   http://www.jrsoftware.org/is3rdparty.php
;
; Note: When translating this text, do not add periods (.) to the end of
; messages that didn't have them already, because on those messages Inno
; Setup adds the periods automatically (appending a period would result in
; two periods being displayed).

[LangOptions]
; The following three entries are very important. Be sure to read and 
; understand the '[LangOptions] section' topic in the help file.
LanguageName=Türkçe
LanguageID=$041F
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

[Messages]

; *** Application titles
SetupAppTitle=Kur
SetupWindowTitle=%1 - Kur
UninstallAppTitle=Kaldýr
UninstallAppFullTitle=%1 Kaldýr

; *** Misc. common
InformationTitle=Bilgi
ConfirmTitle=Onay
ErrorTitle=Hata

; *** SetupLdr messages
SetupLdrStartupMessage=Bu kurulum %1 uygulamasýný yükleyecek. Devam etmek istiyor musunuz?
LdrCannotCreateTemp=Geçici bir dosya oluþturulamadý. Kurulum iptal edildi
LdrCannotExecTemp=Geçici klasördeki dosya çalýþtýrýlamadý. Kurulum iptal edildi

; *** Startup error messages
LastErrorMessage=%1.%n%nHata %2: %3
SetupFileMissing=%1 adlý dosya kurulum klasöründe bulunamadý. Lütfen sorunu düzeltin ya da uygulamanýn yeni bir kopyasýný edinin.
SetupFileCorrupt=Kurulum dosyalarý bozulmuþ. Lütfen uygulamanýn yeni bir kopyasýný edinin.
SetupFileCorruptOrWrongVer=Kurulum dosyalarý bozulmuþ ya da bu kurucu sürümü ile uyuþmuyor olabilir. Lütfen sorunu düzeltin ya da uygulamanýn yeni bir kopyasýný edinin.
NotOnThisPlatform=Bu uygulama %1 üzerinde çalýþmaz.
OnlyOnThisPlatform=Bu uygulama %1 üzerinde çalýþtýrýlmalýdýr.
OnlyOnTheseArchitectures=Bu uygulama yalnýz aþaðýdaki iþlemci mimarileri için geliþtirilmiþ Windows sürümlerinde çalýþýr:%n%n%1
MissingWOW64APIs=Kullandýðýnýz Windows sürümü, kurucunun 64-bit kurulum yapabilmesi için gerekli özellikleri taþýmýyor. Bu sorunu aþmak için Hizmet Paketi %1 sürümünü yükleyin.
WinVersionTooLowError=Bu uygulama için %1 %2 ya da üzerindeki bir sürüm gereklidir.
WinVersionTooHighError=Bu uygulama %1 %2 ya da üzerindeki sürümlere kurulamaz.
AdminPrivilegesRequired=Bu uygulama kurulurken yönetici olarak oturum açmýþ olmanýz gereklidir.
PowerUserPrivilegesRequired=Bu uygulama kurulurken Yönetici ya da Power Users Grubu üyesi olarak oturum açmýþ olmanýz gereklidir.
SetupAppRunningError=Kurucu %1 uygulamasýnýn çalýþtýðýný algýladý.%n%nLütfen bu uygulamanýn çalýþan tüm kopyalarýný kapatýn ve devam etmek için Tamam üzerine, çýkmak için Ýptal üzerine týklayýn.
UninstallAppRunningError=Kaldýrýcý %1 uygulamasýnýn çalýþtýðýný algýladý.%n%nLütfen bu uygulamanýn çalýþan tüm kopyalarýný kapatýn ve devam etmek için Tamam üzerine ya da çýkmak için Ýptal üzerine týklayýn.

; *** Misc. errors
ErrorCreatingDir=Kurucu " %1 " klasörünü oluþturamadý.
ErrorTooManyFilesInDir=" %1 " klasöründe çok fazla dosya bulunduðundan burada bir dosya oluþturulamadý.

; *** Setup common messages
ExitSetupTitle=Kurucudan Çýk
ExitSetupMessage=Kurulum tamamlanmadý. Kurucudan þimdi çýkarsanýz uygulama kurulmuþ olmayacak.%n%nDaha sonra Kurucuyu yeniden çalýþtýrarak kurulumu tamamlayabilirsiniz.%n%nKurucudan çýkmak istediðinize emin misiniz?
AboutSetupMenuItem=Kurucu H&akkýnda...
AboutSetupTitle=Kurucu Hakkýnda
AboutSetupMessage=%1 %2 sürümü%n%3%n%n%1 Web Sitesi:%n%4
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
SelectLanguageTitle=Kurucu Dilini Seçin
SelectLanguageLabel=Lütfen kurulum sýrasýnda kullanacaðýnýz dili seçin:

; *** Common wizard text
ClickNext=Devam etmek için Ýleri üzerine , çýkmak için Ýptal üzerine týklayýn.
BeveledLabel=
BrowseDialogTitle=Klasöre Gözat
BrowseDialogLabel=Aþaðýdaki listeden bir klasör seçip Tamam üzerine týklayýn.
NewFolderName=Yeni Klasör

; *** "Welcome" wizard page
WelcomeLabel1=[name] Kurulum Yardýmcýsýna Hoþ Geldiniz.
WelcomeLabel2=Kurucu þimdi [name/ver] uygulamasýný bilgisayarýnýza kuracak.%n%nDevam etmeden önce çalýþan tüm diðer uygulamalarý kapatmanýz önerilir.

; *** "Password" wizard page
WizardPassword=Parola
PasswordLabel1=Bu kurulum parola korumalýdýr.
PasswordLabel3=Lütfen parolayý yazýn. Daha sonra devam etmek için Ýleri üzerine týklayýn. Lütfen parolayý yazarken Büyük-Küçük harflere dikkat edin.
PasswordEditLabel=&Parola:
IncorrectPassword=Yazdýðýnýz parola hatalý. Lütfen yeniden deneyin.

; *** "License Agreement" wizard page
WizardLicense=Lisans Anlaþmasý
LicenseLabel=Devam etmeden önce aþaðýdaki önemli bilgileri okuyun.
LicenseLabel3=Lütfen aþaðýdaki lisans anlaþmasýný okuyun. Kuruluma devam edebilmek için bu anlaþmanýn koþullarýný kabul etmelisiniz.
LicenseAccepted=Anlaþmayý Kabul &Ediyorum.
LicenseNotAccepted=Anlaþmayý Kabul Et&miyorum.

; *** "Information" wizard pages
WizardInfoBefore=Bilgiler
InfoBeforeLabel=Devam etmeden önce aþaðýdaki önemli bilgileri okuyun.
InfoBeforeClickLabel=Kuruluma devam etmeye hazýr olduðunuzda Ýleri üzerine týklayýn.
WizardInfoAfter=Bilgiler
InfoAfterLabel=Lütfen devam etmeden önce aþaðýdaki önemli bilgileri okuyun.
InfoAfterClickLabel=Kuruluma devam etmeye hazýr olduðunuzda Ýleri üzerine týklayýn.

; *** "User Information" wizard page
WizardUserInfo=Kullanýcý Bilgileri
UserInfoDesc=Lütfen bilgilerinizi yazýn.
UserInfoName=K&ullanýcý Adý:
UserInfoOrg=Ku&rum:
UserInfoSerial=&Seri Numarasý:
UserInfoNameRequired=Bir ad yazmalýsýnýz.

; *** "Select Destination Directory" wizard page
WizardSelectDir=Kurulum klasörünü seçin
SelectDirDesc=[name] hangi klasöre kurulsun?
SelectDirLabel3=Kurucu [name] uygulamasýný aþaðýdaki klasöre kuracak.
SelectDirBrowseLabel=Devam etmek için Ýleri üzerine týklayýn. Baþka bir klasör seçmek için Gözat üzerine týklayýn.
DiskSpaceMBLabel=Bu uygulama için en az [mb] MB disk alaný gereklidir.
ToUNCPathname=Kurucu, UNC tipindeki klasör yollarýna (Örneðin:\\yol gibi) kurulum yapamaz. Að üzerine kurulum yapmaya çalýþýyorsanýz. Að sürücüsüne bir harf atayarak denemeniz gerekir.
InvalidPath=Sürücü adý ile birlikte tam yolu yazmalýsýnýz; Örneðin %nC:\APP%n%n ya da bir UNC yolunu %n%n\\sunucu\paylaþým%n%n þeklinde yazmalýsýnýz.
InvalidDrive=Seçtiðiniz sürücü bulunamadý ya da ulaþýlamýyor. Lütfen baþka bir sürücü seçin.
DiskSpaceWarningTitle=Disk Alaný Yetersiz
DiskSpaceWarning=Kurucu için en az %1 KB kullanýlabilir disk alaný gerekli. Ancak seçili diskte %2 KB boþ alan var.%n%nYine de devam etmek istiyor musunuz?
DirNameTooLong=Klasör adý ya da yolu çok uzun.
InvalidDirName=Klasör adý geçersiz.
BadDirName32=Klasör adýnda þu karakterlerden herhangi biri bulunamaz:%n%n%1
DirExistsTitle=Klasör Bulundu
DirExists=%n%n%1%n%n klasörü zaten var. Yine de bu klasöre kurmak istediðinizden emin misiniz?
DirDoesntExistTitle=Klasör Bulunamadý
DirDoesntExist=%n%n%1%n%n klasörü bulunamadý. Bu klasörü oluþturmak ister misiniz?

; *** "Select Components" wizard page
WizardSelectComponents=Bileþenleri Seçin
SelectComponentsDesc=Hangi bileþenler kurulsun?
SelectComponentsLabel2=Kurmak istediðiniz bileþenleri seçin; istemediklerinizi seçmeyin.Devam etmeye hazýr olduðunuz zaman Ýleri üzerine týklayýn.
FullInstallation=Tam Kurulum
; if possible don't translate 'Compact' as 'Minimal' (I mean 'Minimal' in your language)
CompactInstallation=Normal Kurulum
CustomInstallation=Özel Kurulum
NoUninstallWarningTitle=Kurulu Bileþenler
NoUninstallWarning=Kurucu aþaðýdaki bileþenlerin zaten kurulu olduðunu algýladý:%n%n%1%n%nBu bileþenleri seçmeyerek kaldýrmýþ olmayacaksýnýz.%n%nYine de devam etmek istiyor musunuz?
ComponentSize1=%1 KB
ComponentSize2=%1 MB
ComponentsDiskSpaceMBLabel=Seçilmiþ bileþenler için en az [mb] MB disk alaný gereklidir.

; *** "Select Additional Tasks" wizard page
WizardSelectTasks=Ek Görevleri Seçin
SelectTasksDesc=Hangi görevler yerine getirilsin?
SelectTasksLabel2=[name] kurulurken istediðiniz ek görevleri seçip Ýleri üzerine týklayýn.

; *** "Baþlat Menüsü Klasörünü Seçin" yardýmcý sayfasý
WizardSelectProgramGroup=Baþlat Menüsü Klasörünü Seçin
SelectStartMenuFolderDesc=Kurucu uygulama kýsayollarýný nereye eklesin?
SelectStartMenuFolderLabel3=Kurucu uygulama kýsayollarýný aþaðýdaki Baþlat Menüsü klasörüne kuracak.
SelectStartMenuFolderBrowseLabel=Devam etmek için, Ýleri üzerine týklayýn. Baþka bir klasör seçmek istiyorsanýz, Gözat üzerine týklayýn.
MustEnterGroupName=Bir klasör adý yazmalýsýnýz.
GroupNameTooLong=Klasör adý ya da yolu çok uzun.
InvalidGroupName=Klasör adý geçersiz.
BadGroupName=Klasör adýnda þu karakterlerden herhangi biri bulunamaz:%n%n%1
NoProgramGroupCheck2=&Baþlat menüsü klasörü oluþturulmasýn

; *** "Ready to Install" wizard page
WizardReady=Kuruluma Hazýr
ReadyLabel1=Kurucu [name] uygulamasýný bilgisayarýnýza kurmaya hazýr.
ReadyLabel2a=Kuruluma devam etmek için Kur üzerine, ayarlarýnýzý kontrol etmek ya da deðiþtirmek için Geri üzerine týklayýn.
ReadyLabel2b=Kuruluma devam etmek için Kur üzerine týklayýn.
ReadyMemoUserInfo=Kullanýcý Bilgileri:
ReadyMemoDir=Hedef Klasör:
ReadyMemoType=Kurulum Türü:
ReadyMemoComponents=Seçilmiþ Bileþenler:
ReadyMemoGroup=Baþlat Menüsü :
ReadyMemoTasks=Ek Görevler:

; *** "Kurulum Hazýrlanýyor" yardýmcý sayfasý
WizardPreparing=Kurulum Hazýrlanýyor
PreparingDesc=Kurucu [name] uygulamasýný bilgisayarýnýza kurmaya hazýrlanýyor.
PreviousInstallNotCompleted=Bir önceki Kur/Kaldýr uygulamasýna ait iþlem tamamlanmamýþ.Önceki kurulum iþleminin tamamlanmasý için bilgisayarý yeniden baþlatmalýsýnýz.%n%nBilgisayarýn yeniden baþlatýlmasýndan sonra, Kurucuyu yeniden çalýþtýrarak [name] uygulamasýný kurma iþlemine devam edebilirsiniz.
CannotContinue=Kurucu devam edemiyor. Lütfen Ýptal üzerine týklayýp Çýkýn.

; *** "Kuruluyor" yardýmcý sayfasý
WizardInstalling=Kuruluyor
InstallingLabel=Lütfen [name] uygulamasý bilgisayara kurulurken bekleyin.

; *** "Setup Completed" wizard page
FinishedHeadingLabel=[name] kurulum yardýmcýsý tamamlanýyor
FinishedLabelNoIcons=Kurucu [name] uygulamasýný bilgisayarýnýza kurdu.
FinishedLabel=Kurucu [name] uygulamasýný bilgisayarýnýza kurdu. Uygulama yüklenen kýsayol simgelerine týklanarak çalýþtýrýlabilir.
ClickFinish=Kurucudan çýkmak için Son üzerine týklayýn.
FinishedRestartLabel=[name] uygulamasýnýn kurulumunu bitirmek için, Kurucu bilgisayarý yeniden baþlatacak. Bilgisayar þimdi yeniden baþlatýlsýn mý?
FinishedRestartMessage=[name] kurulumunu bitirmek için, bilgisayarýn yeniden baþlatýlmasý gerekiyor. %n%nBilgisayar yeniden baþlatýlsýn mý?
ShowReadmeCheck=Beni Oku dosyasýný okumak istiyorum.
YesRadio=&Evet, bilgisayar yeniden baþlatýlsýn.
NoRadio=&Hayýr, daha sonra yeniden baþlatacaðým.
; used for example as 'Run MyProg.exe'
RunEntryExec=%1 uygulamasýný çalýþtýr
; used for example as 'View Readme.txt'
RunEntryShellExec=%1 dosyasýný görüntüle

; *** "Setup Needs the Next Disk" stuff
ChangeDiskTitle=Bir sonraki disketi takýn.
SelectDiskLabel2=%1 numaralý disketi takýp, Tamam üzerine týklayýn.%n%nDosyalar farklý bir konumda bulunuyor ise doðru yolu yazýn ya da Gözat üzerine týklayýn.
PathLabel=&Yol:
FileNotInDir2=" %1 " adlý dosya " %2 " klasöründe bulunamadý. Lütfen doðru diski ya da dosyayý seçin.
SelectDirectoryLabel=Lütfen sonraki disketin konumunu belirtin.

; *** Installation phase messages
SetupAborted=Kurulum tamamlanamadý.%n%nLütfen sorunu düzeltin ya da Kurucuyu yeniden çalýþtýrýn.
EntryAbortRetryIgnore=Yeniden denemek için Yeniden Dene üzerine, devam etmek için Yoksay üzerine, kurulumu iptal etmek için Ýptal üzerine týklayýn.

; *** Installation status messages
StatusCreateDirs=Klasörler oluþturuluyor...
StatusExtractFiles=Dosyalar ayýklanýyor...
StatusCreateIcons=Uygulama kýsayollarý oluþturuluyor...
StatusCreateIniEntries=INI kayýtlarý oluþturuluyor...
StatusCreateRegistryEntries=Kayýt Defteri kayýtlarý oluþturuluyor...
StatusRegisterFiles=Dosyalar sisteme kaydediliyor...
StatusSavingUninstall=Kaldýrma bilgileri kaydediliyor...
StatusRunProgram=Kurulum sonlandýrýlýyor...
StatusRollback=Deðiþiklikler geri alýnýyor...

; *** Misc. errors
ErrorInternal2=Ýç sorun: %1
ErrorFunctionFailedNoCode=%1 baþarýsýz oldu.
ErrorFunctionFailed=%1 baþarýsýz oldu; kod  %2
ErrorFunctionFailedWithMessage=%1 baþarýsýz oldu ; kod  %2.%n%3
ErrorExecutingProgram=%1 adlý dosya çalýþtýrýlamadý.

; *** Registry errors
ErrorRegOpenKey=Aþaðýdaki Kayýt Defteri anahtarý açýlýrken sorun çýktý:%n%1\%2
ErrorRegCreateKey=Aþaðýdaki Kayýt Defteri anahtarý oluþturulurken sorun çýktý:%n%1\%2
ErrorRegWriteKey=Aþaðýdaki Kayýt Defteri anahtarýna yazýlýrken sorun çýktý:%n%1\%2

; *** INI errors
ErrorIniEntry="%1" dosyasýna INI kaydý yazýlamadý.

; *** File copying errors
FileAbortRetryIgnore=Yeniden denemek için Yeniden Dene üzerine, dosyayý atlamak için Yoksay üzerine (önerilmez), kurulumu iptal etmek için Ýptal üzerine týklayýn.
FileAbortRetryIgnore2=Yeniden denemek için Yeniden Dene üzerine, devam etmek için Yoksay üzerine (önerilmez), kurulumu iptal etmek için Ýptal üzerine týklayýn.
SourceIsCorrupted=Kaynak Dosya Bozulmuþ
SourceDoesntExist="%1" kaynak dosyasý bulunamadý.
ExistingFileReadOnly=Dosya salt okunur.%n%nSalt okunur özelliðini kaldýrýp yeniden denemek için Yeniden Dene üzerine, dosyayý atlamak için Yoksay üzerine, kurulumu iptal etmek için Ýptal üzerine týklayýn.
ErrorReadingExistingDest=Var olan dosya okunmaya çalýþýlýrken bir sorun çýktý:
FileExists=Dosya zaten var.%n%nKurucunun üzerine yazmasýný ister misiniz?
ExistingFileNewer=Kurucunun yüklemek istediði dosyadan daha yeni bir dosya var. Bu dosyayý korumanýz önerilir.%n%nVar olan dosya korunsun mu?
ErrorChangingAttr=Var olan dosyanýn özelliði deðiþtirilirken bir sorun çýktý:
ErrorCreatingTemp=Hedef klasörde dosya oluþturulurken bir sorun çýktý:
ErrorReadingSource=Kaynak dosya okunurken bir sorun çýktý:
ErrorCopying=Bir dosya kopyalanýrken bir sorun çýktý:
ErrorReplacingExistingFile=Var olan dosya deðiþtirilirken bir sorun çýktý:
ErrorRestartReplace=Yeniden baþlat deðiþtirilemedi:
ErrorRenamingTemp=Hedef klasörde bulunan dosyanýn adý deðiþtirilirken bir sorun çýktý:
ErrorRegisterServer=%1 adlý DLL/OCX sisteme tanýtýlamadý.
ErrorRegSvr32Failed=RegSvr32 %1 çýkýþ hatasý ile baþarýsýz oldu
ErrorRegisterTypeLib=%1 adlý tür kitaplýðý sisteme tanýtýlamadý

; *** Post-installation errors
ErrorOpeningReadme=Beni Oku dosyasý açýlýrken bir sorun çýktý.
ErrorRestartingComputer=Kurulum bilgisayarý yeniden baþlatamadý. Lütfen kendiniz yeniden baþlatýn.

; *** Uninstaller messages
UninstallNotFound=%1 dosyasý bulunamadý. Kaldýrýlamadý.
UninstallOpenError="%1" dosyasý açýlamadý. Kaldýrýlamadý.
UninstallUnsupportedVer="%1" kaldýrma günlüðü dosyasý kurucunun bu bu sürümüne uygun deðil. Kaldýrýlamadý.
UninstallUnknownEntry=Kaldýrma günlüðünde bilinmeyen bir kayýt ile (%1) karþýlaþýldý
ConfirmUninstall=%1 ve bileþenlerini kaldýrmak istediðinize emin misiniz?
UninstallOnlyOnWin64=Bu kurulum yalnýz 64-bit Windows üzerinden kaldýrýlabilir.
OnlyAdminCanUninstall=Bu kurulum yalnýz yönetici yetkisi olan kullanýcýlar tarafýndan kaldýrabilir.
UninstallStatusLabel=Lütfen %1 uygulamasý bilgisayarýnýzdan kaldýrýlýrken bekleyin...
UninstalledAll=%1 uygulamasý bilgisayarýnýzdan tamamen kaldýrýldý.
UninstalledMost=%1 uygulamasý kaldýrýldý.%n%nBazý bileþenler kaldýrýlamadý. Bu dosyalarý kendiniz silebilirsiniz.
UninstalledAndNeedsRestart=%1 uygulamasý kaldýrýldý. Bilgisayarý yeniden baþlatmalýsýnýz.%n%nÞimdi yeniden baþlatýlsýn mý?
UninstallDataCorrupted="%1" dosyasý bozuk. Kaldýrýlamadý

; *** Uninstallation phase messages
ConfirmDeleteSharedFileTitle=Paylaþýlan dosya kaldýrýlsýn mý?
ConfirmDeleteSharedFile2=Sistem paylaþýlan bazý dosyalarýn artýk hiçbir uygulama tarafýndan kullanýlmadýðýný belirledi. Bu paylaþýlan dosyalar silinsin mi?%n%n Bu dosyalar belirlenemeyen bazý uygulamalar tarafýndan kullanýlýyorsa ve silerseniz, söz konusu uygulamalar düzgün çalýþmayabilir. Ne yapacaðýnýzdan emin deðilseniz, Hayýr üzerine týklayýn. Bu dosyalarýn sisteminizde durmasýnýn herhangi bir zararý olmaz.
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
CreateDesktopIcon=Masaüstü simg&esi oluþturulsun
CreateQuickLaunchIcon=Hýzlý Baþlat simgesi &oluþturulsun
ProgramOnTheWeb=%1 Web Sitesi
UninstallProgram=%1 uygulamasýný kaldýr
LaunchProgram=%1 uygulamasýný çalýþtýr
AssocFileExtension=%2 dosya uzantýlarý %1 ile iliþkilendirilsin
AssocingFileExtension=%2 dosya uzantýlarý %1 ile iliþkilendiriliyor...

;Things we can also localize
CompanyName=Thingamahoochie Software

;Types
TypicalInstallation=Tipik Kurulum
FullInstallation=Tam Kurulum
CompactInstallation=Küçük Kurulum
CustomInstallation=Özel Kurulum

;Components
AppCoreFiles=WinMerge Çekirdek Dosyalarý
Filters=Süzgeçler
Plugins=Uygulama Ekleri
Frhed=Frhed(Özgür onaltýlýk düzenleyici)
WinIMerge=WinIMerge(Görsel Fark/Birleþtirme)
ArchiveSupport=Arþiv Desteði
ShellExtension32bit=32-bit WinMerge Kabuk Eklentisi
Patch=GnuWin32 Windows Yamasý

;Localization Components
Languages=Diller
BasqueLanguage=Baskça menü ve pencereler
BulgarianLanguage=Bulgarca menü ve pencereler
CatalanLanguage=Katalanca menü ve pencereler
ChineseSimplifiedLanguage=Çince (Basitleþtirilmiþ) menü ve pencereler
ChineseTraditionalLanguage=Çince (Geleneksel) menü ve pencereler
CroatianLanguage=Hýrvatça menü ve pencereler
CzechLanguage=Çekce menü ve pencereler
DanishLanguage=Danimarkaca menü ve pencereler
DutchLanguage=Hollandaca menü ve pencereler
FinnishLanguage=Fince menü ve pencereler
FrenchLanguage=Fransýzca menü ve pencereler
GalicianLanguage=Galiçce menü ve pencereler
GermanLanguage=Almanca menü ve pencereler
GreekLanguage=Yunanca menü ve pencereler
HungarianLanguage=Macarca menü ve pencereler
ItalianLanguage=Italyanca menü ve pencereler
JapaneseLanguage=Japonca menü ve pencereler
KoreanLanguage=Korece menü ve pencereler
NorwegianLanguage=Norveçce menü ve pencereler
PersianLanguage=Farsça menü ve pencereler
PolishLanguage=Lehçe menü ve pencereler
PortugueseBrazilLanguage=Portekizce (Brezilya) menü ve pencereler
PortugueseLanguage=Portekizce menü ve pencereler
RomanianLanguage=Romence menü ve pencereler
RussianLanguage=Rusça menü ve pencereler
SerbianLanguage=Sýrpça menü ve pencereler
SinhalaLanguage=Sinhalaca menü ve pencereler
SlovakLanguage=Slovakça menü ve pencereler
SlovenianLanguage=Slovence menü ve pencereler
SpanishLanguage=Ýspanyolca menü ve pencereler
SwedishLanguage=Ýsveçce menü ve pencereler
TurkishLanguage=Türkçe menü ve pencereler
UkrainianLanguage=Ukraynaca menü ve pencereler

;Tasks
ExplorerContextMenu=&Windows gezgini sað týk menü bütünleþtirmesi kullanýlsýn
IntegrateTortoiseCVS=&TortoiseCVS ile bütünleþtir
IntegrateTortoiseGIT=To&rtoiseGIT ile bütünleþtir
IntegrateTortoiseSVN=T&ortoiseSVN ile bütünleþtir
IntegrateClearCase=Rational &ClearCase ile bütünleþtir
AddToPath=Win&Merge klasörü sistem yoluna eklensin

; 3-way merge wizard page
ThreeWayMergeWizardPageCaption=3 Yollu Birleþtirme
ThreeWayMergeWizardPageDescription=WinMerge uygulamasýný TortoiseSVN/GIT için 3 yollu birleþtirme aracý olarak kullanýyor musunuz?
RegisterWinMergeAs3WayMergeTool=WinMerge 3 yollu birleþtirme aracý olarak kaydedilsin
MergeAtRightPane=Sað panoda birleþtirilsin
MergeAtCenterPane=Orta panoda birleþtirilsin
MergeAtLeftPane=Sol panoda birleþtirilsin
AutoMergeAtStartup=Açýlýþta otomatik birleþtirilsin

;Icon Labels
ReadMe=Beni Oku
UsersGuide=Kullanýcý Kitabý
ViewStartMenuFolder=&WinMerge Baþlangýç Menüsü Klasöründe Görüntülensin

;Code Dialogs
DeletePreviousStartMenu=Kurucu baþlangýç menüsünün konumunun "%s" yerine "%s" konumunda bulunduðunu algýladý. Önceki baþlangýç menüsü klasörünü silmek ister misiniz?

; Project file description
ProjectFileDesc=WinMerge Proje dosyasý