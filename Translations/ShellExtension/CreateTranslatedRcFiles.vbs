Option Explicit
''
' This script creates the translated RC files for the shell extension.
'
' Copyright (C) 2007-2009 by Tim Gerundt
' Released under the "GNU General Public License"
'
' ID line follows -- this is updated by SVN
' $Id: CreateTranslatedRcFiles.vbs 6780 2009-05-23 11:58:54Z gerundt $

Const ForReading = 1

Const NO_BLOCK = 0
Const STRINGTABLE_BLOCK = 1

Const PATH_SHELLEXTTEMPLATE_RC = "../../ShellExtension/Languages/ShellExtensionTemplate.rc"

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
oCharsets.Add "CP1257", "Windows-1257"
oCharsets.Add "UTF-8", "UTF-8"

Call Main

''
' ...
Sub Main
  Dim oLanguages, sLanguage
  Dim oLanguageTranslations, sLanguagePoFilePath
  Dim StartTime, EndTime, Seconds
  Dim sCharset
  
  StartTime = Time
  
  InfoBox "Warning: " & Wscript.ScriptName & " can take several seconds to finish!", 3
  
  Set oLanguages = GetLanguages
  For Each sLanguage In oLanguages.Keys 'For all languages...
    If (bRunFromCmd = True) Then 'If run from command line...
      Wscript.Echo sLanguage
    End If
    Set oLanguageTranslations = GetTranslationsFromPoFile(oLanguages(sLanguage), sCharset)
    If (oLanguageTranslations.Count > 0) Then 'If translations exists...
      CreateRcFileWithTranslations PATH_SHELLEXTTEMPLATE_RC, "../../ShellExtension/Languages/ShellExtension" & sLanguage & ".rc", oLanguageTranslations, sCharset
    End If
  Next
  
  EndTime = Time
  Seconds = DateDiff("s", StartTime, EndTime)
  
  InfoBox Wscript.ScriptName & " finished after " & Seconds & " seconds!", 3
End Sub

''
' ...
Function GetLanguages()
  Dim oLanguages, oFile
  
  Set oLanguages = CreateObject("Scripting.Dictionary")
  
  For Each oFile In oFSO.GetFolder(".").Files 'For all files in the current folder...
    If (LCase(oFSO.GetExtensionName(oFile.Name)) = "po") Then 'If a PO file...
      oLanguages.Add oFSO.GetBaseName(oFile.Name), oFile.Path
    End If
  Next
  Set GetLanguages = oLanguages
End Function

''
' ...
Function GetTranslationsFromPoFile(ByVal sPoPath, sCharset)
  Dim oTranslations, oTextFile, sLine
  Dim oMatch, iMsgStarted, sMsgId, sMsgStr
  Dim reMsgId, reMsgStr, reMsgContinued, reCharset
  
  Set reMsgId = New RegExp
  reMsgId.Pattern = "^msgid ""(.*)""$"
  reMsgId.IgnoreCase = True
  
  Set reMsgStr = New RegExp
  reMsgStr.Pattern = "^msgstr ""(.*)""$"
  reMsgStr.IgnoreCase = True
  
  Set reMsgContinued = New RegExp
  reMsgContinued.Pattern = "^""(.*)""$"
  reMsgContinued.IgnoreCase = True
  
  ' 
  sCharset = "_autodetect"
  Set reCharset = New RegExp
  reCharset.Pattern = "harset=(.*)\\n""$"
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

  Set oTranslations = CreateObject("Scripting.Dictionary")
  
  If (oFSO.FileExists(sPoPath) = True) Then 'If the PO file exists...
    iMsgStarted = 0
    sMsgId = ""
    sMsgStr = ""
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
      
      If (sLine <> "") Then 'If NOT empty line...
        If (Left(sLine, 1) <> "#") Then 'If NOT comment line...
          '--------------------------------------------------------------------------------
          ' Note: We must replace \" temporary with FormFeed and convert them later to ""
          '--------------------------------------------------------------------------------
          sLine = Replace(sLine, "\""", vbFormFeed)
          If reMsgId.Test(sLine) Then 'If "msgid"...
            iMsgStarted = 1
            Set oMatch = reMsgId.Execute(sLine)(0)
            sMsgId = oMatch.SubMatches(0)
          ElseIf reMsgStr.Test(sLine) Then 'If "msgstr"...
            iMsgStarted = 2
            Set oMatch = reMsgStr.Execute(sLine)(0)
            sMsgStr = oMatch.SubMatches(0)
          ElseIf reMsgContinued.Test(sLine) Then 'If "msgid" or "msgstr" continued...
            Set oMatch = reMsgContinued.Execute(sLine)(0)
            If (iMsgStarted = 1) Then
              sMsgId = sMsgId & oMatch.SubMatches(0)
            ElseIf (iMsgStarted = 2) Then
              sMsgStr = sMsgStr & oMatch.SubMatches(0)
            End If
          End If
          sMsgId = Replace(sMsgId, vbFormFeed, """""")
          sMsgStr = Replace(sMsgStr, vbFormFeed, """""")
        End If
      Else 'If empty line
        iMsgStarted = 0
      End If
      
      If (iMsgStarted = 0) Then 'If NOT inside a translation...
        If (sMsgId <> "") And (sMsgStr <> "") And (sMsgId <> sMsgStr) Then 'If translated...
          oTranslations.Add sMsgId, sMsgStr
        End If
        sMsgId = ""
        sMsgStr = ""
      End If
    Loop
    If (sMsgId <> "") And (sMsgStr <> "") And (sMsgId <> sMsgStr) Then 'If translated...
      oTranslations.Add sMsgId, sMsgStr
    End If
    oTextFile.Close
  End If
  Set GetTranslationsFromPoFile = oTranslations
End Function

''
' ...
Sub CreateRcFileWithTranslations(ByVal sMasterRcPath, ByVal sLanguageRcPath, ByVal oTranslations, sCharset)
  Dim oMasterRcFile, sMasterLine
  Dim oLanguageRcFile, sLanguageLine
  Dim iBlockType, oMatches, oMatch, sMsgId, sMsgStr
  Dim reAfxTarg, reLanguage, reCodePage, reString, sTemp
  
  Set reAfxTarg = New RegExp
  reAfxTarg.Pattern = "defined\((AFX_TARG_\w*)\)"
  reAfxTarg.IgnoreCase = True
  
  Set reLanguage = New RegExp
  reLanguage.Pattern = "LANGUAGE (LANG_\w*, SUBLANG_\w*)"
  reLanguage.IgnoreCase = True
  
  Set reCodePage = New RegExp
  reCodePage.Pattern = "code_page\(([\d]+)\)"
  reCodePage.IgnoreCase = True
  
  Set reString = New RegExp
  reString.Pattern = """(.*?)"""
  reString.IgnoreCase = True
  
  If (oFSO.FileExists(sMasterRcPath) = True) Then 'If the master RC file exists...
    iBlockType = NO_BLOCK
    Set oMasterRcFile = oFSO.OpenTextFile(sMasterRcPath, ForReading)
    Set oLanguageRcFile = CreateObject("ADODB.Stream")
    oLanguageRcFile.Type = 2 ' adTypeText
    oLanguageRcFile.LineSeparator = -1 ' adCRLF
    oLanguageRcFile.Charset = sCharset
    oLanguageRcFile.Open
    Do Until oMasterRcFile.AtEndOfStream = True 'For all lines...
      sMasterLine = oMasterRcFile.ReadLine
      sLanguageLine = sMasterLine
      sMasterLine = Trim(sMasterLine) 'Save Masterline trimmed!
      
      If (sMasterLine = "STRINGTABLE") Then 'STRINGTABLE...
        iBlockType = STRINGTABLE_BLOCK
      ElseIf (sMasterLine = "BEGIN") Then 'BEGIN...
        'IGNORE FOR SPEEDUP!
      ElseIf (sMasterLine = "END") Then 'END...
        If (iBlockType = STRINGTABLE_BLOCK) Then 'If inside stringtable...
          iBlockType = NO_BLOCK
        End If
      ElseIf (Left(sMasterLine, 2) = "//") Then 'If comment line...
        'IGNORE FOR SPEEDUP!
      ElseIf (sMasterLine <> "") Then 'If NOT empty line...
        Select Case iBlockType
          Case NO_BLOCK:
            If reAfxTarg.Test(sMasterLine) Then 'AFX_TARG_*...
              Set oMatch = reAfxTarg.Execute(sMasterLine)(0)
              sMsgId = oMatch.SubMatches(0)
              If (sMsgId <> "") And (oTranslations.Exists(sMsgId) = True) Then 'If translation located...
                sMsgStr = oTranslations(sMsgId)
                sLanguageLine = Replace(sLanguageLine, "defined(" & sMsgId, "defined(" & sMsgStr)
              End If
            ElseIf reLanguage.Test(sMasterLine) Then 'LANGUAGE...
              Set oMatch = reLanguage.Execute(sMasterLine)(0)
              sMsgId = oMatch.SubMatches(0)
              If (sMsgId <> "") And (oTranslations.Exists(sMsgId) = True) Then 'If translation located...
                sMsgStr = oTranslations(sMsgId)
                sLanguageLine = Replace(sLanguageLine, "LANGUAGE " & sMsgId, "LANGUAGE " & sMsgStr)
              End If
            ElseIf reCodePage.Test(sMasterLine) Then 'code_page...
              Set oMatch = reCodePage.Execute(sMasterLine)(0)
              sMsgId = oMatch.SubMatches(0)
              If (sMsgId <> "") And (oTranslations.Exists(sMsgId) = True) Then 'If translation located...
                sMsgStr = oTranslations(sMsgId)
                sLanguageLine = Replace(sLanguageLine, "code_page(" & sMsgId & ")", "code_page(" & sMsgStr & ")")
              End If
            End If
            
          Case STRINGTABLE_BLOCK:
            If (InStr(sMasterLine, """") > 0) Then 'If quote found (for speedup)...
              '--------------------------------------------------------------------------------
              ' Note: We must replace "" temporary with FormFeed...
              '--------------------------------------------------------------------------------
              sTemp = Replace(sMasterLine, """""", vbFormFeed)
              If reString.Test(sTemp) Then 'String...
                Set oMatches = reString.Execute(sTemp)
                For Each oMatch In oMatches 'For all strings...
                  sMsgId = Replace(oMatch.SubMatches(0), vbFormFeed, """""")
                  If (sMsgId <> "") And (oTranslations.Exists(sMsgId) = True) Then 'If translation located...
                    sMsgStr = oTranslations(sMsgId)
                    sLanguageLine = Replace(sLanguageLine, """" & sMsgId & """", """" & sMsgStr & """")
                  End If
                Next
              End If
            End If
            
        End Select
      End If
      oLanguageRcFile.WriteText sLanguageLine, 1
    Loop
    oMasterRcFile.Close
    WScript.Echo sLanguageRcPath
    oLanguageRcFile.SaveToFile sLanguageRcPath, 2
    oLanguageRcFile.Close
  End If
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
