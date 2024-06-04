////
// This script checks the menu resources.
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
  
  InfoBox("Checking menu resources....", 3);
  
  var oMenuItems = GetMenuItemsFromRcFile("..\\..\\Src\\Merge.rc");
  var oIds = GetIdsFromRcFile("..\\..\\Src\\Merge.rc");
  
  var oTxtFile = oFSO.CreateTextFile("MenuResources.txt", true);
  
  oTxtFile.WriteLine("----------------------------------------");
  oTxtFile.WriteLine("NO MNEMONIC CHAR");
  oTxtFile.WriteLine("----------------------------------------");
  for (var sKey in oMenuItems) { //For all menu items...
    var oMenuItem = oMenuItems[sKey];
    if (oMenuItem.Str.indexOf("&") === -1) { //If WITHOUT mnemonic...
      oTxtFile.WriteLine(oMenuItem.Str + "   (" + oMenuItem.Menu + " -> " + oMenuItem.Id + ")");
    }
  }
  
  oTxtFile.WriteLine("");
  oTxtFile.WriteLine("----------------------------------------");
  oTxtFile.WriteLine("NO DESCRIPTION TEXT");
  oTxtFile.WriteLine("----------------------------------------");
  for (var sKey in oMenuItems) { //For all menu items...
    var oMenuItem = oMenuItems[sKey];
    if (oMenuItem.Id !== "") { //If NOT empty...
      if (!(oMenuItem.Id in oIds)) { //If WITHOUT description...
        oTxtFile.WriteLine(oMenuItem.Str + "   (" + oMenuItem.Menu + " -> " + oMenuItem.Id + ")");
      }
    }
  }
  
  oTxtFile.Close();
  
  var EndTime = new Date().getTime();
  var Seconds = (EndTime - StartTime) / 1000.0;
  
  InfoBox("File \"MenuResources.txt\" created, after " + Seconds + " second(s).", 10);
}

////
// ...
function GetMenuItemsFromRcFile(sRcFilePath) {
  var oMenuItems = {};
  if (oFSO.FileExists(sRcFilePath)) { //If the RC file exists...
    var iCount = 0;
    var iBlockType = NO_BLOCK;
    var sMenu = "";
    var oRcFile = oFSO.OpenTextFile(sRcFilePath, ForReading);
    while (!oRcFile.AtEndOfStream) { //For all lines...
      var sLine = oRcFile.ReadLine().replace(/^\s+|\s+$/g, "");
      var sString = "";
      var sId = "";
      var oMatch = /(IDR_\w+) MENU/.exec(sLine);
      if (oMatch) { //MENU...
        iBlockType = MENU_BLOCK;
        sMenu = oMatch[1];
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
        if (iBlockType === MENU_BLOCK) {
          if (sLine.indexOf("\"") >= 0) { //If quote found (for speedup)...
            var oMatch = /"(.*)"/.exec(sLine);
            if (oMatch && sLine.indexOf("_POPUP_") === -1) { //If "string"...
              sString = oMatch[1];
              oMatch = /(ID_\w+)/.exec(sLine);
              if (sString !== "") { //If NOT empty...
                if (oMatch) { //If ID...
                  sId = oMatch[1];
                }
                iCount++;
              }
            }
          }
        }
      }
      
      if (sString !== "") {
        var oMenuItem = { "Menu": "", "Str": "", "Id": "" };
        oMenuItem.Menu = sMenu;
        oMenuItem.Str = sString;
        oMenuItem.Id = sId;
        oMenuItems[iCount] = oMenuItem;
      }
    }
    oRcFile.Close();
  }
  return oMenuItems;
}

////
// ...
function GetIdsFromRcFile(sRcPath) {
  var oIds = {};
  var reId = /(ID_\w+)\s+\"(.*)\"/;
  if (oFSO.FileExists(sRcPath)) { //If the RC file exists...
    var oTextFile = oFSO.OpenTextFile(sRcPath, ForReading);
    while (!oTextFile.AtEndOfStream) { //For all lines...
      var sLine = oTextFile.ReadLine().replace(/^\s+|\s+$/g, "");
      var oMatch = reId.exec(sLine); //If ID...
      if (oMatch) {
        var sId = oMatch[1];
        var sString = oMatch[2];
        if (!(sId in oIds)) { //If the key is NOT already used...
          oIds[sId] = sString;
        }
      }
    }
    oTextFile.Close();
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

