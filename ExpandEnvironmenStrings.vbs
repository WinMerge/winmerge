Dim fso: Set fso = CreateObject("Scripting.FileSystemObject")
Dim sh: Set sh = CreateObject("WScript.Shell")
Dim ot: Set ot = fso.OpenTextFile(WScript.Arguments.Unnamed(0), 1)

Do Until ot.AtEndOfStream = True
	WScript.Echo sh.ExpandEnvironmentStrings(ot.ReadLine)
Loop

ot.Close

