unit UnpackDFMLib_TLB;

// ************************************************************************ //
// WARNING                                                                  //
// -------                                                                  //
// The types declared in this file were generated from data read from a     //
// Type Library. If this type library is explicitly or indirectly (via      //
// another type library referring to this type library) re-imported, or the //
// 'Refresh' command of the Type Library Editor activated while editing the //
// Type Library, the contents of this file will be regenerated and all      //
// manual modifications will be lost.                                       //
// ************************************************************************ //

// PASTLWTR : $Revision$
// File generated on 01/11/03 12:34:14 from Type Library described below.

// ************************************************************************ //
// Type Lib: C:\wjb\Delphi\ActiveX\WinMerge\UnpackDFM\UnpackDFM.tlb
// IID\LCID: {DA4E2505-F060-11D7-9350-444553540000}\0
// Helpfile: 
// HelpString: UnpackDFM - DO NOT REGISTER TYPE LIBRARY
// Version:    1.0
// ************************************************************************ //

interface

uses Windows, ActiveX, Classes, Graphics, OleCtrls, StdVCL;

// *********************************************************************//
// GUIDS declared in the TypeLibrary. Following prefixes are used:      //
//   Type Libraries     : LIBID_xxxx                                    //
//   CoClasses          : CLASS_xxxx                                    //
//   DISPInterfaces     : DIID_xxxx                                     //
//   Non-DISP interfaces: IID_xxxx                                      //
// *********************************************************************//
const
  LIBID_UnpackDFMLib: TGUID = '{DA4E2505-F060-11D7-9350-444553540000}';
  IID_IWinMergeScript: TGUID = '{DA4E2506-F060-11D7-9350-444553540000}';
  CLASS_WinMergeScript: TGUID = '{DA4E2508-F060-11D7-9350-444553540000}';
type

// *********************************************************************//
// Forward declaration of interfaces defined in Type Library            //
// *********************************************************************//
  IWinMergeScript = interface;
  IWinMergeScriptDisp = dispinterface;

// *********************************************************************//
// Declaration of CoClasses defined in Type Library                     //
// (NOTE: Here we map each CoClass to its Default Interface)            //
// *********************************************************************//
  WinMergeScript = IWinMergeScript;


// *********************************************************************//
// Interface: IWinMergeScript
// Flags:     (4416) Dual OleAutomation Dispatchable
// GUID:      {DA4E2506-F060-11D7-9350-444553540000}
// *********************************************************************//
  IWinMergeScript = interface(IDispatch)
    ['{DA4E2506-F060-11D7-9350-444553540000}']
    function Get_PluginEvent: WideString; safecall;
    function Get_PluginDescription: WideString; safecall;
    function Get_PluginFileFilters: WideString; safecall;
    function Get_PluginIsAutomatic: WordBool; safecall;
    function UnpackFile(const fileSrc: WideString; const fileDst: WideString; 
                        var pChanged: WordBool; var pSubcode: Integer): WordBool; safecall;
    function PackFile(const fileSrc: WideString; const fileDst: WideString; var pChanged: WordBool; 
                      Subcode: Integer): WordBool; safecall;
    property PluginEvent: WideString read Get_PluginEvent;
    property PluginDescription: WideString read Get_PluginDescription;
    property PluginFileFilters: WideString read Get_PluginFileFilters;
    property PluginIsAutomatic: WordBool read Get_PluginIsAutomatic;
  end;

// *********************************************************************//
// DispIntf:  IWinMergeScriptDisp
// Flags:     (4416) Dual OleAutomation Dispatchable
// GUID:      {DA4E2506-F060-11D7-9350-444553540000}
// *********************************************************************//
  IWinMergeScriptDisp = dispinterface
    ['{DA4E2506-F060-11D7-9350-444553540000}']
    property PluginEvent: WideString readonly dispid 1;
    property PluginDescription: WideString readonly dispid 2;
    property PluginFileFilters: WideString readonly dispid 3;
    property PluginIsAutomatic: WordBool readonly dispid 4;
    function UnpackFile(const fileSrc: WideString; const fileDst: WideString; 
                        var pChanged: WordBool; var pSubcode: Integer): WordBool; dispid 7;
    function PackFile(const fileSrc: WideString; const fileDst: WideString; var pChanged: WordBool; 
                      Subcode: Integer): WordBool; dispid 8;
  end;

  CoWinMergeScript = class
    class function Create: IWinMergeScript;
    class function CreateRemote(const MachineName: string): IWinMergeScript;
  end;

implementation

uses ComObj;

class function CoWinMergeScript.Create: IWinMergeScript;
begin
  Result := CreateComObject(CLASS_WinMergeScript) as IWinMergeScript;
end;

class function CoWinMergeScript.CreateRemote(const MachineName: string): IWinMergeScript;
begin
  Result := CreateRemoteComObject(MachineName, CLASS_WinMergeScript) as IWinMergeScript;
end;

end.
