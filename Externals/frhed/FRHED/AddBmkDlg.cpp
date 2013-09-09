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
 * @file  AddBmkDlg.cpp
 *
 * @brief Implementation of the Add Bookmark dialog.
 *
 */
#include "precomp.h"
#include "resource.h"
#include "hexwnd.h"
#include "hexwdlg.h"
#include "StringTable.h"
#include "offset.h"

/** @brief Max length of the offset number. */
static const int OffsetLen = 16;

/**
 * @brief Initialize the dialog.
 * @param [in] hDlg Handle to dialog.
 * @return TRUE
 */
BOOL AddBmkDlg::OnInitDialog(HWindow *pDlg)
{
	TCHAR buf[OffsetLen + 1];
	_stprintf(buf, _T("0x%x"), iCurByte);
	pDlg->SetDlgItemText(IDC_BMKADD_OFFSET, buf);
	// Limit edit text lengths
	pDlg->SendDlgItemMessage(IDC_BMKADD_OFFSET, EM_SETLIMITTEXT, OffsetLen, 0);
	pDlg->SendDlgItemMessage(IDC_BMKADD_NAME, EM_SETLIMITTEXT, BMKTEXTMAX, 0);
	return TRUE;
}

/**
 * @brief Handle dialog commands.
 * @param [in] hDlg Handle to dialog.
 * @param [in] wParam First param (depends on command).
 * @param [in] lParam Second param (depends on command).
 * @return TRUE if command (message) was handled, FALSE if not.
 */
BOOL AddBmkDlg::OnCommand(HWindow *pDlg, WPARAM wParam, LPARAM lParam)
{
	TCHAR buf[OffsetLen + 1];
	int i, offset;
	switch (wParam)
	{
	case IDOK:
		if (pDlg->GetDlgItemText(IDC_BMKADD_OFFSET, buf, RTL_NUMBER_OF(buf)) &&
			!offset_parse(buf, offset))
		{
			MessageBox(pDlg, GetLangString(IDS_OFFSET_START_ERROR), MB_ICONERROR);
			return TRUE;
		}
		if (offset < 0 || offset > m_dataArray.GetLength())
		{
			MessageBox(pDlg, GetLangString(IDS_BMK_INVALID_POSITION), MB_ICONERROR);
			return TRUE;
		}
		// Is there already a bookmark on this offset?
		for (i = 0 ; i < iBmkCount ; i++)
		{
			if (pbmkList[i].offset == offset)
			{
				MessageBox(pDlg, GetLangString(IDS_BMK_ALREADY_THERE), MB_ICONERROR);
				return TRUE;
			}
		}
		// No bookmark on that position yet.
		pbmkList[iBmkCount].offset = offset;
		pDlg->GetDlgItemText(IDC_BMKADD_NAME, pbmkList[iBmkCount].name);
		iBmkCount++;
		repaint();
		// fall through
	case IDCANCEL:
		pDlg->EndDialog(wParam);
		return TRUE;
	}
	return FALSE;
}

/**
 * @brief Handle dialog messages.
 * @param [in] hDlg Handle to the dialog.
 * @param [in] iMsg The message.
 * @param [in] wParam The command in the message.
 * @param [in] lParam The optional parameter for the command.
 * @return TRUE if the message was handled, FALSE otherwise.
 */
INT_PTR AddBmkDlg::DlgProc(HWindow *pDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		return OnInitDialog(pDlg);

	case WM_COMMAND:
		return OnCommand(pDlg, wParam, lParam);

	case WM_HELP:
		OnHelp(pDlg);
		break;
	}
	return FALSE;
}
