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
Option Explicit

Public Property Get PluginEvent() As String
         PluginEvent = "BUFFER_PACK_UNPACK"
End Property

Public Property Get PluginDescription() As String
         PluginDescription = "This plugins hides the last non-space character"
End Property

Public Property Get PluginFileFilters() As String
         PluginFileFilters = "\.nolast$"
End Property

Public Property Get PluginIsAutomatic() As Boolean
         PluginIsAutomatic = True
End Property



Public Function UnpackBufferA(ByRef buffer() As Byte, ByRef size As Long, ByRef bChanged As Boolean, ByRef subcode As Long) As Boolean
    If (size < 10) Then
        subcode = -1
        bChanged = False
        UnpackBufferA = True
        Exit Function
    End If
    
    ' memorize and delete the last letter (exclude space/EOL characters)
    Dim i
    Dim oldsubcode
    Dim nIgnored
    nIgnored = 0
    For i = size - 1 To 0 Step -1
        subcode = buffer(i)
        buffer(i) = oldsubcode
        oldsubcode = subcode
        If (oldsubcode > 32) Then
        Exit For
        End If
        nIgnored = nIgnored + 1
    Next
    
    subcode = subcode + (nIgnored * 256)
    size = size - 1
    bChanged = True
    UnpackBufferA = True
End Function

Public Function PackBufferA(ByRef buffer() As Byte, ByRef size As Long, ByRef bChanged As Boolean, subcode As Long) As Boolean
    If subcode = -1 Then
        bChanged = False
        PackBufferA = True
        Exit Function
    End If
    
    ' restore the last letter
    size = size + 1
    ReDim Preserve buffer(0 To size - 1)
    
    Dim nIgnored
    For nIgnored = 1 To (subcode / 256)
        buffer(size - nIgnored) = buffer(size - nIgnored - 1)
    Next
    buffer(size - (subcode / 256) - 1) = (subcode And 255)
    
    bChanged = True
    PackBufferA = True
End Function

