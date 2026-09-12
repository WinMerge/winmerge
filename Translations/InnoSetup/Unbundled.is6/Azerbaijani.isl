; *** İnno Tərtibat Yığıcı versiyası 6.4.0+ Azərbaycan Dili məlumatları ***
;Language            "Dil"                    ::::::    Azərbaycan Dili / Azerbaijani
;Translator Company  "Tərcümə Edən Şirkət"    ::::::    Azari Proqramlar Korporasiyası / Azari Programs Corporation
;Translate           "Tərcüməçi "             ::::::    Miryusif Rəhimov / Miryusif Rahimov
;E-mail              "Elektron Poçt"          ::::::    miryusifrahimov@gmail.com & azari.programs.corporation@gmail.com
;Web Site            "Veb Sayt"               ::::::    http://www.apk.az
;Facebook Page       "Facebook Səhifəsi"      ::::::    http://www.fb.com/apk.az
;Youtube Canal       "Youtube Kanalı"         ::::::    http://www.youtube.com/apk.az
;
; Bu faylın istifadəçi tərəfindən verilmiş tərcümələrini endirmək üçün bura keçin:
;   https://jrsoftware.org/files/istrans/
;
; Qeyd: Bu mətni tərcümə edərkən sonuna nöqtə (.) qoymayın
; onsuz da olmayan mesajlar, çünki o mesajlarda Inno
; Quraşdırma dövrləri avtomatik olaraq əlavə edir (nöqtə əlavə etməklə nəticələnir
; iki dövr göstərilir).

[LangOptions]
; Aşağıdakı üç giriş çox vacibdir. Mütləq oxuyun və
; yardım faylındakı '[LangOptions] bölməsi' mövzusunu anlayın.
LanguageName=Az<0259>rbaycan Dili
LanguageID=$042C
LanguageCodePage=0
; Tərcümə etdiyiniz dil xüsusi şrift üzləri tələb edirsə və ya
; ölçülər, aşağıdakı qeydlərdən hər hansı birini şərhdən çıxarın və müvafiq olaraq dəyişdirin.
;DialogFontName=
;DialogFontSize=8
;WelcomeFontName=Verdana
;WelcomeFontSize=12
;TitleFontName=Arial
;TitleFontSize=29
;CopyrightFontName=Arial
;CopyrightFontSize=8

[Messages]

; *** Tətbiq başlıqları
SetupAppTitle=Tərtibat köməkçisi
SetupWindowTitle=%1 - Tərtibat köməkçisi
UninstallAppTitle=Silmə köməkçisi
UninstallAppFullTitle=%1 silmə köməkçisi

; *** Ümumi müxtəlif sənədlər
InformationTitle=Məlumat
ConfirmTitle=Təsdiq et
ErrorTitle=Xəta

; *** Tərtibat Yükləyici məlumatları
SetupLdrStartupMessage=%1 tətbiqi yüklənəcək. Davam etmək istəyirsinizmi?
LdrCannotCreateTemp=Müvəqqəti fayllar yaradıla bilmədiyi üçün yükləmə ləğv edildi
LdrCannotExecTemp=Müvəqqəti qovluqdakı fayl başlıdılmadığı üçün, yükləmə ləğv edildi
HelpTextNote=

; *** Başlanğıc xəta məlumatları
LastErrorMessage=%1.%n%nXəta %2: %3
SetupFileMissing=Tərtibat qovluğunda %1 faylı əksikdi. Zəhmət olmasa problemi düzəldin və ya proqramın təzə bir kopiyası ilə yükləməyi yoxlayın.
SetupFileCorrupt=Tərtibat faylları xarabdır. Zəhmət olmasa proqramın təzə bir kopiyası ilə yükləməyi yoxlayın.
SetupFileCorruptOrWrongVer=Tərtibat faylları xarab olub və ya tərtibatın bu versiyası ilə uyğunlaşmır. Zəhmət olmasa problemi düzəldin və ya Proqramın təzə bir kopiyasıyla yükləməyi yoxlayın.
InvalidParameter=Əmr sətrində etibarsız bir parametr yazılıb:%n%n%1
SetupAlreadyRunning=Tərtibat köməkçisi onsuz da işləyir.
WindowsVersionNotSupported=Bu proqram kompüterinizdəki Windows versiyası ilə uyğun deyil.
WindowsServicePackRequired=Bu proqram, %1 xidmət paketi %2 və üstündəki versiyaları ilə işləyir.
NotOnThisPlatform=Bu proqram, %1 üstündə işləyə bilməz.
OnlyOnThisPlatform=Bu proqram, %1 üstündə işlədilməməlidir.
OnlyOnTheseArchitectures=Bu proqram, sadəcə bu prosessor arxitekturası üçün hazırlanmış Windows proqramları ilə işləyə bilər:%n%n%1
WinVersionTooLowError=Bu proqram üçün %1 versiyası %2 və ya daha yüksək versiyası olmalıdır.
WinVersionTooHighError=Bu proqram, '%1' versiyası '%2' və ya daha yüksək versiyasına yüklənə bilməz.
AdminPrivilegesRequired=Bu proqramı quraşdırmaq üçün İnzibatçı olaraq sistemə daxil olmalısınız.
PowerUserPrivilegesRequired=Bu proqramı quraşdırmaq üçün İnzibatçı və ya Güclü İdarəçi Qrupu üzvü olaraq daxil olmaq lazımdır.
SetupAppRunningError=Tərtibat köməkçisi %1 proqramının işlədiyini müəyyən etdi.%n%nZəhmət olmasa bu proqramın işləyən bütün hissələrini bağlayın, davam etmək üçün Təsdiq et düyməsinə və ya çıxmaq üçün Ləğv et düyməsinə basın.
UninstallAppRunningError=Tərtibat köməkçisi, %1 proqramının işlədiyini müəyyən etdi.%n%nZəhmət olmasa bu proqramın işləyən bütün hissələrini bağlayın, davam etmək üçün Təsdiq et düyməsinə və ya çıxmaq üçün Ləğv et düyməsinə basın.

; *** Başlanğıc sualları
PrivilegesRequiredOverrideTitle=Yükləmə Rejimi Seçin
PrivilegesRequiredOverrideInstruction=Yükləmə rejimi seçin
PrivilegesRequiredOverrideText1=%1 bütün istifadçilər üçün (inzibatçı icazəsi tələb olunur) və ya sadəcə sizin hesabınıza yüklənə bilər.
PrivilegesRequiredOverrideText2=%1 sadəcə sizin hesabınız üçün və ya bütün istifadəçilər üçün (inzibatçı icazəsi tələb olunur) yüklənə bilər.
PrivilegesRequiredOverrideAllUsers=&Bütün istifadəçilər üçün yüklənsin
PrivilegesRequiredOverrideAllUsersRecommended=&Bütün istifadəçilər üçün yüklənsin (tövsiyə olunur)
PrivilegesRequiredOverrideCurrentUser=&Sadəcə cari istifadəçi üçün yüklənsin
PrivilegesRequiredOverrideCurrentUserRecommended=&Sadəcə cari istifadəçi üçün yüklənsin (tövsiyə olunur)

; *** Müxtəlif xəta mətinləri
ErrorCreatingDir=Tərtibat köməkçisi "%1" qovluğunu yarada bilmədi.
ErrorTooManyFilesInDir="%1" qovluğunda bir fayl yaradıla bilmədi. Çünki qovluqda həddən çox fayl mövcuddur.

; *** Ümumi tərtibat məlumatları
ExitSetupTitle=Tərtibat köməkçisindən çıx
ExitSetupMessage=Yükləmə başa çatmadı. İndi çıxsanız tətbiq yüklənməyəcək.%n%Quraşdırmanı başa çatdırmaq üçün istədiyiniz zaman tərtibat köməkçisini təzədən başlada bilərsiniz.%n%nTərtibat köməkçisindən çıxmaq istəyirsinizmi?
AboutSetupMenuItem=Tərtibat h&aqqında...
AboutSetupTitle=Tərtibat haqqında
AboutSetupMessage=%1 %2 versiyası%n%3%n%n%1 veb səhifə:%n%4
AboutSetupNote=Azari Proqramlar Korporasiyası%nhttp://www.apk.az%nhttp://www.fb.com/apk.az%nhttp://www.youtube.com/apk.az%n%nElektron Poçt%nazari.programs.corporation@gmail.com %ninfo.apk.az@gmail.com %nmiryusifrahimov@gmail.com
TranslatorNote=İnno Tərtibatı Kompilyatoru (Inno Setup Compiler) - Azari Proqramlar Korporasiyası'nın yaradıcısı Miryusif Rəhimov tərəfindən Azərbaycan Dili'nə tərcümə edilmişdir. Azərbaycan'ın ən mükəmməl proqram təminat şirkətlərindən olan Azari Proqramlar Korporasiyası peşəkarlıqla Azərbaycan Dili'ndə proqramlar yaradır və yaradılmış digər proqramları Azərbaycan Dili'nə tərçümə edir.

; *** Düymələr
ButtonBack=< &Geri
ButtonNext=&İrəli >
ButtonInstall=&Quraşdır
ButtonOK=Təsdiq et
ButtonCancel=Çıxış
ButtonYes=&Bəli
ButtonYesToAll=&Hamısına Bəli
ButtonNo=&Xeyir
ButtonNoToAll=&Hamısına Xeyir
ButtonFinish=&Bağla
ButtonBrowse=&Bax...
ButtonWizardBrowse=&Bax...
ButtonNewFolder=&Təzə Qovluq Yarat

; *** "Quraşdırılma Dilini Seçin" səhifə mesajları
SelectLanguageTitle=Tərtibat Köməkçisi dilini seçin
SelectLanguageLabel=Quraşdırılma əsnasında istifadə ediləcək dili seçin.

; *** Ümumi mətnləri
ClickNext=Davam etmək üçün İrəli düyməsinə, çıxmaq üçün isə Çıxış düyməsinə  basın.
BeveledLabel=
BrowseDialogTitle=Qovluğa bax
BrowseDialogLabel=Siyahıdakı qovluqlardan birini seçin və Təsdiq et düyməsinə basın.
NewFolderName=Təzə Qovluq

; *** "Xoş Gəldiniz" tərtibat səhifəsi
WelcomeLabel1=[name] Tərtibat Köməkçisinə Xoş Gəldiniz.
WelcomeLabel2=Tərtibat köməkçisi [name/ver] proqramını kompüterinizə yükləyəcək.%n%nDavam etməzdən əvvəl, açıq digər bütün proqramlardan çıxmanızı tövsiyə edirik.

; *** "Şifrə" tərtibat səhifəsi
WizardPassword=Şifrə
PasswordLabel1=Bu Tərtibat şifrə tələb edir.
PasswordLabel3=Zəhmət olmasa şifrəni daxil edin və davam etmək üçün İrəli düyməsinə basın. Şifrələr böyük-kiçik hərfə həssasdır.
PasswordEditLabel=&Şifrə:
IncorrectPassword=Daxil etdiyiniz şifrə düz deyil. Zəhmət olmasa təkrar yoxlayın.

; *** "Lisenziya müqaviləsi" tərtibat səhifəsi
WizardLicense=Lisenziya müqaviləsi
LicenseLabel=Zəhmət olmasa bu müqaviləni oxuyub qəbul etdikdən sonra davam edin.
LicenseLabel3=Zəhmət olmasa Lisenziya müqaviləsini oxuyun. Müqavilədəki şərtləri diqqətlə oxumağınızı məsləhət görürük. Qəbul edirsinizsə İrəli düyməsinə, qəbul etmirsinizsə Çıxış düyməsinə basın.
LicenseAccepted=Oxuduğum lisenziya müqaviləsini &qəbul edirəm.
LicenseNotAccepted=Oxuduğum lisenziya müqaviləsini qəbul etmirəm.

; *** "Məlumatlar" tərtibat səhifəsi
WizardInfoBefore=Məlumatlar
InfoBeforeLabel=Zəhmət olmasa proqram ilə əlaqədar əhəmiyyətli məlumatları oxuduqdan sonra davam edin.
InfoBeforeClickLabel=Yükləməyə davam etmək üçün hazır olduğunuz zaman İrəli düyməsinə basın.
WizardInfoAfter=Məlumatlar
InfoAfterLabel=Zəhmət olmasa proqram ilə əlaqədar əhəmiyyətli məlumatları oxuduqdan sonra davam edin.
InfoAfterClickLabel=Yükləməyə davam etmək üçün hazır olduğunuz zaman İrəli düyməsinə basın.

; *** "İstifadəçi Məlumatları" tərtibat səhifəsi
WizardUserInfo=İstifadəçi Məlumatları
UserInfoDesc=Zəhmət olmasa məlumatlarınızı daxil edin.
UserInfoName=&İstifadəçi adı:
UserInfoOrg=&Təşkilat Adı:
UserInfoSerial=&Seriya nömrəsi:
UserInfoNameRequired=Bir ad yazmalısınız.

; *** "Yükləmə qovluğu seçin" tərtibat səhifəsi
WizardSelectDir=Yükləmə qovluğunu seçin
SelectDirDesc=[name] hara yüklənsin?
SelectDirLabel3=[name] seçilən qovluğa qurulacaq.
SelectDirBrowseLabel=Davam etmək üçün İrəli düyməsinə basın. Təyin edilən qovluğun xaricində qovluq təyin etmək üçün Bax düyməsinə basın.
DiskSpaceGBLabel=Ən az [gb] GB boş sahə lazımdır.
DiskSpaceMBLabel=Ən az [mb] MB boş sahə lazımdır.
CannotInstallToNetworkDrive=Tətbiq bir şəbəkə diskinə quraşdırıla bilməz.
CannotInstallToUNCPath=Tərtibat UNC mövqe mövqeyinə (\\mövqey kimi) quraşdırıla bilməz. Əgər bu quraşdırmanı şəbəkə üzərindən edirsinizsə zəhmət olmasa şəbəkə hesabınızı daxil edin.
InvalidPath=Disk adıyla qovluğun tam mövqeyini yazmalısınız. Misal: %n%nC:\APP%n%n və ya bu şəkildə bir UNC mövqeyini:%n%n\\server\paylaşma
InvalidDrive=Disk və ya UNC paylaşımı mövcud deyil və ya əlçatmazdır. Zəhmət olmasa başqa birini seçin.
DiskSpaceWarningTitle=Kifayət qədər disk sahəsi mövcud deyil
DiskSpaceWarning=Yükləmək üçün %1 KB boş yer tələb olunur, ancaq seçilmiş diskdə sadəcə %2 KB boş yer var.%n%nYenə də davam etmək istəyirsinizmi?
DirNameTooLong=Qovluq adı və ya mövqeyi çox uzundur.
InvalidDirName=Qovluq adı etibarsızdır.
BadDirName32=Qovluq adlarında bu simvollardan hər hansı biri istifadə edə bilməz:%n%n%1
DirExistsTitle=Qovluq mövcuddur
DirExists=Qovluq:%n%n%1%n%nmövcuddur. Quraşdırmaq üçün bu qovluğu istifadə etmək istəyirsiniz mi?
DirDoesntExistTitle=Qovluq tapılmadı
DirDoesntExist=Qovluq:%n%n%1%n%tapılmadı. Qovluğu yaratmaq istəyirsinizmi?

; *** "Komponentləri seçin" tərtibat səhifəsi
WizardSelectComponents=Komponentləri seçin
SelectComponentsDesc=Hansı komponentləri yükləməyə daxil etmək istəyirsiniz?
SelectComponentsLabel2=Yükləmək istədiyiniz komponentləri seçin; yükləmək istəmədiyiniz komponentlərin işarəsini passiv edin. Seçiminizi edib İrəli düyməsinə basın.
FullInstallation=Tam yükləmə
; Mümkünsə, 'Kompakt' sözünü dilinizdə 'Minimal' mənasına tərcümə etməyin.
CompactInstallation=Normal yükləmə
CustomInstallation=Xüsusi yükləmə
NoUninstallWarningTitle=Komponentlər mövcuddur
NoUninstallWarning=Tərtibat bu komponentlərin kompüterinizdə yüklənmiş olduğunu müəyyən etdi:%n%n%1%n%n Bu komponentlərin işarələrinin passiv edilməsi mövcud olan komponentləri silməz.%n%nYenə də de davam etmək istəyirsiniz mi?
ComponentSize1=%1 KB
ComponentSize2=%1 MB
ComponentsDiskSpaceGBLabel=Seçdiyiniz komponentlər üçün diskdə ən az [gb] GB boş sahə lazımdır.
ComponentsDiskSpaceMBLabel=Seçdiyiniz komponentlər üçün diskdə ən az [mb] MB boş sahə lazımdır.

; *** "Əlavə tapşırıqları seçin" tərtibat səhifəsi
WizardSelectTasks=Əlavə tapşırıqları seçin
SelectTasksDesc=Başqa hansı tapşırıqlar görülməlidir?
SelectTasksLabel2=[name] yükləmə zamanı yerinə yetirmək istədiyiniz əlavə tapşırıqları seçin və İrəli düyməsini basın.

; *** "Başlat menyusu qovluğunu seçin" tərtibat səhifəsi
WizardSelectProgramGroup=Başlat menyusu qovluğunu seçin
SelectStartMenuFolderDesc=Tətbiq qısayollarını hara əlvə edilsin?
SelectStartMenuFolderLabel3=Tərtibat tətbiq qısayollarını, şirkətinin veb saytını, silmə proqramını və digər qısayolları aşağıdakı Başlat Menyusu qovluğuna əlvə edəcək.
SelectStartMenuFolderBrowseLabel=Davam etmək üçün İrəli düyməsinə, fərqli qovluq seçmək üçün isə Bax düyməsinə basın.
MustEnterGroupName=Bir qovluq adı yazmalısınız.
GroupNameTooLong=Qovluq adı və ya mövqeyi çox uzundur.
InvalidGroupName=Qovluq adı etibarsızdı.
BadGroupName=Qovluq adında bu simvollar istifadə edəilə bilməz:%n%n%1
NoProgramGroupCheck2=&Başlat menyusu qovluğu yaradılmasın

; *** "Yüklənməyə hazırdı" tərtibat səhifəsi
WizardReady=Yüklənməyə hazırdı
ReadyLabel1=[name] kompüterinizə yüklənməyə hazırdı.
ReadyLabel2a=Yükləməyə davam etmək üçün İrəli düyməsinə, dəyişdirmək üçün isə Geri düyməsinə basın.
ReadyLabel2b=Yükləməyə  başlamaq üçün İrəli düyməsinə basın.
ReadyMemoUserInfo=İstifadəçi məlumatları:
ReadyMemoDir=Yükləmə mövqeyi:
ReadyMemoType=Yükləmə növü:
ReadyMemoComponents=Seçilmiş komponentlər:
ReadyMemoGroup=Başlat menyusu qovluğu:
ReadyMemoTasks=Əlavə tapşırıqlar:

; *** "Əlavə fayıllar" tərtibat səhifəsi
DownloadingLabel=Əlavə fayıllar yüklənir...
ButtonStopDownload=Yükləməni &dayandır
StopDownload=Yükləməni dayandırmaq istədiyinizə əminsiniz?
ErrorDownloadAborted=Yükləmə dayandırıldı
ErrorDownloadFailed=Yüklənmədi: %1 %2
ErrorDownloadSizeFailed=Ölçü ala bilmədi: %1 %2
ErrorFileHash1=Fayl heşinin yoxlanılması uğursuz oldu: %1
ErrorFileHash2=Fayl heşi etibarsızdır: %1 olmalıdı %2
ErrorProgress=Etibarsızdır: %1 / %2
ErrorFileSize=Fayl ölçüsü etibarsızdır: %1 olmalıdı %2

; *** "7Zip arxiv faylların çıxarılması" tərtibat səhifəsi
ExtractionLabel=7Zip arxiv faylların çıxarılması...
ButtonStopExtraction=Çıxarmağı &dayandır
StopExtraction=Çıxarmağı dayandır istədiyinizə əminsiniz?
ErrorExtractionAborted=Çıxarma dayandırıldı
ErrorExtractionFailed=Çıxarılmadı: %1

; *** "Quraşdırılma hazırlanır" tərtibat səhifəsi
WizardPreparing=Quraşdırılma hazırlanır
PreparingDesc=[name] kompüterinizə yüklənməyə hazırlanır.
PreviousInstallNotCompleted=Əvvəlki Yükləmə və ya Silmə əməliyyatı başa çatmayıb. Bu Yükləmə və ya Silmə əməliyyatının başa çatması üçün kompüterinizi təzədən başlatmalısınız.%n%nProsesi başa çatdırmaq üçün kompüterinizi təzədən başlatdıqdan sonra [name] tərtibat köməkçisini təzədən açın.
CannotContinue=Yükləmə uğursuz oldu. Çıxmaq üçün Ləğv et düyməsinə basıb çıxın.
ApplicationsFound=Yükləmə köməkçisi tərəfindən yenilənməsi lazım olan fayllar, bu proqramlar tərəfindən istifadə olunur. Yükləmə köməkçisinə bu proqramları avtomatik bağlamağa icazə verməyiniz tövsiyə olunur.
ApplicationsFound2=Yükləmə köməkçisi tərəfindən yenilənməsi lazım olan fayllar, bu proqramlar tərəfindən istifadə olunur. Yükləmə köməkçisinə bu proqramları avtomatik bağlamağa icazə verməyiniz tövsiyə olunur. Yükləmə başa çatdıqdan sonra proqramları təzədən başlatmağa çalışacaq.
CloseApplications=&Tətbiqlər bağlansın
DontCloseApplications=Tətbiqlər &bağlanmasın
ErrorCloseApplications=Tərtibat köməkçisi tətbiqləri bağlaya bilmədi. Tərtibat köməkçisi tərəfindən yenilənməli olan fayılları istifadə edən tətbiləri əl ilə bağlamağınız məsləhətdir.
PrepareToInstallNeedsRestart=Yükləmə üçün kompüter təzədən başladılmalıdır. Kompüteri təzədən başlatdıqdan sonra [name] Yükləməni başa çatdırmaq üçün yükləmə köməkçisini təzədən başladın..%n%nİndi kompüteri təzədən başlatmaq istəyirsiniz mi?

; *** "Quraşdırılır" tərtibat səhifəsi
WizardInstalling=Quraşdırılır
InstallingLabel=Zəhmət olmasa [name] kompüterinizdə yüklənənə qədər gözləyin.

; *** "Qurşdırılma Başa Çatdı" tərtibat səhifəsi
FinishedHeadingLabel=[name] yükləmə əməliyyatını başa çatdırdı.
FinishedLabelNoIcons=Tərtibat [name] yükləməni başa çatdırdı.
FinishedLabel=Kompüterinizə [name] yükləməni başa çatdırıldı. Nişanları yükləməni seçtinizsə, nişanlara basaraq proqramı başlada bilərsiniz.
ClickFinish=Tərtibat'dan çıxmaq üçün "Son" düyməsinə basın.
FinishedRestartLabel=[name] tətbiqinin tam yüklənməsi üçün, Tərtibat kompüterinizi təzədən başlatmalıdır. Kompüteriniz təzədən başladılsın mı?
FinishedRestartMessage=[name] tətbiqinin tam yüklənməsi üçün, kompüterinizi təzədən başlatmaq lazımdır.%n%nİndi kompüteriniz təzədən başladılsın mı?
ShowReadmeCheck=Bəli, Məni Oxu faylını göstərin
YesRadio=&Bəli, kompüter indi təzədən başladılsın
NoRadio=&Xeyir, kompüteri daha sonra təzədən başladaram
; məsələn, "MyProg.exe-ni işə salın" kimi istifadə olunur
RunEntryExec=%1 tətbiqini başlat
; məsələn, 'Readme.txt'ə bax' kimi istifadə olunur
RunEntryShellExec=%1 faylını göstər

; *** "Yükləmə üçün növbəti disk lazımdır" ismarıclar
ChangeDiskTitle=Yükləmə köməkçisinə növbəti disk lazımdır
SelectDiskLabel2=Zəhmət olmasa %1 diski taxın və Təsdiq et düyməsinə basın.%n%nDiskdəki fayllar aşağıdakılardan başqa bir qovluqda yerləşirsə, düzgün mövqeyi yazın və ya düzgün qovluğu seçmək üçün "Bax" düyməsinə basın.
PathLabel=&Mövqey:
FileNotInDir2="%1" faylı "%2" mövqesində tapılmadı. Zəhmət olmasa düzgün diski taxın və ya fərqli qovluğu seçin.
SelectDirectoryLabel=Zəhmət olmasa sonrakı diskin mövqeyini təyin edin.

; *** Yükləmə mərhələsi məlumatları
SetupAborted=Yükləmə başa çatdırılmadı.%n%nZəhmət olmasa problemi həll edərək yükləmə köməkçisini təkrar yoxlayın.
AbortRetryIgnoreSelectAction=Edəcəyiniz əməliyyatı seçin
AbortRetryIgnoreRetry=&Təkrar yoxla
AbortRetryIgnoreIgnore=&Problemə məhəl qoymayın və davam edin.
AbortRetryIgnoreCancel=Yükləməni ləğv edin

; *** Yükləmə vəziyyəti məlumatları
StatusClosingApplications=Tətbiqlər bağlanır...
StatusCreateDirs=Qovluqlar yaradılır...
StatusExtractFiles=Fayllar ayırd edilir...
StatusCreateIcons=Simvollar yaradılır...
StatusCreateIniEntries=INI fayl məlumatları yaradılır...
StatusCreateRegistryEntries=Qeyd dəftəri qeydləri yaradılır...
StatusRegisterFiles=Fayllar yaradılır...
StatusSavingUninstall=Silmə məlumatları yaradılır...
StatusRunProgram=Yükləmə başa çatdırılır...
StatusRestartingApplications=Tətbiqlər təzədən başladılır...
StatusRollback=Dəyişikliklər geri yüklənilir...

; *** Müxtəlif xəta məlumatları
ErrorInternal2=Daxili xəta: %1
ErrorFunctionFailedNoCode=%1 başa çatdırılmadı.
ErrorFunctionFailed=%1 başa çatdırılmadı; kod %2
ErrorFunctionFailedWithMessage=%1 başa çatdırılmadı; kod %2.%n%3
ErrorExecutingProgram=Bu fayl işləmədi:%n%1

; *** Qeydiyyat dəftəri xətaları
ErrorRegOpenKey=eydiyyat dəftəri açarları açılanda bu xəta yarandı:%n%1%2
ErrorRegCreateKey=Qeydiyyat dəftəri açarları yüklənəndə bu xəta yarandı:%n%1%2
ErrorRegWriteKey=Qeydiyyat dəftəri açarları yazılanda bu xəta yarandı:%n%1%2

; *** INI xətaları
ErrorIniEntry="%1" fayılına INI məlumatları yüklənəndə xəta yarandı.

; *** Fayl köçürmə xətaları
FileAbortRetryIgnoreSkipNotRecommended=&Bu dosya atlansın (tövsiyə edilmir)
FileAbortRetryIgnoreIgnoreNotRecommended=&Problem olmadığını bildirib davam et (tövsiyə edilmir)
SourceIsCorrupted=Mənbəə faylı xarabdır
SourceDoesntExist="%1" Mənbəə faylı tapılmadı
ExistingFileReadOnly2=Mövcud faylın üstünə yazıla bilmədi, çünki sadəcə oxunan kimi qeyd olunub.
ExistingFileReadOnlyRetry=&Sadəcə oxunan xüsusiyyətini ləğv edib təzədən yoxlansın
ExistingFileReadOnlyKeepExisting=&Mövcud fayl qorunsun
ErrorReadingExistingDest=Mövcud faylı oxuyanda bir problem yarandı.
FileExistsSelectAction=Görüləcək prosesi seçin
FileExists2=Fayl onsuz da mövcuddur.
FileExistsOverwriteExisting=&Mövcud faylın üstünə yazın
FileExistsKeepExisting=Mövcud &faylın qorunsun
FileExistsOverwriteOrKeepAll=&Sonrakı münaqişələrdə bu proses aparılsın.
ExistingFileNewerSelectAction=Görüləcək prosesi seçin
ExistingFileNewer2=Mövcud fayl yükləmə köməkçisinin yazmağa çalışdığı fayldan daha təzədir.
ExistingFileNewerOverwriteExisting=&Var olan faylın üstünə yazılsın
ExistingFileNewerKeepExisting=Mövcud &fayl qorunsun (tövsiyə edilir)
ExistingFileNewerOverwriteOrKeepAll=&Sonrakı münaqişələrdə bu proses aparılsın.
ErrorChangingAttr=Mövcud faylın atributlarını dəyişdirərkən problem baş verdi:
ErrorCreatingTemp=Hədəf mövqeydə fayl yaradılanda bir xəta yarandı:
ErrorReadingSource=Mənbəə fayl oxunanda bir xəta yarandı:
ErrorCopying=Faylın kopyalananda problem yarandı:
ErrorReplacingExistingFile=Var olan dosya değiştirilirken sorun çıktı:
ErrorRestartReplace=Təzədən başladıqda üstünə yazmaq alınmadı:
ErrorRenamingTemp=Hədəf mövqesində olan faylın adı dəyişdiriləndə bir xəta yarandı:
ErrorRegisterServer=DLL/OCX yazıla bilmədi: %1
ErrorRegSvr32Failed=RegSvr32 əməliyyatı bu kod ilə başa çatdırılmadı: %1
ErrorRegisterTypeLib=Reyestrə tip kitabxanası əlavə etmək mümkün deyil: %1

; *** Silmə mərhələsində göstəriləcək ad işarələri
; used for example as 'My Program (32-bit)'
UninstallDisplayNameMark=%1 (%2)
; used for example as 'My Program (32-bit, All users)'
UninstallDisplayNameMarks=%1 (%2, %3)
UninstallDisplayNameMark32Bit=32 bit
UninstallDisplayNameMark64Bit=64 bit
UninstallDisplayNameMarkAllUsers=Bütün istifadəçilər
UninstallDisplayNameMarkCurrentUser=Cari istifadəçilər

; *** Yükləmə sonrası xətalar
ErrorOpeningReadme="Oxu məni" faylını açanda problem yarandı.
ErrorRestartingComputer=Yükləmə köməkçisi kompüterinizi təzədən başlada bilmir. Zəhmət olmasa kompüterinizi təzədən başladın.

; *** Silmə köməkçisi məlumatları
UninstallNotFound="%1" faylı tapılmadı. Tətbiq silinmir.
UninstallOpenError="%1" faylı açılmadı. Tətbiq silinmir.
UninstallUnsupportedVer="%1" tətbiqinin silinməsi jurnalının formatı silmə köməkçisinin bu versiyası tərəfindən başa düşülmür. Tətbiq silinmir.
UninstallUnknownEntry=Silmə məlumat faylında bilinməyən (%1) adlı sətir var.
ConfirmUninstall=%1 tətbiqini bütün komponentləri ilə silmək istədiyinizdən əminsinizmi?
UninstallOnlyOnWin64=Bu tətbiq sadəcə 64-bit Windows'lardan silinə bilər.
OnlyAdminCanUninstall=Bu tətbiq sadəcə inzibatçı səlahiyyətinə sahib istifadəçilər tərəfindən silinə bilər.
UninstallStatusLabel=Zəhmət olmasa %1 tətbiqi kompüterinizdən silərkən gözləyin...
UninstalledAll=%1 tətbiqi kompüterinizdən tamamilə silindi.
UninstalledMost=%1 tətbiqinin silinmə əməliyyatı başa çatdırıldı.%n%nBəzi komponentlər silinmədi. Bu faylları özünüz silə bilərsiniz.
UninstalledAndNeedsRestart=%1 tətbiqinin silinməsinin başa çatdırılması üçün, Kompüterinizi təzədən başlatmalısınız.%n%nİndi təzədən başladılsın mı?
UninstallDataCorrupted="%1" faylı xarabdır. Silinmir..

; *** Silmə mərhələsi məlumatları
ConfirmDeleteSharedFileTitle=Paylaşılan fayl silinsin mi?
ConfirmDeleteSharedFile2=Sistemə görə, paylaşılan bu fayl başqa bir tətbiq tərəfindən istifadə edilmir və silinə bilər. Bu paylaşılmış faylı silmək istəyirsiniz mi?%n%nBu fayl, başqa hər hansı bir tətbiq tərəfindən istifadə edilirsə, silindiyində digər tətbiq düzgün işləməyə bilər. Əmin deyilsinizsə İmtina et düyməsinə basın. Faylı sisteminizdə saxlamağın bir zərəri olmaz.
SharedFileNameLabel=Fayl adı:
SharedFileLocationLabel=Mövqey:
WizardUninstalling=Silinmə vəziyyəti
StatusUninstalling=%1 silinir...

; *** Bağlamağa maneə olma səbəbləri
ShutdownBlockReasonInstallingApp=%1 quraşdırılır.
ShutdownBlockReasonUninstallingApp=%1 silinir.

; The custom messages below aren't used by Setup itself, but if you make
; use of them in your scripts, you'll want to translate them.

[CustomMessages]

NameAndVersion=%1 %2 versiya
AdditionalIcons=Əlavə simvollar:
CreateDesktopIcon=İş stolu &simvolu yaradılsın
CreateQuickLaunchIcon=Başlat menyusu &simvolu yaradılsın
ProgramOnTheWeb=%1 veb saytı
UninstallProgram=%1 tətbiqini sil
LaunchProgram=%1 tətbiqini işlət
AssocFileExtension=%1 &tətbiqi ilə %2 fayl davamı əlaqələndirilsin
AssocingFileExtension=%1 tətbiqi ilə %2 fayl davamı əlaqələndirilir...
AutoStartProgramGroupDescription=Başlanğıc:
AutoStartProgram=%1 avtomatik olaraq başladılsın
AddonHostProgramNotFound=%1 seçtiyiniz qovluq tapılmadı.%n%nYenə də davam etmək istəyirsiniz mi?
