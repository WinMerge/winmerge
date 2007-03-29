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
'+----------------------------------------------------------------------+
'| This is a plugin for WinMerge <www.winmerge.org>.                    |
'| It will ignores leading line numbers in text files.                  |
'| Copyright (C) 2007 by Tim Gerundt                                    |
'+----------------------------------------------------------------------+
'| This program is free software; you can redistribute it and/or modify |
'| it under the terms of the GNU General Public License as published by |
'| the Free Software Foundation; either version 2 of the License, or    |
'| (at your option) any later version.                                  |
'|                                                                      |
'| This program is distributed in the hope that it will be useful,      |
'| but WITHOUT ANY WARRANTY; without even the implied warranty of       |
'| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
'| GNU General Public License for more details.                         |
'|                                                                      |
'| You should have received a copy of the GNU General Public License    |
'| along with this program; if not, write to the Free Software          |
'| Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.            |
'+----------------------------------------------------------------------+
Option Explicit

Public Property Get PluginEvent() As String
  PluginEvent = "BUFFER_PREDIFF"
End Property

Public Property Get PluginDescription() As String
  PluginDescription = "This plugin ignores the leading line numbers in text files (e.g. NC and BASIC files)."
End Property

Public Property Get PluginFileFilters() As String
  PluginFileFilters = "\.nc$"
End Property

Public Property Get PluginIsAutomatic() As Boolean
  PluginIsAutomatic = True
End Property

Public Function PrediffBufferW(ByRef text As String, ByRef size As Long, ByRef bChanged As Boolean) As Boolean
  Dim sTemp As String, bLineHasStarted As Boolean
  Dim i As Long, sChar As String
  
  sTemp = ""
  bChanged = False
  bLineHasStarted = True
  For i = 0 To size - 1 'For all chars...
    sChar = Mid(text, i + 1, 1)
    Select Case sChar
      Case "N", "n", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0" 'N or number...
        If (bLineHasStarted = True) Then 'If line has started...
          sChar = ""
          bChanged = True
        End If
      Case vbCr, vbLf 'EOL...
        bLineHasStarted = True
      Case Else
        bLineHasStarted = False
    End Select
    sTemp = sTemp & sChar
  Next i
  
  If (bChanged = True) Then 'If text has changed...
    text = sTemp
    size = Len(sTemp)
  End If
  PrediffBufferW = True
End Function
