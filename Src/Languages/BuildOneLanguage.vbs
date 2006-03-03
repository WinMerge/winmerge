' BuildOneLanguage.vbs
' Ask user for language name, and recompile it (BuildDll.bat)
' and recopy all language files to build runtime (CopyAll.bat)
'
' RCS ID line follows -- this is updated by CVS
' $Id$

Option Explicit

Call Main

Sub Main
  Dim msg : msg = "Build which language?"
  Dim title : title = "Select language"
  Dim defval : defval = ""
  Dim language
  language = InputBox(msg, title, defval)
  If Len(language)<1 Then Exit Sub

  Dim cmd
  rem -v=verbose, -p=pause
  cmd = ".\MakeResDll -v -p -o .\DLL .\" & language & "\Merge" & language & ".rc"

  Dim WshShell : Set WshShell = WScript.CreateObject("WScript.Shell")
  rem 1=Window Style, True=WaitOnReturn
  Dim rtn
  rtn = WshShell.Run(cmd, 1, True)
  If rtn<>0 Then
    MsgBox "Compilation failed", vbCritical, "Fatal Error"
    Exit Sub
  End If

  WshShell.Run "CopyAll.bat " & language, 1, True

End Sub
