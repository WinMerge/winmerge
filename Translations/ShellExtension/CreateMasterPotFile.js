////
// This script creates the master POT file (English.pot) for the shell extension.
//
// Copyright (C) 2007-2009 by Tim Gerundt
// Released under the "GNU General Public License"

var ForReading = 1;

var NO_BLOCK = 0;
var STRINGTABLE_BLOCK = 1;

var PATH_ENGLISH_POT = "English.pot";
var PATH_SHELLEXTTEMPLATE_RC = "../../ShellExtension/ShellExtension/ShellExtension.rc";

var oFSO = new ActiveXObject("Scripting.FileSystemObject");

var bRunFromCmd = false;
if (oFSO.GetFileName(WScript.FullName).toLowerCase() === "cscript.exe") {
  bRunFromCmd = true;
}

Main();

////
// ...
function Main() {
  var StartTime = new Date().getTime();
  
  InfoBox("Creating POT file from ShellExtension.rc...", 3);
  
  var bNecessary = true;
  if (oFSO.FileExists(PATH_ENGLISH_POT) && oFSO.FileExists(PATH_SHELLEXTTEMPLATE_RC)) { //If the POT and RC file exists...
    bNecessary = GetArchiveBit(PATH_SHELLEXTTEMPLATE_RC) || GetArchiveBit(PATH_ENGLISH_POT); //RCs or POT file changed?
  }
  
  if (bNecessary) { //If update necessary...
    var oStrings = GetStringsFromRcFile(PATH_SHELLEXTTEMPLATE_RC);
    CreateMasterPotFile(PATH_ENGLISH_POT, oStrings);
    SetArchiveBit(PATH_SHELLEXTTEMPLATE_RC, false);
    SetArchiveBit(PATH_ENGLISH_POT, false);
    for (var it = new Enumerator(oFSO.GetFolder(".").Files); !it.atEnd(); it.moveNext()) { //For all files in the current folder...
      var oFile = it.item();
      if (oFSO.GetExtensionName(oFile.Name).toLowerCase() === "po") { //If a PO file...
        SetArchiveBit(oFile.Path, true);
      }
    }
    
    var EndTime = new Date().getTime();
    var Seconds = (EndTime - StartTime) / 1000.0;
    
    InfoBox("POT file created, after " + Seconds + " second(s).", 10);
  } else { //If update NOT necessary...
    InfoBox("POT file already up-to-date.", 10);
  }
}

////
// ...
function GetStringsFromRcFile(sRcFilePath) {
  var oStrings = {};
  if (oFSO.FileExists(sRcFilePath)) { //If the RC file exists...
    var sRcFileName = oFSO.GetFileName(sRcFilePath);
    var iLine = 0;
    var iBlockType = NO_BLOCK;
    var oRcFile = oFSO.OpenTextFile(sRcFilePath, ForReading);
    while (!oRcFile.AtEndOfStream) { //For all lines...
      var sLine = oRcFile.ReadLine().replace(/^\s+|\s+$/g, "");
      iLine++;
      
      var sReference = sRcFileName + ":" + iLine;
      var sString = "";
      var sComment = "";
      var sContext = "";
      
      if (sLine === "STRINGTABLE") { //STRINGTABLE...
        iBlockType = STRINGTABLE_BLOCK;
      } else if (sLine === "BEGIN") { //BEGIN...
        //IGNORE FOR SPEEDUP!
      } else if (sLine === "END") { //END...
        if (iBlockType === STRINGTABLE_BLOCK) { //If inside stringtable...
          iBlockType = NO_BLOCK;
        }
      } else if (sLine.substring(0, 2) === "//") { //If comment line...
        sLine = "";
        //IGNORE FOR SPEEDUP!
      } else if (sLine !== "") { //If NOT empty line...
        switch (iBlockType) {
          case NO_BLOCK:
            var oMatch0 = /defined\((AFX_TARG_\w*)\)/.exec(sLine);
            var oMatch1 = /LANGUAGE (LANG_\w*, SUBLANG_\w*)/.exec(sLine);
            if (oMatch0) { //AFX_TARG_*...
              sString = oMatch0[1];
              sComment = "AFX_TARG_*";
            } else if (oMatch1) { //LANGUAGE...
              sString = oMatch1[1];
              sComment = "LANGUAGE, SUBLANGUAGE";
            }
            break;

          case STRINGTABLE_BLOCK:
            if (sLine.indexOf("\"") >= 0) { //If quote found (for speedup)...
              //--------------------------------------------------------------------------------
              // Replace 1st string literal only - 2nd string literal specifies control class!
              //--------------------------------------------------------------------------------
              var oMatch2 = /"((?:""|[^"])*)"/.exec(sLine);
              if (oMatch2) { //String...
                var sTemp = oMatch2[1];
                if (sTemp !== "") { //If NOT empty...
                  sString = sTemp.replace(/\"\"/g, "\\\"");
                  var oMatch3 = /\/\/#\. (.*?)$/.exec(sLine);
                  var oMatch4 = /\/\/msgctxt (.*?)$/.exec(sLine);
                  if (oMatch3) { //If found a comment for the translators...
                    sComment = oMatch3[1].replace(/^\s+|\s+$/g, "");
                  } else if (oMatch4) { //If found a context for the translation...
                    sContext = oMatch4[1].replace(/^\s+|\s+$/g, "");
                    sComment = sContext;
                  }
                }
              }
            }
            break;
        }
      }
      
      if (sString !== "") {
        var sKey = sContext + sString;
        var oString = { "Comment": "", "References": "", "Context": "", "Id": "", "Str": "" };
        if (sKey in oStrings) { //If the key is already used...
          oString = oStrings[sKey];
        }
        if (sComment !== "") {
          oString.Comment = sComment;
        }
        if (oString.References !== "") {
          oString.References = oString.References + "\t" + sReference;
        } else {
          oString.References = sReference;
        }
        oString.Context = sContext;
        oString.Id = sString;
        oString.Str = "";
        
        if (sKey in oStrings) { //If the key is already used...
          oStrings[sKey] = oString;
        } else { //If the key is NOT already used...
          oStrings[sContext + sString] = oString;
        }
      }
    }
    oRcFile.Close();
  }
  return oStrings;
}

////
// ...
function CreateMasterPotFile(sPotPath, oStrings) {
  var oPotFile = oFSO.CreateTextFile(sPotPath, true);
  
  oPotFile.WriteLine("# This file is from WinMerge <https://winmerge.org/>");
  oPotFile.WriteLine("# Released under the \"GNU General Public License\"");
  oPotFile.WriteLine("#");
  oPotFile.WriteLine("msgid \"\"");
  oPotFile.WriteLine("msgstr \"\"");
  oPotFile.WriteLine("\"Project-Id-Version: WinMerge Shell Extension\\n\"");
  oPotFile.WriteLine("\"Report-Msgid-Bugs-To: https://github.com/WinMerge/winmerge/issues\\n\"");
  oPotFile.WriteLine("\"POT-Creation-Date: " + GetPotCreationDate() + "\\n\"");
  oPotFile.WriteLine("\"PO-Revision-Date: \\n\"");
  oPotFile.WriteLine("\"Last-Translator: \\n\"");
  oPotFile.WriteLine("\"Language-Team: English <winmerge-translate@lists.sourceforge.net>\\n\"");
  oPotFile.WriteLine("\"MIME-Version: 1.0\\n\"");
  oPotFile.WriteLine("\"Content-Type: text/plain; charset=UTF-8\\n\"");
  oPotFile.WriteLine("\"Content-Transfer-Encoding: 8bit\\n\"");
  oPotFile.WriteLine("\"X-Poedit-Language: English\\n\"");
  oPotFile.WriteLine("\"X-Poedit-SourceCharset: UTF-8\\n\"");
  oPotFile.WriteLine("\"X-Poedit-Basepath: ../../ShellExtension/Languages/\\n\"");
  //oPotFile.WriteLine("\"X-Generator: CreateMasterPotFile.js\\n\"");
  oPotFile.WriteLine("");
  for (var sKey in oStrings) { //For all strings...
    var oString = oStrings[sKey];
    if (oString.Comment !== "") { //If comment exists...
      oPotFile.WriteLine("#. " + oString.Comment);
    }
    var aReferences = oString.References.split("\t");
    for (var i = 0; i < aReferences.length; i++) { //For all references...
      oPotFile.WriteLine("#: " + aReferences[i]);
    }
    oPotFile.WriteLine("#, c-format");
    if (oString.Context !== "") { //If context exists...
      oPotFile.WriteLine("msgctxt \"" + oString.Context + "\"");
    }
    oPotFile.WriteLine("msgid \"" + oString.Id + "\"");
    oPotFile.WriteLine("msgstr \"\"");
    oPotFile.WriteLine("");
  }
  oPotFile.Close();
}

////
// ...
function GetPotCreationDate() {
  var oNow = new Date();
  var sYear = oNow.getFullYear();
  var sMonth = oNow.getMonth() + 1;
  if (sMonth < 10) { sMonth = "0" + sMonth; }
  var sDay = oNow.getDate();
  if (sDay < 10) { sDay = "0" + sDay; }
  var sHour = oNow.getHours();
  if (sHour < 10) { sHour = "0" + sHour; }
  var sMinute = oNow.getMinutes();
  if (sMinute < 10) { sMinute = "0" + sMinute; }
  
  return sYear + "-" + sMonth + "-" + sDay + " " + sHour + ":" + sMinute + "+0000";
}

////
// ...
function InfoBox(sText, iSecondsToWait) {
  if (!bRunFromCmd) { //If run from command line...
    var oShell = WScript.CreateObject("WScript.Shell");
    return oShell.Popup(sText, iSecondsToWait, WScript.ScriptName, 64);
  } else { //If NOT run from command line...
    WScript.Echo(sText);
  }
}

////
// ...
function GetArchiveBit(sFilePath) {
  if (oFSO.FileExists(sFilePath)) { //If the file exists...
    var oFile = oFSO.GetFile(sFilePath);
    if (oFile.Attributes & 32) { //If archive bit set...
      return true;
    }
  }
  return false;
}

////
// ...
function SetArchiveBit(sFilePath, bValue) {
  if (oFSO.FileExists(sFilePath)) { //If the file exists...
    var oFile = oFSO.GetFile(sFilePath);
    if (oFile.Attributes & 32) { //If archive bit set...
      if (!bValue) {
        oFile.Attributes -= 32;
      }
    } else { //If archive bit NOT set...
      if (bValue) {
        oFile.Attributes += 32;
      }
    }
  }
}
