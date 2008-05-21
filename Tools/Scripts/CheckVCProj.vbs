Option Explicit
''
' This script checks the file "Merge.vcproj".
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
  Dim oRelativePaths, sKey, sRelativePath, oTxtFile
  Dim StartTime, EndTime, Seconds
  
  StartTime = Time
  
  InfoBox "Checking file ""Merge.vcproj""...", 3
  
  Set oRelativePaths = GetRelativePathsFromVcprojFile("..\..\Src\Merge.vcproj")
  
  Set oTxtFile = oFSO.CreateTextFile("MergeVcproj.txt", True)
  
  oTxtFile.WriteLine "----------------------------------------"
  oTxtFile.WriteLine "FILE NOT FOUND"
  oTxtFile.WriteLine "----------------------------------------"
  For Each sKey In oRelativePaths.Keys 'For all relative paths...
    sRelativePath = "..\..\Src\" & sKey
    If (oFSO.FileExists(sRelativePath) = False) Then 'If the file NOT exists...
      oTxtFile.WriteLine sKey
    End If
  Next
  
  oTxtFile.Close
  
  EndTime = Time
  Seconds = DateDiff("s", StartTime, EndTime)
  
  InfoBox "File ""MergeVcproj.txt"" created, after " & Seconds & " second(s).", 10
End Sub

''
' ...
Function GetRelativePathsFromVcprojFile(ByVal sVcprojPath)
  Dim oRelativePaths, oTextFile, sLine
  Dim reRelativePath, oMatch, sRelativePath
  
  Set oRelativePaths = CreateObject("Scripting.Dictionary")
  
  Set reRelativePath = New RegExp
  reRelativePath.Pattern = "RelativePath\=""(.*)"""
  reRelativePath.IgnoreCase = False
  
  If (oFSO.FileExists(sVcprojPath) = True) Then 'If the vcproj file exists...
    Set oTextFile = oFSO.OpenTextFile(sVcprojPath, ForReading)
    Do Until oTextFile.AtEndOfStream = True 'For all lines...
      sLine = Trim(oTextFile.ReadLine)
      
      sRelativePath = ""
      If reRelativePath.Test(sLine) Then 'If relative path...
        Set oMatch = reRelativePath.Execute(sLine)(0)
        sRelativePath = oMatch.SubMatches(0)
        
        If (oRelativePaths.Exists(sRelativePath) = False) Then 'If the key is NOT already used...
          oRelativePaths.Add sRelativePath, sRelativePath
        End If
      End If
    Loop
    oTextFile.Close
  End If
  Set GetRelativePathsFromVcprojFile = oRelativePaths
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
