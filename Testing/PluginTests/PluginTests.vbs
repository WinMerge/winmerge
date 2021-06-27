Option Explicit

Dim FileSys
Dim ScriptFolder
Dim g_cnt
Dim g_testname

Set FileSys = CreateObject("Scripting.FileSystemObject")
ScriptFolder = FileSys.getParentFolderName(WScript.ScriptFullName)

Sub SetTestName(testname)
    g_cnt = 0
    g_testname = testname
End Sub

Function GetCurrentTestName()
    GetCurrentTestName = g_testname & g_cnt
End Function

Sub AssertTrue(value)
    g_cnt = g_cnt + 1
    If Not value Then
        Err.Raise 30001, , GetCurrentTestName()
    End If
End Sub

Sub AssertEquals(expected, actual)
    g_cnt = g_cnt + 1
    If expected <> actual Then
	    Err.Raise 30001, , GetCurrentTestName() & vbCrLf & "expected: """ & expected & """" & vbCrLf & "actual: """ & actual & """"
    End If
End Sub

Sub CompareMSExcelFilesTest
    Dim p, changed, subcode
    Set p = GetObject("script: " & ScriptFolder & "\..\..\Plugins\dlls\CompareMSExcelFiles.sct")
    WScript.Echo "PluginDescription: " & p.PluginDescription
    WScript.Echo "PluginEvent      : " & p.PluginEvent      
    WScript.Echo "PluginFileFilters: " & p.PluginFileFilters
    WScript.Echo "PluginIsAutomatic: " & p.PluginIsAutomatic
    On Error Resume Next
    WScript.Echo "PluginUnpackedFileExtension: " & p.PluginUnpackedFileExtension
    On Error GoTo 0

    changed = false
    subcode = 0
    p.UnpackFile ScriptFolder & "\..\Data\Office\excel.xls", ScriptFolder & "\result.txt", changed, subcode
    
End Sub

Sub EditorAddinTest
    Dim p
    Dim i
    Dim asciiChars
    Dim chars
    Set p = GetObject("script: " & ScriptFolder & "\..\..\Plugins\dlls\editor addin.sct")

    For i = 0 To 127
        asciiChars = asciiChars & Chr(i)
    Next

    ' MakeUpper
    SetTestName "MakeUpper"
    AssertEquals "", p.MakeUpper("")
    AssertEquals " ~0129ABCZABCZ", p.MakeUpper(" ~0129abczABCZ")
    AssertEquals Replace(asciiChars, "abcdefghijklmnopqrstuvwxyz", "ABCDEFGHIJKLMNOPQRSTUVWXYZ"), p.MakeUpper(asciiChars)

    ' MakeLower
    SetTestName "MakeLower"
    AssertEquals "", p.MakeLower("")
    AssertEquals " ~0129abczabcz", p.MakeLower(" ~0129abczABCZ")
    AssertEquals Replace(asciiChars, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", "abcdefghijklmnopqrstuvwxyz"), p.MakeLower(asciiChars)

    ' ExecFilterCommand
    SetTestName "ExecFilterCommand"
    p.PluginArguments = "sort"
    AssertEquals "1" & vbCrLf & "2" & vbCrLf, p.ExecFilterCommand("2" & vbCrLf & "1" & vbCrLf)
    p.PluginArguments = "echo a"
    AssertEquals "a " & vbCrLf, p.ExecFilterCommand("")

    ' SelectColumns
    SetTestName "SelectColumns"
    p.PluginArguments = "1"
    AssertEquals "a", p.SelectColumns("abc")
    AssertEquals "a" & vbCrLf & "d", p.SelectColumns("abc" & vbCrLf & "def")
    AssertEquals "a" & vbCrLf & "d" & vbCrLf, p.SelectColumns("abc" & vbCrLf & "def" & vbCrLf)
    p.PluginArguments = "3"
    AssertEquals "c" & vbCrLf & "f", p.SelectColumns("abc" & vbCrLf & "def")
    p.PluginArguments = "1,3"
    AssertEquals "ac" & vbCrLf & "d", p.SelectColumns("abc" & vbCrLf & "de")
    p.PluginArguments = "1-3"
    AssertEquals "abc" & vbCrLf & "def", p.SelectColumns("abc" & vbCrLf & "def")
    p.PluginArguments = "-3"
    AssertEquals "abc" & vbCrLf & "def", p.SelectColumns("abc" & vbCrLf & "def")
    p.PluginArguments = "1-"
    AssertEquals "abc" & vbCrLf & "def", p.SelectColumns("abc" & vbCrLf & "def")
    p.PluginArguments = "2-"
    AssertEquals "bc" & vbCrLf & "ef", p.SelectColumns("abc" & vbCrLf & "def")
    p.PluginArguments = "3-"
    AssertEquals "c" & vbCrLf & "f", p.SelectColumns("abc" & vbCrLf & "def")
    p.PluginArguments = "4-"
    AssertEquals "" & vbCrLf & "", p.SelectColumns("abc" & vbCrLf & "def")

    p.PluginArguments = "-e ab"
    AssertEquals "", p.SelectColumns("")
    AssertEquals "ab" & vbCrLf & "", p.SelectColumns("abc" & vbCrLf & "def")
    p.PluginArguments = "-e ""ab|de"""
    AssertEquals "ab" & vbCrLf & "de", p.SelectColumns("abc" & vbCrLf & "def")
    p.PluginArguments = "-e ^."
    AssertEquals "a" & vbCrLf & "d", p.SelectColumns("abc" & vbCrLf & "def")
    p.PluginArguments = "-e .*"
    AssertEquals "abc" & vbCrLf & "def", p.SelectColumns("abc" & vbCrLf & "def")
    p.PluginArguments = "-e -i ""Ab|dE"""
    AssertEquals "ab" & vbCrLf & "de", p.SelectColumns("abc" & vbCrLf & "def")
    p.PluginArguments = "-e -i -g ab"
    AssertEquals "abAB" & vbCrLf & "", p.SelectColumns("abcABC" & vbCrLf & "def")
    p.PluginArguments = "-v -e ""ab|de"""
    AssertEquals "c" & vbCrLf & "f", p.SelectColumns("abc" & vbCrLf & "def")

    p.PluginArguments = "1-3 -e ab 2-3"
    AssertEquals "abcabbc" & vbCrLf & "defef", p.SelectColumns("abc" & vbCrLf & "def")

    ' SelectLines
    SetTestName "SelectLines"
    p.PluginArguments = "1"
    AssertEquals "", p.SelectLines("")
    AssertEquals "aaa", p.SelectLines("aaa")
    p.PluginArguments = "2"
    AssertEquals "bbb", p.SelectLines("aaa" & vbCrLf & "bbb")
    p.PluginArguments = "1,2"
    AssertEquals "aaa" & vbCrLf & "bbb", p.SelectLines("aaa" & vbCrLf & "bbb")
    p.PluginArguments = "1-2"
    AssertEquals "aaa" & vbCrLf & "bbb", p.SelectLines("aaa" & vbCrLf & "bbb")
    AssertEquals "aaa" & vbCrLf & "bbb" & vbCrLf, p.SelectLines("aaa" & vbCrLf & "bbb" & vbCrLf)
    p.PluginArguments = "1-"
    AssertEquals "aaa" & vbCrLf & "bbb", p.SelectLines("aaa" & vbCrLf & "bbb")
    AssertEquals "aaa" & vbCrLf & "bbb" & vbCrLf, p.SelectLines("aaa" & vbCrLf & "bbb" & vbCrLf)
    AssertEquals "aaa" & vbCrLf & vbCrLf & vbCrLf, p.SelectLines("aaa" & vbCrLf & vbCrLf & vbCrLf)
    p.PluginArguments = "-2"
    AssertEquals "aaa" & vbCrLf & "bbb", p.SelectLines("aaa" & vbCrLf & "bbb")
    AssertEquals "aaa" & vbCrLf & "bbb" & vbCrLf, p.SelectLines("aaa" & vbCrLf & "bbb" & vbCrLf)
    p.PluginArguments = "4-5"
    AssertEquals "", p.SelectLines("aaa" & vbCrLf & "bbb")
    AssertEquals "", p.SelectLines("aaa" & vbCrLf & "bbb" & vbCrLf)

    p.PluginArguments = "-e aa"
    AssertEquals "", p.SelectLines("")
    AssertEquals "aaa", p.SelectLines("aaa" & vbCrLf & "bbb")
    AssertEquals "aaa" & vbCrLf, p.SelectLines("aaa" & vbCrLf & "bbb" & vbCrLf)
    p.PluginArguments = "-e -i AA"
    AssertEquals "aaa", p.SelectLines("aaa" & vbCrLf & "bbb")
    AssertEquals "aaa" & vbCrLf, p.SelectLines("aaa" & vbCrLf & "bbb" & vbCrLf)
    p.PluginArguments = "-e .*"
    AssertEquals "aaa" & vbCrLf & "bbb", p.SelectLines("aaa" & vbCrLf & "bbb")
    AssertEquals "aaa" & vbCrLf & "bbb" & vbCrLf, p.SelectLines("aaa" & vbCrLf & "bbb" & vbCrLf)
    p.PluginArguments = "^a"
    AssertEquals "aaa", p.SelectLines("aaa" & vbCrLf & "bbb")
    AssertEquals "aaa" & vbCrLf, p.SelectLines("aaa" & vbCrLf & "bbb" & vbCrLf)
    p.PluginArguments = "a$"
    AssertEquals "aaa", p.SelectLines("aaa" & vbCrLf & "bbb")
    AssertEquals "aaa" & vbCrLf, p.SelectLines("aaa" & vbCrLf & "bbb" & vbCrLf)
    p.PluginArguments = "-v a$"
    AssertEquals "bbb", p.SelectLines("aaa" & vbCrLf & "bbb")
    AssertEquals "bbb" & vbCrLf, p.SelectLines("aaa" & vbCrLf & "bbb" & vbCrLf)

    p.PluginArguments = "1 -e c$"
    AssertEquals "aaa" & vbCrLf & "ccc", p.SelectLines("aaa" & vbCrLf & "bbb" & vbCrLf & "ccc")

    ' Replace
    SetTestName "Replace"
    p.PluginArguments = "abc def"
    AssertEquals "", p.Replace("")
    AssertEquals "def def def def", p.Replace("abc def abc def")
    p.PluginArguments = "-e a.c XXX"
    AssertEquals "", p.Replace("")
    AssertEquals "XXX def XXX def", p.Replace("abc def abc def")
    p.PluginArguments = "-e -i A.C XXX"
    AssertEquals "XXX def XXX def", p.Replace("abc def abc def")
    p.PluginArguments = "-e ^\d+ XXX"
    AssertEquals "XXX abc" & vbCrLf & "XXX def", p.Replace("1000 abc" & vbCrLf & "1001 def")

    ' ReverseColumns
    SetTestName "ReverseColumns"
    AssertEquals "", p.ReverseColumns("")
    AssertEquals "a", p.ReverseColumns("a")
    AssertEquals "a" & vbCrLf & "b", p.ReverseColumns("a" & vbCrLf & "b")
    AssertEquals "ba", p.ReverseColumns("ab")
    AssertEquals "ba" & vbCrLf & "dc", p.ReverseColumns("ab" & vbCrLf & "cd")
    AssertEquals "cba", p.ReverseColumns("abc")
    AssertEquals "cba" & vbCrLf & "fed", p.ReverseColumns("abc" & vbCrLf & "def")
    AssertEquals "dcba", p.ReverseColumns("abcd")
    AssertEquals "dcba" & vbCrLf & "hgfe", p.ReverseColumns("abcd" & vbCrLf & "efgh")

    ' ReverseLines
    SetTestName "ReverseLines"
    AssertEquals "", p.ReverseLines("")
    AssertEquals "a", p.ReverseLines("a")
    AssertEquals "a" & vbCrLf, p.ReverseLines("a" & vbCrLf)
    AssertEquals "b" & vbCrLf & "a", p.ReverseLines("a" & vbCrLf & "b")
    AssertEquals "b" & vbCrLf & "a" & vbCrLf, p.ReverseLines("a" & vbCrLf & "b" & vbCrLf)
    AssertEquals "c" & vbCrLf & "b" & vbCrLf & "a", p.ReverseLines("a" & vbCrLf & "b" & vbCrLf & "c")
    AssertEquals "c" & vbCrLf & "b" & vbCrLf & "a" & vbCrLf, p.ReverseLines("a" & vbCrLf & "b" & vbCrLf & "c" & vbCrLf)
    AssertEquals "d" & vbCrLf & "c" & vbCrLf & "b" & vbCrLf & "a", p.ReverseLines("a" & vbCrLf & "b" & vbCrLf & "c" & vbCrLf & "d")

    ' Tokenize
    SetTestName "Tokenize"
    p.PluginArguments = "[^\w]+"
    AssertEquals "", p.Tokenize("")
    AssertEquals "aaa", p.Tokenize("aaa")
    AssertEquals "aaa" & vbCrLf & "abcd", p.Tokenize("aaa abcd")

    ' RemoveDuplicates
    SetTestName "RemoveDuplicates"
    AssertEquals "", p.RemoveDuplicates("")
    AssertEquals "aaa", p.RemoveDuplicates("aaa")
    AssertEquals "aaa", p.RemoveDuplicates("aaa" & vbCrLf & "aaa")
    AssertEquals "aaa" & vbCrLf, p.RemoveDuplicates("aaa" & vbCrLf & "aaa" & vbCrLf)
    AssertEquals "aaa" & vbCrLf & "bbb", p.RemoveDuplicates("aaa" & vbCrLf & "bbb")
    AssertEquals "aaa" & vbCrLf & "bbb" & vbCrLf, p.RemoveDuplicates("aaa" & vbCrLf & "bbb" & vbCrLf)

    ' CountDuplicates
    SetTestName "CountDuplicates"
    AssertEquals "", p.CountDuplicates("")
    AssertEquals "aaa" & vbTab & "1", p.CountDuplicates("aaa")
    AssertEquals "aaa" & vbTab & "2" & vbCrLf, p.CountDuplicates("aaa" & vbCrLf & "aaa")
    AssertEquals "aaa" & vbTab & "2" & vbCrLf, p.CountDuplicates("aaa" & vbCrLf & "aaa" & vbCrLf)
    AssertEquals "aaa" & vbTab & "1" & vbCrLf & "bbb" & vbTab & "1" & vbCrLf, p.CountDuplicates("aaa" & vbCrLf & "bbb")
    AssertEquals "aaa" & vbTab & "1" & vbCrLf & "bbb" & vbTab & "1" & vbCrLf, p.CountDuplicates("aaa" & vbCrLf & "bbb" & vbCrLf)

    ' Trim
    SetTestName "Trim"
    AssertEquals "", p.Trim("")
    AssertEquals "", p.Trim(" ")
    AssertEquals "aaa", p.Trim("aaa")
    AssertEquals "aaa", p.Trim(" aaa")
    AssertEquals "aaa", p.Trim("aaa  ")
    AssertEquals "aaa", p.Trim("  aaa  ")
    AssertEquals "aaa", p.Trim(vbTab & "  aaa  " & vbTab)
    AssertEquals "aaa" & vbCrLf & "bbb", p.Trim(" aaa  " & vbCrLf & "  bbb ")
    AssertEquals "aaa" & vbCrLf & "bbb" & vbCrLf, p.Trim(" aaa  " & vbCrLf & "  bbb " & vbCrLf)

End Sub

EditorAddinTest
'CompareMSExcelFilesTest

