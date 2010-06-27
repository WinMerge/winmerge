Dim fso: Set fso = CreateObject("Scripting.FileSystemObject")

Dim src: Set src = fso.OpenTextFile(WScript.Arguments.Unnamed(0), 1)
Dim txt: txt = src.ReadAll
src.Close

Dim dst: Set dst = fso.CreateTextFile(WScript.Arguments.Unnamed(1), True)
dst.Write Replace(Replace(txt, vbCrLf, vbLf), vbLf, vbCrLf)
dst.Close

