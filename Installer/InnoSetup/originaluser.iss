Var
    g_OriginalUserSID: string;

const
  SECURITY_MAX_SID_SIZE = 68;

function LookupAccountName(lpSystemName: string; lpAccountName: string;
  Sid: DWORD_PTR; var cbSid: DWORD; ReferencedDomainName: string;
  var cchReferencedDomainName: DWORD; var peUse: DWORD): BOOL;
  external 'LookupAccountNameW@Advapi32.dll stdcall';

function ConvertSidToStringSid(Sid: DWORD_PTR; var pStringSid: DWORD_PTR): BOOL;
  external 'ConvertSidToStringSidW@Advapi32.dll stdcall';

function LocalAlloc(uFlags: UINT; uBytes: DWORD): DWORD_PTR;
  external 'LocalAlloc@Kernel32.dll stdcall';

function LocalFree(hMem: DWORD_PTR): DWORD_PTR;
  external 'LocalFree@Kernel32.dll stdcall';
  
function OemToCharA(lpszSrc, lpszDst: PAnsiChar): BOOL;
  external 'OemToCharA@user32.dll stdcall';

function ConvertOemToAnsi(const OemStr: AnsiString): AnsiString;
begin
  SetLength(Result, Length(OemStr) + 1);
  OemToCharA(PAnsiChar(OemStr), PAnsiChar(Result));
end;

function GetOriginalUserName(): string;
var
  TempFile: string;
  Output: AnsiString;
  ResultCode: Integer;
begin
  TempFile := ExpandConstant('{commonappdata}\whoami_output.txt');
  if ExecAsOriginalUser('cmd.exe', '/C whoami > "' + TempFile + '"', '', SW_HIDE, ewWaitUntilTerminated, ResultCode) then begin
    if LoadStringFromFile(TempFile, Output) then begin
      Result := Trim(ConvertOemToAnsi(Output));
    end;
  end;
  if Result = '' then
    Result := GetUserNameString();
  DeleteFile(TempFile);
end;
  
function GetUserSID(UserName: string): string;
var
  Sid: DWORD_PTR;
  SidSize: DWORD;
  DomainName: string;
  DomainNameSize: DWORD;
  peUse: DWORD;
  pStringSid: DWORD_PTR;
begin
  Result := '';
  SidSize := SECURITY_MAX_SID_SIZE;
  DomainNameSize := 256;
  SetLength(DomainName, DomainNameSize);

  Sid := LocalAlloc(0, SidSize);
  if Sid = 0 then
    Exit;

  if LookupAccountName('', UserName, Sid, SidSize, DomainName, DomainNameSize, peUse) then
  begin
    if ConvertSidToStringSid(Sid, pStringSid) then begin
      Result := CastIntegerToString(pStringSid);
      LocalFree(pStringSid);
    end
  end;

  LocalFree(Sid);
end;

function GetOriginalUserSID(param: string): string;
var
  UserName: string;
begin
  if g_OriginalUserSID <> '' then begin
    Result := g_OriginalUserSID;
    exit
  end;

  UserName := GetOriginalUserName();
  g_OriginalUserSID := GetUserSID(UserName);
  if g_OriginalUserSID = '' then begin
    UserName := GetUserNameString();
    g_OriginalUserSID := GetUserSID(UserName);
  end;
  if g_OriginalUserSID = '' then begin
    RaiseException('Could not retrieve the SID for user: ' + UserName);
  end;

  Result := g_OriginalUserSID
end;
