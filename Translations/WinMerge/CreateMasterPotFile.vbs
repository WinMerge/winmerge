Option Explicit
''
' This script creates the master POT file (English.pot).
'
' Copyright (C) 2007-2009 by Tim Gerundt
' Released under the "GNU General Public License"

Const ForReading = 1

Const NO_BLOCK = 0
Const MENU_BLOCK = 1
Const DIALOGEX_BLOCK = 2
Const STRINGTABLE_BLOCK = 3
Const VERSIONINFO_BLOCK = 4
Const ACCELERATORS_BLOCK = 5

Const PATH_ENGLISH_POT = "English.pot"
Const PATH_MERGE_RC = "../../Src/Merge.rc"
Const PATH_PLUGIN_STRINGS_RC = "../../Plugins/Strings.rc"

Dim oFSO, bRunFromCmd, bInsertLineNumbers

Set oFSO = CreateObject("Scripting.FileSystemObject")

bRunFromCmd = False
If LCase(oFSO.GetFileName(Wscript.FullName)) = "cscript.exe" Then
  bRunFromCmd = True
End If
bInsertLineNumbers = False
If WScript.Arguments.Named.Exists("InsertLineNumbers") Then
  bInsertLineNumbers = CBool(WScript.Arguments.Named("InsertLineNumbers"))
End If

Call Main

''
' ...
Sub Main
  Dim oStrings
  Dim StartTime, EndTime, Seconds
  Dim bNecessary, oFile
  
  StartTime = Time
  
  InfoBox "Creating POT file from Merge.rc...", 3
  
  bNecessary = True
  If (oFSO.FileExists(PATH_ENGLISH_POT) = True) Then 'If the POT file exists...
    bNecessary = GetArchiveBit(PATH_MERGE_RC) Or GetArchiveBit(PATH_ENGLISH_POT) 'RCs or POT file changed?
  End If
  
  If (bNecessary = True) Then 'If update necessary...
    Set oStrings = MergeDictionaries( _
      GetStringsFromRcFile(PATH_MERGE_RC), _
      GetStringsFromRcFile(PATH_PLUGIN_STRINGS_RC))
    CreateMasterPotFile PATH_ENGLISH_POT, oStrings
    SetArchiveBit PATH_MERGE_RC, False
    SetArchiveBit PATH_PLUGIN_STRINGS_RC, False
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
Function MergeDictionaries(dict1, dict2)
    Dim mergedDict
    Set mergedDict = CreateObject("Scripting.Dictionary")
    
    Dim key
    For Each key In dict1.Keys
        mergedDict.Add key, dict1.Item(key)
    Next
    
    For Each key In dict2.Keys
        If Not mergedDict.Exists(key) Then
            mergedDict.Add key, dict2.Item(key)
        End If
    Next
    
    Set MergeDictionaries = mergedDict
End Function

''
' ...
Function GetStringsFromRcFile(ByVal sRcFilePath)
  Dim oBlacklist, oStrings, oString, oRcFile, sLine, iLine
  Dim sRcFileName, iBlockType, sReference, sString, sComment, sContext, oMatch, sTemp, sKey
  Dim fContinuation

  Set oBlacklist = GetStringBlacklist("StringBlacklist.txt")
  
  Set oStrings = CreateObject("Scripting.Dictionary")
  
  If (oFSO.FileExists(sRcFilePath) = True) Then 'If the RC file exists...
    sRcFileName = oFSO.GetFileName(sRcFilePath)
    iLine = 0
    iBlockType = NO_BLOCK
    Set oRcFile = oFSO.OpenTextFile(sRcFilePath, ForReading)
    Do Until oRcFile.AtEndOfStream = True 'For all lines...
      sLine = Trim(oRcFile.ReadLine)
      iLine = iLine + 1
      
      sReference = sRcFileName & ":" & iLine
      sString = ""
      sComment = ""
      sContext = ""
      
      If fContinuation Then
        ' Nothing to do
      ElseIf (InStr(sLine, " MENU") > 0) And (InStr(sLine, "IDR_") > 0) Then 'MENU...
        iBlockType = MENU_BLOCK
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
        Select Case iBlockType
          Case NO_BLOCK:
            If (FoundRegExpMatch(sLine, "LANGUAGE (LANG_\w*, SUBLANG_\w*)", oMatch) = True) Then 'LANGUAGE...
              sString = oMatch.SubMatches(0)
              sComment = "LANGUAGE, SUBLANGUAGE"
            End If
            
          Case MENU_BLOCK, DIALOGEX_BLOCK, STRINGTABLE_BLOCK:
            If (InStr(sLine, """") > 0) Then 'If quote found (for speedup)...
              '--------------------------------------------------------------------------------
              ' Replace 1st string literal only - 2nd string literal specifies control class!
              '--------------------------------------------------------------------------------
              If FoundRegExpMatch(sLine, "NC_\s*\(""([^""]*)""\s*,\s*""([^""]*)""\s*\)", oMatch) Then 'String...
                sContext = Trim(oMatch.SubMatches(0))
                sTemp = oMatch.SubMatches(1)
              ElseIf FoundRegExpMatch(sLine, """((?:""""|[^""])*)""", oMatch) Then 'String...
                sTemp = oMatch.SubMatches(0)
              Else
                sTemp = ""
              End If 
              If (sTemp <> "") And (oBlacklist.Exists(sTemp) = False) Then 'If NOT blacklisted...
                sString = Replace(sTemp, """""", "\""")
                If (FoundRegExpMatch(sLine, "//#\. (.*?)$", oMatch) = True) Then 'If found a comment for the translators...
                  sComment = Trim(oMatch.SubMatches(0))
                ElseIf (FoundRegExpMatch(sLine, "//msgctxt (.*?)$", oMatch) = True) Then 'If found a context for the translation...
                  sContext = Trim(oMatch.SubMatches(0))
                  sComment = sContext
                End If
              End If
            End If
            
          Case VERSIONINFO_BLOCK:
            If (FoundRegExpMatch(sLine, "BLOCK ""([0-9A-F]+)""", oMatch) = True) Then 'StringFileInfo.Block...
              sString = oMatch.SubMatches(0)
              sComment = "StringFileInfo.Block"
            ElseIf (FoundRegExpMatch(sLine, "VALUE ""Comments"", ""(.*?)\\?0?""", oMatch) = True) Then 'StringFileInfo.Comments...
              sString = oMatch.SubMatches(0)
              sComment = "You should use a string like ""Translated by "" followed by the translator names for this string. It is ONLY VISIBLE in the StringFileInfo.Comments property from the final resource file!"
            ElseIf (FoundRegExpMatch(sLine, "VALUE ""Translation"", (.*?)$", oMatch) = True) Then 'VarFileInfo.Translation...
              sString = oMatch.SubMatches(0)
              sComment = "VarFileInfo.Translation"
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
        If bInsertLineNumbers Then
          If (oString.References <> "") Then
            oString.References = oString.References & vbTab & sReference
          Else
            oString.References = sReference
          End If
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
      fContinuation = sLine <> "" And InStr(",|", Right(sLine, 1)) <> 0
    Loop
    oRcFile.Close
  End If
  Set GetStringsFromRcFile = oStrings
End Function

''
' ...
Function GetStringBlacklist(ByVal sTxtFilePath)
  Dim oBlacklist, oTxtFile, sLine
  
  Set oBlacklist = CreateObject("Scripting.Dictionary")
  
  If (oFSO.FileExists(sTxtFilePath) = True) Then 'If the blacklist file exists...
    Set oTxtFile = oFSO.OpenTextFile(sTxtFilePath, ForReading)
    Do Until oTxtFile.AtEndOfStream = True 'For all lines...
      sLine = Trim(oTxtFile.ReadLine)
      
      If (sLine <> "") Then
        If (oBlacklist.Exists(sLine) = False) Then 'If the key is NOT already used...
          oBlacklist.Add sLine, True
        End If
      End If
    Loop
    oTxtFile.Close
  End If
  Set GetStringBlacklist = oBlacklist
End Function

''
' ...
Sub CreateMasterPotFile(ByVal sPotPath, ByVal oStrings)
  Dim oPotFile, sKey, oString, aReferences, i
  
  Set oPotFile = oFSO.CreateTextFile(sPotPath, True)
  
  oPotFile.Write "# This file is part from WinMerge <https://winmerge.org/>" & vbLf
  oPotFile.Write "# Released under the ""GNU General Public License""" & vbLf
  oPotFile.Write "#" & vbLf
  oPotFile.Write "msgid """"" & vbLf
  oPotFile.Write "msgstr """"" & vbLf
  oPotFile.Write """Project-Id-Version: WinMerge\n""" & vbLf
  oPotFile.Write """Report-Msgid-Bugs-To: https://bugs.winmerge.org/\n""" & vbLf
  oPotFile.Write """POT-Creation-Date: " & GetPotCreationDate() & "\n""" & vbLf
  oPotFile.Write """PO-Revision-Date: \n""" & vbLf
  oPotFile.Write """Last-Translator: \n""" & vbLf
  oPotFile.Write """Language-Team: English <winmerge-translate@lists.sourceforge.net>\n""" & vbLf
  oPotFile.Write """MIME-Version: 1.0\n""" & vbLf
  oPotFile.Write """Content-Type: text/plain; charset=UTF-8\n""" & vbLf
  oPotFile.Write """Content-Transfer-Encoding: 8bit\n""" & vbLf
  oPotFile.Write """X-Poedit-Language: English\n""" & vbLf
  oPotFile.Write """X-Poedit-SourceCharset: UTF-8\n""" & vbLf
  oPotFile.Write """X-Poedit-Basepath: ../../Src/\n""" & vbLf
  'oPotFile.Write """X-Generator: CreateMasterPotFile.vbs\n""" & vbLf
  oPotFile.Write vbLf
  For Each sKey In oStrings.Keys 'For all strings...
    Set oString = oStrings(sKey)
    If (oString.Comment <> "") Then 'If comment exists...
      oPotFile.Write "#. " & oString.Comment & vbLf
    End If
    If bInsertLineNumbers Then
      aReferences = SplitByTab(oString.References)
      For i = LBound(aReferences) To UBound(aReferences) 'For all references...
        oPotFile.Write "#: " & aReferences(i) & vbLf
      Next
    End If
    If (InStr(oString.Id, "%") > 0) Then 'If c-format...
      oPotFile.Write "#, c-format" & vbLf
    End If
    If (oString.Context <> "") Then 'If context exists...
      oPotFile.Write "msgctxt """ & oString.Context & """" & vbLf
    End If
    oPotFile.Write "msgid """ & oString.Id & """" & vbLf
    oPotFile.Write "msgstr """"" & vbLf
    oPotFile.Write vbLf
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
