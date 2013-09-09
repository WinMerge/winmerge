/*
Frhed - Free hex editor
Copyright (C) 2000 Raihan Kibria

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License, version 2, as published by
the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with this program; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA.

Last change: 2013-02-24 by Jochen Neubeck
*/
#ifndef gktools_h
#define gktools_h

#include "EncoderLib.h"

BOOL WINAPI GetDllExportNames(LPCTSTR pszFilename, ULONG* lpulOffset, ULONG* lpulSize);
BOOL WINAPI GetDllImportNames(LPCTSTR pszFilename, ULONG* lpulOffset, ULONG* lpulSize);

extern PartitionInfo* SelectedPartitionInfo;

/**
 * @brief A dialog for selecting encoder DLL.
 */
class EncodeDecodeDialog : public HexEditorWindow
{
public:
	enum { IDD = IDD_ENCODE_DECODE_DIALOG };
	INT_PTR DlgProc(HWindow *, UINT, WPARAM, LPARAM);
};

/**
 * @brief A dialog for opening disk drives.
 */
class OpenDriveDialog : public HexEditorWindow
{
public:
	enum { IDD = IDD_OPEN_DRIVE_DIALOG };
	INT_PTR DlgProc(HWindow *, UINT, WPARAM, LPARAM);
};

/**
 * @brief A dialog for moving in disk.
 */
class GotoTrackDialog : public HexEditorWindow
{
public:
	enum { IDD = IDD_GOTO_TRACK_DIALOG };
	INT_PTR DlgProc(HWindow *, UINT, WPARAM, LPARAM);
};


#endif // gktools_h
