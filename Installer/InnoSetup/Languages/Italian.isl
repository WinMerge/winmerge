; *** Inno Setup versione 4.1.8+ lingua Italiana ***
;
; To download user-contributed translations of this file, go to:
;   http://www.jrsoftware.org/is3rdparty.php
;
; Note: When translating this text, do not add periods (.) to the end of
; messages that didn't have them already, because on those messages Inno
; Setup adds the periods automatically (appending a period would result in
; two periods being displayed).
;
; $jrsoftware: issrc/Files/Default.isl,v 1.53 2004/02/25 01:55:24 jr Exp $
; Italian.isl revisione 16 - 2004/02/26
;
; Tradotto da ale5000 - ale5000@tiscali.it - http://digilander.libero.it/kazaaita/
; Segnalatemi via e-mail eventuali errori o suggerimenti

[LangOptions]
LanguageName=Italiano
LanguageID=$0410
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
SetupAppTitle=Installazione
SetupWindowTitle=Installazione di %1
UninstallAppTitle=Disinstallazione
UninstallAppFullTitle=Disinstallazione di %1

; *** Misc. common
InformationTitle=Informazioni
ConfirmTitle=Conferma
ErrorTitle=Errore

; *** SetupLdr messages
SetupLdrStartupMessage=Questa è l'installazione di %1. Vuoi continuare?
LdrCannotCreateTemp=Impossibile creare i file temporanei. Installazione annullata
LdrCannotExecTemp=Impossibile eseguire i file nella cartella temporanea. Installazione annullata

; *** Startup error messages
LastErrorMessage=%1.%n%nErrore %2: %3
SetupFileMissing=File %1 non trovato nella cartella di installazione. Correggere il problema o richiedere una nuova copia del software.
SetupFileCorrupt=I file di installazione sono danneggiati. Richiedere una nuova copia del software.
SetupFileCorruptOrWrongVer=I file di installazione sono danneggiati, oppure sono incompatibili con questa versione del programma. Correggere il problema o richiedere una nuova copia del software.
NotOnThisPlatform=Questo programma non è compatibile con %1.
OnlyOnThisPlatform=Questo programma richiede per funzionare %1.
WinVersionTooLowError=Questo programma richiede %1 versione %2 o successiva.
WinVersionTooHighError=Questo programma non può essere installato su %1 versione %2 o successiva.
AdminPrivilegesRequired=Per installare questo programma si deve accedere al sistema come amministratore.
PowerUserPrivilegesRequired=Devi essere connesso come Amministratore per poter installare questo programma.
SetupAppRunningError=%1 è attualmente in esecuzione.%n%nChiudere adesso tutte le istanze del programma e poi premere OK, oppure premere Annulla per uscire.
UninstallAppRunningError=%1 è attualmente in esecuzione.%n%nChiudere adesso tutte le istanze del programma e poi premere OK, oppure premere Annulla per uscire.

; *** Misc. errors
ErrorCreatingDir=La cartella "%1" non può essere creata
ErrorTooManyFilesInDir=Impossibile creare i file nella cartella "%1" perchè contiene troppi file

; *** Setup common messages
ExitSetupTitle=Uscita dall'installazione
ExitSetupMessage=L'installazione non è completa. Uscendo dall'installazione in questo momento, il programma non sarà installato.%n%nÈ possibile eseguire l'installazione in un secondo tempo.%n%nUscire dall'installazione?
AboutSetupMenuItem=&Informazioni sull'installazione...
AboutSetupTitle=Informazioni sull'installazione
AboutSetupMessage=%1 versione %2%n%3%n%n%1 home page:%n%4
AboutSetupNote=

; *** Buttons
ButtonBack=< &Indietro
ButtonNext=&Avanti >
ButtonInstall=&Installa
ButtonOK=OK
ButtonCancel=Annulla
ButtonYes=&Si
ButtonYesToAll=Si a &Tutto
ButtonNo=&No
ButtonNoToAll=N&o a Tutto
ButtonFinish=&Fine
ButtonBrowse=&Sfoglia...
ButtonWizardBrowse=S&foglia...
ButtonNewFolder=&Crea nuova cartella

; *** "Select Language" dialog messages
SelectLanguageTitle=Selezionare la lingua dell'installazione
SelectLanguageLabel=Selezionare la lingua da utilizzare durante l'installazione:

; *** Common wizard text
ClickNext=Premere Avanti per continuare, o Annulla per uscire.
BeveledLabel=
BrowseDialogTitle=Sfoglia per cartelle
BrowseDialogLabel=Selezionare la cartella in questa lista, poi premere OK.
NewFolderName=Nuova cartella

; *** "Welcome" wizard page
WelcomeLabel1=Benvenuti nel programma di installazione di [name]
WelcomeLabel2=[name/ver] sarà installato sul computer.%n%nSi consiglia di chiudere tutte le applicazioni attive prima di procedere.

; *** "Password" wizard page
WizardPassword=Password
PasswordLabel1=Questa installazione è protetta da password.
PasswordLabel3=Inserire la password, poi premere Avanti per continuare. Le password distinguono maiuscole/minuscole.
PasswordEditLabel=&Password:
IncorrectPassword=La password inserita non è corretta, riprovare.

; *** "License Agreement" wizard page
WizardLicense=Contratto di Licenza
LicenseLabel=Leggere con attenzione le informazioni che seguono prima di procedere.
LicenseLabel3=Leggere il seguente contratto di licenza. È necessario accettare tutti i termini di questo accordo per procedere con l'installazione.
LicenseAccepted=&Accetto i termini del contratto di licenza 
LicenseNotAccepted=&Non accetto i termini del contratto di licenza

; *** "Information" wizard pages
WizardInfoBefore=Informazioni
InfoBeforeLabel=Leggere le importanti informazioni che seguono prima di procedere.
InfoBeforeClickLabel=Quando sei pronto per proseguire, premi Avanti.
WizardInfoAfter=Informazioni
InfoAfterLabel=Leggere le importanti informazioni che seguono prima di procedere.
InfoAfterClickLabel=Quando sei pronto per proseguire, premi Avanti.

; *** "User Information" wizard page
WizardUserInfo=Informazioni Utente
UserInfoDesc=Inserire le seguenti informazioni.
UserInfoName=&Nome:
UserInfoOrg=&Società:
UserInfoSerial=&Numero di Serie:
UserInfoNameRequired=E' necessario inserire un nome.

; *** "Select Destination Location" wizard page
WizardSelectDir=Selezione della cartella di destinazione
SelectDirDesc=Dove vuoi installare [name]?
SelectDirLabel3=[name] sarà installato nella seguente cartella.
SelectDirBrowseLabel=Per continuare, premere Avanti. Per selezionare un'altra cartella, premere Sfoglia.
DiskSpaceMBLabel=Il programma richiede almeno [mb] MB di spazio sul disco.
ToUNCPathname=Non è possiblie installare su un percorso UNC. Se stai installando attraverso una rete, devi connettere la risorsa come una unità di rete.
InvalidPath=Devi inserire un percorso completo di lettera di unità; per esempio:%n%nC:\APP%n%no un percorso UNC nella forma:%n%n\\server\share
InvalidDrive=L'unità o lo share UNC selezionato non esiste o non è accessibile. Selezionarne un'altro.
DiskSpaceWarningTitle=Spazio su disco insufficiente
DiskSpaceWarning=L'installazione richiede almeno %1 KB di spazio libero per installare, ma l'unità selezionata ha solo %2 KB disponibili.%n%nVuoi continuare comunque?
DirNameTooLong=Il nome della cartella o il percorso sono troppo lunghi.
InvalidDirName=Il nome della cartella non è valido.
BadDirName32=Il nome della cartella non può includere nessuno dei caratteri seguenti:%n%n%1
DirExistsTitle=Cartella già esistente
DirExists=La cartella:%n%n%1 esiste già.%n%nVuoi utilizzarla comunque?
DirDoesntExistTitle=Cartella inesistente
DirDoesntExist=La cartella:%n%n%1 non esiste.%n%nVuoi crearla?

; *** "Select Components" wizard page
WizardSelectComponents=Selezione componenti
SelectComponentsDesc=Quali componenti devono essere installati?
SelectComponentsLabel2=Selezionare i componenti da installare, deselezionare quelli che non vuoi installare. Premere Avanti per continuare.
FullInstallation=Installazione completa
; if possible don't translate 'Compact' as 'Minimal' (I mean 'Minimal' in your language)
CompactInstallation=Installazione compatta
CustomInstallation=Installazione personalizzata
NoUninstallWarningTitle=Componente esistente
NoUninstallWarning=L'installazione ha rilevato che i seguenti componenti sono già installati sul computer:%n%n%1%n%nDeselezionando questi componenti non verranno disinstallati.%n%nVuoi continuare comunque?
ComponentSize1=%1 KB
ComponentSize2=%1 MB
ComponentsDiskSpaceMBLabel=La selezione corrente richiede almeno [mb] MB di spazio su disco.

; *** "Select Additional Tasks" wizard page
WizardSelectTasks=Selezione processi addizionali
SelectTasksDesc=Quali processi addizionali vuoi avviare?
SelectTasksLabel2=Selezionare i processi addizionali da eseguire installando [name], poi premere Avanti.

; *** "Select Start Menu Folder" wizard page
WizardSelectProgramGroup=Selezione della cartella nel Menu Avvio/Start
SelectStartMenuFolderDesc=Dove vuoi che l'installazione inserisca i collegamenti al programma?
SelectStartMenuFolderLabel3=Saranno creati i collegamenti al programma nella seguente cartella del Menu Avvio/Start.
SelectStartMenuFolderBrowseLabel=Per continuare, premere Avanti. Per selezionare un'altra cartella, premere Sfoglia.
NoIconsCheck=&Non creare nessuna Icona
MustEnterGroupName=Devi inserire il nome della cartella.
GroupNameTooLong=Il nome della cartella o il percorso sono troppo lunghi.
InvalidGroupName=Il nome della cartella non è valido.
BadGroupName=Il nome della cartella non può includere nessuno dei caratteri seguenti:%n%n%1
NoProgramGroupCheck2=&Non creare una cartella nel Menu Avvio/Start

; *** "Ready to Install" wizard page
WizardReady=Pronto per l'installazione
ReadyLabel1=Il programma è pronto per iniziare l'installazione di [name] sul computer.
ReadyLabel2a=Premere Installa per continuare con l'installazione, o premere Indietro per rivedere o modificare le impostazioni.
ReadyLabel2b=Premere Installa per procedere con l'installazione.
ReadyMemoUserInfo=Informazioni Utente:
ReadyMemoDir=Cartella di destinazione:
ReadyMemoType=Tipo di installazione:
ReadyMemoComponents=Componenti selezionati:
ReadyMemoGroup=Cartella del menu Avvio/Start:
ReadyMemoTasks=Processi addizionali:

; *** "Preparing to Install" wizard page
WizardPreparing=Preparazione all'installazione
PreparingDesc=Preparazione all'installazione di [name] sul computer.
PreviousInstallNotCompleted=L'installazione/disinstallazione precedente del programma non è stata completata. È necessario riavviare il computer per completare l'installazione.%n%nAl successivo riavvio del sistema eseguire di nuovo l'installazione di [name].
CannotContinue=L'installazione non può continuare. Premere Annulla per uscire.

; *** "Installing" wizard page
WizardInstalling=Installazione in corso
InstallingLabel=Attendere il completamento dell'installazione di [name] sul computer.

; *** "Setup Completed" wizard page
FinishedHeadingLabel=Completamento dell'installazione guidata di [name]
FinishedLabelNoIcons=L'installazione di [name] è stata completata con successo.
FinishedLabel=L'installazione di [name] è stata completata con successo. L'applicazione può essere eseguita selezionando le icone installate.
ClickFinish=Premere Fine per uscire dall'installazione.
FinishedRestartLabel=Per completare l'installazione di [name], bisogna riavviare il computer. Vuoi riavviare ora?
FinishedRestartMessage=Per completare l'installazione di [name], bisogna riavviare il computer.%n%nVuoi riavviare ora?
ShowReadmeCheck=Sì, desidero vedere il file LEGGIMI adesso
YesRadio=&Si, riavvia il computer adesso
NoRadio=&No, riavvio il computer più tardi
; used for example as 'Run MyProg.exe'
RunEntryExec=Lancia %1
; used for example as 'View Readme.txt'
RunEntryShellExec=Visualizza %1

; *** "Setup Needs the Next Disk" stuff
ChangeDiskTitle=L'installazione necessita del disco successivo
SelectDiskLabel2=Inserire il disco %1 e premere OK.%n%nSe i file su questo disco possono essere trovati in una cartella diversa da quella visualizzata sotto, inserire il percorso corretto o premere Sfoglia.
PathLabel=&Percorso:
FileNotInDir2=Il file "%1" non può essere individuato  in "%2". Inserire il disco corretto o selezionare un'altra cartella.
SelectDirectoryLabel=Specificare il percorso del prossimo disco.

; *** Installation phase messages
SetupAborted=L'installazione non è stata completata.%n%nCorreggere il problema e rieseguire nuovamente l'installazione.
EntryAbortRetryIgnore=Premere Riprova per ritentare nuovamente, Ignora per procedere in ogni caso, o Interrompi per terminare l'installazione.

; *** Installation status messages
StatusCreateDirs=Creazione cartelle...
StatusExtractFiles=Estrazione files...
StatusCreateIcons=Creazione icone...
StatusCreateIniEntries=Creazione voci del file INI...
StatusCreateRegistryEntries=Creazione voci di Registro...
StatusRegisterFiles=Registrazione files...
StatusSavingUninstall=Salvataggio delle informazioni di disinstallazione...
StatusRunProgram=Termine dell'installazione...
StatusRollback=Recupero delle modifiche...

; *** Misc. errors
ErrorInternal2=Errore Interno %1
ErrorFunctionFailedNoCode=%1 fallito
ErrorFunctionFailed=%1 fallito; codice %2
ErrorFunctionFailedWithMessage=%1 fallito; codice %2.%n%3
ErrorExecutingProgram=Impossibile eseguire il file:%n%1

; *** Registry errors
ErrorRegOpenKey=Errore di apertura della chiave di Registro:%n%1\%2
ErrorRegCreateKey=Errore di creazione della chiave di Registro:%n%1\%2
ErrorRegWriteKey=Errore di scrittura della chiave di Registro:%n%1\%2

; *** INI errors
ErrorIniEntry=Errore nella creazione delle voci INI nel file "%1".

; *** File copying errors
FileAbortRetryIgnore=Premere Riprova per tentare di nuovo, Ignora per saltare questo file (sconsigliato), o Interrompi per terminare l'installazione.
FileAbortRetryIgnore2=Premere Riprova per tentare di nuovo, Ignora per proseguire comunque (sconsigliato), o Interrompi per terminare l'installazione.
SourceIsCorrupted=Il file sorgente è danneggiato
SourceDoesntExist=Il file sorgente "%1" non esiste
ExistingFileReadOnly=Il file esistente ha l'attributo di sola lettura.%n%nPremere Riprova per rimuovere l'attributo di sola lettura e ritentare, Ignora per saltare questo file, o Interrompi per terminare l'installazione.
ErrorReadingExistingDest=Si è verificato un errore durante la lettura del file esistente:
FileExists=Il file esiste già.%n%nDesiderate sovrascriverlo?
ExistingFileNewer=Il file esistente è più recente di quello che si stà installando. Si raccomanda di mantenere il file esistente.%n%nVuoi mantenere il file esistente?
ErrorChangingAttr=Si è verificato un errore durante il tentativo di modifica dell'attributo del file esistente:
ErrorCreatingTemp=Si è verificato un errore durante la creazione di un file nella cartella di destinazione:
ErrorReadingSource=Si è verificato un errore durante la lettura del file sorgente:
ErrorCopying=Si è verificato un errore durante la copia di un file:
ErrorReplacingExistingFile=Si è verificato un errore durante la sovrascrittura del file esistente:
ErrorRestartReplace=Errore durante Riavvio-Sostituzione:
ErrorRenamingTemp=Si è verificato un errore durante il tentativo di rinominare un file nella cartella di destinazione:
ErrorRegisterServer=Impossibile registrare la DLL/OCX: %1
ErrorRegisterServerMissingExport=DllRegisterServer esportazione mancante
ErrorRegisterTypeLib=Impossibile registrare il tipo di Libreria: %1

; *** Post-installation errors
ErrorOpeningReadme=Si è verificato un errore durante l'apertura del file LEGGIMI.
ErrorRestartingComputer=L'installazione non è riuscita a riavviare il computer. Riavviare manualmente.

; *** Uninstaller messages
UninstallNotFound=Il file "%1" non esiste. Impossibile disinstallare.
UninstallOpenError=Il file "%1" non può essere aperto. Impossibile disinstallare
UninstallUnsupportedVer=Il file log di disinstallazione "%1" è in un formato non riconosciuto da questa versione del programma. Impossibile disinstallare
UninstallUnknownEntry=Trovata una voce sconosciuta (%1) nel file di log della disinstallazione
ConfirmUninstall=Sei sicuro di voler rimuovere completamente %1 e tutti i suoi componenti?
OnlyAdminCanUninstall=Questa applicazione può essere disinstallata solo da un utente con privilegi di Amministratore.
UninstallStatusLabel=Attendere fino a che %1 è stato rimosso dal computer.
UninstalledAll=%1 è stato rimosso con successo dal computer.
UninstalledMost=%1 disinstallazione completata.%n%nAlcuni elementi non possono essere rimossi. Dovranno essere rimossi manualmente.
UninstalledAndNeedsRestart=Per completare la disinstallazione di %1, è necessario riavviare il sistema.%n%nVuoi riavviare adesso?
UninstallDataCorrupted="%1" file è danneggiato. Impossibile disinstallare

; *** Uninstallation phase messages
ConfirmDeleteSharedFileTitle=Rimuovere il file condiviso?
ConfirmDeleteSharedFile2=Il sistema indica che il seguente file condiviso non è più usato da nessun programma. Vuoi rimuovere questo file condiviso?%n%nSe qualche programma usasse questo file, potrebbe non funzionare più correttamente. Se non sei sicuro, scegli No. Lasciare il file nel sistema non può causare danni.
SharedFileNameLabel=Nome del file:
SharedFileLocationLabel=Posizione:
WizardUninstalling=Stato della disinstallazione
StatusUninstalling=Disinstallazione in corso di %1...

; The custom messages below aren't used by Setup itself, but if you make
; use of them in your scripts, you'll want to translate them.

[CustomMessages]

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
AppCoreFile=%1 Core Files
ApplicationRuntimes=Application Runtimes
UsersGuide=User's Guide
Filters=Filters
Plugins=Plugins (enhance core behavior)

;Localization Components
BulgarianLanguage=Bulgarian menus and dialogs
CatalanLanguage=Catalan menus and dialogs
ChineseSimplifiedLanguage=Chinese(Simplified) menus and dialogs
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
ExplorerContextMenu=&Enable Explorer context menu integration
IntegrateTortoiseCVS=Integrate with &TortoiseCVS
IntegrateDelphi4=Borland® Delphi &4 Binary File Support
UpdatingCommonControls=Updating the System's Common Controls

;Icon Labels
ReadMe=Read Me
UsersGuide=User's Guide
UpdatingCommonControls=Updating the System's Common Controls
ViewStartMenuFolder=&View the %1 Start Menu Folder