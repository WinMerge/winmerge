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
 * @file  AppendDlg.cpp
 *
 * @brief Implementation of the Append dialog.
 *
 */
#include "precomp.h"
#include "resource.h"
#include "hexwnd.h"
#include "hexwdlg.h"
#include "StringTable.h"

/**
 * @brief Append the data to current file.
 * @param [in] hDlg Handle to the dialog.
 * @return TRUE if appending succeeded, FALSE if failed.
 */
BOOL AppendDlg::Apply(HWindow *pDlg)
{
	const int bufsize = 64;
	TCHAR buf[bufsize + 1];
	int iAppendbytes;
	if (pDlg->GetDlgItemText(IDC_APPEND_BYTES, buf, bufsize) &&
		_stscanf(buf, _T("%d"), &iAppendbytes) == 0)
	{
		MessageBox(pDlg, GetLangString(IDS_APPEND_UNKNOWN_AMOUNT), MB_ICONERROR);
		return FALSE;
	}
	int oldupbound = m_dataArray.GetLength();
	if (!m_dataArray.SetSize(m_dataArray.GetSize() + iAppendbytes))
	{
		MessageBox(pDlg, GetLangString(IDS_APPEND_NO_MEM), MB_ICONERROR);
		return FALSE;
	}
	m_dataArray.ExpandToSize();
	WaitCursor wc;
	for (int i = 0 ; i < iAppendbytes ; i++)
		m_dataArray[oldupbound + i] = 0;
	iFileChanged = TRUE;
	bFilestatusChanged = true;
	resize_window();
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
INT_PTR AppendDlg::DlgProc(HWindow *pDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		pDlg->SetDlgItemInt(IDC_APPEND_BYTES, 1, TRUE);
		return TRUE;
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
