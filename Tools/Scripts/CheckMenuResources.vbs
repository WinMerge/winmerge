Option Explicit
''
' This script checks the menu resources.
'
' Copyright (C) 2008 by Tim Gerundt
' Released under the "GNU General Public License"
'
' ID line follows -- this is updated by SVN
' $Id$

Const ForReading = 1

Const NO_BLOCK = 0
Const MENU_BLOCK = 1
Const DIALOGEX_BLOCK = 2
Const STRINGTABLE_BLOCK = 3
Const VERSIONINFO_BLOCK = 4
Const ACCELERATORS_BLOCK = 5

Dim oFSO, bRunFromCmd

Set oFSO = CreateObject("Scripting.FileSystemObject")

bRunFromCmd = False
If LCase(oFSO.GetFileName(Wscript.FullName)) = "cscript.exe" Then
  bRunFromCmd = True
End If

Call Main

''
' ...
Sub Main
  Dim oMenuItems, oMenuItem, oIds, sKey, oTxtFile
  Dim StartTime, EndTime, Seconds
  
  StartTime = Time
  
  InfoBox "Checking menu resources....", 3
  
  Set oMenuItems = GetMenuItemsFromRcFile("..\..\Src\Merge.rc")
  Set oIds = GetIdsFromRcFile("..\..\Src\Merge.rc")
  
  Set oTxtFile = oFSO.CreateTextFile("MenuResources.txt", True)
  
  oTxtFile.WriteLine "----------------------------------------"
  oTxtFile.WriteLine "NO MNEMONIC CHAR"
  oTxtFile.WriteLine "----------------------------------------"
  For Each sKey In oMenuItems.Keys 'For all menu items...
    Set oMenuItem = oMenuItems(sKey)
    If (InStr(oMenuItem.Str, "&") = 0) Then 'If WITHOUT mnemonic...
      oTxtFile.WriteLine oMenuItem.Str + "   (" + oMenuItem.Menu + " -> " + oMenuItem.Id + ")"
    End If
  Next
  
  oTxtFile.WriteLine ""
  oTxtFile.WriteLine "----------------------------------------"
  oTxtFile.WriteLine "NO DESCRIPTION TEXT"
  oTxtFile.WriteLine "----------------------------------------"
  For Each sKey In oMenuItems.Keys 'For all menu items...
    Set oMenuItem = oMenuItems(sKey)
    If (oMenuItem.Id <> "") Then 'If NOT empty...
      If (oIds.Exists(oMenuItem.Id) = False) Then 'If WITHOUT description...
        oTxtFile.WriteLine oMenuItem.Str + "   (" + oMenuItem.Menu + " -> " + oMenuItem.Id + ")"
      End If
    End If
  Next
  
  oTxtFile.Close
  
  EndTime = Time
  Seconds = DateDiff("s", StartTime, EndTime)
  
  InfoBox "File ""MenuResources.txt"" created, after " & Seconds & " second(s).", 10
End Sub

''
' ...
Class CMenuItem
  Dim Menu, Str, Id
End Class

''
' ...
Function GetMenuItemsFromRcFile(ByVal sRcFilePath)
  Dim oMenuItems, oMenuItem, oRcFile, sLine, iCount
  Dim iBlockType, sMenu, sString, sId, oMatch
  
  Set oMenuItems = CreateObject("Scripting.Dictionary")
  
  If (oFSO.FileExists(sRcFilePath) = True) Then 'If the RC file exists...
    iCount = 0
    iBlockType = NO_BLOCK
    sMenu = ""
    Set oRcFile = oFSO.OpenTextFile(sRcFilePath, ForReading)
    Do Until oRcFile.AtEndOfStream = True 'For all lines...
      sLine = Trim(oRcFile.ReadLine)
      
      sString = ""
      sId = ""
      
      If (FoundRegExpMatch(sLine, "(IDR_\w+) MENU", oMatch) = True) Then 'MENU...
        iBlockType = MENU_BLOCK
        sMenu = oMatch.SubMatches(0)
      ElseIf (InStr(sLine, " DIALOGEX") > 0) Then 'DIALOGEX...
        iBlockType = DIALOGEX_BLOCK
      ElseIf (sLine = "STRINGTABLE") Then 'STRINGTABLE...
        iBlockType = STRINGTABLE_BLOCK
      ElseIf (InStr(sLine, " VERSIONINFO") > 0) Then 'VERSIONINFO...
        iBlockType = VERSIONINFO_BLOCK
      ElseIf (InStr(sLine, " ACCELERATORS") > 0) Then 'ACCELERATORS...
        iBlockType = ACCELERATORS_BLOCK
      ElseIf (sLine = "BEGIN") Then 'BEGIN...
        'IGNORE FOR SPEEDUP!
      ElseIf (sLine = "END") Then 'END...
        If (iBlockType = STRINGTABLE_BLOCK) Then 'If inside stringtable...
          iBlockType = NO_BLOCK
        End If
      ElseIf (Left(sLine, 2) = "//") Then 'If comment line...
        sLine = ""
        'IGNORE FOR SPEEDUP!
      ElseIf (sLine <> "") Then 'If NOT empty line...
        If (iBlockType = MENU_BLOCK) Then
          If (InStr(sLine, """") > 0) Then 'If quote found (for speedup)...
            If (FoundRegExpMatch(sLine, """(.*)""", oMatch) = True) And (InStr(sLine, "_POPUP_") = 0) Then 'If "string"...
              sString = oMatch.SubMatches(0)
              If (sString <> "") Then 'If NOT empty...
                If (FoundRegExpMatch(sLine, "(ID_\w+)", oMatch) = True) Then 'If ID...
                  sId = oMatch.SubMatches(0)
                End If
                iCount = iCount + 1
              End If
            End If
          End If
        End If
      End If
      
      If (sString <> "") Then
        Set oMenuItem = New CMenuItem
        oMenuItem.Menu = sMenu
        oMenuItem.Str = sString
        oMenuItem.Id = sId
        oMenuItems.Add iCount, oMenuItem
      End If
    Loop
    oRcFile.Close
  End If
  Set GetMenuItemsFromRcFile = oMenuItems
End Function

''
' ...
Function GetIdsFromRcFile(ByVal sRcPath)
  Dim oIds, oTextFile, sLine
  Dim reId, oMatch, sId, sString
  
  Set oIds = CreateObject("Scripting.Dictionary")
  
  Set reId = New RegExp
  reId.Pattern = "(ID_\w+)\s+""(.*)"""
  reId.IgnoreCase = False
  
  If (oFSO.FileExists(sRcPath) = True) Then 'If the RC file exists...
    Set oTextFile = oFSO.OpenTextFile(sRcPath, ForReading)
    Do Until oTextFile.AtEndOfStream = True 'For all lines...
      sLine = Trim(oTextFile.ReadLine)
      
      sId = ""
      sString = ""
      If reId.Test(sLine) Then 'If ID...
        Set oMatch = reId.Execute(sLine)(0)
        sId = oMatch.SubMatches(0)
        sString = oMatch.SubMatches(1)
        
        If (oIds.Exists(sId) = False) Then 'If the key is NOT already used...
          oIds.Add sId, sString
        End If
      End If
    Loop
    oTextFile.Close
  End If
  Set GetIdsFromRcFile = oIds
End Function

''
' ...
Function FoundRegExpMatch(ByVal sString, ByVal sPattern, ByRef oMatchReturn)
  Dim oRegExp, oMatches
  
  Set oRegExp = New RegExp
  oRegExp.Pattern = sPattern
  oRegExp.IgnoreCase = True
  
  oMatchReturn = Null
  FoundRegExpMatch = False
  If (oRegExp.Test(sString) = True) Then
    Set oMatches = oRegExp.Execute(sString)
    Set oMatchReturn = oMatches(0)
    FoundRegExpMatch = True
  End If
End Function

''
' ...
Function InfoBox(ByVal sText, ByVal iSecondsToWait)
  Dim oShell
  
  If (bRunFromCmd = False) Then 'If run from command line...
    Set oShell = Wscript.CreateObject("WScript.Shell")
    InfoBox = oShell.Popup(sText, iSecondsToWait, Wscript.ScriptName, 64)
  Else 'If NOT run from command line...
    Wscript.Echo sText
  End If
End Function
