[code]
Const
  WM_Close = $00000010;
  
Function WinMergeID(): LongInt;
Begin
    Result := FindWindowByWindowName('WinMerge')

    If Result = 0 Then
        Begin
            Result := FindWindowByWindowName('WinMerge - [File Comparison]')

            If Result = 0 Then
                Begin
                    Result := FindWindowByWindowName('WinMerge - [Directory Comparison Results]')
                end;
        end;

End;


{Modifies the Desktop.ini shortcut's text, note we still need regular Inno Setup to set the file attributes :(}
Function CustomFolderIcon(Enabled: string);
  Var
    {Stores the path to the Desktop.ini file}
    strFile_Path: string;

    {Stores the text to place inside the ini file}
    strINI_Text: string;
  Begin
    {Determines the path that Desktop.ini should be created or deleted at}
    strFile_Path := ExpandCOnstant('{app}') + '\desktop.ini';

    {If the user chose to have the installer customize the folder's icon then...}
    If ShouldProcessEntry('main', 'CustomFolderIcon') = srYes then
        begin
            {Generates the text that should be included in the Desktop.ini file}
            strINI_Text := '[.ShellClassInfo]' + #13;
            strINI_Text := strINI_Text + 'IconFile=' + ExpandConstant('{app}') + '\' + EXEName('what') + #13;
            strINI_TExt := strINI_Text + 'IconIndex=1';



            {Debug  }
            msgbox('The following file ' + strFile_Path + ' was created with these contents' + #13 + strINI_Text, mbInformation, MB_OK)

            {OverWrites the text contents of Desktop.ini
            Append=False so it'll overwrite the previous contents}
            SaveStringToFile(strFile_Path, strINI_Text, False)
        end
    {If the user doesn't want a custom folder icon then...}
    else
        {Deletes the desktop.ini file so the folder's icon won't be customized}
        DeleteFile(strFile_Path);
  End;


function InitializeSetup(): Boolean;
var
    intWinMerge: longInt;
    intTries: integer;
begin
    {Determines if WinMerge is Running}
    intWinMerge := WinMergeID;

    {If WinMerge is Running then...}
    If intWinMerge <> 0 Then
        begin
            {Asks the user if they'd like to programatically close WinMerge or abort the installation}
            If msgbox('WinMerge cannot be installed, because WinMerge is currently running.  The moment the installation is completed the application can be restarted.  Would you like to close the application now?  Clicking [No] will abort the installation entirely.', mbConfirmation, MB_YESNO) = IDYES then
                begin
                    {Initiates a Looping Sequence}
                    repeat
                        {Sends a close message to the WinMerge Window}
                        SendMessage(intWinMerge, WM_Close, 0, 0)

                        {Waits a 1/4 second}
                        Sleep(250)

                        {Checks for the WinMerge Window yet again}
                        intWinMerge := WinMergeID;

                        {Incriments the number of times we've tried to close WinMerge and waited}
                        intTries := intTries + 1
                    until (intWinMerge = 0) or (intTries = 52)

                    If intTries = 100 Then
                        begin
                            Msgbox('The installer has tried for the past 13 seconds to close the WinMerge window, but it refuses to exit.  Please close WinMerge manually and then try running this installer again.', mbInformation, mb_OK)
                            Result := False
                        end
                    else
                        Result := True;
                end
            else
                Result := False;
        end
      else
        Result := True;
  end;



{Determines whether or not the user previously had a start menu installed}
Function StartMenuExists(Unused: string): boolean;
Begin
    {If the Path to the user's Start Menu isn't blank then...}
    If strStart_Menu = '' Then
        Begin
            {If a start menu location couldn't be found then..}
            If RegQueryStringValue(HKLM, '', '', strStart_Menu) = False Then

                {We report that a previous start menu didn't exist}
                Result := False
            {The path of the start menu was detected}
            Else
                If strStart_Menu <> '' Then
                    {We know that a previous start menu does indeed exist}
                    Result := True
                else
                    {Since the value was blank we know that the user didn't have a start menu last time}
                    Result := False;
        end
        
    {The path of the start menu group is already known}
    Else
        {We report that a previous start menu does indeed exist}
        Result := True;
            
End;


;Function LastStartMenuLocation(Unused: string): string;
;Begin

;End

Function FolderPath(strInput): string;
    Var
        i: integer;
        chrCurrent: char;
Begin
    i := Length(strINput);

    Repeat
        chrCurrent := Copy(strINput, i, 1);
        i := i -1;
    Until

    Result := Copy(strInput, 1, i);

End;
