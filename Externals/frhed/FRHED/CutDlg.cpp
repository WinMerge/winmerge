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
 * @file  CutDlg.cpp
 *
 * @brief Cut dialog implementation.
 *
 */
#include "precomp.h"
#include "resource.h"
#include "hexwnd.h"
#include "hexwdlg.h"
#include "BinTrans.h"
#include "StringTable.h"
#include "offset.h"

/** Length of the offset buffer. */
static const int OffsetLen = 16;

/**
 * @brief Initialize the dialog.
 * @param [in] hDlg Handle to the dialog.
 * @return TRUE
 */
BOOL CutDlg::OnInitDialog(HWindow *pDlg)
{
	int iStart = iGetStartOfSelection();
	int iEnd = iGetEndOfSelection();
	TCHAR buf[OffsetLen + 1];

	_stprintf(buf, _T("0x%x"), iStart);
	pDlg->SetDlgItemText(IDC_CUT_STARTOFFSET, buf);
	_stprintf(buf, _T("0x%x"), iEnd);

	pDlg->CheckDlgButton(IDC_CUT_INCLUDEOFFSET, BST_CHECKED);
	pDlg->SetDlgItemText(IDC_CUT_ENDOFFSET, buf);
	pDlg->SetDlgItemInt(IDC_CUT_NUMBYTES, iEnd - iStart + 1, TRUE);

	return TRUE;
}

/**
 * @brief Cut the data.
 * This function cuts the data based on values user entered to the dialog.
 * @param [in] hDlg Handle to the dialog.
 * @return TRUE if the cutting succeeded, FALSE otherwise.
 */
BOOL CutDlg::Apply(HWindow *pDlg)
{
	TCHAR buf[OffsetLen + 1];
	int iOffset;
	int iNumberOfBytes;

	if (pDlg->GetDlgItemText(IDC_CUT_STARTOFFSET, buf, OffsetLen) &&
		!offset_parse(buf, iOffset))
	{
		MessageBox(pDlg, GetLangString(IDS_OFFSET_START_ERROR), MB_ICONERROR);
		return FALSE;
	}

	if (pDlg->IsDlgButtonChecked(IDC_CUT_INCLUDEOFFSET))
	{
		if (pDlg->GetDlgItemText(IDC_CUT_ENDOFFSET, buf, OffsetLen) &&
			!offset_parse(buf, iNumberOfBytes))
		{
			MessageBox(pDlg, GetLangString(IDS_OFFSET_END_ERROR), MB_ICONERROR);
			return FALSE;
		}
		iNumberOfBytes = iNumberOfBytes - iOffset + 1;
	}
	else
	{// Get number of bytes.
		if (pDlg->GetDlgItemText(IDC_CUT_NUMBYTES, buf, OffsetLen) &&
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
		MessageBox(pDlg, GetLangString(IDS_CUT_TOO_MANY), MB_ICONERROR);
		return FALSE;
	}

	// Transfer to cipboard.
	int destlen = Text2BinTranslator::iBytes2BytecodeDestLen(&m_dataArray[iOffset], iNumberOfBytes);
	HGLOBAL hGlobal = GlobalAlloc(GHND, destlen);
	if (hGlobal == 0)
	{
		// Not enough memory for clipboard.
		MessageBox(pDlg, GetLangString(IDS_CUT_NO_MEM), MB_ICONERROR);
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

	// Delete data.
	if (!m_dataArray.RemoveAt(iOffset, iNumberOfBytes))
	{
		MessageBox(pDlg, GetLangString(IDS_CUT_FAILED), MB_ICONERROR);
		return FALSE;
	}
	iCurByte = iOffset;
	if (iCurByte > m_dataArray.GetUpperBound())
		iCurByte = m_dataArray.GetUpperBound();
	if (iCurByte < 0)
		iCurByte = 0;
	iFileChanged = TRUE;
	bFilestatusChanged = true;
	bSelected = false;
	resize_window();
	return TRUE;
}

/**
 * @brief Process messages.
 * @param [in] hDlg Handle to dialog.
 * @param [in] iMsg Message ID.
 * @param [in] wParam First message parameter (depends on message).
 * @param [in] lParam Second message parameter (depends on message).
 * @return TRUE if message was handled, FALSE if ignored.
 */
INT_PTR CutDlg::DlgProc(HWindow *pDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
		case IDC_CUT_INCLUDEOFFSET:  
		case IDC_CUT_NUMBEROFBYTES:
			EnableDlgItem(pDlg, IDC_CUT_NUMBYTES,
				pDlg->IsDlgButtonChecked(IDC_CUT_NUMBEROFBYTES));
			EnableDlgItem(pDlg, IDC_CUT_ENDOFFSET,
				pDlg->IsDlgButtonChecked(IDC_CUT_INCLUDEOFFSET));
			return TRUE;
		}
		break;

	case WM_HELP:
		OnHelp(pDlg);
		break;
	}
	return FALSE;
}
