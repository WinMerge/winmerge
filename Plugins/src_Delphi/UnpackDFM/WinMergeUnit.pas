unit WinMergeUnit;

interface

uses
  ComObj, ActiveX, UnpackDFMLib_TLB;

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

uses ComServ, SysUtils, Dialogs, Classes;

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
var
  src : TFileStream;
  dst : TFileStream;
begin
  try

    src := TFileStream.Create(fileSrc, fmOpenRead);
    try
      dst := TFileStream.Create(fileDst, fmCreate);
      try
        ObjectResourceToText(src, dst);
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

initialization
  TAutoObjectFactory.Create(ComServer, TWinMergeScript, Class_WinMergeScript,
    ciMultiInstance, tmApartment);
end.

