Option Explicit
''
' This script updates the language PO files from the master POT file.
'
' Copyright (C) 2007-2008 by Tim Gerundt
' Released under the "GNU General Public License"
'
' ID line follows -- this is updated by SVN
' $Id: UpdatePoFilesFromPotFile.vbs 6754 2009-05-16 17:42:05Z kimmov $

Const ForReading = 1

Dim oFSO, oCharsets, bRunFromCmd

Set oFSO = CreateObject("Scripting.FileSystemObject")

bRunFromCmd = False
If LCase(oFSO.GetFileName(Wscript.FullName)) = "cscript.exe" Then
  bRunFromCmd = True
End If

Set oCharsets = CreateObject("Scripting.Dictionary")
oCharsets.Add "CP932", "Shift_JIS"
oCharsets.Add "CP936", "GB2312"
oCharsets.Add "CP949", "EUC-KR"
oCharsets.Add "CP950", "BIG5"
oCharsets.Add "CP1250", "Windows-1250"
oCharsets.Add "CP1251", "Windows-1251"
oCharsets.Add "CP1252", "Windows-1252"
oCharsets.Add "CP1253", "Windows-1253"
oCharsets.Add "CP1254", "Windows-1254"
oCharsets.Add "CP1256", "Windows-1256"
oCharsets.Add "UTF-8", "UTF-8"

Call Main

''
' ...
Sub Main
  Dim oLanguages, oLanguage, sLanguage, sDir, bPotChanged
  Dim oEnglishPotContent, oLanguagePoContent
  Dim StartTime, EndTime, Seconds
  Dim sCharset
  
  StartTime = Time
  
  InfoBox "Updating PO files from POT file...", 3
  
  sDir = oFSO.GetParentFolderName(Wscript.ScriptFullName)
  Set oEnglishPotContent = GetContentFromPoFile(sDir & "\English.pot", sCharset)
  If oEnglishPotContent.Count = 0 Then Err.Raise vbObjectError, "Sub Main", "Error reading content from English.pot"
  bPotChanged = GetArchiveBit("English.pot")
  Set oLanguages = Wscript.Arguments
  If oLanguages.Count = 0 Then Set oLanguages = oFSO.GetFolder(".").Files
  For Each oLanguage In oLanguages 'For all languages...
    sLanguage = CStr(oLanguage)
    If LCase(oFSO.GetExtensionName(sLanguage)) = "po" Then
      If bPotChanged Or GetArchiveBit(sLanguage) Then 'If update necessary...
        If bRunFromCmd Then 'If run from command line...
          Wscript.Echo oFSO.GetFileName(sLanguage)
        End If
        Set oLanguagePoContent = GetContentFromPoFile(sLanguage, sCharset)
        If oLanguagePoContent.Count > 0 Then 'If content exists...
          CreateUpdatedPoFile sLanguage, oEnglishPotContent, oLanguagePoContent, sCharset
        End If
        SetArchiveBit sLanguage, False
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
  Dim sMsgCtxt2, sMsgId2, sMsgStr2, sTranslatorComments, sExtractedComments, sReferences, sFlags
End Class

''
' ...
Function GetContentFromPoFile(ByVal sPoPath, sCharset)
  Dim oContent, oSubContent, oTextFile, sLine
  Dim oMatch, iMsgStarted, sMsgCtxt, sMsgId
  Dim reMsgCtxt, reMsgId, reMsgContinued, reCharset

  Set reMsgCtxt = New RegExp
  reMsgCtxt.Pattern = "^msgctxt ""(.*)""$"
  reMsgCtxt.IgnoreCase = True

  Set reMsgId = New RegExp
  reMsgId.Pattern = "^msgid ""(.*)""$"
  reMsgId.IgnoreCase = True

  Set reMsgContinued = New RegExp
  reMsgContinued.Pattern = "^""(.*)""$"
  reMsgContinued.IgnoreCase = True
  
  ' 
  sCharset = "_autodetect"
  Set reCharset = New RegExp
  reCharset.Pattern = "harset[=:] *(.*)\\n""$"
  reCharset.IgnoreCase = True
  Set oTextFile = oFSO.OpenTextFile(sPoPath, ForReading)
  Do Until oTextFile.AtEndOfStream 'For all lines...
    sLine = Trim(oTextFile.ReadLine)
    If reCharset.Test(sLine) Then
      Set oMatch = reCharset.Execute(sLine)(0)
      sCharset = oCharsets(oMatch.SubMatches(0))
      Exit Do
    End If
  Loop
  oTextFile.Close

  Set oContent = CreateObject("Scripting.Dictionary")
  
  iMsgStarted = 0
  sMsgCtxt = ""
  Set oSubContent = New CSubContent
  Set oTextFile = CreateObject("ADODB.Stream")
  oTextFile.Type = 2 ' adTypeText
  oTextFile.LineSeparator = 10 ' adLF
  oTextFile.Charset = sCharset
  oTextFile.Open
  oTextFile.LoadFromFile(sPoPath)
  Do Until oTextFile.EOS 'For all lines...
    sLine = oTextFile.ReadText(-2) ' -2 = adReadLine
    If Len(sLine) > 0 Then
      If Right(sLine, 1) = vbCR Then
        sLine = Left(sLine, Len(sLine) - 1)
      End If
    End If
    sLine = Trim(sLine)
    If sLine <> "" Then 'If NOT empty line...
      If Left(sLine, 1) <> "#" Then 'If NOT comment line...
        If reMsgCtxt.Test(sLine) Then 'If "msgctxt"...
          iMsgStarted = 1
          Set oMatch = reMsgCtxt.Execute(sLine)(0)
          sMsgCtxt = oMatch.SubMatches(0)
          oSubContent.sMsgCtxt2 = sLine & vbLf
        ElseIf reMsgId.Test(sLine) Then 'If "msgid"...
          iMsgStarted = 2
          Set oMatch = reMsgId.Execute(sLine)(0)
          sMsgId = oMatch.SubMatches(0)
          oSubContent.sMsgId2 = sLine & vbLf
        ElseIf Left(sLine, 8) = "msgstr """ Then 'If "msgstr"...
          iMsgStarted = 3
          oSubContent.sMsgStr2 = sLine & vbLf
        ElseIf reMsgContinued.Test(sLine) Then 'If "msgctxt", "msgid" or "msgstr" continued...
          If iMsgStarted = 1 Then
            sMsgCtxt = sMsgCtxt & oMatch.SubMatches(0)
            oSubContent.sMsgCtxt2 = oSubContent.sMsgCtxt2 & sLine & vbLf
          ElseIf iMsgStarted = 2 Then
            Set oMatch = reMsgContinued.Execute(sLine)(0)
            sMsgId = sMsgId & oMatch.SubMatches(0)
            oSubContent.sMsgId2 = oSubContent.sMsgId2 & sLine & vbLf
          ElseIf iMsgStarted = 3 Then
            oSubContent.sMsgStr2 = oSubContent.sMsgStr2 & sLine & vbLf
          End If
        End If
      Else 'If comment line...
        iMsgStarted = -1
        Select Case Left(sLine, 2)
          Case "#~" 'Obsolete message...
            iMsgStarted = 0
          Case "#." 'Extracted comment...
            oSubContent.sExtractedComments = oSubContent.sExtractedComments & sLine & vbLf
          Case "#:" 'Reference...
            oSubContent.sReferences = oSubContent.sReferences & sLine & vbLf
          Case "#," 'Flag...
            oSubContent.sFlags = oSubContent.sFlags & sLine & vbLf
          Case Else 'Translator comment...
            oSubContent.sTranslatorComments = oSubContent.sTranslatorComments & sLine & vbLf
        End Select
      End If
    ElseIf iMsgStarted <> 0 Then 'If empty line AND there is pending translation...
      iMsgStarted = 0 'Don't process same translation twice
      If sMsgId = "" Then sMsgId = "__head__"
      If (oContent.Exists(sMsgCtxt & sMsgId) = False) Then 'If the key is NOT already used...
        oContent.Add sMsgCtxt & sMsgId, oSubContent
      End If
      sMsgCtxt = ""
      Set oSubContent = New CSubContent
    End If
  Loop
  If iMsgStarted <> 0 Then 'If there is pending translation...
    If (oContent.Exists(sMsgCtxt & sMsgId) = False) Then 'If the key is NOT already used...
      oContent.Add sMsgCtxt & sMsgId, oSubContent
    End If
  End If
  oTextFile.Close
  Set GetContentFromPoFile = oContent
End Function

''
' ...
Sub CreateUpdatedPoFile(ByVal sPoPath, ByVal oEnglishPotContent, ByVal oLanguagePoContent, ByVal sCharset)
  Dim sBakPath, oPoFile, sKey, oEnglish, oLanguage
  
  '--------------------------------------------------------------------------------
  ' Backup the old PO file...
  '--------------------------------------------------------------------------------
  sBakPath = sPoPath & ".bak"
  If oFSO.FileExists(sBakPath) Then
    oFSO.DeleteFile sBakPath
  End If
  oFSO.MoveFile sPoPath, sBakPath
  '--------------------------------------------------------------------------------
  
  Set oPoFile = CreateObject("ADODB.Stream")
  oPoFile.Type = 2 ' adTypeText
  oPoFile.LineSeparator = 10 ' adLF
  oPoFile.Charset = sCharset
  oPoFile.Open
  
  Set oLanguage = oLanguagePoContent("__head__")
  oPoFile.WriteText oLanguage.sTranslatorComments
  oPoFile.WriteText oLanguage.sMsgId2
  oPoFile.WriteText oLanguage.sMsgStr2
  oPoFile.WriteText vbLf
  For Each sKey In oEnglishPotContent.Keys 'For all English content...
    If sKey <> "__head__" Then
      Set oEnglish = oEnglishPotContent(sKey)
      If oLanguagePoContent.Exists(sKey) Then 'If translation exists...
        Set oLanguage = oLanguagePoContent(sKey)
      Else 'If translation NOT exists...
        Set oLanguage = oEnglish
      End If
      oPoFile.WriteText oLanguage.sTranslatorComments
      oPoFile.WriteText oEnglish.sExtractedComments
      oPoFile.WriteText oEnglish.sReferences
      oPoFile.WriteText oLanguage.sFlags
      oPoFile.WriteText oLanguage.sMsgCtxt2
      oPoFile.WriteText oLanguage.sMsgId2
      oPoFile.WriteText oLanguage.sMsgStr2
      oPoFile.WriteText vbLf
    End If
  Next
  oPoFile.SaveToFile sPoPath, 2
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

''
' ...
Function GetArchiveBit(ByVal sFilePath)
  Dim oFile
  
  GetArchiveBit = False
  If (oFSO.FileExists(sFilePath) = True) Then 'If the file exists...
    Set oFile = oFSO.GetFile(sFilePath)
    If (oFile.Attributes AND 32) Then 'If archive bit set...
      GetArchiveBit = True
    End If
  End If
End Function

''
' ...
Sub SetArchiveBit(ByVal sFilePath, ByVal bValue)
  Dim oFile
  
  If (oFSO.FileExists(sFilePath) = True) Then 'If the file exists...
    Set oFile = oFSO.GetFile(sFilePath)
    If (oFile.Attributes AND 32) Then 'If archive bit set...
      If (bValue = False) Then
        oFile.Attributes = oFile.Attributes - 32
      End If
    Else 'If archive bit NOT set...
      If (bValue = True) Then
        oFile.Attributes = oFile.Attributes + 32
      End If
    End If
  End If
End Sub
