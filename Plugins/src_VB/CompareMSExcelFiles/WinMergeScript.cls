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
'    It will display the text content of MS Excel files.
'    Copyright (C) 2005  Christian List
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
'
' RCS ID line follows -- this is updated by CVS
' $Id$

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
    PluginDescription = "Display the text content of MS Excel files."
End Property

Public Property Get PluginFileFilters() As String
    PluginFileFilters = "\.xls$"
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
Private Function GetComponentCount(objDoc As Object)
'  Returns 0 if there is a problem accessing VBComponents
'  On Error GoTo ErrHandler
  
  GetComponentCount = objDoc.VBProject.VBComponents.Count
  
  Exit Function

ErrHandler:
  GetComponentCount = 0
End Function

Public Function UnpackFile(fileSrc As String, fileDst As String, ByRef bChanged As Boolean, ByRef subcode As Long) As Boolean
    On Error GoTo CleanUp
    
    myLastErrorNumber = 0
    myLastErrorString = ""
    
    ' Start MS Excel
    Dim objWD As Object
    Set objWD = CreateObject("Excel.Application")
    
    ' Load the workbook into MS Excel
    Dim objDoc As Object
    Set objDoc = objWD.Workbooks.Open(fileSrc, 0, True)

    ' Create an array of temporary paths
    ' These temporary files are needed because excel only outputs one page at a time in CSV format
    Dim arrTempPaths() As String
    ReDim arrTempPaths(objDoc.Worksheets.Count - 1) As String
    
    ' (ProjectComponents may be zero due to Code Access Security)
    Dim ProjectComponents As Long
    ProjectComponents = GetComponentCount(objDoc)
    
    Dim macTempPaths() As String
    If ProjectComponents > 0 Then
        ReDim macTempPaths(ProjectComponents - 1) As String
    End If

    Dim iCountSheets As Integer
    iCountSheets = 0
    
    Dim oTextToSave As String
    
    ' Document Properties that are not easy to get
    oTextToSave = oTextToSave & "Document Properties" & vbCrLf
    oTextToSave = oTextToSave + "There are " & objDoc.Excel4MacroSheets.Count & _
        " Microsoft Excel 4.0 macro sheets in this workbook." & vbCrLf
    If ProjectComponents > 0 Then
        oTextToSave = oTextToSave + "The VB Project Name is " & objDoc.VBProject.Name & vbCrLf
        oTextToSave = oTextToSave + "There are " & objDoc.VBProject.VBComponents.Count & _
            " Microsoft Excel macros in this workbook." & vbCrLf
    End If

    Dim itemValue As String
    Dim hFile As Long
    
    ' Get the document properties
    On Error Resume Next
    Dim p As Object
    For Each p In objDoc.BuiltinDocumentProperties
        oTextToSave = oTextToSave + p.Name
        oTextToSave = oTextToSave & " = "
        itemValue = "?"
        itemValue = objDoc.BuiltinDocumentProperties.Item(p.Name).Value
        If itemValue <> "" Then
            oTextToSave = oTextToSave + itemValue
        End If
       oTextToSave = oTextToSave + vbCrLf
    Next
    On Error GoTo 0
    oTextToSave = oTextToSave & vbCrLf

    ' Get the Macros
    If ProjectComponents > 0 Then
        oTextToSave = oTextToSave & "Macros in document" & vbCrLf
        On Error Resume Next
        Dim VBComp As Object
        Dim iCountMacros As Integer
        For Each VBComp In objDoc.VBProject.VBComponents
            VBComp = objDoc.VBProject.VBComponents(iCountMacros)
            oTextToSave = oTextToSave & VBComp.Name & vbCrLf
    
            macTempPaths(iCountMacros) = CreateTempFile("WMS")
    
            ' Remove the temporary file
            Kill macTempPaths(iCountMacros)
    
            ' Save the text content of the macro
            VBComp.Export macTempPaths(iCountMacros)
    
            ' Read the content back from the file
            hFile = FreeFile
            Open macTempPaths(iCountMacros) For Input Shared As #hFile
            'oTextToSave = oTextToSave + macTempPaths(iCountMacros) + vbCrLf
            
            Dim oMacroLine As String
            Do While Not EOF(1)   ' Loop until end of file.
               Line Input #hFile, oMacroLine   ' Read line into variable.
    
               oTextToSave = oTextToSave + oMacroLine + vbCrLf
            Loop
    
            Close #hFile
    
            oTextToSave = oTextToSave + vbCrLf
            iCountMacros = iCountMacros + 1
        'Next VBComp
        Next
        oTextToSave = oTextToSave & vbCrLf
        On Error GoTo 0
    End If
   
    ' Names
    oTextToSave = oTextToSave & "Names in document" & vbCrLf
    On Error Resume Next
    Dim nms As Object
    Set nms = objDoc.Names
    Dim iCountNames As Integer
    For iCountNames = 1 To nms.Count
        oTextToSave = oTextToSave + nms(iCountNames).Name
        oTextToSave = oTextToSave & " = "
        itemValue = "?"
        itemValue = nms(iCountNames).Value
        If itemValue <> "" Then
            oTextToSave = oTextToSave + itemValue
        End If
        oTextToSave = oTextToSave + vbCrLf
        iCountNames = iCountNames + 1
    Next
    On Error GoTo 0
    oTextToSave = oTextToSave & vbCrLf
    
    Dim objSheet As Object
    For Each objSheet In objDoc.Worksheets

        objSheet.Activate
        oTextToSave = oTextToSave + objSheet.Name + vbCrLf
        
        ' Get the comments for this sheet
        Dim cmt As Object
        Dim c As Object
        Set cmt = objSheet.Comments
        For Each c In cmt
            oTextToSave = oTextToSave + c.Author + " " + c.Text + vbCrLf
        Next

        arrTempPaths(iCountSheets) = CreateTempFile("WMS")
        
        ' Remove the temporary file
        Kill arrTempPaths(iCountSheets)

        ' Save the text content of the workbook as comma separated file (CSV format)
        objDoc.SaveAs arrTempPaths(iCountSheets), 6
        
        ' Read the content back from the file
        hFile = FreeFile
        Open arrTempPaths(iCountSheets) For Input Shared As #hFile
        
        Dim oTextLine As String
        Do While Not EOF(1)   ' Loop until end of file.
           Line Input #hFile, oTextLine   ' Read line into variable.
           
           oTextToSave = oTextToSave + oTextLine + vbCrLf
        Loop
        
        Close #hFile

        oTextToSave = oTextToSave + vbCrLf
        iCountSheets = iCountSheets + 1
    Next
    
    
    ' Save the collected text
    hFile = FreeFile
    Open fileDst For Output Shared As #hFile
    Print #hFile, oTextToSave
    Close #hFile

    ' Close the Workbook without saving changes
    objDoc.Close False
    
    ' Now kill all the temporary files
    Dim i As Integer
    For i = 0 To iCountSheets - 1
        ' Remove the temporary file
        Kill arrTempPaths(i)
    Next
    
    bChanged = True
    UnpackFile = True
    subcode = 1
    
CleanUp:
    myLastErrorNumber = Err
    myLastErrorString = CStr(Err) & ": " & Error(myLastErrorNumber)

    If Not objWD Is Nothing Then
        ' Stop MS Excel
        objWD.Quit
    End If
End Function
 
Public Function PackFile(fileSrc As String, fileDst As String, ByRef bChanged As Boolean, subcode As Long) As Boolean
    ' We can't repack MS Excel files
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

