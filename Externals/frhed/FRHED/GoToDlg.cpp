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
 * @file  GotoDlg.cpp
 *
 * @brief Implementation of the Go To-dialog.
 *
 */
#include "precomp.h"
#include "resource.h"
#include "hexwnd.h"
#include "hexwdlg.h"
#include "StringTable.h"
#include "offset.h"

/** Size of offset edit and buffer. */
static const UINT EditLen = 16;

/**
 * Initialize the dialog.
 * This function initializes the dialog by setting the initial value for the
 * offset edit box and setting text lenhgt limit for it.
 * @param [in] hDlg Handle to dialog to initialize.
 * @return TRUE.
 */
BOOL GoToDlg::OnInitDialog(HWindow *pDlg)
{
	// Get current offset
	TCHAR buffer[EditLen + 1];
	_itot(iCurByte, buffer, 10);
	pDlg->SetDlgItemText(IDC_GOTO_OFFSET, buffer);
	// Limit edit field char amount
	pDlg->SendDlgItemMessage(IDC_GOTO_OFFSET, EM_SETLIMITTEXT, EditLen);
	return TRUE;
}

/**
 * @brief Go to offset user gave to the dialog.
 * @param [in] hDlg Handle of Goto-dialog.
 * @return TRUE if new offset was applied, FALSE for invalid offset.
 */
BOOL GoToDlg::Apply(HWindow *pDlg)
{
	TCHAR buffer[EditLen + 1];
	int offset, i = 0, r = 0;
	pDlg->GetDlgItemText(IDC_GOTO_OFFSET, buffer, RTL_NUMBER_OF(buffer));
	// For a relative jump, read offset from 2nd character on.
	if (buffer[0] == '+' || buffer[0] == '-')
		r = 1;
	if (!offset_parse(buffer + r, offset))
	{
		MessageBox(pDlg, GetLangString(IDS_OFFSET_ERROR), MB_ICONERROR);
		return FALSE;
	}
	if (r)
	{
		// Relative jump.
		if (buffer[0] == '-' )
			offset = -offset;
		offset += iCurByte;
	}
	
	// Absolute jump.
	// Check limits and jump to begin/end if out of limits
	if (offset < 0)
		offset = 0;
	if (offset >= m_dataArray.GetLength())
		offset = m_dataArray.GetLength() - 1;

	iCurByte = offset;
	snap_caret();
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
INT_PTR GoToDlg::DlgProc(HWindow *pDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
		}
		break;

	case WM_HELP:
		OnHelp(pDlg);
		break;
	}
	return FALSE;
}
