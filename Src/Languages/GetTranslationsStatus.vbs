Option Explicit
''
' This script gets the status of the translations.
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
If (LCase(Right(Wscript.FullName, 11))) = "cscript.exe" Then
  bRunFromCmd = True
End If

Call Main

''
' ...
Sub Main
  Dim oTranslationsStatus
  Dim oLanguages, sLanguage
  Dim StartTime, EndTime, Seconds
  
  StartTime = Time
  
  Wscript.Echo "Warning: " & Wscript.ScriptName & " can take several seconds to finish!"
  
  Set oTranslationsStatus = CreateObject("Scripting.Dictionary")
  
  If (bRunFromCmd = True) Then 'If run from command line...
    Wscript.Echo "English"
  End If
  oTranslationsStatus.Add "English", GetTranslationsStatusFromPoFile("English.pot")
  
  Set oLanguages = GetLanguages
  For Each sLanguage In oLanguages.Keys 'For all languages...
    If (bRunFromCmd = True) Then 'If run from command line...
      Wscript.Echo sLanguage
    End If
    oTranslationsStatus.Add sLanguage, GetTranslationsStatusFromPoFile(oLanguages(sLanguage))
  Next
  
  CreateTranslationsStatusHtmlFile "TranslationsStatus.html", oTranslationsStatus
  
  CreateTranslationsStatusWikiFile "TranslationsStatus.wiki", oTranslationsStatus
  
  EndTime = Time
  Seconds = DateDiff("s", StartTime, EndTime)
  
  Wscript.Echo Wscript.ScriptName & " finished after " & Seconds & " seconds!"
End Sub

''
' ...
Function GetLanguages()
  Dim oLanguages, oSubFolder, sPoPath
  
  Set oLanguages = CreateObject("Scripting.Dictionary")
  
  For Each oSubFolder In oFSO.GetFolder(".").SubFolders 'For all subfolders in the current folder...
    If (oSubFolder.Name <> ".svn") Then 'If NOT a SVN folder...
      sPoPath = oFSO.BuildPath(oSubFolder.Path, oSubFolder.Name & ".po")
      If (oFSO.FileExists(sPoPath) = True) Then 'If the PO file exists...
        oLanguages.Add oSubFolder.Name, sPoPath
      End If
    End If
  Next
  Set GetLanguages = oLanguages
End Function

''
' ...
Function GetTranslationsStatusFromPoFile(ByVal sPoPath)
  Dim oStatus, oTextFile, sLine
  Dim oMatch, iMsgStarted, sMsgId, sMsgStr, bFuzzy
  
  Set oStatus = CreateObject("Scripting.Dictionary")
  
  oStatus.Add "count", 0
  oStatus.Add "translated", 0
  oStatus.Add "untranslated", 0
  oStatus.Add "fuzzy", 0
  oStatus.Add "po-revision-date", ""
  If (oFSO.FileExists(sPoPath) = True) Then 'If the PO file exists...
    iMsgStarted = 0
    sMsgId = ""
    sMsgStr = ""
    bFuzzy = False
    Set oTextFile = oFSO.OpenTextFile(sPoPath, ForReading)
    Do Until oTextFile.AtEndOfStream = True 'For all lines...
      sLine = Trim(oTextFile.ReadLine)
      
      If (sLine <> "") Then 'If NOT empty line...
        If (Left(sLine, 1) <> "#") Then 'If NOT comment line...
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
        Else 'If comment line...
          iMsgStarted = -1
          If (Left(sLine, 2) = "#,") Then 'If "flag" line...
            If (InStr(sLine, "fuzzy") > 0) Then 'If "fuzzy"...
              bFuzzy = True
            End If
          End If
        End If
      Else 'If empty line
        iMsgStarted = 0
      End If
      
      If (iMsgStarted = 0) Then 'If NOT inside a translation...
        If (sMsgId <> "") Then
          oStatus("count") = oStatus("count") + 1
          If (bFuzzy = False) Then 'If NOT a fuzzy translation...
            If (sMsgStr <> "") Then
              oStatus("translated") = oStatus("translated") + 1
            Else
              oStatus("untranslated") = oStatus("untranslated") + 1
            End If
          Else 'If a fuzzy translation...
            oStatus("fuzzy") = oStatus("fuzzy") + 1
          End If
        ElseIf(sMsgStr <> "") Then
          oStatus("po-revision-date") = GetRegExpSubMatch(sMsgStr, "PO-Revision-Date: ([0-9 :\+\-]+)")
          oStatus("pot-creation-date") = GetRegExpSubMatch(sMsgStr, "POT-Creation-Date: ([0-9 :\+\-]+)")
        End If
        sMsgId = ""
        sMsgStr = ""
        bFuzzy = False
      End If
    Loop
    oTextFile.Close
  End If
  Set GetTranslationsStatusFromPoFile = oStatus
End Function

''
' ...
Sub CreateTranslationsStatusHtmlFile(ByVal sHtmlPath, ByVal oTranslationsStatus)
  Dim oHtmlFile, sLanguage, oLanguageStatus
  
  Set oHtmlFile = oFSO.CreateTextFile(sHtmlPath, True)
  
  oHtmlFile.WriteLine "<!DOCTYPE HTML PUBLIC ""-//W3C//DTD HTML 4.01 Transitional//EN"""
  oHtmlFile.WriteLine "  ""http://www.w3.org/TR/html4/loose.dtd"">"
  oHtmlFile.WriteLine "<html>"
  oHtmlFile.WriteLine "<head>"
  oHtmlFile.WriteLine "  <title>Translations Status</title>"
  oHtmlFile.WriteLine "  <meta http-equiv=""content-type"" content=""text/html; charset=ISO-8859-1"">"
  oHtmlFile.WriteLine "  <style type=""text/css"">"
  oHtmlFile.WriteLine "  <!--"
  oHtmlFile.WriteLine "    body {"
  oHtmlFile.WriteLine "      font-family: Verdana,Helvetica,Arial,sans-serif;"
  oHtmlFile.WriteLine "      font-size: small;"
  oHtmlFile.WriteLine "    }"
  oHtmlFile.WriteLine "    code,pre {"
  oHtmlFile.WriteLine "      font-family: ""Courier New"",Courier,monospace;"
  oHtmlFile.WriteLine "      font-size: 1em;"
  oHtmlFile.WriteLine "    }"
  oHtmlFile.WriteLine "    #status {"
  oHtmlFile.WriteLine "      border-collapse: collapse;"
  oHtmlFile.WriteLine "      border: 1px solid #aaaaaa;"
  oHtmlFile.WriteLine "    }"
  oHtmlFile.WriteLine "    #status th {"
  oHtmlFile.WriteLine "      padding: 3px;"
  oHtmlFile.WriteLine "      background: #f2f2f2;"
  oHtmlFile.WriteLine "      border: 1px solid #aaaaaa ;"
  oHtmlFile.WriteLine "    }"
  oHtmlFile.WriteLine "    #status td {"
  oHtmlFile.WriteLine "      padding: 3px;"
  oHtmlFile.WriteLine "      background: #f9f9f9;"
  oHtmlFile.WriteLine "      border: 1px solid #aaaaaa;"
  oHtmlFile.WriteLine "    }"
  oHtmlFile.WriteLine "    .left { text-align: left; }"
  oHtmlFile.WriteLine "    .center { text-align: center; }"
  oHtmlFile.WriteLine "    .right { text-align: right; }"
  oHtmlFile.WriteLine "  -->"
  oHtmlFile.WriteLine "  </style>"
  oHtmlFile.WriteLine "</head>"
  oHtmlFile.WriteLine "<body>"
  oHtmlFile.WriteLine "<h1>Translations Status</h1>"
  oHtmlFile.WriteLine "<p>Status from <strong>" & GetCreationDate() & "</strong>:</p>"
  oHtmlFile.WriteLine "<table id=""status"">"
  oHtmlFile.WriteLine "  <tr>"
  oHtmlFile.WriteLine "    <th class=""left"">Language</th>"
  oHtmlFile.WriteLine "    <th class=""right"">Total</th>"
  oHtmlFile.WriteLine "    <th class=""right"">Translated</th>"
  oHtmlFile.WriteLine "    <th class=""right"">Fuzzy</th>"
  oHtmlFile.WriteLine "    <th class=""right"">Untranslated</th>"
  oHtmlFile.WriteLine "    <th class=""center"">Last Update</th>"
  oHtmlFile.WriteLine "  </tr>"
  For Each sLanguage In oTranslationsStatus.Keys 'For all languages...
    If (sLanguage <> "English") Then 'If NOT English...
      Set oLanguageStatus = oTranslationsStatus(sLanguage)
      oHtmlFile.WriteLine "  <tr>"
      oHtmlFile.WriteLine "    <td class=""left"">" & sLanguage & "</td>"
      oHtmlFile.WriteLine "    <td class=""right"">" & oLanguageStatus("count") & "</td>"
      oHtmlFile.WriteLine "    <td class=""right"">" & oLanguageStatus("translated") & "</td>"
      oHtmlFile.WriteLine "    <td class=""right"">" & oLanguageStatus("fuzzy") & "</td>"
      oHtmlFile.WriteLine "    <td class=""right"">" & oLanguageStatus("untranslated") & "</td>"
      oHtmlFile.WriteLine "    <td class=""center"">" & Left(oLanguageStatus("po-revision-date"), 10) & "</td>"
      oHtmlFile.WriteLine "  </tr>"
    End If
  Next
  Set oLanguageStatus = oTranslationsStatus("English")
  oHtmlFile.WriteLine "  <tr>"
  oHtmlFile.WriteLine "    <td class=""left"">English</td>"
  oHtmlFile.WriteLine "    <td class=""right"">" & oLanguageStatus("count") & "</td>"
  oHtmlFile.WriteLine "    <td class=""right"">" & oLanguageStatus("count") & "</td>"
  oHtmlFile.WriteLine "    <td class=""right"">0</td>"
  oHtmlFile.WriteLine "    <td class=""right"">0</td>"
  oHtmlFile.WriteLine "    <td class=""center"">" & Left(oLanguageStatus("pot-creation-date"), 10) & "</td>"
  oHtmlFile.WriteLine "  </tr>"
  oHtmlFile.WriteLine "</table>"
  oHtmlFile.WriteLine "</body>"
  oHtmlFile.WriteLine "</html>"
  oHtmlFile.Close
End Sub

''
' ...
Sub CreateTranslationsStatusWikiFile(ByVal sWikiPath, ByVal oTranslationsStatus)
  Dim oWikiFile, sLanguage, oLanguageStatus
  
  Set oWikiFile = oFSO.CreateTextFile(sWikiPath, True)
  
  oWikiFile.WriteLine "== Translations Status =="
  oWikiFile.WriteLine "Status from '''" & GetCreationDate() & "''':"
  oWikiFile.WriteLine "{| class=""wikitable"" border=""1"""
  oWikiFile.WriteLine "! Language"
  oWikiFile.WriteLine "! Total"
  oWikiFile.WriteLine "! Translated"
  oWikiFile.WriteLine "! Fuzzy"
  oWikiFile.WriteLine "! Untranslated"
  oWikiFile.WriteLine "! Last Update"
  For Each sLanguage In oTranslationsStatus.Keys 'For all languages...
    If (sLanguage <> "English") Then 'If NOT English...
      Set oLanguageStatus = oTranslationsStatus(sLanguage)
      oWikiFile.WriteLine "|-"
      oWikiFile.WriteLine "|align=""left""| " & sLanguage
      oWikiFile.WriteLine "|align=""right""| " & oLanguageStatus("count")
      oWikiFile.WriteLine "|align=""right""| " & oLanguageStatus("translated")
      oWikiFile.WriteLine "|align=""right""| " & oLanguageStatus("fuzzy")
      oWikiFile.WriteLine "|align=""right""| " & oLanguageStatus("untranslated")
      oWikiFile.WriteLine "|align=""center""| " & Left(oLanguageStatus("po-revision-date"), 10)
    End If
  Next
  Set oLanguageStatus = oTranslationsStatus("English")
  oWikiFile.WriteLine "|-"
  oWikiFile.WriteLine "|align=""left""| English"
  oWikiFile.WriteLine "|align=""right""| " & oLanguageStatus("count")
  oWikiFile.WriteLine "|align=""right""| " & oLanguageStatus("count")
  oWikiFile.WriteLine "|align=""right""| 0"
  oWikiFile.WriteLine "|align=""right""| 0"
  oWikiFile.WriteLine "|align=""center""| " & Left(oLanguageStatus("pot-creation-date"), 10)
  oWikiFile.WriteLine "|}"
  oWikiFile.Close
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

''
' ...
Function GetCreationDate()
  Dim oNow, sYear, sMonth, sDay, sHour, sMinute
  
  oNow = Now()
  sYear = Year(oNow)
  sMonth = Month(oNow)
  If (sMonth < 10) Then sMonth = "0" & sMonth
  sDay = Day(oNow)
  If (sDay < 10) Then sDay = "0" & sDay
  
  GetCreationDate = sYear & "-" & sMonth & "-" & sDay
End Function
