Option Explicit
' CheckTranslations.vbs
' WinMerge Project
'
' This script compares each language resource (translation) against the English
' and then reports how many differ, and offers to pop them up, one at a time, in WinMerge, for merging
'
' Created: 2006-01-02, Perry Rapp
' $Id$
'

'Globals
Dim noisy
Dim test

Call Main

Function GeneratePrompt(DiffList)
  Dim PromptMsg
  PromptMsg = "Translations to be edited:"
  Dim i
  For i=1 To UBound(DiffList)
    Dim Language
    Language = DiffList(i)
    If i > 1 Then
      PromptMsg = PromptMsg + ","
    End If
    PromptMsg = PromptMsg + " " + Language
  Next
  PromptMsg = PromptMsg & VbCrLf & "Do you want to edit language " + Language + " now?"
  GeneratePrompt = PromptMsg
End Function

Sub Main
  ' Check for command line arguments to this vbs file
  noisy = IsArgTrue(WScript.Arguments.Named("noisy"), "noisy")
  test = IsArgTrue(WScript.Arguments.Named("test"), "test")

  Dim LanguageList
  LanguageList = Array("Brazilian", "Bulgarian", "Catalan", "ChineseSimplified", _
   "ChineseTraditional", "Czech", "Danish", "Dutch", "German", "Hungarian", _
   "Italian", "Japanese", "Korean", "Norwegian", "Polish", "Russian", "Slovak", _
   "Spanish", "Swedish", "Turkish")

  ReDim DiffList(0)
  Dim DiffCount
  DiffCount = 0

  Dim fso
  Set fso = CreateObject("Scripting.FileSystemObject")

  Dim WinMergePath
  WinMergePath = "..\..\Build\MergeUnicodeRelease\WinMergeU.exe"

  If Not fso.FileExists(WinMergePath) Then
    Fail "Required WinMerge binary does not exist: " + WinMergePath
    Exit Sub
  End If

  Dim prediffer
  prediffer = "..\..\Build\MergeUnicodeRelease\MergePlugins\RCLocalizationHelperU.dll"
  If Not fso.FileExists(prediffer) Then
    Fail "Required RC prediffer binary does not exist: " + prediffer
    Exit Sub
  End If

  Set fso = Nothing

  Dim i
  Dim Language
  Dim rtn
  For i=LBound(LanguageList) To UBound(LanguageList)
    Language = LanguageList(i)
    rtn = CompareEnglishTo(WinMergePath, Language, "/noninteractive")
    If rtn <> 0 Then
      DiffCount = DiffCount + 1
      ReDim Preserve DiffList(DiffCount)
      DiffList(DiffCount) = Language
    End If
  Next

  If DiffCount > 0 Then
    While UBound(DiffList)>0
      Dim PromptMsg
      PromptMsg = GeneratePrompt(DiffList)
      rtn = MsgBox(PromptMsg, MB_YESNOCANCEL + MB_ICONQUESTION)
      If rtn = IDCANCEL Then Exit Sub
      DiffCount = UBound(DiffList)
      Language = DiffList(DiffCount)
      ReDim Preserve DiffList(DiffCount-1)
      If rtn = IDYES Then
        rtn = CompareEnglishTo(WinMergePath, Language, "")
      End If
    Wend
  End If

End Sub

Function CompareEnglishTo(WinMergePath, Language, options)
  Dim cmd
  cmd = WinMergePath
  cmd = cmd + " /prediffer:RCLocalizationHelperU.dll /ignorews:1"
  cmd = cmd + " /ub"
  cmd = cmd + " " + options
  cmd = cmd + " ..\Merge.rc " + Language + "\Merge" + Language + ".rc"
  NoisyMsg cmd
  Dim rtn
  rtn = RunCmd(cmd)
  CompareEnglishTo = rtn
End Function

Function IsArgTrue(value, name)
  Dim val2
  val2 = UCase(value)
  If val2 ="TRUE" or val2="YES" or val2="Y" or _
    val2="1" or val2="-1" or val2=UCase(name) Then
    IsArgTrue = True
  else
    IsArgTrue = False  
  End If
End Function

' Echo message if noisy flag is set
Sub NoisyMsg(message)
  If noisy Then
    Wscript.Echo message
  End If
End Sub

' Call via Shell the specified commandline, synchronously
Function RunCmd(cmd)
  Dim rtn
  rtn = 0
  Dim oShell
  Set oShell = WScript.CreateObject("WScript.Shell")
  If test Then
    NoisyMsg "Test, would run: " + cmd
  Else
    NoisyMsg "Running: " + cmd
    rtn = oShell.Run("%comspec% /c " + cmd, 0, True)
  End If
  RunCmd = rtn
End Function

Sub Fail(msg)
  MsgBox msg, MB_ICONSTOP
End Sub

' MsgBox parameters
Const MB_OK = 0
Const MB_OKCANCEL = 1
Const MB_ABORTRETRYIGNORE = 2
Const MB_YESNOCANCEL = 3
Const MB_YESNO = 4
Const MB_RETRYCANCEL = 5

Const MB_ICONSTOP = 16
Const MB_ICONQUESTION = 32
Const MB_ICONEXCLAMATION = 48
Const MB_ICONINFORMATION = 64

Const MB_DEFBUTTON1 = 0
Const MB_DEFBUTTON2 = 256
Const MB_DEFBUTTON3 = 512

' MsgBox return values
Const IDOK = 1
Const IDCANCEL = 2
Const IDABORT = 3
Const IDRETRY = 4
Const IDIGNORE = 5
Const IDYES = 6
Const IDNO = 7
