////
// This script creates the master POT file (English.pot) for the shell extension.
//
// Copyright (C) 2007-2009 by Tim Gerundt
// Released under the "GNU General Public License

var ForReading = 1;

var NO_BLOCK = 0;
var STRINGTABLE_BLOCK = 1;

var PATH_ENGLISH_POT = "English.pot";
var PATH_SHELLEXTTEMPLATE_RC = "../../ShellExtension/ShellExtension/ShellExtension.rc";

var bRunFromCmd;

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
  if (oFSO.FileExists(PATH_ENGLISH_POT) && oFSO.FileExists(PATH_SHELLEXTTEMPLATE_RC)) { //if the POT and RC file exists...
    bNecessary = GetArchiveBit(PATH_SHELLEXTTEMPLATE_RC) || GetArchiveBit(PATH_ENGLISH_POT); //RCs or POT file changed?
  }
  
  if (bNecessary) { //if update necessary...
    var oStrings = GetStringsFromRcFile(PATH_SHELLEXTTEMPLATE_RC, sCodePage);
    CreateMasterPotFile(PATH_ENGLISH_POT, oStrings, sCodePage);
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
  } else { //if update NOT necessary...
    InfoBox("POT file already up-to-date.", 10);
  }
}

////
// ...
function GetStringsFromRcFile(ByVal sRcFilePath, ByRef sCodePage)
  Dim oStrings, oString, oRcFile, sLine, iLine
  Dim sRcFileName, iBlockType, sReference, sString, sComment, sContext, oMatch, sTemp, sKey
  
  var oStrings = {};
  if (oFSO.FileExists(sRcFilePath)) { //if the RC file exists...
    var sRcFileName = oFSO.GetFileName(sRcFilePath);
    var iLine = 0;
    var iBlockType = NO_BLOCK;
    var sCodePage = "";
    var oRcFile = oFSO.OpenTextFile(sRcFilePath, ForReading);
    while (!oRcFile.AtEndOfStream) { //For all lines...
      var sLine = oRcFile.ReadLine().replace(/^\s+|\s+$/g, "");
      iLine++;
      
      var sReference = sRcFileName + ":" + iLine;
      var sString = "";
      var sComment = "";
      var sContext = "";
      
      if (sLine === "STRINGTABLE") { //STRINGTABLE...
        iBlockType = STRINGTABLE_BLOCK
      } else if (sLine === "BEGIN") { //BEGIN...
        //IGNORE FOR SPEEDUP!
      } else if (sLine === "END") { //END...
        if (iBlockType === STRINGTABLE_BLOCK) { //If inside stringtable...
          iBlockType = NO_BLOCK;
        }
      } else if (sLine.substring(0, 2) === "//") { //If comment line...
        sLine = "";
        //IGNORE FOR SPEEDUP!
      } else if (sLine !== "") { //if NOT empty line...
        switch (iBlockType) {
          Case NO_BLOCK:
            if (FoundRegExpMatch(sLine, "defined\((AFX_TARG_\w*)\)", oMatch)) { //AFX_TARG_*...
              sString = oMatch.SubMatches(0)
              sComment = "AFX_TARG_*"
            } else if (FoundRegExpMatch(sLine, "LANGUAGE (LANG_\w*, function LANG_\w*)", oMatch)) { //LANGUAGE...
              sString = oMatch.SubMatches(0)
              sComment = "LANGUAGE, function LANGUAGE"
            } else if (FoundRegExpMatch(sLine, "code_page\(([\d]+)\)", oMatch)) { //code_page...
              sString = oMatch.SubMatches(0)
              sComment = "Codepage"
              sCodePage = oMatch.SubMatches(0)
            }
            
          Case STRINGTABLE_BLOCK:
            if (sLine.indexOf("\"") >= 0) { //If quote found (for speedup)...
              //--------------------------------------------------------------------------------
              // Replace 1st string literal only - 2nd string literal specifies control class!
              //--------------------------------------------------------------------------------
              if FoundRegExpMatch(sLine, """((?:""""|[^""])*)""", oMatch) { //String...
                sTemp = oMatch.SubMatches(0)
                if (sTemp !== "") { //if NOT empty...
                  sString = Replace(sTemp, """""", "\""")
                  if (FoundRegExpMatch(sLine, "//#\. (.*?)$", oMatch)) { //if found a comment for the translators...
                    sComment = Trim(oMatch.SubMatches(0))
                  } else if (FoundRegExpMatch(sLine, "//msgctxt (.*?)$", oMatch)) { //if found a context for the translation...
                    sContext = Trim(oMatch.SubMatches(0))
                    sComment = sContext
                  }
                }
              }
            }
            
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
          oString.References = oString.References + "\t" + sReference
        } else {
          oString.References = sReference;
        }
        oString.Context = sContext;
        oString.Id = sString;
        oString.Str = "";
        
        if (sKey in oStrings) { //If the key is already used...
          oStrings[sKey] = oString;
        } else { //if the key is NOT already used...
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
function CreateMasterPotFile(sPotPath, oStrings, sCodePage) {
  var oPotFile = oFSO.CreateTextFile(sPotPath, true);
  
  oPotFile.WriteLine("# This file is part from WinMerge <https://winmerge.org/>");
  oPotFile.WriteLine("# Released under the \"GNU General Public License\"");
  oPotFile.WriteLine("#");
  oPotFile.WriteLine("msgid \"\"");
  oPotFile.WriteLine("msgstr \"\"");
  oPotFile.WriteLine("\"Project-Id-Version: WinMerge Shell Extension\n\"");
  oPotFile.WriteLine("\"Report-Msgid-Bugs-To: https://bugs.winmerge.org/\n\"");
  oPotFile.WriteLine("\"POT-Creation-Date: " + GetPotCreationDate() + "\n\"");
  oPotFile.WriteLine("\"PO-Revision-Date: \n\"");
  oPotFile.WriteLine("\"Last-Translator: \n\"");
  oPotFile.WriteLine("\"Language-Team: English <winmerge-translate@lists.sourceforge.net>\n\"");
  oPotFile.WriteLine("\"MIME-Version: 1.0\n\"");
  oPotFile.WriteLine("\"Content-Type: text/plain; charset=CP" + sCodePage + "\n\"");
  oPotFile.WriteLine("\"Content-Transfer-Encoding: 8bit\n\"");
  oPotFile.WriteLine("\"X-Poedit-Language: English\n\"");
  oPotFile.WriteLine("\"X-Poedit-SourceCharset: CP" + sCodePage + "\n\"");
  oPotFile.WriteLine("\"X-Poedit-Basepath: ../../ShellExtension/Languages/\n\"");
  //oPotFile.WriteLine("\"X-Generator: CreateMasterPotFile.js\n\"");
  oPotFile.WriteLine("");
  For (var sKey In oStrings) { //For all strings...
    var oString = oStrings[sKey];
    if (oString.Comment !== "") { //if comment exists...
      oPotFile.WriteLine("#. " + oString.Comment);
    }
    var aReferences = oString.References.split("\t");
    for (var i = 0; i < aReferences.length; i++) { //For all references...
      oPotFile.WriteLine("#: " + aReferences[i]);
    }
    oPotFile.WriteLine("#, c-format");
    if (oString.Context !== "") { //if context exists...
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
  if (!bRunFromCmd) { //if run from command line...
    var oShell = WScript.CreateObject("WScript.Shell");
    return oShell.Popup(sText, iSecondsToWait, WScript.ScriptName, 64);
  } else { //if NOT run from command line...
    WScript.Echo(sText);
  }
}

////
// ...
function GetArchiveBit(sFilePath) {
  if (oFSO.FileExists(sFilePath)) { //if the file exists...
    var oFile = oFSO.GetFile(sFilePath);
    if (oFile.Attributes & 32) { //if archive bit set...
      return true;
    }
  }
  return false;
}

////
// ...
function SetArchiveBit(sFilePath, bValue) {
  if (oFSO.FileExists(sFilePath)) { //if the file exists...
    var oFile = oFSO.GetFile(sFilePath);
    if (oFile.Attributes & 32) { //if archive bit set...
      if (!bValue) {
        oFile.Attributes -= 32;
      }
    } else { //if archive bit NOT set...
      if (bValue) {
        oFile.Attributes += 32;
      }
    }
  }
}
