Dim fso: Set fso = CreateObject("Scripting.FileSystemObject")
Dim sh: Set sh = CreateObject("WScript.Shell")
Dim otf: Set otf = fso.OpenTextFile(WScript.Arguments.Unnamed(0), 1)

Do Until otf.AtEndOfStream = True
	WScript.Echo sh.ExpandEnvironmentStrings(otf.ReadLine)
Loop

otf.Close

