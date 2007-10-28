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
  
  Wscript.Echo "Attention: " & Wscript.ScriptName & " can take several seconds to finish!"
  
  sDir = oFSO.GetParentFolderName(Wscript.ScriptFullName)
  Set oEnglishPotContent = GetContentFromPoFile(sDir & "\English.pot")
  If oEnglishPotContent.Count = 0 Then Err.Raise vbObjectError, "Sub Main", "Error reading content from English.pot"
  Set oLanguages = Wscript.Arguments
  If oLanguages.Count = 0 Then Set oLanguages = oFSO.GetFolder(".").Files
  For Each oLanguage In oLanguages 'For all languages...
    sLanguage = CStr(oLanguage)
    If LCase(oFSO.GetExtensionName(sLanguage)) = "po" Then
      Set oLanguagePoContent = GetContentFromPoFile(sLanguage)
      If oLanguagePoContent.Count > 0 Then 'If content exists...
        CreateUpdatedPoFile sLanguage, oEnglishPotContent, oLanguagePoContent
      End If
    End If
  Next
  
  EndTime = Time
  Seconds = DateDiff("s", StartTime, EndTime)
  
  Wscript.Echo Wscript.ScriptName & " finished after " & Seconds & " seconds!"
End Sub

''
' ...
Function GetContentFromPoFile(ByVal sPoPath)
  Dim oContent, oSubContent, oTextFile, sLine
  Dim oMatch, iMsgStarted, sMsgId, sMsgStr, sMsgId2, sMsgStr2
  Dim sTranslatorComments, sExtractedComments, sReferences, sFlags
  
  Set oContent = CreateObject("Scripting.Dictionary")
  If bRunFromCmd Then 'If run from command line...
    Wscript.Echo sPoPath
  End If
  
  If (oFSO.FileExists(sPoPath) = True) Then 'If the PO file exists...
    iMsgStarted = 0
    sMsgId = ""
    sMsgStr = ""
    sMsgId2 = ""
    sMsgStr2 = ""
    sTranslatorComments = ""
    sExtractedComments = ""
    sReferences = ""
    sFlags = ""
    Set oTextFile = oFSO.OpenTextFile(sPoPath, ForReading)
    Do Until oTextFile.AtEndOfStream = True 'For all lines...
      sLine = Trim(oTextFile.ReadLine)
      
      If (sLine <> "") Then 'If NOT empty line...
        If (Left(sLine, 1) <> "#") Then 'If NOT comment line...
          If (Left(sLine, 7) = "msgid """) Then 'If "msgid"...
            iMsgStarted = 1
            sMsgId = GetRegExpSubMatch(sLine, "^msgid ""(.*)""$")
            sMsgId2 = sLine & vbCrLf
          ElseIf (Left(sLine, 8) = "msgstr """) Then 'If "msgstr"...
            iMsgStarted = 2
            sMsgStr = GetRegExpSubMatch(sLine, "^msgstr ""(.*)""$")
            sMsgStr2 = sLine & vbCrLf
          ElseIf (FoundRegExpMatch(sLine, "^""(.*)""$", oMatch) = True) Then 'If "msgid" or "msgstr" continued...
            If (iMsgStarted = 1) Then
              sMsgId = sMsgId & oMatch.SubMatches(0)
              sMsgId2 = sMsgId2 & sLine & vbCrLf
            ElseIf (iMsgStarted = 2) Then
              sMsgStr = sMsgStr & oMatch.SubMatches(0)
              sMsgStr2 = sMsgStr2 & sLine & vbCrLf
            End If
          End If
        Else 'If comment line...
          iMsgStarted = -1
          Select Case Left(sLine, 2)
            Case "#." 'Extracted comment...
              sExtractedComments = sExtractedComments & sLine & vbCrLf
            Case "#:" 'Reference...
              sReferences = sReferences & sLine & vbCrLf
            Case "#," 'Flag...
              sFlags = sFlags & sLine & vbCrLf
            Case Else 'Translator comment...
              sTranslatorComments = sTranslatorComments & sLine & vbCrLf
          End Select
        End If
      Else 'If empty line
        iMsgStarted = 0
      End If
      
      If (iMsgStarted = 0) Then 'If NOT inside a translation...
        Set oSubContent = CreateObject("Scripting.Dictionary")
        
        oSubContent.Add "msgid", sMsgId
        oSubContent.Add "msgstr", sMsgStr
        oSubContent.Add "msgid2", sMsgId2
        oSubContent.Add "msgstr2", sMsgStr2
        oSubContent.Add "#", sTranslatorComments
        oSubContent.Add "#.", sExtractedComments
        oSubContent.Add "#:", sReferences
        oSubContent.Add "#,", sFlags
        If (sMsgId <> "") Then
          oContent.Add sMsgId, oSubContent
        Else
          oContent.Add "__head__", oSubContent
        End If
        sMsgId = ""
        sMsgStr = ""
        sMsgId2 = ""
        sMsgStr2 = ""
        sTranslatorComments = ""
        sExtractedComments = ""
        sReferences = ""
        sFlags = ""
      End If
    Loop
    oTextFile.Close
  End If
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
  If (oFSO.FileExists(sBakPath) = True) Then
    oFSO.DeleteFile sBakPath
  End If
  oFSO.MoveFile sPoPath, sBakPath
  '--------------------------------------------------------------------------------
  
  Set oPoFile = oFSO.CreateTextFile(sPoPath, True)
  
  Set oLanguage = oLanguagePoContent("__head__")
  oPoFile.Write oLanguage("#")
  oPoFile.Write oLanguage("msgid2")
  oPoFile.Write oLanguage("msgstr2")
  oPoFile.Write vbCrLf
  For Each sMsgId In oEnglishPotContent.Keys 'For all English content...
    If (sMsgId <> "__head__") Then
      Set oEnglish = oEnglishPotContent(sMsgId)
      
      If (oLanguagePoContent.Exists(sMsgId) = True) Then 'If translation exists...
        Set oLanguage = oLanguagePoContent(sMsgId)
        
        oPoFile.Write oLanguage("#")
        oPoFile.Write oEnglish("#.")
        oPoFile.Write oEnglish("#:")
        oPoFile.Write oLanguage("#,")
        oPoFile.Write oLanguage("msgid2")
        oPoFile.Write oLanguage("msgstr2")
      Else 'If translation NOT exists...
        oPoFile.Write oEnglish("#")
        oPoFile.Write oEnglish("#.")
        oPoFile.Write oEnglish("#:")
        oPoFile.Write oEnglish("#,")
        oPoFile.Write oEnglish("msgid2")
        oPoFile.Write oEnglish("msgstr2")
      End If
      oPoFile.Write vbCrLf
    End If
  Next
  oPoFile.Close
End Sub

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
Function GetRegExpSubMatch(ByVal sString, ByVal sPattern)
  Dim oMatch
  
  GetRegExpSubMatch = ""
  If (FoundRegExpMatch(sString, sPattern, oMatch)) Then 'If pattern found...
    GetRegExpSubMatch = oMatch.SubMatches(0)
  End If
End Function
