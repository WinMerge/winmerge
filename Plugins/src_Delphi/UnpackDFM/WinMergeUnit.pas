unit WinMergeUnit;

interface

uses
  ComObj, UnpackDFMLib_TLB;

type
  TWinMergeScript = class(TAutoObject, IWinMergeScript)
  protected
    function Get_PluginDescription : WideString; safecall;
    function Get_PluginEvent : WideString; safecall;
    function Get_PluginFileFilters : WideString; safecall;
    function Get_PluginIsAutomatic : WordBool; safecall;
    function PackFile(const fileSrc, fileDst : WideString;
      var pChanged : WordBool; Subcode : Integer) : WordBool; safecall;
    function UnpackFile(const fileSrc, fileDst : WideString;
      var pChanged : WordBool; var pSubcode : Integer) : WordBool; safecall;
  end;

implementation

uses
  ComServ, SysUtils, Classes;

function TWinMergeScript.Get_PluginDescription : WideString;
begin
  result := 'Delphi DFM plugin';
end;

function TWinMergeScript.Get_PluginEvent : WideString;
begin
  result := 'FILE_PACK_UNPACK';
end;

function TWinMergeScript.Get_PluginFileFilters : WideString;
begin
  result := '\.dfm$';
end;

function TWinMergeScript.Get_PluginIsAutomatic : WordBool;
begin
  result := true;
end;

// ------------------------------------------------------------------
// Converts the text version of a DFM to the binary version.
// ------------------------------------------------------------------
function TWinMergeScript.PackFile(const fileSrc, fileDst : WideString;
  var pChanged : WordBool; Subcode : Integer) : WordBool;
var
  src : TFileStream;
  dst : TFileStream;
begin
  try

    src := TFileStream.Create(fileSrc, fmOpenRead);
    try
      dst := TFileStream.Create(fileDst, fmCreate);
      try
        ObjectTextToResource(src, dst);
      finally
        dst.Free;
      end;
    finally
      src.Free;
    end;

    pChanged := true;
    result := true;
  except
    pChanged := false;
    result := false;
  end;
end;



// ------------------------------------------------------------------
// Converts the binary DFM file to the text version.
// ------------------------------------------------------------------
function TWinMergeScript.UnpackFile(const fileSrc, fileDst : WideString;
  var pChanged : WordBool; var pSubcode : Integer) : WordBool;
const
  FilerSignature = integer($30465054);           // 'TPF0'
  ResSignature = integer($00000AFF);             // FF 04 00
var
  src: TFileStream;
  dst: TFileStream;
  Signature: Integer;
begin
  result := false;
  pChanged := false;
  try

    src := TFileStream.Create(fileSrc, fmOpenRead);
    try
      //if (TestStreamFormat(src) = sofBinary) then begin
      src.Read(Signature, SizeOf(Signature));
      if (Signature = FilerSignature) or (Signature and $00FFFFFF = ResSignature) then begin
        src.Seek(0, soFromBeginning);
        dst := TFileStream.Create(fileDst, fmCreate);
        try
          ObjectResourceToText(src, dst);
          pChanged := true;
        finally
          dst.Free;
        end;
      end;
      result := true;
    finally
      src.Free;
    end;

  except
  end;
end;

initialization
  TAutoObjectFactory.Create(ComServer, TWinMergeScript, Class_WinMergeScript,
    ciMultiInstance, tmApartment);
end.

