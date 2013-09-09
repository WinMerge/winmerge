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
 * @file  SelectBlockDlg.cpp
 *
 * @brief Implementation of the Block selecting dialog.
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
 * @param [in] hDlg Handle to the dialog.
 */
BOOL SelectBlockDlg::OnInitDialog(HWindow *pDlg)
{
	TCHAR buf[128];
	_stprintf(buf, _T("x%x"), bSelected ? iStartOfSelection : iCurByte);
	pDlg->SetDlgItemText(IDC_BLOCKSEL_OFFSET, buf);
	_stprintf(buf, _T("x%x"), bSelected ? iEndOfSelection : iCurByte);
	pDlg->SetDlgItemText(IDC_BLOCKSEL_OFFSETEND, buf);
	// Limit edit text lengths
	pDlg->SendDlgItemMessage(IDC_BLOCKSEL_OFFSET, EM_SETLIMITTEXT, OffsetLen, 0);
	pDlg->SendDlgItemMessage(IDC_BLOCKSEL_OFFSETEND, EM_SETLIMITTEXT, OffsetLen, 0);
	return TRUE;
}

/**
 * @brief Handle dialog commands.
 * @param [in] hDlg Hanle to the dialog.
 * @param [in] wParam The command to handle.
 * @param [in] lParam Optional parameter for the command.
 * @return TRUE if the command was handled, FALSE otherwise.
 */
BOOL SelectBlockDlg::OnCommand(HWindow *pDlg, WPARAM wParam, LPARAM lParam)
{
	TCHAR buf[128];
	int iStartOfSelSetting = 0;
	int iEndOfSelSetting =  0;
	int maxb;
	switch (wParam)
	{
	case IDOK:
		if (pDlg->GetDlgItemText(IDC_BLOCKSEL_OFFSET, buf, 128) &&
			!offset_parse(buf, iStartOfSelSetting))
		{
			MessageBox(pDlg, GetLangString(IDS_OFFSET_START_ERROR), MB_ICONERROR);
			return TRUE;
		}
		if (pDlg->GetDlgItemText(IDC_BLOCKSEL_OFFSETEND, buf, 128) &&
			!offset_parse(buf, iEndOfSelSetting))
		{
			MessageBox(pDlg, GetLangString(IDS_OFFSET_END_ERROR), MB_ICONERROR);
			return TRUE;
		}
		//make the selection valid if it is not
		maxb = m_dataArray.GetUpperBound();
		if (iStartOfSelSetting < 0)
			iStartOfSelSetting = 0;
		if (iStartOfSelSetting > maxb)
			iStartOfSelSetting = maxb;
		if (iEndOfSelSetting < 0)
			iEndOfSelSetting = 0;
		if (iEndOfSelSetting > maxb)
			iEndOfSelSetting = maxb;
		iStartOfSelection = iStartOfSelSetting;
		iEndOfSelection = iEndOfSelSetting;
		bSelected = true;
		adjust_view_for_selection();
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
INT_PTR SelectBlockDlg::DlgProc(HWindow *pDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
