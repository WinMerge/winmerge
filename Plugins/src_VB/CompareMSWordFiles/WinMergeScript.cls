VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
  Persistable = 0  'NotPersistable
  DataBindingBehavior = 0  'vbNone
  DataSourceBehavior  = 0  'vbNone
  MTSTransactionMode  = 0  'NotAnMTSObject
END
Attribute VB_Name = "WinMergeScript"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = True
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = True
'/////////////////////////////////////////////////////////////////////////////
'    This is a plugin for WinMerge.
'    It will display the text content of MS Word files.
'    Copyright (C) 2005  Christian List
'    Portions contributed by March Hare Software Ltd February 2006
'
'    This program is free software; you can redistribute it and/or modify
'    it under the terms of the GNU General Public License as published by
'    the Free Software Foundation; either version 2 of the License, or
'    (at your option) any later version.
'
'    This program is distributed in the hope that it will be useful,
'    but WITHOUT ANY WARRANTY; without even the implied warranty of
'    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
'    GNU General Public License for more details.
'
'    You should have received a copy of the GNU General Public License
'    along with this program; if not, write to the Free Software
'    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
'
'/////////////////////////////////////////////////////////////////////////////

Option Explicit

Dim myLastErrorNumber As Long
Dim myLastErrorString As String

Private Declare Function GetTempPath Lib "kernel32" _
    Alias "GetTempPathA" (ByVal nBufferLength As Long, _
    ByVal lpBuffer As String) As Long

Private Declare Function GetTempFileName Lib "kernel32" _
    Alias "GetTempFileNameA" (ByVal lpszPath As String, _
    ByVal lpPrefixString As String, ByVal wUnique As Long, _
    ByVal lpTempFileName As String) As Long

Public Property Get PluginEvent() As String
    PluginEvent = "FILE_PACK_UNPACK"
End Property

Public Property Get PluginDescription() As String
    PluginDescription = "Display MS Word text content."
End Property

Public Property Get PluginFileFilters() As String
    PluginFileFilters = "\.doc(\..*)?$;\.dot(\..*)?$;\.docx(\..*)?$;\.docm(\..*)?$;\.dotx(\..*)?$;\.dotm(\..*)?$"
End Property

Public Property Get PluginIsAutomatic() As Boolean
    PluginIsAutomatic = True
End Property

Public Property Get LastErrorNumber() As Long
    LastErrorNumber = myLastErrorNumber
End Property

Public Property Get LastErrorString() As String
    LastErrorString = myLastErrorString
End Property

Private Function GetMacrosHead(objDoc As Object) As String
    Dim oTextToSave As String
    
    On Error GoTo NoMacrosHead
    
    oTextToSave = ""
    If Not objDoc.VBProject Is Nothing Then
        oTextToSave = oTextToSave & "The VB Project Name is " & objDoc.VBProject.Name & vbCrLf
        If Not objDoc.VBProject.VBComponents Is Nothing Then
            oTextToSave = oTextToSave & "There are " & objDoc.VBProject.VBComponents.Count & _
                " Microsoft Word macros in this document." & vbCrLf
        End If
    End If
    GetMacrosHead = oTextToSave
    Exit Function
    
NoMacrosHead:
    If Err = -2147188160 Or Err = -2146822220 Or Err = 6068 Then
        oTextToSave = "Cannot get Macros." & vbCrLf & _
            "   To allow WinMerge to compare macros, use MS Office to alter the settings in the Macro Security for the current application." & vbCrLf & _
            "   The Trust access to Visual Basic Project feature should be turned on to use this feature in WinMerge." & vbCrLf
    Else
        oTextToSave = oTextToSave & "There are no Microsoft Word macros in this document." & vbCrLf
    End If
    GetMacrosHead = oTextToSave
End Function

Private Function GetMacros(objDoc As Object) As String
    Dim VBComp As Object
    Dim iCountMacros As Integer
    Dim oMacroLine As String
    Dim oTextToSave As String
    Dim macTempPaths() As String
    Dim hFile As Long
    
    On Error GoTo GetMacros
    
    oTextToSave = ""
    If Not objDoc.VBProject.VBComponents Is Nothing Then
        If objDoc.VBProject.VBComponents.Count > 0 Then
            ReDim macTempPaths(objDoc.VBProject.VBComponents.Count - 1) As String
            oTextToSave = oTextToSave & "Macros in document" & vbCrLf
            
            iCountMacros = 0
            For Each VBComp In objDoc.VBProject.VBComponents
                oTextToSave = oTextToSave & VBComp.Name & vbCrLf
                
                macTempPaths(iCountMacros) = CreateTempFile("WMS")
                
                ' Remove the temporary file
                Kill macTempPaths(iCountMacros)
                
                ' Save the text content of the macro
                VBComp.Export macTempPaths(iCountMacros)
                
                ' Read the content back from the file
                hFile = FreeFile
                Open macTempPaths(iCountMacros) For Input Shared As #hFile
                    Do While Not EOF(1) 'Loop until end of file...
                        Line Input #hFile, oMacroLine 'Read line into variable.
                        oTextToSave = oTextToSave & oMacroLine & vbCrLf
                    Loop
                Close #hFile
                
                oTextToSave = oTextToSave & vbCrLf
                iCountMacros = iCountMacros + 1
            Next
        End If
    End If
    GetMacros = oTextToSave
    Exit Function
    
GetMacros:
    oTextToSave = ""
    GetMacros = oTextToSave
End Function

Private Function GetDocProperty(objDoc As Object, pName As String)
  On Error GoTo ErrHandler
  
  GetDocProperty = ""
  If Not objDoc.BuiltinDocumentProperties.Item(pName) Is Nothing Then
      GetDocProperty = objDoc.BuiltinDocumentProperties.Item(pName).Value
  End If
  
  Exit Function

ErrHandler:
  GetDocProperty = ""
End Function

Public Function UnpackFile(fileSrc As String, fileDst As String, ByRef bChanged As Boolean, ByRef subcode As Long) As Boolean
    On Error GoTo CleanUp
    
    myLastErrorNumber = 0
    myLastErrorString = ""
    
    ' Start MS Word
    Dim objWD As Object
    Set objWD = CreateObject("Word.Application")

    objWD.DisplayAlerts = False
    
    ' Load the document into MS Word
    Dim objDoc As Object
    Set objDoc = objWD.Documents.Open(fileSrc)

    Dim oTextToSave As String
    
'    On Error Resume Next
'
'    oTextToSave = oTextToSave & "Document Properties" & vbCrLf
'    oTextToSave = oTextToSave & GetMacrosHead(objDoc)
'
'    On Error GoTo 0
'
    Dim itemValue As String
    Dim hFile As Long
'
'    ' Get the document properties
'    On Error Resume Next
'    Dim p As Object
'    For Each p In objDoc.BuiltinDocumentProperties
'        oTextToSave = oTextToSave & p.Name
'        oTextToSave = oTextToSave & " = "
'        itemValue = GetDocProperty(objDoc, p.Name)
'        If itemValue <> "" Then
'            oTextToSave = oTextToSave & itemValue
'        End If
'        oTextToSave = oTextToSave & vbCrLf
'    Next
'    On Error GoTo CleanUp
'
'    oTextToSave = oTextToSave & vbCrLf
    
    ' Get the Macros
    oTextToSave = oTextToSave & GetMacros(objDoc)
    
    On Error GoTo CleanUp
    
    oTextToSave = oTextToSave & vbCrLf
   
    ' Bookmarks
    On Error Resume Next
    Dim nms As Object
    Set nms = objDoc.Bookmarks
    If nms.Count > 0 Then
        oTextToSave = oTextToSave & "Bookmarks in document" & vbCrLf
    End If
    Dim iCountNames As Integer
    For iCountNames = 1 To nms.Count
        If nms(iCountNames).Name <> "" Then
            oTextToSave = oTextToSave & nms(iCountNames).Name & vbCrLf
        End If
        iCountNames = iCountNames + 1
    Next
    On Error GoTo 0
    oTextToSave = oTextToSave & vbCrLf
    
    ' Save the text content of the document
    oTextToSave = oTextToSave & objDoc.Content.Text & vbCrLf

    ' Save the collected text
    hFile = FreeFile
    Open fileDst For Output Shared As #hFile
    Print #hFile, oTextToSave
    Close #hFile

    ' Close the document without saving changes
    objDoc.Close False
    
    bChanged = True
    UnpackFile = True
    subcode = 1
    
CleanUp:
    myLastErrorNumber = Err
    myLastErrorString = CStr(Err) & ": " & Error(myLastErrorNumber)

    If Not objWD Is Nothing Then
        ' Stop MS Word
        objWD.Quit
    End If
End Function
 
Public Function PackFile(fileSrc As String, fileDst As String, ByRef bChanged As Boolean, subcode As Long) As Boolean
    ' We can't repack MS Word files
    bChanged = False
    PackFile = False
    subcode = 1
End Function

' Returns complete path and name for a temporary file
Private Function CreateTempFile(sPrefix As String) As String
    Dim sTmpPath As String * 512
    Dim sTmpName As String * 576
    Dim nRet As Long
    
    nRet = GetTempPath(512, sTmpPath)
    If (nRet > 0 And nRet < 512) Then
        nRet = GetTempFileName(sTmpPath, sPrefix, 0, sTmpName)
        If nRet <> 0 Then
            CreateTempFile = Left$(sTmpName, InStr(sTmpName, vbNullChar) - 1)
        End If
    End If
End Function

Public Function ShowSettingsDialog() As Boolean
    ShowSettingsDialog = False
    Err.Raise 30001, , "Not Implemented"
End Function

