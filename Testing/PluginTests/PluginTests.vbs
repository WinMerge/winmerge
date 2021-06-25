Option Explicit

Dim FileSys
Dim ScriptFolder

Set FileSys = CreateObject("Scripting.FileSystemObject")
ScriptFolder = FileSys.getParentFolderName(WScript.ScriptFullName)

Sub AssertTrue(ByVal value, ByVal msg)
    If Not value Then
        Err.Raise 30001, , msg
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
    Set p = GetObject("script: " & ScriptFolder & "\..\..\Plugins\dlls\editor addin.sct")

    ' Trim
    AssertTrue ("aaa" = p.Trim("aaa")), "Trim1"
    AssertTrue ("aaa" = p.Trim(" aaa")), "Trim2"
    AssertTrue ("aaa" = p.Trim("aaa  ")), "Trim3"
    AssertTrue ("aaa" = p.Trim("  aaa  ")), "Trim4"
    AssertTrue ("aaa" = p.Trim(vbTab & "  aaa  " & vbTab)), "Trim5"
    AssertTrue ("aaa" & vbCrLf & "bbb" = p.Trim(" aaa  " & vbCrLf & "  bbb ")), "Trim6"
    AssertTrue ("aaa" & vbCrLf & "bbb" & vbCrLf = p.Trim(" aaa  " & vbCrLf & "  bbb " & vbCrLf)), "Trim7"
End Sub

EditorAddinTest

'CompareMSExcelFilesTest


