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
/** 
 * @file  OpenPartiallyDlg.cpp
 *
 * @brief Implementation of the dialog for opening files partially.
 *
 */
#include "precomp.h"
#include "resource.h"
#include "hexwnd.h"
#include "hexwdlg.h"
#include "LangArray.h"
#include "StringTable.h"
#include "offset.h"

int OpenPartiallyDlg::filehandle = 0;
bool OpenPartiallyDlg::bShowFileStatsPL = false;

/**
 * @brief Initialize the dialog.
 * @param [in] hDlg Handle to the dialog.
 * @return TRUE.
 */
BOOL OpenPartiallyDlg::OnInitDialog(HWindow *pDlg)
{
	INT64 iPLFileLen = _filelengthi64(filehandle);
	TCHAR buf[128] = {0};
	pDlg->SetDlgItemText(IDC_OPENPARTIAL_OFFSET, _T("x0"));
	_sntprintf(buf, RTL_NUMBER_OF(buf) - 1, GetLangString(IDS_PARTIAL_LOAD_BYTES), iPLFileLen);
	pDlg->SetDlgItemText(IDC_OPENPARTIAL_BYTECOUNT, buf);
	_sntprintf(buf, RTL_NUMBER_OF(buf) - 1, _T("%lld"), iPLFileLen);
	pDlg->SetDlgItemText(IDC_OPENPARTIAL_BYTES, buf);
	pDlg->CheckDlgButton(IDC_OPENPARTIAL_BEGINOFF, BST_CHECKED);
	const UINT state = bShowFileStatsPL ? BST_CHECKED : BST_UNCHECKED;
	pDlg->CheckDlgButton(IDC_OPENPARTIAL_RELOFFSET, state);
	return TRUE;
}

/**
 * @brief Apply user's selections and open the file.
 * @param [in] hDlg Handle to the dialog.
 * @return TRUE if file was successfully opened, FALSE otherwise.
 */
BOOL OpenPartiallyDlg::Apply(HWindow *pDlg)
{
	const INT64 iPLFileLen = _filelengthi64(filehandle);
	const UINT state = pDlg->IsDlgButtonChecked(IDC_OPENPARTIAL_RELOFFSET);
	bShowFileStatsPL = state == BST_CHECKED;
	TCHAR buf[128];
	UINT numBytesPl = 0; // Bytes to read
	
	// Only complain about wrong offset in start offset editbox if loading from start.
	if (pDlg->GetDlgItemText(IDC_OPENPARTIAL_BYTES, buf, RTL_NUMBER_OF(buf)) &&
		_stscanf(buf, _T("%u"), &numBytesPl) == 0)
	{
		MessageBox(pDlg, GetLangString(IDS_BYTES_NOT_KNOWN), MB_ICONERROR);
		return FALSE;
	}

	if (numBytesPl >= INT_MAX)
	{
		MessageBox(pDlg, GetLangString(IDS_PARTIAL_TOO_BIG), MB_ICONERROR);
		return FALSE;
	}

	INT64 iStartPL = 0;
	if (pDlg->IsDlgButtonChecked(IDC_OPENPARTIAL_ENDBYTES))
	{
		// Load from end of file: arguments must be adapted.
		iStartPL = iPLFileLen - numBytesPl;
		if (iStartPL < 0)
		{
			MessageBox(pDlg, GetLangString(IDS_PARTIAL_BIGGER), MB_ICONERROR);
			return FALSE;
		}
	}
	else if (pDlg->GetDlgItemText(IDC_OPENPARTIAL_OFFSET, buf, RTL_NUMBER_OF(buf)) &&
		!offset_parse64(buf, iStartPL))
	{
		MessageBox(pDlg, GetLangString(IDS_OFFSET_START_ERROR), MB_ICONERROR);
		return FALSE;
	}
	if (iStartPL + numBytesPl > iPLFileLen)
	{
		MessageBox(pDlg, GetLangString(IDS_PARTIAL_TOO_MANY_BYTES), MB_ICONERROR);
		return FALSE;
	}

	BOOL done = FALSE;
	if (m_dataArray.SetSize(numBytesPl))
	{
		m_dataArray.ExpandToSize();
		_lseeki64(filehandle, iStartPL, 0);
		iPartialOffset = iStartPL;
		iPartialOpenLen = (int) numBytesPl;
		iPartialFileLen = iPLFileLen;
		bPartialStats = bShowFileStatsPL;

		// m_dataArray restricts max size to 2 GB.
		if (_read(filehandle, m_dataArray, numBytesPl) != -1)
		{
			done = TRUE;
		}
		else
		{
			m_dataArray.ClearAll();
			MessageBox(pDlg, GetLangString(IDS_ERR_READING_FILE), MB_ICONERROR);
		}
	}
	else
	{
		MessageBox(pDlg, GetLangString(IDS_NO_MEM_FOR_FILE), MB_ICONERROR);
	}
	return done;
}

/**
 * @brief Handle dialog messages.
 * @param [in] hDlg Handle to the dialog.
 * @param [in] iMsg The message.
 * @param [in] wParam The command in the message.
 * @param [in] lParam The optional parameter for the command.
 * @return TRUE if the message was handled, FALSE otherwise.
 */
INT_PTR OpenPartiallyDlg::DlgProc(HWindow *pDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_INITDIALOG:
		return OnInitDialog(pDlg);
	case WM_COMMAND:
		switch (wParam)
		{
		case IDOK:
			if (Apply(pDlg))
			{
			case IDCANCEL:
				pDlg->EndDialog(wParam);
			}
			return TRUE;
		case IDC_OPENPARTIAL_BEGINOFF:
		case IDC_OPENPARTIAL_ENDBYTES:
			EnableDlgItem(pDlg, IDC_OPENPARTIAL_OFFSET,
				pDlg->IsDlgButtonChecked(IDC_OPENPARTIAL_BEGINOFF));
			return TRUE;
		}
		break;

	case WM_HELP:
		OnHelp(pDlg);
		break;
	}
	return FALSE;
}
