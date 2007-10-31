Option Explicit
''
' This script creates the language RC files from the language PO files.
'
' Copyright (C) 2007 by Tim Gerundt
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
If (LCase(Right(Wscript.FullName, 11))) = "cscript.exe" Then
  bRunFromCmd = True
End If

Call Main

''
' ...
Sub Main
  Dim oLanguages, sLanguage
  Dim oLanguageTranslations
  Dim StartTime, EndTime, Seconds
  
  StartTime = Time
  
  Wscript.Echo "Warning: " & Wscript.ScriptName & " can take several seconds to finish!"
  
  Set oLanguages = GetLanguages
  For Each sLanguage In oLanguages.Keys 'For all languages...
    If (bRunFromCmd = True) Then 'If run from command line...
      Wscript.Echo sLanguage
    End If
    Set oLanguageTranslations = GetTranslationsFromPoFile(oLanguages(sLanguage), sLanguage)
    If (oLanguageTranslations.Count > 0) Then 'If translations exists...
      CreateRcFileWithTranslations "../Merge.rc", sLanguage & "\Merge" & sLanguage & ".rc", oLanguageTranslations
    End If
  Next
  
  EndTime = Time
  Seconds = DateDiff("s", StartTime, EndTime)
  
  Wscript.Echo Wscript.ScriptName & " finished after " & Seconds & " seconds!"
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
Function GetTranslationsFromPoFile(ByVal sPoPath, ByVal sLanguage)
  Dim oTranslations, oTextFile, sLine
  Dim oMatch, iMsgStarted, sMsgId, sMsgStr
  
  Set oTranslations = CreateObject("Scripting.Dictionary")
  
  If (oFSO.FileExists(sPoPath) = True) Then 'If the PO file exists...
    iMsgStarted = 0
    sMsgId = ""
    sMsgStr = ""
    Set oTextFile = oFSO.OpenTextFile(sPoPath, ForReading)
    Do Until oTextFile.AtEndOfStream = True 'For all lines...
      sLine = Trim(oTextFile.ReadLine)
      
      If (sLine <> "") Then 'If NOT empty line...
        If (Left(sLine, 1) <> "#") Then 'If NOT comment line...
          '--------------------------------------------------------------------------------
          ' Note: We must replace \" temporary with FormFeed and convert them later to ""
          '--------------------------------------------------------------------------------
          sLine = Replace(sLine, "\""", vbFormFeed)
          If (Left(sLine, 7) = "msgid """) Then 'If "msgid"...
            iMsgStarted = 1
            sMsgId = GetRegExpSubMatch(sLine, "^msgid ""(.*)""$")
          ElseIf (Left(sLine, 8) = "msgstr """) Then 'If "msgstr"...
            iMsgStarted = 2
            sMsgStr = GetRegExpSubMatch(sLine, "^msgstr ""(.*)""$")
          ElseIf (FoundRegExpMatch(sLine, "^""(.*)""$", oMatch) = True) Then 'If "msgid" or "msgstr" continued...
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
    oTextFile.Close
    
    '--------------------------------------------------------------------------------
    ' Add additional translations...
    '--------------------------------------------------------------------------------
    If (oTranslations.Exists("WinMerge Application") = False) Then
      oTranslations.Add "WinMerge Application", sLanguage & " translation for WinMerge"
    End If
    If (oTranslations.Exists("WinMerge.EXE") = False) Then
      oTranslations.Add "WinMerge.EXE", "Merge" & sLanguage & ".lang"
    End If
    '--------------------------------------------------------------------------------
  End If
  Set GetTranslationsFromPoFile = oTranslations
End Function

''
' ...
Sub CreateRcFileWithTranslations(ByVal sMasterRcPath, ByVal sLanguageRcPath, ByVal oTranslations)
  Dim oMasterRcFile, sMasterLine
  Dim oLanguageRcFile, sLanguageLine, sLanguageRcFolder
  Dim iBlockType, oMatches, oMatch, sMsgId, sMsgStr
  
  If (oFSO.FileExists(sMasterRcPath) = True) Then 'If the master RC file exists...
    '--------------------------------------------------------------------------------
    ' Create the language RC folder, if necessary...
    '--------------------------------------------------------------------------------
    sLanguageRcFolder = oFSO.GetParentFolderName(sLanguageRcPath)
    If (oFSO.FolderExists(sLanguageRcFolder) = False) Then 'If the language RC folder NOT exists...
      oFSO.CreateFolder sLanguageRcFolder
    End If
    '--------------------------------------------------------------------------------
    
    iBlockType = NO_BLOCK
    Set oMasterRcFile = oFSO.OpenTextFile(sMasterRcPath, ForReading)
    Set oLanguageRcFile = oFSO.CreateTextFile(sLanguageRcPath, True)
    Do Until oMasterRcFile.AtEndOfStream = True 'For all lines...
      sMasterLine = oMasterRcFile.ReadLine
      sLanguageLine = sMasterLine
      sMasterLine = Trim(sMasterLine) 'Save Masterline trimmed!
      
      If (InStr(sMasterLine, " MENU") > 0) And (InStr(sMasterLine, "IDR_") > 0) Then 'MENU...
        iBlockType = MENU_BLOCK
      ElseIf (InStr(sMasterLine, " DIALOGEX") > 0) Then 'DIALOGEX...
        iBlockType = DIALOGEX_BLOCK
      ElseIf (sMasterLine = "STRINGTABLE") Then 'STRINGTABLE...
        iBlockType = STRINGTABLE_BLOCK
      ElseIf (InStr(sMasterLine, " VERSIONINFO") > 0) Then 'VERSIONINFO...
        iBlockType = VERSIONINFO_BLOCK
      ElseIf (InStr(sMasterLine, " ACCELERATORS") > 0) Then 'ACCELERATORS...
        iBlockType = ACCELERATORS_BLOCK
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
            If (FoundRegExpMatch(sMasterLine, "LANGUAGE (LANG_\w*, SUBLANG_\w*)", oMatch) = True) Then 'LANGUAGE...
              sMsgId = oMatch.SubMatches(0)
              If (sMsgId <> "") And (oTranslations.Exists(sMsgId) = True) Then 'If translation located...
                sMsgStr = oTranslations(sMsgId)
                sLanguageLine = Replace(sLanguageLine, "LANGUAGE " & sMsgId, "LANGUAGE " & sMsgStr)
              End If
            ElseIf (FoundRegExpMatch(sMasterLine, "code_page\(([\d]+)\)", oMatch) = True) Then 'code_page...
              sMsgId = oMatch.SubMatches(0)
              If (sMsgId <> "") And (oTranslations.Exists(sMsgId) = True) Then 'If translation located...
                sMsgStr = oTranslations(sMsgId)
                sLanguageLine = Replace(sLanguageLine, "code_page(" & sMsgId & ")", "code_page(" & sMsgStr & ")")
              End If
            ElseIf (InStr(sMasterLine, "#include ""resource.h""") > 0) Then '#include "resource.h"...
              sLanguageLine = Replace(sLanguageLine, "#include ""resource.h""", "#include ""..\..\resource.h""")
            ElseIf (InStr(sMasterLine, "Merge.rc2") > 0) Then 'Merge.rc2...
              sLanguageLine = ""
            End If
            
          Case MENU_BLOCK, DIALOGEX_BLOCK, STRINGTABLE_BLOCK:
            If (InStr(sMasterLine, """") > 0) Then 'If quote found (for speedup)...
              '--------------------------------------------------------------------------------
              ' Note: We must replace "" temporary with FormFeed...
              '--------------------------------------------------------------------------------
              If (FoundRegExpMatches(Replace(sMasterLine, """""", vbFormFeed), """(.*?)""", oMatches) = True) Then 'String...
                For Each oMatch In oMatches 'For all strings...
                  sMsgId = Replace(oMatch.SubMatches(0), vbFormFeed, """""")
                  If (sMsgId <> "") And (oTranslations.Exists(sMsgId) = True) Then 'If translation located...
                    sMsgStr = oTranslations(sMsgId)
                    sLanguageLine = Replace(sLanguageLine, """" & sMsgId & """", """" & sMsgStr & """")
                  End If
                Next
              End If
            End If
            
          Case VERSIONINFO_BLOCK:
            If (FoundRegExpMatch(sMasterLine, "BLOCK ""([0-9A-F]+)""", oMatch) = True) Then 'StringFileInfo.Block...
              sMsgId = oMatch.SubMatches(0)
              If (sMsgId <> "") And (oTranslations.Exists(sMsgId) = True) Then 'If translation located...
                sMsgStr = oTranslations(sMsgId)
                sLanguageLine = Replace(sLanguageLine, """" & sMsgId & """", """" & sMsgStr & """")
              End If
            ElseIf (FoundRegExpMatch(sMasterLine, "VALUE ""(.*?)"", ""(.*?)\\?0?""", oMatch) = True) Then 'StringFileInfo.Value...
              sMsgId = oMatch.SubMatches(1)
              If (sMsgId <> "") And (oTranslations.Exists(sMsgId) = True) Then 'If translation located...
                sMsgStr = oTranslations(sMsgId)
                sLanguageLine = Replace(sLanguageLine, """, """ & sMsgId, """, """ & sMsgStr)
              End If
            ElseIf (FoundRegExpMatch(sMasterLine, "VALUE ""Translation"", (.*?)$", oMatch) = True) Then 'VarFileInfo.Translation...
              sMsgId = oMatch.SubMatches(0)
              If (sMsgId <> "") And (oTranslations.Exists(sMsgId) = True) Then 'If translation located...
                sMsgStr = oTranslations(sMsgId)
                sLanguageLine = Replace(sLanguageLine, sMsgId, sMsgStr)
              End If
            End If
            
        End Select
      End If
      oLanguageRcFile.WriteLine sLanguageLine
    Loop
    oMasterRcFile.Close
    oLanguageRcFile.Close
  End If
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
Function FoundRegExpMatches(ByVal sString, ByVal sPattern, ByRef oMatchesReturn)
  Dim oRegExp
  
  Set oRegExp = New RegExp
  oRegExp.Pattern = sPattern
  oRegExp.IgnoreCase = True
  oRegExp.Global = True
  
  oMatchesReturn = Null
  FoundRegExpMatches = False
  If (oRegExp.Test(sString) = True) Then
    Set oMatchesReturn = oRegExp.Execute(sString)
    FoundRegExpMatches = True
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
