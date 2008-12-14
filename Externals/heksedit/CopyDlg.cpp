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
 * @file  CopyDlg.cpp
 *
 * @brief Copy dialog implementation.
 *
 */
// ID line follows -- this is updated by SVN
// $Id: CopyDlg.cpp 197 2008-12-09 19:17:06Z kimmov $

#include "precomp.h"
#include "resource.h"
#include "hexwnd.h"
#include "hexwdlg.h"
#include "BinTrans.h"

BOOL CopyDlg::OnInitDialog(HWND hDlg)
{
	int iStart = iGetStartOfSelection();
	int iEnd = iGetEndOfSelection();
	TCHAR buf[32];
	_stprintf(buf, _T("x%x"), iStart);
	SetDlgItemText(hDlg, IDC_COPY_STARTOFFSET, buf);
	_stprintf(buf, _T("x%x"), iEnd);
	SetDlgItemText(hDlg, IDC_COPY_OFFSETEDIT, buf);
	SetDlgItemInt(hDlg, IDC_COPY_BYTECOUNT, iEnd - iStart + 1, TRUE);
	CheckDlgButton(hDlg, IDC_COPY_OFFSET, BST_CHECKED);
	return TRUE;
}

BOOL CopyDlg::Apply(HWND hDlg)
{
	TCHAR buf[32];
	int iOffset;
	int iNumberOfBytes;
	if (GetDlgItemText(hDlg, IDC_COPY_STARTOFFSET, buf, RTL_NUMBER_OF(buf)) &&
		_stscanf(buf, _T("x%x"), &iOffset) == 0 &&
		_stscanf(buf, _T("%d"), &iOffset) == 0)
	{
		MessageBox(hDlg, _T("Start offset not recognized."), _T("Copy"), MB_ICONERROR);
		return FALSE;
	}
	if (IsDlgButtonChecked(hDlg, IDC_COPY_OFFSET))
	{
		if (GetDlgItemText(hDlg, IDC_COPY_OFFSETEDIT, buf, RTL_NUMBER_OF(buf)) &&
			_stscanf(buf, _T("x%x"), &iNumberOfBytes) == 0 &&
			_stscanf(buf, _T("%d"), &iNumberOfBytes) == 0)
		{
			MessageBox(hDlg, _T("End offset not recognized."), _T("Copy"), MB_ICONERROR);
			return FALSE;
		}
		iNumberOfBytes = iNumberOfBytes - iOffset + 1;
	}
	else
	{// Get number of bytes.
		if (GetDlgItemText(hDlg, IDC_COPY_BYTECOUNT, buf, RTL_NUMBER_OF(buf)) &&
			_stscanf(buf, _T("%d"), &iNumberOfBytes) == 0)
		{
			MessageBox(hDlg, _T("Number of bytes not recognized."), _T("Copy"), MB_ICONERROR);
			return FALSE;
		}
	}
	// Can requested number be cut?
	// DataArray.GetLength ()-iCutOffset = number of bytes from current pos. to end.
	if (DataArray.GetLength() - iOffset < iNumberOfBytes)
	{
		MessageBox(hDlg, _T("Can't copy more bytes than are present."), _T("Copy"), MB_ICONERROR);
		return FALSE;
	}
	// Transfer to cipboard.
	int destlen = Text2BinTranslator::iBytes2BytecodeDestLen((char*) &DataArray[iOffset], iNumberOfBytes);
	HGLOBAL hGlobal = GlobalAlloc(GHND, destlen);
	if (hGlobal == 0)
	{
		// Not enough memory for clipboard.
		MessageBox(hDlg, _T("Not enough memory for copying."), _T("Copy"), MB_ICONERROR);
		return FALSE;
	}
	WaitCursor wc;
	char *pd = (char *)GlobalLock(hGlobal);
	Text2BinTranslator::iTranslateBytesToBC(pd, &DataArray[iOffset], iNumberOfBytes);
	GlobalUnlock(hGlobal);
	OpenClipboard(hwnd);
	EmptyClipboard();
	SetClipboardData(CF_TEXT, hGlobal);
	CloseClipboard();
	return TRUE;
}

INT_PTR CopyDlg::DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
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
		case IDC_COPY_OFFSET:
		case IDC_COPY_BYTES:
			EnableWindow(GetDlgItem(hDlg, IDC_COPY_OFFSETEDIT),
					IsDlgButtonChecked(hDlg, IDC_COPY_OFFSET));
			EnableWindow(GetDlgItem(hDlg, IDC_COPY_BYTECOUNT),
					IsDlgButtonChecked(hDlg, IDC_COPY_BYTES));
			return TRUE;
		}
		break;
	}
	return FALSE;
}
