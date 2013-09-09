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
 * @file  CharacterSetDlg.cpp
 *
 * @brief Implementation of the Character set selection dialog.
 *
 */
#include "precomp.h"
#include "resource.h"
#include "hexwnd.h"
#include "hexwdlg.h"

INT_PTR CharacterSetDlg::DlgProc(HWindow *pDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_INITDIALOG:
		pDlg->SetDlgItemInt(IDC_CHSET_FONTSIZE, iFontSize, TRUE);
		switch (iCharacterSet)
		{
		case ANSI_FIXED_FONT:
			pDlg->CheckDlgButton(IDC_CHSET_ANSI, BST_CHECKED);
			break;
		case OEM_FIXED_FONT:
			pDlg->CheckDlgButton(IDC_CHSET_OEM, BST_CHECKED);
			break;
		}
		return TRUE;
	case WM_COMMAND:
		switch (wParam)
		{
		case IDOK:
			iFontSize = pDlg->GetDlgItemInt(IDC_CHSET_FONTSIZE, 0, TRUE);
			iFontZoom = 0;
			iCharacterSet = pDlg->IsDlgButtonChecked(IDC_CHSET_ANSI) ?
				ANSI_FIXED_FONT : OEM_FIXED_FONT;
			save_ini_data();
			resize_window();
			// fall through
		case IDCANCEL:
			pDlg->EndDialog(wParam);
			return TRUE;
		}
		break;

	case WM_HELP:
		OnHelp(pDlg);
		break;
	}
	return FALSE;
}
