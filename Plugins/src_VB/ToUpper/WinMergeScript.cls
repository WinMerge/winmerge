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
         PluginEvent = "CONTEXT_MENU"
End Property

Public Property Get PluginDescription() As String
         PluginDescription = "A simple function to convert text to uppercase"
End Property


Public Function MakeUpperVB(text As String)
    MakeUpperVB = StrConv(text, vbUpperCase)
End Function



