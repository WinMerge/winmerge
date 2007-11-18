Option Explicit
''
' This script updates the language PO files from the master POT file.
'
' Copyright (C) 2007 by Tim Gerundt
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
  Dim oLanguages, oLanguage, sLanguage, sDir
  Dim oEnglishPotContent, oLanguagePoContent
  Dim StartTime, EndTime, Seconds
  
  StartTime = Time
  
  InfoBox "Updating PO files from POT file...", 3
  
  sDir = oFSO.GetParentFolderName(Wscript.ScriptFullName)
  Set oEnglishPotContent = GetContentFromPoFile(sDir & "\English.pot")
  If oEnglishPotContent.Count = 0 Then Err.Raise vbObjectError, "Sub Main", "Error reading content from English.pot"
  Set oLanguages = Wscript.Arguments
  If oLanguages.Count = 0 Then Set oLanguages = oFSO.GetFolder(".").Files
  For Each oLanguage In oLanguages 'For all languages...
    sLanguage = CStr(oLanguage)
    If LCase(oFSO.GetExtensionName(sLanguage)) = "po" Then
      If bRunFromCmd Then 'If run from command line...
        Wscript.Echo oFSO.GetFileName(sLanguage)
      End If
      Set oLanguagePoContent = GetContentFromPoFile(sLanguage)
      If oLanguagePoContent.Count > 0 Then 'If content exists...
        CreateUpdatedPoFile sLanguage, oEnglishPotContent, oLanguagePoContent
      End If
    End If
  Next
  
  EndTime = Time
  Seconds = DateDiff("s", StartTime, EndTime)
  
  InfoBox "All PO files updated, after " & Seconds & " second(s).", 10
End Sub

''
' ...
Class CSubContent
  Dim sMsgId2, sMsgStr2, sTranslatorComments, sExtractedComments, sReferences, sFlags
End Class

''
' ...
Function GetContentFromPoFile(ByVal sPoPath)
  Dim oContent, oSubContent, oTextFile, sLine
  Dim oMatch, iMsgStarted, sMsgId
  Dim reMsgId, reMsgContinued

  Set reMsgId = New RegExp
  reMsgId.Pattern = "^msgid ""(.*)""$"
  reMsgId.IgnoreCase = True

  Set reMsgContinued = New RegExp
  reMsgContinued.Pattern = "^""(.*)""$"
  reMsgContinued.IgnoreCase = True
  
  Set oContent = CreateObject("Scripting.Dictionary")
  
  iMsgStarted = 0
  Set oSubContent = New CSubContent
  Set oTextFile = oFSO.OpenTextFile(sPoPath, ForReading)
  Do Until oTextFile.AtEndOfStream 'For all lines...
    sLine = Trim(oTextFile.ReadLine)
    If sLine <> "" Then 'If NOT empty line...
      If Left(sLine, 1) <> "#" Then 'If NOT comment line...
        If reMsgId.Test(sLine) Then 'If "msgid"...
          iMsgStarted = 1
          Set oMatch = reMsgId.Execute(sLine)(0)
          sMsgId = oMatch.SubMatches(0)
          oSubContent.sMsgId2 = sLine & vbCrLf
        ElseIf Left(sLine, 8) = "msgstr """ Then 'If "msgstr"...
          iMsgStarted = 2
          oSubContent.sMsgStr2 = sLine & vbCrLf
        ElseIf reMsgContinued.Test(sLine) Then 'If "msgid" or "msgstr" continued...
          If iMsgStarted = 1 Then
            Set oMatch = reMsgContinued.Execute(sLine)(0)
            sMsgId = sMsgId & oMatch.SubMatches(0)
            oSubContent.sMsgId2 = oSubContent.sMsgId2 & sLine & vbCrLf
          ElseIf iMsgStarted = 2 Then
            oSubContent.sMsgStr2 = oSubContent.sMsgStr2 & sLine & vbCrLf
          End If
        End If
      Else 'If comment line...
        iMsgStarted = -1
        Select Case Left(sLine, 2)
          Case "#." 'Extracted comment...
            oSubContent.sExtractedComments = oSubContent.sExtractedComments & sLine & vbCrLf
          Case "#:" 'Reference...
            oSubContent.sReferences = oSubContent.sReferences & sLine & vbCrLf
          Case "#," 'Flag...
            oSubContent.sFlags = oSubContent.sFlags & sLine & vbCrLf
          Case Else 'Translator comment...
            oSubContent.sTranslatorComments = oSubContent.sTranslatorComments & sLine & vbCrLf
        End Select
      End If
    ElseIf iMsgStarted <> 0 Then 'If empty line AND there is pending translation...
      iMsgStarted = 0 'Don't process same translation twice
      If sMsgId = "" Then sMsgId = "__head__"
      oContent.Add sMsgId, oSubContent
      Set oSubContent = New CSubContent
    End If
  Loop
  oTextFile.Close
  Set GetContentFromPoFile = oContent
End Function

''
' ...
Sub CreateUpdatedPoFile(ByVal sPoPath, ByVal oEnglishPotContent, ByVal oLanguagePoContent)
  Dim sBakPath, oPoFile, sMsgId, oEnglish, oLanguage
  
  '--------------------------------------------------------------------------------
  ' Backup the old PO file...
  '--------------------------------------------------------------------------------
  sBakPath = sPoPath & ".bak"
  If oFSO.FileExists(sBakPath) Then
    oFSO.DeleteFile sBakPath
  End If
  oFSO.MoveFile sPoPath, sBakPath
  '--------------------------------------------------------------------------------
  
  Set oPoFile = oFSO.CreateTextFile(sPoPath, True)
  
  Set oLanguage = oLanguagePoContent("__head__")
  oPoFile.Write oLanguage.sTranslatorComments
  oPoFile.Write oLanguage.sMsgId2
  oPoFile.Write oLanguage.sMsgStr2
  oPoFile.Write vbCrLf
  For Each sMsgId In oEnglishPotContent.Keys 'For all English content...
    If sMsgId <> "__head__" Then
      Set oEnglish = oEnglishPotContent(sMsgId)
      If oLanguagePoContent.Exists(sMsgId) Then 'If translation exists...
        Set oLanguage = oLanguagePoContent(sMsgId)
      Else 'If translation NOT exists...
        Set oLanguage = oEnglish
      End If
      oPoFile.Write oLanguage.sTranslatorComments
      oPoFile.Write oEnglish.sExtractedComments
      oPoFile.Write oEnglish.sReferences
      oPoFile.Write oLanguage.sFlags
      oPoFile.Write oLanguage.sMsgId2
      oPoFile.Write oLanguage.sMsgStr2
      oPoFile.Write vbCrLf
    End If
  Next
  oPoFile.Close
End Sub

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
