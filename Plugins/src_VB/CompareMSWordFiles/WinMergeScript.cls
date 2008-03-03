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
'
' RCS ID line follows -- this is updated by CVS
' $Id$

Option Explicit

Dim myLastErrorNumber As Long
Dim myLastErrorString As String

Public Property Get PluginEvent() As String
    PluginEvent = "FILE_PACK_UNPACK"
End Property

Public Property Get PluginDescription() As String
    PluginDescription = "Display the text content of MS Word files."
End Property

Public Property Get PluginFileFilters() As String
    PluginFileFilters = "\.doc$"
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

Public Function UnpackFile(fileSrc As String, fileDst As String, ByRef bChanged As Boolean, ByRef subcode As Long) As Boolean
    On Error GoTo CleanUp
    
    myLastErrorNumber = 0
    myLastErrorString = ""
    
    ' Start MS Word
    Dim objWD As Object
    Set objWD = CreateObject("Word.Application")
    
    ' Load the document into MS Word
    Dim objDoc As Object
    Set objDoc = objWD.Documents.Open(fileSrc)

    Dim oTextToSave As String
    
    On Error Resume Next
    
    oTextToSave = oTextToSave & "Document Properties" & vbCrLf
    oTextToSave = oTextToSave & GetMacrosHead(objDoc)
    
    Dim hFile As Long
    
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

