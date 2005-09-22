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

Public Function UnpackFile(fileSrc As String, fileDst As String, ByRef bChanged As Boolean, ByRef subcode As Long) As Boolean
    On Error GoTo CleanUp
    
    ' Start MS Word
    Dim objWD As Object
    Set objWD = CreateObject("Word.Application")
    
    ' Load the document into MS Word
    Dim objDoc As Object
    Set objDoc = objWD.Documents.Open(fileSrc)

    ' Save the text content of the document
    Open fileDst For Output Shared As #1
    Print #1, objDoc.Content.Text
    Close #1
    
    bChanged = True
    UnpackFile = True
    subcode = 1
    
CleanUp:
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

