Option Explicit
''
' This script creates a list of (hopefully) unused resource IDs.
'
' Copyright (C) 2008 by Tim Gerundt
' Released under the "GNU General Public License"
'
' ID line follows -- this is updated by SVN
' $Id$

Const ForReading = 1

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
  Dim oHIds, oRcIds, oRc2Ids, oCppIds, sKey, oTxtFile
  Dim StartTime, EndTime, Seconds
  
  StartTime = Time
  
  InfoBox "Creating list of unused resource IDs...", 3
  
  Set oHIds = GetIdsFromHeaderFile("..\..\Src\resource.h")
  Set oRcIds = GetIdsFromResourceFile("..\..\Src\Merge.rc")
  Set oRc2Ids = GetIdsFromResourceFile("..\..\Src\Merge2.rc")
  
  Set oCppIds = CreateObject("Scripting.Dictionary")
  GetIdsFromCppFiles "..\..\Src\", oCppIds
  
  Set oTxtFile = oFSO.CreateTextFile("UnusedResources.txt", True)
  
  oTxtFile.WriteLine "----------------------------------------"
  oTxtFile.WriteLine "ONLY IN RESOURCE.H FILE"
  oTxtFile.WriteLine "----------------------------------------"
  For Each sKey In oHIds.Keys 'For all header IDs...
    If (oRcIds.Exists(sKey) = False) And (oRc2Ids.Exists(sKey) = False) And (oCppIds.Exists(sKey) = False) Then 'If header ID is NOT used...
      oTxtFile.WriteLine sKey
    End If
  Next
  
  oTxtFile.WriteLine ""
  oTxtFile.WriteLine "----------------------------------------"
  oTxtFile.WriteLine "NOT USED IN *.CPP/*.H FILES"
  oTxtFile.WriteLine "----------------------------------------"
  For Each sKey In oRcIds.Keys 'For all RC IDs...
    If (oCppIds.Exists(sKey) = False) Then 'If RC ID is NOT used...
      oTxtFile.WriteLine sKey
    End If
  Next
  For Each sKey In oRc2Ids.Keys 'For all RC2 IDs...
    If (oCppIds.Exists(sKey) = False) Then 'If RC2 ID is NOT used...
      oTxtFile.WriteLine sKey
    End If
  Next
  
  oTxtFile.Close
  
  EndTime = Time
  Seconds = DateDiff("s", StartTime, EndTime)
  
  InfoBox "File ""UnusedResources.txt"" created, after " & Seconds & " second(s).", 10
End Sub

''
' ...
Function GetIdsFromHeaderFile(ByVal sHPath)
  Dim oIds, oTextFile, sLine
  Dim reDefineId, oMatch, sId, sResource
  
  Set oIds = CreateObject("Scripting.Dictionary")
  
  Set reDefineId = New RegExp
  reDefineId.Pattern = "^#define (ID\w+)\s+(\d+)$"
  reDefineId.IgnoreCase = False
  
  If (oFSO.FileExists(sHPath) = True) Then 'If the Header file exists...
    Set oTextFile = oFSO.OpenTextFile(sHPath, ForReading)
    Do Until oTextFile.AtEndOfStream = True 'For all lines...
      sLine = Trim(oTextFile.ReadLine)
      
      sId = ""
      sResource = ""
      If reDefineId.Test(sLine) Then 'If ID...
        Set oMatch = reDefineId.Execute(sLine)(0)
        sId = oMatch.SubMatches(0)
        sResource = oMatch.SubMatches(1)
        
        oIds.Add sId, sResource
      End If
    Loop
    oTextFile.Close
  End If
  Set GetIdsFromHeaderFile = oIds
End Function

''
' ...
Function GetIdsFromResourceFile(ByVal sRcPath)
  Dim oIds, oTextFile, sLine, iLine
  Dim reId, oMatch, sId
  
  Set oIds = CreateObject("Scripting.Dictionary")
  
  Set reId = New RegExp
  reId.Pattern = "(AFX_ID[A-Z]?_\w+|ID[A-Z]?_\w+)"
  reId.IgnoreCase = False
  
  If (oFSO.FileExists(sRcPath) = True) Then 'If the RC file exists...
    iLine = 0
    Set oTextFile = oFSO.OpenTextFile(sRcPath, ForReading)
    Do Until oTextFile.AtEndOfStream = True 'For all lines...
      sLine = Trim(oTextFile.ReadLine)
      iLine = iLine + 1
      
      sId = ""
      If reId.Test(sLine) Then 'If ID...
        Set oMatch = reId.Execute(sLine)(0)
        sId = oMatch.SubMatches(0)
        
        If (oIds.Exists(sId) = True) Then 'If the key is already used...
          oIds(sId) = oIds(sId) & vbTab & iLine
        Else 'If the key is NOT already used...
          oIds.Add sId, iLine
        End If
      End If
    Loop
    oTextFile.Close
  End If
  Set GetIdsFromResourceFile = oIds
End Function

''
' ...
Function GetIdsFromCppFiles(ByVal sFolderPath, ByRef oIds)
  Dim oFolder, oFile, oSubFolder, sExtension
  Dim oTextFile, sLine, iLine
  Dim reId, oMatches, oMatch, sId
  
  Set reId = New RegExp
  reId.Pattern = "(AFX_ID[A-Z]?_\w+|ID[A-Z]?_\w+)"
  reId.IgnoreCase = False
  reId.Global = True
  
  If (oFSO.FolderExists(sFolderPath) = True) Then 'If the folder exists...
    Set oFolder = oFSO.GetFolder(sFolderPath)
    
    For Each oFile In oFolder.Files 'For all files...
      sExtension = LCase(oFSO.GetExtensionName(oFile.Name))
      If (sExtension = "cpp") Or (sExtension = "h") And (oFile.Name <> "resource.h") Then 'If a CPP/H file...
        iLine = 0
        Set oTextFile = oFSO.OpenTextFile(oFile.Path, ForReading)
        Do Until oTextFile.AtEndOfStream = True 'For all lines...
          sLine = Trim(oTextFile.ReadLine)
          iLine = iLine + 1
          
          sId = ""
          If reId.Test(sLine) Then 'If ID...
            Set oMatches = reId.Execute(sLine)
            For Each oMatch In oMatches 'For all results...
              sId = oMatch.SubMatches(0)
              
              If (oIds.Exists(sId) = True) Then 'If the key is already used...
                oIds(sId) = oIds(sId) & vbTab & oFile.Name & ":" & iLine
              Else 'If the key is NOT already used...
                oIds.Add sId, oFile.Name & ":" & iLine
              End If
            Next
          End If
        Loop
        oTextFile.Close
      End If
    Next
    
    For Each oSubFolder In oFolder.SubFolders 'For all folders...
      If (oSubFolder.Name <> ".svn") Then 'If NOT a SVN folder...
        GetIdsFromCppFiles oSubFolder.Path, oIds
      End If
    Next
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
