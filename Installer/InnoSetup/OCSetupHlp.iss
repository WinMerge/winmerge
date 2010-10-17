;---------------------------------------------------------------------------;
;                            OCSetupHlp.iss                                 ;
;---------------------------------------------------------------------------;
;                                                                           ;
; Header file for OpenCandy integration                                     ;
; with Inno Setup 5 projects and files.                                     ;
;                                                                           ;
;---------------------------------------------------------------------------;

;
; OpenCandy Defines
; -----------------
;
; Used to abstract file names.
;

#define OCDLL    "OCSetupHlp.dll"
#define OCREADME "OpenCandy_Why_Is_This_Here.txt"

[Code]

#ifdef UNICODE
type OCString = AnsiString;
#else
type OCString = String;
#endif


//
// OpenCandy Global Variables
// --------------------------
//

var

  bDetached:boolean;         // Used to keep track of offer page state
  UseOfferPage: Boolean;     // Used to know if we should shown an offer
  OfferAccepted: Boolean;    // Used to keep track of the user selection
  OCOfferPage: TWizardPage;  // Handle to the offer page wizard page
  OCNOCANDY: Boolean;        // Used to keep track of /NOCANDY parameter
  ProductKey: OCString;      // Used to store the product key
  GoodOS: Boolean;           // Used to store the results of the OS check
  SignalSent: Boolean;       // Used to keep track or server comm
  GetOffersCalled: Boolean;  // Used to keep track of the first call
  ProductFailed: Boolean;    // Keeps track of errors that may occur during product installation, so we can signal the DLL on failure
  OffersReady: Boolean;      // Set to false if offers are not ready in time

//
// OpenCandy DLL (OCSetupHlp) function declarations
// ------------------------------------------------
//
// These functions are exported from OCSetupHlp.dll
// and used throughout the setup to perform the operations
// needed for the display of the offer page
//

function OCInit(a,b,c,d,e:OCString; f,bAsyncMode:Boolean): Integer;
external 'OCInit2A@files:OCSetupHlp.dll cdecl loadwithalteredsearchpath delayload';

function OCGetBannerInfo(a,b:OCString):Integer;
external 'OCGetBannerInfo@files:OCSetupHlp.dll cdecl loadwithalteredsearchpath delayload';

function OCRunPage(hwnd,r,g,b:Integer): Integer;
external 'OCRunDialog@files:OCSetupHlp.dll cdecl loadwithalteredsearchpath delayload';

function OCAdjustPage(hwnd,x,y,w,h : Integer): Integer;
external 'OCInnoAdjust@files:OCSetupHlp.dll cdecl loadwithalteredsearchpath delayload';

function OCRestorePage(hwnd:Integer): Integer;
external 'OCInnoRestore@files:OCSetupHlp.dll cdecl loadwithalteredsearchpath delayload';

function OCGetOfferState(): Integer;
external 'OCGetOfferState@files:OCSetupHlp.dll cdecl loadwithalteredsearchpath delayload';

function OCGetOfferType(cmdline:OCString): Integer;
external 'OCGetOfferType@files:OCSetupHlp.dll cdecl loadwithalteredsearchpath delayload';

function OCFinish(Mgr:OCString):Integer;
external 'OCExecuteOffer@files:OCSetupHlp.dll cdecl loadwithalteredsearchpath delayload';

function OCShutdown(): Integer;
external 'OCShutdown@files:OCSetupHlp.dll cdecl loadwithalteredsearchpath delayload';

function OCDetach(): Integer;
external 'OCDetach@files:OCSetupHlp.dll cdecl loadwithalteredsearchpath delayload';

function OCSignalProductInstalled():Integer;
external 'OCSignalProductInstalled@files:OCSetupHlp.dll cdecl loadwithalteredsearchpath delayload';

function OCSetOfferLocation(location,OID,PID:OCString):Integer;
external 'OCSetOfferLocation@files:OCSetupHlp.dll cdecl loadwithalteredsearchpath delayload';

function OCGetMsg(a,b:OCString):Integer;
external 'OCGetMsg@files:OCSetupHlp.dll cdecl loadwithalteredsearchpath delayload';

function OCSignalProductFailed():Integer;
external 'OCSignalProductFailed@files:OCSetupHlp.dll cdecl loadwithalteredsearchpath delayload';

// Checks if offers are ready or not.
// 0 = Offers are ready to show
// 1 = Offers are not ready, but might be. try waiting a little while
// 2 = Offers are not ready, but might be. the delay could be long though.
// 3 = No offers available
function OpenCandyAreOffersReady():Integer;
external 'OCGetAsyncOfferStatus@files:OCSetupHlp.dll cdecl loadwithalteredsearchpath delayload';

//
// String manipulation function declaration from Kernel32.dll
// We use those to convert from UTF-8 to unicode and other codepages
//

Function MultiByteToWideChar(CodePage: UINT; dwFlags: DWORD;
                             lpMultiByteStr: OCString; cbMultiByte: integer;
                             lpWideCharStr: String; cchWideChar: integer): longint;
external 'MultiByteToWideChar@kernel32.dll stdcall';

Function WideCharToMultiByte(CodePage: UINT; dwFlags: DWORD;
                             lpWideCharStr: String; cchWideChar: integer;
                             lpMultiByteStr: OCString; cbMultiByte: integer;
                             lpDefaultChar: integer; lpUsedDefaultChar: integer): longint;
external 'WideCharToMultiByte@kernel32.dll stdcall';

//
// Internal OpenCandy Helper Functions
// -----------------------------------
//
// These functions are for internal use ONLY
//

//
// ConvertUtf8ToCodePage()
// -----------------------
//
// Internal function used to convert
// the i18n messages we get from the DLL
// into a given codepage for diplay in
// the ASCII version of Inno
//
//

function ConvertUtf8ToCodePage(strSource: OCString; codePage: UINT): OCString;
var
    nRet: integer;
    WideCharBuf: OCString;
    MultiByteBuf: OCString;

begin
    strSource := strSource + chr(0);
    SetLength( WideCharBuf, Length( strSource ) * 2 );
    SetLength( MultiByteBuf, Length( strSource ) * 3 );

    // CP_UTF8 is 65001
    nRet := MultiByteToWideChar(65001,  0, strSource, -1, WideCharBuf, Length(WideCharBuf));
    nRet := WideCharToMultiByte(codePage, 0, WideCharBuf, -1, MultiByteBuf, Length(MultiByteBuf), 0, 0);

    if(nRet = 0) then
    begin
      // Try with CP_ACP (ASCII)
      WideCharToMultiByte(0, 0, WideCharBuf, -1, MultiByteBuf, Length(MultiByteBuf), 0, 0);
    end;

    Result := MultiByteBuf;
end;

//
// ConvertUtf8ToUnicode()
// -----------------------
//
// Internal function used to convert
// the i18n messages we get from the DLL
// into Unicode from UTF8
//
//

function ConvertUtf8ToUnicode(strSource: OCString): String;
var
    nRet: integer;
    WideCharBuf: String;
begin
    strSource := strSource + chr(0);
    SetLength(WideCharBuf, Length(strSource)*2);

    // CP_UTF8 is 65001
    nRet := MultiByteToWideChar(65001, 0, strSource, -1, WideCharBuf, Length(WideCharBuf));

    Result := WideCharBuf;
end;



// OpenCandyDetectOS()
// -------------------
//
// Internal Operating System version
// detection code, used to rule out
// operation on Windows 2000 and below
//

procedure OpenCandyDetectOS();
var
  WindowsInfo: TWindowsVersion;
begin
  GoodOS := False;

  GetWindowsVersionEx(WindowsInfo);
  if WindowsInfo.NTPlatform and
   ((WindowsInfo.Major > 5) or
   ((WindowsInfo.Major = 5) and (WindowsInfo.minor > 0) )) then
  begin
      GoodOS := True;
  end;
end;

//
// External OpenCandy API Functions
// -----------------------------------
//
// These functions are for use in your setup
//

//
// OpenCandyInitRemnant()
// ----------------------
//
// This function initializes the OpenCandy sytem. This function takes
// and extra argument compared to OpenCandyInit() (dontShowOC:boolean)
// that controls whrether an offer page is shown or not. This function
// is for using with installations where a primary offer can be shown
// in place of the OpenCandy offer.
//


procedure OpenCandyInitRemnant2(publisher:  OCString;
                               key:        OCString;
                               secret:     OCString;
                               language:   OCString;
                               location:   OCString;
                               dontShowOC: boolean;
							   bUseAsyncMode: boolean);
var
	retval:integer;
	i:integer;
	title:OCString;
	description:OCString;
	OID: OCString;
  uDesc: String;
  uTitle: String;

begin
	// Set ProductInstalled to True once entire product install completes.
	ProductFailed := False;
	bDetached := True;
	ProductKey := key;
	OCNOCANDY := False;
	OfferAccepted := False;
	SignalSent := False;
	UseOfferPage := False;
	GetOffersCalled := False;
	OffersReady := True;

	OpenCandyDetectOS();

	if GoodOS = False then Exit;
	for i := 0 to ParamCount() do
	begin;
		if CompareText(ParamStr(i), '/NOCANDY') = 0 then
		begin
			OCNOCANDY := True;
			if( i > 1) then
			begin;
				OID:=ParamStr(i-1);
				retval:=OCSetOfferLocation(location,OID,key);
			end;
		end;
		if CompareText(ParamStr(i), '/SILENT') = 0 then
		begin
			OCNOCANDY := True;			
		end;
		if CompareText(ParamStr(i), '/VERYSILENT') = 0 then
		begin
			OCNOCANDY := True;			
		end;
	end;

	if(OCNOCANDY <> True) and (dontShowOC <> True) then
	begin
		retval := OCInit(publisher, key, secret, language, location, false,bUseAsyncMode);

		GetOffersCalled := True;

		if retval = 0 then
		begin

			UseOfferPage := True;
			title:=StringOfChar(' ',512);
			description:=StringOfChar(' ',512);
			retval:= OCGetBannerInfo(title,description);
			if retval = 1 then
			begin
			 	description:='';
			end;
			if retval = 2 then
			begin
			  title :='';
			end;
			if (retval = -1) OR (retval=0) then
			begin
			  description:='blah';
			  title :='blah';
			end;

  #ifdef UNICODE
			uDesc  := ConvertUtf8ToUnicode(description);
			uTitle := ConvertUtf8ToUnicode(title);
      OCOfferPage := CreateCustomPage(wpSelectTasks, uTitle, uDesc);
   #else
			description := ConvertUtf8ToCodePage(description, GetUILanguage());
			title       := ConvertUtf8ToCodePage(title, GetUILanguage());
      OCOfferPage := CreateCustomPage(wpSelectTasks, title, description);
  #endif


		end;
	end;
end;


procedure OpenCandyInitRemnant(key:        OCString;
                               secret:     OCString;
                               language:   OCString;
                               location:   OCString;
                               dontShowOC: boolean);
begin
		OpenCandyInitRemnant2('', key, secret, language, location, dontShowOC, False);
end;


//
// OpenCandyInit()
// ---------------
//
// These function initializes the OpenCandy system.
// It calls the OpenCandyInitRemnant() function with
// the last parameter set to False, so that the offer
// page will be shown (if available). This is the
// normal scenario.

// Includes publisher parameter
procedure OpenCandyInit2(publisher: OCString;
                        key:       OCString;
                        secret:    OCString;
                        language:  OCString;
                        location:  OCString;
						bUseAsyncMode: Boolean);
begin
  OpenCandyInitRemnant2(publisher,
                       key,
                       secret,
                       language,
                       location,
                       False,
					   bUseAsyncMode);
end;

// Use server setting
procedure OpenCandyInit(publisher: OCString;
						key:       OCString;
                        secret:    OCString;
                        language:  OCString;
                        location:  OCString);
begin
  OpenCandyInitRemnant2(publisher,
                       key,
                       secret,
                       language,
                       location,
                       False,
					   False);
end;



//
// OpenCandyShouldSkipPage()
// -------------------------
//
// Checks if page should be skipped
//


function OpenCandyShouldSkipPage(CurPageID: Integer) : Boolean;
var
	retval: Integer;
	nOffersState: Integer;
begin
  
	if GoodOS = false then Exit;

	if UseOfferPage = True then
	begin
		if CurPageID = OCOfferPage.ID then
		begin
			if OffersReady = False then
			begin
				Result := True;
				//MsgBox('OpenCandyShouldSkipPage', mbInformation, MB_OK);
				Exit;
			end;
			if bDetached = True then
			begin
				Result := True;
				bDetached := False;
				OffersReady := False;
				//MsgBox('ShouldSkipPage', mbInformation, MB_OK);
				retval := OCRunPage(OCOfferPage.Surface.Handle,240,240,240);
				if retval <> -1 then
				begin
					Result := False;
					OffersReady := True;
					//MsgBox('Not skipping Page', mbInformation, MB_OK);
				end;
				if retval = -1 then
				begin
					//MsgBox('OpenCandyShouldSkipPage', mbInformation, MB_OK);
				end;
			end;
		end;
	end;
end;

//
// OpenCandyCurPageChanged()
// -------------------------
//
// This function is to be called from CurPageChanged()
// It makes sure to run the OpenCandy offer page when it
// is time to run it.
//

procedure OpenCandyCurPageChanged(CurPageID: Integer);
var
	retval: Integer;
begin
  if GoodOS = false then Exit;
  if OffersReady = False then
  begin
    Exit;
  end;

	if UseOfferPage = True then
	begin
		if CurPageID = OCOfferPage.ID then
		begin
			retval := OCAdjustPage(OCOfferPage.Surface.Handle,12,70,480,300);
		end;
	end;
end;

//
// OpenCandyNextButtonClick()
// --------------------------
//
// This function should be called from NextButtonClick()
// It handles the logic of the Offer page, making
// sure a selection was made (offer or no offer) before.
//

function OpenCandyNextButtonClick(CurPageID: Integer): Boolean;
var
  retval: Integer;
  getmsg: Integer;
  shouldContinue: Boolean;
  msgText: OCString;
  uMsg: String;
begin
  shouldContinue := True;

  if GoodOS = false then
  begin
    Result := shouldContinue;
    Exit;
  end;

	if UseOfferPage = True then
	begin
		if CurPageID = OCOfferPage.ID then
		begin

      retval := OCGetOfferState();

			if( retval < 0) then
			  begin
			     msgText := StringOfChar(' ', 1024);
			     getmsg := OCGetMsg('PleaseChoose', msgText);
#ifdef UNICODE
           uMsg := ConvertUtf8ToUnicode(msgText);
			     MsgBox(uMsg, mbInformation, MB_OK);
#else
           msgText := ConvertUtf8ToCodePage(msgText, GetUILanguage());
			     MsgBox(msgText, mbInformation, MB_OK);
#endif
			     shouldContinue := False;
			  end;

			if(retval >= 0) then
			  begin

			    OfferAccepted := False;
			    if(retval = 1) then
			    begin
            OfferAccepted := True;
	        end;
          //MsgBox('OCRestorePage 1', mbInformation, MB_OK);
  			  retval := OCRestorePage(OCOfferPage.Surface.Handle);
  			  shouldContinue := True;
			  end;

		end;
	end;

	Result := shouldContinue;
end;

//
// OpenCandyBackButtonClick()
// --------------------------
//
// This function should be called from BackButtonClick().
// It restores the layout of the installer window for
// other pages to use.
//

function OpenCandyBackButtonClick(CurPageID: Integer): Boolean;
begin
  if GoodOS = false then
  begin
    Result := True;
    Exit;
  end;

	if UseOfferPage = True then
	begin
		if CurPageID = OCOfferPage.ID then
		begin
      //MsgBox('OCRestorePage 2', mbInformation, MB_OK);
  		OCRestorePage(OCOfferPage.Surface.Handle);
		end;
	end;

	Result := True;
end;

//
// OpenCandyDeinitializeSetup()
// ----------------------------
//
// This should be called from DeinitializeSetup()
// It cleans up the temporarily loaded DLL and releases
// the memory we have been using (if any).
//

procedure OpenCandyDeinitializeSetup();
var
retval:integer;
begin
  if GoodOS = false then Exit;

	if UseOfferPage = True then
	begin
	    if ProductFailed = True then
	    begin
			   retval := OCSignalProductFailed();
      end;

		if bDetached = False then
		begin
			bDetached:=True;
			retval:=OCDetach();
		end;
		OCShutdown();
	end;

end;

//
// OpenCandyCurStepChanged()
// -------------------------
//
// This should be called from CurStepChanged().
//
// It handles necesary operations at the various
// different stages of the setup.
//

procedure OpenCandyCurStepChanged(CurStep: TSetupStep);
var
	retval: Integer;
	s,s2: OCString;
	cmdline: OCString;
begin
	if CurStep = ssInstall then
	begin
		// Set to true in case it fails.
		ProductFailed:= True;
	end;
	if CurStep = ssPostInstall then
	begin
		ProductFailed:= False;
	end;



	if GoodOS = false then Exit;

	// Install Step

	if CurStep = ssInstall then
	begin
		if UseOfferPage = True then
		begin
			if bDetached = False then
			  begin
				  bDetached := True;
				  retval := OCDetach();
			  end;
			end;
		end;

	// Post Install Step

	if CurStep = ssPostInstall then
	begin
		if OCNOCANDY = False then
		begin
		  if UseOfferPage = True then
		  begin
			  cmdline := StringOfChar(' ',1024);
				retval := OCGetOfferType(cmdline);

				if retval = 1 then  // OC_OFFER_TYPE_NORMAL
				begin
					s := ExpandConstant('{app}') + '\OpenCandy\{#OCDLL}';
					s := GetShortName(s);
					s2 := s + ',_MgrCheck@16';
					retval:=OCFinish(s2);
				end;
			end;
		end;
	end;

	// Install Done Step

	if CurStep = ssDone then
	begin
		if (OCNOCANDY = False) and (GetOffersCalled = True) then
		begin
			// Product was installed so let's signal it

			retval := OCSignalProductInstalled();

			// Check if offer was accepted
			if (OfferAccepted = True) then
			begin

				cmdline := StringOfChar(' ',1024);
				retval := OCGetOfferType(cmdline);

				if retval = 1 then  // OC_OFFER_TYPE_NORMAL
				begin
					// Offer was accepted, let's run
					s := ExpandConstant('{app}') + '\OpenCandy\OCSetupHlp.Dll';
					s := GetShortName(s);
					s := s + ',_MgrCheck@16';
					s:='RunDll32.exe ' + s;
					Exec('>', s, '', SW_SHOW, ewNoWait, retval);
				end;
			end;
		end;
	end;
end;

//
// OpenCandyProcessEmbedded()
// --------------------------
//
// This function should NOT be called
// directly. It serves as an installation
// handle for the OCDLL file (see the Files
// section in the main setup script)
//

procedure OpenCandyProcessEmbedded();
var
  retval:     Integer;
  str:        OCString;
  cmdline:    OCString;
  params:     OCString;
  tmp:        OCString;
  ResultCode: Integer;
begin
  if GoodOS = false then Exit;

	if UseOfferPage = True then
	begin
		cmdline := StringOfChar(' ',1024);
		retval := OCGetOfferType(cmdline);
		if retval = 2 then // OC_OFFER_TYPE_EMBEDDED
		begin

      str := ExpandConstant('{app}') + '\OpenCandy\{#OCDLL}';
			str := GetShortName(str);
			str := str + ',_MgrCheck@16';
			retval:=OCFinish(str);

			if(OfferAccepted = True) then
			begin
			  str := 'RunDll32.exe ' + str;

        // The DLL does not append a \0 since inno chokes on it, so we need to trim the string back down.
			  tmp := TrimRight(cmdline);
			  params := tmp + ' /S' + ProductKey;

        str := str + params;
			  Exec('>', str, '', SW_HIDE, ewWaitUntilTerminated, ResultCode);
		  end;

    end;
	end;
end;

//
// OpenCandyCheckInstallDLL()
// --------------------------
//
// This function should NOT be called
// directly. It serves as an installation
// check for the OCDLL files (see the Files
// section in the main setup script)
//

function OpenCandyCheckInstallDLL(): Boolean;
var
  str:     OCString;
  retval:  Integer;
  cmdline: OCString;
begin

  Result := False;

  if GoodOS = false then Exit;

  if (UseOfferPage = True) and (OCNOCANDY = False) then
  begin
    if (OfferAccepted = False) and (SignalSent = False) then
    begin
      cmdline := StringOfChar(' ',1024);
		  retval := OCGetOfferType(cmdline);

      if retval = 2 then  // OC_OFFER_TYPE_EMBEDDED
	    begin
        // We won't install the DLL but
        // we still need to call OCFinish()

        str := ExpandConstant('{app}') + '\OpenCandy\{#OCDLL}';
		    str := GetShortName(str);
		    str := str + ',_MgrCheck@16';
		    OCFinish(str);
		    SignalSent := True;
  	  end;
    end;
  end;

  Result := OfferAccepted;
end;

//
// OpenCandyCheckInstallReadme()
// -----------------------------
//
// This function should NOT be called
// directly. It serves as an installation
// check for the OCREADME file (see the Files
// section in the main setup script)
//

function OpenCandyCheckInstallReadme(): Boolean;
begin
  Result := OfferAccepted;
end;

//---------------------------------------------------------------------------//
//                            End of File                                    //
//---------------------------------------------------------------------------//










