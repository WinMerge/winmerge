Option Explicit
''
' This script creates PO files for easier reviewing of the translations.
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
  Dim oOriginalTranslations, oLanguageTranslations
  Dim StartTime, EndTime, Seconds
  
  StartTime = Time
  
  Wscript.Echo "Warning: " & Wscript.ScriptName & " can take several minutes to finish!"
  
  Set oOriginalTranslations = GetTranslationsFromRcFile("../Merge.rc")
  
  Set oLanguages = GetLanguages
  For Each sLanguage In oLanguages.Keys 'For all languages...
    If (bRunFromCmd = True) Then 'If run from command line...
      Wscript.Echo sLanguage
    End If
    Set oLanguageTranslations = GetTranslationsFromRcFile(oLanguages(sLanguage))
    CreateReviewPoFile sLanguage, oOriginalTranslations, oLanguageTranslations
  Next
  
  EndTime = Time
  Seconds = DateDiff("s", StartTime, EndTime)
  
  Wscript.Echo Wscript.ScriptName & " finished after " & Seconds & " seconds!"
End Sub

''
' ...
Function GetLanguages()
  Dim oLanguages, oSubFolder, sRcPath
  
  Set oLanguages = CreateObject("Scripting.Dictionary")
  
  For Each oSubFolder In oFSO.GetFolder(".").SubFolders 'For all subfolders in the current folder...
    If (oSubFolder.Name <> ".svn") Then 'If NOT a SVN folder...
      sRcPath = oFSO.BuildPath(oSubFolder.Path, "Merge" & oSubFolder.Name & ".rc")
      If (oFSO.FileExists(sRcPath) = True) Then 'If the RC file exists...
        oLanguages.Add oSubFolder.Name, sRcPath
      End If
    End If
  Next
  Set GetLanguages = oLanguages
End Function

''
' ...
Function GetTranslationsFromRcFile(ByVal sRcPath)
  Dim oTranslations, oTextFile, sLine
  Dim oMatch, iBlockType, sKey1, sKey2, iPosition, sValue
  Dim sCodePage
  
  Set oTranslations = CreateObject("Scripting.Dictionary")
  
  If (oFSO.FileExists(sRcPath) = True) Then
    iBlockType = NO_BLOCK
    sKey1 = ""
    sKey2 = ""
    iPosition = 0
    sCodePage = ""
    Set oTextFile = oFSO.OpenTextFile(sRcPath, ForReading)
    Do Until oTextFile.AtEndOfStream = True
      sLine = Trim(oTextFile.ReadLine)
      
      sValue = ""
      
      If (FoundRegExpMatch(sLine, "(IDR_.*) MENU", oMatch) = True) Then 'MENU...
        iBlockType = MENU_BLOCK
        sKey1 = oMatch.SubMatches(0)
        iPosition = 0
      ElseIf (FoundRegExpMatch(sLine, "(IDD_.*) DIALOGEX", oMatch) = True) Then 'DIALOGEX...
        iBlockType = DIALOGEX_BLOCK
        sKey1 = oMatch.SubMatches(0)
        iPosition = 0
      ElseIf (sLine = "STRINGTABLE") Then 'STRINGTABLE...
        iBlockType = STRINGTABLE_BLOCK
        sKey1 = "STRINGTABLE"
        'iPosition = 0
      ElseIf (FoundRegExpMatch(sLine, "(VS_.*) VERSIONINFO", oMatch) = True) Then 'VERSIONINFO...
        iBlockType = VERSIONINFO_BLOCK
        sKey1 = "VERSIONINFO"
        iPosition = 0
      ElseIf (sLine = "END") Then 'END...
        If (iBlockType = STRINGTABLE_BLOCK) Then 'If inside stringtable...
          iBlockType = NO_BLOCK
          sKey1 = ""
          'iPosition = 0
        End If
      ElseIf (sLine <> "") Then 'If NOT empty line...
        Select Case iBlockType
          Case NO_BLOCK:
            If (FoundRegExpMatch(sLine, "code_page\(([\d]+)\)", oMatch) = True) Then 'code_page...
              sCodePage = oMatch.SubMatches(0)
            End If
            
          Case MENU_BLOCK:
            If (FoundRegExpMatch(sLine, "POPUP ""(.*)""", oMatch) = True) Then 'POPUP...
              If (InStr(oMatch.SubMatches(0), "_POPUP_") = 0) Then
                sKey2 = iPosition
                iPosition = iPosition + 1
                sValue = oMatch.SubMatches(0)
              End If
            ElseIf (FoundRegExpMatch(sLine, "MENUITEM.*""(.*)"".*(ID_.*)", oMatch) = True) Then 'MENUITEM...
              sKey2 = oMatch.SubMatches(1)
              sValue = oMatch.SubMatches(0)
            End If
            
          Case DIALOGEX_BLOCK:
            If (FoundRegExpMatch(sLine, "CAPTION.*""(.*)""", oMatch) = True) Then 'CAPTION...
              sKey2 = "CAPTION"
              sValue = oMatch.SubMatches(0)
            ElseIf (FoundRegExpMatch(sLine, "PUSHBUTTON.*""(.*)"",(\w+)", oMatch) = True) Then 'DEFPUSHBUTTON/PUSHBUTTON...
              sKey2 = oMatch.SubMatches(1)
              sValue = oMatch.SubMatches(0)
            ElseIf (FoundRegExpMatch(sLine, "[L|R|C]TEXT.*""(.*)"",(\w+)", oMatch) = True) Then 'LTEXT/RTEXT...
              If (oMatch.SubMatches(0) <> "") And (oMatch.SubMatches(0) <> "Static") Then
                If (oMatch.SubMatches(1) <> "IDC_STATIC") Then
                  sKey2 = oMatch.SubMatches(1)
                Else
                  sKey2 = iPosition & "_TEXT"
                  iPosition = iPosition + 1
                End If
                sValue = oMatch.SubMatches(0)
              End If
            ElseIf (FoundRegExpMatch(sLine, "[L|R]TEXT.*""(.*)"",", oMatch) = True) Then 'LTEXT/RTEXT (without ID)...
              sKey2 = iPosition & "_TEXT"
              iPosition = iPosition + 1
              sValue = oMatch.SubMatches(0)
            ElseIf (FoundRegExpMatch(sLine, "CONTROL +""(.*?)"",(\w+)", oMatch) = True) Then 'CONTROL...
              If (oMatch.SubMatches(0) <> "Dif") And (oMatch.SubMatches(0) <> "Btn") And (oMatch.SubMatches(0) <> "Button1") _
                And (oMatch.SubMatches(0) <> "List1") And (oMatch.SubMatches(0) <> "Tree1") Then
                sKey2 = oMatch.SubMatches(1)
                sValue = oMatch.SubMatches(0)
              End If
            ElseIf (FoundRegExpMatch(sLine, "CONTROL +""(.*?)"",", oMatch) = True) Then 'CONTROL (without ID)...
              sKey2 = iPosition & "_CONTROL"
              iPosition = iPosition + 1
              sValue = oMatch.SubMatches(0)
            ElseIf (FoundRegExpMatch(sLine, "GROUPBOX +""(.*?)"",(\w+)", oMatch) = True) Then 'GROUPBOX...
              If (oMatch.SubMatches(1) <> "IDC_STATIC") Then
                sKey2 = oMatch.SubMatches(1)
              Else
                sKey2 = iPosition & "_GROUPBOX"
                iPosition = iPosition + 1
              End If
              sValue = oMatch.SubMatches(0)
            End If
            
          Case STRINGTABLE_BLOCK:
            If (FoundRegExpMatch(sLine, "(\w+).*""(.*)""", oMatch) = True) Then 'String...
              sKey2 = oMatch.SubMatches(0)
              sValue = oMatch.SubMatches(1)
            ElseIf (FoundRegExpMatch(sLine, """(.*)""", oMatch) = True) Then 'String (without ID)...
              sKey2 = iPosition
              iPosition = iPosition + 1
              sValue = oMatch.SubMatches(0)
            End If
            
          Case VERSIONINFO_BLOCK:
            '...
            '...
            '...
            
        End Select
      End If
      
      If (sValue <> "") Then
        oTranslations.Add sKey1 & "." & sKey2, sValue
      End If
    Loop
    oTextFile.Close
    
    oTranslations.Add "code_page", sCodePage
  End If
  Set GetTranslationsFromRcFile = oTranslations
End Function

''
' ...
Sub CreateReviewPoFile(ByVal sLanguage, ByVal oOriginalTranslations, ByVal oLanguageTranslations)
  Dim oPoFile, sKey
  Dim sOriginalTranslation, sLanguageTranslation
  
  Set oPoFile = oFSO.CreateTextFile(sLanguage & "\" & sLanguage & "Review.po", True)
  
  oPoFile.WriteLine "# DO NOT EDIT THIS FILE!"
  oPoFile.WriteLine "# This file is only for easier reviewing of the WinMerge translation..."
  oPoFile.WriteLine "#"
  oPoFile.WriteLine "msgid """""
  oPoFile.WriteLine "msgstr """""
  oPoFile.WriteLine """Project-Id-Version: \n"""
  oPoFile.WriteLine """Report-Msgid-Bugs-To: \n"""
  oPoFile.WriteLine """POT-Creation-Date: \n"""
  oPoFile.WriteLine """PO-Revision-Date: \n"""
  oPoFile.WriteLine """Last-Translator: \n"""
  oPoFile.WriteLine """Language-Team: \n"""
  oPoFile.WriteLine """MIME-Version: 1.0\n"""
  oPoFile.WriteLine """Content-Type: text/plain; charset=CP" & oLanguageTranslations("code_page") & "\n"""
  oPoFile.WriteLine """Content-Transfer-Encoding: 8bit\n"""
  oPoFile.WriteLine
  For Each sKey In oOriginalTranslations.Keys 'For all original translations...
    If (sKey <> "code_page") Then
      sOriginalTranslation = oOriginalTranslations(sKey)
      sLanguageTranslation = oLanguageTranslations(sKey)
      oPoFile.WriteLine "#: " & sKey
      If (sOriginalTranslation = sLanguageTranslation) Then 'If MAYBE NOT translated...
        oPoFile.WriteLine "#, fuzzy"
      End If
      oPoFile.WriteLine "msgid """ & sOriginalTranslation & """"
      oPoFile.WriteLine "msgstr """ & sLanguageTranslation & """"
      oPoFile.WriteLine
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
