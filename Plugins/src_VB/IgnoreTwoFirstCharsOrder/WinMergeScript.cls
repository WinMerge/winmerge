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
         PluginEvent = "PREDIFFING"
End Property

Public Property Get PluginDescription() As String
         PluginDescription = "This plugins ignores the difference concerning the order of the two first characters"
End Property

Public Property Get PluginFileFilters() As String
         PluginFileFilters = "\.txt$"
End Property

Public Property Get PluginIsAutomatic() As Boolean
         PluginIsAutomatic = True
End Property

Public Function DiffingPreprocessW(ByRef text As String, ByRef size As Long, ByRef bChanged As Boolean) As Boolean
    If Mid(text, 1, 1) < Mid(text, 2, 1) Then
        Dim c1, c2
        c1 = Mid(text, 1, 1)
        c2 = Mid(text, 2, 1)
        text = c2 + c1 + Mid(text, 3)
        bChanged = True
    Else
        bChanged = False
    End If
    DiffingPreprocessW = True
End Function


