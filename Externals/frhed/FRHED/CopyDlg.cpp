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
 * @file  CopyDlg.cpp
 *
 * @brief Copy dialog implementation.
 *
 */
#include "precomp.h"
#include "resource.h"
#include "hexwnd.h"
#include "hexwdlg.h"
#include "BinTrans.h"
#include "StringTable.h"
#include "offset.h"

/** @brief Maximum text length in dialog edit boxes. */
static const int MaxTextLen = 16;

/**
 * @brief Initialize the dialog.
 * @param [in] hDlg Handle to the dialog.
 * @return TRUE.
 */
BOOL CopyDlg::OnInitDialog(HWindow *pDlg)
{
	int iStart = iGetStartOfSelection();
	int iEnd = iGetEndOfSelection();
	TCHAR buf[32];
	_stprintf(buf, _T("0x%x"), iStart);
	pDlg->SetDlgItemText(IDC_COPY_STARTOFFSET, buf);
	_stprintf(buf, _T("0x%x"), iEnd);
	pDlg->SetDlgItemText(IDC_COPY_OFFSETEDIT, buf);
	pDlg->SetDlgItemInt(IDC_COPY_BYTECOUNT, iEnd - iStart + 1, TRUE);
	pDlg->CheckDlgButton(IDC_COPY_OFFSET, BST_CHECKED);
	// Limit edit text lengths
	pDlg->SendDlgItemMessage(IDC_COPY_STARTOFFSET, EM_SETLIMITTEXT, MaxTextLen);
	pDlg->SendDlgItemMessage(IDC_COPY_OFFSETEDIT, EM_SETLIMITTEXT, MaxTextLen);
	pDlg->SendDlgItemMessage(IDC_COPY_BYTECOUNT, EM_SETLIMITTEXT, MaxTextLen);
	return TRUE;
}

/**
 * @brief Copy the bytes.
 * @param [in] hDlg Handle to the dialog.
 * @return TRUE if bytes were copied, FALSE otherwise.
 */
BOOL CopyDlg::Apply(HWindow *pDlg)
{
	const int bufSize = 64;
	TCHAR buf[bufSize + 1] = {0};
	int iOffset;
	int iNumberOfBytes;
	if (pDlg->GetDlgItemText(IDC_COPY_STARTOFFSET, buf, bufSize) &&
		!offset_parse(buf, iOffset))
	{
		MessageBox(pDlg, GetLangString(IDS_OFFSET_START_ERROR), MB_ICONERROR);
		return FALSE;
	}
	if (pDlg->IsDlgButtonChecked(IDC_COPY_OFFSET))
	{
		if (pDlg->GetDlgItemText(IDC_COPY_OFFSETEDIT, buf, bufSize) &&
			!offset_parse(buf, iNumberOfBytes))
		{
			MessageBox(pDlg, GetLangString(IDS_OFFSET_END_ERROR), MB_ICONERROR);
			return FALSE;
		}
		iNumberOfBytes = iNumberOfBytes - iOffset + 1;
	}
	else
	{// Get number of bytes.
		if (pDlg->GetDlgItemText(IDC_COPY_BYTECOUNT, buf, bufSize) &&
			_stscanf(buf, _T("%d"), &iNumberOfBytes) == 0)
		{
			MessageBox(pDlg, GetLangString(IDS_BYTES_NOT_KNOWN), MB_ICONERROR);
			return FALSE;
		}
	}
	// Can requested number be cut?
	// DataArray.GetLength ()-iCutOffset = number of bytes from current pos. to end.
	if (m_dataArray.GetLength() - iOffset < iNumberOfBytes)
	{
		MessageBox(pDlg, GetLangString(IDS_COPY_TOO_MANY), MB_ICONERROR);
		return FALSE;
	}
	// Transfer to clipboard.
	int destlen = Text2BinTranslator::iBytes2BytecodeDestLen(&m_dataArray[iOffset], iNumberOfBytes);
	HGLOBAL hGlobal = GlobalAlloc(GHND, destlen);
	if (hGlobal == 0)
	{
		// Not enough memory for clipboard.
		MessageBox(pDlg, GetLangString(IDS_COPY_NO_MEM), MB_ICONERROR);
		return FALSE;
	}
	WaitCursor wc;
	char *pd = (char *)GlobalLock(hGlobal);
	Text2BinTranslator::iTranslateBytesToBC(pd, &m_dataArray[iOffset], iNumberOfBytes);
	GlobalUnlock(hGlobal);
	pwnd->OpenClipboard();
	EmptyClipboard();
	SetClipboardData(CF_TEXT, hGlobal);
	CloseClipboard();
	return TRUE;
}

/**
 * @brief Handle dialog messages.
 * @param [in] hDlg Handle to the dialog.
 * @param [in] iMsg The message.
 * @param [in] wParam The command in the message.
 * @param [in] lParam The optional parameter for the command.
 * @return TRUE if the message was handled, FALSE otherwise.
 */
INT_PTR CopyDlg::DlgProc(HWindow *pDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
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
		case IDC_COPY_OFFSET:
		case IDC_COPY_BYTES:
			EnableDlgItem(pDlg, IDC_COPY_OFFSETEDIT,
				pDlg->IsDlgButtonChecked(IDC_COPY_OFFSET));
			EnableDlgItem(pDlg, IDC_COPY_BYTECOUNT,
				pDlg->IsDlgButtonChecked(IDC_COPY_BYTES));
			return TRUE;
		}
		break;

	case WM_HELP:
		OnHelp(pDlg);
		break;
	}
	return FALSE;
}
