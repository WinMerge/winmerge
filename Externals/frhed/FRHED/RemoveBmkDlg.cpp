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
 * @file  RemoveBmkDlg.cpp
 *
 * @brief Implementation of the Bookmark removing dialog.
 *
 */
#include "precomp.h"
#include "resource.h"
#include "hexwnd.h"
#include "hexwdlg.h"

/**
 * @brief Initialize the dialog.
 * @param [in] hDlg Handle to the dialog.
 */
BOOL RemoveBmkDlg::OnInitDialog(HWindow *pDlg)
{
	TCHAR buf[INFOTIPSIZE];
	HListBox *list = static_cast<HListBox *>(pDlg->GetDlgItem(IDC_REMOVEBMK_LIST));
	for (int i = 0 ; i < iBmkCount ; i++)
	{
		if (pbmkList[i].name.length())
			_stprintf(buf, _T("%d %s (0x%x)"), i + 1, pbmkList[i].name.c_str(), pbmkList[i].offset);
		else
			_stprintf(buf, _T("%d 0x%x"), i + 1, pbmkList[i].offset);
		list->AddString(buf);
	}
	list->SetCurSel(0);
	return TRUE;
}

/**
 * @brief Handle dialog commands.
 * @param [in] hDlg Hanle to the dialog.
 * @param [in] wParam The command to handle.
 * @param [in] lParam Optional parameter for the command.
 * @return TRUE if the command was handled, FALSE otherwise.
 */
BOOL RemoveBmkDlg::OnCommand(HWindow *pDlg, WPARAM wParam, LPARAM lParam)
{
	int i;
	switch (wParam)
	{
	case IDOK:
		i = static_cast<int>(pDlg->SendDlgItemMessage(IDC_REMOVEBMK_LIST, LB_GETCURSEL));
		pbmkList[i].name.clear();
		--iBmkCount;
		for ( ; i < iBmkCount ; i++)
			pbmkList[i] = pbmkList[i + 1];
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
INT_PTR RemoveBmkDlg::DlgProc(HWindow *pDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
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
