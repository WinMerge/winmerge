Option Explicit
''
' This script creates the master POT file (English.pot) for the shell extension.
'
' Copyright (C) 2007-2009 by Tim Gerundt
' Released under the "GNU General Public License"

Const ForReading = 1

Const NO_BLOCK = 0
Const STRINGTABLE_BLOCK = 1

Const PATH_ENGLISH_POT = "English.pot"
Const PATH_SHELLEXTTEMPLATE_RC = "../../ShellExtension/ShellExtension/ShellExtension.rc"

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
  Dim oStrings, sCodePage
  Dim StartTime, EndTime, Seconds
  Dim bNecessary, oFile
  
  StartTime = Time
  
  InfoBox "Creating POT file from ShellExtension.rc...", 3
  
  bNecessary = True
  If (oFSO.FileExists(PATH_ENGLISH_POT) = True) And (oFSO.FileExists(PATH_SHELLEXTTEMPLATE_RC) = True) Then 'If the POT and RC file exists...
    bNecessary = GetArchiveBit(PATH_SHELLEXTTEMPLATE_RC) Or GetArchiveBit(PATH_ENGLISH_POT) 'RCs or POT file changed?
  End If
  
  If (bNecessary = True) Then 'If update necessary...
    Set oStrings = GetStringsFromRcFile(PATH_SHELLEXTTEMPLATE_RC, sCodePage)
    CreateMasterPotFile PATH_ENGLISH_POT, oStrings, sCodePage
    SetArchiveBit PATH_SHELLEXTTEMPLATE_RC, False
    SetArchiveBit PATH_ENGLISH_POT, False
    For Each oFile In oFSO.GetFolder(".").Files 'For all files in the current folder...
      If (LCase(oFSO.GetExtensionName(oFile.Name)) = "po") Then 'If a PO file...
        SetArchiveBit oFile.Path, True
      End If
    Next
    
    EndTime = Time
    Seconds = DateDiff("s", StartTime, EndTime)
    
    InfoBox "POT file created, after " & Seconds & " second(s).", 10
  Else 'If update NOT necessary...
    InfoBox "POT file already up-to-date.", 10
  End If
End Sub

''
' ...
Class CString
  Dim Comment, References, Context, Id, Str
End Class

''
' ...
Function GetStringsFromRcFile(ByVal sRcFilePath, ByRef sCodePage)
  Dim oStrings, oString, oRcFile, sLine, iLine
  Dim sRcFileName, iBlockType, sReference, sString, sComment, sContext, oMatch, sTemp, sKey
  
  Set oStrings = CreateObject("Scripting.Dictionary")
  
  If (oFSO.FileExists(sRcFilePath) = True) Then 'If the RC file exists...
    sRcFileName = oFSO.GetFileName(sRcFilePath)
    iLine = 0
    iBlockType = NO_BLOCK
    sCodePage = ""
    Set oRcFile = oFSO.OpenTextFile(sRcFilePath, ForReading)
    Do Until oRcFile.AtEndOfStream = True 'For all lines...
      sLine = Trim(oRcFile.ReadLine)
      iLine = iLine + 1
      
      sReference = sRcFileName & ":" & iLine
      sString = ""
      sComment = ""
      sContext = ""
      
      If (sLine = "STRINGTABLE") Then 'STRINGTABLE...
        iBlockType = STRINGTABLE_BLOCK
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
        Select Case iBlockType
          Case NO_BLOCK:
            If (FoundRegExpMatch(sLine, "defined\((AFX_TARG_\w*)\)", oMatch) = True) Then 'AFX_TARG_*...
              sString = oMatch.SubMatches(0)
              sComment = "AFX_TARG_*"
            ElseIf (FoundRegExpMatch(sLine, "LANGUAGE (LANG_\w*, SUBLANG_\w*)", oMatch) = True) Then 'LANGUAGE...
              sString = oMatch.SubMatches(0)
              sComment = "LANGUAGE, SUBLANGUAGE"
            ElseIf (FoundRegExpMatch(sLine, "code_page\(([\d]+)\)", oMatch) = True) Then 'code_page...
              sString = oMatch.SubMatches(0)
              sComment = "Codepage"
              sCodePage = oMatch.SubMatches(0)
            End If
            
          Case STRINGTABLE_BLOCK:
            If (InStr(sLine, """") > 0) Then 'If quote found (for speedup)...
              '--------------------------------------------------------------------------------
              ' Replace 1st string literal only - 2nd string literal specifies control class!
              '--------------------------------------------------------------------------------
              If FoundRegExpMatch(sLine, """((?:""""|[^""])*)""", oMatch) Then 'String...
                sTemp = oMatch.SubMatches(0)
                If (sTemp <> "") Then 'If NOT empty...
                  sString = Replace(sTemp, """""", "\""")
                  If (FoundRegExpMatch(sLine, "//#\. (.*?)$", oMatch) = True) Then 'If found a comment for the translators...
                    sComment = Trim(oMatch.SubMatches(0))
                  ElseIf (FoundRegExpMatch(sLine, "//msgctxt (.*?)$", oMatch) = True) Then 'If found a context for the translation...
                    sContext = Trim(oMatch.SubMatches(0))
                    sComment = sContext
                  End If
                End If
              End If
            End If
            
        End Select
      End If
      
      If (sString <> "") Then
        sKey = sContext & sString
        Set oString = New CString
        If (oStrings.Exists(sKey) = True) Then 'If the key is already used...
          Set oString = oStrings(sKey)
        End If
        If (sComment <> "") Then
          oString.Comment = sComment
        End If
        If (oString.References <> "") Then
          oString.References = oString.References & vbTab & sReference
        Else
          oString.References = sReference
        End If
        oString.Context = sContext
        oString.Id = sString
        oString.Str = ""
        
        If (oStrings.Exists(sKey) = True) Then 'If the key is already used...
          Set oStrings(sKey) = oString
        Else 'If the key is NOT already used...
          oStrings.Add sContext & sString, oString
        End If
      End If
    Loop
    oRcFile.Close
  End If
  Set GetStringsFromRcFile = oStrings
End Function

''
' ...
Sub CreateMasterPotFile(ByVal sPotPath, ByVal oStrings, ByVal sCodePage)
  Dim oPotFile, sKey, oString, aReferences, i
  
  Set oPotFile = oFSO.CreateTextFile(sPotPath, True)
  
  oPotFile.WriteLine "# This file is part from WinMerge <https://winmerge.org/>"
  oPotFile.WriteLine "# Released under the ""GNU General Public License"""
  oPotFile.WriteLine "#"
  oPotFile.WriteLine "msgid """""
  oPotFile.WriteLine "msgstr """""
  oPotFile.WriteLine """Project-Id-Version: WinMerge Shell Extension\n"""
  oPotFile.WriteLine """Report-Msgid-Bugs-To: https://bugs.winmerge.org/\n"""
  oPotFile.WriteLine """POT-Creation-Date: " & GetPotCreationDate() & "\n"""
  oPotFile.WriteLine """PO-Revision-Date: \n"""
  oPotFile.WriteLine """Last-Translator: \n"""
  oPotFile.WriteLine """Language-Team: English <winmerge-translate@lists.sourceforge.net>\n"""
  oPotFile.WriteLine """MIME-Version: 1.0\n"""
  oPotFile.WriteLine """Content-Type: text/plain; charset=CP" & sCodePage & "\n"""
  oPotFile.WriteLine """Content-Transfer-Encoding: 8bit\n"""
  oPotFile.WriteLine """X-Poedit-Language: English\n"""
  oPotFile.WriteLine """X-Poedit-SourceCharset: CP" & sCodePage & "\n"""
  oPotFile.WriteLine """X-Poedit-Basepath: ../../ShellExtension/Languages/\n"""
  'oPotFile.WriteLine """X-Generator: CreateMasterPotFile.vbs\n"""
  oPotFile.WriteLine
  For Each sKey In oStrings.Keys 'For all strings...
    Set oString = oStrings(sKey)
    If (oString.Comment <> "") Then 'If comment exists...
      oPotFile.WriteLine "#. " & oString.Comment
    End If
    aReferences = SplitByTab(oString.References)
    For i = LBound(aReferences) To UBound(aReferences) 'For all references...
      oPotFile.WriteLine "#: " & aReferences(i)
    Next
    oPotFile.WriteLine "#, c-format"
    If (oString.Context <> "") Then 'If context exists...
      oPotFile.WriteLine "msgctxt """ & oString.Context & """"
    End If
    oPotFile.WriteLine "msgid """ & oString.Id & """"
    oPotFile.WriteLine "msgstr """""
    oPotFile.WriteLine
  Next
  oPotFile.Close
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
Function SplitByTab(ByVal sString)
  SplitByTab = Array()
  If (InStr(sString, vbTab) > 0) Then
    SplitByTab = Split(sString, vbTab, -1)
  Else
    SplitByTab = Array(sString)
  End If
End Function

''
' ...
Function GetPotCreationDate()
  Dim oNow, sYear, sMonth, sDay, sHour, sMinute
  
  oNow = Now()
  sYear = Year(oNow)
  sMonth = Month(oNow)
  If (sMonth < 10) Then sMonth = "0" & sMonth
  sDay = Day(oNow)
  If (sDay < 10) Then sDay = "0" & sDay
  sHour = Hour(oNow)
  If (sHour < 10) Then sHour = "0" & sHour
  sMinute = Minute(oNow)
  If (sMinute < 10) Then sMinute = "0" & sMinute
  
  GetPotCreationDate = sYear & "-" & sMonth & "-" & sDay & " " & sHour & ":" & sMinute & "+0000"
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
