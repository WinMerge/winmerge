////
// This script creates the master POT file (English.pot).
//
// Copyright (C) 2007-2009 by Tim Gerundt
// Released under the "GNU General Public License"

var ForReading = 1;

var NO_BLOCK = 0;
var MENU_BLOCK = 1;
var DIALOGEX_BLOCK = 2;
var STRINGTABLE_BLOCK = 3;
var VERSIONINFO_BLOCK = 4;
var ACCELERATORS_BLOCK = 5;

var PATH_ENGLISH_POT = "English.pot";
var PATH_MERGE_RC = "../../Src/Merge.rc";
var PATH_PLUGIN_STRINGS_RC = "../../Plugins/Strings.rc";

var oFSO = new ActiveXObject("Scripting.FileSystemObject");

var bRunFromCmd = false;
if (oFSO.GetFileName(WScript.FullName).toLowerCase() === "cscript.exe") {
  bRunFromCmd = true;
}
var bInsertLineNumbers = false;
if (WScript.Arguments.Named.Exists("InsertLineNumbers")) {
  bInsertLineNumbers = (WScript.Arguments.Named("InsertLineNumbers").toLowerCase() === "true");
}

Main();

////
// ...
function Main() {
  var StartTime = new Date().getTime();
  
  InfoBox("Creating POT file from Merge.rc...", 3);
  
  var bNecessary = true;
  if (oFSO.FileExists(PATH_ENGLISH_POT)) { //If the POT file exists...
    bNecessary = GetArchiveBit(PATH_MERGE_RC) || GetArchiveBit(PATH_ENGLISH_POT); //RCs or POT file changed?
  }
  
  if (bNecessary) { //If update necessary...
    var oStrings = MergeDictionaries(
      GetStringsFromRcFile(PATH_MERGE_RC),
      GetStringsFromRcFile(PATH_PLUGIN_STRINGS_RC));
    CreateMasterPotFile(PATH_ENGLISH_POT, oStrings);
    SetArchiveBit(PATH_MERGE_RC, false);
    SetArchiveBit(PATH_PLUGIN_STRINGS_RC, false);
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
function MergeDictionaries(dict1, dict2) {
  var mergedDict = {};
  for (var key in dict1) {
    mergedDict[key] = dict1[key];
  }

  for (var key in dict2) {
    if (!(key in mergedDict)) {
      mergedDict[key] = dict2[key];
    }
  }
  return mergedDict;
}

////
// ...
function GetStringsFromRcFile(sRcFilePath) {
  var fContinuation = false;
  var oBlacklist = GetStringBlacklist("StringBlacklist.txt");
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
      
      if (fContinuation) {
        // Nothing to do
      } else if (sLine.indexOf(" MENU") >= 0 && sLine.indexOf("IDR_") >= 0) { //MENU...
        iBlockType = MENU_BLOCK;
      } else if (sLine.indexOf(" DIALOGEX") >= 0) { //DIALOGEX...
        iBlockType = DIALOGEX_BLOCK;
      } else if (sLine === "STRINGTABLE") { //STRINGTABLE...
        iBlockType = STRINGTABLE_BLOCK;
      } else if (sLine.indexOf(" VERSIONINFO") >= 0) { //VERSIONINFO...
        iBlockType = VERSIONINFO_BLOCK;
      } else if (sLine.indexOf(" ACCELERATORS") >= 0) { //ACCELERATORS...
        iBlockType = ACCELERATORS_BLOCK;
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
            var oMatch0 = /LANGUAGE (LANG_\w*, SUBLANG_\w*)/.exec(sLine);
            if (oMatch0) { //LANGUAGE...
              sString = oMatch0[1];
              sComment = "LANGUAGE, SUBLANGUAGE";
            }
            break;

          case MENU_BLOCK:
          case DIALOGEX_BLOCK:
          case STRINGTABLE_BLOCK:
            if (sLine.indexOf("\"") >= 0) { //If quote found (for speedup)...
              //--------------------------------------------------------------------------------
              // Replace 1st string literal only - 2nd string literal specifies control class!
              //--------------------------------------------------------------------------------
              var oMatch1 = /NC_\s*\("([^"]*)"\s*,\s*"([^"]*)"\s*\)/.exec(sLine);
              var oMatch2 = /"((?:""|[^"])*)"/.exec(sLine);
              var sTemp;
              if (oMatch1) { //String...
                sContext = oMatch1[1].replace(/^\s+|\s+$/g, "");
                sTemp = oMatch1[2];
              } else if (oMatch2) { //String...
                sTemp = oMatch2[1];
              } else {
                sTemp = "";
              }
              if (sTemp !== "" && !(sTemp in oBlacklist)) { //If NOT blacklisted...
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
            break;

          case VERSIONINFO_BLOCK:
            var oMatch5 = /BLOCK "([0-9A-F]+)"/.exec(sLine);
            var oMatch6 = /VALUE "Comments", "(.*?)\\?0?"/.exec(sLine);
            var oMatch7 = /VALUE "Translation", (.*?)$/.exec(sLine);
            if (oMatch5) { //StringFileInfo.Block...
              sString = oMatch5[1];
              sComment = "StringFileInfo.Block";
            } else if (oMatch6) { //StringFileInfo.Comments...
              sString = oMatch6[1];
              sComment = "You should use a string like \"Translated by \" followed by the translator names for this string. It is ONLY VISIBLE in the StringFileInfo.Comments property from the final resource file!";
            } else if (oMatch7) { //VarFileInfo.Translation...
              sString = oMatch7[1];
              sComment = "VarFileInfo.Translation";
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
        if (bInsertLineNumbers) {
          if (oString.References !== "") {
            oString.References = oString.References + "\t" + sReference;
          } else {
            oString.References = sReference;
          }
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
      fContinuation = (sLine !== "") && ",|".indexOf(sLine.substring(sLine.length - 1)) >= 0;
    }
    oRcFile.Close();
  }
  return oStrings;
}

////
// ...
function GetStringBlacklist(sTxtFilePath) {
  var oBlacklist = {};
  if (oFSO.FileExists(sTxtFilePath)) { //If the blacklist file exists...
    var oTxtFile = oFSO.OpenTextFile(sTxtFilePath, ForReading);
    while (!oTxtFile.AtEndOfStream) { //For all lines...
      var sLine = oTxtFile.ReadLine().replace(/^\s+|\s+$/g, "");
      if (sLine !== "") {
        if (!(sLine in oBlacklist)) { //If the key is NOT already used...
          oBlacklist[sLine] = true;
        }
      }
    }
    oTxtFile.Close();
  }
  return oBlacklist;
}

////
// ...
function CreateMasterPotFile(sPotPath, oStrings) {
  var oPotFile = oFSO.CreateTextFile(sPotPath, true);
  
  oPotFile.Write("# This file is from WinMerge <https://winmerge.org/>\n");
  oPotFile.Write("# Released under the \"GNU General Public License\"\n");
  oPotFile.Write("#\n");
  oPotFile.Write("msgid \"\"\n");
  oPotFile.Write("msgstr \"\"\n");
  oPotFile.Write("\"Project-Id-Version: WinMerge\\n\"\n");
  oPotFile.Write("\"Report-Msgid-Bugs-To: https://github.com/WinMerge/winmerge/issues/\\n\"\n");
  oPotFile.Write("\"POT-Creation-Date: " + GetPotCreationDate() + "\\n\"\n");
  oPotFile.Write("\"PO-Revision-Date: \\n\"\n");
  oPotFile.Write("\"Last-Translator: \\n\"\n");
  oPotFile.Write("\"Language-Team: English <winmerge-translate@lists.sourceforge.net>\\n\"\n");
  oPotFile.Write("\"MIME-Version: 1.0\\n\"\n");
  oPotFile.Write("\"Content-Type: text/plain; charset=UTF-8\\n\"\n");
  oPotFile.Write("\"Content-Transfer-Encoding: 8bit\\n\"\n");
  oPotFile.Write("\"X-Poedit-Language: English\\n\"\n");
  oPotFile.Write("\"X-Poedit-SourceCharset: UTF-8\\n\"\n");
  oPotFile.Write("\"X-Poedit-Basepath: ../../Src/\\n\"\n");
  //oPotFile.Write("\"X-Generator: CreateMasterPotFile.js\\n\"\n");
  oPotFile.Write("\n");
  for (var sKey in oStrings) { //For all strings...
    var oString = oStrings[sKey];
    if (oString.Comment !== "") { //If comment exists...
      oPotFile.Write("#. " + oString.Comment + "\n");
    }
    if (bInsertLineNumbers) {
      var aReferences = oString.References.split("\t");
      for (var i = 0; i < aReferences.length; i++) { //For all references...
        oPotFile.Write("#: " + aReferences[i] + "\n");
      }
    }
    if (oString.Id.indexOf("%") >= 0) { //If c-format...
      oPotFile.Write("#, c-format\n");
    }
    if (oString.Context !== "") { //If context exists...
      oPotFile.Write("msgctxt \"" + oString.Context + "\"\n");
    }
    oPotFile.Write("msgid \"" + oString.Id + "\"\n");
    oPotFile.Write("msgstr \"\"\n");
    oPotFile.Write("\n");
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
