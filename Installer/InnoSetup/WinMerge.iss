;      Original File Name:  WinMerge.iss
;           File Revision:  18
;           Revision Date:  2004/03/10 16:51
;           Programmed by:  Christian Blackburn and ?
;                 Purpose:  The is the Inno Setup installation script for distributing our WinmMerge application.
; Tools Needed To Compile:  Inno Setup 4.18+ (http://www.jrsoftware.org/isdl.php), Inno Setup QuickStart Pack 4.18+(http://www.jrsoftware.org/isdl.php)
;                           note: the versions of Inno Setup and the QuickStart Pack should be identical to ensure proper function
;Directly Dependant Files:  Because this is an installer. It would be difficult to list and maintain each of the files referenced
;                           throughout the script in the header.  If you search this plain text script for a particular file in our CVS and it
;                           doesn't appear then this script is not directly dependant on that file.
;Compilation Instructions:  1.  MANUALLY combine the textual contents of \Docs\Read Me.rtf and \Docs\Contributors.rtf
;                               making an updated \InnoSetup\Info.rtf
;                           2.  Open this file in Inno Setup or ISTool

;Installer Todo List:
;  1. Determine the minimum allowable version Comctl32.dll.  If they don't have it distribute IE 6.01 on those platforms.
;     We'll actually need to determine the user's locale and download the correct version of IE for their platform (Yikes!, maybe we'll only
;     Distribute IE for those lucky enough to speak English :)
;     Note the following file could be downloaded this would list the locales and their download paths:
;     http://www.microsoft.com/windows/ie/downloads/critical/ie6sp1/default.asp
;     inside of this file lines such as can be found:
;     <option value="http://download.microsoft.com/download/ie6sp1/finrel/6_sp1/W98NT42KMeXP/da/ie6setup.exe|da">Danish</option>
;     this could be converted to two valuable bits of info:
;     URL: http://download.microsoft.com/download/ie6sp1/finrel/6_sp1/W98NT42KMeXP/da/ie6setup.exe
;     Locale: da
;     Note this seemingly awesome feature will break eventually (MS does change their website regularly) so we'll need to have some
;     damage control built in.
;  2. Determine if our application really needs 7-Zip installed to function.  If it does then we should probably just bundle 7-Zip or provide
;      integrated download on the fly support for it.
;  3. Automatically detect the user's locale, select, and install that language the and the registry settings that would make that language be
;     the language on startup.
;  4. Add WinMerge is running would you like to close it now support with programmatic termination [ISX]
;  5. Add WinMerge to the user's path so they can execute comparison's from a Dos Prompt (Cmd.exe/Command.exe)
;  6. Make an uninstall icon and use it: UninstallIconFile=..\src\res\Merge.ico
;  7. Make a Floppy Disk /Low Bandwidth Edition of the WinMerge Installer that doesn't include outdated (3.11, 3.12) 7-Zip Support or the Language files
;     If the user requires any of these we'll download it on the fly.  (maybe that should be the default behavior from the get go?)
;  8. We need to look in the registry and or inno setup log file and actually determine what the previous start
;     menu location was so that we can remove it if the user chooses to install to a different location.
;  9. Determine whether NT 3.51 with a 3.0 or higher version of IE can run our application I don't want the system requirements
;     in \Docs\Read Me.RTF to be inaccurate.
; 10. Create two info pages during installation one for our Contributors and a second one for our Read Me file.
;     If this isn't possible then we'll need to use ISPP and somehow programatically combine the two RTF files prior to compilation.
; 11. Copy our Read Me file to the intallation directory and include it in the start menu.
; 12. Open the start menu group that the user installed to automatically towards the end of the installation.
; 13. Make the Install7ZipDll() Function automatically work with future versions of Merge7zDLL

#define AppVersion GetFileVersion(AddBackSlash(SourcePath) + "..\Build\MergeRelease\WinMerge.exe")

[Setup]
AppName=WinMerge
AppVerName=WinMerge {#AppVersion}
AppPublisher=Thingamahoochie Software
AppPublisherURL=http://winmerge.org/
AppSupportURL=http://winmerge.org/
AppUpdatesURL=http://winmerge.org/

;This is in case an older version of the installer happened to be
DirExistsWarning=No

;This requires IS Pack 4.18(full install).  Once installed you must compile using ISTool, not Inno Setup directly.
AppVersion={#AppVersion}

DefaultDirName={pf}\WinMerge
DefaultGroupName=WinMerge
DisableStartupPrompt=yes
AllowNoIcons=yes
LicenseFile=..\src\COPYING
InfoBeforeFile=Info.rtf
OutputBaseFilename=WinMerge {#AppVersion}
PrivilegesRequired=poweruser
UninstallDisplayIcon={app}\{code:ExeName}

;File Version Info
VersionInfoVersion={#AppVersion}

;Artwork References
WizardImageFile=Art\Large Logo.rle
WizardSmallImageFile=Art\Small Logo.rle
WizardImageStretch=No

SetupIconFile=..\src\res\Merge.ico

;The uninstall icon shouldn't match the WinMerge icon, because it would look confusing in the start menu.
;  So I've remmed this until someone (probably me [Seier Blackburn]) creates a decent WinMerge specific uninstall icon
;UninstallIconFile=..\src\res\Merge.ico

;Compression Parameters
;Please note while Compression=lzma/ultra and InternalCompressLevel=Ultra are better than max
;they also require 320 MB RAM for compression, if you're system has more than than in RAM then by all
;means set it to ultra before compilation
Compression=lzma/ultra
InternalCompressLevel=ultra
SolidCompression=true


[Tasks]
Name: desktopicon; Description: Create a &desktop icon; GroupDescription: Additional icons:; Flags: unchecked
Name: quicklaunchicon; Description: Create a &Quick Launch icon; GroupDescription: Additional icons:


[Components]
Name: main; Description: WinMerge Core Files; Types: full compact custom; Flags: fixed
Name: docs; Description: User's Guide; Types: full
Name: filters; Description: Filters; Types: full

;Languages are no longer a default part of a normal installation.  Users that had chosen a foreign
;language in the past will still have one now though
Name: brazilian; Description: Portuguese (Brazilian) menus and dialogs; Flags: disablenouninstallwarning
Name: chinesesimplifiedlanguage; Description: Chinese (simplified) menus and dialogs; Flags: disablenouninstallwarning
Name: chinesetraditionallanguage; Description: Chinese (traditional) menus and dialogs; Flags: disablenouninstallwarning
Name: czechlanguage; Description: Czech menus and dialogs; Flags: disablenouninstallwarning
Name: danishlanguage; Description: Danish menus and dialogs; Flags: disablenouninstallwarning
Name: dutchlanguage; Description: Dutch menus and dialogs; Flags: disablenouninstallwarning
Name: frenchlanguage; Description: French menus and dialogs; Flags: disablenouninstallwarning
Name: germanlanguage; Description: German menus and dialogs; Flags: disablenouninstallwarning
Name: italianlanguage; Description: Italian menus and dialogs; Flags: disablenouninstallwarning
Name: koreanlanguage; Description: Korean menus and dialogs; Flags: disablenouninstallwarning
Name: slovaklanguage; Description: Slovak menus and dialogs; Flags: disablenouninstallwarning
Name: spanishlanguage; Description: Spanish menus and dialogs; Flags: disablenouninstallwarning


[InstallDelete]
; Diff.txt is a file left over from previous versions of WinMerge (before version 2.0), we just delete it to be nice.
Type: files; Name: {app}\Diff.txt

; A few users might have some intermediate Chinese translations on their machines (from version 2.0.0.2),
;we just delete those to be nice.
Type: files; Name: {app}\MergeChineseSimplifiedGB2312.lang
Type: files; Name: {app}\MergeChineseTraditionalBIG5.lang

;All of these files are removed so if the user upgrades their operating system or changes their language selections only the
;necessary files will be left in the installation folder
;Another reason these files might be strays is if a user extracted one of the experimental builds such as:
;WinMerge.{#AppVersion}-exe.7z.
Name: {app}\Merge7z311.dll; Type: files
Name: {app}\Merge7z311U.dll; Type: files
Name: {app}\Merge7z312.dll; Type: files
Name: {app}\Merge7z312U.dll; Type: files
Name: {app}\Merge7z313.dll; Type: files
Name: {app}\Merge7z313U.dll; Type: files
Name: {app}\MergeBrazilian.lang; Type: files
Name: {app}\MergeChineseSimplified.lang; Type: files
Name: {app}\MergeChineseTraditional.lang; Type: files
Name: {app}\MergeCzech.lang; Type: files
Name: {app}\MergeDanish.lang; Type: files
Name: {app}\MergeDutch.lang; Type: files
Name: {app}\MergeFrench.lang; Type: files
Name: {app}\MergeGerman.lang; Type: files
Name: {app}\MergeItalian.lang; Type: files
Name: {app}\MergeKorean.lang; Type: files
Name: {app}\MergeSlovak.lang; Type: files
Name: {app}\MergeSpanish.lang; Type: files
Name: {app}\WinMerge.exe; Type: files
Name: {app}\WinMergeU.exe; Type: files

;Removes the previous start menu items and group in case the user chooses to install to a new start menu location next time
Name: {commonstartmenu}\Programs\WinMerge\WinMerge.lnk; Type: files
Name: {commonstartmenu}\Programs\WinMerge\Uninstall WinMerge.lnk; Type: files
Name: {commonstartmenu}\Programs\WinMerge\WinMerge on the Web.lnk; Type: files
Name: {commonstartmenu}\Programs\WinMerge\Read Me.lnk; Type: files
Name: {commonstartmenu}\Programs\WinMerge; Type: dirifempty

;This removes the quick launch icon in case the user chooses not to install it after previously having it installed
Name: {userappdata}\Microsoft\Internet Explorer\Quick Launch\WinMerge.lnk; Type: files

;This removes the desktop icon in case the user chooses not to install it after previously having it installed
Name: {userdesktop}\WinMerge.lnk; Type: files

[Files]
;The MinVersion forces Inno Setup to only copy the following file if the user is running a WinNT platform system
Source: ..\Build\MergeUnicodeRelease\WinMergeU.exe; DestDir: {app}; MinVersion: 0, 4; Components: main; Flags: ignoreversion

;The MinVersion forces Inno Setup to only copy the following file if the user is running Win9X platform system
Source: ..\Build\MergeRelease\WinMerge.exe; DestDir: {app}; MinVersion: 4, 0; Components: main; Flags: ignoreversion

; begin VC system files
Source: Runtimes\mfc42.dll; DestDir: {sys}; Flags: restartreplace uninsneveruninstall regserver sharedfile
Source: Runtimes\mfc42u.dll; DestDir: {sys}; Flags: restartreplace uninsneveruninstall regserver sharedfile; MinVersion: 0, 4
Source: Runtimes\msvcrt.dll; DestDir: {sys}; Flags: restartreplace uninsneveruninstall sharedfile
; end VC system files


;Please do not reorder the 7z Dlls by version they compress better ordered by platform and then by version
Source: ..\Build\MergeUnicodeRelease\Merge7z313U.dll; DestDir: {app}; Flags: ignoreversion; MinVersion: 0, 4; Check: Install7ZipDll(313)
Source: ..\Build\MergeUnicodeRelease\Merge7z312U.dll; DestDir: {app}; Flags: ignoreversion; MinVersion: 0, 4; Check: Install7ZipDll(312)
Source: ..\Build\MergeUnicodeRelease\Merge7z311U.dll; DestDir: {app}; Flags: ignoreversion; MinVersion: 0, 4; Check: Install7ZipDll(311)


Source: ..\Build\MergeRelease\Merge7z313.dll; DestDir: {app}; Flags: ignoreversion; MinVersion: 4, 0; Check: Install7ZipDll(313)
Source: ..\Build\MergeRelease\Merge7z312.dll; DestDir: {app}; Flags: ignoreversion; MinVersion: 4, 0; Check: Install7ZipDll(312)
Source: ..\Build\MergeRelease\Merge7z311.dll; DestDir: {app}; Flags: ignoreversion; MinVersion: 4, 0; Check: Install7ZipDll(311)

Source: ..\Docs\*.*; DestDir: {app}\Docs; Components: docs; Flags: ignoreversion sortfilesbyextension
Source: ..\Filters\*.*; DestDir: {app}\Filters; Components: filters; Flags: ignoreversion sortfilesbyextension

Source: ..\Languages\DLL\MergeChineseSimplified.lang; DestDir: {app}; Components: chinesesimplifiedlanguage; Flags: ignoreversion
Source: ..\Languages\DLL\MergeChineseTraditional.lang; DestDir: {app}; Components: chinesetraditionallanguage; Flags: ignoreversion
Source: ..\Languages\DLL\MergeKorean.lang; DestDir: {app}; Components: koreanlanguage; Flags: ignoreversion

Source: ..\Languages\DLL\MergeCzech.lang; DestDir: {app}; Components: czechlanguage; Flags: ignoreversion
Source: ..\Languages\DLL\MergeSlovak.lang; DestDir: {app}; Components: slovaklanguage; Flags: ignoreversion

Source: ..\Languages\DLL\MergeDanish.lang; DestDir: {app}; Components: danishlanguage; Flags: ignoreversion
Source: ..\Languages\DLL\MergeDutch.lang; DestDir: {app}; Components: dutchlanguage; Flags: ignoreversion

Source: ..\Languages\DLL\MergeFrench.lang; DestDir: {app}; Components: frenchlanguage; Flags: ignoreversion
Source: ..\Languages\DLL\MergeGerman.lang; DestDir: {app}; Components: germanlanguage; Flags: ignoreversion
Source: ..\Languages\DLL\MergeItalian.lang; DestDir: {app}; Components: italianlanguage; Flags: ignoreversion
Source: ..\Languages\DLL\MergeSpanish.lang; DestDir: {app}; Components: spanishlanguage; Flags: ignoreversion
Source: ..\Languages\DLL\MergeBrazilian.lang; DestDir: {app}; Components: brazilian; Flags: ignoreversion

Source: ..\Docs\Read Me.rtf; DestDir: {app}; Components: main; Flags: ignoreversion

[INI]
Filename: {app}\WinMerge.url; Section: InternetShortcut; Key: URL; String: http://winmerge.org/


[Icons]
;Start Menu Icons
Name: {group}\WinMerge; Filename: {app}\{code:ExeName}; HotKey: Ctrl+Alt+M
Name: {group}\Read Me; Filename: {app}\Read Me.rtf; IconFileName: {win}\NOTEPAD.EXE
Name: {group}\WinMerge on the Web; Filename: {app}\WinMerge.url
Name: {group}\Uninstall WinMerge; Filename: {uninstallexe}

;Desktop Icon
Name: {userdesktop}\WinMerge; Filename: {app}\{code:ExeName}; Tasks: desktopicon

;Quick Launch Icon
Name: {userappdata}\Microsoft\Internet Explorer\Quick Launch\WinMerge; Filename: {app}\{code:ExeName}; Tasks: quicklaunchicon


[Registry]
Root: HKCU; Subkey: Software\Thingamahoochie; Flags: uninsdeletekeyifempty
Root: HKCU; Subkey: Software\Thingamahoochie\WinMerge; Flags: uninsdeletekey
Root: HKCU; Subkey: Software\Thingamahoochie\WinMerge\Font

; Set the default font to Courier New size 12
Root: HKCU; Subkey: Software\Thingamahoochie\WinMerge\Font; ValueType: string; ValueName: FaceName; ValueData: Courier New
Root: HKCU; Subkey: Software\Thingamahoochie\WinMerge\Font; ValueType: dword; ValueName: Specified; ValueData: 1
Root: HKCU; Subkey: Software\Thingamahoochie\WinMerge\Font; ValueType: dword; ValueName: Height; ValueData: $fffffff0
Root: HKCU; Subkey: Software\Thingamahoochie\WinMerge\Font; ValueType: dword; ValueName: Width; ValueData: 0
Root: HKCU; Subkey: Software\Thingamahoochie\WinMerge\Font; ValueType: dword; ValueName: Escapement; ValueData: 0
Root: HKCU; Subkey: Software\Thingamahoochie\WinMerge\Font; ValueType: dword; ValueName: Orientation; ValueData: 0
Root: HKCU; Subkey: Software\Thingamahoochie\WinMerge\Font; ValueType: dword; ValueName: Weight; ValueData: $190
Root: HKCU; Subkey: Software\Thingamahoochie\WinMerge\Font; ValueType: dword; ValueName: Italic; ValueData: 0
Root: HKCU; Subkey: Software\Thingamahoochie\WinMerge\Font; ValueType: dword; ValueName: Underline; ValueData: 0
Root: HKCU; Subkey: Software\Thingamahoochie\WinMerge\Font; ValueType: dword; ValueName: StrikeOut; ValueData: 0
Root: HKCU; Subkey: Software\Thingamahoochie\WinMerge\Font; ValueType: dword; ValueName: CharSet; ValueData: 0
Root: HKCU; Subkey: Software\Thingamahoochie\WinMerge\Font; ValueType: dword; ValueName: OutPrecision; ValueData: 3
Root: HKCU; Subkey: Software\Thingamahoochie\WinMerge\Font; ValueType: dword; ValueName: ClipPrecision; ValueData: 2
Root: HKCU; Subkey: Software\Thingamahoochie\WinMerge\Font; ValueType: dword; ValueName: Quality; ValueData: 1
Root: HKCU; Subkey: Software\Thingamahoochie\WinMerge\Font; ValueType: dword; ValueName: PitchAndFamily; ValueData: $31

; delete obsolete values
;In Inno Setup Version 4.18 ValueData couldn't be null and compile,
;if this is fixed in a later version feel free to remove the parameter
Root: HKCU; Subkey: Software\Thingamahoochie\WinMerge\Settings; ValueType: dword; ValueName: LeftMax; ValueData: 0; Flags: deletevalue
Root: HKCU; Subkey: Software\Thingamahoochie\WinMerge\Settings; ValueType: dword; ValueName: DirViewMax; ValueData: 0; Flags: deletevalue
Root: HKCR; SubKey: Directory\shell\WinMerge\command; ValueType: string; Flags: deletekey noerror
Root: HKCR; SubKey: Directory\shell\WinMerge; ValueType: string; Flags: deletekey noerror

;Adds "Start Menu" --> "Run" Support for WinMerge
Root: HKLM; Subkey: Software\Microsoft\Windows\CurrentVersion\App Paths\WinMerge.exe; ValueType: none; Flags: uninsdeletekey
Root: HKLM; Subkey: Software\Microsoft\Windows\CurrentVersion\App Paths\WinMergeU.exe; ValueType: none; Flags: uninsdeletekey
Root: HKLM; SubKey: SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\WinMerge.exe; ValueType: string; ValueName: ; ValueData: {app}\{code:ExeName}
Root: HKLM; SubKey: SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\WinMergeU.exe; ValueType: string; ValueName: ; ValueData: {app}\{code:ExeName}



[Run]
Filename: {win}\Explorer.exe; Parameters: """{group}"""; Flags: nowait skipifsilent
Filename: {app}\{code:ExeName}; Description: Launch WinMerge; Flags: nowait postinstall skipifsilent runmaximized



[UninstallDelete]
Type: files; Name: {app}\WinMerge.url
Type: dirifempty; Name: {app}

[Code]
Var
    int7Zip_Version: Integer;

Function ExeName(Unused: string): string;
Begin

  If UsingWinNT() = True Then
	 Result := 'WinMergeU.exe'
  Else
    Result := 'WinMerge.exe';

End;

Function FixVersion(strInput: string): string;
{Returns a version with four segments A.B.C.D}
Var
  {Stores the number of periods found within the version string}
  intPeriods: integer;

  {Creates a counter}
  i: integer;

  {Generates the string to be returned to the user}
  strVersion: string;
Begin

  {Creates a copy of the input string before we tear it apart}
  strVersion := strInput;

  {Until strInput is empty do..}
  While Length(strInput) > 0 do
    Begin
      {if the first character of the input string is a period then}
      If Copy(strInput, 1, 1) = '.' Then

        {Incriment the number of periods found}
        intPeriods := intPeriods + 1;

      {Remove the first character from the Input string}
      strInput := Copy(strINput, 2, length(strINput));
    End;

  {For every period shy of 3 do..}
  For i := 1 to 3 - intperiods do

    {Add a '.0' to the version string}
    strVersion := strVersion + '.0';

  {Returns the Version string with the correct number of segments}
  Result := strVersion;

End;

Function RemoveLeadingZeros(strInput: string): string;
{Removes the leading zeros if any from a numeric string}
Begin

  {While the first character of the string is a zero}
  While Copy(strInput, 1, 1) = '0' Do
    begin

    {Removes one leading zero from the input string}
      strInput := Copy(strInput, 2, Length(strINput));
    end;

  {Returns the formatted string}
  Result := strInput;

End;


{Returns whether or not the version string detected is meets the version number requirement}
Function VersionAtLeast(strVersion_Installed: string; intMajor: integer; intMinor: integer; intRevision: integer; intBuild: integer): boolean;
Var

  {Stores the Major of the Version installed (X.0.0.0)}
  intMajor_Installed: Integer;

  {Stores the Minor of the Version installed (1.X.0.0)}
  intMinor_Installed: Integer;

  {Stores the Revision of the Version installed (1.0.X.0)}
  intRevision_Installed: Integer;

  {Stores the Revision of the Version installed (1.0.0.X)}
  intBuild_Installed: Integer;

  {Stores the last valid character of a particular segment (Major, Minor, Revision) of the Version string}
  intEnd_Pos: Integer;

begin
  {Debug
  Msgbox('The version installed is ' + strVersion_Installed + ' and the required version is ' + IntToStr(intMajor) + '.' + IntToStr(intMinor) + '.' + IntToStr(intRevision) + '.' + IntToStr(intBuild), mbINformation, mb_OK)
        }

  {Makes sure the version string contains four numberic segments 5.2 ---> 5.2.0.0}
  strVersion_Installed := FixVersion(strVersion_Installed);

  {If the version number is empty then quit the function}
  if strVersion_Installed = '' Then
    begin
      Result := False;

      {Stops analyzing the version installed and returns that the version installed was inadequate}
      exit;
    end;

  {Starts detecting the Major value of the Version Installed}

  {Sets the end position equal to one character before the first period in the version number}
  intEnd_Pos := Pos('.', strVersion_Installed) -1

  {Sets the major version equal to all character before the first period }
  intMajor_installed := StrToIntDef(RemoveLeadingZeros(Copy(strVersion_Installed, 1, intEnd_Pos)), 0);

  {Debug
  msgbox('The Major version installed is ' + IntToStr(intMajor_installed) + ' and the required Major is ' + IntToStr(intMajor) + '.', mbInformation, MB_OK)
        }

  {If the Major Version Installed is greater than the required value then...}
  if intMajor_Installed > intMajor Then
    begin
      {Returns that the version number was adequate, since it actually exceeded the requirement}
      Result := True;

        {Debug
        msgbox(IntToStr(intMajor_installed) + ' > ' +  IntToStr(intMajor), mbInformation, MB_OK)
        }

      {Stops analyzing the version number since we already know it met the requirement}
      exit;
    end;

  {If the Major version installed is less than the requirement then...}
  If intMajor_Installed < intMajor Then
    begin
      {Debug
       msgbox(IntToStr(intMajor_installed) + ' < ' +  IntToStr(intMajor), mbInformation, MB_OK)
       }

      Result := False;

      {Stops analyzing the version number since we already know it's inadequate and returns False (inadequate)}
      exit;
    end


  {Starts detecting the Minor version of the Version Installed}

  {Modifies strVersion_Installed removing the first period and everything prior to it (Removes the Major Version)}
  strVersion_Installed := Copy(strVersion_Installed, intEnd_Pos + 2, Length(strVersion_Installed));

  {Sets the end position equal to one character before the first period in the version number}
  intEnd_Pos := Pos('.', strVersion_Installed) -1

  {Sets the Minor version equal to all character before the first period }
	intMinor_installed := StrToIntDef(RemoveLeadingZeros(Copy(strVersion_Installed, 1, intEnd_Pos)), 0)

	{Debug
  msgbox('The Minor version installed is ' + IntToStr(intMinor_installed) + ' and the required Minor is ' + IntToStr(intMinor) + '.', mbInformation, MB_OK)
        }

	{If the Minor Version Installed is greater than the required value then...}
	If intMinor_Installed > intMinor Then
    begin
      {Returns that the version number was adequate}
      Result := True;

      {Stops further analyzation of the version number}
      exit
    end;

  {If the minor installed is less than what was required}
  If intMinor_Installed < intMinor Then
    Begin
      Result := False;

      {Returns that the version installed did not meet the required value and stops analyzing the version number}
      exit;
    end;


  {Starts Detecting the Revision of the Version Installed}

	{Modifies strVersion_Installed removing the first period and everything prior to it (Removes the Minor Version)}
	strVersion_Installed := Copy(strVersion_Installed, intEnd_Pos + 2, Length(strVersion_Installed));

	{Sets the last character of the Revision to last character before the first period}
	intEnd_Pos := Pos('.', strVersion_Installed) -1

	{Sets the Revision Installed equal to everything before the first period}
	intRevision_Installed := strToIntDef(RemoveLeadingZeros(Copy(strVersion_Installed, 1, intEnd_Pos)), 0);

	{Debug
  msgbox('The Revision version installed is ' + IntToStr(intRevision_installed) + ' and the required Revision is ' + IntToStr(intRevision) + '.', mbInformation, MB_OK)
        }

	{If the Revision Installed is greater than the required value then...}
  If intRevision_Installed > intRevision Then
    begin
      {Return that the version installed was adequate}
      Result := True;

      {Stops further analyzation of the version number}
      exit
    end;

  {If the revision installed did not meet the requirement then...}
  If intRevision_Installed < intRevision Then
    begin
      Result := False;

    {Return that the version number failed to meet the requirement and stops further analyzation of the version number}
      exit;
    end;

  {Start Detection the Build Installed}

	{Modifies strVersion_Installed removing the first period and everything prior to it (Removes the Revision) leaving behind only the build number}
	strVersion_Installed := Copy(strVersion_Installed, intEnd_Pos + 2, Length(strVersion_Installed));

	{Set the build installed = to what's left of the strVersion_Installed text}
	intBuild_installed := strToIntDef(RemoveLeadingZeros(strVersion_Installed), 0);

	{Debug
  msgbox('The Build version installed is ' + IntToStr(intBuild_installed) + ' and the required Build is ' + IntToStr(intBuild) + '.', mbInformation, MB_OK)
        }

	{If the build installed is greater than or equal to the requirement then...}
	if intBuild_installed >= intBuild Then

	 {Report that the version installed was adequate}
    Result := True
  else

    {Reports the inadequacy of the version installed and seases processing }
    Result := False;
end;

{Returns whether or not the version of particular file is at least equal to requested value}
Function FileVersionAtLeast(strFile_Path: string; intMajor: integer; intMinor: integer; intRevision: integer; intBuild: integer): boolean;
  Var
  {Stores the version of the file to be compared}
  strVersion: string;
Begin
  {Gets the version number of the specified file}
  GetVersionNumbersString(strFile_Path, strVersion)

  {File Version at least is the result of the VersionAtLeast Determination}
  Result := VersionAtLeast(strVersion, intMajor, intMinor, intRevision, intBuild);


  {Debug: If you'd like to debug with a messagebox then un rem this
	If Result = True then

	 Msgbox('The version of ' + strFile_path + ' required is "' + IntToStr(intMajor) + '.' + IntToStr(intMinor) + '.' + IntToStr(intRevision) + '.' + IntToStr(intBuild) + '". The version found was "'  + strVersion + '.  The version detected met the required value.', mbInformation, MB_OK)
	else
	 Msgbox('The version of ' + strFile_path + ' required is "' + IntToStr(intMajor) + '.' + IntToStr(intMinor) + '.' + IntToStr(intRevision) + '.' + IntToStr(intBuild) + '". The version found was "' + strVersion + '.  The version detected did not meet the required value.', mbInformation, MB_OK);
    }
end;

function Install7ZipDll(strDLL_Version: string): Boolean;
Var
    {Stores the file path of the 7-Zip File Manager Program}
    str7Zip_Path: String;

    {Stores the version of 7-Zip Installed}
    str7Zip_Version: String;

    {Stores the DLL's Version Function Input Parameter in integer format}
    intDLL_Version: Integer;
Begin

    {If the actual version of 7-Zip Installed hasn't been determined yet then...}
    If int7Zip_Version = 0 Then
        Begin
	       {Detects the install location of 7-Zip from the registry, if it's installed}
	       RegQueryStringValue(HKLM, 'SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\7zFM.exe', '', str7Zip_Path)

	        {If there is 7-Zip information in the registry then...}
			If length(str7Zip_Path) > 0 Then
				begin
				    {If the 7zFM.exe file exists then...}
				    If FileExists(str7Zip_Path) = True Then
				        Begin
							{Detects the version of the 7-Zip Installed}
							GetVersionNumbersString(str7Zip_Path, str7Zip_Version)



				            {If the version of 7-Zip Installed is at least 3.11 Then...}
				            If VersionAtLeast(str7Zip_Version, 3, 11, 0, 0) = True Then
				                begin
				                    {If the user has 3.12 or higher installed then...}
				                    If VersionAtLeast(str7Zip_Version, 3, 12, 0, 0) = True Then
                                        Begin
				                            {If the user has 3.13 or higher installed then...}
				                            If VersionAtLeast(str7Zip_Version, 3, 13, 0, 0) = True Then

				                                {We record the version of 7-Zip installed as 3.13 regardless of whether or not it's actually 3.14, 3.15, etc..}
				                                int7Zip_Version := 313

				                            Else

				                                {Since it was at least 3.12, but not 3.13 then it must be 3.12}
				                                int7Zip_Version := 312;

				                        end
				                    Else
				                            {Since it was at least 3.11, but not 3.12 then it must be 3.11}
				                            int7Zip_Version := 311;
				                end;
                        End
				    Else
				       {Records that the 7-Zip program didn't exist for the rest of the installation}
				        int7Zip_Version := -1;
				end
			Else
				{Records that the 7-Zip program wasn't installed for rest of the installation}
				int7Zip_Version := -1;
	    end;

    {Converts the DLL Version String to an Integer for numeric evaluation}
    intDLL_Version := StrToInt(strDLL_Version);

	{If 7-Zip either wasn't installed or was of inadequate version then...}
	If int7Zip_Version = -1 Then
		Begin
			{If the program is trying to determine if the 313 DLL should be installed then the answer is yes
			we install this, because it's the most recent version and if they were to install 7-zip this
			would be the version they'd want (since people generally install the latest and greatest)}
			if intDLL_Version = 313 Then
				Result := True

			{If the program is trying to install anything, but 313 on a system without 7-Zip
			then we disallow the installation of that DLL}
			else
				Result := False;
		End

	{If the version of 7-Zip was sufficient then...}
	Else
		Begin
			{if the version the program is trying to install matches the version installed on the clients system then...}
			If int7Zip_Version = intDLL_Version Then
				Result := True

			{If the program is trying to install the 31X DLL on a 31Y system then we won't allow the file to be copied...}
            else
				Result := False;
		End;

    {Debug:
    If UsingWinNT = True Then
        begin
            If Result = True Then
                Msgbox('We''re are installing Merge7z' + strDLL_Version + 'U.DLL because the system has 7-Zip ' + IntToStr(int7Zip_Version) + ' installed.', mbInformation, mb_Ok)
            Else
                Msgbox('We''re aren''t installing Merge7z' + strDLL_Version + 'U.DLL because the system has 7-Zip ' + IntToStr(int7Zip_Version) + ' installed.', mbInformation, mb_Ok);
        end


    Else
         begin
            If Result = True Then
                Msgbox('We''re are installing Merge7z' + strDLL_Version + '.DLL because the system has 7-Zip ' + IntToStr(int7Zip_Version) + ' installed.', mbInformation, mb_Ok)
            Else
                Msgbox('We''re aren''t installing Merge7z' + strDLL_Version + '.DLL because the system has 7-Zip ' + IntToStr(int7Zip_Version) + ' installed.', mbInformation, mb_Ok);
        end }

End;
[_ISTool]
OutputExeFilename=D:\Programming\Visual C++\WinMerge\WinMerge\InnoSetup\Output\WinMerge 2.1.5.13.exe
