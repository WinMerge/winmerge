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
 * @file  OpenPartiallyDlg.cpp
 *
 * @brief Implementation of the dialog for opening files partially.
 *
 */
// ID line follows -- this is updated by SVN
// $Id: OpenPartiallyDlg.cpp 179 2008-12-03 22:01:59Z kimmov $

#include "precomp.h"
#include "resource.h"
#include "hexwnd.h"
#include "hexwdlg.h"

int OpenPartiallyDlg::filehandle = 0;
int OpenPartiallyDlg::bShowFileStatsPL = 0;

BOOL OpenPartiallyDlg::OnInitDialog(HWND hDlg)
{
	__int64 iPLFileLen = _filelengthi64(filehandle);
	TCHAR buf[128];
	SetDlgItemText(hDlg, IDC_EDIT1, _T("x0"));
	_stprintf(buf, _T("Size of file: %lld. Load how many bytes:"), iPLFileLen);
	SetDlgItemText(hDlg, IDC_STATIC2, buf);
	_stprintf(buf, _T("%lld"), iPLFileLen);
	SetDlgItemText(hDlg, IDC_EDIT2, buf);
	CheckDlgButton(hDlg, IDC_RADIO1, BST_CHECKED);
	CheckDlgButton(hDlg, IDC_CHECK1, bShowFileStatsPL);
	return TRUE;
}

BOOL OpenPartiallyDlg::Apply(HWND hDlg)
{
	__int64 iPLFileLen = _filelengthi64(filehandle);
	bShowFileStatsPL = IsDlgButtonChecked(hDlg, IDC_CHECK1);
	TCHAR buf[32];
	int iNumBytesPl;
	// Only complain about wrong offset in start offset editbox if loading from start.
	if (GetDlgItemText(hDlg, IDC_EDIT2, buf, RTL_NUMBER_OF(buf)) &&
		_stscanf(buf, _T("%lld"), &iNumBytesPl) == 0)
	{
		MessageBox(hDlg, _T("Number of bytes not recognized."), _T("Open partially"), MB_ICONERROR);
		return FALSE;
	}

	if (iNumBytesPl >= INT_MAX)
	{
		MessageBox(hDlg, _T("Cannot open more than 2 GB of data."),
				_T("Open partially"), MB_ICONERROR);
		return FALSE;
	}

	__int64 iStartPL;
	if (IsDlgButtonChecked(hDlg, IDC_RADIO2))
	{
		// Load from end of file: arguments must be adapted.
		iStartPL = iPLFileLen - iNumBytesPl;
		if (iStartPL < 0)
		{
			MessageBox(hDlg, _T("Specified number of bytes to load\ngreater than file size."), _T("Open partially"), MB_ICONERROR);
			return FALSE;
		}
	}
	else if (GetDlgItemText(hDlg, IDC_EDIT1, buf, RTL_NUMBER_OF(buf)) &&
		_stscanf(buf, _T("x%llx"), &iStartPL) == 0 &&
		_stscanf(buf, _T("%lld"), &iStartPL) == 0)
	{
		MessageBox(hDlg, _T("Start offset not recognized."), _T("Open partially"), MB_ICONERROR);
		return FALSE;
	}
	if (iStartPL + iNumBytesPl > iPLFileLen)
	{
		MessageBox(hDlg, _T("Too many bytes to load."), _T("Open partially"), MB_ICONERROR);
		return FALSE;
	}
	BOOL done = FALSE;
	if (DataArray.SetSize((int)iNumBytesPl))
	{
		DataArray.ExpandToSize();
		_lseeki64(filehandle, iStartPL, 0);
		iPartialOffset = iStartPL;
		iPartialOpenLen = (int)iNumBytesPl;
		iPartialFileLen = iPLFileLen;
		bPartialStats = bShowFileStatsPL;
		if (_read(filehandle, DataArray, iNumBytesPl) != -1)
		{
			done = TRUE;
		}
		else
		{
			DataArray.ClearAll();
			MessageBox(hDlg, _T("Error while reading from file."), _T("Open partially"), MB_ICONERROR);
		}
	}
	else
	{
		MessageBox(hDlg, _T("Not enough memory to load file."), _T("Open partially"), MB_ICONERROR);
	}
	return done;
}

INT_PTR OpenPartiallyDlg::DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_INITDIALOG:
		return OnInitDialog(hDlg);
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
		case IDC_RADIO1:
		case IDC_RADIO2:
			EnableWindow(GetDlgItem(hDlg, IDC_EDIT1), IsDlgButtonChecked(hDlg, IDC_RADIO1));
			return TRUE;
		}
		break;
	}
	return FALSE;
}
