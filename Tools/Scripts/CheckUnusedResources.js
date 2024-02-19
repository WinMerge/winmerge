////
// This script creates a list of (hopefully) unused resource IDs.
//
// Copyright (C) 2008 by Tim Gerundt
// Released under the "GNU General Public License"

var ForReading = 1;

var NO_BLOCK = 0;
var MENU_BLOCK = 1;
var DIALOGEX_BLOCK = 2;
var STRINGTABLE_BLOCK = 3;
var VERSIONINFO_BLOCK = 4;
var ACCELERATORS_BLOCK = 5;


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
  
  InfoBox("Creating list of unused resource IDs...", 3);
  
  var oHIds = GetIdsFromHeaderFile("..\\..\\Src\\resource.h");
  var oRcIds = GetIdsFromResourceFile("..\\..\\Src\\Merge.rc");
  var oRc2Ids = GetIdsFromResourceFile("..\\..\\Src\\Merge2.rc");
  
  var oCppIds = {};
  oCppIds = GetIdsFromCppFiles("..\\..\\Src\\", oCppIds);
  oCppIds = GetIdsFromCppFiles("..\\..\\Externals\\crystaledit\\editlib\\", oCppIds);
  
  var oTxtFile = oFSO.CreateTextFile("UnusedResources.txt", true);
  
  oTxtFile.WriteLine("----------------------------------------");
  oTxtFile.WriteLine("ONLY IN RESOURCE.H FILE");
  oTxtFile.WriteLine("----------------------------------------");
  for (var sKey in oHIds) { //For all header IDs...
    if (!(sKey in oRcIds) && !(sKey in oRc2Ids) && !(sKey in oCppIds)) { //If header ID is NOT used...
      oTxtFile.WriteLine(sKey);
    }
  }
  
  oTxtFile.WriteLine("");
  oTxtFile.WriteLine("----------------------------------------");
  oTxtFile.WriteLine("NOT USED IN *.CPP/*.H FILES");
  oTxtFile.WriteLine("----------------------------------------");
  for (var sKey in oRcIds) { //For all RC IDs...
    if (!(sKey in oCppIds)) { //If RC ID is NOT used...
      oTxtFile.WriteLine(sKey);
    }
  }
  for (var sKey in oRc2Ids) { //For all RC2 IDs...
    if (!(sKey in oCppIds)) { //If RC2 ID is NOT used...
      oTxtFile.WriteLine(sKey);
    }
  }
  
  oTxtFile.Close();
  
  var EndTime = new Date().getTime();
  var Seconds = (EndTime - StartTime) / 1000.0;
  
  InfoBox("File \"UnusedResources.txt\" created, after " + Seconds + " second(s).", 10);
}

////
// ...
function GetIdsFromHeaderFile(sHPath) {
  var oIds = {};
  var reDefineId = /^#define (ID\w+)\s+(\d+)$/;
  if (oFSO.FileExists(sHPath)) { //If the Header file exists...
    var oTextFile = oFSO.OpenTextFile(sHPath, ForReading);
    while (!oTextFile.AtEndOfStream) { //For all lines...
      var sLine = oTextFile.ReadLine().replace(/^\s+|\s+$/g, "");
      var oMatch = reDefineId.exec(sLine); //If ID...
      if (oMatch) {
        var sId = oMatch[1];
        var sResource = oMatch[2];
        oIds[sId] = sResource;
      }
    }
    oTextFile.Close();
  }
  return oIds;
}

////
// ...
function GetIdsFromResourceFile(sRcPath) {
  var reId = /(AFX_ID[A-Z]?_\w+|ID[A-Z]?_\w+)/g;
  var oIds = {};
  if (oFSO.FileExists(sRcPath)) { //If the RC file exists...
    var iLine = 0;
    var iBlockType = NO_BLOCK;
    var oTextFile = oFSO.OpenTextFile(sRcPath, ForReading);
    while (!oTextFile.AtEndOfStream) { //For all lines...
      var sLine = oTextFile.ReadLine().replace(/^\s+|\s+$/g, "");
      iLine++;
      if (sLine.indexOf(" MENU") >= 0) { //MENU...
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
        //if (iBlockType === MENU_BLOCK || iBlockType === DIALOGEX_BLOCK) {
        //  sLine = "";
        //}
      }
      
      var oMatch;
      while (oMatch = reId.exec(sLine)) { //If ID...
        var sId = oMatch[1];
        if (sId in oIds) { //If the key is already used...
          oIds[sId] = oIds[sId] + "\t" + iLine;
        } else { //If the key is NOT already used...
          oIds[sId] = iLine;
        }
      }
    }
    oTextFile.Close();
  }
  return oIds;
}

////
// ...
function GetIdsFromCppFiles(sFolderPath, oIds) {
  var reId = /(AFX_ID[A-Z]?_\w+|ID[A-Z]?_\w+)/g;
  if (oFSO.FolderExists(sFolderPath)) { //If the folder exists...
    var oFolder = oFSO.GetFolder(sFolderPath);
    for (var it = new Enumerator(oFolder.Files); !it.atEnd(); it.moveNext()) { //For all files...
      var oFile = it.item();
      var sExtension = oFSO.GetExtensionName(oFile.Name).toLowerCase();
      if (sExtension === "cpp" || (sExtension == "h" && oFile.Name !== "resource.h")) { //If a CPP/H file...
        var iLine = 0;
        var oTextFile = oFSO.OpenTextFile(oFile.Path, ForReading);
        while (!oTextFile.AtEndOfStream) { //For all lines...
          var sLine = oTextFile.ReadLine().replace(/^\s+|\s+$/g, "");
          iLine++;
          var oMatch;
          while (oMatch = reId.exec(sLine)) { //For all results...
            var sId = oMatch[1];
            if (sId in oIds) { //If the key is already used...
              oIds[sId] = oIds[sId] + "\t" + oFile.Name + ":" + iLine;
            } else { //If the key is NOT already used...
              oIds[sId] = oFile.Name + ":" + iLine;
            }
          }
        }
        oTextFile.Close();
      }
    }
    
    for (var it = new Enumerator(oFolder.SubFolders); !it.atEnd(); it.moveNext()) { //For all folders...
      var oSubFolder = it.item();
      if (oSubFolder.Name !== ".svn" && oSubFolder.Name !== ".hg" && oSubFolder.Name !== ".git") { //If NOT a SVN folder...
        oIds = GetIdsFromCppFiles(oSubFolder.Path, oIds);
      }
    }
  }
  return oIds;
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

