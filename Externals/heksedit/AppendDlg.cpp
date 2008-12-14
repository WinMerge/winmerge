/////////////////////////////////////////////////////////////////////////////
//    License (GPLv2+):
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but
//    WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  AppendDlg.cpp
 *
 * @brief Implementation of the Append dialog.
 *
 */
// ID line follows -- this is updated by SVN
// $Id: AppendDlg.cpp 199 2008-12-09 19:59:53Z kimmov $

#include "precomp.h"
#include "resource.h"
#include "hexwnd.h"
#include "hexwdlg.h"

BOOL AppendDlg::Apply(HWND hDlg)
{
	TCHAR buf[32];
	int iAppendbytes;
	if (GetDlgItemText(hDlg, IDC_APPEND_BYTES, buf, RTL_NUMBER_OF(buf)) &&
		_stscanf(buf, _T("%d"), &iAppendbytes) == 0)
	{
		MessageBox(hDlg, _T("Number of bytes to append not recognized."), _T("Append"), MB_ICONERROR);
		return FALSE;
	}
	int oldupbound = DataArray.GetLength();
	if (!DataArray.SetSize(DataArray.GetSize() + iAppendbytes))
	{
		MessageBox(hDlg, _T("Not enough memory for appending."), _T("Append"), MB_ICONERROR);
		return FALSE;
	}
	DataArray.ExpandToSize();
	WaitCursor wc;
	memset(&DataArray[oldupbound], 0, iAppendbytes);
	iFileChanged = TRUE;
	bFilestatusChanged = TRUE;
	resize_window();
	return TRUE;
}

INT_PTR AppendDlg::DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_INITDIALOG:
		SetDlgItemInt(hDlg, IDC_APPEND_BYTES, 1, TRUE);
		return TRUE;
	case WM_COMMAND:
		switch (wParam)
		{
		case IDOK:
			if (Apply(hDlg))
			{
			case IDCANCEL:
				EndDialog(hDlg, wParam);
			}
			return TRUE;
		}
		break;
	}
	return FALSE;
}
